#include "Engine.h"
#include "Core/FileSystem/LogBuffer.h"
#include "Engine/AnimatedMesh.h"
#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;
#include "Engine/AnimatedMeshManager.h"
#include "Render/Material.h"

extern ENGINE_API LogBuffer GLog;
//LogBuffer MLog;
// ============================================================

AnimatedMeshManager::AnimatedMeshManager() : ShadowMapFilter(NULL), ShadowMapSize(128)
{
    // parse config file and build a list of texture paths
    //const char* path = GConfig.GetString("Paths","Models");
	ModelsPath = "../data/models/";

	//MLog.Open("Mesh.log");
}

AnimatedMeshManager::~AnimatedMeshManager()
{
    for(int i=0; i<AnimatedMeshesHash.Size(); ++i)
        delete AnimatedMeshesHash.GetPair(i).Value;
}

void AnimatedMeshManager::Clear()
{
    for(int i=0; i<AnimatedMeshesHash.Size(); ++i)
        delete AnimatedMeshesHash.GetPair(i).Value;
    AnimatedMeshesHash.Clear();
}

AnimatedMesh* AnimatedMeshManager::CreateAnimatedMesh(const char* name, float scale)
{
    Assert(name);

    AnimatedMesh* mesh = NULL;
	String hashName = String(name) + String::Sprintf("%.2f", scale);
	AnimatedMesh** ptr = AnimatedMeshesHash.Find(hashName);
    if(ptr)
        mesh = (*ptr);
    else
    {
#ifdef ANNOYING_GRAPHICS_LOGS
		MLog.Print("\nLoading skinned mesh %s (scale:%.2f)\n", name, scale);
#endif

		String modelsDir("../Data/Models/");
	
        m_setTag("Mesh");
		ObjectPackage* package = new ObjectPackage((modelsDir+name+".pkmdl").AsChar());

		if(package)
		{
//			RenderDevice* dev=GEngine->GetRenderDevice();

            if (package->GetInstanceNum()>0)
            {
			    mesh = (AnimatedMesh*)package->GetInstance(0,"AnimatedMeshMatPal");
			    mesh->LoadPreprocess(scale, name);
                AnimatedMeshesHash[hashName] = mesh;
            }
            else
            {
                GLog.Print("!!! BUG BUG BUG !!!! Loading skinned mesh %s (scale:%.2f)\n", name, scale);
            }

			delete package;
		}
    }

    return mesh;
}

void AnimatedMeshManager::SetShadowMapParams(int shadowMapSize, int shadowMapNum)
{
	ShadowMapSize = shadowMapSize;
	ShadowMaps.ResizeArray(shadowMapNum);
}


Texture* AnimatedMeshManager::CreateShadowMap(int size)
{
	for (int i=0; i<ShadowMaps.Size(); i++)
	{
		if (!ShadowMaps[i].Used)
		{
			ShadowMaps[i].Used = true;
			return ShadowMaps[i].Tex;
		}
	}

	return 0;
}

void AnimatedMeshManager::ReleaseShadowMap(Texture* tex)
{
	for (int i=0; i<ShadowMaps.Size(); i++)
	{
		if (ShadowMaps[i].Tex == tex) 
		{
			ShadowMaps[i].Used = false;
			return;
		}
	}
}

void AnimatedMeshManager::RenderInitialize(RenderDevice* dev, MaterialSystem* matSys)
{
    TRACE("AnimatedMeshManager::RenderInitialize(): creating %d shadow maps of size %dx%d\n", ShadowMaps.Size(), ShadowMapSize);

	for (int i=0; i<ShadowMaps.Size(); i++)
	{
        m_setTag("TextureRenderTarget");
		Texture* ShadowMap = new Texture(Texture::ETexRenderTarget);
		ShadowMap->Height = ShadowMapSize;  
		ShadowMap->Width  = ShadowMapSize;

		dev->CreateTexture(ShadowMap);
		dev->SetRenderTarget(ShadowMap, NULL);
		dev->ClearFB(0xFF000000);

		ShadowMaps[i].Tex = ShadowMap;
	}

    m_setTag("TextureRenderTarget");
	ShadowMapFilter = new Texture(Texture::ETexRenderTarget);
	ShadowMapFilter->Height = ShadowMapSize;
	ShadowMapFilter->Width  = ShadowMapSize;

	dev->CreateTexture(ShadowMapFilter);
	dev->SetRenderTarget(ShadowMapFilter, NULL);
	dev->ClearFB(0xFF000000);

	dev->SetFBRenderTarget();
}

void AnimatedMeshManager::RenderCleanup(RenderDevice* dev, MaterialSystem* matSys)
{
	for (int i=0; i<ShadowMaps.Size(); i++)
	{
		if (ShadowMaps[i].Tex) {
			dev->ReleaseResource(RenderDevice::EResTexture, &ShadowMaps[i].Tex->DeviceId, 1);
			delete ShadowMaps[i].Tex;
			ShadowMaps[i].Tex = NULL;
		}
	}

	if (ShadowMapFilter) {
		dev->ReleaseResource(RenderDevice::EResTexture, &ShadowMapFilter->DeviceId, 1);
		delete ShadowMapFilter;
		ShadowMapFilter = NULL;
	}
}
