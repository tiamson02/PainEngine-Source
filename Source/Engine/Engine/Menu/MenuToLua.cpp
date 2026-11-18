#include "Engine.h"
#include "Render/Movie.h"
#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Net/NetworkDevice2.h"
#include "Engine/BaseGame.h"
#include "Render/Video.h"
#include "Render/View.h"
#include "Engine/PainMenu.h"
#include "Engine/MagicBoard.h"
#include "Engine/MapSelect.h"
#include "Render/HUD.h"
#ifdef DEBUG
# include "Render/Material.h"
#endif
#ifdef TRANSGAMING
# include "Engine/InputSystem.h"
#endif
#ifdef PKTV_ENABLED
# include "Net/PKTVManager.h"
#endif

#include <shellapi.h>

#ifndef	TRANSGAMING_NO_SAFEDISC
// SafeDisc Protection
#include <pfnapi.h>
#endif

#ifndef	TRANSGAMING_NO_GAMESPY
#define USE_SBSERVER	1
#else
#define USE_SBSERVER	0
#endif

//--------------------------------------------------------------------
//	LUA Interface
//--------------------------------------------------------------------

#define MENU_SCREEN GEngine->Video->PMenu->Screen

static int luaMenu_Activate( lua_State *state )
{
	Script L( state );

	bool active = L.GetBool( 1 );

#ifdef DEBUG
    //GEngine->Video->MaterialSys->EnumerateTextures();
#endif
    m_SetInGame(active ? MEM_MENU : MEM_LEVEL);

    if (GEngine->Video)
		GEngine->Video->PMenu->Activate( active );

	return 0;
}

static int luaMenu_IsActive( lua_State *state )
{
	Script L( state );
	if (GEngine->Video)
		L.PushBool( GEngine->Video->PMenu->Active() );
	else
		L.PushBool( false);
	return 1;
}


static int luaMenu_Clear( lua_State *state )
{
	Script L( state );
	if (GEngine->Video)
		GEngine->Video->PMenu->Clear();
	return 0;
}


static int luaMenu_ClearScreen( lua_State *state )
{
	Script L( state );
	MENU_SCREEN.Clear();
	return 0;
}


#ifdef TRANSGAMING
static int luaMenu_AddTextButton( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * desc = L.GetString( 3 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = MENU_SCREEN.AddTextButton( name, text, desc );
	}

	return 0;
}


static int luaMenu_AddCheckbox( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * desc = L.GetString( 3 );
	bool checked = L.GetBool( 4 );

	MenuItemCheckbox *item = ( MenuItemCheckbox *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemCheckbox *)MENU_SCREEN.AddCheckbox( name, text, desc );
		item->Checked = checked;
	}

	return 0;
}


static int luaMenu_AddTextButtonEx( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * desc = L.GetString( 3 );
	const char * value = L.GetString( 4, "" );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = MENU_SCREEN.AddTextButtonEx( name, text, desc, value );
	}

	return 0;
}


static int luaMenu_AddSlider( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * desc = L.GetString( 3 );
	int min = L.GetInt( 4 );
	int max = L.GetInt( 5 );
	bool isFloat = L.GetBool( 6 );
	int curr = L.GetInt( 7 );
	int width = L.GetInt( 8 );
	int ctrlWidth = L.GetInt( 9 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = MENU_SCREEN.AddSlider( name, text, desc, min, max, isFloat, curr, width, ctrlWidth );
	}

	return 0;
}


static int luaMenu_AddKeyControl( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * opt1 = L.GetString( 3 );
	const char * opt2 = L.GetString( 4 );
	const char * val1 = L.GetString( 5 );
	const char * val2 = L.GetString( 6 );
	const char * eng1 = L.GetString( 7, val1 );
	const char * eng2 = L.GetString( 8, val2 );

	MenuItemKeyConf *item = ( MenuItemKeyConf *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemKeyConf *)MENU_SCREEN.AddKeyControl( name, text, opt1, opt2, val1, val2, eng1, eng2 );
	}

	return 0;
}


static int luaMenu_AddSimpleKeyConf( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * val = L.GetString( 2 );
	const char * eng = L.GetString( 3, val );
	int index = L.GetInt( 4, 0 );

	MenuItemSimpleKeyConf *item = ( MenuItemSimpleKeyConf *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemSimpleKeyConf *)MENU_SCREEN.AddSimpleKeyConf( name, val, eng, index );
	}

	return 0;
}


static int luaMenu_AddScroller( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * desc = L.GetString( 3 );
	int min = L.GetInt( 4 );
	int max = L.GetInt( 5 );
	int curr = L.GetInt( 6 );
	int height = L.GetInt( 7 );

	MenuItemScroller *scr = MENU_SCREEN.GetScroller();
	if( scr == NULL )
	{
		scr = ( MenuItemScroller *)MENU_SCREEN.AddScroller( name, text, desc, min, max, curr, height );
	}

	return 0;
}


static int luaMenu_AddTextEdit( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * desc = L.GetString( 3 );
	int len = L.GetInt( 4 );
	const char * value = L.GetString( 5 );

	MenuItemTextEdit *item = ( MenuItemTextEdit *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemTextEdit *)MENU_SCREEN.AddTextEdit( name, text, desc, len, value );
	}

	return 0;
}


static int luaMenu_AddSliderImage( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * desc = L.GetString( 3 );
	int min = L.GetInt( 4 );
	int max = L.GetInt( 5 );
	bool isFloat = L.GetBool( 6 );
	int curr = L.GetInt( 7 );
	int width = L.GetInt( 8 );
	int ctrlWidth = L.GetInt( 9 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = MENU_SCREEN.AddSliderImage( name, text, desc, min, max, isFloat, curr, width, ctrlWidth );
	}

	return 0;
}


static int luaMenu_AddServerList( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * desc = L.GetString( 3 );
	bool lan = L.GetBool( 4 );
	bool fav = L.GetBool( 5 );
	bool pktv = L.GetBool( 6 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = MENU_SCREEN.AddServerList( name, text, desc, lan, fav, pktv );
	}

	return 0;
}


static int luaMenu_AddNumEdit( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * desc = L.GetString( 3 );
	int len = L.GetInt( 4 );
	const char * value = L.GetString( 5 );

	MenuItemNumEdit *item = ( MenuItemNumEdit *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemNumEdit *)MENU_SCREEN.AddNumEdit( name, text, desc, len, value );
	}

	return 0;
}


static int luaMenu_AddPassword( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * desc = L.GetString( 3 );
	int len = L.GetInt( 4 );
	const char * value = L.GetString( 5 );

	MenuItemPassword *item = ( MenuItemPassword *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemPassword *)MENU_SCREEN.AddPassword( name, text, desc, len, value );
	}

	return 0;
}


static int luaMenu_AddList( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	bool useHeader = L.GetBool( 2, true );

	MenuItemList *item = ( MenuItemList *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemList *)MENU_SCREEN.AddList( name, useHeader );
	}

	return 0;
}


static int luaMenu_AddWeaponList( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	bool useHeader = L.GetBool( 2, true );

	MenuItemWeaponList *item = ( MenuItemWeaponList *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemWeaponList *)MENU_SCREEN.AddWeaponList( name, useHeader );
	}

	return 0;
}


static int luaMenu_AddMapTable( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemMapTable *item = ( MenuItemMapTable *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemMapTable *)MENU_SCREEN.AddMapTable( name );
	}

	return 0;
}


static int luaMenu_AddLoadSave( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	bool useHeader = L.GetBool( 2 );

	MenuItemLoadSave *item = ( MenuItemLoadSave *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemLoadSave *)MENU_SCREEN.AddLoadSave( name, useHeader );
	}

	return 0;
}

#ifndef TRANSGAMING_NO_SAFEDISC
# pragma optimize( "", off )
# pragma inline_depth(0)

// One of three identical functions for safedisc
static int luaMenu_ActivateMap( lua_State *state )
{
	Script L( state );

# ifdef DEBUG
    //GEngine->Video->MaterialSys->EnumerateTextures();
# endif
    m_SetInGame(MEM_MENU);
	GEngine->Game->SwitchMapSelect( true );

	bool showWorld = L.GetBool( 1, true );
	if( !showWorld )
		GEngine->Video->GetActiveView()->Mode |= View::EModeDoNotRenderWorld;

	return 0;
}

# pragma inline_depth()
# pragma optimize( "", on )

PFNAPI_APPLY_SECURITY( luaMenu_ActivateMap );
#endif


static int luaMenu_AddNumRange( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * desc = L.GetString( 3 );
	int min = L.GetInt( 4 );
	int max = L.GetInt( 5 );
	int curr = L.GetInt( 6 );

	MenuItemNumRange *item = ( MenuItemNumRange *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemNumRange *)MENU_SCREEN.AddNumRange( name, text, desc, min, max, curr );
	}

	return 0;
}


static int luaMenu_AddStaticText( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );

	MenuItemStaticText *item = ( MenuItemStaticText *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemStaticText *)MENU_SCREEN.AddStaticText( name, text );
	}

	return 0;
}


static int luaMenu_AddImageButton( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * desc = L.GetString( 3 );
	const char * img  = L.GetString( 4 );
	const char * under = L.GetString( 5 );
	const char * pressed = L.GetString( 6 );

	MenuItemImageButton *item = ( MenuItemImageButton *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemImageButton *)MENU_SCREEN.AddImageButton( name, text, desc, img, under, pressed );
	}

	return 0;
}


static int luaMenu_AddKeyList( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	bool useHeader = L.GetBool( 2 );

	MenuItemKeyList *item = ( MenuItemKeyList *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemKeyList *)MENU_SCREEN.AddKeyList( name, useHeader );
	}

	return 0;
}


static int luaMenu_AddBorder( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	bool dark = L.GetBool( 2, false );

	MenuItemBorder *item = ( MenuItemBorder *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemBorder *)MENU_SCREEN.AddBorder( name );
		item->SetDark( dark );
	}

	return 0;
}


static int luaMenu_AddTabGroup( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	bool dark = L.GetBool( 2, false );

	MenuItemTabGroup *item = ( MenuItemTabGroup *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemTabGroup *)MENU_SCREEN.AddTabGroup( name );
		item->SetDark( dark );
	}

	return 0;
}


static int luaMenu_AddPlayerModel( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemPlayerModel *item = ( MenuItemPlayerModel *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemPlayerModel *)MENU_SCREEN.AddPlayerModel( name );
	}

	return 0;
}


static int luaMenu_AddColorPicker( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * desc = L.GetString( 3 );

	MenuItemColorPicker *item = ( MenuItemColorPicker *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemColorPicker *)MENU_SCREEN.AddColorPicker( name, text, desc );
	}

	return 0;
}


static int luaMenu_AddCharPicker( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * desc = L.GetString( 3 );

	MenuItemCharPicker *item = ( MenuItemCharPicker *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemCharPicker *)MENU_SCREEN.AddCharPicker( name, text, desc );
	}

	return 0;
}


static int luaMenu_AddImageButtonEx( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );
	const char * desc = L.GetString( 3 );
	int value = L.GetInt( 4, 0 );

	MenuItemImageButtonEx *item = ( MenuItemImageButtonEx *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemImageButtonEx *)MENU_SCREEN.AddImageButtonEx( name, text, desc, value );
	}

	return 0;
}


#ifdef TRANSGAMING
static int luaMenu_SetMouseToControl(lua_State *state){
    Script L(state);

	const char *    name = L.GetString( 1 );
    int             x;
    int             y;
    MenuItem *      item;


	item = MENU_SCREEN.FindItem(name);

    if (item){
        item->GetCenter(&x, &y);


        if (GEngine->InputSys)
		    GEngine->InputSys->SetMousePosition(x, y);
    }

    return 0;
}

