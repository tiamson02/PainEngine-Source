
// #pragma once
#ifndef _ENVIRONMENT_
#define _ENVIRONMENT_


#include "Render/Light.h"

class FVec2 {
public:
	FVec2() {}
	FVec2(float a, float b) : X(a), Y(b) {}

	float X,Y;
};

class WorldMesh;

struct TWater
{
	float	FresnelBias;
	float	FresnelExponent;
	Vector	DeepWaterColor;
	Vector	ShallowWaterColor;
	float	BumpHeight;
	float	WaveAmplitude;
	float	WaveFreq;
	float	WaveSpeed;
	float	WaterAmount;
	float	ReflectionAmount;
	float	WaterLevel;
	int		ReflectFlags;
	float	ReflectScale;
	float	RefractScale;
	float	PlaneShift;
	float	ReflectRadius;
	float	ReflectDist;
	bool	ReflectSky;

	FVec2	Pan[2];
	FVec2	Tile[2];

	DynamicArray<WorldMesh*>	ReflectList;

	TWater() : FresnelBias(0), FresnelExponent(1), DeepWaterColor(0,0,0), ShallowWaterColor(0,0,0)
	,	BumpHeight(0), WaveAmplitude(0), WaveFreq(1), WaveSpeed(1), WaterAmount(1)
	,	ReflectionAmount(1), WaterLevel(0), ReflectFlags(0)
	,	ReflectScale(0.25F), RefractScale(0.25F)
	,	PlaneShift(0.2F), ReflectRadius(1024), ReflectDist(256), ReflectSky(0)
	{
		for (int i=0; i<2; ++i) {
			Pan[i]		= FVec2(0,0);
			Tile[i]		= FVec2(1,1);
		}
	}
};

class ENGINE_API Environment : public Entity
{
public:
	enum 
	{
		EEnvDirLight	= (1<<0),
		EEnvAmbient		= (1<<1),
		EEnvFog			= (1<<2),
	};

	int				Defines;
	Light			DirLight;
#ifdef	TRANSGAMING
    Color_TG		AmbientColor;
#else
    Color			AmbientColor;
#endif	
	FogState		FogParams;
	float			LightFadeTime;
	ConvexVolume	BoundingVolume;
	TWater			Water;

	DynamicArray<Entity*>	Entities;
	DynamicArray<Light*>	DependentLights;

public:
	Environment(void);
	~Environment(void);

	virtual void UpdateTransform(bool updateInWorld = true);
	BOOL PointTest(const Vector& point) const;
	void EnableDraw(bool) {}

	void AddEntity(Entity* entity);
	void RemoveEntity(Entity* entity);

	void AddLight(Light* light);
	void RemoveLight(Light* light);
	void RemoveLights();

#ifndef TRANSGAMING
    // Demo recording
	virtual bool DemoReadEntity( FileObject *f );
	virtual bool DemoWriteEntity( FileObject *f );
	virtual bool DemoReadFull( FileObject *f );
	virtual bool DemoWriteFull( FileObject *f );
#endif
};


#endif //_ENVIRONMENT_

