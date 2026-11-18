//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// shellMemCard - TRC-compliant, asynchronous Memory Card manager
// 
// $Workfile: shellMemCard.c $
// $Author: Mdean $
// $Date: 9/09/03 10:41 $
// $Revision: 6 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#ifdef __MWERKS__
	#pragma mark INCLUDES
#endif
	
#include <shellCore.h>
#include <shellMemCard.h>

//-----------------------------------------------------------------------------
// M A C R O S
//-----------------------------------------------------------------------------

#ifdef __MWERKS__
	#pragma mark -
	#pragma mark MACROS
#endif

#define	SHELL_MEM_CARD_PRINT_DEBUG_TEXT
#ifdef SHELL_MEM_CARD_PRINT_DEBUG_TEXT
	#define SHELL_MEM_CARD_PRINTF		PRINT	//For debug text
#else
	#define SHELL_MEM_CARD_PRINTF		;		//For no debug text	
#endif

#define INVALID							(-1)
#define LENGTH_DIRECTORY_EXTENSION		(9)
#define LENGTH_KEY_CODE					(2)
#define	LENGTH_PRODUCT_MODEL_NUMBER		(10)
#define MAX_FULL_FILENAME_LENGTH		(64)

#define NUM_FILE_ENTRIES_TO_GET			(SHELLMEMCARD_MAX_FILES_PER_DIRECTORY + 2)	//2 file entries are used by "." and "..":
#define NUM_DIR_ENTRIES_TO_GET			(16)
#define SCE_MC_GET_DIR_FIRST_CALL		(0)
#define SCE_MC_GET_DIR_NEXT_CALL		(1)


//-----------------------------------------------------------------------------
// C O N S T A N T S
//-----------------------------------------------------------------------------

#ifdef __MWERKS__
	#pragma mark -
	#pragma mark CONSTANTS
#endif


//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

#ifdef __MWERKS__
	#pragma mark -
	#pragma mark TYPEDEFS
#endif

typedef int	ShellMemCardBool;

//Structure for storing parms passed to async fns:
typedef struct _ShellMemCardFnParms
{
	int 				portNo;
	int 				slotNo;
	sceMcTblGetDir* 	pDirInfo;
	sceMcTblGetDir* 	pFileInfo;
	ShellMemCardStatus*	pCardStatus;
	
	ShellMemCardBool*	pDirFound;
	int*				pDirSize;
	int					numFreeClusters;	//For shellMemCardCreateDir
	int*				pNumFiles;
	char* 				pData;
	
	int*				pFindFileDataSize;			//Returned by shellMemCardFindFile
	//char*				pFindFileDirNameExtension;
	int 				numDirsToSkip;
	int					numDirsToGet;
	int* 				pNumDirsObtained;
	
	u_int				dataSize;
	char 				dirNameExtension[SHELLMEMCARD_MAX_LEN_FILENAME];
	char				fileName[		 SHELLMEMCARD_MAX_LEN_FILENAME];
	char				dirNamePrefix[	 SHELLMEMCARD_MAX_LEN_FILENAME];

} ShellMemCardFnParms;

const ShellMemCardFnParms NULL_FUNCTION_PARMS = 
{
	INVALID,	//portNo
	INVALID,	//slotNo
	NULL,		//pDirInfo
	NULL,		//pFileInfo
	NULL,		//pCardStatus
	
	FALSE,		//dirFound
	NULL,		//pDirSize
	0,			//numFreeClusters
	NULL,		//pNumFiles
	NULL,		//pData
	
	NULL,		//pFindFileDataSize
	//NULL,		//pFindFileDirNameExtension
	0, 			//numDirsToSkip;
	0,			//numDirsToGet;
	NULL,		//pNumDirsObtained
	
	0,			//dataSize
	"",			//dirNameExtension
	"",			//fileName
	""			//dirNamePrefix
};


const ShellMemCardStatus NULL_CARD_STATUS = 
{
	-1,			//type;			
	-1,			//numFreeClusters;
	-1			//formatStatus;	
};


//-----------------------------------------------------------------------------
// S T A T I C   V A R I A B L E S 
//-----------------------------------------------------------------------------

#ifdef __MWERKS__
	#pragma mark -
	#pragma mark STATIC VARIABLES
#endif

static ShellMemCardFunctionResult	_functionResultArray[SHELLMEMCARD_STATE_NUM_STATES];

static int							_memCardModuleInitialised 	= FALSE;
static EShellMemCardState			_state						= SHELLMEMCARD_STATE_IDLE;
static EShellMemCardState			_prevState					= SHELLMEMCARD_STATE_IDLE;
static EShellMemCardState			_requestedState				= SHELLMEMCARD_STATE_NULL;
static int							_subState 					= 0;

//Each state may contain a number of substates if they req a number of async libmc calls:
//Alignment is not guaranteed on stack vars => define these as statics:
static ShellMemCardStatus			_cardStatus	= 	{
														SHELLMEMCARD_INDETERMINATE,	//type;			
														SHELLMEMCARD_INDETERMINATE,	//numFreeClusters;
														SHELLMEMCARD_INDETERMINATE	//formatStatus;	
													};

static int	 						_errorCodeLibMC 			= sceMcResSucceed;
static char							_keyCode[LENGTH_KEY_CODE];
static char							_productModelNo[LENGTH_PRODUCT_MODEL_NUMBER];
static ShellMemCardFnParms			_functionParms;
static sceMcTblGetDir				_rootDirectoryTable[NUM_DIR_ENTRIES_TO_GET];
static sceMcTblGetDir				_fileListTable[NUM_FILE_ENTRIES_TO_GET];	//SHELLMEMCARD_MAX_FILES_PER_DIRECTORY];
static int							_fileDescriptor 			= INVALID;


//-----------------------------------------------------------------------------
// F U N C T I O N   P R O T O T Y P E S
//-----------------------------------------------------------------------------

#ifdef __MWERKS__
	#pragma mark -
	#pragma mark FUNCTION PROTOTYPES
#endif
	
static EShellResult 	_UpdateGetNumFilesInDir(int functionNo, int functionResult, int mcSyncReturnVal);
static void			_ResetFunctionResultArray();
static void			_ResetModuleState();
static EShellResult	_SetModuleState(int portNo, int slotNo, EShellMemCardState state);
static void			_ExitModuleState(EShellResult success, int libMCErrorCode);

static void 		_UpdateFormat(		int functionNo, int functionResult, int mcSyncReturnVal);
static void 		_UpdateCreateDir(	int functionNo, int functionResult, int mcSyncReturnVal);
static void			_UpdateFindDir(		int functionNo, int functionResult, int mcSyncReturnVal);
static void		 	_UpdateFindFile(	int functionNo, int functionResult, int mcSyncReturnVal);
static void			_UpdateSaveFile(	int functionNo, int functionResult, int mcSyncReturnVal);
static void 		_UpdateLoadFile(	int functionNo, int functionResult, int mcSyncReturnVal);				


//-----------------------------------------------------------------------------
// S T A T I C   F U N C T I O N S
//-----------------------------------------------------------------------------

#ifdef __MWERKS__
	#pragma mark -
	#pragma mark STATIC FUNCTIONS:
#endif
	
static void	_ResetFunctionResultArray()
{
	int i;

	for(i = 0; i < SHELLMEMCARD_STATE_NUM_STATES; i++)
	{
		_functionResultArray[i].success			= SHELL_INDETERMINATE;
		_functionResultArray[i].subState		= -1;
		_functionResultArray[i].libMCErrorCode	= sceMcResSucceed;	
	}
}



