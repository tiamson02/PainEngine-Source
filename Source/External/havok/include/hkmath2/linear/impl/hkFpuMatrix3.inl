/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkVector4& hkMatrix3::getColumn(int x)
{
	return m_cols[x];
}

inline const hkVector4& hkMatrix3::getColumn(int x) const
{
	return m_cols[x];
}

inline hkReal& hkMatrix3::operator() (int r, int c)
{
	return getColumn(c)(r);
}

inline const hkReal& hkMatrix3::operator() (int r, int c) const
{
	return getColumn(c)(r);
}


inline void hkMatrix3::setRows( const hkVector4& r0,
								const hkVector4& r1,
								const hkVector4& r2)
{
	getColumn(0).set( r0(0), r1(0), r2(0) );
	getColumn(1).set( r0(1), r1(1), r2(1) );
	getColumn(2).set( r0(2), r1(2), r2(2) );
}

inline void hkMatrix3::getRows( hkVector4& r0,
								hkVector4& r1,
								hkVector4& r2)
{
	r0.set( getColumn(0)(0), getColumn(1)(0), getColumn(2)(0) );
	r1.set( getColumn(0)(1), getColumn(1)(1), getColumn(2)(1) );
	r2.set( getColumn(0)(2), getColumn(1)(2), getColumn(2)(2) );
}

inline void hkMatrix3::setCols( const hkVector4& r0,
								const hkVector4& r1,
								const hkVector4& r2)
{
	getColumn(0) = r0;
	getColumn(1) = r1;
	getColumn(2) = r2;
}

inline void hkMatrix3::operator= ( const hkMatrix3& a )
{
	setCols( a.getColumn(0), a.getColumn(1), a.getColumn(2) );
}

inline void hkMatrix3::getCols( hkVector4& r0,
								hkVector4& r1,
								hkVector4& r2) const
{
	r0 = getColumn(0);
	r1 = getColumn(1);
	r2 = getColumn(2);
}

inline void hkMatrix3::getRow( int row, hkVector4& r) const
{
	r.set( getColumn(0)(row), getColumn(1)(row), getColumn(2)(row) );
}

inline void hkMatrix3::setZero()
{
	getColumn(0).setZero4();
	getColumn(1).setZero4();
	getColumn(2).setZero4();
}

inline void hkMatrix3::setDiagonal( hkReal m00, hkReal m11, hkReal m22 )
{
	setZero();
	(*this)(0,0) = m00;
	(*this)(1,1) = m11;
	(*this)(2,2) = m22;
}

inline void hkMatrix3::setIdentity()
{
	extern hkReal hkTransformIdentity[];
	(*this) = reinterpret_cast<hkMatrix3&>(hkTransformIdentity);
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
