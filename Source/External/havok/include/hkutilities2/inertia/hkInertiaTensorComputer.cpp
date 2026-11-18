/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkdynamics2/hkDynamics2.h>

#include <hkutilities2/inertia/hkInertiaTensorComputer.h>
#include <hkgeometry2/hkGeometry.h>
#include <hkgeometryutil2/hkGeometryUtil.h>
#include <hkgeometry2/hkTriangleUtil.h>



#include <hkcollide2/shape/hkBoxShape.h>
#include <hkcollide2/shape/hkSphereShape.h>
#include <hkcollide2/shape/hkTransformShape.h>
#include <hkcollide2/shape/hkListShape.h>
#include <hkcollide2/shape/hkConvexVerticesShape.h>
#include <hkcollide2/shape/hkTriangleShape.h>
#include <hkgeometry2/hkSphere.h>


#include <hkcollide2/shape/hkBvShape.h>
#include <hkcollide2/shape/hkBvTreeShape.h>
#include <hkcollide2/shape/hkMultiSphereShape.h>
#include <hkcollide2/shape/hkLineSegmentShape.h>


#include <hkdynamics2/entity/hkRigidBodyConstructionInfo.h>


hkResult HK_CALL hkInertiaTensorComputer::computeSphereVolumeMassProperties( hkReal radius, hkReal mass, hkMassProperties& result)
{
	HK_ASSERT2( mass > 0.0f, "Cannot calculate mass properties with zero mass or less." );
	if(mass <= 0.0f)
	{
		return HK_FAILURE;
	}

	HK_ASSERT2( radius > 0.0f, "Cannot calculate sphere mass properties with zero radius or less." );
	if(radius <= 0.0f)
	{
		return HK_FAILURE;
	}

	hkReal k = mass * radius * radius * 0.4f;

	result.m_inertiaTensor.setIdentity();

	result.m_inertiaTensor( 0, 0 ) = k;
	result.m_inertiaTensor( 1, 1 ) = k;
	result.m_inertiaTensor( 2, 2 ) = k;

	result.m_centerOfMass.setZero4();
	result.m_volume = 4.0f/3.0f * hkMath::HK_REAL_PI * radius * radius * radius;
	result.m_mass = mass;

	return HK_SUCCESS;
}

		
hkResult HK_CALL hkInertiaTensorComputer::computeSphereSurfaceMassProperties( hkReal radius, hkReal mass, hkReal surfaceThickness, hkMassProperties& result)
{
	HK_ASSERT2( mass > 0.0f, "Cannot calculate mass properties with zero mass or less." );
	if(mass <= 0.0f)
	{
		return HK_FAILURE;
	}
		// Compute mass properties of 2 spheres and subtract.
	HK_ASSERT2( radius > surfaceThickness, "Cannot calculate sphere surface mass properties when surfaceThickness greater than or equal to radius." );
	if(radius <= surfaceThickness)
	{
		return HK_FAILURE;
	}

	HK_ASSERT2( surfaceThickness > 0.0f, "Cannot calculate surface mass properties with zero or negative surfaceThickness." );
	if(surfaceThickness <= 0.0f)
	{
		return HK_FAILURE;
	}

	hkReal radiusBig = radius;
	hkReal radiusSmall = radius - surfaceThickness;
	

	hkReal volBig = 4.0f/3.0f * hkMath::HK_REAL_PI * radiusBig * radiusBig * radiusBig;
	hkReal volSmall = 4.0f/3.0f * hkMath::HK_REAL_PI * radiusSmall * radiusSmall * radiusSmall;

	// Assume uniform density ( rho = 1.0f)
	hkReal massBig = 1.0f * volBig;
	hkReal massSmall = 1.0f * volSmall;

	hkMassProperties resultBig;
	hkMassProperties resultSmall;

	computeSphereVolumeMassProperties(radiusBig, massBig, resultBig);
	computeSphereVolumeMassProperties(radiusSmall, massSmall, resultSmall);

	result.m_centerOfMass.setZero4();
	result.m_inertiaTensor = resultBig.m_inertiaTensor;
	result.m_inertiaTensor.sub(resultSmall.m_inertiaTensor);

	result.m_mass = resultBig.m_mass - resultSmall.m_mass;
	result.m_volume = resultBig.m_volume - resultSmall.m_volume;

	scaleMassProperties(mass / (volBig - volSmall), result);

	return HK_SUCCESS;
}

hkResult HK_CALL hkInertiaTensorComputer::computeBoxSurfaceMassProperties(const hkVector4& halfExtents, hkReal mass, hkReal surfaceThickness, hkMassProperties& result)
{
	HK_ASSERT2( mass > 0.0f, "Cannot calculate mass properties with zero mass or less." );
	if(mass <= 0.0f)
	{
		return HK_FAILURE;
	}
		// Compute mass properties of 2 boxes and subtract.

	HK_ASSERT2(  (halfExtents(0) > surfaceThickness) && (halfExtents(1) > surfaceThickness) && (halfExtents(2) > surfaceThickness),
		"Cannot calculate box surface mass properties with surfaceThickness less than min of half-extents" );
	if(!((halfExtents(0) > surfaceThickness) && (halfExtents(1) > surfaceThickness) && (halfExtents(2) > surfaceThickness)))
	{
		return HK_FAILURE;
	}

	HK_ASSERT2( surfaceThickness > 0.0f, "Cannot calculate surface mass properties with zero or negative surfaceThickness." );
	if(surfaceThickness <= 0.0f)
	{
		return HK_FAILURE;
	}

	hkVector4 halfExtentsSmall((halfExtents(0) - surfaceThickness), (halfExtents(1) - surfaceThickness), (halfExtents(2) - surfaceThickness));

	hkReal volBig = 8.0f * halfExtents(0) * halfExtents(1) * halfExtents(2);
	hkReal volSmall = 8.0f * halfExtentsSmall(0) * halfExtentsSmall(1) * halfExtentsSmall(2);

	// Assume uniform density ( rho = 1.0f)
	hkReal massBig = 1.0f * volBig;
	hkReal massSmall = 1.0f * volSmall;

	hkMassProperties resultBig;
	hkMassProperties resultSmall;

	computeBoxVolumeMassProperties(halfExtents, massBig, resultBig);
	computeBoxVolumeMassProperties(halfExtentsSmall, massSmall, resultSmall);

	result.m_centerOfMass.setZero4();
	result.m_inertiaTensor = resultBig.m_inertiaTensor;
	result.m_inertiaTensor.sub(resultSmall.m_inertiaTensor);

	result.m_mass = resultBig.m_mass - resultSmall.m_mass;
	result.m_volume = resultBig.m_volume - resultSmall.m_volume;

	scaleMassProperties(mass / (volBig - volSmall), result);

	return HK_SUCCESS;
}

