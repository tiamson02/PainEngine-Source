/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline bool hkVector4::equals3(const hkVector4 &v, hkReal epsilon) const
{
	//non_euclidean, manhattan based
	hkVector4 t;
	t.setSub4(*this, v);
	t.setAbs4( t );
	hkVector4 epsilon_v;
	epsilon_v.setAll3( epsilon );
	int compare_mask = epsilon_v.compareLessThan4( t ) & HK_VECTOR3_COMPARE_MASK_XYZ;
	return (compare_mask == 0);
}

inline bool hkVector4::equals4(const hkVector4 &v, hkReal epsilon) const
{
	//non_euclidean, manhattan based
	hkVector4 t;
	t.setSub4(*this, v);
	t.setAbs4( t );
	hkVector4 epsilon_v;
	epsilon_v.setAll( epsilon );
	int compare_mask = epsilon_v.compareLessThan4( t );
	return (compare_mask == 0);
}

inline int	hkVector4::getMajorAxis() const
{
	hkVector4 tmp;
	tmp.setAbs4(*this);
	return tmp(0) < tmp(1) ? (tmp(1) < tmp(2) ? 2 : 1) : (tmp(0) < tmp(2) ? 2 : 0);
}

inline void hkVector4::setAddMul4(const hkVector4& a, const hkVector4& b, hkSimdRealParameter r)
{
	*this = a;
	this->addMul4(r,b);
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
