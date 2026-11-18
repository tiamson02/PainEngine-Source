/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkfilterscene/hkFilterScene.h>
#include <hkfilterscene/hkFilterSceneAlterMesh.h>
#include <hkfilterscene/resource.h>

extern HINSTANCE hInstance;

BOOL CALLBACK hkFilterSceneAlterMeshDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{	
	hkFilterSceneAlterMesh* filter = reinterpret_cast<hkFilterSceneAlterMesh*> ( (hkUlong) GetWindowLongPtr(hWnd,GWLP_USERDATA)) ; 

	switch(message) 
	{
		case WM_INITDIALOG:
		{	
			filter = (hkFilterSceneAlterMesh*)lParam;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)lParam); // so that it can be retrieved later
					
			CheckDlgButton(hWnd, IDC_REMOVE_INDICES, filter->m_options.m_removeIndices);
			
			return TRUE; // did handle it
		}
		case WM_COMMAND: // UI Changes
		{
			
		}
	}
	return FALSE; //didn't handle it / didn't do much with it
}	

HWND hkFilterSceneAlterMesh::showOptions(HWND owner, const hkRootLevelContainer& data, class hkFilterClassRegistry& classReg)
{
	if (m_optionsDialog)
		hideOptions();
	
	m_optionsDialog = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_ALTER_MESH_DIALOG),
		owner, hkFilterSceneAlterMeshDialogProc, (LPARAM) this );

	return m_optionsDialog;
}

void hkFilterSceneAlterMesh::updateOptions()
{
	if (m_optionsDialog)
	{
		m_options.m_removeIndices = IsDlgButtonChecked(m_optionsDialog, IDC_REMOVE_INDICES) == TRUE;
	}
}

void hkFilterSceneAlterMesh::hideOptions()
{
	updateOptions();

	if (m_optionsDialog)
	{
		DestroyWindow(m_optionsDialog);
	}

	m_optionsDialog = NULL;
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
