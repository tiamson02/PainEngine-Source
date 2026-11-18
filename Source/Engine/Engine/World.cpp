// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

#ifndef FINAL_BUILD
#define SHOW_ZONE_PORTAL_DATA
#endif

//#define SHADOW_TEST

#include "Engine.h"

#include "Core/FileSystem/FileManagerStandard.h"
extern ENGINE_API FileManagerStd GFileMan;

#include "Render/HUD.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Render/Material.h"
#include "Render/ParticleEffect.h"
#include "Render/Billboard.h"
#include "Render/Decal.h"
#include "Render/Trail.h"
#include "Render/View.h"
#include "Engine/Portal.h"
#include "Engine/Mesh.h"
#include "Render/FogVolume.h"
#include "Core/List.h"
#include "Physics/Ragdoll.h"
#include "Physics/PhysicsEngine.h"
#include "Physics/PhysicsWorld.h"
#include "Engine/Region.h"
#include "Engine/Sound.h"
#include "Physics/PhysicsObject.h"
#include "Render/Video.h"
#include "Engine/Model.h"
#include "Engine/World.h"
#include "Engine/PainMenu.h"
#include "Net/NetworkDevice2.h"

#ifndef TRANSGAMING_NO_MILES
// Blair - Keeps this off until we have a miles-psp solution
#include "Audio/MilesEngine.h"
#endif

#include "Core/nommgr.h"
#ifndef HAVOK3
#include <hkbase/hkBase.h>
#endif
#include "Core/mmgr.h"

IMPLEMENT_OBJECT_CLASS(Zone);
IMPLEMENT_OBJECT_CLASS(Portal);
IMPLEMENT_OBJECT_CLASS(AntiPortal);
IMPLEMENT_OBJECT_CLASS(WorldMesh);

#define USE_ENTITYCACHE
//#define DA_CACHE

#ifdef USE_ENTITYCACHE
static bool gCacheEnabled = false;
bool ECache_DeleteEntity(Entity* e);
Entity* ECache_CreateEntity(int type, const char* fileName, const char* entityName, float scale, bool centerGeometry = false);
void ECache_Clear();
#endif

class CPackManager;
extern CPackManager *GPackManager;
// ==========================================================================
#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
int World::mMaxPtrMapUsage = 0;
int CPackageManager::mMaxCacheUsage = 0;
#endif


World::World()
:   EntityList(0)
#ifndef TRANSGAMING
,   PackageCount(0)
#endif
,   Scale(1.0f), UseFog(false), LateVBs(false)
,	FarClipDist(4096.0f),DynamicSpecular(0.5,0.5,0.5,20), RTCubeMap(false)
,	InLoadingScreen(false), Frame(0), Overbright(false),GhostsFX(false)
,	PixelLightingMultiplier(1),Name(""), FixedMeshCounter(0)
,	TexSceneCubemap(NULL), TexStaticCubemap(NULL), RenderToCubemap(false), TexSceneDepth(NULL), UseNewPaths(false)
,	Physics(0), LoadInProgress(false), DrawDynLights(true), HighQualityLights(false), WaterQuality(1)
,	LastSwitchZoneIndex(-1)
,	LastSwitchZoneName("(none)"), CubemapFrame(0)
,	PickedEntity(NULL), SwitchZonesOff( false )
,	TargetsReleased(true)
{
    TRACE("World::World(): creating a new world at %p\n", this);
#ifdef	TRANSGAMING
	Env.AmbientColor = Color_TG(0.2f, 0.2f, 0.2f);
#else
	Env.AmbientColor = Color(0.2f, 0.2f, 0.2f);
#endif
#ifndef TRANSGAMING
    GMemZero(&Packages[0],sizeof(Packages));
#endif
	GMemZero(&TexSceneMotionBlur[0],sizeof(TexSceneMotionBlur));
	Paths.Init(this);

    EntityGIndexes.Add(NULL); // 0 idx is null

	ScriptObject cfg = GScript.Globals()["Cfg"];
	DrawDynLights = cfg["DynamicLights"].GetInt();

	TexFB[0] = TexFB[1] = TexFB[2] = NULL;

	TexZBuf	= NULL;
	for (int i=0; i<4; ++i)
		TexFrame[i]	= NULL;

#ifndef TRANSGAMING
	SkySpecularCubemap = NULL;
#endif
	WaterMesh = NULL;
}

World::~World()
{
	Release();

	for (int i=0; i<Models.Size(); ++i)
		if (Models[i])
			delete Models[i];

    for (int i=0; i<Lights.Size(); ++i)
        if (Lights[i])
            delete Lights[i];

	/*
    for (int i=0; i<ParticleEffects.Size(); ++i)
        if (ParticleEffects[i])
	*/

//	for( int i = 0; i < MAX_NET_INDICES; ++i )
//		EntityNetIndices[i] = NULL;
//	FirstFreeNetIndex = 0;
//	FreeNetIndices.Clear();
//	NetIndicesWaitingToBeReused.Clear();
//	NetIndicesWaitingToBeReusedTime.Clear();
}

// internal
int World::CreatePlane(const Vector& a, const Vector& b, const Vector& c)
{
	Plane plane(a,b,c);
	plane.Snap();

 	for (int i=0; i<Planes.Size(); ++i) 
	{
		if (plane.Equal(Planes[i])) 
			return i;
	}

    return Planes.Add(plane);
}

static void ReleaseTexture(RenderDevice* Device, Texture*& Tex)
{
	if (Tex)
	{
		Device->ReleaseResource(RenderDevice::EResTexture, &Tex->DeviceId, 1);
		delete Tex;
		Tex = NULL;
	}
}

void World::Release(const char* NewMap)
{
    TRACE("World::Release(): starting world release {memUsed = %d}\n", m_getMemUsed());
#if defined(MEMORY_TEST) && defined(TRANSGAMING)
    m_dumpMemoryReport("memory report (world release start).txt", false, true);
#endif

#ifdef USE_ENTITYCACHE
    // clear cache
    gCacheEnabled = false;
    ECache_Clear();
#endif
	// reset world attributes
	GhostsFX = false;
	DemonFX.Enabled = false;
	DemonFX.Super   = false;


    // delete entities
	DeleteDelayedEntities();
    TRACE("World::Release(): emptied the entity cache {memUsed = %d}\n", m_getMemUsed());

	if (TexStaticCubemap) {
		GEngine->Video->MaterialSys->ReleaseTexture(TexStaticCubemap);
		TexStaticCubemap = 0;
	}

#ifdef DA_CACHE
	const bool sameLevel = (NewMap && FileName.AsChar() && FileName.Len() > 0 && !stricmp(NewMap,FileName.AsChar()));
#else
	const bool sameLevel = false;
#endif // DA_CACHE

	RenderDevice *dev = GEngine->GetRenderDevice();

	if (dev) {
		
		TargetsReleased = 0;

		if (!sameLevel) {
            TRACE("World::Release(): releasing textures and render targets {memUsed = %d}\n", m_getMemUsed());
			// delete textures
			if (TexSceneCubemap) {
				dev->ReleaseResource(RenderDevice::EResTexture,&TexSceneCubemap->DeviceId,1);
				delete TexSceneCubemap;
				TexSceneCubemap = 0;
			}

			for (int i=0; i<3; ++i)
				ReleaseTexture(dev, TexFB[i]);

#ifndef TRANSGAMING
			ReleaseTexture(dev, SkySpecularCubemap);	// TODO: wtf is this?! not used anywhere
#endif
			ReleaseTexture(dev, TexZBuf);
			for (int i=0; i<4; ++i)
				ReleaseTexture(dev, TexFrame[i]);

			for (int i=0; i<TEX_MBLUR; ++i)
			{
				if (TexSceneMotionBlur[i])
				{
					dev->ReleaseResource(RenderDevice::EResTexture,&TexSceneMotionBlur[i]->DeviceId,1);
					delete TexSceneMotionBlur[i];
					TexSceneMotionBlur[i] = 0;
				}
			}

			if (TexSceneDepth) 
			{
				dev->ReleaseResource(RenderDevice::EResTexture,&TexSceneDepth->DeviceId,1);
				delete TexSceneDepth;
				TexSceneDepth = 0;
			}

			TargetsReleased = 1;
            TRACE("World::Release(): done releasing textures and render targets {memUsed = %d}\n", m_getMemUsed());
		}

#ifdef DA_CACHE
		// freeze refs
		GEngine->Video->MaterialSys->FreezeRefs = sameLevel;
#endif

        TRACE("World::Release(): cleaning up animated meshes and sky {memUsed = %d}\n", m_getMemUsed());
		// remove meshes from physics
		// cleanup meshes
		AnimMeshManager.RenderCleanup(dev,GEngine->Video->MaterialSys);
        mSky.RenderCleanup(dev,GEngine->Video->MaterialSys);
        TRACE("World::Release(): done cleaning up meshes and sky {memUsed = %d}\n", m_getMemUsed());
	}

    else
        TRACE("World::Release(): ERROR-> no render device to release resources from! {memUsed = %d}\n", m_getMemUsed());


	mSky.Unload();
    TRACE("World::Release(): unloaded sky {memUsed = %d}\n", m_getMemUsed());

    for (int i=0; i<Meshes.Size(); ++i)
	{
		WorldMesh* mesh = Meshes[i];
		if (mesh)
		{
			if (GEngine->Video)
				mesh->RenderCleanup(GEngine->Video->RenderDev,GEngine->Video->MaterialSys);

			RemoveEntity(mesh);
//			RemoveEntityFromNetworkStructs(mesh,false);

            EntityGIndexes[mesh->GIndex] = NULL;
			delete mesh;
		}
	}
    TRACE("World::Release(): released all world meshes {memUsed = %d}\n", m_getMemUsed());

	// unfreeze refs
	//GEngine->Video->MaterialSys->FreezeRefs = 0;

    // clear entity list
	Entity* next;
    for (Entity* entity = EntityList; entity; entity=next)
    {
		next = entity->Next;
#ifndef TRANSGAMING_NO_HAVOK
		entity->UnregisterAllChildren(); // XXX
#endif
			DeleteEntity(entity);
    }
    TRACE("World::Release(): cleared entity list {memUsed = %d}\n", m_getMemUsed());

    
	Volumes.ClearArray();
    Meshes.ClearArray();
    TRACE("World::Release(): cleared volumes and meshes arrays {memUsed = %d}\n", m_getMemUsed());

	// remove glass structures
    for( int i = 0; i < Glasses.Size(); ++i )
		delete Glasses[i];
	Glasses.ClearArray();
    TRACE("World::Release(): cleared glasses array {memUsed = %d}\n", m_getMemUsed());

	for( int i = 0; i < Portals.Size(); ++i )
		delete Portals[i];
	Portals.ClearArray();
    TRACE("World::Release(): cleared portals array {memUsed = %d}\n", m_getMemUsed());

	for( int i = 0; i < Mirrors.Size(); ++i )
		delete Mirrors[i];
	Mirrors.ClearArray();
    TRACE("World::Release(): cleared mirrors array {memUsed = %d}\n", m_getMemUsed());

	for( int i = 0; i < AntiPortals.Size(); ++i )
		delete AntiPortals[i];
	AntiPortals.ClearArray();
    TRACE("World::Release(): cleared antiportals array {memUsed = %d}\n", m_getMemUsed());

	for( int i = 0; i < Zones.Size(); ++i )
		delete Zones[i];
	Zones.ClearArray();
    TRACE("World::Release(): cleared zones array {memUsed = %d}\n", m_getMemUsed());

	for (int i=0; i<Environments.Size(); ++i)
		if (Environments[i])
			delete Environments[i];
	Environments.ClearArray();
    TRACE("World::Release(): cleared environments array {memUsed = %d}\n", m_getMemUsed());

	for( int i = 0; i < SwitchZones.Size(); ++i )
		delete SwitchZones[i];
	SwitchZones.ClearArray();
	LastSwitchZoneIndex = -1;
	LastSwitchZoneName = "(none)";
    TRACE("World::Release(): cleared switch zones {memUsed = %d}\n", m_getMemUsed());

	for( int i = 0; i < DeathZones.Size(); ++i )
		delete DeathZones[i];
	DeathZones.ClearArray();
    TRACE("World::Release(): cleared death zones {memUsed = %d}\n", m_getMemUsed());

	for( int i = 0; i < IceZones.Size(); ++i )
		delete IceZones[i];
	IceZones.ClearArray();
    TRACE("World::Release(): cleared ice zones {memUsed = %d}\n", m_getMemUsed());

	for( int i = 0; i < LadderZones.Size(); ++i )
		delete LadderZones[i];
	LadderZones.ClearArray();
    TRACE("World::Release(): cleared ladder zones {memUsed = %d}\n", m_getMemUsed());

	// Release static geometry and active meshes
	if( Physics )
	{
#ifndef TRANSGAMING_NO_HAVOK
		Physics->ReleaseStaticMeshes();
        TRACE("World::Release(): released meshes from the local physics engine\n");
		delete Physics;
        TRACE("World::Release(): destroying local physics engine\n");
#endif
		Physics = 0;
	}
    TRACE("World::Release(): cleared havok meshes {memUsed = %d}\n", m_getMemUsed());

#ifndef TRANSGAMING
    // eric-> the packages array is a remnant of the old MPK loading method
    // clear mesh packages
    for (int i=0; i<PackageCount; ++i)
    {
        if (Packages[i])
        {
            delete Packages[i];
            Packages[i]=0;
        }
    }
    PackageCount=0;
    TRACE("World::Release(): cleared packages {memUsed = %d}\n", m_getMemUsed());
#endif

    // kill loaded packages
    // mesh destructors will cleanup buffers and textures
    // remove models

	FixedMeshCounter = 0;
    MPKFilesLoaded.ClearArray();

    Name = "";
    FileName = "";
    LevelName = "";
    EntityList = NULL;
	WaterMesh = NULL;

	if( GEngine->Video && GEngine->Video->GetActiveView() )
		GEngine->Video->GetActiveView()->Mode &= ~View::EModeNoAntiportals;
    
    #ifndef FINAL_BUILD
    for (int i=0;i<EntityGIndexes.Size();i++)
        if (EntityGIndexes[i])
        {
#ifdef TRANSGAMING
            Entity * e = EntityGIndexes[i];


            TRACE("[]::World::Release(): WARNING-> entity is not deleted! (GIndex = %d, type = %d, filename = '%s', name = '%s')\n", 
                    i, e->Type,
                    e->FileName.Size() ? e->FileName.AsChar() : "",
                    e->Name.Size() ? e->Name.AsChar() : "");
#else
            char buf[1000];
            Entity * e = EntityGIndexes[i];
            sprintf(buf,"GIndex: %d, Type: %d, %s, %s", i, e->Type, e->FileName.Size()?e->FileName.AsChar():"", e->Name.Size()?e->Name.AsChar():"");
            MessageBox(NULL,buf,"Critical Error - Entity is not deleted",MB_TOPMOST);
#endif
        }
    #endif

        
    EntityGIndexes.ClearArray();
    EntityGIndexes.Add(NULL); // 0 idx is null
	
    TRACE("World::Release(): cleared extra entity indices {memUsed = %d}\n", m_getMemUsed());
	EntityHash.Clear();
    TRACE("World::Release(): cleared the entity hash table {memUsed = %d}\n", m_getMemUsed());
    
    AnimMeshManager.Clear();
    TRACE("World::Release(): cleared the animated mesh manager {memUsed = %d}\n", m_getMemUsed());
    LastExplodedEntities.ClearArray();
    TRACE("World::Release(): cleared the exploded items array {memUsed = %d}\n", m_getMemUsed());

    TRACE("World::Release(): clearing the package manager {size = %d, memUsed = %d}\n", GEngine->PackManager->Size(), m_getMemUsed());
    GEngine->PackManager->Clear();
    TRACE("World::Release(): cleared the package manager {memUsed = %d}\n", m_getMemUsed());

#ifndef TRANSGAMING_NO_MILES
	if (GEngine->Audio)
		GEngine->Audio->LoadFilesOffMemory();
#endif
	AntiPortal::Counter = 0;

    TRACE("World::Release(): done releasing the world {memUsed = %d}\n", m_getMemUsed());
#if defined(MEMORY_TEST) && defined(TRANSGAMING)
    m_dumpMemoryReport("memory report (world release end).txt", false, true);
#endif

	GEngine->Video->ParticleSys.Clear();
	GEngine->PackManager->Clear();
#ifndef TRANSGAMING_NO_MILES
	GEngine->Audio->Clear();
#endif

#ifdef DEBUG
    TRACE("World::Release(): world release complete.  Remaining items: {PtrMap.size = %d, Models.size = %d, ParticleEffects.size = %d}\n",
            PtrMap.Size(), Models.Size(), ParticleEffects.Size());
#endif
}

