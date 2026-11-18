/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//----------------------------------------------------------------
//
// The following changes have been made to the QHull Code:
//
// 1. All filenames have been changed from xxx to qhull_xxx
//
// 2. All uses of NULL have been replaced with HK_NULL	
//
// 3. All i/o statments have been replaced as follows:
//		fprintf() has been replaced with an empty function FPRINTF_REPLACEMENT
//		fputs() has been replaced with an empty function FPUTS_REPLACEMENT
//		fgets() has been replaced with an empty function FGETS_REPLACEMENT
//		freopen() has been replaced with an empty function FREOPEN_REPLACEMENT
//
// 4. The memory management has been changed to use havok's memory management
//		malloc() has been replaced with the function hkAllocate()
//		free() has been replaced with the function hkDeallocate()
//		The implementations of hkAllocate and hkDeallocate may be found in hkbase/memory/hkmemory.h
//
// By: Nick Gray
// Date: 19/10/2001
//
// 5. 'char*' has been changed to 'const char*' where appropriate
// Stephen Kennedy 05/06/2002
//
//----------------------------------------------------------------
#ifndef HK_GEOMETRYUTIL2FORCEINCLUDESFORQHULL_H
#define HK_GEOMETRYUTIL2FORCEINCLUDESFORQHULL_H

void HK_CALL FPRINTF_REPLACEMENT( void *stream, const char *bob,...);
int HK_CALL FPUTS_REPLACEMENT( const char *string, void *stream );
char* HK_CALL FGETS_REPLACEMENT( const char *string, int n, void *stream );
void* HK_CALL FREOPEN_REPLACEMENT( const char *filename, const char *mode, void *stream);

#if defined(HK_COMPILER_MSVC)
	// C4701 local variable 'name' may be used without having been initialized
	// C4706 assignment within conditional expression
#	pragma warning(disable: 4701 4706)
#endif

#endif // HK_GEOMETRYUTIL2FORCEINCLUDESFORQHULL_H

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
