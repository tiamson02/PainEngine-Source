
// #pragma once
#ifndef _MAGICBOARD_
#define _MAGICBOARD_


#include "PainMenu.h"

class MagicCard
{
public:

	enum MagicCardType
	{
        ECardTypeNone = 0,
		ECardTypeTime = 1,
		ECardTypePerm = 2,
		ECardTypePerm2 = 3,
	};

	MagicCard();
	~MagicCard();

	void SetName( const char *name ){ mName = name; }
	void SetAvailable( bool val )	{ mAvailable = val; }
	void SetDesc( const char *desc ){ mDesc = desc; }
	void SetCost( unsigned int cost ){ mCost = cost; }
	bool IsAvailable() const		{ return mAvailable; }


	const char *GetName() const		{ return mName.AsChar(); }
	const char *GetDesc() const		{ return mDesc.AsChar(); }
	unsigned  GetCost() const		{ return mCost; }

	void SetTexture( const char *name, DWORD flags = 0 );
	Texture *GetTexture() const		{ return mTexture; }

	void SetType( int type )		{ mType = ( MagicCardType )type; }
	int GetType() const				{ return mType; }

	void SetIndex( int i )			{ mIndex = i; }
	int GetIndex() const			{ return mIndex; }

	void SetBigTex( const char * tex )	{ mBigTex = tex; }
	const char * GetBigTex() const		{ return mBigTex.AsChar(); }

private:

	MagicCardType	mType;
	String			mName;
	String			mDesc;
	unsigned int	mCost;
	String			mBigTex;

	bool			mAvailable;
	int				mIndex;

	Texture			*mTexture;

#ifndef TRANSGAMING
	MenuItem		*mCrystal;
#endif
};


class BoardSlotGroup
{
public:

	BoardSlotGroup();
	~BoardSlotGroup();

	void Clear();

	void CheckMouse();
	bool IsUnderMouse() const					{ return mUnderMouse != -1; }
	int SlotUnderMouse()						{ return mUnderMouse; }

	void SetPosition( int x, int y, int y2 = -1 )			{ mPosX = x; mPosY = y; mPosY2 = y2; }
	void SetNumSlots( int count )				{ mNumSlots = count; }
	void SetSlotPosition( int slot, int x )		{ mPositions[slot] = x; }
	void SetSlotSize( int width, int height )	{ mSlotWidth = width; mSlotHeight = height; }
	void SetSpaceWidth( int width )				{ mSpaceWidth = width; }

	int GetNumSlots() const						{ return mNumSlots; }

	MagicCard *GetCard( int slot );
	void GetCardSize( int &x, int &y );
	void GetCardPosition( MagicCard *card, int &x, int &y );

	bool InsertCard( MagicCard *card, int slot = -1 );
	void RemoveCard( int slot ) { mCards[slot] = NULL; }

	void DrawCards();
	void DrawCardDesc( MagicCard *card );
	void DrawTooltip();

#ifdef TRANSGAMING
    bool isSlotAvailable();
#endif

private:

	int	mPosX;
	int mPosY;
	int mPosY2;

	int mNumSlots;
	int	mSlotWidth;
	int mSlotHeight;
	int mSpaceWidth;

	int	mUnderMouse;
	float mTimer;

	DynamicArray<MagicCard *> mCards;
	int mPositions[24];
};


class ENGINE_API MagicBoard
{
public:

	enum SlotGroups
	{
		ETimeAllSlots = 0,
		EPermAllSlots = 1,
		ETimeSelSlots = 2,
		EPermSelSlots = 3,
	};

	MagicBoard();
	~MagicBoard();

	void Render();
	void Clear();
	void Setup();

	void Activate( bool active );
	bool Active() const { return mActive; }

	void SetMouseTexture( int xClickOffset, int yClickOffset, const char *texName, DWORD texFlags = 0 );
	void ShowMouse( bool show ) { mShowMouse = show; }

	void SetBackground( const char *texName, DWORD texFlags = 0 );

	bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	void SetupSlots( int index, int count, int y, int size_x, int size_y, int spaceWidth, int y2 );
	void SetSlotPosition( int group, int slot, int pos );

	void AddCard( int type, const char * name, const char * tex, const char * desc, int cost, bool avail, bool sel, const char * bigTex );

	void DrawCounter();

	unsigned int GetCash() const { return mTargetCash; }
	void SetCash( unsigned int amount ) { mTargetCash = amount; }
	void SetCashCheat( unsigned int amount ) { mTargetCash = amount; mCash = amount; }

	bool IsCardInSlot( int group, int slot ) { return( mSlots[group].GetCard( slot ) != NULL  ); }
	bool FindCardInGroup( int group, int index );

	bool GetAllFree() const { return mAllFree; }
	void SetAllFree( bool free ) { mAllFree = free; }

	void MakeAllFree( bool free );
	bool CanPutDivineAway();

	bool AddOnEnabled() const { return mAddOn; }

	MagicCard *GetZoomCard() const { return mZoomCard; }

private:

	bool		mActive;
	Texture		*mBackground;
	bool		mAddOn;

	BoardSlotGroup	mSlots[5];

	unsigned int	mCash;
	unsigned int	mTargetCash;
	bool			mAllFree;
	bool			mAllFreeBonus;

	unsigned int	mNotEnoughMoney;
	float			mTimer;

	// Mouse
	bool		mShowMouse;
	int			mMouseClickOffsetX;
	int			mMouseClickOffsetY;
	Texture		*mMouseTexture;

	MenuItem	*mCrystal;

	MagicCard	*mTimeCards[36];
	MagicCard	*mPermCards[36];

	MagicCard	*mDraggedCard;
	MagicCard	*mZoomCard;
	Texture		*mZoomTex;
	int			mTakenFrom;
	int			mDragX;
	int			mDragY;

	Texture		*mCounterShadow;
	Texture		*mNumbers[10];
	Texture		*mCounterOverlay;

	int			mPausedSounds;
};


void MagicBoard_RegisterLUALib();


#endif //_MAGICBOARD_

