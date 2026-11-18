#ifndef __PFNAPI_H__
#define __PFNAPI_H__
/***************************************************************************************************
 *
 * Description:	 
 *  Defines macros used to mark functions / member functions for security application.
 *	Functions marked for security protection have the "full strength" protection
 *  strategies applied to them, which means that they will be subject to significant
 *  increases in processing time. 
 *  The following pre-conditions should be met for correct operation of code marking:
 *      - Single threaded operation
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

#include "PfnApiImpl.h"     // Contains internal data used to implement macros 

/*************************************************************************************************
 *
 * Macro to mark C (and C++ non-member) functions for security processing
 *
 **************************************************************************************************/
#define PFNAPI_APPLY_SECURITY( Function ) PFNAPI_FUNCTION_DEFINE( Function, Function, 0, 0, 0 ) 



/*************************************************************************************************
 *
 * Macros to mark C++ member functions for security processing
 *
 **************************************************************************************************/
#define PFNAPI_MEMBER_DECLARE  __declspec( dllexport ) 

#define PFNAPI_MEMBER_APPLY_SECURITY( ClassName, FunctionName ) PFNAPI_FUNCTION_DEFINE( ##ClassName##__##FunctionName##, 0, 0, 0, 0 )	



/*************************************************************************************************
 *  
 * EXAMPLE #1 - marking C and C++ non-member functions
 *
 *  int     Function1( int Value );  // Usually goes in header file
 *
 *      The function declaration can be anywhere as long as it is in scope 
 *      for the protection macro reference shown below
 *  
 *  
 *  PFNAPI_APPLY_SECURITY( Function1 ); // Goes in implementation file
 *  
 *      The macro reference should be placed in a compilation unit
 *      i.e. in the C or C++ implementation file
 *  
 *************************************************************************************************/

/*************************************************************************************************
 *  
 * EXAMPLE #2 - marking C++ member functions
 *
 *  class CExample
 *  {
        PFNAPI_MEMBER_DECLARE  int  Member1( int Value );  // Usually goes in header file
 *  };
 *
 *      The member must have the PFNAPI_MEMBER_DECLARE macro (or must
 *      export the function manually using the .def file)
 *      The function declaration can be anywhere as long as it is in scope 
 *      for the protection macro shown reference below
 * 
 *  
 *  PFNAPI_MEMBER_APPLY_SECURITY( CExample, Member1 ); // Goes in implementation file
 *
 *      The macro reference should be placed in a compilation unit
 *      i.e. in the C or C++ implementation file
 *  
 *************************************************************************************************/




#endif