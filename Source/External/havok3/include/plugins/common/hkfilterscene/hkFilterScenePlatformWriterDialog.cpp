/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkfilterscene/hkFilterScene.h>
#include <hkfilterscene/hkFilterScenePlatformWriter.h>
#include <hkfilterscene/hkFilterScenePlatformWriterOptions.h>

#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkClassEnum.h>
#include <hkserialize/util/hkStructureLayout.h>
#include <hkserialize/packfile/binary/hkBinaryPackfileWriter.h>
#include <hkserialize/packfile/xml/hkXmlPackfileWriter.h>
#include <hkserialize/util/hkRootLevelContainer.h>
#include <hkserialize/util/hkFilterClassRegistry.h>
#include <hkserialize/util/hkClassNameRegistry.h>

#include <hkfiltermanager/hkFilterUtils.h>

#include <hkfilterscene/resource.h>
#include <shlobj.h>
#include <tchar.h>
#include <stdio.h>


LPWSTR ConvertMBToWide( TCHAR* tStr )
{
	int len = MultiByteToWideChar( CP_ACP, 0, tStr, -1, 0, 0);
	WCHAR* res = hkAllocate<WCHAR>( len, HK_MEMORY_CLASS_EXPORT );
	MultiByteToWideChar(CP_ACP, 0, tStr, -1, res, len);
	return res;
}
TCHAR* ConvertWideToMB( LPCWSTR wideStr )
{
	int len = WideCharToMultiByte( CP_ACP, 0, wideStr, -1, 0, 0 , 0, 0 );
	TCHAR* res = hkAllocate<TCHAR>( len, HK_MEMORY_CLASS_EXPORT );
	WideCharToMultiByte(CP_ACP, 0, wideStr, -1, res, len, 0, 0);
	return res;
}

extern HINSTANCE hInstance;

struct OptionToLayoutEntry
{
	hkFilterScenePlatformWriterOptions::Preset preset;
	const hkStructureLayout::LayoutRules* layout;
	char* label;
};

// Storage for the custom layout rules.
static hkStructureLayout::LayoutRules customRules;

typedef hkFilterScenePlatformWriterOptions OPTTYPE;

static OptionToLayoutEntry OptionToLayoutTable[] =
{
{ OPTTYPE::XML,HK_NULL, "XML" },
{ OPTTYPE::MSVC_WIN32,&(hkStructureLayout::MsvcWin32LayoutRules), "Win32 MSVC" },
{ OPTTYPE::MSVC_AMD64,&(hkStructureLayout::MsvcAmd64LayoutRules), "AMD64 MSVC" },
{ OPTTYPE::MSVC_XBOX,&(hkStructureLayout::MsvcXboxLayoutRules), "Xbox" },
{ OPTTYPE::CW_PS2,&(hkStructureLayout::CwPs2LayoutRules), "PS2 CW" },
{ OPTTYPE::GCC32_PS2,&(hkStructureLayout::Gcc32Ps2LayoutRules), "PS2 GCC 3.2" },
{ OPTTYPE::SN31_PS2,&(hkStructureLayout::Sn31Ps2LayoutRules), "PS2 SN 3.1" },
{ OPTTYPE::CW_GAMECUBE,&(hkStructureLayout::CwNgcLayoutRules), "GameCube CW" },
{ OPTTYPE::SN393_GAMECUBE,&(hkStructureLayout::Sn393NgcLayoutRules), "GameCube SN 3.93" },
{ OPTTYPE::GCC295_PS2,&(hkStructureLayout::Gcc295LinuxLayoutRules), "Linux GCC 2.95" },
{ OPTTYPE::GCC33_PS2,&(hkStructureLayout::Gcc33LinuxLayoutRules), "Linux GCC 3.3" },
{ OPTTYPE::SNC_PSP,&(hkStructureLayout::Sn10PspLayoutRules), "PSP SNC 1.0.x" },
{ OPTTYPE::GCC151_PSP,&(hkStructureLayout::Gcc151PspLayoutRules), "PSP GCC 1.5.1" },
{ OPTTYPE::X360,&(hkStructureLayout::Xbox360LayoutRules), "Xbox 360" },
{ OPTTYPE::GCC_PS3,&(hkStructureLayout::GccPs3LayoutRules), "PS3 GCC" },
{ OPTTYPE::CUSTOM, &(customRules), "Custom..." }
};
const int OptionToLayoutTableSize = sizeof(OptionToLayoutTable) / sizeof(OptionToLayoutEntry);

