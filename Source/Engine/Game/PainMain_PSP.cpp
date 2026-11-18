//#include "Core/Debug.h"
#include "Engine.h"
#include "Core/FileSystem/LogBuffer.h"
#include "Scripting/Scripting.h"
#include "Engine/PCFSystem.h"
#include "HashMap.h"
#include "FileSystem/GFileManager.h"
#include <time.h>

#ifdef TRANSGAMING_PSP
# include <kernel.h>
# include <libfpu.h>
# include <allegrex.h>
# include "Game/PainGame_PSP.h"
# include "Engine/BaseGame.h"
# include "Render/View.h"
# include <tg_pc2psp.h>
# include <LeString.h>
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


extern ENGINE_API EngineGame* (*OurGame)(void);
extern ENGINE_API View* (*OurView)(void);

static EngineGame* MakeAGame( void ) {
    m_setTag("Game");

    return (EngineGame*)new PainGame;
}

static View* MakeAView( void ) { 
    m_setTag("Game");

    return (View*)new View; 
}


// Blair - Memory...SWEET MEMORY! This gives us a good chunk of the
// remaining user memory to use ( our prx is currently 16.7M with 
// debug symbols ) This is a standard PSP call made at the beginning
// of the app to allocate the heap size.
int sce_newlib_heap_kb_size = 39 * 1024;

// This allows us to use the VFPU
int sce_newlib_attribute = SCE_KERNEL_TH_USE_VFPU;
#endif

void SwitchToFullScreen(bool enable)
{
	const char *res = "480X272";
	float g=1.0f, b=0.5f, c=0.5f;

/*
	if( !GScript.Globals()["Cfg"]["Resolution"].IsNil() )
	{
		res = GScript.Globals()["Cfg"]["Resolution"].GetString();
	}
*/
	GScript.DoString("R3D.ApplyVideoSettings(\"%s\",%s)", res, enable ? "true" : "false");
}

//#define RUN_SCRIPT_TEST
//#define RUN_CODE_TEST

