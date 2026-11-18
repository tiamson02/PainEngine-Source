/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_COLLISION_SPHERES_SHAPE_H
#define HK_COLLIDE2_COLLISION_SPHERES_SHAPE_H

#include <hkcollide2/shape/hkShape.h>


/// This interface produces a set of spheres that represent a very simplified version of the objects surface.
/// Note: This interface's function is not currently used.
class hkCollisionSpheresShape : public hkShape
{
	public:
			/// Gets a set of spheres representing a simplified shape. For instance, a box could return its eight corners.
		virtual void getCollisionSpheres(hkArray<hkSphere>& collisionSpheres) const = 0;

	protected:
			/// The constructor - see hkShape::hkShape.
		HK_FORCE_INLINE hkCollisionSpheresShape();
};

#include <hkcollide2/shape/hkCollisionSpheresShape.inl>

#endif // HK_COLLIDE2_COLLISION_SPHERES_SHAPE_H

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
