/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY2_SPHERE_H
#define HK_GEOMETRY2_SPHERE_H

#include <hkmath2/hkMath.h>

/// A sphere defined by a center and radius. 
class hkSphere
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_GEOMETRY, hkSphere);

			/// Creates an arbitrary sphere with given center and radius.
		inline hkSphere();


			/// Creates an arbitrary sphere with given center and radius.
		inline hkSphere(const hkVector4& pt, hkReal radius);


			/// Returns the radius.
		inline hkReal getRadius() const;


			/// Changes the radius.
		inline void setRadius(hkReal newRadius);


			/// Returns the position of the center of the sphere.
		inline const hkVector4& getPosition() const;


			/// Changes the position of the center of the sphere.
		inline void setPosition(const hkVector4& newPos);

			/// Same as setPosition, using the hkVector4's w component as the radius.
		inline void setPositionAndRadius( const hkVector4& newPos );


	private:
		hkVector4 m_pos;		// note: the w part is the radius
};

#include <hkgeometry2/hkSphere.inl>

#endif // HK_GEOMETRY2_SPHERE_H

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