hkResult HK_CALL hkInertiaTensorComputer::computeTriangleSurfaceMassProperties(const hkVector4& v0, const hkVector4& v1, const hkVector4& v2, hkReal mass, hkReal surfaceThickness, hkMassProperties& result)
{
	HK_ASSERT2( mass > 0.0f, "Cannot calculate mass properties with zero mass or less." );
	if(mass <= 0.0f)
	{
		return HK_FAILURE;
	}

	HK_ASSERT2( surfaceThickness >= 0.0f, "Cannot calculate surface mass properties with negative surfaceThickness." );
	if(surfaceThickness < 0.0f)
	{
		return HK_FAILURE;
	}

	const hkReal minTwiceArea = 1e-5f;

	hkVector4 com;
	hkMatrix3 it;
	hkVector4 normal;

	hkTriangleUtil::calcNormal(normal, v0, v1, v2);

	hkReal twiceArea = normal.length3();
	// If thickness less than 1e-5, use exact formula for triangular lamina.
	if(surfaceThickness < 1e-5f)
	{
		com = v0;
		com.add4(v1);
		com.add4(v2);
		com.setMul4(1.0f/3.0f, com);

		hkReal Ixx = mass/12.0f * ( 9.0f*com(0)*com(0) + v0(0)*v0(0) + v1(0)*v1(0) + v2(0)*v2(0) );
		hkReal Iyy = mass/12.0f * ( 9.0f*com(1)*com(1) + v0(1)*v0(1) + v1(1)*v1(1) + v2(1)*v2(1) );
		hkReal Izz = mass/12.0f * ( 9.0f*com(2)*com(2) + v0(2)*v0(2) + v1(2)*v1(2) + v2(2)*v2(2) );
		hkReal Ixy = mass/12.0f * ( 9.0f*com(0)*com(1) + v0(0)*v0(1) + v1(0)*v1(1) + v2(0)*v2(1) );
		hkReal Ixz = mass/12.0f * ( 9.0f*com(0)*com(2) + v0(0)*v0(2) + v1(0)*v1(2) + v2(0)*v2(2) );
		hkReal Iyz = mass/12.0f * ( 9.0f*com(1)*com(2) + v0(1)*v0(2) + v1(1)*v1(2) + v2(1)*v2(2) );

		it(0,0) = Iyy + Izz;
		it(1,1) = Ixx + Izz;
		it(2,2) = Ixx + Iyy;
		it(0,1) = it(1,0) = -Ixy;
		it(0,2) = it(2,0) = -Ixz;
		it(2,1) = it(1,2) = -Iyz;

		shiftInertiaToCom(com, mass, it);
	}
	else
	{
			// If very small area triangle, approx by point mass. Strictly speaking we should approximate by
			// a "bar" of length = surfaceThickness, but we assume that surfaceThickness is small.
		if(twiceArea < minTwiceArea)
		{
			com = v0;
			com.add4(v1);
			com.add4(v2);
			com.setMul4(1.0f/3.0f, com);

			hkReal rx = com(0);
			hkReal ry = com(1);
			hkReal rz = com(2);

			it(0,0) = mass * (ry*ry+rz*rz);
			it(1,1) = mass * (rx*rx+rz*rz);
			it(2,2) = mass * (ry*ry+rx*rx);
			it(0,1) = -mass * (ry*rx);
			it(1,0) = -mass * (ry*rx);
			it(0,2) = -mass * (rz*rx);
			it(2,0) = -mass * (rz*rx);
			it(2,1) = -mass * (ry*rz);
			it(1,2) = -mass * (ry*rz);	
		}
		else
		{
			normal.normalize3();

			hkInplaceArray<hkVector4, 6> verts;
			verts.setSize(6);

			verts[0] = v0;
			verts[1] = v0;
			verts[0].addMul4(surfaceThickness * 0.5f,normal);
			verts[1].addMul4(-surfaceThickness * 0.5f,normal);

			verts[2] = v1;
			verts[3] = v1;
			verts[2].addMul4(surfaceThickness * 0.5f,normal);
			verts[3].addMul4(-surfaceThickness * 0.5f,normal);

			verts[4] = v2;
			verts[5] = v2;
			verts[4].addMul4(surfaceThickness * 0.5f,normal);
			verts[5].addMul4(-surfaceThickness * 0.5f,normal);

			
			hkMassProperties properties;		
			{
				hkGeometry triGeom;
				triGeom.addVertices(verts.begin(), 6);

				hkInplaceArray<hkTriangleIndices, 8> tris;
				tris.setSize(8);

				tris[0].setTriangleIndices(0, 2, 4);	// top
				tris[1].setTriangleIndices(1, 5, 3);	// bottom

				tris[2].setTriangleIndices(0, 3, 2);
				tris[3].setTriangleIndices(0, 1, 3);

				tris[4].setTriangleIndices(1, 0, 4);
				tris[5].setTriangleIndices(1, 4, 5);

				tris[6].setTriangleIndices(2, 5, 4);
				tris[7].setTriangleIndices(2, 3, 5);

				triGeom.addTriangles(tris.begin(), 8);
				hkInertiaTensorComputer::computeGeometryVolumeMassProperties(&triGeom, mass, properties);
			}
			
			com = properties.m_centerOfMass;
			it = properties.m_inertiaTensor;
		}
	}

	result.m_mass = mass;
	result.m_inertiaTensor = it;
	result.m_centerOfMass = com;
	result.m_volume = twiceArea * 0.5f * surfaceThickness;

	return HK_SUCCESS;

}


hkResult HK_CALL hkInertiaTensorComputer::computeBoxVolumeMassProperties(const hkVector4& halfExtents, hkReal mass, hkMassProperties &result)
{
	HK_ASSERT2( mass > 0.0f, "Cannot calculate mass properties with zero mass or less." );
	if(mass <= 0.0f)
	{
		return HK_FAILURE;
	}

	hkReal x = halfExtents(0);
	hkReal y = halfExtents(1);
	hkReal z = halfExtents(2);

	hkReal k = mass * ( 1.0f / 3.0f);

	result.m_inertiaTensor.setIdentity();

	result.m_inertiaTensor( 0, 0 ) = (y*y + z*z) * k;
	result.m_inertiaTensor( 1, 1 ) = (x*x + z*z) * k;
	result.m_inertiaTensor( 2, 2 ) = (x*x + y*y) * k;

	result.m_centerOfMass.setZero4();
	result.m_volume = x * y * z * 8.0f;
	result.m_mass = mass;

	return HK_SUCCESS;

}

hkResult HK_CALL hkInertiaTensorComputer::computeVertexHullVolumeMassProperties(const hkReal* vertexIn, int striding, int numVertices, hkReal mass, hkMassProperties &result)
{
	HK_ASSERT2( mass > 0.0f, "Cannot calculate mass properties with zero mass or less." );
	if(mass <= 0.0f)
	{
		return HK_FAILURE;
	}

		// Extract vertices into an array
	hkArray<hkVector4> verts;
	verts.setSize(numVertices);
	hkReal *ptr = const_cast<hkReal*> (vertexIn);
	for(int i = 0; i < numVertices; i++)
	{
		verts[i](0) = ptr[0];
		verts[i](1) = ptr[1];
		verts[i](2) = ptr[2];
		ptr = hkAddByteOffset( ptr, striding );
	}

	hkGeometry* g = hkGeometryUtil::createConvexGeometry(&(verts[0]), numVertices, false);
	if(g == HK_NULL)
	{
		return HK_FAILURE;
	}

	hkInertiaTensorComputer::computeGeometryVolumeMassProperties(g, mass, result);
	result.m_mass = mass;

	delete g;
	return HK_SUCCESS;
}

