/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_GEOMETRY2_TRIANGLEINDICES_H
#define HK_GEOMETRY2_TRIANGLEINDICES_H


#include <hkmath2/hkMath.h>


/// A triangle representation (by vertex 32-bit indices)
class hkTriangleIndices
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_GEOMETRY, hkTriangleIndices);

			/// Constructs the hkTriangleIndices with the given vertex indices.
		inline hkTriangleIndices();

			/// Constructs the hkTriangleIndices with the given vertex indices.
		inline hkTriangleIndices( const hkUint32 vertices[3] );

			/// Constructs the hkTriangleIndices with the given vertex indices.
		inline hkTriangleIndices( hkUint32 a, hkUint32 b, hkUint32 c );


			/// Sets the hkTriangleIndices from another hkTriangleIndices.
		inline void setTriangleIndices( const hkTriangleIndices &other );

			/// Sets the hkTriangleIndices with the given indices.
		inline void setTriangleIndices( const hkUint32 vertices[3] );

			/// Sets the hkTriangleIndices with the given indices.
		inline void setTriangleIndices( hkUint32 a, hkUint32 b, hkUint32 c );


			/// Returns a read-only reference to the first index of this hkTriangleIndices.
		inline const hkUint32* getIndexBase() const;
		
			/// Returns the ith vertex of this hkTriangleIndices.
		inline hkUint32 getVertex( hkUint32 i ) const;

			/// Sets the ith vertex of this hkTriangleIndices.
		inline void setVertex( hkUint32 i, hkUint32 v );


	private:

		hkUint32 m_verts[3];

};


#include <hkgeometry2/hkTriangleIndices.inl>


#endif // HK_GEOMETRY2_TRIANGLEINDICES_H


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
