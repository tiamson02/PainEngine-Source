// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
// $Header: /Game/Source/Engine/Render/ParticleSystem.h 101   9/10/04 5:54p Bart $
//----------------------------------------------------------------------------

// #pragma once
#ifndef _PARTICLESYSTEM_
#define _PARTICLESYSTEM_


class ParticleEffect;
class Camera;
class Material;
#ifdef	TRANSGAMING
struct Sprite_TG;
#else
struct Sprite;
#endif

#include "Core/HashMap.h"
#include "Render/VertexBuffer.h"
#include "Render/Vertex.h"

inline float FloatRand(float min, float max)
{
	return max + (min-max) * (rand()/(float)RAND_MAX);
}

inline float FloatRand()
{
    return rand()/(float)RAND_MAX;
}

#ifndef TRANSGAMING
inline Vector VRand()
{
	Vector Result;
	do
	{
		// Check random vectors in the unit sphere so result is statistically uniform.
		Result.X = FloatRand()*2.0F - 1.0F;
		Result.Y = FloatRand()*2.0F - 1.0F;
		Result.Z = FloatRand()*2.0F - 1.0F;
	} 
	while (Result.MagnitudeSquared() > 1.0F);

	return Result.GetNormal();
}
#endif

struct FloatRange
{
	float	Min,Max;

	FloatRange() {}
	FloatRange(const float x) : Min(x), Max(x) {}
	FloatRange(const float min, const float max) : Min(min), Max(max) {}
	inline float RandomValue() const
	{
		return Min==Max ? Min : FloatRand(Min,Max);
	}
	inline float Lerp(const float t) const
	{
		return Min+(Max-Min)*t;
	}
};

struct VectorRange
{
	Vector Min,Max;

	VectorRange() {}
	VectorRange(const Vector& a) : Min(a), Max(a) {}
	VectorRange(const Vector& min, const Vector& max) : Min(min), Max(max) {}
	inline Vector RandomValue() const
	{
		return Vector
		(
			Min.X==Max.X ? Min.X : FloatRand(Min.X,Max.X),
			Min.Y==Max.Y ? Min.Y : FloatRand(Min.Y,Max.Y),
			Min.Z==Max.Z ? Min.Z : FloatRand(Min.Z,Max.Z)
		);				
	}
	inline Vector Lerp(const float t) const
	{
		return Vector
		(
			Min.X+(Max.X-Min.X)*t,
			Min.Y+(Max.Y-Min.Y)*t,
			Min.Z+(Max.Z-Min.Z)*t
		);		
	}
};

struct ENGINE_API Particle
{
	Vector	Position;	// current position
	Vector	Velocity;	// current velocity
	Vector	Color;		// current color (rgb)
	Vector	AccelVal;   // current acceleration
	Vector	Accel;

	float StartDelta;

	float	LifeTime;	// max life time
	float	Time;		// current time
	float	Size;		// current size
	float	Alpha;		// current alpha
	float	AnimTime;

	float	StartRot;
	float	Rotation;	// current rotation

    float   Thickness;  // sparks: thickness
    float   Length;     // sparks: length as a fraction of velocity

	FloatRange	SizeRange;

	VectorRange	VelRange;

	int			TexFrame;

	Particle*	Next;
	Particle*	Prev;
	Particle*	NextFree;
};

#ifdef	TRANSGAMING
struct ENGINE_API Sprite_TG
{
    Vector  Position;
    DWORD   Color;
    float   Size;
	float   Rotation;
    Quaternion   QRotation;
	Sprite_TG() : Size(0), Rotation(0)
	{
	}

    // render specific
    int     BlendMode;
    int     TexRef;
};
#else
struct ENGINE_API Sprite
{
    Vector  Position;
    DWORD   Color;
    float   Size;
	float   Rotation;
    Quaternion   QRotation;
	Sprite() : Size(0), Rotation(0)
	{
	}

    // render specific
    int     BlendMode;
    int     TexRef;
};
#endif

