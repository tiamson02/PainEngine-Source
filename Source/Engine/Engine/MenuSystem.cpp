#error This file is obselete
#include "Engine.h"

// Inclusion of GameSpy C Engine SDK
//#include "Net/GameSpy/cengine/goaceng.h"

static unsigned char TextInputConversion( int vkcode )
{
	bool shift1 = (GEngine->InputSys->GetKeyStatus(VK_SHIFT)!=0);
	bool shift2 = (GEngine->InputSys->GetKeyStatus(VK_LSHIFT)!=0);
	bool shift3 = (GEngine->InputSys->GetKeyStatus(VK_RSHIFT)!=0);
	bool shift = shift1 || shift2 || shift3;

	switch( vkcode )
	{
	case VK_BACK:	return 255;
	case VK_DELETE:	return 255;
	case 48:		return shift?')':'0';
	case 49:		return shift?'!':'1';
	case 50:		return shift?'@':'2';
	case 51:		return shift?'#':'3';
	case 52:		return shift?'$':'4';
	case 53:		return shift?'%':'5';
	case 54:		return shift?'^':'6';
	case 55:		return shift?'&':'7';
	case 56:		return shift?'*':'8';
	case 57:		return shift?'(':'9';
	case 186:		return shift?':':';';
	case 187:		return shift?'+':'=';
	case 188:		return shift?'<':',';
	case 189:		return shift?'_':'-';
	case 190:		return shift?'>':'.';
	case 191:		return shift?'?':'/';
	case 219:		return shift?'{':'[';
	case 220:		return shift?'|':'\\';
	case 221:		return shift?'}':']';
	case 222:		return shift?'\"':'\'';
	default:		return (unsigned char)(vkcode&0xFF);
	}
}

static bool MenuEventHandlerButton( MenuWindow* window, int event, void* data )
{
	if( event == EMenuKeyDown )
	{
		if( *(char*)data == 13 )	// Return
		{
			if( GEngine->Audio && !( window->MouseClickedSoundName.IsNull() ) )
				GEngine->Audio->Play2DSound( window->MouseClickedSoundName.AsChar() );
			GScript.DoStringA( "Menu_MenuEvent( '%s', 'Clicked' )", window->GetName() );
			return GEngine->Video->Menu.GetLUAReply();
		}
	}
	else if( event == EMenuMouseDown )
	{
		Vector mouse = *(Vector*)data;
		if( floor(mouse.Z+0.5f) == EMouseLeftButton )
		{
			if( GEngine->Audio && !( window->MouseClickedSoundName.IsNull() ) )
				GEngine->Audio->Play2DSound( window->MouseClickedSoundName.AsChar() );
			GScript.DoStringA( "Menu_MenuEvent( '%s', 'Clicked' )", window->GetName() );
			return GEngine->Video->Menu.GetLUAReply();
		}
	}
	else if( event == EMenuMouseLeft )
	{
		window->StopOnMouseSound();
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'MouseLeft' )", window->GetName() );
		return true;
	}
	else if( event == EMenuMouseEntered )
	{
		window->PlayOnMouseSound();
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'MouseEntered' )", window->GetName() );
		return true;
	}
	return false;
}

static bool MenuEventHandlerCheckBox( MenuWindow* window, int event, void* data )
{
	MenuCheckBox* cb = (MenuCheckBox*)window;
	if( event == EMenuKeyDown )
	{
		if( *(char*)data == 13 )	// Return
			cb->Checked = !cb->Checked;
	}
	else if( event == EMenuMouseDown )
	{
		Vector mouse = *(Vector*)data;
		if( floor(mouse.Z+0.5f) == EMouseLeftButton )
			cb->Checked = !cb->Checked;
	}
	else if( event == EMenuMouseLeft )
	{
		window->StopOnMouseSound();
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'MouseLeft' )", window->GetName() );
		return true;
	}
	else if( event == EMenuMouseEntered )
	{
		window->PlayOnMouseSound();
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'MouseEntered' )", window->GetName() );
		return true;
	}
	return false;
}

static bool MenuEventHandlerEditField( MenuWindow* window, int event, void* data )
{
	MenuEditField* ef = (MenuEditField*)window;
	if( ( event == EMenuKeyDown ) && (!ef->ReadOnly) )
	{
		unsigned char c = TextInputConversion(*(int*)data);
		if( c == 0 )
			return true;	// ignore key
		if( c == 13 )	// Return
		{
			GScript.DoStringA( "Menu_MenuEvent( '%s', 'TextEntered' )", window->GetName() );
			GEngine->Video->Menu.SetFocus( 0 );
			return GEngine->Video->Menu.GetLUAReply();
		}
		if( ( ( c == 255 ) ) && ( window->Text.Len() > ef->MinimalNumberOfCharacters ) )
		{
			window->Text = window->Text.SubStr( window->Text.Len()-1 );
		}
		else if( window->Text.Len() >= ef->MaximalNumberOfCharacters )
			return true;
		else if( ef->NumberField )
		{
			if( ( ( c >= '0' ) && ( c <= '9' ) ) || ( c == '-') || ( c == '.' ) )
			{
				String s;
				s = s.Sprintf( "%c", c );
				window->Text += s;
			}
			return true;
		}
		else if( ef->IPField )
		{
			if( ( ( c >= '0' ) && ( c <= '9' ) ) || ( c == '.') || ( c == ':' ) )
			{
				String s;
				s = s.Sprintf( "%c", c );
				window->Text += s;
			}
			return true;
		}
		else if( ( c >= ' ' ) && ( c <= 'z' ) )
		{
			if( ( c >= 'A' ) && ( c <= 'Z' ) )
			{
				if( !GEngine->InputSys->GetKeyStatus(VK_SHIFT) )
					c += 32;
			}
				
			String s;
			s = s.Sprintf( "%c", c );
			window->Text += s;
		}
		return true;
	}
	else if( event == EMenuMouseDown )	// set focus on LMB click
	{
		Vector mouse = *(Vector*)data;
		if( floor(mouse.Z+0.5f) == EMouseLeftButton )
		{
			GEngine->Video->Menu.SetFocus( window );
			return true;
		}
	}
	else if( event == EMenuMouseLeft )
	{
		window->StopOnMouseSound();
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'MouseLeft' )", window->GetName() );
		return true;
	}
	else if( event == EMenuMouseEntered )
	{
		window->PlayOnMouseSound();
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'MouseEntered' )", window->GetName() );
		return true;
	}

	return false;
}

static bool MenuEventHandlerListControl( MenuWindow* window, int event, void* data )
{
	MenuListControl* lc = (MenuListControl*)window;
	GScript.DoStringA( "Menu_MenuEvent( '%s', 'AnyEvent' )", window->GetName() );

	if( event == EMenuKeyDown )
	{
		unsigned char c = *(unsigned char*)data;
		switch( c )
		{
		case VK_HOME:		lc->Home(); lc->mSelectedRecord = lc->NumberOfRecords()?0:-1;						break;
		case VK_END:		lc->End(); lc->mSelectedRecord = lc->NumberOfRecords()-1;							break;
		case VK_UP:			lc->PreviousLine();	 lc->mSelectedRecord--; lc->ValidateSelected();					break;
		case VK_DOWN:		lc->NextLine(); lc->mSelectedRecord++; lc->ValidateSelected();						break;
		case VK_PRIOR:		lc->PreviousPage(); lc->mSelectedRecord-=lc->mLinesPerPage; lc->ValidateSelected();	break;	// Page up
		case VK_NEXT:		lc->NextPage(); lc->mSelectedRecord+=lc->mLinesPerPage; lc->ValidateSelected();		break;	// Page down
		case 13:		// Return
			GScript.DoStringA( "Menu_MenuEvent( '%s', 'Changed' )", window->GetName() );
			GScript.DoStringA( "Menu_MenuEvent( '%s', 'ChosenItem' )", window->GetName() );
			return GEngine->Video->Menu.GetLUAReply();
		default:
			return false;
		}
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'Changed' )", window->GetName() );
	}
	else if( event == EMenuKeyDownRepeated )
	{
		unsigned char c = *(unsigned char*)data;
		switch( c )
		{
		case VK_HOME:		lc->Home(); lc->mSelectedRecord = lc->NumberOfRecords()?0:-1;						break;
		case VK_END:		lc->End(); lc->mSelectedRecord = lc->NumberOfRecords()-1;							break;
		case VK_UP:			lc->PreviousLine();	 lc->mSelectedRecord--; lc->ValidateSelected();					break;
		case VK_DOWN:		lc->NextLine(); lc->mSelectedRecord++; lc->ValidateSelected();						break;
		case VK_PRIOR:		lc->PreviousPage(); lc->mSelectedRecord-=lc->mLinesPerPage; lc->ValidateSelected();	break;	// Page up
		case VK_NEXT:		lc->NextPage(); lc->mSelectedRecord+=lc->mLinesPerPage; lc->ValidateSelected();		break;	// Page down
		default:
			return false;
		}
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'Changed' )", window->GetName() );
	}
	else if( event == EMenuMouseDown )	// set focus on LMB click
	{
		Vector mouse = *(Vector*)data;
		if( !(window->Flags & MenuWindow::HasFocus ) && ( floor(mouse.Z+0.5f) == EMouseLeftButton ) )
			GEngine->Video->Menu.SetFocus( window );

		if( lc->NoSelect )
		{
			lc->SetSelected( -1 );
			return false;
		}
		else
		{
			lc->SetSelected( lc->mDisplayFromLine + (int)floor(mouse.Y*1.02f*lc->mLinesPerPage ) );
			GScript.DoStringA( "Menu_MenuEvent( '%s', 'ChosenItem' )", window->GetName() );
		}
		return true;
	}
	else if( event == EMenuMouseLeft )
	{
		window->StopOnMouseSound();
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'MouseLeft' )", window->GetName() );
		return true;
	}
	else if( event == EMenuMouseEntered )
	{
		window->PlayOnMouseSound();
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'MouseEntered' )", window->GetName() );
		return true;
	}

	return false;
}

static bool MenuEventHandlerRedefineKeysControl( MenuWindow* window, int event, void* data )
{
	MenuListControl* lc = (MenuListControl*)window;
	InputSystem* inputSys = GEngine->InputSys;
	Assert( inputSys );

	if( event == EMenuKeyDown )
	{
		if( lc->GetSelected() != -1 )
		{
			unsigned char c = *(unsigned char*)data;
			// Check if this key has an action already - if it has, clear all fields with this key
			int oldAction = inputSys->GetKeyAction(c);
			if( oldAction != 0 )
			{
				MenuListControl* primary = ((MenuRedefineKeysControl*)lc->mParentWindow)->mPrimaryList;
				MenuListControl* altern = ((MenuRedefineKeysControl*)lc->mParentWindow)->mAlternativeList;
				int primaryKey;
				do {
					primaryKey = primary->GetRecordByValue( inputSys->GetKeyName(c) );
					if( primaryKey != -1 )
						primary->SetRecord( primaryKey, "None" );
				} while( primaryKey != -1 );

				int alternKey;
				do {
					alternKey = altern->GetRecordByValue( inputSys->GetKeyName(c) );
					if( alternKey != -1 )
						altern->SetRecord( alternKey, "None" );
				} while( alternKey != -1 );
			}

			const char *keyText = inputSys->GetKeyName(c);
			lc->SetRecord( lc->GetSelected(), keyText );
			int action = lc->GetSelected()+1;
			inputSys->BindKey( action, c );
			MenuRedefineKeysControl* rkc = (MenuRedefineKeysControl*)lc->mParentWindow;
			rkc->SaveKeysToLUA();
			lc->SetSelected(-1);
			return true;
		}
		return false;
	}
	else if( event == EMenuMouseDown )	// set focus on LMB click
	{
		Vector mouse = *(Vector*)data;
		if( !(window->Flags & MenuWindow::HasFocus ) && ( floor(mouse.Z+0.5f) == EMouseLeftButton ) )
			GEngine->Video->Menu.SetFocus( window );

		if( lc->NoSelect )
			lc->SetSelected( -1 );
		else
		{
			if( lc->GetSelected() == lc->mDisplayFromLine + floor( mouse.Y*1.06f*lc->mLinesPerPage ) )
			{
				// Mouse key is being defined for current control
				int mouseKey = 0;
				switch( (int)floor(mouse.Z) )
				{
				default:				return true;
				case EMouseLeftButton:		mouseKey = EMouseKeyLeft; break;
				case EMouseRightButton:		mouseKey = EMouseKeyRight; break;
				case EMouseMiddleButton:	mouseKey = EMouseKeyMiddle; break;
				}

				// Check if this key has an action already - if it has, clear all fields with this key
				int oldAction = inputSys->GetKeyAction(mouseKey);
				if( oldAction != 0 )
				{
					MenuListControl* primary = ((MenuRedefineKeysControl*)lc->mParentWindow)->mPrimaryList;
					MenuListControl* altern = ((MenuRedefineKeysControl*)lc->mParentWindow)->mAlternativeList;
					int primaryKey;
					do {
						primaryKey = primary->GetRecordByValue( inputSys->GetKeyName(mouseKey) );
						if( primaryKey != -1 )
							primary->SetRecord( primaryKey, "None" );
					} while( primaryKey != -1 );

					int alternKey;
					do {
						alternKey = altern->GetRecordByValue( inputSys->GetKeyName(mouseKey) );
						if( alternKey != -1 )
							altern->SetRecord( alternKey, "None" );
					} while( alternKey != -1 );
				}

				const char *keyText = inputSys->GetKeyName(mouseKey);
				lc->SetRecord( lc->GetSelected(), keyText );
				int action = lc->GetSelected()+1;
				inputSys->BindKey( action, mouseKey );
				MenuRedefineKeysControl* rkc = (MenuRedefineKeysControl*)lc->mParentWindow;
				rkc->SaveKeysToLUA();
				lc->SetSelected(-1);
				return true;
			}
			else
				lc->SetSelected( lc->mDisplayFromLine + (int)floor( mouse.Y*1.06f*lc->mLinesPerPage ) );
		}
		return true;
	}
	else if( event == EMenuMouseLeft )
	{
		window->StopOnMouseSound();
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'MouseLeft' )", window->GetName() );
		return true;
	}
	else if( event == EMenuMouseEntered )
	{
		window->PlayOnMouseSound();
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'MouseEntered' )", window->GetName() );
		return true;
	}

	return false;
}

