/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_WINDOW_GCN_H
#define HK_GRAPHICS_WINDOW_GCN_H

#include <hkgraphics/hkgWindow.h>

class hkgWindowGCN : public hkgWindow
{
	public:

		static hkgWindow* createWindowGCN() { return new hkgWindowGCN(); }

		virtual bool initialize( HKG_WINDOW_CREATE_FLAG flags, HKG_WINDOW_BUFFERS buffers, int w, int h, const char* name, void* windowHandle = HK_NULL, void* parentHandle = HK_NULL ); // returns success
		virtual bool initializeContext();

		virtual	bool setAsCurrent(); // only need to call this if you have > 1 window
		virtual bool clearBuffers();
		virtual bool swapBuffers();

		virtual HKG_WINDOW_MSG_STATUS peekMessages(bool inViewportResize = false); // will loop on windows messages if any. Will return if idle.
		virtual void waitMessages(); // block until there is messages

	protected:
	
		hkgWindowGCN();
		~hkgWindowGCN();
		
		void updatePads();
		
		PADStatus m_gcnPads[ PAD_MAX_CONTROLLERS ];
};

#include <hkgraphicsgcn/hkgWindowGCN.inl>

#endif //HK_GRAPHICS_WINDOW_GCN_H


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
