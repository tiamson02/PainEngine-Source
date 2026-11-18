/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_DISPLAY_OBJECT_DX8_H
#define HK_GRAPHICS_DISPLAY_OBJECT_DX8_H

#include <hkgraphics/hkgDisplayObject.h>

class hkgDisplayObjectDX8 : public hkgDisplayObject
{
public:
	
	static hkgDisplayObject* createDisplayObjectDX8()
	{
		return new hkgDisplayObjectDX8;
	}
	
	virtual void render(hkgDisplayContext* state) const;	

protected:

	hkgDisplayObjectDX8() {}
	virtual ~hkgDisplayObjectDX8() {}

	static D3DMATRIX m_tempMatrix;
};

#endif // HK_GRAPHICS_DISPLAY_OBJECT_DX8_H
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
