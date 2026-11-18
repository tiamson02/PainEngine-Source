/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_UTILITIES2_SURFACE_VELOCITY_UTIL
#define HK_UTILITIES2_SURFACE_VELOCITY_UTIL


#include <hkmath2/hkMath.h>
#include <hkdynamics2/collide/hkCollisionListener.h>

#include <hkdynamics2/entity/hkEntityListener.h>

class hkRigidBody;
class hkMovingSurfaceContactMgr;

	/// Adds a surface velocity to an entity.
	/// Note: If you want to set a surface velocity once and forever and you do not keep a pointer to
	/// the instance, do not forget to call removeReference
class hkSurfaceVelocityUtil: public hkReferencedObject, private hkCollisionListener, private hkEntityListener
{
	public:
			/// Creates a handle to a surface velocity
		hkSurfaceVelocityUtil( hkRigidBody* body, const hkVector4& surfaceVelocityWorld );

			/// Sets the surface velocity for the rigid body in world space
		inline void setSurfaceVelocity( const hkVector4& velWorld );

			/// Get the surface velocity for the rigid body in world space
		inline const hkVector4& getSurfaceVelocity() const;

			/// Get a non-const reference to the surface velocity for the rigid body in world space
		inline hkVector4& accessSurfaceVelocity();

	protected:


			// The hkCollisionListener interface implementation
		virtual void contactPointAddedCallback(	  hkContactPointAddedEvent& event);

			// The hkCollisionListener interface implementation
		virtual void contactPointRemovedCallback( hkContactPointRemovedEvent& event){}

			// The hkCollisionListener interface implementation
		virtual void contactProcessCallback( hkContactProcessEvent& event);

		virtual void entityDeletedCallback( hkEntity* entity );

	protected:

		hkRigidBody* m_rigidBody;
		hkVector4 m_surfaceVelocity;
};

#include <hkutilities2/collide/hkSurfaceVelocityUtil.inl>

#endif		// HK_UTILITIES2_SURFACE_VELOCITY_UTIL

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
