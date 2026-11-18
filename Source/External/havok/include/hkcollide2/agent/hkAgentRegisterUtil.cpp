/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkcollide2/hkCollide2.h>
#include <hkcollide2/agent/hkAgentRegisterUtil.h>


#include <hkcollide2/agent/hkConvexConvexAgent.h>
#include <hkcollide2/agent/hkBoxBoxAgent.h>
#include <hkcollide2/agent/hkBvShapeAgent.h>
#include <hkcollide2/agent/hkBvTreeShapeAgent.h>
#include <hkcollide2/agent/hkPhantomAgent.h>
#include <hkcollide2/agent/hkSphereSphereAgent.h>
#include <hkcollide2/agent/hkSphereTriangleAgent.h>
#include <hkcollide2/agent/hkSphereBoxAgent.h>
#include <hkcollide2/agent/hkMultiSphereTriangleAgent.h>
#include <hkcollide2/agent/hkMultiRayConvexAgent.h>
#include <hkcollide2/agent/hkAabbCastAabbCastAgent.h>

#include <hkcollide2/agent/hkTransformAgent.h>
#include <hkcollide2/agent/hkListagent.h>
#include <hkcollide2/agent/hkShapeCollectionAgent.h>
#include <hkcollide2/agent/hkAabbCastConvexAgent.h>
#include <hkcollide2/agent/hkConvexWelderConvexAgent.h>
#include <hkcollide2/agent/hkConvexWelderSphereAgent.h>


// Register agents
void HK_CALL hkAgentRegisterUtil::registerAllAgents(hkCollisionDispatcher* dis)
{

	//
	//	Warning: order is important, entries at the bottom have higher priority
	//


	//
	//	Unary agents 
	//
	hkBvTreeShapeAgent::registerAgent(dis);
	hkBvShapeAgent::registerAgent(dis);
	hkListAgent::registerAgent(dis);
	hkShapeCollectionAgent::registerAgent(dis);
	
	hkPhantomAgent::registerAgent(dis);

	// should have highest priority
	hkTransformAgent::registerAgent(dis);	

	//
	//	Terminal agents: secondary type
	//

	hkConvexConvexAgent::registerAgent(dis);

	//
	//	Special agents: primary type
	//
	hkBoxBoxAgent::registerAgent(dis);
	hkSphereSphereAgent::registerAgent(dis);
	hkSphereTriangleAgent::registerAgent(dis);
	hkSphereBoxAgent::registerAgent(dis);
	hkMultiSphereTriangleAgent::registerAgent(dis);
	hkMultiRayConvexAgent::registerAgent(dis);
	hkAabbCastConvexAgent::registerAgent(dis);
	hkAabbCastAabbCastAgent::registerAgent(dis);

	//
	// Convex "welder" agents - used for removing internal collisions in landscapes
	//
	hkConvexWelderConvexAgent::registerAgent(dis);
	hkConvexWelderSphereAgent::registerAgent(dis);
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