int _findPreset( OPTTYPE::Preset p )
{
	int i=0;
	while ((i < OptionToLayoutTableSize) && OptionToLayoutTable[i].preset != p) 
	{
		++i;
	}
	return i < OptionToLayoutTableSize? i : 0; // default to win32 msvc
}

void hkFilterScenePlatformWriter::process( class hkRootLevelContainer& data, hkFilterClassRegistry& classReg, bool batchMode, const char* infoString, const char* configName, HWND ownerWindow )
{
	// Find the scene in the root level container
	hkxScene* scenePtr = reinterpret_cast<hkxScene*>( data.findObjectByType( hkxSceneClass.getName() ) );
	if (scenePtr == HK_NULL)
	{
		HK_WARN_ALWAYS (0xabbaa5f0, "No scene data found");
		return;
	}
	hkxScene& scene = *scenePtr;

	hkString err; 
	int curPresetIndex = _findPreset( m_options.m_preset );
	if ( (curPresetIndex >=0) && (curPresetIndex < OptionToLayoutTableSize))
	{
		hkFilterUtils::getAssetPath( scene.m_asset? scene.m_asset : "", m_assetPath );
		hkString fullFilename;
		
		// If the filename hasn't been set, use the default.
		if( m_filename.getLength() == 0)
		{
			m_filename = hkString( ".\\$(asset)_$(configuration).hkx" );
		}

		// Process the filename, replacing supported tags with the correct values.
		hkString hkxFilename = hkFilterUtils::hkFilterReplaceVariables( m_filename.cString(), configName, scene.m_asset );

		// Location may be hardcoded and on different drive to m_asset (eg a net drive, or desktop etc)
		// so we should only add asset path if hkxFilename is relative (starts with .)
		// only add asset drive if starts with root (\) and leave as is if starts with drive letter:
		if (hkxFilename.beginsWith(".")) // purely relative, just add asset path
			fullFilename = m_assetPath + hkxFilename;
		else if (hkxFilename.beginsWith("\\") || hkxFilename.beginsWith("/") ) // a root offest, add the start (drive) of asset path
		{
			hkString driveLetter = m_assetPath.substr(0,2);
			fullFilename = driveLetter + hkxFilename;
		}
		// might either be a absolute path (starts with drive) or a normal filename (then add asset path)
		else if ( (hkxFilename.getLength() > 4) && !hkString::strChr(hkxFilename.substr(0,2).cString(), ':'))
		{
			// normal filename (has not got a : in the first 2 entries (or rather [1]), add asset path
            fullFilename = m_assetPath + hkxFilename;
		}
		else // must be absolute or something we don't handle wrt to paths
		{
			fullFilename = hkxFilename;
		}

		hkOstream f(fullFilename.cString());
		if ( f.isOk() )
		{
			hkPackfileWriter::Options o;
			o.m_userTag = m_options.m_userTag; // HKA-612
			o.m_writeMetaInfo = ! m_options.m_removeMetadata; // HKA-612

			hkPackfileWriter* pw = HK_NULL;
			if (OptionToLayoutTable[curPresetIndex].preset != hkFilterScenePlatformWriterOptions::XML)
			{
				pw = new hkBinaryPackfileWriter;
			}
			else
			{
				pw = new hkXmlPackfileWriter;
			}

			if (OptionToLayoutTable[curPresetIndex].preset == hkFilterScenePlatformWriterOptions::CUSTOM)
			{
				customRules.m_bytesInPointer = m_options.m_bytesInPointer;
				customRules.m_emptyBaseClassOptimization = m_options.m_emptyBaseClassOptimized;
				customRules.m_littleEndian = m_options.m_littleEndian;
				customRules.m_reusePaddingOptimization = m_options.m_reusePaddingOptimized;
			}

			pw->setContentsWithRegistry(&data, hkRootLevelContainerClass, &classReg.getVtableRegistry() /*XX add versioning*/);

			if (OptionToLayoutTable[curPresetIndex].layout)
			{
				o.m_layout =  *(OptionToLayoutTable[curPresetIndex].layout);
			}

			hkResult res = pw->save( f.getStreamWriter(), o );
			pw->removeReference();

			if (res == HK_SUCCESS)
			{
				err.printf("Wrote layout for [%s] to file [%s].", OptionToLayoutTable[curPresetIndex].label, fullFilename.cString());
				HK_REPORT( err.cString() );
			}
			else
			{
				err.printf("Could not write layout for [%s] to file [%s]. File may now be corrupt.", OptionToLayoutTable[curPresetIndex].label, fullFilename.cString());
				HK_WARN_ALWAYS( 0xabba9832, err.cString() );
			}
		}
		else
		{
			err.printf("Could not create file [%s] to write to. Aborting.", fullFilename.cString());
			HK_WARN_ALWAYS( 0xabba9833, err.cString() );
		}
	}
	else
	{
		HK_WARN_ALWAYS( 0xabba4323, "Preset layout not valid. Aborting." );
	}
}