void World::ReleaseWithoutMap()
{
    TRACE("World::ReleaseWithoutMap(): releasing the world without map {memUsed = %d}\n", m_getMemUsed());
	DeleteDelayedEntities();

    TRACE("World::ReleaseWithoutMap(): deleted delayed entities {memUsed = %d}\n", m_getMemUsed());

	// clear entity list
	Entity* next;
	for (Entity* entity = EntityList; entity; entity=next)
	{
		next = entity->Next;

		if (entity->Flags & Entity::EEntDestroyOnReload)
		{
#ifndef TRANSGAMING_NO_HAVOK
			entity->UnregisterAllChildren(); // XXX
#endif
			DeleteEntity(entity);
		}
	}
    TRACE("World::ReleaseWithoutMap(): deleted all entities {memUsed = %d}\n", m_getMemUsed());


	for (int i=0; i<Environments.Size(); ++i)
		if (Environments[i])
			delete Environments[i];

    TRACE("World::ReleaseWithoutMap(): deleted environments {memUsed = %d}\n", m_getMemUsed());

	Environments.ClearArray();
    LastExplodedEntities.ClearArray();
    TRACE("World::ReleaseWithoutMap(): cleared arrays {memUsed = %d}\n", m_getMemUsed());
}

#ifdef TRANSGAMING_USE_EDITOR
BOOL World::SaveMeshPakArray(const char* name, const DynamicArray<WorldMesh*> Meshes)
{
	FileObject* file = GFileMan.CreateFileWriter(name);
	DynamicArray<DWORD>	offsets;

	if (file) 
	{
		int count = Meshes.Size();

		DWORD offsets[4096];

		for (int i=0; i<count; ++i) 
		{
			offsets[i] = file->Tell();
			Meshes[i]->SaveMesh(file);
		}

		for (int i=0; i<count; ++i)
			*file << offsets[i];

		*file << Meshes.Size();
		*file << 0xDeadBeef;

		GFileMan.Close(file);

		return TRUE;
	}

	return FALSE;
}
#endif

DynamicArray<WorldMesh*> World::LoadMeshPakArray(const char* name)
{
	DynamicArray<WorldMesh*> Meshes;

	FileObject* file = GFileMan.CreateFileReader(name);

	if (file) {
		DWORD magic;
		int pos;

		pos = file->GetSize() - sizeof(DWORD);
		file->Seek(pos);
		*file >> magic;
		if (magic == 0xDeadBeef) {
			int count;

			pos -= sizeof(DWORD);
			file->Seek(pos);
			*file >> count;

			if (count > 0) {
				DWORD offsets[4096];

				pos -= count*sizeof(DWORD);
				file->Seek(pos);

#ifdef TRANSGAMING
                file->Read(offsets, sizeof(DWORD) * count);
#else
				for (int i=0; i<count; ++i)
					*file >> offsets[i];
#endif

				for (int i=0; i<count; ++i) {
                    m_setTag("MeshWorld");
					WorldMesh* mesh = new WorldMesh();

					file->Seek(offsets[i]);
					mesh->LoadMesh(file);
					Meshes.Add(mesh);
				}
			}
		}
		GFileMan.Close(file);
	}

	return Meshes;
}

BOOL World::LoadMeshPakFile(const char* name)
{
#ifdef TRANSGAMING
    char        tmpBuf[MAX_PATH];
    const char *baseName = String::GetFilename(name);
	int         mpkIndex = MPKFilesLoaded.AddUnique( baseName );


    Assert(mpkIndex < MAX_MPKINDEX);
#else
	String baseName = String::BaseName( name );
	int mpkIndex = MPKFilesLoaded.AddUnique( baseName );
#endif
	int nonBoxZones = 0;

#ifndef HAVOK3 // For now, turn this off...
	if( !GEngine->Physics )
		hkBaseSystem::init();	// to enable generation of convex hulls when there's no physics in engine
#endif
    TRACE("World::LoadMeshPakFile(): loading a mesh pack from the file '%s' (MPKIndex = %d)\n", name, mpkIndex);

	FileObject* file = GFileMan.CreateFileReader(name);
	if (file) {
        TRACE("World::LoadMeshPakFile(): opened the pack file '%s'\n", name);
		DWORD magic;
		int pos;

		pos = file->GetSize() - sizeof(DWORD);
		file->Seek(pos);
		*file >> magic;
		if (magic == 0xDeadBeef) {
			int count;

			pos -= sizeof(DWORD);
			file->Seek(pos);
			*file >> count;

			if( InLoadingScreen )
				GScript.DoString("MenuLocalLoadingScreenObjectsOverall = MenuLocalLoadingScreenObjectsOverall + %d", count);

			if( GEngine->Video )
			{
				LoadingScreen &lscreen = GEngine->Video->PMenu->LoadScreen;
				if( lscreen.Active() )
					lscreen.SetOverall( lscreen.GetOverall() + count, 20 );
			}

			if (count > 0) {
				int c_zones,c_portals;
				DWORD offsets[4096];

				pos -= count*sizeof(DWORD);
				file->Seek(pos);
#ifdef TRANSGAMING
				// Let us save the overhead of reading from the file a few hundred times and just read once...
				file->Read( offsets, count * sizeof(DWORD) );
#else
				for (int i=0; i<count; ++i)
					*file >> offsets[i];
#endif
				c_zones = c_portals = 0;

				for (int i=0; i<count; ++i) {
                    m_setTag("MeshWorld");
					WorldMesh *             mesh = new WorldMesh();
                    WorldMesh::VolumeParams volParams;


					mesh->Flags &= ~Entity::EEntDestroyOnReload;
					mesh->FixedMeshIndex = FixedMeshCounter++;
					Assert( mesh->FixedMeshIndex < 65535 );

					file->Seek(offsets[i]);
					mesh->LoadMesh(file, &volParams);

					if (strstr(mesh->Name.AsChar(),"portal")) {

                        m_setTag("Portal");
						Portal* portal = new Portal();

						portal->BuildFromMesh(this, mesh);
						Portals.Add(portal);

#ifdef SHOW_ZONE_PORTAL_DATA
						GLog.Print("Portal [%02d] \"%s\" Box\t:",c_portals,portal->Name.AsChar());
						GLog.Print(portal->Box);
						GLog.Print("\n");
#endif
						c_portals++;
						delete mesh;
					} else if( strstr(mesh->Name.AsChar(),"antyp") ) {
                        m_setTag("PortalAnti");
						AntiPortal* antiportal = new AntiPortal( mesh );
						if( !antiportal->BuildTriangleAdjacencies() )
						{
							GLog.Print("WARNING: Antiportal %s is not a closed shape, ignoring!\n", antiportal->Name.AsChar() );
							delete antiportal;
						}
						else
						{
							if( strstr(mesh->Name.AsChar(),"antypswitch") ) antiportal->Disabled = true;
							AntiPortals.Add( antiportal );
#ifdef SHOW_ZONE_PORTAL_DATA
							GLog.Print("Antiportal %s, Box\t:",antiportal->Name.AsChar());
							GLog.Print(antiportal->Box);
							if( antiportal->Disabled ) GLog.Print( ", switch." );
							GLog.Print(antiportal->Box);
							GLog.Print("\n");
#endif
						}
						delete mesh;
					} else if (strstr(mesh->Name.AsChar(),"zone")) {

                        m_setTag("Zone");
						Zone* zone = new Zone();
						zone->Name = mesh->Name;
						zone->BBox = mesh->Box;

						DynamicArray<int> vertexTranslator;
						for( int i = 0; i < mesh->Data->VertList.Size(); ++i )
						{
							Vector v = mesh->Data->VertList.getVertPos(i);
							int j;
							for( j = 0; j < zone->Vertices.Size(); ++j )
								if( zone->Vertices[j] == v )
									break;
							vertexTranslator.Add( j );
							if( j == zone->Vertices.Size() )
								zone->Vertices.Add( v );
						}

                        //make sure the zone's mesh isn't tri stripped
                        Assert(mesh->PrimType != RenderDevice::EPrimTriStrip);

#ifdef TRANSGAMING_DEINDEX
                        if (!mesh->isIndexed){
						    for( int i = 0; i < mesh->VertList.Size(); ++i )
							    zone->Triangles.Add( vertexTranslator[i] );
                        }

                        else{
#endif
						    for( int i = 0; i < mesh->Data->Triangles.Size(); ++i )
							    zone->Triangles.Add( vertexTranslator[mesh->Data->Triangles[i]] );
#ifdef TRANSGAMING_DEINDEX
                        }
#endif

						zone->BuildConvex();
						zone->IsBox = zone->IsABoxReally();
						if( !zone->IsBox )
							nonBoxZones++;

						if (strstr(zone->Name.AsChar(),"death") || strstr(zone->Name.AsChar(),"DEATH"))
						{
#ifdef SHOW_ZONE_PORTAL_DATA
							GLog.Print("DEATH Zone\t:");
							GLog.Print(zone->BBox);
							GLog.Print("\n");
#endif
							DeathZones.Add(zone);
						}
						else if (strstr(zone->Name.AsChar(),"ice") || strstr(zone->Name.AsChar(),"ICE"))
						{
#ifdef SHOW_ZONE_PORTAL_DATA
							GLog.Print("ICE Zone\t:");
							GLog.Print(zone->BBox);
							GLog.Print("\n");
#endif
							IceZones.Add(zone);
						}
						else if (strstr(zone->Name.AsChar(),"ladder") || strstr(zone->Name.AsChar(),"LADDER"))
						{
#ifdef SHOW_ZONE_PORTAL_DATA
							GLog.Print("Ladder Zone\t:");
							GLog.Print(zone->BBox);
							GLog.Print("\n");
#endif
							LadderZones.Add(zone);
						}
						else if (strstr(zone->Name.AsChar(),"switch") || strstr(zone->Name.AsChar(),"SWITCH"))
						{
#ifdef SHOW_ZONE_PORTAL_DATA
							GLog.Print("SWITCH Zone\t:");
							GLog.Print(zone->BBox);
							GLog.Print("\n");
#endif
							SwitchZones.Add(zone);
						}
						else
						{
							zone->Index = Zones.Add(zone);
#ifdef SHOW_ZONE_PORTAL_DATA
							GLog.Print("Zone [%02d] \"%s\" Box\t:",c_zones,zone->Name.AsChar());
							GLog.Print(zone->BBox);
							GLog.Print("\n");
#endif
							c_zones++;
						}
						delete mesh;
					} else if (strstr(mesh->Name.AsChar(),"vollight")) {

                        m_setTag("Light");
#ifdef TRANSGAMING_MESH_WARNINGS
                        TRACE("World::LoadMeshPakFile(): creating a new light volume from object '%s' {data = %p, vertList = %p, this = %p}\n", mesh->Name.AsChar(), mesh->Data, &mesh->Data->VertList, mesh);
#endif
						Volume *vol = new Volume(*mesh, Volume::EVolLight);

                        //copy the volume information into the new volume
                        if (mesh->hasVolumeInfo){
                            vol->VolColor = volParams.color;
                            vol->VolEnd = volParams.end;
                        }

						AddEntity(vol);
						PtrMap[vol] = Volumes.Add(vol);
#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
                        if (PtrMap.Size() > mMaxPtrMapUsage){
                            mMaxPtrMapUsage = PtrMap.Size();
                            TRACE("@@@ World::LoadMeshPakFile(): peak PtrMap hash usage = %d\n", mMaxPtrMapUsage);
                        }
#endif
                        delete mesh;

					} else if (strstr(mesh->Name.AsChar(),"volfog")) {

                        m_setTag("Fog");
#ifdef TRANSGAMING_MESH_WARNINGS
                        TRACE("World::LoadMeshPakFile(): creating a new fog volume from object '%s' {data = %p, vertList = %p, this = %p}\n", mesh->Name.AsChar(), mesh->Data, &mesh->Data->VertList, mesh);
#endif
						Volume *vol = new Volume(*mesh, Volume::EVolFog);

                        //copy the volume information into the new volume
                        if (mesh->hasVolumeInfo){
                            vol->VolColor = volParams.color;
                            vol->VolEnd = volParams.end;
                        }

						AddEntity(vol);
						PtrMap[vol] = Volumes.Add(vol);
#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
                        if (PtrMap.Size() > mMaxPtrMapUsage){
                            mMaxPtrMapUsage = PtrMap.Size();
                            TRACE("@@@ World::LoadMeshPakFile(): peak PtrMap hash usage = %d\n", mMaxPtrMapUsage);
                        }
#endif
						delete mesh;

					} else if (strstr(mesh->Name.AsChar(),"glass")) {
						mesh->MPKIndex = mpkIndex;
						if (mesh->Flags & SimpleMesh::EVertFormatNTU)
						{
                            m_setTag("MeshGlass");
							Glass* glass = new Glass( mesh );
							Glasses.Add( glass );
							mesh = glass->GetMainMesh();
							
							AddEntity(mesh);
							PtrMap[mesh] = SlotAdd(Meshes, mesh);
#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
                            if (PtrMap.Size() > mMaxPtrMapUsage){
                                mMaxPtrMapUsage = PtrMap.Size();
                                TRACE("@@@ World::LoadMeshPakFile(): peak PtrMap hash usage = %d\n", mMaxPtrMapUsage);
                            }
#endif
						}
						else
							GLog.Print("WARNING: %s - Glass could not be created (2 uv-sets)\n",mesh->Name.AsChar());
					} else {

                        if( strstr(mesh->Name.AsChar(),"barrier") )
                        {
                            mesh->Flags |= Entity::EEntNoDraw;
                            mesh->ActiveGroup = 99;
                        }

						char* actgrpIndex = strstr(mesh->Name.AsChar(),"actgrp");
                        if( actgrpIndex )
                        {
                            int g = 0;
                            sscanf(actgrpIndex,"actgrp%d",&g);
                            mesh->ActiveGroup = g;
						}
						mesh->MPKIndex = mpkIndex;
						AddEntity(mesh);
						PtrMap[mesh] = Meshes.Add(mesh);
#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
                        if (PtrMap.Size() > mMaxPtrMapUsage){
                            mMaxPtrMapUsage = PtrMap.Size();
                            TRACE("@@@ World::LoadMeshPakFile(): peak PtrMap hash usage = %d\n", mMaxPtrMapUsage);
                        }
#endif
					}

					if( InLoadingScreen )
					{
						GScript.DoString("MenuLocalLoadingScreenObjectsCounter = MenuLocalLoadingScreenObjectsCounter + 1" );
						GScript.DoString("Menu_RenderLoadingScreen(MenuLocalLoadingScreenObjectsCounter)" );
					}

					GEngine->LoadingTick();
				}
			}
		}

        TRACE("World::LoadMeshPakFile(): loaded the mesh pack '%s'\n", name);
		GFileMan.Close(file);
	}

	GLog.Print("Non-box zones: %d\n", nonBoxZones );

#ifndef HAVOK3 // For now, turn this off...
	if( !GEngine->Physics )
		hkBaseSystem::quit();
#endif

	return TRUE;
}

