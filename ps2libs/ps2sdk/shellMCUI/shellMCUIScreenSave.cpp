//-----------------------------------------------------------------------------
//              shellMCUIScreenSave - Memory Card User Interface Driver
//
// Memory Card User Interface Driver for TRC compliant memory card user
// interface implementation
//
// $Workfile: memCardUIScreenLoad.h $
// $Author: Hbodasin $
// $Date: 1/11/02 14:46 $
// $Revision: 1 $
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
#include "shellMCUIScreenSave.h"


//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------
#ifdef __MWERKS__
	#pragma mark -
	#pragma mark TYPEDEFS
#endif

typedef enum _EUpdateSaveState
{
	//Pre-save checks:
	UPDATE_SAVE_STATE_INITIALISE	= 0,
	UPDATE_SAVE_STATE_FORMAT,
	UPDATE_SAVE_STATE_FIND_SAVE,
	UPDATE_SAVE_STATE_SAVE_CHECK_SPACE,
	UPDATE_SAVE_STATE_SAVE_INSUFFICIENT_SPACE,
	UPDATE_SAVE_STATE_SAVE_CREATE_DIR,
	UPDATE_SAVE_STATE_SAVE_CREATE_DIR_WAIT,
	UPDATE_SAVE_STATE_OVERWRITE,
	UPDATE_SAVE_STATE_DELETE_ICON_SYS,

	//Save files:
	UPDATE_SAVE_STATE_SAVE_START,
	UPDATE_SAVE_STATE_SAVE_USER_DATA_FILE = UPDATE_SAVE_STATE_SAVE_START,
	UPDATE_SAVE_STATE_SAVE_USER_DATA_FILE_WAIT,
	UPDATE_SAVE_STATE_SAVE_DATA_FILES,
	UPDATE_SAVE_STATE_SAVE_DATA_FILES_WAIT,
	UPDATE_SAVE_STATE_SAVE_ICON_FILES,
	UPDATE_SAVE_STATE_SAVE_ICON_FILES_WAIT,
	UPDATE_SAVE_STATE_SAVE_ICON_SYS,
	UPDATE_SAVE_STATE_SAVE_ICON_SYS_WAIT,

	//Save completion:
	UPDATE_SAVE_STATE_COMPLETE,
	
	UPDATE_SAVE_STATE_NUM
} EUpdateSaveState;




//-----------------------------------------------------------------------------
// M E M B E R   F U N C T I O N S
//-----------------------------------------------------------------------------
#ifdef __MWERKS__
	#pragma mark -
	#pragma mark MEMBER FUNCTIONS:
#endif

CMCUIScreenSave::CMCUIScreenSave()
{
	_saveSuccessful 	= true;
	_formatSuccessful	= true;
	_saveDirFound		= true;
	_numDataFilesSaved 	= 0;	
	_numIconFilesSaved 	= 0;	
	_substateFormat		= 0;
}


/*
//ToDo: Perhaps make this a base class fn?
void CMCUIScreenSave::CheckReturnValue(bool* pSuccess, int returnVal,
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
*/

void CMCUIScreenSave::CheckSaveParms(ShellMCUIInputs* pInputParms)
{
	int i;
	
	ASSERT(pInputParms->saveParms.pIconSysData, "Null pointer");
	
	for(i = 0; i < SHELLMEMCARD_ICON_NUM_ICONS; i++)
	{	
		ASSERT(pInputParms->saveParms.pIconFileData[i], 			"Null pointer");
		ASSERT(pInputParms->saveParms.iconFileSize[i] > 0, 			"Invalid file size");
		ASSERT(strlen(pInputParms->saveParms.iconFileNames[i]) > 0, "Invalid file name");
	}

	ASSERT(pInputParms->saveParms.pUserFileData, 		"Null pointer");
	ASSERT(pInputParms->saveParms.userFileSize > 0, 	"Invalid file size");

	ASSERT(((pInputParms->numDataFiles >= 0) && 
		    (pInputParms->numDataFiles <= SHELLMCUI_MAX_NUM_DATA_FILES)),
		   "Invalid numDataFiles");

	for(i = 0; i < pInputParms->numDataFiles; i++)
	{			
		ASSERT(pInputParms->pDataFileData[i], 				"Null pointer");
		ASSERT(pInputParms->dataFileSize[i] > 0, 			"Invalid file size");
		ASSERT(strlen(pInputParms->dataFileNames[i]) > 0, 	"Invalid file name");
	}
}



