/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_FAKE_RIGIDBODY_DEACTIVATOR_H
#define HK_DYNAMICS2_FAKE_RIGIDBODY_DEACTIVATOR_H

#include <hkdynamics2/entity/hkRigidBodyDeactivator.h>

// Class for all objects that are never deactivated.
class hkFakeRigidBodyDeactivator : public hkRigidBodyDeactivator
{

	public:

			/// Returns the deactivator type.
		virtual hkRigidBodyDeactivatorType getRigidBodyDeactivatorType() const;

			/// High frequency deactivation is used to quickly deactivate entities that have come to a definite halt. 
		virtual bool shouldDeactivateHighFrequency( const hkEntity* entity );

			/// Low frequency deactivation checks less frequently if the object needs to be deactivated.
			/// This makes it less likely that the system will deactivate an object that might quickly need to move again.
		virtual bool shouldDeactivateLowFrequency( const hkEntity* entity );

			/// Returns the world hkFakeRigBodyDeactivator that is used by all entities that wish never to deactivate.
		inline static hkFakeRigidBodyDeactivator* getFakeRigidBodyDeactivator();

	private:

		static hkFakeRigidBodyDeactivator m_fakeRigidBodyDeactivator;

};

#include <hkdynamics2/entity/hkFakeRigidBodyDeactivator.inl>

#endif // HK_DYNAMICS2_FAKE_RIGIDBODY_DEACTIVATOR_H

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
