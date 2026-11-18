/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_SHAPE_RAY_CAST_RESULTS
#define HK_SHAPE_RAY_CAST_RESULTS

#include <hkcollide/shape/hkShape.h>

	/// The structure used for ray cast results.
	/// Note: the structure can be used only for one raycast,
	/// If you want to reuse it, you have to call hkShapeRayCastOutput::reset()
struct hkShapeRayCastOutput
{
		/// Constructor initialises the hitFraction to 1
	inline hkShapeRayCastOutput();

		/// Resets this structure if you want to reuse it for another raycast, by setting the hitFraction to 1
	inline void reset();

		/// returns true, if the ray had hit
	inline hkBool hasHit() const;
		
		/// The normalized normal of the surface hit by the ray. This normal is in the space of the ray.
	hkVector4  m_normal;

		/// If the ray has hit a child shape of a shape collection, this value is set to the subshape index 
		/// (e.g. the triangle id) used to get the shape. For other shapes, this value will be HK_INVALID_SHAPE_KEY.
	hkShapeKey m_shapeKey;	 

		/// This value will be in the range of [0..1]. It determines where along the ray the hit occurred, where 1 is the end position
		/// of the ray and 0 is the start position of the ray. It is also used as an "early out" value and has to be initialized with 1.0f
	hkReal	   m_hitFraction;
};

#include <hkcollide/shape/hkShapeRayCastOutput.inl>


#endif //HK_SHAPE_RAY_CAST_RESULTS

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
