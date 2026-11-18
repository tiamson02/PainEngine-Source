/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsogl/hkgraphicsogl.h> // precompiled

#include <GL/gl.h>

void glColor3ub( GLubyte r, GLubyte g, GLubyte b)
{
	glColor3f( r / 255.0f, g / 255.0f, b / 255.0f);
}

// its just that the ps2 libs compiled for the sony compiler are
// a slighly newer version and have definitions for the following.
#if !defined(HK_COMPILER_GCC_VERSION) || HK_COMPILER_GCC_VERSION < 30100
void glDrawElements( GLenum mode, GLsizei count,GLenum type, const GLvoid *indices )
{
	HK_ASSERT( 0 && "Can't do glDrawElements in PS2GL");
}

void glColor4f( GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glColor3f( r, g, b);
}

GLenum glGetError( void )
{
	return GL_NO_ERROR;
}

void glColor4fv( const GLfloat* rgba)
{
	glColor3f( rgba[0], rgba[1], rgba[2] );
}
#endif


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