static bool MenuEventHandlerGiveClickToParent( MenuWindow* window, int event, void* data )
{
	if( ( event == EMenuMouseDown ) && window->mParentWindow )
	{
		if( ( window->mParentWindow->Type() == MenuWindow::ScrollBar ) &&
			(((MenuScrollBar*)window->mParentWindow)->mBar == window ) )
			return false;	// scroll bar bar lets the event get to main scroll bar window
		return window->mParentWindow->OtherEvent( EMenuClicked, 0, (void*)window );
	}
	if( event == EMenuMouseEntered )
		return window->mParentWindow->OtherEvent( EMenuMouseEntered, 0, 0 );
	if( event == EMenuMouseLeft )
		return window->mParentWindow->OtherEvent( EMenuMouseLeft, 0, 0 );
	return false;
}

static bool MenuEventHandlerScrollBar( MenuWindow* window, int event, void* data )
{
	MenuScrollBar* sb = (MenuScrollBar*)window;
	if( event == EMenuClicked )
	{
		MenuButton* button = (MenuButton*)data;
		if( button == sb->mButton0 )
		{
			sb->Position -= sb->PercentCovered/(1.0000001f-sb->PercentCovered);
			if( sb->Position < 0.0f ) sb->Position = 0.0f;
			GScript.DoStringA( "Menu_MenuEvent( '%s', 'Changed' )", window->GetName() );
			return true;
		}
		else if( button == sb->mButton1 )
		{
			sb->Position += sb->PercentCovered/(1.0000001f-sb->PercentCovered);
			if( sb->Position > 1.0f ) sb->Position = 1.0f;
			GScript.DoStringA( "Menu_MenuEvent( '%s', 'Changed' )", window->GetName() );
			return true;
		}
		return true;	// bar clicked, no reaction is correct
	}
	else if( event == EMenuMouseDown )
	{
		Vector mouse = *(Vector*)data;
		if( floor(mouse.Z+0.5f) == EMouseLeftButton )	// someone clicked in the background area above or below bar
		{
			sb->mMouseCaught = true;
			Vector pos = sb->mBar->GetPos();
			if( pos.Y == 0.0f )	// horizontal
				sb->Position = (mouse.X - sb->mButton0->GetSize().X)/(1.0f - sb->mButton0->GetSize().X - sb->mButton1->GetSize().X);
			else					// vertical
				sb->Position = (mouse.Y - sb->mButton0->GetSize().Y)/(1.0f - sb->mButton0->GetSize().Y - sb->mButton1->GetSize().Y);
			if( sb->Position < 0.0f ) sb->Position = 0.0f;
			if( sb->Position > 1.0f ) sb->Position = 1.0f;
			GScript.DoStringA( "Menu_MenuEvent( '%s', 'Changed' )", window->GetName() );
			return true;
		}
	}
	else if( event == EMenuMouseLeft )
	{
		window->StopOnMouseSound();
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'MouseLeft' )", window->GetName() );
		return true;
	}
	else if( event == EMenuMouseEntered )
	{
		window->PlayOnMouseSound();
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'MouseEntered' )", window->GetName() );
		return true;
	}
	return false;
}

// ********************************************************************
//						MENU WINDOW METHODS
// ********************************************************************

MenuWindow::MenuWindow() : mWindowName(""), mWindowIdent(-1), mParentWindow(0), X(0.0f), Y(0.0f), W(0.0f), H(0.0f)
		,Color(DWORD(-1)),TextColor(DWORD(-1)), mTexture(0), Text(""), U1(0.0f), V1(0.0f), U2(1.0f), V2(1.0f)
		,mTextX(0.02f), mTextY(0.02f), Flags(0), mEventHandler(0), mFontName("defaultMenu"), TextJustification(0)
		, mUnderMouse(false), ScaledText( false ), XAspected( false ), TextColorUnderMouse(DWORD(-1))
		, ColorUnderMouse(DWORD(-1)), Ticked( false ), mTextureName(""), TextScale(1.0f), mTextureFlags(DWORD(0))
		, MouseOnSoundName(""), mMouseOnSound(0), MouseClickedSoundName(""), MultiresolutionText(false)
{
	GEngine->Video->Menu.mCompleteWindowSet.Add( this );
	mType = SimpleWindow;
	mLastTick = GEngine->SysDriver.GetCurrentTime();
}

MenuWindow::~MenuWindow( void )
{
	if( GEngine->Audio )
	{
		if( mMouseOnSound )
		{
			GEngine->Audio->Delete2DSound( mMouseOnSound );
			mMouseOnSound = 0;
		}
	}

	if( mTexture && GEngine->Video->MaterialSys )
	{
		GEngine->Video->MaterialSys->ReleaseTexture( mTexture );
		mTexture = 0;
	}

	for( int i = 0; i < mChildWindows.Size(); ++i )
	{
		mChildWindows[i]->mParentWindow = NULL;
		delete mChildWindows[i];
	}
	mChildWindows.Clear();

	if( mWindowIdent != -1 )	// is in menu system - remove it from there
		GEngine->Video->Menu.ForgetWindow( mWindowIdent );

	if( mParentWindow )
		mParentWindow->RemoveChildWindow( this );

	for( int i = 0; i < GEngine->Video->Menu.mCompleteWindowSet.Size(); ++i )
	{
		if( GEngine->Video->Menu.mCompleteWindowSet[i] == this )
		{
			GEngine->Video->Menu.mCompleteWindowSet.Remove( i );
			break;
		}
	}
}

void MenuWindow::Show( void )
{
	Flags |= Shown;
}

void MenuWindow::Hide( void )
{
	Flags &= ~Shown;
}

void MenuWindow::MakeModal( bool modal )
{
	if( modal )
		Flags |= Modal;
	else
		Flags &= ~Modal;
}

void MenuWindow::RefreshChildrenIndices( void )
{
	for( int i = 0; i < mChildWindows.Size(); ++i )
		mChildWindows[i]->SetParent( this );
}

bool MenuWindow::SetParent( MenuWindow* window )
{
	// If no parent window, or parent window not in menu system, remove this window and its children from menu system
	if( ( !window || ( window->mWindowIdent == -1 ) ) && ( mWindowIdent != -1 ) )
	{
		GEngine->Video->Menu.ForgetWindow( mWindowIdent );
		RefreshChildrenIndices();
	}

	if( mParentWindow )
		mParentWindow->RemoveChildWindow( this );

	mParentWindow = window;

	if( !window )
		return true;

	// If parent window in menu system, add this window and its children to menu system
	if( ( window->mWindowIdent != -1 ) && ( mWindowIdent == -1 ) )
	{
		GEngine->Video->Menu.RememberWindow( this );
		RefreshChildrenIndices();
	}
	window->AddChildWindow( this );

	return true;
}

bool MenuWindow::SetParent( int ident )
{
	MenuWindow* wnd = GEngine->Video->Menu.GetWindowByIdent( ident );
	if( !wnd )
		return false;
	return SetParent( wnd );
}

bool MenuWindow::SetParent( const char *windowName )
{
	MenuWindow* wnd = GEngine->Video->Menu.GetWindowByName( windowName );
	if( !wnd )
		return false;
	return SetParent( wnd );
}

void MenuWindow::AddChildWindow( MenuWindow* window )
{
	for( int i = 0; i < mChildWindows.Size(); ++i )
		if( mChildWindows[i] == window )
			return;

	mChildWindows.Add( window );
}

void MenuWindow::RemoveChildWindow( MenuWindow* window )
{
	int index = -1;
	for( int i = 0; i < mChildWindows.Size(); ++i )
	{
		if( mChildWindows[i] == window )
		{
			index = i;
			break;
		}
	}
		
	if( index != -1 )
	{
		mChildWindows[i]->mParentWindow = 0;
		mChildWindows.Remove(index);
	}
}

void MenuWindow::RemoveAllChildren( void )
{
	for( int i = 0; i < mChildWindows.Size(); ++i )
		mChildWindows[i]->mParentWindow = 0;
	mChildWindows.Clear();
}

void MenuWindow::SetTexture( const char *texName, DWORD texFlags )
{
	if( !strcmp(mTextureName, texName) ) return;
	if( !GEngine->Video->MaterialSys ) return;
	if( mTexture )
	{
		GEngine->Video->MaterialSys->ReleaseTexture( mTexture );
		mTexture = 0;
	}
	mTextureName = texName;
	mTextureFlags = texFlags;
	if( mTextureName.IsNull() ) Flags &= ~HasTexture;
}

// Generate tick if window is ticked ( every half a second )

#ifdef TRANSGAMING
# define MENU_TICK_PERIOD 0.2f
#else
# define MENU_TICK_PERIOD 0.2
#endif

void MenuWindow::GenerateTick( void )
{
	if( Ticked )
	{
#ifdef TRANSGAMING
		float  tick = GEngine->SysDriver.GetCurrentTime();
#else
		double tick = GEngine->SysDriver.GetCurrentTime();
#endif
		if( tick - mLastTick > MENU_TICK_PERIOD )
		{
			GScript.DoStringA( "Menu_MenuEvent( '%s', 'Tick' )", GetName() );
			mLastTick = tick;
		}
	}
}

void MenuWindow::Render( HUD& hud, float absX, float absY, float absW, float absH )
{
	if( !(Flags & Shown) )
		return;		// don't draw if not shown

	if( !mTexture && !( mTextureName.IsNull() ) )
	{
		mTexture = GEngine->Video->MaterialSys->CreateTexture( mTextureName.AsChar(), mTextureFlags | Texture::ETexNoMipMaps );
		Flags |= HasTexture;
	}

	float myAbsX = absX + (float)floor(X*absW);
	float myAbsY = absY + (float)floor(Y*absH);
	float myAbsW = (float)floor(absW*W);
	float myAbsH = (float)floor(absH*H);

	GenerateTick();

	// Render window texture if present

	float aspected = myAbsW;
	float moved = 0;
	if( XAspected )
	{
		if( mTexture )
		{
			aspected = myAbsH*mTexture->Width/mTexture->Height;
		}
		else
		{
			aspected = myAbsH;
		}
		moved = (myAbsW - aspected)/2;
	}

	if( ( Flags & HasRect ) )
        hud.DrawRect( myAbsX+moved-0.6f, myAbsY-0.6f, myAbsX+aspected+0.7f, myAbsY+myAbsH+0.7f, mUnderMouse?ColorUnderMouse:Color );

	if( Flags & HasTexture )
	{
		if( mTexture && !(mTexture->Flags & Texture::ETexInvalid ) )
		{
			if( GEngine->Video->Menu.mMapWindow == this )	// we're the map, render texture using special shader
			{
				HUD::QuadDef quadTemp;

				quadTemp.SetCoords((int)(myAbsX+moved),(int)myAbsY,(int)aspected,(int)myAbsH);
				quadTemp.SetTexCoords(U1,V1,U2,V2);
				quadTemp.Color = Color;
				quadTemp.ColorMap = mTexture;

				hud.DrawQuad(quadTemp);
			}
			else	// we're not the map, render normal texture
				hud.DrawQuad( mTexture, myAbsX+moved, myAbsY, aspected, myAbsH, mUnderMouse?ColorUnderMouse:Color, U1, V1, U2, V2 );
		}
		else
			hud.DrawQuad( 0, myAbsX+moved, myAbsY, aspected, myAbsH, mUnderMouse?ColorUnderMouse:Color, U1, V1, U2, V2 );
	}

	// Render map contents if we are the map
	if( GEngine->Video->Menu.mMapWindow == this )
	{
		// render player arrow
		Texture* tex = GEngine->Video->MaterialSys->CreateTexture( "HUD/mapaplayer", Texture::ETexNoMipMaps );

		RenderDevice* dev = GEngine->Video->RenderDev;
		TextureState st(0);
		Matrix mat;
		mat = TexturePanMatrix( -0.5f, -0.5f )*
			ZRotationMatrix( F_PI-GEngine->Video->Menu.mMapPlayerAngle )*
			TexturePanMatrix( 0.5f, 0.5f );
		st.Transform = TextureState::ETexTransform2;
		st.MatPtr = &mat;
		dev->SetTextureState(&st,1);

		hud.DrawQuad( tex, myAbsX+(myAbsW/2)-(myAbsH*0.05f),
			myAbsY+(myAbsH/2)-(myAbsH*0.05f), myAbsH*0.1f, myAbsH*0.1f );

		st.MatPtr = 0;
		st.Transform = TextureState::ETexTransformNone;
		dev->SetTextureState(&st,1);

		GEngine->Video->MaterialSys->ReleaseTexture( tex );

		// TO DO - render monsters and special points
	}

	if( ( Flags & HasText ) )
	{
		String fontname = MultiresolutionText?mFontName + GEngine->Video->Menu.mResolutionString:mFontName;
		hud.SetFont( fontname.AsChar() );
		TextJustify();
		int myAbsTextX = (int)(myAbsX + (float)floor(mTextX*myAbsW));
		int myAbsTextY = (int)(myAbsY + (float)floor(mTextY*myAbsH));
		if( MultiresolutionText )
			hud.Print( (int)(myAbsTextX+moved), myAbsTextY, Text.AsChar(), mUnderMouse?TextColorUnderMouse:TextColor );
		else
			hud.Print( (int)(myAbsTextX+moved), myAbsTextY, Text.AsChar(), mUnderMouse?TextColorUnderMouse:TextColor, ScaledText, TextScale,
				GEngine->Video->Menu.mRenderTargetX, GEngine->Video->Menu.mRenderTargetY );
	}

	// Render child windows
	for( int i = 0; i < mChildWindows.Size(); ++i )
		mChildWindows[i]->Render( hud, myAbsX, myAbsY, myAbsW, myAbsH );
}

