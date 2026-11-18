#include "Engine.h"
#include "Render/HUD.h"
#include "Render/View.h"
#include "Engine/InputSystem.h"
#include "Render/Video.h"
#ifdef TRANSGAMING_PS2
# include "Render/Material.h"
#endif
#include "Engine/PainMenu.h"

//--------------------------------------------------------------------
//	MenuItemList class
//--------------------------------------------------------------------
MenuItemList::MenuItemList() : MenuItem()
{
	mType = EMenuItemList;
	mSelected = -1;
	mDoubleClicked = -1;
	mMaxHeight = 0;
	mNumVisible = 0;
	mScroller = NULL;
	mUseHeader = false;
	mBorder = NULL;
	mBorderWidth = 300;
}


MenuItemList::~MenuItemList()
{
	for( int i=0; i<mItems.Size(); i++ )
	{
		delete mItems[i];
	}
	mItems.ClearArray();

	if( mScroller )
	{
		delete mScroller;
		mScroller = NULL;
	}

	if( mBorder )
	{
		delete mBorder;
		mBorder = NULL;
	}
}


void MenuItemList::Render()
{
	if( !mVisible )
		return;

	UpdateScale();
	CalcPosition();

	if( mBorder )
		mBorder->Render();

	if( mUseHeader && mItems.Size() > 0 )
	{
		DrawElem( mItems[0], mX, (int)(mY - 4 * mScaleY), true );
	}

	int first = 0;
	int offset = 0;
	if( mScroller ) first = mScroller->GetValue();
	int count = mItems.Size();
	if( count > mNumVisible ) count = mNumVisible;
	if( mUseHeader )
	{
		first++;
		count--;
		offset = (int)(16 * mScaleY);
	}

	for( int i=first, curr=( mUseHeader ? 1 : 0 ); i<(count+first); i++, curr++ )
	{
		if( i < mItems.Size() )
			DrawElem( mItems[i], mX, mY + offset + curr * GEngine->Video->Hud->GetTextHeight() );
	}
}


void MenuItemList::DrawElem( MenuItemList::MenuListItem *elem, int x, int y, bool header )
{
	CalcSize();
	SetUnderMouse( false );
	CheckIfUnderMouse();

	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	// Shadow
	if( mDrawShadow )
		GEngine->Video->Hud->Print( x + 1, y + 1, elem->name.AsChar(), CalcTransColor( 0x50000000 ) );

	bool underMouse = false;
	if( mUnderMouse )
	{
		POINT mousePos = GEngine->InputSys->GetMousePosition();

		if( y < mousePos.y && ( y + GEngine->Video->Hud->GetTextHeight() ) > mousePos.y )
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
	else if( header )
		color = CalcTransColor( mTextColor );
	else
		color = CalcTransColor( mDisabledColor );

	GEngine->Video->Hud->Print( x, y, elem->name.AsChar(), color );
}


void MenuItemList::CalcSize()
{
	if( mItems.Size() == 0 )
	{
		mWidth = 0;
		mHeight = 0;
		mNumVisible = 0;
	}
	else
	{
		mWidth = (int)(( mBorderWidth - 40 ) * mScaleX);
		GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );
		mNumVisible = ( int )floor( ( ( float )mMaxHeight * mScaleY ) / ( float )GEngine->Video->Hud->GetTextHeight() );

		mHeight = (int)(( mMaxHeight + 14 ) * mScaleY);

		if( mBorder )
		{
			mBorder->SetSize( mBorderWidth, mMaxHeight + 40 );
			if( mUseHeader )
				mBorder->SetHeader( 40 );
		}
	}
}


void MenuItemList::CalcPosition()
{
    if( !mBorder ){
        m_setTag("Menu");
		mBorder = new MenuItemBorder();
    }

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	if( mPosX == -1 )
		mX = (int)(( ( screenWidth - GEngine->Video->PMenu->Screen.GetMenuWidth() * mScaleX ) / 2 ));
	else
		mX = (int)(mPosX * mScaleX);
	mY = (int)(mPosY * mScaleY);

	mBorder->SetPosition( (int)(( float )mPosX - 20), (int)(( float )mPosY - 20) );
}