hkResult HK_CALL hkInertiaTensorComputer::computeVertexCloudMassProperties(const hkReal* vertexIn, int striding, int numVertices, hkReal mass, hkMassProperties &result)
{
	
	HK_ASSERT2( numVertices > 0, "Cannot calculate mass properties with zero vertices." );

	if(numVertices <= 0)
	{
		return HK_FAILURE;
	}

		// Extract vertices into an array
	hkArray<hkVector4> verts;
	verts.setSize(numVertices);
	hkReal *ptr = const_cast<hkReal*> (vertexIn);
	int i;
	for(i = 0; i < numVertices; i++)
	{
		verts[i](0) = ptr[0];
		verts[i](1) = ptr[1];
		verts[i](2) = ptr[2];
		ptr = hkAddByteOffset( ptr, striding );
	}

	result.m_mass = mass;
	result.m_inertiaTensor.setZero();
	result.m_centerOfMass.setZero4();

	hkReal pointMass = mass / numVertices;

	// We can do this calculation all-in-one (computing the inertia tensor in another frame
	// and shifting it back to be around the center of mass), but it's more accurate to compute the
	// center of mass first
	for(i = 0; i < numVertices; i++)
	{
		result.m_centerOfMass.add4(verts[i]);	
	}
	result.m_centerOfMass.setMul4(1.0f/numVertices, result.m_centerOfMass);

	for(i = 0; i < numVertices; i++)
	{
		hkReal rx, ry, rz;
		rx = verts[i](0) - result.m_centerOfMass(0);
		ry = verts[i](1) - result.m_centerOfMass(1);
		rz = verts[i](2) - result.m_centerOfMass(2);
		
		result.m_inertiaTensor(0,0) += pointMass * (ry*ry+rz*rz);
		result.m_inertiaTensor(1,1) += pointMass * (rx*rx+rz*rz);
		result.m_inertiaTensor(2,2) += pointMass * (ry*ry+rx*rx);
		result.m_inertiaTensor(0,1) -= pointMass * (ry*rx);
		result.m_inertiaTensor(1,0) -= pointMass * (ry*rx);
		result.m_inertiaTensor(0,2) -= pointMass * (rz*rx);
		result.m_inertiaTensor(2,0) -= pointMass * (rz*rx);
		result.m_inertiaTensor(2,1) -= pointMass * (ry*rz);
		result.m_inertiaTensor(1,2) -= pointMass * (ry*rz);
		
	
	}

	return HK_SUCCESS;	
}


hkResult HK_CALL hkInertiaTensorComputer::computeCapsuleVolumeMassProperties(const hkVector4& startAxis, const hkVector4& endAxis, hkReal radius, hkReal mass, hkMassProperties& result)
{
	HK_ASSERT2( mass > 0.0f, "Cannot calculate mass properties with zero mass or less." );
	if(mass <= 0.0f)
	{
		return HK_FAILURE;
	}

	
	HK_ASSERT2( radius > 0.0f, "Cannot calculate capsule mass properties with zero radius or less. You may edit hkInertiaTensorComputer::computeCapsuleVolumeProperties() to bypass this assert if you are sure this is the behaviour you desire." );
	if(radius <= 0.0f)
	{
		return HK_FAILURE;
	}
	
	// First determine a transform from the capsule to "canonical space": (0,0,0) is the centre of the capsule
	// and the axis lies along the Z-axis

	hkVector4 axis;
	axis.setSub4(endAxis, startAxis);
	hkReal height = axis.length3();
	hkTransform capsuleToLocal;
	if(height > 0.0f)
	{
		axis.normalize3();

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
		toCentre.setAdd4(startAxis, endAxis);
		toCentre.mul4(0.5f);
		capsuleToLocal.setTranslation(toCentre);
	}




	// Add together the mnass properties of two hemispheres and a cylinder



	// Divide mass evenly amongst the caps and cyliner, based on volume
	const hkReal volumeOfCaps = 4.0f / 3.0f * hkMath::HK_REAL_PI * radius * radius * radius;
	const hkReal volumeOfCylinder = hkMath::HK_REAL_PI * radius * radius * height;
	const hkReal totalVolume = volumeOfCaps + volumeOfCylinder;

	const hkReal massOfCapsules = mass * volumeOfCaps / totalVolume;
	const hkReal massOfCylinder = mass * volumeOfCylinder / totalVolume;
	

	hkArray<hkMassElement> elements;

	
	// First do cylinder:
	// See Graphics Gems III p 142
	{
		hkMassElement cylinderElement;
		cylinderElement.m_transform = capsuleToLocal;

		cylinderElement.m_properties.m_centerOfMass.set(0,0,0);
		cylinderElement.m_properties.m_inertiaTensor.setIdentity();

		cylinderElement.m_properties.m_inertiaTensor(0,0) = radius * radius * 0.25f	+ (height/2 * height/2) / 3.0f;
		cylinderElement.m_properties.m_inertiaTensor(1,1) = cylinderElement.m_properties.m_inertiaTensor(0,0);
		cylinderElement.m_properties.m_inertiaTensor(2,2) = radius * radius * 0.5f;

			// Scale by mass
		cylinderElement.m_properties.m_inertiaTensor.mul(massOfCylinder);

		cylinderElement.m_properties.m_centerOfMass.setZero4();

		cylinderElement.m_properties.m_volume = volumeOfCylinder;
		cylinderElement.m_properties.m_mass = massOfCylinder;

		elements.pushBack(cylinderElement);
	}
	

	
		// Top cap
	{
		hkMassElement capElement;
		capElement.m_transform = capsuleToLocal;
		hkVector4 pos(0, 0, (height / 2.0f));
		pos.setRotatedDir(capsuleToLocal.getRotation(), pos);
		pos.add4(capElement.m_transform.getTranslation());
		capElement.m_transform.setTranslation(pos);

		capElement.m_properties.m_centerOfMass.set(0, 0, 3.0f / 8.0f * radius);
		// Now here's the tricky bit. TWO hemispheres make a sphere, hence "half a sphere IT" shifted by COM
		// should be the IT of a hemispere, right? This only works because of the symmetry of the integrals x^2, y^2 and z^2
		// over the sphere.
		{
			hkMassProperties sphereResult;
			computeSphereVolumeMassProperties(radius, massOfCapsules, sphereResult);

			capElement.m_properties.m_inertiaTensor = sphereResult.m_inertiaTensor;
			capElement.m_properties.m_inertiaTensor.mul(0.5f);

			// Shift IT back
			shiftInertiaToCom(capElement.m_properties.m_centerOfMass, massOfCapsules * 0.5f, capElement.m_properties.m_inertiaTensor);
		}

		capElement.m_properties.m_volume = volumeOfCaps * 0.5f;
		capElement.m_properties.m_mass = massOfCapsules * 0.5f;

		elements.pushBack(capElement);
	}

	
		// Bottom cap
	{
		hkMassElement capElement;
		capElement.m_transform = capsuleToLocal;
		hkVector4 pos(0, 0, -(height / 2.0f));
		pos.setRotatedDir(capsuleToLocal.getRotation(), pos);
		pos.add4(capElement.m_transform.getTranslation());
		capElement.m_transform.setTranslation(pos);


		capElement.m_properties.m_centerOfMass.set(0, 0, -3.0f / 8.0f * radius);
		// Now here's the tricky bit. TWO hemispheres make a sphere, hence "half a sphere IT" shifted by COM
		// should be the IT of a hemispere, right? This only works because of the symmetry of the integrals x^2, y^2 and z^2
		// over the sphere.
		{
			hkMassProperties sphereResult;
			computeSphereVolumeMassProperties(radius, massOfCapsules, sphereResult);

			capElement.m_properties.m_inertiaTensor = sphereResult.m_inertiaTensor;
			capElement.m_properties.m_inertiaTensor.mul(0.5f);

			// Shift IT back
			shiftInertiaToCom(capElement.m_properties.m_centerOfMass, massOfCapsules * 0.5f, capElement.m_properties.m_inertiaTensor);
		}

		capElement.m_properties.m_volume = volumeOfCaps * 0.5f;
		capElement.m_properties.m_mass = massOfCapsules * 0.5f;

		elements.pushBack(capElement);
	}
	
	
	combineMassProperties(elements, result);


	return HK_SUCCESS;

	
}



