/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsgcn/hkgraphicsgcn.h> // precompiled
#include <hkgraphicsgcn/hkgDisplayContextGCN.h>

hkgDisplayContextGCN::~hkgDisplayContextGCN()
{

}

hkgDisplayContextGCN::hkgDisplayContextGCN(hkgWindow* owner)
: hkgDisplayContext(owner), m_numLightsEnabled(0)
{

	hkgVec3Zero( m_curNorm );
	m_curNorm[1] = 1.0f;
	m_curUV[0] = 0;
	m_curUV[1] = 1.0f;
	m_curColor = 0xffffffff;


    GXSetNumChans(1); // default, color = vertex color
	GXSetChanCtrl(
        GX_COLOR0,
        GX_DISABLE,    // enable channel
        GX_SRC_REG,  // amb source
        GX_SRC_REG,  // mat source
        GX_LIGHT_NULL,   // light mask
        GX_DF_CLAMP, // diffuse function
        GX_AF_NONE);
        
    GXSetChanCtrl(
        GX_ALPHA0,
        GX_DISABLE,       // enable channel
        GX_SRC_REG,  // amb source
        GX_SRC_REG,  // mat source
        GX_LIGHT_NULL,   // light mask
        GX_DF_NONE,  // diffuse function
        GX_AF_NONE);
	
	// Tev	   	
	GXSetNumTexGens(0); // no texture in this demo
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
           
	GXColor c;
	c.r = 255;
	c.g = 100;
	c.b = 155;
	c.a = 255;
	GXSetChanMatColor(GX_COLOR0A0, c);
	
	c.r = 25;
	c.g = 25;
	c.b = 25;
	c.a = 255;
	GXSetChanAmbColor(GX_COLOR0A0, c);
    
    GXSetCurrentMtx( GX_PNMTX0 );   
        
    // we use VAT 0 for our internal vertex format
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
   
   	// we use VAT 1 for our immediate mode
    GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    
    
    GXSetCullMode( GX_CULL_FRONT );
 
}

void hkgDisplayContextGCN::setLightingState(bool on)
{
    unsigned int lightMask = (unsigned int)GX_LIGHT_NULL;
    unsigned int i =0;
    while (i++ < m_numLightsEnabled)
    {
	    lightMask = lightMask << 1;
    	lightMask |= 0x01;
    }
  
	if (on )
	{
		GXSetChanCtrl(
	        GX_COLOR0,
	        GX_ENABLE,   // enable channel
	        GX_SRC_REG,  // amb source
	        GX_SRC_REG,  // mat source
	        (GXLightID)lightMask,   // light mask
	        GX_DF_CLAMP, // diffuse function
	        GX_AF_NONE);
	       		
		m_currentState |= HKG_ENABLED_LIGHTING;
	}
	else
	{
		GXSetChanCtrl(
	        GX_COLOR0,
	        GX_DISABLE,    // enable channel
	        GX_SRC_REG,  // amb source
	        GX_SRC_REG,  // mat source
	        (GXLightID)lightMask,   // light mask
	        GX_DF_CLAMP, // diffuse function
	        GX_AF_NONE);

		m_currentState &= ~HKG_ENABLED_LIGHTING;
	}
}

void hkgDisplayContextGCN::setTexture2DState(bool on)
{
	if (on)
	{
		GXSetNumTexGens(1);
    	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);    	   
   		GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
   
		m_currentState |= HKG_ENABLED_TEXTURE2D;
	}
	else
	{
		GXSetNumTexGens(0); // no texture in this demo
    	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
   		GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
   
		m_currentState &= ~HKG_ENABLED_TEXTURE2D;
	}
}

