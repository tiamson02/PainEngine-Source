#include "Engine.h"

#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;
#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "Engine/InputSystem.h"
#include "Render/Video.h"

#include "Engine/PainMenu.h"

//--------------------------------------------------------------------
//	MenuItemMapTable class
//--------------------------------------------------------------------
MenuItemMapTable::MenuItemMapTable() : MenuItemList()
{
	mType = EMenuItemMapTable;

	ScriptObject txt = GScript.Globals()["TXT"];
	const char * add = txt["Menu"]["Add"].GetString();
	const char * addAll = txt["Menu"]["AddAll"].GetString();
	const char * remove = txt["Menu"]["Remove"].GetString();
	const char * removeAll = txt["Menu"]["RemoveAll"].GetString();
	const char * up = txt["Menu"]["Up"].GetString();
	const char * down = txt["Menu"]["Down"].GetString();

	txt = GScript.Globals()["TXT"];
	const char * addDesc = txt["MenuDesc"]["Add"].GetString();
	const char * addAllDesc = txt["MenuDesc"]["AddAll"].GetString();
	const char * removeDesc = txt["MenuDesc"]["Remove"].GetString();
	const char * removeAllDesc = txt["MenuDesc"]["RemoveAll"].GetString();
	const char * upDesc = txt["MenuDesc"]["Up"].GetString();
	const char * downDesc = txt["MenuDesc"]["Down"].GetString();


    m_setTag("Menu");
	mAvail = new MenuItemList;
	mAvail->SetName( "Available" );
	mAvail->SetText( "" );
	mAvail->SetDesc( "" );
	mAvail->SetHeaderUse( true );
	mAvail->AddItem( txt["Menu"]["AvailableMaps"].GetString(), NULL );

    m_setTag("Menu");
	mOnServer = new MenuItemList;
	mOnServer->SetName( "OnServer" );
	mOnServer->SetText( "" );
	mOnServer->SetDesc( "" );
	mOnServer->SetHeaderUse( true );
	mOnServer->AddItem( txt["Menu"]["ServerMaps"].GetString(), NULL );

    m_setTag("Menu");
	mAddButton = new MenuItem;
	mAddButton->SetName( "Add" );
	mAddButton->SetText( add );
	mAddButton->SetDesc( addDesc );

    m_setTag("Menu");
	mRemoveButton = new MenuItem;
	mRemoveButton->SetName( "Remove" );
	mRemoveButton->SetText( remove );
	mRemoveButton->SetDesc( removeDesc );

    m_setTag("Menu");
	mUpButton = new MenuItem;
	mUpButton->SetName( "Up" );
	mUpButton->SetText( up );
	mUpButton->SetDesc( upDesc );

    m_setTag("Menu");
	mDownButton = new MenuItem;
	mDownButton->SetName( "Down" );
	mDownButton->SetText( down );
	mDownButton->SetDesc( downDesc );

    m_setTag("Menu");
	mAddAllButton = new MenuItem;
	mAddAllButton->SetName( "AddAll" );
	mAddAllButton->SetText( addAll );
	mAddAllButton->SetDesc( addAllDesc );

    m_setTag("Menu");
	mRemoveAllButton = new MenuItem;
	mRemoveAllButton->SetName( "RemoveAll" );
	mRemoveAllButton->SetText( removeAll );
	mRemoveAllButton->SetDesc( removeAllDesc );

    m_setTag("Menu");
	mBorder = new MenuItemBorder;
	mBorder->SetDark( true );

	mMode = "";
}


MenuItemMapTable::~MenuItemMapTable()
{
	if( mBorder )
	{
		delete mBorder;
		mBorder = NULL;
	}

	if( mAvail )
	{
		delete mAvail;
		mAvail = NULL;
	}

	if( mOnServer )
	{
		delete mOnServer;
		mOnServer = NULL;
	}

	if( mAddButton )
	{
		delete mAddButton;
		mAddButton = NULL;
	}

	if( mRemoveButton )
	{
		delete mRemoveButton;
		mRemoveButton = NULL;
	}

	if( mUpButton )
	{
		delete mUpButton;
		mUpButton = NULL;
	}

	if( mDownButton )
	{
		delete mDownButton;
		mDownButton = NULL;
	}

	if( mAddAllButton )
	{
		delete mAddAllButton;
		mAddAllButton = NULL;
	}

	if( mRemoveAllButton )
	{
		delete mRemoveAllButton;
		mRemoveAllButton = NULL;
	}
}


