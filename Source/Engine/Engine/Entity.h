// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created:	2002/04	Bart Sekura
//----------------------------------------------------------------------------

// #pragma once
#ifndef _ENTITY_
#define _ENTITY_


class PhysicsObject;
class UdpMessage;
class Environment;
class Light;
class Model;
class WorldRegion;
class Volume;
class Billboard;
class World;
class Zone;
class WorldMesh;
class ParticleEffect;
class Decal;
class Trail;
class MaterialSystem;

#include "Core/Object.h"
#include "Core/Math/LeMath.h"
#include "Render/RenderDevice.h"

class ENGINE_API Entity : public Object
{
	friend class ObjectSynchronizer;
public:
    enum
    {
        EEntTranslucent			= (1<<1),
        EEntMovable				= (1<<2),
		EEntNoTick				= (1<<3),
		EEntDontDeleteOnTimer	= (1<<4),
        EEntAlwaysVisible		= (1<<5),       // always visible, always add this entity to visible entities' list
        EEntNoDraw				= (1<<6),       // no draw, never call ::Draw() in this entity, even if it's on visible entities' list
												// useful for entities that are never linked to any zone
	    EEntCenteredGeometry	= (1<<7),       // geometry centered to local space
		EEntDontUpdate			= (1<<8),		// don't sychronize object in unreliable network messages
		EEntDestroyOnReload		= (1<<9),
	};

	// NOTE: the entity flags above cannot exceed 1<<9!

	enum
    {
		ETypeNone       = 0,
        ETypeMesh       = 1,
        ETypeLight      = 2,
        ETypeParticleFx = 3,
		ETypeModel		= 4,
		ETypeRegion     = 5,
		ETypeDecal		= 6,
		ETypeVolume		= 7,
		ETypeBillboard	= 8,
		ETypeEnv		= 9, // defines environment settings overwriting those in level
		ETypeTrail		= 10,
        ETypeSound		= 11,
    };

	enum
	{
		ESortSky,
		ESortOpaque,
		ESortDecal,	
		ESortDecalDyn,	
		ESortTrans,	
		ESortVolume,
		ESortCorona,
		ESortGun,
		ESortWarp,

		ESortNumEntries
	};

	enum SynchroFlags
	{
		ESynchroEntity				= (1<<0),
		ESynchroDelete				= (1<<1),	// helper marker
		ESynchroRagdoll				= (1<<2),
		ESynchroAnimation			= (1<<3),
		ESynchroRagdollCreation		= (1<<4),
	};

	enum LightFlags
	{
		ELightingNone,
		ELightingAllLights,
		ELightingDLights,
	};

	enum
	{
		EReflectScene	= (1<<0),
		ERefractScene	= (1<<2),
	};

    unsigned int    GIndex; // global unique index
    DWORD           Flags;
	DWORD			Lighting; // lighting type of entity
    int             Type;
	int				Sort;
	bool			Demonic; // render using custom pass/shader when in demon mode
	int				WarpScreen;
	int				WaterSurf;
	int				ReflectSurf;	// render using reflected scene
	int				VisCounter;
	BoundingBox		Box;
	BoundingBox		LocalBox;
	Matrix			ObjectToWorld;
	PhysicsObject*	PO;

	float			TimeToDelete;
	float			FadeTime;
	float			DistanceFromCamera;
	float			KillDist;				// don't draw this entity if dist from camera > KillDist

    World*			WorldLink;
    Entity*			Next;
    Entity**		Prev;

	String			FileName;
	
	struct TEnv {
		bool			Initiated;
		bool			OverwriteAmbient; // dumb flag for brightskins ambient
		Environment*	Link;
		float			LightFadeTime;
		float			LightProgress;
		float			LightProgressDelta;
		Light*			DirLight;
		Vector			Ambient;
		Vector			DirColor;

		TEnv() 
		: Link(NULL), LightFadeTime(0), LightProgress(1), LightProgressDelta(0)
		, DirLight(0), Ambient(0,0,0), DirColor(0,0,0), Initiated(false), OverwriteAmbient(false)
		{}
	} Env;

	DynamicArray<Entity*>	Children;
	Entity*					Parent;
	char                    ParentJoint;
	bool TransformByParent, KillByParent, TestDeathZone;

    struct TZoneLink
    {
        Zone*       ZoneLink;
        Entity*     Next;
        Entity**    Prev;
		int			_pad;
    };

    static const int MaxZoneLinks=64;
    TZoneLink   ZoneLinks[MaxZoneLinks];

