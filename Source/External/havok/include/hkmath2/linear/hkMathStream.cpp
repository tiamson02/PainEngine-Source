/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkmath2/hkMath.h>
#include <hkmath2/linear/hkMathStream.h>

hkOstream& operator<< (hkOstream& os, const hkVector4& v)
{
	os.printf("[%g,%g,%g,%g]", v(0), v(1), v(2), v(3));
	return os;
}

hkOstream& operator<< (hkOstream& os, const hkQuadReal& q)
{
	const hkReal* r = reinterpret_cast<const hkReal*>(&q);
	os.printf("[%f,%f,%f,%f]", r[0], r[1], r[2], r[3]);
	return os;
}

hkOstream& operator<< (hkOstream& os, const hkQuaternion& q)
{
	os.printf("[%f,%f,%f,(%f)]", q.m_vec(0), q.m_vec(1), q.m_vec(2), q.m_vec(3));
	return os;
}

hkOstream& operator<< (hkOstream& os, const hkMatrix3& m)
{
	for( int i=0; i<3; ++i)
	{
		os.printf("|%f,%f,%f|\n", m(i,0), m(i,1), m(i,2) );
	}
	return os;
}

hkOstream& operator<< (hkOstream& os, const hkTransform& t)
{
	return os << t.getRotation() << t.getTranslation();
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
