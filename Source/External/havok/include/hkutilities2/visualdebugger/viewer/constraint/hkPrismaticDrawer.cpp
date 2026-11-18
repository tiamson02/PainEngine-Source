/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/viewer/constraint/hkPrismaticDrawer.h>
#include <hkdynamics2/constraint/bilateral/prismatic/hkPrismaticConstraint.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkPrimitiveDrawer.h>
#include <hkdynamics2/entity/hkRigidBody.h>

////////////////////////////////////////////////////////////////////

void hkPrismaticDrawer::drawConstraint(hkConstraint* constraint, hkDisplayHandler* displayHandler, int tag)
{
	hkPrismaticConstraint* prismatic = static_cast<hkPrismaticConstraint*>(constraint);

	m_primitiveDrawer.setDisplayHandler(displayHandler);

	{
		hkTransform refLocalToWorld;
		hkTransform attLocalToWorld;
		updateCommonParameters(prismatic, refLocalToWorld, attLocalToWorld);
		m_bodyBWPivot.setTransformedPos(refLocalToWorld, prismatic->m_pivotInB);
		m_bodyAWPivot.setTransformedPos(attLocalToWorld, prismatic->m_pivotInA);
	}

	drawPivots(tag);

	drawBodyFrames(tag);

	hkVector4 axisInWorld;
	axisInWorld.setRotatedDir(m_RB, prismatic->m_shaftInB);
	hkVector4 axisPerpInWorld;
	axisPerpInWorld.setRotatedDir(m_RB, prismatic->m_perpToShaftInB1);

	// not a full depiction of the error
	// draw a red error line between the pivots
	displayHandler->displayLine(m_bodyAWPivot, m_bodyBWPivot, hkColor::RED, tag);
	

	// draw the free DOF
	{
		hkVector4 startAxis,endAxis;
		if(prismatic->getMaxLinearLimit() != hkMath::HK_REAL_MAX)
		{
			endAxis.setMul4(prismatic->getMaxLinearLimit(), axisInWorld);
		    endAxis.add4(m_bodyBWPivot);
			startAxis.setMul4(prismatic->getMinLinearLimit(), axisInWorld);
			startAxis.add4(m_bodyBWPivot);
			displayHandler->displayLine(startAxis, endAxis, hkColor::rgbFromFloats(0.f, .5f, 1.f), tag);
		}
		else
		{
			endAxis.setMul4(m_lineLength, axisInWorld);
		    endAxis.add4(m_bodyBWPivot);
			startAxis.setMul4(-m_lineLength, axisInWorld);
			startAxis.add4(m_bodyBWPivot);
			displayHandler->displayLine(startAxis, endAxis, hkColor::rgbFromFloats(0.f, .5f, 1.f), tag);
		}
	}

	// draw limits in white
	{
		hkVector4 startLine;
		hkVector4 endLine;
		hkVector4 center;
		hkVector4 temp;

		hkReal minLimit = prismatic->getMinLinearLimit();
		hkReal maxLimit = prismatic->getMaxLinearLimit();

		temp.setMul4(minLimit, axisInWorld);
		center.setAdd4(m_bodyBWPivot,temp);
		temp.setMul4(.5f * m_lineLength, axisPerpInWorld);
		startLine.setAdd4(center,temp);
		temp.setMul4(-.5f * m_lineLength, axisPerpInWorld);
		endLine.setAdd4(center, temp);
		displayHandler->displayLine(startLine, endLine, hkColor::WHITE, tag);

		temp.setMul4(maxLimit, axisInWorld);
		center.setAdd4(m_bodyBWPivot,temp);
		temp.setMul4(.5f * m_lineLength, axisPerpInWorld);
		startLine.setAdd4(center,temp);
		temp.setMul4(-.5f * m_lineLength, axisPerpInWorld);
		endLine.setAdd4(center, temp);
		displayHandler->displayLine(startLine, endLine, hkColor::WHITE, tag);
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
