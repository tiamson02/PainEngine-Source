/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsgcn/hkgraphicsgcn.h> // precomp
#include <hkgraphicsgcn/hkgFontGCN.h>
#include <hkBase/string/hkString.h>
 
static float white[4] = {1.0f, 1.0f, 1.0f, 0.5f};

void hkgFontGCN::render(hkgDisplayContext* context, const char* text, int origx, int origy, float color[4] )
{
	GXColor c;
	c.r = color[0] * 255;
	c.g = color[1] * 255;
	c.b = color[2] * 255;
	c.a = color[3] * 255;
	
	GXSetChanMatColor(GX_COLOR0A0, c);
	
	if (text == HK_NULL || text[0] == '\0')
		return;
	
	if (m_texture)
		context->setCurrentTexture( m_texture );
	
	GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT); 

	// find actual (renderable) string length    
	int len = 0;
	int k = 0;
		
	while ( text[k] )
	{
		char c = text[k++];
		
		if (c != '\n')
			++len;
	}
	
	if (len < 1)
		return;

	k = 0;
  	float u0, v0, u1, v1;
	GXBegin( GX_TRIANGLES, GX_VTXFMT1, len*6);
	
		const float depth = -0.001f;
		int x = origx;
		int y = origy;
		while(text[k])
		{	
			if( text[k] != '\n' )
			{
				resolve( text[k], u0, v0, u1, v1);

				GXPosition3f32(x, y, depth);
				GXTexCoord2f32(u0, v0);
				
				GXPosition3f32(x+m_charWidth, y, depth);
				GXTexCoord2f32(u1, v0);
				
				GXPosition3f32(x, y+m_charHeight, depth);
				GXTexCoord2f32(u0, v1);
				
				GXPosition3f32(x, y+m_charHeight, depth);
				GXTexCoord2f32(u0, v1);
				
				GXPosition3f32(x+m_charWidth, y, depth);
				GXTexCoord2f32(u1, v0);
				
				GXPosition3f32(x+m_charWidth, y+m_charHeight, depth);
				GXTexCoord2f32(u1, v1);

				x += m_charWidth;
			}
			else
			{
				x = origx;
				y -= m_charHeight;
			}

			++k;
        }
 
	GXEnd();
	
		
/*

	glBegin( GL_TRIANGLES );

	int k = 0;
	while (text[k])
	{
		resolve( text[k], u0, v0, u1, v1);

   	    glTexCoord2f(u0, v0);
	    glVertex3f(x+m_charWidth*k, y, 0);
	    
	    glTexCoord2f(u1, v0);
	    glVertex3f(x+m_charWidth*(k+1), y, 0);

	    glTexCoord2f(u0, v1);
	    glVertex3f(x+m_charWidth*k, y+m_charHeight, 0);

		glTexCoord2f(u0, v1);
	    glVertex3f(x+m_charWidth*k, y+m_charHeight, 0);

	    glTexCoord2f(u1, v0);
	    glVertex3f(x+m_charWidth*(k+1), y, 0);

	    glTexCoord2f(u1, v1);
	    glVertex3f(x+m_charWidth*(k+1), y+m_charHeight, 0);

		++k;
	}	

	glEnd();
*/
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