hkResult HK_CALL hkInertiaTensorComputer::computeGeometrySurfaceMassProperties(const hkGeometry* geom, hkReal surfaceThickness, bool distributeUniformly, hkReal mass, hkMassProperties &result)
{
	HK_ASSERT2( mass > 0.0f, "Cannot calculate mass properties with zero mass or less." );
	if(mass <= 0.0f)
	{
		return HK_FAILURE;
	}

	HK_ASSERT2( surfaceThickness > 0.0f, "Cannot calculate surface mass properties with zero or negative surfaceThickness." );
	if(surfaceThickness <= 0.0f)
	{
		return HK_FAILURE;
	}
	


	result.m_inertiaTensor.setZero();
	result.m_centerOfMass.setZero4();

	// First we need to find the mass of each triangle.
	// We'll say it's proportional to the area if distributeUniformly=true, otherwise we
	// give the same mass to each triangle, independent of its area. This means that "highly tessellated"
	// areas will have more mass, usually undesirable, but perhaps useful in some instances.
	int i;

	const hkReal minTwiceArea = 1e-5f;
	hkArray<hkReal>	triArea;
	triArea.setSize(geom->getNumTriangles());
	hkReal totalArea = 0.0f;

	for(i = 0; i < geom->getNumTriangles(); i++)
	{
		const hkTriangleIndices& tri = geom->getTriangle(i); 
	
		hkVector4 normal;
		hkVector4 v0 = geom->getVertex(tri.getVertex(0));
		hkVector4 v1 = geom->getVertex(tri.getVertex(1));
		hkVector4 v2 = geom->getVertex(tri.getVertex(2));

		hkTriangleUtil::calcNormal(normal, v0, v1, v2);
		hkReal twiceArea = normal.length3();


		if(distributeUniformly)
		{
				// We have a minimum area allowed (so as to avoid breaking the convex hull code)
			if(twiceArea < minTwiceArea)
			{
				triArea[i] = 0.5f * minTwiceArea;
			}
			else
			{
				triArea[i] = 0.5f * twiceArea;
			}
		}
		else
		{
			// Assume every triangle has the same "area" eg. 1.0
			triArea[i] = 1.0f;
		}
		totalArea += triArea[i];
	}

	// For each triangle
	for(i = 0; i < geom->getNumTriangles(); i++)
	{
		const hkTriangleIndices& tri = geom->getTriangle(i); 
	
		// OK, let's pick the triangle, expand it out, and use this as geom to pass to the volume integrator

		hkVector4 normal;
		hkVector4 v0 = geom->getVertex(tri.getVertex(0));
		hkVector4 v1 = geom->getVertex(tri.getVertex(1));
		hkVector4 v2 = geom->getVertex(tri.getVertex(2));

		hkReal triMass = mass * triArea[i] / totalArea;

		hkMassProperties triMassProperties;
		hkInertiaTensorComputer::computeTriangleSurfaceMassProperties(v0, v1, v2, triMass, surfaceThickness, triMassProperties);
		hkVector4 com = triMassProperties.m_centerOfMass;
		hkMatrix3 it = triMassProperties.m_inertiaTensor;

		result.m_centerOfMass.addMul4(triMass, com);
  	    // To "add" a new inertia tensor, we must move it to a common space first
		hkVector4 shift = com;
		shiftInertiaFromCom(shift, triMass, it);
		result.m_inertiaTensor.add(it);			
	}

	result.m_mass = mass;
	result.m_centerOfMass.setMul4(1.0f/mass, result.m_centerOfMass);
	// Finally shift back to new center of mass
	shiftInertiaToCom(result.m_centerOfMass, mass, result.m_inertiaTensor);



	return HK_SUCCESS;
}


hkResult HK_CALL hkInertiaTensorComputer::combineMassProperties(const hkArray<hkMassElement>& elements, hkMassProperties& result )
{
	// We'll have to move all ITs to a common space, and combine them. To determine the common space,
	// find the new center of mass.
	hkVector4 groupCenterOfMass;
	groupCenterOfMass.setZero4();

	hkReal groupMass = 0.0f;
	hkReal groupVolume = 0.0f;

	int i;
	for(i = 0; i < elements.getSize(); i++)
	{
		hkVector4 centerOfMassInCommon;
		centerOfMassInCommon.setTransformedPos(elements[i].m_transform, elements[i].m_properties.m_centerOfMass);
		groupCenterOfMass.addMul4(elements[i].m_properties.m_mass, centerOfMassInCommon);
		
		groupMass += elements[i].m_properties.m_mass;
		groupVolume += elements[i].m_properties.m_volume;
	}


	HK_ASSERT2( groupMass > 0.0f, "Cannot calculate group mass properties with zero mass or less." );
	if(groupMass <= 0.0f)
	{
		return HK_FAILURE;
	}

	groupCenterOfMass.setMul4(1.0f/groupMass, groupCenterOfMass);

	result.m_centerOfMass = groupCenterOfMass;
	result.m_mass = groupMass;
	result.m_volume = groupVolume;

	result.m_inertiaTensor.setZero();


	// We now have to "antishift" each IT from its center of mass, and rotate them to the common space
	// after which we can add them.
	for(i = 0; i < elements.getSize(); i++)
	{

		hkMatrix3 inertiaTensorInCommonSpace;
		inertiaTensorInCommonSpace = elements[i].m_properties.m_inertiaTensor;

				// First rotate.
		inertiaTensorInCommonSpace.changeBasis(elements[i].m_transform.getRotation());
				// Then shift.
		hkVector4 shift;
		shift.setTransformedPos(elements[i].m_transform, elements[i].m_properties.m_centerOfMass);
		shift.setSub4(shift, groupCenterOfMass);
		shiftInertiaFromCom(shift, elements[i].m_properties.m_mass, inertiaTensorInCommonSpace);

		// Now can add to other inertia tensors
		result.m_inertiaTensor.add(inertiaTensorInCommonSpace);
	
	}
	return HK_SUCCESS;


}


