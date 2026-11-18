/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY2_LINE_SEGMENT_H
#define HK_GEOMETRY2_LINE_SEGMENT_H

#include <hkmath2/hkMath.h>


/// This class is used to store a pair of hkVector4 objects.
class hkLineSegment
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_GEOMETRY, hkLineSegment);

			/// Instantiates an hkLineSegment.
		inline hkLineSegment();

			/// Constructs the hkLineSegment with the given vertices.
		hkLineSegment( const hkVector4& a, const hkVector4& b);

			/// Sets the hkLineSegment with the given vertices.
		inline void setLineSegment(const hkVector4& a, const hkVector4& b );

			/// Returns the ith vertex on this line segment (non-const).
		inline hkVector4& getVertex( int i );
		
			/// Returns the ith vertex on this line segment (const).
		inline const hkVector4& getVertex( int i ) const;

			/// Sets the ith vertex on this hkLineSegment to v
		inline void setVertex( unsigned char i, const hkVector4& v);


	private:

		hkVector4 m_verts[2];

};

#include <hkgeometry2/hkLineSegment.inl>


#endif // HK_GEOMETRY2_LINE_SEGMENT_H

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