BOOL World::LoadMeshPak(const char* name)
{
#ifdef TRANSGAMING_PSP
	const char* s = strstr(name,".mpk");
	//const char* s = strstr(name,".mpp");
#elif defined(TRANSGAMING_PS2)
	const char* s = strstr(name,".mpk");
    //const char* s = strstr(name,".mp2");
#else
	const char* s = strstr(name,".mpk");
#endif
	if (s)
		return LoadMeshPakFile(name);

    TRACE("*************************************************************************************************\n");
    TRACE("World::LoadMeshPack(): WARNING-> loading a mesh pack from '%s' as an object package\n", name);
    TRACE("*************************************************************************************************\n");
    Assert(!"Not an MPK!");

    return FALSE;
#ifndef TRANSGAMING
    m_setTag("Object");
	ObjectPackage* package = new ObjectPackage(name);
	if( package )
	{
		int num;

        TRACE("World::LoadMeshPak(): loaded package '%s'.  Processing...\n", name);
        //package->Dump(GLog);
		Packages[PackageCount++]=package;

		if( GEngine->Video )
		{
			LoadingScreen &lscreen = GEngine->Video->PMenu->LoadScreen;
			if( lscreen.Active() )
				lscreen.SetOverall( lscreen.GetOverall() + package->Objects.Size(), 20 );
		}

		if( InLoadingScreen )
			GScript.DoString("MenuLocalLoadingScreenObjectsOverall = MenuLocalLoadingScreenObjectsOverall + %d", package->Objects.Size() );

		num = 0;

        for (ClassIterator<Zone> i(package); i; ++i)
        {
            Zone* zone = i.GetInstance();
			if (strstr(zone->Name.AsChar(),"death") || strstr(zone->Name.AsChar(),"DEATH")) {
#ifdef SHOW_ZONE_PORTAL_DATA
				GLog.Print("DEATH Zone\t:");
				GLog.Print(zone->BBox);
				GLog.Print("\n");
#endif
				DeathZones.Add(zone);
				if( InLoadingScreen )
				{
					GScript.DoString("MenuLocalLoadingScreenObjectsCounter = MenuLocalLoadingScreenObjectsCounter + 1" );
					GScript.DoString("Menu_RenderLoadingScreen(MenuLocalLoadingScreenObjectsCounter)" );
				}

				GEngine->LoadingTick();
				continue;
			}

			if (strstr(zone->Name.AsChar(),"ice") || strstr(zone->Name.AsChar(),"ICE")) {
#ifdef SHOW_ZONE_PORTAL_DATA
				GLog.Print("ICE Zone\t:");
				GLog.Print(zone->BBox);
				GLog.Print("\n");
#endif
				IceZones.Add(zone);
				if( InLoadingScreen )
				{
					GScript.DoString("MenuLocalLoadingScreenObjectsCounter = MenuLocalLoadingScreenObjectsCounter + 1" );
					GScript.DoString("Menu_RenderLoadingScreen(MenuLocalLoadingScreenObjectsCounter)" );
				}
				GEngine->LoadingTick();
				continue;
			}

			if( strstr( zone->Name.AsChar(),"ladder") || strstr(zone->Name.AsChar(),"LADDER"))
			{
#ifdef SHOW_ZONE_PORTAL_DATA
				GLog.Print("Ladder Zone\t:");
				GLog.Print(zone->BBox);
				GLog.Print("\n");
#endif
				LadderZones.Add( zone );
				if( InLoadingScreen )
				{
					GScript.DoString("MenuLocalLoadingScreenObjectsCounter = MenuLocalLoadingScreenObjectsCounter + 1" );
					GScript.DoString("Menu_RenderLoadingScreen(MenuLocalLoadingScreenObjectsCounter)" );
				}
				GEngine->LoadingTick();
				continue;
			}

			zone->Index = Zones.Add(zone);

#ifdef SHOW_ZONE_PORTAL_DATA
			GLog.Print("Zone [%02d] Box\t:",num);
			GLog.Print(zone->BBox);
			GLog.Print("\n");
#endif
			num++;
			if( InLoadingScreen )
			{
				GScript.DoString("MenuLocalLoadingScreenObjectsCounter = MenuLocalLoadingScreenObjectsCounter + 1" );
				GScript.DoString("Menu_RenderLoadingScreen(MenuLocalLoadingScreenObjectsCounter)" );
			}
			GEngine->LoadingTick();
        }

		num = 0;

        for (ClassIterator<Portal> i(package); i; ++i)
        {
            Portal* portal = i.GetInstance();
#ifdef SHOW_ZONE_PORTAL_DATA
			GLog.Print("Portal [%02d] Box\t:",num);
			GLog.Print(portal->Box);
			GLog.Print("\n");
#endif
			num++;
			portal->PlaneIndex=CreatePlane(portal->Poly.Verts[0],
			                               portal->Poly.Verts[1],
				                           portal->Poly.Verts[2]);

			portal->BuildOtherPoly();     
			Portals.Add(portal);

			if( InLoadingScreen )
			{
				GScript.DoString("MenuLocalLoadingScreenObjectsCounter = MenuLocalLoadingScreenObjectsCounter + 1" );
				GScript.DoString("Menu_RenderLoadingScreen(MenuLocalLoadingScreenObjectsCounter)" );
			}
			GEngine->LoadingTick();
        }

        for( ClassIterator<AntiPortal> i(package); i; ++i )
        {
			AntiPortal* antiportal = i.GetInstance();

#ifdef SHOW_ZONE_PORTAL_DATA
			GLog.Print("Antiportal %s, Box\t:",antiportal->Name.AsChar());
			GLog.Print(antiportal->Box);
			GLog.Print("\n");
#endif
//			GEngine->LoadingTick();
		}

        for (ClassIterator<WorldMesh> i(package); i; ++i)
        {
            WorldMesh* mesh = i.GetInstance();
			PtrMap[mesh] = Meshes.Add(mesh);
#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
            if (PtrMap.Size() > mMaxPtrMapUsage){
                mMaxPtrMapUsage = PtrMap.Size();
                TRACE("@@@ World::LoadMeshPakFile(): peak PtrMap hash usage = %d\n", mMaxPtrMapUsage);
            }
#endif
            AddEntity(mesh);
			mesh->FileName = name;
			mesh->FixedMeshIndex = FixedMeshCounter++;
            mesh->MPKIndex = mesh->FixedMeshIndex;
			Assert( mesh->FixedMeshIndex < 65535 );

#ifdef SHOW_ZONE_PORTAL_DATA
# ifdef TRANSGAMING
			GLog.Print("mesh [%04d]: %s [L:%s] (%d, %d) (%s) ",num,
						mesh->Name.AsChar(),mesh->LightMapName.AsChar(),
						mesh->VertList.Size(),mesh->Triangles.Size()/3,
                        (mesh->Flags & WorldMesh::EVertFormatNTU ? "NTU" : "TU2"));
# else
			GLog.Print("mesh [%04d]: %s [L:%s] (%d, %d, %d) CloneOf: %s (%s) ",num,
						mesh->Name.AsChar(),mesh->LightMapName.AsChar(),
						mesh->VertList.Size(),mesh->Triangles.Size()/3,mesh->Elems.Size(),
                        mesh->CloneOf.AsChar(),(mesh->Flags & WorldMesh::EVertFormatNTU ? "NTU" : "TU2"));
# endif

			GLog.Print(mesh->Box);
			GLog.Print("\n");
#endif
			num++;

			if( InLoadingScreen )
			{
				GScript.DoString("MenuLocalLoadingScreenObjectsCounter = MenuLocalLoadingScreenObjectsCounter + 1" );
				GScript.DoString("Menu_RenderLoadingScreen(MenuLocalLoadingScreenObjectsCounter)" );
			}
//			GEngine->LoadingTick();
        }
        
        TRACE("World::LoadMeshPak(): processing of package '%s' complete\n", name);
        //Name = String::BaseName(name);
        return TRUE;
	}

    return FALSE;
#endif
}
// =======================================================================================
Entity* World::CreateEntity(int type, const char* fileName, const char* entityName, float scale, bool centerGeometry )
{
	Entity* entity = 0;
    int index = -1;
    
    if (scale == 0.0f)
    {
        GLog.Print("ERROR!!! Entity Scale 0.0f: %s\n",fileName?fileName:"NoName");
        scale = 1.0f;
    }

	#ifdef ENGINE_LOG
	GLog.Print("*CREATE_ENTITY: %d,%s,%s\n",type,fileName?fileName:"NoName",entityName?entityName:"NoName");
	#endif


    #ifdef USE_ENTITYCACHE
    entity = ECache_CreateEntity(type,fileName,entityName,scale,centerGeometry);
    #endif

    if (!entity)
	switch (type)
	{
	case Entity::ETypeNone:
		{
            m_setTag("Object");
			Entity* e = new Entity(Entity::ETypeNone);
			entity = e;
		}
		break;
	case Entity::ETypeLight:
		{
            if (!GEngine->Video) return NULL;

            m_setTag("Light");
			Light* light = new Light();
			index = SlotAdd(Lights,light);
			entity = light;
			entity->FileName = fileName?fileName:"";
			entity->Flags |= Entity::EEntDestroyOnReload;			
            entity->Name = entityName?entityName:"";
        }
		break;
	case Entity::ETypeRegion:
		{
            m_setTag("Region");
			WorldRegion* region = new WorldRegion();
			entity = region;
			entity->Flags |= Entity::EEntDestroyOnReload;			
		}
		break;
    case Entity::ETypeSound:
        {
            if (!GEngine->Audio) return NULL;
            m_setTag("Audio");
            Sound* sound = new Sound();
            entity = sound;
            entity->Flags |= Entity::EEntDestroyOnReload;			
            entity->FileName = fileName?fileName:""; // from save
            entity->Name = entityName?entityName:"";
        }
        break;
	case Entity::ETypeMesh:
		if (fileName && entityName)
		{
			ObjectPackage *pack = GEngine->PackManager->Get(fileName);
			pack->DeleteObjects = true;
			WorldMesh* instance = (WorldMesh*)   pack->GetInstance("WorldMesh",entityName);
			if (instance)
            {
                m_setTag("MeshWorldEntity");
                WorldMesh* mesh = new WorldMesh();
                *mesh = *instance;

				mesh->Flags |= (WorldMesh::EPhysics|Entity::EEntMovable);
				mesh->TransformToObjectSpace();
				mesh->SetIdentity();
				mesh->SetScale(scale);
				mesh->RecomputeBox();
#ifndef TRANSGAMING
                mesh->NormalMapName = ""; // poniewaz byl blad w exporterze dat'ow
#endif
                if (centerGeometry)
                    mesh->CenterGeometry();
                if (GEngine->Video)
                    mesh->RenderInitialize(GEngine->Video->RenderDev,GEngine->Video->MaterialSys,"Items/");
                else
                    mesh->SetupFlags();
				index = SlotAdd(Meshes,mesh);
				entity = mesh;
				entity->FileName = fileName;
				entity->Flags |= Entity::EEntDestroyOnReload;			
				//GLog.Print("CreateEntity(): Mesh \"%s\"\n",mesh->Name.AsChar());
            }
            else
                if (strlen(fileName) > 0) // czasami tworze ""
#ifdef TRANSGAMING
                    TRACE("[]::World::CreateEntity(): ERROR-> missing mesh! (filename = '%s')\n", fileName);
#else
                    MessageBox(NULL,fileName,"Missing mesh:",MB_TOPMOST);
#endif
		}
		break;
	case Entity::ETypeModel:
		if (fileName)
        {
            m_setTag("Model");
            Model *model = new Model();
            model->FileName = fileName;
            if (!model->Load(fileName, scale, AnimMeshManager)) 
            {
                delete model;
                model = NULL;
            }            
            if (model)
            {
				index = SlotAdd(Models,model);
				entity = model;
//				entity->FileName = fileName;	// done in CreateModel()
                entity->Name = entityName?entityName:"";
            }
            else
                if (strlen(fileName) > 0) // czasami tworze "" (demon_weapon)
#ifdef TRANSGAMING
                    TRACE("[]::World::CreateEntity(): ERROR-> missing model! (filename = '%s')\n", fileName);
#else
                    MessageBox(NULL,fileName,"Missing model:",MB_TOPMOST);
#endif
        }
		break;
	case Entity::ETypeParticleFx:
		{
            if (!GEngine->Video) return NULL;

            m_setTag("Particle");
            ParticleEffect* fx = new ParticleEffect();

            entity = fx;
            entity->Name = entityName?entityName:"";
			entity->FileName = fileName;
            entity->SetScale(scale);
			index = SlotAdd(ParticleEffects,fx);
			entity->Flags |= Entity::EEntDestroyOnReload;			
		}
		break;
	case Entity::ETypeBillboard:
		{
            if (!GEngine->Video) return NULL;
            m_setTag("Billboard");
			entity = new Billboard();
			entity->Flags |= Entity::EEntDestroyOnReload;
            entity->Name = entityName?entityName:"";
            entity->FileName = fileName?fileName:"";
		}
		break;
	case Entity::ETypeEnv:
		{
            if (!GEngine->Video) return NULL;
            m_setTag("Environment");
			Environment *env = new Environment();
			entity = env;
			entity->Flags |= Entity::EEntDestroyOnReload;
			index = SlotAdd(Environments,env);
            entity->FileName = fileName;
            entity->Name = entityName;
		}
		break;
	case Entity::ETypeDecal:
		{
            if (!GEngine->Video) return NULL;
#ifndef TRANSGAMING_HACK
			Decal* decal = GEngine->Video->DecalSys.CreateDecal(entityName, fileName);
			entity = decal;
			entity->Flags |= Entity::EEntDestroyOnReload;
            entity->FileName = fileName;
            entity->Name = entityName;
#endif
		}
		break;	
	case Entity::ETypeTrail:
		{
            if (!GEngine->Video) return NULL;
			Trail* trail = GEngine->Video->TrailSys.CreateTrail(entityName, fileName);
			entity = trail;
			entity->Flags |= Entity::EEntDestroyOnReload;
            entity->FileName = fileName;
            entity->Name = entityName;
		}
		break;
	}

	if (entity)
	{
        if (index!=-1){
			PtrMap[entity]=index;
#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
            if (PtrMap.Size() > mMaxPtrMapUsage){
                mMaxPtrMapUsage = PtrMap.Size();
                TRACE("@@@ World::CreateEntity(): peak PtrMap hash usage = %d\n", mMaxPtrMapUsage);
            }
#endif
        }

		if (type == Entity::ETypeEnv)
		{
			SortEnvironmentByNames();
		}

		if( scale != 1.0f )
			entity->SetScale(scale);

		if( !LoadInProgress )
		{
            TRACE("World::CreateEntity(): adding the entity '%s' from '%s' (type = %d)\n", entityName, fileName, type);
	        unsigned int idx = EntityGIndexes.Add(entity);
	        entity->GIndex = idx;
			if (entity->Name.AsChar())
				EntityHash[entity->Name] = idx;
		}
        else
            TRACE("World::CreateEntity(): load is in progress... not adding the entity '%s' from '%s' (type = %d)\n", entityName, fileName, type);
		#ifdef ENGINE_LOG
		GLog.Print("*CREATE_ENTITY_OK: %d\n",entity->GIndex);
		#endif
	}


	if( InLoadingScreen )
	{
		GScript.DoString( "MenuLocalLoadingScreenObjectsCounter = MenuLocalLoadingScreenObjectsCounter + 1" );
		GScript.DoString( "Menu_RenderLoadingScreen(MenuLocalLoadingScreenObjectsCounter)" );
	}

//	GEngine->LoadingTick();

	return entity;
}

