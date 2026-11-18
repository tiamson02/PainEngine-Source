//-----------------------------------------------------------------------------
//              shellMCUIScreenLoad - Memory Card User Interface Driver
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
#include "shellMCUIScreenLoad.h"

CMCUIScreenLoad::CMCUIScreenLoad()
{
	_numFilesLoaded = 0;
	_loadSuccessful = true;
}


EShellMCUIScreen CMCUIScreenLoad::Update(ShellMCUIOutputs* pOutputParms,
							 			 ShellMCUIInputs*  pInputParms)
{
	typedef enum _EUpdateLoadState
	{
		UPDATE_LOAD_STATE_INITIALISE			= 0,
		UPDATE_LOAD_STATE_CHECK_ICON_SYS,
		UPDATE_LOAD_STATE_FIND_FILE,
		UPDATE_LOAD_STATE_LOAD_FILE,
		UPDATE_LOAD_STATE_CHECK_LOAD_COMPLETION,
		UPDATE_LOAD_STATE_COMPLETE,

		UPDATE_LOAD_STATE_NUM
	} EUpdateLoadState;


	static int 					iconSysSize;
	ShellMemCardFunctionResult	functionResult;
	int							returnVal;

	//Card status must be OK in order for this fn to be called (see
	//_UpdateListSaves())

	/*
	//Check card status:
	
	//If status not OK => Display error message
	//Possible errors:
	//	- PS2 card not inserted
	//		Display message and return to Check card status
	//
	//	- Unformatted
	//		If unformatted, go to format Y/N menu, and if yes then return to save
	//		menu following formatting

	//Select load slot:

	//Check load exists:
	//	- If load doesn't exist, display message and return to Check card status

	//Check card status:
	*/

	//Do load:
	//	- Display TRC "Loading" message
	//Enable Loading" message:
	pOutputParms->statusFlags[SHELLMCUI_STATUS_LOADING]	= true;
	
	//	- Load user data file if reqd
	//	- Load any other data files

	switch(_substate)
	{		
		case UPDATE_LOAD_STATE_INITIALISE:
		{			
			//Reset local state variables:
			_numFilesLoaded = 0;
			
			//Wait for app to pass in load file(s) parms:
			//if(pInputParms->numDataFiles > 0)
			if(pInputParms->dataValid)			
			{
				ASSERT(pInputParms->numDataFiles > 0, "Invalid numDataFiles");

				//Check icon.sys file here:
				returnVal = shellMemCardFindFile(	pInputParms->portNo,
												 	pInputParms->slotNo,											 	
													SHELLMEMCARD_FILE_NAME_ICON_SYS,
													pInputParms->dirNameExtension, 		
													&iconSysSize,
													NULL);
				
				CheckReturnValue(	&_loadSuccessful, 				//pSuccess
									returnVal,						//returnVal
									(_substate + 1), 				//nextStateOnSuccess
							 	 	UPDATE_LOAD_STATE_COMPLETE);	//nextStateOnFail
			}
			else
			{
				//Request application for load parms:
				pOutputParms->requestData[SHELLMCUI_REQUEST_DATA_LOAD_PARMS] = true;			
			}
		}
		break;

		//Check icon.sys file here:
		case UPDATE_LOAD_STATE_CHECK_ICON_SYS:
		{
			//Wait for find file completion:
			shellMemCardGetFunctionResult(SHELLMEMCARD_STATE_FIND_FILE,
								 		 &functionResult);

			if(functionResult.success == SHELL_SUCCESS)					
			{		
				if(iconSysSize == sizeof(sceMcIconSys))
				{
					//icon.sys OK => goto next substate:
					_substate++;
				}
				else
				{
					//icon.sys corrupted...
					_loadSuccessful	= false;
					_substate		= UPDATE_LOAD_STATE_COMPLETE;
					PRINT("icon.sys is corrupted (size = %d)\n", iconSysSize);
					break;
				}
			}
			else if(functionResult.success == SHELL_FAIL)
			{
				_loadSuccessful	= false;
				_substate		= UPDATE_LOAD_STATE_COMPLETE;
			}
		}
		break;

		case UPDATE_LOAD_STATE_FIND_FILE:
		{
			//Start load file loop:
			//Determine file size:
			returnVal = shellMemCardFindFile(	pInputParms->portNo,
											 	pInputParms->slotNo,											 	
												pInputParms->dataFileNames[_numFilesLoaded],
												pInputParms->dirNameExtension, 		
												&pInputParms->dataFileSize[_numFilesLoaded],
												NULL);
												
												
			CheckReturnValue(&_loadSuccessful, returnVal, (_substate + 1), 
							 UPDATE_LOAD_STATE_COMPLETE);
			
			PRINT("Loading %s/%s\n", pInputParms->dirNameExtension,
									 pInputParms->dataFileNames[_numFilesLoaded]);			
		}
		break;

		case UPDATE_LOAD_STATE_LOAD_FILE:
		{
			//Wait for find file completion:
			shellMemCardGetFunctionResult(SHELLMEMCARD_STATE_FIND_FILE,
								 		 &functionResult);
		

			if(functionResult.success == SHELL_SUCCESS)					
			{		
				if(pInputParms->dataFileSize[_numFilesLoaded] <= 0)
				{
					//Find file failed...
					_loadSuccessful	= false;
					_substate		= UPDATE_LOAD_STATE_COMPLETE;
					PRINT("File %s not found\n", pInputParms->dataFileNames[_numFilesLoaded]);
					break;
				}
			
		
				//Malloc memory for file load:
				pInputParms->pDataFileData[_numFilesLoaded] = 
					(char*) MCUI_MALLOC((u_int) pInputParms->dataFileSize[_numFilesLoaded]);
				if(!pInputParms->pDataFileData[_numFilesLoaded])
				{
					_loadSuccessful	= false;
					_substate		= UPDATE_LOAD_STATE_COMPLETE;				
					PRINT("Malloc failed");
					break;
				}

				//Initiate loading:
				returnVal = shellMemCardLoadFile(	pInputParms->portNo,
												 	pInputParms->slotNo,
													pInputParms->dirNameExtension, 												
													pInputParms->dataFileNames[_numFilesLoaded],
													pInputParms->pDataFileData[_numFilesLoaded],
													(u_int) pInputParms->dataFileSize[_numFilesLoaded]);

				CheckReturnValue(&_loadSuccessful, returnVal, (_substate + 1), 
				 				 UPDATE_LOAD_STATE_COMPLETE);
			}
			else if(functionResult.success == SHELL_FAIL)
			{
				_loadSuccessful	= false;
				_substate		= UPDATE_LOAD_STATE_COMPLETE;
			}																														
			
		}
		break;

		case UPDATE_LOAD_STATE_CHECK_LOAD_COMPLETION:
		{			
			//Wait for load completion:
			shellMemCardGetFunctionResult(SHELLMEMCARD_STATE_LOAD_FILE,
								 		 &functionResult);
			
			if(functionResult.success == SHELL_SUCCESS)					
			{
				//If load complete:			
				_numFilesLoaded++;
				
				//If (more files to be loaded), go to
				//UPDATE_LOAD_STATE_FIND_FILE state:
				if(_numFilesLoaded < pInputParms->numDataFiles)
				{
					_substate = UPDATE_LOAD_STATE_FIND_FILE;
				}
				else
				{
					//else loading complete
					_loadSuccessful = true;
					_substate++;
					
					PRINT("Load complete!\n");
				}
			}
			else if(functionResult.success == SHELL_FAIL)
			{
				_loadSuccessful = false;
				_substate++;
				PRINT("Load failed\n");
			}
		}
		break;
		
		case UPDATE_LOAD_STATE_COMPLETE:
		{			
			//Prompt for user continue, then return to main menu:

			//Disable "loading" message:
			pOutputParms->statusFlags[SHELLMCUI_STATUS_LOADING]				= false;

			if(_loadSuccessful)
			{
				//Display "load successful" message:
				pOutputParms->statusFlags[SHELLMCUI_STATUS_LOAD_SUCCESSFUL]	= true;
			}
			else
			{
				//Display "load failed" message:
				pOutputParms->statusFlags[SHELLMCUI_STATUS_LOAD_FAILED]		= true;			
			}
			
			//Prompt for user continue:
			pOutputParms->requestUser[SHELLMCUI_REQUEST_USER_CONTINUE]		= true;

			//Upon user continue, return to main screen:
			if(pInputParms->userInput.userContinue == true)
			{	
				ResetScreen();
			}
		}
		break;
		
		default:
			ASSERT(0, "Invalid switch case");				
	}

	//If load not OK => Display error message
	//Possible errors:
	//	- PS2 card removed
	//		Display message and return to Check card status
	//	- Memory allocation failed
	
	return _currentScreen;
}
