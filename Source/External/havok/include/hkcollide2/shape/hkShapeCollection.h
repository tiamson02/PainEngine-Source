/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_SHAPE_COLLECTION_H
#define HK_COLLIDE2_SHAPE_COLLECTION_H

#include <hkcollide2/shape/hkShape.h>

	/// An interface to a collection of Shapes, each of which can be identified using a hkShapeKey.
class hkShapeCollection: public  hkShape
{
	public:
		
		struct  hkShapeKey
		{
			hkInt32 m_primary;	
			hkInt32 m_secondary;
			
			inline bool operator< (const hkShapeKey& b) const;
			inline bool operator == (const hkShapeKey& b) const;
		};

	public:

		enum { HK_GET_SHAPE_BUFFER_SIZE = 256 };

			/// A buffer type, to be passed to getChildShape
		typedef char HK_ALIGNED_VARIABLE( AllocBuffer[HK_GET_SHAPE_BUFFER_SIZE], 16);

			/// The number of child shapes.
		virtual int getNumChildShapes( ) const = 0;

			/// A function to convert all integers between 0 and getNumChildShapes() into a proper hkShapeKey
		virtual hkShapeKey getKeyAt( int i ) const = 0;


			/// Gets a child shape using a shape key.
			/// Any implementation of this method can return HK_NULL as the corresponding child shape if desired.
		virtual const hkShape* getChildShape(const hkShapeKey& key, AllocBuffer& buffer ) const = 0;


				///Gets this hkShape's secondary type. For hkShapeCollections, this is HK_SHAPE_COLLECTION.
		virtual hkShapeType getSecondaryType() const { return HK_SHAPE_COLLECTION; }

				///Gets this hkShape's primary type. For hkShapeCollections, this is HK_SHAPE_COLLECTION.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_COLLECTION; }

				/// Casts ray into the hkShapeCollection.
				/// Will iterate through the shapes in the shape collection and 
				/// cast the ray into each shape.
				/// This will return ray results for the closest shape hit and
				/// will return 0 if no hit has occurred.
		virtual int castRay(const hkVector4& fromLocal,const hkVector4& toLocal,hkRayResults& results) const;

		virtual int castAabb(const hkVector4& fromLocal,const hkVector4& toLocal, const hkRotation& localToWorld, const hkVector4& halfExtents, hkRayResults& results) const;

				/// Gets the childshape and perform a raycast on it.
		virtual int childShapeCastRay(const hkShapeKey& key , const hkVector4& fromLocal,const hkVector4& toLocal,hkRayResults& results) const;
		
	protected:
			/// A constructor, which sets the secondary type to HK_SHAPE_COLLECTION 

		inline hkShapeCollection( ) : hkShape() { }
};

#include <hkcollide2/shape/hkShapeCollection.inl>

#endif // HK_COLLIDE2_SHAPE_COLLECTION_H

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
