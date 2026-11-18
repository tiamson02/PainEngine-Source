// $Header: /Game/Source/Engine/Render/TrailSystem.cpp 13    8/19/04 9:40p Bart $

#include "Engine.h"
#include "Core/FileSystem/ConfigFile.h"
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Render/TrailSystem.h"
#include "Render/Trail.h"
#include "Render/Video.h"
#include "Render/Material.h"

//============================================================================
// TrailEffect
//============================================================================
TrailEffect::TrailEffect()
:	TexturePtr(NULL), Width(0.1f), BlendMode(RenderState::EBlendTranslucent), LifeTime(1), FPS(0), ConstLen(false)
{
}

TrailEffect::~TrailEffect()
{
}

void TrailEffect::Load(const char *name)
{
	const char *path = "../Data/Scripts/Trails/";
    char        buf[MAX_PATH];

	Name = name;

    ConfigFile cfg;
    snprintf(buf, MAX_PATH, "%s%s.ini", path, name);


    if (cfg.Load(buf)) 
	{
        const char* s;
		const char* section="General";
		s=cfg.GetString(section,"Texture");         		if(s) TextureName=s;
		s=cfg.GetString(section,"LifeTime");        		if(s) LifeTime=atof(s);
		s=cfg.GetString(section,"Width");        			if(s) Width=atof(s);
		s=cfg.GetString(section,"FPS");						if(s) FPS = atof(s); 
		s=cfg.GetString(section,"BlendMode");				if(s) { TokenParser t(s); BlendMode = t.ParseBlendMode(); }
		s=cfg.GetString(section,"ConstLen");				if(s) ConstLen = (atoi(s)!=0);
	}

	if (TextureName.Len())
	{
        snprintf(buf, MAX_PATH, "Particles/%s", TextureName.AsChar());

		TexturePtr = GEngine->Video->MaterialSys->CreateTexture(buf);
	}
	else
	{
		TexturePtr = GEngine->Video->MaterialSys->Textures.NoTex;
	}
}
//============================================================================
//  TrailSystem
//============================================================================
TrailSystem::~TrailSystem()
{
	GLog.Print("Deleting TrailSys\n");
}

BOOL TrailSystem::Init()
{
#ifndef TRANSGAMING
	RenderDevice* dev=GEngine->GetRenderDevice();
	Assert(dev);

	if(!VBuf.Create(dev, 4096, true)) {
		GLog.Print("TrailsSystem: Failed to create vertex buffer\n");
		return FALSE;
	}
#endif
	return TRUE;
}

void TrailSystem::RenderCleanup()
{
#ifndef TRANSGAMING
	VBuf.Release();
#endif
}

Trail* TrailSystem::CreateTrail(const char* entityName, const char* fileName)
{
	TrailEffect *effect = TrailEffects.Find(fileName);
	
	if (!effect)
	{
		effect = LoadTrailEffect(fileName);
		TrailEffects.Insert(fileName, effect);
	}

    m_setTag("Trail");
	Trail *trail = new Trail(effect);
	trail->Name = entityName;

	return trail;
}

TrailEffect* TrailSystem::LoadTrailEffect(const char *name)
{
    m_setTag("Trail");
	TrailEffect *effect = new TrailEffect();

	effect->Load(name);

	return effect;
}

void TrailSystem::Reload()
{
	for (int i=0; i<TrailEffects.Size(); ++i)
	{
		TrailsHashMap::HashPair pair = TrailEffects.GetPair(i);

		const char *key = pair.Key.AsChar();
		TrailEffect* effect = TrailEffects.Find(pair.Key);

		effect->Load(key);
	}
}
