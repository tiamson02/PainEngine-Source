//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// shellCore header file
//
// $Workfile: shellCore.h $
// $Author: Mdean $
// $Date: 9/02/04 14:59 $
// $Revision: 14 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

#ifndef _SHELLCORE_H_
#define _SHELLCORE_H_


typedef enum
{
	SHELL_FAIL = 0,
	SHELL_SUCCESS = 1,
	SHELL_INDETERMINATE = -1
} EShellResult;

#include <eetypes.h>
#include <eekernel.h>
#include <eestruct.h>

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include <sifdev.h>
#include <sif.h>
#include <sifrpc.h>

#include <libmtap.h>
#include <libpad.h>
#include <libgraph.h>
#include <libmc.h>
#include <libcdvd.h>

#include "shellFile.h"
#include "shellPad.h"
#include "shellAssert.h"

#define GET_ALIGNMENT(a) ((((int)a-1)&~((int)a))+1)
#define bss_align(val)  __attribute__ ((aligned(val))) __attribute__ ((section (".bss")))
#define data_align(val) __attribute__ ((aligned(val)))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

typedef void ShellCoreIdleFn(int data);
typedef int ShellCoreDmacHandlerFn(int ch, void *arg, void *addr);
typedef int ShellCoreIntcHandlerFn(int ca, void *arg, void *addr);

#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

void shellCoreSetIdleRoutine(ShellCoreIdleFn *routine,int data);
void shellCoreCallIdleRoutine();
void shellCoreAsciiToJis(unsigned char* dest, const char* string);
int shellCoreJisToAscii(char* destination, const unsigned char* string);
int shellCoreAddIntcHandler(int cause, ShellCoreIntcHandlerFn *handler,int next,void *arg);
int shellCoreAddDmacHandler(int cause, ShellCoreDmacHandlerFn *handler,int next,void *arg);
void shellCoreRemoveIntcHandler(int cause,int id);
void shellCoreRemoveDmacHandler(int cause,int id);

#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif // _SHELLCORE_H_
