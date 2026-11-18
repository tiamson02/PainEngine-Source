/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsogl/hkgraphicsogl.h> // precompiled
#include <hkgraphicsogl/hkgVertexSetOGL.h>
#include <hkgraphicsogl/hkgDisplayContextOGL.h>

hkgVertexSetOGL::~hkgVertexSetOGL()
{
	setNumVerts(0);
}

void hkgVertexSetOGL::setNumVerts(int s, bool uvs, bool colors)
{
	if (m_numVerts > 0)
		hkDeallocate<float>( m_vertsPos );	

	if ( s < 1 )
	{
		m_vertsPos = m_vertsUVs = m_vertsColors = m_vertsNormals = HK_NULL;
		m_numVerts = 0;
		return;
	}

	m_numVerts = s;
	m_vertsPos = hkAllocate<float>( m_numVerts * ( 6 + (uvs?2:0) + (colors?3:0) ), HK_MEMORY_CLASS_DISPLAY); // p3 n3 uv2 c3 == 11 floats per vert 
	m_vertsNormals = m_vertsPos + m_numVerts*3;
	
	m_vertexFormat = HKG_VERTEXSET_POS | HKG_VERTEXSET_NORMAL;
	

	if (uvs)
	{
		m_vertsUVs = m_vertsNormals + m_numVerts*3;
		m_vertexFormat |= HKG_VERTEXSET_UV;
	}
	else
		m_vertsUVs = HK_NULL;

	if (colors) 
	{
		m_vertsColors = (uvs? m_vertsUVs + m_numVerts*2 : m_vertsNormals + m_numVerts*3 );
		m_vertexFormat |= HKG_VERTEXSET_COLOR;
	}
	else
		m_vertsColors = HK_NULL;


	

	resetCounters();
}

void hkgVertexSetOGL::addPosInfo(const float* posInfo, int numPos)
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before altering.");
	HK_ASSERT( (numPos + m_lastAddedPosIndex) <= m_numVerts);
	
	int s = numPos*3;
	hkString::memCpy( m_vertsPos + (m_lastAddedPosIndex*3), posInfo, s * sizeof(float));
	m_lastAddedPosIndex += numPos;
}

void hkgVertexSetOGL::addUVInfo(const float* uvInfo, int numUVs)
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before altering.");
	HK_ASSERT( m_vertsUVs && (numUVs + m_lastAddedUVIndex) <= m_numVerts);
	
	int s = numUVs*2;
	hkString::memCpy( m_vertsUVs + (m_lastAddedUVIndex*2), uvInfo, s * sizeof(float));
	m_lastAddedUVIndex += numUVs;
}

void hkgVertexSetOGL::addColorInfo(const float* colorInfo, int numColors)
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before altering.");
	HK_ASSERT( m_vertsColors && ( (numColors + m_lastAddedColorIndex) <= m_numVerts) );
	
	int s = numColors*3;
	hkString::memCpy( m_vertsColors + (m_lastAddedColorIndex*3), colorInfo, s * sizeof(float));
	m_lastAddedColorIndex += numColors;
}

void hkgVertexSetOGL::addNormalInfo(const float* normalInfo, int numNormals)
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before altering.");
	HK_ASSERT( (numNormals + m_lastAddedNormalIndex) <= m_numVerts);

	int s = numNormals*3;
	hkString::memCpy( m_vertsNormals + (m_lastAddedNormalIndex*3), normalInfo, s  * sizeof(float));
	m_lastAddedNormalIndex += numNormals;
}

unsigned char*  hkgVertexSetOGL::lock(int offset, int len)
{
	// does nothing
	m_lockedBuf = (unsigned char*)( m_vertsPos );
	return m_lockedBuf;
}

void hkgVertexSetOGL::unlock(int offset, int len)
{
	m_lockedBuf = HK_NULL;
	// does nothing
}

void hkgVertexSetOGL::setStreams(hkgDisplayContext* context)
{
	hkgDisplayContextOGL* dcOGL = static_cast<hkgDisplayContextOGL*>( context );

	HKG_VERTEX_STREAMS enabledState = dcOGL->getStreamState();

	// pos and normal
	
	if ( (enabledState & HKG_OGL_VTX_POS) == 0) 
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		enabledState |= HKG_OGL_VTX_POS;
	}

	if ( (enabledState & HKG_OGL_VTX_NORMAL) == 0) 
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		enabledState |= HKG_OGL_VTX_NORMAL;
	}

	glVertexPointer( 3, GL_FLOAT, 0, m_vertsPos );
	glNormalPointer( GL_FLOAT, 0, m_vertsNormals );

	// Texture coords
	if (m_vertsUVs && !(context->getVertexOptions() & HKG_VTX_DISABLE_TEXTURE2D) )
	{
		if ( (enabledState & HKG_OGL_VTX_UV) == 0)
		{
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			enabledState |= HKG_OGL_VTX_UV;
		}
		glTexCoordPointer( 2, GL_FLOAT, 0, m_vertsUVs );
	}
	else if (enabledState & HKG_OGL_VTX_UV)
	{
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		enabledState &= ~HKG_OGL_VTX_UV;
	}
	

	// Vertex Colors
	if (m_vertsColors && !(context->getVertexOptions() & HKG_VTX_DISABLE_COLOR))
	{
		if ( (enabledState & HKG_OGL_VTX_COLOR) == 0)
		{
			glEnableClientState( GL_COLOR_ARRAY );
			enabledState |= HKG_OGL_VTX_COLOR;
		}

		glColorPointer( 3, GL_FLOAT, 0, m_vertsColors );
	}
	else if (enabledState & HKG_OGL_VTX_COLOR)
	{
		glDisableClientState( GL_COLOR_ARRAY );
		enabledState &= ~HKG_OGL_VTX_COLOR;
	}	

	dcOGL->setStreamState( enabledState );
}

float* hkgVertexSetOGL::getPosInfo( int i )
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before referencing.");
	return getPosArray(i);
}

float* hkgVertexSetOGL::getUVInfo( int i )
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before referencing.");
	return getUVArray(i);
}

float* hkgVertexSetOGL::getColorInfo( int i )
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before referencing.");
	return getColorArray(i);
}

float* hkgVertexSetOGL::getNormalInfo( int i )
{
	HK_ASSERT( m_lockedBuf && "Must lock vertex set before referencing.");
	return getNormalArray(i);
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
