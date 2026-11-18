#include "Engine.h"
#include "Engine/PCFSystem.h"
#include "Engine/BaseGame.h"

class EditorGame: public EngineGame
{
public:
	EditorGame() {};
	virtual ~EditorGame() {};

	// przeniesc do skryptow:
	void ProcessInput()
	{
	};
};

extern ENGINE_API PCFSystem* GEngine;

static EngineGame* MakeMyGame( void ) { return (EditorGame*) new EditorGame(); };


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE /*hPrevInstance*/,
                     LPSTR     lpCmdLine,
                     int       /*nCmdShow*/)
{
    //try 
	//{
        // start the engine
        GEngine = new PCFSystem();
		//EngineGame* OurGame = MakeMyGame();
        if(GEngine) 
		{
            if(GEngine->Initialize(hInstance,lpCmdLine))
                GEngine->RunLoop();

            delete GEngine;
			GEngine = 0;
        }
    //}
    //catch(...) 
	//{
    //    StackTracer::ErrorMessageBox();
    //}
    return 0;
}