static int luaMenu_GetItemCenter(lua_State *state){
    Script L(state);

	const char *    name = L.GetString( 1 );
	int             x;
    int             y;
    MenuItem *      item;


	item = MENU_SCREEN.FindItem(name);

	if( item ){
        item->GetCenter(&x, &y);

        L.PushInt(x);
        L.PushInt(y);
	}

    else{
        L.PushInt(0);
        L.PushInt(0);
    }


    return 2;
}

static int luaMenu_SetItemNavAction(lua_State *state){
    Script L(state);

    const char *    name =   L.GetString(1);
    int             action = L.GetInt(2, MenuItem::NAV_NONE);
    const char *    cmd =    L.GetString(3, NULL);
    MenuItem *      item;


    item = MENU_SCREEN.FindItem(name);

    if (item)
        item->SetNavAction((MenuItem::NavAction)action, cmd);


    return 0;
}
#endif


static int luaMenu_SetItemText( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * text = L.GetString( 2 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetText( text );
	}

	return 0;
}


static int luaMenu_SetItemDesc( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * desc = L.GetString( 2 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetDesc( desc );
	}

	return 0;
}


static int luaMenu_SetItemPosition( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	int x = L.GetInt( 2 );
	int y = L.GetInt( 3 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetPosition( x, y );
	}

	return 0;
}


static int luaMenu_SetItemAction( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * action = L.GetString( 2, "" );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetAction( action );
	}

	return 0;
}

static int luaMenu_SetScrollerHeight( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	int h = L.GetInt( 2, 0 );

	MenuItemScroller *item = ( MenuItemScroller *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetScrollerHeight( h );
	}

	return 0;
}


static int luaMenu_SetItemFonts( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * big = L.GetString( 2 );
	int bigSize = L.GetInt( 3 );
	const char * sm = L.GetString( 4 );
	int smSize = L.GetInt( 5 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetBigFont( big, bigSize );
		item->SetSmallFont( sm, smSize );
	}

	return 0;
}

#ifndef TRANSGAMING_NO_SAFEDISC
# pragma optimize( "", off )
# pragma inline_depth(0)
#endif

// One of three identical functions for safedisk
static int luaMenu_SwitchToMap( lua_State *state )
{
	Script L( state );

#ifdef DEBUG
    //GEngine->Video->MaterialSys->EnumerateTextures();
#endif
    m_SetInGame(MEM_MENU);
	GEngine->Game->SwitchMapSelect( true );

	bool showWorld = L.GetBool( 1, true );
	if( !showWorld )
		GEngine->Video->GetActiveView()->Mode |= View::EModeDoNotRenderWorld;


	return 0;
}

#ifndef TRANSGAMING_NO_SAFEDISC
# pragma inline_depth()
# pragma optimize( "", on )

PFNAPI_APPLY_SECURITY( luaMenu_SwitchToMap );
#endif

static int luaMenu_SetItemFontsTex( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * big = L.GetString( 2 );
	const char * sm = L.GetString( 3 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetBigFontTex( big );
		item->SetSmallFontTex( sm );
	}

	return 0;
}


static int luaMenu_SetItemColors( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	int text = L.GetInt( 2 );
	int disabled = L.GetInt( 3 );
	int underMouse = L.GetInt( 4 );
	int desc = L.GetInt( 5 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetColors( text, disabled, underMouse, desc );
	}

	return 0;
}


static int luaMenu_SetItemExitMovie( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	int start = L.GetInt( 2 );
	int end = L.GetInt( 3 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->PlayExitMovie( start, end );
	}

	return 0;
}


static int luaMenu_SetItemAlign( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	MenuAlign align = ( MenuAlign )L.GetInt( 2 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetAlign( align );
	}

	return 0;
}


static int luaMenu_SetItemWidth( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	int width = L.GetInt( 2 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetWidth( width );
	}

	return 0;
}


static int luaMenu_SetItemVisibility( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	bool vis = L.GetBool( 2 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetVisibility( vis );
		if( item->GetType() == EMenuItemKeyConf )
			MENU_SCREEN.UpdateKeyConfig();
	}

	return 0;
}


static int luaMenu_SetItemApplyRequired( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	bool req = L.GetBool( 2 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->ApplyRequired( req );
	}

	return 0;
}


static int luaMenu_SetItemWarning( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	bool warn = L.GetBool( 2, false );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetWarning( warn );
	}

	return 0;
}


static int luaMenu_SetButtonGlowAnim( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * glow = L.GetString( 2, NULL );

	MenuItemImageButton *item = ( MenuItemImageButton *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		if( glow && glow[0] != 0 )
			item->SetGlowAnim( glow );
		else
			item->SetGlowAnim( NULL );
	}

	return 0;
}


static int luaMenu_SetBorderSize( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	int width = L.GetInt( 2 );
	int height = L.GetInt( 3 );

	MenuItemBorder *item = ( MenuItemBorder *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetSize( width, height );
	}

	return 0;
}


static int luaMenu_SetItemSounds( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * accept = L.GetString( 2 );
	const char * lightOn = L.GetString( 3 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetAcceptSound( accept );
		item->SetLightOnSound( lightOn );
	}

	return 0;
}


static int luaMenu_SetKeyItemIndex( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	int index = L.GetInt( 2 );

	MenuItemKeyConf *item = ( MenuItemKeyConf *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetIndex( index );
	}

	return 0;
}


static int luaMenu_SetSliderFullWidth( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	bool full = L.GetBool( 2 );

	MenuItemSlider *item = ( MenuItemSlider *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetFullWidth( full );
	}

	return 0;
}


static int luaMenu_SetListMaxHeight( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	int height = L.GetInt( 2 );

	MenuItemList *item = ( MenuItemList *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetMaxHeight( height );
	}

	return 0;
}


static int luaMenu_SetListBorderWidth( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	int width = L.GetInt( 2, 300 );

	MenuItemList *item = ( MenuItemList *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetBorderWidth( width );
	}

	return 0;
}


static int luaMenu_SetListSeparatorPos( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	int pos = L.GetInt( 2, 0 );

	MenuItemWeaponList *item = ( MenuItemWeaponList *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetSeparatorPos( pos );
	}

	return 0;
}


static int luaMenu_GetListSeparatorPos( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemWeaponList *item = ( MenuItemWeaponList *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushInt( item->GetSeparatorPos() );
		return 1;
	}

	return 0;
}

#ifndef	TRANSGAMING_NO_SAFEDISC
#pragma optimize( "", off )
#pragma inline_depth(0)
#endif

/* One of two identical functions for safedisk */
static int luaMenu_SetBorderScroller( lua_State *state )
{
	Script L( state );

	const char * bor_c = L.GetString( 1 );
	const char * scr_c = L.GetString( 2 );

	MenuItemBorder *bor = ( MenuItemBorder *)MENU_SCREEN.FindItem( bor_c );
	MenuItemScroller *scr = ( MenuItemScroller *)MENU_SCREEN.FindItem( scr_c );
	if( bor && scr )
	{
		bor->AddScroller( scr );
	}

	L.PushString( bor_c );

	return 1;
}

#ifndef	TRANSGAMING_NO_SAFEDISC
#pragma inline_depth()
#pragma optimize( "", on )

PFNAPI_APPLY_SECURITY( luaMenu_SetBorderScroller );
#endif

static int luaMenu_SetBorderColCount( lua_State *state )
{
	Script L( state );

	const char * borN = L.GetString( 1 );
	int count = L.GetInt( 2 );

	MenuItemBorder *bor = ( MenuItemBorder *)MENU_SCREEN.FindItem( borN );
	if( bor )
	{
		bor->SetColumnsCount( count );
	}

	return 0;
}


static int luaMenu_SetBorderColumn( lua_State *state )
{
	Script L( state );

	const char * borN = L.GetString( 1 );
	int col = L.GetInt( 2 );
	int w = L.GetInt( 3 );

	MenuItemBorder *bor = ( MenuItemBorder *)MENU_SCREEN.FindItem( borN );
	if( bor )
	{
		bor->SetColumnWidth( col, w );
	}

	return 0;
}


static int luaMenu_SetBorderHeader( lua_State *state )
{
	Script L( state );

	const char * borN = L.GetString( 1 );
	int h = L.GetInt( 2 );

	MenuItemBorder *bor = ( MenuItemBorder *)MENU_SCREEN.FindItem( borN );
	if( bor )
	{
		bor->SetHeader( h );
	}

	return 0;
}


static int luaMenu_SetStaticTextRect( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	RECT rect;
	rect.left = L.GetInt( 2 );
	rect.top = L.GetInt( 3 );
	rect.right = L.GetInt( 4 );
	rect.bottom = L.GetInt( 5 );

	MenuItemStaticText *item = ( MenuItemStaticText *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetRect( &rect );
	}

	return 0;
}


static int luaMenu_AddImageToSlider( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * img = L.GetString( 2 );

	MenuItemSliderImage *item = ( MenuItemSliderImage *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->AddImage( img );
	}

	return 0;
}


static int luaMenu_AddItemToList( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * itemName = L.GetString( 2 );

	MenuItemList *item = ( MenuItemList *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->AddItem( itemName, NULL );
	}

	return 0;
}


static int luaMenu_AddSaveGameToList( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * itemName = L.GetString( 2 );
	const char * sname = L.GetString( 3 );
	const char * elapsed = L.GetString( 4 );
	const char * date = L.GetString( 5 );
	const char * diff = L.GetString( 6 );

	MenuItemLoadSave *item = ( MenuItemLoadSave *)MENU_SCREEN.FindItem( name );
	if( item )
	{
        m_setTag("Game");
		SaveGame *save = new SaveGame( sname, elapsed, date, diff );
		item->AddItem( itemName, save );
	}

	return 0;
}


static int luaMenu_DisableItem( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->Disable();
	}

	return 0;
}


static int luaMenu_EnableItem( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->Enable();
	}

	return 0;
}


static int luaMenu_SetCheckboxValue( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemCheckbox *item = ( MenuItemCheckbox * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->Checked = L.GetBool( 2 );
	}

	return 0;
}


static int luaMenu_IsItemChecked( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemCheckbox *item = ( MenuItemCheckbox * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushBool( item->Checked );
	}

	return 1;
}


static int luaMenu_IsSliderFloat( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemSlider *item = ( MenuItemSlider * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushBool( item->IsFloat() );
	}

	return 1;
}


static int luaMenu_MoveListItemUp( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemList *item = ( MenuItemList *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->MoveItemUp( item->GetSelectedIndex() );
	}

	return 0;
}


static int luaMenu_MoveListItemDown( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemList *item = ( MenuItemList *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->MoveItemDown( item->GetSelectedIndex() );
	}

	return 0;
}


static int luaMenu_GetListItems( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemList *list = ( MenuItemList * )MENU_SCREEN.FindItem( name );
	if( !list )
		return 0;

	MenuItemList::MenuListItem **items = list->GetItems();

	L.NewTable();
	for( int i=1; i<list->GetNumItems(); i++ )
	{
		L.PushInt(i);
		L.PushString(items[i]->name.AsChar());
		L.SetTable(-3);
	}

	return 1;
}


static int luaMenu_GetNumRangeValue( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemNumRange *item = ( MenuItemNumRange * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushInt( item->GetCurrValue() );
	}

	return 1;
}


static int luaMenu_GetSliderValue( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemSlider *item = ( MenuItemSlider * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushInt( item->GetValue() );
	}

	return 1;
}


static int luaMenu_GetImageButtonExValue( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemImageButtonEx *item = ( MenuItemImageButtonEx * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushInt( item->GetValue() );
	}

	return 1;
}


static int luaMenu_GetTextEditValue( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemTextEdit *item = ( MenuItemTextEdit * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushString( item->GetEditText() );
	}
	else
	{
		L.PushString( "" );
	}

	return 1;
}


