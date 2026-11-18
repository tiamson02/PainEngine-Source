/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//
// Havok 2.0 Visual Debug Viewer Display Handle
//
#include <hkbase/hkBase.h>
#include <hkmath2/hkMath.h>

#include <hkgraphics/hkGraphics.h>
#include <hkgraphics/hkgDisplayWorld.h>
#include <hkgraphics/hkgWindow.h>
#include <hkgraphics/hkgViewport.h>
#include <hkgraphics/hkgFont.h>
#include <hkgraphics/hkgDisplayObject.h>
#include <hkgraphics/hkgMaterialManager.h>
#include <hkgraphicsbridge2/hkgGeometryConverter.h>
#include <hkgeometry2/display/hkDisplayGeometry.h>

#include <hkgraphicsbridge2/hkgDisplayHandler.h>

#include <hkutilities2/visualdebugger/hkKeyboard.h>

hkgDisplayHandler::hkgDisplayHandler(hkgDisplayWorld* displayWorld, hkgDisplayContext* displayContext)
	: m_displayContext(displayContext), m_displayWorld(displayWorld)
{
	m_displayWorld->reference();
	m_displayContext->reference();

	if (m_displayWorld->getMaterialManager() == HK_NULL)
	{
		hkgMaterialManager* matMan = hkgMaterialManager::create(m_displayContext);
		m_displayWorld->setMaterialManager(matMan);
		matMan->release();
	}

	// prime the buffers
	m_loadingBuffer = 0;
	m_readyBuffer = 1;
	m_lastHitIndex = 0;

	m_holdImmediate = false;

	m_wantGrid = false;
	m_upAxis = 1;
	m_wantAxis = false;
	m_scale = 1.0f;
	m_span = HKG_DISPLAY_HANDLER_DEFAULT_GRID_SPAN;

	// initialize statistics
	{
		m_stats.setSize(60 * 5);
		for(int i = 0; i < m_stats.getSize(); i++)
		{
			m_stats[i] = 0.0f;
		}

		m_wantStats = 0;
	}
}

hkgDisplayHandler::~hkgDisplayHandler()
{
	m_displayContext->release();
	m_displayWorld->release();
}

static void changeColor(hkgDisplayWorld* displayWorld, hkgDisplayObject* displayBody, unsigned int color)
{
	hkgMaterial* m = hkgMaterial::create();

	float r = hkgColor4GetRed(color);
	float g = hkgColor4GetGreen(color);
	float b = hkgColor4GetBlue(color);
	float a = hkgColor4GetAlpha(color);
	m->setDiffuseColor(r, g, b, a);
	displayBody->setMaterialOnAll(m);
	const int displayId = displayWorld->findDisplayObject(displayBody);

	// this does not remove the reference from the display object,
	// but addDisplayObject does.
	displayWorld->removeDisplayObject(displayId);
	
	// this will add a reference to the displayObject
	displayWorld->addDisplayObject(displayBody);

	// remove the reference that was implictly given to us by the 
	// 'removeDisplayObject' call...
	displayBody->release();
	m->release();
}


hkDisplayHandler::Status hkgDisplayHandler::displayGeometry(const hkArray<hkDisplayGeometry*>& geometries, const hkTransform& transform, int color, int tag)
{
	
	hkArray<hkVector4> lineArray;
	hkVector4 start;
	hkVector4 end;
	{
		for (int i = 0; i < geometries.getSize(); ++i)
		{
			if(!geometries[i])
			{
				HK_WARN_ONCE("Trying to display NULL geometry in display handler");
			}

			geometries[i]->getWireframeGeometry(lineArray);
			
			for(int j = lineArray.getSize() - 1; j >= 0; j-- )
			{
				start = lineArray[j];
				end = lineArray[j-1];
				j--;
				start.setTransformedPos(transform, start);
				end.setTransformedPos(transform, end);
				displayLine(start, end, color, tag);
			}
		}
	}

	return hkDisplayHandler::HK_OK;
}



hkDisplayHandler::Status hkgDisplayHandler::displayGeometry(const hkArray<hkDisplayGeometry*>& geometries, int color, int tag)
{
	hkVector4 start,end;	
	hkArray<hkVector4> lineArray;
	{
		for (int i = 0; i < geometries.getSize(); ++i)
		{
			if(!geometries[i])
			{
				HK_WARN_ONCE("Trying to display NULL geometry in display handler");
			}
			
			geometries[i]->getWireframeGeometry(lineArray);
			
			for(int j = lineArray.getSize() - 1; j >= 0; j-- )
			{
				start = lineArray[j]; 
				end = lineArray[j-1];
				j--;
				displayLine(start, end, color, tag);
			}
		}
	}
	return hkDisplayHandler::HK_OK;
}


