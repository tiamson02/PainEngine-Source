/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_UTILITIES2_SIMULATION_ISLAND_VIEWER_H
#define HK_UTILITIES2_SIMULATION_ISLAND_VIEWER_H

#include <hkbase/hkBase.h>
#include <hkmath2/hkMath.h>
#include <hkdynamics2/world/listener/hkWorldPostSimulationListener.h>
#include <hkdynamics2/world/listener/hkWorldDeletionListener.h>
#include <hkgeometry2/display/hkDisplayAABB.h>


class hkDisplayHandler;
class hkWorld;

	/// Displays the bounding boxes of all simulation islands in the world.  The the boxes will
	/// be blue for active simulation islands and green for inactive simulation islands.
class hkSimulationIslandViewer : public hkReferencedObject, public hkWorldPostSimulationListener, public hkWorldDeletionListener
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS)

			/// Creates a hkSimulationIslandViewer.
		static hkReferencedObject* HK_CALL create(hkWorld* world, hkDisplayHandler* displayHandler);

			/// Registers the hkSimulationIslandViewer with the hkViewerFactory.
		static void HK_CALL registerViewer();

			/// Gets the tag associated with this viewer type
		static inline int HK_CALL getTag() { return m_tag; }

			// Control the state of the simulation island viewer
		bool m_showActiveIslands;
		bool m_showInactiveIslands;

	protected:

		hkSimulationIslandViewer(hkWorld* world, hkDisplayHandler* displayHandler);
		~hkSimulationIslandViewer();

		virtual void postSimulationCallback(hkWorld* world, const hkStepInfo& stepInfo);
		virtual void worldDeletedCallback(hkWorld* world);

	private:
		static int m_tag;

		hkWorld* m_world;
		hkDisplayHandler* m_displayHandler;
		hkObjectArray<hkDisplayAABB> m_inactiveIslandDisplayGeometries;
		hkObjectArray<hkDisplayAABB> m_activeIslandDisplayGeometries;
};

#endif	// HK_UTILITIES2_SIMULATION_ISLAND_VIEWER_H
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
