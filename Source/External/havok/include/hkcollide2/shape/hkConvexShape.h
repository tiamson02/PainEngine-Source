/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_CONVEX_SHAPE_H
#define HK_COLLIDE2_CONVEX_SHAPE_H

#include <hkcollide2/shape/hkCollisionSpheresShape.h>

	/// An interface shape class that allows its implementations to work with GJK. It also holds an extra radius to allow for shells around objects.
class hkConvexShape : public hkCollisionSpheresShape
{
	public:
			/// The GJK interface. It can use the vertexIdCache, which is initialized with 0.
		virtual void getSupportingVertex( const hkVector4& direction, hkVector4& supportingVertexOut, int& vertexIdCache ) const = 0;
		
		/// MOPP support
		virtual void getBoundingSupport( const hkVector4& direction, hkVector4& supportingVertexOut, hkReal& extraRadiusOut) const;

			/// Returns the first vertex of this shape. This is only used for initialization of collision detection data.
		virtual void getFirstVertex(hkVector4& v) const;

			/// Gets the extra radius.
		inline hkReal getRadius() const;

			/// Sets the extra radius (Note: changing this does not wake up sleeping objects attached to this).
		inline void setRadius(hkReal radius);

			/// hkShape interface implementation. Note that this secondary type is inherited by the various
			/// hkConvexShape subclasses, such as hkBoxShape and hkSphereShape.
		virtual hkShapeType getSecondaryType() const { return HK_SHAPE_CONVEX; }
			
			/// getPlaneEquations returns bounding planes used by the aabb cast. Called by the hkAabbCastConvexAgent.
			/// These bounding planes represent the explicit Minkowsky difference with an aabb. During the aabb cast, the planes are dynamically expanded proportionally to the the aabb size internally.
		virtual	void	getPlaneEquations( const hkRotation& localToWorld, const hkVector4& halfExtents, hkArray<hkVector4>& planeEquations ) const {}


		static hkReal DEFAULT_RADIUS;

	protected:
			/// The protected constructor, which sets the secondary type to be convex.
		inline hkConvexShape(hkReal radius=DEFAULT_RADIUS );

		hkReal m_radius;

};

#include <hkcollide2/shape/hkConvexShape.inl>

#endif // HK_COLLIDE2_CONVEX_SHAPE_H

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
