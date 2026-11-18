/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_TRIANGLE_SHAPE_H
#define HK_COLLIDE2_TRIANGLE_SHAPE_H

#include <hkcollide2/shape/hkConvexShape.h>

/// A triangle shape with its details stored as an hkTriangle.
class hkTriangleShape : public hkConvexShape
{
	public:
 
			/// Default constructor - nothing gets initialized.
		HK_FORCE_INLINE hkTriangleShape();
		
			/// Constructor that sets the points of the triangle.
		HK_FORCE_INLINE hkTriangleShape(const hkVector4& v0, const hkVector4& v1, const hkVector4& v2 );

			/// Gets the triangle.
		HK_FORCE_INLINE const hkVector4* getVertices() const;

			/// Gets a vertex
		HK_FORCE_INLINE const hkVector4& getVertex(int i) const;

			/// Sets a vertex
		HK_FORCE_INLINE void setVertex(int i, const hkVector4& vertex);


			/// Gets a user id  (e.g. hkMeshShape uses it for triangle id)
		inline hkUint32 getUserIdA() const;

			/// Gets the second user id   (e.g. hkMeshShape uses it for the subpart id)
		inline  hkUint32 getUserIdB() const;

			/// Sets the first user id.
		inline void setUserIdA( hkUint32 id );

		/// Sets the second user id.
		inline void setUserIdB( hkUint32 id );

			//	hkConvexShape interface implementation.
		virtual void getSupportingVertex( const hkVector4& direction, hkVector4& supportingVertex, int& vertexID ) const;

			//	hkConvexShape interface implementation.
		virtual void getFirstVertex(hkVector4& v) const;

			///	hkCollisionSpheresShape interface implementation. This returns the triangle's vertices as an array of hkSpheres.
		virtual void getCollisionSpheres(hkArray<hkSphere>& collisionSpheres) const;


			// hkShape interface implementation
 		void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out  ) const;

		// hkShape interface implementation
		void getBoundingSphere( hkSphere& sphereOut ) const;
		
		
		virtual int castRay(const hkVector4& fromLocal,const hkVector4& toLocal,hkRayResults& results) const;

		virtual int castAabb(const hkVector4& fromLocal,const hkVector4& toLocal,const hkRotation& localToWorld, const hkVector4& halfExtents, hkRayResults& results) const;
		
		virtual	void	getPlaneEquations( const hkRotation& localToWorld, const hkVector4& halfExtents, hkArray<hkVector4>& planeEquations ) const ;
			
			///Gets this hkShape's primary type. For hkTriangleShapes, this is HK_SHAPE_TRIANGLE.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_TRIANGLE; }

	private:
		hkVector4 m_vertices[3];
		hkUint32 m_userIdA;
		hkUint32 m_userIdB;

			// pad out to 16 bytes
		hkUint32 m_pad0;
		hkUint32 m_pad1;
};

HK_COMPILE_TIME_ASSERT( sizeof ( hkTriangleShape ) == 5 * 16 );

#include <hkcollide2/shape/hkTriangleShape.inl>

#endif // HK_COLLIDE2_TRIANGLE_SHAPE_H

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