void HK_CALL hkInertiaTensorComputer::setShapeVolumeMassProperties(const hkShape* shape, hkReal mass, hkRigidBodyConstructionInfo& bodyInfo)
{
	HK_ASSERT2(shape != HK_NULL, "shape is NULL in setShapeVolumeMassProperties()!");

	hkMassElement element;

		// Default constructor already initialises these, but this makes
		// it explicit as to what the "uncomputed" values are.
	element.m_properties.m_centerOfMass.setZero4();
	element.m_properties.m_inertiaTensor.setZero();
	element.m_properties.m_mass = 0.0f;
	element.m_properties.m_volume = 0.0f;
	element.m_transform.setIdentity();


	computeRecursiveShapeVolumeMassProperties(shape, element.m_transform, element.m_properties);

	HK_ASSERT2(element.m_properties.m_volume != 0.0f, " Cannot call computeShapeVolumeMassProperties on a shape with no volume!");
	if(element.m_properties.m_volume == 0.0f)
	{
		element.m_properties.m_centerOfMass.setZero4();
		element.m_properties.m_inertiaTensor.setZero();
		element.m_properties.m_mass = 0.0f;
		element.m_properties.m_volume = 0.0f;
		return;
	}

	// The above call assumes density = 1.0, so we must rescale using mass and volume.
	scaleMassProperties(mass/element.m_properties.m_volume, element.m_properties);
	
	// Extract the relevant info
	bodyInfo.m_mass = element.m_properties.m_mass;
	bodyInfo.m_centerOfMass = element.m_properties.m_centerOfMass;
	bodyInfo.m_inertiaTensor = element.m_properties.m_inertiaTensor;

}

void HK_CALL hkInertiaTensorComputer::computeShapeVolumeMassProperties(const hkShape* shape, hkReal mass, hkMassProperties &result)
{
	HK_ASSERT2(shape != HK_NULL, "shape is NULL in computeShapeVolumeMassProperties()!");
 
	hkMassElement element;

		// Default constructor already initialises these, but this makes
		// it explicit as to what the "uncomputed" values are.
	element.m_properties.m_centerOfMass.setZero4();
	element.m_properties.m_inertiaTensor.setZero();
	element.m_properties.m_mass = 0.0f;
	element.m_properties.m_volume = 0.0f;
	element.m_transform.setIdentity();


	computeRecursiveShapeVolumeMassProperties(shape, element.m_transform, element.m_properties);

	HK_ASSERT2(element.m_properties.m_volume != 0.0f, " Cannot call computeShapeVolumeMassProperties on  a shape with no volume!");
	if(element.m_properties.m_volume == 0.0f)
	{
		element.m_properties.m_centerOfMass.setZero4();
		element.m_properties.m_inertiaTensor.setZero();
		element.m_properties.m_mass = 0.0f;
		element.m_properties.m_volume = 0.0f;
		return;
	}

	// The above call assumes density = 1.0, so we must rescale using mass and volume.
	scaleMassProperties(mass/element.m_properties.m_volume, element.m_properties);

	result = element.m_properties;

}

	
void HK_CALL hkInertiaTensorComputer::computeRecursiveShapeVolumeMassProperties(const hkShape* shape, const hkTransform& transform, hkMassProperties& massPropertiesOut)
{
	hkMassProperties newMassProperties;

		// Here's the deal. We don't know what the final mass distribution should be, so we ASSUME uniform density
		// which means we can compute massProperties with mass=1, then scale to get values for DENSITY=1 using the volume
		// computed. This means we can ONLY work on shapes which have a non-zero volume.

		// If we can actually COMPUTE the mass properties given the shape type, we do so, and combine (below the
		// switch statement), otherwise we recurse ( eg. hkTransformShape, hkListShape etc.)
	HK_ASSERT2( shape != HK_NULL, "Error: shape is NULL!");
	
	switch (shape->getPrimaryType())
	{
		
		case HK_SHAPE_SPHERE:
		{
			const hkSphereShape* sphereShape = static_cast<const hkSphereShape*>(shape);
			hkInertiaTensorComputer::computeSphereVolumeMassProperties(sphereShape->getRadius(), 1.0f, newMassProperties);
			break;
		}
		
		
		case HK_SHAPE_BOX:
		{
			const hkBoxShape* boxShape = static_cast<const hkBoxShape*>(shape);
			hkVector4 halfExtents = boxShape->getHalfExtents();
			hkInertiaTensorComputer::computeBoxVolumeMassProperties(halfExtents, 1.0f, newMassProperties);
			break;
		}

		
		case HK_SHAPE_CONVEX_VERTICES:
		{
			const hkConvexVerticesShape* convexVerticesShape = static_cast<const hkConvexVerticesShape*>(shape);
	
				// To get vertices, we must get spheres as there is no immediate interface to the vertices.
			hkArray<hkSphere> spheres;
			convexVerticesShape->getCollisionSpheres(spheres);

				// Extract these vertices as the sphere centres.
			hkArray<hkVector4> vertices;
			vertices.setSize( spheres.getSize() );
			for(int i = 0; i < vertices.getSize(); i++)
			{
				vertices[i] = spheres[i].getPosition();
			}			
			hkResult result = hkInertiaTensorComputer::computeVertexHullVolumeMassProperties(reinterpret_cast<hkReal*>(vertices.begin()), sizeof(hkVector4), vertices.getSize(), 1.0f, newMassProperties);
			HK_ASSERT2( result != HK_FAILURE, "Failed to build hull of vertices - Perhaps the convexVerticesShape has no volume!." );

			
			break;
		}

		
		case HK_SHAPE_TRIANGLE:
		{
			const hkTriangleShape* triangleShape = static_cast<const hkTriangleShape*>(shape);
			hkVector4 v0 = triangleShape->getVertex(0);
			hkVector4 v1 = triangleShape->getVertex(1);
			hkVector4 v2 = triangleShape->getVertex(2);
			hkInertiaTensorComputer::computeTriangleSurfaceMassProperties(v0, v1, v2, 1.0f, triangleShape->getRadius(), newMassProperties);
			if(triangleShape->getRadius() == 0.0f)
			{
				HK_WARN("Computing mass properties of triangle with thickness (radius) of 0. It has no volume, so mass cannot be distributed correctly in computeShapeVolumeMassProperties(), hence properties are ignored.\n");
			}
			break;
		}


		case HK_SHAPE_MULTI_RAY:
		{
			// EMPTY - Rays have no volume!
			HK_WARN("HK_SHAPE_MULTI_RAY has no volume, mass properties ignored.\n");
			return;
		}
		

		case HK_SHAPE_BV:
		{
			const hkBvShape* bvShape = static_cast<const hkBvShape*>(shape);
		
			computeRecursiveShapeVolumeMassProperties(bvShape->getChildShape(), transform, massPropertiesOut);

			// Return here, since the above call will have actually done the "addition" of the child's 
			// mass properties.
			return;
		}		
		
			// Recurse here
		case HK_SHAPE_TRANSFORM:
		{
			const hkTransformShape* ts = static_cast<const hkTransformShape*>( shape );

			// Must concatenate this transform on before recursing. 
			hkTransform t;
			t.setMul(transform, ts->getTransform());	
			const hkShape* childShape = ts->getChildShape();
			computeRecursiveShapeVolumeMassProperties(childShape, t, massPropertiesOut);

			// Return here, since the above call will have actually done the "addition" of the child's 
			// mass properties.
			return;
		}


		case HK_SHAPE_LIST:
		case HK_SHAPE_MULTI_SPHERE:
		case HK_SHAPE_COLLECTION:
		case HK_SHAPE_BV_TREE:
		{
				// All of these work on a "hkShapeCollection".
				// The LIST, MULTI_SPHERE and COLLECTION are hkShapeCollections
				// The BV_TREE "owns" a hkShapeCollection.

			const hkShapeCollection* shapeCollection;			
			if(shape->getPrimaryType() == HK_SHAPE_BV_TREE)
			{
				const hkBvTreeShape* bvTreeshape = static_cast<const hkBvTreeShape*>(shape);	
				shapeCollection = bvTreeshape->getShapeCollection();	
			}
			else
			{
				shapeCollection = static_cast<const hkShapeCollection*>(shape);			
			}

			// Loop through all children
			for (int i = 0; i < shapeCollection->getNumChildShapes(); ++i)
			{
				hkShapeCollection::AllocBuffer buffer;
				const hkShape* childShape = shapeCollection->getChildShape( shapeCollection->getKeyAt(i), buffer );
				if (childShape != HK_NULL)
				{
					computeRecursiveShapeVolumeMassProperties(childShape, transform, massPropertiesOut);	
				}
			}
			// Return here, since the above call will have actually done the "addition" of the child's 
			// mass properties.
			return;
		}

						

		case HK_SHAPE_PLANE:
		{
			// TODO - Plane shape not yet fully implemented.
			HK_ASSERT2( 0, "Cannot compute mass propeties of HK_SHAPE_PLANE." );
			break;
		}


		case HK_SHAPE_LINE_SEGMENT:
		{
			const hkLineSegmentShape* lineSegmentShape = static_cast<const hkLineSegmentShape*>(shape);
			computeCapsuleVolumeMassProperties(lineSegmentShape->getBottom(), lineSegmentShape->getTop(), lineSegmentShape->getRadius(), 1.0f, newMassProperties);
			break;
		}
		
		
		default:
		{
			HK_WARN("Unknown shape type to compute mass propeties of. Have you implemented a user shape but not yet implemented mass property calculation in hkInertiaTensorComputer::computeRecursiveShapeVolumeMassProperties()?\n" );
			return;
		}
		

	}

	// If volume of shape is zero, we cannot distribute the mass correctly, so we ignore this shape's contribution.
	if(newMassProperties.m_volume == 0.0f)
	{
		return;
	}

	// First rescale the mass properties so density = 1.0f
	scaleMassProperties(newMassProperties.m_volume/1.0f, newMassProperties);

	
	{
		hkArray<hkMassElement> allElements;

		// Even in the base case (where we just want to do massProperties = newMassProperties),
		// we can still use the call to combineMassProperties since the base massProperties will be
		// "zero", and will combine correctly with newMassProperties. cf. combineMassProperties()

		hkMassElement currentMassElement;
		currentMassElement.m_properties = massPropertiesOut;
		allElements.pushBack(currentMassElement);
		

		hkMassElement newMassElement;
		newMassElement.m_properties = newMassProperties;
		newMassElement.m_transform = transform;
		allElements.pushBack(newMassElement);

		combineMassProperties(allElements, massPropertiesOut);
	}	

}

