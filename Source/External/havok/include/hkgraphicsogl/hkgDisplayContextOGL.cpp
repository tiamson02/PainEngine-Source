/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsogl/hkgraphicsogl.h> // precompiled
#include <hkgraphicsogl/hkgDisplayContextOGL.h>

//
// Statics
//

hkgDisplayContextOGL::~hkgDisplayContextOGL()
{
}

hkgDisplayContextOGL::hkgDisplayContextOGL(hkgWindow* owner)
: hkgDisplayContext(owner), m_enabledVertexStreams( HKG_OGL_VTX_NONE )
{
	float ambient[4] = {0.1f,0.1f,0.1f,1.0f};
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ambient);
}

void hkgDisplayContextOGL::setLightingState(bool on)
{
	if (on)
	{
		glEnable(GL_LIGHTING);
		m_currentState |= HKG_ENABLED_LIGHTING;
	}
	else
	{
		glDisable(GL_LIGHTING);
		m_currentState &= ~HKG_ENABLED_LIGHTING;
	}
}

void hkgDisplayContextOGL::setTexture2DState(bool on)
{
	if (on)
	{
		glEnable(GL_TEXTURE_2D);
		m_currentState |= HKG_ENABLED_TEXTURE2D;
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
		m_currentState &= ~HKG_ENABLED_TEXTURE2D;
	}
}

void hkgDisplayContextOGL::setBlendState(bool on)
{
	if (on)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		m_currentState |= HKG_ENABLED_ALPHABLEND;
	}
	else
	{
		glDisable(GL_BLEND);
		m_currentState &= ~HKG_ENABLED_ALPHABLEND;
	}
}

void hkgDisplayContextOGL::setDepthReadState(bool on)
{
	if (on)
	{
#ifndef HK_PLATFORM_PS2
		glEnable(GL_DEPTH_TEST);
#endif
		m_currentState |= HKG_ENABLED_ZREAD;
	}
	else
	{
#ifndef HK_PLATFORM_PS2
		glDisable(GL_DEPTH_TEST);
#endif
		m_currentState &= ~HKG_ENABLED_ZREAD;
	}
}

void hkgDisplayContextOGL::setDepthWriteState(bool on)
{

	if (on)
	{
		glDepthMask( GL_TRUE );
		m_currentState |= HKG_ENABLED_ZWRITE;
	}
	else
	{
		glDepthMask( GL_FALSE );
		m_currentState &= ~HKG_ENABLED_ZWRITE;
	}	

}

void hkgDisplayContextOGL::setCullFaceState(bool on)
{
	if (on)
	{
		glEnable(GL_CULL_FACE);
#ifndef HK_PLATFORM_PS2
		glFrontFace(m_cullfaceMode == HKG_CULLFACE_CCW? GL_CCW : GL_CW );
#endif
		m_currentState |= HKG_ENABLED_CULLFACE;
	}
	else
	{
		glDisable(GL_CULL_FACE);
		m_currentState &= ~HKG_ENABLED_CULLFACE;
	}
}

void hkgDisplayContextOGL::setWireframeState(bool on)
{
	if (on)
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); 
		m_currentState |= HKG_ENABLED_WIREFRAME;
	}
	else
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); 
		m_currentState &= ~HKG_ENABLED_WIREFRAME;
	}
}

void hkgDisplayContextOGL::popMatrix()
{
	glPopMatrix();
}

void hkgDisplayContextOGL::pushMatrix()
{
	glPushMatrix();
}

void hkgDisplayContextOGL::multMatrix( float m[16] )
{	
	glMultMatrixf( m );
}

void hkgDisplayContextOGL::setCurrentColor3( float c[3])
{
	glColor3fv( c );
}

void hkgDisplayContextOGL::setCurrentColor4( float c[4])
{
	glColor4fv( c );
}

void  hkgDisplayContextOGL::setCurrentColorPacked( unsigned int c)
{
	glColor3ub( GLubyte( (c & 0x00ff0000) >> 16), 
				GLubyte( (c & 0x0000ff00) >> 8 ),
				GLubyte(c & 0x000000ff) );
}

void hkgDisplayContextOGL::beginGroup(HKG_IMM_GROUP type)
{
	switch ( type )
	{
		case HKG_IMM_LINES:
			glBegin( GL_LINES );
			break;
		case HKG_IMM_TRIANGLE_LIST:
			glBegin( GL_TRIANGLES );
			break;
		case HKG_IMM_TRIANGLE_STRIP:
			glBegin( GL_TRIANGLE_STRIP );
			break;
		case HKG_IMM_POINTS:
			glBegin( GL_POINTS );
			break;
		default: break;
	}
}

void hkgDisplayContextOGL::setCurrentTextureCoord(float uv[2])
{
	glTexCoord2fv( uv );
}

void hkgDisplayContextOGL::setCurrentNormal(float n[3])
{
	glNormal3fv( n );
}

void hkgDisplayContextOGL::setCurrentPosition(float pos[3])
{
	glVertex3fv( pos ); 
}

void hkgDisplayContextOGL::endGroup()
{
	glEnd();
}

void hkgDisplayContextOGL::flush()
{
	glFlush();
}

void hkgDisplayContextOGL::setLightState(int light, bool on)
{
	if (light < 8)
	{
		if (on)
			glEnable( (GLenum)(GL_LIGHT0+light) );
		else
			glDisable( (GLenum)(GL_LIGHT0+light) );
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
