/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_MULTI_RAY_SHAPE_H
#define HK_COLLIDE2_MULTI_RAY_SHAPE_H

#include <hkcollide2/shape/hkConvexShape.h>

	/// A shape that allows you to cast multiple rays.
class hkMultiRayShape : public hkShape
{
	public:
		///A ray from m_start to m_end.
		struct Ray
		{
			hkVector4 m_start;
			hkVector4 m_end;
		};

	public:

			/// Creates a new hkMultiRayShape with the specified rays.
		hkMultiRayShape( const Ray* rays, int nRays );

			// hkShape implementation.
 		void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out  ) const;

			///Gets this hkShape's secondary type. For hkMultiRayShapes, this is HK_SHAPE_MULTI_RAY.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_MULTI_RAY; }

			///Gets this hkShape's secondary type. For hkMultiRayShapes, this is HK_SHAPE_MULTI_RAY.
		virtual hkShapeType getSecondaryType() const { return HK_SHAPE_MULTI_RAY; }

			/// Returns the shape's rays.
		inline const hkArray<Ray>& getRays() const{ return m_rays; }

	public:
		// The ray defined in local space

	private:

		hkInplaceArrayAligned16<Ray,4> m_rays;
};


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
