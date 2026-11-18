#ifndef __PFNAPIIMPL_H__
#define __PFNAPIIMPL_H__
/***************************************************************************************************
 *
 * Description:	 
 *  Internal implementation macros for PFNAPI header file - should not be used directly
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
 * $NoKeywords: $
 *
 **************************************************************************************************/

#define	PFNAPI_VERSION		5
#define	PFNAPI_MAGIC	    0xD4F70933	

#pragma pack(push, 4)
typedef struct
{
	unsigned long Magic;				
	unsigned long Version;				                                     
	unsigned long Size;					
	unsigned long FunctionAddress;		
	unsigned long MaxOverhead;			
	unsigned long Constraints;			
	char 		  *PrivateStr;
	unsigned long State;				
	unsigned long ProtectionGroup;		
	unsigned long AdtControl;			
	unsigned long EncryptionType;		
	unsigned long SequenceId;			
	unsigned char Private[84];

} 
PFNAPI_DATA_TYPE;
#pragma pack(pop)


#define PFNAPI_FUNCTION_DEFINE( FunctionName, pFunction, Overhead, Constraints, Sequence )	\
\
	PFNAPI_DATA( FunctionName, pFunction, Overhead, Constraints, Sequence  );


#define PFNAPI_DATA( FunctionName, pFunction, Overhead, Constraints, Sequence ) \
    PFNAPI_DATA_SET( FunctionName, pFunction, Overhead, Constraints, 0, 1, 0, 0, Sequence )


#define PFNAPI_DATA_SET( FunctionName, pFunction, Overhead, Constraints, State, Group, Adt, Encryption, Sequence ) \
    PFNAPI_MEMBER_DECLARE PFNAPI_DATA_TYPE CDAPFN0506_##FunctionName = { \
    PFNAPI_MAGIC, PFNAPI_VERSION, sizeof(PFNAPI_DATA_TYPE), (unsigned long)pFunction, Overhead, Constraints, (char *)0, State, Group, Adt, Encryption, Sequence \
};


#endif	/* __PFNAPIIMPL_H__ */

