/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>

#include <hkinternal/preprocess/convexhull/hkGeometryUtility.h>

#include <hkexport2/exportinlinegeometry.h>
#include <hkexport2/exportgeometricprimitive.h>
#include <hkexport2/exportmopprepository.h>
#include <hkexport2/exportrigidbody.h>
#include <hkexport2/exportworld.h>

#include <hkmath/basetypes/hkGeometry.h>

#include <hkdynamics/entity/hkRigidBody.h>
#include <hkcollide/shape/box/hkBoxShape.h>
#include <hkcollide/shape/capsule/hkCapsuleShape.h>
#include <hkcollide/shape/cylinder/hkCylinderShape.h>
#include <hkcollide/shape/simplemesh/hkSimpleMeshShape.h>
#include <hkcollide/shape/convexvertices/hkConvexVerticesShape.h>
#include <hkcollide/shape/mesh/hkMeshShape.h>
#include <hkinternal/collide/mopp/code/hkMoppCode.h>
#include <hkcollide/shape/mopp/hkMoppUtility.h>
#include <hkcollide/shape/mopp/hkMoppBvTreeShape.h>
#include <hkcollide/util/hkAabbUtil.h>

using namespace NS_ExportStream;

// FACTORY ----------------------------------------------------------------------------------------------

EXPORT_SINGLETON_IMPLEMENTATION(ExportInlineGeometryFactory);

ExportGeometry* ExportInlineGeometryFactory::CreateExportGeometry(Token &token, ExportWorld *world)
{
	if (token=="Inline") return new ExportInlineGeometry(world);
	return HK_NULL;
}
// ------------------------------------------------------------------------------------------------------

ExportInlineGeometry::ExportInlineGeometry(ExportWorld *world) : ExportGeometry(world)
{
	
}

// Destructor
ExportInlineGeometry::~ExportInlineGeometry()
{

}


#if !defined( HK_PLATFORM_PSP )

static inline hkBool approxEquals(const hkReal&  a, const hkReal& b, hkReal tolerance = .001f)
{
	if ( hkMath::fabs(a - b) < tolerance )
	{
		return true;
	}
	else
	{
		return false;
	}
} 

#else

// the R3000 compiler produces an internal error when inlining the function below
// so we'll force it to be out of line.

static hkBool approxEquals(const hkReal&  a, const hkReal& b, hkReal tolerance = .001f);

#endif

