/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_UTILITIES2_SHAPE_DISPLAY_VIEWER_H
#define HK_UTILITIES2_SHAPE_DISPLAY_VIEWER_H

#include <hkbase/hkBase.h>
#include <hkmath2/hkMath.h>

#include <hkdynamics2/world/listener/hkWorldDeletionListener.h>
#include <hkdynamics2/world/listener/hkWorldPostSimulationListener.h>
#include <hkdynamics2/entity/hkEntityListener.h>

class hkDisplayHandler;
class hkWorld;

class hkCollidable;

	/// Displays all the entities in a world.
class hkShapeDisplayViewer :	public hkReferencedObject, protected hkWorldDeletionListener, 
								protected hkEntityListener, protected hkWorldPostSimulationListener
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS)

			/// Creates a hkShapeDisplayViewer.
		static hkReferencedObject* HK_CALL create(hkWorld* world, hkDisplayHandler* displayHandler);

			/// Registers the hkShapeDisplayViewer with the hkViewerFactory.
		static void HK_CALL registerViewer();

			/// Gets the tag associated with this viewer type
		static inline int HK_CALL getTag() { return m_tag; }

	protected:

		hkShapeDisplayViewer( hkWorld* world, hkDisplayHandler* displayHandler );
		~hkShapeDisplayViewer();

		virtual void worldDeletedCallback( hkWorld* world );

		virtual void entityAddedCallback( hkEntity* entity );
		virtual void entityRemovedCallback( hkEntity* entity );

		virtual void postSimulationCallback( hkWorld* world, const hkStepInfo& stepInfo );

	private:
		void removeAllGeometries();

	private:
		static int m_tag;

		hkWorld* m_world;
		hkDisplayHandler* m_displayHandler;

		hkArray<int> m_entitiesCreated;
};

#endif	// HK_UTILITIES2_SHAPE_DISPLAY_VIEWER_H


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
