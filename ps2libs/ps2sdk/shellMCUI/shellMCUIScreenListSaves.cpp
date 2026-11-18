//-----------------------------------------------------------------------------
//              shellMCUIScreenListSaves - Memory Card User Interface Driver
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
#include "shellMCUIScreenListSaves.h"


CMCUIScreenListSaves::CMCUIScreenListSaves()
{




}


EShellMCUIScreen CMCUIScreenListSaves::Update(ShellMCUIOutputs* pOutputParms,
						 				 	  ShellMCUIInputs*  pInputParms)
{
	typedef enum _EUpdateListSavesState
	{
		UPDATE_LIST_SAVES_STATE_INITIALISE		= 0,
		UPDATE_LIST_SAVES_STATE_GET_SAVE_LIST,
		UPDATE_LIST_SAVES_STATE_GET_SAVE_LIST_WAIT,
		UPDATE_LIST_SAVES_STATE_COMPLETE,

		UPDATE_LIST_SAVES_STATE_NUM
	} EUpdateListSavesState;

	ShellMemCardFunctionResult	functionResult;
	bool						checkUserCancel	= true;
	int							returnVal;
	int							i;
	
	//Card status must be OK in order for this fn to be called (see
	//_UpdateMain())
	
	/*
	//Check card status:
	
	//If status not OK => Display error message
	//Possible errors:
	//	- PS2 card not inserted
	//		Display message and return to Check card status
	//
	//	- Unformatted
	//		Mustn't reach this screen if user is loading => ASSERT that user is saving
	//		Skip to "wait for slot selection" state, then proceed to save state
	*/
	
	//Get dir list (for one dir per save slot)
	//	- Display TRC "Please wait" message	
	//pOutputParms->statusFlags[SHELLMCUI_STATUS_DETECTING] = true;
	
	switch(_substate)
	{		
		case UPDATE_LIST_SAVES_STATE_INITIALISE:
		{		
			if(pInputParms->dataValid)			
			{
				//Initialise relevant output parms:
				pOutputParms->numSavesFound = 0;
				_substate++;
			}
			else
			{
				//Request application for list save parms:
				pOutputParms->requestData[SHELLMCUI_REQUEST_DATA_LIST_SAVE_PARMS] = true;			
			}
		} 
		break;


		case UPDATE_LIST_SAVES_STATE_GET_SAVE_LIST:
		{
			if(pOutputParms->cardStatus.formatStatus)
			{
				returnVal = shellMemCardGetDirList(	pInputParms->portNo,
												 	pInputParms->slotNo,
													pInputParms->dirNameExtension, 
													0, 
													//_maxNumSavesPerCard, 
													SHELLMCUI_MAX_NUM_SAVES_PER_CARD,
													&pOutputParms->numSavesFound,												
													_saveData);									

				if(returnVal == SHELL_FAIL)
				{
					//Handle error:
					//Return to main menu; error message not required since load / save has 
					//not commenced?:
					ResetScreen();				
				}		
				
				_substate++;
			}
			else
			{
				//Card not formatted; skip card reading and wait for user slot selection:
				_substate = UPDATE_LIST_SAVES_STATE_COMPLETE;
			}
		}
		break;

		case UPDATE_LIST_SAVES_STATE_GET_SAVE_LIST_WAIT:
		{
			//Wait for save GetDirList completion:
			shellMemCardGetFunctionResult(SHELLMEMCARD_STATE_GET_DIR_LIST,
								 		 &functionResult);
			
			if(functionResult.success == SHELL_SUCCESS)					
			{
				//printf("Success!\n");
				
				ASSERT(	pOutputParms->numSavesFound < SHELLMCUI_MAX_NUM_SAVES_PER_CARD,
						"SHELLMCUI_MAX_NUM_SAVES_PER_CARD exceeded");
						
				//Copy save names into pOutputParms:
				for(i = 0; i < pOutputParms->numSavesFound; i++)
				{
					strcpy(pOutputParms->saveNames[i], (char*) _saveData[i].EntryName);
				}
				_substate++;	
			}
			else if(functionResult.success == SHELL_FAIL)
			{
				//printf("Fail...\n");

				//Return to main menu; error message not required since load / save has 
				//not commenced?:
				ResetScreen();
			}	
		}
		break;
		
		//ToDo: Check icon.sys in each dir here. If icon.sys is not found, 
		//don't list save.
		
		case UPDATE_LIST_SAVES_STATE_COMPLETE:
		{
			//Disable "Busy" message:
			pOutputParms->statusFlags[SHELLMCUI_STATUS_DETECTING] 			 	= false;

			pOutputParms->statusFlags[SHELLMCUI_STATUS_LIST_SAVES_SUCCESSFUL] 	= true;
			
			if(pOutputParms->numSavesFound > 0)
			{
				//pOutputParms->statusFlags[SHELLMCUI_STATUS_LIST_SAVES_SUCCESSFUL] = true;
			}
			else
			{
				//pOutputParms->statusFlags[SHELLMCUI_STATUS_LIST_SAVES_SUCCESSFUL] = false;
				//Only display "No saves" message if loading:
				if(_mainMenuOption == SHELLMCUI_MAIN_MENU_OPTION_LOAD)
				{
					pOutputParms->statusFlags[SHELLMCUI_STATUS_NO_SAVES]		= true;			

					//Prompt for user continue:
					pOutputParms->requestUser[SHELLMCUI_REQUEST_USER_CONTINUE]	= true;
					checkUserCancel = false;
				}
			}
					
			//Upon slot selection, go to load / save screen:			
			if(pInputParms->userInput.userContinue == true)
			{	
				//Go either to load or save screen depending upon user's
				//main menu selection:
				_substate = 0;
				
				switch(_mainMenuOption)
				{
					case SHELLMCUI_MAIN_MENU_OPTION_LOAD:
					{
						if(pOutputParms->numSavesFound > 0)
						{
							SetNextScreen(SHELLMCUI_SCREEN_LOAD);
						}
						else
						{
							//If no saves found, return to main screen:
							SetNextScreen(SHELLMCUI_SCREEN_MAIN);						
						}
					}
					break;

					case SHELLMCUI_MAIN_MENU_OPTION_SAVE:
						SetNextScreen(SHELLMCUI_SCREEN_SAVE);
					break;

					default:
						ASSERT(0, "Invalid switch case");
				}		
					
			}
			CheckCardStatus(pOutputParms);
		}
		break;
		
		default:
			ASSERT(0, "Invalid switch case");				
	}

	//If list not OK => Display error message
	//Possible errors:
	//	- PS2 card removed
	//		Display message and return to previous menu?

	//Return to previous menu (load or save)

	if(checkUserCancel)
	{
		CheckUserCancel(pOutputParms, pInputParms, SHELLMCUI_SCREEN_MAIN);
	}

	return _currentScreen;
}