static void	_ResetModuleState()
{
	_prevState		= _state;
	_state 			= SHELLMEMCARD_STATE_IDLE;
	_requestedState = SHELLMEMCARD_STATE_NULL;
	_subState		= 0;
	_functionParms	= NULL_FUNCTION_PARMS;
}


static EShellResult _SetModuleState(int portNo, int slotNo, EShellMemCardState state)
{
	_ResetFunctionResultArray();

	//If busy, return SHELL_FAIL:
	if(	(_state 			!= SHELLMEMCARD_STATE_IDLE) ||
		(_cardStatus.type 	!= sceMcTypePS2))
	{
		_ResetModuleState();
		return SHELL_FAIL;
	}
	
	_state					= SHELLMEMCARD_STATE_ENTERING_STATE;
	_requestedState			= state;
	_subState				= 0;
	
	_functionParms.portNo	= portNo;
	_functionParms.slotNo	= slotNo;			
	
	return SHELL_SUCCESS;
}


static void	_ExitModuleState(EShellResult success, int libMCErrorCode)
{
	ASSERT((_state >= 0) && (_state < SHELLMEMCARD_STATE_NUM_STATES),
			"Invalid EShellMemCardState");

	_functionResultArray[_state].success		= success;
	_functionResultArray[_state].subState		= _subState;
	_functionResultArray[_state].libMCErrorCode	= libMCErrorCode;
		
	_ResetModuleState();
}



static void _UpdateFormat(int functionNo, int functionResult,
				  		  int mcSyncReturnVal)
{
	int returnVal;
	(void) functionNo;

	switch(_subState)
	{
		case 0:
		{
			returnVal = sceMcFormat(_functionParms.portNo, 
									_functionParms.slotNo);

			if(returnVal < sceMcResSucceed)
			{
				_errorCodeLibMC = returnVal;
				_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
				return;				
			}
			
			_subState++;
		}
		break;
			
		case 1:		
		{																								
			if(mcSyncReturnVal == sceMcExecFinish)
			{
				ASSERT(functionNo == sceMcFuncNoFormat, "Incorrect fn registered");
				if(functionResult < sceMcResSucceed)
				{
					_errorCodeLibMC = functionResult;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;
				}	

				_ExitModuleState(SHELL_SUCCESS, sceMcResSucceed);
			}
		}
		break;
		
		default:
			ASSERT(0, "Invalid switch case");
	}	
}


//Handles up to SHELL_MEM_CARD_MAX_NUM_FILES_PER_DIRECTORY + 2 file entries:
EShellResult _UpdateGetNumFilesInDir(int functionNo, int functionResult,
									int mcSyncReturnVal)
{
	char			dirNameFull[MAX_FULL_FILENAME_LENGTH];
	int				numFileEntriesObtained;

	(void) functionNo;

	switch(_subState)
	{
		case 0:
		{
			ASSERT(	GET_ALIGNMENT(_fileListTable) >= 64,
					"_fileListTable not 64 byte aligned");

			ASSERT(_functionParms.portNo != INVALID, "Invalid port no");
			ASSERT(_functionParms.slotNo != INVALID, "Invalid slot no");
			ASSERT(_functionParms.pNumFiles, "NULL pointer");
			*_functionParms.pNumFiles = 0;

			sprintf(dirNameFull, "/%s%s%s/*", _keyCode, _productModelNo, _functionParms.dirNameExtension);		
			
			_errorCodeLibMC = sceMcGetDir(	_functionParms.portNo, _functionParms.slotNo, dirNameFull,
											SCE_MC_GET_DIR_FIRST_CALL, NUM_FILE_ENTRIES_TO_GET, 
											_fileListTable);
			_subState++;
		}
		break;
			
		case 1:			
		{																										
			if(mcSyncReturnVal == sceMcExecFinish)
			{
				ASSERT(functionNo == sceMcFuncNoGetDir, "Incorrect fn registered");
				
				if(functionResult < sceMcResSucceed)
				{
					_errorCodeLibMC = functionResult;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return SHELL_FAIL;
				}	
				numFileEntriesObtained = functionResult;
				*_functionParms.pNumFiles += functionResult;

				//2 file entries are used by "." and "..":
				ASSERT(*_functionParms.pNumFiles >= 2, "Corrupt file entries");
				*_functionParms.pNumFiles -= 2;
				
				_ExitModuleState(SHELL_SUCCESS, sceMcResSucceed);				
			}
		}
		break;			
	}


	return SHELL_SUCCESS;
}



static void _UpdateCreateDir(int functionNo, int functionResult,
					 		 int mcSyncReturnVal)
{
	char	dirName[MAX_FULL_FILENAME_LENGTH];

	(void) functionNo;

	switch(_subState)
	{
		case 0:
		{		
			if(_cardStatus.numFreeClusters < SHELLMEMCARD_CLUSTERS_FOR_DIRECTORY)
			{
				_ExitModuleState(SHELL_FAIL, sceMcResFullDevice);
				return;
			}

			//Attempt to create dir:
			sprintf(dirName, "/%s%s%s", _keyCode, _productModelNo, 
					_functionParms.dirNameExtension);
			sceMcMkdir(_functionParms.portNo, _functionParms.slotNo, dirName);

			_subState++;
		}
		break;

		case 1:
		{																								
			if(mcSyncReturnVal == sceMcExecFinish)
			{
				ASSERT(functionNo == sceMcFuncNoMkdir, "Incorrect fn registered");
				if(functionResult < sceMcResSucceed)
				{
					_errorCodeLibMC = functionResult;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;
				}	

				_ExitModuleState(SHELL_SUCCESS, sceMcResSucceed);
			}
		}
		break;	

		default:
			ASSERT(0, "Invalid switch case");				
	}	
}


static void _UpdateFindDir(int functionNo, int functionResult,
				   		   int mcSyncReturnVal)
{
	char dirNameFull[MAX_FULL_FILENAME_LENGTH];
	int 			returnVal;

	//To eliminate "unused arguments" warnings:
	(void) functionNo;

	switch(_subState)
	{
		case 0:
		{
			sprintf(dirNameFull, "/%s%s%s", _keyCode, _productModelNo, 
					_functionParms.dirNameExtension);
		
			returnVal = sceMcGetDir(_functionParms.portNo, _functionParms.slotNo,
									dirNameFull,
									SCE_MC_GET_DIR_FIRST_CALL, 									
									1,	//NUM_DIR_ENTRIES_TO_GET,									
									_rootDirectoryTable);

			if(returnVal < sceMcResSucceed)
			{
				_errorCodeLibMC = returnVal;
				_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
				return;		
			}
			_subState++;
		}
		break;

		case 1:		
		{				
			if(mcSyncReturnVal == sceMcExecFinish)
			{
				ASSERT(functionNo == sceMcFuncNoGetDir, "Incorrect fn registered");
				//if(functionResult < sceMcResSucceed)
				//{
				//	_errorCodeLibMC = functionResult;
				//	_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
				//	return;
				//}	
							
				ASSERT(_functionParms.pDirFound, "NULL pointer");
				if(functionResult > sceMcResSucceed)
				{
					*_functionParms.pDirFound = TRUE;
				}
				else
				{
					*_functionParms.pDirFound = FALSE;				
				}
	
				_ExitModuleState(SHELL_SUCCESS, sceMcResSucceed);											
			}
		}
		break;

		default:
			ASSERT(0, "Invalid switch case");				
	}			
}



