/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgeometry2/display/hkDisplayCapsule.h>

hkDisplayCapsule::hkDisplayCapsule( const hkVector4& top, const hkVector4& bottom, hkReal radius, int numSides , int numHeightSegments )
:  hkDisplayGeometry(HK_DISPLAY_CAPSULE), 
   m_numSides(numSides), m_numHeightSegments(numHeightSegments)
{
	m_top = top;
	m_bottom = bottom;
	m_radius = radius; 
}


void hkDisplayCapsule::buildGeometry()
{
	const hkVector4& start = m_top;
	const hkVector4& end = m_bottom;
	const hkReal radius = m_radius;

	const int heightSamples	= m_numHeightSegments;
	const int thetaSamples	= m_numSides;
	const int phiSamples	= thetaSamples >> 1;

	hkArray<hkVector4> verts;

		// Create "transform" from start, end.
	hkTransform capsuleToLocal;
	

	hkVector4 axis;
	axis.setSub4(end, start);
	hkReal height = axis.length3();
	if(height > 0.0f)
	{
		axis.normalize3();

		// find a quat which rotates (1,0,0) to axis
		hkVector4 canonicalZ(0,0,1);
		if(hkMath::fabs(axis.dot3(canonicalZ)) < 1 - 1e-5f)
		{
			hkVector4 rotAxis;
			rotAxis.setCross(canonicalZ, axis);
			rotAxis.normalize3();

			hkReal rotAngle = hkMath::acos(axis.dot3(canonicalZ));

			hkQuaternion q(rotAxis, rotAngle);
			capsuleToLocal.setRotation(q);
		}
		else
		{
			capsuleToLocal.setIdentity();	
		}

	}
	else
	{
		capsuleToLocal.setIdentity();
	}

			// Now recentre
	{
		hkVector4 toCentre;
		toCentre.setAdd4(start, end);
		toCentre.mul4(0.5f);
		capsuleToLocal.setTranslation(toCentre);
	}
	
	


	// We'll sweep aound the axis of the deflector, from top to bottom, using the original
	// sample directions and data to defien the vertices. We'll tessellate in the obvious way.
	// N.B. Top and bottom vertices are added to cap the object. These are calculated as the 
	// average of the surrounding vertcies, hence will be slightly flattened.

	int i,j;

	hkVector4 vert;

	hkVector4 bottomInGeom = hkVector4(0,0,-height/2);
	hkVector4 topInGeom = hkVector4(0,0,height/2);
	hkVector4 axisInGeom;
	axisInGeom.setSub4(topInGeom, bottomInGeom);
	hkVector4 axisNInGeom = hkVector4(0,0,1);
	hkVector4 normalInGeom = hkVector4(1,0,0);
	hkVector4 binormalInGeom = hkVector4(0,-1,0);

	verts.reserveExactly(2 * phiSamples * thetaSamples + 2 + (heightSamples-2) * thetaSamples);

		///////////////////// GET TOP VERTICES ///////////////////
	hkArray<hkVector4> topverts;
	for (i = phiSamples-1 ; i >= 0; i--)
	{
		hkQuaternion qTop(binormalInGeom, hkReal(i) / phiSamples * hkMath::HK_REAL_PI * .5f);
		hkVector4 topElevation;
		topElevation.setRotatedDir(qTop, normalInGeom);

		hkQuaternion qBottom(binormalInGeom, -hkReal(i) / phiSamples * hkMath::HK_REAL_PI * .5f);
		hkVector4 bottomElevation;
		bottomElevation.setRotatedDir(qBottom, normalInGeom);

		for (j = 0; j < thetaSamples; j++)
		{
			hkQuaternion rotationTop(axisNInGeom, hkReal(j) / thetaSamples * hkMath::HK_REAL_PI * 2);			
			hkVector4 topDirection;
			topDirection.setRotatedDir(rotationTop, topElevation);

			hkReal dist = radius;
			vert.setAddMul4(topInGeom, topDirection, dist);

			vert.setTransformedPos(capsuleToLocal, vert);

				// Temporarily store since we'll need these to calculate "top" vertx.
			topverts.pushBack(vert);

		}
	}

	vert = hkVector4(0, 0 ,height*0.5f + radius);
	vert.setTransformedPos(capsuleToLocal, vert);

		// Push back top vertex, and then the rest
	verts.pushBack(vert);
	for(i = 0; i < (phiSamples)*thetaSamples; i++)
	{
		verts.pushBack(topverts[i]);
	}

	
		///////////////////// GET MIDDLE VERTICES ///////////////////
	for (j = heightSamples-2; j >= 1; j--)
	{
	
		for (i = 0; i < thetaSamples; i++)
		{	
		//
		// Calculate direction vector for this angle
		//

			hkQuaternion q(axisNInGeom, hkReal(i) / thetaSamples * hkMath::HK_REAL_PI * 2);
			hkVector4 direction;
			direction.setRotatedDir(q, normalInGeom);
			
			hkVector4 start;
			start.setAddMul4(bottomInGeom, axisInGeom, hkReal(j) / hkReal(heightSamples - 1));

			hkReal dist = radius;

			vert.setAddMul4(start, direction, dist);

			vert.setTransformedPos(capsuleToLocal, vert);

			verts.pushBack(vert);

		}
	}

 


		///////////////////// GET BOTTOM VERTICES ///////////////////
	for (i = 0; i < phiSamples; i++)
	{
		hkQuaternion qTop(binormalInGeom, hkReal(i) / phiSamples * hkMath::HK_REAL_PI * .5f);
		hkVector4 topElevation;
		topElevation.setRotatedDir(qTop, normalInGeom);

		hkQuaternion qBottom(binormalInGeom, -hkReal(i) / phiSamples * hkMath::HK_REAL_PI * .5f);
		hkVector4 bottomElevation;
		bottomElevation.setRotatedDir(qBottom, normalInGeom);

		for (j = 0; j < thetaSamples; j++)
		{
			hkQuaternion rotationBottom(axisNInGeom, hkReal(j) / thetaSamples * hkMath::HK_REAL_PI * 2);			
			hkVector4 bottomDirection;
			bottomDirection.setRotatedDir(rotationBottom, bottomElevation);

			hkReal dist = radius;

			vert.setAddMul4(bottomInGeom, bottomDirection, dist);
			vert.setTransformedPos(capsuleToLocal, vert);
			verts.pushBack(vert);

		}
	}

	
	vert.set(0, 0 , -(height*0.5f + radius));
	vert.setTransformedPos(capsuleToLocal, vert);

		// Push back bottom vertex
	verts.pushBack(vert);


	
	///////////////////// CONSTRUCT FACE DATA ///////////////////

	hkGeometry* geom = new hkGeometry();
	geom->addVertices(verts.begin(), verts.getSize());
	// Right, num samples AROUND axis is thetaSamples.

	// First off, we have thetaSamples worth of faces connected to the top
	hkTriangleIndices tr;
	int currentBaseIndex = 1;
	for (i = 0; i < thetaSamples; i++)
	{
		tr.setVertex(0,0);
		tr.setVertex(1,currentBaseIndex + i);
		tr.setVertex(2,currentBaseIndex + (i+1)%(thetaSamples));
		geom->addTriangles(&tr,1);
	}

	// Next we have phi-1 + height-1 + phi-1 lots of thetaSamples*2 worth of faces connected to the previous row
	for(j = 0; j < 2*(phiSamples-1) + heightSamples-1; j++)
	{
		for (i = 0; i < thetaSamples; i++)
		{
			tr.setVertex(0,currentBaseIndex + i);
			tr.setVertex(1,currentBaseIndex + thetaSamples + i);
			tr.setVertex(2,currentBaseIndex + thetaSamples + (i+1)%(thetaSamples));


			geom->addTriangles(&tr,1);

			tr.setVertex(0,currentBaseIndex + i);
			tr.setVertex(2,currentBaseIndex + (i+1)%(thetaSamples));
			tr.setVertex(1,currentBaseIndex + thetaSamples + (i+1)%(thetaSamples));
		
			geom->addTriangles(&tr,1);

		
		}
		currentBaseIndex += thetaSamples;

	}

	// Finally, we have thetaSamples worth of faces connected to the bottom
	for (i = 0; i < thetaSamples; i++)
	{
		tr.setVertex(0,currentBaseIndex + i);
		tr.setVertex(2,currentBaseIndex + (i+1)%(thetaSamples));
		tr.setVertex(1,currentBaseIndex + thetaSamples);
		geom->addTriangles(&tr,1);
	}

	m_geometry = geom;
}


void hkDisplayCapsule::getWireframeGeometry(hkArray<hkVector4>& lines)
{
	//TODO
}
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
