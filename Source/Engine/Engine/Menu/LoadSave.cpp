#include "Engine.h"
#include "Render/HUD.h"
#include "Render/View.h"
#include "Engine/InputSystem.h"
#include "Render/Video.h"

#include "Engine/PainMenu.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;


static String gAutoPrefix = "";
static String gCheckptPrefix = "";
static String gQuickPrefix = "";

//--------------------------------------------------------------------
//	MenuItemLoadSave class
//--------------------------------------------------------------------
MenuItemLoadSave::MenuItemLoadSave() : MenuItemList()
{
	mType = EMenuItemLoadSave;
	mAllowSave = true;

	ScriptObject txt = GScript.Globals()["TXT"];
	gAutoPrefix = txt["Menu"]["AutoPrefix"].GetString();
	gCheckptPrefix = txt["Menu"]["CheckptPrefix"].GetString();
	gQuickPrefix = txt["Menu"]["QuickPrefix"].GetString();
}


MenuItemLoadSave::~MenuItemLoadSave()
{
	for( int i=0; i<mItems.Size(); i++ )
	{
        if( mItems[i]->obj ){
#ifdef TRANSGAMING
            // eric-> WARNING!!  This is horribly unsafe!  This should not even have to be done
            //   in the first place, but someone insisted on defining the menu objects as void *
            //   pointers instead of some proper base class with a virtual destructor.  The items
            //   always seem to be dealt with as SaveGame objects and are added to the list as
            //   such, so delete them that way as well.
			delete (reinterpret_cast<SaveGame *>(mItems[i]->obj));
#else
			delete mItems[i]->obj;
#endif
        }
	}
}


void MenuItemLoadSave::DrawElem( MenuItemList::MenuListItem *elem, int x, int y, bool header )
{
	CalcSize();
	SetUnderMouse( false );
	CheckIfUnderMouse();

	SaveGame *save = ( SaveGame *)elem->obj;

	HUD *hud = GEngine->Video->Hud;

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	float scale = screenWidth / 1024.0f;

	int x1, x2, x3, x4;
	if( header )
		x1 = (int)(x + ( 380.0f * scale - hud->GetTextWidth( save->name.AsChar() ) ) / 2);
	else
		x1 = x;

	x2 = (int)(x + 380.0f * scale + ( 130.0f * scale - hud->GetTextWidth( save->elapsed.AsChar() ) ) / 2);
	x3 = (int)(x + 506.0f * scale + ( 210.0f * scale - hud->GetTextWidth( save->date.AsChar() ) ) / 2);
	x4 = (int)(x + 716.0f * scale + ( 110.0f * scale - hud->GetTextWidth( save->difficulty.AsChar() ) ) / 2);

	// Shadow
	if( mDrawShadow )
	{
		hud->Print( x1 + 1, y + 1, save->name.AsChar(), CalcTransColor( 0x50000000 ) );
		hud->Print( x2 + 1, y + 1, save->elapsed.AsChar(), CalcTransColor( 0x50000000 ) );
		hud->Print( x3 + 1, y + 1, save->date.AsChar(), CalcTransColor( 0x50000000 ) );
		hud->Print( x4 + 1, y + 1, save->difficulty.AsChar(), CalcTransColor( 0x50000000 ) );
	}

	bool underMouse = false;
	if( mUnderMouse )
	{
		POINT mousePos = GEngine->InputSys->GetMousePosition();

		if( y < mousePos.y && ( y + hud->GetTextHeight() ) > mousePos.y )
			underMouse = true;
	}

	bool selected = false;
	if( mSelected != -1 )
	{
		if( mItems[mSelected] == elem )
			selected = true;
	}

	// Normal
	DWORD color = 0;
	if( !mDisabled && !selected && !header )
		color = CalcTransColor( underMouse ? mTextColorUnderMouse : mTextColor );
	else if( !header )
		color = CalcTransColor( mDisabledColor );
	else
		color = CalcTransColor( mTextColor );

	hud->Print( x1, y, save->name.AsChar(), color );
	hud->Print( x2, y, save->elapsed.AsChar(), color );
	hud->Print( x3, y, save->date.AsChar(), color );
	hud->Print( x4, y, save->difficulty.AsChar(), color );

#ifdef TRANSGAMING
    const int   BUFSIZE = 256;
	char        prefix[BUFSIZE];
#else
	char prefix[255];
#endif
	int www = hud->GetTextWidth( "[" );


	if( strstr( save->name.AsChar(), gCheckptPrefix.AsChar() ) )
	{
#ifdef TRANSGAMING
		strncpy( prefix, gCheckptPrefix.AsChar() + 1, BUFSIZE);
		prefix[strlen(prefix)-1] = 0;
		hud->Print( x1 + www, y, prefix, 0xff1010e5 );
#else
		strcpy( prefix, gCheckptPrefix.AsChar() + 1 );
		prefix[strlen(prefix)-1] = 0;
		hud->Print( x1 + www, y, prefix, 0xffe51010 );
#endif
	}
	else if( strstr( save->name.AsChar(), gAutoPrefix.AsChar() ) )
	{
#ifdef TRANSGAMING
		strncpy( prefix, gAutoPrefix.AsChar() + 1, BUFSIZE);
#else
		strcpy( prefix, gAutoPrefix.AsChar() + 1 );
#endif
        prefix[strlen(prefix)-1] = 0;
		hud->Print( x1 + www, y, prefix, 0xffb8b8b8 );
    }
	else if( strstr( save->name.AsChar(), gQuickPrefix.AsChar() ) )
	{
#ifdef TRANSGAMING
		strncpy( prefix, gQuickPrefix.AsChar() + 1, BUFSIZE);
        prefix[strlen(prefix)-1] = 0;
		hud->Print( x1 + www, y, prefix, 0xff1010e5 );
#else
		strcpy( prefix, gQuickPrefix.AsChar() + 1 );
        prefix[strlen(prefix)-1] = 0;
		hud->Print( x1 + www, y, prefix, 0xffe51010 );
#endif
	}
}


void MenuItemLoadSave::CalcSize()
{
	MenuItemList::CalcSize();

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	mWidth = (int)(824 * ( screenWidth / 1024.0f ));

	if( mBorder )
	{
		mBorder->SetSize( 850, mMaxHeight + 40 );
		mBorder->SetColumnsCount( 4 );
		mBorder->SetColumnWidth( 0, 400 );
		mBorder->SetColumnWidth( 1, 126 );
		mBorder->SetColumnWidth( 2, 200 );
		mBorder->SetColumnWidth( 3, 140 );
		mBorder->SetHeader( 40 );
	}

	MenuItem *del = GEngine->Video->PMenu->Screen.FindItem( "DeleteButton" );
	MenuItem *save = GEngine->Video->PMenu->Screen.FindItem( "SaveButton" );
	MenuItem *load = GEngine->Video->PMenu->Screen.FindItem( "LoadButton" );

	if( mSelected != -1 )
	{
		if( del ) del->Disable();
		if( load ) load->Disable();
		if( save ) save->Disable();

		if( mAllowSave )
			if( save ) save->Enable();
		if( mItems[mSelected]->name != "empty" )
		{
			if( del ) del->Enable();
			if( load ) load->Enable();
		}
	}
	else
	{
		if( del ) del->Disable();
		if( save ) save->Disable();
		if( load ) load->Disable();
	}
}
