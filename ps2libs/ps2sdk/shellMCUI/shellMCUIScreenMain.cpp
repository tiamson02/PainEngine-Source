//-----------------------------------------------------------------------------
//              shellMCUIScreenMain - Memory Card User Interface Driver
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
#include "shellMCUIScreenMain.h"


CMCUIScreenMain::CMCUIScreenMain()
{




}


EShellMCUIScreen CMCUIScreenMain::Update(ShellMCUIOutputs* pOutputParms,
							 			 ShellMCUIInputs*  pInputParms)
{
	//Debug:
	//pOutputParms->statusMessages[SHELLMCUI_STATUS_MSG_FORMAT_FAILED]	= true;

	pOutputParms->mainMenuOptionsValid[SHELLMCUI_MAIN_MENU_OPTION_LOAD]	= true;
	pOutputParms->mainMenuOptionsValid[SHELLMCUI_MAIN_MENU_OPTION_SAVE]	= true;
	
	if(pOutputParms->cardStatus.type != sceMcTypePS2)
	{
		//If card not inserted, disable load & save options:
		pOutputParms->mainMenuOptionsValid[SHELLMCUI_MAIN_MENU_OPTION_LOAD]	= false;
		pOutputParms->mainMenuOptionsValid[SHELLMCUI_MAIN_MENU_OPTION_SAVE]	= false;	
	}
	else if(!pOutputParms->cardStatus.formatStatus)
	{
		//If card not formatted, disable load only:
		pOutputParms->mainMenuOptionsValid[SHELLMCUI_MAIN_MENU_OPTION_LOAD]	= false;	
	}

	//If formatted PS2 card inserted, allow user to select option:
	//Allow save option to be selected if card unformatted
	if(pOutputParms->cardStatus.type == sceMcTypePS2)
	{
		switch(pInputParms->userInput.userMainMenuOption)
		{
	
			case SHELLMCUI_MAIN_MENU_OPTION_LOAD:
			{
				//If card formatted, do load:
				if(pOutputParms->cardStatus.formatStatus)
				{
					SetNextScreen(SHELLMCUI_SCREEN_LIST_SAVES);				
				}
			}			
			break;	

			case SHELLMCUI_MAIN_MENU_OPTION_SAVE:
			{
				//If card formatted, do save regardless of format status:
				SetNextScreen(SHELLMCUI_SCREEN_LIST_SAVES);				
			}			
			break;	
	
			case SHELLMCUI_MAIN_MENU_OPTION_NULL:
			{
				//Do nothing
			}			
			break;	
	
			default:
				ASSERT(0, "Invalid switch case");
		}		

		//Store main menu option, ie remember whether user selected load or save:
		_mainMenuOption							= pInputParms->userInput.userMainMenuOption;
		pOutputParms->mainMenuOptionSelected	= pInputParms->userInput.userMainMenuOption;		
	}

	return _currentScreen;
}
