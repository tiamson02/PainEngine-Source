/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics/constraint/bilateral/pointtoplane/hkPointToPlaneConstraintData.h>
#include <hkutilities/visualdebugger/viewer/constraint/hkPointToPlaneDrawer.h>
#include <hkutilities/visualdebugger/viewer/constraint/hkPrimitiveDrawer.h>
#include <hkdynamics/entity/hkRigidBody.h>
#include <hkvisualize/type/hkColor.h>
#include <hkvisualize/hkDebugDisplayHandler.h>

#include <hkmath/linear/hkVector4Util.h>


////////////////////////////////////////////////////////////////////

void hkPointToPlaneDrawer::drawConstraint(hkConstraintInstance* constraint, hkDebugDisplayHandler* displayHandler, int tag)
{

	hkPointToPlaneConstraintData* plane = static_cast<hkPointToPlaneConstraintData*>(constraint->getData());

	m_primitiveDrawer.setDisplayHandler(displayHandler);

	{
		hkTransform refLocalToWorld;
		hkTransform attLocalToWorld;
		updateCommonParameters(constraint, refLocalToWorld, attLocalToWorld);
		m_bodyBWPivot.setTransformedPos(refLocalToWorld, plane->m_pivotInB);
		m_bodyAWPivot.setTransformedPos(attLocalToWorld, plane->m_pivotInA);
	}

	drawPivots(tag);

	drawBodyFrames(tag);

		// illustrate the constraint plane attached to body A
	    // by drawing a set of lines in the plane
	{
		hkVector4 perpVec0;
		hkVector4Util::calculatePerpendicularVector(plane->m_planeNormalA, perpVec0);
		hkVector4 perpVec1; perpVec1.setCross(plane->m_planeNormalA, perpVec0);

		hkVector4 lines[6][2];
		for (int coord = 0; coord < 2; coord++)
		{
			for (int i = -1; i <= 1; i++)
			{
				hkVector4 v0 = coord ? perpVec0 : perpVec1;
				hkVector4 v1 = coord ? perpVec1 : perpVec0;
				lines[coord*3 + i + 1][0] = m_bodyAWPivot;
				lines[coord*3 + i + 1][1] = m_bodyAWPivot;
				lines[coord*3 + i + 1][0].addMul4( 2.0f, v0);
				lines[coord*3 + i + 1][1].addMul4(-2.0f, v0);
				lines[coord*3 + i + 1][0].addMul4(hkReal(i) * 1.4f, v1);
				lines[coord*3 + i + 1][1].addMul4(hkReal(i) * 1.4f, v1);
			}
		}

		for (int i = 0; i < 6; i++)
		{
			displayHandler->displayLine(lines[i][0],lines[i][1],hkColor::rgbFromFloats(0.8f,.8f,.8f), tag);
		}
	}

		// draw the pivot arm for body B
	displayHandler->displayLine(m_bodyBWPivot,m_bodyBWPos,hkColor::rgbFromFloats(0.3f,.3f,.8f), tag);
}

////////////////////////////////////////////////////////////////////

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
