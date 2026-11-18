/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgMaterialManager.h>

#define NUM_DEFAULT_MATERIALS 7
static const unsigned int defaultColors[NUM_DEFAULT_MATERIALS] = 
{
	0xffff3333, 
	0xff33ff33, 
	0xff3333ff,
	0xffff33ff,
	0xffffff33,
	0xff33ffff,
	0xffffffff,
};
hkgMaterialManager* (*hkgMaterialManager::create)(hkgDisplayContext* context);

hkgMaterialManager::~hkgMaterialManager()
{
	clearMaterials();
	clearDefaultMaterials();
}

void hkgMaterialManager::realizeTextures(hkgDisplayContext* context)
{
	int nm = m_materials.getSize();
	for (int i =0; i < nm; ++i)
	{
		hkgTexture* t = m_materials[i]->getTexture();
		if (t)
			t->realize();
	}
}

void hkgMaterialManager::createDefaultMaterials(hkgDisplayContext* context)
{
//#define DEFAULT_TEXTURE
#ifdef DEFAULT_TEXTURE
	hkgTexture* defTexture = hkgTexture::create(context);
	defTexture->loadBuiltIn( HKG_TEXTURE_CHECKER );
#endif
	
	for (int i = 0; i < NUM_DEFAULT_MATERIALS; ++i)
	{
		hkgMaterial* m = hkgMaterial::create();

		unsigned int c = defaultColors[i];

		m->setDiffuseColor(	hkgColor4GetRed(c), 
							hkgColor4GetGreen(c), 
							hkgColor4GetBlue(c) );
#ifdef DEFAULT_TEXTURE
		m->setTexture(defTexture);
#endif
		m_defMaterials.pushBack(m);
	}

#ifdef DEFAULT_TEXTURE
	defTexture->realize();
	defTexture->release();
#endif
}


void hkgMaterialManager::print(hkOstream& stream) const
{
	stream << "[" << m_materials.getSize() << "] materials.\n";
	stream << "[" << m_defMaterials.getSize() << "] default materials.\n";
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
