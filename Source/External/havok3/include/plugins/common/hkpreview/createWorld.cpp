/* 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkpreview/hkPreview.h>
#include <hkpreview/createWorld.h>
#include <hkpreview/properties.h>


// Havok Dynamics includes
#include <hkbase/hkBase.h>
#include <hkcollide/filter/group/hkGroupFilter.h>
#include <hkcollide/agent/hkProcessCollisionInput.h>

// Exporter includes
#include <hkexport2/hkexport.h>
#include <hkexport2/exportworld.h>
#include <hkexport2/inexportstream.h>

// windows includes
#include <Windows.h>

// standard includes
#include <stdio.h>

#include <resource.h>

// DLL instance
extern HINSTANCE g_hDLLModule;

/*
*	CREATION OF HAVOK WORLD FROM HKE
*	USED FOR BOTH PREVIEW AND CONVERSION TO XML
*	MODIFY AT OWN RISK
*/


// Global so it can be accessed by dlg proc
static ErrorLog* g_errorLog;

// Dialog Proc for the Warnings Dialog - Forward declaration
BOOL CALLBACK errorlogDialogCallback (HWND hDialog, UINT msg, WPARAM wparam, LPARAM lparam);

ExportWorld* createWorldFromHke (const char* hkeFileName)
{
	// We start checking that the file exists
	FILE *myFile = fopen(hkeFileName,"r");
	if (myFile == NULL)
	{
		MessageBox(
			NULL,															// handle to owner window
			"The specified hke file does not exist, aborting operation",    // text in message box
			"Havok2 Preview",												// message box title
			MB_OK															// message box style
			);

		return NULL;
	}
	fclose(myFile);


	// This log will contain any warnings generated during import / creation of the world
	g_errorLog = new ErrorLog();

	// Retrieve properties
	const bool hideWarnings = getPropertyDB().getPropertyBool("hideWarnings");
	hkConvertCS::ConversionType conversionType;
	{
		switch (getPropertyDB().getPropertyInt("csConversion"))
		{
		case 0 :
			conversionType = hkConvertCS::CT_IDENTITY;
			break;
		case 1 :
			conversionType = hkConvertCS::CT_FLIP_X;
			break;
		case 2 :
			conversionType = hkConvertCS::CT_FLIP_Y;
			break;
		case 3 :
			conversionType = hkConvertCS::CT_FLIP_Z;
			break;
		case 4 :
			conversionType = hkConvertCS::CT_SWITCH_YZ;
			break;
		default:
			conversionType = hkConvertCS::CT_IDENTITY;
			g_errorLog->AddError("Unknown CS conversion; ignoring");
			break;
		}
	}

	// We use all default values
	// Modify here for other solvers, etc..
	hkWorldCinfo info;
	hkWorld* world = new hkWorld( info );

	//
	hkAgentRegisterUtil::registerAllAgents(world->getCollisionDispatcher());

	// Use collision group system
	hkGroupFilter* groupFilter = new hkGroupFilter();
	world->setCollisionFilter(groupFilter);
	groupFilter->removeReference();

	NS_ExportStream::InExportStream instream(hkeFileName);

	ExportWorld* exportWorld = new ExportWorld(world, conversionType);
	world->removeReference();

	// Havok 3 - We don't collapse transforms into rigid body transform
	exportWorld->SetCollapseTransforms(false);

	exportWorld->Import(instream,*g_errorLog);

	world->getCollisionInput()->setTolerance( exportWorld->GetCollisionTolerance() );

	// If there was any warning, and user doesn't want to hide them, show them
	if ((g_errorLog->IsEmpty() == false) && (hideWarnings == false))
	{
		LPCTSTR pTemplate = MAKEINTRESOURCE(IDD_ERRORLOG_DIALOG);

		DialogBox(HINSTANCE(g_hDLLModule), pTemplate, NULL, errorlogDialogCallback );
	}


	delete g_errorLog;
	g_errorLog = NULL;

	return exportWorld;
}


// DIALOG PROC FOR WARNINGS DIALOG
BOOL CALLBACK errorlogDialogCallback (HWND hDialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// Handle the initialization message
	if( WM_INITDIALOG == msg )
	{
		//CenterWindow(hDialog, GetParent(hDialog));

		HWND listbox=GetDlgItem(hDialog, IDC_ERRORLOG_LIST);

		SendMessage(listbox,LB_RESETCONTENT,0,0);
		for (int it=0; it<g_errorLog->NumberOfLines(); it++)
		{
			const char* pErr = g_errorLog->GetLine(it);
			SendMessage(listbox,LB_INSERTSTRING,(WPARAM) -1,(LPARAM) pErr);
		}

		return TRUE;
	}
	else if( WM_COMMAND == msg )
	{
		if( IDOK == LOWORD(wparam) )
		{
			EndDialog(hDialog,0);
			return TRUE;
		}
		else if ( IDCANCEL == LOWORD(wparam) )
		{
			EndDialog(hDialog,1);
			return TRUE;
		}

		return TRUE;
	}

	return FALSE;
}


/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
