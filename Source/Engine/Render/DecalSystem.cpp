// $Header: /Game/Source/Engine/Render/DecalSystem.cpp 31    8/19/04 9:39p Bart $

#include "Engine.h"

#include "Core/FileSystem/ConfigFile.h"
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "DecalSystem.h"
#include "Render/Decal.h"
#include "Render/Video.h"
#include "Render/Material.h"

//  Decal Effect

DecalEffect::DecalEffect()
:	TexturePtr(NULL), Scale(1), ZScale(0), CullBackFaces(-1), BlendMode(RenderState::EBlendInvModulate)
, LifeTime(6), DecayTime(1), FPS(0), CutTris(true), AnimationTime(0)
{
}

DecalEffect::~DecalEffect()
{
}


void DecalEffect::Load(const char *name)
{
#ifdef TRANSGAMING_HACK
    return;
#endif
	const char *path = "../Data/Scripts/Decals/";
    char        buf[MAX_PATH];


	Name = name;

    ConfigFile cfg;
    snprintf(buf, MAX_PATH, "%s%s.ini", path, name);

    if (cfg.Load(buf)) 
	{
        const char* s,*section;
        section="General";
        s=cfg.GetString(section,"Texture");         		if(s) TextureName=s;
        s=cfg.GetString(section,"Scale");           		if(s) Scale=atof(s);		
		s=cfg.GetString(section,"LifeTime");        		if(s) LifeTime=atof(s);
		s=cfg.GetString(section,"DecayTime");        		if(s) DecayTime=atof(s), AnimationTime=atof(s);
		s=cfg.GetString(section,"AnimationTime");        	if(s) AnimationTime=atof(s);
		s=cfg.GetString(section,"ZScale");					if(s) ZScale = atof(s); else ZScale = 0.0f; 
		s=cfg.GetString(section,"CullBackFaces");			if(s) CullBackFaces = atof(s); else CullBackFaces = -1.0f; 
		s=cfg.GetString(section,"FPS");						if(s) FPS = atof(s); 
		s=cfg.GetString(section,"BlendMode");				if(s) { TokenParser t(s); BlendMode = t.ParseBlendMode(); }
		s=cfg.GetString(section,"CutTris");					if(s) CutTris = (atoi(s)!=0);

#ifdef DEBUG
        TRACE("DecalEffect::Load(): loaded a decal effect from the file '%s'\n", name);
#endif
	}

	if (TextureName.Len())
	{
        snprintf(buf, MAX_PATH, "Decals/%s", TextureName.AsChar());

		TexturePtr = GEngine->Video->MaterialSys->CreateTexture(buf);
	}
	else
	{
		TexturePtr = GEngine->Video->MaterialSys->Textures.NoTex;
	}
}

//  Decal System

DecalSystem::~DecalSystem()
{
}

BOOL DecalSystem::Init()
{
	RenderDevice* dev=GEngine->GetRenderDevice();
	Assert(dev);

#ifdef TRANSGAMING
# ifdef TRANSGAMING_PS2
#  ifdef DEBUG
    if(!VBuf.Create(dev, 128, false, __FILE__, __LINE__)){
#  else
    if(!VBuf.Create(dev, 128)){
#  endif
        TRACE("DecalSystem::Init(): ERROR-> Failed to create vertex buffer\n");
        return FALSE;
    }
# else
    if(!VBuf.Create(dev, 128)){
        TRACE("DecalSystem::Init(): ERROR-> Failed to create vertex buffer\n");
        return FALSE;
    }
# endif
#else
	if(!VBuf.Create(dev, 4096)) {
		GLog.Print("DecalSystem: Failed to create vertex buffer\n");
		return FALSE;
	}
#endif

	return TRUE;
}

void DecalSystem::RenderCleanup()
{
	VBuf.Release();
}

Decal* DecalSystem::CreateDecal(const char* entityName, const char* fileName)
{
#ifdef TRANSGAMING_HACK
    return NULL;
#endif
	DecalEffect *effect = DecalEffects.Find(fileName);
	
	if (!effect)
	{
		effect = LoadDecalEffect(fileName);
		DecalEffects.Insert(fileName, effect);
	}

    m_setTag("Particle");
	Decal *decal = new Decal(effect);
	decal->Name = entityName;

	return decal;
}

DecalEffect* DecalSystem::LoadDecalEffect(const char *name)
{
    m_setTag("Particle");
	DecalEffect *effect = new DecalEffect();

	effect->Load(name);

	return effect;
}

void DecalSystem::Reload()
{
	for (int i=0; i<DecalEffects.Size(); ++i)
	{
		DecalsHashMap::HashPair pair = DecalEffects.GetPair(i);

		const char *key = pair.Key.AsChar();
		DecalEffect* effect = DecalEffects.Find(pair.Key);

		effect->Load(key);
	}
}
