#include "Engine.h"
#include "Render/Viewport.h"
#include "Render/View.h"
#include "Engine/Mesh.h"
#include "Render/Video.h"
#include "Render/Sky.h"
#include "Engine/World.h"
#include "Render/Material.h"

// =========================================================================================
// [SkyLayer]
// =========================================================================================

SkyLayer::SkyLayer():
    Rot1(0),    Rot2(0),
    Pan1U(0),   Pan1V(0),   Pan2U(0),   Pan2V(0),
    Tile1U(1),  Tile1V(1),  Tile2U(1),  Tile2V(1)	
{
	Mesh = NULL;
    Shader = NULL;
}


/*SkyLayer::SkyLayer(WorldMesh* mesh)
: 	Rot1(0),Rot2(0),
	Pan1U(0),Pan1V(0),Pan2U(0),Pan2V(0),
	Tile1U(1),Tile1V(1),Tile2U(1),Tile2V(1)	
{
	Mesh = mesh;
}//*/

SkyLayer::~SkyLayer()
{
	if (Mesh)
		delete Mesh;
}

void SkyLayer::RenderInitialize(RenderDevice* dev, MaterialSystem* matSys)
{
    if (Mesh){
        //TRACE("SkyLayer::RenderInitialize(): initializing the sky mesh '%s'\n", Mesh->Name.AsChar());
		Mesh->RenderInitialize(dev, matSys, "Skies/");
        //TRACE("SkyLayer::RenderInitialize(): done initializing the sky mesh '%s'\n", Mesh->Name.AsChar());
    }
}

void SkyLayer::RenderCleanup(RenderDevice* dev, MaterialSystem* matSys)
{
	if (Mesh)
		Mesh->RenderCleanup(dev, matSys);
}

void SkyLayer::Render()
{
	RenderDevice* dev = GEngine->Video->RenderDev;
    dev->SetStreams(Mesh->Data->VertStream, Mesh->Data->VertStreamCount, Mesh->Data->ElemStream);

#ifdef TRANSGAMING_MESH_WARNINGS
    //TRACE("Sky::Render(): drawing sky mesh for object '%s' {verts = %d, tris = %d}\n", Mesh->Name.AsChar(), Mesh->VertList.Size(), Mesh->Triangles.Size());
#endif
	Mesh->RenderDefaultPass(Mesh->DefaultShader);
}

void SkyLayer::Setup()
{
    TRACE("SkyLayer::Setup(): setting up the sky layer '%s'\n", Mesh->Name.AsChar());
	RenderDevice* dev = GEngine->Video->RenderDev;
	MaterialSystem* matSys = GEngine->Video->MaterialSys;
	Mesh->RenderCleanup(dev, matSys);
	Mesh->AlphaMap.Name = Mask;
	Mesh->LightMapName  = LMap;
	for (int g=0; g<Mesh->Data->Groups.Size(); ++g)
	{
		Mesh->Data->Groups[g].TexName = Tex1;
		Mesh->Data->Groups[g].BlendMap.Name = Tex2;
	}

#ifndef TRANSGAMING
    // eric-> the RenderInitialize() function gets called again almost immediately after this
    //   function exits.  This means that all textures will be loaded twice, vertices will be
    //   converted twice, and vertex and index buffers will be recreated (ending up in a small
    //   memory/resource leak).  Skipping this here will let the SkyLayer::RenderInitialize()
    //   function perform its actual job instead of letting it do something that's already been
    //   done :P.
	Mesh->RenderInitialize(dev, matSys, "Skies/");
#endif
    TRACE("SkyLayer::Setup(): done setting up the sky layer '%s'\n", Mesh->Name.AsChar());
}

void SkyLayer::Tick(float time)
{
	for (int i=0; i<Mesh->Data->Groups.Size(); ++i) 
	{
		SimpleMesh::Group& grp=Mesh->Data->Groups[i];

		grp.ColorMap.Tile.X = Tile1U;
		grp.ColorMap.Tile.Y = Tile1V;
		grp.ColorMap.Pan.X	= Pan1U;
		grp.ColorMap.Pan.Y	= Pan1V;
		grp.ColorMap.Rot	= Rot1;

		grp.BlendMap.Tile.X = Tile2U;
		grp.BlendMap.Tile.Y = Tile2V;
		grp.BlendMap.Pan.X	= Pan2U;
		grp.BlendMap.Pan.Y	= Pan2V;
		grp.BlendMap.Rot	= Rot2;
	}
}

// =========================================================================================
// [Sky]
// =========================================================================================

Sky::Sky() : Time(0), LowQuality(false), FlashTime(-1), Height(0), Angle(0)
{
}

Sky::~Sky()
{
}

SkyLayer* Sky::GetLayer(int layer)
{
	if (layer>=0 && layer<Layers.Size())
		return &Layers[layer];
	else
		return NULL;
}

