/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsdx8/hkgraphicsdx8.h> // precompiled
#include <hkgraphicsdx8/hkgVertexSetDX8.h>
#include <hkgraphicsdx8/hkgDisplayContextDX8.h>
#include <hkgraphicsdx8/hkgFaceSetDX8.h>

hkgVertexSetDX8::~hkgVertexSetDX8()
{
	setNumVerts(0);
}

void hkgVertexSetDX8::setNumVerts(int s, bool uvs, bool colors)
{
	LPDIRECT3DDEVICE8 dev = static_cast<hkgFaceSetDX8*>(m_owner)->getDevice();

	if (m_pVB)
		m_pVB->Release();


	m_vertexFormat = HKG_VERTEXSET_POS | HKG_VERTEXSET_NORMAL;
	m_fvf = ( D3DFVF_XYZ | D3DFVF_NORMAL );
	m_stride = 6* sizeof(float);
	m_numVerts = s;

	m_normalFVFPos = 3*sizeof(float);
	
	if (colors) 
	{
		m_vertexFormat |= HKG_VERTEXSET_COLOR;
		m_fvf |= D3DFVF_DIFFUSE;
		m_stride += sizeof(DWORD);

		m_colorFVFPos =  6*sizeof(float); // x y z nx ny nz
	}
	else
		m_colorFVFPos = 0;

	
	if (uvs)
	{
		m_vertexFormat |= HKG_VERTEXSET_UV;
		m_fvf |= D3DFVF_TEX1;
		m_stride += 2* sizeof(float);
		m_uvFVFPos = ( colors? m_colorFVFPos + sizeof(DWORD) : 6*sizeof(float) );
	}
	else
		m_uvFVFPos = 0;
	
	if (m_numVerts)
	{
	 	HRESULT hr = dev->CreateVertexBuffer( m_numVerts * m_stride, 0, m_fvf, D3DPOOL_MANAGED, &m_pVB);
		HK_ASSERT( hr == D3D_OK );
	}
	else
		m_pVB = HK_NULL;

	resetCounters();
}

void hkgVertexSetDX8::addPosInfo(const float* posInfo, int numPos)
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before altering.");
	HK_ASSERT( (numPos + m_lastAddedPosIndex) <= m_numVerts);

	for (int i =0; i < numPos; ++i)
	{
		float* p = reinterpret_cast<float*>( &(m_lockedBuf[m_lastAddedPosIndex*m_stride]) );
		hkgVec3Copy(p, &(posInfo[i*3]));
		m_lastAddedPosIndex++;
	}
}

void hkgVertexSetDX8::addUVInfo(const float* uvInfo, int numUVs)
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before altering.");
	HK_ASSERT( m_uvFVFPos && ( ( numUVs + m_lastAddedUVIndex) <= m_numVerts) );

	for (int i =0; i < numUVs; ++i)
	{
		float* uv = reinterpret_cast<float*>( &(m_lockedBuf[m_lastAddedUVIndex*m_stride + m_uvFVFPos]) );
		uv[0] = uvInfo[i*2]; 
		uv[1] = uvInfo[i*2 + 1]; 
		m_lastAddedUVIndex++;
	}
}

void hkgVertexSetDX8::addColorInfo(const float* colorInfo, int numColors)
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before altering.");
	HK_ASSERT( m_colorFVFPos && (numColors + m_lastAddedColorIndex) <= m_numVerts);

	for (int i =0; i < numColors; ++i)
	{
		float* c = reinterpret_cast<float*>( &(m_lockedBuf[m_lastAddedColorIndex*m_stride + m_colorFVFPos]) );
		hkgVec3Copy( c, &(colorInfo[i*3]) );
		m_lastAddedColorIndex++;
	}
}

void hkgVertexSetDX8::addNormalInfo(const float* normalInfo, int numNormals)
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before altering.");
	HK_ASSERT( m_normalFVFPos && (numNormals + m_lastAddedNormalIndex) <= m_numVerts);

	for (int i =0; i < numNormals; ++i)
	{
		float* n = reinterpret_cast<float*>( &(m_lockedBuf[m_lastAddedNormalIndex*m_stride + m_normalFVFPos]) );
		hkgVec3Copy( n, &(normalInfo[i*3]) );
		m_lastAddedNormalIndex++;
	}	
}

unsigned char*  hkgVertexSetDX8::lock(int offset, int len)
{
	HK_ASSERT( m_pVB != HK_NULL );
	
	BYTE* bptr; 
	int blen = len < 0? (m_numVerts-offset)*m_stride : len*m_stride;
	m_pVB->Lock(offset*m_stride, blen, &bptr, 0); // full lock, no discard
	m_lockedBuf = bptr;
	return m_lockedBuf;
}

void hkgVertexSetDX8::unlock(int offset, int len)
{
	HK_ASSERT( m_pVB != HK_NULL );
	m_pVB->Unlock();
	m_lockedBuf = HK_NULL;
}

void hkgVertexSetDX8::setStreams(hkgDisplayContext* context)
{
	HK_ASSERT( m_pVB != HK_NULL );
	
	LPDIRECT3DDEVICE8 dev = static_cast<hkgFaceSetDX8*>(m_owner)->getDevice();
	dev->SetVertexShader( m_fvf );
	dev->SetStreamSource( 0, m_pVB, m_stride );

	if (m_colorFVFPos)
	{
		dev->SetRenderState( D3DRS_COLORVERTEX, TRUE);
		dev->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
	}
	else
	{
		dev->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
		dev->SetRenderState( D3DRS_COLORVERTEX, FALSE);
	}
}

float* hkgVertexSetDX8::getPosInfo( int i )
{
	HK_ASSERT( m_lockedBuf ); // locked
	return reinterpret_cast<float*>(&m_lockedBuf[ (i * m_stride) ]);
}

float* hkgVertexSetDX8::getUVInfo( int i )
{
	HK_ASSERT( m_lockedBuf && m_uvFVFPos ); // locked and have uvs
	return reinterpret_cast<float*>(&m_lockedBuf[ (i * m_stride) + m_uvFVFPos ]);
}

float* hkgVertexSetDX8::getColorInfo( int i )
{
	HK_ASSERT( m_lockedBuf && m_colorFVFPos);// locked and have colors
	return reinterpret_cast<float*>(&m_lockedBuf[ (i * m_stride) + m_colorFVFPos ]);
}

float* hkgVertexSetDX8::getNormalInfo( int i )
{
	HK_ASSERT( m_lockedBuf && m_normalFVFPos ); // locked and have normals
	return reinterpret_cast<float*>(&m_lockedBuf[ (i * m_stride) + m_normalFVFPos ]);
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
