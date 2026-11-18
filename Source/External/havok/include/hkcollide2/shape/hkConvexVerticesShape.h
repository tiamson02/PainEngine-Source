/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_CONVEX_VERTICES_SHAPE_H
#define HK_COLLIDE2_CONVEX_VERTICES_SHAPE_H

#include <hkcollide2/shape/hkConvexShape.h>

/// You can use this shape class to create a convex geometric object by specifying a set of vertices. 
class hkConvexVerticesShape : public hkConvexShape
{
	public:
	
			/// Construct the shape from the given vertices and matching plane equations.
		    /// These are plane equations of the convex hull and can be generated
		    /// using the hkGeometryUtil::computePlanesFromVertices method. 
		    /// This constructor makes an internal copy of the vertices.
		    /// The striding parameter is the byte difference between one float triple 
		    /// and the next float triple (so usually 12 or 16 or greater).
		hkConvexVerticesShape(const float* vertexIn, int striding, int numVertices, const hkArray<hkVector4>& planeEquations);
	
			// hkConvexShape interface implementation.
		virtual void getSupportingVertex( const hkVector4& direction, hkVector4& supportingVertex, int& vertexID ) const;

			/// hkCollisionSpheresShape interface implementation. This gets all the shape's vertices as an array of hkSpheres.
		virtual void getCollisionSpheres(hkArray<hkSphere>& collisionSpheres) const;

			// hkShape interface implementation.
		virtual void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out  ) const;

		virtual void getFirstVertex(hkVector4& v) const;

		virtual int castRay(const hkVector4& fromLocal,const hkVector4& toLocal,hkRayResults& results) const;

			/// Gets this hkShape's primary type. For hkConvexVerticesShapes, this is HK_SHAPE_CONVEX_VERTICES.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_CONVEX_VERTICES; }

			/// Return plane equations for the convex shape
			/// This will return the plane quations passed to the constructor as well as additional bevel planes
		virtual void getPlaneEquations( const hkRotation& localToWorld, const hkVector4& halfExtents, hkArray<hkVector4>& planeEquations ) const;

			/// Returns the plane equations passed into the constructor
		const hkArray<hkVector4>& getOriginalPlaneEquations() const;

	private:

		void copyVertexData(const float* vertexIn, int striding, int numVertices);

		struct FourVectors
		{
			hkVector4 x;
			hkVector4 y;
			hkVector4 z;
		};

		hkVector4	m_aabbHalfExtents;
		hkVector4	m_aabbCenter;

		hkInplaceArray<FourVectors, 3> m_rotatedVertices;
	protected:
		hkArray<hkVector4>	m_planeEquations;
};


#endif // HK_COLLIDE2_CONVEX_VERTICES_SHAPE_H

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