static void _UpdateGetDirSize(int functionNo, int functionResult,
				   		   	  int mcSyncReturnVal)
{
	char	directoryName[MAX_FULL_FILENAME_LENGTH];
	int		numFileEntries;
	int		numFilesInDirectory;
	int 	returnVal;
	int 	fileNo		= 0;
	int		dirSize		= 0;

	//To eliminate "unused arguments" warnings:
	(void) functionNo;

	switch(_subState)
	{
		case 0:
		{
			ASSERT(	GET_ALIGNMENT(_fileListTable) >= 64,
					"_fileListTable not 64 byte aligned");

			sprintf(directoryName, "/%s%s%s/*", _keyCode, _productModelNo, 
					_functionParms.dirNameExtension);	

			returnVal = sceMcGetDir(_functionParms.portNo, 
									_functionParms.slotNo,
									directoryName,
									SCE_MC_GET_DIR_FIRST_CALL, 
									NUM_FILE_ENTRIES_TO_GET,
									_fileListTable);

			if(returnVal < sceMcResSucceed)
			{
				_errorCodeLibMC = returnVal;
				_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
				return;		
			}
			_subState++;
		}
		break;

		case 1:		
		{			
			if(mcSyncReturnVal == sceMcExecFinish)
			{
				ASSERT(functionNo == sceMcFuncNoGetDir, "Incorrect fn registered");

				//if(functionResult < sceMcResSucceed)
				//{
				//	*_functionParms.pDirSize	= -1;					
				//	_errorCodeLibMC 			= functionResult;
				//	_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
				//	return;
				//}	

				if(functionResult >= sceMcResSucceed)
				{
					//2 file entries are used by "." and "..":
					numFileEntries		= functionResult;
					numFilesInDirectory = numFileEntries - 2;
					
					if(numFilesInDirectory > SHELLMEMCARD_MAX_FILES_PER_DIRECTORY)
					{
						WARNING(0, "Memory Card shellMemCardFindFile:\n");			
						WARNING(0, "Dir %s contains %d files; limit is %d files\n",
								directoryName, numFilesInDirectory, 
								SHELLMEMCARD_MAX_FILES_PER_DIRECTORY);
						_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
						return;
					}

					//Sum all fileszies in _fileListTable:
					dirSize = 0;
					
					for(fileNo = 0; fileNo < numFileEntries; fileNo++)
					{
						dirSize += (int) SHELLMEMCARD_GET_CLUSTER_SIZE(_fileListTable[fileNo].FileSizeByte);
					}

					dirSize += SHELLMEMCARD_GET_CLUSTERS_FOR_FILE_ENTRIES(numFilesInDirectory);
					dirSize += SHELLMEMCARD_CLUSTERS_FOR_DIRECTORY;
					
					*_functionParms.pDirSize	= dirSize;
				}
				else
				{
					*_functionParms.pDirSize	= -1;					
					_errorCodeLibMC 			= functionResult;				
				}
				
				_ExitModuleState(SHELL_SUCCESS, sceMcResSucceed);				
		
			} //End of if(mcSyncReturnVal == sceMcExecFinish)	
		}
		break;
		
		default:
			ASSERT(0, "Invalid switch case");
	}
}


static void _UpdateGetDirList(int functionNo, int functionResult,
				   		      int mcSyncReturnVal)
{
	//Local enum defn for substates:
	typedef enum _EGetDirListState
	{
		SKIP_FIRST_DIRS 	= 0,
		SKIP_REMAINING_DIRS,
		GET_DIR_LIST,
		WAIT_FOR_GET_DIR_LIST		
	} EGetDirListState;

	char		searchDirName[MAX_FULL_FILENAME_LENGTH];

	static int		numDirsToSkip	= 0;	//numDirsToSkip in one step
	static int		numDirsSkipped	= 0;
	static u_int 	finalcallMode	= SCE_MC_GET_DIR_FIRST_CALL;
	int				numDirsObtained;
	int 			returnVal;

	//To eliminate "unused arguments" warnings:
	(void) functionNo;

	sprintf(searchDirName, "/%s%s%s*", _keyCode, _productModelNo, 
			_functionParms.dirNamePrefix);

	switch(_subState)
	{
		case SKIP_FIRST_DIRS:
		{		
			numDirsToSkip 	= _functionParms.numDirsToSkip;
			numDirsSkipped	= 0;
			
			if(numDirsToSkip == 0)
			{
				_subState 		= GET_DIR_LIST;
				finalcallMode	= SCE_MC_GET_DIR_FIRST_CALL;
			}	
			else
			{
				finalcallMode	= SCE_MC_GET_DIR_NEXT_CALL;

				if(numDirsToSkip > NUM_DIR_ENTRIES_TO_GET)
				{
					numDirsToSkip = NUM_DIR_ENTRIES_TO_GET;
				}	

				
				returnVal = sceMcGetDir(_functionParms.portNo,
										_functionParms.slotNo,
										searchDirName,
										SCE_MC_GET_DIR_FIRST_CALL, 									
										numDirsToSkip,									
										_rootDirectoryTable);			

				if(returnVal < sceMcResSucceed)
				{
					_errorCodeLibMC = returnVal;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;		
				}			
			
				_subState = SKIP_REMAINING_DIRS;
			}			
		}
		break;

		case SKIP_REMAINING_DIRS:
		{		
			if(mcSyncReturnVal == sceMcExecFinish)
			{
				ASSERT(functionNo == sceMcFuncNoGetDir, "Incorrect fn registered");
				if(functionResult < sceMcResSucceed)
				{
					_errorCodeLibMC = functionResult;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;
				}	

				numDirsObtained	= functionResult;
				
				ASSERT(numDirsObtained <= numDirsToSkip, "Invalid numDirsObtained");
				if(numDirsObtained < numDirsToSkip)				
				{
					//Reached end of list; exit completely
					*_functionParms.pNumDirsObtained = 0;
					_ExitModuleState(SHELL_SUCCESS, sceMcResSucceed);
				}
				else
				{
					numDirsSkipped += numDirsObtained;
					
					//Calculate remaining number of dirs to skip:
					numDirsToSkip = _functionParms.numDirsToSkip - numDirsSkipped;
					ASSERT(numDirsToSkip >= 0, "Invalid numDirsToSkip");
					if(numDirsToSkip == 0)
					{
						//Requested number of dirs skipped; go to next substate:
						_subState = GET_DIR_LIST;
					}		
					else 
					{
						if(numDirsToSkip > NUM_DIR_ENTRIES_TO_GET)
						{
							numDirsToSkip = NUM_DIR_ENTRIES_TO_GET;
						}

						returnVal = sceMcGetDir(_functionParms.portNo,
												_functionParms.slotNo,
												searchDirName,
												SCE_MC_GET_DIR_NEXT_CALL, 									
												numDirsToSkip,									
												_rootDirectoryTable);

						//Remain in this substate
					}	
				}											
			} //End of if(mcSyncReturnVal == sceMcExecFinish)
		}
		break;

		case GET_DIR_LIST:
		{
			returnVal = sceMcGetDir(_functionParms.portNo, 
									_functionParms.slotNo,
									searchDirName,
									finalcallMode, 									
									_functionParms.numDirsToGet,									
									_functionParms.pDirInfo);

			if(returnVal < sceMcResSucceed)
			{
				_errorCodeLibMC = returnVal;
				_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
				return;		
			}
			_subState = WAIT_FOR_GET_DIR_LIST;
		}
		break;
		
		case WAIT_FOR_GET_DIR_LIST:		
		{				
			if(mcSyncReturnVal == sceMcExecFinish)
			{
				ASSERT(functionNo == sceMcFuncNoGetDir, "Incorrect fn registered");
				if(functionResult < sceMcResSucceed)
				{
					_errorCodeLibMC = functionResult;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;
				}	
							
				ASSERT(_functionParms.pNumDirsObtained, "NULL pointer");
				*_functionParms.pNumDirsObtained = functionResult;

				//Successful completion; exit:								
				_ExitModuleState(SHELL_SUCCESS, sceMcResSucceed);											
			}
		}
		break;

		default:
			ASSERT(0, "Invalid switch case");				
	}			
}


