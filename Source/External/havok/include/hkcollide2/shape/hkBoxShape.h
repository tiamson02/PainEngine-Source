/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_BOX_SHAPE_H
#define HK_COLLIDE2_BOX_SHAPE_H

#include <hkcollide2/shape/hkConvexShape.h>

	/// A simple box shape centered around the origin.
class hkBoxShape : public hkConvexShape
{
	public:
		
			/// Creates a box with the given half extents ( An (X by Y by Z) box has the half-extents (X/2, Y/2, Z/2) ).
		hkBoxShape( const hkVector4& halfExtents );

			/// Gets the half extents ( An (X by Y by Z) box has the half-extent (X/2, Y/2, Z/2) ).
		inline const hkVector4& getHalfExtents() const;

			/// Sets the half extents. Note that changing the half extents will not wake up sleeping objects.
		inline void setHalfExtents(const hkVector4& halfExtents);

		// hkConvexShape interface implementation.
		void getSupportingVertex( const hkVector4& dir, hkVector4& supportingVertexOut, int& vertexIdCache ) const;

			// hkCollisionSpheresShape interface implementation.
		void getCollisionSpheres(hkArray<hkSphere>& collisionSpheres) const;

			//	hkShape interface implementation.
 		void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out  ) const;

		virtual int castRay(const hkVector4& fromLocal, const hkVector4& toLocal, hkRayResults& results) const;
		
		virtual int castAabb(const hkVector4& fromLocal,const hkVector4& toLocal,const hkRotation& localToWorld, const hkVector4& halfExtents, hkRayResults& results) const;
		
			// hkConvexShape interface implementation
			//note: not all bevelplanes are added (edge-edge combinations are left out, too expensive to calculate in real-time), it is still conservative (not missing collisions)
			//this means the collision is more conservative then necessary
		virtual void getPlaneEquations( const hkRotation& localToWorld, const hkVector4& halfExtents, hkArray<hkVector4>& planeEquations ) const;

			/// Gets this hkShape's primary type. For hkBoxShapes, this is HK_SHAPE_BOX.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_BOX; }

	private:

		hkVector4 m_halfExtents;

};

#include <hkcollide2/shape/hkBoxShape.inl>

#endif // HK_COLLIDE2_BOX_SHAPE_H

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
