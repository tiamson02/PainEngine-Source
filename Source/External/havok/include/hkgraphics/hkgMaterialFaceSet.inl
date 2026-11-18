/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkgMaterialFaceSet::hkgMaterialFaceSet( )
: m_material( HK_NULL )
{
}
	
inline void hkgMaterialFaceSet::addFaceSet(hkgFaceSet* faceSet) 
{ 
	faceSet->reference();
	m_faceSets.pushBack(faceSet); 
}
	
inline int hkgMaterialFaceSet::getNumFaceSets() const
{ 
	return m_faceSets.getSize(); 
} 

inline const hkgFaceSet* hkgMaterialFaceSet::getFaceSet(int i) const 
{ 
	return m_faceSets[i];
} 

inline hkgFaceSet* hkgMaterialFaceSet::getFaceSet(int i) 
{ 
	return m_faceSets[i];
} 

inline const hkgMaterial* hkgMaterialFaceSet::getMaterial() const 
{ 
	return m_material; 
}

inline hkgMaterial* hkgMaterialFaceSet::getMaterial() 
{ 
	return m_material; 
}

inline void hkgMaterialFaceSet::setMaterial( hkgMaterial* m )
{
	if (m)
	{
		m->reference();
	}
	if (m_material)
	{
		m_material->release();
	}
	m_material = m;
}


inline void hkgMaterialFaceSet::reserve( int estimateNum )
{
	if (estimateNum > m_faceSets.getCapacity())
		m_faceSets.reserve(estimateNum);
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