extern ENGINE_API bool IsPKInstalled();


void MenuItemMapTable::FillMapList()
{
	mAvail->UpdateScale();
	mAvail->CalcSize();
	mAvail->CalcPosition();

	DynamicArray<String> files;
	GFileMan.FindFiles("../Data/Levels/dm*.*",files,EFindDirs);

	for (int i=0;i<files.Size();i++)
	{
		const char *lname = files[i].AsChar();

#ifndef STAND_ALONE_SERVER
		if( !IsPKInstalled() && 
			( !stricmp( lname, "DM_Sacred" ) || !stricmp( lname, "DM_Cursed" ) || !stricmp( lname, "DM_Illuminati" )
			 || !stricmp( lname, "DM_Unseen" ) || !stricmp( lname, "DM_Psycho" ) || !stricmp( lname, "DMPCF_Tower" )
			 || !stricmp( lname, "DMPCF_Warehouse" ) ) )
			 continue;
		if (lname[0] != '_')
			mAvail->AddItem( lname, NULL );
#endif
	}

	files.ClearArray();
	GFileMan.FindFiles("../Data/Levels/ctf*.*",files,EFindDirs);

	for (int i=0;i<files.Size();i++)
	{
		const char *lname = files[i].AsChar();
		if (lname[0] != '_')
			mAvail->AddItem( lname, NULL );
	}

	mAvail->UpdateScale();
	mAvail->CalcSize();
	mAvail->CalcPosition();
}


void MenuItemMapTable::Render()
{
	if( !mVisible )
		return;

	if( mAvail->GetNumItems() == 1 )
		Update( mMode.AsChar() );

	if( mBorder )
	{
		mBorder->UpdateScale();
//		mBorder->CalcSize();
		mBorder->CalcPosition();
		mBorder->Render();
	}

	if( mAvail )
	{
		mAvail->UpdateScale();
		mAvail->CalcSize();
		mAvail->CalcPosition();
		mAvail->SetUnderMouse( false );
		mAvail->CheckIfUnderMouse();
		mAvail->Render();
	}
	if( mOnServer )
	{
		mOnServer->UpdateScale();
		mOnServer->CalcSize();
		mOnServer->CalcPosition();
		mOnServer->SetUnderMouse( false );
		mOnServer->CheckIfUnderMouse();
		mOnServer->Render();
	}

	if( mAddButton )
	{
		mAddButton->UpdateScale();
		mAddButton->CalcSize();
		mAddButton->CalcPosition();
		mAddButton->SetUnderMouse( false );
		mAddButton->CheckIfUnderMouse();
		mAddButton->Render();
		mAddButton->RenderDesc();
	}

	if( mRemoveButton )
	{
		mRemoveButton->UpdateScale();
		mRemoveButton->CalcSize();
		mRemoveButton->CalcPosition();
		mRemoveButton->SetUnderMouse( false );
		mRemoveButton->CheckIfUnderMouse();
		mRemoveButton->Render();
		mRemoveButton->RenderDesc();
	}

	if( mUpButton )
	{
		mUpButton->UpdateScale();
		mUpButton->CalcSize();
		mUpButton->CalcPosition();
		mUpButton->SetUnderMouse( false );
		mUpButton->CheckIfUnderMouse();
		mUpButton->Render();
		mUpButton->RenderDesc();
	}

	if( mDownButton )
	{
		mDownButton->UpdateScale();
		mDownButton->CalcSize();
		mDownButton->CalcPosition();
		mDownButton->SetUnderMouse( false );
		mDownButton->CheckIfUnderMouse();
		mDownButton->Render();
		mDownButton->RenderDesc();
	}

	if( mAddAllButton )
	{
		mAddAllButton->UpdateScale();
		mAddAllButton->CalcSize();
		mAddAllButton->CalcPosition();
		mAddAllButton->SetUnderMouse( false );
		mAddAllButton->CheckIfUnderMouse();
		mAddAllButton->Render();
		mAddAllButton->RenderDesc();
	}

	if( mRemoveAllButton )
	{
		mRemoveAllButton->UpdateScale();
		mRemoveAllButton->CalcSize();
		mRemoveAllButton->CalcPosition();
		mRemoveAllButton->SetUnderMouse( false );
		mRemoveAllButton->CheckIfUnderMouse();
		mRemoveAllButton->Render();
		mRemoveAllButton->RenderDesc();
	}
}