void ExportInlineGeometry::Import(InExportStream &xtream, CheckLog &log)
{
	// Name
	xtream>>prot_buffer; SetName(prot_buffer);

	m_shapeTransform.setIdentity();

	const hkConvertCS& converter = GetWorld()->getCSConverter();

	int numVerts, numFaces;

	xtream>>numVerts;

	hkGeometry geom;
	geom.m_vertices.setSize( numVerts );
	
	for (int vert_it=0; vert_it<numVerts; vert_it++)
	{
		hkVector4 vert;
		xtream>>vert;

		converter.convertVector(vert);
		geom.m_vertices[vert_it]=vert;
	}

	xtream>>numFaces;

	geom.m_triangles.setSize( numFaces );

	for (int i = 0; i < numFaces; i++ )
	{
		xtream >> geom.m_triangles[i].m_a;
		xtream >> geom.m_triangles[i].m_b;
		xtream >> geom.m_triangles[i].m_c;
	}

	// Loading of geometry type
	if (xtream.GetVersion()>=2000)
	{
		Token geomTypeToken;
		xtream.TokenMustBe("TYPE"); xtream>>geomTypeToken;

		if (geomTypeToken=="MESH")
		{
			m_geometryType = GT_MESH;
		}
		else if (geomTypeToken=="HULL")
		{
			m_geometryType = GT_HULL;
		}
		else if (geomTypeToken=="BOX")
		{
			m_geometryType = GT_BOX;
		}
		else if (geomTypeToken=="CAPSULE")
		{
			m_geometryType = GT_CAPSULE;
		}
		else if (geomTypeToken=="CYLINDER")
		{
			m_geometryType = GT_CYLINDER;
		}
		else
		{
			log.AddError(LOC("Unkown Geometry Type importing geometry"));
			m_geometryType = GT_HULL;
		}
	}

	// We only create all geometry types prior to version 2000, after this we know the geometry type we need to create

	/*
	** Creation on concave shape
	*/
	if( (m_geometryType == GT_MESH) || (xtream.GetVersion() < 2000) )
	{
		m_storageMeshShape = new hkSimpleMeshShape(/*radius*/);
		m_storageMeshShape->m_vertices = geom.m_vertices;
		int numT = geom.m_triangles.getSize();
		m_storageMeshShape->m_triangles.setSize( numT );
		for (int ti=0; ti < numT; ++ti)
		{
			m_storageMeshShape->m_triangles[ti].m_a = geom.m_triangles[ti].m_a;
			m_storageMeshShape->m_triangles[ti].m_b = geom.m_triangles[ti].m_b;
			m_storageMeshShape->m_triangles[ti].m_c = geom.m_triangles[ti].m_c;
		}

		hkMoppCode* moppCode = HK_NULL;

		// We check if that Mopp code has been precreated
		moppCode = ExportPreCreatedMoppRepository::getInstance().getPreCreatedMoppCode( prot_name );

		// Otherwise we create it
		if (!moppCode)
		{
			// Usually Mopps are not built at run time but preprocessed instead. We disable the performance warning
			hkBool wasEnabled = hkError::getInstance().isEnabled(0x6e8d163b); 
			hkError::getInstance().setEnabled(0x6e8d163b, false); // hkMoppUtility.cpp:18
			moppCode = hkMoppUtility::buildCode(m_storageMeshShape, hkMoppFitToleranceRequirements());
			hkError::getInstance().setEnabled(0x6e8d163b, wasEnabled);
		}
		else
		{
			moppCode->addReference();
			// we only allow a one to one mapping between names and mopps so no other primitive can use the mopp 
			// and we don't want the repository to hold a ref to the mopp code after it's served its purpose
			ExportPreCreatedMoppRepository::getInstance().removePreCreatedMoppCode( prot_name );
		}

		HK_ASSERT2(0x67be1428, moppCode, "Mopp creation failed");

		m_moppBvTreeShape = new hkMoppBvTreeShape(m_storageMeshShape, moppCode);
		moppCode->removeReference();

//XXNAMES:		m_moppBvTreeShape->setName(GetSerializableName().cString());
	}

	/*
	** Convex vertices shape
	*/
	if( (m_geometryType == GT_HULL) || (xtream.GetVersion() < 2000) )
	{
		// cycle through all the information in the hkgeometry and 
		// then propagate the convexShape

		hkArray<hkVector4> planeEquations;
		hkGeometry usedGeom;
		// Usually plane equations should be precomputed off line. We disable the performance warning

		hkStridedVertices stridedVerts;
		{
			stridedVerts.m_numVertices = geom.m_vertices.getSize();
			stridedVerts.m_striding = sizeof(hkVector4);
			stridedVerts.m_vertices = &(geom.m_vertices[0](0));
		}

		// Usually plane equations should be precomputed off line. We disable the performance warning
		hkBool wasEnabled = hkError::getInstance().isEnabled(0x34df5494);
		hkError::getInstance().setEnabled(0x34df5494, false); //hkGeometryUtility.cpp:26
		hkGeometryUtility::createConvexGeometry( stridedVerts, usedGeom, planeEquations );
		hkError::getInstance().setEnabled(0x34df5494, wasEnabled); //hkGeometryUtility.cpp:26

		{
			stridedVerts.m_vertices = &(usedGeom.m_vertices[0](0));
			stridedVerts.m_striding = sizeof(hkVector4);
			stridedVerts.m_numVertices = usedGeom.m_vertices.getSize();
		}
		m_convexVerticesShape = new hkConvexVerticesShape( stridedVerts, planeEquations);

		// HVK-1344 : Fixing issue for new scenes ; old HKEs will behave as before
		if (xtream.GetVersion()<2000)
		{
			m_convexVerticesShape->setRadius( 0.01f );
		}

//XXNAMES:		m_convexVerticesShape->setName(GetSerializableName().cString());
	}

	/*
	** Box shape (if possible)
	*/
	if( (m_geometryType == GT_BOX) || (xtream.GetVersion() < 2000) )
	{
		hkVector4 halfExtents;
		const hkBool isBox = getOrOtherwiseForceBoxGeometry(geom.m_vertices, halfExtents, m_shapeTransform);

		if (isBox)
		{
			m_boxShape = new hkBoxShape( halfExtents );
//XXNAMES:			m_boxShape->setName(GetSerializableName().cString());
		}
	}

	/*
	** Capsule shape (if possible)
	** Capsules can only be created from box geometries
	*/
	if( (m_geometryType == GT_CAPSULE) || (xtream.GetVersion() < 2000) )
	{
		hkVector4 halfExtents;
		const hkBool isBox = getOrOtherwiseForceBoxGeometry(geom.m_vertices, halfExtents, m_shapeTransform);

		if (isBox)
		{
			int mainAxis = -1;
			hkReal longest=-1.0f;
			hkReal secondLongest=-1.0f;

			for (int axis=0; axis<3; axis++)
			{
				if (halfExtents(axis) > longest)
				{
					secondLongest=longest;
					longest = halfExtents(axis);
					mainAxis = axis;
				}
				else
				{
					if (halfExtents(axis) > secondLongest)
					{
						secondLongest = halfExtents(axis);
					}
				}
			}

			hkVector4 pointA; pointA.setZero4( );
			hkVector4 pointB; pointB.setZero4( );

			hkReal dist = hkMath::max2( longest-secondLongest, hkMath::HK_REAL_EPSILON );
			pointA(mainAxis) = dist;
			pointB(mainAxis) = -pointA(mainAxis);

			m_capsuleShape = new hkCapsuleShape( pointA,pointB,secondLongest );
//XXNAMES:		m_capsuleShape->setName(GetSerializableName().cString());
		}
	}

	if( (m_geometryType == GT_CYLINDER) || (xtream.GetVersion() < 2000) )
	{
		hkVector4 pointA;
		hkVector4 pointB;
		hkReal radius;
		findBestCylinder(geom.m_vertices, pointA, pointB, radius);

		m_cylinderShape = new hkCylinderShape( pointA,pointB, radius );

		m_shapeTransform.setIdentity();
	}

	xtream.TokenMustBe("END_GEOMETRY");
}