bool MenuItemList::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	if( mScroller )
	{
		mScroller->SendEvent( eventType, eventData1, eventData2 );
	}

	switch( eventType )
	{
	case EMenuKeyDown:
	case EMenuKeyDownRepeated:
		break;

	case EMenuKeyUp:
		if( eventData1 != EMouseKeyLeft )
			break;

		mDoubleClicked = -1;

		if( mUnderMouse && !mDisabled )
		{
			GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

			int first = 0;
			if( mScroller ) first = mScroller->GetValue();
			int count = mItems.Size();
			if( count > mNumVisible ) count = mNumVisible;
			int oldSelected = mSelected;
			mSelected = -1;
			int offset = (int)(14 * mScaleY);
			for( int i=first, curr=0; i<(count+first); i++, curr++ )
			{
				POINT mousePos = GEngine->InputSys->GetMousePosition();

				int posy = mY + offset + curr * GEngine->Video->Hud->GetTextHeight();

				if( posy < mousePos.y && ( posy + GEngine->Video->Hud->GetTextHeight() ) > mousePos.y )
				{
					mSelected = i;
					if( oldSelected == i )
						mDoubleClicked = i;
				}
			}
			if( mDoubleClicked < 1 )
				mDoubleClicked = -1;
		}
		break;

	default:
		break;
	}

	return false;
}


void MenuItemList::AddItem( const char *name, void *obj )
{
	for( int i=0; i<mItems.Size(); i++ )
	{
		if( !strcmp(mItems[i]->name.AsChar(), name) )
			return;
	}

    m_setTag("Menu");
	MenuListItem *item = new MenuListItem;

	item->name = name;
	item->obj = obj;

	mItems.Add( item );

	CalcSize();

	if( ( mItems.Size() ) > mNumVisible && mItems.Size() > 1 )
	{
		if( !mScroller )
		{
			int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;

            m_setTag("Menu");
			mScroller = new MenuItemScroller;

#ifdef TRANSGAMING
			mScroller->SetName( (mName+"Scroller").AsChar() );
#else
			mScroller->SetName( mName+"Scroller" );
#endif
			mScroller->SetText( "" );
			mScroller->SetDesc( "" );
			mScroller->SetValue( 0 );
			mScroller->SetScrollerHeight( mMaxHeight );
			mScroller->SetPosition( (int)(mPosX + ( mWidth + 10 ) * 1024.0f / screenWidth), mPosY );
			mScroller->SetColors( mTextColor, mDisabledColor, mTextColorUnderMouse, mDescColor );
			mScroller->CalcSize();
			mScroller->CalcPosition();

			if( mBorder )
				mBorder->AddScroller( mScroller );
		}

		mScroller->SetMinMax( 0, mItems.Size() - mNumVisible );
	}

	mDoubleClicked = -1;
}


void MenuItemList::RemoveItem( int index )
{
	if( index > -1 && index < mItems.Size() )
	{
		delete mItems[index];
		mItems.Remove( index );
	}

	if( ( mItems.Size() ) <= mNumVisible )
	{
		delete mScroller;
		mScroller = NULL;
		mBorder->RemoveScroller();
	}
	else
	{
		mScroller->SetMinMax( 0, mItems.Size() - mNumVisible );
	}

	mSelected = -1;
	mDoubleClicked = -1;
}


void MenuItemList::MoveItemUp( int index )
{
	if( index <= 0 || ( index == 1 && mUseHeader ) )
		return;

	mItems.Swap( index, index - 1 );
	mSelected--;
	
	if( mScroller )
	{
		int first = mScroller->GetValue();
		if( mSelected < first )
			mScroller->ScrollUp();
	}

	mDoubleClicked = -1;
}


void MenuItemList::MoveItemDown( int index )
{
	if( index >= mItems.Size() - 1 || mItems.Size() <= 1 || ( index == 0 && mUseHeader ) || index < 0 )
		return;

	mItems.Swap( index, index + 1 );
	mSelected++;

	if( mScroller )
	{
		int first = mScroller->GetValue();
		if( mSelected >= first + mNumVisible )
			mScroller->ScrollDown();
	}

	mDoubleClicked = -1;
}


void MenuItemList::RemoveAll()
{
	int first = 0;
	if( mUseHeader )
		first++;
	while( mItems.Size() > first )
	{
		RemoveItem( first );
	}

	mDoubleClicked = -1;
}


void MenuItemList::SetPosition( int x, int y )
{
	int tmp = 0;
	MenuItem::SetPosition( tmp, y );

	CalcSize();

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	if( x == -1 )
		x = (int)(( ( screenWidth - GEngine->Video->PMenu->Screen.GetMenuWidth() * ( screenWidth / 1024.0f ) ) / 2 ));
	else
		MenuItem::SetPosition( x, y );

	if( mScroller )
	{
		mScroller->SetPosition( x + ( int )( ( float )mWidth * 1024.0f / ( float )screenWidth ), mPosY );
	}
}


void MenuItemList::SetColors( DWORD text, DWORD disabled, DWORD underMouse, DWORD desc )
{
	MenuItem::SetColors( text, disabled, underMouse, desc );

	if( mScroller )
		mScroller->SetColors( mTextColor, mDisabledColor, mTextColorUnderMouse, mDescColor );
}