bool MenuItemMapTable::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	if( mAvail )
	{
		mAvail->SendEvent( eventType, eventData1, eventData2 );
		MenuListItem *item = mAvail->GetDoubleClicked();
		if( item )
		{
			AddMapToServer( item );
		}
	}
	if( mOnServer )
	{
		mOnServer->SendEvent( eventType, eventData1, eventData2 );
		int item = mOnServer->GetDoubleClickedIndex();
		if( item != -1 )
			RemoveMapFromServer( item );
	}

	switch( eventType )
	{
	case EMenuKeyDown:
	case EMenuKeyDownRepeated:
		break;

	case EMenuKeyUp:
		if( eventData1 != EMouseKeyLeft )
			break;

		if( !mDisabled )
		{
			if( mAddButton->UnderMouse() )
			{
				AddMapToServer( mAvail->GetSelected() );
			}
			else if( mRemoveButton->UnderMouse() )
			{
				RemoveMapFromServer( mOnServer->GetSelectedIndex() );
			}
			else if( mUpButton->UnderMouse() )
			{
				mOnServer->MoveItemUp( mOnServer->GetSelectedIndex() );
			}
			else if( mDownButton->UnderMouse() )
			{
				mOnServer->MoveItemDown( mOnServer->GetSelectedIndex() );
			}
			else if( mAddAllButton->UnderMouse() )
			{
				AddAllMapsToServer();
			}
			else if( mRemoveAllButton->UnderMouse() )
			{
				RemoveAllMapsFromServer();
			}
		}
		break;

	default:
		break;
	}

	return false;
}


void MenuItemMapTable::SetPosition( int x, int y )
{
	if( x == -1 )
	{
		x = (int)(( 1024.0f - GEngine->Video->PMenu->Screen.GetMenuWidth() ) / 2);
	}

	float scale = 1.0f / mScaleX;

	MenuItemList::SetPosition( x, y );

	mAvail->SetPosition( x, y );
	mOnServer->SetPosition( (int)(x + mAvail->GetWidth() * scale + 240), y );

	mUpButton->SetPosition( (int)(x + mAvail->GetWidth() * scale + 70), y + 24 );
	mAddButton->SetPosition( (int)(x + mAvail->GetWidth() * scale + 70), y + 48 );
	mAddAllButton->SetPosition( (int)(x + mAvail->GetWidth() * scale + 70), y + 72 );
	mRemoveAllButton->SetPosition( (int)(x + mAvail->GetWidth() * scale + 70), y + 96 );
	mRemoveButton->SetPosition( (int)(x + mAvail->GetWidth() * scale + 70), y + 120 );
	mDownButton->SetPosition( (int)(x + mAvail->GetWidth() * scale + 70), y + 144 );

	if( mBorder )
	{
		mBorder->SetPosition( (int)(x + mAvail->GetWidth() * scale + 10), y - 20 );
		mBorder->SetSize( 220, 220 );
	}
}

void MenuItemMapTable::SetSmallFont( const char *name, int size )
{
	MenuItem::SetSmallFont( name, size );
    const char *newName = mSmallFont.AsChar();

	mAvail->SetSmallFont( newName, mSmallSize );
	mOnServer->SetSmallFont( newName, mSmallSize );
	mAddButton->SetSmallFont( newName, mSmallSize );
	mRemoveButton->SetSmallFont( newName, mSmallSize );
	mUpButton->SetSmallFont( newName, mSmallSize );
	mDownButton->SetSmallFont( newName, mSmallSize );
	mAddAllButton->SetSmallFont( newName, mSmallSize );
	mRemoveAllButton->SetSmallFont( newName, mSmallSize );
}


void MenuItemMapTable::SetBigFont( const char *name, int size )
{
	MenuItem::SetBigFont( name, size );
    const char *newName = mBigFont.AsChar();

	mAvail->SetBigFont( newName, mBigSize );
	mOnServer->SetBigFont( newName, mBigSize );
	mAddButton->SetBigFont( newName, mBigSize );
	mRemoveButton->SetBigFont( newName, mBigSize );
	mUpButton->SetBigFont( newName, mBigSize );
	mDownButton->SetBigFont( newName, mBigSize );
	mAddAllButton->SetBigFont( newName, mBigSize );
	mRemoveAllButton->SetBigFont( newName, mBigSize );
}


