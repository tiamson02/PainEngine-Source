/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkbase/stopwatch/hkStopwatch.h>

#include <hkutilities2/visualdebugger/viewer/hkStatisticsViewer.h>
#include <hkutilities2/visualdebugger/hkDisplayHandler.h>
#include <hkutilities2/visualdebugger/hkDisplayCallbackUtil.h>
#include <hkdynamics2/world/hkWorld.h>

#include <hkutilities2/visualdebugger/hkViewerFactory.h>

#include <hkbase/debugutil/hkMonitorCaptureStreamUtility.h>

int hkStatisticsViewer::m_tag = 0;

hkStatisticsViewer::hkStatisticsViewer( hkWorld* world, hkDisplayHandler* displayHandler )
:	m_world(world), 
	m_displayHandler(displayHandler)
{
	m_mc = new hkMonitorCaptureStreamUtility(1000000);

	// utility which expects it's numbers to be in seconds.
	// The factors have been set accordingly here.
#ifdef HK_PLATFORM_PS2
	m_mc->m_timerFactor0 = 1.0f / 300e6f;
	m_mc->m_timerFactor1 = 1.0f / 300e6f;
#else
	m_mc->m_timerFactor0 = 1.0f / (hkReal)hkStopwatch::getTicksPerSecond();
	m_mc->m_timerFactor1 = 0.0f;
#endif

	m_displayHandler->addReference();
	world->addWorldDeletionListener(this);
	world->addWorldPostSimulationListener( this );
	HK_ADD_STEP_DISPLAY_LISTENER( this );
}

void HK_CALL hkStatisticsViewer::registerViewer()
{
	m_tag = hkViewerFactory::getInstance().registerViewer( "Statistics", create );
}


void hkStatisticsViewer::postSimulationCallback( hkWorld* world, const hkStepInfo& stepInfo )
{

}

void hkStatisticsViewer::stepDisplayCallback(hkReal frameTimeInMs)
{
	// grab the data from the monitors
	m_mc->captureFrame();

	// sent it over the connection
	m_displayHandler->sendStatisticsDump(m_mc->getData(), m_mc->getDataSize());

	// clear the buffer for the next frame
	m_mc->reset();
}

void hkStatisticsViewer::worldDeletedCallback( hkWorld* world )
{
	m_world->removeWorldPostSimulationListener( this );
	m_world->removeWorldDeletionListener( this );
	m_world = HK_NULL;
	this->removeReference();
}


hkStatisticsViewer::~hkStatisticsViewer()
{
	HK_REMOVE_STEP_DISPLAY_LISTENER( this );

	if(m_world)
	{
		m_world->removeWorldPostSimulationListener( this );
		m_world->removeWorldDeletionListener( this );
	}
	m_displayHandler->removeReference();

	delete m_mc;
}


hkReferencedObject* HK_CALL hkStatisticsViewer::create(hkWorld* world, hkDisplayHandler* displayHandler)
{
	return new hkStatisticsViewer(world, displayHandler);
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