void World::DeleteEntityDelayed( Entity* entity )
{
//	GLog.Print( "Frame %d: deleting delayed '%s' filename '%s', pointer %x\n", Frame, entity->Name.AsChar(), entity->FileName.AsChar(), (int)entity );
//	Assert( strstr( entity->Name.AsChar(), "_phys_pinned_" ) == 0 );

	mDelayedDeleteEntities.AddUnique( entity );
}

void World::DeleteDelayedEntities( void )
{
	for( int i = 0; i < mDelayedDeleteEntities.Size(); ++i )
	{
		Entity * e= mDelayedDeleteEntities[i];
		mDelayedDeleteEntities[i] = 0;
		DeleteEntity(e);
	}
	mDelayedDeleteEntities.ClearArray();

/*
	while(mDelayedDeleteEntities.Size())
	{
		Entity * e= mDelayedDeleteEntities[0];
		mDelayedDeleteEntities.Remove(0);
		DeleteEntity(e);
	}
*/
}

void World::DeleteEntity(Entity* entity )
{
	Assert(entity);

    if (mDelayedDeleteEntities.Find(entity) != -1)
        return;

#ifdef ENGINE_LOG
	//GLog.Print("*DELETE_ENTITY: %d,%s,%s,%d\n",entity->Type,entity->FileName.AsChar()?entity->FileName.AsChar():"NoName",entity->Name.AsChar()?entity->Name.AsChar():"NoName",entity->GIndex);
    TRACE("World::DeleteEntity(): deleting the entity '%s' (filename = '%s', type = %d, index = %d)\n", entity->Name.AsChar(), entity->FileName.AsChar(), entity->Type, entity->GIndex);
#endif

    EntityGIndexes[entity->GIndex] = NULL;
	RemoveEntity(entity);

//	GLog.Print( "Frame %d: deleted '%s' filename '%s', pointer %x\n", Frame, entity->Name.AsChar(), entity->FileName.AsChar(), (int)entity );

	bool deleted = false;
	#ifdef USE_ENTITYCACHE
		deleted = ECache_DeleteEntity(entity);
	#endif
    if (!deleted)
    {
        int* pindex = PtrMap.Find(entity);
        if (pindex)
        {
            int idx = *pindex;
            if (idx!=EInvalidIndex)
            {
                switch (entity->Type)
                {
                case Entity::ETypeMesh:
					Meshes[idx]=0;
					for( int i = 0; i < Volumes.Size(); ++i)
					{
						if (Volumes[i])
						for (int j=0; j<Volumes[i]->Meshes.Size(); ++j)
						{
							if (Volumes[i]->Meshes[j]==entity) 
							{
								Volumes[i]->Meshes.FastRemove(j);
								j--;
							}
						}
					}

					break;
                case Entity::ETypeVolume:		Volumes[idx]=0; break;
                case Entity::ETypeLight:		Lights[idx]=0; break;
                case Entity::ETypeModel:		Models[idx]=0; break;
			    case Entity::ETypeEnv:			Environments[idx]=0; break;
                case Entity::ETypeParticleFx:   ParticleEffects[idx]=0; break;
                }

                PtrMap[entity]=EInvalidIndex;
            }
        }

        if (GEngine->Video)
            entity->RenderCleanup(GEngine->Video->RenderDev,GEngine->Video->MaterialSys);
	    ///GLog.Print("DeleteEntity() \"%s\"\n",entity->Name.AsChar());
	    //GScript.DoString("Game:Print('delete c++')");
       
		if (entity->Type == Entity::ETypeEnv)
			UpdateAllEntities();
        delete entity;
    }
}

void World::MeshesTableToSlotAdd( WorldMesh* worldmesh )
{
	int* pindex = PtrMap.Find(worldmesh);
	if( pindex && ( *pindex != EInvalidIndex ) )
	{
		Meshes[*pindex]=0;
		PtrMap[worldmesh]=EInvalidIndex;
	}
	PtrMap[worldmesh] = SlotAdd( Meshes, worldmesh );
}


void World::AddEntity(Entity* entity)
{
    Assert(entity);

    if (entity->WorldLink != this)
    {
        ListInsert(EntityList,entity);
        entity->WorldLink = this;
    }

    if (entity->GIndex == 0) // for map entities don't created by CreateEntity
    {
        unsigned int idx = EntityGIndexes.Add(entity);
        entity->GIndex = idx;
		if (entity->Name.AsChar())
			EntityHash[entity->Name] = idx;
    }

	UpdateEntity(entity);
}

void World::RemoveEntity(Entity* entity)
{
    Assert(entity);

    if (entity->WorldLink == this)
    {
        ListRemove(entity);
        entity->WorldLink = 0;
    }

    for (int i=0; i<Zones.Size(); ++i)
		if( entity->ZoneLinks[i].ZoneLink == Zones[i] )
	        Zones[i]->RemoveEntity(entity);
}

void World::UpdateEntity(Entity* entity)
{
#ifndef TRANSGAMING_NO_DEDICATED_SERVER 
    if (GEngine->GameState == PCFSystem::DedicatedServer)
        return;
#endif

	if (entity->WorldLink)
	{
		int lastZoneIndex = -1;
	    for( int i = 0; i < Zones.Size(); ++i )
			if( entity->ZoneLinks[i].ZoneLink == Zones[i] )
			{
				Zones[i]->RemoveEntity(entity);
				lastZoneIndex = i;
			}

		if (!(entity->Flags & Entity::EEntNoDraw))
		{
			if (entity->Box.Valid())
			{
				if( ( lastZoneIndex != -1 ) && ( Zones[lastZoneIndex]->BoxTestExact( entity->Box ) == 2 ) )
					Zones[lastZoneIndex]->AddEntity(entity);
				else
				{
					bool intersectsAZone = false;
					for (int i=0; i<Zones.Size(); ++i)
					{
						Zone* zone = Zones[i];
						if( !intersectsAZone )
						{
							int result = zone->BoxTestExact( entity->Box );
						    if( result )
			                    zone->AddEntity(entity);
							if( result == 2 )
								break;
							else
								intersectsAZone = true;
						}
						else if( zone->BoxTest( entity->Box ) )
							zone->AddEntity(entity);
					}
				}

				if (entity->AsModel() || entity->AsMesh())
				{
					Environment* oldEnvLink = entity->Env.Link;
					Environment* newEnvLink = NULL;

					for (int i=0; i<Environments.Size(); ++i)
					{
						Environment* env = Environments[i];
						if (env && env->PointTest(entity->GetCenter()))
						{
							newEnvLink = env;
							break;
						}
					}

					if (newEnvLink != oldEnvLink)
					{
                        entity->Env.Link = NULL;
                        if (oldEnvLink)
                            oldEnvLink->RemoveEntity(entity);
                        if (newEnvLink)
                            newEnvLink->AddEntity(entity);
                    }

                    if (!entity->Env.Initiated)
                    {
                        entity->Env.Initiated = true;
                        entity->Env.LightProgress = 1;

						if (!entity->Env.OverwriteAmbient) {
							if (entity->Env.Link && (entity->Env.Link->Defines & Environment::EEnvAmbient))
								entity->Env.Ambient = entity->Env.Link->AmbientColor;	
							else
								entity->Env.Ambient = Env.AmbientColor;
						}

                        Light *dstLight = NULL;

                        if (entity->Env.Link && (entity->Env.Link->Defines & Environment::EEnvDirLight))
                            dstLight = &entity->Env.Link->DirLight;	
                        else
                            dstLight = &Env.DirLight;

                        if (dstLight)
                        {
                            if (!entity->Env.DirLight)
                            {
                                m_setTag("Light");
                                entity->Env.DirLight = new Light();
                                entity->Env.DirLight->SetType(Light::ELightDir);
                            }
                            entity->Env.DirLight->Color		= dstLight->Color;
                            entity->Env.DirLight->Intensity = dstLight->Intensity;
                            entity->Env.DirLight->SetDirection(dstLight->GetDirection());
                        }
                    }
                }
            }
        }
    }
}

void World::UpdateAllEntities()
{
    for (Entity* entity = EntityList; entity; entity=entity->Next)
	{
		entity->Env.Initiated = false;
		entity->UpdateTransform();

		/*
		if (entity->ReflectSurf & Entity::EReflectScene) {
			Assert(entity->AsMesh());
			GLog.Print("REFLECT: %s\n", entity->Name.AsChar());

            m_setTag("Portal");
			Portal* portal = new Portal();
			portal->BuildFromMesh(this, entity->AsMesh());

			for (int i=0; i<Mirrors.Size(); ++i) {
				//
			}

			Mirrors.Add(portal);
		}
		*/
    }
}

void World::BuildZones()
{
    for (int i=0; i<Portals.Size(); ++i)
    {
        Portals[i]->Zones.ClearArray();
        Portals[i]->ZoneRefs.ClearArray();   // TODO: compatibility
    }

    for (int i=0; i<Zones.Size(); ++i)
    {
        Zones[i]->Portals.ClearArray();
        Zones[i]->PortalRefs.ClearArray();   // TODO: compatibility
        Zones[i]->AntiPortals.ClearArray();
        Zones[i]->ClearEntityList();
    }

    /*
    GLog.Print(" ==================== ENTITY LIST =====================\n");
    for (Entity* entity=EntityList; entity; entity=entity->Next)
    GLog.Print("Entity: %s\n",entity->Name.AsChar());
    */

    // First, locate antiportals in zones
    for( int zoneIndex=0; zoneIndex<Zones.Size(); ++zoneIndex )
    {
        Zone* zone = Zones[zoneIndex];
        for( int apIndex = 0; apIndex < AntiPortals.Size(); ++apIndex )
        {
            if( zone->BoxTest( AntiPortals[apIndex]->Box ) )
                zone->AntiPortals.Add( AntiPortals[apIndex] );
        }
    }

    // remove fake portals
    for (int portalIndex=0; portalIndex<Portals.Size(); ++portalIndex) 
    {
        Portal* portal=Portals[portalIndex];
        const float portalThicknessTolerance=2.0f;

        int numZones = 0;
        // see which zones are connected
        for (int zoneIndex=0; zoneIndex<Zones.Size(); ++zoneIndex)
            if( Zones[zoneIndex]->PortalTest(*portal, portalThicknessTolerance ) )
                ++numZones;

        if (numZones<2) {
            GLog.Print("portal %s is fake, removing\n",portal->Name.AsChar());
			delete Portals[portalIndex];
            Portals.FastRemove(portalIndex);
        }		

    }

    // build a portal ref list for each zone
    for (int zoneIndex=0; zoneIndex<Zones.Size(); ++zoneIndex) 
    {
        Zone* zone=Zones[zoneIndex];

        // link portals
        for (int portalIndex=0; portalIndex<Portals.Size(); ++portalIndex) 
        {
            Portal* portal=Portals[portalIndex];

            const float portalThicknessTolerance=0.1f;
            if (zone->PortalTest( *portal, portalThicknessTolerance ) ) 
            {                
                portal->Zones.AddUnique(zone);
                zone->Portals.AddUnique(portal);
                portal->ZoneRefs.AddUnique(zoneIndex);
                zone->PortalRefs.AddUnique(portalIndex);
            }
        }

        for (Entity* entity=EntityList; entity; entity=entity->Next)
            if (entity->Box.Valid() && zone->BoxTest( entity->Box ) )
                zone->AddEntity(entity);
    }

    // report suspicious portals
    for( int portalIndex = 0; portalIndex < Portals.Size(); ++portalIndex ) 
    {
        Portal* portal=Portals[portalIndex];
        if( portal->Zones.Size() > 2 )
        {
            GLog.Print( "Portal %d ( name %s ) connects to more than 2 zones!!\n", portalIndex, portal->Name.AsChar() );
            for( int i = 0; i < portal->Zones.Size(); ++i )
                GLog.Print( "    Zone %d name: '%s'\n", portal->Zones[i]->Index, portal->Zones[i]->Name.AsChar() );
        }
    }

	// report portals not in main axes
    for( int portalIndex = 0; portalIndex < Portals.Size(); ++portalIndex ) 
    {
        Portal* portal=Portals[portalIndex];
		Vector normal = Planes[portal->PlaneIndex].Normal;
		int nonzero = 0;
		if( normal.X != 0.0f ) nonzero++;
		if( normal.Y != 0.0f ) nonzero++;
		if( normal.Z != 0.0f ) nonzero++;
		if( nonzero != 1 )
			GLog.Print( "Portal %d ( name %s ) is not in main world axis!!\n", portalIndex, portal->Name.AsChar() );
		else
		{
			// report portals not snapped to zone
			Plane plane(portal->Poly.Verts[0],portal->Poly.Verts[1],portal->Poly.Verts[2]);

			for( int zoneIndex = 0; zoneIndex < portal->Zones.Size(); ++zoneIndex )
				if( !portal->Zones[zoneIndex]->HasPlane( plane ) )
					GLog.Print( "Portal %d ( name %s ) is not in plane of zone %d (name %s )!\n",
						portalIndex, portal->Name.AsChar(), portal->Zones[zoneIndex]->Index, portal->Zones[zoneIndex]->Name.AsChar() );
		}
	}

    GLog.Flush();
}

