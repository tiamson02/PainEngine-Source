/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_SWEEP_BV_TREE_SHAPE_H
#define HK_COLLIDE2_SWEEP_BV_TREE_SHAPE_H

#include <hkcollide2/shape/hkBvTreeShape.h>

class hk3AxisSweep;


/// An hkSweepBvTreeShape  adds the 3d Sweep and Prune as a culling information to an hkShapeCollection, such as an hkMeshShape.
/// Use this as an alternative to the hkMoppBvTreeShape, especially when the shape is expected be moveable or deformable.
/// CPU Performance is expected to be better then the hkMoppBvTreeShape in the future, because of the incremental querying.
/// Memory usage is bigger then the hkMoppBvTreeShape (about 36 bytes per childshape).
/// This allows for concave moving, deforming and changing objects for example a car chassis that takes damage.

class hkSweepBvTreeShape : public hkBvTreeShape
{
	public:

		hkSweepBvTreeShape( hkShapeCollection* collection );

		virtual ~hkSweepBvTreeShape();

			/// Returns the hkShapeKey for all shapes in the hkShapeCollection that intersect with the sphereQuery.
		virtual void querySphere( const hkSphere &sphereQuery, hkArray<hkShapeCollection::hkShapeKey>& hits ) const ;

			/// Returns the hkShapeKey for all shapes in the hkShapeCollection that intersect with the obb (defined by obbTransform and obbExtent).
		virtual void queryObb( const hkTransform& obbTransform, const hkVector4& obbExtent, hkReal tolerance, hkArray<hkShapeCollection::hkShapeKey>& hits ) const ;

			//	hkShape interface implementation.
		virtual void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out ) const;

				/// Returns the hkShapeKey for all shapes in the hkShapeCollection that intersect with the AABB
		virtual void queryAabb( const hkAabb& aabb, hkReal tolerance, hkArray<hkShapeCollection::hkShapeKey>& hits ) const;
		
		//	hkShape interface implementation.
		int castRay(const hkVector4& fromLocal,const hkVector4& toLocal,hkRayResults& results) const;


	private:
		
		hk3AxisSweep*	m_3dSweep;
		hkShapeCollection* m_collection;
			// this objecthandles array is necessary for the 3dsweep and prune to work
		hkArray<hkBroadPhaseHandle>	m_objectHandles;
		
};

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
