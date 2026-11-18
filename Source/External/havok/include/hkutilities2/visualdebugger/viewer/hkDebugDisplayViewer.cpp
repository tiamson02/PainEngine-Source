/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/hkDebugDisplay.h>
#include <hkutilities2/visualdebugger/viewer/hkDebugDisplayViewer.h>
#include <hkutilities2/visualdebugger/hkDisplayHandler.h>
#include <hkdynamics2/world/hkWorld.h>
#include <hkutilities2/visualdebugger/hkViewerFactory.h>

#include <hkbase/stream/hkInplaceStringStreambuf.h>

int hkDebugDisplayViewer::m_tag = 0;

hkDebugDisplayViewer::hkDebugDisplayViewer( hkWorld* world, hkDisplayHandler* displayHandler )
	: m_world(world), m_displayHandler(displayHandler)
{
	// also must register here just in case the user creates the debug display viewer without using the factories
	// we must have a valid tag for the debug display output!
	m_tag = hkViewerFactory::getInstance().registerViewer( "DebugDisplay", create );

	m_displayHandler->addReference();
	world->addWorldDeletionListener(this);

	hkDebugDisplay::getInstance().addDebugDisplayViewer(this);
}

void HK_CALL hkDebugDisplayViewer::registerViewer()
{
	m_tag = hkViewerFactory::getInstance().registerViewer( "DebugDisplay", create );
}


	/// Called, when the world gets deleted
void hkDebugDisplayViewer::worldDeletedCallback( hkWorld* world )
{
	m_world->removeWorldDeletionListener( this );
	m_world = HK_NULL;
	this->removeReference();
}

hkDebugDisplayViewer::~hkDebugDisplayViewer()
{
	if(m_world)
	{
		m_world->removeWorldDeletionListener( this );
	}

	hkDebugDisplay::getInstance().removeDebugDisplayViewer(this);
	m_displayHandler->removeReference();
}


hkReferencedObject* HK_CALL hkDebugDisplayViewer::create(hkWorld* world, hkDisplayHandler* displayHandler)
{
	return new hkDebugDisplayViewer(world, displayHandler);
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
