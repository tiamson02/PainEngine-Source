/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>

#include <hkexport2/exportgeometricprimitive.h>
#include <hkexport2/exportrigidbody.h>
#include <hkexport2/exportworld.h>
#include <hkexport2/exportrigidbodycollection.h>
#include <hkexport2/exportmopprepository.h>

#include <hkgeometry2/hkGeometry.h>
#include <hkgeometry2/hkSphere.h>
#include <hkgeometryutil2/hkGeometryChecker.h>
#include <hkgeometryutil2/hkGeometryUtil.h>

#include <hkdynamics2/entity/hkRigidBody.h>
 
#include <hkcollide2/shape/hkBoxShape.h>
#include <hkcollide2/shape/hkConvexVerticesShape.h>
#include <hkcollide2/shape/hkMeshShape.h>
#include <hkcollide2/algorithms/mopp/code/hkMoppCode.h>
#include <hkcollide2/algorithms/mopp/utility/hkMoppUtility.h>
#include <hkcollide2/shape/hkMoppBvTreeShape.h>
#include <hkcollide2/algorithms/mopp/utility/hkMoppFitToleranceRequirements.h>

using namespace NS_ExportStream;

// FACTORY ----------------------------------------------------------------------------------------------

ExportPrimitive *ExportGeometricPrimitiveFactory::CreateExportPrimitive(Token &token, ExportRigidBody *rigid_body, CheckLog &log)
{
	if (token=="GeometricPrimitive") return new ExportGeometricPrimitive(rigid_body);
	return HK_NULL;
}

EXPORT_SINGLETON_IMPLEMENTATION(ExportGeometricPrimitiveFactory);
// ------------------------------------------------------------------------------------------------------

int ExportGeometricPrimitive::priv_CLASS_SerialNumber=1;

// Constructor
ExportGeometricPrimitive::ExportGeometricPrimitive (ExportRigidBody *rigid_body) : ExportPrimitive(rigid_body)
{
	PROT_AutoName();
	priv_CLASS_SerialNumber++;
}

// Destructor
ExportGeometricPrimitive::~ExportGeometricPrimitive()
{ 
}

static bool approxEquals(const hkReal&  a, const hkReal& b, hkReal tolerance = .001f)
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

// Import&Export
void ExportGeometricPrimitive::Import (InExportStream &xtream, CheckLog &log)
{
	xtream>>prot_buffer; SetName(prot_buffer);
	log.BeginLevel(LOC("GEOMETRIC PRIMITIVE"),GetName());

	CommonPreImport(xtream, log);

	xtream.TokenMustBe("GEOMETRY");

	xtream>>prot_buffer;

	// make it a shape here 
	hkStorageMeshShape *geom = GetRigidBody()->GetRigidBodyCollection()->GetSubspace()->GetWorld()->GetGeometry(prot_buffer)->GetHKStorageMeshShape();
	source_shape = geom;
	
	
	if (!geom) throw_exception("Unknown geometry used in Geometric Primitive");

	bool shape_convexity = false;

	if (xtream.GetVersion()>=140)
	{
		xtream.TokenMustBe("CONVEX"); xtream>>shape_convexity;
	}

	// Search for movable concave primitives, to give warning and set some volume
	if ((!shape_convexity) && (prot_mass > 0))
	{
		char buffer[200];
		// We give the warning
		hkString::sprintf(buffer, LOC("Warning : Movable concave primitive. Notice that collisions between concave objects are not supported"));
		// We set the radius for the triangles; otherwise volume is zero and intertial tensor calculations would fail
		geom->setRadius(0.01f);
		log.AddError(buffer);
	}

	//this checks to see if the geometry is a box and if so creates a box shape
	//which is faster than a convex/concave shape
	hkTransform geomFromBox;
	hkVector4 half_extents;
	bool is_box = getIsBoxGeometry( geom, half_extents, geomFromBox );

	hkMoppCode* preCreatedMoppCode = ExportPreCreatedMoppRepository::getInstance().getPreCreatedMoppCode( prot_name );

	if ( !shape_convexity && ( preCreatedMoppCode != HK_NULL ) )
	{
		prot_shape = new hkMoppBvTreeShape( geom, preCreatedMoppCode );
		// we only allow a one to one mapping between names and mopps so no other primitive can use the mopp 
		// and we don't want the repository to hold a ref to the mopp code after it's served its purpose
		ExportPreCreatedMoppRepository::getInstance().removePreCreatedMoppCode( prot_name );
	}
	else
	{
		if (is_box)
		{
			prot_shape = new hkBoxShape( half_extents);
			prot_transform.setMulEq( geomFromBox );
		}
		else
		{
			if (shape_convexity) 
			{
				// cycle through all the information in the hkgeometry and 
				// then propagate the convexShape

				const int numVertices = geom->getVertices().getSize();
				hkReal* vertices = hkAllocate<hkReal>(numVertices * 3,HK_MEMORY_CLASS_EXPORT);
				
				hkVector4 temp;
				int count = 0;
				for(int i = 0; i < numVertices * 3; count++)
				{
					temp = geom->getVertices()[count];
					vertices[i++] = temp(0);
					vertices[i++] = temp(1);
					vertices[i++] = temp(2);
				}

				hkArray<hkVector4> planeEquations;
				hkArray<hkVector4> usedVertices;
				hkGeometryUtil::computePlanesFromVertices(vertices, numVertices, 3 * sizeof(float), planeEquations, usedVertices);
				hkConvexVerticesShape *geom = new hkConvexVerticesShape(vertices, 3 * sizeof(float), numVertices, planeEquations);
				geom->setRadius( 0.01f );
				prot_shape = geom;

				// delete the vertices
				hkDeallocate<hkReal>(vertices);
			}
			else // if the body is concave then fill up the 
			{
				hkMoppCode* code = hkMoppUtility::buildCode(geom, hkMoppFitToleranceRequirements());
				prot_shape = new hkMoppBvTreeShape(geom, code);
				code->removeReference();
			}
		}
	}
	
	CommonPostImport(xtream, log);

	xtream.TokenMustBe("END_PRIMITIVE");

	log.EndLevel();
}

bool ExportGeometricPrimitive::getIsBoxGeometry( const hkStorageMeshShape* geom, hkVector4& half_extents, hkTransform& boxToGeom, const hkReal epsilon )
{
	//checks if the verts are a convex box containing all verts and if so it returns true and the extents of the box and the transform
	if (geom->getVertices().getSize() != 8)
	{
		return false;
	}

	hkVector4 point_radius;
	hkReal point_error = 0.0f;
	hkSphere min_bounding_sphere = hkGeometryUtil::createMinBoundingSphere( &(geom->getVertices()[0]), geom->getVertices().getSize());
	hkVector4 sphere_center = min_bounding_sphere.getPosition();
	hkReal sphere_radius = min_bounding_sphere.getRadius();
	hkArray< hkVector4 > box_points (8);
	hkArray< hkVector4 > possible_points;

	int i=0;

	const hkArray<hkVector4>& vertices = geom->getVertices();
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

	half_extents(0) = xMax - xMin;
	half_extents(1) = yMax - yMin;
	half_extents(2) = zMax - zMin;
	half_extents.mul4(.5f);
	hkVector4 centre((xMax + xMin) * .5f, (yMax + yMin) * .5f, (zMax + zMin) * .5f);
	centre.mul4( -1 );
	boxToGeom.setInverse( hkTransform(rot, centre));
	return true;

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