// This is used by the method below
static bool isMultOfXYorZ(const hkVector4& v, const hkReal epsilon)
{
	hkVector4 vabs( hkMath::fabs(v(0)), hkMath::fabs(v(1))  , hkMath::fabs(v(2)) );
	hkReal max = hkMath::max2(vabs(0) , vabs(1));
	max =  hkMath::max2(vabs(2) , max);

	return ( hkMath::fabs(max - 1.0f) < epsilon );
}


hkBool ExportInlineGeometry::getIsBoxGeometry( const hkArray<hkVector4>& vertices, hkVector4& halfExtents, hkTransform& boxToGeom, const hkReal epsilon )
{ 
	//checks if the verts are a convex box containing all verts and if so it returns true and the extents of the box and the transform
	if (vertices.getSize() != 8)
	{
		return false;
	}

	hkVector4 point_radius;
	hkReal point_error = 0.0f;
	
	hkSphere min_bounding_sphere;
	{
		hkArray<hkVector4> planeEquations;
		hkArray<hkVector4> usedVertices;

		hkStridedVertices stridedVerts;
		{
			stridedVerts.m_numVertices = vertices.getSize();
			stridedVerts.m_striding = sizeof(hkVector4);
			stridedVerts.m_vertices = &(vertices[0](0));
		}

		min_bounding_sphere = hkGeometryUtility::createMinBoundingSphere( stridedVerts );
	}


	hkVector4 sphere_center = min_bounding_sphere.getPosition();
	hkReal sphere_radius = min_bounding_sphere.getRadius();
	hkArray< hkVector4 > box_points (8);
	hkArray< hkVector4 > possible_points;

	int i=0;

	const int numVerts = vertices.getSize();

	for ( i = 0; i < numVerts; i++ )
	{
		point_radius.setSub4( vertices[i], sphere_center);
		point_error = hkMath::fabs( (hkReal(point_radius.length3())) - sphere_radius );

		if ( point_error < epsilon )
		{
			possible_points.pushBack( vertices[i] );
		}
	}
	//if all the points aren't on the surface of the sphere the quit out
	if ( possible_points.getSize() != 8 )
	{
		return false;
	}
	else
	{
		for ( i = 0; i < 8; i++ )
		{
			box_points[i] = possible_points[i];
		}
	}

	hkVector4 axisA( box_points[0] );
	axisA.sub4( hkVector4( box_points[1] ) );

	hkVector4 axisB( box_points[0] );
	axisB.sub4( hkVector4( box_points[2] ) );
	for (i = 3; i < 8; ++i)
	{
		hkVector4 test( box_points[0] );
		test.sub4( box_points[i] );
		if (test.dot3(test) < axisA.dot3(axisA))
		{
			if (axisA.dot3(axisA) < axisB.dot3(axisB))
			{
				axisB = axisA;
			}
			axisA = test;
		}
		else
		{
			if (test.dot3(test) < axisB.dot3(axisB))
			{
				axisB = test;
			}
		}
	}

	//if the axes aren't exactly perpendicular, quit out
	if (axisA.dot3(axisB) > .00001f)
	{
		return false;
	}
	hkVector4 axisC;
	axisC.setCross(axisA, axisB);
	axisA.normalize3();
	axisB.normalize3();
	axisC.normalize3();

	hkRotation rot;
	rot.setCols((hkVector4&)axisA, (hkVector4&)axisB, (hkVector4&)axisC);
	rot.invert(0.01f);
	// We want mat to help rotate these "nasty" vertices to nice "axis-aligned" vertices, hence
	// mat should be the inverse of the one formed by the orthonormal system (axisA,axisB,axisC)
	// since if M = (axisA,axisB,axisC), then M clearly rotates "nice to nasty" eg (1,0,0) ->axisA
	// but we want "nasty-to-nice" !
	for (i = 0; i < 8; i++ )
	{
		(box_points[i]).setRotatedDir( rot, box_points[i] );
	}
	hkReal xMin = box_points[0](0);
	hkReal xMax = box_points[0](0);
	hkReal yMin = box_points[0](1);
	hkReal yMax = box_points[0](1);
	hkReal zMin = box_points[0](2);
	hkReal zMax = box_points[0](2);

	for ( i = 1; i < 8; ++i)
	{
		if (xMin > box_points[i](0))
		{
			xMin = box_points[i](0);
		}
		else if (xMax < box_points[i](0))
		{
			xMax = box_points[i](0);
		}
		
		if (yMin > box_points[i](1))
		{
			yMin = box_points[i](1);
		}
		else if (yMax < box_points[i](1))
		{
			yMax = box_points[i](1);
		}
		if (zMin > box_points[i](2))
		{
			zMin = box_points[i](2);
		}
		else if (zMax < box_points[i](2))
		{
			zMax = box_points[i](2);
		}
	}

	for ( i = 0; i < 8; ++i)
	{
		if ( (approxEquals(box_points[i](0),xMin) || approxEquals(box_points[i](0),xMax)) &&
			 (approxEquals(box_points[i](1),yMin) || approxEquals(box_points[i](1),yMax)) &&
			 (approxEquals(box_points[i](2),zMin) || approxEquals(box_points[i](2),zMax)) )
		{
			continue;
		}
		else
		{
			return false;
		}
	}

	halfExtents(0) = xMax - xMin;
	halfExtents(1) = yMax - yMin;
	halfExtents(2) = zMax - zMin;
	halfExtents.mul4(.5f);
	hkVector4 centre; centre.set((xMax + xMin) * .5f, (yMax + yMin) * .5f, (zMax + zMin) * .5f);
	centre.mul4( -1 );
	boxToGeom.setInverse( hkTransform(rot, centre));

	{
		// If rotation is "canonical" (ie. each axis is close enough to being multiple of X, Y or Z)
		// then replace rotation with identity and permute halfextents. 
		if( isMultOfXYorZ(rot.getColumn(0), epsilon) && isMultOfXYorZ(rot.getColumn(1), epsilon) && isMultOfXYorZ(rot.getColumn(2), epsilon) )
		{
				// Rotate by boxToGeom.getRotation()
				// eg. A 1x2x3 box might come out as T = rotation 90 degrees around Z, and E = (2,1,3). 
				// In this case we set E' = T * E, ie (-1,2,3). Then we take the absolute values of each, since halfextents is always posiitive.

			halfExtents.setRotatedDir(boxToGeom.getRotation(), halfExtents);
			halfExtents(3) = 0.0f;	// since we're going to use setAbs4
			halfExtents.setAbs4(halfExtents);

			rot.setIdentity();
			boxToGeom.setRotation(rot);
		}
	}

	return true;

}

