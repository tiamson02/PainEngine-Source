#include "TestGame.h"

class MyWindow : public Window
{
public:
    //BOOL HandleMessages(UINT msg, WPARAM wp, LPARAM lp, LRESULT& res)
	LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
	{
	    if(WM_CLOSE == msg)
		{
		    GEngine->Close();
			Status=EClosed;
			return TRUE;
		}

		return FALSE;
	}
};

MyWindow wnd;
MyWindow wnd2;
//Client client;
//Client client2;
Client* client;
Client* client2;

// ==============================================================

namespace { FactoryRegistration<BaseGame,TestGame> reg("testgame"); }

TestGame::TestGame()
{
}

TestGame::~TestGame()
{
}

BOOL TestGame::Initialize()
{
	RenderManager* render=GEngine->GetRender();

    Window::Resources res;
    wnd.Initialize(GEngine->GetInstance(),res,"Dupa3D");
    wnd.Create("DUPA");

	/*
    wnd2.Initialize(GEngine->GetInstance(),res,"Shit3D");
    wnd2.Create("SHIT");
	*/

    wnd.AdjustSize(640,480);
    //wnd2.AdjustSize(320,200);
    //render->SetRes(RenderDeviceManager::ED3DRenderDevice,wnd.GetHandle());
	//client.SetWindow (wnd.GetHandle());
    //client2.SetWindow(wnd2.GetHandle());

	client =render->CreateClient(wnd.GetHandle());
	//client2=render->CreateClient(wnd2.GetHandle());

	GEngine->GetInputSys()->LockMouse2(wnd.GetHandle());
    //render->SetRes(RenderDeviceManager::ED3DRenderDevice,1024,768,TRUE);
	//client.SetWindow(0);

	wnd.Show();
	wnd2.Show();

    const char* section="TestGame";
    const char* mapName=GConfig.GetString(section,"Map");
    if(mapName) {
        const char* scale=GConfig.GetString(section,"Scale");
        if(scale) {
            //mWorld.ObjectToWorld=ScaleMatrix((float)atof(scale));
        }

        String tmp("../Data/Maps/");
        tmp+=mapName;

        GLog.Print("loading test level: %s\n",tmp.AsChar());
		Level& level = mWorld.GetLevel();
        if(level.LoadPackage(tmp.AsChar())) 
		{
			mWorld.Init();
			mWorld.Dump();

            return TRUE;
        }
        else
        {
            GLog.Print("WARNING: couldnt load map: %s\n",mapName);
        }
    }

    return FALSE;
}

void TestGame::Close()
{
}

void TestGame::Tick(float timeDelta)
{
	InputSystem* input = GEngine->GetInputSys();

	input->Tick();

    for(InputSystem::KeyInfoItem* key=input->GetActiveKeyList(); key; key=key->Next)
    {
        if(key->Status==EKeyPressed)
        {
			GLog.Print("key_pressed: %d\n",key->Key);
		}
	}


	//client.Update(mWorld);
	//client2.Update(mWorld);
	client->Tick(timeDelta, mWorld);
	//client2->Update(mWorld);
}
