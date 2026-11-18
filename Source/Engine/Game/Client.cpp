#error This file is obselete

#include "Engine.h"


Client::Client()
:	Mode(EModeFlyMove), RenderHWND(0)
{
    mCamera.Fov=90.0f;
    mCamera.Position=Vector(0,0,0);
    mCamera.Rotation=Vector(0,0,0);
}

Client::~Client()
{
}

void Client::LockMouse(BOOL lock)
{
    InputSystem* inputSys = GEngine->GetInputSys();
    if(lock)
    {
        inputSys->LockMouse2(RenderHWND);
        Mode|=EModeLockedMouse;
    }
    else
    {
        inputSys->LockMouse2(0);
        Mode&=~EModeLockedMouse;
    }
}

void Client::FrameSetup(float timeDelta, FWorld& world)
{
    InputSystem* inputSys = GEngine->GetInputSys();
    const DWORD Action=inputSys->GetActionStatus();

    if(Mode & EModeFlyMove) 
    {
        const float speed=32.f;
        Vector move(0,0,0);
        if(Action & (1<<EInputMoveForward))
            move+=mCamera.GetForwardVector();
        if(Action & (1<<EInputMoveBackward))
            move+=-mCamera.GetForwardVector();
        if(Action & (1<<EInputMoveLeft))
            move+=-mCamera.GetRightVector();
        if(Action & (1<<EInputMoveRight))
            move+=mCamera.GetRightVector();

        if(!move.AlmostZero())
            mCamera.Position+=move*speed*timeDelta;
    }

    if(Mode & EModeLockedMouse) 
    {
        mCamera.Rotation+=inputSys->GetMouseDelta();
        // clamp vertical rotation
        if(mCamera.Rotation.Y>+80.0f) mCamera.Rotation.Y=+80.0f;
        if(mCamera.Rotation.Y<-80.0f) mCamera.Rotation.Y=-80.0f;
    }
}

void Client::Tick(float timeDelta, FWorld& world)
{
    FrameSetup(timeDelta,world);

    mViewport.SetWindow(RenderHWND);
	mViewport.ClearColor=0x2a2a2a2a;

	mCamera.Tick();
	mViewport.Tick(mCamera);

	RenderDevice* dev=GEngine->GetDevice2();
	if(dev->Lock2(mViewport))
	{
        Render(world);

	    dev->Unlock();
		dev->Flip(mViewport);
	}
}

void Client::Render(FWorld& world)
{
	RenderWorld(1,world);
	//RenderZones(world);
	DrawHUD();
}

#ifndef TRANSGAMING
void Client::RenderZones(FWorld& world)
{
	LineRender& lineBuf=GEngine->GetRender()->GetLineBuf();

	RenderDevice* dev=GEngine->GetDevice2();
	lineBuf.SetState(dev);
	
	const DynamicArray<Zone*>& zones=world.GetZones();
	for(int i=0; i<zones.Size(); ++i)
		lineBuf.DrawBox(zones[i]->Box,0xff);

	const DynamicArray<Portal*>& portals=world.GetPortals();
	for(int i=0; i<portals.Size(); ++i)
		lineBuf.DrawPoly(portals[i]->Poly,0xff00);

	SceneRender scene(mViewport,world,0);
	scene.Occlude();

	for(TEntityPtr* p=scene.EntityList; p; p=p->Next)
		lineBuf.DrawBox((*p)->Box,0xff0000);

	/*
	Zone* zone=scene.GetEyeZone();
	if(zone)
	{
		for(TEntityPtr* p=zone->EntityList; p; p=p->Next)
			lineBuf.DrawBox((*p)->Box,0xff0000);
	}
	*/
}
#endif

#ifndef TRANSGAMING
void Client::RenderWorld(int renderMode, FWorld& world)
{
	RenderDevice* dev=GEngine->GetDevice2();

	TextureState texSt[2];

	switch(renderMode) {
	case 2: // lightmaps only

		dev->SetConstColor(0xff0000);
		texSt[0].Set(0,TextureState::ETexOpColor);
		texSt[1].Set(1,TextureState::ETexOpTexture);
		break;

	case 1: // textures only
		texSt[0].Set(0,TextureState::ETexOpTexture,0,TextureState::ETexFilterBilinear);
		texSt[1].Set(1,TextureState::ETexOpDisable);
		break;

	case 0: // normal
	default:
		texSt[0].Set(0,TextureState::ETexOpTexture,0,TextureState::ETexFilterBilinear);
		texSt[1].Set(1,TextureState::ETexOpModulate,TextureState::ETexArgFB,TextureState::ETexFilterBilinear);
		//texSt[0].Set(0,TextureState::ETexOpTexture,0,TextureState::ETexFilterBilinear);
		//texSt[1].Set(1,TextureState::ETexOpModulate,TextureState::ETexArgFB,TextureState::ETexFilterBilinear);
		break;
	}

	dev->SetTextureState(texSt,ARRAY_COUNT(texSt));
	
	RenderState state;
	state.SetDepthTest(true);
	state.SetDepthWrite(true);
	dev->SetState(state);

	SceneRender scene(mViewport,world,0);
	scene.Occlude();
	//scene.Dump();

	if(scene.EntityList)
	{
		for(TEntityPtr* p=scene.EntityList; p; p=p->Next)
			(*p)->Draw(dev);
	}
	else
	{
		for(TEntityPtr* p=world.GetEntityList(); p; p=p->Next)
			(*p)->Draw(dev);
	}
		//for(TEntityPtr* p=world.GetEntityList(); p; p=p->Next)
		//	(*p)->Draw(dev);
}
#endif

void Client::DrawHUD()
{
	RenderDevice* dev=GEngine->GetDevice2();
	HUD& hud=GEngine->GetRender()->GetHUD();

	TextureState st[2];
    st[0].Set(0,TextureState::ETexOpColor,0,TextureState::ETexFilterPoint);
    st[1].Set(1,TextureState::ETexOpDisable);

    dev->SetTextureState(st,ARRAY_COUNT(st));            
	dev->SetConstColor(0xff0000);

    hud.SetRenderState();
    hud.SetFont(0);

	hud.Print(10,10,"Dupa Blada");
	
	RenderState state;
	state.SetDepthTest(true);
	state.SetDepthWrite(true);
	state.SetBlendMode(RenderState::EBlendNone);
	dev->SetState(state);
}