void MenuItemList::SetMaxHeight( int height )
{
	mMaxHeight = height;

	UpdateScale();
	CalcSize();

	if( mScroller )
	{
		mScroller->SetScrollerHeight( height );
		mScroller->SetMinMax( 0, mItems.Size() - mNumVisible );
	}
}


void MenuItemList::SetBorderWidth( int width )
{
	mBorderWidth = width;

	UpdateScale();
	CalcSize();
}


MenuItemList::MenuListItem *MenuItemList::GetFirst() const
{
	int index = 0;
	if( mUseHeader )
		index = 1;

	if( mItems.Size() > index )
		return mItems[index];

	return NULL;
}


//--------------------------------------------------------------------
//	MenuItemWeaponList class
//--------------------------------------------------------------------
MenuItemWeaponList::MenuItemWeaponList()
{
	mType = EMenuItemWeaponList;
	mSeparatorPos = 0;
}


MenuItemWeaponList::~MenuItemWeaponList()
{
}


void MenuItemWeaponList::Render()
{
	if( !mVisible )
		return;

	UpdateScale();
	CalcPosition();

	if( mBorder )
		mBorder->Render();

	if( mUseHeader && mItems.Size() > 0 )
	{
		DrawElem( mItems[0], mX, (int)(mY - 4 * mScaleY), true );
	}

	int first = 0;
	int offset = 0;
	if( mScroller ) first = mScroller->GetValue();
	int count = mItems.Size();
	if( count > mNumVisible ) count = mNumVisible;
	if( mUseHeader )
	{
		first++;
		count--;
		offset = (int)(16 * mScaleY);
	}

	DWORD oldColor = mTextColor;
	DWORD oldSelColor = mDisabledColor;

	for( int i=first, curr=( mUseHeader ? 1 : 0 ); i<(count+first); i++, curr++ )
	{
		if( i < mSeparatorPos + 1 )
		{
			mTextColor = oldColor;
			mDisabledColor = oldSelColor;
		}
		else
		{
			mTextColor = 0xff505050;
			mDisabledColor = 0xffa0a0a0;
		}

		if( i < mItems.Size() )
			DrawElem( mItems[i], mX, mY + offset + curr * GEngine->Video->Hud->GetTextHeight() );
	}

	mTextColor = oldColor;
	mDisabledColor = oldSelColor;

//	GEngine->Video->Hud->Print( mX, mY + offset + ( mSeparatorPos + first - 1 ) * GEngine->Video->Hud->GetTextHeight(), "_______________", 0xffffffff );
	if( mSeparatorPos >= first )
		if( offset + ( mSeparatorPos - first + 2 ) * GEngine->Video->Hud->GetTextHeight() < mHeight )
#ifdef TRANSGAMING_PS2
			GEngine->Video->Hud->DrawQuad( GEngine->Video->MaterialSys->Textures.White, mX, mY + offset + ( mSeparatorPos - first + 2 ) * GEngine->Video->Hud->GetTextHeight() - 2, mWidth + 3, 1, mTextColor );
#else
			GEngine->Video->Hud->DrawQuad( NULL, mX, mY + offset + ( mSeparatorPos - first + 2 ) * GEngine->Video->Hud->GetTextHeight() - 2, mWidth + 3, 1, mTextColor );
#endif
}


void MenuItemWeaponList::MoveItemUp( int index )
{
	if( index <= 0 || ( index == 1 && mUseHeader ) )
	{
		if( mSeparatorPos == 0 && mItems.Size() > 1 )
			mSeparatorPos++;

		return;
	}

	if( index == mSeparatorPos + 1 )
	{
		mSeparatorPos++;
	}
	else
	{
		mItems.Swap( index, index - 1 );
		mSelected--;
		
		if( mScroller )
		{
			int first = mScroller->GetValue();
			if( mSelected < first )
				mScroller->ScrollUp();
		}
	}

	mDoubleClicked = -1;
}


void MenuItemWeaponList::MoveItemDown( int index )
{
	if( index >= mItems.Size() - 1 || mItems.Size() <= 1 || ( index == 0 && mUseHeader ) || index < 0 )
	{
		if( mSeparatorPos == mItems.Size() - 1 && mSeparatorPos > 0 )
			mSeparatorPos--;

		return;
	}

	if( index == mSeparatorPos )
	{
		if( mSeparatorPos > 0 )
			mSeparatorPos--;
	}
	else
	{
		mItems.Swap( index, index + 1 );
		mSelected++;

		if( mScroller )
		{
			int first = mScroller->GetValue();
			if( mSelected >= first + mNumVisible )
				mScroller->ScrollDown();
		}
	}

	mDoubleClicked = -1;
}