void hkgDisplayContextGCN::setBlendState(bool on)
{
	if (on)
	{
		GXSetBlendMode( GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		m_currentState |= HKG_ENABLED_ALPHABLEND;
	}
	else
	{
		GXSetBlendMode( GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		m_currentState &= ~HKG_ENABLED_ALPHABLEND;
	}
}

void hkgDisplayContextGCN::setDepthReadState(bool on)
{
	if (on)
	{
		GXSetZMode( GX_TRUE, GX_LESS, m_currentState & HKG_ENABLED_ZWRITE? GX_TRUE : GX_FALSE );
		m_currentState |= HKG_ENABLED_ZREAD;
	}
	else
	{
		GXSetZMode( GX_FALSE, GX_LESS, m_currentState & HKG_ENABLED_ZWRITE? GX_TRUE : GX_FALSE );
		m_currentState &= ~HKG_ENABLED_ZREAD;
	}
}

void hkgDisplayContextGCN::setDepthWriteState(bool on)
{
	if (on)
	{
		GXSetZMode( m_currentState & HKG_ENABLED_ZREAD? GX_TRUE : GX_FALSE, GX_LESS, GX_TRUE );
		m_currentState |= HKG_ENABLED_ZWRITE;
	}
	else
	{
		GXSetZMode( m_currentState & HKG_ENABLED_ZREAD? GX_TRUE : GX_FALSE, GX_LESS, GX_FALSE );
		m_currentState &= ~HKG_ENABLED_ZWRITE;
	}	
}

void hkgDisplayContextGCN::setCullFaceState(bool on)
{
	if (on)
	{
		GXSetCullMode( GX_CULL_FRONT );
		m_currentState |= HKG_ENABLED_CULLFACE;
	}
	else
	{
		GXSetCullMode( GX_CULL_NONE );
		m_currentState &= ~HKG_ENABLED_CULLFACE;
	}
}

void hkgDisplayContextGCN::setWireframeState(bool on)
{
	if (on)
	{
		m_currentState |= HKG_ENABLED_WIREFRAME;
	}
	else
	{
		m_currentState &= ~HKG_ENABLED_WIREFRAME;
	}
}

void hkgDisplayContextGCN::popMatrix()
{

}

void hkgDisplayContextGCN::pushMatrix()
{

}

void hkgDisplayContextGCN::multMatrix( float m[16] )
{	

}

void hkgDisplayContextGCN::beginGroup(HKG_IMM_GROUP type)
{
	m_immVerts.clear();
	m_immMode = type;
}

void hkgDisplayContextGCN::endGroup()
{
	unsigned int numVerts = m_immVerts.getSize();

	unsigned int minPoints;
	unsigned int numPrim;
	GXPrimitive gcType;

	switch (m_immMode)
	{
		case HKG_IMM_LINES:			minPoints = 2; gcType = GX_LINES; break;
		case HKG_IMM_TRIANGLE_LIST:	minPoints = 3; gcType = GX_TRIANGLES; break;
		case HKG_IMM_TRIANGLE_STRIP: minPoints = 3; gcType = GX_TRIANGLESTRIP; break;
		case HKG_IMM_POINTS:		minPoints = 1; gcType = GX_POINTS; break;
		default: HK_ASSERT( 0 && "immediate mode broken\n"); return;
	}
			
	if (numVerts < minPoints)
		return; // not enough info;
		
	GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT); 
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT); 
  
	GXBegin( gcType, GX_VTXFMT1, numVerts);

		for (unsigned int v = 0; v < numVerts; ++v)
		{
			VertInfo& iv = m_immVerts[v];
			GXPosition3f32(iv.x, iv.y, iv.z);
			GXNormal3f32(iv.nx, iv.ny, iv.nz);
			u8 r = (iv.c & 0xff);
			u8 g = (iv.c & 0xff00) >> 8;
			u8 b = (iv.c & 0xff0000) >> 16;
			u8 a = (iv.c & 0xff000000) >> 24;
        	GXColor4u8(r,g,b,a);
        	GXTexCoord2f32(iv.tu, iv.tv);
        }
 
	GXEnd();
	
	m_immVerts.clear();
}

void hkgDisplayContextGCN::setCurrentTextureCoord(float uv[2])
{
	m_curUV[0] = uv[0];
	m_curUV[1] = uv[1];
}

void hkgDisplayContextGCN::setCurrentNormal(float n[3])
{
	hkgVec3Copy( m_curNorm, n );
}

void hkgDisplayContextGCN::setCurrentColor3( float c[3])
{
	m_curColor = hkgColor4FromVec3(c);
}

void hkgDisplayContextGCN::setCurrentColor4( float c[4])
{
	m_curColor = hkgColor4FromVec4(c);
}

void  hkgDisplayContextGCN::setCurrentColorPacked( unsigned int c)
{
	m_curColor = c;
}

void hkgDisplayContextGCN::setCurrentPosition(float pos[3])
{
	// add a vert

	VertInfo vi;

	hkgVec3Copy( (float*)(&vi.x), pos );
	hkgVec3Copy( (float*)(&vi.nx), m_curNorm );
	vi.c = m_curColor;
	vi.tu = m_curUV[0];
	vi.tv = m_curUV[1];
	
	m_immVerts.pushBack(vi); // will copy
}

void hkgDisplayContextGCN::flush()
{
	GXFlush();
}

void hkgDisplayContextGCN::setLightState(int light, bool on)
{
	if (light < 8)
	{
		if (on && ((light+1) > m_numLightsEnabled) )
			m_numLightsEnabled = light + 1;
		else if ((!on) && (light < m_numLightsEnabled) )
			m_numLightsEnabled = light;
			
		setLightingState( m_currentState & HKG_ENABLED_LIGHTING );	// to refresh channel control
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