struct ENGINE_API Sprite1DOF
{
    DynamicArray<Vector> Position;
    float   Size;
    DWORD   Color;
    Sprite1DOF() : Size(0), DrawMode(0)
	{
	}

    // render specific
    int     DrawMode; // 0 - binormal, 1 - horizontal, 2 - vertical
                      // 10 - binormal, 11 - horizontal, 12 - vertical (+ texture tailing)
    int     BlendMode;
    int     TexRef;
};
class ENGINE_API ParticlePool
{

public:
	int Count;
#ifdef TRANSGAMING_MEMORY_CONSTRAINTS
    //this value controls the total number of global particles that may be in use at any
    //given time in the entire world.  If this limit is reached, some horrible stuff may
    //happen since there is no error checking in the Alloc() function
	static const int MaxParticleCount = 500;
#else
	static const int MaxParticleCount = 4096*10;
#endif
	ParticlePool();

	Particle*	Alloc();
	void		Free(Particle* particle);

	Particle	Particles[MaxParticleCount];
	Particle*	FreeList;
};


class ENGINE_API ParticleEmitter
{
public:
    enum 
	{
        SpriteEmitter   = 1,
        SparkEmitter    = 2,
        BeamEmitter     = 3,    // TODO
    };

public:

    int					Type;
	Vector				Position;
	Vector				LastPosition;
	VectorRange			PosRange;		// random position spread
	VectorRange			VelStart;		// velocity spread
	VectorRange			VelEnd;		// velocity spread
	VectorRange			AccelRange;
	ParticleEmitter*	Master;

	VectorRange			ColorRange;		// color change during particle lifetime
										//	(color not used)

	FloatRange	StartSize;	// starting size spread
	FloatRange	EndSize;	// end size spread
	FloatRange	LifeTime;	// life time spread
	FloatRange	Alpha;		// alpha spread - fade in, fade out	
	float       AlphaMid;
	FloatRange	FadeTime;	// alpha fade time (0-100% life time)

	FloatRange	VelBlend;	// velocity blender fade time (0-100% life time)

	FloatRange	RotRange;	// rotation range
	BOOL		Immortal;

    // spark emitter only
    FloatRange  Thickness;
    FloatRange  Length;     

	ParticlePool*	Pool;

	// list of particles
	Particle*		ListFront;
	Particle*		ListBack;

	FLOAT	SpawnRate;	// emission rate, 1.0f/particles per second
	FLOAT	SpawnTime;	// timestamp to keep the rate going

	int		MaxParticles;
    int     ActiveParticleCount;
    int     SpawnedParticleCount;

    String		Name;
    String		TextureName;
    Texture*	TexturePtr;
	String		WarpTexName;
	Texture*	WarpTexPtr;
	String		MaterialName;
	Material*	MaterialPtr;
	float		TexAnimFPS;
	float		KillDist;		// don't render this emitter if dist from camera > KillDist
	//float		DepthBias;		// move towards camera to avoid ugly clipping
    int			BlendMode;

	ParticleEmitter*	Next;
	ParticleEmitter**	Prev;

	BoundingBox		Box;
	ParticleEffect*	Effect;

    Quaternion  RotQuat;

	DWORD	Active:1;
	DWORD   UseRandomNormal:1;
	DWORD	Respawn:1;
	DWORD   Evolve:1;
	DWORD	LerpColor:1;
	DWORD	DepthTest:1;
	DWORD   Dying:1;
	DWORD	Warp:1;
	
public:
	ParticleEmitter();
	ParticleEmitter& operator=(const ParticleEmitter& emitter);

    void Restart();
	void Deactivate(BOOL instantKill=FALSE);
	void InitParticle(Particle* particle);
	void Tick(float deltaTime);
	void SetScale(const float scale);
	BOOL IsActive() const { return Active; }
};

class ParticleRender;

