#include "Engine.h"
#include "Render/Movie.h"
#include "Render/HUD.h"
#include "Render/View.h"
#include "Render/Video.h"
#include "Render/Material.h"

#include "Engine/InputSystem.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Engine/PainMenu.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

//--------------------------------------------------------------------
//	MenuScreen class
//--------------------------------------------------------------------
MenuScreen::MenuScreen()
{
	mBackground = NULL;
	mShowItemsFrame = 0;
	mLoopStart = 0;
	mLoopEnd = 0;
	mLoopChanged = false;
	mPlayingExitMovie = false;
	mFirstShown = 0;
	mTopPos = 160;
	mMovie = NULL;
	mItemAction = "";
	mScroller = NULL;
	mActiveTextEdit = false;
	mWait = 0.0f;
	mWaitStart = 0.0f;

#ifndef	TRANSGAMING_NO_GAMESPY
	memset( &mSelectedServerData, 0, sizeof( MySBServerType ) );
#endif	
}


MenuScreen::~MenuScreen()
{
	Clear();
}


void MenuScreen::Activate()
{
}


void MenuScreen::Render()
{
	View *view = GEngine->Video->GetActiveView();

	static bool firstFrames = true;

	if( mScroller )
		mFirstShown = mScroller->GetValue() + 1;
	else
		mFirstShown = 0;

    //TRACE("MenuScreen::Render(): rendering the menu background using texture %p\n", mBackground);
	if( mBackground )
	{
        //TRACE("MenuScreen::Render(): rendering the menu background using texture '%s'\n", mBackground->FileName.AsChar());
		GEngine->Video->Hud->DrawQuad( mBackground, 0.0f, 0.0f, view->LocalViewport.Width, view->LocalViewport.Height );
	}

	static float lastCheck = GEngine->SysDriver.GetCurrentTime();

	bool keyConfUpdated = false;
	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i]->GetType() == EMenuItemKeyConf && keyConfUpdated == false )
		{
			UpdateKeyConfig();
			keyConfUpdated = true;
		}

		mItems[i]->UpdateScale();
		mItems[i]->CalcSize();
		mItems[i]->CalcPosition();
		mItems[i]->SetUnderMouse( false );
	}

	BYTE trans = 0xFF;

	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i]->CheckIfUnderMouse() )
			break;
	}

	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i]->GetType() == EMenuItemTabGroup )
		{
			mItems[i]->SetTransparency( trans );
			mItems[i]->Render();
			mItems[i]->RenderDesc();
		}
	}

	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i]->GetType() == EMenuItemBorder && strstr( mItems[i]->GetName(), "Settings" ) )
		{
			mItems[i]->SetTransparency( trans );
			mItems[i]->Render();
			mItems[i]->RenderDesc();
		}
	}

	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i]->GetType() == EMenuItemBorder && !strstr( mItems[i]->GetName(), "Settings" ) )
		{
			mItems[i]->SetTransparency( trans );
			mItems[i]->Render();
			mItems[i]->RenderDesc();
		}
	}

	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i]->GetType() == EMenuItemLoadSave )
		{
			mItems[i]->SetTransparency( trans );
			mItems[i]->Render();
			mItems[i]->RenderDesc();
		}
	}

	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i]->GetType() != EMenuItemBorder && mItems[i]->GetType() != EMenuItemTabGroup
			&& mItems[i]->GetType() != EMenuItemCheckbox && mItems[i]->GetType() != EMenuItemLoadSave )
		{
			mItems[i]->SetTransparency( trans );
			mItems[i]->Render();
			mItems[i]->RenderDesc();
		}
	}

	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i]->GetType() == EMenuItemCheckbox )
		{
			mItems[i]->SetTransparency( trans );
			mItems[i]->Render();
			mItems[i]->RenderDesc();
		}
	}

	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i]->GetType() == EMenuItemWeaponList )
		{
			MenuItemBorder *b = ( ( MenuItemWeaponList *)mItems[i] )->GetBorder();
			if( b )
			{
				MenuItemScroller *s = b->GetScroller();
				if( s )
				{
					s->UpdateScale();
					s->CalcSize();
					s->CalcPosition();
					s->SetUnderMouse( false );
					s->CheckIfUnderMouse();
					s->Render();
				}
			}
		}
	}