static void _UpdateFindFile(int functionNo, int functionResult,
				   		    int mcSyncReturnVal)
{
	char	directoryName[MAX_FULL_FILENAME_LENGTH];
	int		numFileEntries;
	int		numFilesInDirectory;
	int		fileFound 	= FALSE;
	int 	fileNo		= 0;
	int 	returnVal;

	//To eliminate "unused arguments" warnings:
	(void) functionNo;

	switch(_subState)
	{
		case 0:
		{
			ASSERT(	GET_ALIGNMENT(_fileListTable) >= 64,
					"_fileListTable not 64 byte aligned");

			if(_functionParms.pFindFileDataSize)
			{
				*_functionParms.pFindFileDataSize = 0;
			}

			sprintf(directoryName, "/%s%s%s/*", _keyCode, _productModelNo, 
					_functionParms.dirNameExtension);	

			returnVal = sceMcGetDir(_functionParms.portNo, 
									_functionParms.slotNo,
									directoryName,
									SCE_MC_GET_DIR_FIRST_CALL, 
									NUM_FILE_ENTRIES_TO_GET,
									_fileListTable);

			if(returnVal < sceMcResSucceed)
			{
				_errorCodeLibMC = returnVal;
				_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
				return;		
			}
			_subState++;
		}
		break;

		case 1:		
		{			
			if(mcSyncReturnVal == sceMcExecFinish)
			{
				ASSERT(functionNo == sceMcFuncNoGetDir, "Incorrect fn registered");
				if(functionResult < sceMcResSucceed)
				{
					_errorCodeLibMC = functionResult;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;
				}	

				numFileEntries = functionResult;
				ASSERT(numFileEntries <= NUM_FILE_ENTRIES_TO_GET, 
					   "Invalid number of file entries");
				
				//2 file entries are used by "." and "..":
				numFilesInDirectory = numFileEntries - 2;
				if(numFilesInDirectory > SHELLMEMCARD_MAX_FILES_PER_DIRECTORY)
				{
					WARNING(0, "Memory Card shellMemCardFindFile:\n");			
					WARNING(0, "Dir %s contains %d files; limit is %d files\n",
							directoryName, numFilesInDirectory, 
							SHELLMEMCARD_MAX_FILES_PER_DIRECTORY);
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;
				}

				//Search filenames in _fileListTable:
				//Search all filenames in subdir:
				fileNo = 0;

				while((fileNo < numFileEntries) && (!fileFound))
				{
					fileFound = (strcmp(_functionParms.fileName, (char*) _fileListTable[fileNo].EntryName) == 0);
					if(fileFound)
					{
						if(_functionParms.pFindFileDataSize)
						{
							*_functionParms.pFindFileDataSize = (int) _fileListTable[fileNo].FileSizeByte;
						}

						
						if(_functionParms.pFileInfo)
						{
							//Fill in sceMcTblGetDir structure:
							*_functionParms.pFileInfo = _fileListTable[fileNo];
						}
					}
					fileNo++;
				}


				if(!fileFound)
				{
					if(_functionParms.pFindFileDataSize)
					{
						*_functionParms.pFindFileDataSize = -1;
					}
				}
				_ExitModuleState(SHELL_SUCCESS, sceMcResSucceed);				
		
			} //End of if(mcSyncReturnVal == sceMcExecFinish)	
		} 
		break;

		default:
			ASSERT(0, "Invalid switch case");
	}
}



