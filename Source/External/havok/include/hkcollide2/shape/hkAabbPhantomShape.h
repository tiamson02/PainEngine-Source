/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_AABB_PHANTOM_SHAPE_H
#define HK_COLLIDE2_AABB_PHANTOM_SHAPE_H

#include <hkcollide2/shape/hkPhantomShape.h>

	/// This is a phantom shape that already holds its AABB in world space. 
	/// This shape is used as a top level shape that will make the physics skip the narrowphase collision detection for this type of phantom and rely solely on the broadphase.
class hkAabbPhantomShape : public hkPhantomShape
{
	public:
			/// Gets the AABB, similar to the hkBoxShape
 		virtual void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out ) const;

		/// Gets this hkShape's primary type. For hkAabbPhantomShapes, this is HK_SHAPE_PHANTOM_WITH_NO_AGENT.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_PHANTOM_WITH_NO_AGENT; }

	public:
		hkVector4 m_min;
		hkVector4 m_max;

		hkAabbPhantomShape(const hkVector4& min, const hkVector4& max): m_min(min), m_max(max){}
};

#endif //HK_COLLIDE2_AABB_PHANTOM_SHAPE_H

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
