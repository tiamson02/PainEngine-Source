/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgWindow.h> 
#include <hkgraphics/hkgDisplayWorld.h> 
#include <hkgraphics/hkgTexture.h>
#include <hkgraphics/hkgCamera.h>

#define MAX_MOUSE_DELTA 50
#define HKG_VIRTUAL_MOUSE_SPEED 0.015f

hkgWindow* (*hkgWindow::create)();

hkgWindow::~hkgWindow()
{
	int nv = m_viewports.getSize();
	for (int i = 0; i < nv; ++i)
		m_viewports[i]->release();

	m_context->setCurrentTexture( HK_NULL );

	if (m_windowOrthoView)
		m_windowOrthoView->release();

	if (m_havokLogoTexture)
		m_havokLogoTexture->release();

	if (m_mousePointerTexture)
		m_mousePointerTexture->release();

	if (m_context)
		m_context->release();
}

bool hkgWindow::initialize(  HKG_WINDOW_CREATE_FLAG flags, HKG_WINDOW_BUFFERS buffers, int w, int h, const char* name, void* platformHandle , void* m_platformParentHandle)
{		
	m_platformHandle = platformHandle;
	m_platformParentHandle = m_platformParentHandle; 

	m_width = w;
	m_height = h;
	m_fullscreen = flags & HKG_WINDOW_FULLSCREEN;
	
	this->initializeContext();

	// create a default viewport for the window
	hkgViewport* v = hkgViewport::create(this);
	v->setAsCurrent(m_context); 
	addViewport( v );
	v->release();

	// Handy orthographic full window viewport
	m_windowOrthoView = hkgViewport::create(this);
	m_windowOrthoView->setDesiredState(HKG_ENABLED_NONE); 

	hkgCamera* oc = m_windowOrthoView->getCamera();

	oc->setOrthoLeft(0);
	oc->setOrthoTop((float)(getHeight()+1));
	oc->setOrthoRight((float)(getWidth()+1));
	oc->setOrthoBottom(0);
	oc->setOrthoNear(0);
	oc->setOrthoFar(2);
	oc->computeModelView(true); // identity
	oc->setProjectionMode(HKG_CAMERA_ORTHOGRAPHIC);	
	oc->computeProjection();


	return true;
}

bool hkgWindow::initializeContext()
{
	if (m_context == HK_NULL)
		m_context = hkgDisplayContext::create(this);

	m_context->setBlendState( false );
	m_context->setCullFaceState( false );
	m_context->setDepthReadState( true );
	m_context->setDepthWriteState( true );
	m_context->setLightingState( false );
	m_context->setWireframeState( false );
	m_context->setTexture2DState( false );

	return true;
}

void hkgWindow::renderToAllViewports( hkgDisplayWorld* w ) const
{
	int nv = m_viewports.getSize();
	for (int i = 0; i < nv; ++i)
	{
		hkgViewport* v = m_viewports[i];
		v->setAsCurrent( m_context );

		if (w)
			w->render( m_context); //, false);
	}

	// cleanup state:
	getCurrentViewport()->setAsCurrent(m_context);
}

void hkgWindow::finishScene() const
{
	if ( (m_drawMousePointer || m_drawHavokLogo || m_viewportBorders) && m_windowOrthoView)	
	{
		m_windowOrthoView->setAsCurrent( m_context );
		
		if (m_viewportBorders)
			drawViewportBorders();

		if (m_drawMousePointer)
			drawMousePointer();

		if (m_drawHavokLogo)
			drawHavokLogo();
	}
}

bool hkgWindow::updateSize( unsigned int w, unsigned int h)
{
	bool changed = (w!=m_width) || (h!=m_height);
	m_width = w;
	m_height = h;

	if (changed && m_windowResizeFunc)
	{
		m_windowResizeFunc( this, w, h, m_windowResizeFuncUserContext);
	}

	if (m_windowOrthoView)
	{
		m_windowOrthoView->updateExtents();
		hkgCamera* oc = m_windowOrthoView->getCamera();
		oc->setOrthoTop((float)(getHeight()+1));
		oc->setOrthoRight((float)(getWidth()+1));
		oc->computeProjection();
	}
	
	// resize any viewports that are relative to window
	int nv = m_viewports.getSize();
	for (int i = 0; i < nv; ++i)
	{
		hkgViewport* v = m_viewports[i];
		v->updateExtents();
	}

	return true;
}