hkDisplayHandler::Status hkgDisplayHandler::addGeometry(
							const hkArray<hkDisplayGeometry*>& geometries, const hkTransform& transform, int id, int tag)
{
	// create a new display object
	hkgDisplayObject* displayObject = hkgDisplayObject::create();

	// convert the display geometries to the display library format, and add them to the display object
	{
		for (int i = 0; i < geometries.getSize(); ++i)
		{
			hkgGeometry* graphicsDisplayGeometry = hkgGeometryConverter::createGraphicsDisplay(geometries[i], m_displayContext);
			if (graphicsDisplayGeometry != HK_NULL)
			{
				displayObject->addGeometry( graphicsDisplayGeometry );  // TODO - plus transform
				graphicsDisplayGeometry->release();	// Must release, since m_displayWorld adds a reference to this.
			}
		}
	}

	// initialize the transform of the display object
	{
		float cm16[16];
		transform.get4x4ColumnMajor(cm16);
		displayObject->setTransform(cm16);
	}

	// pick a random material and apply it to the object
	hkgMaterial* material = hkgMaterial::create();
	{
		float r = hkMath::randRange(0.0f, 1.0f);
		float g = hkMath::randRange(0.0f, 1.0f);
		float b = hkMath::randRange(0.0f, 1.0f);
		float a = 1.0f;

		material->setDiffuseColor(r, g, b, a);
		displayObject->setMaterialOnAll(material);
		material->release();

		//int matId = 1;
		//displayObject->setMaterialOnAll(m_displayWorld->getMaterialManager()->getDefaultMaterial(matId));
	}

	// add the display geometry to the world
	{
		m_displayWorld->addDisplayObject(displayObject);
		displayObject->release();	// Must release, since m_displayWorld adds a reference to this.
			// do this for the moment... should really keep a reference and then dereference on destruction...
	}

	// Setup the mapping between id and displayObject
	{
		IdDisplayObjectLink& link = *m_idDoMap.expandBy(1);
		link.m_id = (int)id;
		link.m_do = displayObject;
	}

	return hkDisplayHandler::HK_OK;
}




hkDisplayHandler::Status hkgDisplayHandler::setGeometryColor(int color, int id, int tag)
{
	for(int i = 0; i < m_idDoMap.getSize(); i++)
	{
		if(m_idDoMap[i].m_id == id)
		{
			changeColor(m_displayWorld, m_idDoMap[i].m_do, color);
		}
	}

	return hkDisplayHandler::HK_OK;
}

hkDisplayHandler::Status hkgDisplayHandler::updateGeometry(const hkTransform& transform, int id, int tag)
{
	const int mapSize = m_idDoMap.getSize();

	if ( m_lastHitIndex >= mapSize )
	{
		m_lastHitIndex = 0;
	}


	// try to find our object after m_lastHitIndex
	while(1)
	{
		for(int i = m_lastHitIndex; i < mapSize; i++)
		{
			if(m_idDoMap[i].m_id == (int)id)
			{
				if ( i != m_lastHitIndex )
				{
					hkAlgorithm::swap( m_idDoMap[i], m_idDoMap[m_lastHitIndex] );
				}

				hkgDisplayObject* displayObject = m_idDoMap[m_lastHitIndex].m_do;
				float cm16[16];
				transform.get4x4ColumnMajor(cm16);
				displayObject->setTransform(cm16, false);

				m_lastHitIndex++;

				return hkDisplayHandler::HK_OK;
			}
		}
		// not found, try from the start
		if ( m_lastHitIndex > 0 )
		{
			m_lastHitIndex = 0;
			continue;
		}
		else
		{
			return hkDisplayHandler::HK_FAIL;
		}
	}
}