void MenuWindow::TextJustify( void )
{
	float fw, fh;
	GetTextExtents( fw, fh );

	switch( TextJustification )
	{
	case ETextNotJustified:
	default:
		return;
	case ETextFromLeft:
		mTextX = 0.0f;
		mTextY = 0.5f-fh/2;
		break;
	case ETextFromRight:
		mTextX = 1.0f - fw;
		mTextY = 0.5f - fh/2;
		break;
	case ETextCentered:
		mTextX = 0.5f-fw/2;
		mTextY = 0.5f-fh/2;
		break;
	case ETextFromLeftOutside:
		mTextX = 0.0f-fw;
		mTextY = 0.5f-fh/2;
		break;
	}
}

void MenuWindow::SetPos( float x, float y )
{
	if( x < 0.0f ) x = 0.0f;
	if( y < 0.0f ) y = 0.0f;
	if( x > 1.0f ) x = 1.0f;
	if( y > 1.0f ) y = 1.0f;
	X = x; Y = y;
}

void MenuWindow::SetSize( float w, float h )
{
	if( w < 0.0f ) w = 0.0f;
	if( h < 0.0f ) h = 0.0f;
	if( w > 1.0f - X ) w = 1.0f - X;
	if( h > 1.0f - Y ) h = 1.0f - Y;
	W = w; H = h;
}

void MenuWindow::SetTextPos( float x, float y )
{
	mTextX = x; mTextY = y;
}

void MenuWindow::GetTextExtents( float& x, float& y )
{
	Assert( GEngine->Video->GetActiveView() );
	int width = GEngine->Video->Hud.GetTextWidth( Text.AsChar() );
	int height = GEngine->Video->Hud.GetTextHeight( Text.AsChar() );

	float fw, fh;

	if( ScaledText && !MultiresolutionText )
	{
		fw = ((float)width)/1266.0f;
		fh = ((float)height)/876.0f;
	}
	else
	{
		fw = ((float)width)/GEngine->Video->Menu.mRenderTargetX;
		fh = ((float)height)/GEngine->Video->Menu.mRenderTargetY;
	}

	float wFactor = 1.0f;
	float hFactor = 1.0f;
	MenuWindow* wnd = this;
	while( wnd->mParentWindow )
	{
		wFactor *= wnd->W;
		hFactor *= wnd->H;
		wnd = wnd->mParentWindow;
	}

	fw /= wFactor;
	fh /= hFactor;

	if( !MultiresolutionText )
	{
		fw *= TextScale;
		fh *= TextScale;
	}

	x = fw; y = fh;
}


void MenuWindow::ToFront( const char *windowName )
{
	int index = -1;
	for( int i = 0; i < mChildWindows.Size(); ++i )
	{
		if ( !strcmp(mChildWindows[i]->mWindowName.AsChar(), windowName) )
		{
			index = i;
			break;
		}
	}
	if( index == -1 )
		return;

	ToFront( index );
}

void MenuWindow::ToFront( MenuWindow* window )
{
	int index = -1;
	for( int i = 0; i < mChildWindows.Size(); ++i )
	{
		if( mChildWindows[i] == window )
		{
			index = i;
			break;
		}
	}
	if( index == -1 )
		return;

	ToFront( index );
}

void MenuWindow::ToFront( int index )
{
	Assert( ( index >= 0 ) && ( index < mChildWindows.Size() ) )

	MenuWindow* window = mChildWindows[index];
	mChildWindows.Remove( index );
	mChildWindows.Add( window );
}

bool MenuWindow::IsAtFront( void )
{
	return ( !mParentWindow || ( mParentWindow->mChildWindows[0] == this ) );
}

MenuWindow* MenuWindow::GetChildWindow( float& x, float& y )
{
	// For each child window
	for( int i = mChildWindows.Size()-1; i>= 0; --i )
	{
		Vector pos = mChildWindows[i]->GetPos();
		Vector size = mChildWindows[i]->GetSize();

		if( x < pos.X || y < pos.Y || x > pos.X+size.X || y > pos.Y+size.Y )
			continue;

		x = (x - pos.X)/size.X;
		y = (y - pos.Y)/size.Y;
		return mChildWindows[i];
	}
	return NULL;	// no child window found to encompass coords
}

bool MenuWindow::MouseEvent( float x, float y, int mouseButton, bool buttonDown )
{
	if( !(Flags & Shown) )
		return false;		// don't accept mouse calls if not shown

	// For each child window
	for( int i = mChildWindows.Size()-1; i>= 0; --i )
	{
		Vector pos = mChildWindows[i]->GetPos();
		Vector size = mChildWindows[i]->GetSize();

		if( x < pos.X || y < pos.Y || x > pos.X+size.X || y > pos.Y+size.Y )
			continue;

		float childX = (x - pos.X)/size.X;
		float childY = (y - pos.Y)/size.Y;
		if( mChildWindows[i]->MouseEvent( childX, childY, mouseButton, buttonDown ) )
			return true;	// Child window handled that event
	}

	Vector mousePos( x, y, (float)mouseButton );

	if( !(Flags & Modal) && CheckChildModal() )
		return true;	// one of the children is visible and modal - the event misses us

	if( mEventHandler )
		return (*mEventHandler)( this, buttonDown?EMenuMouseDown:EMenuMouseUp, &mousePos );

	if( Flags & Modal )
		return true;	// we're modal - our parents don't get event
	return false;	// no child window found to encompass coords and we don't handle it
}

bool MenuWindow::CheckChildModal( void )
{
	if( !(Flags & Shown) )
		return false;

	if( Flags & Modal )
		return true;

	// For each child window
	for( int i = mChildWindows.Size()-1; i>= 0; --i )
		if( mChildWindows[i]->CheckChildModal() )
			return true;
	return false;
}

MenuWindow* MenuWindow::GetWindowUnderMouse( float mouseX, float mouseY )
{
	if( !(Flags & Shown) )
		return 0;		// can't be under mouse if not shown

	// For each child window
	for( int i = mChildWindows.Size()-1; i>= 0; --i )
	{
		Vector pos = mChildWindows[i]->GetPos();
		Vector size = mChildWindows[i]->GetSize();

		if( mouseX < pos.X || mouseY < pos.Y || mouseX > pos.X+size.X || mouseY > pos.Y+size.Y )
			continue;

		float childX = (mouseX - pos.X)/size.X;
		float childY = (mouseY - pos.Y)/size.Y;
		MenuWindow* wnd = mChildWindows[i]->GetWindowUnderMouse( childX, childY );
		if( wnd )
			return wnd;		// Child window is under mouse
	}
	return this;
}

bool MenuWindow::KeyEvent( int key, int event )
{
	if( !(Flags & Modal) && CheckChildModal() )
		return false;

	// If we can be seen, and our event handler handles this event, return
	if( ( (Flags & Shown) && mEventHandler ) &&(*mEventHandler)( this, event, &key ) )
		return true;

	// Else give it to our parent to handle - if we have parent
	if( mParentWindow && !(Flags & Modal) )
		return mParentWindow->KeyEvent( key, event );
	return false;
}

bool MenuWindow::OtherEvent( MenuEvent eventType, int eventData1, void* eventData2 )
{
	// We don't propagate unknown events - give them to focus window, and that's all
	if( eventType == EMenuMouseLeft )
		mUnderMouse = false;
	if( eventType == EMenuMouseEntered )
		mUnderMouse = true;
	if( mEventHandler )
		return (*mEventHandler)( this, eventType, eventData2 );
	return false;
}

#ifdef TRANSGAMING
char *MenuWindow::CutToWidth(const HUD& hud, const char *str, int width, char *buf, size_t len) const{
    int textWidth = hud.GetTextWidth(str);
    

    strncpy(buf, str, len);


    //string already fits within the width => just copy and return it
    if (textWidth < width)
		return buf;



	int     delWidth = hud.GetTextWidth("...");
	int     i = 1;
    char    brake;


    //chop the string down until it fits in the specified width
	do {
        brake = buf[i];
        buf[i] = 0;

		textWidth = hud.GetTextWidth(buf);
        buf[i] = brake;
        i++;
	} while (textWidth + delWidth < width);

	i -= 2;
	if (i > 0){
        strncpy(&buf[i], "...", len - i - 1);
		return s;
	}
    else{
        buf[0] = 0;
        return buf;
    }
}
#else
String MenuWindow::CutToWidth( HUD& hud, String str, int width )
{
    int textWidth = hud.GetTextWidth( str.AsChar() );
	if( textWidth < width )
		return str;
	int delWidth = hud.GetTextWidth( "..." );
	int i = 1;
	do {
		String s = str.SubStr(i++);
		textWidth = hud.GetTextWidth( s.AsChar() );
	} while( textWidth+delWidth < width );
	i -= 2;
	if( i > 0 )
	{
		String s = str.SubStr(i);
		s += "...";
		return s;
	}
	else
		return String("");
}
#endif

void MenuWindow::GetRelativeMouseCoords( float& x, float& y )
{
	float x2, y2;

	if( mParentWindow )
		mParentWindow->GetRelativeMouseCoords( x2, y2 );
	else
		GEngine->Video->Menu.GetMenuMousePosition( x2, y2 );
	x = (x2 - X)/W;
	y = (y2 - Y)/H;
}

void MenuWindow::PlayOnMouseSound( void )
{
	if( GEngine->Audio && !mMouseOnSound && !( MouseOnSoundName.IsNull() ) )
	{
		mMouseOnSound = GEngine->Audio->Create2DSound( MouseOnSoundName.AsChar() );
		mMouseOnSound->SetLoopCount( 0 );
		mMouseOnSound->Play();
	}
}

void MenuWindow::StopOnMouseSound( void )
{
	if( GEngine->Audio && mMouseOnSound )
	{
		GEngine->Audio->Delete2DSound( mMouseOnSound );
		mMouseOnSound = 0;
	}
}

// ********************************************************************
//						MENU BUTTON METHODS
// ********************************************************************

MenuButton::MenuButton() : MenuWindow()
{
	mType = Button;
	SetEventHandler( MenuEventHandlerButton );
}

// ********************************************************************
//						MENU PROGRESS BAR METHODS
// ********************************************************************

MenuProgressBar::MenuProgressBar() : MenuWindow(), NonResizingTextures(false), CenteredPercentOver(true)
	, Percent( 0.0f ), mTexture1(0), mTexture2(0), mColor1(0xFFFFFFFF), mColor2(0xFF000000)
{
	mType = ProgressBar;
	SetEventHandler( 0 );
}

MenuProgressBar::~MenuProgressBar( void )
{
	if( GEngine->Video->MaterialSys )
	{
		if( mTexture1 )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mTexture1 );
			mTexture1 = 0;
		}
		if( mTexture2 )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mTexture2 );
			mTexture2 = 0;
		}
	}
}

void MenuProgressBar::Render( HUD& hud, float absX, float absY, float absW, float absH )
{
	if( !(Flags & Shown) )
		return;		// don't draw if not shown
/*
	// ugly hack - moving progress bar
	Percent += 0.0001f;
	if( Percent >= 1.0f )
		Percent = 0.0f;
*/
	if( !mTexture && !( mTextureName.IsNull() ) )
	{
		mTexture = GEngine->Video->MaterialSys->CreateTexture( mTextureName.AsChar(), mTextureFlags | Texture::ETexNoMipMaps );
		Flags |= HasTexture;
	}

	GenerateTick();

	if( Percent > 1.0f )
		Percent = 1.0f;
	if( Percent < 0.0f )
		Percent = 0.0f;

	float myAbsX = absX + (float)floor(X*absW);
	float myAbsY = absY + (float)floor(Y*absH);
	float myAbsW = (float)floor(absW*W);
	float myAbsH = (float)floor(absH*H);
	float myAbsPercent = Percent*myAbsW;

	if( ( Flags & HasRect ) )
        hud.DrawRect( myAbsX-0.6f, myAbsY-0.6f, myAbsX+myAbsW+0.7f, myAbsY+myAbsH+0.7f, Color );

	float u11 = 0.0f, u12 = 1.0f, v11 = 0.0f, v12 = 1.0f;	// over
	float u21 = 0.0f, u22 = 1.0f, v21 = 0.0f, v22 = 1.0f;	// under

	if( !NonResizingTextures )
		u12 = Percent;

	if( mTexture2  && !(mTexture2->Flags & Texture::ETexInvalid ) )
        hud.DrawQuad( mTexture2, myAbsX, myAbsY, myAbsW, myAbsH, mColor2, u21, v21, u22, v22 );
	if( mTexture1  && !(mTexture1->Flags & Texture::ETexInvalid ) )
        hud.DrawQuad( mTexture1, myAbsX, myAbsY, myAbsPercent, myAbsH, mColor1, u11, v11, u12, v12 );

	if( CenteredPercentOver )
	{
		Text = Text.Sprintf( "%.1f %%", Percent*100.0f );
		String fontname = MultiresolutionText?mFontName + GEngine->Video->Menu.GetResolutionString():mFontName;
		hud.SetFont( fontname.AsChar() );
		TextJustification = ETextCentered;
		TextJustify();
		int myAbsTextX = (int)(myAbsX + (float)floor(mTextX*myAbsW));
		int myAbsTextY = (int)(myAbsY + (float)floor(mTextY*myAbsH));
		if( MultiresolutionText )
			hud.Print( myAbsTextX, myAbsTextY, Text.AsChar(), mUnderMouse?TextColorUnderMouse:TextColor );
		else
			hud.Print( myAbsTextX, myAbsTextY, Text.AsChar(), mUnderMouse?TextColorUnderMouse:TextColor, ScaledText, TextScale,
				GEngine->Video->Menu.mRenderTargetX, GEngine->Video->Menu.mRenderTargetY );
	}
	else if( ( Flags & HasText ) )
	{
		String fontname = MultiresolutionText?mFontName + GEngine->Video->Menu.GetResolutionString():mFontName;
		hud.SetFont( fontname.AsChar() );
		TextJustify();
		int myAbsTextX = (int)(myAbsX + (float)floor(mTextX*myAbsW));
		int myAbsTextY = (int)(myAbsY + (float)floor(mTextY*myAbsH));
		if( MultiresolutionText )
			hud.Print( myAbsTextX, myAbsTextY, Text.AsChar(), mUnderMouse?TextColorUnderMouse:TextColor );
		else
			hud.Print( myAbsTextX, myAbsTextY, Text.AsChar(), mUnderMouse?TextColorUnderMouse:TextColor, ScaledText, TextScale,
				GEngine->Video->Menu.mRenderTargetX, GEngine->Video->Menu.mRenderTargetY);
	}

	// Render child windows
	for( int i = 0; i < mChildWindows.Size(); ++i )
		mChildWindows[i]->Render( hud, myAbsX, myAbsY, myAbsW, myAbsH );
}

