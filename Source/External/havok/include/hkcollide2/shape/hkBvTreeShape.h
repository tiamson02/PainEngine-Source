/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_BV_TREE_SHAPE_H
#define HK_COLLIDE2_BV_TREE_SHAPE_H

#include <hkcollide2/shape/hkShape.h>
#include <hkcollide2/shape/hkShapeCollection.h>

class hkShapeCollection;

/// An hkBVTreeShape adds bounding volume tree information to an hkShapeCollection, such as an hkMeshShape.
class hkBvTreeShape: public hkShape
{
	public:
			/// Returns the hkShapeKey for all shapes in the hkShapeCollection that intersect with the sphereQuery.
		virtual void querySphere( const hkSphere &sphereQuery, hkArray<hkShapeCollection::hkShapeKey>& hits ) const = 0;

			/// Returns the hkShapeKey for all shapes in the hkShapeCollection that intersect with the obb (defined by obbTransform and obbExtent).
		virtual void queryObb( const hkTransform& obbTransform, const hkVector4& obbExtent, hkReal tolerance, hkArray<hkShapeCollection::hkShapeKey>& hits ) const = 0;

			/// Returns the hkShapeKey for all shapes in the hkShapeCollection that intersect with the AABB
		virtual void queryAabb( const hkAabb& aabb, hkReal tolerance, hkArray<hkShapeCollection::hkShapeKey>& hits ) const = 0;

			/// Gets the hkShapeCollection.
		inline const hkShapeCollection* getShapeCollection() const;

			//	hkShape interface implementation.
		virtual int castAabb(const hkVector4& fromLocal,const hkVector4& toLocal, const hkRotation& aabbToLocal, const hkVector4& halfExtents, hkRayResults& results) const;

			/// Gets this hkShape's primary type. For hkBVTreeShapes, this is HK_SHAPE_BV_TREE.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_BV_TREE; }

			/// Gets this hkShape's secondary type. For hkBVTreeShapes, this is the same as their primary type, HK_SHAPE_BV_TREE.
		virtual hkShapeType getSecondaryType() const { return HK_SHAPE_BV_TREE; }

	protected:
			/// Creates an hkBvTreeShape with the specified hkShapeCollection.
		inline hkBvTreeShape( const hkShapeCollection* collection );

		virtual ~hkBvTreeShape();

		const hkShapeCollection*	m_shapeCollection;
};


#include <hkcollide2/shape/hkBvTreeShape.inl>

#endif // HK_COLLIDE2_BV_TREE_SHAPE_H

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