int CMCUIScreenSave::CalculateSaveSize(ShellMCUIInputs* pInputParms)
{
	int		numFilesToSave,
			fileDataSize = 0,
			i;
	u_int 	iconDataSize = 0,			
			totalSaveSize;
			//i;

	iconDataSize += SHELLMEMCARD_GET_CLUSTER_SIZE(sizeof(sceMcIconSys));
	
	for(i = 0; i < SHELLMEMCARD_ICON_NUM_ICONS; i++)
	{			
		iconDataSize += SHELLMEMCARD_GET_CLUSTER_SIZE(pInputParms->saveParms.iconFileSize[i]);
	}

	for(i = 0; i < pInputParms->numDataFiles; i++)
	{			
		fileDataSize += SHELLMEMCARD_GET_CLUSTER_SIZE(pInputParms->dataFileSize[i]);
	}
	
	
	//+2 for icon.sys and userDataFile:
	numFilesToSave = pInputParms->numDataFiles + SHELLMEMCARD_ICON_NUM_ICONS + 2;
	
	totalSaveSize =	
		iconDataSize														+	
		fileDataSize														+
		//User data file:
		SHELLMEMCARD_GET_CLUSTER_SIZE(pInputParms->saveParms.userFileSize)	+							+
		
		SHELLMEMCARD_GET_CLUSTERS_FOR_FILE_ENTRIES(numFilesToSave)			+
		SHELLMEMCARD_CLUSTERS_FOR_DIRECTORY;
		
	//PRINT("Total save size = %dK\n", totalSaveSize);	
	return (int) totalSaveSize;
}


