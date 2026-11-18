#include "Engine.h"
#error This file is obselete

RenderManager::RenderManager()
:   RenderDevType(RenderDeviceManager::ENullRenderDevice)
,   RenderWnd(0), RenderDev(0)
,	ClientList(0), MainClient(0)
{
}

RenderManager::~RenderManager()
{
	guard(RenderManager::dtor);
	for(TheClient* client=ClientList; client;)
	{
		TheClient* ptr=client;
		client=client->Next;
		delete ptr;
	}

    Close();
	unguard;
}

BOOL RenderManager::SetRes(int renderDeviceType, int xRes, int yRes, BOOL fullscreen)
{
    BOOL success=FALSE;
   
    if(RenderDev) // device already opened
	{ 
        // if it's the same device that requested
        if(RenderDevType==renderDeviceType)
            if(RenderDev->SetMode(xRes,yRes,fullscreen))
				return TRUE;
        
        // they want different device, free this one
        CloseRenderDevice();
    }

    if(NULL == RenderDev)
	{
        RenderDev = RenderDeviceManager::CreateRenderDevice(renderDeviceType);
        if(RenderDev) 
		{
            RenderDevType=renderDeviceType;
            if(RenderDev->Open(RenderWnd->GetHandle())) 
			{
                GLog.Print("CLIENT: Created rendering device: %d\n",RenderDevType);
			
			    if(!RenderDev->SetMode(xRes,yRes,fullscreen)) 
				{
                    GLog.Print("CLIENT: Warning - Could not init RenderDevice with: %d,%d %s, trying safe mode\n",
                                xRes,yRes,(fullscreen ? "FullScreen" : "Windowed"));

                    if(!RenderDev->SetMode(640,480,TRUE)) 
					{
                        GLog.Print("CLIENT: Fatal - Safe mode didn't quite work, bailing out\n");
                        return FALSE;
                    }
                }

                // contrast,gamma,brightness values
                float contrast  =0.5f;
                float gamma     =1.5f;
                float brightness=0.5f;

			    const char* s;
                const char* section="Video";
			    s=GConfig.GetString(section,"Contrast"  ); if(s) contrast  =float(atof(s));
			    s=GConfig.GetString(section,"Gamma"     ); if(s) gamma     =float(atof(s));
			    s=GConfig.GetString(section,"Brightness"); if(s) brightness=float(atof(s));

    			RenderDev->SetGamma(contrast,gamma,brightness);

                // all good
                success=TRUE;
            }
        }
    }

	return success;
}

BOOL RenderManager::Init(int renderDeviceType, int xRes, int yRes, DWORD initOpts)
{
    if(!xRes || !yRes)
    {
        // get screen configuration
	    const char* section="Video";			
	    const char* xResValue=GConfig.GetString(section,"XRes");
	    const char* yResValue=GConfig.GetString(section,"YRes");
	    if(xResValue && yResValue) 
        {
		    xRes=atoi(xResValue);
		    yRes=atoi(yResValue);
	    }
	    else 
        {
		    // kinda safe defaults
		    xRes=640;
		    yRes=480;
	    }
    }

    // TODO: check xRes,yRes if sane

	BOOL fullscreen=FALSE;

    if(initOpts == EInitFullScreen)
        fullscreen = TRUE;
    else
    if(initOpts == EInitWindowed)
        fullscreen = FALSE;
    else
    {   // initOpts == EInitUseConfig
        const char* section = "Video";
	    const char* fullscrnValue=GConfig.GetString(section,"FullScreen");
	    if(fullscrnValue)
	        if(!stricmp(fullscrnValue,"true"))
		        fullscreen=TRUE;
    }

    if(!RenderWnd) 
	{
        // create rendering window
        m_setTag("EngineWindow");
        RenderWnd = new RenderWindow();
        Assert(RenderWnd);
        Window::Resources res;
        RenderWnd->Initialize(GEngine->GetInstance(),res,"PKRender3D");
        TRACE("RenderManager::Init(): creating default window\n");
        RenderWnd->Create("PainKiller");

        if(FALSE==fullscreen)
            RenderWnd->AdjustSize(xRes,yRes);

		MainClient=CreateClient(RenderWnd->GetHandle());
    }

    if(SetRes(renderDeviceType,xRes,yRes,fullscreen))
	{
		if(!Hud.Create(RenderDev)) {
			GLog.Print("ERROR: Could not initialize HUD\n");
			return FALSE;
		}

#ifdef TRANSGAMING_USE_EDITOR
		if(!LineBuf.Create(RenderDev))
		{
			GLog.Print("ERROR: Could not initialize LineBuf\n");
			return FALSE;
		}
#endif

		return TRUE;
	}

	return FALSE;
}

void RenderManager::Close()
{
    if(RenderWnd)
    {
        delete RenderWnd;
		RenderWnd=0;
        CloseRenderDevice();
    }
}

void RenderManager::CloseRenderDevice()
{
    guard(RenderManager::CloseRenderDevice);

    if(RenderDev) 
	{
        RenderDev->Close();
        RenderDeviceManager::DestroyRenderDevice(RenderDev);
        RenderDevType=RenderDeviceManager::ENullRenderDevice;
        RenderDev=NULL;
    }

    unguard;
}

Client* RenderManager::CreateClient(HWND hwnd)
{
    m_setTag("EngineRender");
	TheClient* client = new TheClient();
	client->SetWindow(hwnd);
	client->Link(ClientList);

	return client;
}

void RenderManager::RemoveClient(Client* cptr)
{
	Assert(cptr);
	TheClient* client=reinterpret_cast<TheClient*>(cptr);
	client->Unlink();
}

BOOL RenderManager::ToggleFullScreen(BOOL toggle, int width, int height, Client* client)
{
    if(SetRes(RenderDevType,width,height,TRUE))
    {
        if(client)
        {
        }

        return TRUE;
    }

    return FALSE;
}
