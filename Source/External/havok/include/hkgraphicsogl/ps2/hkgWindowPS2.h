/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/*           Copyright (C) 2001 Sony Computer Entertainment America
                              All Rights Reserved
                               SCEA Confidential                                */


#ifndef HK_GRAPHICS_WINDOW_PS2_H
#define HK_GRAPHICS_WINDOW_PS2_H

#include <hkgraphics/hkgWindow.h>

class CEETimer;

class hkgWindowPS2 : public hkgWindow
{
public:
	
	virtual bool initialize( HKG_WINDOW_CREATE_FLAG flags, HKG_WINDOW_BUFFERS buffers, int w, int h, const char* name, void* platformHandle = HK_NULL, void* platformParentHandle = HK_NULL); // returns success
	virtual bool initializeContext();
	
	virtual	bool setAsCurrent(); // only need to call this if you have > 1 window
	virtual bool clearBuffers();
	virtual bool swapBuffers();


	virtual HKG_WINDOW_MSG_STATUS peekMessages(bool inViewportResize = false); // will loop on windows messages if any. Will return if idle.
	
	// Call this after hkBaseSystemInit but before any IO.
	// When the first window (and only window on a console)
	// is created then the sub system will init too

	static void setSCEModulePath(const char* str);

	static hkgWindow* createWindowPS2() { return new hkgWindowPS2(); }

protected:

	hkgWindowPS2();
	virtual ~hkgWindowPS2();

	void initSubsystem();
	void handlePadInput();
	
	// The module path used to init the system
	// it defaults to 'host0:/usr/local/sce/iop/modules'
	static char* 	g_sModulePath;
	static bool		g_bSystemInit; // so that we init only once.

	bool			m_bFirstDraw; // so that we know not to wait on the draw from before.
	CEETimer*		m_ceeTimer;
};




#endif //HK_GRAPHICS_WINDOW_PS2_H

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
