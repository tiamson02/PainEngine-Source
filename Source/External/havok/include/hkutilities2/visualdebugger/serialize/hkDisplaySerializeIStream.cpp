/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//
// Havok 2.0 Visual Debug Viewer Display Serialization
//
#include <hkbase/hkBase.h>
#include <hkmath2/hkMath.h>

#include <hkutilities2/visualdebugger/serialize/hkDisplaySerializeIStream.h>

// Definitions of complex types
#include <hkgeometry2/hkTriangleIndices.h>
#include <hkgeometry2/hkGeometry.h>
#include <hkgeometry2/display/hkDisplayConvex.h>
#include <hkgeometry2/display/hkDisplaySphere.h>
#include <hkgeometry2/display/hkDisplayBox.h>
#include <hkgeometry2/display/hkDisplayAABB.h>
#include <hkgeometry2/display/hkDisplayCone.h>
#include <hkgeometry2/display/hkDisplaySemiCircle.h>
#include <hkgeometry2/display/hkDisplayPlane.h>
#include <hkgeometry2/display/hkDisplayCapsule.h>

hkDisplaySerializeIStream::hkDisplaySerializeIStream(hkStreambuf* ioStreamBuf)
	: hkIArchive(ioStreamBuf)
{
}

void hkDisplaySerializeIStream::readQuadVector4(hkVector4& v)
{
//	readFloats(&v(0), 4);
	readFloats(&v(0), 3);
	v(3) = 0.0f;	// initialize element that is not transmitted
}

//	void hkDisplaySerializeIStream::readRotation(hkRotation& r)
//	{
//		readFloats(&r.getColumn(0)(0), 12);
//	}

void hkDisplaySerializeIStream::readTransform(hkTransform& t)
{
//	readFloats(&t.getRotation().getColumn(0)(0), 16);

//*
	// get the position
	hkVector4 position;
	readFloats(&position(0), 3);
	position(3) = 1.0f;

	// get the orientation
	hkQuaternion orientation;
	readFloats(const_cast<hkReal*>(&orientation(0)), 4);

	// setup the transform
	t.setTranslation(position);
	t.setRotation(orientation);
//*/
}

void hkDisplaySerializeIStream::readTriangleIndices(hkTriangleIndices& ti)
{
	ti.setVertex(0, read32());
	ti.setVertex(1, read32());
	ti.setVertex(2, read32());
}

void hkDisplaySerializeIStream::readGeometry(hkGeometry& g)
{
	// extract the array of vertices
	{
		int numVertices = read32();

		hkVector4* vertices = hkAllocate<hkVector4>(numVertices, HK_MEMORY_CLASS_GEOMETRY);
		for(int i = 0; i < numVertices; i++)
		{
			readQuadVector4(vertices[i]);
		}

		g.addVertices(vertices, numVertices);
		hkDeallocate<hkVector4>(vertices);
	}

	// extract the array of triangle indicies
	{
		int numTriangles = read32();

		hkTriangleIndices* triangleIndices = new hkTriangleIndices[numTriangles];
		for(int i = 0; i < numTriangles; i++)
		{
			readTriangleIndices(triangleIndices[i]);
		}

		g.addTriangles(triangleIndices, numTriangles);
		delete[] triangleIndices;
	}

}

void hkDisplaySerializeIStream::readDisplayGeometry(hkDisplayGeometry*& dg)
{
	// grab the type information
	char t_asChar = read8();
	hkDisplayGeometryType t = (hkDisplayGeometryType)t_asChar;

	// for grabbing the transform
	hkTransform transform; 


	// grab the specific information partaining to this geometry
	switch(t)
	{
		case HK_DISPLAY_CONVEX:
			{
				readTransform(transform);
				// read in the geometry
				hkGeometry* geometry = new hkGeometry();
				readGeometry(*geometry);

				dg = new hkDisplayConvex(geometry);
				dg->getTransform() = transform;
			}
			break;

		case HK_DISPLAY_SPHERE:
			{
				readTransform(transform);
				// read in the sphere and the x and y tesslation resolutions
				hkReal radius = readFloat32();

				hkVector4 position;	
				readQuadVector4(position);
				hkSphere sphere(position, radius);

				int xRes = read32();
				int yRes = read32();

				dg = new hkDisplaySphere(sphere, xRes, yRes);
				dg->getTransform() = transform;
			}
			break;

		case HK_DISPLAY_CAPSULE:
			{
				readTransform(transform);
				hkReal radius = readFloat32();

				hkVector4 top;
				readQuadVector4( top );
				hkVector4 bottom;
				readQuadVector4( bottom );

				int numSides = read32();
				int numHeightSegments = read32();

				dg = new hkDisplayCapsule( top, bottom, radius, numSides, numHeightSegments );
				dg->getTransform() = transform;
			}
			break;

		case HK_DISPLAY_BOX:
			{
				readTransform(transform);
				hkVector4 halfExtents; readQuadVector4(halfExtents);

				dg = new hkDisplayBox(halfExtents);
				dg->getTransform() = transform;
			}
			break;

		case HK_DISPLAY_AABB:
			{
				// no transform for this type
				hkVector4 minExtent; 
				hkVector4 maxExtent; 
				readQuadVector4(minExtent);
				readQuadVector4(maxExtent);
				dg = new hkDisplayAABB(minExtent, maxExtent);
			}
			break;

		case HK_DISPLAY_CONE:
			{
				hkVector4 position;
				hkVector4 axis;
				hkReal angle;
				hkReal height;
				int numSegments;

				readQuadVector4(position);
				readQuadVector4(axis);
				angle = readFloat32();
				height = readFloat32();
				numSegments = read32();

				dg = new hkDisplayCone(angle,height,numSegments,axis,position);
			}
			break;
	
		case HK_DISPLAY_SEMICIRCLE:
			{
				hkDisplaySemiCircle* dsemi = static_cast<hkDisplaySemiCircle*>(dg);
				hkVector4 center;
				hkVector4 normal;
				hkVector4 perp;
				hkReal radius;
				hkReal thetaMin;
				hkReal thetaMax;
				int numSegments;

				readQuadVector4(center);
				readQuadVector4(normal);
				readQuadVector4(perp);
				radius = readFloat32();
				thetaMin = readFloat32();
				thetaMax = readFloat32();
				numSegments = read32();
				
				dg = new hkDisplaySemiCircle(center, normal, perp, 
											 thetaMin, thetaMax, radius,
											 numSegments);
			}
			break;

		case HK_DISPLAY_PLANE:
			{
				hkDisplayPlane* dplane = static_cast<hkDisplayPlane*>(dg);
				hkVector4 center;
				hkVector4 normal;
				hkVector4 perpToNormal;
				hkReal extent;

				readQuadVector4(center);
				readQuadVector4(normal);
				readQuadVector4(perpToNormal);
				extent = readFloat32();
				
				dg = new hkDisplayPlane(normal, perpToNormal, center, extent);
			}
			break;

		default:
			HK_ASSERT2(0, "Display stream corrupt or unsupported display geometry revieved!");
	}
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
