/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_HAVOK2_COMMON_H
#define HK_HAVOK2_COMMON_H

//proposal include files

#include <hkdynamics2/world/hkWorld.h>
#include <hkdynamics2/world/hkWorldConstructionInfo.h>
#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkdynamics2/entity/hkRigidBodyConstructionInfo.h>

#include <hkdynamics2/constraint/bilateral/ballandsocket/hkBallAndSocketConstraint.h>
#include <hkdynamics2/constraint/bilateral/hinge/hkHingeConstraint.h>
#include <hkdynamics2/constraint/bilateral/limitedhinge/hkLimitedHingeConstraint.h>
#include <hkdynamics2/constraint/bilateral/prismatic/hkPrismaticConstraint.h>
#include <hkdynamics2/constraint/bilateral/ragdoll/hkRagdollConstraint.h>
#include <hkdynamics2/constraint/bilateral/stiffspring/hkStiffSpringConstraint.h>
#include <hkdynamics2/constraint/bilateral/wheel/hkWheelConstraint.h>

#include <hkdynamics2/collide/hkGroupFilter.h>
#include <hkdynamics2/collide/hkGroupFilterSetup.h>


#include <hkcollide2/shape/hkBoxShape.h>
#include <hkcollide2/shape/hkSphereShape.h>
#include <hkcollide2/shape/hkTriangleShape.h>
#include <hkcollide2/shape/hkConvexVerticesShape.h>
//#include <hkcollide2/shape/hkMeshShape.h>
//#include <hkcollide2/shape/hkStorageMeshShape.h>
#include <hkcollide2/shape/hkMoppBvTreeShape.h>
#include <hkcollide2/shape/hkTransformShape.h>
#include <hkcollide2/shape/hkListShape.h>
#include <hkcollide2/shape/hkPhantomShape.h>

//#include <hkcollide2/algorithms/mopp/utility/hkMoppUtility.h>
#include <hkcollide2/agent/hkAgentRegisterUtil.h>
//#include <hkcollide2/contact/hkCollisionResults.h>

#include <hkutilities2/inertia/hkInertiaTensorComputer.h>
//#include <hkutilities2/collide/hkMoppCodeStreamer.h>

#endif //HK_HAVOK2_COMMON_H

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
