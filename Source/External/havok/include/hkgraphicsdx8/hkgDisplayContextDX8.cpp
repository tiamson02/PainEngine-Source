/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsdx8/hkgraphicsdx8.h> // precompiled
#include <hkgraphicsdx8/hkgDisplayContextDX8.h>

//
// Statics
//

hkgDisplayContextDX8::hkgDisplayContextDX8(hkgWindow* owner)
: hkgDisplayContext(owner), m_device( static_cast<hkgWindowDX8*>( owner )->getDevice() ),m_immMode(HKG_IMM_LINES)
{
	hkgVec3Zero( m_curNorm );
	m_curNorm[1] = 1.0f;
	m_curUV[0] = 0;
	m_curUV[1] = 1.0f;
	m_curColor = 0xffffffff;

	initD3DState();
}

void hkgDisplayContextDX8::initD3DState()
{
	// set some common basic states

	// min and mag modes 
	m_device->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_device->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	
	// modulate with diffuse color 
	m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);    
	m_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);   
	m_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE); 

	// scene ambient
	D3DCOLOR ambient = 0xff191919; // roughly (0.1f, 0.1f, 0.1f, 1.0f};
	m_device->SetRenderState(D3DRS_AMBIENT, ambient);

	// z test
	m_device->SetRenderState(D3DRS_ZENABLE, TRUE);
}


hkgDisplayContextDX8::~hkgDisplayContextDX8()
{
}

void hkgDisplayContextDX8::setLightingState(bool on)
{
	if (on)
	{
		m_device->SetRenderState( D3DRS_LIGHTING, TRUE);
		m_currentState |= HKG_ENABLED_LIGHTING;
	}
	else
	{
		m_device->SetRenderState( D3DRS_LIGHTING, FALSE);
		m_currentState &= ~HKG_ENABLED_LIGHTING;
	}
}

void hkgDisplayContextDX8::setTexture2DState(bool on)
{
	if (on)
	{
		m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_currentState |= HKG_ENABLED_TEXTURE2D;
	}
	else
	{
		m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		m_currentState &= ~HKG_ENABLED_TEXTURE2D;
	}
}

void hkgDisplayContextDX8::setBlendState(bool on)
{
	if (on)
	{
		m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); 
		m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); 
		m_currentState |= HKG_ENABLED_ALPHABLEND;
	}
	else
	{
		m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_currentState &= ~HKG_ENABLED_ALPHABLEND;
	}
}

void hkgDisplayContextDX8::setDepthReadState(bool on)
{
	if (on)
	{
		m_device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
		m_currentState |= HKG_ENABLED_ZREAD;
	}
	else
	{
		// always pass z test.. ie, don't read values
		m_device->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
		m_currentState &= ~HKG_ENABLED_ZREAD;
	}
}

void hkgDisplayContextDX8::setDepthWriteState(bool on)
{
	if (on)
	{
		m_device->SetRenderState(D3DRS_ZWRITEENABLE , TRUE);
		m_currentState |= HKG_ENABLED_ZWRITE;
	}
	else
	{
		m_device->SetRenderState(D3DRS_ZWRITEENABLE , FALSE);
		m_currentState &= ~HKG_ENABLED_ZWRITE;
	}	

}

void hkgDisplayContextDX8::setCullFaceState(bool on)
{
	// use OPOSITE culling than requested as we are flipping Z
	// in the view matrix to revert the handedness
	if (on)
	{
		m_device->SetRenderState(D3DRS_CULLMODE, m_cullfaceMode == HKG_CULLFACE_CCW? D3DCULL_CW : D3DCULL_CCW);
		m_currentState |= HKG_ENABLED_CULLFACE;
	}
	else
	{
		m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE );
		m_currentState &= ~HKG_ENABLED_CULLFACE;
	}
}

void hkgDisplayContextDX8::setWireframeState(bool on)
{
	if (on)
	{
	   	m_device->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		m_currentState |= HKG_ENABLED_WIREFRAME;
	}
	else
	{
		m_device->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		m_currentState &= ~HKG_ENABLED_WIREFRAME;
	}
}


void hkgDisplayContextDX8::popMatrix()
{

}

void hkgDisplayContextDX8::pushMatrix()
{

}

void hkgDisplayContextDX8::multMatrix( float m[16] )
{	

}

void hkgDisplayContextDX8::beginGroup(HKG_IMM_GROUP type)
{
	m_immVerts.clear();
	m_immMode = type;
}

