/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkcollide2/hkCollide2.h>
#include <hkcollide2/shape/hkAabbCastShape.h>
#include <hkcollide2/algorithms/boundingvolume/hkAabbUtil.h>
#include <hkcollide2/algorithms/continuous/hkAabbConvexCast.h>

/// Creates a new hkMultiRayShape with the specified rays.
hkAabbCastShape ::hkAabbCastShape( const hkVector4&	fromWorld, const hkVector4& toWorld, const hkVector4&	halfExtents )
{
	m_fromWorld = fromWorld;
	m_toWorld = toWorld;
	m_halfExtents = halfExtents;
	m_ignorePenetration = false;

}

	// hkShape implementation.
void hkAabbCastShape ::getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out  ) const
{
	hkVector4 halfExtent;
	halfExtent.setSub4(m_aabb.m_max , m_aabb.m_min);
	halfExtent.mul4(0.5f);

	hkVector4 centre;
	centre.setAdd4(m_aabb.m_max , m_aabb.m_min);
	centre.mul4(0.5f);

	hkAabbUtil::calcAabb( localToWorld, halfExtent, centre, tolerance, out );
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
