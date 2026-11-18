/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/viewer/constraint/hkRagdollDrawer.h>
#include <hkdynamics2/constraint/bilateral/ragdoll/hkRagdollConstraint.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkPrimitiveDrawer.h>
#include <hkdynamics2/entity/hkRigidBody.h>


////////////////////////////////////////////////////////////////////

void hkRagdollDrawer::drawConstraint(hkConstraint* constraint, hkDisplayHandler* displayHandler, int tag)
{

	hkRagdollConstraint* ragdoll = static_cast<hkRagdollConstraint*>(constraint);

	m_primitiveDrawer.setDisplayHandler(displayHandler);

	{
		hkTransform refLocalToWorld;
		hkTransform attLocalToWorld;
		updateCommonParameters(ragdoll, refLocalToWorld, attLocalToWorld);
		m_bodyBWPivot.setTransformedPos(refLocalToWorld, ragdoll->m_pivotInB);
		m_bodyAWPivot.setTransformedPos(attLocalToWorld, ragdoll->m_pivotInA);
	}

	drawPivots(tag);

	// Get parameters from ragdoll constraint
	hkVector4 refTwistAxisWorld;
	refTwistAxisWorld.setRotatedDir(m_RB, ragdoll->m_twistAxisInB);

	hkVector4 refCrossedWorld;
	refCrossedWorld.setCross( ragdoll->m_twistAxisInB, ragdoll->m_planeAxisInB);
	refCrossedWorld.normalize3();
	refCrossedWorld.setRotatedDir(m_RB, refCrossedWorld);

	hkVector4 refPlaneNormalWorld;
	refPlaneNormalWorld.setRotatedDir(m_RB, ragdoll->m_planeAxisInB);

	hkVector4 cross;
	cross.setCross(ragdoll->m_planeAxisInA, ragdoll->m_twistAxisInA);
	cross.normalize3();
	hkVector4 attCrossedWorld;
	attCrossedWorld.setRotatedDir(m_RA, cross);

	hkVector4 attTwistAxisWorld;
	attTwistAxisWorld.setRotatedDir(m_RA, ragdoll->m_twistAxisInA);
	
	/////////////////////////////////////////////

	//always display twist axes
	m_primitiveDrawer.displayArrow(m_bodyBWPivot, refTwistAxisWorld, refCrossedWorld, hkColor::GREEN,m_arrowSize, tag);
	m_primitiveDrawer.displayArrow(m_bodyAWPivot, attTwistAxisWorld, attCrossedWorld, hkColor::rgbFromChars(255, 255, 0), m_arrowSize, tag);

	// draw twist cone
	m_twistCone.setParameters( ragdoll->getMinConeAngularLimit(), 3, 24, refTwistAxisWorld, m_bodyBWPivot);
	
	// draw plane	
	m_plane.setParameters(refPlaneNormalWorld, refCrossedWorld, m_bodyBWPivot, 1.5f);



	// draw plane cones
	{
		hkReal planeMaxLimit = hkMath::asin(ragdoll->m_planeMaxAngle);
		hkReal planeMinLimit = hkMath::asin(ragdoll->m_planeMinAngle);

		// These limits are negative for "lower cone", positive for upper,
		// but are "negative" cones (ie. areas in which the constrained bject/axis
		// should not be, so we have to draw cones of internal angle PI/2 - limit.

		m_planeCone1.setParameters((hkMath::HK_REAL_PI/2.f - hkMath::fabs(planeMaxLimit)), 3, 24, refPlaneNormalWorld, m_bodyBWPivot);

		refPlaneNormalWorld.setNeg4(refPlaneNormalWorld);	// Flip axis of cone to draw min limit

		m_planeCone2.setParameters((hkMath::HK_REAL_PI/2.f - hkMath::fabs(planeMinLimit)), 3, 24, refPlaneNormalWorld, m_bodyBWPivot);
	}


	// Pass geometries off to display handler
	{
		hkArray<hkDisplayGeometry*> twist;
		hkArray<hkDisplayGeometry*> planeCones;
		hkArray<hkDisplayGeometry*> plane;
	    twist.setSize(1);
		plane.setSize(1);
		planeCones.setSize(2);
		twist[0] = &m_twistCone;
		planeCones[0] = &m_planeCone1;
		planeCones[1] =  &m_planeCone2;
		plane[0] = &m_plane;
		displayHandler->displayGeometry(twist, hkColor::YELLOW, tag);
		displayHandler->displayGeometry(planeCones, hkColor::RED, tag);
		displayHandler->displayGeometry(plane, hkColor::rgbFromChars(255, 0, 255), tag);
	}
}

////////////////////////////////////////////////////////////////////

























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