// main entry
//int main(HINSTANCE hInstance, const char* commandLine)
int main(int argc, char **argv)
{
#ifdef TRANSGAMING
    LogBuffer::logDisabled = false;
#endif
#ifdef DEBUG
    time_t t = time(0);
    char buf[1024];

    strftime(buf, 1024, "********************************* Log Started %A %B %d, %Y (%I:%M:%S%p) *********************************\n", localtime(&t));
    TRACE("%s", buf);


# ifdef RUN_CODE_TEST
#  include "codeTestMain.cpp"
# endif
#endif

    // disable some fpu exceptions
    unsigned fcr31 = sceFpuGetFCR31();
    fcr31 &= ~SCE_ALLEGREX_FCR31_ENABLES_I;
    fcr31 &= ~SCE_ALLEGREX_FCR31_ENABLES_V;
    fcr31 &= ~SCE_ALLEGREX_FCR31_ENABLES_O;
    sceFpuSetFCR31(fcr31);

    //try 
	//	{
//#define LEVELNAME  "DM_SacredTNG5"
//#define LEVELNAME  "C2L6_SwampTNG4"
//#define LEVELNAME  "C2L6_SwampTNG6"
//#define LEVELNAME  "C2L6_SwampTNG7"
//#define LEVELNAME  "C2L6_SwampTNG8"
//#define LEVELNAME  "C2L6_SwampTNG9"
//#define LEVELNAME  "C6L4_PentagonTNG2"
//#define LEVELNAME  "C6L4_PentagonTNG5"
//#define LEVELNAME  "C6L4_PentagonTNG6"
//#define LEVELNAME  "C6L4_PentagonTNG7"
//#define LEVELNAME  "DM_UnseenTNG1"


#ifdef TRANSGAMING
/* Blair - Ok, these were commented out in the source we got, but they seem to get
the ball rolling with the game scripts.  In the end, we may have to create some
loader and shuffle things around, but for now, we'll keep these in.  The warning above
doesn't give us enough info... */
		GScript.Init();

		//GScript.DoFile("../Data/LScripts/Loader.lua");

# ifndef TRANSGAMING_USE_PHYSFS
        m_setTag("Pack");
		GFileMan.RegisterPack(new GPack(), "..\\Data\\LScripts.pak", "..\\Data\\LScripts\\", true);
		//GFileMan.RegisterPack(new GPack(), "..\\Data\\Levels.pak", "..\\Data\\Levels\\", false);
		//GFileMan.RegisterPack(new GPack(), "..\\Data\\Scripts.pak", "..\\Data\\Scripts\\", false);

        //m_setTag("Pack");
		//GFileMan.RegisterPack(new GPack(), "..\\Data\\"LEVELNAME".pkm", "..\\Data\\",false);
        
		//GFileMan.RegisterPack(new GPack(), ("../data/" + files[i]).AsChar(), "../data/",false);

		/*GScript.DoFile("../Data/LScripts/Main/Tweak.lua");
		GScript.DoFile("../Data/LScripts/Main/Cfg.lua");
		GScript.DoFile("../Data/LScripts/Main/Definitions.lua");
		GScript.DoString("CfgFile = [[%s]]", "config.ini" );
		GScript.DoString("Cfg:Load();Tweak:Load()");//*/


		//GFileMan.RegisterPack(new GPack(), "..\\Data\\Textures.pak", "..\\Data\\Textures\\", false);

        m_setTag("Pack");
		GFileMan.RegisterPack(new GPack(), "..\\Data\\Shaders.pak", "..\\Data\\Shaders\\", true);
		//GFileMan.RegisterPack(new GPack(), "..\\Data\\Maps.pak", "..\\Data\\Maps\\", false);
		//GFileMan.RegisterPack(new GPack(), "..\\Data\\Items.pak", "..\\Data\\Items\\", false);
		//GFileMan.RegisterPack(new GPack(), "..\\Data\\Models.pak", "..\\Data\\Models\\", false);
		//GFileMan.RegisterPack(new GPack(), "..\\Data\\Fonts.pak", "..\\Data\\Fonts\\", false);
		//GFileMan.RegisterPack(new GPack(), "..\\Data\\Sounds.pak", "..\\Data\\Sounds\\", false);
# endif

        m_dumpMemoryReport("memory report (pre-scripts).txt", false, true);
		GScript.DoFile("../Data/LScripts/Loader.lua");

		//GScript.DoString("CfgFile = [[%s]]", "config.ini" );


		OurGame = MakeAGame;
		OurView = MakeAView;		

#else
		// Script initialization - start. Not working yet.
		//GScript.Init();
		//GScript.DoFile("../Data/LScripts/Loader.lua");
#endif

        // start the engine
        m_setTag("Engine");
        GEngine=new PCFSystem();
        if(GEngine) 
	    {
		//GScript.DoString("Game:Init()");

            if(GEngine->Initialize(argc, argv))
			{
				SwitchToFullScreen(true);
#ifdef TRANSGAMING_HACK
		// Blair - This will load up a few objects, for us to test...once
		// we have the capability to load an entire level, this should be
		// removed...
                // eric-> the Game:TNGLoadObjs() function is commented out in the scripts (Main/Game.lua)
				//GScript.DoString("Game:TNGLoadObjs()");
				// If you want to load up a level, comment out the line above and uncomment the one below.
				// Then enter the name of the level to be loaded as a parameter. This one is the
				// smallest.
# ifdef RUN_SCRIPT_TEST
                TRACE("**************************** Starting Script Test ****************************\n");{
                #  include "scriptTestMain.cpp"
                TRACE("**************************** Starting Script Test ****************************\n");}
# else
                TRACE("**************************** Startup Complete ********************************\n");
                TRACE("main(): executing the startup command '%s'\n", GEngine->StartupCommand.AsChar());
				GScript.DoString(GEngine->StartupCommand.AsChar());
                GEngine->StartupCommand = "";

                TRACE("************************** Level Load Complete *******************************\n");

                GEngine->RunLoop();
# endif
#else
                GEngine->RunLoop();
#endif
			}

            delete GEngine;
			GEngine = 0;
        }
    //}
    //catch(...) 
	//{
    //    StackTracer::ErrorMessageBox();
    //}

	//ObjectPackage::GClosePackages();
    return 0;
}
