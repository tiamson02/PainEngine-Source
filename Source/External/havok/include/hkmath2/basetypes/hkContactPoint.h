/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_MATH2_CONTACT_POINT_H
#define HK_MATH2_CONTACT_POINT_H

#include <hkmath2/hkMath.h>

/// A simple class to hold contact point data. NOTE: All data is in World Space.
class hkContactPoint
{
	public:

			
		inline const hkVector4& getPosition() const;

		inline void setPosition(const hkVector4& position);

		
		inline const hkVector4& getNormal() const;

		inline void setNormal( const hkVector4& normal );


		inline hkReal getDistance() const;

		inline void setDistance( hkReal d );


		inline void set( const hkVector4& position, const hkVector4& normal, float dist );

	protected:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE( HK_MEMORY_CLASS_COLLIDE, sizeof( hkContactPoint ) );

		hkVector4 m_position;					// .w is the the distance
		hkVector4 m_normal;			
};

HK_COMPILE_TIME_ASSERT( sizeof( hkContactPoint ) == 32 );

#include <hkmath2/basetypes/hkContactPoint.inl>

#endif // HK_MATH2_CONTACT_POINT_H

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