hkDisplayHandler::Status hkgDisplayHandler::removeGeometry(int id, int tag)
{
	// lookup the display object based on the id
	hkgDisplayObject* displayObject = HK_NULL;
	int i;
	for(i = 0; i < m_idDoMap.getSize(); i++)
	{
		if(m_idDoMap[i].m_id == (int)id)
		{
			displayObject = m_idDoMap[i].m_do;
			break;
		}
	}

	//some phantoms don't have displayobject, but some do...
	if (displayObject)
	{
		m_displayWorld->removeDisplayObject( m_displayWorld->findDisplayObject(displayObject) );

		displayObject->release();


		m_idDoMap.removeAt(i);
	}

	return hkDisplayHandler::HK_OK;
}


hkDisplayHandler::Status hkgDisplayHandler::updateCamera(const hkVector4& from, const hkVector4& to, const hkVector4& up, hkReal nearPlane, hkReal farPlane, hkReal fov, const char* name)
{

	return hkDisplayHandler::HK_OK;
}


hkDisplayHandler::Status hkgDisplayHandler::displayPoint(const hkVector4& position, int color, int tag)
{
	if(!m_holdImmediate)
	{
		DisplayPoint& dp = *m_displayPoints[m_loadingBuffer].expandBy(1);
		dp.m_position = position;
		dp.m_color = color;
	}
	else
	{
		DisplayPoint& dp = *m_displayPoints[m_readyBuffer].expandBy(1);
		dp.m_position = position;
		dp.m_color = color;
	}

	return hkDisplayHandler::HK_OK;
}


hkDisplayHandler::Status hkgDisplayHandler::displayLine(const hkVector4& start, const hkVector4& end, int color, int tag)
{
	if(!m_holdImmediate)
	{
		DisplayLine& dl = *m_displayLines[m_loadingBuffer].expandBy(1);
		dl.m_start = start;
		dl.m_end = end;
		dl.m_color = color;
	}
	else
	{
		DisplayLine& dl = *m_displayLines[m_readyBuffer].expandBy(1);
		dl.m_start = start;
		dl.m_end = end;
		dl.m_color = color;
	}

	return hkDisplayHandler::HK_OK;
}

hkDisplayHandler::Status hkgDisplayHandler::displayText(const char* text, int color, int tag)
{

	return hkDisplayHandler::HK_OK;
}

hkDisplayHandler::Status hkgDisplayHandler::sendStatisticsDump(const char* data, int length)
{
	

	return hkDisplayHandler::HK_OK;
}


hkDisplayHandler::Status hkgDisplayHandler::holdImmediate()
{
	m_holdImmediate = true;

	return hkDisplayHandler::HK_OK;
}


hkDisplayHandler::Status hkgDisplayHandler::step(hkReal frameTimeInMs)
{
	// swap the immediate mode buffers
	clearAndSwapImmediate(); 

	return hkDisplayHandler::HK_OK;
}

//	hkDisplayHandler::Status hkgDisplayHandler::step()
//	{
//		for(int i = 0; i < window->getNumViewports(); ++i )
//		{
//			window->getViewport(i)->setAsCurrent( window->getContext() );
//			environment.m_displayWorld->render( window->getContext() );
//			environment.m_displayHandler->drawImmediate();
//		}
//		environment.m_textDisplay->displayText(window);			
//		window->swapBuffers();
//	}


//
// implementation specific methods for handling immediate mode data
//
void hkgDisplayHandler::drawImmediate()
{
	// alter state
	m_displayContext->setDepthReadState(true);
	m_displayContext->setDepthWriteState(false);

	m_displayContext->setLightingState(false);
	m_displayContext->setTexture2DState(false);

	// draw the grid
	if(m_wantGrid)
	{
		drawGrid();
	}

	// draw axes
	if(m_wantAxis)
	{
		drawAxis();
	}

	// draw points
	if(m_displayPoints[m_readyBuffer].getSize())
	{
		m_displayContext->beginGroup( HKG_IMM_POINTS );
		for(int i = 0; i < m_displayPoints[m_readyBuffer].getSize(); i++)
		{
			const unsigned int color = m_displayPoints[m_readyBuffer][i].m_color;
			m_displayContext->setCurrentColorPacked(color);
			m_displayContext->setCurrentPosition( (float*)&m_displayPoints[m_readyBuffer][i].m_position );
		}
		m_displayContext->endGroup();
	}

	// immediate mode data
	if(m_displayLines[m_readyBuffer].getSize())
	{
		m_displayContext->beginGroup( HKG_IMM_LINES );
		for(int i = 0; i < m_displayLines[m_readyBuffer].getSize(); i++)
		{
			const unsigned int color = m_displayLines[m_readyBuffer][i].m_color;
			m_displayContext->setCurrentColorPacked(color);
			m_displayContext->setCurrentPosition( (float*)&m_displayLines[m_readyBuffer][i].m_start );
			m_displayContext->setCurrentColorPacked(color);
			m_displayContext->setCurrentPosition( (float*)&m_displayLines[m_readyBuffer][i].m_end );
		}
		m_displayContext->endGroup();
	}

	/*// draw graph info
	if(m_wantStats)
	{
		drawGraph();
	}

	if(m_wantStats > 0)
	{
		m_wantStats--;
	}
	//*/

	// restore state
	m_displayContext->setDepthReadState(true);
	m_displayContext->setDepthWriteState(true);
}

