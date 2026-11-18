/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>

#include <hkmath/basetypes/hkSphere.h>

#include <hkexport2/exportgeometricprimitive.h>
#include <hkexport2/exportrigidbody.h>
#include <hkexport2/exportworld.h>
#include <hkexport2/exportrigidbodycollection.h>
#include <hkexport2/exportmopprepository.h>

#include <hkdynamics/entity/hkRigidBody.h>
 
#include <hkcollide/shape/box/hkBoxShape.h>
#include <hkcollide/shape/capsule/hkCapsuleShape.h>
#include <hkcollide/shape/cylinder/hkCylinderShape.h>
#include <hkcollide/shape/convexvertices/hkConvexVerticesShape.h>
#include <hkcollide/shape/mesh/hkMeshShape.h>
#include <hkinternal/collide/mopp/code/hkMoppCode.h>
#include <hkcollide/shape/mopp/hkMoppUtility.h>
#include <hkcollide/shape/mopp/hkMoppBvTreeShape.h>

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

// Import&Export
void ExportGeometricPrimitive::Import (InExportStream &xtream, CheckLog &log)
{
	xtream>>prot_buffer; SetName(prot_buffer);
	log.BeginLevel(LOC("GEOMETRIC PRIMITIVE"),GetName());

	CommonPreImport(xtream, log);

	xtream.TokenMustBe("GEOMETRY");

	xtream>>prot_buffer;

	// make it a shape here 
	ExportGeometry* geometry = GetRigidBody()->GetRigidBodyCollection()->GetSubspace()->GetWorld()->GetGeometry(prot_buffer);
	if (!geometry) throw_exception("Unknown geometry used in Geometric Primitive");

	hkBool isConvex = false;

	if (xtream.GetVersion()>=140)
	{
		xtream.TokenMustBe("CONVEX"); xtream>>isConvex;
	}

	//search for movable concave primitives
	if ((!isConvex) && (prot_mass > 0))
	{
		char buffer[200];
		hkString::sprintf(buffer, LOC("Concave objects with mass are unsupported; this primitive will be simulated as a convex mesh."));
		log.AddError(buffer);
		isConvex = true;
	}

	ExpGeometryType geometryType;
	{
		// if the HKE is old, we don't have information about the geometry type
		// so we have to figure it out
		if (xtream.GetVersion()<2000)
		{
			if (!isConvex)
			{
				geometryType = GT_MESH;
			}
			else
			{
				if (geometry->getBoxShape()!=HK_NULL)
				{
					geometryType = GT_BOX;
				}
				else
				{
					geometryType = GT_HULL;
				}
			}
		}
		else
		{
			geometryType = geometry->getGeometryType();
		}
	}

	switch (geometryType)
	{
		case GT_MESH:
			prot_shape = geometry->getConcaveMeshShape();
			prot_shape->addReference();
			break;

		case GT_HULL:
			prot_shape = geometry->getConvexHullShape();
			prot_shape->addReference();
			break;

		case GT_BOX:
			prot_shape = geometry->getBoxShape();
			prot_shape->addReference();
			prot_transform.setMulEq( geometry->getShapeTransform() );
			break;
		
		case GT_CAPSULE:
			prot_shape = geometry->getCapsuleShape();
			prot_shape->addReference();
			prot_transform.setMulEq( geometry->getShapeTransform() );
			break;

		case GT_CYLINDER:
			prot_shape = geometry->getCylinderShape();
			prot_shape->addReference();
			prot_transform.setMulEq( geometry->getShapeTransform() );
			break;

		default:
			{
				char buffer[200];
				hkString::sprintf(buffer, LOC("Unknown geometry type") );
				log.AddError(buffer);
				break;
			}
	}

	CommonPostImport(xtream, log);

	xtream.TokenMustBe("END_PRIMITIVE");

	log.EndLevel();
}

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
