//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// Macros for your debugging pleasure
//
// $Workfile: shellAssert.h $
// $Author: Mdean $
// $Date: 9/09/03 16:09 $
// $Revision: 4 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------


#ifndef _SHELLCORE_ASSERT_H_
#define _SHELLCORE_ASSERT_H_

#ifdef __cplusplus
extern "C" {
#endif

#undef WARNING
#undef PRINT
#undef ASSERT

#ifdef NDEBUG

#if __GNUC__ == 2 && __GNUC_MINOR__ <= 95
#define PRINT(A...) ((void)0)
#define WARNING(e,F...) ((void)0)
#define ASSERT(e,F...) ((void)0)
#else
#define PRINT(...) ((void)0)
#define WARNING(e,...) ((void)0)
#define ASSERT(e,...) ((void)0)
#endif

#else

#if __GNUC__ == 2 && __GNUC_MINOR__ <= 95
#define PRINT(A...) do{scePrintf("%s(%d) : ",__FILE__,__LINE__); scePrintf(##A); scePrintf("\n");} while(0)
#define WARNING(e,F...) do{if(!(e)) {scePrintf("Warning in \"%s\" at line %d\n(%s) == FALSE\n", __FILE__, __LINE__, #e); scePrintf(##F);scePrintf("\n");}} while(0)
#define ASSERT(e,F...) do{if(!(e))   {scePrintf("Assert in \"%s\" at line %d\n(%s) == FALSE\n", __FILE__, __LINE__, #e); scePrintf(##F);scePrintf("\n"); while(1) {}} } while(0)
#else
#define PRINT(...) do{scePrintf("%s(%d) : ",__FILE__,__LINE__); scePrintf(__VA_ARGS__);scePrintf("\n");} while(0)
#define WARNING(e,...) do{if(!(e)) {scePrintf("Warning in \"%s\" at line %d\n(%s) == FALSE\n", __FILE__, __LINE__, #e); scePrintf(__VA_ARGS__);scePrintf("\n");}} while(0)
#define ASSERT(e,...) do{if(!(e))   {scePrintf("Assert in \"%s\" at line %d\n(%s) == FALSE\n", __FILE__, __LINE__, #e); scePrintf(__VA_ARGS__);scePrintf("\n"); while(1) {} }} while(0)
#endif


#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif

#endif // _SHELLCORE_ASSERT_H_