bool hkgWindow::updatePos( int x, int y)
{
	return false;
}

void hkgWindow::drawMousePointer() const
{
	if (m_mousePointerTexture == HK_NULL)
	{
		m_mousePointerTexture = hkgTexture::create(m_context);
		bool res = m_mousePointerTexture->loadBuiltIn( HKG_TEXTURE_POINTER );
		HK_ASSERT( res );
		m_mousePointerTexture->realize();
	}

	float color[3];
	float tl[3];
	float br[3];

	float p[3];
	float uv[2];

	const float depth = -0.01f;

	tl[0] = (float)(m_mouse.getPosX());
	tl[1] = (float)(m_mouse.getPosY());
	tl[2] = depth;
		
	br[0] = m_mouse.getPosX()+ (getWidth()/30.0f);
	br[1] = m_mouse.getPosY()- (getHeight()/25.0f);
	br[2] = depth;
	
	color[0] = color[1] = color[2] = 1.0f;
	m_context->setCurrentColor3( color ); 
	
	m_context->setTexture2DState( true ); // turn on textures for this
	m_context->setBlendState( true );
//	m_context->setCullFaceState( false );

	m_context->setCurrentTexture( m_mousePointerTexture );
	
	m_context->beginGroup( HKG_IMM_TRIANGLE_LIST );

		p[0] = tl[0]; p[1] = tl[1]; p[2] = depth;
		uv[0] = 0.1f; uv[1] = 0.9f;
		m_context->setCurrentTextureCoord( uv );
		m_context->setCurrentPosition( p );
		
		p[0] = tl[0]; p[1] = br[1]; 
		uv[0] = 0.1f; uv[1] = 0.1f;
		m_context->setCurrentTextureCoord( uv );
		m_context->setCurrentPosition( p );

		p[0] = br[0]; p[1] = tl[1]; 
		uv[0] = 0.9f; uv[1] = 0.9f;
		m_context->setCurrentTextureCoord( uv );
		m_context->setCurrentPosition( p );

		p[0] = tl[0]; p[1] = br[1];
		uv[0] = 0.1f; uv[1] = 0.1f;
		m_context->setCurrentTextureCoord( uv );
		m_context->setCurrentPosition( p );

		p[0] = br[0]; p[1] = tl[1]; 
		uv[0] = 0.9f; uv[1] = 0.9f;
		m_context->setCurrentTextureCoord( uv );
		m_context->setCurrentPosition( p );

		p[0] = br[0]; p[1] = br[1]; 
		uv[0] = 0.9f; uv[1] = 0.1f;
		m_context->setCurrentTextureCoord( uv );
		m_context->setCurrentPosition( p );

	m_context->endGroup();

	m_context->setBlendState( false );
	m_context->setTexture2DState( false ); // turn back to standard window ortho view

}

void hkgWindow::drawHavokLogo() const
{	
	if (m_havokLogoTexture == HK_NULL)
	{
		m_havokLogoTexture  = hkgTexture::create(m_context);
		bool res = m_havokLogoTexture->loadBuiltIn( HKG_TEXTURE_HAVOKLOGO );
		HK_ASSERT( res );
		m_havokLogoTexture->realize();
	}

	float white[4] = { 1.0f, 1.0f, 1.0f, 0.5f};
	
	m_context->setCurrentTexture( m_havokLogoTexture );
		
	m_context->setTexture2DState( true ); // turn on textures for this
	m_context->setBlendState( true );

	m_context->setCurrentColor4( white );
	
	float p[3];
	float uv[2];
	p[2] = -0.01f; //depth

	float tl[3];
	float lr[3];

	unsigned int logoW = 64;
	unsigned int logoH = 32;
	
	tl[0] = (float)(getWidth() - logoW - m_tvDeadZoneH); 
	tl[1] = (float)(logoH + m_tvDeadZoneV); 

	lr[0] = (float)(getWidth() - m_tvDeadZoneH); 
	lr[1] = (float)m_tvDeadZoneV; 

	m_context->beginGroup( HKG_IMM_TRIANGLE_LIST );

		p[0] = tl[0]; p[1] = tl[1]; 
		uv[0] = 0.0f; uv[1] = 1.0f;
		m_context->setCurrentTextureCoord( uv );
		m_context->setCurrentPosition( p );
		
		p[0] = tl[0]; p[1] = lr[1]; 
		uv[0] = 0.0f; uv[1] = 0.0f;
		m_context->setCurrentTextureCoord( uv );
		m_context->setCurrentPosition( p );

		p[0] = lr[0]; p[1] = tl[1]; 
		uv[0] = 1.0f; uv[1] = 1.0f;
		m_context->setCurrentTextureCoord( uv );
		m_context->setCurrentPosition( p );

		p[0] = tl[0]; p[1] = lr[1];
		uv[0] = 0.0f; uv[1] = 0.0f;
		m_context->setCurrentTextureCoord( uv );
		m_context->setCurrentPosition( p );

		p[0] = lr[0]; p[1] = tl[1]; 
		uv[0] = 1.0f; uv[1] = 1.0f;
		m_context->setCurrentTextureCoord( uv );
		m_context->setCurrentPosition( p );

		p[0] = lr[0]; p[1] = lr[1]; 
		uv[0] = 1.0f; uv[1] = 0.0f;
		m_context->setCurrentTextureCoord( uv );
		m_context->setCurrentPosition( p );

	m_context->endGroup();

	m_context->setBlendState( false );
	m_context->setTexture2DState( false ); // turn back to standard window ortho view

}

