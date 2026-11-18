/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_FEATURE_MANIFOLD_H
#define HK_COLLIDE2_FEATURE_MANIFOLD_H

#include <hkcollide2/manifold/hkFeatureContactPoint.h>

class hkCollidable;
struct hkCollisionResult;
struct hkCollisionEnvironment;


#define HK_BOXBOX_MANIFOLD_MAX_POINTS 8

class hkFeatureManifold
{
 
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO, hkFeatureManifold );

		hkFeatureManifold();

		// for 2.0 bridge
		int addPoint( const hkCollidable& bodyA, const hkCollidable& bodyB, hkFeatureContactPoint& fcp );

		void removePoint( int index, const hkCollidable& bodyA, const hkCollidable& bodyB );

		inline int getNumPoints();

		inline hkFeatureContactPoint& operator[]( int index );

		inline bool findInManifold( const hkFeatureContactPoint& fcp );

		inline bool isComplete();
		inline void setComplete( bool complete );
		
		inline bool hasNoPointsLeft();

		hkVector4 m_manifoldNormalA;
		hkVector4 m_manifoldNormalB;

		hkFeatureContactPoint m_contactPoints[HK_BOXBOX_MANIFOLD_MAX_POINTS];
		
		int  m_faceVertexFeatureCount;
		int	 m_numPoints;

		bool m_isComplete;  // a complete manifold is one where no new points can be added via small rotations.
		bool m_manifoldNormalInitialized;
#if !defined(HK_COMPILER_GCC) || defined(HK_PLATFORM_UNIX)
		// pad out to 16 byte boundary, have to do this because simd diabled on Xbox
		int m_pad0;
#endif

};


#include <hkcollide2/manifold/hkFeatureManifold.inl>


#endif // HK_COLLIDE2_FEATURE_MANIFOLD_H

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
