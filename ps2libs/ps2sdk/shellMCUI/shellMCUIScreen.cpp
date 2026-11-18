//-----------------------------------------------------------------------------
//               shellMCUIScreen - Memory Card User Interface Driver
//
// Memory Card User Interface Driver for TRC compliant memory card user
// interface implementation
//
// $Workfile: shellMCUI.h $
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
#include "shellMCUIScreen.h"


//Initialise statics:
EShellMCUIMainMenuOption	CMCUIScreen::_mainMenuOption		= SHELLMCUI_MAIN_MENU_OPTION_NULL;
EShellMCUIScreen			CMCUIScreen::_currentScreen			= SHELLMCUI_SCREEN_MAIN;
EShellMCUIScreen			CMCUIScreen::_previousScreen		= SHELLMCUI_SCREEN_MAIN;

CMCUIScreen::CMCUIScreen()
{
	_substate	= 0;
}


void CMCUIScreen::SetNextScreen(EShellMCUIScreen nextScreen)
{
	//Error checking:
	ASSERT(	(nextScreen > SHELLMCUI_SCREEN_NULL) &&
			(nextScreen < SHELLMCUI_SCREEN_NUM),
			"Invalid nextScreen");
	
	_previousScreen	= _currentScreen;
	_currentScreen 	= nextScreen;
	
	if(_currentScreen == SHELLMCUI_SCREEN_MAIN)
	{
		_mainMenuOption = SHELLMCUI_MAIN_MENU_OPTION_NULL;
	}
}


void CMCUIScreen::ResetScreen()
{
	//Reset substate:
	_substate	= 0;
	SetNextScreen(SHELLMCUI_SCREEN_MAIN);
}


void CMCUIScreen::CheckUserCancel(ShellMCUIOutputs*	pOutputParms,
								  ShellMCUIInputs* 	pInputParms, 
							  	  EShellMCUIScreen	previousScreen)
{
	//Prompt for user cancel:
	pOutputParms->requestUser[SHELLMCUI_REQUEST_USER_CANCEL] = true;

	if(pInputParms->userInput.userCancel)
	{
		//Reset substate:
		_substate	= 0;		
		SetNextScreen(previousScreen);
	}	
}


void CMCUIScreen::CheckCardStatus(ShellMCUIOutputs* pOutputParms)
{
	//If PS2 card not inserted, return to main menu:
	if(pOutputParms->cardStatus.type != sceMcTypePS2)
	{
		ResetScreen();
	}
}



void CMCUIScreen::CheckReturnValue(bool* pSuccess, int returnVal,
								   int nextStateOnSuccess, int nextStateOnFail)
{
	switch(returnVal)
	{
		case SHELL_SUCCESS:
		{
			*pSuccess 	= true;
			_substate	= nextStateOnSuccess;		
		}
		break;

		case SHELL_FAIL:
		{
			//Handle error:
			*pSuccess 	= false;
			_substate	= nextStateOnFail;
			PRINT("Save failed\n");		
		}
		break;

		case SHELL_INDETERMINATE:
		{
			//Do nothing:
		}
		break;
		
		default:
			ASSERT(0, "Invalid switch case");					
	}
}
