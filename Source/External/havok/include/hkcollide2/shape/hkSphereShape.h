/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_SPHERE_SHAPE_H
#define HK_COLLIDE2_SPHERE_SHAPE_H

#include <hkcollide2/shape/hkConvexShape.h>

/// This the basic sphere shape. Note that the functions for getting and setting the radius are in the hkConvexShape base class. Thus a sphere shape need not
/// hold any data, it is simply an implicit point at the origin with a radius.
class hkSphereShape : public hkConvexShape
{
	public:
			/// Creates an arbitrary sphere with given radius.
		hkSphereShape(hkReal radius = 1);

			// hkConvexShape interface implentation.
		virtual void getSupportingVertex( const hkVector4& direction, hkVector4& supportingVertex, int& vertexID ) const;

	
			//	hkShape interface implementation.
		void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out ) const;

		//	hkShape interface implementation.
		void getBoundingSphere( hkSphere& sphereOut ) const;

			///	hkCollisionSpheresShape interface implementation, identical to getVertices.
		virtual void getCollisionSpheres(hkArray<hkSphere>& collisionSpheres) const;

			///Gets this hkShape's secondary type. For hkSphereShapes, this is HK_SHAPE_SPHERE.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_SPHERE; }

		virtual int castRay(const hkVector4& fromLocal,const hkVector4& toLocal,hkRayResults& results) const;
	
};

#endif // HK_COLLIDE2_SPHERE_SHAPE_H

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