void HK_CALL hkInertiaTensorComputer::shiftInertiaToCom(hkVector4& shift, hkReal mass, hkMatrix3& inertia)
{
	inertia(0,0) -= mass * (shift(1)*shift(1) + shift(2)*shift(2));
	inertia(1,1) -= mass * (shift(2)*shift(2) + shift(0)*shift(0));
	inertia(2,2) -= mass * (shift(0)*shift(0) + shift(1)*shift(1));
	inertia(0,1) = inertia(1,0) += mass * shift(0) * shift(1); 
	inertia(1,2) = inertia(2,1) += mass * shift(1) * shift(2); 
	inertia(2,0) = inertia(0,2) += mass * shift(2) * shift(0); 
}

void HK_CALL hkInertiaTensorComputer::shiftInertiaFromCom(hkVector4& shift, hkReal mass, hkMatrix3& inertia)
{
	inertia(0,0) += mass * (shift(1)*shift(1) + shift(2)*shift(2));
	inertia(1,1) += mass * (shift(2)*shift(2) + shift(0)*shift(0));
	inertia(2,2) += mass * (shift(0)*shift(0) + shift(1)*shift(1));
	inertia(0,1) = inertia(1,0) -= mass * shift(0) * shift(1); 
	inertia(1,2) = inertia(2,1) -= mass * shift(1) * shift(2); 
	inertia(2,0) = inertia(0,2) -= mass * shift(2) * shift(0); 
}


void HK_CALL hkInertiaTensorComputer::scaleMassProperties(hkReal scale, hkMassProperties& massProperties)
{
	// We change the MASS values only
	massProperties.m_mass = massProperties.m_mass * scale;
	massProperties.m_inertiaTensor(0,0) *= scale;
	massProperties.m_inertiaTensor(0,1) *= scale;
	massProperties.m_inertiaTensor(0,2) *= scale;
	massProperties.m_inertiaTensor(1,0) *= scale;
	massProperties.m_inertiaTensor(1,1) *= scale;
	massProperties.m_inertiaTensor(1,2) *= scale;
	massProperties.m_inertiaTensor(2,0) *= scale;
	massProperties.m_inertiaTensor(2,1) *= scale;
	massProperties.m_inertiaTensor(2,2) *= scale;
}


