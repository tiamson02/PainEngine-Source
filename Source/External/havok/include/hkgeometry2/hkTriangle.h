/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY2_TRIANGLE_H
#define HK_GEOMETRY2_TRIANGLE_H

#include <hkmath2/hkMath.h>

/// A Triangle Representation (by vertices)
class hkTriangle
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_GEOMETRY, hkTriangle);

			/// Default constructor
		inline hkTriangle();

			/// Instantiates a Triangle.
		inline hkTriangle( const hkVector4& a, const hkVector4& b, const hkVector4& c );

			/// Sets from 3 Vector4s.
		inline void setTriangle( const hkVector4& a, const hkVector4& b, const hkVector4& c );

			/// Returns the ith vertex on this Triangle (const).
		inline const hkVector4& getVertex( int i ) const;

			/// Returns the ith vertex on this Triangle (non-const).
		inline hkVector4& getVertex( int i );

			/// Sets the ith vertex to x.
		inline void setVertex( int i, const hkVector4& x );

	protected:

		hkVector4 m_verts[3];

};

#include <hkgeometry2/hkTriangle.inl>

#endif // HK_GEOMETRY2_TRIANGLE_H

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