static int luaMenu_GetPrimaryKey( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemKeyConf *item = ( MenuItemKeyConf * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushString( item->GetPrimary() );
	}

	return 1;
}


static int luaMenu_GetAlternateKey( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemKeyConf *item = ( MenuItemKeyConf * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushString( item->GetAlternate() );
	}

	return 1;
}


static int luaMenu_GetSimpleKey( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemSimpleKeyConf *item = ( MenuItemSimpleKeyConf * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushString( item->GetKey() );
	}

	return 1;
}


static int luaMenu_RefreshServerList( lua_State *state )
{
#ifndef	TRANSGAMING_NO_GAMESPY
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemServerList *item = ( MenuItemServerList * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->Refresh();
	}
#endif
	return 0;
}


static int luaMenu_StopServerList( lua_State *state )
{
#ifndef	TRANSGAMING_NO_GAMESPY
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemServerList *item = ( MenuItemServerList * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->Stop();
	}
#endif
	return 0;
}


static int luaMenu_ChangeTextButtonExValue( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * value = L.GetString( 2, "" );

	MenuItemTextButtonEx *item = ( MenuItemTextButtonEx * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->ChangeValue( value );
	}

	return 0;
}

static int luaMenu_SetBackground( lua_State *state )
{
	Script L( state );
	const char *file = L.GetString( 1 );
	int type = L.GetInt( 2 );

	MENU_SCREEN.SetBackground( file, ( MenuBGType )type );

	return 0;
}

static int luaMenu_EnableItemBG( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * bg = L.GetString( 2 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetBackground( bg );
	}

	return 0;
}

static int luaMenu_FillMapTable( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemMapTable *item = ( MenuItemMapTable * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->FillMapList();
	}

	return 0;
}


static int luaMenu_UpdateMapTable( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemMapTable *item = ( MenuItemMapTable * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->Update(L.GetString(2,"Free For All"));
	}

	return 0;
}


static int luaMenu_AddMapToServer( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * map = L.GetString( 2 );

	MenuItemMapTable *item = ( MenuItemMapTable * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		MenuItemList::MenuListItem elem;
		elem.name = map;
		elem.obj = NULL;
		item->AddMapToServer( &elem );
	}

	return 0;
}


static int luaMenu_RemoveMapFromTable( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * map = L.GetString( 2 );

	MenuItemMapTable *item = ( MenuItemMapTable * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->RemoveMap( map );
	}

	return 0;
}


static int luaMenu_RemoveAllMapsFromServer( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	MenuItemMapTable *item = ( MenuItemMapTable * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->RemoveAllMapsFromServer();
	}

	return 0;
}


static int luaMenu_GetSelectedMap( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );

	const char * map;

	MenuItemMapTable *item = ( MenuItemMapTable * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		map = item->GetSelectedMap();
        L.PushString( map ? map : "" );
	}
    else
        L.PushString("");


	return 1;
}


static int luaMenu_GetMapsOnServer( lua_State *state )
{
	Script L( state );

	const char * name = "MapSelect";

	MenuItemMapTable *item = ( MenuItemMapTable * )MENU_SCREEN.FindItem( name );
	if( !item )
		return 0;

	MenuItemList *list = item->GetServerMapList();
	if( !list )
		return 0;

	MenuItemList::MenuListItem **maps = list->GetItems();

	L.NewTable();
	for( int i=1; i<list->GetNumItems(); i++ )
	{
		L.PushInt(i);
		L.PushString(maps[i]->name.AsChar());
		L.SetTable(-3);
	}

	return 1;
}

#ifndef	TRANSGAMING_NO_SAFEDISC
//#pragma optimize( "", off )
//#pragma inline_depth(0)

/* One of two identical functions for safedisk */
static int luaMenu_SetScrollerForBorder( lua_State *state )
{
	Script L( state );

	const char * bor_c = L.GetString( 1 );
	const char * scr_c = L.GetString( 2 );

	MenuItemBorder *bor = ( MenuItemBorder *)MENU_SCREEN.FindItem( bor_c );
	MenuItemScroller *scr = ( MenuItemScroller *)MENU_SCREEN.FindItem( scr_c );
	if( bor && scr )
	{
		bor->AddScroller( scr );
	}

	L.PushString( scr_c );

	return 1;
}

//#pragma inline_depth()
//#pragma optimize( "", on )

//PFNAPI_APPLY_SECURITY( luaMenu_SetScrollerForBorder );
#endif

static int luaMenu_AddImageToButtonEx( lua_State *state )
{
	Script L( state );

	const char * name = L.GetString( 1 );
	const char * img = L.GetString( 2, "" );

	MenuItemImageButtonEx *item = ( MenuItemImageButtonEx * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->AddImage( img );
	}

	return 0;
}

static int luaMenu_GetCDKey( lua_State *state )
{
	Script L( state );

	const char * cdkey = GEngine->Registry.CDKey.AsChar();

	L.PushString( cdkey );

	return 1;
}


static int luaMenu_SaveCDKey( lua_State *state )
{
	Script L( state );
	const char * cdkey = L.GetString( 1 );

	GEngine->Registry.CDKey = cdkey;

	return 0;
}


static int luaMenu_AddServerToList( lua_State *state )
{
#ifndef	TRANSGAMING_NO_GAMESPY
	Script L( state );

	const char * ip = L.GetString( 1 );
	unsigned short port = ( unsigned short )L.GetInt( 2 );

	MenuItemServerList *list = ( MenuItemServerList *)MENU_SCREEN.FindItem( "ServerList" );
	if( list )
	{
		list->AddServer( ip, port );
	}
#endif
	return 0;
}


static int luaMenu_GetSelectedServerIP( lua_State *state )
{
#ifndef	TRANSGAMING_NO_GAMESPY
	Script L( state );

	const char * ip = "";

	MenuItemServerList *list = ( MenuItemServerList *)MENU_SCREEN.FindItem( "ServerList" );
	if( list )
	{
		ip = list->GetServerIP();
	}

	L.PushString( ip );
#endif
	return 1;
}


static int luaMenu_GetSelectedServerPort( lua_State *state )
{
#ifndef	TRANSGAMING_NO_GAMESPY
	Script L( state );

	unsigned short port = 0;

	MenuItemServerList *list = ( MenuItemServerList *)MENU_SCREEN.FindItem( "ServerList" );
	if( list )
	{
		port = list->GetServerPort();
	}

	L.PushInt( port );
#endif
	return 1;
}


static int luaMenu_StartServer( lua_State *state )
{
	Script L( state );

	const char * playerName = L.GetString( 1 );
	const char * passwd = L.GetString( 2 );
	const char * map = L.GetString( 3 );
	unsigned short port = ( unsigned short )L.GetInt( 5 );
	const char * interfaceToUse = L.GetString( 6, NULL );
	bool isPublic = L.GetBool( 7, false );

	/*if( ( interfaceToUse == NULL ) && ( !GEngine->InterfaceToUse.IsNull() ) )
		interfaceToUse = GEngine->InterfaceToUse.AsChar();//*/

	String cdkey = GEngine->Registry.CDKey;

	GEngine->Video->PMenu->LoadScreen.Activate( true );

	GEngine->Video->GetActiveView()->Mode |= View::EModeDoNotRenderWorld;
    
    // kasuje obecny swiat
    GScript.DoString("Game:NewLevel('NoName','','',0.3); WORLD.Release()");

	GEngine->SwitchToState( PCFSystem::MultiplayerServer );
	GEngine->Net->SetCDKeyAndIsPublic( cdkey, isPublic );
    if( !GEngine->Net->NetworkServer( map, passwd, interfaceToUse == NULL ? "" : interfaceToUse, port ) )
	{
		L.PushBool( false );
	}
	else
	{
		L.PushBool( true );
	}

	return 1;
}


static int luaMenu_JoinServer( lua_State *state )
{
	Script L( state );

	const char * playerName = L.GetString( 1 );
	const char * passwd = L.GetString( 2 );
	const char * host = L.GetString( 4 );
	unsigned short port = ( unsigned short )L.GetInt( 5 );
	bool isPublic = L.GetBool( 6 );
	bool spec = L.GetBool( 7 );

#ifndef	TRANSGAMING_NO_GAMESPY
#if USE_SBSERVER
	void *s = GEngine->Video->PMenu->Screen.GetSelectedServer();
	if( *(int*)s == 0 ) s = NULL;
#endif
#endif

	String cdkey = GEngine->Registry.CDKey;

	GEngine->Video->PMenu->LoadScreen.Activate( true, 0, "HUD/Map/sketch_mp" );

	GLog.Print( "Delete world\n" );
    // kasuje obecny swiat
    GScript.DoString("Game:NewLevel('NoName','','',0.3); WORLD.Release()");

	GEngine->Video->PMenu->LoadScreen.Activate( true, 0, "HUD/Map/sketch_mp" );

	GLog.Print( "Switch state\n" );
	GEngine->SwitchToState( PCFSystem::MultiplayerClient );

	GEngine->Video->PMenu->LoadScreen.Activate( true, 0, "HUD/Map/sketch_mp" );

	GLog.Print( "Set CDKey\n" );
	GEngine->Net->SetCDKeyAndIsPublic( cdkey, isPublic );

	GEngine->Video->PMenu->LoadScreen.Activate( true, 0, "HUD/Map/sketch_mp" );

	GLog.Print( "ClientAndConnect\n" );
#if !USE_SBSERVER
	if( !GEngine->Net->NetworkClientAndConnect( host, passwd, port, spec ) )
		L.PushBool( false );
	else
		L.PushBool( true );
#else
	if( s )
	{
		GLog.Print( "Connecting with SBServer\n" );
		if( !GEngine->Net->NetworkClientAndConnect( &s, passwd, spec ) )
			L.PushBool( false );
		else
			L.PushBool( true );
	}
	else
	{
		GLog.Print( "Connecting with IP\n" );
		if( !GEngine->Net->NetworkClientAndConnect( host, passwd, port, spec ) )
			L.PushBool( false );
		else
			L.PushBool( true );
	}
#endif // USE_SBSERVER

	GEngine->Video->PMenu->LoadScreen.Activate( true, 0, "HUD/Map/sketch_mp" );

	GEngine->Video->GetActiveView()->Mode |= View::EModeDoNotRenderWorld;

	return 1;
}
#else
static int luaMenu_AddTextButton( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String desc = L.GetString( 3 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = MENU_SCREEN.AddTextButton( name, text, desc );
	}

	return 0;
}


static int luaMenu_AddCheckbox( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String desc = L.GetString( 3 );
	bool checked = L.GetBool( 4 );

	MenuItemCheckbox *item = ( MenuItemCheckbox *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemCheckbox *)MENU_SCREEN.AddCheckbox( name, text, desc );
		item->Checked = checked;
	}

	return 0;
}


static int luaMenu_AddTextButtonEx( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String desc = L.GetString( 3 );
	String value = L.GetString( 4, "" );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = MENU_SCREEN.AddTextButtonEx( name, text, desc, value );
	}

	return 0;
}


static int luaMenu_AddSlider( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String desc = L.GetString( 3 );
	int min = L.GetInt( 4 );
	int max = L.GetInt( 5 );
	bool isFloat = L.GetBool( 6 );
	int curr = L.GetInt( 7 );
	int width = L.GetInt( 8 );
	int ctrlWidth = L.GetInt( 9 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = MENU_SCREEN.AddSlider( name, text, desc, min, max, isFloat, curr, width, ctrlWidth );
	}

	return 0;
}


