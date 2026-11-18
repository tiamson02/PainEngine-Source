/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

hkgGeometry::hkgGeometry()
: m_computedAABB(false),
  m_trans(HKG_GEOMETRY_TRANS_UNKNOWN)

{
	m_aabbMin[0] = m_aabbMin[1] = m_aabbMin[2] =  3e+38f;
	m_aabbMax[0] = m_aabbMax[1] = m_aabbMax[2] = -3e+38f;
}

inline const hkgMaterialFaceSet* hkgGeometry::getMaterialFaceSet(int i) const
{
	const int sfs = m_matSolidFaceSets.getSize();
	HK_ASSERT( i < (sfs + m_matAlphaFaceSets.getSize()) );
	if ( i < sfs )
		return m_matSolidFaceSets[i];
	else
		return m_matAlphaFaceSets[i-sfs];
}

inline hkgMaterialFaceSet* hkgGeometry::getMaterialFaceSet(int i)
{
	const int sfs = m_matSolidFaceSets.getSize();
	HK_ASSERT( i < (sfs + m_matAlphaFaceSets.getSize()) );
	if ( i < sfs )
		return m_matSolidFaceSets[i];
	else
		return m_matAlphaFaceSets[i-sfs];
}

inline void  hkgGeometry::setName( const char* name ) 
{ 
	m_name = name; 
}

const hkString& hkgGeometry::getName() const 
{ 
	return m_name; 
}

inline int hkgGeometry::addMaterialFaceSet( hkgMaterialFaceSet* ms )
{
	int index;
	if (ms->getMaterial() && ms->getMaterial()->isTransparent())
	{
		index = m_matAlphaFaceSets.getSize();
		m_matAlphaFaceSets.pushBack( ms );
		m_trans |= HKG_GEOMETRY_TRANS_TRANSPARENT;
	}
	else
	{
		index = m_matSolidFaceSets.getSize();
		m_matSolidFaceSets.pushBack( ms );
		m_trans |= HKG_GEOMETRY_TRANS_OPAQUE;
	}

	ms->reference();

	return index;
}

inline int hkgGeometry::getNumMaterialFaceSets() const
{
	return ( m_matAlphaFaceSets.getSize() + m_matSolidFaceSets.getSize() );
}

inline void hkgGeometry::render(hkgDisplayContext* context) const
{
	hkArray<hkgMaterialFaceSet*>* afs;
	switch (m_trans)
	{
		case HKG_GEOMETRY_TRANS_OPAQUE: // all opaque
			afs = const_cast<hkArray<hkgMaterialFaceSet*>*>(&m_matSolidFaceSets);
			break;
			
		case HKG_GEOMETRY_TRANS_TRANSPARENT: // all transparent
			afs = const_cast<hkArray<hkgMaterialFaceSet*>*>(&m_matAlphaFaceSets);
			break;

		default:
			mixedModeRender(context);
			return;
	}
			
	
	HK_ASSERT(afs);
	
	int nmfs = afs->getSize();
	for (int i =0; i < nmfs; ++i)
	{
		hkgMaterialFaceSet* fs = (*afs)[i];
		fs->render(context);
	}
}

inline HKG_GEOMETRY_TRANSPARENCY hkgGeometry::getTransparencyType() const
{
	return m_trans;
}


inline hkgMaterialFaceSet* hkgGeometry::removeMaterialFaceSet(int i)
{
	const int sfs = m_matSolidFaceSets.getSize();
	HK_ASSERT( i < (sfs + m_matAlphaFaceSets.getSize()) );

	hkgMaterialFaceSet* ret;

	if ( i < sfs )
	{
		ret = m_matSolidFaceSets[i];
		m_matSolidFaceSets.removeAt(i);
		if (m_matSolidFaceSets.getSize() == 0)
			m_trans &= ~HKG_GEOMETRY_TRANS_OPAQUE; 
	}
	else
	{
		ret = m_matSolidFaceSets[i-sfs];
		m_matAlphaFaceSets.removeAt(i-sfs);
		if (m_matAlphaFaceSets.getSize() == 0)
			m_trans &= ~HKG_GEOMETRY_TRANS_TRANSPARENT; 
	}

	return ret;
}

void hkgGeometry::getAABB(float minE[3], float maxE[3]) const
{
	hkgVec3Copy( minE, m_aabbMin );
	hkgVec3Copy( maxE, m_aabbMax );
}

const float* hkgGeometry::getAABBMaxPtr() const
{
	return m_aabbMax;
}

const float* hkgGeometry::getAABBMinPtr() const
{
	return m_aabbMin;
}

bool hkgGeometry::hasAABB() const
{
	return m_computedAABB;
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
