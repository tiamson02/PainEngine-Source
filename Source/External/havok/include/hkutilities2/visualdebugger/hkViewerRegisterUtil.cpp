/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/viewer/hkShapeDisplayViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkBroadphaseViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkSimulationIslandViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkContactPointViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkDebugDisplayViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkRigidBodyInertiaViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkRigidBodyCentreOfMassViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkStatisticsViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkConstraintViewer.h>

#include <hkutilities2/visualdebugger/hkViewerRegisterUtil.h>

void HK_CALL hkViewerRegisterUtil::registerAllViewers()
{
	hkBroadphaseViewer::registerViewer();
	hkShapeDisplayViewer::registerViewer();
	hkDebugDisplayViewer::registerViewer();
	hkRigidBodyInertiaViewer::registerViewer();
	hkRigidBodyCentreOfMassViewer::registerViewer();
	hkSimulationIslandViewer::registerViewer();
	hkContactPointViewer::registerViewer();
	hkStatisticsViewer::registerViewer();
	hkConstraintViewer::registerViewer();
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
