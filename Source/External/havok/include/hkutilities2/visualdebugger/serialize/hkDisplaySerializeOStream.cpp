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

#include <hkutilities2/visualdebugger/hkVisualDebuggerDebugOutput.h>
#include <hkutilities2/visualdebugger/serialize/hkDisplaySerializeOStream.h>

// Definitions of complex types
#include <hkgeometry2/hkTriangleIndices.h>
#include <hkgeometry2/hkGeometry.h>
#include <hkgeometry2/display/hkDisplayGeometry.h>
#include <hkgeometry2/display/hkDisplaySphere.h>
#include <hkgeometry2/display/hkDisplayBox.h>
#include <hkgeometry2/display/hkDisplayAABB.h>
#include <hkgeometry2/display/hkDisplayCone.h>
#include <hkgeometry2/display/hkDisplaySemiCircle.h>
#include <hkgeometry2/display/hkDisplayPlane.h>
#include <hkgeometry2/display/hkDisplayCapsule.h>

hkDisplaySerializeOStream::hkDisplaySerializeOStream(hkStreambuf* ioStreamBuf)
	: hkOArchive(ioStreamBuf)
{
}

void hkDisplaySerializeOStream::writeQuadVector4(const hkVector4& v)
{
//	writeFloats(&v(0), 4);
	writeFloats(&v(0), 3);
}

//	void hkDisplaySerializeOStream::writeRotation(const hkRotation& r)
//	{
//		writeFloats(&r.getColumn(0)(0), 12);
//	}

void hkDisplaySerializeOStream::writeTransform(const hkTransform& t)
{
//	writeFloats(&t.getRotation().getColumn(0)(0), 16);
	
//*
	// get the position
	hkVector4 position = t.getTranslation();

	// get the orientation
	hkRotation rotation = t.getRotation();
	hkQuaternion orientation(rotation);
//	HK_VISUAL_DEBUGGER_INFO(0, "Warning rotation of transform no normalized!");
//	if(orientation.getReal() != 1.0f) this should be done differently

	// send data
	writeFloats(&position(0), 3);
	writeFloats(&orientation(0), 4);
//*/
}

void hkDisplaySerializeOStream::writeTriangleIndices(const hkTriangleIndices& ti)
{
	write32(ti.getVertex(0));
	write32(ti.getVertex(1));
	write32(ti.getVertex(2));
}

void hkDisplaySerializeOStream::writeGeometry(const hkGeometry& g)
{
	// dump out the array of vertices
	{
		const int s = g.getNumVertices();

		write32(s);
		for(int i = 0; i < s; i++)
		{
			writeQuadVector4(g.getVertex(i));
		}
	}

	// dump out the array of triangle indices
	{
		const int s = g.getNumTriangles();

		write32(s);
		for(int i = 0; i < s; i++)
		{
			writeTriangleIndices(g.getTriangle(i));
		}
	}
}

void hkDisplaySerializeOStream::writeDisplayGeometry(hkDisplayGeometry* dg)
{
	// dump the information which defines the given type of display geometry
	const hkDisplayGeometryType t = dg->getType();
	write8(t);

	switch(t)
	{
		case HK_DISPLAY_CONVEX:
			{
				writeTransform(dg->getTransform());
				HK_ASSERT2(dg->getGeometry(), "A display geometry must have a valid geometry pointer!");
				hkGeometry& g = *dg->getGeometry();
				writeGeometry(g);
			}
			break;

		case HK_DISPLAY_SPHERE:
			{
				writeTransform(dg->getTransform());
				hkDisplaySphere* ds = static_cast<hkDisplaySphere*>(dg);
				writeFloat32(ds->getSphere().getRadius());
				writeQuadVector4(ds->getSphere().getPosition());
				write32(ds->getXRes());
				write32(ds->getYRes());
			}
			break;

		case HK_DISPLAY_CAPSULE:
			{
				writeTransform(dg->getTransform());
				hkDisplayCapsule* dc = static_cast<hkDisplayCapsule*>(dg);
				writeFloat32(dc->getRadius());
				writeQuadVector4(dc->getTop());
				writeQuadVector4(dc->getBottom());
				write32(dc->getNumSides());
				write32(dc->getNumHeightSamples());
			}
			break;

		case HK_DISPLAY_BOX:
			{
				writeTransform(dg->getTransform());
				hkDisplayBox* db = static_cast<hkDisplayBox*>(dg);
				writeQuadVector4(db->getHalfExtents());
			}
			break;

		case HK_DISPLAY_AABB:
			{
				hkDisplayAABB* daabb = static_cast<hkDisplayAABB*>(dg);
				writeQuadVector4(daabb->getMinExtent());
				writeQuadVector4(daabb->getMaxExtent());
			}
			break;

		case HK_DISPLAY_CONE:
			{
				hkDisplayCone* dcone = static_cast<hkDisplayCone*>(dg);
				writeQuadVector4(dcone->getPosition());
				writeQuadVector4(dcone->getAxis());
				writeFloat32(dcone->getAngle());
				writeFloat32(dcone->getHeight());
				write32(dcone->getNumSegments());
			}
		
			break;
	
		case HK_DISPLAY_SEMICIRCLE:
			{
				hkDisplaySemiCircle* dsemi = static_cast<hkDisplaySemiCircle*>(dg);
				writeQuadVector4(dsemi->getCenter());
				writeQuadVector4(dsemi->getNormal());
				writeQuadVector4(dsemi->getPerp());
				writeFloat32(dsemi->getRadius());
				writeFloat32(dsemi->getThetaMin());
				writeFloat32(dsemi->getThetaMax());
				write32(dsemi->getNumSegments());
			}
			break;

		case HK_DISPLAY_PLANE:
			{
				hkDisplayPlane* dplane = static_cast<hkDisplayPlane*>(dg);
				writeQuadVector4(dplane->getCenter());
				writeQuadVector4(dplane->getNormal());
				writeQuadVector4(dplane->getPerpToNormal());
				writeFloat32(dplane->getExtent());
			}
			break;

		default:
			HK_ASSERT2(0, "Display stream corrupt or unsupported display geometry cannot serialize!");
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