void World::BuildVolumetrics()
{
    for (int iVol=0; iVol<Volumes.Size(); ++iVol)
    {
        Volume *fogVolume = Volumes[iVol];
		if (strstr(fogVolume->Name.AsChar(),"fast"))
			continue;

        GLog.Print("Adding static meshes to volumetric %s:", fogVolume->Name.AsChar());

        for (int iMesh=0; iMesh<Meshes.Size(); ++iMesh)
        {
            if (Meshes[iMesh]->Type != Entity::ETypeMesh)
                continue;

            if (fogVolume->Box.Intersect(Meshes[iMesh]->Box))
            {
                GLog.Print(" %s", Meshes[iMesh]->Name.AsChar());
                fogVolume->Meshes.Add(Meshes[iMesh]);
            }
        }

        GLog.Print("\n");
    }
}

void World::SortEnvironmentByNames()
{
	for (int i=0; i<Environments.Size()-1; i++)
	{
		for (int j=i+1; j<Environments.Size(); j++)
		{
			Environment *e0 = Environments[i];
			Environment *e1 = Environments[j];
			if (e0 && e1 && e0->Name.GreaterNoCase(e1->Name))
			{
				Environments[i] = e1;
				Environments[j] = e0;
				PtrMap[e0] = j;
				PtrMap[e1] = i;
			}
		}
	}
}


void World::BuildGlasses( void )
{
	float time = GEngine->SysDriver.GetCurrentTime();
	GLog.Print( "Precreating glasses (%d to make)\n", Glasses.Size() );
	for( int i = 0; i < Glasses.Size(); ++i )
		Glasses[i]->PreloadBreakup();
	GLog.Print( "Glasses precreated. Time spent: %f seconds\n", GEngine->SysDriver.GetCurrentTime()-time );
}

void World::Dump()
{
    // dump info
    for (int i=0; i<Zones.Size(); ++i)
    {
        int num;
        const Zone* zone=Zones[i];

        num = 0;
        for (Entity* entity = zone->EntityList; entity; entity=zone->NextEntity(entity))
            ++num;

        GLog.Print("Zone '%s': %5i entities\n",zone->Name.AsChar(),num);

        /*
        GLog.Print("Zone '%s' -> entity list:\n",zone->Name.AsChar());

        for (Entity* entity = zone->EntityList; entity; entity=zone->NextEntity(entity))
        GLog.Print("\t%s\n",entity->Name.AsChar());
        */
    }

    for (int i=0; i<Portals.Size(); ++i)
    {
        const Portal* portal=Portals[i];
        GLog.Print("Portal '%s': %5i zones\n",portal->Name.AsChar(),portal->Zones.Size());
        /*
        GLog.Print("Portal '%s' -> zone list:\n",portal->Name.AsChar());

        for (int j=0; j<portal->Zones.Size(); ++j)
        GLog.Print("\t%s\n",portal->Zones[j]->Name.AsChar());
        */
    }
}

static void CreateRenderTarget(Texture*& Tex, int Type, int Format, int SizeX, int SizeY)
{
    m_setTag("TextureRenderTarget");
	Tex = new Texture(Type);
	Tex->Format	= Format;
	Tex->Width	= SizeX;
	Tex->Height	= SizeY;

	GEngine->Video->RenderDev->CreateTexture(Tex);
	Assert(Tex->DeviceId != EInvalidIndex);
}

void World::Init(float scale)
{
	if( GEngine->Video )
	{
		if( GEngine->Video->PMenu->LoadScreen.Active() )
			GEngine->Video->PMenu->LoadScreen.SetOverall( GEngine->Video->PMenu->LoadScreen.GetOverall() + 10, 10 );
	}

	if (scale!=1.0f)
    {
        for (int i=0; i<Volumes.Size(); ++i)
            Volumes[i]->PreScale(scale);
        for (int i=0; i<Meshes.Size(); ++i)
            Meshes[i]->PreScale(scale);
        for (int i=0; i<Zones.Size(); ++i)
            Zones[i]->Scale(scale);
        for (int i=0; i<SwitchZones.Size(); ++i)
            SwitchZones[i]->Scale(scale);
        for (int i=0; i<DeathZones.Size(); ++i)
            DeathZones[i]->Scale(scale);
        for (int i=0; i<IceZones.Size(); ++i)
            IceZones[i]->Scale(scale);
        for (int i=0; i<LadderZones.Size(); ++i)
            LadderZones[i]->Scale(scale);
        for	(int i=0; i<AntiPortals.Size(); ++i)
            AntiPortals[i]->Scale(scale);
        for	(int i=0; i<Portals.Size(); ++i)
        {
            Portal* portal = Portals[i];
            portal->Scale(scale);

            portal->PlaneIndex=CreatePlane(portal->Poly.Verts[0],
                portal->Poly.Verts[1],
                portal->Poly.Verts[2]);

        }
        for	(int i=0; i<Mirrors.Size(); ++i)
        {
            Portal* portal = Mirrors[i];
            portal->Scale(scale);

            portal->PlaneIndex=CreatePlane(portal->Poly.Verts[0],
                portal->Poly.Verts[1],
                portal->Poly.Verts[2]);

        }

        Scale = scale;
    }

	GEngine->LoadingTick();

    BuildZones();

	GEngine->LoadingTick();

    BuildVolumetrics();

	GEngine->LoadingTick();

    String subPath;
    if (Name.Size())
        subPath = String("Levels/")+Name+"/";

	if (GEngine->Video)
    {
        //PSP & PS2 versions use TnL renderers only
#ifdef TRANSGAMING_NON_TNL
		if (GEngine->GetRenderDevice()->HWClass!=RenderDevice::EHWClassGenericTnL)
		{			
			RenderDevice* Device = GEngine->GetRenderDevice();

			if (Device->HWClass >= RenderDevice::EHWClassNV30 && DrawDynLights == 2)
				HighQualityLights = true;

#ifdef DA_CACHE
			if (TargetsReleased) 
#endif
			{
				UINT BufSizeX, BufSizeY;
				Device->GetBackBufferSize(BufSizeX, BufSizeY);
						
				if (BufSizeX > 1280) BufSizeX = 1280;
				if (BufSizeY > 1024) BufSizeY = 1024;

				BufSizeX = 1024;
				BufSizeY = 1024;

				GLog.Print("FB: back buffer size (%d %d)\n", BufSizeX,BufSizeY);

                m_setTag("TextureRenderTarget");
				TexFB[0] = new Texture(Texture::ETexRenderTarget);
				TexFB[0]->Width  = BufSizeX / 2;
				TexFB[0]->Height = BufSizeY / 2;

				GEngine->Video->RenderDev->CreateTexture(TexFB[0]);
				Assert(TexFB[0]->DeviceId != EInvalidIndex);
				if (TexFB[0]->DeviceId == EInvalidIndex)
					GLog.Print("FB: WARNING -- could not create RT (%d,%d)\n", TexFB[0]->Width, TexFB[0]->Height);

				// 2 FBs + Zbuf
				const int SX = BufSizeX / 2;
				const int SY = BufSizeY / 2;

				CreateRenderTarget(TexZBuf,	Texture::ETexDepthTarget, 0, SX, SY);
				if (TexZBuf->DeviceId == EInvalidIndex)
					GLog.Print("FB: WARNING -- could not create Depth Target (%d,%d)\n", TexZBuf->Width, TexZBuf->Height);

				for (int i=1; i<=2; ++i)
				{
                    m_setTag("TextureRenderTarget");
					TexFB[i] = new Texture(Texture::ETexRenderTarget);
					TexFB[i]->Width  = SX;
					TexFB[i]->Height = SY;

					GEngine->Video->RenderDev->CreateTexture(TexFB[i]);
					Assert(TexFB[i]->DeviceId != EInvalidIndex);
					if (TexFB[i]->DeviceId == EInvalidIndex)
						GLog.Print("FB: WARNING -- could not create RT (%d,%d)\n", TexFB[i]->Width, TexFB[i]->Height);
				}

	#ifdef ANNOYING_GRAPHICS_LOGS
				GLog.Print("FBTex: %dx%d (%dx%d) X 2\n", 
					TexFB[0]->Width,TexFB[0]->Height,
					TexFB[1]->Width,TexFB[1]->Height);
	#endif

				// ================================================
				//CreateRenderTarget(TexFrame[3],	Texture::ETexRenderTarget, 0, BufSizeX/4, BufSizeY/4);
				//CreateRenderTarget(TexFrame[3],	Texture::ETexRenderTarget,	Texture::ETexFmt_F32_ARGB, BufSizeX/4, BufSizeY/4);
				//CreateRenderTarget(TexFrame[3],	Texture::ETexDepthTarget, 0, 1024, 1024);
				// ================================================

				const int texSize = 512; 
				for (int i=0; i<TEX_MBLUR; ++i)
				{
                    m_setTag("TextureRenderTarget");
					TexSceneMotionBlur[i] = new Texture(Texture::ETexRenderTarget);
					if (TexSceneMotionBlur[i])
					{
						TexSceneMotionBlur[i]->Width  = texSize;
						TexSceneMotionBlur[i]->Height = texSize;
						GEngine->Video->RenderDev->CreateTexture(TexSceneMotionBlur[i]);
					}
				}

				GEngine->LoadingTick();

                m_setTag("TextureRenderTarget");
				TexSceneDepth = new Texture(Texture::ETexDepthTarget);
				if (TexSceneDepth)
				{
					TexSceneDepth->Width  = texSize;
					TexSceneDepth->Height = texSize;
					GEngine->Video->RenderDev->CreateTexture(TexSceneDepth);

				}

				GEngine->LoadingTick();

				if (RTCubeMap)
				{
					const int cubeSize = 512;
                    m_setTag("TextureCubeMap");
					TexSceneCubemap  = new Texture(Texture::ETexRenderTargetCubeMap);
					if (TexSceneCubemap)
					{
						TexSceneCubemap->Width  = cubeSize;
						TexSceneCubemap->Height = cubeSize;
						GEngine->Video->RenderDev->CreateTexture(TexSceneCubemap);
					}

					GEngine->LoadingTick();
				}			
			}

			AnimMeshManager.RenderInitialize(GEngine->Video->RenderDev,GEngine->Video->MaterialSys);
		}
#endif

		GEngine->LoadingTick();

	//	mSky.RenderInitialize(GEngine->Video->RenderDev,GEngine->Video->MaterialSys);
        for (int i=0; i<Meshes.Size(); ++i)
        {	
            Meshes[i]->RenderInitialize(GEngine->Video->RenderDev,GEngine->Video->MaterialSys,subPath.AsChar());
			if( i % 100 == 0 )
				GEngine->LoadingTick(0);
        }

        for (int i=0; i<Volumes.Size(); ++i)
        {	
            Volumes[i]->RenderInitialize(GEngine->Video->RenderDev,GEngine->Video->MaterialSys,subPath.AsChar());
        }

		GEngine->LoadingTick();
    }
    else
    {
        for (int i=0; i<Meshes.Size(); ++i)
        {	
            Meshes[i]->SetupFlags();
        }

    }

	GEngine->LoadingTick();

#ifndef TRANSGAMING_NO_HAVOK
    // init physics
    TRACE("World::Init(): initializing the physics system (Physics = %p, GEngine->Physics = %p)\n", Physics, GEngine->Physics);
	if( Physics )
	{
        TRACE("World::Init(): physicsFile = '%s'\n", PhysicsFile.AsChar());
        if( !PhysicsFile.IsNull() ){
            TRACE("World::Init(): loading physics data from HKE\n");
	        Physics->GetFromHKE( PhysicsFile.AsChar() );
            TRACE("World::Init(): done loading data from HKE\n");
        }
        else{
            TRACE("World::Init(): reloading the physics world\n");
			Physics->ReloadWorld();
            TRACE("World::Init(): done reloading the physics world\n");
        }

		GEngine->LoadingTick();
	}
#endif

#ifdef DA_CACHE
	if (GEngine->Video)
		GEngine->Video->MaterialSys->CleanupFrozenStuff();
#endif

    /*
#ifdef _DEBUG
    //char *buf;
    //buf = new char[10];
    //strcpy(buf,"012345678900123456789001234567890");

    if (!_CrtCheckMemory())
        MessageBox(NULL,"Corrupted integrity of memory blocks!!!","Engine Error",MB_TOPMOST);
#endif
        */

#ifdef USE_ENTITYCACHE
    gCacheEnabled = true;
#endif
}

int World::FindZone(const Vector& pos)
{
    // brute force
    for (int index=0; index<Zones.Size(); ++index) 
    {
        Zone* zone=Zones[index];
        if (zone->PointTest( pos ))
            return index;
    }

    return EInvalidIndex;
}

void World::Tick(float timeDelta)
{	
    ++Frame;
    for( int i = 0; i < Glasses.Size(); ++i )
        Glasses[i]->Tick( timeDelta );

	DynamicArray<Entity*> toRemove;

    for (Entity* entity=EntityList; entity; entity=entity->Next)
    {
		if( !(entity->Flags & Entity::EEntNoTick) )
			entity->Tick(timeDelta);		

        // obiekt przywiazany do jointa parenta nie posiadajacego PO musi samodzielnie zupdetowac swoja macierz transformacji
        // gdy parent posiada PO to dziecko transformowane jest automatycznie przy przepisywaniu pozycji fizycznej do entity parenta
        if (entity->Parent && !entity->Parent->PO && entity->Type != Entity::ETypeParticleFx && entity->TransformByParent && entity->ParentJoint>-1)
            entity->UpdateTransform();

        if (entity->TimeToDelete >= 0)
        {
            entity->TimeToDelete -= timeDelta;
            if (entity->TimeToDelete <= 0)
			{
				if( entity->Flags & Entity::EEntDontDeleteOnTimer )
				{
					entity->RemovePhysicsObject();
					entity->EnableDraw(false);
					entity->TimeToDelete = -1;
					toRemove.Add( entity );
				}
				else
	                DeleteEntityDelayed(entity);
			}
        }
    }

	for( int i = 0; i < toRemove.Size(); ++i )
		RemoveEntity( toRemove[i] );
	toRemove.ClearArray();

    // logic update -- not faster than 15fps
    bool update = false;
    static float ltick = 0.0f;
    if (ltick > 1.0f/15.f) 
    {
        ltick = fmod(ltick,1.0f/15.f);
        update = true;
    }
    ltick += timeDelta;

    if (update && DeathZones.Size()>0)
        for (Entity* entity=EntityList; entity; entity=entity->Next)
            if (entity->TestDeathZone)
            {				
                Vector v = entity->GetPosition();
                Model* mdl = entity->AsModel();
                if (mdl && mdl->IsRagdoll())
                {
                    int idx = mdl->GetJointIndex("root");
                    if (idx > -1)
                        v = mdl->GetJointTransform(idx).GetTranslation();
                }
#ifndef TRANSGAMING_NO_HAVOK
		else if( entity->PO )
			v = entity->PO->GetPosition();
#endif
                const char * zone = InDeathZone(Vector(v.X,v.Y-1,v.Z));
                if (zone)
                {
                    GScript.QueueMessage("IN_DEATH_ZONE","%e,%f,%f,%f,%s",entity,v.X,v.Y,v.Z,zone);
                    entity->TestDeathZone = false;
                }
            };

    mSky.Tick(timeDelta);
}