void MenuProgressBar::SetTextureOver( const char *texName, DWORD color, DWORD texFlags )
{
	if( mTexture1 )
		GEngine->Video->MaterialSys->ReleaseTexture( mTexture1 );
	mTexture1 = GEngine->Video->MaterialSys->CreateTexture( texName, texFlags | Texture::ETexNoMipMaps );
	mColor1 = color;
}

void MenuProgressBar::SetTextureUnder( const char *texName, DWORD color, DWORD texFlags )
{
	if( mTexture2 )
		GEngine->Video->MaterialSys->ReleaseTexture( mTexture2 );
	mTexture2 = GEngine->Video->MaterialSys->CreateTexture( texName, texFlags | Texture::ETexNoMipMaps );
	mColor2 = color;
}

// ********************************************************************
//						MENU CHECK BOX METHODS
// ********************************************************************

MenuCheckBox::MenuCheckBox() : MenuWindow(), mTexture1(0), mTexture2(0)
	, mColor1(0xFFFFFFFF), mColor2(0xFF000000), Checked( false )
{
	mType = CheckBox;
	SetEventHandler( MenuEventHandlerCheckBox );

	SetTextureChecked( "HUD/ChkChecked" );
	SetTextureUnchecked( "HUD/ChkUnchecked" );
}

MenuCheckBox::~MenuCheckBox( void )
{
	if( GEngine->Video->MaterialSys )
	{
		if( mTexture1 )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mTexture1 );
			mTexture1 = 0;
		}
		if( mTexture2 )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mTexture2 );
			mTexture2 = 0;
		}
	}
}

void MenuCheckBox::Render( HUD& hud, float absX, float absY, float absW, float absH )
{
	if( !(Flags & Shown) )
		return;		// don't draw if not shown

	if( !mTexture && !( mTextureName.IsNull() ) )
	{
		mTexture = GEngine->Video->MaterialSys->CreateTexture( mTextureName.AsChar(), mTextureFlags | Texture::ETexNoMipMaps );
		Flags |= HasTexture;
	}

	GenerateTick();

	float myAbsX = absX + (float)floor(X*absW);
	float myAbsY = absY + (float)floor(Y*absH);
	float myAbsW = (float)floor(absW*W);
	float myAbsH = (float)floor(absH*H);

	if( ( Flags & HasRect ) )
        hud.DrawRect( myAbsX-0.6f, myAbsY-0.6f, myAbsX+myAbsW+0.7f, myAbsY+myAbsH+0.7f, Color );

	if( !Checked )
	{
		if( mTexture2  && !(mTexture2->Flags & Texture::ETexInvalid ) )
			hud.DrawQuad( mTexture2, myAbsX, myAbsY, myAbsH, myAbsH, mColor2 );
	}
	else
	{
		if( mTexture1  && !(mTexture1->Flags & Texture::ETexInvalid ) )
			hud.DrawQuad( mTexture1, myAbsX, myAbsY, myAbsH, myAbsH, mColor1 );
	}

	if( Flags & HasText )
	{
		String fontname = MultiresolutionText?mFontName + GEngine->Video->Menu.GetResolutionString():mFontName;
		hud.SetFont( fontname.AsChar() );
		TextJustification = ETextFromLeft;
		TextJustify();
		int myAbsTextX = (int)(myAbsX + (float)floor(mTextX*myAbsW));
		int myAbsTextY = (int)(myAbsY + (float)floor(mTextY*myAbsH));
		myAbsTextX += (int)myAbsH + 1;
		if( MultiresolutionText )
			hud.Print( myAbsTextX, myAbsTextY, Text.AsChar(), mUnderMouse?TextColorUnderMouse:TextColor );
		else
			hud.Print( myAbsTextX, myAbsTextY, Text.AsChar(), mUnderMouse?TextColorUnderMouse:TextColor, ScaledText, TextScale,
				GEngine->Video->Menu.mRenderTargetX, GEngine->Video->Menu.mRenderTargetY);
	}

	// Render child windows
	for( int i = 0; i < mChildWindows.Size(); ++i )
		mChildWindows[i]->Render( hud, myAbsX, myAbsY, myAbsW, myAbsH );
}

void MenuCheckBox::SetTextureChecked( const char *texName, DWORD color, DWORD texFlags )
{
	if( mTexture1 )
		GEngine->Video->MaterialSys->ReleaseTexture( mTexture1 );
	mTexture1 = GEngine->Video->MaterialSys->CreateTexture( texName, texFlags | Texture::ETexNoMipMaps );
	mColor1 = color;
}

void MenuCheckBox::SetTextureUnchecked( const char *texName, DWORD color, DWORD texFlags )
{
	if( mTexture2 )
		GEngine->Video->MaterialSys->ReleaseTexture( mTexture2 );
	mTexture2 = GEngine->Video->MaterialSys->CreateTexture( texName, texFlags | Texture::ETexNoMipMaps );
	mColor2 = color;
}

// ********************************************************************
//						MENU LIST CONTROL METHODS
// ********************************************************************

MenuListControl::MenuListControl() : MenuWindow(), mListCtrlInited( false )
	, mListCtrlColumns(0), mDelimiter('|'), mDisplayFromLine(0), mLinesPerPage(0), mSelectedRecord(-1)
	, SelectionColor( 0xFF00FF00 ), NoSelect( false ), KeepAtLast( false )
{
	mType = ListControl;
	SetEventHandler( MenuEventHandlerListControl );
	for( int i = 0; i < LISTCTRL_MAX_COLUMNS; ++i )
		mColumnWidths[i] = 0.0f;
}

MenuListControl::~MenuListControl( void )
{
	ClearControl();
}

void MenuListControl::Render( HUD& hud, float absX, float absY, float absW, float absH )
{
	if( !(Flags & Shown) )
		return;		// don't draw if not shown

	if( !mTexture && !( mTextureName.IsNull() ) )
	{
		mTexture = GEngine->Video->MaterialSys->CreateTexture( mTextureName.AsChar(), mTextureFlags | Texture::ETexNoMipMaps );
		Flags |= HasTexture;
	}

	GenerateTick();

	float myAbsX = absX + (float)floor(X*absW);
	float myAbsY = absY + (float)floor(Y*absH);
	float myAbsW = (float)floor(absW*W);
	float myAbsH = (float)floor(absH*H);

	// Render window texture if present

	if( ( Flags & HasRect ) )
        hud.DrawRect( myAbsX-1.6f, myAbsY-1.6f, myAbsX+myAbsW+0.7f, myAbsY+myAbsH+0.7f, Color );

	if( ( Flags & HasTexture ) && mTexture && !(mTexture->Flags & Texture::ETexInvalid ) )
        hud.DrawQuad( mTexture, myAbsX, myAbsY, myAbsW, myAbsH, Color, U1, V1, U2, V2 );

	// Find out current lines per page
	String fontname = MultiresolutionText?mFontName + GEngine->Video->Menu.GetResolutionString():mFontName;
	hud.SetFont( fontname.AsChar() );

	int newLinesPerPage = (int)floor( myAbsH/ (hud.GetTextHeight()+LISTCTRL_PIXELS_BETWEEN_LINES) );

	// The hack below is to update connected scrollbar after first refresh, if it's needed
	if( ( mLinesPerPage == 0 ) && ( newLinesPerPage != 0 ) )
	{
		mLinesPerPage = newLinesPerPage;
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'Changed' )", GetName() );
	}
	mLinesPerPage = newLinesPerPage;

	if( KeepAtLast )
		End();

	DWORD SelectedColor = (0xFFFFFFFF - TextColor) + 1;

	// Render lists
	int myAbsLinesY = (int)myAbsY;
	for( int i = mDisplayFromLine; (i < NumberOfRecords()) && (i < mDisplayFromLine+mLinesPerPage); ++i )
	{
		bool colorAsSelected = false;
		if( !NoSelect && ( Flags & HasFocus ) && i == mSelectedRecord )
			colorAsSelected = true;

		int myAbsLinesX = (int)myAbsX;
#ifdef TRANSGAMING
        const int BUFSIZE = 256;
        char buf[BUFSIZE];


        for (int j = 0; j < mListCtrlColumns; ++j){
			CutToWidth(hud, mColumns[j][i].AsChar(), (int)floor(myAbsW*mColumnWidths[j]), buf, BUFSIZE);
			hud.Print( myAbsLinesX+3, myAbsLinesY+LISTCTRL_PIXELS_BETWEEN_LINES/2, buf, colorAsSelected?SelectedColor:TextColor );
			myAbsLinesX += (int)floor(myAbsW*mColumnWidths[j]);
		}

#else
		for( int j = 0; j < mListCtrlColumns; ++j )
		{
			String s = CutToWidth( hud, mColumns[j][i], (int)floor(myAbsW*mColumnWidths[j]) );
			hud.Print( myAbsLinesX+3, myAbsLinesY+LISTCTRL_PIXELS_BETWEEN_LINES/2, s.AsChar(), colorAsSelected?SelectedColor:TextColor );
			myAbsLinesX += (int)floor(myAbsW*mColumnWidths[j]);
		}
#endif
		myAbsLinesY += ( hud.GetTextHeight()+ LISTCTRL_PIXELS_BETWEEN_LINES );
	}
/*
	// Render selected line
	if( !NoSelect && ( Flags & HasFocus ) &&
		( mSelectedRecord >= mDisplayFromLine ) &&
		( mSelectedRecord < mDisplayFromLine + mLinesPerPage ) )
	{
		int pixelsDown = (mSelectedRecord-mDisplayFromLine)*( hud.GetTextHeight() + LISTCTRL_PIXELS_BETWEEN_LINES );
        hud.DrawRect( myAbsX, myAbsY+pixelsDown-LISTCTRL_PIXELS_BETWEEN_LINES,
			myAbsX+myAbsW, myAbsY+pixelsDown+hud.GetTextHeight()+LISTCTRL_PIXELS_BETWEEN_LINES+0.8f, SelectionColor );
	}
*/
	// Render child windows
	for( int i = 0; i < mChildWindows.Size(); ++i )
		mChildWindows[i]->Render( hud, myAbsX, myAbsY, myAbsW, myAbsH );
}

void MenuListControl::ClearControl( void )
{
	if( mListCtrlInited )
	{
		DeleteAllRecords();
		mListCtrlColumns = 0;
		for( int i = 0; i < LISTCTRL_MAX_COLUMNS; ++i )
			mColumnWidths[i] = 0.0f;
	}
	mListCtrlInited = false;
	mSelectedRecord = -1;
}

void MenuListControl::SetupControl( int numberOfColumns, const char *columnWidths, char delimiter )
{
	if( mListCtrlInited )
		ClearControl();

	Assert( numberOfColumns > 0 );
	mListCtrlColumns = numberOfColumns;
	mDelimiter = delimiter;

	DynamicArray<String> tokens;
	String delims;
	delims = delims.Sprintf( "%c", mDelimiter );
	String::Tokenize( columnWidths, delims.AsChar(), tokens );

	for( int i = 0; (i < mListCtrlColumns) && (i<tokens.Size()); ++i )
		mColumnWidths[i] = (float)atof( tokens[i].AsChar() );

	mListCtrlInited = true;
}

void MenuListControl::DeleteAllRecords( void )
{
	for( int i = 0; i < mListCtrlColumns; ++i )
		mColumns[i].Clear();
	SetSelected( -1 );
}

void MenuListControl::AddRecord( const char *record )
{
	DynamicArray<String> tokens;
	String delims;
	delims = delims.Sprintf( "%c", mDelimiter );
	String::Tokenize( record, delims.AsChar(), tokens );

	for( int i = 0; i < mListCtrlColumns; ++i )
	{
		if( i < tokens.Size() )
		{
			mColumns[i].Add( tokens[i] );
		}
		else
		{
			mColumns[i].Add( "" );
		}
	}
}

String MenuListControl::GetRecord( int index )
{
	String out = "";
	String delim;
	delim = delim.Sprintf( "%c", mDelimiter );
	for( int i = 0; i < mListCtrlColumns-1; ++i )
	{
		out += mColumns[i][index];
		out += delim;
	}
	if( mListCtrlColumns > 0 )
		out += mColumns[i][index];

	return out;
}

int MenuListControl::GetRecordByValue( const char *value )
{
	DynamicArray<String> tokens;
	String delims;
	delims = delims.Sprintf( "%c", mDelimiter );
	String::Tokenize( value, delims.AsChar(), tokens );

	for( int i = 0; i < NumberOfRecords(); ++i )
	{
		bool ok = true;
		for( int j = 0; j < mListCtrlColumns; ++j )
			if( !(mColumns[j][i] == tokens[j]) )
				ok = false;
		if( ok )
			return i;
	}
	return -1;
}

void MenuListControl::SetRecord( int index, const char *record )
{
	Assert( index < NumberOfRecords() );
	DynamicArray<String> tokens;
	String delims;
	delims = delims.Sprintf( "%c", mDelimiter );
	String::Tokenize( record.AsChar(), delims.AsChar(), tokens );

	for( int i = 0; i < mListCtrlColumns; ++i )
	{
		if( i < tokens.Size() )
		{
			mColumns[i][index] = tokens[i];
		}
		else
		{
			mColumns[i][index] = "";
		}
	}
}

void MenuListControl::DeleteRecord( int index )
{
	for( int i = 0; i < mListCtrlColumns; ++i )
		mColumns[i].Remove( index );
	if( mSelectedRecord >= NumberOfRecords() )
		mSelectedRecord = NumberOfRecords()-1;
}