#ifndef TRANSGAMING
	GScript.Call("PainMenu_PrintGameVersion", 0, "" );
#endif
}


bool MenuScreen::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	if( GEngine->SysDriver.GetCurrentTime() - mWaitStart < mWait )
		return true;


    // eric-> this loop will disappear later on because the MenuItemSimpleKeyConf control will
    //   be obselete.  We'll just keep it in for laughs und giggles for now.
	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i]->GetType() == EMenuItemSimpleKeyConf && mItems[i]->SendEvent( eventType, eventData1, eventData2 ) )
		{
			if( !mItemAction.IsNull() )
			{
				GScript.DoString( mItemAction.AsChar() );
				mItemAction = "";
			}
			return true;
		}
	}

	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i]->GetType() != EMenuItemSimpleKeyConf && mItems[i]->SendEvent( eventType, eventData1, eventData2 ) )
		{
			if( !mItemAction.IsNull() )
			{
#ifdef TRANSGAMING
				GEngine->InputSys->SetLuaToDo(mItemAction.AsChar(), true);
#else
				GEngine->InputSys->SetLuaToDo(mItemAction.AsChar());
#endif
				mItemAction = "";
			}
			return true;
		}
	}

	switch( eventType )
	{
	case EMenuKeyDown:
	case EMenuKeyDownRepeated:
#ifdef TRANSGAMING

        //check if any of the menu navigation keys were pressed.  If the were, find the control
        //that's currently under the mouse (if any) and execute the corresponding navigation
        //action for that control.  
        //NOTE: finding the control under the mouse should be made more efficient!  Currently
        //  the only way to detect it is to loop through all the controls and check the 
        //  MenuItem::UnderMouse() function.  The control that's under the mouse should be
        //  stored as a member of the MenuScreen class and it should get updated when the
        //  mouse check is performed in MenuScreen::Render().
        if (eventData1 != EMouseKeyRight){
            bool                loopItems = true;
            MenuItem::NavAction action = MenuItem::NAV_NONE;
            //TRACE("MenuScreen::SendEvent(): checking navigation key events\n");


            switch (eventData1){
                case BUTTON_UP:
                    TRACE("MenuScreen::SendEvent(): pushed UP\n");
                    action = MenuItem::NAV_UP;
                    break;

                case BUTTON_DOWN:
                    TRACE("MenuScreen::SendEvent(): pushed DOWN\n");
                    action = MenuItem::NAV_DOWN;
                    break;

                case BUTTON_LEFT:
                    TRACE("MenuScreen::SendEvent(): pushed LEFT\n");
                    action = MenuItem::NAV_LEFT;
                    break;

                case BUTTON_RIGHT:
                    TRACE("MenuScreen::SendEvent(): pushed RIGHT\n");
                    action = MenuItem::NAV_RIGHT;
                    break;

                case EMouseKeyMiddle:
                    TRACE("MenuScreen::SendEvent(): pushed MIDDLE MOUSE\n");
                    action = MenuItem::NAV_MIDDLE;
                    break;

                default:
                    loopItems = false;
                    break;
            }


            if (loopItems){

                //find the control that's under the mouse (if any) and enqueue its navigation command
                //to be executed at the end of the frame.
                for (int i = 0; i < mItems.Size(); i++){

                    //this control is under the mouse => enqueue it's navigation action if it has one
                    if (mItems[i]->UnderMouse()){
                        /*int x, y;
                        mItems[i]->GetCenter(&x, &y);
                        TRACE("MenuScreen::SendEvent(): menu item '%s' (pos = <%d, %d>, size = <%d, %d>, center = <%d, %d>) is under the mouse\n", mItems[i]->GetName(), mItems[i]->GetX(), mItems[i]->GetY(), mItems[i]->GetWidth(), mItems[i]->GetHeight(), x, y);//*/

                        if (mItems[i]->HasNavAction(action)){
                            TRACE("MenuScreen::SendEvent(): it has a nav event '%s'\n", mItems[i]->GetNavAction(action));

                            GEngine->InputSys->QueueLuaToDo(mItems[i]->GetNavAction(action));
                        }
                    }
                }
            }
        }
#endif
        
        break;

	case EMenuKeyUp:
#ifdef TRANSGAMING
        if (eventData1 == EMouseKeyRight)
#else
		if( eventData1 == VK_ESCAPE )
#endif
		{
			MenuItem *back = FindItem( "BackButton" );
			if( back )
			{
				const char *action = back->GetAction();
				GScript.DoString( action );
				return true;
			}
		}

		break;

	default:
		break;
	}

	return false;
}