#ifdef PKTV_ENABLED
void World::PKTVDemoTick(float timeDelta)
{	
    ++Frame;
//	for( int i = 0; i < Glasses.Size(); ++i )
//		Glasses[i]->Tick( timeDelta );

    for (Entity* entity=EntityList; entity; entity=entity->Next)
    {
		if( !(entity->Flags & Entity::EEntNoTick) )
			entity->Tick(timeDelta);		

		if( entity->Parent &&
			!entity->Parent->PO &&
			entity->Type != Entity::ETypeParticleFx &&
			entity->TransformByParent &&
			entity->ParentJoint > -1 )
            entity->UpdateTransform();
    }

    mSky.Tick(timeDelta);
}
#endif

void World::DeleteDyingEntities()
{
    for (Entity* entity=EntityList; entity; entity=entity->Next)
    {
        if (entity->TimeToDelete >= 0 && !(entity->Flags & Entity::EEntDontDeleteOnTimer))
            DeleteEntityDelayed(entity);
    }
}

Entity*	World::PickEntity(Viewport& vp, int ptx, int pty, BOOL UseHavok)
{
    float x=float(ptx);
    float y=float(pty);

    // Compute the vector of the pick ray in screen space
    Vector v;
    v.X =  (((2.0f * x) / vp.Width ) - 1) / vp.CameraToScreen.A00;
    v.Y = -(((2.0f * y) / vp.Height) - 1) / vp.CameraToScreen.A11;
    v.Z =  1.0f;

	Entity* OldPicked = PickedEntity;

	PickedEntity = NULL;

#ifndef TRANSGAMING_NO_HAVOK
	if (UseHavok)
	{
		const Matrix ScreenToWorld = vp.WorldToCamera.Inverse();
		const Vector Dir = ScreenToWorld.TransformNormal(v).GetNormal();

		HitData hitData;
#ifdef	TRANSGAMING
		if (GEngine->pcfWorld->Physics->LineTrace(vp.EyePosition, vp.EyePosition + Dir*1024.0F, hitData))
#else
		if (GEngine->World->Physics->LineTrace(vp.EyePosition, vp.EyePosition + Dir*1024.0F, hitData))
#endif		
		{
			Entity* E = (Entity*) hitData.UserData;
			GLog.Print("PICK: %s\n", E->Name.AsChar());
			
			//PickedEntity = E;
			PickedEntity = (OldPicked == E) ? NULL : E;
		}

		return PickedEntity;
	}
#endif // TRANSGAMING_NO_HAVOK

	// old method
    Vector hitPoint;
    for (Entity* entity=EntityList; entity; entity=entity->Next)
    {		
        if( !(entity->Type == Entity::ETypeModel && entity->FileName.Size() && entity->FileName == "player_box") &&
            entity->Type != Entity::ETypeEnv &&
			entity->Type != Entity::ETypeDecal &&
			entity->Type != Entity::ETypeLight &&
            entity->Type != Entity::ETypeSound &&
            entity->Type != Entity::ETypeParticleFx &&
            entity->Type != Entity::ETypeRegion &&
            entity->Type != Entity::ETypeTrail &&
            entity->Type != Entity::ETypeVolume &&
            entity->Type != Entity::ETypeBillboard &&
			!(entity->AsMesh() && (entity->AsMesh()->FixedMeshIndex !=	65535)))
			//(etype < 0 || entity->Type == etype))
        {
            RaySegment ray;
            Matrix CameraToObject = (entity->ObjectToWorld * vp.WorldToCamera).Inverse();
            ray.Dir    = CameraToObject.TransformNormal(v);
            ray.Origin = CameraToObject.GetTranslation();

            if (ray.Intersect(entity->LocalBox)) 
                return entity;
        }
    }

    return NULL; 	
}

void World::TestSwitchZones( const Vector& pt )
{
	int     newZoneIndex = -1;
    char    buf[MAX_PATH];
    char *  zoneString;


	if( !SwitchZonesOff )
	{
	    for( int i=0; i<SwitchZones.Size(); ++i )
	        if( SwitchZones[i]->PointTest( pt ) )
			{
	            newZoneIndex = i;
				break;
			}
	}

	if( newZoneIndex == LastSwitchZoneIndex ) return;

	if( LastSwitchZoneIndex != -1 )
	{
		zoneString = SwitchZones[LastSwitchZoneIndex]->GetSwitchZoneName(buf, MAX_PATH);
	    for( int i=0; i<AntiPortals.Size(); ++i )
	        if( strstr( AntiPortals[i]->Name.AsChar(), zoneString ) &&
				strstr( AntiPortals[i]->Name.AsChar(), "antypswitch" ) )
					AntiPortals[i]->Disabled = true;
	}

	if( newZoneIndex != -1 )
	{
		zoneString = SwitchZones[newZoneIndex]->GetSwitchZoneName(buf, MAX_PATH);
	    for( int i=0; i<AntiPortals.Size(); ++i )
	        if( strstr( AntiPortals[i]->Name.AsChar(), zoneString ) &&
				strstr( AntiPortals[i]->Name.AsChar(), "antypswitch" ) )
					AntiPortals[i]->Disabled = false;
		LastSwitchZoneName = zoneString;
	}
	else
		LastSwitchZoneName = "(none)";

	LastSwitchZoneIndex = newZoneIndex;
}

const char *World::InDeathZone(const BoundingBox& box) const
{
    for (int i=0; i<DeathZones.Size(); ++i) {
        if( DeathZones[i]->Enabled && DeathZones[i]->BoxTest( box ) )
            return DeathZones[i]->Name.AsChar();
    }

    return NULL;
}

const char *World::InDeathZone(const Vector& pt) const
{
    for( int i=0; i<DeathZones.Size(); ++i ) {
        if( DeathZones[i]->Enabled && DeathZones[i]->PointTest( pt ) )
            return DeathZones[i]->Name.AsChar();
    }

    return NULL;
}

bool World::NearLadder(const BoundingBox& box) const
{
    for( int i = 0; i < LadderZones.Size(); ++i )
        if( LadderZones[i]->BoxTest( box ) )
            return true;
    return false;
}

bool World::NearLadder(const Vector& pt) const
{
    for( int i = 0; i < LadderZones.Size(); ++i )
    {
        if( LadderZones[i]->PointTest( pt ) )
            return true;
    }

    return false;
}

bool World::OnIce(const BoundingBox& box) const
{
    for( int i = 0; i < IceZones.Size(); ++i )
        if( IceZones[i]->BoxTest( box ) )
            return true;
    return false;
}

bool World::OnIce(const Vector& pt) const
{
    for( int i = 0; i < IceZones.Size(); ++i )
    {
        if( IceZones[i]->PointTest( pt ) )
            return true;
    }

    return false;
}


int World::ExplodeItem( Entity* item, const char* fileName, float strength, float radius, float lifetimeAfterExplosion, float ascale, bool nonCollidable, const char* meshName , Vector offset)
{
    Assert( item );
    Assert( item->PO );

#ifndef TRANSGAMING_NO_HAVOK
    float scale = item->GetScale() * ascale;
    Vector velocity = item->PO->GetVel();
    Vector angularVelocity = item->PO->GetAngularVel();

    float massToDynamic = item->PO->GetMass()/item->PO->GetVolumeMass();

    ObjectPackage *pack = GEngine->PackManager->Get(fileName);
    DynamicArray<PhysicsObject*> physicsObjects;
	int destroyedItem = 0;
 
    LastExplodedEntities.ClearArray();
    for( int i = 0; i < pack->GetInstanceNum(); ++i )
    {
        // Find out entity name
        String objectName = pack->GetObjectName( i );

        // Create entity
        WorldMesh* instance = (WorldMesh*)CreateEntity( Entity::ETypeMesh, fileName, objectName.AsChar(), scale );

        Assert( instance );

//		instance->Box.Reset();
//		BoundingBox b = instance->LocalBox;
//		instance->LocalBox.Reset();

        instance->Flags |= Entity::EEntDestroyOnReload;

        // Position entity
		instance->SetPositionAndRotation( item->GetPosition(), item->GetRotation() );

        // Add entity to world
        AddEntity( instance );

        // Create physics object
		int collGroup = -1;

		if( nonCollidable )
			collGroup = HCGNormalNCWithSelf;

		instance->CreatePhysicsObject( EPawnFromMeshNonCentered, scale, collGroup );
        if( !instance->PO ||
            lifetimeAfterExplosion == 0 ) // for level caching
        {
            DeleteEntity( instance );
            continue;
        }

//		instance->LocalBox = b;

		instance->PO->SetMass( instance->PO->GetVolumeMass()*massToDynamic );
        instance->PO->Activate(true);
        instance->PO->SetVel( velocity );
        instance->PO->SetAngularVel( angularVelocity );
        instance->PO->SetFriction(1.0f);

		// Add physicsObjects to the list
		physicsObjects.Add( instance->PO );

		instance->PO->SetHardDeactivator();

		// autodelete after 10-20 sec.
		///instance->TimeToDelete = 10 + rand()%10;
		if( strstr( objectName.AsChar(),"pinned" ) )
			instance->PO->SetPinned( true );
		else
			instance->TimeToDelete  =  lifetimeAfterExplosion;

//		instance->RecomputeBox();
//		instance->LocalBox = instance->Box;
//		instance->Box = instance->LocalBox;

		if (meshName && strcmp(meshName, objectName.AsChar()) == 0)
			destroyedItem = instance->GIndex;
        
        LastExplodedEntities.Add(instance);
	}

	// Give impulse to physics objects
#ifdef TRANSGAMING
    TRACE("World::ExplodeItem(): adding impulse (physics = %p)\n", GEngine->pcfWorld->Physics);
	Assert( GEngine->pcfWorld->Physics );
#else
	Assert( GEngine->World->Physics );
#endif
	Vector pos = item->PO?item->PO->GetCenterOfMass():item->GetPosition();
	pos += offset;
#ifdef	TRANSGAMING
//	GEngine->pcfWorld->Physics->SelfExplosion( pos, strength, radius, &physicsObjects );
#else
	GEngine->World->Physics->SelfExplosion( pos, strength, radius, &physicsObjects );
#endif
	return destroyedItem;

#else // TRANSGAMING_NO_HAVOK
	return 0;
#endif //TRANSGAMING_NO_HAVOK
}

void World::EnablePortal( const char* name, bool enable )
{
	for (int i=0; i<Portals.Size(); ++i)
		if( Portals[i]->Name == name )
			Portals[i]->IsDisabled = !enable;
}

bool World::IsPortalEnabled( const char* name )
{
	for (int i=0; i<Portals.Size(); ++i)
		if( Portals[i]->Name == name )
			return !Portals[i]->IsDisabled;
	return true;
}

void World::EnableAntiPortal( const char* name, bool enable )
{
	for (int i=0; i<AntiPortals.Size(); ++i)
		if( AntiPortals[i]->Name == name )
			AntiPortals[i]->Disabled = !enable;
}

void World::DeleteAntiPortal( const char* name)
{
	int zoneIndex=0;
    for (int i=0; i<AntiPortals.Size(); ++i)
        if( AntiPortals[i]->Name == name )
        {
#ifdef	TRANSGAMING
			for( zoneIndex=0; zoneIndex<GEngine->pcfWorld->Zones.Size(); ++zoneIndex )
			{
                Zone* zone = GEngine->pcfWorld->Zones[zoneIndex];
                int index = zone->AntiPortals.Find( AntiPortals[i] );
                if( index != -1 )
                    zone->AntiPortals.FastRemove( index );
            }
#else
			for( zoneIndex=0; zoneIndex<GEngine->World->Zones.Size(); ++zoneIndex )
			{
                Zone* zone = GEngine->World->Zones[zoneIndex];
                int index = zone->AntiPortals.Find( AntiPortals[i] );
                if( index != -1 )
                    zone->AntiPortals.FastRemove( index );
            }
#endif

            delete AntiPortals[i];
            AntiPortals.FastRemove(i);
            break;
        }
}

bool World::IsAntiPortalEnabled( const char* name )
{
	for (int i=0; i<AntiPortals.Size(); ++i)
		if( AntiPortals[i]->Name == name )
			return !AntiPortals[i]->Disabled;
	return true;
}

WorldMesh* World::DamageItem( WorldMesh* item, const char* fileName, const char* meshName )
{
	Assert( item );
	Assert( item->PO );

	float scale = item->GetScale();
#ifndef TRANSGAMING_NO_HAVOK
	Vector velocity = item->PO->GetVel();
	Vector angularVelocity = item->PO->GetAngularVel();

	WorldMesh* instance = (WorldMesh*)CreateEntity( Entity::ETypeMesh, fileName, meshName, scale );
	if( !instance )
		return 0;

	instance->LocalBox.Reset();
	instance->Flags |= Entity::EEntDestroyOnReload;

	// Position entity
	instance->SetPositionAndRotation( item->GetPosition(), item->GetRotation() );

	// Remove old entity from world
	DeleteEntityDelayed( item );
	item->PO->Enable(false);

	// Add entity to world
	AddEntity( instance );

	// Create physics object
	instance->CreatePhysicsObject( EPawnFromMeshConvex, scale );

	instance->PO->Activate(true);
	instance->PO->SetVel( velocity );
	instance->PO->SetAngularVel( angularVelocity );

	instance->PO->SetHardDeactivator();

	instance->RecomputeBox();
	instance->LocalBox = instance->Box;
#else
	WorldMesh* instance = (WorldMesh*)CreateEntity( Entity::ETypeMesh, fileName, meshName, scale );
	if( !instance )
		return 0;
#endif
	return instance;
}