static void _UpdateSaveFile(int functionNo, int functionResult,
				    	    int mcSyncReturnVal)
{
	//Local enum defn for substates:
	typedef enum _ESaveFileState
	{
		FIND_DIR = 0,
		SEARCH_DIR,
		GET_NUM_FILES_IN_DIR,
		WRITE_FILE,
		CLOSE_FILE,
		WAIT_FOR_FILE_CLOSE
	} ESaveFileState;

	char			dirNameFull[MAX_FULL_FILENAME_LENGTH];
	int 			returnVal 			= 0;
	int				numFilesInDirectory	= 0;
	ShellMemCardBool	dirFound 			= FALSE;
	u_int			numClustersReqd;

	(void) functionNo;

	switch(_subState)
	{
		case FIND_DIR:
		{
			if(strlen(_functionParms.dirNameExtension) > LENGTH_DIRECTORY_EXTENSION)
			{
				SHELL_MEM_CARD_PRINTF("Dir name extension is %d characters; must be no more than %d characters\n",
										strlen(_functionParms.dirNameExtension), LENGTH_DIRECTORY_EXTENSION);
				_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
				return;
			}

			if(strlen(_functionParms.fileName) > SHELLMEMCARD_MAX_LEN_FILENAME)
			{
				SHELL_MEM_CARD_PRINTF("File name extension is %d characters; must be no more than %d characters\n",
										strlen(_functionParms.fileName), SHELLMEMCARD_MAX_LEN_FILENAME);
				_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
				return;
			}

			//A file entry is 512 bytes (half a cluster), but since mem card works 
			//in single clusters always allow a single cluster for the file entry:
			//Calculate num clusters reqd; 1 cluster may be reqd for file entry:
			numClustersReqd = 1 + (_functionParms.dataSize + (SHELLMEMCARD_CLUSTER_SIZE-1)) / SHELLMEMCARD_CLUSTER_SIZE;
			
			//Check dir exists:
			//ToDo: Check for more than SHELLMEMCARD_MAX_DIRECTORIES:
			sprintf(dirNameFull, "/%s%s%s", _keyCode, _productModelNo, 
					_functionParms.dirNameExtension);

			returnVal = sceMcGetDir(_functionParms.portNo, _functionParms.slotNo,
									dirNameFull,
									SCE_MC_GET_DIR_FIRST_CALL, 									
									1,						
									_rootDirectoryTable);
			if(returnVal < sceMcResSucceed)
			{
				_errorCodeLibMC = returnVal;
				_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
				return;		
			}

			_subState = SEARCH_DIR;
		}
		break;		

		case SEARCH_DIR:
		{
			if(mcSyncReturnVal == sceMcExecFinish)
			{
				ASSERT(functionNo == sceMcFuncNoGetDir, "Incorrect fn registered");
				if(functionResult < sceMcResSucceed)
				{
					_errorCodeLibMC = functionResult;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;
				}	

				dirFound = (functionResult > sceMcResSucceed);

				if(dirFound)
				{
					//Check num files in dir (blocking fn):
					sprintf(dirNameFull, "/%s%s%s/*", _keyCode, 
							_productModelNo, _functionParms.dirNameExtension);		


					returnVal = sceMcGetDir(_functionParms.portNo, _functionParms.slotNo,
											dirNameFull, SCE_MC_GET_DIR_FIRST_CALL,
											NUM_FILE_ENTRIES_TO_GET, _fileListTable);
				
					if(returnVal < sceMcResSucceed)
					{
						_errorCodeLibMC = returnVal;
						_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
						return;		
					}
					_subState = GET_NUM_FILES_IN_DIR;

				}
				else
				{
					SHELL_MEM_CARD_PRINTF(	"Error: Dir '%s%s%s' not found\n", 
											_keyCode, _productModelNo,
										 	_functionParms.dirNameExtension);
					_errorCodeLibMC = sceMcResNoEntry;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;
				}

			}	//End of if(mcSyncReturnVal == sceMcExecFinish)
		}
		break;		


		case GET_NUM_FILES_IN_DIR:
		{
		
			if(mcSyncReturnVal == sceMcExecFinish)
			{
				ASSERT(functionNo == sceMcFuncNoGetDir, "Incorrect fn registered");
				if(functionResult < sceMcResSucceed)
				{
					_errorCodeLibMC = functionResult;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;
				}	
		

				//2 file entries are used by "." and "..":
				numFilesInDirectory = functionResult - 2;
				if(numFilesInDirectory > SHELLMEMCARD_MAX_FILES_PER_DIRECTORY)
				{
					//Check num files in dir (blocking fn):
					sprintf(dirNameFull, "/%s%s%s/*", _keyCode, 
							_productModelNo, _functionParms.dirNameExtension);		
				
					WARNING(0, "Memory Card shellMemCardFindFile:\n");			
					WARNING(0, "Dir %s contains %d files; limit is %d files\n",
							dirNameFull, numFilesInDirectory, 
							SHELLMEMCARD_MAX_FILES_PER_DIRECTORY);
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;
				}

				//-------------------------------------------------------------------------
				//sceMcOpen requires that the file name be less than 31 chars => change
				//to dir first, then save file:
				sprintf(dirNameFull, "/%s%s%s", _keyCode, _productModelNo,
						_functionParms.dirNameExtension);		

				returnVal = sceMcChdir(_functionParms.portNo, _functionParms.slotNo, 
									   dirNameFull, 0);
				if(returnVal < sceMcResSucceed)
				{
					_errorCodeLibMC = returnVal;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;		
				}
					
				sceMcSync(0, 0, &_errorCodeLibMC);	//Blocking call but shouldn't take too long
				if(_errorCodeLibMC < sceMcResSucceed)
				{
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;		
				}


				returnVal = sceMcOpen(_functionParms.portNo, _functionParms.slotNo,
									  _functionParms.fileName, SCE_CREAT|SCE_WRONLY);
				if(returnVal < sceMcResSucceed)
				{
					_errorCodeLibMC = returnVal;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;	
				}

				_subState = WRITE_FILE;

			} //End of if(mcSyncReturnVal == sceMcExecFinish)	
		} 
		break;
		
		
		case WRITE_FILE:
		{
			if(mcSyncReturnVal == sceMcExecFinish)
			{
				ASSERT(functionNo == sceMcFuncNoOpen, "Incorrect fn registered");
				
				if(functionResult < sceMcResSucceed)
				{
					_errorCodeLibMC = functionResult;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;
				}	

				_fileDescriptor = functionResult;
								  
				if(_fileDescriptor < 0)
				{
					WARNING(0, "Memory Card Error %d\n", _fileDescriptor);
					_errorCodeLibMC = _fileDescriptor;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;
				}

				//-------------------------------------------------------------				   		
				//Write data to file:		
				returnVal = sceMcWrite(_fileDescriptor, _functionParms.pData,
						   (int)_functionParms.dataSize);

				if(returnVal < sceMcResSucceed)
				{
					_errorCodeLibMC = returnVal;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;			
				}

				_subState = CLOSE_FILE;
			} //End of if(mcSyncReturnVal == sceMcExecFinish)	
		}		
		break;
		
		
		case CLOSE_FILE:
		{
			//Only execute once sceMcWrite has completed:
			if(mcSyncReturnVal == sceMcExecFinish)
			{
				ASSERT(functionNo == sceMcFuncNoWrite, "Incorrect fn registered");
				
				if(functionResult < sceMcResSucceed)
				{
					_errorCodeLibMC = functionResult;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;
				}	

				//SHELL_MEM_CARD_PRINTF("%s write ret -- %d\n", _functionParms.fileName, _errorCodeLibMC);

				returnVal = sceMcClose(_fileDescriptor);
				ASSERT(returnVal == sceMcResSucceed, "Fn not registered");
				_subState = WAIT_FOR_FILE_CLOSE;
			}
		}		
		break;

		case WAIT_FOR_FILE_CLOSE:
		{
			//Only execute once sceMcClose has completed:
			if(mcSyncReturnVal == sceMcExecFinish)
			{
				ASSERT(functionNo == sceMcFuncNoClose, "Incorrect fn registered");
				
				if(functionResult < sceMcResSucceed)
				{
					_errorCodeLibMC = functionResult;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;
				}					
		
				//Save successful:
				_fileDescriptor = INVALID;

				_ExitModuleState(SHELL_SUCCESS, sceMcResSucceed);				
			}		
		}		
		break;


		default:
			ASSERT(0, "Invalid switch case");				

	} //End of switch(_subState)
}



static void _UpdateLoadFile(int functionNo, int functionResult,
				   		    int mcSyncReturnVal)						
{
	char	dirNameFull[MAX_FULL_FILENAME_LENGTH];
	int		returnVal;

	//To eliminate "unused argument" warnings:
	(void) functionNo;
	(void) functionResult;
	(void) mcSyncReturnVal;
	
	switch(_subState)
	{
		case 0:
		{
			sprintf(dirNameFull, "/%s%s%s", _keyCode, _productModelNo, 
					_functionParms.dirNameExtension);		
			returnVal = sceMcChdir(_functionParms.portNo, _functionParms.slotNo,
					   			   dirNameFull, 0);

			sceMcSync(0, 0, &_errorCodeLibMC);
			
			if(	(returnVal < sceMcResSucceed)			||
				(_errorCodeLibMC != sceMcResSucceed))
			{
				_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
				return;
			}		

			returnVal = sceMcOpen(_functionParms.portNo, _functionParms.slotNo, 
					  			  _functionParms.fileName, SCE_RDONLY);
			sceMcSync(0, 0, &_fileDescriptor);

			if(	(returnVal < sceMcResSucceed)			||
				(_fileDescriptor < sceMcResSucceed))
			{
				_errorCodeLibMC = _fileDescriptor;
				_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
				return;
			}

			//Read data from file:
			returnVal = sceMcRead(_fileDescriptor, _functionParms.pData,
					  			  (int)_functionParms.dataSize);

			if(returnVal < sceMcResSucceed)
			{
				_errorCodeLibMC = _fileDescriptor;
				_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
				return;	
			}

			_subState++;			
		}
		break;

		case 1:		
		{				
			sceMcSync(0, 0, &_errorCodeLibMC);

			if(_errorCodeLibMC < sceMcResSucceed)
			{
				_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
				return;
			}

			sceMcClose(_fileDescriptor);
			sceMcSync(0, 0, &_errorCodeLibMC);
		
			_fileDescriptor = INVALID;

			//-----------------------------------------------------------------			    
			//SHELL_MEM_CARD_PRINTF("File contents: %s\n", _functionParms.pData);
			_ExitModuleState(SHELL_SUCCESS, sceMcResSucceed);				
		}
		break;
		
		default:
			ASSERT(0, "Invalid switch case");				
	}
}