EShellMCUIScreen CMCUIScreenSave::Update(ShellMCUIOutputs* 	pOutputParms,
							 			 ShellMCUIInputs*  	pInputParms)
{
	if(_substate < UPDATE_SAVE_STATE_SAVE_START)
	{
		UpdatePreSaveChecks(pOutputParms, pInputParms);
	}
	else if(_substate < UPDATE_SAVE_STATE_COMPLETE)
	{
		UpdateSaveFiles(pOutputParms, pInputParms);
	}
	else
	{
		UpdateSaveCompletion(pOutputParms, pInputParms);		
	}

	return _currentScreen;
}



	
//If save exists, 
//	Calc existing save size
//	Calc reqd save size
//	Check for sufficient space
//	Prompt for overwrite:
// 		Display Explicit Approval Message (Are You Sure? YES/NO) ?
//If dir doesn't exist, create dir:
void CMCUIScreenSave::UpdatePreSaveChecks(ShellMCUIOutputs* pOutputParms,
					 					  ShellMCUIInputs*  pInputParms)
{
	ShellMemCardFunctionResult	functionResult;
	int							returnVal;

	switch(_substate)
	{		
		case UPDATE_SAVE_STATE_INITIALISE:
		{		
			//numDataFilesSaved = 0;
			//numIconFilesSaved = 0;	
			_saveSuccessful 	= true;
			_saveDirFound		= true;
			_numDataFilesSaved 	= 0;	
			_numIconFilesSaved 	= 0;	

			//Wait for app to pass in save files parms:			
			//if(pInputParms->saveParms.pIconSysData)
			if(pInputParms->dataValid)
			{				
				//Verify save parms (no null pointers, empty filenames, etc):
				CheckSaveParms(pInputParms);
								
				if(pOutputParms->cardStatus.formatStatus)
				{
					_substate = UPDATE_SAVE_STATE_FIND_SAVE;
				}
				else
				{
					_substate 		= UPDATE_SAVE_STATE_FORMAT;			
					_substateFormat	= 0;
				}
			}
			else
			{			
				//Request application for save parms:
				pOutputParms->requestData[SHELLMCUI_REQUEST_DATA_SAVE_PARMS] = true;			
			}
		}
		break;

		case UPDATE_SAVE_STATE_FORMAT:
		{			
			//Switch to format screen:
			UpdateFormat(pOutputParms, pInputParms);
		}
		break;

		case UPDATE_SAVE_STATE_FIND_SAVE:
		{
			//returnVal = shellMemCardFindDir(pInputParms->portNo,
			//								pInputParms->slotNo,	
			//								pInputParms->dirNameExtension, 		
			//								&_saveDirFound, NULL);

			returnVal = shellMemCardGetDirSize(	pInputParms->portNo,
												pInputParms->slotNo,	
												pInputParms->dirNameExtension, 		
												&_existingSaveSize);

			CheckReturnValue(&_saveSuccessful, returnVal, (_substate + 1),
							 UPDATE_SAVE_STATE_COMPLETE);
		}
		break;


		case UPDATE_SAVE_STATE_SAVE_CHECK_SPACE:
		{
			int spaceAvailable, currentSaveSize;
			
			//Wait for find save completion:
			//shellMemCardGetFunctionResult(SHELLMEMCARD_STATE_FIND_DIR,
			//					 		 &functionResult);
			shellMemCardGetFunctionResult(SHELLMEMCARD_STATE_GET_DIR_SIZE,
								 		 &functionResult);

			if(functionResult.success == SHELL_SUCCESS)					
			{		
				//int existingSaveSize = 0;
				
				//If save exists, calc existing save size:		
				_saveDirFound = (_existingSaveSize >= 0);
				
				if(_saveDirFound)
				{
					PRINT("Existing save found\n");
				}
				
				if(_existingSaveSize < 0)
				{
					_existingSaveSize = 0;
				}

				currentSaveSize = CalculateSaveSize(pInputParms);
				pOutputParms->spaceRequiredForSave = (int) currentSaveSize;
				
				//Space available = (free space) + (existing save space)
				spaceAvailable = pOutputParms->cardStatus.numFreeClusters + _existingSaveSize;
			
				//If (space available) < (space reqd)
				//if(0)
				if(spaceAvailable < currentSaveSize)
				{
					//Insufficent space - save failed:
					_saveSuccessful 	= true;					
					_substate = UPDATE_SAVE_STATE_SAVE_INSUFFICIENT_SPACE;																				
				}
				else if(_saveDirFound)
				{
					_substate = UPDATE_SAVE_STATE_OVERWRITE;							
				}
				else
				{
					_substate = UPDATE_SAVE_STATE_SAVE_CREATE_DIR;											
				}
			}
			else if(functionResult.success == SHELL_FAIL)
			{
				_saveSuccessful = false;
				_substate = UPDATE_SAVE_STATE_COMPLETE;															
			}
		}
		break;

		case UPDATE_SAVE_STATE_SAVE_INSUFFICIENT_SPACE:
		{
			//Prompt for overwrite:		
			pOutputParms->statusFlags[SHELLMCUI_STATUS_INSUFFICIENT_SPACE]	= true;

			//Prompt for user continue:
			pOutputParms->requestUser[SHELLMCUI_REQUEST_USER_CONTINUE]		= true;

			//Upon user continue, return to list saves screen:
			if(pInputParms->userInput.userContinue == true)
			{	
				_substate = UPDATE_SAVE_STATE_INITIALISE;
				SetNextScreen(SHELLMCUI_SCREEN_LIST_SAVES);
			}
			
			CheckCardStatus(pOutputParms);
		}
		break;

		case UPDATE_SAVE_STATE_SAVE_CREATE_DIR:
		{
			//Create dir and save icon files:
			returnVal = shellMemCardCreateDir(pInputParms->portNo,
								 			 pInputParms->slotNo,	
								 			 pInputParms->dirNameExtension);

			CheckReturnValue(&_saveSuccessful, returnVal, (_substate + 1),
							 UPDATE_SAVE_STATE_COMPLETE);

			//Upon success:
			//_substate = UPDATE_SAVE_STATE_SAVE_START;
		}
		break;

		case UPDATE_SAVE_STATE_SAVE_CREATE_DIR_WAIT:
		{
			shellMemCardGetFunctionResult(SHELLMEMCARD_STATE_CREATE_DIR,
								 		 &functionResult);

			CheckReturnValue(&_saveSuccessful, functionResult.success,
							 UPDATE_SAVE_STATE_SAVE_START,
							 UPDATE_SAVE_STATE_COMPLETE);
		}
		break;

		case UPDATE_SAVE_STATE_OVERWRITE:
		{
			//Prompt for overwrite:		
			//pOutputParms->statusFlags[SHELLMCUI_STATUS_EXISTING_SAVE_EXISTS]	= true;
			pOutputParms->statusFlags[SHELLMCUI_STATUS_REQUEST_USER_OVERWRITE]	= true;

			//Prompt for user overwrite y/n:
			pOutputParms->requestUser[SHELLMCUI_REQUEST_USER_YESNO]		= true;

			if(pInputParms->userInput.userYesNo == SHELLMCUI_YESNO_YES)
			{	
				//Delete existing icon.sys file first (reqd in order to check for 
				//corrupt saves later):
				returnVal = shellMemCardDeleteFile(	pInputParms->portNo,
								 			 		pInputParms->slotNo,	
								 			 		pInputParms->dirNameExtension,
								 			 		SHELLMEMCARD_FILE_NAME_ICON_SYS);
				
				CheckReturnValue(&_saveSuccessful, returnVal,
								 UPDATE_SAVE_STATE_DELETE_ICON_SYS,
								 UPDATE_SAVE_STATE_COMPLETE);
				
			}
			else if(pInputParms->userInput.userYesNo == SHELLMCUI_YESNO_NO)
			{
				_substate = UPDATE_SAVE_STATE_INITIALISE;
				SetNextScreen(SHELLMCUI_SCREEN_LIST_SAVES);
			}
			
			CheckUserCancel(pOutputParms, pInputParms, SHELLMCUI_SCREEN_LIST_SAVES);			
			//If PS2 card not inserted, return to main menu:			
			CheckCardStatus(pOutputParms);
		}
		break;

		case UPDATE_SAVE_STATE_DELETE_ICON_SYS:
		{
			shellMemCardGetFunctionResult(SHELLMEMCARD_STATE_DELETE_FILE,
								 		  &functionResult);

			if(functionResult.success != SHELL_INDETERMINATE)
			{
				//If overwriting a corrupted save, icon.sys may not exist => go
				//to next state regardless of successful icon.sys deletion:
				_substate = UPDATE_SAVE_STATE_SAVE_START;
			}
		}
		break;

		default:
			ASSERT(0, "Invalid switch case");				
	}
}



