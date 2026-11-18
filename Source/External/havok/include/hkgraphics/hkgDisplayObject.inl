/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

hkgDisplayObject::~hkgDisplayObject()
{
	int ng;
	int i;

	ng = m_solidGeoms.getSize();
	for (i =0; i < ng;++i)
		m_solidGeoms[i]->release();

	ng = m_alphaGeoms.getSize();
	for (i =0; i < ng;++i)
		m_alphaGeoms[i]->release();

	ng = m_mixedGeoms.getSize();
	for (i =0; i < ng;++i)
		m_mixedGeoms[i]->release();

}

inline void hkgDisplayObject::setStatusFlag( HKG_DISPLAY_OBJECT_STATUS f)
{
	m_status = f;
}

inline HKG_DISPLAY_OBJECT_STATUS hkgDisplayObject::getStatusFlag() const
{
	return m_status;
}

inline void hkgDisplayObject::setTransform( const float* t, bool isIdentity) 
{ 
	HK_ASSERT( t != HK_NULL );
	hkgMat4Copy( m_trans, t ); 

	if (!m_zeroOffset)
	{
		hkgMat4RotateAndAdd( m_trans, m_offset );
		m_identityTrans = false;
	}
	else
		m_identityTrans = isIdentity;
} 

inline const float*	hkgDisplayObject::getTransform() const 
{ 
	return m_trans;
}

inline bool hkgDisplayObject::getIdentityFlag() const 
{ 
	return m_identityTrans; 
}

inline void hkgDisplayObject::setIdentityFlag(bool t) 
{ 
	m_identityTrans = t; 
}

inline void hkgDisplayObject::setTransToIdentity() 
{ 
	hkString::memSet( m_trans, 0, 16*sizeof(float)); 
	m_trans[0] = m_trans[5] = m_trans[10] = m_trans[15] = 1.0f; 
	m_identityTrans = true; 
} 

inline void hkgDisplayObject::addGeometry(hkgGeometry* g) 
{ 
	g->reference();

	HKG_GEOMETRY_TRANSPARENCY t = g->getTransparencyType();
	m_geomTransp |= t;

	switch (t)
	{
		case HKG_GEOMETRY_TRANS_OPAQUE:
			m_solidGeoms.pushBack(g); 
			break;
		case HKG_GEOMETRY_TRANS_TRANSPARENT:
			m_alphaGeoms.pushBack(g); 
			break;
		default : // mixed or unknown
			m_mixedGeoms.pushBack(g); 
			break;
	}
}

inline int hkgDisplayObject::getNumGeometry() const 
{ 
	return ( m_solidGeoms.getSize() + m_alphaGeoms.getSize() + m_mixedGeoms.getSize() );
}

inline hkgGeometry* hkgDisplayObject::getGeometry(int i) 
{ 
	HK_ASSERT( i >= 0 && i < getNumGeometry());
	int ss = m_solidGeoms.getSize();
	if (i < ss)
		return m_solidGeoms[i]; 
	else if ( i < (ss + m_alphaGeoms.getSize()) )
		return m_alphaGeoms[i-ss]; 
	else
		return m_mixedGeoms[i-(ss+m_alphaGeoms.getSize())]; 
}

inline const hkgGeometry* hkgDisplayObject::getGeometry(int i) const 
{ 
	return const_cast<hkgDisplayObject*>(this)->getGeometry(i);
}

inline hkArray<hkgGeometry*>* hkgDisplayObject::remapIndex( int& index ) const
{
	HK_ASSERT( index < getNumGeometry() );
	
	int ss = m_solidGeoms.getSize();
	hkArray<hkgGeometry*>* ga = HK_NULL;

	if (index < ss)
	{
		ga = const_cast<hkArray<hkgGeometry*>*>( &m_solidGeoms );
	}
	else if ( index < (ss + m_alphaGeoms.getSize()) )
	{
		index -= ss;
		ga = const_cast<hkArray<hkgGeometry*>*>( &m_alphaGeoms ); 
	}
	else
	{
		index -= (ss - m_alphaGeoms.getSize());
		ga = const_cast<hkArray<hkgGeometry*>*>( &m_mixedGeoms ); 
	}

	return ga;
}