//* 
const char* name = "Simulate";
unsigned int color = 0xff00aa00;

void hkgDisplayHandler::drawGraph()
{
	// setup camera
	hkgViewport* v = m_displayContext->getCurrentViewport();
	hkgViewport* gv = m_displayContext->getOwner()->getWindowOrthoView();
	gv->setAsCurrent(m_displayContext);

	// set state variables
	m_displayContext->setDepthReadState(false);
	m_displayContext->setDepthWriteState(false);
	m_displayContext->setBlendState(true);

	// draw a polygon
	if(1)
	{
		hkReal point[3] = { 0.0f, 0.0f, -0.1f };

		float color[4] = { 0.0f, 0.0f, 0.0f, 0.25f };
		m_displayContext->setCurrentColor4(color);

		m_displayContext->beginGroup( HKG_IMM_TRIANGLE_STRIP );

		point[0] = 0.0f;
		point[1] = 0.0f;
		m_displayContext->setCurrentPosition(point);

		point[1] = 32.0f;
		m_displayContext->setCurrentPosition(point);
		point[1] = 0.0f;
		
		point[0] = (hkReal)gv->getWidth();
		m_displayContext->setCurrentPosition(point);

		point[1] = 32.0f;
		m_displayContext->setCurrentPosition(point);

		m_displayContext->endGroup();
	}

	// draw graph
	{
		hkReal start[3] = { 0.0f, 0.0f, -0.1f };
		hkReal end[3] = { 0.0f, 0.0f, -0.1f };

		m_displayContext->beginGroup( HKG_IMM_LINES );

		for(int i = 0; i < m_stats.getSize() - 1; i++)
		{
			start[0] = (hkReal)i * ((hkReal)gv->getWidth() / (m_stats.getSize() - 1));
			end[0] = (i + 1.0f) * ((hkReal)gv->getWidth() / (m_stats.getSize() - 1));

			start[1] = m_stats[i] * 2.0f + 1.0f;
			end[1] = m_stats[i + 1] * 2.0f + 1.0f;
			m_displayContext->setCurrentColorPacked(color);
			m_displayContext->setCurrentPosition(start);
			m_displayContext->setCurrentColorPacked(color);
			m_displayContext->setCurrentPosition(end);
		}

		m_displayContext->endGroup();
	}
	
	// compute float version of the color
	float floatColor[4] = { 0.0f };
	{
		floatColor[3] = ((color & 0xff000000) >> 24) / 255.0f;
		floatColor[0] = ((color & 0xff0000) >> 16) / 255.0f;
		floatColor[1] = ((color & 0xff00) >> 8) / 255.0f;
		floatColor[2] = (color & 0xff) / 255.0f;
	}

	// draw text label
	if(1)
	{
		hkgFont* f = hkgFont::create();
		hkgFont::setDrawState(m_displayContext);
		hkgTexture* ft = hkgTexture::create(m_displayContext);
		ft->loadBuiltIn( HKG_TEXTURE_FONT );
		ft->realize();

		f->setTexture(ft);
		ft->release();
		
		f->render(m_displayContext, name, 2, 2, floatColor);

		f->release();
	}

	// restore camera
	v->setAsCurrent(m_displayContext);
	
	// restore state
	m_displayContext->setDepthReadState(true);
	m_displayContext->setDepthWriteState(true);
}
//*/

static bool gotLast = false;
static float lastMatrix[16] = { 0.0f };