static inline void draw2DBox(hkgDisplayContext* ctx, int illx, int illy, int iurx, int iury)
{
	const float llx = (float)illx;
	const float lly = (float)illy;
	const float urx = (float)iurx;
	const float ury = (float)iury;
	
	float p[3];

	// 0
	p[0] = llx; p[1] = lly; p[2] = -0.01f;
	ctx->setCurrentPosition(p);
	p[0] = llx; p[1] = ury;
	ctx->setCurrentPosition(p);	
	
	// 1
	ctx->setCurrentPosition(p);
	p[0] = urx; p[1] = ury;
	ctx->setCurrentPosition(p);

	// 2
	ctx->setCurrentPosition(p);
	p[0] = urx; p[1] = lly;
	ctx->setCurrentPosition(p);

	// 3
	ctx->setCurrentPosition(p);
	p[0] = llx; p[1] = lly;
	ctx->setCurrentPosition(p);
}

void hkgWindow::drawViewportBorders() const
{
	float bdColor[3];
	// try and make it just a bit lighter than the clear color, or invert if that is not possible
	bdColor[0] = m_clearColor[0] < 0.8f? m_clearColor[0] + 0.2f : 1.0f - m_clearColor[0];
	bdColor[1] = m_clearColor[1] < 0.8f? m_clearColor[1] + 0.2f : 1.0f - m_clearColor[1];
	bdColor[2] = m_clearColor[2] < 0.8f? m_clearColor[2] + 0.2f : 1.0f - m_clearColor[2];

	float cvColor[3];
	// current viewport color
	cvColor[0] = 1.0f;
	cvColor[1] = 245/255.0f;
	cvColor[2] = 77/255.0f;

	m_context->setCurrentColor3( bdColor ); 
	m_context->beginGroup( HKG_IMM_LINES );
	
	int nv = m_viewports.getSize();
	int i;
	int illx, illy;
	int iurx, iury;

	for ( i = 0; i < nv;++i)
	{
		if (i != m_currentViewport)
		{
			hkgViewport* cv = m_viewports[i];

			cv->getLowerLeftCoord(illx, illy);
			cv->getUpperRightCoord(iurx, iury);

			if (m_viewportThickGrayBorders)
			{
				illx += 2; illy += 2;
				iurx -= 2; iury -= 2;
			}

			draw2DBox(m_context, illx, illy, iurx, iury);
		}
	}

	if ( m_viewportThickGrayBorders ) 
	{
		const float grayColor[3] = {212/255.0f, 208/255.0f, 200/255.0f };
		
		m_context->setCurrentColor3( (float*)grayColor ); 
	
		for ( i = 0; i < nv;++i)
		{
			hkgViewport* cv = m_viewports[i];

			cv->getLowerLeftCoord(illx, illy);
			cv->getUpperRightCoord(iurx, iury);

			draw2DBox(m_context, illx, illy, iurx, iury);
			draw2DBox(m_context, illx+1, illy+1, iurx-1, iury-1);	
		}
	}

	// Current viewport indicator., last so always ontop
	{
#ifndef HK_PLATFORM_PS2 // dodgy OGL impl.. no color inside group unless all have colors.
		m_context->setCurrentColor3( cvColor ); 
#endif

		hkgViewport* cv = m_viewports[m_currentViewport];
		cv->getLowerLeftCoord(illx, illy);
		cv->getUpperRightCoord(iurx, iury);

		if (m_viewportThickGrayBorders)
		{
			illx += 1; illy += 1;
			iurx -= 1; iury -= 1;
		}

		draw2DBox(m_context, illx, illy, iurx, iury);
	}

	m_context->endGroup();
}

