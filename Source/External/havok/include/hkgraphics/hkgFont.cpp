/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgFont.h>

hkgFont* (*hkgFont::create)();

hkgFont::hkgFont()
    :	m_texture( HK_NULL ),
        m_charWidth( 9 ),
        m_charHeight( 15 )
{
}

hkgFont::~hkgFont()
{
	if (m_texture)
		m_texture->release();
}

void hkgFont::setTexture(hkgTexture* t)
{
	if (m_texture)
		m_texture->release();

	m_texture = t;

	if (t)
		m_texture->reference();
}

void hkgFont::resolve( char c, float& u0, float& v0, float& u1, float& v1)
{
	// only accept bottom 7 bits of the character
	// top bit is set if it is one of the special symbols
	int x = c & 0xf; // bottom 4 bits == x
	int y = 7 - ((c & 0x70) >> 4); // top 3 bits == y
	
	u0 = x / 16.0f;
	v0 = y / 8.0f;
	u1 = (x+1) / 16.0f;
	v1 = (y+1) / 8.0f;

	//hack, fatten character if it is not one of the special ones triangle, square, etc.
	hkReal fattenX = (c & 0x80) ? 0.0f : 0.012f;

	u0 += fattenX;
	u1 -= fattenX;
	//hkReal fattenY = 0.01f;
	//v0 += fattenY;
	//v1 -= fattenY;
}

void hkgFont::setDrawState( hkgDisplayContext* context )
{
	HKG_ENABLED_STATE state = context->getEnabledState();

 	if ((state & HKG_ENABLED_TEXTURE2D) == 0)
		context->setTexture2DState(true);
	if (state & HKG_ENABLED_LIGHTING)
		context->setLightingState(false);
	if (state & HKG_ENABLED_ZWRITE)
		context->setDepthWriteState(false);
	if (state & HKG_ENABLED_ZREAD)
		context->setDepthReadState(false);
	if (state & HKG_ENABLED_WIREFRAME)
		context->setWireframeState(false);
	if ((state & HKG_ENABLED_ALPHABLEND) == 0)
		context->setBlendState(true);
//	if (state & HKG_ENABLED_CULLFACE)
//		context->setCullFaceState(false);
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
