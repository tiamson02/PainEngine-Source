/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


inline bool hkgMaterialManager::materialIsTextured(int i) const
{
	HK_ASSERT( i < m_materials.getSize() );
	return ( m_materials[i]->getTexture() != HK_NULL );
}
	
inline hkgMaterial* hkgMaterialManager::getMaterial(int i)
{
	HK_ASSERT( i < m_materials.getSize() );
	return m_materials[i];
}

inline const hkgMaterial* hkgMaterialManager::getMaterial(int i) const
{
	HK_ASSERT( i < m_materials.getSize() );
	return m_materials[i];
}

inline const hkgMaterial* hkgMaterialManager::getDefaultMaterial(int i) const
{
	HK_ASSERT( i < m_defMaterials.getSize() );
	return m_defMaterials[i];
}

inline hkgMaterial* hkgMaterialManager::getDefaultMaterial(int i)
{
	HK_ASSERT( i < m_defMaterials.getSize() );
	return m_defMaterials[i];
}

inline void hkgMaterialManager::clearMaterials()
{
	// release all materials
	int nm = m_materials.getSize();	
	for (int i =0; i < nm; ++i)
		m_materials[i]->release();

	m_materials.clear();
}

inline void hkgMaterialManager::clearDefaultMaterials()
{
	// release all default materials
	int nm = m_defMaterials.getSize();	
	for (int i =0; i < nm; ++i)
		m_defMaterials[i]->release();

	m_defMaterials.clear();
}

inline int hkgMaterialManager::addMaterial(hkgMaterial* m)
{
	m_materials.pushBack(m);
	m->reference();
	return ( m_materials.getSize() - 1 );
}

inline int hkgMaterialManager::getNumMaterials() const
{
	return m_materials.getSize();
}
	
inline int hkgMaterialManager::getNumDefaultMaterials() const
{
	return m_defMaterials.getSize();
}
	
hkgMaterialManager::hkgMaterialManager(hkgDisplayContext* context)
{
	createDefaultMaterials(context);
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
