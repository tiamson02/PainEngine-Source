/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_RIGIDBODY_DEACTIVATOR_H
#define HK_DYNAMICS2_RIGIDBODY_DEACTIVATOR_H

#include <hkdynamics2/entity/hkEntityDeactivator.h>
#include <hkdynamics2/entity/hkRigidBodyDeactivatorTypes.h>

// An empty class, purely for type safety
class hkRigidBodyDeactivator : public hkEntityDeactivator
{

	public:

		// Pure virtual method that must be implemented by derived classes to return their hkRigidBodyDeactivatorType.
		virtual hkRigidBodyDeactivatorType getRigidBodyDeactivatorType() const = 0;

};


#endif // HK_DYNAMICS2_RIGID_BODYDEACTIVATOR_H

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
