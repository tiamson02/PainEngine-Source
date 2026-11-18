//-----------------------------------------------------------------------------
//              shellMCUIScreenLoad - Memory Card User Interface Driver
//
// Memory Card User Interface Driver for TRC compliant memory card user
// interface implementation
//
// $Workfile: shellMCUIScreenLoad.h $
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
#ifndef MCUI_SCREEN_LOAD
#define MCUI_SCREEN_LOAD
#include "shellMCUIScreen.h"

class CMCUIScreenLoad : public CMCUIScreen
{
//Operations:
public:
	//Constructor:
	CMCUIScreenLoad();

	//Destructor:
	~CMCUIScreenLoad()	{}

	//Initialise:
	//virtual void Init();
	
	//Update screen:
	virtual EShellMCUIScreen Update(ShellMCUIOutputs* pOutputParms,
									ShellMCUIInputs*  pInputParms);	
	
//Accessor fns:

private:
		
	
//Attributes:	
private:
	int 	_numFilesLoaded;
	bool 	_loadSuccessful;

};


#endif
