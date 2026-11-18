/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_LIST_SHAPE_H
#define HK_COLLIDE2_LIST_SHAPE_H

#include <hkcollide2/shape/hkShapeCollection.h>

/// A simple static list of hkShapes. You can use this shape class to create compound bodies.
/// A list shape can hold a mix of different shape types e.g. an ice cream cone could be made 
/// from a list shape containing a sphere for the ice cream and a convex vertices shape 
/// for the wafer cone.
class hkListShape : public hkShapeCollection
{
	public:

			/// Constructs a list shape with an array of pointers to shapes.
		hkListShape( hkShape* shapeArray[], int numShapes );

			/// The destructor removes references to child shapes.
		~hkListShape();

			/// Returns the ith child shape.
		inline const hkShape* getChildShape(int i) const { return m_childShapes[i]; }

			// hkShapeCollection interface implementation.
		virtual int getNumChildShapes() const;

			// hkShapeCollection interface implementation.
		virtual hkShapeCollection::hkShapeKey getKeyAt( int i ) const;


			// hkShapeCollection interface implementation.
		virtual const hkShape* getChildShape(const hkShapeCollection::hkShapeKey& key, AllocBuffer& buffer ) const;


			/// Gets the AABB for a given transform and an extra tolerance.
 		virtual void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out ) const;

			///Gets this hkShape's primary type. For hkListShapes, this is HK_SHAPE_LIST.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_LIST; }


	protected:
		friend class hkListAgent;
		hkInplaceArray<hkShape*,4> m_childShapes;

};



#endif // HK_COLLIDE2_LIST_SHAPE_H

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
