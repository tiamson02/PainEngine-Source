/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_FONT_DX8_H
#define HK_GRAPHICS_FONT_DX8_H

#include <hkgraphics/hkgFont.h>

class hkgFontDX8 : public hkgFont
{
public:

	static hkgFont* createFontDX8()
	{
		return new hkgFontDX8;
	}

	virtual void render(hkgDisplayContext* context, const char* text, int x, int y, float color[4] );
	
protected:

	hkgFontDX8() {}
	virtual ~hkgFontDX8() {}

};

#endif // HK_GRAPHICS_FONT_DX8_H
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