void MenuScreen::SetBackground( const char *name, MenuBGType type )
{
    if (!GEngine->Video)
        return;

	//static bool introWasPlayed = false;

	switch( type )
	{
	case EMenuBGImage:
        TRACE("MenuScreen::SetBackground(): setting the background image to '%s'\n", name);
		mBackground = GEngine->Video->MaterialSys->CreateTexture( name, Texture::ETexNoMipMaps | Texture::ETexNoLOD );
        //TRACE("MenuScreen::SetBackground(): set the background to '%s' {%p}\n", mBackground->FileName.AsChar(), mBackground);
		break;
	}
}


void MenuScreen::Clear( bool delMovie )
{
    if (!GEngine->Video)
        return;

	if( mBackground )
	{
		GEngine->Video->MaterialSys->ReleaseTexture( mBackground );
	}
	mBackground = NULL;

	// Clear items
	for( int i=0; i<mItems.Size(); i++ )
	{
		delete mItems[i];
	}
	mItems.ClearArray();

	mScroller = NULL;
	mFirstShown = 0;

//	mMovie = NULL;
//	mBackground = NULL;
	mShowItemsFrame = 0;
//	mLoopStart = 0;
//	mLoopEnd = 0;
	mLoopChanged = false;
//	mPlayingExitMovie = false;
}

#ifdef TRANSGAMING
MenuItem *MenuScreen::FindItem( const char * name ){
	for( int i=0; i<mItems.Size(); i++ ){
		if (mItems[i]){
			if (!strcmp(mItems[i]->GetName(), name))
				return mItems[i];
		}
	}

	return NULL;
}


