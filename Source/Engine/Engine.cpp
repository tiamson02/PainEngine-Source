// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 04/02 Bart Sekura
//----------------------------------------------------------------------------

#include "Core/Debug.h"
#include "Engine.h"
#include "Core/FileSystem/LogBuffer.h"
#include "Scripting/Scripting.h"
#include "Engine/PCFSystem.h"
#include "core/HashMap.h"
#include "FileSystem/GFileManager.h"

#ifdef TRANSGAMING_PSP
#include <kernel.h>
#endif

// Globals
//MemoryManagerStd	GMem;
ENGINE_API GFileManager  	GFileMan;
ENGINE_API GFileManager  	GFileManAudio;


ENGINE_API	LogBuffer			GLog;

ENGINE_API	PCFSystem*	GEngine = NULL;
ENGINE_API	Script	GScript;

class EngineGame;
class View;

// transition between PainKiller and Engine
ENGINE_API	EngineGame* (*OurGame)(void) = 0;
ENGINE_API	View* (*OurView)(void) = 0;

#ifdef TRANSGAMING_PSP
SCE_MODULE_INFO( PainKiller, 0, 1, 1 );

// Blair - Memory...SWEET MEMORY! This gives us a good chunk of the
// remaining user memory to use ( our prx is currently 16.7M with 
// debug symbols ) This is a standard PSP call made at the beginning
// of the app to allocate the heap size.
int sce_newlib_heap_kb_size = 30 * 1024;
#endif

// main entry
#ifdef TRANSGAMING_PSP
int main(HINSTANCE hInstance, const char* commandLine)
#else
int Main(HINSTANCE hInstance, const char* commandLine)
#endif
{
	/*
    // prevent multiple instances
    CreateMutex(NULL,TRUE,"PCFPainKiller");
    if(ERROR_ALREADY_EXISTS==GetLastError()) 
	{
        MessageBox(NULL,"Already Running!","ERROR",MB_OK);        
        return 0;
    }
*/
    try 
	{
#ifdef TRANSGAMING
/* Blair - Ok, these were commented out in the source we got, but they seem to get
the ball rolling with the game scripts.  In the end, we may have to create some
loader and shuffle things around, but for now, we'll keep these in.  The warning above
doesn't give us enough info... */
		GScript.Init();

		GScript.DoFile("host0:../Data/LScripts/Loader.lua");
#else
		// Script initialization - start. Not working yet.
//		GScript.Init();
//		GScript.DoFile("host0:../Data/LScripts/Loader.lua");
#endif

        // start the engine
        m_setTag("Engine");
        GEngine=new PCFSystem();
        if(GEngine) 
		{
            if(GEngine->Initialize(hInstance,commandLine))
                GEngine->RunLoop();

            delete GEngine;
			GEngine = 0;
        }
    }
    catch(...) 
	{
        StackTracer::ErrorMessageBox();
    }

	//ObjectPackage::GClosePackages();
    return 0;
}