Model* World::GibModel( Model* model, const char* gibFileName, float strength, float radius, Entity* owner, int weapon_type, int collisionGroup, const char* getVelFromJoint )
{
	Assert( model );
//	if (!(model->PO)) {
//		return NULL;
//	}

	float scale = model->Mesh->Scale;

	Vector velocity(0,0,0);
	Vector angularVelocity(0,0,0);
#ifndef TRANSGAMING_NO_HAVOK
	if( model->PO )
	{
		velocity = model->PO->GetVel();
		angularVelocity = model->PO->GetAngularVel();
	}
	else {
			if (getVelFromJoint && model->RagdollPtr && model->RagdollPtr->IsActive()) {
				int joint = model->GetJointIndex(getVelFromJoint);
				if (joint >= 0) {
					velocity = model->RagdollPtr->Joint_GetLinearVelocity(joint);
					angularVelocity = model->RagdollPtr->Joint_GetAngularVelocity(joint);
				}
			}
	}
#endif
	Vector position = model->GetPosition();
	Quaternion rotation = model->GetRotation();
#ifndef TRANSGAMING_NO_HAVOK
	if( model->PO )
		model->PO->Enable( false );
#endif
	// Create gib
	Model* gib = (Model*)CreateEntity( Entity::ETypeModel, gibFileName, "", scale );
	if( gib )
	{
		// Position gib
		gib->SetPositionAndRotation( position, rotation );
#ifdef	TRANSGAMING
		GEngine->pcfWorld->AddEntity( gib );
#else
		GEngine->World->AddEntity( gib );
#endif
		model->SetupGib( gib );

		// Add gib to world
		gib->RagDoll = true;
#ifndef TRANSGAMING_NO_HAVOK
		if( gib->RagdollPtr )
		{
			gib->RagdollPtr->Activate( gib->FrameMatrixArr, collisionGroup );
			gib->ActivateAnimation(0); // ragdoll
			const float F=42.f;
			gib->LocalBox.Reset();
			gib->LocalBox += Vector(+F,+F,+F);
			gib->LocalBox += Vector(-F,-F,-F);
			gib->UpdateTransform();
	
			gib->RagdollPtr->SetVelocities( velocity, angularVelocity );
		}
		else
		{
#ifdef	TRANSGAMING
			GEngine->pcfWorld->DeleteEntityDelayed( gib );
#else
			GEngine->World->DeleteEntityDelayed( gib );
#endif
			gib = 0;
		}
#else // TRANSGAMING_NO_HAVOK
		GEngine->pcfWorld->DeleteEntityDelayed( gib );
		gib = 0;
#endif // TRANSGAMING_NO_HAVOK
	}

	return gib;
}


int World::GetEntities(int type, const BoundingBox& box, Entity** entityArr, int elems)
{
	int entitiesNum = 0;
	int ent=0;
	
	// go through zones
	for( int i = 0; i < Zones.Size(); ++i ) 
	{
		Zone *zone = Zones[i];

		if (!zone->BoxTestExact(box))
			continue;

		// go through entities in zone
		for (Entity* entity = zone->EntityList; entity; entity=zone->NextEntity(entity))
		{
			if (entity->Type != type)	// invalid type
				continue;

			if (!box.Intersect(entity->Box)) // box test
				continue;

			for (ent=0; ent<entitiesNum; ++ent) // maybe we already got one
			{
				if (entityArr[ent] == entity)
					break;
			}

			if (ent>=entitiesNum)
			{
				entityArr[entitiesNum++] = entity; // finaly add her

				if (entitiesNum>=elems)	// check for array bounds
					return entitiesNum;	
			}
		}
	}

	return entitiesNum;
}

void World::ComposeTextures(Texture* targetTex, Texture** sourceTex, float* sourceArg, int sourceNum) const
{
	const int MAX_TEX = 4;
	if (sourceNum<1)
		return;
	else if (sourceNum>MAX_TEX)
		sourceNum=MAX_TEX;

	MaterialSystem	*matSys = GEngine->Video->MaterialSys;
	RenderDevice	*dev = GEngine->Video->RenderDev;
	Texture			*notex = matSys->Textures.NoTex;
	Material		*ComposeShader;
	if (sourceNum>2)
		ComposeShader = matSys->CreateMaterial("compose4");
	else
		ComposeShader = matSys->CreateMaterial("compose2");
	const float texW = targetTex->Width;
	const float texH = targetTex->Height;

	DWORD col = 0xFFFFFFFF;
	const float w = targetTex->Width;
#ifdef TRANSGAMING
    Plane tmpPlane;
    Plane nullPlane(0, 0, 0, 0);
#endif

	int unlock = GEngine->Video->Hud->BuildQuad(	-1.0f/w-1.0f,1.0f/w-1.0f,0,col,0,1,
													-1.0f/w-1.0f,1.0f/w+1.0f,0,col,0,0,
													-1.0f/w+1.0f,1.0f/w-1.0f,0,col,1,1,
													-1.0f/w+1.0f,1.0f/w+1.0f,0,col,1,0,
													-1.0f/w+1.0f,1.0f/w-1.0f,0,col,1,1,
													-1.0f/w-1.0f,1.0f/w+1.0f,0,col,0,0);
	GEngine->Video->Hud->SetStream();

	for (int tex=0; tex<MAX_TEX; tex++)
	{
		float alpha = (sourceNum>tex)?sourceArg[tex]:0.0f;

#ifdef TRANSGAMING
        tmpPlane.set(alpha,alpha,alpha,alpha);
		dev->SetPixelShaderMemory(tex,&tmpPlane,1);
		dev->SetShaderMemory(10+tex,&nullPlane,1);
#else
		dev->SetPixelShaderMemory(tex,&Plane(alpha,alpha,alpha,alpha),1);
		dev->SetShaderMemory(10+tex,&Plane(0,0,0,0),1);
#endif
	}

	// compose
	dev->SetRenderTarget(targetTex, NULL);
	dev->SetViewport(0,0,(int)texW,(int)texH);
	dev->ClearFB(0xFF000000);

	for (int iPass=0; iPass<ComposeShader->PassCount; ++iPass)
	{
		RenderPass& pass = ComposeShader->Passes[iPass];	

		for (int tex=0; tex<MAX_TEX; tex++)
		{
			if (tex<sourceNum)
				pass.TexMaps[tex].TexPtr = sourceTex[tex];
			else 
				pass.TexMaps[tex].TexPtr = notex;
		}
		pass.Activate();

#ifdef TRANSGAMING
        MaterialInput tmpMaterial(notex, notex, notex);

		pass.SetInput(&tmpMaterial);
#else
		pass.SetInput(&MaterialInput(notex, notex, notex));
#endif
		dev->Draw(RenderDevice::EPrimTriList,unlock,2);
	}

	dev->SetFBRenderTarget();
}

void World::BlurTexture(Texture* texture, Texture* textureFilter, float size, bool color) const 
{
	MaterialSystem *matSys = GEngine->Video->MaterialSys;
	RenderDevice *dev = GEngine->Video->RenderDev;

	Texture* notex = matSys->Textures.NoTex;
	Material *BlurShader = matSys->CreateMaterial(color ? "blur_color" : "blur");

	DWORD col = 0xFFFFFFFF;
	const float w = texture->Width/(size>F_EPS ? size : 1);
#ifdef TRANSGAMING
    Plane tmpPlane;
#endif

	int unlock = GEngine->Video->Hud->BuildQuad(	-1.0f/w-1.0f,-1.0f/w+1.0f,0,col,0,1,
													-1.0f/w-1.0f,-1.0f/w-1.0f,0,col,0,0,
													-1.0f/w+1.0f,-1.0f/w+1.0f,0,col,1,1,
													-1.0f/w+1.0f,-1.0f/w-1.0f,0,col,1,0,
													-1.0f/w+1.0f,-1.0f/w+1.0f,0,col,1,1,
													-1.0f/w-1.0f,-1.0f/w-1.0f,0,col,0,0);
	GEngine->Video->Hud->SetStream();

#ifdef TRANSGAMING
    tmpPlane.set(0.125f,0.125f,0.125f,0.125f);
	dev->SetPixelShaderMemory(0,&tmpPlane,1);
    tmpPlane.set(0.375f,0.375f,0.375f,0.375f);
	dev->SetPixelShaderMemory(1,&tmpPlane,1);
    tmpPlane.set(0.375f,0.375f,0.375f,0.375f);
	dev->SetPixelShaderMemory(2,&tmpPlane,1);
    tmpPlane.set(0.125f,0.125f,0.125f,0.125f);
	dev->SetPixelShaderMemory(3,&tmpPlane,1);
#else
	dev->SetPixelShaderMemory(0,&Plane(0.125f,0.125f,0.125f,0.125f),1);
	dev->SetPixelShaderMemory(1,&Plane(0.375f,0.375f,0.375f,0.375f),1);
	dev->SetPixelShaderMemory(2,&Plane(0.375f,0.375f,0.375f,0.375f),1);
	dev->SetPixelShaderMemory(3,&Plane(0.125f,0.125f,0.125f,0.125f),1);
#endif

	// blur horizontaly
	dev->SetRenderTarget(textureFilter, NULL);
	dev->ClearFB(0xFF000000);
	dev->SetViewport(0,0,texture->Width,texture->Height);

#ifdef TRANSGAMING
    tmpPlane.set(-1.5f/w,0,0,0);
	dev->SetShaderMemory(10,&tmpPlane,1);
    tmpPlane.set(-0.5f/w,0,0,0);
	dev->SetShaderMemory(11,&tmpPlane,1);
    tmpPlane.set( 0.5f/w,0,0,0);
	dev->SetShaderMemory(12,&tmpPlane,1);
    tmpPlane.set( 1.5f/w,0,0,0);
	dev->SetShaderMemory(13,&tmpPlane,1);
#else
	dev->SetShaderMemory(10,&Plane(-1.5f/w,0,0,0),1);
	dev->SetShaderMemory(11,&Plane(-0.5f/w,0,0,0),1);
	dev->SetShaderMemory(12,&Plane( 0.5f/w,0,0,0),1);
	dev->SetShaderMemory(13,&Plane( 1.5f/w,0,0,0),1);
#endif

	for (int iPass=0; iPass<BlurShader->PassCount; ++iPass)
	{
		RenderPass& pass = BlurShader->Passes[iPass];	
		pass.Activate();

#ifdef TRANSGAMING
        MaterialInput tmpMaterial(texture, notex, notex);

		pass.SetInput(&tmpMaterial);
#else
		pass.SetInput(&MaterialInput(texture, notex, notex));
#endif
		dev->Draw(RenderDevice::EPrimTriList,unlock,2);
	}

	// blur verticaly
	dev->SetRenderTarget(texture, NULL);
	dev->SetViewport(0,0,texture->Width,texture->Height);

#ifdef TRANSGAMING
    tmpPlane.set(0,-1.5f/w,0,0);
	dev->SetShaderMemory(10,&tmpPlane,1);
    tmpPlane.set(0,-0.5f/w,0,0);
	dev->SetShaderMemory(11,&tmpPlane,1);
    tmpPlane.set(0, 0.5f/w,0,0);
	dev->SetShaderMemory(12,&tmpPlane,1);
    tmpPlane.set(0, 1.5f/w,0,0);
	dev->SetShaderMemory(13,&tmpPlane,1);
#else
	dev->SetShaderMemory(10,&Plane(0,-1.5f/w,0,0),1);
	dev->SetShaderMemory(11,&Plane(0,-0.5f/w,0,0),1);
	dev->SetShaderMemory(12,&Plane(0, 0.5f/w,0,0),1);
	dev->SetShaderMemory(13,&Plane(0, 1.5f/w,0,0),1);
#endif

	for (int iPass=0; iPass<BlurShader->PassCount; ++iPass)
	{
		RenderPass& pass = BlurShader->Passes[iPass];	
		pass.Activate();

#ifdef TRANSGAMING
        MaterialInput tmpMaterial(textureFilter, notex, notex);

		pass.SetInput(&tmpMaterial);
#else
		pass.SetInput(&MaterialInput(textureFilter, notex, notex));
#endif
		dev->Draw(RenderDevice::EPrimTriList,unlock,2);
	}
	dev->SetFBRenderTarget();
}

void World::DrawTexture(Texture* texture, Material* material) const
{
	MaterialSystem	*matSys = GEngine->Video->MaterialSys;
	RenderDevice	*dev = GEngine->Video->RenderDev;
	Texture			*notex = matSys->Textures.NoTex;
	DWORD col = 0xFFFFFFFF;
	const float w = texture->Width;

	int unlock = GEngine->Video->Hud->BuildQuad(	-1.0f/w-1.0f,1.0f/w-1.0f,0.0f, 0.0f,1.0f, col,
													-1.0f/w-1.0f,1.0f/w+1.0f,0.0f, 0.0f,0.0f, col,
													-1.0f/w+1.0f,1.0f/w-1.0f,0.0f, 1.0f,1.0f, col,
													-1.0f/w+1.0f,1.0f/w+1.0f,0.0f, 1.0f,0.0f, col,
													-1.0f/w+1.0f,1.0f/w-1.0f,0.0f, 1.0f,1.0f, col,
													-1.0f/w-1.0f,1.0f/w+1.0f,0.0f, 0.0f,0.0f, col);
	GEngine->Video->Hud->SetStream();

	for (int iPass=0; iPass<material->PassCount; ++iPass)
	{
		RenderPass& pass = material->Passes[iPass];	

		pass.Activate();

#ifdef TRANSGAMING
        MaterialInput tmpMaterial(texture, notex, notex);

		pass.SetInput(&tmpMaterial);
#else
		pass.SetInput(&MaterialInput(texture, notex, notex));
#endif
		dev->Draw(RenderDevice::EPrimTriList,unlock,2);
	}
}

#ifndef TRANSGAMING
void World::MeshesSetDefaultDetailMaps(const char *filename, float tileU, float tileV)
{
	for (int i=0; i<Meshes.Size(); ++i) {
		if (Meshes[i]) {
			Meshes[i]->DetailMap.Tile.U = tileU;
			Meshes[i]->DetailMap.Tile.V = tileV;

			if (stricmp(Meshes[i]->DetailMap.Name.AsChar(), filename)) {
				Meshes[i]->SetTextureName(SimpleMesh::ETexDetailMap, filename);
				Meshes[i]->ReloadTextures();
			}
		}
	}
}
#endif

void World::MeshesSetDefaultCubeMaps(const char *filename)
{
	if (!GEngine->Video)
		return;

	RenderDevice *dev = GEngine->GetRenderDevice();

	if (RTCubeMap && dev->HWClass == RenderDevice::EHWClassGenericTnL)
	{
		MaterialSystem *matSys = GEngine->Video->MaterialSys;
		matSys->ReleaseTexture(TexStaticCubemap);
		TexStaticCubemap = NULL;
		if (filename && filename[0])
			TexStaticCubemap = matSys->CreateCubeMap(filename);
		return;
	}

	for (int i=0; i<Meshes.Size(); ++i) 
	{
		if (Meshes[i]) 
		{
			Meshes[i]->SetTextureName(SimpleMesh::ETexCubeMap, filename);
			Meshes[i]->ReloadTextures();
		}
	}
}

void World::MeshesSetDefaultNormalMaps(const char *filename)
{
	for (int i=0; i<Meshes.Size(); ++i) {
		if (Meshes[i]) {
			Meshes[i]->SetTextureName(SimpleMesh::ETexNormalMap, filename);
			Meshes[i]->ReloadTextures();
		}
	}
}

void World::MeshesActiveGroupEnableDraw(BYTE group,bool enable)
{
    for (int i=0; i<Meshes.Size(); ++i) 
        if (Meshes[i] && Meshes[i]->ActiveGroup == group)
        {
            Meshes[i]->EnableDraw(enable);
        }
}


