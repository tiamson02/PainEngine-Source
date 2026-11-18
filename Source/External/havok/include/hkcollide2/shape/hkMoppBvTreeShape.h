/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_MOPP_BV_TREE_SHAPE_H
#define HK_COLLIDE2_MOPP_BV_TREE_SHAPE_H

#include <hkcollide2/shape/hkBvTreeShape.h>

class hkMoppCode;

	/// This class implements a hkBvTreeShape using MOPP technology.
class hkMoppBvTreeShape: public hkBvTreeShape
{
	public:

			/// Constructs a new hkMoppBvTreeShape. You can use the <hkcollide2/algorithms/mopp/utility/hkMoppUtility.h> to build a MOPP code.
		hkMoppBvTreeShape( const hkShapeCollection* collection, hkMoppCode* code);
		virtual ~hkMoppBvTreeShape();

			// hkBvTreeShape interface implementation.
		virtual void querySphere( const class hkSphere &sphere, hkArray<hkShapeCollection::hkShapeKey>& hits ) const;

			// hkBvTreeShape interface implementation.
		virtual void queryObb(    const hkTransform& BvToWorld, const hkVector4& extent, hkReal tolerance, hkArray<hkShapeCollection::hkShapeKey>& hits ) const;

			/// Returns the hkShapeKey for all shapes in the hkShapeCollection that intersect with the AABB
		virtual void queryAabb( const hkAabb& aabb, hkReal tolerance, hkArray<hkShapeCollection::hkShapeKey>& hits ) const;

			//	hkShape Interface 
 		void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out  ) const;

		virtual int castRay(const hkVector4& fromLocal,const hkVector4& toLocal,hkRayResults& results) const;
		
			///Gets this hkShape's primary type. For hkMoppBvTreeShapes, this is HK_SHAPE_BV_TREE.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_BV_TREE; }

		hkReal	m_averageLeaveNodeSize;
	private:
		hkMoppCode* m_code;
		
};


#endif // HK_COLLIDE2_MOPP_BV_TREE_SHAPE_H

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