void CMCUIScreenSave::UpdateSaveFiles(ShellMCUIOutputs* pOutputParms,
					 				  ShellMCUIInputs*  pInputParms)
{
	ShellMemCardFunctionResult	functionResult;
	int							returnVal;

	(void) pOutputParms;

	pOutputParms->statusFlags[SHELLMCUI_STATUS_SAVING] = true;
	
	switch(_substate)
	{		
		case UPDATE_SAVE_STATE_SAVE_USER_DATA_FILE:
		{
			char	userDataFileName[SHELLMEMCARD_MAX_LEN_FILENAME];
			
			shellMemCardCreateUserDataFileName(userDataFileName, 
											  pInputParms->dirNameExtension);

			returnVal = shellMemCardSaveFile(	pInputParms->portNo,
												pInputParms->slotNo,
												pInputParms->dirNameExtension,
												userDataFileName,
												pInputParms->saveParms.pUserFileData, 
												pInputParms->saveParms.userFileSize);

			CheckReturnValue(&_saveSuccessful, returnVal, (_substate + 1),
							 UPDATE_SAVE_STATE_COMPLETE);
		}
		break;

		
		case UPDATE_SAVE_STATE_SAVE_USER_DATA_FILE_WAIT:
		{			
			shellMemCardGetFunctionResult(SHELLMEMCARD_STATE_SAVE_FILE,
								 		 &functionResult);


			CheckReturnValue(&_saveSuccessful, functionResult.success,
							 (_substate + 1), UPDATE_SAVE_STATE_COMPLETE);
		}
		break;

		case UPDATE_SAVE_STATE_SAVE_DATA_FILES:
		{			
			returnVal = shellMemCardSaveFile(	pInputParms->portNo,
												pInputParms->slotNo,
												pInputParms->dirNameExtension,
												pInputParms->dataFileNames[_numDataFilesSaved],
												pInputParms->pDataFileData[_numDataFilesSaved],
												(u_int) pInputParms->dataFileSize[_numDataFilesSaved]);

			CheckReturnValue(&_saveSuccessful, returnVal, (_substate + 1),
							 UPDATE_SAVE_STATE_COMPLETE);

			PRINT("Saving %s/%s\n", pInputParms->dirNameExtension,
									pInputParms->dataFileNames[_numDataFilesSaved]);							 
		}
		break;


		case UPDATE_SAVE_STATE_SAVE_DATA_FILES_WAIT:
		{	
			int nextStateOnSuccess = _substate + 1;
					
			shellMemCardGetFunctionResult(SHELLMEMCARD_STATE_SAVE_FILE,
								 		 &functionResult);

			if(functionResult.success == SHELL_SUCCESS)					
			{
				//If this save complete:			
				_numDataFilesSaved++;
			}
			
			if(_numDataFilesSaved < pInputParms->numDataFiles)
			{
				//Save next data file:
				nextStateOnSuccess = UPDATE_SAVE_STATE_SAVE_DATA_FILES;
			}
			
			CheckReturnValue(&_saveSuccessful, functionResult.success,
							 nextStateOnSuccess, UPDATE_SAVE_STATE_COMPLETE);			
		}
		break;

		case UPDATE_SAVE_STATE_SAVE_ICON_FILES:
		{			
			returnVal = shellMemCardSaveFile(	pInputParms->portNo,
												pInputParms->slotNo,
												pInputParms->dirNameExtension,
												pInputParms->saveParms.iconFileNames[_numIconFilesSaved],
												pInputParms->saveParms.pIconFileData[_numIconFilesSaved],
												pInputParms->saveParms.iconFileSize[_numIconFilesSaved]);

			CheckReturnValue(&_saveSuccessful, returnVal, (_substate + 1),
							 UPDATE_SAVE_STATE_COMPLETE);

			PRINT("Saving %s/%s\n", pInputParms->dirNameExtension,
									pInputParms->saveParms.iconFileNames[_numIconFilesSaved]);							 
		}
		break;

		case UPDATE_SAVE_STATE_SAVE_ICON_FILES_WAIT:
		{	
			int nextStateOnSuccess = _substate + 1;
					
			shellMemCardGetFunctionResult(SHELLMEMCARD_STATE_SAVE_FILE,
								 		 &functionResult);

			if(functionResult.success == SHELL_SUCCESS)					
			{
				//If this save complete:			
				_numIconFilesSaved++;
			}
			
			if(_numIconFilesSaved < SHELLMEMCARD_ICON_NUM_ICONS)
			{
				//Save next icon file:
				nextStateOnSuccess = UPDATE_SAVE_STATE_SAVE_ICON_FILES;
			}
			
			CheckReturnValue(&_saveSuccessful, functionResult.success,
							 nextStateOnSuccess, UPDATE_SAVE_STATE_COMPLETE);			
		}
		break;

		case UPDATE_SAVE_STATE_SAVE_ICON_SYS:
		{
			returnVal = shellMemCardSaveFile(	pInputParms->portNo,
												pInputParms->slotNo,
												pInputParms->dirNameExtension,
												SHELLMEMCARD_FILE_NAME_ICON_SYS,
												(char*) pInputParms->saveParms.pIconSysData, 
												sizeof(sceMcIconSys));

			CheckReturnValue(&_saveSuccessful, returnVal, (_substate + 1),
							 UPDATE_SAVE_STATE_COMPLETE);
		}
		break;
		
		case UPDATE_SAVE_STATE_SAVE_ICON_SYS_WAIT:
		{			
			shellMemCardGetFunctionResult(SHELLMEMCARD_STATE_SAVE_FILE,
								 		 &functionResult);

			CheckReturnValue(&_saveSuccessful, functionResult.success,
							 (_substate + 1), UPDATE_SAVE_STATE_COMPLETE);
		}
		break;
		
		default:
			ASSERT(0, "Invalid switch case");				
	}
}



