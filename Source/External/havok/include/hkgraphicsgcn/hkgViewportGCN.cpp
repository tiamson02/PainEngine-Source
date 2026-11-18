/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsgcn/hkgraphicsgcn.h> // precompiled
#include <hkgraphics/hkgWindow.h>
#include <hkgraphicsgcn/hkgViewportGCN.h>

void hkgViewportGCN::setAsCurrent(hkgDisplayContext* context)
{
	int vw = m_winUR[0] - m_winLL[0];
	int vh = m_winUR[1] - m_winLL[1];

	int wh = m_owner->getHeight(); 
	
   GXSetViewport(
        (f32)m_winLL[0],
        (f32)(wh - m_winUR[1]),
        (f32)vw,
        (f32)vh,
        0.0F,
        1.0F );
    GXSetScissor(
        (u32)m_winLL[0],
        (u32)(wh - m_winUR[1]),
        (u32)vw,
        (u32)vh );

	hkgViewport::setAsCurrent(context);
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
