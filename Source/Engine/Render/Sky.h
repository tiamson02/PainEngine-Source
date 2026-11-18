
// #pragma once
#ifndef _SKY_
#define _SKY_


class WorldMesh;
class Material;
class RenderDevice;
class MaterialSystem;

#include "Core/LeString.h"

class SkyLayer
{
public:
	WorldMesh* Mesh;

	float	Rot1,Rot2;
	float	Pan1U,Pan1V,Pan2U,Pan2V;
	float	Tile1U,Tile1V,Tile2U,Tile2V;

	String	Tex1, Tex2, Mask, LMap;

	Material*	Shader;

public:
    SkyLayer();
	//SkyLayer(WorldMesh* Mesh=NULL);
	~SkyLayer();

	void Render();
	void Setup();
    void RenderInitialize(RenderDevice* dev, MaterialSystem* matSys);
    void RenderCleanup(RenderDevice* dev, MaterialSystem* matSys);
	void Tick(float time);
};

class ENGINE_API Sky
{
public:
	bool	LowQuality;
	float	Height;
	float	Angle;

    Sky();
    ~Sky();

	int Load(const char* mpkName);
	void Unload();

    void RenderInitialize(RenderDevice* dev, MaterialSystem* matSys);
    void RenderCleanup(RenderDevice* dev, MaterialSystem* matSys);
	void Tick(float timeDelta);
	void FlashTexture(float time, int texnum);
    void Draw();
	int GetLayerNum() { return Layers.Size(); }
	SkyLayer* GetLayer(int layer);


private:
    float                   FlashTime;
	float					Time;
	DynamicArray<SkyLayer>	Layers;
};



#endif //_SKY_

