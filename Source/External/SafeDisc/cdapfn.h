/***************************************************************************************************
 * Name: CdaPfn.h
 *
 * $Revision: 26 $
 *
 * Description:	Public header for Application Security Functions
 *
 *
 *	This header contains the macro declarations needed to declare a function as
 *  a SafeDisc/SafeWrap API Protected Function.
 *
 *	For full details see the SafeDisc/SafeWrap API User Guide
 *
 ***************************************************************************************************
 *
 * NOTICE OF COPYRIGHT AND OWNERSHIP OF SOFTWARE:
 *
 * Copyright (c) 1993-2003, Macrovision Europe Ltd.  All Rights Reserved.
 *
 * This computer program is the property of Macrovision Europe Ltd. of Reading, Berkshire, England 
 * and Macrovision Corporation of Santa Clara, California, U.S.A. 
 * Any use, copy, publication, distribution, display, modification, or transmission of this computer
 * program in whole or in part in any form or by any means without the prior express written 
 * permission of Macrovision Europe Ltd. or Macrovision Corporation is strictly prohibited.
 * 
 * Except when expressly provided by Macrovision Europe Ltd. or Macrovision Corporation in writing,
 * possession of this computer program shall not be construed to confer any license or rights under 
 * any of Macrovision Europe Ltd.'s or Macrovision Corporation's intellectual property rights, 
 * whether by estoppel, implication, or otherwise.  
 * 
 * ALL COPIES OF THIS PROGRAM MUST DISPLAY THIS NOTICE OF COPYRIGHT AND OWNERSHIP IN FULL.
 ***************************************************************************************************
 * 
 * $Nokeywords: $
 *
 **************************************************************************************************/

#ifndef _CDAPFN_H
#define _CDAPFN_H

// Version the macros

#define	CDAPFN_VERSION		6

// Declare a structure that will be instanced and exported for each ASF.
// The structure packing pragma is needed to make sure that SafeDisc/SafeWrap 
// software has the same view of the structure as the application.

#pragma pack(push, 4)

typedef void (*CDAPFN_PTR)();

typedef struct
{
	unsigned long Magic;				// Identifies this as a protected function property block
										// (used for integrity checks)
	unsigned long Version;				// Internal version number used to identify the data structure
                                        // for backward compatibilty reasons - this is NOT intended to 
                                        // be the version of the PFN library or header file.
	unsigned long Size;					// Size of this structure in bytes
	unsigned long FunctionAddress;		// The address of the protected function
	unsigned long MaxOverhead;			// Maximum execution overhead
	unsigned long Constraints;			// Protection constraints
	char 		  *PrivateStr;
	unsigned long State;				// Initial state is 0
	unsigned long ProtectionGroup;		// The group that this function belongs to
	unsigned long AdtControl;			// Type of anti-debugger protection
	unsigned long EncryptionType;		// Type of encryption
	unsigned long SequenceId;			// Sequence identifier
	unsigned char Private[84];

} CDAPFN_PROPERTIES, *PCDAPFN_PROPERTIES, **PPCDAPFN_PROPERTIES;

#define	CDAPFN_MAGIC	0xD4F70933	

#pragma pack(pop)

//******************************************************************************
// Protection properties
//******************************************************************************

// Execution overhead levels

// Comments provide guidelines for the increase in the execution time of the function.
// The guidelines are based on a P500 processor and a function length of 500 bytes.
// At version 1 
//		encryption is provided at all levels
//		anti-debugger checks are performed - most extensive at L5. least at L1

#define CDAPFN_OVERHEAD_L5		5	// Guideline: 10 seconds
#define CDAPFN_OVERHEAD_L4		4	// Guideline: 1 second
#define CDAPFN_OVERHEAD_L3		3	// Guideline: 500 milliseconds
#define CDAPFN_OVERHEAD_L2		2	// Guideline: 100 milliseconds
#define CDAPFN_OVERHEAD_L1		1	// Guideline: 50 milliseconds

#define CDAPFN_OVERHEAD_ENCODING_ENCRYPT 0
#define CDAPFN_OVERHEAD_ENCODING_MANGLE  1 

#define CDAPFN_OVERHEAD_LEVEL( Level )               ( Level % 6 )  
#define CDAPFN_OVERHEAD_ENCODING_TYPE( EncodeType )  ( ( EncodeType % 2 ) * 0x10000 )
#define CDAPFN_OVERHEAD_ENCODING_DENSITY( Percent )  ( ( Percent % 100 ) * 0x100 ) 

