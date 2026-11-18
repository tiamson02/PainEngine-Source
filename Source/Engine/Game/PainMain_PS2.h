#ifndef _PAINGAME_PS2_H_
#define _PAINGAME_PS2_H_

#ifndef ENGINE_API
#define ENGINE_API  DLL_IMPORT
#endif

/*
#define PKTV_ENABLED

// core stuff
#include "Core/nommgr.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Core/VisualWin32.h"
#include "Core/mmgr.h"
*/
#include "Engine/BaseGame.h"

class PainGame : public EngineGame
{
public:
	PainGame();
	virtual ~PainGame();

	void InitInstance();
	void ProcessInput();
};

#pragma warning(disable:4127) //conditional expression is constant

#endif //_PAINGAME_PS2_H_