	// material sorted rendering list
	Entity*	MatNext;
	Entity*	MatPrev;
	float	Distance;

	struct TVSLight
	{
		Plane Color;
		Plane Dir;
		Plane HalfDir;		
	};

	static const int MaxInfluences=4;

	static const int MaxLights=4;

	Light*		Lights[MaxLights];
	float		LightIntensity[MaxLights];
	TVSLight	VSLights[MaxLights];
	int			LightCount;

	int ComputeVSLights(int maxLights=MaxLights);

protected:
	float			Scale;
	Quaternion		Rotation;
	Vector			Translation;

public:

    Entity(int type);
	virtual ~Entity();

    WorldMesh*      AsMesh()		{ return Type==ETypeMesh		? reinterpret_cast<WorldMesh*>(this)		: 0; }
    Light*          AsLight()		{ return Type==ETypeLight		? reinterpret_cast<Light*>(this)			: 0; }
	ParticleEffect* AsParticleFx()	{ return Type==ETypeParticleFx	? reinterpret_cast<ParticleEffect*>(this)	: 0; }
	Model*			AsModel()		{ return Type==ETypeModel		? reinterpret_cast<Model*>(this)			: 0; }
	WorldRegion*	AsRegion()		{ return Type==ETypeRegion		? reinterpret_cast<WorldRegion*>(this)		: 0; }
	Decal*			AsDecal()		{ return Type==ETypeDecal		? reinterpret_cast<Decal*>(this)			: 0; }
	Volume*			AsVolume()		{ return Type==ETypeVolume		? reinterpret_cast<Volume*>(this)			: 0; }
	Billboard*		AsBillboard()	{ return Type==ETypeBillboard	? reinterpret_cast<Billboard*>(this)		: 0; }
	Environment*	AsEnvironment()	{ return Type==ETypeEnv			? reinterpret_cast<Environment*>(this)		: 0; }
	Trail*			AsTrail()		{ return Type==ETypeTrail		? reinterpret_cast<Trail*>(this)			: 0; }

	virtual void RenderInitialize(RenderDevice* dev, MaterialSystem* matSys, const char* texSubPath=0) {}
	virtual void RenderCleanup   (RenderDevice* dev, MaterialSystem* matSys) {}
	virtual void Draw(RenderDevice* /*dev*/, int /*mode*/) {}
	virtual void Tick(const float /*timeDelta*/);
    virtual void Reset(); // reset entity for reusing
#ifdef TRANSGAMING_USE_EDITOR
	void  RenderBoundingBox(int color=0xFFFFFFFF);
	void  RenderLocalBoundingBox(int color=0xFFFFFFFF);
#endif
	virtual void EnableDraw(bool en);

	// Parenting
#ifdef TRANSGAMING
	void RegisterChild(Entity* child,bool transformByParent=true,char joint=-1);
	void UnregisterChild(Entity* child);
	void UnregisterAllChildren( int type = -1 );
	void KillAllChildren( int type = -1 );
	bool KillAllChildrenByName( const char *name);
	Entity * GetChildByName( const char *name);
#else
	inline void UnregisterAllChildren( int type = -1 );
    inline void KillAllChildren( int type = -1 );
    inline bool KillAllChildrenByName( const char *name);
    inline Entity * GetChildByName( const char *name);
#endif
	virtual bool OnUnregister() {return true;}; // Child Event - return true to autodelete children

	void AddLight(Light* light);
#ifdef	TRANSGAMING	
	void ResetLights();
	void SetEnvironmentAmbient(bool enable, const Color_TG& col); // enforce ambient
	const Color_TG GetEnvironmentAmbient();
#else
	inline void ResetLights();
	void SetEnvironmentAmbient(bool enable, const Color& col); // enforce ambient
	const Color GetEnvironmentAmbient();
#endif
	Light* GetEnvironmentDirLight();

	virtual void UpdateVolEntity() {}

	// Load/save
	virtual bool LoadEntity( FileObject* f );
	virtual bool SaveEntity( FileObject* f );

#ifndef TRANSGAMING
    // Demo recording
	virtual bool DemoCalcSize( int &size, DWORD &contents );
	virtual bool DemoReadEntity( FileObject *f );
	virtual bool DemoWriteEntity( FileObject *f );
	virtual bool DemoReadFull( FileObject *f );
	virtual bool DemoWriteFull( FileObject *f );
#endif

