/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_SHAPE_GENERATOR
#define HK_SHAPE_GENERATOR

#include <hkcollide2/shape/hkConvexVerticesShape.h>

///This class can be used to create random hkConvexVerticesShapes.
class hkShapeGenerator
{
	public:

		static hkConvexVerticesShape* HK_CALL createRandomShape( const hkVector4& minbox, const hkVector4& maxbox, int numvert);
		static hkConvexVerticesShape* HK_CALL createRandomShapeWithThinTriangles( const hkVector4& minbox, const hkVector4& maxbox, int numvert, float minEdgeLen);
};

#endif //HK_SHAPE_GENERATOR


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
