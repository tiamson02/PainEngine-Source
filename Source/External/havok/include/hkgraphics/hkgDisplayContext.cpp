/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgViewport.h>
#include <hkgraphics/hkgTexture.h>

hkgDisplayContext* (*hkgDisplayContext::create)(hkgWindow*);
int hkgDisplayContext::s_maxNumLights;

hkgDisplayContext::~hkgDisplayContext()
{
}

void hkgDisplayContext::setCurrentTexture(hkgTexture* t)
{
	if (m_currentBoundTexture == t)
		return;

	if (m_currentBoundTexture)
		m_currentBoundTexture->release();

	m_currentBoundTexture = t;

	if (m_currentBoundTexture)
	{
		m_currentBoundTexture->reference();
		m_currentBoundTexture->bind();
	}	
}

void hkgDisplayContext::matchState( HKG_ENABLED_STATE desiredState, HKG_CULLFACE_MODE desiredCullfaceMode )
{
	HKG_ENABLED_STATE state = (HKG_ENABLED_STATE) (desiredState & HKG_ENABLED_LIGHTING);
	if ( state != (m_currentState & HKG_ENABLED_LIGHTING) )
		setLightingState( state != 0 );
	
	state = (HKG_ENABLED_STATE)( desiredState & HKG_ENABLED_TEXTURE2D );
	if ( state != (m_currentState & HKG_ENABLED_TEXTURE2D) )
		setTexture2DState( state != 0);
	
	state = (HKG_ENABLED_STATE)( desiredState & HKG_ENABLED_ALPHABLEND );
	if ( state != (m_currentState & HKG_ENABLED_ALPHABLEND) )
		setBlendState( state != 0);
	
	state = (HKG_ENABLED_STATE)( desiredState & HKG_ENABLED_ZREAD );
	if ( state != (m_currentState & HKG_ENABLED_ZREAD) )
		setDepthReadState( state != 0);
	
	state = (HKG_ENABLED_STATE)( desiredState & HKG_ENABLED_ZWRITE);
	if ( state != (m_currentState & HKG_ENABLED_ZWRITE) )
		setDepthWriteState( state != 0);
	
	// make sure the cull mode is ok before set the enabled state
	m_cullfaceMode = desiredCullfaceMode;

	state = (HKG_ENABLED_STATE)( desiredState & HKG_ENABLED_CULLFACE );
	if ( state != (m_currentState & HKG_ENABLED_CULLFACE) )
		setCullFaceState( state != 0);
	
	state = (HKG_ENABLED_STATE)( desiredState & HKG_ENABLED_WIREFRAME );
	if ( state != (m_currentState & HKG_ENABLED_WIREFRAME) )
		setWireframeState( state != 0);
}

void hkgDisplayContext::syncState()
{
	HKG_ENABLED_STATE state;
	
	state = (HKG_ENABLED_STATE)( m_currentState & HKG_ENABLED_LIGHTING );
	setLightingState( state != 0 );
	
	state = (HKG_ENABLED_STATE)( m_currentState & HKG_ENABLED_TEXTURE2D );
	setTexture2DState( state != 0);
	
	state = (HKG_ENABLED_STATE)( m_currentState & HKG_ENABLED_ALPHABLEND );
	setBlendState( state != 0);
	
	state = (HKG_ENABLED_STATE)( m_currentState & HKG_ENABLED_ZREAD );
	setDepthReadState( state != 0);
	
	state = (HKG_ENABLED_STATE)( m_currentState & HKG_ENABLED_ZWRITE );
	setDepthWriteState( state != 0);
	
	state = (HKG_ENABLED_STATE)( m_currentState & HKG_ENABLED_CULLFACE );
	setCullFaceState( state != 0);
	
	state = (HKG_ENABLED_STATE)( m_currentState & HKG_ENABLED_WIREFRAME );
	setWireframeState( state != 0);
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