void _fillCombo( HWND combo )
{
	for (int i=0; i < OptionToLayoutTableSize; ++i)
	{
		OptionToLayoutEntry& e = OptionToLayoutTable[i];
		SendMessage( combo, CB_ADDSTRING, 0, (LPARAM)e.label);
	}
}

void _enableAllCustom( HWND dlg, bool enabled)
{
	EnableWindow(GetDlgItem(dlg, IDC_EDIT_PTR_BYTES), enabled);
	EnableWindow(GetDlgItem(dlg, IDC_LITTLE_ENDIAN), enabled);
	EnableWindow(GetDlgItem(dlg, IDC_REUSE_PADDING_OPT), enabled);
	EnableWindow(GetDlgItem(dlg, IDC_EMPTYBASE_CLASS_OPT), enabled);
	EnableWindow(GetDlgItem(dlg, IDC_STATIC_BYTESINPTR), enabled);
}

void _reflectCurrentPlatform( HWND hWnd, const OptionToLayoutEntry& curEntry )
{
	const hkStructureLayout::LayoutRules* rules = curEntry.layout;
	if ( rules )
	{
		TCHAR text[1024]; _stprintf(text, "%d", rules->m_bytesInPointer);
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_PTR_BYTES), text);
		CheckDlgButton(hWnd, IDC_LITTLE_ENDIAN, rules->m_littleEndian);
		CheckDlgButton(hWnd, IDC_REUSE_PADDING_OPT, rules->m_reusePaddingOptimization);
		CheckDlgButton(hWnd, IDC_EMPTYBASE_CLASS_OPT, rules->m_emptyBaseClassOptimization);
	}
	else
	{
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_PTR_BYTES), "0");
		CheckDlgButton(hWnd, IDC_LITTLE_ENDIAN, FALSE);
		CheckDlgButton(hWnd, IDC_REUSE_PADDING_OPT, FALSE);
		CheckDlgButton(hWnd, IDC_EMPTYBASE_CLASS_OPT, FALSE);
		_enableAllCustom(hWnd, false);
	}
}