bool MenuListControl::DeleteRecordByField( int columnIndex, String field )
{
	Assert( ( columnIndex >= 0 ) && ( columnIndex < mListCtrlColumns ) );
	int index = -1;
	for( int i = 0; i < mColumns[0].Size(); ++i )
	{
		if( mColumns[columnIndex][i] == field )
		{
			index = i;
			break;
		}
	}
	if( index != -1 )
	{
		DeleteRecord( index );
		if( mSelectedRecord >= NumberOfRecords() )
			mSelectedRecord = NumberOfRecords()-1;
		return true;
	}
	return false;
}

// ********************************************************************
//						MENU GAME SPY CONTROL METHODS
// ********************************************************************
/*
static void GameSpyControlCallback( void* serverlist, int msg, void *instance, void *param1, void *param2 )
{
	MenuGameSpyControl* control = (MenuGameSpyControl *)instance;
	if( msg == LIST_PROGRESS ) // a progress / new server message
	{
		control->AddRecordFromServerList( (void*)param1 );
//		pDlg->m_ProgressBar.SetPos((int)param2);
		if( !(control->GetName().IsNull()) )
			GScript.DoStringA( "Menu_MenuEvent( '%s', 'Changed' )", control->GetName() );
	}
	else if( msg == LIST_STATECHANGED ) // list state changed
	{
		switch( ServerListState( (GServerList)serverlist ) )
		{
		case sl_idle : // done updating
			if( control->mThink )
				for( int i = 0; i < GEngine->Video->Menu.mGameSpyServerLists.Size(); ++i )
					if( GEngine->Video->Menu.mGameSpyServerLists[i] == control->mServerList )
					{
						GEngine->Video->Menu.mGameSpyServerLists.Remove( i );
						break;
					}

//			pDlg->m_ProgressBar.SetPos(0);

			control->GameSpyStatus = MenuGameSpyControl::EStatusDone;
			ServerListSort( (GServerList)serverlist, true, "hostname", cm_stricase );
			control->UpdateControlFromServerList();
			break;
		case sl_listxfer:
			control->GameSpyStatus = MenuGameSpyControl::EStatusGettingList;
			break;
		case sl_lanlist:
			control->GameSpyStatus = MenuGameSpyControl::EStatusSearchingLAN;
			break;
		case sl_querying:
			control->GameSpyStatus = MenuGameSpyControl::EStatusQuerying;
			break;
		}
		if( !(control->GetName().IsNull()) )
			GScript.DoStringA( "Menu_MenuEvent( '%s', 'ChangedState' )", control->GetName() );
	}
}

MenuGameSpyControl::MenuGameSpyControl() : MenuListControl(), mServerList(0), mAscending(false)
	, mThink(false), GameSpyStatus(EStatusDone), mNoIP(false), GameSpyPlayersNumber(0)
{
	mType = GameSpyControl;
}

void MenuGameSpyControl::StartGameSpy( bool noIP )
{
	char secret_key[9];

	secret_key[4] = 'd';
	secret_key[0] = 'k';
	secret_key[6] = '\0';
	secret_key[2] = 'F';
	secret_key[3] = '4';
	secret_key[1] = '7';
	secret_key[5] = '2';

	mServerList = ServerListNew( "unreal", GAMESPY_GAMENAME, secret_key,
		10, GameSpyControlCallback, GCALLBACK_FUNCTION, this );
	if( !mServerList )
	{
		KeepAtLast = false;
		SetupControl( 1, "1.0", '|' );
		AddRecord( "GameSpy control initialization failure!" );
	}
	else
	{
		// check it we are updating.. if so, cancel it
		if( ServerListState( (GServerList)mServerList ) != sl_idle )
			ServerListHalt( (GServerList)mServerList );

		mNoIP = noIP;
		if( !mNoIP )
		{
			// Assuming table columns: hostname, mapname, numplayers/maxplayers, gametype, ping, IP
			SetupControl( 6, "0.3|0.25|0.05|0.15|0.05|0.2", '|' );
		}
		else
		{
			// Assuming table columns: hostname, mapname, numplayers/maxplayers, gametype, ping
			SetupControl( 6, "0.3|0.3|0.1|0.2|0.1", '|' );
		}
	}
}

MenuGameSpyControl::~MenuGameSpyControl()
{
	if( mServerList )
	{
		if( mThink )
			for( int i = 0; i < GEngine->Video->Menu.mGameSpyServerLists.Size(); ++i )
				if( GEngine->Video->Menu.mGameSpyServerLists[i] == mServerList )
				{
					GEngine->Video->Menu.mGameSpyServerLists.Remove( i );
					break;
				}

		ServerListFree( (GServerList)mServerList );
	}
	mServerList = 0;
}

void MenuGameSpyControl::SortList( int sortType )
{
	if( !mServerList ) return;

	mAscending = !mAscending;
	switch( sortType )
	{
	case EServerName:
		ServerListSort( (GServerList)mServerList, mAscending, "hostname", cm_stricase );
		break;
	case EPing:
		ServerListSort( (GServerList)mServerList, mAscending, "ping", cm_int );
		break;
	case EPlayers:
		ServerListSort( (GServerList)mServerList, mAscending, "numplayers", cm_int );
		break;
	case EMapName:
		ServerListSort( (GServerList)mServerList, mAscending, "mapname", cm_stricase );
		break;
	case EGameType:
		ServerListSort( (GServerList)mServerList, mAscending, "gametype", cm_stricase );
		break;
	case EHostAddress:
		ServerListSort( (GServerList)mServerList, mAscending, "hostaddr", cm_strcase );
		break;
	}
	UpdateControlFromServerList();
}

void MenuGameSpyControl::AddRecordFromServerList( void* serv )
{
	GServer server = (GServer)serv;

	// Assuming table columns: hostname, mapname, numplayers/maxplayers, gametype, ping, IP

	String record = "";
	String temp;
	String delimiter;
	delimiter = delimiter.Sprintf( "%c", mDelimiter );

	// Add hostname
	temp = ServerGetStringValue( server, "hostname","(NONE)" );
	record += temp;
	record += delimiter;

	// Add map name
	temp = ServerGetStringValue( server, "mapname","(NO MAP)" );
	record += temp;
	record += delimiter;

	// Add numplayers/maxplayers
	temp = temp.Sprintf( "%d/%d", ServerGetIntValue( server, "numplayers" ,0 ),
		ServerGetIntValue( server, "maxplayers", 0 ) );
	GameSpyPlayersNumber += ServerGetIntValue( server, "numplayers" ,0 );
	record += temp;
	record += delimiter;

	// Add game type
	temp = ServerGetStringValue( server, "gametype","" );
	if( temp.IsNull() ) temp = "(Unknown)";
	record += temp;
	record += delimiter;

	// Add ping
	int ping = ServerGetPing( server );
	if( ping == 9999 )
		return;
	temp = temp.Sprintf( "%d", ping );
	record += temp;

	if( !mNoIP )	// add IP
	{
		record += delimiter;
		// Add IP
		temp = ServerGetAddress( server );
		if( temp.IsNull() ) temp = "Unknown";
		record += temp;
	}

	AddRecord( record );
}

void MenuGameSpyControl::UpdateControlFromServerList( void )
{
	if( !mServerList ) return;
	DeleteAllRecords();
	for( int i = 0; i < ServerListCount( (GServerList)mServerList ); ++i )
	{
		GServer server = ServerListGetServer( (GServerList)mServerList, i );
		AddRecordFromServerList( (void*)server );
	}
}

void MenuGameSpyControl::RefreshList( bool lan )
{
	if( !mServerList ) return;
	if( !GEngine ) return;

	// check it we are updating.. if so, cancel it
	if( ServerListState( (GServerList)mServerList) != sl_idle )
	{
		ServerListHalt( (GServerList)mServerList );
		return;
	}

	// clear the old list
	ServerListClear( (GServerList)mServerList );
	DeleteAllRecords();
	GameSpyPlayersNumber = 0;
	mThink = true;
	GEngine->Video->Menu.mGameSpyServerLists.Add( mServerList );
	if( !lan ) // Internet
		ServerListUpdate( (GServerList)mServerList, true );
	else
		ServerListLANUpdate( (GServerList)mServerList, true, DEFAULT_SERVER_PORT, DEFAULT_SERVER_PORT+10, 1 );
}

void MenuGameSpyControl::HaltRefreshList( void )
{
	// check it we are updating.. if so, cancel it
	if( ServerListState( (GServerList)mServerList) != sl_idle )
		ServerListHalt( (GServerList)mServerList );
}

bool MenuGameSpyControl::FillListCtrlWithPlayers( MenuListControl* lc )
{
	if( !lc || !mServerList || ( mSelectedRecord == -1 ) ) return false;

	GServer server = ServerListGetServer( (GServerList)mServerList, mSelectedRecord );

	int numplayers = ServerGetIntValue( server, "numplayers", 0 );

	if( !numplayers ) return false;

	lc->DeleteAllRecords();
	for( int i = 0; i < numplayers; ++i )
	{
		// Assumming list ctrl has 3 columns: playername, ping, frag
		String record = "";
		String temp;
		String delimiter;
		delimiter = delimiter.Sprintf( "%c", mDelimiter );

		// Add player name
		temp = ServerGetPlayerStringValue( server, i, "player","" );
		record += temp;
		record += delimiter;

		// Add ping
		temp = ServerGetPlayerStringValue( server, i, "ping","0" );
		record += temp;
		record += delimiter;

		// Add frags
		temp = ServerGetPlayerStringValue( server, i, "frags","0" );
		record += temp;

		lc->AddRecord( record );
	}
	return true;
}
*/
// ********************************************************************
//						MENU REDEFINE KEYS CONTROL METHODS
// ********************************************************************

MenuRedefineKeysControl::MenuRedefineKeysControl() : MenuWindow()
{
	mType = RedefineKeysControl;

    m_setTag("Menu");
	mActionList = new MenuListControl;
	mActionList->SetParent( this );
	mActionList->SetPos( 0.0f, 0.0f );
	mActionList->SetSize( 0.4f, 1.0f );
	mActionList->NoSelect = true;
	mActionList->MultiresolutionText = true;
	mActionList->Flags |= Shown|HasTexture;
	mActionList->SetEventHandler( MenuEventHandlerRedefineKeysControl );
	mActionList->Color = 0x90707070;
	mActionList->ColorUnderMouse = 0x90707070;
	mActionList->TextColor = 0xFF20FF20;
	mActionList->TextColorUnderMouse = 0xFF20FF20;
	mActionList->SetupControl( 1, "1.0" );

    m_setTag("Menu");
	mPrimaryList = new MenuListControl;
	mPrimaryList->SetParent( this );
	mPrimaryList->SetPos( 0.4f, 0.0f );
	mPrimaryList->SetSize( 0.3f, 1.0f );
	mPrimaryList->MultiresolutionText = true;
	mPrimaryList->Flags |= Shown|HasTexture;
	mPrimaryList->SetEventHandler( MenuEventHandlerRedefineKeysControl );
	mPrimaryList->Color = 0x90707070;
	mPrimaryList->ColorUnderMouse = 0x90707070;
	mPrimaryList->TextColor = 0xFF20FF20;
	mPrimaryList->TextColorUnderMouse = 0xFF20FF20;
	mPrimaryList->SetupControl( 1, "1.0" );

    m_setTag("Menu");
    mAlternativeList = new MenuListControl;
	mAlternativeList->SetParent( this );
	mAlternativeList->SetPos( 0.7f, 0.0f );
	mAlternativeList->SetSize( 0.3f, 1.0f );
	mAlternativeList->MultiresolutionText = true;
	mAlternativeList->Flags |= Shown|HasTexture;
	mAlternativeList->SetEventHandler( MenuEventHandlerRedefineKeysControl );
	mAlternativeList->Color = 0x90707070;
	mAlternativeList->ColorUnderMouse = 0x90707070;
	mAlternativeList->TextColor = 0xFF20FF20;
	mAlternativeList->TextColorUnderMouse = 0xFF20FF20;
	mAlternativeList->SetupControl( 1, "1.0" );

//	SetDefaultKeys();
	LoadKeysFromLUA();
}

MenuRedefineKeysControl::~MenuRedefineKeysControl()
{
}