void MenuItemMapTable::SetColors( DWORD text, DWORD disabled, DWORD underMouse, DWORD desc )
{
	MenuItemList::SetColors( text, disabled, underMouse, desc );

	mAvail->SetColors( mTextColor, mDisabledColor, mTextColorUnderMouse, mDescColor );
	mOnServer->SetColors( mTextColor, mDisabledColor, mTextColorUnderMouse, mDescColor );
	mAddButton->SetColors( mTextColor, mDisabledColor, mTextColorUnderMouse, mDescColor );
	mRemoveButton->SetColors( mTextColor, mDisabledColor, mTextColorUnderMouse, mDescColor );
	mUpButton->SetColors( mTextColor, mDisabledColor, mTextColorUnderMouse, mDescColor );
	mDownButton->SetColors( mTextColor, mDisabledColor, mTextColorUnderMouse, mDescColor );
	mAddAllButton->SetColors( mTextColor, mDisabledColor, mTextColorUnderMouse, mDescColor );
	mRemoveAllButton->SetColors( mTextColor, mDisabledColor, mTextColorUnderMouse, mDescColor );
}


void MenuItemMapTable::SetMaxHeight( int height )
{
	MenuItemList::SetMaxHeight( height );

	mAvail->SetMaxHeight( height );
	mOnServer->SetMaxHeight( height );
}


void MenuItemMapTable::AddMapToServer( MenuItemList::MenuListItem *elem )
{
	if( elem )
		mOnServer->AddItem( elem->name.AsChar(), elem->obj );
}


void MenuItemMapTable::RemoveMapFromServer( int index )
{
	mOnServer->RemoveItem( index );
}


void MenuItemMapTable::AddAllMapsToServer()
{
	for( int i=1; i<mAvail->mItems.Size(); i++ )
	{
		AddMapToServer( mAvail->mItems[i] );
	}
}


void MenuItemMapTable::RemoveAllMapsFromServer()
{
	mOnServer->RemoveAll();
}


const char *MenuItemMapTable::GetSelectedMap(){
    MenuListItem *item = mOnServer->GetFirst();
    if( item )
        return item->name.AsChar();

    return NULL;
}

void MenuItemMapTable::RemoveMap( const char *name )
{
	int index = -1;
	for( int i=0; i<mAvail->mItems.Size(); i++ )
	{
		if ( !strcmp(mAvail->mItems[i]->name.AsChar(), name) )
		{
			index = i;
			break;
		}
	}

	if( index > -1 )
		mAvail->RemoveItem( index );

	index = -1;
	for( int i=0; i<mOnServer->mItems.Size(); i++ )
	{
		if ( !strcmp(mOnServer->mItems[i]->name.AsChar(), name) )
		{
			index = i;
			break;
		}
	}

	if( index > -1 )
		mOnServer->RemoveItem( index );
}


void MenuItemMapTable::Update( const char *mode )
{
	DynamicArray<String> mapsToRemove;

	char *toFind = NULL;

	if( !mAvail ) return;

	mAvail->RemoveAll();
	FillMapList();

	if( !strcmp(mode, "People Can Fly") )
	{
		toFind = "DMPCF_";
	}
	else if( !strcmp(mode, "Capture The Flag") )
	{
		toFind = "CTF_";
	}
	else
	{
		toFind = "DM_";
	}

	for( int i=1; i<mAvail->mItems.Size(); i++ )
	{
#ifdef TRANSGAMING
		char map[255];
		strncpy( map, mAvail->mItems[i]->name.AsChar(), sizeof(map));
#else
		char map[255];
		strcpy( map, mAvail->mItems[i]->name.AsChar() );
#endif
        if( strstr( map, toFind ) != map )
			mapsToRemove.Add( map );
	}

	for( int i=0; i<mapsToRemove.Size(); i++ )
	{
		RemoveMap( mapsToRemove[i].AsChar() );
	}

	mMode = mode;

	mapsToRemove.ClearArray();
}
