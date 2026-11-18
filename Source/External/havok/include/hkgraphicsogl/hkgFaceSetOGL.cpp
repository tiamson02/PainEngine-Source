/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsogl/hkgraphicsogl.h> // precomp
#include <hkgraphicsogl/hkgFaceSetOGL.h>
#include <hkgraphicsogl/hkgVertexSetOGL.h>

hkgFaceSetOGL::~hkgFaceSetOGL()
{

}


void hkgFaceSetOGL::initialize()
{
#ifdef HK_PLATFORM_PS2

	// As PS2GL does not have index draw mode (glDrawElements) 
	// we must redo the vertex buffers so that they have no connetivity
	// information, just big vertex lists for glDrawArrays
	
	
	int nm = getNumPrimitives();
	int m;
	for (m = 0; m < nm; ++m)
	{
		const hkgFaceSetPrimitive* prim = getPrimitive(m);	
		if (prim->getIndices())
			break;
	}
	
	if ( m < nm ) // found a prim that has indices so must redo vertex buffer
		removeIndexArrays();
		
#endif
}

#define USE_DRAWELEMS

void hkgFaceSetOGL::render(hkgDisplayContext* state)
{
#ifdef USE_DRAWELEMS
	m_vertices->setStreams(state);
#endif

	hkgVertexSetOGL* oglVerts = static_cast<hkgVertexSetOGL*>( m_vertices );
	
	bool needTextureCoords = (state->getVertexOptions() & HKG_VTX_DISABLE_TEXTURE2D) == 0;
	bool needVtxColors = (state->getVertexOptions() & HKG_VTX_DISABLE_COLOR) == 0;

	int nm = getNumPrimitives();
	for (int m = 0; m < nm; ++m)
	{
		const hkgFaceSetPrimitive* prim = getPrimitive(m);
		const unsigned short* conn = prim->getIndices();
		int offset = conn? 0 : prim->getVertexBufferStartIndex();
	
#ifndef USE_DRAWELEMS

		glBegin( prim->getType() == HKG_TRI_LIST? GL_TRIANGLES : GL_TRIANGLE_STRIP );

			for ( int n = offset; n < (prim->getLength() + offset); ++n)
			{
				const short c = conn? conn[n] : n;
				
				//if (needTextureCoords && m_vertices->getUVArray() )
				//	glTexCoord2fv(oglVerts->getUVArray(c));
				//if (needVtxColor && m_vertices->getColorArray() )
				//	glColor3fv( oglVerts->getColorArray(c));

				glNormal3fv(oglVerts->getNormalArray(c));
				glVertex3fv(oglVerts->getV->getVertArray(c));
			}

		glEnd();
	
#else 
		if (conn)
			glDrawElements( prim->getType() == HKG_TRI_LIST? GL_TRIANGLES : GL_TRIANGLE_STRIP,
				prim->getLength(), GL_UNSIGNED_SHORT, conn );
		else
		{
			int numverts = prim->getLength() ;
			
			glDrawArrays( prim->getType() == HKG_TRI_LIST? GL_TRIANGLES : GL_TRIANGLE_STRIP,
				offset, numverts);//min(numverts,32768));
		}
#endif
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
