/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_FACE_SET_DX8_H
#define HK_GRAPHICS_FACE_SET_DX8_H

#include <hkgraphics/hkgFaceSet.h>
#include <hkgraphicsdx8/hkgDisplayContextDX8.h>

class hkgFaceSetDX8 : public hkgFaceSet
{
public:

	static hkgFaceSet* createFaceSetDX8(hkgDisplayContext* context)
	{
		return new hkgFaceSetDX8(context);
	}	

	void initialize();
	void render(hkgDisplayContext* context);

	inline LPDIRECT3DDEVICE8 getDevice();
	inline const LPDIRECT3DDEVICE8 getDevice() const;

protected:
		
	inline hkgFaceSetDX8(hkgDisplayContext* context);
	virtual ~hkgFaceSetDX8();

	void createIndexBuffer();

	LPDIRECT3DDEVICE8		m_device;
	LPDIRECT3DINDEXBUFFER8	m_pIB;

	struct IndexBufferInfo
	{
		unsigned int minIndex;
		unsigned int startIndex;
	};

	hkArray<IndexBufferInfo> m_connectionInfo;
};

#include <hkgraphicsdx8/hkgFaceSetDX8.inl>

#endif // HK_GRAPHICS_FACE_SET_DX8_H
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
