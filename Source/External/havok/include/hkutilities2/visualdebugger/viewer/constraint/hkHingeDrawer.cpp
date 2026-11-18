/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/viewer/constraint/hkHingeDrawer.h>
#include <hkdynamics2/constraint/bilateral/hinge/hkHingeConstraint.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkPrimitiveDrawer.h>
#include <hkdynamics2/entity/hkRigidBody.h>


////////////////////////////////////////////////////////////////////

void hkHingeDrawer::drawConstraint(hkConstraint* constraint, hkDisplayHandler* displayHandler, int tag)
{

	hkHingeConstraint* hinge = static_cast<hkHingeConstraint*>(constraint);

	m_primitiveDrawer.setDisplayHandler(displayHandler);

	{
		hkTransform refLocalToWorld;
		hkTransform attLocalToWorld;
		updateCommonParameters(hinge, refLocalToWorld, attLocalToWorld);
		m_bodyBWPivot.setTransformedPos(refLocalToWorld, hinge->m_pivotInB);
		m_bodyAWPivot.setTransformedPos(attLocalToWorld, hinge->m_pivotInA);
	}

	drawBodyFrames(tag);

	drawPivots(tag);

	// not a complete representation of error
	// but some feedback
	// draw a red error line between the pivots
	displayHandler->displayLine(m_bodyAWPivot,m_bodyBWPivot,hkColor::RED, tag);

	// Draw the free axis of rotation
	{
		hkVector4 axis;
		axis.setCross(hinge->m_perpToAxleInA1, hinge->m_perpToAxleInA2);
		hkVector4 axisInWorld;
		axisInWorld.setRotatedDir(m_RA, axis);

		hkVector4 startAxis,endAxis;
		endAxis.setMul4(.5f * m_lineLength, axisInWorld);
		endAxis.add4(m_bodyAWPivot);

		startAxis.setMul4(-.5f * m_lineLength, axisInWorld);
		startAxis.add4(m_bodyAWPivot);
		displayHandler->displayLine(startAxis, endAxis, hkColor::rgbFromFloats(0.f,.5f,1.f), tag);
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