void MenuRedefineKeysControl::LoadKeysFromLUA( void )
{
	mActionList->DeleteAllRecords();
	mActionList->AddRecord( "Move forward" );
	mActionList->AddRecord( "Move backward" );
	mActionList->AddRecord( "Strafe left" );
	mActionList->AddRecord( "Strafe right" );
	mActionList->AddRecord( "Jump" );
	mActionList->AddRecord( "Fire" );
	mActionList->AddRecord( "Alternative Fire" );
	mActionList->AddRecord( "Next Weapon" );
	mActionList->AddRecord( "Previous Weapon" );
#ifndef TRANSGAMING
	mActionList->AddRecord( "Weapon #1" );
	mActionList->AddRecord( "Weapon #2" );
	mActionList->AddRecord( "Weapon #3" );
	mActionList->AddRecord( "Weapon #4" );
	mActionList->AddRecord( "Weapon #5" );
#endif
	mActionList->AddRecord( "Pause" );
	mActionList->AddRecord( "Screenshot" );
	mActionList->AddRecord( "Menu" );
	mActionList->AddRecord( "Scoreboard" );
#ifndef TRANSGAMING
	mActionList->AddRecord( "Say to all" );
	mActionList->AddRecord( "Say to team" );
	mActionList->AddRecord( "Rocket Jump" );
	mActionList->AddRecord( "Forward Rocket Jump" );
#endif

	InputSystem* inputSys = GEngine->InputSys;
	Assert( inputSys );
    inputSys->ZeroBindings();

	mPrimaryList->DeleteAllRecords();
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryMoveForward","None") );
	inputSys->BindKey( EInputMoveForward, inputSys->GetKeyByName( mPrimaryList->GetRecord(0) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryMoveBackward","None") );
	inputSys->BindKey( EInputMoveBackward, inputSys->GetKeyByName( mPrimaryList->GetRecord(1) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryStrafeLeft","None") );
	inputSys->BindKey( EInputMoveLeft, inputSys->GetKeyByName( mPrimaryList->GetRecord(2) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryStrafeRight","None") );
	inputSys->BindKey( EInputMoveRight, inputSys->GetKeyByName( mPrimaryList->GetRecord(3) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryJump","None") );
	inputSys->BindKey( EInputJump, inputSys->GetKeyByName( mPrimaryList->GetRecord(4) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryFire","None") );
	inputSys->BindKey( EInputFire, inputSys->GetKeyByName( mPrimaryList->GetRecord(5) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryAlternativeFire","None") );
	inputSys->BindKey( EInputAltFire, inputSys->GetKeyByName( mPrimaryList->GetRecord(6) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryNextWeapon","None") );
	inputSys->BindKey( EInputNextWeapon, inputSys->GetKeyByName( mPrimaryList->GetRecord(7) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryPreviousWeapon","None") );
	inputSys->BindKey( EInputPrevWeapon, inputSys->GetKeyByName( mPrimaryList->GetRecord(8) ) );
#ifndef TRANSGAMING
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryWeapon1","None") );
	inputSys->BindKey( EInputWeapon1, inputSys->GetKeyByName( mPrimaryList->GetRecord(9) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryWeapon2","None") );
	inputSys->BindKey( EInputWeapon2, inputSys->GetKeyByName( mPrimaryList->GetRecord(10) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryWeapon3","None") );
	inputSys->BindKey( EInputWeapon3, inputSys->GetKeyByName( mPrimaryList->GetRecord(11) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryWeapon4","None") );
	inputSys->BindKey( EInputWeapon4, inputSys->GetKeyByName( mPrimaryList->GetRecord(12) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryWeapon5","None") );
	inputSys->BindKey( EInputWeapon5, inputSys->GetKeyByName( mPrimaryList->GetRecord(13) ) );
#endif
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryPause","None") );
	inputSys->BindKey( EInputPause, inputSys->GetKeyByName( mPrimaryList->GetRecord(14) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryScreenshot","None") );
	inputSys->BindKey( EInputScreenshot, inputSys->GetKeyByName( mPrimaryList->GetRecord(15) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryMenu","None") );
	inputSys->BindKey( EInputMenu, inputSys->GetKeyByName( mPrimaryList->GetRecord(16) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryScoreboard","None") );
	inputSys->BindKey( EInputScoreboard, inputSys->GetKeyByName( mPrimaryList->GetRecord(17) ) );
#ifndef TRANSGAMING
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimarySayToAll","None") );
	inputSys->BindKey( EInputSayToAll, inputSys->GetKeyByName( mPrimaryList->GetRecord(18) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimarySayToTeam","None") );
	inputSys->BindKey( EInputSayToTeam, inputSys->GetKeyByName( mPrimaryList->GetRecord(19) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryRocketJump","None") );
	inputSys->BindKey( EInputRocketJump, inputSys->GetKeyByName( mPrimaryList->GetRecord(20) ) );
	mPrimaryList->AddRecord( GScript.GetSafeCfgVar("KeyPrimaryForwardRocketJump","None") );
	inputSys->BindKey( EInputForwardRocketJump, inputSys->GetKeyByName( mPrimaryList->GetRecord(21) ) );
#endif

	mAlternativeList->DeleteAllRecords();
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeMoveForward","None") );
	inputSys->BindKey( EInputMoveForward, inputSys->GetKeyByName( mAlternativeList->GetRecord(0) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeMoveBackward","None") );
	inputSys->BindKey( EInputMoveBackward, inputSys->GetKeyByName( mAlternativeList->GetRecord(1) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeStrafeLeft","None") );
	inputSys->BindKey( EInputMoveLeft, inputSys->GetKeyByName( mAlternativeList->GetRecord(2) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeStrafeRight","None") );
	inputSys->BindKey( EInputMoveRight, inputSys->GetKeyByName( mAlternativeList->GetRecord(3) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeJump","None") );
	inputSys->BindKey( EInputJump, inputSys->GetKeyByName( mAlternativeList->GetRecord(4) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeFire","None") );
	inputSys->BindKey( EInputFire, inputSys->GetKeyByName( mAlternativeList->GetRecord(5) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeAlternativeFire","None") );
	inputSys->BindKey( EInputAltFire, inputSys->GetKeyByName( mAlternativeList->GetRecord(6) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeNextWeapon","None") );
	inputSys->BindKey( EInputNextWeapon, inputSys->GetKeyByName( mAlternativeList->GetRecord(7) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativePreviousWeapon","None") );
	inputSys->BindKey( EInputPrevWeapon, inputSys->GetKeyByName( mAlternativeList->GetRecord(8) ) );
#ifndef TRANSGAMING
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeWeapon1","None") );
	inputSys->BindKey( EInputWeapon1, inputSys->GetKeyByName( mAlternativeList->GetRecord(9) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeWeapon2","None") );
	inputSys->BindKey( EInputWeapon2, inputSys->GetKeyByName( mAlternativeList->GetRecord(10) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeWeapon3","None") );
	inputSys->BindKey( EInputWeapon3, inputSys->GetKeyByName( mAlternativeList->GetRecord(11) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeWeapon4","None") );
	inputSys->BindKey( EInputWeapon4, inputSys->GetKeyByName( mAlternativeList->GetRecord(12) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeWeapon5","None") );
	inputSys->BindKey( EInputWeapon5, inputSys->GetKeyByName( mAlternativeList->GetRecord(13) ) );
#endif
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativePause","None") );
	inputSys->BindKey( EInputPause, inputSys->GetKeyByName( mAlternativeList->GetRecord(14) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeScreenshot","None") );
	inputSys->BindKey( EInputScreenshot, inputSys->GetKeyByName( mAlternativeList->GetRecord(15) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeMenu","None") );
	inputSys->BindKey( EInputMenu, inputSys->GetKeyByName( mAlternativeList->GetRecord(16) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeScoreboard","None") );
	inputSys->BindKey( EInputScoreboard, inputSys->GetKeyByName( mAlternativeList->GetRecord(17) ) );
#ifndef TRANSGAMING
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeSayToAll","None") );
	inputSys->BindKey( EInputSayToAll, inputSys->GetKeyByName( mAlternativeList->GetRecord(18) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeSayToTeam","None") );
	inputSys->BindKey( EInputSayToTeam, inputSys->GetKeyByName( mAlternativeList->GetRecord(19) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeRocketJump","None") );
	inputSys->BindKey( EInputRocketJump, inputSys->GetKeyByName( mAlternativeList->GetRecord(20) ) );
	mAlternativeList->AddRecord( GScript.GetSafeCfgVar("KeyAlternativeForwardRocketJump","None") );
	inputSys->BindKey( EInputForwardRocketJump, inputSys->GetKeyByName( mAlternativeList->GetRecord(21) ) );
#endif
}

void MenuRedefineKeysControl::SaveKeysToLUA( void )
{
	GScript.SetSafeCfgVar( "KeyPrimaryMoveForward", mPrimaryList->GetRecord(0).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryMoveBackward", mPrimaryList->GetRecord(1).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryStrafeLeft", mPrimaryList->GetRecord(2).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryStrafeRight", mPrimaryList->GetRecord(3).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryJump", mPrimaryList->GetRecord(4).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryFire", mPrimaryList->GetRecord(5).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryAlternativeFire", mPrimaryList->GetRecord(6).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryNextWeapon", mPrimaryList->GetRecord(7).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryPreviousWeapon", mPrimaryList->GetRecord(8).AsChar() );
#ifndef TRANSGAMING
	GScript.SetSafeCfgVar( "KeyPrimaryWeapon1", mPrimaryList->GetRecord(9).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryWeapon2", mPrimaryList->GetRecord(10).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryWeapon3", mPrimaryList->GetRecord(11).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryWeapon4", mPrimaryList->GetRecord(12).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryWeapon5", mPrimaryList->GetRecord(13).AsChar() );
#endif
	GScript.SetSafeCfgVar( "KeyPrimaryPause", mPrimaryList->GetRecord(14).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryScreenshot", mPrimaryList->GetRecord(15).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryMenu", mPrimaryList->GetRecord(16).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryScoreboard", mPrimaryList->GetRecord(17).AsChar() );
#ifndef TRANSGAMING
	GScript.SetSafeCfgVar( "KeyPrimarySayToAll", mPrimaryList->GetRecord(18).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimarySayToTeam", mPrimaryList->GetRecord(19).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryRocketJump", mPrimaryList->GetRecord(20).AsChar() );
	GScript.SetSafeCfgVar( "KeyPrimaryForwardRocketJump", mPrimaryList->GetRecord(21).AsChar() );
#endif

	GScript.SetSafeCfgVar( "KeyAlternativeMoveForward", mAlternativeList->GetRecord(0).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeMoveBackward", mAlternativeList->GetRecord(1).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeStrafeLeft", mAlternativeList->GetRecord(2).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeStrafeRight", mAlternativeList->GetRecord(3).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeJump", mAlternativeList->GetRecord(4).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeFire", mAlternativeList->GetRecord(5).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeAlternativeFire", mAlternativeList->GetRecord(6).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeNextWeapon", mAlternativeList->GetRecord(7).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativePreviousWeapon", mAlternativeList->GetRecord(8).AsChar() );
#ifndef TRANSGAMING
	GScript.SetSafeCfgVar( "KeyAlternativeWeapon1", mAlternativeList->GetRecord(9).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeWeapon2", mAlternativeList->GetRecord(10).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeWeapon3", mAlternativeList->GetRecord(11).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeWeapon4", mAlternativeList->GetRecord(12).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeWeapon5", mAlternativeList->GetRecord(13).AsChar() );
#endif
	GScript.SetSafeCfgVar( "KeyAlternativePause", mAlternativeList->GetRecord(14).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeScreenshot", mAlternativeList->GetRecord(15).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeMenu", mAlternativeList->GetRecord(16).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeScoreboard", mAlternativeList->GetRecord(17).AsChar() );
#ifndef TRANSGAMING
	GScript.SetSafeCfgVar( "KeyAlternativeSayToAll", mAlternativeList->GetRecord(18).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeSayToTeam", mAlternativeList->GetRecord(19).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeRocketJump", mAlternativeList->GetRecord(20).AsChar() );
	GScript.SetSafeCfgVar( "KeyAlternativeForwardRocketJump", mAlternativeList->GetRecord(21).AsChar() );
#endif
}

void MenuRedefineKeysControl::SetDefaultKeys( void )
{
	mActionList->DeleteAllRecords();
	mActionList->AddRecord( "Move forward" );
	mActionList->AddRecord( "Move backward" );
	mActionList->AddRecord( "Strafe left" );
	mActionList->AddRecord( "Strafe right" );
	mActionList->AddRecord( "Jump" );
	mActionList->AddRecord( "Fire" );
	mActionList->AddRecord( "Alternative Fire" );
	mActionList->AddRecord( "Next Weapon" );
	mActionList->AddRecord( "Previous Weapon" );
#ifndef TRANSGAMING
	mActionList->AddRecord( "Weapon #1" );
	mActionList->AddRecord( "Weapon #2" );
	mActionList->AddRecord( "Weapon #3" );
	mActionList->AddRecord( "Weapon #4" );
	mActionList->AddRecord( "Weapon #5" );
#endif
	mActionList->AddRecord( "Pause" );
	mActionList->AddRecord( "Screenshot" );
	mActionList->AddRecord( "Menu" );
	mActionList->AddRecord( "Scoreboard" );
#ifndef TRANSGAMING
	mActionList->AddRecord( "Say to all" );
	mActionList->AddRecord( "Say to team" );
	mActionList->AddRecord( "Rocket Jump" );
	mActionList->AddRecord( "Forward Rocket Jump" );
#endif

	mPrimaryList->DeleteAllRecords();
	mPrimaryList->AddRecord( "W" );
	mPrimaryList->AddRecord( "S" );
	mPrimaryList->AddRecord( "A" );
	mPrimaryList->AddRecord( "D" );
	mPrimaryList->AddRecord( "Space" );
	mPrimaryList->AddRecord( "Left Mouse Button" );
	mPrimaryList->AddRecord( "Right Mouse Button" );
	mPrimaryList->AddRecord( "None" );
	mPrimaryList->AddRecord( "None" );
#ifndef TRANSGAMING
	mPrimaryList->AddRecord( "None" );
	mPrimaryList->AddRecord( "None" );
	mPrimaryList->AddRecord( "None" );
	mPrimaryList->AddRecord( "None" );
	mPrimaryList->AddRecord( "None" );
#endif
	mPrimaryList->AddRecord( "None" );
	mPrimaryList->AddRecord( "None" );
	mPrimaryList->AddRecord( "Escape" );
	mPrimaryList->AddRecord( "Tab" );
#ifndef TRANSGAMING
	mPrimaryList->AddRecord( "None" );
	mPrimaryList->AddRecord( "None" );
	mPrimaryList->AddRecord( "None" );
	mPrimaryList->AddRecord( "None" );
#endif

	mAlternativeList->DeleteAllRecords();
	mAlternativeList->AddRecord( "Cursor Up" );
	mAlternativeList->AddRecord( "Cursor Down" );
	mAlternativeList->AddRecord( "Cursor Left" );
	mAlternativeList->AddRecord( "Cursor Right" );
	mAlternativeList->AddRecord( "None" );
	mAlternativeList->AddRecord( "None" );
	mAlternativeList->AddRecord( "None" );
	mAlternativeList->AddRecord( "None" );
	mAlternativeList->AddRecord( "None" );
#ifndef TRANSGAMING
	mAlternativeList->AddRecord( "None" );
	mAlternativeList->AddRecord( "None" );
	mAlternativeList->AddRecord( "None" );
	mAlternativeList->AddRecord( "None" );
	mAlternativeList->AddRecord( "None" );
#endif
	mAlternativeList->AddRecord( "None" );
	mAlternativeList->AddRecord( "None" );
	mAlternativeList->AddRecord( "None" );
	mAlternativeList->AddRecord( "None" );
#ifndef TRANSGAMING
	mAlternativeList->AddRecord( "None" );
	mAlternativeList->AddRecord( "None" );
	mAlternativeList->AddRecord( "None" );
	mAlternativeList->AddRecord( "None" );
#endif
}

// ********************************************************************
//						MENU EDIT FIELD METHODS
// ********************************************************************

MenuEditField::MenuEditField() : MenuWindow(), ReadOnly(false), NumberField(false)
	, MinimalNumberOfCharacters(0), MaximalNumberOfCharacters(40), IPField( false )
{
	mType = EditField;
	SetEventHandler( MenuEventHandlerEditField );
}

MenuEditField::~MenuEditField()
{
}

void MenuEditField::Render( HUD& hud, float absX, float absY, float absW, float absH )
{
	if( !(Flags & Shown) )
		return;		// don't draw if not shown

	if( !mTexture && !( mTextureName.IsNull() ) )
	{
		mTexture = GEngine->Video->MaterialSys->CreateTexture( mTextureName.AsChar(), mTextureFlags | Texture::ETexNoMipMaps );
		Flags |= HasTexture;
	}

	GenerateTick();

	float myAbsX = absX + (float)floor(X*absW);
	float myAbsY = absY + (float)floor(Y*absH);
	float myAbsW = (float)floor(absW*W);
	float myAbsH = (float)floor(absH*H);

	// Render window texture if present

	if( ( Flags & HasRect ) )
        hud.DrawRect( myAbsX-0.6f, myAbsY-0.6f, myAbsX+myAbsW+0.7f, myAbsY+myAbsH+0.7f, mUnderMouse?ColorUnderMouse:Color );

	if( ( Flags & HasTexture ) && mTexture && !(mTexture->Flags & Texture::ETexInvalid ) )
        hud.DrawQuad( mTexture, myAbsX, myAbsY, myAbsW, myAbsH, mUnderMouse?ColorUnderMouse:Color, U1, V1, U2, V2 );

	String fontname = MultiresolutionText?mFontName + GEngine->Video->Menu.GetResolutionString():mFontName;
	hud.SetFont( fontname.AsChar() );
	TextJustify();
	int myAbsTextX = (int)(myAbsX + (float)floor(mTextX*myAbsW));
	int myAbsTextY = (int)(myAbsY + (float)floor(mTextY*myAbsH));

	String toPrint = Text;
	if( Flags & HasFocus )
	{
#ifdef TRANSGAMING
		float t = GEngine->SysDriver.GetCurrentTime();
#else
		float t = (float)GEngine->SysDriver.GetCurrentTime();
#endif
		if( t - floor(t) > 0.5f )
			toPrint += "|";
	}

	if( MultiresolutionText )
		hud.Print( myAbsTextX, myAbsTextY, toPrint.AsChar(), mUnderMouse?TextColorUnderMouse:TextColor );
	else
		hud.Print( myAbsTextX, myAbsTextY, toPrint.AsChar(), mUnderMouse?TextColorUnderMouse:TextColor, ScaledText, TextScale,
			GEngine->Video->Menu.mRenderTargetX, GEngine->Video->Menu.mRenderTargetY );

	// Render child windows
	for( int i = 0; i < mChildWindows.Size(); ++i )
		mChildWindows[i]->Render( hud, myAbsX, myAbsY, myAbsW, myAbsH );
}

// ********************************************************************
//						MENU SCROLL BAR METHODS
// ********************************************************************

MenuScrollBar::MenuScrollBar() : MenuWindow(), Position(0.0f), PercentCovered(1.0f)
	, DrawPercent( false ), mMouseCaught( false ), PercentAddTo( 0.0f ), PercentMultiplyBy( 100.0f )
	, PercentFollowBy( "%" )
{
	mType = ScrollBar;
	SetEventHandler( MenuEventHandlerScrollBar );

	// Let's assume that buttons will have the same width as height, and the bar will be proportional to
	// the extent covered by the scrollbar.

	// Make the first button - in default coords now, they will be adjusted before each display
    m_setTag("Menu");
    mButton0 = new MenuButton;
	mButton0->SetParent( this );
	mButton0->Flags |= Shown;
	mButton0->SetEventHandler( MenuEventHandlerGiveClickToParent );
	mButton0->Color = 0xFFF0F0F0;
	mButton0->ColorUnderMouse = 0xFFFFFFFF;

	// Make the second button - in default coords now, they will be adjusted before each display
    m_setTag("Menu");
	mButton1 = new MenuButton;
	mButton1->SetParent( this );
	mButton1->Flags |= Shown;
	mButton1->SetEventHandler( MenuEventHandlerGiveClickToParent );
	mButton1->Color = 0xFFF0F0F0;
	mButton1->ColorUnderMouse = 0xFFFFFFFF;

	// Make the bar - in default coords now, they will be adjusted before each display
    m_setTag("Menu");
	mBar = new MenuButton;
	mBar->SetParent( this );
	mBar->Flags |= Shown;
	mBar->SetEventHandler( MenuEventHandlerGiveClickToParent );
	mBar->Color = 0xFFF0F0F0;
	mBar->ColorUnderMouse = 0xFFFFFFFF;
}

MenuScrollBar::~MenuScrollBar()
{
	// Nothing; it will destroy its children automatically
}

void MenuScrollBar::SetTextureButton0( const char * texName, DWORD color, DWORD texFlags )
{
	mButton0->SetTexture( texName, texFlags );
	mButton0->Color = color;
}

void MenuScrollBar::SetTextureButton1( const char * texName, DWORD color, DWORD texFlags )
{
	mButton1->SetTexture( texName, texFlags );
	mButton1->Color = color;
}

void MenuScrollBar::SetTextureBack( const char * texName, DWORD color, DWORD texFlags )
{
	SetTexture( texName, texFlags );
	Color = color;
}

void MenuScrollBar::SetTextureBar( const char * texName, DWORD color, DWORD texFlags )
{
	mBar->SetTexture( texName, texFlags );
	mBar->Color = color;
}

void MenuScrollBar::Render( HUD& hud, float absX, float absY, float absW, float absH )
{
	if( !(Flags & Shown) )
		return;		// don't draw if not shown

	if( !mTexture && !( mTextureName.IsNull() ) )
	{
		mTexture = GEngine->Video->MaterialSys->CreateTexture( mTextureName.AsChar(), mTextureFlags | Texture::ETexNoMipMaps );
		Flags |= HasTexture;
	}

	GenerateTick();

	float myAbsX = absX + (float)floor(X*absW);
	float myAbsY = absY + (float)floor(Y*absH);
	float myAbsW = (float)floor(absW*W);
	float myAbsH = (float)floor(absH*H);

	// Render window texture if present

	if( ( Flags & HasRect ) )
        hud.DrawRect( myAbsX-0.6f, myAbsY-0.6f, myAbsX+myAbsW+0.7f, myAbsY+myAbsH+0.7f, Color );

	if( ( Flags & HasTexture ) && mTexture && !(mTexture->Flags & Texture::ETexInvalid ) )
        hud.DrawQuad( mTexture, myAbsX, myAbsY, myAbsW, myAbsH, Color, U1, V1, U2, V2 );

	if( mMouseCaught )
	{
		if( !(GEngine->InputSys->GetMouseStatus() & (1<<EMouseLeftButton) ) )
			mMouseCaught = false;
		else
		{
			// Update the scroll bar position from mouse
			float mouseX = 0.0, mouseY = 0.0;
			GetRelativeMouseCoords( mouseX, mouseY );

			Vector pos = mBar->GetPos();
			if( pos.Y == 0.0f )	// horizontal
				Position = (mouseX - mButton0->GetSize().X)/(1.0f - mButton0->GetSize().X - mButton1->GetSize().X);
			else					// vertical
				Position = (mouseY - mButton0->GetSize().Y)/(1.0f - mButton0->GetSize().Y - mButton1->GetSize().Y);
			if( Position < 0.0f ) Position = 0.0f;
			if( Position > 1.0f ) Position = 1.0f;
			GScript.DoStringA( "Menu_MenuEvent( '%s', 'Changed' )", GetName() );
		}
	}

	// Set up child windows positions
	if( myAbsW > myAbsH )	// horizontal
	{
		float freeRegionMin = myAbsX;
		float freeRegionMax = myAbsX+myAbsW;
		if( myAbsW > 2 * myAbsH )	// Draw buttons and update free region for bar
		{
			freeRegionMin += myAbsH;
			freeRegionMax -= myAbsH;
			mButton0->X = 0.0f; mButton0->Y = 0.0f; mButton0->W = myAbsH/myAbsW; mButton0->H = 1.0f;
			mButton0->Flags |= Shown;
			mButton1->X = 1.0f-myAbsH/myAbsW; mButton1->Y = 0.0f; mButton1->W = myAbsH/myAbsW; mButton1->H = 1.0f;
			mButton1->Flags |= Shown;
		}
		else						// Don't draw buttons
		{
			mButton0->Flags &= ~Shown;
			mButton1->Flags &= ~Shown;
		}

		// Draw bar
		mBar->X = (freeRegionMin-myAbsX)/myAbsW + Position*(freeRegionMax-freeRegionMin)/myAbsW*(1.0f-PercentCovered);
		mBar->Y = 0.0f;
		mBar->W = (freeRegionMax-freeRegionMin)/myAbsW*PercentCovered;
		mBar->H = 1.0f;
		mBar->Flags |= Shown;
	}
	else					// vertical
	{
		float freeRegionMin = myAbsY;
		float freeRegionMax = myAbsY+myAbsH;
		if( myAbsH > 2 * myAbsW )	// Draw buttons and update free region, else don't draw buttons
		{
			freeRegionMin += myAbsW;
			freeRegionMax -= myAbsW;
			mButton0->X = 0.0f; mButton0->Y = 0.0f; mButton0->W = 1.0f; mButton0->H = myAbsW/myAbsH;
			mButton0->Flags |= Shown;
			mButton1->X = 0.0f; mButton1->Y = 1.0f-myAbsW/myAbsH; mButton1->W = 1.0f; mButton1->H = myAbsW/myAbsH;
			mButton1->Flags |= Shown;
		}
		else
		{
			mButton0->Flags &= ~Shown;
			mButton1->Flags &= ~Shown;
		}

		// Draw bar
		mBar->X = 0.0f;
		mBar->Y = (freeRegionMin-myAbsY)/myAbsH + Position*(freeRegionMax-freeRegionMin)/myAbsH*(1.0f-PercentCovered);
		mBar->W = 1.0f;
		mBar->H = (freeRegionMax-freeRegionMin)/myAbsH*PercentCovered;
		mBar->Flags |= Shown;
	}

	// Render child windows
	for( int i = 0; i < mChildWindows.Size(); ++i )
		mChildWindows[i]->Render( hud, myAbsX, myAbsY, myAbsW, myAbsH );

	// Draw percent, if we can
	if( DrawPercent )
	{
		String tempText = Text;
		Text = Text.Sprintf( "%.1f %s", Position*PercentMultiplyBy + PercentAddTo, PercentFollowBy.AsChar() );
		String fontname = MultiresolutionText?mFontName + GEngine->Video->Menu.GetResolutionString():mFontName;
		hud.SetFont( fontname.AsChar() );
		int temp = TextJustification;
		TextJustification = ETextCentered;
		TextJustify();
		TextJustification = temp;
		int myAbsTextX = (int)(myAbsX + (float)floor(mTextX*myAbsW));
		int myAbsTextY = (int)(myAbsY + (float)floor(mTextY*myAbsH));
		if( MultiresolutionText )
			hud.Print( myAbsTextX, myAbsTextY, Text.AsChar(), mUnderMouse?TextColorUnderMouse:TextColor );
		else
			hud.Print( myAbsTextX, myAbsTextY, Text.AsChar(), mUnderMouse?TextColorUnderMouse:TextColor, ScaledText, TextScale,
				GEngine->Video->Menu.mRenderTargetX, GEngine->Video->Menu.mRenderTargetY );
		Text = tempText;
	}

	// Draw text, if exists
	if( Flags & HasText )
	{
		String fontname = MultiresolutionText?mFontName + GEngine->Video->Menu.GetResolutionString():mFontName;
		hud.SetFont( fontname.AsChar() );
		TextJustify();
		int myAbsTextX = (int)(myAbsX + (float)floor(mTextX*myAbsW));
		int myAbsTextY = (int)(myAbsY + (float)floor(mTextY*myAbsH));
		if( MultiresolutionText )
			hud.Print( myAbsTextX, myAbsTextY, Text.AsChar(), mUnderMouse?TextColorUnderMouse:TextColor );
		else
			hud.Print( myAbsTextX, myAbsTextY, Text.AsChar(), mUnderMouse?TextColorUnderMouse:TextColor, ScaledText, TextScale,
				GEngine->Video->Menu.mRenderTargetX, GEngine->Video->Menu.mRenderTargetY );
	}
}

// ********************************************************************
//						MENU SYSTEM METHODS
// ********************************************************************

MenuSystem::MenuSystem( void ) : mActive(false), mRootWindow(0), mFocusWindow(0), mHandledEventMarker(false)
	, mCurrentWindowUnderMouse( 0 ), MouseShown( false ), mMouseTexture( 0 ), mMouseClickOffsetX(0)
	, mMouseClickOffsetY(0), mMapWindow(0), mMapX(0.0f), mMapY(0.0f), mMapW(1.0f), mMapH(1.0f)
	, mMapTexture(""), mMapExtent(1.0f), mMapPlayerX(0.0f), mMapPlayerY(0.0f), mMapPlayerAngle(0.0f)
	, mResolutionString( "640" )
{
}

MenuSystem::~MenuSystem( void )
{
	Clear();
	if( mMouseTexture && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( mMouseTexture );
	mMouseTexture = 0;
}

void MenuSystem::Clear( void )
{
	for( int i = 0; i < mAllWindows.Size(); ++i )
		if( mAllWindows[i] )
			mAllWindows[i]->RemoveAllChildren();

	for( int i = 0; i < mAllWindows.Size(); ++i )
		delete mAllWindows[i];

	mAllWindows.Clear();
	mFreeWindowIndices.Clear();
	mRootWindow = NULL;
}

void MenuSystem::Activate( bool active )
{
	mActive = active;
	if( active )
	{
		if( !mMouseTexture )
			SetMouseTexture( 0, 0, "HUD/kursor" );
		// BLAIR - We're disabling this mouse...permanently!
#ifndef TRANSGAMING
		ShowMouse( true );
#endif
	}
	else if( mMouseTexture )
	{
		ShowMouse( false );
		GEngine->Video->MaterialSys->ReleaseTexture( mMouseTexture );
		mMouseTexture = 0;
	}
}

void MenuSystem::Render( void )
{
	if( !mActive || !mRootWindow )
		return;	// don't render anything if menu system not active

	RenderDevice* dev=GEngine->Video->RenderDev;

	TextureState st[2];
	st[0].Set(0,TextureState::ETexOpColor,0,TextureState::ETexFilterPoint);
	st[0].AlphaOp = TextureState::ETexOpTexture;
	//st[0].AlphaArg = TextureState::ETexOpDiffuse;

	st[1].Set(1,TextureState::ETexOpDisable);

	dev->SetTextureState(st,ARRAY_COUNT(st));            
	dev->SetShader(EInvalidIndex,RenderDevice::EShaderPixel);
//	dev->SetConstColor(0x22ff00);

	HUD& hud = GEngine->Video->Hud;
	hud.SetRenderState();
	hud.SetFont(0);

	// MouseEntered / MouseLeft messages generation
	MenuWindow* windowUnderMouse = GetWindowUnderMouse();
	if( windowUnderMouse != mCurrentWindowUnderMouse )
	{
		if( mCurrentWindowUnderMouse )
			mCurrentWindowUnderMouse->OtherEvent( EMenuMouseLeft, 0, 0 );
		if( windowUnderMouse )
			windowUnderMouse->OtherEvent( EMenuMouseEntered, 0, 0 );
		mCurrentWindowUnderMouse = windowUnderMouse;
	}

	// If map window exists and is visible, apply its new texture and coords based on player position and orientation
	if( mMapWindow && mMapWindow->Visible() )
	{
		mMapWindow->SetTexture( mMapTexture.AsChar() );
		float u1 = ((mMapPlayerX - mMapExtent) - mMapX)/mMapW;
		float u2 = ((mMapPlayerX + mMapExtent) - mMapX)/mMapW;
		float v1 = ((mMapPlayerY - mMapExtent) - mMapY)/mMapH;
		float v2 = ((mMapPlayerY + mMapExtent) - mMapY)/mMapH;
		mMapWindow->SetUV(u1,v1,u2,v2);
	}

	mRenderTargetX = GEngine->Video->GetActiveView()->LocalViewport.Width;
	mRenderTargetY = GEngine->Video->GetActiveView()->LocalViewport.Height;

	if( mRenderTargetX <= 640 )
		mResolutionString = "640";
	else if( mRenderTargetX <= 800 )
		mResolutionString = "800";
	else if( mRenderTargetX <= 1024 )
		mResolutionString = "1024";
	else
		mResolutionString = "1280";


	// Actual rendering
	mRootWindow->Render( hud, 0.0f, 0.0f, mRenderTargetX, mRenderTargetY);



//	for( int i = 0; i < mGameSpyServerLists.Size(); ++i )
//		ServerListThink( (GServerList)mGameSpyServerLists[i] );

	// Mose cursor rendering
	if( mMouseTexture && !(mMouseTexture->Flags & Texture::ETexInvalid ) && MouseShown )
	{
		POINT mousePos = GEngine->InputSys->GetMousePosition();
		hud.DrawQuad( mMouseTexture, (float)(mousePos.x-mMouseClickOffsetX),
			(float)(mousePos.y-mMouseClickOffsetY), (float)mMouseTexture->Width, (float)mMouseTexture->Height );
	}
}

MenuWindow* MenuSystem::GetWindowByName( const char *name )
{
	if( name[0] == 0 )
		return NULL;	// Windows with no name can't be found - internal to C++ code, widgets and so on

	for( int i = 0; i < mCompleteWindowSet.Size(); ++i )
		if ( !strcmp(mCompleteWindowSet[i]->mWindowName.AsChar(), name) )
			return mCompleteWindowSet[i];
	return NULL;
}

MenuWindow* MenuSystem::GetWindowByIdent( int ident )
{
	if( ( ident < 0 ) || ( ident >= mAllWindows.Size() ) )
		return NULL;
	return mAllWindows[ident];
}

void MenuSystem::RememberWindow( MenuWindow* window )
{
	Assert( window->mWindowIdent == -1 );

	if( mFreeWindowIndices.Size() )
	{
		int index = mFreeWindowIndices[ mFreeWindowIndices.Size()-1 ];
		mFreeWindowIndices.RemoveLast();
		mAllWindows[index] = window;
		window->mWindowIdent = index;
	}
	else
	{
		int index = mAllWindows.Size();
		mAllWindows.Add( window );
		window->mWindowIdent = index;
	}
}

void MenuSystem::ForgetWindow( int ident )
{
	MenuWindow* window = GetWindowByIdent( ident );
	if( !window )
		return;

	mFreeWindowIndices.Add( ident );
	mAllWindows[ident] = 0;
	window->mWindowIdent = -1;
}

/*
static bool MenuEventHandlerLog( MenuWindow* window, int event, void* data )
{
	String one;
	if( window->GetName().IsNull() )
	{
		if( window->GetIdent() == -1 )
		{
			one = "Menu Event in internal window, ";
		}
		else
		{
			one = one.Sprintf("Menu Event in window %d, ", window->GetIdent() );
		}
	}
	else
	{
		one = one.Sprintf("Menu Event in window \"%s\", ", window->GetName() );
	}

	String two;
	switch( event )
	{
	case EMenuKeyDown:
		two = two.Sprintf( " key %d down.\n", *(char*)data );
		break;
	case EMenuKeyUp:
		two = two.Sprintf( " key %d up.\n", *(char*)data );
		break;
	case EMenuMouseDown:
		{
			int mouseButton = ((Vector*)data)->Z;
			float x = ((Vector*)data)->X;
			float y = ((Vector*)data)->Y;
			two = two.Sprintf( " mouse button %d down at (%f, %f).\n", mouseButton, x, y );
		}
		break;
	case EMenuMouseUp:
		{
			int mouseButton = ((Vector*)data)->Z;
			float x = ((Vector*)data)->X;
			float y = ((Vector*)data)->Y;
			two = two.Sprintf( " mouse button %d up at (%f, %f).\n", mouseButton, x, y );
		}
		break;
	default:
		two = "unknown event\n";
		break;
	}

	one += two;
	GLog.Print( one.AsChar() );

	return true;
}
*/

void MenuSystem::SetRootWindow( MenuWindow* window )
{
	if( mRootWindow == window )
		return;

	if( mRootWindow )	// Remove children from menu system this way
	{
		ForgetWindow( mRootWindow->mWindowIdent );
		mRootWindow->RefreshChildrenIndices();
	}

	mRootWindow = window;

	if( mRootWindow )	// Add children to menu system this way
	{
		RememberWindow( mRootWindow );
		mRootWindow->RefreshChildrenIndices();
	}
}

bool MenuSystem::SendEvent( MenuEvent eventType, int eventData1, void* eventData2 )
{
	if( !Active() || !mRootWindow )
		return false;

	switch( eventType )
	{
	case EMenuKeyDown:
	case EMenuKeyDownRepeated:
	case EMenuKeyUp:
		{
			// Check for mouse key
			int mouseButton = 0;

			switch( eventData1 )
			{
			case EMouseKeyLeft:		mouseButton = EMouseLeftButton; break;
			case EMouseKeyRight:	mouseButton = EMouseRightButton; break;
			case EMouseKeyMiddle:	mouseButton = EMouseMiddleButton; break;
			default:	break;
			}

			if( mouseButton )
			{
				// Get mouse position
				float x, y;
				GetMenuMousePosition( x, y );

				Vector rootPos = mRootWindow->GetPos();
				Vector rootSize = mRootWindow->GetSize();

				if( x < rootPos.X || y < rootPos.Y || x > rootPos.X+rootSize.X || y > rootPos.Y+rootSize.Y )
					return false;

				x = (x - rootPos.X)/rootSize.X;
				y = (y - rootPos.Y)/rootSize.Y;

				return mRootWindow->MouseEvent( x, y, mouseButton, (eventType == EMenuKeyDown) );
			}
			else if( mFocusWindow )
				return mFocusWindow->KeyEvent( eventData1, eventType );
		}
		break;
	default:
		if( mFocusWindow )
			return mFocusWindow->OtherEvent( eventType, eventData1, eventData2 );
		break;
	};
	return false;
}

MenuWindow* MenuSystem::MakeNewWindow( char type )
{
    m_setTag("Menu");

    switch( type )
	{
	case MenuWindow::SimpleWindow:			return new MenuWindow;
	case MenuWindow::Button:				return new MenuButton;
	case MenuWindow::ProgressBar:			return new MenuProgressBar;
	case MenuWindow::ListControl:			return new MenuListControl;
	case MenuWindow::EditField:				return new MenuEditField;
	case MenuWindow::ScrollBar:				return new MenuScrollBar;
	case MenuWindow::CheckBox:				return new MenuCheckBox;
//	case MenuWindow::GameSpyControl:		return new MenuGameSpyControl;
	case MenuWindow::RedefineKeysControl:	return new MenuRedefineKeysControl;
	default:
        m_setTag("");

        return 0;
	}
}

void MenuSystem::SetFocus( MenuWindow* window )
{
	if( mFocusWindow == window ) return;
	if( mFocusWindow )
	{
		mFocusWindow->Flags &= ~MenuWindow::HasFocus;
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'LostFocus' )", mFocusWindow->GetName() );
	}
	mFocusWindow = window;
	if( mFocusWindow )
	{
		mFocusWindow->Flags |= MenuWindow::HasFocus;
		GScript.DoStringA( "Menu_MenuEvent( '%s', 'GotFocus' )", mFocusWindow->GetName() );
	}
}

void MenuSystem::SetFocus( int windowIdent )
{
	Assert( ( windowIdent >= 0 ) && ( windowIdent < mAllWindows.Size() ) );
	SetFocus( mAllWindows[windowIdent] );
}

void MenuSystem::SetFocus( String windowName )
{
	MenuWindow* window = GetWindowByName( windowName );
	SetFocus( window );	
}

MenuWindow* MenuSystem::GetWindowUnderMouse( void )
{
	if( mActive && mRootWindow )
	{
		float x;
		float y;
		GetMenuMousePosition( x, y );

		Vector rootPos = mRootWindow->GetPos();
		Vector rootSize = mRootWindow->GetSize();

		if( x < rootPos.X || y < rootPos.Y || x > rootPos.X+rootSize.X || y > rootPos.Y+rootSize.Y )
			return 0;

		x = (x - rootPos.X)/rootSize.X;
		y = (y - rootPos.Y)/rootSize.Y;

		return mRootWindow->GetWindowUnderMouse( x, y );
	}
	return 0;
}

void MenuSystem::SetMouseTexture( int xClickOffset, int yClickOffset, const char *texName, DWORD texFlags )
{
	if( mMouseTexture )
		GEngine->Video->MaterialSys->ReleaseTexture( mMouseTexture );
	mMouseTexture = GEngine->Video->MaterialSys->CreateTexture( texName, texFlags | Texture::ETexNoMipMaps );
	mMouseClickOffsetX = xClickOffset;
	mMouseClickOffsetY = yClickOffset;
	if( mMouseTexture == GEngine->Video->MaterialSys->Textures.NoTex )
		mMouseTexture = 0;
}

void MenuSystem::ShowMouse( bool show )
{
	MouseShown = show;
	//GEngine->TempClient->InputSys->ShowMouse(false);	
}

void MenuSystem::RenderOnce( void )
{
	// Render no more often than 10/s
#ifdef TRANSGAMING
	static float    lastRenderTime = 0;
	float           renderTime = GEngine->SysDriver.GetCurrentTime();
#else
	static double lastRenderTime = 0;
	double renderTime = GEngine->SysDriver.GetCurrentTime();
#endif
	if( renderTime - lastRenderTime < 0.1 )
		return;

	// prepare window
	HWND hwnd = GEngine->Video->GetActiveView()->GetActiveWindow();
	RenderDevice* dev = GEngine->Video->RenderDev;
	if(GEngine->Video->IsFullScreen && !GEngine->Video->RenderWnd->HasFocus) return;
	GEngine->InputSys->SetWindow( hwnd );

	// setup camera
	GEngine->Video->GetActiveView()->mCamera.Fov=90;
	GEngine->Video->GetActiveView()->mCamera.InverseTransform();

	// setup viewport
	GEngine->Video->GetActiveView()->LocalViewport.ClearFlags=RenderDevice::EClearAll;
	GEngine->Video->GetActiveView()->LocalViewport.ClearColor=0x1a1a1a;
	GEngine->Video->GetActiveView()->LocalViewport.MinZ=0.1f;
	GEngine->Video->GetActiveView()->LocalViewport.MaxZ=4096.f;
	GEngine->Video->GetActiveView()->LocalViewport.Resize(hwnd);
	GEngine->Video->GetActiveView()->LocalViewport.Update(GEngine->Video->GetActiveView()->mCamera);

    if (dev->Lock(GEngine->Video->GetActiveView()->LocalViewport))
	{
		GEngine->Video->Menu.Render();
		dev->Unlock();
		dev->Flip(hwnd);
	}
}

void MenuSystem::ShowOnly( String windowName )
{
	MenuWindow* window = GetWindowByName( windowName );
	if( !window ) return;
	MenuWindow* parent = window->mParentWindow;
	if( !parent ) return;
	for( int i = 0; i < parent->mChildWindows.Size(); ++i )
	{
		if( strcmp(parent->mChildWindows[i]->GetName(), windowName) )
			parent->mChildWindows[i]->Hide();
		else
			parent->mChildWindows[i]->Show();
	}
}

void MenuSystem::GetMenuMousePosition( float& x, float& y )
{
	POINT p = GEngine->InputSys->GetMousePosition();
    x = ((float)p.x)/GEngine->Video->GetActiveView()->LocalViewport.Width;
	y = ((float)p.y)/GEngine->Video->GetActiveView()->LocalViewport.Height;
}