inline hkgGeometry* hkgDisplayObject::removeGeometry(int i, bool preserveOrder)
{
	int index = i;
	hkArray<hkgGeometry*>* ga = remapIndex( index );

	hkgGeometry* g = (*ga)[index];
	if (preserveOrder)
		ga->removeAtAndCopy(index);
	else
		ga->removeAt(index);

	if (ga->getSize() == 0)
		computeTransparency();

	return g;
}

inline hkgGeometry* hkgDisplayObject::searchAndRemoveByName( hkArray<hkgGeometry*>* geomArray, const char* name, bool preserveOrder)
{
	int s = geomArray->getSize();
	int i = 0;

	while(i < s)
	{
		if ( hkString::strCmp(name, (*geomArray)[i]->getName().cString()) == 0 )
		{
			hkgGeometry* g = (*geomArray)[i];
		
			if (preserveOrder)
				geomArray->removeAtAndCopy(i);
			else
				geomArray->removeAt(i);

			if (geomArray->getSize() == 0)
				computeTransparency();	

			return g;
		}
	}
	return HK_NULL;
}

inline hkgGeometry* hkgDisplayObject::removeGeometry(const char* name, bool preserveOrder)
{
	hkgGeometry* g = searchAndRemoveByName( &m_solidGeoms, name, preserveOrder );
	if (g)
		return g;

	g = searchAndRemoveByName( &m_alphaGeoms, name, preserveOrder );
	if (g)
		return g;

	g = searchAndRemoveByName( &m_mixedGeoms, name, preserveOrder );
	return g;
}


inline void hkgDisplayObject::setName( const char* s)
{
	m_name = s;
}

inline const hkString& hkgDisplayObject::getName() const
{
	return m_name;
}

inline void hkgDisplayObject::setGeometryOffset( const float o[3] )
{
	hkgVec3Copy(m_offset, o);
	m_zeroOffset = 	(hkg_fabs(m_offset[0]) < 0.001f) &&
   					(hkg_fabs(m_offset[1]) < 0.001f) &&
					(hkg_fabs(m_offset[2]) < 0.001f);
	
}

inline const float* hkgDisplayObject::getGeometryOffset() const
{
	return m_offset;
}

inline float* hkgDisplayObject::getGeometryOffset()
{
	return m_offset;
}

inline void hkgDisplayObject::setOffsetToZero()
{
	hkgVec3Zero( m_offset );
	m_zeroOffset = true;
}

inline HKG_GEOMETRY_TRANSPARENCY hkgDisplayObject::getTransparencyType() const
{
	return m_geomTransp;
}
		
inline bool hkgDisplayObject::hasAABB() const
{
	return m_computedRadius > 0;
}

inline bool hkgDisplayObject::boundsVisibleInFrustum( const hkgCamera* camera )
{
	if (m_computedRadius < 0.001f)
		return true;

	float wc[3];
	hkgVec3Copy( wc, m_aabbCent);
	if (!m_identityTrans)
		hkgVec3Add( wc, &(m_trans[12]));

	return camera->sphereVisible( wc, m_computedRadius );
}

const float* hkgDisplayObject::getAABBCentPtr() const
{
	return m_aabbCent;
}

const float* hkgDisplayObject::getAABBExtPtr() const
{
	return m_aabbExtent;
}

inline hkgDisplayObject::hkgDisplayObject() 
: m_geomTransp( HKG_GEOMETRY_TRANS_UNKNOWN ), m_computedRadius(-1), m_status( HKG_DISPLAY_OBJECT_DYNAMIC )
{ 
	setTransToIdentity(); 
	setOffsetToZero();
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