void Sky::RenderInitialize(RenderDevice* dev, MaterialSystem* matSys)
{
	for (int i=0; i<Layers.Size(); ++i)
		Layers[i].RenderInitialize(dev, matSys);
}

void Sky::RenderCleanup(RenderDevice* dev, MaterialSystem* matSys)
{
	for (int i=0; i<Layers.Size(); ++i)
		Layers[i].RenderCleanup(dev, matSys);
}

void Sky::Tick(float timeDelta)
{
	Time += timeDelta;

	for (int l=0; l<Layers.Size(); ++l)
	{
		Layers[l].Tick(Time);
	}

    if (FlashTime!=-1)
    {
        if (FlashTime<Time)
        {
            Texture *tex = Layers[0].Mesh->Data->Groups[0].TexPtr;
            tex->DeviceId = tex->FrameIds[0];
            FlashTime = -1;
        }
    }
}

void Sky::Unload()
{
    FlashTime = -1;
	Layers.ClearArray();
}

int Sky::Load(const char* mpkName)
{
    FlashTime = -1;
	RenderDevice* dev = GEngine->Video->RenderDev;
	MaterialSystem* matSys = GEngine->Video->MaterialSys;

	for (int i=0; i<Layers.Size(); ++i)
		RenderCleanup(dev, matSys);

	Layers.ClearArray();

	DynamicArray<WorldMesh*> Meshes = World::LoadMeshPakArray(mpkName);

	char layerNum[256];

	Layers.ResizeArray(Meshes.Size());
	int layer = 0;

	for (int j=0; j<100; ++j)
	{
		sprintf(layerNum, "layer%.2d", j);

		for (int i=0; i<Meshes.Size(); ++i)
		{
			if (strstr(Meshes[i]->Name.AsChar(),layerNum))
			{
				Meshes[i]->Flags |= SimpleMesh::ENoDepth;
				Layers[layer].Mesh = Meshes[i];
#ifdef	TRANSGAMING
				GEngine->pcfWorld->PtrMap[Meshes[i]] = layer;
# ifdef FIND_HASH_PEAK
                if (GEngine->pcfWorld->PtrMap.Size() > GEngine->pcfWorld->mMaxPtrMapUsage){
                    GEngine->pcfWorld->mMaxPtrMapUsage = GEngine->pcfWorld->PtrMap.Size();
                    TRACE("@@@ Sky::Load(): peak PtrMap hash usage = %d\n", GEngine->pcfWorld->mMaxPtrMapUsage);
                }
# endif
#else
				GEngine->World->PtrMap[Meshes[i]] = layer;
#endif
				layer++;
				break;
			}
		}
	}

	Layers.Shrink(layer);

	return layer;
}

void Sky::Draw()
{
	if (Layers.Size()==0)
		return;

	RenderDevice* dev = GEngine->Video->RenderDev;
    Viewport &pViewport = GEngine->Video->GetActiveView()->LocalViewport;

	Matrix ObjToScr = pViewport.WorldToCamera;
	ObjToScr.SetTranslation(Vector(0,Height,0));
	ObjToScr = YRotationMatrix(Angle)*ObjToScr;
#ifdef TRANSGAMING
	dev->SetCameraTransform(&ObjToScr);
	dev->SetObjectTransform(GIdentityMatrix);
#endif
	ObjToScr *= pViewport.GetCameraToScreen();

	ObjToScr.A02 = ObjToScr.A03;
	ObjToScr.A12 = ObjToScr.A13;
	ObjToScr.A22 = ObjToScr.A23;
	ObjToScr.A32 = ObjToScr.A33-1/65536.f;
	dev->SetShaderMemory(0,&ObjToScr.GetTranspose()[0], 4);

	Matrix CamtoScr = pViewport.CameraToScreen;
	CamtoScr.A02 = CamtoScr.A03;
	CamtoScr.A12 = CamtoScr.A13;
	CamtoScr.A22 = CamtoScr.A23;
	CamtoScr.A32 = CamtoScr.A33-1/65536.f;
	dev->SetScreenTransform(&CamtoScr);

	dev->SetFog(false);
	for (int i=0; i<Layers.Size(); ++i)
	{
		Layers[i].Render();
	}
#ifdef	TRANSGAMING
	dev->SetFog(GEngine->pcfWorld->UseFog);
#else
	dev->SetFog(GEngine->World->UseFog);
#endif

	dev->SetScreenTransform(&pViewport.CameraToScreen);

#ifdef TRANSGAMING
	dev->SetCameraTransform(&pViewport.WorldToCamera);
#endif
}

void Sky::FlashTexture(float time, int texnum)
{    
    if (Layers.Size()>0 && Layers[0].Mesh->Data->Groups[0].TexPtr)
	{
        Texture *tex = Layers[0].Mesh->Data->Groups[0].TexPtr;

		if (texnum<tex->NumFrames)
		{
			FlashTime = Time+time;
			tex->DeviceId = tex->FrameIds[texnum];
		}
		else
		{
			FlashTime = -1;
		}
	}
}
