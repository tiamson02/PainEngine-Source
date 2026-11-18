/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_BV_SHAPE_H
#define HK_COLLIDE2_BV_SHAPE_H

#include <hkcollide2/shape/hkShape.h>	// because of the relPntr

/// An hkBvShape has two hkShape members, one of which is the child shape, the other
/// of which describes the bounding volume used for the child shape. 
class hkBvShape : public hkShape
{

	public:	
			/// Constructs the shape. The first parameter is the bounding volume shape, the second is the child shape.
		inline hkBvShape( const hkShape* boundingVolumeShape, const hkShape* childShape );
		
			/// Destructor, removes references to child shapes.
		inline ~hkBvShape();

			/// Gets the bounding volume shape. 
		inline const hkShape* getBoundingVolumeShape() const;

			/// Gets the child shape.
		inline const hkShape* getChildShape() const;


			//	hkShape interface implementation.
 		void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out  ) const;

				/// Gets this hkShape's primary type. For hkBvShapes, this is HK_SHAPE_BV.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_BV; }

			/// Gets this hkShape's secondary type. For hkBvShapes, this is the same as their primary type, HK_SHAPE_BV.
		virtual hkShapeType getSecondaryType() const { return HK_SHAPE_BV; }

	public:
		const hkShape*		m_boundingVolumeShape;
		const hkShape*		m_childShape;
};

#include <hkcollide2/shape/hkBvShape.inl>


#endif // HK_COLLIDE2_BV_SHAPE_H

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
