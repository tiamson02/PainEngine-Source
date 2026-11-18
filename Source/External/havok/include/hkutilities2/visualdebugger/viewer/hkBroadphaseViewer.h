/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_UTILITIES2_BROADPHASE_VIEWER_H
#define HK_UTILITIES2_BROADPHASE_VIEWER_H

#include <hkmath2/hkMath.h>
#include <hkdynamics2/world/listener/hkWorldPostSimulationListener.h>
#include <hkdynamics2/world/listener/hkWorldDeletionListener.h>
#include <hkgeometry2/display/hkDisplayAABB.h>
#include <hkbase/hkBase.h>

class hkDisplayHandler;
class hkWorld;

	/// Shows all the aabbs of the broadphase
class hkBroadphaseViewer : public hkReferencedObject, protected hkWorldPostSimulationListener, protected hkWorldDeletionListener
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS)

			/// Creates a hkBroadphaseViewer.
		static hkReferencedObject* HK_CALL create(hkWorld* world, hkDisplayHandler* displayHandler);

			/// Registers the hkBroadphaseViewer with the hkViewerFactory.
		static void HK_CALL registerViewer();

			/// Gets the tag associated with this viewer type
		static inline int HK_CALL getTag() { return m_tag; }

	protected:

		hkBroadphaseViewer(hkWorld* world, hkDisplayHandler* displayHandler);

		~hkBroadphaseViewer();

			/// Called at the end of the hkWorld::simulate call, Attenttion: this might change
		virtual void postSimulationCallback(hkWorld* world, const hkStepInfo& stepInfo);

			/// Called, when the world gets deleted
		virtual void worldDeletedCallback(hkWorld* world);

	private:
		static int m_tag;

		hkWorld* m_world;
		hkDisplayHandler* m_displayHandler;
		hkObjectArray<hkDisplayAABB> m_broadPhaseDisplayGeometries;
};

#endif	// HK_UTILITIES2_BROADPHASE_VIEWER_H

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
