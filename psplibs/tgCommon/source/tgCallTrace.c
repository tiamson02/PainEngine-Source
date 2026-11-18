/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgCallTrace.c
*   Call tracer
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-23-08      sidwell     initial implementation
*/

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------
#include "tgTypes.h"

//-----------------------------------------------------------------------------
// M A C R O S ,   C O N S T A N T S
//-----------------------------------------------------------------------------

#define TG_GETSP(sp) __asm__ __volatile__ ("addiu %0, $29, 0" : "=r" (sp))
#define TG_GETFP(fp) __asm__ __volatile__ ("addiu %0, $30, 0" : "=r" (fp))
#define TG_GETRA(ra) __asm__ __volatile__ ("addiu %0, $31, 0" : "=r" (ra))

#define INSTR_JR_RA			0x03e00008
#define INSTR_ADDUI_SP		0x27bd0000
#define INSTR_LW_RA			0x8fbf0000
#define INSTR_LW_FP			0x8fbe0000
#define INSTR_MOVE_SP_FP	0x03c0e821
#define INSTR_MASK			0xffff0000

//-----------------------------------------------------------------------------
// S T A T I C   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// _GetProgramCounter
// Description: get the program counter in the calling function
// Params: 	
// Returns: the return address for this function
// Note: the ra for this function should be the pc+2 in the calling function
//-----------------------------------------------------------------------------
static u32 *_GetProgramCounter()
{
	u32 *pc;

	__asm__ __volatile__ ("addiu %0, $31, 0" : "=r" (pc));

	return pc;
}

//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// tgCallTrace
// Description: trace calling functions
// Parameters: 	pAddrBuffer - array to receive addresses
//			iMaxCalls - maximum number of calls to trace
// Returns: number of calls traced
// Note: 
//-----------------------------------------------------------------------------
u32 tgCallTrace(u32 **pAddrBuffer, u32 iMaxCalls)
{
	u32	*pProgramCounter;
	u32	*pStackPointer;
	u32	*pFramePointer;
	u32	*pReturnAddress;
	u32	iNumCalls = 0;

	// get current pc, sp & ra
	pProgramCounter = _GetProgramCounter();
	TG_GETSP(pStackPointer);
	TG_GETFP(pFramePointer);
	TG_GETRA(pReturnAddress);

	// while tracing
	while(iMaxCalls --)
	{
		// while we haven't hit a return
		while(pProgramCounter[0] != INSTR_JR_RA)
		{
			// incrementing sp?
			if((pProgramCounter[0] & INSTR_MASK) == INSTR_ADDUI_SP)
				pStackPointer += (pProgramCounter[0] << 16) >> 18;

			// loading ra?
			if((pProgramCounter[0] & INSTR_MASK) == INSTR_LW_RA)
				pReturnAddress = (u32 *)pStackPointer[(pProgramCounter[0] << 16) >> 18];

			// setting sp from fp?
			if(pProgramCounter[0] == INSTR_MOVE_SP_FP)
				pStackPointer = pFramePointer;

			// loading fp?
			if((pProgramCounter[0] & INSTR_MASK) == INSTR_LW_FP)
				pFramePointer = (u32 *)pStackPointer[(pProgramCounter[0] << 16) >> 18];
			
			pProgramCounter ++;
		}

		// no return address
		if(!pReturnAddress)
		{
			*pAddrBuffer = 0;
			return iNumCalls;
		}

		// check jr ra delay slot for sp increment
		if((pProgramCounter[1] & INSTR_MASK) == INSTR_ADDUI_SP)
			pStackPointer += (pProgramCounter[1] << 16) >> 18;

		// set pc
		pProgramCounter = pReturnAddress;
		pReturnAddress = 0;

		// store pc - 2 in array
		*pAddrBuffer ++ = pProgramCounter - 2;
		iNumCalls ++;
	}

	*pAddrBuffer = 0;
	return iNumCalls;
}
