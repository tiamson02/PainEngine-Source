/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/viewer/constraint/hkPrimitiveDrawer.h>

////////////////////////////////////////////////////////////////

hkPrimitiveDrawer::hkPrimitiveDrawer()
{
	m_displayHandler = HK_NULL;
}

////////////////////////////////////////////////////////////////

void hkPrimitiveDrawer::setDisplayHandler(hkDisplayHandler* displayHandler)
{
	m_displayHandler = displayHandler;
}

////////////////////////////////////////////////////////////////

void hkPrimitiveDrawer::displayArrow(const hkVector4& startPos, const hkVector4& arrowDirection, const hkVector4& perpDirection, int color, hkReal scale, int tag)
{
	hkVector4 endPos = startPos;
	endPos.addMul4(scale, arrowDirection);
	m_displayHandler->displayLine(startPos, endPos, color, tag);
	hkQuaternion FortyFiveAboutPerpDirection(perpDirection, hkMath::HK_REAL_PI/4.f);
	hkQuaternion MinusNinetyAboutPerpDirection(perpDirection, -1.f*hkMath::HK_REAL_PI/2.f);

	hkVector4 headDirection = arrowDirection;
	headDirection.setRotatedDir(FortyFiveAboutPerpDirection, headDirection);
	hkVector4 temp = endPos;
	temp.addMul4(-scale * 0.333f, headDirection);
	m_displayHandler->displayLine(endPos, temp, color, tag);

	headDirection.setRotatedDir(MinusNinetyAboutPerpDirection, headDirection);
	temp = endPos;
	temp.addMul4(-scale * 0.333f, headDirection);
	m_displayHandler->displayLine(endPos, temp, color, tag);
}

/////////////////////////////////////////////////////////////////
/// center is the position of the center in WS
/// normal is the axis about which the circle is drawn
/// startPerp is an orthogonal axis to the normal
/// which defines the start of the sweep

void hkPrimitiveDrawer::drawSemiCircle(const hkVector4& center, hkVector4& normal,
								hkVector4& startPerp,hkReal thetaMin, hkReal thetaMax,
								hkReal radius, int numSegments, int color, int tag)
{
	hkReal thetaIncr = (thetaMax-thetaMin) / numSegments;
	hkVector4 startPos;
	startPerp.normalize3();
	normal.normalize3();

	hkRotation rot;
	rot.setAxisAngle(normal, thetaMin);

		// Initialise start position
	startPos.setMul4(radius, startPerp);	
	startPos.setRotatedDir(rot, startPos);
	startPos.add4(center);
	
	rot.setAxisAngle(normal,thetaIncr);
	
		// Sucessively rotate position to next position (there will be a small amount of numerical drift here)
	for (int i = 0; i < numSegments; i++)
	{
		hkVector4 next;
		next = startPos;
		next.sub4(center);
		next.setRotatedDir(rot, next); 
		next.add4(center);
		m_displayHandler->displayLine(startPos, next, color, tag);

		startPos = next;
	}
}

///////////////////////////////////////////////////////////////////////////////

void hkPrimitiveDrawer::displayOrientedPoint(const hkVector4& position,const hkRotation& rot,
											  hkReal size, int color, int tag)
{
	hkVector4 p1,p2,x,y,z,scaled;

	x = rot.getColumn(0);
	y = rot.getColumn(1);
	z = rot.getColumn(2);

	scaled.setMul4(size,x);
	p1.setSub4(position,scaled);
	scaled.setMul4(-size,x);
	p2.setSub4(position,scaled);
	m_displayHandler->displayLine(p1, p2, color, tag);
	scaled.setMul4(size,y);
	p1.setSub4(position,scaled);
	scaled.setMul4(-size,y);
	p2.setSub4(position,scaled);
	m_displayHandler->displayLine(p1, p2, color, tag);
	scaled.setMul4(size,z);
	p1.setSub4(position,scaled);
	scaled.setMul4(-size,z);
	p2.setSub4(position,scaled);
	m_displayHandler->displayLine(p1, p2, color, tag);
}

////////////////////////////////////////////////////////////////////////////////

void hkPrimitiveDrawer::displayCone(hkReal coneAngle, const hkVector4& startPos, 
							 const hkVector4& coneAxis, const hkVector4& perpVector, 
							 int numSegments, int color, hkReal coneSize, int tag)
{
	hkVector4* cone_points = hkAllocate<hkVector4>(numSegments, HK_MEMORY_CLASS_TOOLS);
	hkReal cos_angle = hkMath::cos(coneAngle);
	hkReal segment_length = coneSize/coneAngle;
	hkQuaternion RotationFromNormal(perpVector, coneAngle);
	hkQuaternion RotationAboutAxis (coneAxis, (hkMath::HK_REAL_PI*2)/numSegments);

	hkVector4 offset_direction;
	offset_direction.setRotatedDir(RotationFromNormal, coneAxis);

	int i;
	for (i = 0; i < numSegments; i++)
	{
		cone_points[i] = startPos;
		cone_points[i].addMul4(segment_length, offset_direction);
		offset_direction.setRotatedDir(RotationAboutAxis, offset_direction);
	}

	for (i = 0; i < numSegments; i++)
	{
		int next_point_index = (i+1)%numSegments;
		m_displayHandler->displayLine(startPos, cone_points[i],color, tag);
		m_displayHandler->displayLine(cone_points[i], cone_points[next_point_index], color, tag);
	}

	hkDeallocate<hkVector4>(cone_points);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void hkPrimitiveDrawer::displayPlane(const hkVector4& startPos, const hkVector4& planeNormal, 
									 const hkVector4& vectorOnPlane, int color, hkReal scale, int tag)
{
	hkVector4 plane_points[4];
	hkReal invRoot2 = 0.70711f; 
	hkVector4 newVectorOnPlane;
	newVectorOnPlane.setMul4(scale * invRoot2, vectorOnPlane);
	hkQuaternion NinetyAboutPlaneNormal (planeNormal, hkMath::HK_REAL_PI/2.f);
	hkQuaternion FortyFiveAboutPlaneNormal (planeNormal,  hkMath::HK_REAL_PI/4.f);// this is only in here so that the 
	//display looks more like a square than a diamond
	newVectorOnPlane.setRotatedDir(FortyFiveAboutPlaneNormal, newVectorOnPlane);

	for (int i = 0; i < 4; i++)
	{
		plane_points[i] = startPos;
		plane_points[i].add4( newVectorOnPlane );
		newVectorOnPlane.setRotatedDir( NinetyAboutPlaneNormal, newVectorOnPlane);
	}
	
	m_displayHandler->displayLine(plane_points[0], plane_points[1], color, tag);
	m_displayHandler->displayLine(plane_points[1], plane_points[2], color, tag);
	m_displayHandler->displayLine(plane_points[2], plane_points[3], color, tag);
	m_displayHandler->displayLine(plane_points[3], plane_points[0], color, tag);
	m_displayHandler->displayLine(plane_points[0], plane_points[2], color, tag);
	m_displayHandler->displayLine(plane_points[1], plane_points[3], color, tag);
	
	// show the plane normal
	newVectorOnPlane.normalize3();
	displayArrow(startPos, planeNormal, newVectorOnPlane, color, scale, tag);
}

////////////////////////////////////////////////////////////////////////



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