HKG_WINDOW_MSG_STATUS hkgWindow::doViewportResizeLoop()
{
	HKG_WINDOW_MSG_STATUS lastMessage = peekMessages(true);

	m_windowOrthoView->setAsCurrent( m_context );
	
	while ( (lastMessage == HKG_WINDOW_MSG_CONTINUE) && (m_viewportResizeArray.getSize() > 0) )
	{
		// resize viewports based on current mouse po

		bool inMove = m_mouse.getButtonState() & HKG_MOUSE_RIGHT_BUTTON;

		int sx = hkg_clampi( m_mouse.m_curX, 0, getWidth()-1 );
		int sy = hkg_clampi( m_mouse.m_curY, 0, getHeight()-1 );
	
		int nv = m_viewportResizeArray.getSize();
		int i;
		
		int cLL[2];
		int cUR[2];

		int nLL[2];
		int nUR[2];

		for (i = 0; i < nv;++i)
		{
			hkgViewport* rv = m_viewportResizeArray[i].m_viewport;
			HKG_VIEWPORT_SIDE sides = m_viewportResizeArray[i].m_sides;

			rv->getLowerLeftCoord( cLL[0], cLL[1] );
			rv->getUpperRightCoord( cUR[0], cUR[1] );

			if (inMove) // move
			{
				int deltaX = (sides & HKG_VIEWPORT_SIDE_LEFT) ? (sx - cLL[0]):
							 ( (sides & HKG_VIEWPORT_SIDE_RIGHT) ? (sx - cUR[0]): 0 );
							 
				int deltaY = (sides & HKG_VIEWPORT_SIDE_BOTTOM) ? (sy - cLL[1]):
						     ( (sides & HKG_VIEWPORT_SIDE_TOP) ? (sy - cUR[1]) : 0 );

			//	printf( "deltas [%d %d]\n", deltaX, deltaY);

				nLL[0] = cLL[0] + deltaX;
				nLL[1] = cLL[1] + deltaY;
				nUR[0] = cUR[0] + deltaX;
				nUR[1] = cUR[1] + deltaY;
				
			}
			else // selective resize
			{
				if (sides & HKG_VIEWPORT_SIDE_LEFT)
					nLL[0] = sx;
				else
					nLL[0] = cLL[0]; // leave unchanged

				if (sides & HKG_VIEWPORT_SIDE_BOTTOM)
					nLL[1] = sy;
				else
					nLL[1] = cLL[1]; // leave unchanged


				if (sides & HKG_VIEWPORT_SIDE_RIGHT)		
					nUR[0] = sx;
				else
					nUR[0] = cUR[0]; // leave unchanged

				if (sides & HKG_VIEWPORT_SIDE_TOP)
					nUR[1] = sy;
				else
					nUR[1] = cUR[1]; // leave unchanged
			}

			float a = nLL[0] / (float)getWidth();
			float b = nLL[1] / (float)getHeight();
			float c = nUR[0] / (float)getWidth();
			float d = nUR[1] / (float)getHeight();
	
			a = hkg_clampf(a, 0.0f, 1.0f);
			b = hkg_clampf(b, 0.0f, 1.0f);
			c = hkg_clampf(c, 0.0f, 1.0f);
			d = hkg_clampf(d, 0.0f, 1.0f);

		//	printf("Viewport %d: [%.3f %.3f %.3f %.3f]\n", i, a,b,c,d);

			rv->setExtentsRelative(a, b, c, d);
			
		}

		clearBuffers();	

			drawViewportBorders();

 		swapBuffers();

		lastMessage = peekMessages(true);
	}

	getCurrentViewport()->setAsCurrent(m_context);

	return lastMessage;
}	

void hkgWindow::waitMessages()
{

}

bool hkgWindow::setAsCurrent()
{
	return false;
}