static void _UpdateDeleteFile(int functionNo, int functionResult,
				   		      int mcSyncReturnVal)						
{
	char	fileNameFull[MAX_FULL_FILENAME_LENGTH];
	int		returnVal;

	//To eliminate "unused argument" warnings:
	(void) functionNo;
	//(void) functionResult;
	//(void) mcSyncReturnVal;

	switch(_subState)
	{
		case 0:
		{
			sprintf(fileNameFull, "/%s%s%s/%s", _keyCode, _productModelNo, 
					_functionParms.dirNameExtension, _functionParms.fileName);

			returnVal = sceMcDelete(_functionParms.portNo, 
									_functionParms.slotNo, 
					  			    fileNameFull);
					
			if(returnVal < sceMcResSucceed)
			{
				_errorCodeLibMC = returnVal;
				_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
				return;
			}
			
			_subState++;			
		}
		break;

		case 1:		
		{		
			//Only execute once sceMcDelete has completed:
			if(mcSyncReturnVal == sceMcExecFinish)
			{
				ASSERT(functionNo == sceMcFuncNoDelete, "Incorrect fn registered");
				
				if(functionResult < sceMcResSucceed)
				{
					_errorCodeLibMC = functionResult;
					_ExitModuleState(SHELL_FAIL, _errorCodeLibMC);
					return;
				}	

				_ExitModuleState(SHELL_SUCCESS, sceMcResSucceed);
			}
		}
		break;
		
		default:
			ASSERT(0, "Invalid switch case");				
	}
}


//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

#ifdef __MWERKS__
	#pragma mark -
	#pragma mark GLOBAL FUNCTIONS:
#endif

//-----------------------------------------------------------------------------
// shellMemCardGetState()
// Description: This function returns the current state 
// Parameters:	-
// Returns: 	Current module state
// Notes: 		-
//-----------------------------------------------------------------------------
EShellMemCardState	shellMemCardGetState()
{
	return _state;
}


//-----------------------------------------------------------------------------
// shellMemCardGetFunctionResult()
// Description: Fills in the given ShellMemCardFunctionResult with the result of 
//				the last executed fn specified by the state parameter. 
// Parameters:	state			: Specifies fn for which to check completion
//				pFunctionResult	: Pointer to ShellMemCardFunctionResult structure
//								  to be filled with fn completion info
// Returns: 	Nothing
// Notes: 		Used for checking fn completion
//-----------------------------------------------------------------------------
void shellMemCardGetFunctionResult(	EShellMemCardState state,
									ShellMemCardFunctionResult* pFunctionResult)
{
	ASSERT((state >= 0) && (state < SHELLMEMCARD_STATE_NUM_STATES),
			"Invalid EShellMemCardState");
	ASSERT(pFunctionResult, "Null pointer");

	*pFunctionResult = _functionResultArray[state];
}


//-----------------------------------------------------------------------------
// shellMemCardInit()
// Description: This function initialises the memory card module
// Parameters:	keyCode			: 2-character key code as described in 
//								  Memory Card Library section of EE Library 
//								  Overview
//				productModelNo	: 10-character key code as described
//								  in Memory Card Library section of EE Library 
//								  Overview
// Returns: 	SHELL_SUCCESS or SHELL_FAIL
// Notes: 		-
//-----------------------------------------------------------------------------
EShellResult shellMemCardInit(const char* keyCode, const char* productModelNo)
{
	const char*		moduleFilenames[] 	= {	"sio2man",											
											"mcman", 
											"mcserv", 
											""};
	EShellResult 	returnVal;
	int				i = 0;

	if(_memCardModuleInitialised)
	{
		return SHELL_SUCCESS;
	}	

	//-------------------------------------------------------------------------
	//Load reqd IOP modules; shellFile can handle attempts to load modules
	//more than once:
	while(strcmp(moduleFilenames[i], "") != 0)
	{
		ASSERT(moduleFilenames[i], "Error in module initialisation parameters");
		shellFileLoadStartModule(moduleFilenames[i], 0, "", NULL);
		i++;
	}

	//-------------------------------------------------------------------------

	if(strlen(keyCode) != LENGTH_KEY_CODE)
	{
		SHELL_MEM_CARD_PRINTF("Key code is %d characters; must be %d characters\n",
								strlen(keyCode), LENGTH_KEY_CODE);
		return SHELL_FAIL;
	}

	if(strlen(productModelNo) != LENGTH_PRODUCT_MODEL_NUMBER)
	{
		SHELL_MEM_CARD_PRINTF("Product code is %d characters; must be %d characters\n",
								strlen(productModelNo), LENGTH_PRODUCT_MODEL_NUMBER);
		return SHELL_FAIL;
	}

	strcpy(_keyCode, keyCode);
	strcpy(_productModelNo, productModelNo);
	
	//Lib call to initialize Memory Card environment:
	_errorCodeLibMC = sceMcInit();

	if(_errorCodeLibMC == sceMcResSucceed)
	{
		sceMcSync(0, 0, &_errorCodeLibMC);
		returnVal = SHELL_SUCCESS;
		_memCardModuleInitialised = TRUE;
	}
	else
	{
		returnVal = SHELL_FAIL;
#ifdef DEBUG
		switch(_errorCodeLibMC)
		{
			case sceMcIniErrKernel:
				SHELL_MEM_CARD_PRINTF("Memory card initialision failed\n");
				break;

			case sceMcIniOldMcserv:
				SHELL_MEM_CARD_PRINTF("Memory card initialision failed: old mcserv.irx\n");
				break;
				
			case sceMcIniOldMcman:
				SHELL_MEM_CARD_PRINTF("Memory card initialision failed: old mcman.irx\n");
				break;
			
			default:
				SHELL_MEM_CARD_PRINTF("Memory card initialision error code: %d\n", returnVal);
		}
#endif
	}
	
	//Initialise internal variables:
	_ResetFunctionResultArray();
	
	return returnVal;
}


//-----------------------------------------------------------------------------
// shellMemCardFormat()
// Description: This function formats the given memory card:
// Parameters:	portNo: Port number for memory card
//				slotNo:	Slot number for memory card
// Returns: 	SHELL_SUCCESS or SHELL_FAIL
// Notes: 		-
//-----------------------------------------------------------------------------
EShellResult shellMemCardFormat(int portNo, int slotNo)
{
	return _SetModuleState(portNo, slotNo, SHELLMEMCARD_STATE_FORMAT);
}



//-----------------------------------------------------------------------------
// shellMemCardGetNumFilesInDir()
// Description: This function finds the number of files in the specified dir on
//				the memory card and returns this value in pNumFiles
//				Returns true if dir is found.
// Parameters:	portNo		: Port number for memory card
//				slotNo		: Slot number for memory card
//				pNumFiles	: Pointer to int which will be filled in with
//							  num files in dir
//				dirNameExtension: Max 8-char extension for dir name
// Returns: 	SHELL_SUCCESS or SHELL_FAIL
// Notes: 		
//-----------------------------------------------------------------------------
EShellResult shellMemCardGetNumFilesInDir(int portNo, int slotNo, int* pNumFiles, 
								const char* dirNameExtension)
{
	EShellResult result;
	
	result = _SetModuleState(portNo, slotNo, SHELLMEMCARD_STATE_GET_NUM_FILES_IN_DIR);

	_functionParms.pNumFiles = pNumFiles;
	strcpy(_functionParms.dirNameExtension, dirNameExtension);

	return result;
}



//-----------------------------------------------------------------------------
// shellMemCardCreateDir()
// Description	: 	This function creates a directory in the root with the specified  
//				  	dirNameExtension
// Parameters	:	portNo			: Port number for memory card
//					slotNo			: Slot number for memory card
//					dirNameExtension: Max 8-char extension for dir name
// Returns		: 	SHELL_SUCCESS or SHELL_FAIL
// Notes		: 		
//-----------------------------------------------------------------------------
EShellResult shellMemCardCreateDir(int portNo, int slotNo, const char* dirNameExtension)
{
	EShellResult returnVal;

	returnVal = _SetModuleState(portNo, slotNo, SHELLMEMCARD_STATE_CREATE_DIR);

	strcpy(_functionParms.dirNameExtension, dirNameExtension);

	return returnVal;
}