class ENGINE_API ParticleSystem
{
// verts=5000*4=20K, elems=5000*6=30K
#ifdef TRANSGAMING_MEMORY_CONSTRAINTS
    //this value controls how many particles can be rendered in any given frame by
    //any single particle emitter.  The particles can be reused as well.  This also
    //controls the maximum number of particles may be spawned at any given frame by
    //any given emitter.
#define MAX_BUFFERED_PARTICLES	ParticlePool::MaxParticleCount     //this limit only controls the size of the index buffer.  At this size, the array will be 6KB
#else
#define MAX_BUFFERED_PARTICLES	5000
#endif


#ifdef TRANSGAMING_PS2
    //this value is the maximum number of verticies that should be needed to
    //render a single emitter assuming deindexed triangle lists.
    //NOTE: the 50 and 50 are additional space in the vertex buffer allocated for use
    //  in the trail system, and the billboard system.  THESE SHOULD NOT BE MAGIC NUMBERS!!!
    static const int MaxVertexCount = (MAX_BUFFERED_PARTICLES + 50 + 50) * 6;

    //deinterleaved vertex buffer types
    typedef VertexBuffer<Vector,    RenderDevice::EVertexPos>       VertexPosBuffer;
    typedef VertexBuffer<DWORD,     RenderDevice::EVertexDiffuse>   VertexColorBuffer;
    typedef VertexBuffer<TexCoord,  RenderDevice::EVertexTU1>       VertexTexBuffer;
#else
    typedef VertexBuffer<SimpleVertex,RenderDevice::ESimpleVertex/*,20000*/>    SimpleVertexBuffer;
# ifndef TRANSGAMING
    typedef VertexBuffer<WORD,RenderDevice::EIndex16/*,30000*/>                 SimpleIndexBuffer;
# endif
#endif

public:
    int ParticlesDetail; // 0-off, 1-low , 2-full

    ParticleSystem();
    ~ParticleSystem();

    BOOL Init();
	void RenderCleanup();
    BOOL LoadEmitter(ParticleEmitter& emitter, const char *fileName);
    BOOL SaveEmitter(const ParticleEmitter& emitter, const char *fileName);

	void Clear(void);

#ifndef TRANSGAMING
	SimpleVertexBuffer&	GetVBuf() { return VBuf; }
	WORD				GetEBuf() { return EBuf; }
#endif
	ParticleEmitter*	CreateEmitter(const char* fileName=0, bool reloadMaster=false);
    ParticleEmitter*    GetMasterEmitter(const char* fileName);
#ifdef TRANSGAMING_USE_EDITOR
    void                UpdateAllEmittersWithMaster(const ParticleEmitter* master);
#endif

    void SetState();
    void ResetState();
#ifdef	TRANSGAMING
	void RenderSprites(const Camera& camera,const Sprite_TG& sprite,bool dontFacing=false);
#else
	void RenderSprites(const Camera& camera,const Sprite& sprite,bool dontFacing=false);
#endif	
	void RenderSprites(const Camera& camera,const Sprite1DOF& sprite);
    void Save( FileObject* f );
    void Load( FileObject* f );

private:
    ParticlePool		Pool;
	ParticleEmitter*	EmitterList;
#ifdef TRANSGAMING_PS2
    VertexPosBuffer     PBuf;
    VertexColorBuffer   CBuf;
    VertexTexBuffer     TBuf;
#else
    SimpleVertexBuffer  VBuf;
# ifndef TRANSGAMING_PSP
	int					EBuf;	// static
	WORD				Elems[MAX_BUFFERED_PARTICLES*6];
# endif
#endif

	typedef DynamicArray<ParticleEmitter*>	EmitterArray;
	struct MasterEmitter
	{
		ParticleEmitter*	Master;
		EmitterArray		Array;
        MasterEmitter() { Master = NULL; }
        ~MasterEmitter() { if (Master) delete Master; }        
	};
	HashMap<String,MasterEmitter>	MasterEmitterHash;
	
	friend class ParticleEffect;
	friend class ParticleEmitter;
	friend class ParticleRender;
    friend class Billboard;
    friend class Trail;

};



#endif //_PARTICLESYSTEM_

