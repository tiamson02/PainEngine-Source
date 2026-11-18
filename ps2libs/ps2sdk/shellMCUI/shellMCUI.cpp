//-----------------------------------------------------------------------------
//               shellMCUI - Memory Card User Interface Driver
//
// Memory Card User Interface Driver for TRC compliant memory card user
// interface implementation
//
// $Workfile: shellMCUI.c $
// $Author: Hbodasin $
// $Date: 30/10/02 14:00 $
// $Revision: 4 $
//
// Copyright (c) 2002 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
// Further notes, if any
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------
#ifdef __MWERKS__
	#pragma mark INCLUDES
#endif
#include "shellMCUI.h"
#include "shellMCUIScreenMain.h"
#include "shellMCUIScreenListSaves.h"
#include "shellMCUIScreenLoad.h"
#include "shellMCUIScreenSave.h"

//-----------------------------------------------------------------------------
// S T A T I C   V A R I A B L E S 
//-----------------------------------------------------------------------------
#ifdef __MWERKS__
	#pragma mark -
	#pragma mark STATIC VARIABLES
#endif

CMCUIScreenMain			_screenMain;
CMCUIScreenListSaves	_screenListSaves;
CMCUIScreenLoad			_screenLoad;
CMCUIScreenSave			_screenSave;

//Ordering of _pScreenArray must be consistent with EShellMCUIScreen as
//defined in shellMCUI.h:
CMCUIScreen* 		_pScreenArray[SHELLMCUI_SCREEN_NUM] = {	&_screenMain,
															&_screenListSaves,
															&_screenLoad,
															&_screenSave};

EShellMCUIScreen	_currentScreenID	= SHELLMCUI_SCREEN_MAIN;
CMCUIScreen*		_pCurrentScreen 	= _pScreenArray[_currentScreenID];

//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------
#ifdef __MWERKS__
	#pragma mark -
	#pragma mark GLOBAL FUNCTIONS:
#endif


//-----------------------------------------------------------------------------
// shellMCUIInit()
// Description	:	This function initialises the memory card UI module
// Parameters	:	pInitParms	: Initialisation parameters (see definition in 
//								  shellMCUI.h)
// Returns		:	Nothing
// Notes		: 	-
//-----------------------------------------------------------------------------
void shellMCUIInit(ShellMCUIInitParms* pInitParms)
{
#ifdef NDEBUG
	char __version[]="PsIIShlMCUIR2000";
#else
	char __version[]="PsIIShlMCUID2000";
#endif	
	asm volatile("nop" : : "r"(__version));

	//shellMemCardInit does error checking on keyCode & productModelNo:
	shellMemCardInit(pInitParms->keyCode, pInitParms->productModelNo);

	//Initial update call to "wake up" the mem card system:
	shellMemCardUpdate(0, 0, NULL);
}


//-----------------------------------------------------------------------------
// shellMCUIUpdate()
// Description	:	Tick function which must be called every frame when using
//					shellMCUI
// Parameters	:	pOutputParms	: Output parameters (see definition in 
//									  shellMCUI.h)
// Parameters	:	pInputParms		: Input parameters (see definition in
//									  shellMCUI.h)
// Returns		:	Nothing
// Notes		: 	-
//-----------------------------------------------------------------------------
EShellMCUIScreen shellMCUIUpdate(ShellMCUIOutputs* pOutputParms,
						 	     ShellMCUIInputs*  pInputParms)
{
	int i;

	 shellMemCardUpdate(pInputParms->portNo,
					   pInputParms->slotNo, 
					   &pOutputParms->cardStatus);

	//*************************************************************************	    		
	//Reset output parms:

	//Reset all statusMessage flags:
	for(i = 0; i < SHELLMCUI_STATUS_NUM; i++)
	{
		pOutputParms->statusFlags[i] = false;
	}

	//Reset all request user flags:
	for(i = 0; i < SHELLMCUI_REQUEST_USER_NUM; i++)
	{
		pOutputParms->requestUser[i] = false;
	}

	//Reset all request data flags:
	for(i = 0; i < SHELLMCUI_REQUEST_DATA_NUM; i++)
	{
		pOutputParms->requestData[i] = false;
	}
	

	//*************************************************************************	    		
	//Set general statusMessage flags:
	if(pOutputParms->cardStatus.type == SHELLMEMCARD_INDETERMINATE)
	{
		pOutputParms->statusFlags[SHELLMCUI_STATUS_DETECTING] = true;
	}
	else if(pOutputParms->cardStatus.type != sceMcTypePS2)
	{
		pOutputParms->statusFlags[SHELLMCUI_STATUS_PS2_CARD_NOT_INSERTED] = true;
	}
	else if(pOutputParms->cardStatus.formatStatus)
	{
		pOutputParms->statusFlags[SHELLMCUI_STATUS_PS2_CARD_INSERTED] = true;
	}
	else
	{
		pOutputParms->statusFlags[SHELLMCUI_STATUS_UNFORMATTED_CARD] = true;
	}		

	//*************************************************************************	    		
	//Calculate freeSpaceToDisplay:
	pOutputParms->freeSpaceToDisplay = pOutputParms->cardStatus.numFreeClusters - 2;
	if(pOutputParms->freeSpaceToDisplay < 0) pOutputParms->freeSpaceToDisplay = 0;

	//*************************************************************************	    		
	ASSERT(_pCurrentScreen, "Null pointer");
	_currentScreenID = _pCurrentScreen->Update(pOutputParms, pInputParms);
	_pCurrentScreen  = _pScreenArray[_currentScreenID];

	//*************************************************************************	    		
	//Reset input parms:
	pInputParms->userInput.userMainMenuOption 	= SHELLMCUI_MAIN_MENU_OPTION_NULL;
	pInputParms->userInput.userYesNo 			= SHELLMCUI_YESNO_NULL;
	pInputParms->userInput.userContinue 		= false;
	pInputParms->userInput.userCancel 			= false;
	pInputParms->dataValid 						= false;
	
	//*************************************************************************	    		

	return _currentScreenID;
}
