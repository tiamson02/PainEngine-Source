/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsdx8/hkgraphicsdx8.h> // precomp
#include <hkgraphicsdx8/hkgFontDX8.h>
#include <hkgraphicsdx8/hkgDisplayContextDX8.h>

static float white[4] = {1.0f, 1.0f, 1.0f, 0.5f};

struct VertInfo
{
	float x,y,z;
	DWORD c;
	float u, v;
};
 

void hkgFontDX8::render(hkgDisplayContext* context, const char* text, int origx, int origy, float color[4] )
{
	unsigned int len = hkString::strLen( text );
	if (len == 0)
		return;

	if (m_texture)
		context->setCurrentTexture( m_texture );

	VertInfo* verts = hkAllocate<VertInfo>( len*6, HK_MEMORY_CLASS_DISPLAY );

	int k = 0;
	DWORD dc = hkgColor4FromVec4( color);

	const float depth = -0.001f;
	float u0, v0, u1, v1;
	unsigned int index = 0;
	int x = origx;
	int y = origy;
	while (text[k])
	{
		if( text[k] != '\n' )
		{
			resolve( text[k], u0, v0, u1, v1);

			//glTexCoord2f(u0, v0);
			//glVertex3f(x+m_charWidth*k, y, 0);
			VertInfo& vert0 = verts[index++];
			vert0.x = (float)x;
			vert0.y = (float)y;
			vert0.z = depth;
			vert0.c = dc;
			vert0.u = u0;
			vert0.v = v0;
			
			//glTexCoord2f(u1, v0);
			//glVertex3f(x+m_charWidth*(k+1), y, 0);
			VertInfo& vert1 = verts[index++];
			vert1.x = (float)x+m_charWidth;
			vert1.y = (float)y;
			vert1.z = depth;
			vert1.c = dc;
			vert1.u = u1;
			vert1.v = v0;
			
			//glTexCoord2f(u0, v1);
			//glVertex3f(x+m_charWidth*k, y+m_charHeight, 0);
			VertInfo& vert2 = verts[index++];
			vert2.x = (float)x;
			vert2.y = (float)y+m_charHeight;
			vert2.z = depth;
			vert2.c = dc;
			vert2.u = u0;
			vert2.v = v1;
			
			//glTexCoord2f(u0, v1);
			//glVertex3f(x+m_charWidth*k, y+m_charHeight, 0);
			VertInfo& vert3 = verts[index++];
			vert3 = vert2;
			
			//glTexCoord2f(u1, v0);
			//glVertex3f(x+m_charWidth*(k+1), y, 0);
			VertInfo& vert4 = verts[index++];
			vert4 = vert1;
			
			//glTexCoord2f(u1, v1);
			//glVertex3f(x+m_charWidth*(k+1), y+m_charHeight, 0);
			VertInfo& vert5 = verts[index++];
			vert5.x = (float)x+m_charWidth;
			vert5.y = (float)y+m_charHeight;
			vert5.z = depth;
			vert5.c = dc;
			vert5.u = u1;
			vert5.v = v1;

			x += m_charWidth;
		}
		else
		{
			x = origx;
			y -= m_charHeight;
		}

		++k;
	}	
	
	DWORD dwFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	LPDIRECT3DDEVICE8 dev = static_cast<hkgDisplayContextDX8*>( context )->getDevice();
		
	dev->SetVertexShader(dwFVF); // fixed function pipeline
	dev->DrawPrimitiveUP(
			D3DPT_TRIANGLELIST,
			len * 2, // two tris per letter
			verts,
			sizeof(VertInfo) );

	hkDeallocate<VertInfo>( verts );
}

/*
	glColor4f( color[0], color[1], color[2], color[3] );
	
	if (m_texture)
		context->setCurrentTexture( m_texture );
	
	float u0, v0, u1, v1;
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
