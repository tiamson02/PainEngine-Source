/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgMaterial.h>
#include <hkgraphics/hkgViewport.h> 

hkgMaterial* (*hkgMaterial::create)();

hkgMaterial::~hkgMaterial()
{
	if (m_texture)
		m_texture->release();
}

void hkgMaterial::unbind(hkgDisplayContext* context)
{
	context->setTexture2DState(false);
	context->setCurrentTexture( HK_NULL );
}

void hkgMaterial::bind(hkgDisplayContext* context) const
{
	HKG_ENABLED_STATE s = context->getEnabledState();
	HKG_ENABLED_STATE ds = context->getCurrentViewport()->getDesiredState();

	// if its transparent due to texture, and textire is disabled.. 
	// then it really isn't transparent.

	const bool haveTextures = (ds & HKG_ENABLED_TEXTURE2D) != 0;
	const bool reallyTransparent = (haveTextures && m_texture && m_texture->isTransparent() ) || (m_diffuse[3] < 0.99f);

	if (reallyTransparent)
	{
		if ( (s & HKG_ENABLED_ALPHABLEND) == 0 )
			context->setBlendState( true );
	}
	else if (s & HKG_ENABLED_ALPHABLEND)
	{
		context->setBlendState( false );
	}
	

	// if we should have textures
	if (m_texture && haveTextures) 
	{
		if ( (s & HKG_ENABLED_TEXTURE2D) == 0)
			context->setTexture2DState( true );
		
		if (context->getCurrentTexture() != m_texture)
			context->setCurrentTexture( m_texture );
	}
	else if (s & HKG_ENABLED_TEXTURE2D)
	{
		context->setTexture2DState( false );
	}
}

void hkgMaterial::print(hkOstream& stream) const
{
	stream << "\t\t\t\t\tTexture: \n";
	if (m_texture)
	//	m_texture->print(stream);
		stream << "\t\t\t\t\t[yes :" << (unsigned int)(m_texture) << "]\n";
	else
		stream << "\t\t\t\t\t[NONE]\n";

	stream << "\t\t\t\t\t[" << m_diffuse[0] << " " << m_diffuse[1] << " " << m_diffuse[2] << " " << m_diffuse[3] << "] diffuse color\n";
	stream << "\t\t\t\t\t[" << m_specular[0] << " " << m_specular[1] << " " << m_specular[2] << "] specular color\n";
	stream << "\t\t\t\t\t[" << (m_isTransparent? "transparent" : "opaque") << "]\n";
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
