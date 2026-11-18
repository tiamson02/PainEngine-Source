/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/constraint/bilateral/stiffspring/hkStiffSpringConstraint.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkStiffSpringDrawer.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkPrimitiveDrawer.h>
#include <hkdynamics2/entity/hkRigidBody.h>


////////////////////////////////////////////////////////////////////

void hkStiffSpringDrawer::drawConstraint(hkConstraint* constraint, hkDisplayHandler* displayHandler, int tag)
{

	hkStiffSpringConstraint* spring = static_cast<hkStiffSpringConstraint*>(constraint);

	m_primitiveDrawer.setDisplayHandler(displayHandler);

	{
		hkTransform refLocalToWorld;
		hkTransform attLocalToWorld;
		updateCommonParameters(spring, refLocalToWorld, attLocalToWorld);
		m_bodyBWPivot.setTransformedPos(refLocalToWorld, spring->m_pivotInB);
		m_bodyAWPivot.setTransformedPos(attLocalToWorld, spring->m_pivotInA);
	}

	drawPivots(tag);

	drawBodyFrames(tag);

	// draw the error
	// first draw the spring segment at its proper length in lblue
	// then draw error..if any in red
	{
		hkReal springLength = spring->getSpringLength();
		
		hkVector4 dir;
		dir.setSub4(m_bodyBWPivot,m_bodyAWPivot);
		dir.normalize3();
		dir.setMul4(springLength,dir);
		dir.add4(m_bodyAWPivot);
		displayHandler->displayLine(m_bodyAWPivot,dir,hkColor::rgbFromFloats(0,.5f,1), tag);
		displayHandler->displayLine(dir,m_bodyBWPivot,hkColor::RED, tag);
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