	// Physics objects methods
	void CreatePhysicsObject( DWORD poType, float scale = 1.0f, int collisionGroup = -1, bool dontAddToWorld = false );
	void RemovePhysicsObject( void );

	bool SeesPoint( Vector& point );
	bool SeesEntity( Entity& entity );

	// Other
	virtual void SetPosition(const Vector &v);
	virtual void SetOrientation(const float angle);
	virtual void SetRotation(const Quaternion& rotation);
	virtual void SetPositionAndRotation( const Vector &v, const Quaternion& rotation );
	virtual void SetScale(float scale);
	virtual void SetObjectTransform(const Matrix& mat);
#ifndef TRANSGAMING
	inline 
#endif
    virtual void UpdateTransform(bool updateInWorld = true);

	// Synchronization
#ifndef TRANSGAMING
	inline 
#endif
    void SetSynchroState( unsigned char newState, unsigned char updateClass, unsigned short specialClient, unsigned char specialClass );
	inline unsigned char SynchroState( void ) { return mSynchroFlags; };
	inline void SetSynchroString( const char* str )
	{
		if( mSynchroString ) delete [] mSynchroString;
		if( !str )
			mSynchroString = 0;
		else
		{
            m_setTag("Object");
#ifdef TRANSGAMING
            size_t len = strlen(str);


			mSynchroString = new char[len + 1];
			::strncpy(mSynchroString, str, len + 1);
#else
			mSynchroString = new char[strlen(str)+1];
			::strcpy( mSynchroString, str );
#endif
		}
	}
	inline char* GetSynchroString( void ) { return mSynchroString; };

protected:
	unsigned char	mSynchroFlags;
	char* mSynchroString;
public:

#ifdef TRANSGAMING
	void SetIdentity();
#else
	inline void SetIdentity();
#endif
	inline Vector GetPosition() const { return Translation; };
	inline Quaternion GetRotation() const { return Rotation; }; 
	inline Vector GetWorldPosition() const { return ObjectToWorld.GetTranslation(); }; // true world position when linked to parent
	inline Quaternion GetWorldRotation() const { return ObjectToWorld.GetRotation(); }; // true world rotation when linked to parent
	virtual float GetWorldScale() const { return (Parent && TransformByParent) ? Scale*Parent->GetWorldScale() : Scale; }
	inline float GetOrientation() const { Vector v = Rotation * Vector(0,0,1); return (float)atan2(v.X,v.Z); };
	virtual Vector GetCenter() const { return Box.GetCenter(); }
	inline Vector GetForwardVector() const { return Vector(ObjectToWorld.Z.X, ObjectToWorld.Z.Y, ObjectToWorld.Z.Z); };
    inline float GetScale() const { return Scale; }
	float GetCamDistance() const;



	// serialization
    void Load(FileObject* file)
    {
		Object::Load(file);
        *file >> Flags >> Box >> ObjectToWorld;
    }

    void Save(FileObject* file)
    {
		Object::Save(file);
        *file << Flags << Box << ObjectToWorld;
    }


    // friend
    friend bool ECache_DeleteEntity(Entity* e);

	// Demo frame contents flags
	enum
	{
		EDemoWriteFlags					= ( 1 << 0 ),
		EDemoWriteTranslation			= ( 1 << 1 ),
		EDemoWriteRotation				= ( 1 << 2 ),
		EDemoWriteScale					= ( 1 << 3 ),
		EDemoWriteDemonic				= ( 1 << 4 ),
		EDemoWriteTimeToDelete			= ( 1 << 5 ),
		EDemoWriteFadeTime				= ( 1 << 6 ),
		EDemoWriteSort					= ( 1 << 7 ),
		EDemoWriteParent				= ( 1 << 8 ),
		EDemoWriteParentGIndex			= ( 1 << 9 ),
		EDemoWriteParentJoint			= ( 1 << 10 ),
		EDemoWriteTransformByParent		= ( 1 << 11 ),
		EDemoWriteTrailBonesNum			= ( 1 << 12 ),
		EDemoWriteMeshAnims				= ( 1 << 13 )
	};

protected:
	// Last frame state for demo recording
	DWORD	mLastFlags;
	Vector	mLastTranslation;
	Quaternion mLastRotation;
	float	mLastScale;
	bool	mLastDemonic;
	float	mLastTimeToDelete;
	float	mLastFadeTime;
	int		mLastSort;
	bool	mLastParent;
	unsigned int	mLastParentGIndex;
	char	mLastParentJoint;
	bool	mLastTransformByParent;
};


#endif //_ENTITY_

