/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_UTILITIES2_CONVEX_SWEEP_SHAPE_H
#define HK_UTILITIES2_CONVEX_SWEEP_SHAPE_H

#include <hkcollide2/shape/hkConvexShape.h>

	/// This class is used in conjunction with the hkClipVelocityListener. Any convex shape in the scene that will be moving
	/// particularly fast, for example explosion debris, may be wrapped in this class, to prevent bullet through paper problem.
	/// You cannot instance hkConvexSweepShape objects, as they store information that is dependent on the velocity of the
	/// rigid body. However, the underlying hkConvexShape, the child shape of the hkConvexSweepShape, can be instanced as normal.
	/// NOTE: This shape uses HK_SHAPE_USER7 as its primary type.
	/// Please see the class description of hkClipVelocityListener for further information.
	/// If you are using this class, you must register the hkConvexSweepConvexAgent.
class hkConvexSweepShape : public hkConvexShape
{
	public:
			
			/// Construct this object with a convex shape as the child shape. When the parent rigid body is not moving, the
			/// shape will behave exactly as the child shape. When the rigid body moves over a given velocity, the hkClipVelocityListener
			/// sets the displacement of this shape, and "stretches" the shape by that amount.
			/// The constructor adds a reference to the child shape.
		hkConvexSweepShape( const hkConvexShape* childShape );
			
			/// Destructor removes a reference to the child shape.
		~hkConvexSweepShape();

			/// Get the child shape.
		inline const hkConvexShape* getChildShape() const;

			/// This shape uses HK_SHAPE_USER7 as its primary type.
		virtual hkShapeType getPrimaryType() const;
		
			// hkConvexShape implementation
		virtual void getSupportingVertex( const hkVector4& direction, hkVector4& supportingVertexOut, int& vertexIdCache ) const;

			// hkConvexShape implementation
 		virtual void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out ) const;

			// hkConvexShape implementation
		virtual	void getPlaneEquations( const hkRotation& localToWorld, const hkVector4& halfExtents, hkArray<hkVector4>& planeEquations ) const;
		
			// hkConvexShape implementation
		virtual int castRay( const hkVector4& fromLocal, const hkVector4& toLocal, hkRayResults& results) const;

			// hkConvexShape implementation
		virtual int castAabb(const hkVector4& fromLocal,const hkVector4& toLocal, const hkRotation& localToWorld, const hkVector4& halfExtents, hkRayResults& results) const;

			// hkConvexShape implementation
		virtual void getCollisionSpheres(hkArray<hkSphere>& collisionSpheres) const;

	public:
			
		mutable hkVector4 m_displacementLocal;
		mutable bool m_wantReset;

	protected:
			
		const hkConvexShape* m_childShape;

};

#include <hkutilities2/simulation/hkConvexSweepShape.inl>

#endif // HK_UTILITIES2_CONVEX_SWEEP_SHAPE_H

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