struct _FilterInfo
{
	hkFilterScenePlatformWriter* filter;
	const hkRootLevelContainer* curData;
};
BOOL CALLBACK hkFilterScenePlatformWriterDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{	
	_FilterInfo* filterInfo = reinterpret_cast<_FilterInfo*> ( (hkUlong) GetWindowLongPtr(hWnd,GWLP_USERDATA)); 

	switch(message) 
	{
		case WM_INITDIALOG:
		{	
			filterInfo = (_FilterInfo*)lParam;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)lParam); // so that it can be retrieved later
			filterInfo->filter->m_optionsDialog = hWnd;

			// If no filename has been set, display the default.
			if ( filterInfo->filter->m_filename.getLength() == 0 )
			{
				filterInfo->filter->m_filename = hkString( ".\\$(asset)_$(configuration).hkx" );
			}
			SetDlgItemText(hWnd, IDC_EDIT_FILENAME, filterInfo->filter->m_filename.cString());

			// Custom data
			TCHAR text[1024]; _stprintf(text, "%d", filterInfo->filter->m_options.m_bytesInPointer);
			SetWindowText( GetDlgItem(hWnd, IDC_EDIT_PTR_BYTES), text);
			CheckDlgButton(hWnd, IDC_LITTLE_ENDIAN, filterInfo->filter->m_options.m_littleEndian);
			CheckDlgButton(hWnd, IDC_REUSE_PADDING_OPT, filterInfo->filter->m_options.m_reusePaddingOptimized);
			CheckDlgButton(hWnd, IDC_EMPTYBASE_CLASS_OPT, filterInfo->filter->m_options.m_emptyBaseClassOptimized);

			// Combo box
			HWND hwndCombo = GetDlgItem(hWnd, IDC_PLATFORM_PRESET_COMBO);
			_fillCombo( hwndCombo );
			// Set cur sel
			int curPresetIndex = _findPreset( filterInfo->filter->m_options.m_preset );
			SendMessage(hwndCombo, CB_SETCURSEL, (WPARAM)curPresetIndex, 0 );		

			bool isCustom = filterInfo->filter->m_options.m_preset == OPTTYPE::CUSTOM;
			_enableAllCustom( hWnd, isCustom );
			if (isCustom)
			{
				TCHAR text[1024]; _stprintf(text, "%d", filterInfo->filter->m_options.m_bytesInPointer);
				SetWindowText( GetDlgItem(hWnd, IDC_EDIT_PTR_BYTES), text);
				CheckDlgButton(hWnd, IDC_LITTLE_ENDIAN, filterInfo->filter->m_options.m_littleEndian);
				CheckDlgButton(hWnd, IDC_REUSE_PADDING_OPT, filterInfo->filter->m_options.m_reusePaddingOptimized);
				CheckDlgButton(hWnd, IDC_EMPTYBASE_CLASS_OPT, filterInfo->filter->m_options.m_emptyBaseClassOptimized);
			}
			else
			{
				_reflectCurrentPlatform(hWnd, OptionToLayoutTable[curPresetIndex]);
			}

			// User tag only available in Binary output
			const BOOL isXml = !isCustom && (curPresetIndex == 0);
			EnableWindow(GetDlgItem(hWnd, IDC_ED_USER_TAG), !isXml);

			CheckDlgButton(hWnd, IDC_CB_REMOVE_METADATA, filterInfo->filter->m_options.m_removeMetadata);

			_stprintf(text, "%d", filterInfo->filter->m_options.m_userTag);
			SetWindowText ( GetDlgItem(hWnd, IDC_ED_USER_TAG), text);

			return TRUE; // did handle it
		}
		case WM_COMMAND: // UI Changes
		{
			switch (LOWORD(wParam))
			{
				case IDC_PLATFORM_PRESET_COMBO:
				{
					HWND hwndCombo = GetDlgItem(hWnd, IDC_PLATFORM_PRESET_COMBO);
					LRESULT ir = SendMessage(hwndCombo, CB_GETCURSEL, 0, 0);	
					bool isCustom = OptionToLayoutTable[ir].preset == OPTTYPE::CUSTOM;
					_enableAllCustom( hWnd, isCustom);
					if (!isCustom) // leave as is (just enabled)
					{
						_reflectCurrentPlatform( hWnd, OptionToLayoutTable[ir] );
					}

					const BOOL isXml = !isCustom && (ir == 0);
					EnableWindow(GetDlgItem(hWnd, IDC_ED_USER_TAG), !isXml);

					break;
				}
				case IDC_BROWSE_FILENAME:
				{
					TCHAR filename[2048];
					filename[0] = '\0';

					// Get the filename from the edit control.
					if ( GetDlgItemText( hWnd, IDC_EDIT_FILENAME, filename, 2048 ) > 0 )
					{
						filterInfo->filter->m_filename = filename;
						filterInfo->filter->m_filename = filterInfo->filter->m_filename.replace( '/', '\\' );

						// Check whether m_filename is relative or not.
						if( PathIsRelative( filterInfo->filter->m_filename.cString() ) )
						{
							// It's relative to the asset, so append the asset path in order to open the dialog in the correct location.
							hkString fullFileName = filterInfo->filter->m_assetPath + filterInfo->filter->m_filename;
							strncpy( filename, fullFileName.cString(), 2048);
						}
						else
						{
							// It's absolute, so copy it back into the filename buffer as is (to update any '/'->'\' changes).
							strncpy( filename, filterInfo->filter->m_filename.cString(), 2048);
						}
					}
					
					OPENFILENAME op;
					hkString::memSet( &op, 0, sizeof(OPENFILENAME) );
					op.lStructSize = sizeof(OPENFILENAME);
					op.hwndOwner = hWnd;
					op.lpstrFilter = "Havok Scene Files (*.HKX)\0*.HKX\0All Files (*.*)\0*.*\0\0";
					if ( PathIsRoot( filename ) )
					{
						op.lpstrInitialDir = filename;
					}
					else
					{
						op.lpstrInitialDir = filterInfo->filter->m_assetPath.cString();
						op.lpstrFile = filename;
					}
					op.lpstrDefExt = ".hkx";
					op.nMaxFile = 2048;
					op.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST;
					if ( GetSaveFileName(&op) )
					{
						hkFilterUtils::getReducedFilename( op.lpstrFile, filterInfo->filter->m_assetPath, filterInfo->filter->m_filename );
						SetDlgItemText(hWnd, IDC_EDIT_FILENAME, filterInfo->filter->m_filename.cString() );
					}
				}
			}
			break;
		}
		case WM_DESTROY:
			{
				if (filterInfo) delete filterInfo;
			}
	}
	return FALSE; //didn't handle it / didn't do much with it
}	


