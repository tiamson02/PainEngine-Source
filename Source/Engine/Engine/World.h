// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

// #pragma once
#ifndef _WORLD_
#define _WORLD_


#include "Core/HashMap.h"
#include "Core/Object.h"
#include "Render/Sky.h"
#include "Pathfinding/Pathfinder.h"
#include "Pathfinding/Pathfinder2.h"
#include "Engine/GlassSystem.h"
#include "Engine/AnimatedMeshManager.h"
#include "Engine/Environment.h"

class PhysicsWorld;
class AntiPortal;

#define MAX_NET_INDICES 32768

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#ifdef TRANSGAMING_WINDOWS
class ENGINE_API World
#else
class World
#endif
{
	friend class NetworkClient;
	friend class Portal;

public:
	DWORD							Frame; // frame counter

    DynamicArray<Plane>             Planes;

	DynamicArray<Zone*>             Zones;
	DynamicArray<Zone*>				DeathZones;
	DynamicArray<Zone*>				LadderZones;
	DynamicArray<Zone*>				IceZones;

	DynamicArray<Zone*>				SwitchZones;
	int								LastSwitchZoneIndex;
	String							LastSwitchZoneName;
	bool							SwitchZonesOff;

	DynamicArray<Portal*>           Portals;
	DynamicArray<Portal*>           Mirrors;
	DynamicArray<AntiPortal*>		AntiPortals;
    DynamicArray<WorldMesh*>        Meshes;
    DynamicArray<Volume*>			Volumes;
	DynamicArray<Environment*>		Environments;
	DynamicArray<Light*>            Lights;
	DynamicArray<Light*>			DLights;
	DynamicArray<Billboard*>        Billboards;
    DynamicArray<ParticleEffect*>   ParticleEffects;
	DynamicArray<Model*>            Models;
	DynamicArray<Glass*>			Glasses;
    DynamicArray<Entity*>			EntityGIndexes;
	HashMap<String,int>				EntityHash;


#ifdef TRANSGAMING
    HashMap<void*,int,256>          PtrMap;
# ifdef FIND_HASH_PEAK
    static int                      mMaxPtrMapUsage;
# endif
#else
    HashMap<void*,int,256>			PtrMap;
#endif

	DynamicArray<WorldMesh*>		LateVBsList;
	bool							LateVBs;

    Entity*                         EntityList;
	Entity*							PickedEntity;

	PhysicsWorld*					Physics;

	Pathfinder				        Paths;
	Pathfinder2						Paths2;
	bool							UseNewPaths;

	float						    Scale;
	float							FarClipDist;
	bool							UseFog;
	bool							Overbright;
	bool							RTCubeMap;
	bool							GhostsFX;
	bool							TargetsReleased;

    String							PhysicsFile;

	struct TBloomFX
	{
		FLOAT	LuminanceThreshold;
		FLOAT	Multiplier;
		DWORD	OverlayColor;
		FLOAT	DimScale;

		TBloomFX() 
		:	LuminanceThreshold(0.25F), Multiplier(1.25F), OverlayColor(0x080808), DimScale(0.8F)
		{}
	};

	TBloomFX	BloomFX;

	struct TDemonFX
	{
		bool	Enabled;
		bool	Super;
		float	Scale;
		float	Bias;
		float	Warp;
		float	MBlur0;
		float	MBlur1;
		TDemonFX() : Enabled(false), Super(false), Scale(1), Bias(0), Warp(0), MBlur0(0), MBlur1(0)
		{
		}
	};

	TDemonFX		DemonFX;

	TWater					Water;
	WorldMesh*				WaterMesh;
	Environment				Env;
	float					PixelLightingMultiplier;
	Plane					DynamicSpecular;

	DynamicArray<String>	MPKFilesLoaded;
	unsigned short			FixedMeshCounter;

	AnimatedMeshManager		AnimMeshManager;

	Sky				mSky;
	String			Name;
    String			FileName;
    String			LevelName;

	const static	int TEX_MBLUR=5;
	Texture*		TexSceneMotionBlur[TEX_MBLUR];
	Texture*		TexSceneDepth;
	Texture*		TexSceneCubemap;
	Texture*		TexStaticCubemap;
	Texture*		TexFB[3];
	Texture*		TexFrame[4];
	Texture*		TexZBuf;
#ifndef TRANSGAMING
	Texture*		SkySpecularCubemap;
#endif
	DWORD			CubemapFrame; // Cubemap update frame

	bool			RenderToCubemap;
	bool			InLoadingScreen;

	bool			LoadInProgress;			// so I don't have to worry about EntityGIndexes table when I load - Krystian

	int				DrawDynLights;
	bool			HighQualityLights;
	int				WaterQuality;

    DynamicArray<Entity*> LastExplodedEntities;

public:
    World();
    ~World();

    BOOL    LoadMeshPak(const char* name);
	BOOL	LoadMeshPakFile(const char* name);
	static	DynamicArray<WorldMesh*> LoadMeshPakArray(const char* name);
#ifdef TRANSGAMING_USE_EDITOR
	static  BOOL SaveMeshPakArray(const char* name, DynamicArray<WorldMesh*> Meshes);
#endif

	// Waypoints/floors load/save
	int		LoadWaypoints(const char* fileName, float scale = 1.0f, bool dontLoadPortals = false ) { return Paths.LoadContents( fileName, scale, dontLoadPortals ); };
	int		LoadFloors(const char* fileName, float scale = 1.0f) { return Paths.LoadFloors( fileName, scale ); };
	int		SaveWaypoints(const char* fileName, float scale = 1.0f) { return Paths.SaveContents( fileName, scale ); };
	int		SaveFloors(const char* fileName, float scale = 1.0f) { return Paths.SaveFloors( fileName, scale ); };

	// Entities load/save - for savegames
	bool	LoadEntities( FileObject* f );
	bool	SaveEntities( FileObject* f );

	// Glasses load/save - for savegames
	bool	LoadGlasses( FileObject* f );
	bool	SaveGlasses( FileObject* f );

	// Portals and antiportals state load/save - for savegames
	bool	LoadPortalState( FileObject* f );
	bool	SavePortalState( FileObject* f );

    // zones state load/save - for savegames
    bool	LoadZoneState( FileObject* f );
    bool	SaveZoneState( FileObject* f );

	Entity* CreateEntity(int type, const char* fileName, const char* entityName, float scale, bool centerGeometry = false);
	void	DeleteEntity(Entity* entity );
	void	DeleteEntityDelayed( Entity* entity );	// This will cause entity deletion at the next tick
	void	DeleteDelayedEntities( void );			// use this to delete entities scheduled to be deleted
    void	DeleteDyingEntities( void );
	void	MeshesTableToSlotAdd( WorldMesh* worldmesh );

	// For mesh explosions
	int		ExplodeItem( Entity* item, const char* fileName, float strength, float radius, float lifetimeAfterExplosion, float ascale = 1.0f, bool nonCollidable = false, const char* meshName = NULL , Vector offset = Vector(0,0,0));
	WorldMesh*	DamageItem( WorldMesh* item, const char* fileName, const char* meshName );
	Model*		GibModel( Model* model, const char* gibFileName, float strength, float radius, Entity* owner, int weapon_type, int collisionGroup, const char* getVelFromJoint = NULL );

	// Rendering
	void		BlurTexture(Texture* texture, Texture* textureFilter, float size = 1, bool color=false) const;
	void		DrawTexture(Texture* texture, Material* material) const;
	void		ComposeTextures(Texture* targetTex, Texture** sourceTex, float* sourceArg, int sourceNum) const;

	Entity*		PickEntity(Viewport& vp, int ptx, int pty, BOOL UseHavok=0);

    inline Entity *GetEntityByIndex(int idx, int etype=-1) const
    {
        if (idx <= 0 || idx >= EntityGIndexes.Size())
            return NULL;

		#ifdef ENGINE_LOG
		GLog.Print("^*GEBI: %d,%d,%s\n",idx,etype,EntityGIndexes[idx]?"+":"-");
		#endif

#ifndef FINAL_BUILD
		if ( etype>-1 && EntityGIndexes[idx])
		{
			Assert(etype == EntityGIndexes[idx]->Type);

# ifdef DEBUG
            if (etype != EntityGIndexes[idx]->Type){
				//MessageBox(NULL,"Bad entity type!","GetEntityByIndex Error!!!",MB_TOPMOST);
                TRACE("World::GetEntityByIndex(): Bad entity type! (idx = %d, etype = %d)\n", idx, etype);
                Assert(0);
            }
# endif

		}
#endif

        return EntityGIndexes[idx];
    }

	inline Entity* GetEntityByName(const char* name) 
	{
		Assert(name);
		const int* p = EntityHash.Find(name);
#ifdef _DEBUG
		if (p) {
			const int idx = (*p);
			Assert(idx >= 0 && idx < EntityGIndexes.Size());
			return EntityGIndexes[idx];
		}

		return NULL;
#else
		return p ? EntityGIndexes[(*p)] : NULL;
#endif // _DEBUG
	}

    void AddEntity(Entity* entity);
    void RemoveEntity(Entity* entity);
	void UpdateEntity(Entity* entity);
	void UpdateAllEntities();

    void Init(float scale = 1.0f);
    void Dump();
    void Tick(const float timeDelta);
#ifdef PKTV_ENABLED
    void PKTVDemoTick(const float timeDelta);	// to avoid deletion of demo entities from World Tick
#endif
	void TestSwitchZones( const Vector& pt );
	void UseSwitchZones( bool use ) { SwitchZonesOff = !use; };

    void Release(const char* NewMap = NULL);
	void ReleaseWithoutMap();

    // compatibility
    int FindZone(const Vector& pos);

#ifndef TRANSGAMING
	String GetFirstPackageName( void ) { return PackageCount?Packages[0]->NameTable[0]:String(""); };
#endif

	const char *InDeathZone(const BoundingBox& box) const;
	const char *InDeathZone(const Vector& pt) const;
	bool NearLadder(const BoundingBox& box) const;
	bool NearLadder(const Vector& pt) const;
	bool OnIce(const BoundingBox& box) const;
	bool OnIce(const Vector& pt) const;

	int GetEntities(int type, const BoundingBox& box, Entity** entityArr, int maxelems);
#ifndef TRANSGAMING
    void MeshesSetDefaultDetailMaps(const char *filename, float tileU, float tileV);
#endif
	void MeshesSetDefaultCubeMaps(const char *filename);
	void MeshesSetDefaultNormalMaps(const char *filename);
    void MeshesActiveGroupEnableDraw(BYTE group,bool enable);
    void MeshesActiveGroupSetTimeToRemove( BYTE group, float time, float randomizer = 0.0f );
	void MeshesActiveGroupRemove( BYTE group );
	void MeshesActiveGroupSetCollisionGroup( BYTE group, int collisionGroup );

	void EnablePortal( const char* name, bool enable = true );
	bool IsPortalEnabled( const char* name );

	void EnableAntiPortal( const char* name, bool enable = true );
	bool IsAntiPortalEnabled( const char* name );
    void DeleteAntiPortal( const char* name);

	void BuildGlasses();
	void SortEnvironmentByNames();

	void ProcessLateVBs();
	void ClearRenderTargets();
	void OnSettingsChanged();

private:

    void BuildZones();
    void BuildVolumetrics();
    int  CreatePlane(const Vector& a, const Vector& b, const Vector& c);

private:
#ifndef TRANSGAMING
    static const int MaxPackages=16;
    ObjectPackage*  Packages[MaxPackages];
    int             PackageCount;
#endif

	// Delayed delete tables
	DynamicArray<Entity*> mDelayedDeleteEntities;
};

class CPackageManager
{
protected:
#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
    static int                          mMaxCacheUsage;
#endif
    HashMap <String,ObjectPackage*,64>  Cache;	
public:
    CPackageManager::~CPackageManager()
    {
        Clear();
    }

    ObjectPackage *Get(const char* name)
    {
        ObjectPackage **pack = Cache.Find(name);
        if (pack)
            return *pack;

        m_setTag("Object");
        ObjectPackage * newpack = new ObjectPackage(name);
        Cache.Insert(name,newpack);

#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
        if (Cache.Size() > mMaxCacheUsage){
            mMaxCacheUsage = Cache.Size();
            TRACE("@@@ Cache::Get(): peak Cache hash usage = %d\n", mMaxCacheUsage);
        }
#endif

        return newpack;
    }

    void Clear(){
        for(int i=0; i<Cache.Size(); ++i)
            delete Cache.GetPair(i).Value;
        
        Cache.Clear();
    }
    int Size(){
        return Cache.Size();
    }
} ;



#endif //_WORLD_

