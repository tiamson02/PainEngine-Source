// #pragma once
#ifndef _ENGINE_
#define _ENGINE_

#ifndef ENGINE_API
#define ENGINE_API  DLL_IMPORT
#endif

//#ifndef	TRANSGAMING_PS2
//#define	TRANSGAMING_PS2
//#endif

#ifndef	TRANSGAMING_NO_GAMESPY
#define	TRANSGAMING_NO_GAMESPY
#endif

#define	TRANSGAMING_NO_SAFEDISC

/*
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
*/

//#define FINAL_BUILD // fullscreen build with menu


//#define CONSOLE_ONLY_SERVER
//#define STAND_ALONE_SERVER
//#define MPDEMO

//#define ENABLE_CD_CHECK

#ifndef TRANSGAMING
# define PKTV_ENABLED
#endif

#ifndef FINAL_BUILD
	//#define SDK_BUILD
	//#ifndef _CRTDBG_MAP_ALLOC
    //#define MEMORY_TEST
	//#endif
	//#define ENGINE_LOG
	//#define LOGS_FLUSH
	//#define ANNOYING_GRAPHICS_LOGS
#endif


// core stuff
#include "Core/nommgr.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Core/VisualWin32.h"
#include "Core/mmgr.h"


#pragma warning(disable:4127) //conditional expression is constant

#endif //_ENGINE_