//-----------------------------------------------------------------------------
// shellMemCardFindDir()
// Description: This function searches the root dir on the memory card for the 
//				specified dir. Returns true if dir is found.
// Parameters:	portNo			: Port number for memory card
//				slotNo			: Slot number for memory card
//				dirNameExtension: Max 8-char extension for dir name
//				pDirFound		: Will be filled in with result of search
//				pDirInfo		: If non-NULL, will be filled in with dir info
// Returns: 	SHELL_SUCCESS or SHELL_FAIL
// Notes: 		-
//-----------------------------------------------------------------------------
EShellResult shellMemCardFindDir(	int portNo, int slotNo, const char* dirNameExtension, 
						int* pDirFound, sceMcTblGetDir* pDirInfo)
{
	EShellResult returnVal;

	returnVal = _SetModuleState(portNo, slotNo, SHELLMEMCARD_STATE_FIND_DIR);

	ASSERT(pDirFound, "NULL pointer");
	_functionParms.pDirFound	= pDirFound;
	_functionParms.pDirInfo 	= pDirInfo;
	strcpy(_functionParms.dirNameExtension, dirNameExtension);

	return returnVal;

}


//-----------------------------------------------------------------------------
// shellMemCardGetDirSize()
// Description: This function returns the size of the specified dir, including 
//				the dir contents.
//				specified dir. Returns true if dir is found.
// Parameters:	portNo			: Port number for memory card
//				slotNo			: Slot number for memory card
//				dirNameExtension: Max 8-char extension for dir name
//				pDirSize		: Will be filled in with dir size in Kb; -1 if
//								  dir not found
// Returns: 	SHELL_SUCCESS or SHELL_FAIL
// Notes: 		-
//-----------------------------------------------------------------------------
EShellResult shellMemCardGetDirSize(int portNo, int slotNo, 
									const char* dirNameExtension, int* pDirSize)
{
	EShellResult returnVal;

	returnVal = _SetModuleState(portNo, slotNo, SHELLMEMCARD_STATE_GET_DIR_SIZE);

	ASSERT(pDirSize, "NULL pointer");
	_functionParms.pDirSize	= pDirSize;
	strcpy(_functionParms.dirNameExtension, dirNameExtension);

	return returnVal;
}



//-----------------------------------------------------------------------------
// shellMemCardGetDirList()
// Description: This function searches the root dir on the memory card for 
//				directories matching the specified criteria
// Parameters:	portNo				: Port number for memory card
//				slotNo				: Slot number for memory card
//				dirPrefix			: Search only for dirs with specified prefix; 
//								 	  specify "" if no prefix required
//				numDirsToSkip		: Ignore the first n matching entries, 
//								  	  where n = numDirsToSkip
//				numDirsToGet		: Number of matching directories to get
//				pNumDirsObtained	: Will be filled in with number of matching 
//									  directories obtained
//				pDirInfo			: Will be filled in with specified dir info
// Returns: 	SHELL_SUCCESS or SHELL_FAIL
// Notes: 		-
//-----------------------------------------------------------------------------
EShellResult shellMemCardGetDirList(	int portNo, int slotNo, const char* dirPrefix, 
									int numDirsToSkip, int numDirsToGet, 
									int* pNumDirsObtained,
									sceMcTblGetDir* pDirInfo)
{
	EShellResult returnVal;

	returnVal = _SetModuleState(portNo, slotNo, SHELLMEMCARD_STATE_GET_DIR_LIST);

	ASSERT(pNumDirsObtained,	"NULL pointer");
	ASSERT(pDirInfo, 			"NULL pointer");
	_functionParms.numDirsToSkip	= numDirsToSkip;
	_functionParms.numDirsToGet 	= numDirsToGet;
	_functionParms.pNumDirsObtained	= pNumDirsObtained;
	_functionParms.pDirInfo 		= pDirInfo;
	strcpy(_functionParms.dirNamePrefix, dirPrefix);

	return returnVal;
}


//-----------------------------------------------------------------------------
// shellMemCardFindFile()
// Description: This function searches the given dir on the memory card for the 
//				specified file. If file is found, the file size and file info 
//				are returned in pFileSize and pFileInfo respectively.
// Parameters:	portNo			: Port number for memory card
//				slotNo			: Slot number for memory card
//				fileName		: File name extension as described in toDo
//				dirNameExtension: Max 8-char extension for dir name
//				pFileSize		: Pointer to int; if non-NULL this will be filled
//								  in with the file size (if file is found)
//				pFileInfo		: If non-NULL, will be filled in with file info
// Returns: 	SHELL_SUCCESS or SHELL_FAIL
// Notes: 		-
//-----------------------------------------------------------------------------
EShellResult shellMemCardFindFile(int portNo, int slotNo, const char* fileName, 
								const char* dirNameExtension, int* pFileSize,
								sceMcTblGetDir* pFileInfo)
{	
	EShellResult returnVal;

	returnVal = _SetModuleState(portNo, slotNo, SHELLMEMCARD_STATE_FIND_FILE);

	_functionParms.pFindFileDataSize			= pFileSize;
	_functionParms.pFileInfo 					= pFileInfo;
	strcpy(_functionParms.dirNameExtension, dirNameExtension);
	strcpy(_functionParms.fileName, 	fileName);

	return returnVal;
}


//-----------------------------------------------------------------------------
// shellMemCardSaveFile()
// Description: Saves the specified data block to the specified memory card
// Parameters:	portNo				: Port number for memory card
//				slotNo				: Slot number for memory card
//				blockingState		: SHELL_MEM_CARD_BLOCKING or SHELL_MEM_CARD_NON_BLOCKING
//				dirNameExtension	: Directory name extension as described in toDo
//				fileName			: File name extension as described in toDo
//				pData				: Pointer to data to be saved
//				dataSize			: Data size (in bytes) of data to be saved
// Returns: 	SHELL_SUCCESS or SHELL_FAIL
// Notes: 		-
//-----------------------------------------------------------------------------
EShellResult shellMemCardSaveFile(int portNo, int slotNo,
						const char* dirNameExtension, 
						const char* fileName, char* pData, u_int dataSize)
{
	EShellResult returnVal;

	returnVal = _SetModuleState(portNo, slotNo, SHELLMEMCARD_STATE_SAVE_FILE);

	ASSERT(pData, "NULL pointer");
	_functionParms.pData	= pData;
	_functionParms.dataSize = dataSize;
	strcpy(_functionParms.dirNameExtension, dirNameExtension);
	strcpy(_functionParms.fileName, 		fileName);

	return returnVal;
}


//-----------------------------------------------------------------------------
// shellMemCardLoadFile()
// Description: Loads a file to the specified memory address
// Parameters:	portNo				: Port number for memory card
//				slotNo				: Slot number for memory card
//				blockingState		: SHELL_MEM_CARD_BLOCKING or SHELL_MEM_CARD_NON_BLOCKING
//				dirNameExtension	: Directory name extension as described in toDo
//				fileName			: File name extension as described in toDo
//				pData				: Pointer to data to be loaded
//				dataSize			: Data size (in bytes) of data to be loaded
// Returns: 	SHELL_SUCCESS or SHELL_FAIL
// Notes: 		Use shellMemCardFindFile to get file's data size
//				sceMcOpen requires that the file name be less than 31 chars =>
//				change to dir first, then load file.
//-----------------------------------------------------------------------------
EShellResult shellMemCardLoadFile(int portNo, int slotNo,
						const char* dirNameExtension,
						const char* fileName, char* pData, u_int dataSize)
{
	EShellResult returnVal;

	returnVal = _SetModuleState(portNo, slotNo, 
								SHELLMEMCARD_STATE_LOAD_FILE);

	ASSERT(pData, "NULL pointer");
	_functionParms.pData	= pData;
	_functionParms.dataSize = dataSize;
	strcpy(_functionParms.dirNameExtension, dirNameExtension);
	strcpy(_functionParms.fileName, 		fileName);

	return returnVal;
}


