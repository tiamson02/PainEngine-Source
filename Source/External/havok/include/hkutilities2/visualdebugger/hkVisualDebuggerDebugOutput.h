/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_VISUAL_DEBUGGER_DEBUG_OUTPUT_H
#define HK_VISUAL_DEBUGGER_DEBUG_OUTPUT_H

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkInplaceStringStreambuf.h>

extern int hkVisualDebuggerDebugLevel;

/// Setting the level to zero will result in most information being supressed
/// only error messages will be displayed at this level
/// Setting it to 1 and higher will enable more detail levels of debug output...
#define HK_VISUAL_DEBUGGER_INFO_SET_LEVEL(LEVEL) \
	do { hkVisualDebuggerDebugLevel = LEVEL; } while(0)

/// Displays debug information regarding the visualdebugger.
/// These messages can be placed on both the client and server side
/// This information is sent to the standard output and does not
/// effect the network payload in anyway.
/// The debug levels start at 1 where this is the highest level
/// informaion.  2 is more detailed and so on...
#define HK_VISUAL_DEBUGGER_INFO(LEVEL, INFO) \
	if(LEVEL <= hkVisualDebuggerDebugLevel) \
	{ \
		hkcout << "VisualDebugger: ["; \
		if(LEVEL == 0) \
		{ \
			hkcout << "ERROR: "; \
		} \
		hkcout << INFO << ']' << hkendl; \
	}

#endif // HK_VISUAL_DEBUGGER_DEBUG_OUTPUT_H

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