void hkgWindow::processMouseMove(int x, int y)
{
	m_mouse.m_valid = true;

	int deltaX = x - m_mouse.m_curX;
	int deltaY = y - m_mouse.m_curY;
	m_mouse.m_curX = x;
	m_mouse.m_curY = y;

	if (m_mouseMoveFunc && m_mouseMoveFunc( this, x, y, deltaX, deltaY, m_mouseMoveFuncUserContext))
		return;

	if (m_mouse.getButtonState() & HKG_MOUSE_ANY_BUTTON)
	{
		if (m_currentViewport >= 0 && m_viewports[m_currentViewport]->getAcceptsMouseEvents())
		{
			hkgViewport* v = m_viewports[m_currentViewport];
			
			if ( (deltaX == 0) && (deltaY == 0))
				return; // no movement..

			if (deltaX < -MAX_MOUSE_DELTA)
				deltaX = -MAX_MOUSE_DELTA;
			else if (deltaX > MAX_MOUSE_DELTA)
				deltaX = MAX_MOUSE_DELTA;
			
			if (deltaY < -MAX_MOUSE_DELTA)
				deltaY = -MAX_MOUSE_DELTA;
			else if (deltaY > MAX_MOUSE_DELTA)
				deltaY = MAX_MOUSE_DELTA;

			v->processMouseMove(x,y, deltaX, deltaY);
		}
	}
}

void hkgWindow::processMouseWheel(int delta, int x, int y)
{
	m_mouse.m_valid = true;

	m_mouse.m_curX = x;
	m_mouse.m_curY = y;

	if (m_mouseWheelFunc && m_mouseWheelFunc( this, delta, m_mouseWheelFuncUserContext) )
		return;

	if (m_currentViewport >= 0 && m_viewports[m_currentViewport]->getAcceptsMouseEvents())
	{
		hkgViewport* v = m_viewports[m_currentViewport];
		v->processMouseWheel(delta, x,y);
	}
}

void hkgWindow::processMouseButton(HKG_MOUSE_BUTTON button, bool state, int x, int y)
{
	m_mouse.m_valid = true;

	m_mouse.m_curX = x;
	m_mouse.m_curY = y;

	if (state)
		m_mouse.m_curButtonState |= button;
	else
		m_mouse.m_curButtonState &= ~button;

	if (state)
	{

		int nv = m_viewports.getSize();
		int i;

		if (m_viewportResizeByMouse && ( button & (HKG_MOUSE_LEFT_BUTTON | HKG_MOUSE_RIGHT_BUTTON)) )
		{
			// have to check for hotspots (borders)
			// If it is in a hotspot, we have to do resize of viewports until user
			// releases. The resize will happend the next time the user calls peekMessages()
			for (i = 0; i < nv; ++i)
			{
				hkgViewport* v = m_viewports[i];
				
				HKG_VIEWPORT_SIDE sides = v->isWindowCoordOnBorder(x, y, 3);// 3 pixel border
				
				if (sides != HKG_VIEWPORT_SIDE_NONE) 
				{
					hkgViewportMouseResize r;
					r.m_viewport = v;
					r.m_sides = sides;
					m_viewportResizeArray.pushBack( r );
				}
			}	
		}
	
		if (m_viewportResizeArray.getSize() == 0)
		{
			// find out if still in current viewport
			for (i = 0; i < nv; ++i)
			{
				hkgViewport* v = m_viewports[i];

				if (v->containsWindowCoord(x, y))
				{
					m_currentViewport = i;
					v->acceptMouseFocus();
					break;
				}
			}

		//	if (i == nv)
		//		m_currentViewport = 0;
		}
	}
	else  // button up
	{
		if (m_viewportResizeByMouse && ( button & (HKG_MOUSE_LEFT_BUTTON | HKG_MOUSE_RIGHT_BUTTON)) )
			m_viewportResizeArray.clear();
	}


	if (m_mouseButtonFunc && m_mouseButtonFunc( this, button, state, m_mouseButtonFuncUserContext) )
		return;

	if ((m_viewportResizeArray.getSize() == 0) && m_currentViewport >= 0 && m_viewports[m_currentViewport]->getAcceptsMouseEvents())
	{
		hkgViewport* v = m_viewports[m_currentViewport];
		v->processMouseButton(button, state, x,y);
	}

}