//-----------------------------------------------------------------------------
// shellMemCardDeleteFile()
// Description	: 	Delete the specified file
// Parameters	:	portNo				: Port number for memory card
//					slotNo				: Slot number for memory card
//					dirNameExtension	: Directory from which to delete file 
//										  (max 8-char extension for dir name)
//					fileName			: File name extension as described in toDo
// Returns		: 	SHELL_SUCCESS or SHELL_FAIL
// Notes		: 	-
//-----------------------------------------------------------------------------
EShellResult shellMemCardDeleteFile(int portNo, int slotNo,
									const char* dirNameExtension,
									const char* fileName)
{
	EShellResult returnVal;

	returnVal = _SetModuleState(portNo, slotNo, 
								SHELLMEMCARD_STATE_DELETE_FILE);

	strcpy(_functionParms.dirNameExtension, dirNameExtension);
	strcpy(_functionParms.fileName, 		fileName);

	return returnVal;
}

//-----------------------------------------------------------------------------
// shellMemCardUpdate()
// Description	:	Tick function which should be called every frame when using
//					shellMemCard fns
// Parameters	:	portNo		: Port number for memory card
//					slotNo		: Slot number for memory card
//					pCardStatus	: Pointer to ShellMemCardStatus struct which 
//								  will be filled in with status of card in 
//								  specified slot
// Returns		:	Current module state 	
// Notes		: 	pCardStatus is updated when current state is 
//					SHELLMEMCARD_STATE_IDLE
//-----------------------------------------------------------------------------
EShellMemCardState shellMemCardUpdate(int portNo, int slotNo, 
									  ShellMemCardStatus* pCardStatus)
{
	//extern void DebugTextPrintList(char* fmt, ...);
	const int NULL_VALUE	= -1;
	int functionNo			= NULL_VALUE;
	int functionResult		= NULL_VALUE;
	int mcSyncReturnVal		= NULL_VALUE;
	
	mcSyncReturnVal = sceMcSync(1, &functionNo, &functionResult);

	//Upon fn completion, store fn result:
	if(mcSyncReturnVal == sceMcExecFinish)
	{
		_errorCodeLibMC = functionResult;
	}
	
	//DebugTextPrintList("Fn no        = %d\n", functionNo);
	//DebugTextPrintList("Fn result    = %d\n", functionResult);
	//DebugTextPrintList("Module state = %d\n", _state);

	switch(_state)
	{
		case SHELLMEMCARD_STATE_ENTERING_STATE:
		{
			//printf("Fn no = %d, Fn result = %d, mcSyncReturnVal = %d\n", 
			//		functionNo, functionResult, mcSyncReturnVal);
		
			//Only enter state when mem card system is idle (no fn registered):
			if(mcSyncReturnVal == sceMcExecIdle)			
			{
				//sceMcGetInfo has completed:
				if(_cardStatus.type != sceMcTypePS2)
				{					
					_ResetModuleState();
					return SHELLMEMCARD_STATE_NULL;
				}
				
				//Change to requested state:
				ASSERT(_requestedState != SHELLMEMCARD_STATE_NULL, "Invalid state");
				_state = _requestedState;
				_requestedState = SHELLMEMCARD_STATE_NULL;								
			}			
		}
		break;

		case SHELLMEMCARD_STATE_IDLE:
		{
			if(_requestedState == SHELLMEMCARD_STATE_NULL)
			{
				sceMcGetInfo(	portNo, slotNo,
				   				&_cardStatus.type, 
								&_cardStatus.numFreeClusters,
								&_cardStatus.formatStatus);		
			}						
		}
		break;

		case SHELLMEMCARD_STATE_FORMAT:
			_UpdateFormat(functionNo, functionResult, mcSyncReturnVal);
		break;
			
		case SHELLMEMCARD_STATE_GET_NUM_FILES_IN_DIR:
			_UpdateGetNumFilesInDir(functionNo, functionResult, mcSyncReturnVal);
		break;

		case SHELLMEMCARD_STATE_CREATE_DIR:
			_UpdateCreateDir(functionNo, functionResult, mcSyncReturnVal);
		break;

		case SHELLMEMCARD_STATE_FIND_DIR:
			_UpdateFindDir(functionNo, functionResult, mcSyncReturnVal);
		break;

		case SHELLMEMCARD_STATE_GET_DIR_LIST:
			_UpdateGetDirList(functionNo, functionResult, mcSyncReturnVal);
		break;

		case SHELLMEMCARD_STATE_GET_DIR_SIZE:
			_UpdateGetDirSize(functionNo, functionResult, mcSyncReturnVal);
		break;

		case SHELLMEMCARD_STATE_FIND_FILE:
			_UpdateFindFile(functionNo, functionResult, mcSyncReturnVal);
		break;

		case SHELLMEMCARD_STATE_DELETE_FILE:
			_UpdateDeleteFile(functionNo, functionResult, mcSyncReturnVal);
		break;

		case SHELLMEMCARD_STATE_LOAD_FILE:
			_UpdateLoadFile(functionNo, functionResult, mcSyncReturnVal);
		break;

		case SHELLMEMCARD_STATE_SAVE_FILE:
			_UpdateSaveFile(functionNo, functionResult, mcSyncReturnVal);
		break;

		default:
			ASSERT(0, "Invalid switch argument");
	}
	
	if(pCardStatus)
	{
		*pCardStatus = _cardStatus;
	}
	return _state;
}


//-----------------------------------------------------------------------------
// shellMemCardCreateUserDataFileName()
// Description	: 	Helper fn which builds the name of a user data file 
//					corresponding to the given directory name extension.
// Parameters	:	int functionReturnVal: Return value from a sceMc.. fn
// Returns		: 	Nothing
// Notes		:	TRC 12.1.1.2.2 requires that a user directory must contain
//					one file which has the same name as that of the directory
//-----------------------------------------------------------------------------
void shellMemCardCreateUserDataFileName(char* userDataFileName,
									   const char* dirNameExtension)
{
	ASSERT(userDataFileName, "NULL pointer");
	ASSERT(dirNameExtension, "NULL pointer");
		
	sprintf(userDataFileName, "%s%s%s", _keyCode, _productModelNo, dirNameExtension);	
}


//-----------------------------------------------------------------------------
// shellMemCardResetIconSys()
// Description	: 	Helper function which sets the following elements in the
//					specified sceMcIconSys structure to their TRC-reqd values:
//						sceMcIconSys.Head
//						sceMcIconSys.Reserv1
//						sceMcIconSys.Reserv2
//						sceMcIconSys.Reserve3
//
// Parameters	:	pIconSys: pointer to sceMcIconSys structure
// Returns		: 	Nothing
// Notes		: 	-
//-----------------------------------------------------------------------------
void shellMemCardResetIconSys(sceMcIconSys* pIconSys)
{
	const char	ICON_SYS_HEADER[]		= "PS2D";
	const int	ICON_SYS_RESERVE3_SIZE	= 512;

	ASSERT(pIconSys, "NULL pointer");
	
	strcpy((char*) pIconSys->Head, ICON_SYS_HEADER);
	
	pIconSys->Reserv1	= 0x0000;
	pIconSys->Reserv2	= 0x00000000;
	memset(pIconSys->Reserve3, 0, ICON_SYS_RESERVE3_SIZE);
}

