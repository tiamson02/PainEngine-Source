//-----------------------------------------------------------------------------
//              shellMCUIScreenListSaves - Memory Card User Interface Driver
//
// Memory Card User Interface Driver for TRC compliant memory card user
// interface implementation
//
// $Workfile: shellMCUIScreenListSaves.h $
// $Author: Hbodasin $
// $Date: 5/02/03 16:01 $
// $Revision: 2 $
//
// Copyright (c) 2002 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
// Further notes, if any
//
//-----------------------------------------------------------------------------
#ifndef MCUI_SCREEN_LIST_SAVES
#define MCUI_SCREEN_LIST_SAVES
#include "shellMCUIScreen.h"

class CMCUIScreenListSaves : public CMCUIScreen
{
//Operations:
public:
	//Constructor:
	CMCUIScreenListSaves();

	//Destructor:
	~CMCUIScreenListSaves()	{}

	//Initialise:
	//virtual void Init();
	
	//Update screen:
	virtual EShellMCUIScreen Update(ShellMCUIOutputs* pOutputParms,
									ShellMCUIInputs*  pInputParms);	
	
//Accessor fns:

private:
	//File table:
	sceMcTblGetDir	_saveData[SHELLMCUI_MAX_NUM_SAVES_PER_CARD];

		
	
//Attributes:	
private:

};


#endif
