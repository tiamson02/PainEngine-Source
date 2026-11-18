/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKAABBCONVEXCAST_H
#define HKAABBCONVEXCAST_H

#include <hkmath2/hkMath.h>
#include <hkcollide2/shape/hkShape.h>

class hkAabbConvexCast
{
	public:

		static int HK_CALL castAabb(const hkVector4& fromLocal,const hkVector4& toLocal,const hkRotation& localToWorldRotation,const hkArray<hkVector4>& planeEquations, const hkVector4& halfExtents, bool ignorePenetration, hkShape::hkRayResults& results);
	
};

#endif //HKAABBCONVEXCAST_H

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
