/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_SHAPE_H
#define HK_COLLIDE2_SHAPE_H

#include <hkmath2/hkMath.h>
#include <hkcollide2/shape/hkShapeTypes.h>

class hkAabb;
class hkSphere;

/// The base class for all collision detection. All rigid bodies have an hkShape that defines their shape and how
/// they collide with other bodies, stored in their hkCollidable. 
class hkShape : public hkReferencedObject
{
	public:
		
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO);

			/// Sets the user data to 0
		inline hkShape();

			/// Gets the hkShape's primary type.
		virtual hkShapeType getPrimaryType() const = 0;

			/// Gets the hkShape's secondary type. 
		virtual hkShapeType getSecondaryType() const = 0;

			/// Gets the AABB for a given transform and an extra tolerance.
 		virtual void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out ) const = 0;

			/// Gets the bounding sphere. The default implementation uses getAabb() to calculate a non-minimal bounding sphere.
		virtual void getBoundingSphere( hkSphere& sphereOut ) const;

			/// Support for creating bounding volume hierarchies of shapes
		virtual void getBoundingSupport( const hkVector4& direction, hkVector4& supportingVertexOut, hkReal& extraRadiusOut) const;

			/// Get the user data for the shape (initialized to 0)
		inline void* getUserData() const;

			/// Set the user data of the shape
		inline void setUserData( void* data );


			/// The structure used for ray cast results
		struct hkRayResults
		{
				/// Constructor initialises the min dist to 1
			inline hkRayResults();

				/// The unnormalized hit normal in hkShape local space
			hkVector4 m_unnormalizedHitNormal;

				/// mindist is used to avoid unnecessary checks in the innerloops of shape raycasts
				/// the ray is the parameter in the ray parameterization 'from+m_mindist*(to-from)'
				/// so the range for m_mindist is [0.0f .. 1.f]
			hkReal	m_mindist;
			
				/// shapekey support - if the ray has hit a sub shape in a shape collection (such as a mesh shape) the shape key of that
				/// shape is set here
			hkInt32 m_primary;	

				/// shapekey support - if the ray has hit a sub shape in a shape collection (such as a mesh shape) the shape key of that
				/// shape is set here
			hkInt32 m_secondary;
		};
		
			/// Finds the first intersection between the shape and a ray defined in the shape's local space, starting at fromLocal, ending at toLocal.
			/// Note: this may change in subsequent versions of Havok 2. 
			/// Generally we recommend that you use the hkWorld or hkPhantom castRay() functions for raycasting.
			/// Always finds the closest hit and only reports this single hit.
			/// Returns 0 if there is no hit and 1 if there is a hit.
		virtual int castRay( const hkVector4& fromLocal, const hkVector4& toLocal, hkRayResults& results) const { return 0; }

			/// Cast an aabb. This feature is in development, and not yet fully supported
		virtual int castAabb(const hkVector4& fromLocal,const hkVector4& toLocal, const hkRotation& localToWorld, const hkVector4& halfExtents, hkRayResults& results) const;


	private:

		void* m_userData;
};

#include <hkcollide2/shape/hkShape.inl>

#endif // HK_COLLIDE2_SHAPE_H

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
