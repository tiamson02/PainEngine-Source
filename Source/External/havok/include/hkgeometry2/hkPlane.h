/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY2_PLANE_H
#define HK_GEOMETRY2_PLANE_H

#include <hkmath2/hkMath.h>

/// A plane defined by a normal and a distance from the origin. 
class hkPlane
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_GEOMETRY, hkPlane);

			/// Default constructor. Create a hkPlane passing though the origin with normal up the Y-axis (0,1,0).
		hkPlane();

			/// Creates a hkPlane from a normal and a distance (from the origin to the hkPlane, in the direction of the normal).
		hkPlane(const hkVector4& normal, hkReal dist);


			/// Returns the distance from the hkPlane to the origin.
		hkReal getDistance() const;


			/// Returns the distance from the point to the hkPlane.
		hkReal getDistance(const hkVector4& pt) const;


			/// Changes the distance from the origin.
		void setDistance( hkReal newDist );


			/// Returns the normal to the hkPlane.
		const hkVector4& getNormal() const;


			/// Changes the normal for the hkPlane.
		void setNormal(const hkVector4& newNorm);


	private:

		hkVector4 m_normal;		// note: the w part is the distance from the origin
};

#include <hkgeometry2/hkPlane.inl>

#endif // HK_GEOMETRY2_PLANE_H

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
