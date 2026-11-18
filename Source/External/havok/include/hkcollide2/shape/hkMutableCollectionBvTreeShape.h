/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_MUTABLE_COLLECTION_TREE_SHAPE_H
#define HK_COLLIDE2_MUTABLE_COLLECTION_TREE_SHAPE_H

#include <hkcollide2/shape/hkShape.h>
#include <hkcollide2/shape/hkShapeCollection.h>
#include <hkcollide2/shape/hkBvTreeShape.h>

 
class hkShapeCollection;

	/// An interface to query bounding volume information and a pointer to a shapeCollection
class hkMutableCollectionBvTreeShape: public hkBvTreeShape
{
	public:

		hkMutableCollectionBvTreeShape( const hkShapeCollection* collection );

			//	hkShape Interface 
 		void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out  ) const;


			/// Returns the hkShapeKey for each shape (in the hkShapeCollection) that intersects with the sphereQuery.
		virtual void querySphere( const hkSphere &sphereQuery, hkArray<hkShapeCollection::hkShapeKey>& hits ) const;

			/// Returns the hkShapeKey for each shape (in the hkShapeCollection) that intersects with the OBB (defined by obbTransform and obbExtent).
		virtual void queryObb( const hkTransform& obbTransform, const hkVector4& obbExtent, hkReal tolerance, hkArray<hkShapeCollection::hkShapeKey>& hits ) const;

			/// Returns the hkShapeKey for all shapes in the hkShapeCollection that intersect with the AABB
		virtual void queryAabb( const hkAabb& aabb, hkReal tolerance, hkArray<hkShapeCollection::hkShapeKey>& hits ) const;
};

#endif // HK_COLLIDE2_MUTABLE_COLLECTION_TREE_SHAPE_H

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