void HK_CALL hkInertiaTensorComputer::simplifyInertiaTensorToOrientedParticle(hkMatrix3 &inertia)
{
		// Take max value. This ensures that low angular impulses applied at large
		// distances from the centre of mass do not result in large changes in velocity
		// (which could cause instability).
	hkReal temp = hkMath::max2( hkMath::max2( inertia(0,0), inertia(1,1) ), inertia(2,2) );
	inertia.setDiagonal(temp, temp, temp);
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////// The rest of this file contains the polytope inertia tensor code /////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

namespace InternalInertiaTensorComputer
{
	static void HK_CALL computeInertialTensorInternal(hkReal mass, hkReal density, hkVector4 &r, hkMatrix3 &J);
	static void HK_CALL compProjectionIntegrals(const hkVector4* v);
	static void HK_CALL compFaceIntegrals(const hkVector4* v,const hkVector4& n);
	static void HK_CALL compVolumeIntegrals(const hkGeometry* geom,const hkVector4& shift);
	static hkVector4 HK_CALL compShift(const hkGeometry* geom);

	static int A;   /* alpha */
	static int B;   /* beta */
	static int C;   /* gamma */

	/* projection integrals */
	static hkReal P1, Pa, Pb, Paa, Pab, Pbb, Paaa, Paab, Pabb, Pbbb;

	/* face integrals */
	static hkReal Fa, Fb, Fc, Faa, Fbb, Fcc, Faaa, Fbbb, Fccc, Faab, Fbbc, Fcca;

	/* volume integrals */
	static hkReal T0, T1[3], T2[3], TP[3];
	
	static inline hkReal HK_CALL sqr(hkReal x)
	{
		return x * x;
	}

	static inline hkReal HK_CALL cube(hkReal x)
	{
		return x * x * x;
	}
}




///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Mirtich inertia tensor computer code
//
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////



	/*******************************************************
    *                                                      *
	*  volInt.cpp                                          *
	*                                                      *
	*  This code computes volume integrals needed for      *
	*  determining mass properties of polyhedral bodies.   *
	*                                                      *
	*  For more information, see thepaper                  *
	*                                                      *
	*  Brian Mirtich, "Fast and Accurate Computation of    *
	*  Polyhedral Mass Properties," journal of graphics    *
	*  tools, volume 1, number 1, 1996.                    *
	*                                                      *
	*  This source code is public domain, and may be used  *
	*  in any way, shape or form, free of charge.          *
	*                                                      *
	*  Copyright 1995 by Brian Mirtich                     *
	*                                                      *
	*  mirtich@cs.berkeley.edu                             *
	*  http://www.cs.berkeley.edu/~mirtich                 *
    *                                                      *
	*******************************************************/

/*
	Revision history

	26 Jan 1996	Program creation.

	 3 Aug 1996	Corrected bug arising when polyhedron density
			is not 1.0.  Changes confined to function main().
			Thanks to Zoran Popovic for catching this one.

	27 May 1997     Corrected sign error in translation of inertia
	                product terms to center of mass frame.  Changes 
			confined to function main().  Thanks to 
			Chris Hecker.
*/






/*
   ============================================================================
   constants
   ============================================================================
*/
#define X 0
#define Y 1
#define Z 2

/* compute various integrations over projection of face */
void HK_CALL InternalInertiaTensorComputer::compProjectionIntegrals(const hkVector4* v)
{
	hkReal a0, a1, da;
	hkReal b0, b1, db;
	hkReal a0_2, a0_3, a0_4, b0_2, b0_3, b0_4;
	hkReal a1_2, a1_3, b1_2, b1_3;
	hkReal C1, Ca, Caa, Caaa, Cb, Cbb, Cbbb;
	hkReal Cab, Kab, Caab, Kaab, Cabb, Kabb;
	int i;

	P1 = Pa = Pb = Paa = Pab = Pbb = Paaa = Paab = Pabb = Pbbb = 0.0f;

    // We are only dealing with triangle meshes so
    // number of vertices is always 3!
   for (i = 0; i < 3; i++) 
   {
	   a0 = v[i](A);//f->poly->verts[f->verts[i]][A];
	   b0 = v[i](B);//f->poly->verts[f->verts[i]][B];

	   a1 = v[(i+1) % 3](A);
	   b1 = v[(i+1) % 3](B);
    
	   da = a1 - a0;
	   db = b1 - b0;
	   a0_2 = a0 * a0; a0_3 = a0_2 * a0; a0_4 = a0_3 * a0;
	   b0_2 = b0 * b0; b0_3 = b0_2 * b0; b0_4 = b0_3 * b0;
	   a1_2 = a1 * a1; a1_3 = a1_2 * a1; 
	   b1_2 = b1 * b1; b1_3 = b1_2 * b1;

	   C1 = a1 + a0;
	   Ca = a1*C1 + a0_2; Caa = a1*Ca + a0_3; Caaa = a1*Caa + a0_4;
	   Cb = b1*(b1 + b0) + b0_2; Cbb = b1*Cb + b0_3; Cbbb = b1*Cbb + b0_4;
	   Cab = 3*a1_2 + 2*a1*a0 + a0_2; Kab = a1_2 + 2*a1*a0 + 3*a0_2;
	   Caab = a0*Cab + 4*a1_3; Kaab = a1*Kab + 4*a0_3;
	   Cabb = 4*b1_3 + 3*b1_2*b0 + 2*b1*b0_2 + b0_3;
	   Kabb = b1_3 + 2*b1_2*b0 + 3*b1*b0_2 + 4*b0_3;

	   P1 += db*C1;
	   Pa += db*Ca;
	   Paa += db*Caa;
	   Paaa += db*Caaa;
	   Pb += da*Cb;
	   Pbb += da*Cbb;
	   Pbbb += da*Cbbb;
	   Pab += db*(b1*Cab + b0*Kab);
	   Paab += db*(b1*Caab + b0*Kaab);
	   Pabb += da*(a1*Cabb + a0*Kabb);
   }

	P1 /= 2.0f;
	Pa /= 6.0f;
	Paa /= 12.0f;
	Paaa /= 20.0f;
	Pb /= -6.0f;
	Pbb /= -12.0f;
	Pbbb /= -20.0f;
	Pab /= 24.0f;
	Paab /= 60.0f;
	Pabb /= -60.0f;
 
}

void HK_CALL InternalInertiaTensorComputer::compFaceIntegrals(const hkVector4* v,const hkVector4& n)
{
	hkReal w;
	hkReal k1, k2, k3, k4;

	compProjectionIntegrals(v);

	w = - n(X) * v[0](X)
		- n(Y) * v[0](Y)
		- n(Z) * v[0](Z);
 
	k1 = 1 / n(C); k2 = k1 * k1; k3 = k2 * k1; k4 = k3 * k1;

	Fa = k1 * Pa;
	Fb = k1 * Pb;
	Fc = -k2 * (n(A)*Pa + n(B)*Pb + w*P1);

	Faa = k1 * Paa;
	Fbb = k1 * Pbb;
	Fcc = k3 * (sqr(n(A))*Paa + 2*n(A)*n(B)*Pab + sqr(n(B))*Pbb
			 + w*(2*(n(A)*Pa + n(B)*Pb) + w*P1));

	Faaa = k1 * Paaa;
    Fbbb = k1 * Pbbb;
    Fccc = -k4 * (cube(n(A))*Paaa + 3*sqr(n(A))*n(B)*Paab 
			+ 3*n(A)*sqr(n(B))*Pabb + cube(n(B))*Pbbb
			+ 3*w*(sqr(n(A))*Paa + 2*n(A)*n(B)*Pab + sqr(n(B))*Pbb)
			+ w*w*(3*(n(A)*Pa + n(B)*Pb) + w*P1));
 
	Faab = k1 * Paab;
	Fbbc = -k2 * (n(A)*Pabb + n(B)*Pbbb + w*Pbb);
	Fcca = k3 * (sqr(n(A))*Paaa + 2*n(A)*n(B)*Paab + sqr(n(B))*Pabb
			+ w*(2*(n(A)*Paa + n(B)*Pab) + w*Pa));

 
}

void HK_CALL InternalInertiaTensorComputer::compVolumeIntegrals(const hkGeometry* geom,const hkVector4& shift)
{
	//InternalInertiaTensorComputer::Face *f;
	hkReal nx, ny, nz;
	int i;
	hkVector4 normal;
	hkVector4 verts[3];
	long numFaces;
	hkTriangleIndices tindices;
	hkVector4 sub1;
	hkVector4 sub2;	
	numFaces = geom->getNumTriangles(); 
	hkReal area,invlength;

	T0 = T1[X] = T1[Y] = T1[Z] 
       = T2[X] = T2[Y] = T2[Z] 
       = TP[X] = TP[Y] = TP[Z] = 0;

	for (i = 0; i < numFaces; i++) 
	{
		tindices = geom->getTriangle(i);
		verts[0].setAdd4(geom->getVertex(tindices.getVertex(0)),shift);
		verts[1].setAdd4(geom->getVertex(tindices.getVertex(1)),shift);
		verts[2].setAdd4(geom->getVertex(tindices.getVertex(2)),shift);
		sub1.setSub4(verts[1],verts[0]);
		sub2.setSub4(verts[2],verts[0]);
		normal.setCross(sub1,sub2);
		
		area = normal.length3();

		if (area > 0)	// ignore zero area faces
		{

			// okay this is twice the area...but we don't care
			invlength = 1.f/area;
			normal.mul4(invlength);
			
			nx = hkMath::fabs(normal(X));
			ny = hkMath::fabs(normal(Y));
			nz = hkMath::fabs(normal(Z));
			if (nx > ny && nx > nz) C = X;
			else C = (ny > nz) ? Y : Z;
			A = (C + 1) % 3;
			B = (A + 1) % 3;
		
			compFaceIntegrals(verts,normal);
			
			T0 += normal(X) * ((A == X) ? Fa : ((B == X) ? Fb : Fc));

			T1[A] += normal(A) * Faa;
			T1[B] += normal(B) * Fbb;
			T1[C] += normal(C) * Fcc;
			T2[A] += normal(A) * Faaa;
			T2[B] += normal(B) * Fbbb;
			T2[C] += normal(C) * Fccc;
			TP[A] += normal(A) * Faab;
			TP[B] += normal(B) * Fbbc;
			TP[C] += normal(C) * Fcca;
		}
	}


    T1[X] /= 2; T1[Y] /= 2; T1[Z] /= 2;
    T2[X] /= 3; T2[Y] /= 3; T2[Z] /= 3;
    TP[X] /= 2; TP[Y] /= 2; TP[Z] /= 2;

#if defined(__MWERKS__)
#pragma optimization_level 0
#endif

}

#if defined(__MWERKS__)
#pragma optimization_level 4
#endif


void HK_CALL InternalInertiaTensorComputer::computeInertialTensorInternal(hkReal mass, hkReal density,
												   hkVector4 &r, hkMatrix3 &J)
{
	/* compute center of mass */
	r(X) = T1[X] / T0;
	r(Y) = T1[Y] / T0;
	r(Z) = T1[Z] / T0;

	J(X,X) = density * (T2[Y] + T2[Z]);
	J(Y,Y) = density * (T2[Z] + T2[X]);
	J(Z,Z) = density * (T2[X] + T2[Y]);
	J(X,Y) = J(Y,X) = - density * TP[X];
	J(Y,Z) = J(Z,Y) = - density * TP[Y];
	J(Z,X) = J(X,Z) = - density * TP[Z];

	/* translate inertia tensor to center of mass */
	J(X,X) -= mass * (r(Y)*r(Y) + r(Z)*r(Z));
	J(Y,Y) -= mass * (r(Z)*r(Z) + r(X)*r(X));
	J(Z,Z) -= mass * (r(X)*r(X) + r(Y)*r(Y));
    J(X,Y) = J(Y,X) += mass * r(X) * r(Y); 
    J(Y,Z) = J(Z,Y) += mass * r(Y) * r(Z); 
    J(Z,X) = J(X,Z) += mass * r(Z) * r(X); 
}

#undef X
#undef Y
#undef Z


// This computes a shift which will ensure that the geometry is "roughly" centered
// around the origin. This enables us to do the computations on a the "shifted" 
// geometry, and avoid bad numerical roundoff errors.
hkVector4 HK_CALL InternalInertiaTensorComputer::compShift(const hkGeometry* geom)
{

	hkVector4 avg;
	avg.setZero4();

	int numv = geom->getNumVertices();
	hkVector4 min;
	min.setAll(hkMath::HK_REAL_MAX);
	hkVector4 max;
	max.setAll(-hkMath::HK_REAL_MAX);
	for(int i = 0; i< numv; i++)
	{
	    hkVector4 vert = geom->getVertex(i);
		min.setMin4(vert, min);
		max.setMax4(vert, max);
	}

	if(numv != 0)
	{		
		avg.setAdd4(min, max);
		avg.mul4(0.5f);
	}


	avg.setNeg4(avg);
	
	return avg;
}



/// WARNING: This function assumes the geometry is properly closed.
hkResult HK_CALL hkInertiaTensorComputer::computeGeometryVolumeMassProperties(const hkGeometry* geom, hkReal mass, hkMassProperties &result)
{
	HK_ASSERT2( mass > 0.0f, "Cannot calculate mass properties with zero mass or less." );

	if (mass <= 0)
	{
		return HK_FAILURE;
	}

	// First calculate a shift to add to the vertices, to "recenter" them
	
	hkVector4 shift = InternalInertiaTensorComputer::compShift(geom);
	InternalInertiaTensorComputer::compVolumeIntegrals(geom,shift);
	
	hkReal vol = InternalInertiaTensorComputer::T0;
    hkReal density = mass/vol;

	result.m_volume = vol;

	hkVector4 r;
	hkMatrix3 J;
	
	InternalInertiaTensorComputer::computeInertialTensorInternal(mass, density, r, J);

	// Must subtract shift (since we forced readPolyhedron to construct genetry data with
	// shift added to each vertex, hence VOL, IT are the same, but C.O.M for the "unshifted"
	// geometry is equal to C.O.M(shifted) - "shift".
	// This has NOTHING to do with any body/local/world transforms.
	r.setSub4(r, shift);

	// When we decide to perform this calculation in body space, we'll be able to use the lines below
	// to map to local.
	result.m_centerOfMass = r;
	result.m_inertiaTensor = J;
	result.m_mass = mass;

	return HK_SUCCESS;
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
