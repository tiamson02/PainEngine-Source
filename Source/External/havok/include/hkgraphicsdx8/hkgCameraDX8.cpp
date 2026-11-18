/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsdx8/hkGraphicsDX8.h> // precompiled
#include <hkgraphicsdx8/hkgCameraDX8.h>
#include <hkgraphicsdx8/hkgWindowDX8.h>

//#include <d3dx8.h>

hkgCamera* hkgCameraDX8::createCameraDX8()
{
	return new hkgCameraDX8(hkgCamera::DEFAULT_FROM, 
			hkgCamera::DEFAULT_TO, 
			hkgCamera::DEFAULT_UP,
			hkgCamera::DEFAULT_FOV); 
}

#define PRINTMAT(str, x) { printf("\n%s Matrix:\t[%.3f %.3f %.3f %.3f]\n\t\t\t[%.3f %.3f %.3f %.3f]\n\t\t\t[%.3f %.3f %.3f %.3f]\n\t\t\t[%.3f %.3f %.3f %.3f]\n",\
						str, \
						x[0], x[4], x[8], x[12], \
						x[1], x[5], x[9], x[13], \
						x[2], x[6], x[10], x[14], \
						x[3], x[7], x[11], x[15] ); }

#define PRINTMATDX(str, x) { printf("\n%s Matrix:\t[%.3f %.3f %.3f %.3f]\n\t\t\t[%.3f %.3f %.3f %.3f]\n\t\t\t[%.3f %.3f %.3f %.3f]\n\t\t\t[%.3f %.3f %.3f %.3f]\n",\
						str, \
						x[0], x[1], x[2], x[3], \
						x[4], x[5], x[6], x[7], \
						x[8], x[9], x[10], x[11], \
						x[12], x[13], x[14], x[15] ); }

void hkgCameraDX8::setAsCurrent(hkgDisplayContext* state)
{
	// D3D uses row vectors, so the matrices have translation
	// on bottom row etc... but they are stored in column major
	// format, so we don't need to transpose as the interpretation
	// does the transpose for us :-)
	
	D3DMATRIX dxProj;
	hkgMat4Copy( (float*)&dxProj, m_projMat );

	// as DX usually maps z from 0-->1, our proj maps
	// it from -1 to 1, so we need to alter the proj matrix a 
	// bit to restore the mapping (along with the negation of z)
	if (m_mode == HKG_CAMERA_PERSPECTIVE)
	{
		dxProj._31 *= -1;
		dxProj._32 *= -1;
		if (this->m_handedness == HKG_CAMERA_HANDEDNESS_RIGHT)
		{
			dxProj._33 = m_far / ( m_far - m_near );
			dxProj._34 = 1; 
		}
		else
		{
			dxProj._33 = m_far / ( m_near - m_far );
			dxProj._34 = -1; 
		}

		dxProj._43 *= 0.5f;
	}
	else // an ortho mode
	{
		dxProj._33 *= -0.5f; // /-2;
		dxProj._43 = m_onear / ( m_onear - m_ofar );
	}

	D3DMATRIX dxView;
	hkgMat4Copy( (float*)&dxView, m_viewMat );

	// RH --> LH with Z neg == col 3 (NOT row 3 as is different form) 
	dxView._13 *= -1;
	dxView._23 *= -1;
	dxView._33 *= -1;
	dxView._43 *= -1;
	

//#define TEST_MATRICES 1
#ifdef _TEST_MATRICES

	D3DXMATRIX dxComputedView;
	D3DXMATRIX dxComputedProj;

	if (m_mode == HKG_CAMERA_PERSPECTIVE)
	{
		const float fov = m_fov * 3.141592654f / 180.0f;
		const float h = 2.0f * m_near * hkg_tan( fov/2.0f );
		const float w = h * m_aspect;
		
		const float l = -w/2.0f;
		const float r = w/2.0f;
		const float b = h/2.0f;// -
		const float t = -h/2.0f;// +
		const float n = m_near;
		const float f = m_far;
		D3DXMatrixPerspectiveOffCenterLH( &dxComputedProj, l, r, t, b, n, f);
	}
	else
	{
		D3DXMatrixOrthoOffCenterLH( &dxComputedProj, m_oleft, m_oright, m_obottom, m_otop, m_onear, m_ofar);
	}


	D3DXVECTOR3 vf(m_from);
	vf[2] *= -1;
	D3DXVECTOR3 vt(m_to);
	vt[2] *= -1;
	D3DXVECTOR3 vu(m_up);
	vu[2] *= -1;
	D3DXMatrixLookAtLH( &dxComputedView, &vf, &vt, &vu );
	dxComputedView._31 *= -1;
	dxComputedView._32 *= -1;
	dxComputedView._33 *= -1;

	
	if (m_mode == HKG_CAMERA_PERSPECTIVE)
	{
		printf("--- PERSP MODE ---:\n");
	}
	else
	{
		printf("--- ORTHO MODE ---:\n");
	}


	{
		PRINTMAT( "View\t", m_viewMat );
		PRINTMAT( "Proj\t", m_projMat );

		PRINTMATDX( "My DXView", ((float*)&dxView) );
		PRINTMATDX( "Computed DXView", ((float*)dxComputedView) );
		PRINTMATDX( "My DXProj", ((float*)&dxProj) );
		PRINTMATDX( "Computed DXProj", ((float*)dxComputedProj) );
	}

#endif // TEST

	hkgWindowDX8* dxWin = static_cast<hkgWindowDX8*>( state->getOwner() );
	LPDIRECT3DDEVICE8 dev = dxWin->getDevice();


	dev->SetTransform( D3DTS_PROJECTION, &dxProj );
	dev->SetTransform( D3DTS_VIEW, &dxView );

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
