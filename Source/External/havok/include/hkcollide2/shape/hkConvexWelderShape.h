/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_CONVEX_WELDER_SHAPE_H
#define HK_COLLIDE2_CONVEX_WELDER_SHAPE_H

#include <hkcollide2/shape/hkBvTreeShape.h>

/// This class filters some collisions between simple convex objects and a compound object. 
/// The compound object is wrapped by this class and must itself consist of convex objects
/// e.g. a triangle mesh.
/// Edges that touch between adjacent convex objects, within the compound shape, are "welded"
/// together. This eliminates the common problem where bodies bounce weirdly off triangle
/// edges in connected meshes.
class hkConvexWelderShape : public hkShape
{
	public:
			/// Constructs the shape. The first parameter is compound object to be welded, like a triangle mesh.
		inline hkConvexWelderShape( const hkBvTreeShape* compoundShapeToBeWelded );

			/// Destructor, removes references to child shapes.
		inline ~hkConvexWelderShape();

			//	hkShape interface implementation.
 		void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out  ) const;

			/// Gets this hkShape's primary type. For hkConvexWelderShape, this is HK_SHAPE_CONVEX_WELDER.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_CONVEX_WELDER; }

			/// Gets this hkShape's secondary type. For hkConvexWelderShape, this is the same as their primary type HK_SHAPE_CONVEX_WELDER.
		virtual hkShapeType getSecondaryType() const { return HK_SHAPE_CONVEX_WELDER; }

			//	hkShape interface implementation.
		virtual int castRay( const hkVector4& fromLocal, const hkVector4& toLocal, hkRayResults& results) const;

			//	hkShape interface implementation.
		virtual int castAabb(const hkVector4& fromLocal,const hkVector4& toLocal, const hkRotation& localToWorld, const hkVector4& halfExtents, hkRayResults& results) const;

	public:
		const hkBvTreeShape*		m_compoundShapeToBeWelded;
};

#include <hkcollide2/shape/hkConvexWelderShape.inl>

#endif // HK_COLLIDE2_CONVEX_WELDER_SHAPE_H

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