void hkgDisplayHandler::drawGrid()
{
	hkgViewport* v = m_displayContext->getCurrentViewport();
	hkgCamera* c = v->getCamera();

	hkReal start[4] = { 0.0f };
	hkReal end[4] = { 0.0f };

	hkReal to[4];
	c->getTo(to);

	int axis1 = (m_upAxis + 2) % 3;
	int axis2 = (m_upAxis + 1) % 3;

	// find out if the camera has changed - fix oscillation needs tidying
	bool hasChanged = false;
	{
		float* matrix = c->getViewMatrix();
		for(int i = 0; i < 16; i++)
		{
			if(gotLast && (matrix[i] != lastMatrix[i]))
			{
				hasChanged = true;
			}
			lastMatrix[i] = matrix[i];
		}
		gotLast = true;
	}

	// update scale parameters
	if(hasChanged)
	{
		hkgViewport* v = m_displayContext->getCurrentViewport();
		hkgCamera* c = v->getCamera();

		// project grid points
		hkReal result[4][3] = { { 0.0f } };
		{
			float point[3] = { 0.0f };
			c->project(point[0], point[1], point[2], v->getWidth(), v->getHeight(), result[0]);
			
			point[axis1] = (hkReal)m_scale;
			c->project(point[0], point[1], point[2], v->getWidth(), v->getHeight(), result[1]);
			point[axis1] = 0.0f;

			point[axis2] = (hkReal)m_scale;
			c->project(point[0], point[1], point[2], v->getWidth(), v->getHeight(), result[2]);

			point[axis1] = (hkReal)m_scale;
			c->project(point[0], point[1], point[2], v->getWidth(), v->getHeight(), result[3]);
		}

		// compute distance (^2)
		hkReal distanceVector[3][2];
		distanceVector[0][0] = result[0][0] - result[1][0];
		distanceVector[0][1] = result[0][1] - result[1][1];
		distanceVector[1][0] = result[0][0] - result[2][0];
		distanceVector[1][1] = result[0][1] - result[2][1];
		distanceVector[2][0] = result[0][0] - result[3][0];
		distanceVector[2][1] = result[0][1] - result[3][1];

		//hkprintf("Distance Vector: %f, %f\n", distanceVector[0][0], distanceVector[0][1]);
		//hkprintf("Distance Vector: %f, %f\n", distanceVector[1][0], distanceVector[1][1]);

		hkReal distance[3];
		distance[0] = distanceVector[0][0] * distanceVector[0][0] + distanceVector[0][1] * distanceVector[0][1];
		distance[1] = distanceVector[1][0] * distanceVector[1][0] + distanceVector[1][1] * distanceVector[1][1];
		distance[2] = distanceVector[2][0] * distanceVector[2][0] + distanceVector[2][1] * distanceVector[2][1];

		hkprintf("Distance: %f, %f\n", distance[0], distance[1]);

		const hkReal distanceTresholdSquard = (HKG_DISPLAY_HANDLER_DEFAULT_LINE_GAP * HKG_DISPLAY_HANDLER_DEFAULT_LINE_GAP);
		if((distance[0] < distanceTresholdSquard) || 
			(distance[1] < distanceTresholdSquard) ||
			(distance[2] < distanceTresholdSquard))
		{
			//if(m_span >= 10)
			{
				m_scale *= (hkReal)HKG_DISPLAY_HANDLER_DEFAULT_GRID_BASE;
				//m_span /= 10;
			}
		}
		const hkReal gradBaseSquared = HKG_DISPLAY_HANDLER_DEFAULT_GRID_BASE * HKG_DISPLAY_HANDLER_DEFAULT_GRID_BASE;
		const hkReal HYSTERSIS = 1.5f * 1.5f;  // The HYSTERSIS above is to prevent oscillation like a schmidt trigger (in electronics)
		if((distance[0] > distanceTresholdSquard * gradBaseSquared * HYSTERSIS) &&
			(distance[1] > distanceTresholdSquard * gradBaseSquared * HYSTERSIS) &&
			(distance[2] > distanceTresholdSquard * gradBaseSquared * HYSTERSIS))
		{
			//if(m_span > 100)
			{
				m_scale /= (hkReal)HKG_DISPLAY_HANDLER_DEFAULT_GRID_BASE;
				//m_span *= 10;
			}
		}
	}

	m_displayContext->beginGroup( HKG_IMM_LINES );
	for(int i = -m_span; i <= m_span; i++)
	{
		unsigned int color = 0x80808080;
		if(!(i % HKG_DISPLAY_HANDLER_DEFAULT_GRID_BASE))
		{
			color = 0xa0a0a0a0;
		}

		// along x
		start[axis1] = m_span * m_scale;
		end[axis1] = -m_span * m_scale;

		start[axis2] = i * m_scale;
		end[axis2] = i * m_scale;

		m_displayContext->setCurrentColorPacked(color);
		m_displayContext->setCurrentPosition(start);
		m_displayContext->setCurrentColorPacked(color);
		m_displayContext->setCurrentPosition(end);

		// along z
		start[axis2] =  m_span * m_scale;
		end[axis2] = -m_span * m_scale;

		start[axis1] = i * m_scale;
		end[axis1] = i * m_scale;

		m_displayContext->setCurrentColorPacked(color);
		m_displayContext->setCurrentPosition(start);
		m_displayContext->setCurrentColorPacked(color);
		m_displayContext->setCurrentPosition(end);
	}
	m_displayContext->endGroup();
}