static int luaMenu_AddKeyControl( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String opt1 = L.GetString( 3 );
	String opt2 = L.GetString( 4 );
	String val1 = L.GetString( 5 );
	String val2 = L.GetString( 6 );
	String eng1 = L.GetString( 7, val1.AsChar() );
	String eng2 = L.GetString( 8, val2.AsChar() );

	MenuItemKeyConf *item = ( MenuItemKeyConf *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemKeyConf *)MENU_SCREEN.AddKeyControl( name, text, opt1, opt2, val1, val2, eng1, eng2 );
	}

	return 0;
}


static int luaMenu_AddSimpleKeyConf( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String val = L.GetString( 2 );
	String eng = L.GetString( 3, val.AsChar() );
	int index = L.GetInt( 4, 0 );

	MenuItemSimpleKeyConf *item = ( MenuItemSimpleKeyConf *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemSimpleKeyConf *)MENU_SCREEN.AddSimpleKeyConf( name, val, eng, index );
	}

	return 0;
}


static int luaMenu_AddScroller( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String desc = L.GetString( 3 );
	int min = L.GetInt( 4 );
	int max = L.GetInt( 5 );
	int curr = L.GetInt( 6 );
	int height = L.GetInt( 7 );

	MenuItemScroller *scr = MENU_SCREEN.GetScroller();
	if( scr == NULL )
	{
		scr = ( MenuItemScroller *)MENU_SCREEN.AddScroller( name, text, desc, min, max, curr, height );
	}

	return 0;
}


static int luaMenu_AddTextEdit( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String desc = L.GetString( 3 );
	int len = L.GetInt( 4 );
	String value = L.GetString( 5 );

	MenuItemTextEdit *item = ( MenuItemTextEdit *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemTextEdit *)MENU_SCREEN.AddTextEdit( name, text, desc, len, value );
	}

	return 0;
}


static int luaMenu_AddSliderImage( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String desc = L.GetString( 3 );
	int min = L.GetInt( 4 );
	int max = L.GetInt( 5 );
	bool isFloat = L.GetBool( 6 );
	int curr = L.GetInt( 7 );
	int width = L.GetInt( 8 );
	int ctrlWidth = L.GetInt( 9 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = MENU_SCREEN.AddSliderImage( name, text, desc, min, max, isFloat, curr, width, ctrlWidth );
	}

	return 0;
}


static int luaMenu_AddServerList( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String desc = L.GetString( 3 );
	bool lan = L.GetBool( 4 );
	bool fav = L.GetBool( 5 );
	bool pktv = L.GetBool( 6 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = MENU_SCREEN.AddServerList( name, text, desc, lan, fav, pktv );
	}

	return 0;
}


static int luaMenu_AddNumEdit( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String desc = L.GetString( 3 );
	int len = L.GetInt( 4 );
	String value = L.GetString( 5 );

	MenuItemNumEdit *item = ( MenuItemNumEdit *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemNumEdit *)MENU_SCREEN.AddNumEdit( name, text, desc, len, value );
	}

	return 0;
}


static int luaMenu_AddPassword( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String desc = L.GetString( 3 );
	int len = L.GetInt( 4 );
	String value = L.GetString( 5 );

	MenuItemPassword *item = ( MenuItemPassword *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemPassword *)MENU_SCREEN.AddPassword( name, text, desc, len, value );
	}

	return 0;
}


static int luaMenu_AddList( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	bool useHeader = L.GetBool( 2, true );

	MenuItemList *item = ( MenuItemList *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemList *)MENU_SCREEN.AddList( name, useHeader );
	}

	return 0;
}


static int luaMenu_AddWeaponList( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	bool useHeader = L.GetBool( 2, true );

	MenuItemWeaponList *item = ( MenuItemWeaponList *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemWeaponList *)MENU_SCREEN.AddWeaponList( name, useHeader );
	}

	return 0;
}


static int luaMenu_AddMapTable( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemMapTable *item = ( MenuItemMapTable *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemMapTable *)MENU_SCREEN.AddMapTable( name );
	}

	return 0;
}


static int luaMenu_AddLoadSave( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	bool useHeader = L.GetBool( 2 );

	MenuItemLoadSave *item = ( MenuItemLoadSave *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemLoadSave *)MENU_SCREEN.AddLoadSave( name, useHeader );
	}

	return 0;
}

#pragma optimize( "", off )
#pragma inline_depth(0)

// One of three identical functions for safedisc
static int luaMenu_ActivateMap( lua_State *state )
{
	Script L( state );

	GEngine->Game->SwitchMapSelect( true );

	bool showWorld = L.GetBool( 1, true );
	if( !showWorld )
		GEngine->Video->GetActiveView()->Mode |= View::EModeDoNotRenderWorld;

	return 0;
}

#pragma inline_depth()
#pragma optimize( "", on )

#ifndef	TRANSGAMING_NO_SAFEDISC
PFNAPI_APPLY_SECURITY( luaMenu_ActivateMap );
#endif

static int luaMenu_AddNumRange( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String desc = L.GetString( 3 );
	int min = L.GetInt( 4 );
	int max = L.GetInt( 5 );
	int curr = L.GetInt( 6 );

	MenuItemNumRange *item = ( MenuItemNumRange *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemNumRange *)MENU_SCREEN.AddNumRange( name, text, desc, min, max, curr );
	}

	return 0;
}


static int luaMenu_AddStaticText( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );

	MenuItemStaticText *item = ( MenuItemStaticText *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemStaticText *)MENU_SCREEN.AddStaticText( name, text );
	}

	return 0;
}


static int luaMenu_AddImageButton( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String desc = L.GetString( 3 );
	String img  = L.GetString( 4 );
	String under = L.GetString( 5 );
	String pressed = L.GetString( 6 );

	MenuItemImageButton *item = ( MenuItemImageButton *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemImageButton *)MENU_SCREEN.AddImageButton( name, text, desc, img, under, pressed );
	}

	return 0;
}


static int luaMenu_AddKeyList( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	bool useHeader = L.GetBool( 2 );

	MenuItemKeyList *item = ( MenuItemKeyList *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemKeyList *)MENU_SCREEN.AddKeyList( name, useHeader );
	}

	return 0;
}


static int luaMenu_AddBorder( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	bool dark = L.GetBool( 2, false );

	MenuItemBorder *item = ( MenuItemBorder *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemBorder *)MENU_SCREEN.AddBorder( name );
		item->SetDark( dark );
	}

	return 0;
}


static int luaMenu_AddTabGroup( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	bool dark = L.GetBool( 2, false );

	MenuItemTabGroup *item = ( MenuItemTabGroup *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemTabGroup *)MENU_SCREEN.AddTabGroup( name );
		item->SetDark( dark );
	}

	return 0;
}


static int luaMenu_AddPlayerModel( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemPlayerModel *item = ( MenuItemPlayerModel *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemPlayerModel *)MENU_SCREEN.AddPlayerModel( name );
	}

	return 0;
}


static int luaMenu_AddColorPicker( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String desc = L.GetString( 3 );

	MenuItemColorPicker *item = ( MenuItemColorPicker *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemColorPicker *)MENU_SCREEN.AddColorPicker( name, text, desc );
	}

	return 0;
}


static int luaMenu_AddCharPicker( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String desc = L.GetString( 3 );

	MenuItemCharPicker *item = ( MenuItemCharPicker *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemCharPicker *)MENU_SCREEN.AddCharPicker( name, text, desc );
	}

	return 0;
}


static int luaMenu_AddImageButtonEx( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );
	String desc = L.GetString( 3 );
	int value = L.GetInt( 4, 0 );

	MenuItemImageButtonEx *item = ( MenuItemImageButtonEx *)MENU_SCREEN.FindItem( name );
	if( item == NULL )
	{
		item = ( MenuItemImageButtonEx *)MENU_SCREEN.AddImageButtonEx( name, text, desc, value );
	}

	return 0;
}


static int luaMenu_SetItemText( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String text = L.GetString( 2 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetText( text );
	}

	return 0;
}


static int luaMenu_SetItemDesc( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String desc = L.GetString( 2 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetDesc( desc );
	}

	return 0;
}


static int luaMenu_SetItemPosition( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	int x = L.GetInt( 2 );
	int y = L.GetInt( 3 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetPosition( x, y );
	}

	return 0;
}


static int luaMenu_SetItemAction( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String action = L.GetString( 2, "" );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetAction( action );
	}

	return 0;
}

static int luaMenu_SetScrollerHeight( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	int h = L.GetInt( 2, 0 );

	MenuItemScroller *item = ( MenuItemScroller *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetScrollerHeight( h );
	}

	return 0;
}


static int luaMenu_SetItemFonts( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String big = L.GetString( 2 );
	int bigSize = L.GetInt( 3 );
	String sm = L.GetString( 4 );
	int smSize = L.GetInt( 5 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetBigFont( big, bigSize );
		item->SetSmallFont( sm, smSize );
	}

	return 0;
}

#pragma optimize( "", off )
#pragma inline_depth(0)

// One of three identical functions for safedisk
static int luaMenu_SwitchToMap( lua_State *state )
{
	Script L( state );

	GEngine->Game->SwitchMapSelect( true );

	bool showWorld = L.GetBool( 1, true );
	if( !showWorld )
		GEngine->Video->GetActiveView()->Mode |= View::EModeDoNotRenderWorld;

	return 0;
}

#pragma inline_depth()
#pragma optimize( "", on )

#ifndef	TRANSGAMING_NO_SAFEDISC
PFNAPI_APPLY_SECURITY( luaMenu_SwitchToMap );
#endif

static int luaMenu_SetItemFontsTex( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String big = L.GetString( 2 );
	String sm = L.GetString( 3 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetBigFontTex( big );
		item->SetSmallFontTex( sm );
	}

	return 0;
}


static int luaMenu_SetItemColors( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	int text = L.GetInt( 2 );
	int disabled = L.GetInt( 3 );
	int underMouse = L.GetInt( 4 );
	int desc = L.GetInt( 5 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetColors( text, disabled, underMouse, desc );
	}

	return 0;
}


static int luaMenu_SetItemExitMovie( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	int start = L.GetInt( 2 );
	int end = L.GetInt( 3 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->PlayExitMovie( start, end );
	}

	return 0;
}


static int luaMenu_SetItemAlign( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	MenuAlign align = ( MenuAlign )L.GetInt( 2 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetAlign( align );
	}

	return 0;
}


static int luaMenu_SetItemWidth( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	int width = L.GetInt( 2 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetWidth( width );
	}

	return 0;
}


static int luaMenu_SetItemVisibility( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	bool vis = L.GetBool( 2 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetVisibility( vis );
		if( item->GetType() == EMenuItemKeyConf )
			MENU_SCREEN.UpdateKeyConfig();
	}

	return 0;
}


static int luaMenu_SetItemApplyRequired( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	bool req = L.GetBool( 2 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->ApplyRequired( req );
	}

	return 0;
}


static int luaMenu_SetItemWarning( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	bool warn = L.GetBool( 2, false );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetWarning( warn );
	}

	return 0;
}


static int luaMenu_SetButtonGlowAnim( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String glow = L.GetString( 2, "" );

	MenuItemImageButton *item = ( MenuItemImageButton *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		if( !glow.IsNull() )
			item->SetGlowAnim( glow.AsChar() );
		else
			item->SetGlowAnim( NULL );
	}

	return 0;
}


static int luaMenu_SetBorderSize( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	int width = L.GetInt( 2 );
	int height = L.GetInt( 3 );

	MenuItemBorder *item = ( MenuItemBorder *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetSize( width, height );
	}

	return 0;
}


static int luaMenu_SetItemSounds( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String accept = L.GetString( 2 );
	String lightOn = L.GetString( 3 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetAcceptSound( accept );
		item->SetLightOnSound( lightOn );
	}

	return 0;
}


