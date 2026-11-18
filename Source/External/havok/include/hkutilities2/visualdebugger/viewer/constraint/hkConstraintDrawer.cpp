/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/viewer/constraint/hkConstraintDrawer.h>
#include <hkdynamics2/entity/hkRigidBody.h>



////////////////////////////////////////////////////////////////

hkConstraintDrawer::hkConstraintDrawer()
{
	m_numSegments = 24;
	m_pivotSize = .5f;
	m_arcRadius = 3.f;
	m_lineLength = 5.f;
    m_arrowSize = 1.5f;
	m_coneSize = 2.f;
	m_planeSize = 2.f;
}

///////////////////////////////////////////////////////////////
		
void hkConstraintDrawer::drawPivots(int tag)
{
	// Draw pivot points
	m_primitiveDrawer.displayOrientedPoint(m_bodyAWPivot, m_RA,.25f, hkColor::RED, tag);
	m_primitiveDrawer.displayOrientedPoint(m_bodyBWPivot, m_RB,.25f, hkColor::WHITE, tag);
}

///////////////////////////////////////////////////////////////
		
void hkConstraintDrawer::drawBodyFrames(int tag)
{
	hkUint32 yellow,orange,purple;

	yellow = hkColor::rgbFromFloats(1.f,1.f,0.f);
	orange = hkColor::rgbFromFloats(1.f,.5f,0.f);
	purple = hkColor::rgbFromFloats(1.f,0.f,1.f);

	
	hkVector4 Ax = m_RA.getColumn(0);
	hkVector4 Ay = m_RA.getColumn(1);
	hkVector4 Az = m_RA.getColumn(2);

	hkVector4 Bx = m_RB.getColumn(0);
	hkVector4 By = m_RB.getColumn(1);
	hkVector4 Bz = m_RB.getColumn(2); 

	// Draw coordinate frames
	m_primitiveDrawer.displayArrow(m_bodyAWPos, Ax, Ay, hkColor::RED, m_arrowSize, tag);
	m_primitiveDrawer.displayArrow(m_bodyAWPos, Ay, Az, hkColor::GREEN, m_arrowSize, tag);
	m_primitiveDrawer.displayArrow(m_bodyAWPos, Az, Ax, hkColor::BLUE, m_arrowSize, tag);
	m_primitiveDrawer.displayArrow(m_bodyBWPos, Bx, By, yellow, m_arrowSize, tag);
	m_primitiveDrawer.displayArrow(m_bodyBWPos, By, Bz, orange, m_arrowSize, tag);
	m_primitiveDrawer.displayArrow(m_bodyBWPos, Bz, Bx, purple, m_arrowSize, tag);
}

///////////////////////////////////////////////////////////////

void hkConstraintDrawer::setNumSegments(int numSegments)
{
	m_numSegments = numSegments;
}

////////////////////////////////////////////////////////////////
		
void hkConstraintDrawer::setPivotSize(hkReal size)
{
	m_pivotSize = size;
}

/////////////////////////////////////////////////////////////////

void hkConstraintDrawer::setArcRadius(hkReal arcRadius)
{
	m_arcRadius = arcRadius;
}

/////////////////////////////////////////////////////////////////

void hkConstraintDrawer::setLineLength(hkReal lineLength)
{
    m_lineLength = lineLength;
}

/////////////////////////////////////////////////////////////////

void hkConstraintDrawer::updateCommonParameters(hkConstraint* constraint, hkTransform& refLocalToWorld, hkTransform& attLocalToWorld)
{
	// Get the rigid bodies from the constraint
	hkRigidBody *refBody = reinterpret_cast<hkRigidBody*>(constraint->getEntityB());
	hkRigidBody *attBody = reinterpret_cast<hkRigidBody*>(constraint->getEntityA());

	// Get their local to world transforms
	refLocalToWorld = refBody->getTransform();
	attLocalToWorld = attBody->getTransform();

	m_RB = refLocalToWorld.getRotation();
	m_RA = attLocalToWorld.getRotation();

	m_bodyBWPos = refLocalToWorld.getTranslation();
	m_bodyAWPos = attLocalToWorld.getTranslation();
}

/////////////////////////////////////////////////////////////////


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
