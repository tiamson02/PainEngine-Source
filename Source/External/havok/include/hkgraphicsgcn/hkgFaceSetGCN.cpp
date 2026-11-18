/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsgcn/hkgraphicsgcn.h> // precomp
#include <hkgraphicsgcn/hkgFaceSetGCN.h>
#include <hkgraphicsgcn/hkgVertexSetGCN.h>

#include <stdio.h>

hkgFaceSetGCN::~hkgFaceSetGCN()
{

}


void hkgFaceSetGCN::initialize()
{
}

static inline void _drawVertex(u16 index, bool c, bool uv)
{
	GXPosition1x16(index);  
	GXNormal1x16(index);  
	if (c)
		GXColor1x16(index);
	if (uv)
		GXTexCoord1x16(index);				
}

void hkgFaceSetGCN::drawWireframe(hkgDisplayContext* state, bool c, bool uv)
{
		
	int nm = getNumPrimitives();
	
	for (int m = 0; m < nm; ++m)
	{
		const hkgFaceSetPrimitive* prim = getPrimitive(m);
		const unsigned short* conn = prim->getIndices();
		unsigned int offset = conn? 0 : prim->getVertexBufferStartIndex();
		unsigned int len = prim->getLength();
		bool tristrip = prim->getType() != HKG_TRI_LIST;
		unsigned int numTri = tristrip? len-2 : len/3;
		
		// for each triangle draw a line strip (slow, but ok for now)
		
		unsigned int v = tristrip? 2 : 0; 
		u16 i0 = conn? conn[0] : 0+offset;
		u16 i1 = conn? conn[1] : 1+offset;
		u16 i2;
		
		for (unsigned int t=0; t < numTri; ++t)
		{
			GXBegin( GX_LINESTRIP, GX_VTXFMT0, 4);
		
				if (tristrip)
				{
					i2 = conn? conn[v] : v+offset;
					
					_drawVertex( i0,c,uv );
					_drawVertex( i1,c,uv );
					_drawVertex( i2,c,uv );
					_drawVertex( i0,c,uv );
					
					i0 = i1;
					i1 = i2;			
					
					++v;		
				}
				else
				{
					i0 = conn? conn[v] : v+offset;
					
					for (unsigned int k=0; k <3;++k)
					{
						i1 = conn? conn[v] : v+offset;
						_drawVertex( i1,c,uv );
						++v;
					}
					
					// close it back again:
					_drawVertex( i0,c,uv );
				}					
				
			GXEnd();
		}
	}
}

void hkgFaceSetGCN::render(hkgDisplayContext* state)
{
	hkgVertexSetGCN* gcnVerts = static_cast<hkgVertexSetGCN*>( m_vertices );
	
	gcnVerts->setStreams(state);

	bool needTextureCoords = (state->getVertexOptions() & HKG_VTX_DISABLE_TEXTURE2D) == 0;
	bool needVtxColors = (state->getVertexOptions() & HKG_VTX_DISABLE_COLOR) == 0;
	
	bool uv = /*needTextureCoords &&*/ (gcnVerts->getVertexFormat() & HKG_VERTEXSET_UV);
	bool c = /*needVtxColors &&*/ (gcnVerts->getVertexFormat() & HKG_VERTEXSET_COLOR);

	bool wireframe = state->getEnabledState() & HKG_ENABLED_WIREFRAME;
	if (wireframe)
	{
		drawWireframe(state, c, uv);
		return;
	}
		
	int nm = getNumPrimitives();
	for (int m = 0; m < nm; ++m)
	{
		const hkgFaceSetPrimitive* prim = getPrimitive(m);
		const unsigned short* conn = prim->getIndices();
		unsigned int offset = conn? 0 : prim->getVertexBufferStartIndex();
		unsigned int len = prim->getLength();
		
		GXBegin( prim->getType() == HKG_TRI_LIST? GX_TRIANGLES : GX_TRIANGLESTRIP, GX_VTXFMT0, len);
	
			for (unsigned int v=0; v<len; ++v)
			{
				u16 index = conn? conn[v] : v+offset;
			
				GXPosition1x16(index);  
				GXNormal1x16(index);  
				
				if (c)
					GXColor1x16(index);
					
				if (uv)
					GXTexCoord1x16(index);				
			}

		GXEnd();
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
