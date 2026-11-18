/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/viewer/hkBroadphaseViewer.h>
#include <hkdynamics2/world/hkWorld.h>
#include <hkcollide2/broadphase/hkBroadPhase.h>

//#define HK_DISABLE_DEBUG_DISPLAY
#include <hkutilities2/visualdebugger/hkDebugDisplay.h>
#include <hkutilities2/visualdebugger/viewer/hkCollideDebugUtil.h>
#include <hkutilities2/visualdebugger/hkViewerFactory.h>

int hkBroadphaseViewer::m_tag = 0;

hkBroadphaseViewer::hkBroadphaseViewer(hkWorld* world, hkDisplayHandler* displayHandler)
	: m_world(world), m_displayHandler(displayHandler)
{
	m_displayHandler->addReference();
	world->addWorldDeletionListener( this );
	world->addWorldPostSimulationListener( this );
}

void HK_CALL hkBroadphaseViewer::registerViewer()
{
	m_tag = hkViewerFactory::getInstance().registerViewer( "Broadphase", create );
}


void hkBroadphaseViewer::postSimulationCallback( hkWorld* world, const hkStepInfo& stepInfo )
{
	hkVector4 dummy(1,2,3,4);	// MSVC6.0 alignment doesn't seem to work for the array below, so this
								// useless variable was necessary...
	hkInplaceArrayAligned16<hkAabb, 1024> allAabbs;
	hkBroadPhase* broadPhase = world->getBroadPhase();

	broadPhase->getAllAabbs( allAabbs );
	if(allAabbs.getSize() > m_broadPhaseDisplayGeometries.getSize())
	{
		m_broadPhaseDisplayGeometries.setSize(allAabbs.getSize());
	}

	hkArray<hkDisplayGeometry*> displayGeometries;
	displayGeometries.setSize(allAabbs.getSize());

	// create display geometries 
	for(int i = allAabbs.getSize()-1; i >= 0; i--)
	{
		m_broadPhaseDisplayGeometries[i].setExtents(allAabbs[i].m_min, allAabbs[i].m_max);
		displayGeometries[i] = &(m_broadPhaseDisplayGeometries[i]);
	}

	m_displayHandler->displayGeometry(displayGeometries, hkColor::RED, m_tag);
}

	/// Called, when the world gets deleted
void hkBroadphaseViewer::worldDeletedCallback( hkWorld* world )
{
	m_world->removeWorldDeletionListener( this );
	m_world->removeWorldPostSimulationListener( this );
	m_world = HK_NULL;
	this->removeReference();
}

hkBroadphaseViewer::~hkBroadphaseViewer()
{
	if(m_world)
	{
		m_world->removeWorldDeletionListener( this );
		m_world->removeWorldPostSimulationListener( this );
	}
	m_displayHandler->removeReference();
}


hkReferencedObject* HK_CALL hkBroadphaseViewer::create(hkWorld* world, hkDisplayHandler* displayHandler)
{
	return new hkBroadphaseViewer(world, displayHandler);
}



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