void hkgWindow::processKey(HKG_KEYBOARD_VKEY c, bool state)
{
	m_keyboard.m_valid = true;

	HK_ASSERT( c < HKG_KEYBOARD_NUM_VKEYS);
	m_keyboard.m_curState[c] = state;
	
	if (m_keyFunc)
		m_keyFunc( this, c, state, m_keyFuncUserContext);
}

void hkgWindow::processPadStickMove( short pad, short stick, float x, float y)
{
	m_pad[pad].m_valid = true;

	float dx = x - m_pad[pad].m_stick[stick].m_cur[0];
	float dy = y - m_pad[pad].m_stick[stick].m_cur[1];
	m_pad[pad].m_stick[stick].m_cur[0] = x;
	m_pad[pad].m_stick[stick].m_cur[1] = y;

	if (m_virtualMouseEnabled && (stick == 0))
	{
		m_virtualMousePos[0] += (int)((x * HKG_VIRTUAL_MOUSE_SPEED) * m_width);
		m_virtualMousePos[1] += (int)((y * HKG_VIRTUAL_MOUSE_SPEED) * m_height);
		hkg_clampi( m_virtualMousePos[0], 0, m_width);
		hkg_clampi( m_virtualMousePos[1], 0, m_height);

		processMouseMove( m_virtualMousePos[0], m_virtualMousePos[1] ); 
	}

	if (m_padMoveFunc && m_padMoveFunc(this, pad, stick, x, y, dx, dy, m_padMoveFuncUserContext) )
		return;

	if (m_currentViewport >= 0 && m_viewports[m_currentViewport]->getAcceptsMouseEvents())
	{
		hkgViewport* v = m_viewports[m_currentViewport];
		v->processPadStickMove(pad, stick, x,y, dx, dy );
	}

	
}

void hkgWindow::processPadButton( short pad, HKG_PAD_BUTTON button, bool state)
{
	m_pad[pad].m_valid = true;
	
	HKG_PAD_BUTTON& curButtonState = m_pad[pad].m_curButtonState;

	if (state)
		curButtonState |= button;
	else
		curButtonState &= ~button;	

	if (m_virtualMouseEnabled)
	{	
		// middle mouse button down, L1 + R1
		if (state && (curButtonState & HKG_PAD_BUTTON_L1) && (curButtonState & HKG_PAD_BUTTON_R1)) // L1 + R1
		{
			processMouseButton(HKG_MOUSE_MIDDLE_BUTTON, state, m_virtualMousePos[0], m_virtualMousePos[1]);
		}
		// middle mouse button up, L1 + R1
		else if (!state && ( ((button & HKG_PAD_BUTTON_L1) && (curButtonState & HKG_PAD_BUTTON_R1)) || // L1 down, R1 up
						 	 ((button & HKG_PAD_BUTTON_R1) && (curButtonState & HKG_PAD_BUTTON_L1)) )) // R1 down, L1 up
		{
			processMouseButton(HKG_MOUSE_MIDDLE_BUTTON, state, m_virtualMousePos[0], m_virtualMousePos[1]);
		}
		// left mouse button, L1
		else if (button & HKG_PAD_BUTTON_L1)
		{
			processMouseButton(HKG_MOUSE_LEFT_BUTTON, state, m_virtualMousePos[0], m_virtualMousePos[1]);
		}
		// right mouse button, R1
		else if (button & HKG_PAD_BUTTON_R1)
		{
			processMouseButton(HKG_MOUSE_RIGHT_BUTTON, state, m_virtualMousePos[0], m_virtualMousePos[1]);
		}
	}

	if (m_padButtonFunc && m_padButtonFunc(this, pad, button, state, m_padButtonFuncUserContext) )
		return;
	
	if (m_currentViewport >= 0) // && m_viewports[m_currentViewport]->getAcceptsMouseEvents())
	{
		hkgViewport* v = m_viewports[m_currentViewport];
		v->processPadButton(pad, button, state);
	}	
}	

bool hkgWindow::processWindowCommand(unsigned int code, unsigned int id, void* nativeHandle)
{
	if (m_windowCommandFunc)
		return m_windowCommandFunc(this, code, id, nativeHandle, m_windowCommandFuncUserContext);

	return false;
}

void hkgWindow::setMousePosition(int x, int y) 
{
	setVirtualMousePos(x,y);
	m_mouse.m_curX = x;
	m_mouse.m_curY = y;
	m_mouse.m_prevX = x;
	m_mouse.m_prevY = y;
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
