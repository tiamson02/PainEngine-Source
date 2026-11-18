/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY2_LINE_SEGMENT_INDICES_H
#define HK_GEOMETRY2_LINE_SEGMENT_INDICES_H


/// This class is used to store a pair of vertex indices which may define an edge of a Geometry.
class hkLineSegmentIndices
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_GEOMETRY, hkLineSegmentIndices);

			/// Instantiate an hkLineSegmentIndices.
		inline hkLineSegmentIndices();

			/// Copy constructor - memberwise copy.
		hkLineSegmentIndices( const hkLineSegmentIndices &other );

			/// Constructs the hkLineSegmentIndices with the given vertices.
		hkLineSegmentIndices( const unsigned short vertices[2] );

			/// Constructs the hkLineSegmentIndices with the given vertices.
		hkLineSegmentIndices( const unsigned short a, const unsigned short b );

			/// Sets the hkLineSegmentIndices with the given vertices.
		inline void setLineSegmentIndices( const unsigned short vertices[2] );

			/// Sets the hkLineSegmentIndices with the given vertices.
		inline void setLineSegmentIndices( const unsigned short a, const unsigned short b );

			/// Returns the ith vertex on this hkLineSegmentIndices.
		inline unsigned short getVertex( unsigned char i ) const;

			/// Sets the ith vertex on this hkLineSegmentIndices to v
		inline void setVertex( unsigned char i, unsigned short v );



	private:

			/// Array of Vertex indices (16 bits each).
		unsigned short m_verts[2];

};

#include <hkgeometry2/hkLineSegmentIndices.inl>

#endif // HK_GEOMETRY2_LINE_SEGMENT_INDICES_H

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
