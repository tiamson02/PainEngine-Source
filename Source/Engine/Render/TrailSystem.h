// $Header: /Game/Source/Engine/Render/TrailSystem.h 9     8/19/04 9:40p Bart $

// #pragma once
#ifndef _TRAILSYSTEM_
#define _TRAILSYSTEM_


#include "Core/HashMap.h"
#include "Render/VertexBuffer.h"
#include "Render/Vertex.h"

class ENGINE_API TrailEffect
{
public:
	String		Name; // needed only for load/save
	String		TextureName;
    int			BlendMode;
	float		Width;
	float		LifeTime;
	float		FPS;
	bool		ConstLen;

    Texture*	TexturePtr;

	TrailEffect();
	~TrailEffect();

	void Load(const char *name);
};

class ENGINE_API TrailSystem
{
	friend class Trail;
	typedef HashMapPtr<String, TrailEffect> TrailsHashMap;
#ifndef TRANSGAMING
	typedef VertexBuffer<SimpleVertex,RenderDevice::ESimpleVertex/*,4096*/>  SimpleVertexBuffer;
#endif

private:
	TrailsHashMap TrailEffects;

	TrailEffect* LoadTrailEffect(const char *name);

public:
#ifndef TRANSGAMING
    //the trail system now uses the particle system's vertex buffer
    SimpleVertexBuffer  VBuf;
#endif

    TrailSystem()
    {}
	~TrailSystem();

	BOOL Init();
	void RenderCleanup();

	void Reload();

	Trail* CreateTrail(const char* entityName, const char* fileName);
};


#endif //_TRAILSYSTEM_

