/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsgcn/hkgraphicsgcn.h> // precompiled
#include <hkgraphicsgcn/hkgVertexSetGCN.h>
#include <hkgraphicsgcn/hkgDisplayContextGCN.h>

const unsigned short THREE_FLOATS = sizeof(float)*3;
const unsigned short TWO_FLOATS = sizeof(float)*2;

hkgVertexSetGCN::~hkgVertexSetGCN()
{
	setNumVerts(0);
}


void hkgVertexSetGCN::setNumVerts(int nv, bool uvs, bool colors)
{
	m_vertexFormat = HKG_VERTEXSET_POS | HKG_VERTEXSET_NORMAL;
	
	if (m_vertBuf)
		hkAlignedDeallocate<float>(m_vertBuf);
		
	if (m_normBuf)
		hkAlignedDeallocate<float>(m_normBuf);
		
	if (m_colorBuf)
		hkAlignedDeallocate<u8>(m_colorBuf);		
		
	if (m_uvBuf)
		hkAlignedDeallocate<float>(m_uvBuf);
		
	m_stride = sizeof(f32)*3; // pos stride in bytes
	m_numVerts = nv;

	if (m_numVerts > 0) 
	{
		// want it 32byte aligned data for speed (v cache is 32b) so wont go through hkMemory			
		m_vertBuf = hkAlignedAllocate<float>( 32, 3*m_numVerts, HK_MEMORY_CLASS_DISPLAY ); 
		m_normBuf = hkAlignedAllocate<float>( 32, 3*m_numVerts, HK_MEMORY_CLASS_DISPLAY );
	}
	else
	{
		m_vertBuf = (float*)HK_NULL;
		m_normBuf = (float*)HK_NULL;
	}
	
	if (colors) 
	{
		m_vertexFormat |= HKG_VERTEXSET_COLOR;
		m_colorBuf = hkAlignedAllocate<u8>( 32, 4*m_numVerts, HK_MEMORY_CLASS_DISPLAY );
	}
	else
		m_colorBuf = HK_NULL;
	
	if (uvs)
	{
		m_vertexFormat |= HKG_VERTEXSET_UV;
		m_uvBuf = hkAlignedAllocate<float>( 32, 2*m_numVerts, HK_MEMORY_CLASS_DISPLAY );
	}
	else
		m_uvBuf = HK_NULL;
	
	resetCounters();
}


void hkgVertexSetGCN::setStreams(hkgDisplayContext* state)
{
    
	GXClearVtxDesc();
	
	GXSetVtxDesc(GX_VA_POS, GX_INDEX16 );  // index into pos array (floats)
	GXSetArray(GX_VA_POS, m_vertBuf, 3*sizeof(f32));	

	GXSetVtxDesc(GX_VA_NRM, GX_INDEX16 );  // index into pos array (floats)
	GXSetArray(GX_VA_NRM, m_normBuf, 3*sizeof(f32));	
           
    // Clr?
    if (m_vertexFormat & HKG_VERTEXSET_COLOR)
	{
		GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
	    GXSetArray(GX_VA_CLR0, m_colorBuf, 4*sizeof(u8)); // RGBA u8
	}
    
    // UV?
    if (m_vertexFormat & HKG_VERTEXSET_UV)
	{
		GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16); 
        GXSetArray(GX_VA_TEX0, m_uvBuf, 2*sizeof(f32)); // float uv 
    }
}

void hkgVertexSetGCN::addPosInfo(const float* posInfo, int numPos)
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before altering.");
	HK_ASSERT( m_vertBuf && ((numPos + m_lastAddedPosIndex) <= m_numVerts) );

	for (unsigned int i =0; i < numPos; ++i)
	{
		float* p = &(m_vertBuf[ m_lastAddedPosIndex*3 ]);
		hkgVec3Copy(p, &(posInfo[i*3]));
		m_lastAddedPosIndex++;
	}
}

void hkgVertexSetGCN::addNormalInfo(const float* normalInfo, int numNormals)
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before altering.");
	HK_ASSERT( m_normBuf && ((numNormals + m_lastAddedNormalIndex) <= m_numVerts) );

	for (unsigned int i =0; i < numNormals; ++i)
	{
		float* n = &(m_normBuf[m_lastAddedNormalIndex*3]);
		hkgVec3Copy( n, &(normalInfo[i*3]) );
		m_lastAddedNormalIndex++;
	}	
}

void hkgVertexSetGCN::addColorInfo(const float* colorInfo, int numColors)
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before altering.");
	HK_ASSERT( m_colorBuf && ((numColors + m_lastAddedColorIndex) <= m_numVerts) );

	for (unsigned int i =0; i < numColors; ++i)
	{
		const float* ci = &colorInfo[i*3];
		u8* c = &(m_colorBuf[m_lastAddedColorIndex*4]);
		c[0] = ci[0] * 255.0f;
		c[1] = ci[1] * 255.0f;
		c[2] = ci[2] * 255.0f;
		c[3] = 255.0f;
		m_lastAddedColorIndex++;
	}
}

void hkgVertexSetGCN::addUVInfo(const float* uvInfo, int numUVs)
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before altering.");
	HK_ASSERT( m_uvBuf && (( numUVs + m_lastAddedUVIndex) <= m_numVerts) );

	for (unsigned int i =0; i < numUVs; ++i)
	{
		float* uv = &(m_uvBuf[m_lastAddedUVIndex*2]);
		uv[0] = uvInfo[i*2]; 
		uv[1] = uvInfo[i*2 + 1]; 
		m_lastAddedUVIndex++;
	}
}

unsigned char*  hkgVertexSetGCN::lock(int offset, int len)
{
	HK_ASSERT( m_vertBuf != HK_NULL );
	m_lockedBuf = (unsigned char*)m_vertBuf;
	return m_lockedBuf;
}

void hkgVertexSetGCN::unlock(int offset, int len)
{
	HK_ASSERT( m_vertBuf != HK_NULL );
	
	if (m_lockedBuf) // we may have written to the buffer so need to flush our local cache before rendering
	{
		unsigned int sov = m_numVerts*sizeof(f32);
		if (m_vertBuf)
			DCStoreRange(m_vertBuf, sov*3);
			
		if (m_normBuf)
			DCStoreRange(m_normBuf, sov*3);
			
		if (m_colorBuf)
			DCStoreRange(m_colorBuf, m_numVerts*sizeof(u8)*4);
		
		if (m_uvBuf)
			DCStoreRange(m_uvBuf, sov*2);
	}
	
	m_lockedBuf = HK_NULL;
}



float* hkgVertexSetGCN::getPosInfo( int i )
{
	HK_ASSERT( m_lockedBuf && m_vertBuf ); // locked
	return &m_vertBuf[i*3];
}

float* hkgVertexSetGCN::getNormalInfo( int i )
{
	HK_ASSERT( m_lockedBuf && m_normBuf ); // locked and have normals
	return &m_normBuf[i*3];
}

float* hkgVertexSetGCN::getColorInfo( int i )
{
	HK_ASSERT( 0 && "Unsported on GameCube at the moment" && m_colorBuf && m_lockedBuf );// locked and have colors
	return reinterpret_cast<float*>(&m_colorBuf[i]);
}

float* hkgVertexSetGCN::getUVInfo( int i )
{
	HK_ASSERT( m_uvBuf && m_lockedBuf ); // locked and have uvs
	return &m_uvBuf[i*2];
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
