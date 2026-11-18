/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_HALF_STEPPING_UTILITY
#define HK_HALF_STEPPING_UTILITY

#include <hkmath2/hkMath.h>
#include <hkdynamics2/world/listener/hkIslandActivationListener.h>
#include <hkdynamics2/world/listener/hkWorldDeletionListener.h>
#include <hkdynamics2/entity/hkEntityListener.h>


class hkWorld;
class hkRigidBody;

/// This class allows you to step the world at half the frequency of your framerate, by
/// performing collision detection and integration on alternate frames, and interpolating the
/// active entities on the in-between frames
class hkHalfSteppingUtility : public hkReferencedObject, protected hkIslandActivationListener, protected hkWorldDeletionListener, protected hkEntityListener
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_UTILITIES);

			/// Creates the utility. This automatically adds the utility as a listener to the world
		hkHalfSteppingUtility( hkWorld* world );

			/// Virtual destructor.	
		virtual ~hkHalfSteppingUtility();


			/// Synchronize the half steppers state of the rigid body with the actual state.
			/// This should be called after calls to hkRigidBody::setPosition, setRotation, 
			/// setPositionAndRotation or setTransform if using the half stepper.
		void syncRigidBody( hkRigidBody* rb );


			/// This needs to be called instead of hkWorld::stepDeltaTime()
		void stepDeltaTime( hkReal deltaTime );

			/// Implementation of hkWorldActivationListener function
		virtual void islandActivatedCallback( hkSimulationIsland* island );

			/// Implementation of hkWorldActivationListener function
		virtual void islandDeactivatedCallback( hkSimulationIsland* island );

			/// Implementation of hkWorldDeletionListener function
		virtual void worldDeletedCallback( hkWorld* world );

			/// Implementation of hkWorldDeletionListener function
		virtual void entityAddedCallback( hkEntity* entity );
		
			/// Implementation of hkWorldListener function
		virtual void entityRemovedCallback( hkEntity* entity );


	private:

		void interpolateHalf();
		void updateToStoredPositions();

		struct ActiveEntity
		{
			hkRigidBody* m_rigidBody;
			hkVector4 m_position;
			hkQuaternion m_rotation;
		};

		hkArray<ActiveEntity> m_activeEntities;

		bool m_integrate;
		hkWorld* m_world;
};

#endif // HK_HALF_STEPPING_UTILITY

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
