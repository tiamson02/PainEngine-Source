/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_MATH2_MOTION_STATE_H
#define HK_MATH2_MOTION_STATE_H

#include <hkmath2/hkMath.h>


/// THIS IS AN INTERNAL STRUCT ONLY. Please use the hkRigidBody and hkPhantom interfaces instead of accessing this struct directly.
/// An hkMotionState holds position and orientation over delta time. It includes information that allows for tims and 
/// continuous collision detection.
struct hkMotionState
{

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MATH, hkMotionState)	

	hkVector4 m_rootPosition0;
	hkVector4 m_rootPosition1;

	hkTransform m_sceneGraphTransform1;
};


#endif // HK_MATH2_MOTION_STATE_H

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
