#include "Engine.h"
#include "Render/HUD.h"
#include "Render/View.h"
#include "Render/Video.h"
#include "Render/Material.h"

#include "Engine/PainMenu.h"

//--------------------------------------------------------------------
//	MenuItemImageButton class
//--------------------------------------------------------------------
MenuItemImageButton::MenuItemImageButton() : MenuItem()
{
	mType = EMenuItemImageButton;
	mImage = NULL;
	mImageUnder = NULL;
	mImagePressed = NULL;
	mImageGlow = NULL;
	mIsPressed = false;
	mFade = 0;
	mFadeStart = 0.0f;
	mCurrTime = 0.0f;
	mFadeLen = 1.0f;
	mGlowTime = 0.0f;
	mGlowDir = 0;
	mGlowTrans = 0;
	mGlowStart = 0.0f;
}


MenuItemImageButton::~MenuItemImageButton()
{
	if( mImage && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( mImage );
	mImage = NULL;

	if( mImageUnder && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( mImageUnder );
	mImageUnder = NULL;

	if( mImagePressed && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( mImagePressed );
	mImagePressed = NULL;

	if( mImageGlow && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( mImageGlow );
	mImageGlow = NULL;
}


void MenuItemImageButton::CalcSize()
{
	if( mImage )
	{
		mWidth = (int)(mImage->OriginalWidth * mScaleX);
		mHeight = (int)(mImage->OriginalHeight * mScaleY);
	}
	else
	{
		mWidth = 0;
		mHeight = 0;
	}
}


bool MenuItemImageButton::CheckIfUnderMouse()
{
	if( !mVisible )
		return false;
/*
	POINT mousePos = GEngine->InputSys->GetMousePosition();

	if( mousePos.x < mX || mousePos.y < mY || mousePos.x > ( mX + mWidth ) || mousePos.y > ( mY + mHeight ) )
		mUnderMouse = false;
	else
		mUnderMouse = true;

	return mUnderMouse;
*/
	return MenuItem::CheckIfUnderMouse();
/*	{
		char txt[255];
		sprintf( txt, "ImageButton: %s, x: %d, y: %d", mName.AsChar(), mX, mY );
		GEngine->Video->Hud->Print( mX, mY + 30 * img_count, txt );
		img_count++;
		return true;
	}
	else
		return false;*/
}


void MenuItemImageButton::Render()
{
	if( !mVisible )
		return;

	float timeDiff = 0.0f;
	static const float maxTime = 0.2f;

	UpdateScale();
	CalcPosition();

	if( mImage && mImageUnder && mImagePressed )
	{
		if( mUnderMouse && !mDisabled )
		{
			if( !mIsPressed )
			{
				if( mFade == false )
				{
					mFadeStart = GEngine->SysDriver.GetCurrentTime();
					mCurrTime = mFadeStart;
					mFadeLen = maxTime;
					mFade = true;
				}
				else
				{
					mCurrTime = GEngine->SysDriver.GetCurrentTime();
				}

				timeDiff = mCurrTime - mFadeStart;
				if( timeDiff < 0.0f ) timeDiff = 0.0f;

				if( timeDiff > mFadeLen )
				{
					GEngine->Video->Hud->DrawQuad( mImageUnder, ( float )mX, ( float )mY, ( float )mWidth, ( float )mHeight );
				}
				else
				{
					GEngine->Video->Hud->DrawQuad( mImage, ( float )mX, ( float )mY, ( float )mWidth, ( float )mHeight );
					BYTE power = ( BYTE )( 255.0f * timeDiff / mFadeLen );
					DWORD color = 0x00FFFFFF | ( DWORD )( power << 24 );
					GEngine->Video->Hud->DrawQuad( mImageUnder, ( float )mX, ( float )mY, ( float )mWidth, ( float )mHeight, color );
				}
			}
			else
			{
				GEngine->Video->Hud->DrawQuad( mImagePressed, ( float )mX, ( float )mY, ( float )mWidth, ( float )mHeight );
				mFade = true;
			}
		}
		else
		{
			if( mFade == true && !mDisabled )
			{
				mFadeLen = mCurrTime - mFadeStart;
				if( mFadeLen > maxTime ) mFadeLen = maxTime;
				mFadeStart = GEngine->SysDriver.GetCurrentTime();
				mCurrTime = mFadeStart;
				mFade = false;
			}
			else
			{
				mCurrTime = GEngine->SysDriver.GetCurrentTime();
			}

			timeDiff = mCurrTime - mFadeStart;
			if( timeDiff < 0.0f ) timeDiff = 0.0f;

			if( timeDiff > mFadeLen || mDisabled )
			{
				GEngine->Video->Hud->DrawQuad( mImage, ( float )mX, ( float )mY, ( float )mWidth, ( float )mHeight );
			}
			else
			{
				GEngine->Video->Hud->DrawQuad( mImage, ( float )mX, ( float )mY, ( float )mWidth, ( float )mHeight );
				BYTE power = (BYTE)(255 * ( mFadeLen / maxTime ) - ( BYTE )( 255.0f * timeDiff / maxTime ));
				DWORD color = 0x00FFFFFF | ( DWORD )( power << 24 );
				GEngine->Video->Hud->DrawQuad( mImageUnder, ( float )mX, ( float )mY, ( float )mWidth, ( float )mHeight, color );
			}
		}
	}

	if( mImageGlow )
	{
		float time = GEngine->SysDriver.GetCurrentTime();
		if( mGlowTrans <= 0 && time - mGlowStart > 0.5f )
		{
			mGlowDir = 1;
			mGlowTrans = 0;
		}
		if( mGlowTrans >= 255 || mUnderMouse )
		{
			mGlowDir = -1;
			mGlowTrans = 255;
			mGlowStart = time;
		}

		if( time - mGlowTime > 0.02f )
		{
			mGlowTrans += (int)(mGlowDir * 300 * ( time - mGlowTime ));

			if( mGlowTrans < 0 )
				mGlowTrans = 0;

			if( mGlowTrans > 255 )
				mGlowTrans = 255;

			mGlowTime = time;
		}

		DWORD color = 0x00FFFFFF | ( DWORD )( mGlowTrans << 24 );
		GEngine->Video->Hud->DrawQuad( mImageUnder, ( float )mX, ( float )mY, ( float )mWidth, ( float )mHeight, color );
	}

	MenuItem::Render();
}


void MenuItemImageButton::SetImage( const char *normal, const char *under, const char *pressed )
{
	Texture *image = GEngine->Video->MaterialSys->CreateTexture( normal, Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	Texture *imageUnder = GEngine->Video->MaterialSys->CreateTexture( under, Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	Texture *imagePressed = GEngine->Video->MaterialSys->CreateTexture( pressed, Texture::ETexNoMipMaps | Texture::ETexNoLOD );

	if( mImage != image )
	{
		if( mImage ) GEngine->Video->MaterialSys->ReleaseTexture( mImage );
		mImage = image;
	}
	else
	{
		GEngine->Video->MaterialSys->ReleaseTexture( image );
	}

	if( mImageUnder != imageUnder )
	{
		if( mImageUnder ) GEngine->Video->MaterialSys->ReleaseTexture( mImageUnder );
		mImageUnder = imageUnder;
	}
	else
	{
		GEngine->Video->MaterialSys->ReleaseTexture( imageUnder );
	}

	if( mImagePressed != imagePressed )
	{
		if( mImagePressed  ) GEngine->Video->MaterialSys->ReleaseTexture( mImagePressed );
		mImagePressed = imagePressed;
	}
	else
	{
		GEngine->Video->MaterialSys->ReleaseTexture( imagePressed );
	}
}


void MenuItemImageButton::SetGlowAnim( const char *glow )
{
	if( mImageGlow )
		GEngine->Video->MaterialSys->ReleaseTexture( mImageGlow );

	if( glow )
		mImageGlow = GEngine->Video->MaterialSys->CreateTexture( glow, Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	else
		mImageGlow = NULL;
}
