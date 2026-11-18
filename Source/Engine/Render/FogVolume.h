// #pragma once
#ifndef _FOGVOLUME_
#define _FOGVOLUME_

#include "Engine/Mesh.h"
class Material;

class ENGINE_API Volume : public WorldMesh
{
public:
	enum 
	{
		EVolLight,
		EVolFog,		
	};

#ifdef	TRANSGAMING
	Color_TG	VolColor;
#else
	Color	VolColor;
#endif

	float	VolDensity;
	float	VolEnd;

	int				VolumeType;
	Material*		BackShader;
	Material*		FrontShader;

	Plane							FarPlane;
	DynamicArray<WorldMesh*>		Meshes;
	DynamicArray<Entity*>			DEntities;

	Volume(const WorldMesh& mesh, int type);
	~Volume();

    // Entity interface
    void RenderInitialize(RenderDevice* dev, MaterialSystem* matSys, const char* texSubPath=0);
    void RenderCleanup   (RenderDevice* dev, MaterialSystem* matSys);

	void DrawBackPass(RenderDevice* dev);
	void DrawFrontPass(RenderDevice* dev);
	void Draw(RenderDevice* dev, int mode) {}

    bool isVolume() const{ return true; }
};



#endif //_FOGVOLUME_

