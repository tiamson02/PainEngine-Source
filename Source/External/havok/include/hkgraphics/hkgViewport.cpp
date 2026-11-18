/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgViewport.h> 
#include <hkgraphics/hkgWindow.h> 
#include <hkgraphics/hkgCamera.h> 
#include <hkgraphics/hkgDisplayWorld.h> 
#include <hkgraphics/hkgDisplayObject.h>

#define XY_PAN_SPEED 0.005f
#define WHEEL_ZOOM_SPEED 0.001f
#define ZOOM_SPEED 0.01f

#define PAD_SCALE 50
#define PAD_XY_PAN_SPEED 0.1f
#define PAD_ZOOM_SPEED 0.1f

hkgViewport* (*hkgViewport::create)(hkgWindow* owner);

hkgViewport::hkgViewport(hkgWindow* owner)
: m_absolute(false), m_camera( hkgCamera::create() ),
  m_owner(owner),
  m_useTrackball(true),m_wantMouseEvents(true),
  m_viewMouseButton(0),
  m_viewPadButton(0), 
  m_desiredState( HKG_ENABLED_LIGHTING | HKG_ENABLED_TEXTURE2D
  | HKG_ENABLED_ZREAD | HKG_ENABLED_ZWRITE ),
  m_desiredCullFaceMode( HKG_CULLFACE_CCW )
{
	m_winLLRelative[0] = 0.0f;
	m_winLLRelative[1] = 0.0f;
	m_winURRelative[0] = 1.0f;
	m_winURRelative[1] = 1.0f;
	
	int winW = m_owner->getWidth();
	int winH = m_owner->getHeight();

	m_winLL[0] = int(winW * m_winLLRelative[0]);
	m_winLL[1] = int(winH * m_winLLRelative[1]);
	m_winUR[0] = int(winW * m_winURRelative[0]);
	m_winUR[1] = int(winH * m_winURRelative[1]);

	// culling kills the PS2 pipe:
//#ifndef HK_PLATFORM_PS2
//	m_desiredState |= HKG_ENABLED_CLLLFACE;
//#endif

}

hkgViewport::~hkgViewport()
{
	if ( m_camera )
	{
		m_camera->release();
	}
}

void hkgViewport::setCamera( hkgCamera* cam )
{
	if (cam)
	{
		cam->reference();
	}

	if (m_camera)
	{
		m_camera->release();
	}

	m_camera = cam;
}

void hkgViewport::setAsCurrent(hkgDisplayContext* context)
{
	context->setCurrentViewport( this );

	HKG_ENABLED_STATE s = context->getEnabledState();

	// if we have the correct state we are done.
	if (s != m_desiredState )
		context->matchState( m_desiredState, m_desiredCullFaceMode );

	m_camera->setAsCurrent(context);
}


void hkgViewport::acceptMouseFocus()
{
	m_viewMouseButton = 0;
	m_viewPadButton = 0;
}

void hkgViewport::processMouseMove(int x, int y, int dx, int dy)
{
	if ( (dx == 0) && (dy == 0))
		return; // a few functions fail with 0 delta.

	if (m_wantMouseEvents && m_useTrackball)
	{
		if ( m_viewMouseButton & HKG_MOUSE_LEFT_BUTTON )
		{
			int vw = m_winUR[0] - m_winLL[0];
			int vh = m_winUR[1] - m_winLL[1];
			int mx = x - m_winLL[0];
			int my = y - m_winLL[1];
			
			//printf("moving trackball to w%d h%d x%d y%d\n", vw, vh, mx, my);
			m_camera->moveTrackball( vw, vh, mx, my );
		}

		if (m_viewMouseButton & HKG_MOUSE_RIGHT_BUTTON )
		{
			// pan
			m_camera->pan( -dx * XY_PAN_SPEED, -dy * XY_PAN_SPEED, true );
		}

		if (m_viewMouseButton & HKG_MOUSE_MIDDLE_BUTTON )
		{
			// zoom
			m_camera->dolly( dy * ZOOM_SPEED, false );
		}
	}
}

void hkgViewport::processMouseWheel(int delta, int x, int y)
{
	m_camera->dolly( delta * WHEEL_ZOOM_SPEED, false);
}

void hkgViewport::processMouseButton(HKG_MOUSE_BUTTON button, bool state, int x, int y)
{
	int vw = m_winUR[0] - m_winLL[0];
	int vh = m_winUR[1] - m_winLL[1];

	if (state)
	{
		m_viewMouseButton |= button;

		if (m_wantMouseEvents && m_useTrackball)
		{
			if ( button == HKG_MOUSE_LEFT_BUTTON )
			{
				// printf(" starting trackball @ w%d h%d x%d y%d\n", vw, vh, x - m_winLL[0], y - m_winLL[1] );
				m_camera->startTrackball( vw, vh, x - m_winLL[0], y - m_winLL[1] );
			}
		}
	}
	else
	{
		m_viewMouseButton &= ~button;
	}
}

void hkgViewport::processPadStickMove( int pad, int stick, float x, float y, float dx, float dy)
{

}

void hkgViewport::processPadButton( int pad, HKG_PAD_BUTTON button, bool state)
{
			
}


bool hkgViewport::pick( int x, int y, const hkgDisplayWorld* world, hkgViewportPickData& data)
{
	// compute the ray to use for intersection solving
	float from[3];
	float to[3];
	float dir[3];
	
	m_camera->unProject( x, y, 0.0f, getWidth(), getHeight(), from);
	m_camera->unProject( x, y, 1.0f, getWidth(), getHeight(), to);

	hkgVec3Sub(dir, to, from);
	hkgVec3Normalize(dir);

	hkgRay ray( from, dir );

	data.m_trianglesHit = 0;

	hkgDisplayObjectPickData ddata;
	
	float minDist = FLT_MAX;

	int ndo = world->getNumDisplayObjects();
	for (int i = 0; i< ndo; ++i)
	{
		const hkgDisplayObject* dispObj = world->getDisplayObject(i);
		unsigned int h = dispObj->intersect( ray, false, ddata);
		if (h > 0)
		{
			float dist = ddata.m_geomPickData.m_facePickData.m_dist;
			// don't store intersections from behind camera
			if ( dist > 0 )
			{
				data.m_trianglesHit += h;

				if ( minDist > dist )
				{
					minDist = dist;
					data.m_objectIndex = i;
					data.m_objectPickData = ddata;
					data.m_world = const_cast<hkgDisplayWorld*>( world );
					hkgVec3Mult( data.m_worldPos, dir, dist);
					hkgVec3Add( data.m_worldPos, data.m_worldPos, from );
				}
			}
		}
	}

	return (data.m_trianglesHit > 0);
}

void hkgViewport::updateExtents()
{
	if (!m_absolute)
	{
		int winW = m_owner->getWidth();
		int winH = m_owner->getHeight();

		m_winLL[0] = unsigned(winW * m_winLLRelative[0]);
		m_winLL[1] = unsigned(winH * m_winLLRelative[1]);

		m_winUR[0] = unsigned(winW * m_winURRelative[0]);
		m_winUR[1] = unsigned(winH * m_winURRelative[1]);
	}

	int vw = m_winUR[0] - m_winLL[0];
	int vh = m_winUR[1] - m_winLL[1];

	if (m_camera)
	{
		m_camera->setAspect( vw / (float) vh );
		m_camera->computeProjection();
	}
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