// General Authentication types
#define CDAPFN_OVERHEAD_AUTH_FULL           0x7f
#define CDAPFN_OVERHEAD_AUTH_SILENT         0x80
#define CDAPFN_OVERHEAD_AUTH_NON_SILENT     0x00
#define CDAPFN_OVERHEAD_AUTH( Authentication )  ( ( Authentication & 0xff ) * 0x100000 ) 


// Constraints
	// No constraints

#define CDAPFN_CONSTRAINT_NONE					0x00000000

	// Function contains an exception handler (called functions may return to it without
	// normal stack unwinding)

#define CDAPFN_CONSTRAINT_EXCEPTION_HANDLER			0x00000001
#define CDAPFN_CONSTRAINT_EXCLUDE_FROM_RUNTIME		0x00000002
#define CDAPFN_CONSTRAINT_EXLUDE_FROM_RUNTIME		0x00000002
#define CDAPFN_CONSTRAINT_FORCED_FAILURE			0x00000004
#define CDAPFN_CONSTRAINT_EXCLUDE_FROM_PEPPERING	0x00000008
#define CDAPFN_CONSTRAINT_SINGLE_PATH				0x00000010			

// Defaulted properties.  Do not change these values

#define	CDAPFN_PROTECTION_GROUP_DEFAULT		1
#define	CDAPFN_STATE_INITIAL_USER			0

//******************************************************************************
// Macros needed to declare a global function as an API Protected Function
//******************************************************************************

/*
	Steps needed
	1.	Prototype the function (if not done elsewhere, e.g. in a header)
	2	Invoke CDAPFN_DECLARE_GLOBAL before the function declaration
	3.	Invoke CDAPFN_ENDMARK exactly once inside the function
	
	Function code before CDAPFN_ENDMARK is protected, code after is not.
	Care must be taken not to position this macro where the compiler might optimise it out,
	for example after the return statement.

	Example:
		
	int MyFunc();					// Must prototype the function if not done elsewhere

	CDAPFN_DECLARE_GLOBAL(MyFunc, CDAPFN_OVERHEAD_L5, CDAPFN_CONSTRAINT_NONE);

	int MyFunc()
	{
		// My protected stuff

		CDAPFN_ENDMARK(MyFunc);	// Marks end of protected function
		return 0;				
	}
*/

#define CDAPFN_DECLARE_PROPERTIES(FnName, MaxOverhead, Constraints, SequenceId ) \
	__declspec(dllexport) CDAPFN_PROPERTIES CDAPFN0506_##FnName =				\
	{																			\
		CDAPFN_MAGIC,															\
		CDAPFN_VERSION,															\
		sizeof(CDAPFN_PROPERTIES),												\
		(unsigned long)FnName,													\
		(MaxOverhead),															\
		(Constraints),															\
		(char *)0,																\
		CDAPFN_STATE_INITIAL_USER,												\
		CDAPFN_PROTECTION_GROUP_DEFAULT,										\
		(unsigned long)0,																		\
		(unsigned long)0,																		\
		(SequenceId)															\
	};

#define CDAPFN_DECLARE_GLOBAL_SEQUENCED( FnName, MaxOverhead, Constraints, SequenceId )	\
\
	CDAPFN_DECLARE_PROPERTIES( FnName, MaxOverhead, Constraints, SequenceId ) \
	void  CDAPFN0506_X_##FnName();\
	CDAPFN_DECLARE_PROPERTIES( CDAPFN0506_X_##FnName, MaxOverhead, \
		Constraints | CDAPFN_CONSTRAINT_EXLUDE_FROM_RUNTIME, 0 ) \
	void  CDAPFN0506_X_##FnName() \
	{\
		if (CDAPFN0506_##FnName.Version == 0)\
		{\
			CDAPFN0506_X_##FnName();\
		}\
		CDAPFN_ENDMARK( CDAPFN0506_X_##FnName );\
		return;\
	}


#define CDAPFN_DECLARE_GLOBAL( FnName, MaxOverhead, Constraints ) \
\
	CDAPFN_DECLARE_GLOBAL_SEQUENCED( FnName, MaxOverhead, Constraints, 0 )


#define CDAPFN_ENDMARK(FnName)													\
	if (CDAPFN0506_##FnName##.Version == 0)	/* always false	*/				\
	{	/* never come here, though compiler can't know that	*/					\
		((CDAPFN_PTR)CDAPFN0506_##FnName##.FunctionAddress)();					\
	}											

#endif	// def _CDAPFN_H