void hkgDisplayHandler::drawAxis()
{
	hkReal start[4] = { 0.0f };
	hkReal end[4] = { 0.0f };
	unsigned int color = 0xff0000;

	m_displayContext->beginGroup( HKG_IMM_LINES );
	for(int i = 0; i < 3; i++)
	{
		end[i] = 1.0f;
		m_displayContext->setCurrentColorPacked(color | 0xff000000);
		m_displayContext->setCurrentPosition(start);
		m_displayContext->setCurrentColorPacked(color | 0xff000000);
		m_displayContext->setCurrentPosition(end);
		end[i] = 0.0f;

		color = (color >> 8);
	}
	m_displayContext->endGroup();
}

void hkgDisplayHandler::setGrid(bool state)
{
	m_wantGrid = state;
}

bool hkgDisplayHandler::getGrid()
{
	return m_wantGrid;
}

void hkgDisplayHandler::setGridUpAxis(int axis)
{
	m_upAxis = axis;
}

void hkgDisplayHandler::setAxes(bool state)
{
	m_wantAxis = state;
}

bool hkgDisplayHandler::getAxes()
{
	return m_wantAxis;
}

//*
void hkgDisplayHandler::addStat(hkReal stat)
{
	for(int i = 0; i < m_stats.getSize() - 1; i++)
	{
		m_stats[i] = m_stats[i+1];
	}
	m_stats[m_stats.getSize() - 1] = stat;

	if(m_wantStats < 60)
	{
		m_wantStats++;
	}
}
//*/
//*/ hkMath::fabs(points[numPoints - 2] + hkMath::randRange(-2.0f, 2.0f));

void hkgDisplayHandler::clearAndSwapImmediate()
{
	if(!m_holdImmediate)
	{
		// clear point buffer
		m_displayPoints[m_readyBuffer].clear();

		// clear line buffer
		m_displayLines[m_readyBuffer].clear();

		// 'swap' the buffers by swapping the 'ready' and 'loadng' indexes
		// essentially the same as swapping pointerrs
		int temp = m_readyBuffer;
		m_readyBuffer = m_loadingBuffer;
		m_loadingBuffer = temp;
	}
	else
	{
		m_holdImmediate = false;
	}
}

void hkgDisplayHandler::stepDisplayCallback(hkReal frameTimeInMs)
{
	clearAndSwapImmediate();
}



//
// Functions specific to the hkg display handler (this will be removed later on hopefully...)
//
const hkgDisplayHandler::IdDisplayObjectLink* hkgDisplayHandler::getDisplayObjectId(const hkgDisplayObject* displayObject)
{
	for(int i = 0; i < m_idDoMap.getSize(); i++)
	{
		if(m_idDoMap[i].m_do == displayObject)
		{
			return &m_idDoMap[i];
		}
	}
	return HK_NULL;
}

void hkgDisplayHandler::clearDisplay()
{
	// clear immediate mode data
	clearAndSwapImmediate();

	// clear geometry data from the display world
	for(int i = 0; i < m_idDoMap.getSize(); i++)
	{
		hkgDisplayObject* displayObject = m_idDoMap[i].m_do;
		
		if(displayObject)
		{
			m_displayWorld->removeDisplayObject(m_displayWorld->findDisplayObject(displayObject));
			displayObject->release();
		}
	}

	// emty the mapping
	m_idDoMap.clear();
}































/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
