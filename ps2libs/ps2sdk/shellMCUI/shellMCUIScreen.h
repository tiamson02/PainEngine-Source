//-----------------------------------------------------------------------------
//              shellMCUIScreen - Memory Card User Interface Driver
//
// Memory Card User Interface Driver for TRC compliant memory card user
// interface implementation
//
// $Workfile: shellMCUIScreen.h $
// $Author: Hbodasin $
// $Date: 5/02/03 16:01 $
// $Revision: 3 $
//
// Copyright (c) 2002 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
// Further notes, if any
//
//-----------------------------------------------------------------------------
#ifndef MCUI_SCREEN
#define MCUI_SCREEN
#include "shellMCUI.h"
#include <stdlib.h>

//Use SHELL memory management:
#define MCUI_MALLOC(size)		malloc(size)



class CMCUIScreen
{
//Operations:
public:
	//Constructor:
	CMCUIScreen();

	//Destructor:
	virtual ~CMCUIScreen()	{};

	//Update screen:
	virtual EShellMCUIScreen Update(ShellMCUIOutputs* pOutputParms,
									ShellMCUIInputs*  pInputParms) = 0;

protected:
	void ResetScreen();
	void SetNextScreen(EShellMCUIScreen 	nextScreen);
	void CheckUserCancel(ShellMCUIOutputs*	pOutputParms,
						 ShellMCUIInputs* 	pInputParms, 
					 	 EShellMCUIScreen	previousScreen);
	void CheckCardStatus(ShellMCUIOutputs* 	pOutputParms);
	void CheckReturnValue(bool* 			pSuccess,
			 			  int 				returnVal,
					      int 				nextStateOnSuccess,
					      int 				nextStateOnFail);
	
//Accessor fns:

private:
		
	
//Attributes:	
protected:
	//Main menu option:
	static EShellMCUIMainMenuOption	_mainMenuOption;

	//Current screen:
	static EShellMCUIScreen			_currentScreen;

	//Previous screen:
	static EShellMCUIScreen			_previousScreen;

	//Sub-state for each screen:
	int								_substate;

private:
	
};


#endif
