/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgFaceSet.h>
#include <hkgraphics/hkgFaceSetPrimitive.h>
 
hkgFaceSetPrimitive::hkgFaceSetPrimitive( const hkgFaceSetPrimitive& p, int connectOffset )
{
	m_type = p.m_type;
	m_vertBufStartIndex = p.m_vertBufStartIndex;
	m_len = p.m_len;

	// copy the indices, if any
	if (p.m_indices)
	{
		m_indices = hkAllocate<unsigned short>( m_len, HK_MEMORY_CLASS_DISPLAY );
		hkString::memCpy( m_indices, p.m_indices, m_len * hkSizeOf(unsigned short));
	}
	else
		m_indices = HK_NULL;

	if (connectOffset != 0)
	{
		// shift indices
		if (m_indices)
		{
			for (int i=0; i < m_len; ++i)
				m_indices[i] += connectOffset;
		}	
		else
			m_vertBufStartIndex += connectOffset;
	}
}


void hkgFaceSetPrimitive::setLength(int n)
{
	if ( (m_len == n) /*|| (n < 0)*/ )
		return;

	if (m_indices)
		hkDeallocate<unsigned short>( m_indices );

	if (n)
		m_indices = hkAllocate<unsigned short>( n, HK_MEMORY_CLASS_DISPLAY );
	else
		m_indices = HK_NULL;

	m_len = n;
}

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