void World::MeshesActiveGroupSetTimeToRemove( BYTE group, float time, float randomizer )
{
#ifndef TRANSGAMING_NO_HAVOK
	// Inactive
	Physics->ActiveMeshGroupSetTimeToRemove( group, time, randomizer );
#endif

	// Active; "autodelete"
	for( Entity* entity=EntityList; entity; entity=entity->Next )
	{
		if( entity->Type == Entity::ETypeMesh )
		{
			WorldMesh* mesh = (WorldMesh*)entity;
#ifndef TRANSGAMING_NO_HAVOK
			if( mesh->ActiveGroup == group && mesh->PO && mesh->PO->IsEnabled() )
			{
#endif
	            float r = ((float)rand()/(float)RAND_MAX) * randomizer;
	            if( rand()%2 == 0 ) r = -r;
				mesh->TimeToDelete = time + r;
				mesh->Flags |= Entity::EEntDontDeleteOnTimer;
#ifndef TRANSGAMING_NO_HAVOK
			}
#endif
		}
	}

	// Active; not "autodelete"
	for( int i = 0; i < Meshes.Size(); ++i )
#ifdef TRANSGAMING_NO_HAVOK
	{
#else
		if( Meshes[i] && ( Meshes[i]->ActiveGroup == group ) && Meshes[i]->PO && Meshes[i]->PO->IsEnabled() )
		{
#endif
            float r = ((float)rand()/(float)RAND_MAX) * randomizer;
            if( rand()%2 == 0 ) r = -r;
			Meshes[i]->TimeToDelete = time + r;
			Meshes[i]->Flags |= Entity::EEntDontDeleteOnTimer;
#ifdef TRANSGAMING_NO_HAVOK
	}
#else
		}
#endif
}

void World::MeshesActiveGroupRemove( BYTE group )
{
#ifndef TRANSGAMING_NO_HAVOK
	// Inactive
	Physics->ActiveMeshGroupSetTimeToRemove( group, 0, 0 );
#endif

	DynamicArray<Entity*> toRemove;

	// Active; "autodelete"
	for( Entity* entity=EntityList; entity; entity=entity->Next )
	{
		if( entity->Type == Entity::ETypeMesh )
		{
			WorldMesh* mesh = (WorldMesh*)entity;
#ifndef TRANSGAMING_NO_HAVOK
			if( mesh->ActiveGroup == group && mesh->PO && mesh->PO->IsEnabled() )
			{
#endif
				mesh->RemovePhysicsObject();
				mesh->EnableDraw(false);
				mesh->TimeToDelete = -1;
				toRemove.Add( mesh );
#ifndef TRANSGAMING_NO_HAVOK
			}
#endif
		}
	}

	for( int i = 0; i < toRemove.Size(); ++i )
		RemoveEntity( toRemove[i] );
	toRemove.ClearArray();

	// Active; not "autodelete"
	for( int i = 0; i < Meshes.Size(); ++i )
#ifdef TRANSGAMING_NO_HAVOK
	{
#else
		if( Meshes[i] && ( Meshes[i]->ActiveGroup == group ) && Meshes[i]->PO && Meshes[i]->PO->IsEnabled() )
		{
#endif
			Meshes[i]->RemovePhysicsObject();
			Meshes[i]->EnableDraw(false);
			Meshes[i]->TimeToDelete = -1;
			RemoveEntity( Meshes[i] );
#ifdef TRANSGAMING_NO_HAVOK
	}
#else
		}
#endif
}

void World::MeshesActiveGroupSetCollisionGroup( BYTE group, int collisionGroup )
{
#ifndef TRANSGAMING_NO_HAVOK
	// Inactive
	Physics->ActiveMeshGroupSetCollisionGroup( group, collisionGroup );
	for( Entity* entity=EntityList; entity; entity=entity->Next )
		if( entity->Type == Entity::ETypeMesh )
		{
			WorldMesh* mesh = (WorldMesh*)entity;
			if( mesh->ActiveGroup == group && mesh->PO )
				mesh->PO->SetCollisionGroup( collisionGroup );
		}
#endif
}

#define WORLD_LOAD_SAVE_ENTITIES_TOKEN 0x5e45

bool World::LoadEntities( FileObject* f )
{
	int count, type;
	String filename, name;
	float scale;

	if( !f->TestToken( WORLD_LOAD_SAVE_ENTITIES_TOKEN ) )
	{
		GLog.Print( "Token test failure: World::LoadEntities()\n" );
		return false;
	}
    
    *f >> DemonFX.Enabled >> DemonFX.Super >> GhostsFX;

	LoadInProgress = true;

	// Entities count
	*f >> count;

	for( int i = 0; i < count; ++i )
	{
        char centered;
		*f >> type >> filename >> name >> scale >> centered;
        Entity* entity = CreateEntity( type, filename.AsChar(), name.AsChar(), scale, centered?true:false);

		if( !entity || !entity->LoadEntity( f ) )
		{
			LoadInProgress = false;
			return false;
		}

        char shouldBeInWorld;
        *f >> shouldBeInWorld;

		if( shouldBeInWorld )
			AddEntity( entity );
	}

    // restore parent pointers
    for( int i = 0; i < EntityGIndexes.Size(); ++i )
    {
        Entity* entity = EntityGIndexes[i];
        if( entity  && entity->LightCount != 0) 
        {
            Entity * parent = EntityGIndexes[entity->LightCount];
            if (parent)
                parent->RegisterChild(entity, entity->TransformByParent,entity->ParentJoint);
            entity->LightCount = 0;
            entity->UpdateTransform(true);
        }
    }

	LoadInProgress = false;

	return true;
}

bool World::SaveEntities( FileObject* f )
{
	if( !f->PutToken( WORLD_LOAD_SAVE_ENTITIES_TOKEN ) ) return false;

    *f << DemonFX.Enabled << DemonFX.Super << GhostsFX;

	// This saves only meshes without MPKIndex. To synchronize MPK meshes, save physics.

	// First determine how many entities to save
	int count = 0;
	for( int i = 0; i < EntityGIndexes.Size(); ++i )
	{
		Entity* entity = EntityGIndexes[i];
		if( !entity ) continue;

		switch( entity->Type )
		{
		case Entity::ETypeMesh:
#ifndef TRANSGAMING_NO_HAVOK
            if (entity->AsMesh()->MPKIndex == INVALID_MPKINDEX && !(entity->PO && entity->PO->GetGlass()))
                count++;
#endif
			break;
        case Entity::ETypeDecal:
            if (entity->FileName != "shadow")
                count++;
            break;
		case Entity::ETypeModel:
		case Entity::ETypeTrail:
		case Entity::ETypeSound:
        case Entity::ETypeBillboard:
        case Entity::ETypeParticleFx:
        case Entity::ETypeLight:
			count++;
			break;

		// TO DO - handle more types

//		case Entity::ETypeRegion:		// don't save regions
//		default:
//			break;
		}
	}

	*f << count;

	for( int i = 0; i < EntityGIndexes.Size(); ++i )
	{
        bool saveInWorld = false;
		Entity* entity = EntityGIndexes[i];
		if( !entity ) continue;

		switch( entity->Type )
		{
		case Entity::ETypeMesh:
#ifndef TRANSGAMING_NO_HAVOK
            if (entity->AsMesh()->MPKIndex == INVALID_MPKINDEX && !(entity->PO && entity->PO->GetGlass()))
            {
//				Assert( strstr( entity->Name.AsChar(), "glass" ) == 0 );
                if( !entity->SaveEntity( f ) )
                    return false;
                saveInWorld = true;
            }
#endif
			break;
        case Entity::ETypeDecal:
            if (entity->FileName != "shadow")
            {
                if( !entity->SaveEntity( f ) )
                    return false;
                saveInWorld = true;
                count++;
            }
            break;
		case Entity::ETypeModel:
		case Entity::ETypeTrail:
        case Entity::ETypeSound:
        case Entity::ETypeBillboard:
        case Entity::ETypeParticleFx:
		case Entity::ETypeLight:
			if( !entity->SaveEntity( f ) )
				return false;
            saveInWorld = true;
			break;

		// TO DO - handle more types
		//case Entity::ETypeRegion:		// don't save regions
		default:
			break;
		}

        if (saveInWorld)
        {
		    char inWorld = entity->WorldLink?1:0;
		    *f << inWorld;
        }
	}

	return true;
}

bool World::LoadGlasses( FileObject* f )
{
	int count;
	*f >> count;
	Assert( count == Glasses.Size() );
	for( int i = 0; i < count; ++i )
		if( !Glasses[i]->LoadGlass( f ) )
			return false;
	return true;
}

bool World::SaveGlasses( FileObject* f )
{
	int count = Glasses.Size();
	*f << count;
	for( int i = 0; i < count; ++i )
		if( !Glasses[i]->SaveGlass( f ) )
			return false;
	return true;
}

bool World::LoadPortalState( FileObject* f )
{
	// 1. Load portals
	unsigned char tmp;
	int count;
	*f >> count;
	if( count > Portals.Size() ) return false;
	for( int i = 0; i < count; ++i )
	{
		*f >> tmp;
        Portals[i]->IsDisabled = (tmp!=0);
	}

	// 2. Count antiportals
	int ourCount = AntiPortals.Size();

	// 3. Load antiportals
	*f >> count;
	if( count != ourCount ) return false;

	for( int i = 0; i < ourCount; ++i )
	{
		*f >> tmp;
		AntiPortals[i]->Disabled = (tmp!=0);
	}

	return true;
}

bool World::SavePortalState( FileObject* f )
{
	// Warning: newly created antiportals ( ie. created from meshes ) will not be saved/loaded here!!!

	// 1. Save portals
	unsigned char tmp;
	*f << Portals.Size();
	for( int i = 0; i < Portals.Size(); ++i )
	{
		tmp = Portals[i]->IsDisabled?1:0;
		*f << tmp;
	}


	*f << AntiPortals.Size();
	for( int i = 0; i < AntiPortals.Size(); ++i )
	{
		tmp = AntiPortals[i]->Disabled?1:0;
		*f << tmp;
	}

	return true;
}


bool World::LoadZoneState( FileObject* f )
{
    int count;

    // death zones
    *f >> count;
    if( count > DeathZones.Size() ) 
        return false;
    
    for( int i = 0; i < count; ++i )
    {
        BYTE tmp;
        *f >> tmp;
        DeathZones[i]->Enabled = tmp?1:0;
    }

    
    return true;
}

bool World::SaveZoneState( FileObject* f )
{
    // death zones
    *f << DeathZones.Size();
    
    for( int i = 0; i < DeathZones.Size(); ++i )
        *f << (BYTE) DeathZones[i]->Enabled;

    return true;
}

void World::ProcessLateVBs()
{
	if (!GEngine->Video)
		return;

#ifdef TRANSGAMING_MESH_WARNINGS
    TRACE("World::ProcessLateVBs(): processing %d late vertex buffers\n", LateVBsList.Size());
#endif

	RenderDevice *dev = GEngine->GetRenderDevice();
#ifndef TRANSGAMING
    GEngine->Video->MaterialSys->PreLoadAll();
#endif


	for (int i=0; i<LateVBsList.Size(); ++i)
	{
        if (!PtrMap.Find(LateVBsList[i])){
#ifdef TRANSGAMING_MESH_WARNINGS
            TRACE("World::ProcessLateVBs(): WARNING-> late VBs mesh '%s' seems to have been deleted! (index = %d)\n", LateVBsList[i]->Name.AsChar(), i);
#endif

			LateVBsList.FastRemove(i--);
        }
	}

#if 1

	for (int i=0; i<LateVBsList.Size(); ++i)
	{
		WorldMesh* mesh = LateVBsList[i];

# ifdef TRANSGAMING_MESH_WARNINGS
        TRACE("World::ProcessLateVBs(): initializing vertex buffers for the mesh '%s'\n", mesh->Name.AsChar());
# endif
		if (mesh->Data->VertStreamCount == 0)
			mesh->InitializeVBs(dev);
	}

#endif

	LateVBsList.ClearArray();
#ifdef TRANSGAMING_MESH_WARNINGS
    TRACE("World::ProcessLateVBs(): done processing late VBs\n");
#endif
}

void World::ClearRenderTargets()
{
    if (GEngine->Video)
    {
		RenderDevice *dev = GEngine->GetRenderDevice();
	    for (int i=0; i<TEX_MBLUR; ++i)
	    {
		    if (TexSceneMotionBlur[i])
		    {
				dev->SetRenderTarget(TexSceneMotionBlur[i], NULL);
				dev->ClearFB(0xFFFFFF);
		    }
	    }
	}
}

void World::OnSettingsChanged()
{
	for (int i=0; i<Meshes.Size(); ++i) {
		if (Meshes[i]) {
			Meshes[i]->SetupShaders();
			Meshes[i]->SetSpecular(Meshes[i]->SpecularPower);	// FORCE Specular reset
		}
	}
}

// =======================================================================================
// TEMPORARY ENTITIES CACHE - zamienic na klase i polaczyc z package cachem
//                          -  nie kasowac PO, tylko wyjmowac z havoka
//                          - zrobic funkcje Reset w entity zamiast recznego czyszczenia
// =======================================================================================
#ifdef USE_ENTITYCACHE
static DynamicArray<Entity*> gCachedEntities;
Entity* ECache_CreateEntity(int type, const char* fileName, const char* entityName, float scale, bool centerGeometry)
{
    if (type != Entity::ETypeMesh) // optimization -- cache works only with meshes
        return NULL;

	if( !fileName || !entityName || !gCacheEnabled ) return NULL;

    //if (type == Entity::ETypeMesh)
    //    GLog.Print("Request to cache(%d): ,%s, %s, %f\n",gCachedEntities.Size(),fileName,entityName,scale);

    for( int i = 0; i < gCachedEntities.Size(); ++i )
    {
        Entity * e = gCachedEntities[i];
        if( ( e->Type == type ) &&
			( e->GetScale() == scale ) &&
			( e->FileName == fileName ) &&
			( e->Name == entityName ) &&
			( (e->Flags & Entity::EEntCenteredGeometry)?true:false == centerGeometry )
           )
        {
            //GLog.Print("Entity from cache(%d/%d): ,%s, %s, %f\n",i,gCachedEntities.Size(),fileName,entityName,scale);
			e->EnableDraw(true); 
			gCachedEntities.FastRemove(i);
            return e;
        }
    }

    //if (type == Entity::ETypeMesh)
    //    GLog.Print("NOT FOUND\n");

    return NULL;
}
// =======================================================================================
bool ECache_DeleteEntity(Entity* e)
{

    if (!gCacheEnabled || !e->FileName.Len() || !e->Name.Len()) // wykluczam szyby itp.
        return false;

    if (e->Type == Entity::ETypeMesh)
    {
        e->Reset();
        gCachedEntities.Add(e);
        return true;
    }
    return false;
}
// =======================================================================================
void ECache_Clear()
{
#ifdef	TRANSGAMING
    TRACE("ECache_Clear(): clearing %d cached entities\n", gCachedEntities.Size());
    for (int i=0; i<gCachedEntities.Size(); ++i)
        GEngine->pcfWorld->DeleteEntityDelayed(gCachedEntities[i]);
#else
    for (int i=0; i<gCachedEntities.Size(); ++i)
        GEngine->World->DeleteEntityDelayed(gCachedEntities[i]);
#endif
    gCachedEntities.ClearArray();
}
#endif
// =======================================================================================