static int luaMenu_SetKeyItemIndex( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	int index = L.GetInt( 2 );

	MenuItemKeyConf *item = ( MenuItemKeyConf *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetIndex( index );
	}

	return 0;
}


static int luaMenu_SetSliderFullWidth( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	bool full = L.GetBool( 2 );

	MenuItemSlider *item = ( MenuItemSlider *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetFullWidth( full );
	}

	return 0;
}


static int luaMenu_SetListMaxHeight( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	int height = L.GetInt( 2 );

	MenuItemList *item = ( MenuItemList *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetMaxHeight( height );
	}

	return 0;
}


static int luaMenu_SetListBorderWidth( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	int width = L.GetInt( 2, 300 );

	MenuItemList *item = ( MenuItemList *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetBorderWidth( width );
	}

	return 0;
}


static int luaMenu_SetListSeparatorPos( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	int pos = L.GetInt( 2, 0 );

	MenuItemWeaponList *item = ( MenuItemWeaponList *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetSeparatorPos( pos );
	}

	return 0;
}


static int luaMenu_GetListSeparatorPos( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemWeaponList *item = ( MenuItemWeaponList *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushInt( item->GetSeparatorPos() );
		return 1;
	}

	return 0;
}

#pragma optimize( "", off )
#pragma inline_depth(0)

/* One of two identical functions for safedisk */
static int luaMenu_SetBorderScroller( lua_State *state )
{
	Script L( state );

	String bor_c = L.GetString( 1 );
	String scr_c = L.GetString( 2 );

	MenuItemBorder *bor = ( MenuItemBorder *)MENU_SCREEN.FindItem( bor_c );
	MenuItemScroller *scr = ( MenuItemScroller *)MENU_SCREEN.FindItem( scr_c );
	if( bor && scr )
	{
		bor->AddScroller( scr );
	}

	L.PushString( bor_c.AsChar() );

	return 1;
}

#pragma inline_depth()
#pragma optimize( "", on )

#ifndef	TRANSGAMING_NO_SAFEDISC
PFNAPI_APPLY_SECURITY( luaMenu_SetBorderScroller );
#endif

static int luaMenu_SetBorderColCount( lua_State *state )
{
	Script L( state );

	String borN = L.GetString( 1 );
	int count = L.GetInt( 2 );

	MenuItemBorder *bor = ( MenuItemBorder *)MENU_SCREEN.FindItem( borN );
	if( bor )
	{
		bor->SetColumnsCount( count );
	}

	return 0;
}


static int luaMenu_SetBorderColumn( lua_State *state )
{
	Script L( state );

	String borN = L.GetString( 1 );
	int col = L.GetInt( 2 );
	int w = L.GetInt( 3 );

	MenuItemBorder *bor = ( MenuItemBorder *)MENU_SCREEN.FindItem( borN );
	if( bor )
	{
		bor->SetColumnWidth( col, w );
	}

	return 0;
}


static int luaMenu_SetBorderHeader( lua_State *state )
{
	Script L( state );

	String borN = L.GetString( 1 );
	int h = L.GetInt( 2 );

	MenuItemBorder *bor = ( MenuItemBorder *)MENU_SCREEN.FindItem( borN );
	if( bor )
	{
		bor->SetHeader( h );
	}

	return 0;
}


static int luaMenu_SetStaticTextRect( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	RECT rect;
	rect.left = L.GetInt( 2 );
	rect.top = L.GetInt( 3 );
	rect.right = L.GetInt( 4 );
	rect.bottom = L.GetInt( 5 );

	MenuItemStaticText *item = ( MenuItemStaticText *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetRect( &rect );
	}

	return 0;
}


static int luaMenu_AddImageToSlider( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String img = L.GetString( 2 );

	MenuItemSliderImage *item = ( MenuItemSliderImage *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->AddImage( img.AsChar() );
	}

	return 0;
}


static int luaMenu_AddItemToList( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String itemName = L.GetString( 2 );

	MenuItemList *item = ( MenuItemList *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->AddItem( itemName, NULL );
	}

	return 0;
}


static int luaMenu_AddSaveGameToList( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String itemName = L.GetString( 2 );
	String sname = L.GetString( 3 );
	String elapsed = L.GetString( 4 );
	String date = L.GetString( 5 );
	String diff = L.GetString( 6 );

	MenuItemLoadSave *item = ( MenuItemLoadSave *)MENU_SCREEN.FindItem( name );
	if( item )
	{
        m_setTag("Game");
		SaveGame *save = new SaveGame( sname, elapsed, date, diff );
		item->AddItem( itemName, save );
	}

	return 0;
}


static int luaMenu_DisableItem( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->Disable();
	}

	return 0;
}


static int luaMenu_EnableItem( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->Enable();
	}

	return 0;
}


static int luaMenu_SetCheckboxValue( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemCheckbox *item = ( MenuItemCheckbox * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->Checked = L.GetBool( 2 );
	}

	return 0;
}


static int luaMenu_IsItemChecked( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemCheckbox *item = ( MenuItemCheckbox * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushBool( item->Checked );
	}

	return 1;
}


static int luaMenu_IsSliderFloat( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemSlider *item = ( MenuItemSlider * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushBool( item->IsFloat() );
	}

	return 1;
}


static int luaMenu_MoveListItemUp( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemList *item = ( MenuItemList *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->MoveItemUp( item->GetSelectedIndex() );
	}

	return 0;
}


static int luaMenu_MoveListItemDown( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemList *item = ( MenuItemList *)MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->MoveItemDown( item->GetSelectedIndex() );
	}

	return 0;
}


static int luaMenu_GetListItems( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemList *list = ( MenuItemList * )MENU_SCREEN.FindItem( name );
	if( !list )
		return 0;

	MenuItemList::MenuListItem **items = list->GetItems();

	L.NewTable();
	for( int i=1; i<list->GetNumItems(); i++ )
	{
		L.PushInt(i);
		L.PushString(items[i]->name.AsChar());
		L.SetTable(-3);
	}

	return 1;
}


static int luaMenu_GetNumRangeValue( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemNumRange *item = ( MenuItemNumRange * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushInt( item->GetCurrValue() );
	}

	return 1;
}


static int luaMenu_GetSliderValue( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemSlider *item = ( MenuItemSlider * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushInt( item->GetValue() );
	}

	return 1;
}


static int luaMenu_GetImageButtonExValue( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemImageButtonEx *item = ( MenuItemImageButtonEx * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushInt( item->GetValue() );
	}

	return 1;
}


static int luaMenu_GetTextEditValue( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemTextEdit *item = ( MenuItemTextEdit * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushString( item->GetEditText().AsChar() );
	}
	else
	{
		L.PushString( "" );
	}

	return 1;
}


static int luaMenu_GetPrimaryKey( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemKeyConf *item = ( MenuItemKeyConf * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushString( item->GetPrimary().AsChar() );
	}

	return 1;
}


static int luaMenu_GetAlternateKey( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemKeyConf *item = ( MenuItemKeyConf * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushString( item->GetAlternate().AsChar() );
	}

	return 1;
}


static int luaMenu_GetSimpleKey( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemSimpleKeyConf *item = ( MenuItemSimpleKeyConf * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		L.PushString( item->GetKey().AsChar() );
	}

	return 1;
}


static int luaMenu_RefreshServerList( lua_State *state )
{
#ifndef	TRANSGAMING_NO_GAMESPY
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemServerList *item = ( MenuItemServerList * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->Refresh();
	}
#endif
	return 0;
}


static int luaMenu_StopServerList( lua_State *state )
{
#ifndef	TRANSGAMING_NO_GAMESPY
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemServerList *item = ( MenuItemServerList * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->Stop();
	}
#endif
	return 0;
}


static int luaMenu_ChangeTextButtonExValue( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String value = L.GetString( 2, "" );

	MenuItemTextButtonEx *item = ( MenuItemTextButtonEx * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->ChangeValue( value );
	}

	return 0;
}

static int luaMenu_SetBackground( lua_State *state )
{
	Script L( state );
	String file = L.GetString( 1 );
	int type = L.GetInt( 2 );

	MENU_SCREEN.SetBackground( file.AsChar(), ( MenuBGType )type );

	return 0;
}

static int luaMenu_EnableItemBG( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String bg = L.GetString( 2 );

	MenuItem *item = MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->SetBackground( bg );
	}

	return 0;
}

static int luaMenu_FillMapTable( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemMapTable *item = ( MenuItemMapTable * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->FillMapList();
	}

	return 0;
}


static int luaMenu_UpdateMapTable( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemMapTable *item = ( MenuItemMapTable * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->Update(L.GetString(2,"Free For All"));
	}

	return 0;
}


static int luaMenu_AddMapToServer( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String map = L.GetString( 2 );

	MenuItemMapTable *item = ( MenuItemMapTable * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		MenuItemList::MenuListItem elem;
		elem.name = map;
		elem.obj = NULL;
		item->AddMapToServer( &elem );
	}

	return 0;
}


static int luaMenu_RemoveMapFromTable( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String map = L.GetString( 2 );

	MenuItemMapTable *item = ( MenuItemMapTable * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->RemoveMap( map );
	}

	return 0;
}


static int luaMenu_RemoveAllMapsFromServer( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	MenuItemMapTable *item = ( MenuItemMapTable * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->RemoveAllMapsFromServer();
	}

	return 0;
}


static int luaMenu_GetSelectedMap( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );

	String map = "";

	MenuItemMapTable *item = ( MenuItemMapTable * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		map = item->GetSelectedMap();
	}

	L.PushString( map.AsChar() );

	return 1;
}


static int luaMenu_GetMapsOnServer( lua_State *state )
{
	Script L( state );

	String name = "MapSelect";

	MenuItemMapTable *item = ( MenuItemMapTable * )MENU_SCREEN.FindItem( name );
	if( !item )
		return 0;

	MenuItemList *list = item->GetServerMapList();
	if( !list )
		return 0;

	MenuItemList::MenuListItem **maps = list->GetItems();

	L.NewTable();
	for( int i=1; i<list->GetNumItems(); i++ )
	{
		L.PushInt(i);
		L.PushString(maps[i]->name.AsChar());
		L.SetTable(-3);
	}

	return 1;
}

//#pragma optimize( "", off )
//#pragma inline_depth(0)

/* One of two identical functions for safedisk */
static int luaMenu_SetScrollerForBorder( lua_State *state )
{
	Script L( state );

	String bor_c = L.GetString( 1 );
	String scr_c = L.GetString( 2 );

	MenuItemBorder *bor = ( MenuItemBorder *)MENU_SCREEN.FindItem( bor_c );
	MenuItemScroller *scr = ( MenuItemScroller *)MENU_SCREEN.FindItem( scr_c );
	if( bor && scr )
	{
		bor->AddScroller( scr );
	}

	L.PushString( scr_c.AsChar() );

	return 1;
}

//#pragma inline_depth()
//#pragma optimize( "", on )

//PFNAPI_APPLY_SECURITY( luaMenu_SetScrollerForBorder );

static int luaMenu_AddImageToButtonEx( lua_State *state )
{
	Script L( state );

	String name = L.GetString( 1 );
	String img = L.GetString( 2, "" );

	MenuItemImageButtonEx *item = ( MenuItemImageButtonEx * )MENU_SCREEN.FindItem( name );
	if( item )
	{
		item->AddImage( img );
	}

	return 0;
}

static int luaMenu_GetCDKey( lua_State *state )
{
	Script L( state );

	String cdkey = GEngine->Registry.CDKey;

	L.PushString( cdkey.AsChar() );

	return 1;
}


static int luaMenu_SaveCDKey( lua_State *state )
{
	Script L( state );
	String cdkey = L.GetString( 1 );

	GEngine->Registry.CDKey = cdkey;

	return 0;
}


