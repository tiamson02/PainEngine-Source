// $Header: /Game/Source/Engine/Render/DecalSystem.h 20    8/19/04 9:39p Bart $

// #pragma once
#ifndef _DECALSYSTEM_
#define _DECALSYSTEM_


#include "Core/HashMap.h"
#include "Render/VertexBuffer.h"
#include "Render/Vertex.h"

class DecalEffect
{
public:
	String		TextureName;
    int			BlendMode;
	float		Scale;
	float		ZScale;
	float		CullBackFaces;
	float		LifeTime;
	float		AnimationTime;
	float		DecayTime;
	float		FPS;
	bool		CutTris;
	String		Name;

    Texture*	TexturePtr;

	DecalEffect();
	~DecalEffect();

	void Load(const char *name);
};

class DecalSystem
{
	friend class Decal;
	typedef HashMapPtr<String, DecalEffect> DecalsHashMap;
	typedef VertexBuffer<SimpleVertex,RenderDevice::ESimpleVertex/*,4096*/>  SimpleVertexBuffer;


private:
	DecalsHashMap DecalEffects;

	DecalEffect* LoadDecalEffect(const char *name);

public:
	SimpleVertexBuffer  VBuf;

	float DecalsStay;
#ifdef TRANSGAMING
    DecalSystem() : DecalsStay(1.0f){}
#else
    bool  DecalsAlwaysStay; // gameplay cheat 

    DecalSystem() : DecalsStay(1.0f), DecalsAlwaysStay(false){}
#endif
		
	~DecalSystem();

	BOOL Init();
	void RenderCleanup();

	void Reload();

	Decal* CreateDecal(const char* entityName, const char* fileName);
};

#endif //_DECALSYSTEM_

