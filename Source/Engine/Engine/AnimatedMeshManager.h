
// #pragma once
#ifndef _ANIMATEDMESHMANAGER_H_
#define _ANIMATEDMESHMANAGER_H_


#include "Core/HashMap.h"


class AnimatedMesh;
#ifdef TRANSGAMING_WINDOWS
#include "Render/Material.h"
#else
class Texture;
class MaterialSystem;
#endif
class RenderDevice;

//#ifdef TRANSGAMING_PS2
//#define AnimatedMeshManager AnimatedMeshManagerPS2
//#endif

class AnimatedMeshManager
{
private:
	String							ModelsPath;
	HashMap<String,AnimatedMesh*>	AnimatedMeshesHash;

public:
	struct ShadowMap
	{
		bool				Used:1;
		Texture*			Tex;

		ShadowMap() : Used(false), Tex(NULL)
		{
		}
	};

	int							ShadowMapSize;
	DynamicArray<ShadowMap>		ShadowMaps;
	Texture*					ShadowMapFilter;

	AnimatedMeshManager();
	~AnimatedMeshManager();
    void Clear();

	void SetShadowMapParams(int shadowMapSize, int shadowMapNum);

	AnimatedMesh* CreateAnimatedMesh(const char* name, float scale);
	Texture* CreateShadowMap(int size);
	void  ReleaseShadowMap(Texture* tex);

	void RenderInitialize(RenderDevice* dev, MaterialSystem* matSys);
	void RenderCleanup(RenderDevice* dev, MaterialSystem* matSys);
};


#endif //_ANIMATEDMESHMANAGER_H_