static int luaMenu_AddServerToList( lua_State *state )
{
#ifndef	TRANSGAMING_NO_GAMESPY
	Script L( state );

	String ip = L.GetString( 1 );
	unsigned short port = ( unsigned short )L.GetInt( 2 );

	MenuItemServerList *list = ( MenuItemServerList *)MENU_SCREEN.FindItem( "ServerList" );
	if( list )
	{
		list->AddServer( ip.AsChar(), port );
	}
#endif
	return 0;
}


static int luaMenu_GetSelectedServerIP( lua_State *state )
{
#ifndef	TRANSGAMING_NO_GAMESPY
	Script L( state );

	String ip = "";

	MenuItemServerList *list = ( MenuItemServerList *)MENU_SCREEN.FindItem( "ServerList" );
	if( list )
	{
		ip = list->GetServerIP();
	}

	L.PushString( ip.AsChar() );
#endif
	return 1;
}


static int luaMenu_GetSelectedServerPort( lua_State *state )
{
#ifndef	TRANSGAMING_NO_GAMESPY
	Script L( state );

	unsigned short port = 0;

	MenuItemServerList *list = ( MenuItemServerList *)MENU_SCREEN.FindItem( "ServerList" );
	if( list )
	{
		port = list->GetServerPort();
	}

	L.PushInt( port );
#endif
	return 1;
}


static int luaMenu_StartServer( lua_State *state )
{
	Script L( state );

	String playerName = L.GetString( 1 );
	String passwd = L.GetString( 2 );
	String map = L.GetString( 3 );
	unsigned short port = ( unsigned short )L.GetInt( 5 );
	String interfaceToUse = L.GetString( 6, "" );
	bool isPublic = L.GetBool( 7, false );

	if( ( interfaceToUse.IsNull() ) && ( !GEngine->InterfaceToUse.IsNull() ) )
		interfaceToUse = GEngine->InterfaceToUse;

	String cdkey = GEngine->Registry.CDKey;

	GEngine->Video->PMenu->LoadScreen.Activate( true );

	GEngine->Video->GetActiveView()->Mode |= View::EModeDoNotRenderWorld;
    
    // kasuje obecny swiat
    GScript.DoString("Game:NewLevel('NoName','','',0.3); WORLD.Release()");

	GEngine->SwitchToState( PCFSystem::MultiplayerServer );
	GEngine->Net->SetCDKeyAndIsPublic( cdkey, isPublic );
	if( !GEngine->Net->NetworkServer( map, passwd, interfaceToUse, port ) )
	{
		L.PushBool( false );
	}
	else
	{
		L.PushBool( true );
	}

	return 1;
}


static int luaMenu_JoinServer( lua_State *state )
{
	Script L( state );

	String playerName = L.GetString( 1 );
	String passwd = L.GetString( 2 );
	String host = L.GetString( 4 );
	unsigned short port = ( unsigned short )L.GetInt( 5 );
	bool isPublic = L.GetBool( 6 );
	bool spec = L.GetBool( 7 );

#ifndef	TRANSGAMING_NO_GAMESPY
#if USE_SBSERVER
	void *s = GEngine->Video->PMenu->Screen.GetSelectedServer();
	if( *(int*)s == 0 ) s = NULL;
#endif
#endif

	String cdkey = GEngine->Registry.CDKey;

	GEngine->Video->PMenu->LoadScreen.Activate( true, 0, "HUD/Map/sketch_mp" );

	GLog.Print( "Delete world\n" );
    // kasuje obecny swiat
    GScript.DoString("Game:NewLevel('NoName','','',0.3); WORLD.Release()");

	GEngine->Video->PMenu->LoadScreen.Activate( true, 0, "HUD/Map/sketch_mp" );

	GLog.Print( "Switch state\n" );
	GEngine->SwitchToState( PCFSystem::MultiplayerClient );

	GEngine->Video->PMenu->LoadScreen.Activate( true, 0, "HUD/Map/sketch_mp" );

	GLog.Print( "Set CDKey\n" );
	GEngine->Net->SetCDKeyAndIsPublic( cdkey, isPublic );

	GEngine->Video->PMenu->LoadScreen.Activate( true, 0, "HUD/Map/sketch_mp" );

	GLog.Print( "ClientAndConnect\n" );
#if !USE_SBSERVER
	if( !GEngine->Net->NetworkClientAndConnect( host.AsChar(), passwd, port, spec ) )
		L.PushBool( false );
	else
		L.PushBool( true );
#else
	if( s )
	{
		GLog.Print( "Connecting with SBServer\n" );
		if( !GEngine->Net->NetworkClientAndConnect( &s, passwd, spec ) )
			L.PushBool( false );
		else
			L.PushBool( true );
	}
	else
	{
		GLog.Print( "Connecting with IP\n" );
		if( !GEngine->Net->NetworkClientAndConnect( host.AsChar(), passwd, port, spec ) )
			L.PushBool( false );
		else
			L.PushBool( true );
	}
#endif // USE_SBSERVER

	GEngine->Video->PMenu->LoadScreen.Activate( true, 0, "HUD/Map/sketch_mp" );

	GEngine->Video->GetActiveView()->Mode |= View::EModeDoNotRenderWorld;

	return 1;
}
#endif





static int luaMenu_SetMenuWidth( lua_State *state )
{
	Script L( state );
	int width = L.GetInt( 1 );

	MENU_SCREEN.SetMenuWidth( width );

	return 0;
}


static int luaMenu_SetTopPosition( lua_State *state )
{
	Script L( state );
	int pos = L.GetInt( 1 );

	MENU_SCREEN.SetTopPos( pos );

	return 0;
}


static int luaMenu_SetMovieLoop( lua_State *state )
{
	Script L( state );
	int loopStart = L.GetInt( 1 );
	int loopEnd = L.GetInt( 2 );

	MENU_SCREEN.SetMovieLoop( loopStart, loopEnd );

	return 0;
}


static int luaMenu_SetItemsFadeLength( lua_State *state )
{
	Script L( state );
	int len = L.GetInt( 1 );

	MENU_SCREEN.SetItemsFadeLength( len );

	return 0;
}


static int luaMenu_SetItemsDrawShadow( lua_State *state )
{
	Script L( state );
	bool draw = L.GetBool( 1 );

	MENU_SCREEN.SetItemsDrawShadow( draw );

	return 0;
}


static int luaMenu_SetShowItemsFrame( lua_State *state )
{
	Script L( state );
	int frame = L.GetInt( 1 );

	MENU_SCREEN.SetShowItemsFrame( frame );

	return 0;
}


static int luaMenu_PlayMovie( lua_State *state )
{
	Script L( state );
//	View *view = GEngine->Video->GetActiveView();
//	view->PlayMovie( L.GetString( 1, "" ) );
	PlayBink( L.GetString( 1, "" ), L.GetInt( 2, -1 ) );
	return 0;
}


#ifndef TRANSGAMING
static int luaMenu_RunningInGame( lua_State *state )
{
    String curLevelName = "NoName";
    if (!GScript.Globals()["Lev"].IsNil())
        curLevelName = GScript.Globals()["Lev"]["_Name"].GetString();

	if( GEngine->Video->PMenu->Active() && curLevelName == "NoName" )
		GEngine->Video->PMenu->InGame = false;
	else
		GEngine->Video->PMenu->InGame = true;

	Script L( state );
	L.PushBool( GEngine->Video->PMenu->InGame );
	return 1;
}
#endif


static int luaMenu_ShowMenu( lua_State *state )
{
	Script L( state );

#ifdef DEBUG
    //GEngine->Video->MaterialSys->EnumerateTextures();
#endif
    m_SetInGame(MEM_MENU);
	GEngine->Game->ShowMenu();

	return 0;
}


static int luaMenu_ReturnToGame( lua_State *state )
{
	Script L( state );

	GEngine->Game->HideMenu();

	return 0;
}


#ifndef TRANSGAMING
static int luaMenu_JoinPKTV( lua_State *state )
{
	Script L( state );

	String args = L.GetString( 1 );
	DynamicArray<String> tokens;
	String::Tokenize( args.AsChar(), " ", tokens );
	String address = tokens[0];
	String passwd = tokens.Size()>1?tokens[1]:"";

	GEngine->SwitchToState( PCFSystem::DemoPlay );
	GLog.Print( "Start Demo Play\n" );
#ifdef PKTV_ENABLED
	GEngine->PKTV->ClientConnect( address.AsChar(), passwd.AsChar(), GEngine->InterfaceToUse.AsChar() );
#endif
	L.PushBool( true );
	return 1;
}

static int luaMenu_PKTVRecordStart( lua_State *state )
{
#ifdef PKTV_ENABLED
	if( GEngine->PKTV )
	{
		Script L( state );
		GEngine->PKTV->RecordStart( L.GetString( 1 ) );
	}
#endif
	return 0;
}

static int luaMenu_PKTVRecordStop( lua_State *state )
{
#ifdef PKTV_ENABLED
	if( GEngine->PKTV )
	{
		GEngine->PKTV->RecordStop();
	}
#endif
	return 0;
}

static int luaMenu_PKTVPlay( lua_State *state )
{
#ifdef PKTV_ENABLED
	if( GEngine->PKTV )
	{
		Script L( state );
		GEngine->SwitchToState( PCFSystem::DemoPlay );
		GLog.Print( "Start Demo Play\n" );
		if( GEngine->Video )
		{
# ifndef TRANSGAMING
			GEngine->Video->Hud->mConsole.Activate( false );
# endif
			GEngine->Video->PMenu->Activate( false );
		}
		GEngine->PKTV->ClientPlayFile( L.GetString( 1 ) );
	}
#endif
	return 0;
}

static int luaMenu_PKTVPlaying( lua_State *state )
{
	Script L( state );

#ifdef PKTV_ENABLED
	if( GEngine->PKTV && GEngine->GameState == PCFSystem::DemoPlay )
	{
		L.PushBool( true );
	}
	else
#endif
	{
		L.PushBool( false );
	}
	return 1;
}
#endif

static int luaMenu_SwitchToMenu( lua_State *state )
{
	Script L( state );

#ifdef DEBUG
    //GEngine->Video->MaterialSys->EnumerateTextures();
#endif
    m_SetInGame(MEM_MENU);
	GEngine->Game->SwitchMenu( true );

	return 0;
}

#ifndef	TRANSGAMING_NO_SAFEDISC
# pragma optimize( "", off )
# pragma inline_depth(0)

// One of three identical functions for safedisk
static int luaMenu_SwitchToLevelSel( lua_State *state )
{
	Script L( state );

# ifdef DEBUG
    //GEngine->Video->MaterialSys->EnumerateTextures();
# endif
    m_SetInGame(MEM_MENU);
	GEngine->Game->SwitchMapSelect( true );

	bool showWorld = L.GetBool( 1, true );
	if( !showWorld )
		GEngine->Video->GetActiveView()->Mode |= View::EModeDoNotRenderWorld;

	return 0;
}

# pragma inline_depth()
# pragma optimize( "", on )

PFNAPI_APPLY_SECURITY( luaMenu_SwitchToLevelSel );
#endif

static int luaMenu_SwitchToBoard( lua_State *state )
{
	Script L( state );

#ifdef DEBUG
    //GEngine->Video->MaterialSys->EnumerateTextures();
#endif
    m_SetInGame(MEM_MENU);
	GEngine->Game->SwitchMagicBoard( true );

	return 0;
}


static int luaMenu_MapReset( lua_State *state )
{
	Script L( state );
	GEngine->Video->MapSel->Reset();
	return 0;
}


