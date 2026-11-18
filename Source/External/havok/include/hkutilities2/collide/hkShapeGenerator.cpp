/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/collide/hkShapeGenerator.h>
#include <hkgeometryutil2/hkGeometryUtil.h>

hkConvexVerticesShape* HK_CALL hkShapeGenerator::createRandomShape(
	const hkVector4& minbox,
	const hkVector4& maxbox,
	int numvert)
{
	HK_ASSERT( (minbox.compareLessThanEqual4(maxbox) & HK_VECTOR3_COMPARE_MASK_XYZ) == HK_VECTOR3_COMPARE_MASK_XYZ );
	hkArray<hkVector4> verts(numvert);
	for(int i = 0; i < numvert; ++i)
	{
		for(int j = 0; j < 3; ++j)
		{
			verts[i](j) = hkMath::randRange( minbox(j), maxbox(j) );
		}
	}
	
	hkArray<hkVector4> planeEquations;
	hkArray<hkVector4> usedVertices;
	hkGeometryUtil::computePlanesFromVertices(&(verts[0](0)), numvert, sizeof(hkVector4), planeEquations, usedVertices);

	return new hkConvexVerticesShape( &(verts[0](0)), sizeof(hkVector4), numvert, planeEquations);
}

hkConvexVerticesShape* HK_CALL hkShapeGenerator::createRandomShapeWithThinTriangles(
	const hkVector4& minbox,
	const hkVector4& maxbox,
	int numvert, float minEdgeLen)
{
	HK_ASSERT( (minbox.compareLessThanEqual4(maxbox) & HK_VECTOR3_COMPARE_MASK_XYZ) == HK_VECTOR3_COMPARE_MASK_XYZ );
	hkInplaceArrayAligned16<hkVector4,48> verts(numvert+1);
	for(int i = 0; i < numvert; ++i)
	{
		{
			for(int j = 0; j < 3; ++j)
			{
				verts[i](j) = hkMath::randRange( minbox(j), maxbox(j) );
			}
		}
		{
			for(int j = 0; j < 3; ++j)
			{
				verts[i+1](j) = verts[i](j) + hkMath::randRange( -minEdgeLen, minEdgeLen );
			}
		}
		i++;
	}

	hkArray<hkVector4> planeEquations;
	hkArray<hkVector4> usedVertices;
	hkGeometryUtil::computePlanesFromVertices(&(verts[0](0)), numvert, sizeof(hkVector4), planeEquations, usedVertices);

	return new hkConvexVerticesShape( &(verts[0](0)), sizeof(hkVector4), numvert, planeEquations);
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
