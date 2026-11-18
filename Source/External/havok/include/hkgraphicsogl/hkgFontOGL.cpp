/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsogl/hkgraphicsogl.h> // precomp
#include <hkgraphicsogl/hkgFontOGL.h>
 
static float white[4] = {1.0f, 1.0f, 1.0f, 0.5f};

void hkgFontOGL::render(hkgDisplayContext* context, const char* text, int origx, int origy, float color[4] )
{
	// make sure there is at least one printable character in text
	// ps2gl freaks out if we send it a zero length tri list.
	{
		int k = 0;
		while(text[k] != HK_NULL)
		{
			if(text[k] != '\n')
			{
				break; // we're good!
			}
			k++;
		}
		
		if(text[k] == HK_NULL)
		{
			return; // early return, the above loop didn't break early.
		}
	}

	glColor4f( color[0], color[1], color[2], color[3] );
	
	if (m_texture)
	{
		context->setCurrentTexture( m_texture );
	}
	
	float u0, v0, u1, v1;
	const GLfloat depth = -0.01f;

	int x = origx;
	int y = origy;

	
	glBegin( GL_TRIANGLES );

	int k = 0;	
	while (text[k] != HK_NULL)
	{
		if( text[k] != '\n' )
		{
			resolve( text[k], u0, v0, u1, v1);

   			glTexCoord2f(u0, v0);
			glVertex3f(GLfloat(x), GLfloat(y), depth);
			
			glTexCoord2f(u1, v0);
			glVertex3f(GLfloat(x+m_charWidth), GLfloat(y), depth);

			glTexCoord2f(u0, v1);
			glVertex3f(GLfloat(x), GLfloat(y+m_charHeight), depth);

			glTexCoord2f(u0, v1);
			glVertex3f(GLfloat(x), GLfloat(y+m_charHeight), depth);

			glTexCoord2f(u1, v0);
			glVertex3f(GLfloat(x+m_charWidth), GLfloat(y), depth);

			glTexCoord2f(u1, v1);
		    glVertex3f(GLfloat(x+m_charWidth), GLfloat(y+m_charHeight), depth);

			x += m_charWidth;
		}
		else
		{
			x = origx;
			y -= m_charHeight;
		}
		
		++k;
	}	
	
	glEnd();
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
