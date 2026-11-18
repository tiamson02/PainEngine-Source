//-----------------------------------------------------------------------------
//              shellMCUIScreenSave - Memory Card User Interface Driver
//
// Memory Card User Interface Driver for TRC compliant memory card user
// interface implementation
//
// $Workfile: shellMCUIScreenSave.h $
// $Author: Hbodasin $
// $Date: 5/02/03 16:02 $
// $Revision: 3 $
//
// Copyright (c) 2002 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
// Further notes, if any
//
//-----------------------------------------------------------------------------
#ifndef MCUI_SCREEN_SAVE
#define MCUI_SCREEN_SAVE
#include "shellMCUIScreen.h"

class CMCUIScreenSave : public CMCUIScreen
{
//Operations:
public:
	//Constructor:
	CMCUIScreenSave();

	//Destructor:
	~CMCUIScreenSave()	{}
	
	//Update screen:
	virtual EShellMCUIScreen Update(ShellMCUIOutputs* pOutputParms,
									ShellMCUIInputs*  pInputParms);		
//Accessor fns:

private:
	void	UpdatePreSaveChecks(	ShellMCUIOutputs* pOutputParms,
						 			ShellMCUIInputs*  pInputParms);
	void	UpdateFormat(			ShellMCUIOutputs* pOutputParms,
									ShellMCUIInputs*  pInputParms);					 			
	void 	UpdateSaveFiles(	  	ShellMCUIOutputs* pOutputParms,
						 			ShellMCUIInputs*  pInputParms);
	void 	UpdateSaveCompletion(	ShellMCUIOutputs* pOutputParms,
						 			ShellMCUIInputs*  pInputParms);

	int		CalculateSaveSize(ShellMCUIInputs* pInputParms);
	void 	CheckSaveParms(	  ShellMCUIInputs* pInputParms);
	//void 	CheckReturnValue(bool* pSuccess, int returnVal,
	//						 int nextStateOnSuccess, int nextStateOnFail);
		
	
//Attributes:	
private:
	bool	_saveSuccessful;	
	bool	_formatSuccessful;	
	int		_saveDirFound;
	int		_existingSaveSize;

	int		_substateFormat;
	int 	_numDataFilesSaved;
	int 	_numIconFilesSaved;

};


#endif
