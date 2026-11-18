/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsdx8/hkgraphicsdx8.h> // precompiled
#include <hkgraphicsdx8/hkgDisplayObjectDX8.h>
#include <hkgraphicsdx8/hkgDisplayContextDX8.h>

D3DMATRIX hkgDisplayObjectDX8::m_tempMatrix;

void hkgDisplayObjectDX8::render(hkgDisplayContext* state) const
{
	LPDIRECT3DDEVICE8 dev;
	if (!m_identityTrans) 
	{
		dev = static_cast<hkgDisplayContextDX8*>( state )->getDevice();
		dev->SetTransform( D3DTS_WORLD, (D3DMATRIX*)m_trans); 
	}
	
	renderGeomsInOrder( state );

	if (!m_identityTrans)
	{
		hkgMat4Identity( (float*)&m_tempMatrix );
		dev->SetTransform( D3DTS_WORLD, &m_tempMatrix );	
	}
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
