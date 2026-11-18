/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_SHAPE_COLLECTION_H
#define HK_COLLIDE2_SHAPE_COLLECTION_H

#include <hkcollide/shape/hkShape.h>

extern hkReal hkConvexShapeDefaultRadius;

extern const hkClass hkShapeCollectionClass;

	/// An interface to a collection of Shapes, each of which can be identified using a hkShapeKey.
class hkShapeCollection : public hkShape
{		
	public:

		HK_DECLARE_REFLECTION();

			/// Empty constructor
		hkShapeCollection();

			/// The number of child shapes. The default implementation just iterates over all keys and is really slow
		virtual int getNumChildShapes( ) const;

			/// Get the first child shape key
			/// see getChildShape() for extra details
		virtual hkShapeKey getFirstKey() const = 0;

			/// Get the next child shape key
			/// If the the "oldKey" parameter is the last key in the shape collection, this function
			/// returns HK_INVALID_SHAPE_KEY
			/// see getChildShape() for extra details
		virtual hkShapeKey getNextKey( hkShapeKey oldKey ) const = 0;

			/// Gets a child shape using a shape key.
			/// This function must return a child shape pointer. This is only called internally by
			/// the collision detection system after having called getFirstKey() or getNextKey().
			/// If you have shape keys that are invalid, you must implement getNextKey() in such
			/// a way that it skips over these shapes.
			/// Important Note: It is assumed by the system that a shape key, if valid (i.e. returned by
			/// getNextkey()) will always remain valid.<br>
			/// Notes:
			///  - You can return a pointer to a shape
			///  - our you can construct a shape in place in the buffer and return a pointer to that buffer.
			///    Attention: When the buffer gets erased, no destructor will be called.
			/// The buffer must be 16 byte aligned.
		enum {  HK_SHAPE_BUFFER_ALIGNMENT = 16,
				HK_SHAPE_BUFFER_SIZE = 512 };

			/// A buffer type, allocated locally on the stack by calling functions,
			/// to be passed to getChildShape
		typedef HK_ALIGN16( char ShapeBuffer[HK_SHAPE_BUFFER_SIZE] );

		virtual const hkShape* getChildShape( hkShapeKey key, ShapeBuffer& buffer ) const = 0;

			/// Return the collision filter info for a given child shape
		virtual hkUint32 getCollisionFilterInfo( hkShapeKey key ) const { return 0; }

			//
			// hkShape interface
			//

			/// Implements the castRay function. Note that for shape collections with many sub-shapes this
			/// function can be very slow. It is better to use a hkBvTreeShape::castRay instead
		virtual hkBool castRay( const hkShapeRayCastInput& input, hkShapeRayCastOutput& results ) const;
	
			/// Note: the default implementation call hkShape::castRay( ..., hkShapeRayCastOutput& results )
		virtual void castRayWithCollector( const hkShapeRayCastInput& input, const hkCdBody& cdBody, hkRayHitCollector& collector ) const;

			/// Gets the AABB for the hkShape given a local to world transform and an extra tolerance.
			/// This default implementation is rather slow and just iterates over all children
 		virtual void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out ) const;

			/// Support for creating bounding volume hierarchies of shapes.
			/// This default implementation is rather slow and just iterates over all children
		virtual hkReal getMaximumProjection( const hkVector4& direction ) const;


			/// Gets this hkShape's type. Unless overridden by a derived class this returns HK_SHAPE_COLLECTION.
		virtual hkShapeType getType() const;

		virtual void calcStatistics( hkStatisticsCollector* collector) const;
	public:

		hkShapeCollection( hkFinishLoadedObjectFlag flag ) : hkShape(flag) {}

			/// By default the landscape collision detection removes interior edge collisions.
			/// If you want to disable this feature, simple set this parameter to true.
		hkBool m_disableWelding;
};

#endif // HK_COLLIDE2_SHAPE_COLLECTION_H

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
