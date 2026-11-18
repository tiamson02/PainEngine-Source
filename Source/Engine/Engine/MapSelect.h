
// #pragma once
#ifndef _MAPSELECT_
#define _MAPSELECT_


#include "PainMenu.h"

class LevelDesc
{
public:
    typedef enum{
        STATUS_UNAVAILABLE,
        STATUS_CURRENT,
        STATUS_FINISHED,
        STATUS_WRONGDIFF,
    } LevelStatus;

	//String	    dir;
	String	    name;
	String	    sketchName;
	String	    cardCondition;
	//Texture	*   sketch;
	int         cardIndex;
	LevelStatus status;	// 0 - unavailable, 1 - current, 2 - finished, 3 - wrong difficulty

    LevelDesc(): /*sketch(NULL), */cardIndex(0), status(STATUS_UNAVAILABLE) {}
    ~LevelDesc();
};


class MapSelect
{
    static const int NUM_CHAPTERS = 5;
    static const int NUM_MAPS = 12;
public:

	MapSelect();
	~MapSelect();

	void Render();
	void Clear();

	void Activate( bool active );
	bool Active() const { return mActive; }

    bool Disabled() const{ return mDisabled; }
    bool Disable(bool state){ mDisabled = state; }

	void Setup();

#ifndef TRANSGAMING
	void SetMouseTexture( int xClickOffset, int yClickOffset, const char *texName, DWORD texFlags = 0 );
	void ShowMouse( bool show ) { mShowMouse = show; }
#endif

	void SetBackground( const char *texName, DWORD texFlags = 0 );

	bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	void AddMap( int chapter, const LevelDesc &desc );
    void UpdateMapStatus(int chapter, int level, const LevelDesc::LevelStatus status);
    void SetMapSketch(int chapter, int level, const char *sketch);
	void SelectNextLevel();

	int GetCurrLevel() const { return mCurrLevel; }
	int GetCurrChapter() const { return mCurrChapter; }
#ifndef TRANSGAMING
	const char *GetCurrLevelName() const { return mMaps[mCurrChapter][mCurrLevel]->dir.AsChar(); }
#endif
	const char *GetCurrLevelCondition() const { return mMaps[mCurrChapter][mCurrLevel]->cardCondition.AsChar(); }
	int GetCurrLevelCardIndex() const { return mMaps[mCurrChapter][mCurrLevel]->cardIndex; }
	void ForceCurrLevel( int l, int c );

	const char * GetLastLevelCondition() const { return mLastCondition.AsChar(); }

	void Reset();

private:

	MenuItemImageButton *CreateButton(const char *name);
	void DrawButton( MenuItemImageButton *button );
	void DrawLevelDesc();
	void DrawWindow();

    void SetSketch(const char *filename);

	void UpdateButtons();

	bool CheckButtons();

    const char *GetActionString(char *buf, size_t len) const;

	bool		mActive;
    bool        mDisabled;
	Texture *   mBackground;
    Texture *   mCurrSketch;

	// Mouse
	bool		mShowMouse;
	int			mMouseClickOffsetX;
	int			mMouseClickOffsetY;
	Texture		*mMouseTexture;

	int			mCurrChapter;
	int			mTargetChapter;
	int			mCurrLevel;
	int			mLastLevel;
	String		mLastCondition;
    String      mCurrSketchName;

	// Buttons
	MenuItemImageButton	*mCard;
	MenuItemImageButton	*mPent;
	MenuItemImageButton	*mCrystal;

	DynamicArray<MenuItemImageButton *>	mButtons;
	DynamicArray<MenuItemImageButton *>	mChapters;
	DynamicArray<MenuItemImageButton *>	mChaptersSel;
	DynamicArray<MenuItemImageButton *>	mLevels;
	DynamicArray<MenuItemImageButton *>	mLevelsSel;
	DynamicArray<MenuItemImageButton *> mQuestions;

	Texture *mWindow;
	float	mWindowRot;

	Texture *mLoadingScreen;
	float	mFadeStart;

	float	mTargetRot;
	float	mStartRot;
	int		mTargetLevel;
#ifdef TRANSGAMING
    bool    mShowStats;
#endif

	LevelDesc *mMaps[NUM_CHAPTERS][NUM_MAPS];
	int mNumMaps[NUM_CHAPTERS];

	int		mPausedSounds;

	bool	mAddOn;
};


#endif //_MAPSELECT_

