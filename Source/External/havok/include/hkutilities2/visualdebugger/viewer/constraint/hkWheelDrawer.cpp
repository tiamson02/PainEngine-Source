/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkutilities2/visualdebugger/viewer/constraint/hkWheelDrawer.h>
#include <hkdynamics2/constraint/bilateral/wheel/hkWheelConstraint.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkPrimitiveDrawer.h>
#include <hkdynamics2/entity/hkRigidBody.h>

///////////////////////////////////////////////////////////////////

void hkWheelDrawer::drawConstraint(hkConstraint* constraint, hkDisplayHandler* displayHandler, int tag)
{

	hkWheelConstraint* wheel = static_cast<hkWheelConstraint*>(constraint);

	m_primitiveDrawer.setDisplayHandler(displayHandler);

	{
		hkTransform refLocalToWorld;
		hkTransform attLocalToWorld;
		updateCommonParameters(wheel, refLocalToWorld, attLocalToWorld);
		m_bodyBWPivot.setTransformedPos(refLocalToWorld, wheel->m_pivotInB);
		m_bodyAWPivot.setTransformedPos(attLocalToWorld, wheel->m_pivotInA);
	}

	drawBodyFrames(tag);
	
	drawPivots(tag);

	// draws red line between pivot on A and B in world space
	displayHandler->displayLine(m_bodyAWPivot, m_bodyBWPivot, hkColor::RED, tag);


	hkVector4 suspensionAxis;
	suspensionAxis.setRotatedDir(m_RB, wheel->m_suspensionAxisInB); 

	// draw suspension axis
	{
		hkVector4 p1;
		hkVector4 p2;
		p1.setMul4(-.5f * m_lineLength, suspensionAxis);
		p1.add4(m_bodyBWPivot);
		p2.setMul4(.5f * m_lineLength, suspensionAxis);
		p2.add4(m_bodyBWPivot);
		displayHandler->displayLine(p1, p2, hkColor::GREEN, tag);
	}

	// draw suspension limits
	{
		hkVector4 p1;
		hkVector4 start;
		hkVector4 end;
		
		hkReal suspensionMin = wheel->m_suspensionMinLimit;
		hkReal suspensionMax = wheel->m_suspensionMaxLimit;
		
		hkVector4 perpSuspension;
		perpSuspension.setRotatedDir(m_RB, wheel->m_perpToSuspensionAxisInB1);

		p1.setMul4(suspensionMax, suspensionAxis);
		p1.add4(m_bodyBWPivot);

		start.setMul4(.5f * m_lineLength, perpSuspension);
		start.add4(p1);

		end.setMul4(-.5f * m_lineLength, perpSuspension);
		end.add4(p1);

		displayHandler->displayLine(start,end, hkColor::WHITE, tag);

		p1.setMul4(suspensionMin, suspensionAxis);
		p1.add4(m_bodyBWPivot);

		start.setMul4(.5f * m_lineLength, perpSuspension);
		start.add4(p1);

		end.setMul4(-.5f * m_lineLength, perpSuspension);
		end.add4(p1);

		displayHandler->displayLine(start, end, hkColor::WHITE, tag);
	}
	
	// draw steering axis
	{
		hkVector4 p1;
		hkVector4 p2;
		hkVector4 steeringAxis;
		steeringAxis.setRotatedDir(m_RB, wheel->m_steeringAxisInB);
		p1.setMul4(-.5f * m_lineLength, steeringAxis);
		p1.add4(m_bodyBWPivot);
		p2.setMul4(.5f * m_lineLength, steeringAxis);
		p2.add4(m_bodyBWPivot);
		displayHandler->displayLine(p1, p2, hkColor::YELLOW, tag);
	}



	// draw axle
	{
		hkVector4 p1;
		hkVector4 p2;
		hkVector4 axle;
		axle.setRotatedDir(m_RA, wheel->m_axleInA);

		p1.setMul4(-.5f * m_lineLength, axle);
		p1.add4(m_bodyBWPivot);

		p2.setMul4(.5f * m_lineLength, axle);
		p2.add4(m_bodyBWPivot);

		displayHandler->displayLine(p1, p2, hkColor::BLUE, tag);
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
