/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_UTILITIES2_RIGIDBODY_INERTIA_VIEWER_H
#define HK_UTILITIES2_RIGIDBODY_INERTIA_VIEWER_H

#include <hkbase/hkBase.h>
#include <hkmath2/hkMath.h>

#include <hkdynamics2/world/listener/hkWorldDeletionListener.h>
#include <hkdynamics2/world/listener/hkWorldPostSimulationListener.h>
#include <hkdynamics2/entity/hkEntityListener.h>
#include <hkgeometry2/display/hkDisplayBox.h>
#include <hkbase/hkBase.h>


class hkDisplayHandler;
class hkWorld;
class hkRigidBody;

	/// Shows the inertia tensor of all rigid bodies added to the world.
	/// The Inertia Tensor is displayed as a box whose dimesions are such that it would have the same inertia
	/// tensor as the body in question, if it was also of equal mass. In other words, it shows an object which
	/// would behave identically from a dynamic point of view (though obviously, not from a colision detection
	/// point of view).
	/// In the case of a rigid body being simulated as an Oriented Particle this box will always be a cube aligned in local space.
	/// In the case of a rigid body being simulated with a DIAGONAL inertia tensor this box may have different edge
	/// lengths, but will still be aligned in local space.
	/// Currently, a FULL inertia tensor is not supported.
	/// N.B. Actually, we display a box which is *slightly bigger* than the one defined above by a factor of 1.01, just for
	/// ease of display.
class hkRigidBodyInertiaViewer :	public hkReferencedObject, protected hkWorldDeletionListener, 
								protected hkEntityListener, protected hkWorldPostSimulationListener
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS)

			/// Creates a hkRigidBodyInertiaViewer.
		static hkReferencedObject* HK_CALL create(hkWorld* world, hkDisplayHandler* displayHandler);

			/// Registers the hkRigidBodyInertiaViewer with the hkViewerFactory.
		static void HK_CALL registerViewer();

			/// Gets the tag associated with this viewer type
		static inline int HK_CALL getTag() { return m_tag; }

	protected:

		hkRigidBodyInertiaViewer( hkWorld* world, hkDisplayHandler* displayHandler );
		~hkRigidBodyInertiaViewer();

		virtual void worldDeletedCallback( hkWorld* world );

		virtual void entityAddedCallback( hkEntity* entity );
		virtual void entityRemovedCallback( hkEntity* entity );

		virtual void postSimulationCallback( hkWorld* world, const hkStepInfo& stepInfo );

	private:
		static int m_tag;

		hkWorld* m_world;
		hkDisplayHandler* m_displayHandler;

		hkArray<hkRigidBody*> m_entitiesCreated;
		hkObjectArray<hkDisplayBox> m_displayBoxes;
};

#endif	// HK_UTILITIES2_RIGIDBODY_INERTIA_VIEWER_H


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
