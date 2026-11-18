/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgAssert.h
*   Assert, warning and print macros
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-23-08      bodasing    initial implementation
*/

#ifndef _TG_ASSERT_H_
#define _TG_ASSERT_H_

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------
#ifdef _PSPEMU
#include <libemu.h>
#else
#include <stdio.h>
#endif

//-----------------------------------------------------------------------------
// M A C R O S
//-----------------------------------------------------------------------------
//Ensure any previous definitions are removed:
#undef TG_ASSERT
#undef TG_WARNING
#undef TG_PRINT

#ifdef  _DEBUG

//Wrap text output function to support future modification:
#ifdef _PSPEMU
#define TG_OUTPUT_TEXT	sceEmuPrintf
#else
#define TG_OUTPUT_TEXT	printf
#endif

//-----------------------------------------------------------------------------
// TG_ASSERT
// Description	: Prints assert message and loops endlessly if expression e
// 				  evaluates to false
// Parameters	: e 	- expression to be tested
//				  ... 	- user message to be printed if expression is false
// Returns		: -
// Note			: If expression e evaluates to false, TG_ASSERT prints the 
//				  following: file name, line number, expression and user 
//				  message.				  
//				  Endless loop allows stack to be preserved for inspection by 
//				  debugger.
//---			  do { } while(0) used to force user to terminate call with a semi-colon
//-----------------------------------------------------------------------------
#define TG_ASSERT(e, ...)															\
	do																				\
	{																				\
		if(!(e))   																	\
		{																			\
			TG_OUTPUT_TEXT("-----------------------------------------------\n");	\
			TG_OUTPUT_TEXT("ASSERT FAILED in \"%s\" at line %d:\n(%s) == FALSE\n",	\
						 __FILE__, __LINE__, #e);   								\
			TG_OUTPUT_TEXT(__VA_ARGS__);											\
			TG_OUTPUT_TEXT("\n-----------------------------------------------\n");	\
			while(1) {}																\
		}																			\
	} while(0)																		\

//-----------------------------------------------------------------------------
// TG_WARNING
// Description	: Prints warning message if expression e evaluates to false
// Parameters	: e 	- expression to be tested
//				  ... 	- user message to be printed if expression is false
// Returns		: -
// Note			: If expression e evaluates to false, TG_WARNING prints the 
//				  following: file name, line number, expression and user 
//				  message.	
//---			  do { } while(0) used to force user to terminate call with a semi-colon
//-----------------------------------------------------------------------------
#define TG_WARNING(e, ...)															\
	do																				\
	{																				\
		if(!(e))   																	\
		{																			\
			TG_OUTPUT_TEXT("-----------------------------------------------\n");	\
			TG_OUTPUT_TEXT("Warning in \"%s\" at line %d:\n(%s) == FALSE\n",		\
						 __FILE__, __LINE__, #e);   								\
			TG_OUTPUT_TEXT(__VA_ARGS__);											\
			TG_OUTPUT_TEXT("\n-----------------------------------------------\n");	\
		}																			\
	} while(0)																		\


//-----------------------------------------------------------------------------
// TG_PRINT
// Description	: Prints user-supplied message
// Parameters	: ... - user message to be printed
// Returns		: -
// Note			: TG_PRINT prints the following: file name, line number and 
//			      user message.	
//---			  do { } while(0) used to force user to terminate call with a semi-colon
//-----------------------------------------------------------------------------
#define TG_PRINT(...)																\
	do																				\
	{																				\
		TG_OUTPUT_TEXT("\"%s\" line %d:\n",											\
					 __FILE__, __LINE__);   										\
		TG_OUTPUT_TEXT(__VA_ARGS__);												\
		TG_OUTPUT_TEXT("\n");														\
	} while(0)																		\

#else	//_DEBUG

//Do nothing if _DEBUG is undefined:
#define TG_ASSERT(e, ...) 	((void) 0)
#define TG_WARNING(e, ...)	((void) 0)
#define TG_PRINT(...)		((void) 0)
#define TG_OUTPUT_TEXT(...) ((void) 0)

#endif	//_DEBUG

//check valid pointer before free
#define TG_SAFE_FREE( x ) if( (x) != NULL ) free( (x) )

#ifdef __cplusplus
}
#endif

#endif //__TG_ASSERT_H_
