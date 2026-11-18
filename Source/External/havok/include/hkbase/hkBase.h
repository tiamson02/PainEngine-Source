/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKBASE_H
#define HKBASE_HKBASE_H

#include <hkbase/types/hkBaseTypes.h>
#if defined HK_COMPILER_MSVC
	// C4100 'identifier' : unreferenced formal parameter
	//		a function need not use all its arguments
	// C4127 conditional expression is constant
	//		constant conditionals are often used inside asserts
	// C4505 'function' : unreferenced local function has been removed
	//		lots of inline functions are not used in a compilation unit
	// C4510 'class' : default constructor could not be generated
	// C4511 'class' : copy constructor could not be generated
	// C4512 'class' : assignment operator could not be generated
	//		many classes are not designed with value semantics
	// C4514 unreferenced inline/local function has been removed
	//		lots of inline functions are not used in a compilation unit
#	pragma warning(disable: 4100 4127 4505 4510 4511 4512 4514)
#endif

#define inline HK_FORCE_INLINE

#include <hkbase/memory/hkMemory.h>
#include <hkbase/baseobject/hkBaseObject.h>
#include <hkbase/baseobject/hkReferencedObject.h>
#include <hkbase/singleton/hkSingleton.h>
#include <hkbase/error/hkError.h>
#include <hkbase/htl/hkAlgorithm.h>
#include <hkbase/htl/hkArray.h>
#include <hkbase/htl/hkObjectArray.h>
#include <hkbase/string/hkString.h>
#include <hkbase/stream/hkStreambuf.h>
#include <hkbase/stream/hkStringStreambuf.h>
#include <hkbase/stream/hkInplaceStringStreambuf.h>
#include <hkbase/stream/hkOstream.h>
#include <hkbase/console/hkConsole.h>
#include <hkbase/basesystem/hkBaseSystem.h>
#include <hkbase/debugutil/hkMonitorStream.h>
#include <hkbase/debugutil/hkMonitorCapture.h>

#undef inline

#endif // HKBASE_HKBASE_H


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
