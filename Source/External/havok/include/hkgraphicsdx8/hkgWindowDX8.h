/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_WINDOW_DX8_H
#define HK_GRAPHICS_WINDOW_DX8_H

#include <hkgraphics/hkgWindow.h>

class hkgWindowDX8 : public hkgWindow
{
public:

	inline const LPDIRECT3DDEVICE8 getDevice() const;
	inline LPDIRECT3DDEVICE8 getDevice();

protected:

	inline hkgWindowDX8();
	virtual ~hkgWindowDX8() {}

	LPDIRECT3DDEVICE8		m_pd3dDevice;
	LPDIRECT3D8				m_pD3D;              
	D3DPRESENT_PARAMETERS	m_d3dpp;   
};

#include <hkgraphicsdx8/hkgWindowDX8.inl>

#endif //HK_GRAPHICS_DISPLAY_CONTEXT_DX8_H
	
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
