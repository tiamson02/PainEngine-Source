
// #pragma once
#ifndef _BASEGAME_
#define _BASEGAME_


class Entity;

#include "Core/Math/LeMath.h"

class ENGINE_API EngineGame
{
public:
    Vector LastCamPos;
	// Members
	bool mPaused;
	bool mUpdateLogic;

//#ifdef _DEBUG
//	bool canPreprocessPlayer;
//	bool preprocessPlayerNow;
//#endif

	// Maintenance
	EngineGame();

	void Initialize();
	void Tick(bool minimized, float delta);
	void Close();
	virtual ~EngineGame() {};

	// Other
    Entity* CreatePlayer(LPCSTR modelName,bool ragdoll=false);
    void SwitchMenu( bool force = false );
	void ShowMenu();
	void HideMenu();
	void SwitchMagicBoard( bool force = false );
	void SwitchMapSelect( bool force = false );
	void SwitchConsole();
	void ShowMPStats();
	void HideMPStats();
	void RecordingUpdateViewFromPlayer( Entity *player, float yaw, float dir );

    void ScriptGameTick(float td);
    void ScriptGameTick2(float td);
    void ScriptGameTick3(float td);

#ifdef TRANSGAMING_ENABLE_LUA_PROFILER
	void StartLuaProfile();
	void StopLuaProfile();
#endif

#ifdef TRANSGAMING_HACK
	void TickObjsAndRender(float timeDelta);
#endif

	// Implemented in editor
	virtual void ProcessInput( void ) = 0;
};

#endif //_BASEGAME_

