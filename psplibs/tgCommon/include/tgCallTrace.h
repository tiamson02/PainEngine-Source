/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgCallTrace.h
*   Call tracer header
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-23-08      sidwell     initial implementation
*/


#ifndef _CALLTRACE_H_
#define _CALLTRACE_H_

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------
#include "tgTypes.h"

//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

u32 tgCallTrace(u32 **pAddrBuffer, u32 iMaxCalls);

#ifdef __cplusplus
}
#endif

#endif //_CALLTRACE_H_
