/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_UTILITIES2_VISCOSE_SURFACE_UTIL
#define HK_UTILITIES2_VISCOSE_SURFACE_UTIL


#include <hkmath2/hkMath.h>
#include <hkdynamics2/collide/hkCollisionListener.h>

#include <hkdynamics2/entity/hkEntity.h>
#include <hkdynamics2/entity/hkEntityListener.h>


class hkRigidBody;

	/// Makes a surface a little slippery (like thick oil or tar).
	/// It allows for removing objects which got stuck in a narrow gap.
	/// Typical applications include cars getting stuck between two buildings
	/// Note: you have to call makeSurfaceViscose before you add the entity to the world
	/// Note: this class only works if the default hkSimpleConstraintContactMgr is used
class hkViscoseSurfaceUtil: public hkReferencedObject, private hkCollisionListener, private hkEntityListener
{
	public:
			/// Creates a handle to a surface velocity
		static inline void makeSurfaceViscose(hkRigidBody* entity);	
		
	protected:
		hkViscoseSurfaceUtil( hkRigidBody* entity );

			// The hkCollisionListener interface implementation
		virtual void contactPointAddedCallback(	  hkContactPointAddedEvent& event){}

			// The hkCollisionListener interface implementation
		virtual void contactPointRemovedCallback( hkContactPointRemovedEvent& event){}

			// The hkCollisionListener interface implementation
		virtual void contactProcessCallback( hkContactProcessEvent& event);

		virtual void entityDeletedCallback( hkEntity* entity );

		//
		// For internal use
		//	
	public:
		hkEntity*   m_entity;
};

void hkViscoseSurfaceUtil::makeSurfaceViscose(hkRigidBody* entity)
{
	new hkViscoseSurfaceUtil( entity );
}

#endif		// HK_UTILITIES2_VISCOSE_SURFACE_UTIL

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
