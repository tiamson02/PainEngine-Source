/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_TRANSFORM_SHAPE_H
#define HK_COLLIDE2_TRANSFORM_SHAPE_H

#include <hkcollide2/shape/hkShape.h>

	/// An hkTransformShape contains an hkShape and an additional transform for that shape. 
	///	This is useful, for instance, if you
	/// want to position child shapes correctly when constructing a compound shape.
class hkTransformShape : public hkShape
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO, hkTransformShape);

			/// Constructs a new transform shape.
		hkTransformShape(hkShape* childShape);
					
			/// The destructor removes references to the child shape.
		~hkTransformShape();

		inline const hkShape* getChildShape() const { return m_childShape; }

			/// Gets the transform from the child shape's space to this transform shape's local space.
		inline hkTransform& getTransform() { return m_parentFromChildTransform; }

			/// Gets the transform from the child shape's space to this transform shape's local space.
		inline const hkTransform& getTransform() const { return m_parentFromChildTransform; }
		
			/// Sets the current transform.
		inline void setTransform(const hkTransform& transform) { m_parentFromChildTransform = transform; }


			//	hkShape interface implementation.
		void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out ) const;

			//	hkShape interface implementation.
		void getBoundingSphere( hkSphere& sphereOut ) const;

			/// support for MOPP
		virtual void getBoundingSupport( const hkVector4& direction, hkVector4& supportingVertexOut, hkReal& extraRadiusOut) const;

			///Gets this hkShape's primary type. For hkTransformShapes, this is HK_SHAPE_TRANSFORM.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_TRANSFORM; }

				///Gets this hkShape's secondary type. For hkTransformShapes, this is HK_SHAPE_TRANSFORM.
		virtual hkShapeType getSecondaryType() const { return HK_SHAPE_TRANSFORM; }
		
		virtual int castRay(const hkVector4& fromLocal,const hkVector4& toLocal,hkRayResults& results) const;

		virtual int castAabb(const hkVector4& fromLocal,const hkVector4& toLocal,const hkRotation& localToWorld, const hkVector4& halfExtents, hkRayResults& results) const;
	
	protected:

		hkShape* m_childShape;

		hkTransform m_parentFromChildTransform;
};


#endif // HK_COLLIDE2_TRANSFORM_SHAPE_H

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