void hkgDisplayContextDX8::endGroup()
{
	/*
	#define HKG_IMM_LINES           (1<<0)
	#define HKG_IMM_TRIANGLE_LIST	(1<<1)
	#define HKG_IMM_TRIANGLE_STRIP	(1<<2)
	#define HKG_IMM_POINTS    
	*/
	/*
    D3DPT_POINTLIST             = 1,
    D3DPT_LINELIST              = 2,
    D3DPT_LINESTRIP             = 3,
    D3DPT_TRIANGLELIST          = 4,
    D3DPT_TRIANGLESTRIP         = 5,
    D3DPT_TRIANGLEFAN           = 6,
	*/


	unsigned int numVerts = m_immVerts.getSize();

	unsigned int minPoints;
	unsigned int numPrim;
	D3DPRIMITIVETYPE dxType;

	switch (m_immMode)
	{
		case HKG_IMM_LINES:			minPoints = 2; numPrim = numVerts / 2; dxType = D3DPT_LINELIST; break;
		case HKG_IMM_TRIANGLE_LIST:	minPoints = 3; numPrim = numVerts / 3; dxType = D3DPT_TRIANGLELIST; break;
		case HKG_IMM_TRIANGLE_STRIP: minPoints = 3; numPrim = numVerts - 2; dxType = D3DPT_TRIANGLESTRIP; break;
		case HKG_IMM_POINTS:		minPoints = 1; numPrim = numVerts; dxType = D3DPT_POINTLIST; break;
		default: HK_ASSERT( 0 && "immediate mode broken\n"); return;
	}
			
	if (numVerts < minPoints)
		return; // not enough info;

	DWORD dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;
		
	m_device->SetVertexShader(dwFVF); // fixed function pipeline
	m_device->DrawPrimitiveUP(
			dxType,
			numPrim,
			(unsigned char*)(&m_immVerts[0]),
			sizeof(VertInfo) );


	m_immVerts.clear();
}

void hkgDisplayContextDX8::setCurrentTextureCoord(float uv[2])
{
	m_curUV[0] = uv[0];
	m_curUV[1] = uv[1];
}

void hkgDisplayContextDX8::setCurrentNormal(float n[3])
{
	hkgVec3Copy( m_curNorm, n );
}

void hkgDisplayContextDX8::setCurrentColor3( float c[3])
{
	m_curColor = hkgColor4FromVec3(c);
}

void hkgDisplayContextDX8::setCurrentColor4( float c[4])
{
	m_curColor = hkgColor4FromVec4(c);
}

void  hkgDisplayContextDX8::setCurrentColorPacked( unsigned int c)
{
	m_curColor = c;
}
void hkgDisplayContextDX8::setCurrentPosition(float pos[3])
{
	// add a vert

	int numVerts = m_immVerts.getSize();
	if (numVerts > (int)(m_maxVerts - 3) ) // getting close to limit, we should split soon but have to wait for a good place to split
	{
		bool needSplit = false;

		switch (m_immMode)
		{
			case HKG_IMM_LINES:			
				needSplit = (numVerts % 2) == 0;
				break;

			case HKG_IMM_TRIANGLE_LIST:	
				needSplit = (numVerts % 3) == 0;
				break;

			case HKG_IMM_TRIANGLE_STRIP: 
				// we can stop now, but we need to re give the last two verts aswell to start the next strip
				// we will do the split code ourselves
				{
					VertInfo v0 = m_immVerts[ numVerts - 2 ];
					VertInfo v1 = m_immVerts[ numVerts - 1 ];
					
					endGroup();
					beginGroup(HKG_IMM_TRIANGLE_STRIP);

					// give the first two again.

					m_immVerts.pushBack(v0);
					m_immVerts.pushBack(v1);
				}
				break;

			case HKG_IMM_POINTS:	
				needSplit = true;
				break;

			default: HK_ASSERT( 0 && "immediate mode broken\n"); return;
		}

		if (needSplit)
		{
			endGroup(); // end;
			beginGroup(HKG_IMM_LINES); // start again.	
		}
	}

	VertInfo vi;

	hkgVec3Copy( (float*)(&vi.x), pos );
	hkgVec3Copy( (float*)(&vi.nx), m_curNorm );
	vi.c = m_curColor;
	vi.tu = m_curUV[0];
	vi.tv = m_curUV[1];
	
	m_immVerts.pushBack(vi); // will copy
}

void hkgDisplayContextDX8::flush()
{

}

void hkgDisplayContextDX8::setLightState(int light, bool on)
{
	m_device->LightEnable( light, on );
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
