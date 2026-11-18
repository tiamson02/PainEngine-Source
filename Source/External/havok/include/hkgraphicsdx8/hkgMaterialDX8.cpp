/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsdx8/hkgraphicsdx8.h> // precompiled
#include <hkgraphicsdx8/hkgMaterialDX8.h>
#include <hkgraphicsdx8/hkgViewportDX8.h>
#include <hkgraphicsdx8/hkgDisplayContextDX8.h>

void hkgMaterialDX8::bind(hkgDisplayContext* context) const
{
	//glColor4fv( m_diffuse );
	LPDIRECT3DDEVICE8 dev = static_cast<hkgDisplayContextDX8*>( context )->getDevice();

	D3DMATERIAL8 mat;
	hkString::memSet( &mat, 0, sizeof(D3DMATERIAL8));
	hkgVec4Copy( (float*)(&mat.Diffuse), m_diffuse );
	// can set specular here too

	dev->SetMaterial( &mat);
	dev->SetRenderState( D3DRS_COLORVERTEX , FALSE );
	dev->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL ); 

	hkgMaterial::bind(context);
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
