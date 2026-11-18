#include "Engine.h"
#include "Render/HUD.h"
#include "Render/Video.h"
#include "Render/Material.h"

#include "Engine/PainMenu.h"

#define COR_LT_X_SHIFT		3
#define COR_LT_Y_SHIFT		3
#define COR_LB_X_SHIFT		3
#define COR_LB_Y_SHIFT		22
#define COR_RT_X_SHIFT		23
#define COR_RT_Y_SHIFT		5
#define COR_RB_X_SHIFT		24
#define COR_RB_Y_SHIFT		25
#define BOR_L_SHIFT			7
#define BOR_R_SHIFT			11
#define BOR_B_SHIFT			11
#define BOR_T_SHIFT			7


//--------------------------------------------------------------------
//	MenuItemBorder class
//--------------------------------------------------------------------
MenuItemBorder::MenuItemBorder() : MenuItem()
{
	mType = EMenuItemBorder;
	mScroller = NULL;
	mHeader = 0;


    mW = 0;
    mH = 0;

    mIsDark = false;

	MaterialSystem *matSys = GEngine->Video->MaterialSys;
	mTextures[kCornerLB] = matSys->CreateTexture( "HUD/border/naroznik_lewy_dol", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mTextures[kCornerLT] = matSys->CreateTexture( "HUD/border/naroznik_lewy_gora", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mTextures[kCornerRB] = matSys->CreateTexture( "HUD/border/naroznik_prawy_dol", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mTextures[kCornerRT] = matSys->CreateTexture( "HUD/border/naroznik_prawy_gora", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mTextures[kBorderB] = matSys->CreateTexture( "HUD/border/ramka_dolna_srodek", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mTextures[kBorderT] = matSys->CreateTexture( "HUD/border/ramka_gorna_srodek", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mTextures[kBorderL] = matSys->CreateTexture( "HUD/border/ramka_lewa", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mTextures[kBorderR] = matSys->CreateTexture( "HUD/border/ramka_prawa", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mTextures[kBG] = matSys->CreateTexture( "HUD/border/tlo_paski", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mTextures[kBGDark] = matSys->CreateTexture( "HUD/border/tlo_paski_ciemne", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
}


MenuItemBorder::~MenuItemBorder()
{
	Clear();
}


void MenuItemBorder::Clear()
{
	if( GEngine->Video->MaterialSys )
	{
		for( int i=0; i<9; i++ )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mTextures[i] );
			mTextures[i] = NULL;
		}
	}

	mColumns.ClearArray();
}


void MenuItemBorder::Render()
{
	if( !mVisible )
		return;

	HUD *hud = GEngine->Video->Hud;

	if( mScroller )
		mScroller->SetPosition( mPosX + mW - ( int )ceil( 16.0f * 1.0f / mScaleX ), mPosY - 14 );

	int bw = (int)(mW * mScaleX);
	int bh = (int)(mH * mScaleY);

	int x, y, w, h;
	Texture *tex;

	// Background
	if( mHeader > 0 )
	{
		hud->DrawTiles( mTextures[kBGDark], mX, mY, bw, ( float )mHeader * mScaleY, false );
	}

	if( mColumns.Size() == 0 )
	{
		int bg = mIsDark ? kBGDark : kBG;
		hud->DrawTiles( mTextures[bg], mX, mY + ceil( ( float )mHeader * mScaleY ), bw, bh - ( float )mHeader * mScaleY, false );
	}
	else
	{
		int currCol = 0;
		for( int i=0; i<mColumns.Size(); i++ )
		{
			int cw = (int)(mColumns[i] * mScaleX);
			if( i == mColumns.Size() - 1 )
				cw = bw - currCol;
			int bg = ( ( i % 2 ) == 0 ) ? kBG : kBGDark;
			hud->DrawTiles( mTextures[bg], mX + currCol, mY + ceil( ( float )mHeader * mScaleY ), cw, bh - ( float )mHeader * mScaleY, false );
			currCol += (int)(mColumns[i] * mScaleX);
		}
	}

	// Columns
	int currCol = 0;
	for( int i=0; i<mColumns.Size() - 1; i++ )
	{
		currCol += (int)(mColumns[i] * mScaleX);
		hud->DrawTiles( mTextures[kBorderL], mX - BOR_L_SHIFT + currCol, mY, 0.0f, bh, false );
	}

	// Header
	if( mHeader > 0 )
	{
		hud->DrawTiles( mTextures[kBorderT], mX, mY - BOR_T_SHIFT + mHeader * mScaleY, bw, 0.0f, false );
	}

	// Left border
	if( bh > mTextures[kCornerLT]->OriginalHeight + COR_LT_Y_SHIFT )
		hud->DrawTiles( mTextures[kBorderL], mX - BOR_L_SHIFT, mY + mTextures[kCornerLT]->OriginalHeight - COR_LT_Y_SHIFT, 0.0f, bh - mTextures[kCornerLT]->OriginalHeight - COR_LT_Y_SHIFT, false );

	// Right border
	if( bh > mTextures[kCornerRT]->OriginalHeight + COR_RT_Y_SHIFT )
		hud->DrawTiles( mTextures[kBorderR], mX + bw - BOR_R_SHIFT, mY + mTextures[kCornerRT]->OriginalHeight - COR_RT_Y_SHIFT, 0.0f, bh - mTextures[kCornerRT]->OriginalHeight - COR_RT_Y_SHIFT, false );

	// Bottom border
	hud->DrawTiles( mTextures[kBorderB], mX + mTextures[kCornerLB]->OriginalWidth - COR_LB_X_SHIFT, mY - BOR_B_SHIFT + bh, bw - mTextures[kCornerLB]->OriginalWidth - COR_LB_X_SHIFT, 0.0f, false );

	// Top border
	hud->DrawTiles( mTextures[kBorderT], mX, mY - BOR_T_SHIFT, bw, 0.0f, false );

	// Left-bottom corner
	tex = mTextures[kCornerLB];
	x = mX - COR_LB_X_SHIFT;
	y = mY + bh - COR_LB_Y_SHIFT;
	w = tex->OriginalWidth;
	h = tex->OriginalHeight;
	hud->DrawQuad( tex, x, y, w, h );

	// Left-top corner
	tex = mTextures[kCornerLT];
	x = mX - COR_LT_X_SHIFT;
	y = mY - COR_LT_Y_SHIFT;
	w = tex->OriginalWidth;
	h = tex->OriginalHeight;
	hud->DrawQuad( tex, x, y, w, h );

	// Right-top corner
	tex = mTextures[kCornerRT];
	x = mX + bw - COR_RT_X_SHIFT;
	y = mY - COR_RT_Y_SHIFT;
	w = tex->OriginalWidth;
	h = tex->OriginalHeight;
	hud->DrawQuad( tex, x, y, w, h );

	// Right-bottom corner
	tex = mTextures[kCornerRB];
	x = mX + bw - COR_RB_X_SHIFT;
	y = mY + bh - COR_RB_Y_SHIFT;
	w = tex->OriginalWidth;
	h = tex->OriginalHeight;
	hud->DrawQuad( tex, x, y, w, h );

	if( mScroller )
	{
		mScroller->UpdateScale();
		mScroller->CalcSize();
		mScroller->CalcPosition();
		mScroller->SetUnderMouse( false );
		mScroller->CheckIfUnderMouse();
		mScroller->Render();
	}
}


void MenuItemBorder::AddScroller( MenuItemScroller *s )
{
	mScroller = s;
	mScroller->SetPosition( mPosX + mW - ( int )ceil( 14.0f * 1.0f / mScaleX ), mPosY - 14 );
	mScroller->SetScrollerHeight( mH + 32 );
}


void MenuItemBorder::SetColumnsCount( int c )
{
	mColumns.ClearArray();

	for( int i=0; i<c; i++ )
	{
		mColumns.Add( 0 );
	}
}


void MenuItemBorder::SetColumnWidth( int c, int w )
{
	mColumns[c] = w;
}


//--------------------------------------------------------------------
//	MenuItemTabGroup class
//--------------------------------------------------------------------
MenuItemTabGroup::MenuItemTabGroup() : MenuItemBorder()
{
	mActive = false;
	mType = EMenuItemTabGroup;
}


MenuItemTabGroup::~MenuItemTabGroup()
{
	Clear();
}


void MenuItemTabGroup::Clear()
{
	MenuItemBorder::Clear();

	mItems.ClearArray();
}


void MenuItemTabGroup::Render()
{
	if( !mVisible )
		return;

	HUD *hud = GEngine->Video->Hud;

	if( mScroller )
		mScroller->SetPosition( mPosX + mW - ( int )ceil( 16.0f * 1.0f / mScaleX ), mPosY - 14 );

	int bw = (int)(mW * mScaleX);
	int bh = (int)(mH * mScaleY);

	int x, y, w, h;
	Texture *tex;

	int tabHeight = (int)(50 * mScaleY);
	int tabWidth = (int)(180 * mScaleX);
	int nonShift = (int)(10 * mScaleY);

	// Background
	int bg = mIsDark ? kBGDark : kBG;
	hud->DrawTiles( mTextures[bg], mX, mY + mHeader * mScaleY + tabHeight, bw, bh - mHeader * mScaleY - tabHeight, false );

	// Left border
	hud->DrawTiles( mTextures[kBorderL], mX - BOR_L_SHIFT, mY + tabHeight, 0.0f, bh - tabHeight, false );

	// Right border
	hud->DrawTiles( mTextures[kBorderR], mX + bw - BOR_R_SHIFT, mY + mTextures[kCornerRT]->OriginalHeight - COR_RT_Y_SHIFT + tabHeight, 0.0f, bh - mTextures[kCornerRT]->OriginalHeight - COR_RT_Y_SHIFT - tabHeight, false );

	// Bottom border
	hud->DrawTiles( mTextures[kBorderB], mX + mTextures[kCornerLB]->OriginalWidth - COR_LB_X_SHIFT, mY - BOR_B_SHIFT + bh, bw - mTextures[kCornerLB]->OriginalWidth - COR_LB_X_SHIFT, 0.0f, false );

	// Top border
	hud->DrawTiles( mTextures[kBorderT], mX, mY - BOR_T_SHIFT + tabHeight, bw, 0.0f, false );

	// Left-bottom corner
	tex = mTextures[kCornerLB];
	x = mX - COR_LB_X_SHIFT;
	y = mY + bh - COR_LB_Y_SHIFT;
	w = tex->OriginalWidth;
	h = tex->OriginalHeight;
	hud->DrawQuad( tex, x, y, w, h );

	// Left-top corner
	tex = mTextures[kCornerLT];
	x = mX - COR_LT_X_SHIFT;
	y = mY - COR_LT_Y_SHIFT + tabHeight;
	w = tex->OriginalWidth;
	h = tex->OriginalHeight;
	hud->DrawQuad( tex, x, y, w, h );

	// Right-top corner
	tex = mTextures[kCornerRT];
	x = mX + bw - COR_RT_X_SHIFT;
	y = mY - COR_RT_Y_SHIFT + tabHeight;
	w = tex->OriginalWidth;
	h = tex->OriginalHeight;
	hud->DrawQuad( tex, x, y, w, h );

	// Right-bottom corner
	tex = mTextures[kCornerRB];
	x = mX + bw - COR_RB_X_SHIFT;
	y = mY + bh - COR_RB_Y_SHIFT;
	w = tex->OriginalWidth;
	h = tex->OriginalHeight;
	hud->DrawQuad( tex, x, y, w, h );

	if( mAlign == EAlignLeft )
	{
		hud->DrawTiles( mTextures[kBGDark], mX, mY, tabWidth, tabHeight, false );
		hud->DrawTiles( mTextures[kBG], mX + tabWidth, mY + nonShift, tabWidth, tabHeight - nonShift, false );

		// Right border
		hud->DrawTiles( mTextures[kBorderR], mX + tabWidth * 2 - BOR_R_SHIFT, mY + nonShift, 0.0f, tabHeight - nonShift, false );

		// Top border
		hud->DrawTiles( mTextures[kBorderT], mX + tabWidth, mY - BOR_T_SHIFT + nonShift, tabWidth, 0.0f, false );

		// Left border
		hud->DrawTiles( mTextures[kBorderL], mX - BOR_L_SHIFT, mY, 0.0f, tabHeight, false );

		// Right border
		hud->DrawTiles( mTextures[kBorderR], mX + tabWidth - BOR_R_SHIFT, mY, 0.0f, tabHeight, false );

		// Top border
		hud->DrawTiles( mTextures[kBorderT], mX, mY - BOR_T_SHIFT, tabWidth, 0.0f, false );

		// Left-top corner
		tex = mTextures[kCornerLT];
		x = mX - COR_LT_X_SHIFT;
		y = mY - COR_LT_Y_SHIFT;
		w = tex->OriginalWidth;
		h = tex->OriginalHeight;
		hud->DrawQuad( tex, x, y, w, h );
	
		// Right-top corner
		tex = mTextures[kCornerRT];
		x = mX + tabWidth - COR_RT_X_SHIFT;
		y = mY - COR_RT_Y_SHIFT;
		w = tex->OriginalWidth;
		h = tex->OriginalHeight;
		hud->DrawQuad( tex, x, y, w, h );

		// Right-top corner
		tex = mTextures[kCornerRT];
		x = mX + tabWidth * 2 - COR_RT_X_SHIFT;
		y = mY - COR_RT_Y_SHIFT + nonShift;
		w = tex->OriginalWidth;
		h = tex->OriginalHeight;
		hud->DrawQuad( tex, x, y, w, h );
	}
	else
	{
		hud->DrawTiles( mTextures[kBGDark], mX + tabWidth, mY, tabWidth, tabHeight, false );
		hud->DrawTiles( mTextures[kBG], mX, mY + nonShift, tabWidth, tabHeight - nonShift, false );

		// Top border
		hud->DrawTiles( mTextures[kBorderT], mX, mY - BOR_T_SHIFT + nonShift, tabWidth, 0.0f, false );

		// Left border
		hud->DrawTiles( mTextures[kBorderL], mX - BOR_L_SHIFT, mY + nonShift, 0.0f, tabHeight - nonShift, false );
		hud->DrawTiles( mTextures[kBorderL], mX + tabWidth - BOR_L_SHIFT, mY, 0.0f, tabHeight, false );

		// Right border
		hud->DrawTiles( mTextures[kBorderR], mX + tabWidth * 2 - BOR_R_SHIFT, mY, 0.0f, tabHeight, false );

		// Top border
		hud->DrawTiles( mTextures[kBorderT], mX + tabWidth, mY - BOR_T_SHIFT, tabWidth, 0.0f, false );

		// Left-top corner
		tex = mTextures[kCornerLT];
		x = mX - COR_LT_X_SHIFT;
		y = mY - COR_LT_Y_SHIFT + nonShift;
		w = tex->OriginalWidth;
		h = tex->OriginalHeight;
		hud->DrawQuad( tex, x, y, w, h );

		// Left-top corner
		tex = mTextures[kCornerLT];
		x = mX + tabWidth - COR_LT_X_SHIFT;
		y = mY - COR_LT_Y_SHIFT;
		w = tex->OriginalWidth;
		h = tex->OriginalHeight;
		hud->DrawQuad( tex, x, y, w, h );
	
		// Right-top corner
		tex = mTextures[kCornerRT];
		x = mX + tabWidth * 2 - COR_RT_X_SHIFT;
		y = mY - COR_RT_Y_SHIFT;
		w = tex->OriginalWidth;
		h = tex->OriginalHeight;
		hud->DrawQuad( tex, x, y, w, h );
	}

	if( mScroller )
	{
		mScroller->UpdateScale();
		mScroller->CalcSize();
		mScroller->CalcPosition();
		mScroller->SetUnderMouse( false );
		mScroller->CheckIfUnderMouse();
		mScroller->Render();
	}
}


void MenuItemTabGroup::AddItem( MenuItem *item )
{
	for( int i=0; i<mItems.Size(); i++ )
	{
		if( mItems[i] == item )
			return;
	}

	mItems.Add( item );
}


void MenuItemTabGroup::Activate( bool active )
{
}