HWND hkFilterScenePlatformWriter::showOptions(HWND owner, const hkRootLevelContainer& data, class hkFilterClassRegistry& classReg)
{
	// Find the scene in the root level container
	hkxScene* scenePtr = reinterpret_cast<hkxScene*>( data.findObjectByType( hkxSceneClass.getName() ) );

	if (m_optionsDialog)
		hideOptions();

	hkFilterUtils::getAssetPath( (scenePtr && scenePtr->m_asset)? scenePtr->m_asset : "", m_assetPath );

	_FilterInfo* fi = new _FilterInfo;
	fi->curData = &data;
	fi->filter = this;

	m_optionsDialog = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_PLATFORM_WRITER_DIALOG),
		owner, hkFilterScenePlatformWriterDialogProc, (LPARAM) fi );

	return m_optionsDialog;
}

void hkFilterScenePlatformWriter::updateOptions()
{
	if (m_optionsDialog)
	{
		HWND hwndCombo = GetDlgItem(m_optionsDialog, IDC_PLATFORM_PRESET_COMBO);
		LRESULT ir = SendMessage(hwndCombo, CB_GETCURSEL, 0, 0);
		m_options.m_preset = OptionToLayoutTable[ir].preset;
		m_options.m_littleEndian = IsDlgButtonChecked(m_optionsDialog, IDC_LITTLE_ENDIAN) != FALSE;
		m_options.m_reusePaddingOptimized = IsDlgButtonChecked(m_optionsDialog, IDC_REUSE_PADDING_OPT) != FALSE;
		m_options.m_emptyBaseClassOptimized = IsDlgButtonChecked(m_optionsDialog, IDC_EMPTYBASE_CLASS_OPT) != FALSE;
		m_options.m_removeMetadata = IsDlgButtonChecked(m_optionsDialog, IDC_CB_REMOVE_METADATA) != FALSE;
		TCHAR text[2048];
		GetDlgItemText(m_optionsDialog, IDC_EDIT_PTR_BYTES, text, 2048);
		m_options.m_bytesInPointer = (hkInt8)atoi(text);
		if (m_options.m_bytesInPointer < 4) m_options.m_bytesInPointer = 4;
		else if (m_options.m_bytesInPointer > 16) m_options.m_bytesInPointer = 16;

		GetDlgItemText(m_optionsDialog, IDC_ED_USER_TAG, text, 2048);
		m_options.m_userTag = (hkUint32)atoi(text);

		GetDlgItemText(m_optionsDialog, IDC_EDIT_FILENAME, text, 2048);
		hkFilterUtils::getReducedFilename( text, m_assetPath, m_filename );
	}
}

void hkFilterScenePlatformWriter::hideOptions()
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
