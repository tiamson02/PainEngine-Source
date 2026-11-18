/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_UTILITIES2_CLIP_VELOCITY_LISTENER_H
#define HK_UTILITIES2_CLIP_VELOCITY_LISTENER_H

#include <hkbase/hkBase.h>
#include <hkdynamics2/collide/hkCollisionListener.h>
#include <hkdynamics2/world/listener/hkWorldDeletionListener.h>
#include <hkdynamics2/world/listener/hkWorldPostSimulationListener.h>

class hkWorld;

	/// This class is designed to be used in conjunction with the hkConvexSweepShape. If you have
	/// hkConvexSweepShape objects in your world, create an instance of this class. It uses
	/// the hkConvexSweepShape objects to clip the velocity of objects just before collisions, preventing
	/// the bullet through paper problem for fast moving objects.
class hkClipVelocityListener :	public hkReferencedObject, 
								public hkCollisionListener, 
								public hkWorldDeletionListener, 
								public hkWorldPostSimulationListener
{
	public:

			/// Create an instance of the class. This automatically adds the instance as a hkCollisionListener to the world,
			/// and as a hkWorldPostSimulationListener. These listeners are used to implement velocity clipping in conjunction with
			/// the hkConvexSweepShape objects (see class description).
		static hkClipVelocityListener* HK_CALL create( hkWorld* world, hkReal linearVelocity );


	public:
			// A callback fired during simulation
		virtual void contactPointAddedCallback(	hkContactPointAddedEvent& event);

			// A callback fired during simulation
		virtual void contactPointRemovedCallback( hkContactPointRemovedEvent& event );

			// A callback fired during simulation
		virtual void postSimulationCallback( hkWorld* world, const hkStepInfo& stepInfo );

			// A callback fired during simulation
		virtual void worldDeletedCallback( hkWorld* world );

	protected:

		hkClipVelocityListener( hkWorld* world, hkReal linearVelocity );
		~hkClipVelocityListener();
	
	protected:

		hkReal m_maxLinearVelocity;
		hkReal m_maxLinearVelocitySqr;

		hkWorld* m_world;
};

#endif // HK_UTILITIES2_CLIP_VELOCITY_LISTENER_H

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
