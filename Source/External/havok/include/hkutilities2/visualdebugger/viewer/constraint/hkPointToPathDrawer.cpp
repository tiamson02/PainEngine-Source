/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/constraint/bilateral/pointtopath/hkPointToPathConstraint.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkPointToPathDrawer.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkPrimitiveDrawer.h>
#include <hkdynamics2/entity/hkRigidBody.h>

void hkPointToPathDrawer::drawConstraint(hkConstraint* constraint, hkDisplayHandler* displayHandler, int tag)
{

	hkPointToPathConstraint* pToPConstraint = static_cast<hkPointToPathConstraint*>(constraint);

	m_primitiveDrawer.setDisplayHandler(displayHandler);

	hkTransform refLocalToWorld;
	hkTransform attLocalToWorld;

	updateCommonParameters(pToPConstraint, refLocalToWorld, attLocalToWorld);
	m_bodyBWPivot = refLocalToWorld.getTranslation();
	m_bodyAWPivot = pToPConstraint->m_pivotWS;	
	
	drawBodyFrames(tag);

	drawPivots(tag);

	// Draw the path
	if(pToPConstraint->m_path != HK_NULL)
	{
		hkTransform refConstraintToWorld;
		refConstraintToWorld.setMul(refLocalToWorld, pToPConstraint->getConstraintToLocalTransform(1));

		hkParametricCurve* curve = pToPConstraint->getPath(); 

		if(curve != HK_NULL)
		{
		
			hkArray<hkVector4> points;
			curve->getPointsToDraw(points);

			int size;
			size = points.getSize();
			hkVector4 p1;
			hkVector4 p2;
			hkVector4 p;

			for(int i = 1; i < size; i++)
			{
				p = points[i];
				p1.setTransformedPos(refConstraintToWorld, p);
				p = points[i - 1];
				p2.setTransformedPos(refConstraintToWorld, p);	
				displayHandler->displayLine(p1, p2, hkColor::rgbFromFloats(0.f, .25f, 1.f), tag);
			}
		}
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