static int luaMenu_AddLevelToMap( lua_State *state )
{
	Script L( state );

	int chapter = L.GetInt( 1 );
    int status = L.GetInt( 6, 0 );

	LevelDesc map;
	//map.dir = L.GetString( 2 );
	map.name = L.GetString( 2 );
	map.sketchName = L.GetString( 3 );
	map.cardCondition = L.GetString( 4 );
	//map.sketch = NULL;
	map.cardIndex = L.GetInt( 5, 1 );

#ifdef DEBUG
    switch (status){
        case 0: map.status = LevelDesc::STATUS_UNAVAILABLE; break;
        case 1: map.status = LevelDesc::STATUS_CURRENT;     break;
        case 2: map.status = LevelDesc::STATUS_FINISHED;    break;
        case 3: map.status = LevelDesc::STATUS_WRONGDIFF;   break;
        default:
            map.status = LevelDesc::STATUS_UNAVAILABLE;
            TRACE("luaMenu_AddLevelToMap(): ERROR-> invalid level status: %d\n", status);
            break;
    }
#else
    map.status = (LevelDesc::LevelStatus)status;
#endif

    TRACE("luaMenu_AddLevelToMap(): adding a map to chapter %d:\n"
          "     name =          '%s'\n"
          "     sketch =        '%s'\n"
          "     cardCondition = '%s'\n"
          "     cardIndex =     %d\n"
          "     status =        %d\n", 
          chapter,
          map.name.AsChar(),
          map.sketchName.AsChar(),
          map.cardCondition.AsChar(),
          map.cardIndex,
          map.status);

	GEngine->Video->MapSel->AddMap( chapter, map );

	return 0;
}

static int luaMenu_UpdateLevelStatus(lua_State *state){
    Script  L(state);
    int     chapter = L.GetInt(1, 1);
    int     level = L.GetInt(2, 1);
    int     status = L.GetInt(3, LevelDesc::STATUS_UNAVAILABLE);


#ifdef DEBUG
    switch (status){
        case 0: GEngine->Video->MapSel->UpdateMapStatus(chapter, level, LevelDesc::STATUS_UNAVAILABLE); break;
        case 1: GEngine->Video->MapSel->UpdateMapStatus(chapter, level, LevelDesc::STATUS_CURRENT);     break;
        case 2: GEngine->Video->MapSel->UpdateMapStatus(chapter, level, LevelDesc::STATUS_FINISHED);    break;
        case 3: GEngine->Video->MapSel->UpdateMapStatus(chapter, level, LevelDesc::STATUS_WRONGDIFF);   break;
        default:
            TRACE("luaMenu_UpdateLevelStatus(): ERROR-> invalid level status: %d\n", status);
            break;
    }
#else
    GEngine->Video->MapSel->UpdateMapStatus(chapter, level, (LevelDesc::LevelStatus)status);
#endif

    return 0;
}

static int luaMenu_SetLevelSketch(lua_State *state){
    Script          L(state);
    int             chapter = L.GetInt(1, 1);
    int             level = L.GetInt(2, 1);
    const char *    sketch = L.GetString(3, NULL);

    
    if (sketch == NULL)
        return 0;

    GEngine->Video->MapSel->SetMapSketch(chapter, level, sketch);

    return 0;
}

static int luaMenu_MapNextLevel( lua_State *state )
{
	Script L( state );

	GEngine->Video->MapSel->SelectNextLevel();

	return 0;
}

static int luaMenu_MapGetLevelCardCondition( lua_State *state )
{
	Script L( state );

	L.PushString( GEngine->Video->MapSel->GetLastLevelCondition() );

	return 1;
}


static int luaMenu_MapGetCurrLevelCardCondition( lua_State *state )
{
	Script L( state );

	L.PushString( GEngine->Video->MapSel->GetCurrLevelCondition() );

	return 1;
}


static int luaMenu_MapGetCurrLevelCardIndex( lua_State *state )
{
	Script L( state );

	L.PushInt( GEngine->Video->MapSel->GetCurrLevelCardIndex() );

	return 1;
}


#ifndef TRANSGAMING
static int luaMenu_MapGetCurrLevelName( lua_State *state )
{
	Script L( state );

	L.PushString( GEngine->Video->MapSel->GetCurrLevelName() );

	return 1;
}
#endif


static int luaMenu_MapGetCurrLevel( lua_State *state )
{
	Script L( state );

	if (GEngine->Video)
		L.PushInt( GEngine->Video->MapSel->GetCurrLevel() );
	else
		L.PushInt(0);

	return 1;
}


static int luaMenu_MapGetCurrChapter( lua_State *state )
{
	Script L( state );

	if (GEngine->Video)
		L.PushInt( GEngine->Video->MapSel->GetCurrChapter() );
	else
		L.PushInt(0);

	return 1;
}


static int luaMenu_MapSetCurrLevel( lua_State *state )
{
	Script L( state );

	if (GEngine->Video)
		GEngine->Video->MapSel->ForceCurrLevel( L.GetInt( 1, 1 ) - 1, L.GetInt( 2, 1 ) - 1 );

	return 0;
}


static int luaMenu_ActivateLoadingScreen( lua_State *state )
{
	Script L( state );

	if( GEngine->Video )
		GEngine->Video->PMenu->LoadScreen.Activate( L.GetBool( 1, true ), L.GetInt( 2, 0 ), L.GetString( 3, "" ), L.GetString( 4, "" ) );

	return 0;
}


#ifndef TRANSGAMING
static int luaMenu_SetProgressIcon( lua_State *state )
{
	Script L( state );

	if( GEngine->Video )
		GEngine->Video->PMenu->LoadScreen.SetIcon( L.GetInt( 1 ), L.GetInt( 2 ), L.GetInt( 3 ), L.GetString( 4 ) );

	return 0;
}
#endif


static int luaMenu_SetLoadingScreenOverall( lua_State *state )
{
	Script L( state );

	if( GEngine->Video )
		GEngine->Video->PMenu->LoadScreen.SetOverall( L.GetInt( 1 ), L.GetFloat( 2, 100.0f ) );

	return 0;
}


static int luaMenu_GetLoadingScreenOverall( lua_State *state )
{
	Script L( state );

	if( GEngine->Video )
		L.PushInt( GEngine->Video->PMenu->LoadScreen.GetOverall() );
	else
		L.PushInt( 0 );

	return 1;
}


static int luaMenu_LoadingProgress( lua_State *state )
{
	Script L( state );

	GEngine->LoadingTick( L.GetInt(1,1) );

	return 0;
}


static int luaMenu_SetWaitTime( lua_State *state )
{
	Script L( state );

	if( GEngine->Video )
		GEngine->Video->PMenu->Screen.SetWaitTime( L.GetFloat( 1, 0.0f ) );

	return 0;
}


static int luaMenu_ShowMouse( lua_State *state )
{
	Script L( state );

	if( GEngine->Video )
		GEngine->Video->PMenu->ShowMouse( L.GetBool( 1, true ) );

	return 0;
}


static int luaMenu_PauseSounds( lua_State *state )
{
	Script L( state );

	if( GEngine->Video )
		GEngine->Video->PMenu->PauseSounds();

	return 0;
}


static int luaMenu_ResumeSounds( lua_State *state )
{
	Script L( state );

	if( GEngine->Video )
		GEngine->Video->PMenu->ResumeSounds();

	return 0;
}


#ifdef TRANSGAMING
static int luaMenu_LaunchURL( lua_State *state )
{
	Script L( state );

	const char *url = L.GetString(1, NULL);
	if( url != NULL )
		ShellExecute( NULL, "open", url, NULL, NULL, SW_SHOWNORMAL );

	return 0;
}


static int luaMenu_PlaySound( lua_State *state )
{
	Script L( state );
	const char * name = L.GetString( 1 );
	bool loop = L.GetBool( 2, false );
	GEngine->Video->PMenu->PlayMenuSound( name, loop );
	return 0;
}
#else
static int luaMenu_LaunchURL( lua_State *state )
{
	Script L( state );

	String url = L.GetString(1,"");
	if( !url.IsNull() )
		ShellExecute( NULL, "open", url.AsChar(), NULL, NULL, SW_SHOWNORMAL );

	return 0;
}


static int luaMenu_PlaySound( lua_State *state )
{
	Script L( state );
	String name = L.GetString( 1 );
	bool loop = L.GetBool( 2, false );
	GEngine->Video->PMenu->PlayMenuSound( name.AsChar(), loop );
	return 0;
}
#endif


static int luaMenu_StopSound( lua_State *state )
{
	Script L( state );
	GEngine->Video->PMenu->StopSound();
	return 0;
}


static int luaMenu_ShowCredits( lua_State *state )
{
	Script L( state );
	GEngine->Video->PMenu->ShowCredits( L.GetBool( 1, false ) );
	return 0;
}


static int luaMenu_GetSelectedSGSlot( lua_State *state )
{
	Script L( state );
	MenuItemLoadSave *item = ( MenuItemLoadSave *)MENU_SCREEN.FindItem( L.GetString( 1, "SaveList" ) );
	if( item )
	{
		MenuItemList::MenuListItem *elem = item->GetSelected();
		char *name = ( char *)elem->name.AsChar();
		if( strcmp( name, "empty" ) == 0 )
		{
			L.PushString( NULL );
		}
		else
		{
			L.PushString( name );
		}
	}
	else
	{
		L.PushString( NULL );
	}

	return 1;
}


static int luaMenu_ClearList( lua_State *state )
{
	Script L( state );
	MenuItemList *item = ( MenuItemList *)MENU_SCREEN.FindItem( L.GetString( 1, "SaveList" ) );
	if( item )
	{
		item->RemoveAll();
	}

	return 0;
}


static int luaMenu_SetAllowSave( lua_State *state )
{
	Script L( state );
	MenuItemLoadSave *item = ( MenuItemLoadSave *)MENU_SCREEN.FindItem( "SaveList" );
	if( item )
	{
		item->AllowSave( L.GetBool( 1, true ) );
	}

	return 0;
}


static int luaMenu_SetRegistryBonus1( lua_State *state )
{
	Script L( state );
	bool val = L.GetBool( 1, false );
	GEngine->Registry.BonusPlayerModel = val;
	return 0;
}


static int luaMenu_SetRegistryBonus2( lua_State *state )
{
	Script L( state );
	bool val = L.GetBool( 1, false );
	GEngine->Registry.BonusCardsUse = val;
	return 0;
}


static int luaMenu_SetRegistryBonus3( lua_State *state )
{
	Script L( state );
	bool val = L.GetBool( 1, false );
	GEngine->Registry.BonusPlayerModel2 = val;
	return 0;
}


static int luaMenu_GetRegistryBonus1( lua_State *state )
{
	Script L( state );
	L.PushBool( GEngine->Registry.BonusPlayerModel );
	return 1;
}


static int luaMenu_GetRegistryBonus2( lua_State *state )
{
	Script L( state );
	L.PushBool( GEngine->Registry.BonusCardsUse );
	return 1;
}


static int luaMenu_GetRegistryBonus3( lua_State *state )
{
	Script L( state );
	L.PushBool( GEngine->Registry.BonusPlayerModel2 );
	return 1;
}


static int luaMenu_ResetRoomType( lua_State *state )
{
	Script L( state );
	if( GEngine->Video )
		GEngine->Video->PMenu->ResetRoomType();
	return 0;
}


static int luaMenu_GetTextColor( lua_State *state )
{
	Script L( state );
#ifdef TRANSGAMING
	const char *str = L.GetString( 1, "0" );
	int index = 0;
	sscanf( str, "%x", &index );
#else
	String str = L.GetString( 1, "0" );
	int index = 0;
	sscanf( str.AsChar(), "%x", &index );
#endif
	DWORD color = gTextColors[index];
	BYTE r = ( color >> 16 ) & 0xff;
	BYTE g = ( color >> 8 ) & 0xff;
	BYTE b = color & 0xff;
	L.PushInt( r );
	L.PushInt( g );
	L.PushInt( b );
	return 3;
}


