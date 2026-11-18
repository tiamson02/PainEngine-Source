/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsdx8/hkgraphicsdx8.h> // precomp
#include <hkgraphicsdx8/hkgFaceSetDX8.h>
#include <hkgraphicsdx8/hkgVertexSetDX8.h>

hkgFaceSetDX8::~hkgFaceSetDX8()
{
	if (m_pIB)
		m_pIB->Release();
}


void hkgFaceSetDX8::initialize()
{

}

void hkgFaceSetDX8::createIndexBuffer()
{
	if (m_pIB)
		m_pIB->Release(); // better to just make a new one.

	m_connectionInfo.setSize(0);

	unsigned int numIndices = 0;
	unsigned int numIndexed = 0;
	int nm = getNumPrimitives();
	int m;
	for (m = 0; m < nm; ++m)
	{
		const hkgFaceSetPrimitive* prim = getPrimitive(m);
		const unsigned short* conn = prim->getIndices();

		if (conn)
		{
			unsigned int pl = prim->getLength();
			
			IndexBufferInfo bi;
			bi.minIndex = 0xffff; // short max
			bi.startIndex = numIndices;

			for (unsigned int i=0; i < pl; ++i)
			{
				if (conn[i] < bi.minIndex)
					bi.minIndex = conn[i];
			}
			
			numIndices += pl;
			numIndexed++;

			m_connectionInfo.pushBack( bi );
		}
	}

	if (numIndices < 1)
	{
		m_flags |= HKG_FACE_SET_PLATFORM_OPT;
		return;
	}

	HRESULT hr = m_device->CreateIndexBuffer( numIndices*sizeof(unsigned short), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB );
	HK_ASSERT( hr == D3D_OK && "Could not create D3D index buffer");

	// lock and fill
	BYTE* bptr;
	unsigned int index = 0;
	m_pIB->Lock(0,0, &bptr, 0);
	unsigned short* sptr = (unsigned short*)bptr;

	for (m = 0; m < nm; ++m)
	{
		const hkgFaceSetPrimitive* prim = getPrimitive(m);
		const unsigned short* conn = prim->getIndices();

		if (conn)
		{
			unsigned int pl = prim->getLength();
			for (unsigned int i=0; i < pl; ++i)
				sptr[index++] = conn[i];
		}
	}

	m_pIB->Unlock();
	m_flags |= HKG_FACE_SET_PLATFORM_OPT;
}

void hkgFaceSetDX8::render(hkgDisplayContext* context)
{
	hkgVertexSetDX8* dxVerts = static_cast<hkgVertexSetDX8*>( m_vertices );
	
	if ((m_flags & HKG_FACE_SET_PLATFORM_OPT) == 0) // then we need to optimize into a proper index buffer
		createIndexBuffer();

	dxVerts->setStreams(context);// set vertex buffer as current stream 0

	if (m_pIB)
		m_device->SetIndices( m_pIB, 0 );
	
	bool needTextureCoords = (context->getVertexOptions() & HKG_VTX_DISABLE_TEXTURE2D) == 0;
	bool needVtxColors = (context->getVertexOptions() & HKG_VTX_DISABLE_COLOR) == 0;

	int nm = getNumPrimitives();
	int cindex =0;
	for (int m = 0; m < nm; ++m)
	{
		const hkgFaceSetPrimitive* prim = getPrimitive(m);
		const unsigned short* conn = prim->getIndices();
		
		D3DPRIMITIVETYPE dxPrim;
		unsigned int dxNumPrim;

		if ( prim->getType() == HKG_TRI_LIST )
		{
			dxPrim = D3DPT_TRIANGLELIST;
			dxNumPrim = prim->getLength() /3;
		}
		else
		{
			dxPrim = D3DPT_TRIANGLESTRIP;
			dxNumPrim = prim->getLength() -2;
		}

		if (conn)
		{
			HK_ASSERT( m_pIB && (cindex < m_connectionInfo.getSize()) );
			IndexBufferInfo& bi = m_connectionInfo[cindex++];
			m_device->DrawIndexedPrimitive(dxPrim, bi.minIndex, prim->getLength(), bi.startIndex, dxNumPrim);
		}
		else
		{
			unsigned int offset = prim->getVertexBufferStartIndex();
			m_device->DrawPrimitive(dxPrim, offset, dxNumPrim);
		}
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
