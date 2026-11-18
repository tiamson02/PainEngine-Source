/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/viewer/constraint/hkLimitedHingeDrawer.h>
#include <hkdynamics2/constraint/bilateral/limitedhinge/hkLimitedHingeConstraint.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkPrimitiveDrawer.h>
#include <hkdynamics2/entity/hkRigidBody.h>


////////////////////////////////////////////////////////////////////

void hkLimitedHingeDrawer::drawConstraint(hkConstraint* constraint, hkDisplayHandler* displayHandler, int tag)
{
	hkLimitedHingeConstraint* lhinge = static_cast<hkLimitedHingeConstraint*>(constraint);

	m_primitiveDrawer.setDisplayHandler(displayHandler);

	{
		hkTransform refLocalToWorld;
		hkTransform attLocalToWorld;
		updateCommonParameters(lhinge, refLocalToWorld, attLocalToWorld);
		m_bodyBWPivot.setTransformedPos(refLocalToWorld, lhinge->m_pivotInB);
		m_bodyAWPivot.setTransformedPos(attLocalToWorld, lhinge->m_pivotInA);
	}
	
	drawBodyFrames(tag);

	drawPivots(tag);

	hkVector4 axisInWorld;
	axisInWorld.setRotatedDir(m_RA, lhinge->m_axleInA);

	hkVector4 axisPerpInWorld;
	axisPerpInWorld.setRotatedDir(m_RA, lhinge->m_perpToAxleInA1);

	// draw a red error line between the pivots
	displayHandler->displayLine(m_bodyAWPivot, m_bodyBWPivot, hkColor::RED, tag);

	// draw the free DOF
	{
		hkVector4 startAxis,endAxis;
		endAxis.setMul4(.5f * m_lineLength, axisInWorld);
		endAxis.add4(m_bodyAWPivot);
		startAxis.setMul4(-.5f * m_lineLength, axisInWorld);
		startAxis.add4(m_bodyAWPivot);
		displayHandler->displayLine(startAxis, endAxis, hkColor::rgbFromFloats(0.f, .5f, 1.f), tag);
	}

	// draw the limits
	{
		hkReal thetaMax = lhinge->getMaxAngularLimit();
		hkReal thetaMin = lhinge->getMinAngularLimit();

		m_angularLimit.setParameters(1.5,-thetaMin,-thetaMax,24,m_bodyBWPivot,axisInWorld,axisPerpInWorld);
		hkArray<hkDisplayGeometry*> geometry;
		geometry.setSize(1);
		geometry[0] = &(m_angularLimit);
		displayHandler->displayGeometry(geometry,hkColor::WHITE, tag);
	}

	// draw a line representing m_axisPerpInWorld to which the angle
	// is with respect to.
	{
		hkVector4 start;
		hkVector4 end;
		start = m_bodyAWPivot;
		end = start;
		end.addMul4(0.5f * m_lineLength, axisPerpInWorld);
		displayHandler->displayLine(start, end, hkColor::YELLOW, tag);

		hkVector4 axisPerpB;
		axisPerpB.setRotatedDir(m_RB, lhinge->m_perp2FreeAxisInB); 
		axisPerpB.setCross(axisPerpB, axisInWorld);	
		axisPerpB.normalize3();

		start = m_bodyBWPivot;
		end = start;
		end.addMul4(0.5f * m_lineLength, axisPerpB);
		displayHandler->displayLine(start, end, hkColor::YELLOW, tag);
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