//will call the above, but if it fails, it will create a box anyway
hkBool ExportInlineGeometry::getOrOtherwiseForceBoxGeometry (const hkArray<hkVector4>& vertices, hkVector4& halfExtents, hkTransform& boxToGeom, const hkReal epsilon )
{
	const hkBool niceTryWorked = getIsBoxGeometry(vertices, halfExtents, boxToGeom, epsilon);

	if (niceTryWorked)
	{
		return true;
	}

	// else : force a box from the AABB

	hkAabb aabb;
	hkAabbUtil::calcAabb((float*) vertices.begin(), vertices.getSize(), sizeof(hkVector4), aabb);

	hkVector4 center;
	center.setInterpolate4(aabb.m_max, aabb.m_min, 0.5f);

	boxToGeom.setIdentity();
	boxToGeom.setTranslation(center);

	halfExtents.setSub4(aabb.m_max, aabb.m_min);
	halfExtents.mul4(0.5f);

	return true;
}


//will find a fitting cylinder for the geometry
void ExportInlineGeometry::findBestCylinder (const hkArray<hkVector4>& vertices, hkVector4& pointAout, hkVector4& pointBout, hkReal& radiusOut )
{
	// We start by creating a nice box around the mesh
	hkVector4 halfExtents;
	hkTransform geomFromBox; geomFromBox.setIdentity();

	getOrOtherwiseForceBoxGeometry(vertices, halfExtents, geomFromBox);

	hkTransform boxFromGeom; boxFromGeom.setInverse(geomFromBox);

	// Work on the vertices in "box space"
	hkArray<hkVector4> transformedVerts;
	{
		for (int i=0; i<vertices.getSize(); i++)
		{
			hkVector4 newVert;
			newVert.setTransformedPos(boxFromGeom, vertices[i]);
			transformedVerts.pushBack(newVert);
		}
	}
	
	int mainAxis = -1;
	{

		// For the cylinder, there should be two axis that are mostly equal
		// The other axis is used as the main axis (the one with the biggest difference)
		hkReal maxDiff = -1.0f;
		hkReal radius = -1.0f;
		for (int axis=0; axis<3; axis++)
		{
			const int axis2 = (axis+1) % 3;
			const int axis3 = (axis+2) % 3;
			const hkReal diff1 = hkMath::fabs( halfExtents(axis) - halfExtents(axis2) );
			const hkReal diff2 = hkMath::fabs( halfExtents(axis) - halfExtents(axis3) );
			const hkReal diff = hkMath::min2( diff1,diff2 );
			if ( diff > maxDiff)
			{
				mainAxis = axis;
				maxDiff = diff;
				// We take the radius from the biggest remaining axis
				radius = hkMath::max2( halfExtents(axis2), halfExtents(axis3) );
			}
		}

		// If the chosen axis is not particularly different (less than 10%) from the rest of axis
		// use a different algorithm
		if (hkMath::fabs(1.0f - (halfExtents(mainAxis) / radius) ) < 0.1f)
		{
			// We find the "main axis" by projecting the vertices to the different planes
			// and finding which projection fits a circle/ellipse best
			mainAxis = -1;	
		
			hkReal minAxisDist = 100.0f;
			for (int axis=0; axis<3; axis++)
			{
				const int axisA = (axis+1) %3;
				const int axisB = (axis+2) %3;


				// We scale the values
				const hkReal stretchA = halfExtents(axisA);
				const hkReal stretchB = halfExtents(axisB);

				hkReal thisAxisMaxDist = -1.0f;

				for (int v=0; v<transformedVerts.getSize(); v++)
				{
					const hkVector4& vertex = transformedVerts[v];

					const hkVector4 projectedVertex (vertex(axisA) / stretchA, vertex(axisB) / stretchB, 0.0f);
					
					// We now have a vertex projected and X Y plane, inside the square of [(-1,-1), (1,1)]
					// See how far it is from the circle (we ignore those inside the circle
					const hkReal distance = hkMath::max2(1.0f, hkReal(projectedVertex.length3())) - 1.0f;

					if (distance > thisAxisMaxDist)
					{
						thisAxisMaxDist = distance;
					}
				}

				if (thisAxisMaxDist < minAxisDist)
				{
					minAxisDist = thisAxisMaxDist;
					mainAxis = axis;
				}
			}

		}
	}

	// We now have the main axis, construct the capsule from it
	// We take the radius from the biggest remaining axis
	const int axis2 = (mainAxis+1) % 3;
	const int axis3 = (mainAxis+2) % 3;
	radiusOut = hkMath::max2( halfExtents(axis2), halfExtents(axis3) );
	
	hkVector4 pointAbox; pointAbox.setZero4( );
	hkVector4 pointBbox; pointBbox.setZero4( );

	pointAbox(mainAxis) = halfExtents(mainAxis);
	pointBbox(mainAxis) = -halfExtents(mainAxis);

	// Finally, transform the vertices using the box transform
	pointAout.setTransformedPos(geomFromBox, pointAbox);
	pointBout.setTransformedPos(geomFromBox, pointBbox);

}



#if defined( HK_PLATFORM_PSP )

hkBool approxEquals(const hkReal&  a, const hkReal& b, hkReal tolerance )
{
	if ( hkMath::fabs(a - b) < tolerance )
	{
		return true;
	}
	else
	{
		return false;
	}
} 

#endif


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