MenuItem *MenuScreen::AddTextButton( const char * name, const char * text, const char * desc )
{
    m_setTag("Menu");
	MenuItem *item = new MenuItem;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddCheckbox( const char * name, const char * text, const char * desc )
{
    m_setTag("Menu");
	MenuItem *item = new MenuItemCheckbox;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddTextButtonEx( const char * name, const char * text, const char * desc, const char * value )
{
    m_setTag("Menu");
	MenuItemTextButtonEx *item = new MenuItemTextButtonEx;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetOriginalText( text );
	item->ChangeValue( value );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddSlider( const char * name, const char * text, const char * desc, int min, int max, bool isFloat, int curr, int width, int ctrlWidth )
{
    m_setTag("Menu");
	MenuItemSlider *item = new MenuItemSlider();

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetValue( curr );
	item->SetMinMax( min, max, isFloat );
	item->SetSliderWidth( width );
	item->SetControlWidth( ctrlWidth );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddKeyControl( const char * name, const char * text, const char * opt1, const char * opt2, const char * val1, const char * val2, const char * eng1, const char * eng2 )
{
    m_setTag("Menu");
	MenuItemKeyConf *item = new MenuItemKeyConf;
	item->SetName( name );
	item->SetText( text );
	item->SetDesc( "" );
	item->SetOptionPrimary( opt1 );
	item->SetOptionAlternate( opt2 );
	item->SetKey( val1, eng1 );
	item->SetAlternate( val2, eng2 );
	item->SetIndex( mItems.Size() );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddSimpleKeyConf( const char * name, const char * val, const char * eng, int index )
{
    m_setTag("Menu");
	MenuItemSimpleKeyConf *item = new MenuItemSimpleKeyConf;
	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );
	item->SetIndex( index );
	item->SetKey( val, eng );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddScroller( const char * name, const char * text, const char * desc, int min, int max, int curr, int height )
{
    m_setTag("Menu");
	mScroller = new MenuItemScroller;

	mScroller->SetName( name );
	mScroller->SetText( text );
	mScroller->SetDesc( desc );
	mScroller->SetValue( curr );
	mScroller->SetMinMax( min, max );
	mScroller->SetScrollerHeight( height );

	mItems.Add( mScroller );

	return mScroller;
}


MenuItem *MenuScreen::AddTextEdit( const char * name, const char * text, const char * desc, int len, const char *value )
{
    m_setTag("Menu");
	MenuItemTextEdit *item = new MenuItemTextEdit;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetMaxLength( len );
	item->SetEditText( value );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddSliderImage( const char * name, const char * text, const char * desc, int min, int max, bool isFloat, int curr, int width, int ctrlWidth )
{
    m_setTag("Menu");
	MenuItemSliderImage *item = new MenuItemSliderImage;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetValue( curr );
	item->SetMinMax( min, max, isFloat );
	item->SetSliderWidth( width );
	item->SetControlWidth( ctrlWidth );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddServerList( const char * name, const char * text, const char * desc, bool lanOnly, bool favorites, bool pktv )
{
#ifndef	TRANSGAMING_NO_GAMESPY	
    m_setTag("Menu");
	MenuItemServerList *item = new MenuItemServerList;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->mIsLAN = lanOnly;
	item->mIsFavorites = favorites;
#ifdef TRANSGAMING
	item->mIsPKTV = false;
#else
	item->mIsPKTV = pktv;
#endif

	mItems.Add( item );

	return item;
#else
	return NULL;
#endif	
}


MenuItem *MenuScreen::AddPassword( const char * name, const char * text, const char * desc, int len, const char * value )
{
    m_setTag("Menu");
	MenuItemPassword *item = new MenuItemPassword;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetMaxLength( len );
	item->SetEditText( value );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddNumEdit( const char * name, const char * text, const char * desc, int len, const char * value )
{
    m_setTag("Menu");
	MenuItemNumEdit *item = new MenuItemNumEdit;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetMaxLength( len );
	item->SetEditText( value );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddList( const char * name, bool useHeader )
{
    m_setTag("Menu");
	MenuItemList *item = new MenuItemList;

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );
	item->SetHeaderUse( useHeader );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddWeaponList( const char * name, bool useHeader )
{
    m_setTag("Menu");
	MenuItemWeaponList *item = new MenuItemWeaponList;

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );
	item->SetHeaderUse( useHeader );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddMapTable( const char * name )
{
    m_setTag("Menu");
	MenuItemMapTable *item = new MenuItemMapTable;

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddNumRange( const char * name, const char * text, const char * desc, int minVal, int maxVal, int curr )
{
    m_setTag("Menu");
	MenuItemNumRange *item = new MenuItemNumRange;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetOriginalText( text );
	item->SetMinMax( minVal, maxVal );
	item->SetCurrValue( curr );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddLoadSave( const char * name, bool useHeader )
{
    m_setTag("Menu");
	MenuItemLoadSave *item = new MenuItemLoadSave;

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );
	item->SetHeaderUse( useHeader );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddStaticText( const char * name, const char * text )
{
    m_setTag("Menu");
	MenuItemStaticText *item = new MenuItemStaticText;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( "" );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddImageButton( const char * name, const char * text, const char * desc, const char * image, const char * imageUnder, const char * imagePressed )
{
    m_setTag("Menu");
	MenuItemImageButton *item = new MenuItemImageButton;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetImage( image, imageUnder, imagePressed );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddKeyList( const char * name, bool useHeader )
{
    m_setTag("Menu");
	MenuItemKeyList *item = new MenuItemKeyList;

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );
	item->SetHeaderUse( useHeader );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddBorder( const char * name )
{
    m_setTag("Menu");
	MenuItemBorder *item = new MenuItemBorder;

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddTabGroup( const char * name )
{
    m_setTag("Menu");
	MenuItemTabGroup *item = new MenuItemTabGroup;

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddPlayerModel( const char * name )
{
    m_setTag("Menu");
	MenuItemPlayerModel *item = new MenuItemPlayerModel();

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddColorPicker( const char * name, const char * text, const char * desc )
{
    m_setTag("Menu");
	MenuItemColorPicker *item = new MenuItemColorPicker;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddCharPicker( const char * name, const char * text, const char * desc )
{
    m_setTag("Menu");
	MenuItemCharPicker *item = new MenuItemCharPicker;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddImageButtonEx( const char * name, const char * text, const char * desc, int value )
{
    m_setTag("Menu");
	MenuItemImageButtonEx *item = new MenuItemImageButtonEx;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetValue( value );

	mItems.Add( item );

	return item;
}
#else
MenuItem *MenuScreen::FindItem( String name )
{
	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i] )
		{
			if( mItems[i]->GetName() == name )
				return mItems[i];
		}
	}

	return NULL;
}


MenuItem *MenuScreen::AddTextButton( String name, String text, String desc )
{
    m_setTag("Menu");
	MenuItem *item = new MenuItem;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddCheckbox( String name, String text, String desc )
{
    m_setTag("Menu");
	MenuItem *item = new MenuItemCheckbox;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddTextButtonEx( String name, String text, String desc, String value )
{
    m_setTag("Menu");
	MenuItemTextButtonEx *item = new MenuItemTextButtonEx;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetOriginalText( text );
	item->ChangeValue( value );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddSlider( String name, String text, String desc, int min, int max, bool isFloat, int curr, int width, int ctrlWidth )
{
    m_setTag("Menu");
	MenuItemSlider *item = new MenuItemSlider();

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetValue( curr );
	item->SetMinMax( min, max, isFloat );
	item->SetSliderWidth( width );
	item->SetControlWidth( ctrlWidth );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddKeyControl( String name, String text, String opt1, String opt2, String val1, String val2, String eng1, String eng2 )
{
    m_setTag("Menu");
	MenuItemKeyConf *item = new MenuItemKeyConf;
	item->SetName( name );
	item->SetText( text );
	item->SetDesc( "" );
	item->SetOptionPrimary( opt1 );
	item->SetOptionAlternate( opt2 );
	item->SetKey( val1, eng1 );
	item->SetAlternate( val2, eng2 );
	item->SetIndex( mItems.Size() );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddSimpleKeyConf( String name, String val, String eng, int index )
{
    m_setTag("Menu");
	MenuItemSimpleKeyConf *item = new MenuItemSimpleKeyConf;
	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );
	item->SetIndex( index );
	item->SetKey( val, eng );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddScroller( String name, String text, String desc, int min, int max, int curr, int height )
{
    m_setTag("Menu");
	mScroller = new MenuItemScroller;

	mScroller->SetName( name );
	mScroller->SetText( text );
	mScroller->SetDesc( desc );
	mScroller->SetValue( curr );
	mScroller->SetMinMax( min, max );
	mScroller->SetScrollerHeight( height );

	mItems.Add( mScroller );

	return mScroller;
}


MenuItem *MenuScreen::AddTextEdit( String name, String text, String desc, int len, String value )
{
    m_setTag("Menu");
	MenuItemTextEdit *item = new MenuItemTextEdit;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetMaxLength( len );
	item->SetEditText( value );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddSliderImage( String name, String text, String desc, int min, int max, bool isFloat, int curr, int width, int ctrlWidth )
{
    m_setTag("Menu");
	MenuItemSliderImage *item = new MenuItemSliderImage;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetValue( curr );
	item->SetMinMax( min, max, isFloat );
	item->SetSliderWidth( width );
	item->SetControlWidth( ctrlWidth );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddServerList( String name, String text, String desc, bool lanOnly, bool favorites, bool pktv )
{
#ifndef	TRANSGAMING_NO_GAMESPY	
    m_setTag("Menu");
	MenuItemServerList *item = new MenuItemServerList;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->mIsLAN = lanOnly;
	item->mIsFavorites = favorites;
#ifdef TRANSGAMING
	item->mIsPKTV = false;
#else
	item->mIsPKTV = pktv;
#endif

	mItems.Add( item );

	return item;
#else
	return NULL;
#endif	
}


MenuItem *MenuScreen::AddPassword( String name, String text, String desc, int len, String value )
{
    m_setTag("Menu");
	MenuItemPassword *item = new MenuItemPassword;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetMaxLength( len );
	item->SetEditText( value );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddNumEdit( String name, String text, String desc, int len, String value )
{
    m_setTag("Menu");
	MenuItemNumEdit *item = new MenuItemNumEdit;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetMaxLength( len );
	item->SetEditText( value );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddList( String name, bool useHeader )
{
    m_setTag("Menu");
	MenuItemList *item = new MenuItemList;

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );
	item->SetHeaderUse( useHeader );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddWeaponList( String name, bool useHeader )
{
    m_setTag("Menu");
	MenuItemWeaponList *item = new MenuItemWeaponList;

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );
	item->SetHeaderUse( useHeader );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddMapTable( String name )
{
    m_setTag("Menu");
	MenuItemMapTable *item = new MenuItemMapTable;

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddNumRange( String name, String text, String desc, int minVal, int maxVal, int curr )
{
    m_setTag("Menu");
	MenuItemNumRange *item = new MenuItemNumRange;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetOriginalText( text );
	item->SetMinMax( minVal, maxVal );
	item->SetCurrValue( curr );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddLoadSave( String name, bool useHeader )
{
    m_setTag("Menu");
	MenuItemLoadSave *item = new MenuItemLoadSave;

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );
	item->SetHeaderUse( useHeader );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddStaticText( String name, String text )
{
    m_setTag("Menu");
	MenuItemStaticText *item = new MenuItemStaticText;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( "" );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddImageButton( String name, String text, String desc, String image, String imageUnder, String imagePressed )
{
    m_setTag("Menu");
	MenuItemImageButton *item = new MenuItemImageButton;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetImage( image.AsChar(), imageUnder.AsChar(), imagePressed.AsChar() );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddKeyList( String name, bool useHeader )
{
    m_setTag("Menu");
	MenuItemKeyList *item = new MenuItemKeyList;

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );
	item->SetHeaderUse( useHeader );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddBorder( String name )
{
    m_setTag("Menu");
	MenuItemBorder *item = new MenuItemBorder;

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddTabGroup( String name )
{
    m_setTag("Menu");
	MenuItemTabGroup *item = new MenuItemTabGroup;

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddPlayerModel( String name )
{
    m_setTag("Menu");
	MenuItemPlayerModel *item = new MenuItemPlayerModel();

	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddColorPicker( String name, String text, String desc )
{
    m_setTag("Menu");
	MenuItemColorPicker *item = new MenuItemColorPicker;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddCharPicker( String name, String text, String desc )
{
    m_setTag("Menu");
	MenuItemCharPicker *item = new MenuItemCharPicker;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );

	mItems.Add( item );

	return item;
}


MenuItem *MenuScreen::AddImageButtonEx( String name, String text, String desc, int value )
{
    m_setTag("Menu");
	MenuItemImageButtonEx *item = new MenuItemImageButtonEx;

	item->SetName( name );
	item->SetText( text );
	item->SetDesc( desc );
	item->SetValue( value );

	mItems.Add( item );

	return item;
}
#endif


bool MenuScreen::PlayingMovie()
{
	return false;
}


void MenuScreen::SetMovieLoop( int start, int end )
{
	mLoopStart = start;
	mLoopEnd = end;
	mLoopChanged = true;
}


void MenuScreen::UpdateMovie()
{
	if( mMovie )
		mMovie->Update();
}


void MenuScreen::StopMovie()
{
	if( mMovie )
		mMovie->Stop();
}


void MenuScreen::SetItemsDrawShadow( bool draw )
{
	for( int i=0; i<mItems.Size(); i++ )
	{
		mItems[i]->DrawShadow( draw );
	}
}


void MenuScreen::ClearKeyConfig( const char *key )
{
	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i]->GetType() != EMenuItemKeyConf )
			continue;

		MenuItemKeyConf *item = ( MenuItemKeyConf *)mItems[i];
		const char *primary = item->GetPrimary();

		if( !strcmp(primary, key) )
			item->SetKey( "None", "None" );

		const char *alt = item->GetAlternate();
		if( !strcmp(alt, key) )
			item->SetAlternate( "None", "None" );
	}
}


void MenuScreen::PlayExitMovie( int start, int end, const char *action )
{
	mAfterExitMovie = action;
	if( mMovie )
	{
		mMovie->PlayFrames( start, end, false );
		mPlayingExitMovie = true;
	}
	else
	{
		mPlayingExitMovie = false;
		GEngine->Video->PMenu->ShowMouse( false );
		GScript.DoString( mAfterExitMovie.AsChar() );
	}
}


void MenuScreen::UpdateKeyConfig()
{
	int first = -1;
	int count = 0;

	if( !mScroller )
		return;

	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i]->GetType() != EMenuItemKeyConf )
			continue;

		if( first == -1 )
			first = i;

		if( mItems[i]->IsVisible() )
			count++;
	}

	if( count == 0 )
		return;

#ifdef TRANSGAMING
	const char *font = mItems[first]->GetBigFont();
	int fontSize = mItems[first]->GetBigFontSize();

	GEngine->Video->Hud->SetFont( font, fontSize );
#else
	String font = mItems[first]->GetBigFont();
	int fontSize = mItems[first]->GetBigFontSize();

	GEngine->Video->Hud->SetFont( font.AsChar(), fontSize );
#endif
	int fontH = GEngine->Video->Hud->GetTextHeight();

	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;

	int fit = ( int )ceil( ( mScroller->GetScrollerHeight() - 130 ) * screenHeight / 768.0f / ( float )fontH );

//	GLog.Print( "Count: %d, fit: %d, fontH: %d, height: %d\n", count, fit, fontH, screenHeight );

	if( count > fit )
		mScroller->SetMinMax( 0, count - fit - 1 );
	else
		mScroller->SetMinMax( 0, 0 );
}


void MenuScreen::SetWaitTime( float wait )
{
	mWait = wait; mWaitStart = GEngine->SysDriver.GetCurrentTime();
}


void MenuScreen::AcceptActiveTextEdit()
{
	for( int i=0; i<mItems.Size(); i++ )
	{
		MenuItemTextEdit *item = ( MenuItemTextEdit *)mItems[i];
		if( item->GetType() == EMenuItemTextEdit || item->GetType() == EMenuItemNumEdit || item->GetType() == EMenuItemPassword )
		{
			if( item->IsInEditMode() )
				item->OnEnter();
		}
	}
}


bool MenuScreen::IsKeyInUse( int key )
{
	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i]->GetType() == EMenuItemSimpleKeyConf )
		{
			MenuItemSimpleKeyConf *item = ( MenuItemSimpleKeyConf *)mItems[i];
			if( !strcmp(item->GetKey(), GEngine->InputSys->GetKeyNameEng( key )) )
				return true;
		}
	}

	return false;
}


void MenuScreen::ClearKey( int key )
{
	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i]->GetType() == EMenuItemSimpleKeyConf )
		{
			MenuItemSimpleKeyConf *item = ( MenuItemSimpleKeyConf *)mItems[i];
			if( !strcmp(item->GetKey(), GEngine->InputSys->GetKeyNameEng( key )) )
				item->SetKey( "", "" );
		}
	}
}
