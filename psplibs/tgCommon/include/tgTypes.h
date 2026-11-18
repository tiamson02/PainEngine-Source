/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgTypes.h
*   Useful Data types
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-23-08      rubens      initial implementation
*/

#ifndef __TGTYPES_H__
#define __TGTYPES_H__

//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

typedef unsigned char		u8;		//unsigned byte		
typedef signed char			s8;		//signed byte			
typedef unsigned short		u16;	//unsigned 16 bit integer		
typedef signed short		s16;	//signed 16 bit	integer
typedef unsigned int		u32;	//unsigned 32 bit integer		
typedef signed int			s32;	//signed 32 bit	integer		
typedef unsigned long long	u64;	//unsigned 64 bit integer	
typedef signed long long	s64;	//signed 64 bit	integer
#ifdef	__GNUC__
typedef	unsigned int		u128	__attribute__ ((mode (TI))); //unsigned quadword	
typedef int					s128	__attribute__ ((mode (TI))); //signed quadword
#endif	// __GNUC__

typedef float				f32;	//floating point 32 bit


// possible return values from function calls
typedef enum
{
	TG_FAIL = 0,			//operation failed
	TG_SUCCESS = 1,			//operation succeeded
	TG_INDETERMINATE = -1,	//indeterminate result
	TG_INPROGRESS = -2		//operation in progress
} ETgResult;

#endif //__TGTYPES_H__