//--------------------------------------------------------------------
//	PMENU library
//--------------------------------------------------------------------
static const luaL_reg p_menu_lib[] = {
	{ "Activate", luaMenu_Activate },
	{ "Active", luaMenu_IsActive },
	{ "Clear", luaMenu_Clear },
	{ "ClearScreen", luaMenu_ClearScreen },
	{ "AddTextButton", luaMenu_AddTextButton },
	{ "AddCheckbox", luaMenu_AddCheckbox },
	{ "AddTextButtonEx", luaMenu_AddTextButtonEx },
	{ "AddSlider", luaMenu_AddSlider },
	{ "AddKeyControl", luaMenu_AddKeyControl },
	{ "AddSimpleKeyConf", luaMenu_AddSimpleKeyConf },
	{ "AddScroller", luaMenu_AddScroller },
	{ "AddTextEdit", luaMenu_AddTextEdit },
	{ "AddSliderImage", luaMenu_AddSliderImage },
	{ "AddServerList", luaMenu_AddServerList },
	{ "AddPassword", luaMenu_AddPassword },
	{ "AddNumEdit", luaMenu_AddNumEdit },
	{ "AddList", luaMenu_AddList },
	{ "AddWeaponList", luaMenu_AddWeaponList },
	{ "AddMapTable", luaMenu_AddMapTable },
	{ "AddNumRange", luaMenu_AddNumRange },
	{ "AddLoadSave", luaMenu_AddLoadSave },
	{ "AddStaticText", luaMenu_AddStaticText },
	{ "AddImageButton", luaMenu_AddImageButton },
	{ "AddKeyList", luaMenu_AddKeyList },
	{ "AddBorder", luaMenu_AddBorder },
	{ "AddTabGroup", luaMenu_AddTabGroup },
	{ "AddPlayerModel", luaMenu_AddPlayerModel },
	{ "AddColorPicker", luaMenu_AddColorPicker },
	{ "AddCharPicker", luaMenu_AddCharPicker },
	{ "AddImageButtonEx", luaMenu_AddImageButtonEx },
#ifdef TRANSGAMING
    { "SetMouseToControl", luaMenu_SetMouseToControl },
    { "GetItemCenter", luaMenu_GetItemCenter },
    { "SetItemNavAction", luaMenu_SetItemNavAction },
#endif
	{ "SetItemText", luaMenu_SetItemText },
	{ "SetItemDesc", luaMenu_SetItemDesc },
	{ "SetItemPosition", luaMenu_SetItemPosition },
	{ "SetItemAction", luaMenu_SetItemAction },
	{ "SetScrollerHeight", luaMenu_SetScrollerHeight },
	{ "SetItemFonts", luaMenu_SetItemFonts },
	{ "SetItemFontsTex", luaMenu_SetItemFontsTex },
	{ "SetItemExitMovie", luaMenu_SetItemExitMovie },
	{ "SetItemColors", luaMenu_SetItemColors },
	{ "SetItemAlign", luaMenu_SetItemAlign },
	{ "SetItemWidth", luaMenu_SetItemWidth },
	{ "SetItemVisibility", luaMenu_SetItemVisibility },
	{ "SetItemApplyRequired", luaMenu_SetItemApplyRequired },
	{ "SetItemWarning", luaMenu_SetItemWarning },
	{ "SetButtonGlowAnim", luaMenu_SetButtonGlowAnim },
	{ "SetBorderSize", luaMenu_SetBorderSize },
	{ "SetItemSounds", luaMenu_SetItemSounds },
	{ "SetKeyItemIndex", luaMenu_SetKeyItemIndex },
	{ "SetSliderFullWidth", luaMenu_SetSliderFullWidth },
	{ "SetListMaxHeight", luaMenu_SetListMaxHeight },
	{ "SetListBorderWidth", luaMenu_SetListBorderWidth },
	{ "SetListSeparatorPos", luaMenu_SetListSeparatorPos },
	{ "GetListSeparatorPos", luaMenu_GetListSeparatorPos },

	/* Two identical functions for safedisk */
	{ "SetBorderScroller", luaMenu_SetBorderScroller },
#ifndef	TRANSGAMING_NO_SAFEDISC
	{ "SetScrollerForBorder", luaMenu_SetScrollerForBorder },
#endif

	{ "SetBorderColCount", luaMenu_SetBorderColCount },
	{ "SetBorderColumn", luaMenu_SetBorderColumn },
	{ "SetBorderHeader", luaMenu_SetBorderHeader },
	{ "SetStaticTextRect", luaMenu_SetStaticTextRect },
	{ "AddImageToSlider", luaMenu_AddImageToSlider },
	{ "AddItemToList", luaMenu_AddItemToList },
	{ "AddSaveGameToList", luaMenu_AddSaveGameToList },
	{ "DisableItem", luaMenu_DisableItem },
	{ "EnableItem", luaMenu_EnableItem },
	{ "SetCheckboxValue", luaMenu_SetCheckboxValue },
	{ "IsItemChecked", luaMenu_IsItemChecked },
	{ "IsSliderFloat", luaMenu_IsSliderFloat },
	{ "ClearList", luaMenu_ClearList },
	{ "MoveListItemUp", luaMenu_MoveListItemUp },
	{ "MoveListItemDown", luaMenu_MoveListItemDown },
	{ "GetListItems", luaMenu_GetListItems },
	{ "GetNumRangeValue", luaMenu_GetNumRangeValue },
	{ "GetSliderValue", luaMenu_GetSliderValue },
	{ "GetImageButtonExValue", luaMenu_GetImageButtonExValue },
	{ "GetTextEditValue", luaMenu_GetTextEditValue },
	{ "GetPrimaryKey", luaMenu_GetPrimaryKey },
	{ "GetAlternateKey", luaMenu_GetAlternateKey },
	{ "GetSimpleKey", luaMenu_GetSimpleKey },
	{ "RefreshServerList", luaMenu_RefreshServerList },
	{ "StopServerList", luaMenu_StopServerList },
	{ "ChangeTextButtonExValue", luaMenu_ChangeTextButtonExValue },
	{ "SetBackground", luaMenu_SetBackground },
	{ "SetMenuWidth", luaMenu_SetMenuWidth },
	{ "SetTopPosition", luaMenu_SetTopPosition },
	{ "SetMovieLoop", luaMenu_SetMovieLoop },
	{ "SetItemsFadeLength", luaMenu_SetItemsFadeLength },
	{ "SetItemsDrawShadow", luaMenu_SetItemsDrawShadow },
	{ "EnableItemBG", luaMenu_EnableItemBG },
	{ "SetShowItemsFrame", luaMenu_SetShowItemsFrame },
	{ "FillMapTable", luaMenu_FillMapTable },
	{ "AddMapToServer", luaMenu_AddMapToServer },
	{ "RemoveMapFromTable", luaMenu_RemoveMapFromTable },
	{ "RemoveAllMapsFromServer", luaMenu_RemoveAllMapsFromServer },
	{ "UpdateMapTable", luaMenu_UpdateMapTable },
	{ "GetSelectedMap", luaMenu_GetSelectedMap },
	{ "GetMapsOnServer", luaMenu_GetMapsOnServer },
	{ "AddImageToButtonEx", luaMenu_AddImageToButtonEx },
	{ "PlayMovie", luaMenu_PlayMovie },
#ifndef TRANSGAMING
    { "RunningInGame", luaMenu_RunningInGame },
#endif
	{ "ShowMenu", luaMenu_ShowMenu },
	{ "ReturnToGame", luaMenu_ReturnToGame },
	{ "GetCDKey", luaMenu_GetCDKey },
	{ "SaveCDKey", luaMenu_SaveCDKey },
	{ "AddServerToList", luaMenu_AddServerToList },
	{ "GetSelectedServerIP", luaMenu_GetSelectedServerIP },
	{ "GetSelectedServerPort", luaMenu_GetSelectedServerPort },
	{ "StartServer", luaMenu_StartServer },
	{ "JoinServer", luaMenu_JoinServer },
#ifndef TRANSGAMING
	{ "JoinPKTV", luaMenu_JoinPKTV },
	{ "PKTVRecordStart", luaMenu_PKTVRecordStart },
	{ "PKTVRecordStop", luaMenu_PKTVRecordStop },
	{ "PKTVPlay", luaMenu_PKTVPlay },
	{ "IsPKTVPlaying", luaMenu_PKTVPlaying },
#endif
	{ "SwitchToMenu", luaMenu_SwitchToMenu },

	/* Three identical functions for safedisk */
	{ "SwitchToMap", luaMenu_SwitchToMap },
#ifndef	TRANSGAMING_NO_SAFEDISC
	{ "SwitchToLevelSel", luaMenu_SwitchToLevelSel },
	{ "ActivateMap", luaMenu_ActivateMap },
#endif

	{ "SwitchToBoard", luaMenu_SwitchToBoard },
	{ "MapReset", luaMenu_MapReset },
	{ "AddLevelToMap", luaMenu_AddLevelToMap },
    { "UpdateLevelStatus", luaMenu_UpdateLevelStatus },
	{ "MapNextLevel", luaMenu_MapNextLevel },
	{ "MapGetLevelCardCondition", luaMenu_MapGetLevelCardCondition },
	{ "MapGetCurrLevelCardCondition", luaMenu_MapGetCurrLevelCardCondition },
#ifndef TRANSGAMING
	{ "MapGetCurrLevelName", luaMenu_MapGetCurrLevelName },
#endif
	{ "MapGetCurrLevelCardIndex", luaMenu_MapGetCurrLevelCardIndex },
	{ "MapGetCurrLevel", luaMenu_MapGetCurrLevel },
	{ "MapGetCurrChapter", luaMenu_MapGetCurrChapter },
	{ "MapSetCurrLevel", luaMenu_MapSetCurrLevel },
	{ "ActivateLoadingScreen", luaMenu_ActivateLoadingScreen },
#ifndef TRANSGAMING
	{ "SetProgressIcon", luaMenu_SetProgressIcon },
#endif
	{ "SetLoadingScreenOverall", luaMenu_SetLoadingScreenOverall },
	{ "GetLoadingScreenOverall", luaMenu_GetLoadingScreenOverall },
	{ "LoadingProgress", luaMenu_LoadingProgress },
	{ "SetWaitTime", luaMenu_SetWaitTime },
	{ "ShowMouse", luaMenu_ShowMouse },
	{ "PauseSounds", luaMenu_PauseSounds },
	{ "ResumeSounds", luaMenu_ResumeSounds },
	{ "LaunchURL", luaMenu_LaunchURL },
	{ "PlaySound", luaMenu_PlaySound },
	{ "StopSound", luaMenu_StopSound },
	{ "ShowCredits", luaMenu_ShowCredits },
	{ "GetSelectedSGSlot", luaMenu_GetSelectedSGSlot },
	{ "SetAllowSave", luaMenu_SetAllowSave },
	{ "SetRegistryBonus1", luaMenu_SetRegistryBonus1 },
	{ "SetRegistryBonus2", luaMenu_SetRegistryBonus2 },
	{ "SetRegistryBonus3", luaMenu_SetRegistryBonus3 },
	{ "GetRegistryBonus1", luaMenu_GetRegistryBonus1 },
	{ "GetRegistryBonus2", luaMenu_GetRegistryBonus2 },
	{ "GetRegistryBonus3", luaMenu_GetRegistryBonus3 },
	{ "ResetRoomType", luaMenu_ResetRoomType },
	{ "GetTextColor", luaMenu_GetTextColor },
	{ NULL, NULL }
};


void PainMenu_RegisterLUALib()
{
	GScript.RegisterLibrary("PMENU",p_menu_lib);
}
