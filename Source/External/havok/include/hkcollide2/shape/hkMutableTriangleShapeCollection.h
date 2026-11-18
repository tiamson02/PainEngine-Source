/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_MUTABLE_TRIANGLE_SHAPE_COLLECTION_H
#define HK_COLLIDE2_MUTABLE_TRIANGLE_SHAPE_COLLECTION_H

#include <hkcollide2/shape/hkShapeCollection.h>
#include <hkcollide2/shape/hkTriangleShape.h>


/// A simple static list of shapes. This class can be used for creating compound bodies
class hkMutableTriangleShapeCollection : public hkShapeCollection
{
	public:

			/// Construct a list shape with an array of pointers to shapes
		hkMutableTriangleShapeCollection( int reserveNumShapes );

			/// Destructor removes references to child shapes
		~hkMutableTriangleShapeCollection();

		hkShapeKey createTriangle( const hkVector4& v0, const hkVector4& v1, const hkVector4& v2 );

		void destroyTriangle( hkShapeKey key );


			/// The number of child shapes
		virtual int getNumChildShapes( ) const;

			/// A function to convert all integers between 0 and getNumChildShapes() into a proper hkShapeKey
		virtual hkShapeKey getKeyAt( int i ) const;

			/// Gets a child shape using a shape key. Note: the implementor of this function can use the buffer passed in to put it's shape, but it does not need to
		virtual const hkShape* getChildShape(const hkShapeKey& key, AllocBuffer& buffer ) const;

			/// hkShape interface implementation
		virtual hkShapeType getSecondaryType() const { return HK_SHAPE_COLLECTION; }

			///
		virtual int castRay(const hkVector4& fromLocal,const hkVector4& toLocal,hkRayResults& results) const{ HK_ASSERT(0); return 0; }

			/// Get the AABB for a given transform and an extra tolerance
 		virtual void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out ) const;

			/// hkShape interface implementation
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_LIST; } //!!me is this right?


	protected:

		int m_lastUsed;
		int m_firstFree;
		int m_numUsed;
	
		hkArray<bool> m_freeField;
		hkInplaceArray<hkTriangleShape,128> m_triangles;

};



#endif // HK_COLLIDE2_MUTABLE_TRIANGLE_SHAPE_COLLECTION_H

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