void CMCUIScreenSave::UpdateSaveCompletion(	ShellMCUIOutputs* pOutputParms,
								 			ShellMCUIInputs*  pInputParms)
{
	ASSERT(_substate == UPDATE_SAVE_STATE_COMPLETE, "Incorrect _substate");
	
	//Disable "saving" message:
	pOutputParms->statusFlags[SHELLMCUI_STATUS_SAVING]				= false;

	//Display "save successful / failed" message:
	if(_saveSuccessful)
	{
		pOutputParms->statusFlags[SHELLMCUI_STATUS_SAVE_SUCCESSFUL]	= true;
	}
	else
	{
		pOutputParms->statusFlags[SHELLMCUI_STATUS_SAVE_FAILED]		= true;			
	}
	
	//Prompt for user continue:
	pOutputParms->requestUser[SHELLMCUI_REQUEST_USER_CONTINUE]		= true;

	//Upon user continue, return to main screen:
	if(pInputParms->userInput.userContinue == true)
	{	
		ResetScreen();
	}
}								 			


void CMCUIScreenSave::UpdateFormat(	ShellMCUIOutputs* pOutputParms,
								 	ShellMCUIInputs*  pInputParms)
{
	typedef enum _EUpdateFormatState
	{
		//Pre-save checks:
		UPDATE_FORMAT_STATE_INITIALISE	= 0,
		
		UPDATE_FORMAT_STATE_FORMAT,
		UPDATE_FORMAT_STATE_FORMAT_WAIT,
		UPDATE_FORMAT_STATE_COMPLETE,
		
		UPDATE_FORMAT_STATE_NUM
	} EUpdateFormatState;

	ShellMemCardFunctionResult	functionResult;
	int							returnVal;

	//If user formats card, skip (find existing save) and 
	//(sufficient space) checks
	//If no, return to list saves screen

	switch(_substateFormat)
	{		
		case UPDATE_FORMAT_STATE_INITIALISE:
		{		
			pOutputParms->statusFlags[SHELLMCUI_STATUS_REQUEST_USER_FORMAT]	= true;

			//Request user for format Y/N:
			pOutputParms->requestUser[SHELLMCUI_REQUEST_USER_YESNO] 		= true;			

			//PRINT("Formatting card");
			if(pInputParms->userInput.userYesNo == SHELLMCUI_YESNO_YES)
			{	
				_substateFormat = UPDATE_FORMAT_STATE_FORMAT;
			}
			else if(pInputParms->userInput.userYesNo == SHELLMCUI_YESNO_NO)
			{
				//If no, return to list saves screen:
				_substateFormat = UPDATE_FORMAT_STATE_INITIALISE;			
				_substate 		= UPDATE_SAVE_STATE_INITIALISE;
				SetNextScreen(SHELLMCUI_SCREEN_LIST_SAVES);
			}

			CheckUserCancel(pOutputParms, pInputParms, SHELLMCUI_SCREEN_LIST_SAVES);					
			//If PS2 card not inserted, return to main menu:			
			CheckCardStatus(pOutputParms);	
		}
		break;

		case UPDATE_FORMAT_STATE_FORMAT:
		{		

			returnVal = shellMemCardFormat(pInputParms->portNo,
										  pInputParms->slotNo);
			if(returnVal == SHELL_FAIL)
			{
				_formatSuccessful = false;
				_substateFormat = UPDATE_FORMAT_STATE_INITIALISE;							
			}
			else
			{
				_substateFormat = UPDATE_FORMAT_STATE_FORMAT_WAIT;							
			}
										  
			//CheckReturnValue(&saveSuccessful, returnVal, (_substate + 1),
			//			 	 UPDATE_SAVE_STATE_COMPLETE);				
		}
		break;


		case UPDATE_FORMAT_STATE_FORMAT_WAIT:
		{		
			pOutputParms->statusFlags[SHELLMCUI_STATUS_FORMATTING]	= true;
		
			shellMemCardGetFunctionResult(SHELLMEMCARD_STATE_FORMAT,
								 		 &functionResult);

			if(functionResult.success == SHELL_SUCCESS)					
			{
				_formatSuccessful = true;
				_substateFormat = UPDATE_FORMAT_STATE_COMPLETE;							
			}
			else if(functionResult.success == SHELL_FAIL)
			{
				_formatSuccessful = false;
				_substateFormat = UPDATE_FORMAT_STATE_COMPLETE;							
			}																									
		}
		break;
		
		case UPDATE_FORMAT_STATE_COMPLETE:
		{		
			//Display "format successful / failed" message:
			if(_formatSuccessful)
			{
				//If card formatted, proceed to actual save without further checks or
				//user intervention:
				pOutputParms->statusFlags[SHELLMCUI_STATUS_FORMAT_SUCCESSFUL]	= true;
				_substateFormat = UPDATE_FORMAT_STATE_INITIALISE;			
				_substate 		= UPDATE_SAVE_STATE_SAVE_CREATE_DIR;
								  //UPDATE_SAVE_STATE_SAVE_START;				

				//If PS2 card not inserted, return to main menu:			
				CheckCardStatus(pOutputParms);						
			}
			else
			{
				pOutputParms->statusFlags[SHELLMCUI_STATUS_FORMAT_FAILED]		= true;			

				//Prompt for user continue:
				pOutputParms->requestUser[SHELLMCUI_REQUEST_USER_CONTINUE]		= true;

				//Upon user continue, return to list saves screen?:
				if(pInputParms->userInput.userContinue == true)
				{	
					_substateFormat = UPDATE_FORMAT_STATE_INITIALISE;			
					_substate 		= UPDATE_SAVE_STATE_INITIALISE;
					SetNextScreen(SHELLMCUI_SCREEN_LIST_SAVES);
				}
			}			
		}
		break;
				
		default:
			ASSERT(0, "Invalid switch case");				
	}	
}



//Check card status:

//If status not OK => Display error message
//Possible errors:
//	- PS2 card not inserted
//		Display message and return to Check card status
//
//	- Insufficient space
//		Display message and return to Check card status
//
//	- Unformatted
//		If unformatted, go to format Y/N menu, and if yes then return to 
//		save menu following formatting

//Select save slot:
//	- Display of save slots performed by app

//If save exists, 
//	Calc existing save size
//	Calc reqd save size
//	Check for sufficient space
//	prompt for overwrite:

//? Display Explicit Approval Message (Are You Sure? YES/NO) ?

//If dir doesn't exist, create dir:

//Do save:
//	- Display TRC "Saving" message
//	- Delete existing icon.sys file?
//	- Save icon.sys and 3 icon files - save icon.sys last
//	- Save user data file
//	- Save any other data files

//If save not OK => Display error message
//Possible errors:
//	- PS2 card removed
//		Display message and return to Check card status
