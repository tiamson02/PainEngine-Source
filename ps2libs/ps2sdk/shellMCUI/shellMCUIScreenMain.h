//-----------------------------------------------------------------------------
//              shellMCUIScreenMain - Memory Card User Interface Driver
//
// Memory Card User Interface Driver for TRC compliant memory card user
// interface implementation
//
// $Workfile: shellMCUIScreenMain.h $
// $Author: Hbodasin $
// $Date: 5/02/03 16:02 $
// $Revision: 2 $
//
// Copyright (c) 2002 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
// Further notes, if any
//
//-----------------------------------------------------------------------------
#ifndef MCUI_SCREEN_MAIN
#define MCUI_SCREEN_MAIN
#include "shellMCUIScreen.h"

class CMCUIScreenMain : public CMCUIScreen
{
//Operations:
public:
	//Constructor:
	CMCUIScreenMain();

	//Destructor:
	~CMCUIScreenMain()	{}

	//Initialise:
	//virtual void Init();
	
	//Update screen:
	virtual EShellMCUIScreen Update(ShellMCUIOutputs* pOutputParms,
										ShellMCUIInputs*  pInputParms);	
	
//Accessor fns:

private:

		
	
//Attributes:	
private:

};


#endif
