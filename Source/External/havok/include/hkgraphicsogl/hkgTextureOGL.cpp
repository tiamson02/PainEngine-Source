/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsogl/hkgraphicsogl.h> // precompiled
#include <hkgraphicsogl/hkgTextureOGL.h>

hkgTextureOGL::~hkgTextureOGL()
{
	if ( m_glID != 0 )
		free();
}

bool hkgTextureOGL::realize()
{
	if (m_glID) 
		return true; // already done.

	glGetError(); // clear error flags

	if (!hasData() || m_clut != HK_NULL)
		return false;

	glGenTextures(1, &m_glID);
	glBindTexture(GL_TEXTURE_2D, m_glID);

	if (m_hasAlpha)
		glTexImage2D(GL_TEXTURE_2D, 0, 4, m_width, m_height, 0,                
					GL_RGBA, GL_UNSIGNED_BYTE, m_data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, 3, m_width, m_height, 0,
					GL_RGB, GL_UNSIGNED_BYTE, m_data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 

	return (GL_NO_ERROR == glGetError());
}

void hkgTextureOGL::bind() const
{
	glBindTexture( GL_TEXTURE_2D, m_glID ); 
}

void hkgTextureOGL::free()
{
	if (m_glID)
	{
		glDeleteTextures(1, &m_glID);
		m_glID = 0;
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
