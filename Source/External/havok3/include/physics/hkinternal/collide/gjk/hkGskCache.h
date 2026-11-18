/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_GSK_CACHE
#define HK_COLLIDE2_GSK_CACHE

class hkConvexShape;
class hkTriangleShape;


class hkGskCache
{
	public:
		void init( const hkConvexShape* shapeA, const hkConvexShape* shapeB, const hkTransform& btoa );
		void initTriangle( const hkConvexShape* shapeA, const hkTriangleShape* shapeB, const hkTransform& btoa );

	public:
		hkUint16  m_vertices[4];
		hkChar	  m_dimA;
		hkChar	  m_dimB;
		hkChar	  m_maxDimA;
		hkChar	  m_maxDimB;
};

#endif // HK_COLLIDE2_GSK_CACHE

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
