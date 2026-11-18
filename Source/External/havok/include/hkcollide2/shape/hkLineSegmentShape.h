/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_LINE_SEGMENT_SHAPE_H
#define HK_COLLIDE2_LINE_SEGMENT_SHAPE_H

#include <hkcollide2/shape/hkConvexShape.h>



	/// A line segment defined by two points.
class hkLineSegmentShape : public hkConvexShape
{
	public:

			///Creates a new hkLineSegmentShape using the specified points
		hkLineSegmentShape( hkReal radius, const hkVector4& bottom,const hkVector4& top);

			/// Gets the first end of the line segment.
		inline const hkVector4& getBottom() const;
		
			/// Sets the first end of the line segment.
		inline void setBottom(const hkVector4& bottom);

			/// Gets the second  end of the line segment.
		inline const hkVector4& getTop() const;
		
			/// Sets the second end of the line segment.
		inline void setTop(const hkVector4& top);


			// hkConvexShape interface implementation.
		void getSupportingVertex( const hkVector4& direction, hkVector4& supportingVertex, int& vertexID ) const;

			//	hkCollisionSpheresShape interface implementation. This gets the line's two points as an array of hkSpheres.
		virtual void getCollisionSpheres(hkArray<hkSphere>& collisionSpheres) const;

		//	hkShape interface implementation. 
 		void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out  ) const;

			///Gets this hkShape's primary type. For hkLineSegmentShapes, this is HK_SHAPE_LINE_SEGMENT.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_LINE_SEGMENT; }

	private:
		// The line's two points.
		hkVector4 m_bottom;
		hkVector4 m_top;
};

#include <hkcollide2/shape/hkLineSegmentShape.inl>

#endif // HK_COLLIDE2_LINE_SEGMENT_SHAPE_H

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
