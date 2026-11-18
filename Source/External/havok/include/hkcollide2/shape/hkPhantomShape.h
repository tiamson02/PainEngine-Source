/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_PHANTOM_SHAPE_H
#define HK_COLLIDE2_PHANTOM_SHAPE_H

#include <hkcollide2/shape/hkShape.h>
class hkCollidable;
struct hkCollisionEnvironment;

/// This shape allows you to create "phantom" shapes, which have no physical effect in a scene but which can trigger events
/// when other shapes penetrate them. This shape is typically the child shape of a hkBvShape, where the bounding volume of the hkBvShape is the boundary used for the phantom. Phantom shapes can also be used directly in the broadphase, where getAabb() is used to get the broad phase AABB. 
/// Using phantom shapes directly with the broadphase is very fast. 
class hkPhantomShape : public hkShape
{
	public:
			/// This callback is called when the phantom shape starts intersecting with another shape.
		virtual void phantomEnterEvent( const hkCollidable& phantomColl, const hkCollidable& otherColl,	const hkCollisionEnvironment& env ) = 0;

			/// This callback is called when the phantom shape stops intersecting with another shape.
		virtual void phantomLeaveEvent( const hkCollidable& phantomColl, const hkCollidable& otherColl,	const hkCollisionEnvironment& env ) = 0;


			/// hkShape interface implementation. Note that phantom shapes are most usually used in combination with an hkBvShape, 
			/// so getAabb() never gets called. However, if you are using the phantom shape directly with the broadphase, you need to
			/// implement this function. The default implementation returns an AABB with no volume.
 		virtual void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out ) const;

			///Gets this hkShape's secondary type. For hkPhantomShapes, this is HK_SHAPE_PHANTOM.
		virtual hkShapeType getSecondaryType() const { return HK_SHAPE_PHANTOM; }

		///Gets this hkShape's primary type. For hkPhantomShapes, this is HK_SHAPE_PHANTOM.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_PHANTOM; }

	protected:

		hkPhantomShape(): hkShape( ){}

};


#endif // HK_COLLIDE2_PHANTOM_SHAPE_H

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
