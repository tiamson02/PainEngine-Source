// -----------------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
// $Header: /Game/Source/Engine/Render/ParticleSystem.cpp 199   10/01/04 6:21p Bart $
//-------------------------------------------------------------------------------------

#include "Engine.h"
#include "Core/FileSystem/ConfigFile.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Render/ParticleEffect.h"
#include "Render/View.h"
#include "Core/List.h"
#include "Engine/InputSystem.h"
#include "Render/ParticleSystem.h"
#include "Render/Video.h"
#include "Render/Material.h"
#include "Engine/World.h"

//============================================================================
// tool stuff
//============================================================================
static const char* FloatToString(const float x)
{
    static char tmp[32];
    snprintf(tmp, 32, "%.2f",x);
    return tmp;
}
static const char* IntToString(const int x)
{
    static char tmp[32];
    snprintf(tmp, 32, "%d",x);
    return tmp;
}
//============================================================================
// ParticlePool
//============================================================================
ParticlePool::ParticlePool()
{
	Count = 0;
    int i;
	for(i=0; i<MaxParticleCount-1; ++i) {
		Particles[i].NextFree=&Particles[i+1];
	}
	Particles[i].NextFree=0;
	FreeList=&Particles[0];
}
//============================================================================
Particle* ParticlePool::Alloc()
{
#ifdef TRANSGAMING
    //no more particles => fail
    if (FreeList == NULL)
        return NULL;
#endif
	Particle* particle=FreeList;
	FreeList=particle->NextFree;
	particle->Next=particle->Prev=0;
	return particle;
}
//============================================================================
void ParticlePool::Free(Particle* particle)
{
	particle->NextFree=FreeList;
	FreeList=particle;
}
//============================================================================
// ParticleEmitter
//============================================================================
ParticleEmitter::ParticleEmitter()
:	Pool(0), ListFront(0), ListBack(0)
,	Next(0), Prev(0)
,   TexturePtr(0)
,	WarpTexPtr(0)
,	MaterialPtr(0)
,	Active(TRUE)
,   Master(NULL)
,   Effect(NULL)
{    
	MaxParticles=128;
    ActiveParticleCount=0;
	SpawnedParticleCount=0;

	Position	= Vector(0,0,0);
	PosRange	= VectorRange(Vector(-1,-1,-1),Vector(1,1,1));
	VelStart	= VectorRange(Vector(-80,-6,0),Vector(-64,6,0));
	VelEnd	= VelStart;

	StartSize	= FloatRange(2,4);
	EndSize		= FloatRange(8,12);
	LifeTime	= FloatRange(.5f,.8f);
	Immortal = 0;
	Alpha		= FloatRange(0.8f,0.0f);
	RotRange	= FloatRange(-1,+1);

	AccelRange	= VectorRange(Vector(0,-0.1f,0),Vector(0,-0.1f,0));
	Respawn		= FALSE;
    Evolve		= TRUE;
	Dying		= FALSE;
	Warp		= FALSE;

    Thickness	= FloatRange(0.3f,0.5f);
    Length		= FloatRange(0.3f,0.5f);

	ColorRange	= VectorRange(Vector(0.5f,0.1f,0),Vector(1,0.5f,0));
	LerpColor	= 1;

	SpawnRate	= 1.0f/100.0f;
	SpawnTime	= SpawnRate;

    Type		= SpriteEmitter;
    BlendMode	= 512;

    UseRandomNormal = 0;
	DepthTest	= 1;

	TexAnimFPS	= 10;	
    RotQuat		= IdentityQuaternion();
	KillDist	= 0.0F;
}
//============================================================================
ParticleEmitter& ParticleEmitter::operator=(const ParticleEmitter& emitter)
{
	ListFront = ListBack = 0;
	ActiveParticleCount = 0;
	SpawnedParticleCount=0;

	Dying = FALSE;

	MaxParticles		= emitter.MaxParticles;

    Active = emitter.Active;

	Name		= emitter.Name;

	TextureName = emitter.TextureName;
	TexturePtr	= emitter.TexturePtr;

	WarpTexName = emitter.WarpTexName;
	WarpTexPtr	= emitter.WarpTexPtr;

	MaterialName	= emitter.MaterialName;
	MaterialPtr		= emitter.MaterialPtr;

	Box.Reset();
	Pool		= emitter.Pool;
	Position	= emitter.Position;
	PosRange	= emitter.PosRange;
	VelStart	= emitter.VelStart;
	VelEnd		= emitter.VelEnd;

	StartSize	= emitter.StartSize;
	EndSize		= emitter.EndSize;
	LifeTime	= emitter.LifeTime;
	Immortal    = emitter.Immortal;
	Alpha		= emitter.Alpha;
	AlphaMid	= emitter.AlphaMid;
	FadeTime	= emitter.FadeTime;
	VelBlend	= emitter.VelBlend;
	RotRange	= emitter.RotRange;

	AccelRange	= emitter.AccelRange;
	Respawn		= emitter.Respawn;
    Evolve		= emitter.Evolve;
	Warp		= emitter.Warp;

    Thickness	= emitter.Thickness;
    Length		= emitter.Length;

	ColorRange	= emitter.ColorRange;
	LerpColor	= emitter.LerpColor;

	SpawnRate	= emitter.SpawnRate;
	SpawnTime	= SpawnRate;

    Type		= emitter.Type;
    BlendMode	= emitter.BlendMode;

    UseRandomNormal	= emitter.UseRandomNormal;
	DepthTest = emitter.DepthTest;

    RotQuat	= emitter.RotQuat;
	TexAnimFPS = emitter.TexAnimFPS;
	KillDist = emitter.KillDist;

	return (*this);
}
//============================================================================
void ParticleEmitter::Restart()
{
    ActiveParticleCount=0;
	SpawnedParticleCount=0;
	SpawnTime = SpawnRate;

	Active=1;
	Dying = FALSE;

    for (Particle* particle = ListFront; particle != NULL; particle=particle->Next) 
	{
		if (particle == ListFront)
			ListFront = ListFront->Next;
		else
			particle->Prev->Next = particle->Next;

		if (particle == ListBack)
			ListBack = ListBack->Prev;
		else
			particle->Next->Prev = particle->Prev;

		Pool->Free(particle);
		Pool->Count--;
    }

	ListFront = 0;
	ListBack = 0;
}
//============================================================================
void ParticleEmitter::Deactivate(BOOL instantKill)
{
    if (instantKill) {
	    Restart();
    	Active=0;
    } else {

        Immortal = FALSE;
		Dying = TRUE;
        Evolve = FALSE;
    }
}
//============================================================================
void ParticleEmitter::InitParticle(Particle* particle)
{
#ifndef TRANSGAMING
	/*	
	// figure out velocity
    if(UseRandomNormal) {
        
        Vector rNormal=(Normal+VRand()*0.4f).GetNormal();
        if(rNormal.Dot(Normal)<0) {
            rNormal*=-0.5f;
        }

        //particle->Velocity=rNormal*10.f;
        particle->Velocity=VelRange.RandomValue()*rNormal;
    }
    //else {*/
//    }
#endif

	// init basic properties
	particle->VelRange = VectorRange(VelStart.RandomValue(),VelEnd.RandomValue());
	particle->VelRange.Min = RotQuat * particle->VelRange.Min;
	particle->VelRange.Max = RotQuat * particle->VelRange.Max;
	particle->Velocity	= 	particle->VelRange.Min;
	particle->Accel		= AccelRange.RandomValue();
	particle->AccelVal  = Vector(0,0,0);

	if (!LerpColor)
		particle->Color = ColorRange.RandomValue();

	particle->StartRot = RotRange.RandomValue();
    particle->Rotation = 0; // fmod(rand(),F_PI*2);

	particle->LifeTime  = LifeTime.RandomValue();
	particle->Time      = 0;
	particle->AnimTime  = 0;
	particle->SizeRange = FloatRange(StartSize.RandomValue(),EndSize.RandomValue());
	particle->TexFrame  = 0;
	

    if(Type==SparkEmitter) {
        particle->Thickness = Thickness.RandomValue();
        particle->Length    = Length.RandomValue();
    }
}
//============================================================================
void ParticleEmitter::Tick(float deltaTime)
{
	Box.Reset();

	int spawnCount=0;
    int maxParticles = MaxParticles;
    SpawnTime += deltaTime;

    if (GEngine->Video->ParticleSys.ParticlesDetail == 1 && maxParticles > 1 )
        maxParticles /= 2;

    if(GEngine->Video->ParticleSys.ParticlesDetail != 0 && SpawnTime>=SpawnRate) 
	{
		spawnCount = int ((SpawnTime / SpawnRate) + 0.5f);
		
        if (ActiveParticleCount + spawnCount >= maxParticles)
            spawnCount = maxParticles - ActiveParticleCount;

		if (Pool->Count + spawnCount >= ParticlePool::MaxParticleCount)
			spawnCount = ParticlePool::MaxParticleCount - Pool->Count;
        		
		SpawnTime = SpawnTime-(SpawnRate*spawnCount);
	}

	//+DDP - we can't spawn anything, so DIE. This stops bug were high SpawnTime emmiters get left in memory trying to spawn
    if (!Evolve && SpawnedParticleCount >= maxParticles)
		Dying = true;

    if (Dying)
		spawnCount = 0;

#ifdef TRANSGAMING_HACK
    if (spawnCount + ActiveParticleCount > MAX_BUFFERED_PARTICLES)
        spawnCount = MAX_BUFFERED_PARTICLES - ActiveParticleCount;
#endif

	for(int i=0; i<spawnCount; ++i) 
    {
		Particle* particle=Pool->Alloc();
        if (particle) 
        {
            //TRACE("ParticleEmitter::Tick(): allocated a new particle (name = '%s', count = %d, activeCount = %d)\n", Effect->Name.AsChar(), Pool->Count + 1, ActiveParticleCount);
		    // insert at back
		    particle->Prev = ListBack;
		    particle->Next = NULL;
		    if (ListFront == NULL) /* list was empty */
			    ListFront = particle;
		    else /* list was nonempty */
			    ListBack->Next = particle;

    		ListBack = particle;
			Vector StartPos = LastPosition + (Position-LastPosition) * (float)((float)(i+1)/(float)spawnCount);
			particle->Position	= RotQuat * PosRange.RandomValue() + StartPos;
    		InitParticle(particle);			
			Pool->Count++; 
            ActiveParticleCount++;
			SpawnedParticleCount++;
			
			particle->StartDelta = (deltaTime/(float)spawnCount) * i;
        }

        else{
            TRACE("ParticleEmitter::Tick(): no mo' buwitts! (name = '%s', count = %d, activeCount = %d)\n", Effect->Name.AsChar(), Pool->Count, ActiveParticleCount);
            break;
        }
	}

	// Warping
	Vector max = Position+PosRange.Max;
	Vector min = Position+PosRange.Min;
	Vector rng = PosRange.Max-PosRange.Min;

	//for(Particle* prev=0, *particle=List; particle; prev=particle, particle=particle->Next) {
    for (Particle* particle = ListFront; particle != NULL; particle=particle->Next) 
	{
		float delta = particle->StartDelta > -1 ? particle->StartDelta : deltaTime;

		particle->Time+=delta;
		if (particle->Time > particle->LifeTime) 
		{
            if (Immortal && !Dying && GEngine->Video->ParticleSys.ParticlesDetail != 0)
				particle->Time = 0;
			else
			{
				// take it off the list
				if (particle == ListFront)
					ListFront = ListFront->Next;
				else
					particle->Prev->Next = particle->Next;

				if (particle == ListBack)
					ListBack = ListBack->Prev;
				else
					particle->Next->Prev = particle->Prev;

				// free it to the pool
				Pool->Free(particle);
				Pool->Count--;
                //TRACE("ParticleEmitter::Tick(): freed a particle (name = '%s', count = %d, activeCount = %d)\n", Effect->Name.AsChar(), Pool->Count, ActiveParticleCount);
				ActiveParticleCount--;
				continue;
			}
		}
		
		const float t=particle->Time/particle->LifeTime;
		float life_percent = t*100.0f;
		
		// velocity blending
		if (life_percent < VelBlend.Min)
		{
			particle->Velocity = particle->VelRange.Min;
		}
		else if (life_percent < VelBlend.Max)
		{
			float lentime = 0.01f * (VelBlend.Max -VelBlend.Min) * particle->LifeTime;
			float start = particle->LifeTime * VelBlend.Min * 0.01f;
			float tt = (particle->Time-start)/lentime;
			particle->Velocity = particle->VelRange.Lerp(tt);
		}
		else
			particle->Velocity = particle->VelRange.Max;

		// acceleration
		particle->AccelVal += (particle->Accel*delta);
		particle->Velocity +=  particle->AccelVal;				

		// alpha blend
		if (life_percent < FadeTime.Min)
		{
			float tt =  particle->Time / (particle->LifeTime * FadeTime.Min * 0.01f);
			particle->Alpha =  Alpha.Min + (AlphaMid-Alpha.Min) * tt;
		}
		else if (life_percent < FadeTime.Max)
		{
			particle->Alpha =  AlphaMid;
		}
		else
		{
			float start = particle->LifeTime * FadeTime.Max * 0.01f;
			float tt = (particle->Time-start)/(particle->LifeTime-start);
			particle->Alpha =  AlphaMid + (Alpha.Max-AlphaMid) * tt;		
			///GScript.DoString("Game:Print('%f')",particle->Alpha);
		}

		particle->Position+=particle->Velocity*delta;

		if (Immortal)
			particle->Position = Effect->GetPosition();

		if (Warp) {
			Vector &p = particle->Position;

			if (p.X > max.X)		p.X -= rng.X;
			else if (p.X < min.X)	p.X += rng.X;
			if (p.Y > max.Y)		p.Y -= rng.Y;
			else if (p.Y < min.Y)	p.Y += rng.Y;
			if (p.Z > max.Z)		p.Z -= rng.Z;
			else if (p.Z < min.Z)	p.Z += rng.Z;
		}

		particle->Size		= particle->SizeRange.Lerp(t);


		if (particle->StartRot)
			particle->Rotation	+= particle->StartRot*delta;

		if (LerpColor)
			particle->Color=ColorRange.Lerp(t);

        if (particle->Alpha > 0.001f) 
		{
			const Vector extent(particle->Size);
			Box += particle->Position + extent;
			Box += particle->Position - extent;
        }

		if (TexturePtr && TexturePtr->NumFrames) 
		{
			particle->AnimTime += delta;
            particle->TexFrame = (int)(particle->AnimTime * TexAnimFPS) % TexturePtr->NumFrames;
		}

		particle->StartDelta = -2;
	}
	
    if (ActiveParticleCount <= 0 && (Dying || (!Evolve && SpawnedParticleCount >= maxParticles)))
		Active = FALSE;
}
//============================================================================
void ParticleEmitter::SetScale(const float scale)
{   
	// to do
	// te parametry ponizej (oraz inne) wywalic z particleemittera i przeniesc do
	// stuct MasterEmitter - a scale uwzgdledniac przy inicjacji i ticku czasteczki
	if (Master)
	{
		PosRange.Min = Master->PosRange.Min * scale;
		PosRange.Max = Master->PosRange.Max * scale;

		VelStart.Min = Master->VelStart.Min * scale;
		VelStart.Max = Master->VelStart.Max * scale;
		VelEnd.Min = Master->VelEnd.Min * scale;
		VelEnd.Max = Master->VelEnd.Max * scale;

		AccelRange.Min = Master->AccelRange.Min * scale;
		AccelRange.Max = Master->AccelRange.Max * scale;

		StartSize.Min = Master->StartSize.Min * scale;
		StartSize.Max = Master->StartSize.Max * scale;
		EndSize.Min = Master->EndSize.Min * scale;
		EndSize.Max = Master->EndSize.Max * scale;

		Thickness.Min = Master->Thickness.Min * scale;
		Thickness.Max = Master->Thickness.Max * scale;
		
		Length.Min = Master->Length.Min * scale;
		Length.Max = Master->Length.Max * scale;    
	}
}
//============================================================================
ParticleEmitter* ParticleSystem::CreateEmitter(const char* fileName, bool reloadMaster)
{
#ifdef TRANSGAMING_PSP
    TRACE("ParticleSystem::CreateEmitter(): attempting to create an emitter from '%s' (reloadMaster = %d)\n", fileName, reloadMaster);
#endif

	MasterEmitter* p = MasterEmitterHash.Find(fileName);
	if (!p) {

		MasterEmitter& master = MasterEmitterHash[fileName];
		p = &master;

        m_setTag("Particle");
		p->Master = new ParticleEmitter();
		p->Master->Pool = &Pool;
		p->Master->Name = fileName;

		if (fileName) 
			reloadMaster = true;
	}
	
	if (reloadMaster)
	{
        char buf[MAX_PATH];
		const char *texPath = "Particles/";


        snprintf(buf, MAX_PATH, "../Data/Scripts/Emitters/%s", fileName);
		LoadEmitter(*p->Master, buf);

		//texPath += p->Master->TextureName;
		MaterialSystem* MatSys = GEngine->Video->MaterialSys; 

		//p->Master->TexturePtr	= MatSys->CreateTexture(texPath,Texture::ETexNoLOD);
        snprintf(buf, MAX_PATH, "%s%s", texPath, p->Master->TextureName.AsChar());
		p->Master->TexturePtr	= MatSys->CreateTexture(buf, Texture::ETexNoLOD);
        
        snprintf(buf, MAX_PATH, "%s%s", texPath, p->Master->WarpTexName.AsChar());
		p->Master->WarpTexPtr	= p->Master->WarpTexName.Size() ? MatSys->CreateTexture(buf, Texture::ETexNoLOD) : NULL;
		p->Master->MaterialPtr	= p->Master->MaterialName.Size() ? MatSys->FindMaterial(p->Master->MaterialName.AsChar()) : NULL;
	}

	if (p) {

		// master already loaded
		EmitterArray &      array = p->Array;
        ParticleEmitter *   emitter;


		// try to find inactive emitter and reuse it
		for (int i=0; i<array.Size(); ++i) {
            emitter = array[i];

			if (!emitter->IsActive() && !emitter->Effect) {
				// copy from master
				(*emitter) = (*p->Master);
				return emitter;
			}
		}

        m_setTag("Particle");
		ParticleEmitter* e = new ParticleEmitter();
		array.Add(e);
		e->Pool = &Pool;
		e->Master = p->Master;

		(*e) = (*p->Master);
		e->Effect = NULL;

		ListInsert(EmitterList,e);
		return e;
	}

	return 0;
}

//============================================================================
// ParticleSystem
//============================================================================
ParticleSystem::ParticleSystem()
:   EmitterList(0), ParticlesDetail(2)
#ifndef TRANSGAMING
,	EBuf(EInvalidIndex)
#endif
{
}
//============================================================================
ParticleSystem::~ParticleSystem()
{
	for (ParticleEmitter* emitter = EmitterList; emitter; )
    {
        ParticleEmitter* p = emitter;
        emitter=emitter->Next;
        delete p;
    }
}
//============================================================================
BOOL ParticleSystem::Init()
{
    RenderDevice* dev=GEngine->GetRenderDevice();
    Assert(dev);

#ifdef TRANSGAMING_PS2
# ifdef DEBUG
    if (!PBuf.Create(dev, MaxVertexCount, true, __FILE__, __LINE__)){
# else
    if (!PBuf.Create(dev, MaxVertexCount, true)){
# endif
        TRACE("ParticleSystem::Init(): ERROR-> could not allocate a vertex buffer to hold %d positions\n", MaxVertexCount);

        return FALSE;
    }

# ifdef DEBUG
    if (!CBuf.Create(dev, MaxVertexCount, true, __FILE__, __LINE__)){
# else
    if (!CBuf.Create(dev, MaxVertexCount, true)){
# endif
        TRACE("ParticleSystem::Init(): ERROR-> could not allocate a vertex buffer to hold %d colors\n", MaxVertexCount);
        PBuf.Release();

        return FALSE;
    }

# ifdef DEBUG
    if (!TBuf.Create(dev, MaxVertexCount, true, __FILE__, __LINE__)){
# else
    if (!TBuf.Create(dev, MaxVertexCount, true)){
# endif
        TRACE("ParticleSystem::Init(): ERROR-> could not allocate a vertex buffer to hold %d texture coordinate sets\n", MaxVertexCount);
        PBuf.Release();
        CBuf.Release();

        return FALSE;
    }
#else
    if (!VBuf.Create(dev, 5000))    //20000)) 
	{
        GLog.Print("ParticleSystem: Failed to create vertex buffer\n");
        return FALSE;
    }


# ifndef TRANSGAMING_PSP
	WORD* E = &Elems[0];
	WORD CurPos = 0;

	for (int i=0; i<MAX_BUFFERED_PARTICLES; ++i, CurPos+=4, E+=6)
	{
		E[0] = CurPos + 0;
		E[1] = CurPos + 1;
		E[2] = CurPos + 2;
		E[3] = CurPos + 0;
		E[4] = CurPos + 2;
		E[5] = CurPos + 3;
	}

    EBuf = dev->CreateStream
    (
        &Elems[0],
        RenderDevice::EIndex16,
        ARRAY_COUNT(Elems),
        sizeof(WORD)
    );
	if (EBuf == EInvalidIndex)
	{
        GLog.Print("ParticleSystem: Failed to create index buffer\n");
        return FALSE;
	}


	/*
	if (!EBuf.Create(dev))
	{
        GLog.Print("ParticleSystem: Failed to create index buffer\n");
        return FALSE;
	}
	*/
# endif
#endif

    return TRUE;
}

void ParticleSystem::RenderCleanup()
{
#ifdef TRANSGAMING_PS2
    PBuf.Release();
    CBuf.Release();
    TBuf.Release();
#else
	VBuf.Release();
# ifndef TRANSGAMING_PSP
    RenderDevice* dev=GEngine->GetRenderDevice();
    const int ids[] = { EBuf };
    dev->ReleaseResource(RenderDevice::EResBuffer,ids,ARRAY_COUNT(ids));
# endif
#endif
}

//============================================================================
BOOL ParticleSystem::LoadEmitter(ParticleEmitter& emitter, const char *fileName)
{
    ConfigFile cfg;
    if(cfg.Load(fileName)) {

        float scale=1;
        int useColorRange=0;

		//emitter.TextureName="";
		//emitter.MaterialName="";
		//emitter.WarpTexName="";
		emitter.TextureName.Clear();
		emitter.MaterialName.Clear();
		emitter.WarpTexName.Clear();

        const char* s,*section;
        section="General";
        s=cfg.GetString(section,"Texture");         if(s) emitter.TextureName=s;
        s=cfg.GetString(section,"Material");		 if(s) emitter.MaterialName=s;
        s=cfg.GetString(section,"WarpTex");			 if(s) emitter.WarpTexName=s;
		s=cfg.GetString(section,"TexAnimFPS");      if(s) emitter.TexAnimFPS=atoi(s);
        s=cfg.GetString(section,"Type");            if(s) emitter.Type=atoi(s);
        s=cfg.GetString(section,"BlendMode");       if(s) emitter.BlendMode=atoi(s);
        //s=cfg.GetString(section,"UseColorRange");   if(s) emitter.UseColorRange=atoi(s);
        s=cfg.GetString(section,"UseColorRange");   if(s) useColorRange=atoi(s);
        s=cfg.GetString(section,"UseRandomNormal"); if(s) emitter.UseRandomNormal=atoi(s);
        s=cfg.GetString(section,"DepthTest");       if(s) emitter.DepthTest=atoi(s);
        s=cfg.GetString(section,"Evolve");          if(s) emitter.Evolve=atoi(s);
        s=cfg.GetString(section,"Warp");			if(s) emitter.Warp=atoi(s);
        s=cfg.GetString(section,"MaxParticles");    if(s) emitter.MaxParticles=atoi(s);
        s=cfg.GetString(section,"SpawnRate");       if(s) emitter.SpawnRate=float(1.0f/atof(s));
        s=cfg.GetString(section,"Scale");           if(s) scale=float(atof(s));		
        s=cfg.GetString(section,"KillDist");        if(s) emitter.KillDist=float(atof(s));

		// we store dist squared to avoid sqrtf() later on ...
		emitter.KillDist = emitter.KillDist * emitter.KillDist;

		// ugly hack -- use default
		if ((emitter.MaterialName == "particle_warp") && !emitter.WarpTexName.Size())
			emitter.WarpTexName = "warp";


        // fix blend mode
        switch (emitter.BlendMode)
        {
        case 64:    emitter.BlendMode=RenderState::EBlendFilter; break;
        case 128:   emitter.BlendMode=RenderState::EBlendAdd; break;
        case 256:   emitter.BlendMode=RenderState::EBlendTranslucent; break;
        case 512:   emitter.BlendMode=RenderState::EBlendAlpha; break;
        }

        section="EditorPosition";
        s=cfg.GetString(section,"Pos.X"); if(s) emitter.Position.X=float(atof(s));
        s=cfg.GetString(section,"Pos.Y"); if(s) emitter.Position.Y=float(atof(s));
        s=cfg.GetString(section,"Pos.Z"); if(s) emitter.Position.Z=float(atof(s));

        section="PosRange";
        s=cfg.GetString(section,"Min.X"); if(s) emitter.PosRange.Min.X=float(atof(s));
        s=cfg.GetString(section,"Min.Y"); if(s) emitter.PosRange.Min.Y=float(atof(s));
        s=cfg.GetString(section,"Min.Z"); if(s) emitter.PosRange.Min.Z=float(atof(s));

        s=cfg.GetString(section,"Max.X"); if(s) emitter.PosRange.Max.X=float(atof(s));
        s=cfg.GetString(section,"Max.Y"); if(s) emitter.PosRange.Max.Y=float(atof(s));
        s=cfg.GetString(section,"Max.Z"); if(s) emitter.PosRange.Max.Z=float(atof(s));

        section="Velocity";
        s=cfg.GetString(section,"Min.X"); if(s) emitter.VelStart.Min.X=float(atof(s));
        s=cfg.GetString(section,"Min.Y"); if(s) emitter.VelStart.Min.Y=float(atof(s));
        s=cfg.GetString(section,"Min.Z"); if(s) emitter.VelStart.Min.Z=float(atof(s));

        s=cfg.GetString(section,"Max.X"); if(s) emitter.VelStart.Max.X=float(atof(s));
        s=cfg.GetString(section,"Max.Y"); if(s) emitter.VelStart.Max.Y=float(atof(s));
        s=cfg.GetString(section,"Max.Z"); if(s) emitter.VelStart.Max.Z=float(atof(s));

		// compatibility
        s=cfg.GetString(section,"Acceleration.X"); if(s) emitter.AccelRange.Min.X=emitter.AccelRange.Max.X=float(atof(s));
        s=cfg.GetString(section,"Acceleration.Y"); if(s) emitter.AccelRange.Min.Y=emitter.AccelRange.Max.Y=float(atof(s));
        s=cfg.GetString(section,"Acceleration.Z"); if(s) emitter.AccelRange.Min.Z=emitter.AccelRange.Max.Z=float(atof(s));

        s=cfg.GetString(section,"AccelMax.X"); if(s) emitter.AccelRange.Max.X=float(atof(s));
        s=cfg.GetString(section,"AccelMax.Y"); if(s) emitter.AccelRange.Max.Y=float(atof(s));
        s=cfg.GetString(section,"AccelMax.Z"); if(s) emitter.AccelRange.Max.Z=float(atof(s));


		section="VelocityEnd";
		s=cfg.GetString(section,"Min.X"); 
		if(s)
			emitter.VelEnd.Min.X=float(atof(s));
		else
			emitter.VelEnd.Min.X=emitter.VelStart.Min.X;

		s=cfg.GetString(section,"Min.Y"); 
		if(s) 
			emitter.VelEnd.Min.Y=float(atof(s));
		else
			emitter.VelEnd.Min.Y=emitter.VelStart.Min.Y;

		s=cfg.GetString(section,"Min.Z");
		if(s) 
			emitter.VelEnd.Min.Z=float(atof(s));
		else
			emitter.VelEnd.Min.Z=emitter.VelStart.Min.Z;

		s=cfg.GetString(section,"Max.X"); 
		if(s) 
			emitter.VelEnd.Max.X=float(atof(s));
		else
			emitter.VelEnd.Max.X=emitter.VelStart.Max.X;

		s=cfg.GetString(section,"Max.Y"); 
		if(s) 
			emitter.VelEnd.Max.Y=float(atof(s));
		else
			emitter.VelEnd.Max.Y=emitter.VelStart.Max.Y;

		s=cfg.GetString(section,"Max.Z"); 
		if(s) 
			emitter.VelEnd.Max.Z=float(atof(s));
		else
			emitter.VelEnd.Max.Z=emitter.VelStart.Max.Z;

        section="Color";
        s=cfg.GetString(section,"Min.X"); if(s) emitter.ColorRange.Min.X=float(atof(s));
        s=cfg.GetString(section,"Min.Y"); if(s) emitter.ColorRange.Min.Y=float(atof(s));
        s=cfg.GetString(section,"Min.Z"); if(s) emitter.ColorRange.Min.Z=float(atof(s));

        s=cfg.GetString(section,"Max.X"); if(s) emitter.ColorRange.Max.X=float(atof(s));
        s=cfg.GetString(section,"Max.Y"); if(s) emitter.ColorRange.Max.Y=float(atof(s));
        s=cfg.GetString(section,"Max.Z"); if(s) emitter.ColorRange.Max.Z=float(atof(s));

        //if (!useColorRange) {
        //}

        s=cfg.GetString(section,"AlphaMin"); if(s) emitter.Alpha.Min=float(atof(s));
        s=cfg.GetString(section,"AlphaMax"); if(s) emitter.Alpha.Max=float(atof(s));
		
		s=cfg.GetString(section,"AlphaMid");
		if(s) emitter.AlphaMid=float(atof(s)); else emitter.AlphaMid = emitter.Alpha.Max;

		s=cfg.GetString(section,"FadeTimeMin");
		if(s) emitter.FadeTime.Min=float(atof(s)); else emitter.FadeTime.Min = 100.0f;

		s=cfg.GetString(section,"FadeTimeMax");
		if(s) emitter.FadeTime.Max=float(atof(s)); else emitter.FadeTime.Max = 100.0f;

		s=cfg.GetString(section,"VelBlendMin");
		if(s) emitter.VelBlend.Min=float(atof(s)); else emitter.VelBlend.Min = 100.0f;

		s=cfg.GetString(section,"VelBlendMax");
		if(s) emitter.VelBlend.Max=float(atof(s)); else emitter.VelBlend.Max = 100.0f;

		emitter.RotRange.Min=emitter.RotRange.Max=0.0f;
        s=cfg.GetString(section,"RotationMin"); if(s) emitter.RotRange.Min=float(atof(s));
        s=cfg.GetString(section,"RotationMax"); if(s) emitter.RotRange.Max=float(atof(s));


        section="SizeLife";
        s=cfg.GetString(section,"StartSizeMin");    if(s) emitter.StartSize.Min=float(atof(s));
        s=cfg.GetString(section,"StartSizeMax");    if(s) emitter.StartSize.Max=float(atof(s));
        s=cfg.GetString(section,"EndSizeMin");      if(s) emitter.EndSize.Min=float(atof(s));
        s=cfg.GetString(section,"EndSizeMax");      if(s) emitter.EndSize.Max=float(atof(s));
        s=cfg.GetString(section,"LifeTimeMin");     if(s) emitter.LifeTime.Min=float(atof(s));
        s=cfg.GetString(section,"LifeTimeMax");     if(s) emitter.LifeTime.Max=float(atof(s));
		s=cfg.GetString(section,"Immortal");        if(s) emitter.Immortal=int(atoi(s));

        section="SparkEmitter";
        s=cfg.GetString(section,"ThicknessMin"); if(s) emitter.Thickness.Min=float(atof(s));
        s=cfg.GetString(section,"ThicknessMax"); if(s) emitter.Thickness.Max=float(atof(s));
        s=cfg.GetString(section,"LengthMin");    if(s) emitter.Length.Min=float(atof(s));
        s=cfg.GetString(section,"LengthMax");    if(s) emitter.Length.Max=float(atof(s));

        return TRUE;
    }

    return FALSE;
}
//============================================================================
BOOL ParticleSystem::SaveEmitter(const ParticleEmitter& emitter, const char *fileName)
{
    ConfigFile cfg;
    const char* section;
    
    section="General";
    cfg.AddOption(section,"Texture",emitter.TextureName.AsChar());
	if (emitter.MaterialName.Len() > 0)
		cfg.AddOption(section,"Material",emitter.MaterialName.AsChar());
	if (emitter.WarpTexName.Len() > 0)
		cfg.AddOption(section,"WarpTex",emitter.WarpTexName.AsChar());
    cfg.AddOption(section,"TexAnimFPS",IntToString((int)emitter.TexAnimFPS));
    cfg.AddOption(section,"Type",IntToString(emitter.Type));
    cfg.AddOption(section,"BlendMode",IntToString(emitter.BlendMode));
    cfg.AddOption(section,"UseColorRange",IntToString(emitter.LerpColor));
    cfg.AddOption(section,"DepthTest",IntToString(emitter.DepthTest));
    cfg.AddOption(section,"UseRandomNormal",IntToString(emitter.UseRandomNormal));
    cfg.AddOption(section,"Evolve",IntToString(emitter.Evolve));
    cfg.AddOption(section,"Warp",IntToString(emitter.Warp));
    cfg.AddOption(section,"MaxParticles",IntToString(emitter.MaxParticles));
    cfg.AddOption(section,"SpawnRate",FloatToString(1.0f/emitter.SpawnRate));
    cfg.AddOption(section,"KillDist",FloatToString(emitter.KillDist));

    section="EditorPosition";
    cfg.AddOption(section,"Pos.X",FloatToString(emitter.Position.X));
    cfg.AddOption(section,"Pos.Y",FloatToString(emitter.Position.Y));
    cfg.AddOption(section,"Pos.Z",FloatToString(emitter.Position.Z));

    section="PosRange";
    cfg.AddOption(section,"Min.X",FloatToString(emitter.PosRange.Min.X));
    cfg.AddOption(section,"Min.Y",FloatToString(emitter.PosRange.Min.Y));
    cfg.AddOption(section,"Min.Z",FloatToString(emitter.PosRange.Min.Z));

    cfg.AddOption(section,"Max.X",FloatToString(emitter.PosRange.Max.X));
    cfg.AddOption(section,"Max.Y",FloatToString(emitter.PosRange.Max.Y));
    cfg.AddOption(section,"Max.Z",FloatToString(emitter.PosRange.Max.Z));

    section="Velocity";
    cfg.AddOption(section,"Min.X",FloatToString(emitter.VelStart.Min.X));
    cfg.AddOption(section,"Min.Y",FloatToString(emitter.VelStart.Min.Y));
    cfg.AddOption(section,"Min.Z",FloatToString(emitter.VelStart.Min.Z));

    cfg.AddOption(section,"Max.X",FloatToString(emitter.VelStart.Max.X));
    cfg.AddOption(section,"Max.Y",FloatToString(emitter.VelStart.Max.Y));
    cfg.AddOption(section,"Max.Z",FloatToString(emitter.VelStart.Max.Z));

	cfg.AddOption(section,"Acceleration.X",FloatToString(emitter.AccelRange.Min.X));
	cfg.AddOption(section,"Acceleration.Y",FloatToString(emitter.AccelRange.Min.Y));
	cfg.AddOption(section,"Acceleration.Z",FloatToString(emitter.AccelRange.Min.Z));

	cfg.AddOption(section,"AccelMax.X",FloatToString(emitter.AccelRange.Max.X));
	cfg.AddOption(section,"AccelMax.Y",FloatToString(emitter.AccelRange.Max.Y));
	cfg.AddOption(section,"AccelMax.Z",FloatToString(emitter.AccelRange.Max.Z));

	section="VelocityEnd";
	cfg.AddOption(section,"Min.X",FloatToString(emitter.VelEnd.Min.X));
	cfg.AddOption(section,"Min.Y",FloatToString(emitter.VelEnd.Min.Y));
	cfg.AddOption(section,"Min.Z",FloatToString(emitter.VelEnd.Min.Z));

	cfg.AddOption(section,"Max.X",FloatToString(emitter.VelEnd.Max.X));
	cfg.AddOption(section,"Max.Y",FloatToString(emitter.VelEnd.Max.Y));
	cfg.AddOption(section,"Max.Z",FloatToString(emitter.VelEnd.Max.Z));

    section="Color";
    cfg.AddOption(section,"Min.X",FloatToString(emitter.ColorRange.Min.X));
    cfg.AddOption(section,"Min.Y",FloatToString(emitter.ColorRange.Min.Y));
    cfg.AddOption(section,"Min.Z",FloatToString(emitter.ColorRange.Min.Z));

    cfg.AddOption(section,"Max.X",FloatToString(emitter.ColorRange.Max.X));
    cfg.AddOption(section,"Max.Y",FloatToString(emitter.ColorRange.Max.Y));
    cfg.AddOption(section,"Max.Z",FloatToString(emitter.ColorRange.Max.Z));

    cfg.AddOption(section,"AlphaMin",FloatToString(emitter.Alpha.Min));
    cfg.AddOption(section,"AlphaMax",FloatToString(emitter.Alpha.Max));
	cfg.AddOption(section,"AlphaMid",FloatToString(emitter.AlphaMid));
	
	cfg.AddOption(section,"FadeTimeMin",FloatToString(emitter.FadeTime.Min));
	cfg.AddOption(section,"FadeTimeMax",FloatToString(emitter.FadeTime.Max));

	cfg.AddOption(section,"VelBlendMin",FloatToString(emitter.VelBlend.Min));
	cfg.AddOption(section,"VelBlendMax",FloatToString(emitter.VelBlend.Max));

    cfg.AddOption(section,"RotationMin",FloatToString(emitter.RotRange.Min));
    cfg.AddOption(section,"RotationMax",FloatToString(emitter.RotRange.Max));

    section="SizeLife";
    cfg.AddOption(section,"StartSizeMin",FloatToString(emitter.StartSize.Min));
    cfg.AddOption(section,"StartSizeMax",FloatToString(emitter.StartSize.Max));
    cfg.AddOption(section,"EndSizeMin",FloatToString(emitter.EndSize.Min));
    cfg.AddOption(section,"EndSizeMax",FloatToString(emitter.EndSize.Max));
    cfg.AddOption(section,"LifeTimeMin",FloatToString(emitter.LifeTime.Min));
    cfg.AddOption(section,"LifeTimeMax",FloatToString(emitter.LifeTime.Max));
	cfg.AddOption(section,"Immortal",IntToString(emitter.Immortal));

    section="SparkEmitter";
    cfg.AddOption(section,"ThicknessMin",FloatToString(emitter.Thickness.Min));
    cfg.AddOption(section,"ThicknessMax",FloatToString(emitter.Thickness.Max));
    cfg.AddOption(section,"LengthMin",FloatToString(emitter.Length.Min));
    cfg.AddOption(section,"LengthMax",FloatToString(emitter.Length.Max));

    if(cfg.Save(fileName)) {
        GLog.Print("Saved emitter: %s\n", fileName);
        return TRUE;
    }
    
    return FALSE;
}
//============================================================================
ParticleEmitter* ParticleSystem::GetMasterEmitter(const char* fileName)
{
	if (fileName) {
		MasterEmitter* p = MasterEmitterHash.Find(fileName);
		if (p)
			return p->Master;
	}

	return 0;
}

void ParticleSystem::Clear(void)
{
	for (int i=0; i<MasterEmitterHash.Size(); ++i)
	{
		MasterEmitter	*p		= &MasterEmitterHash.GetPair(i).Value;
		EmitterArray	&array	= p->Array;

		for (int j=0; j<array.Size(); j++)
		{
			ParticleEmitter* e = array[j];
			delete e;
			array[j] = NULL;
		}
		//delete p;
	}

	MasterEmitterHash.Clear();
}

//============================================================================
#ifdef TRANSGAMING_USE_EDITOR
void ParticleSystem::UpdateAllEmittersWithMaster(const ParticleEmitter* master)
{
    if (master) {
	    MasterEmitter* p = MasterEmitterHash.Find(master->Name);
        if (p) {

		    EmitterArray& array = p->Array;
		    for (int i=0; i<array.Size(); ++i) {
                
				ParticleEmitter* e = array[i];
				if (e->Effect)
				{
					ParticleEffect *pe = e->Effect;
					if (master->MaterialPtr && master->MaterialPtr->Flags & EMF_WarpScreen)
					{
						pe->WarpScreen = true;
						pe->Sort = Entity::ESortWarp;
					}
					else
					{
						pe->WarpScreen = false;
						pe->Sort = Entity::ESortTrans;
					}
				}		

	            e->MaxParticles	= master->MaxParticles;
				e->SpawnRate	= master->SpawnRate;
				e->SpawnTime = master->SpawnRate;

				e->PosRange	= master->PosRange;
	            e->VelStart	= master->VelStart;
				e->VelEnd	= master->VelEnd;

	            e->StartSize = master->StartSize;
	            e->EndSize = master->EndSize;
	            e->LifeTime	= master->LifeTime;
				e->Immortal	= master->Immortal;
	            e->Alpha = master->Alpha;
				e->AlphaMid = master->AlphaMid;
				e->FadeTime = master->FadeTime;
				e->VelBlend = master->VelBlend;
	            e->RotRange	= master->RotRange;

	            e->AccelRange = master->AccelRange;
	            e->Respawn = master->Respawn;
                e->Evolve = master->Evolve;
				e->Warp = master->Warp;

                e->Thickness = master->Thickness;
                e->Length = master->Length;

	            e->ColorRange = master->ColorRange;
	            e->LerpColor = master->LerpColor;

                e->Type = master->Type;
                e->BlendMode = master->BlendMode;
				e->DepthTest = master->DepthTest;
			
                e->UseRandomNormal = master->UseRandomNormal;
	            e->DepthTest = master->DepthTest;

                e->RotQuat = master->RotQuat;
	            e->TexAnimFPS = master->TexAnimFPS;

                e->TextureName = master->TextureName;
                e->TexturePtr = master->TexturePtr;

                e->WarpTexName = master->WarpTexName;
                e->WarpTexPtr = master->WarpTexPtr;

				e->MaterialName = master->MaterialName;
				e->MaterialPtr = master->MaterialPtr;

				e->KillDist = master->KillDist;

				// XXX temporary shit - to restore emitterdef settings
				if (e->Effect)
					e->Effect->SetupEmitters();
            }
        }
    }
}
#endif
//============================================================================
void ParticleSystem::SetState()
{
    RenderDevice* dev=GEngine->GetRenderDevice();
    Assert(dev);

    TextureState st[2];



#ifdef TRANSGAMING_PS2
    st[0].Set(0,TextureState::ETexOpModulate,TextureState::ETexArgColor,TextureState::ETexFilterBilinear);
	st[0].AlphaOp = TextureState::ETexOpModulate;
	st[0].AlphaArg = TextureState::ETexArgDiffuse;

	st[1].Stage = 1;
    st[1].Op = TextureState::ETexOpDisable;
	st[1].AlphaOp = TextureState::ETexOpDisable;

	dev->SetTextureState(st,ARRAY_COUNT(st));


    const int s[] = { PBuf.GetStream(), CBuf.GetStream(), TBuf.GetStream() };
    dev->SetStreams(s, ARRAY_COUNT(s), EInvalidIndex);

    dev->SetShader(GEngine->Video->MaterialSys->Shaders.Fixed_DCTU1, RenderDevice::EShaderVertex);
    dev->SetShader(EInvalidIndex, RenderDevice::EShaderPixel);

    dev->SetFogState(GEngine->pcfWorld->Env.FogParams); // ugly hack: switch to fp'ed fog
#else
    st[0].Set(0,TextureState::ETexOpModulate,TextureState::ETexArgDiffuse,TextureState::ETexFilterBilinear);
	st[0].AlphaOp = TextureState::ETexOpModulate;
	st[0].AlphaArg = TextureState::ETexArgDiffuse;

	st[1].Stage = 1;
    st[1].Op = TextureState::ETexOpDisable;
	st[1].AlphaOp = TextureState::ETexOpDisable;

	dev->SetTextureState(st,ARRAY_COUNT(st));

	
	//dev->SetStream(VBuf.GetStream());

	const int s[] = { VBuf.GetStream() };
	dev->SetStreams(s,1,EInvalidIndex);

	//dev->SetShader(VBuf.GetStream(),RenderDevice::EShaderFixed);
	dev->SetShader(GEngine->Video->MaterialSys->Shaders.FixedSimple,RenderDevice::EShaderVertex);
	dev->SetShader(EInvalidIndex,RenderDevice::EShaderPixel);

# ifdef TRANSGAMING
    dev->SetFogState(GEngine->pcfWorld->Env.FogParams); // ugly hack: switch to fp'ed fog
# else
    dev->SetFogState(GEngine->World->Env.FogParams); // ugly hack: switch to fp'ed fog
# endif
#endif
}

//============================================================================
void ParticleSystem::ResetState()
{
    RenderDevice* dev=GEngine->GetRenderDevice();
    Assert(dev);

#ifdef	TRANSGAMING_PS2
    dev->SetShader(GEngine->Video->MaterialSys->Shaders.Fixed_DCTU1, RenderDevice::EShaderVertex);
    dev->SetShader(EInvalidIndex, RenderDevice::EShaderPixel);
    dev->SetFogState(GEngine->pcfWorld->Env.FogParams.UseVSFog());
#else
	dev->SetShader(GEngine->Video->MaterialSys->Shaders.FixedSimple,RenderDevice::EShaderVertex);
	dev->SetShader(EInvalidIndex,RenderDevice::EShaderPixel);
# ifdef TRANSGAMING
    dev->SetFogState(GEngine->pcfWorld->Env.FogParams.UseVSFog());
# else
    dev->SetFogState(GEngine->World->Env.FogParams.UseVSFog());
# endif
#endif
}

//============================================================================
//============================================================================
//============================================================================
///// XXXX - For Coronas etc.
//============================================================================

#ifdef	TRANSGAMING
void ParticleSystem::RenderSprites(const Camera& camera,
                                   const Sprite_TG& sprite, bool dontFacing)
#else
void ParticleSystem::RenderSprites(const Camera& camera,
                                   const Sprite& sprite, bool dontFacing)
#endif                                   
{
    RenderDevice* dev=GEngine->GetRenderDevice();
    Assert(dev);
    Viewport &pViewport = GEngine->Video->GetActiveView()->LocalViewport;

	{ // temp shit, ALWAYS draw in gun style z-range
		Viewport view = pViewport;
		view.zNear = 0.0f;
		view.zFar = 0.3f;
		dev->SetViewport(0,0,view.ViewportWidth,view.ViewportHeight,view.zNear,view.zFar);
	}

    // set texture state
    TextureState st[2];
    st[0].Set(0,TextureState::ETexOpModulate,TextureState::ETexArgDiffuse,TextureState::ETexFilterBilinear);
	st[0].AlphaOp = TextureState::ETexOpModulate;
	st[0].AlphaArg = TextureState::ETexArgDiffuse;
    st[1].Set(1,TextureState::ETexOpDisable);

    dev->SetTextureState(st,ARRAY_COUNT(st));

    Vector Up,Right;
    Matrix zr;
    if (dontFacing)
    {
        Up = Vector(0,1,0) * sprite.Size;
        Right = Vector(1,0,0) * sprite.Size;
        zr = sprite.QRotation.ToMatrix();
    }
    else
    {
        Up    = camera.GetUpVector()    * sprite.Size;
        Right = camera.GetRightVector() * sprite.Size;
        Vector cp = Right^Up;
        zr = RotationMatrix(sprite.Rotation,cp.X,cp.Y,cp.Z);
    }

    Up = zr.Transform(Up);
    Right = zr.Transform(Right);

	
#ifdef TRANSGAMING_PS2
    const Vector P[4] = 
    {
        sprite.Position-Right+Up,
        sprite.Position+Right+Up,
        sprite.Position-Right-Up,
        sprite.Position+Right-Up
    };
    static const TexCoord T[4] = {
        TexCoord(0, 1),
        TexCoord(1, 1),
        TexCoord(0, 0),
        TexCoord(1, 0)
    };

    Vector *    v = PBuf.Lock(4);
    DWORD *     c = CBuf.Lock(4);
    TexCoord *  t = TBuf.Lock(4);


    memcpy(v, P, sizeof(Vector) * 4);
    memcpy(t, T, sizeof(TexCoord) * 4);
    c[0] = c[1] = c[2] = c[3] = sprite.Color;

#else
    const Vector P[4] = 
    {
        sprite.Position-Right+Up,
        sprite.Position+Right+Up,
        sprite.Position+Right-Up,
        sprite.Position-Right-Up
    };

    
    SimpleVertex* v = VBuf.Lock(4);

	v[0] = SimpleVertex(P[0], sprite.Color, TexCoord(0,1));
	v[1] = SimpleVertex(P[1], sprite.Color, TexCoord(1,1));
	v[2] = SimpleVertex(P[3], sprite.Color, TexCoord(0,0));
	v[3] = SimpleVertex(P[2], sprite.Color, TexCoord(1,0));
#endif

    RenderState state;
    state.SetBlendMode(sprite.BlendMode);
    state.SetCullMode(RenderState::ECullNone);
	state.SetDepthTest(true);
	state.SetAlphaTest(false);
    state.SetDepthWrite(false);
	dev->SetState(state);
	dev->SetObjectTransform(GIdentityMatrix);

    dev->SetTexture(0,sprite.TexRef);

#ifdef TRANSGAMING_PS2
    int     s[] = { PBuf.GetStream(), CBuf.GetStream(), TBuf.GetStream() };
    int     offset;

    dev->SetStreams(s, ARRAY_COUNT(s), EInvalidIndex);

    dev->SetShader(GEngine->Video->MaterialSys->Shaders.Fixed_DCTU1, RenderDevice::EShaderVertex);
    dev->SetShader(EInvalidIndex, RenderDevice::EShaderPixel);


    offset = PBuf.Unlock();
# ifdef DEBUG
    //make sure all the vertex buffers are in sync!
    Assert(CBuf.Unlock() == offset);
    Assert(TBuf.Unlock() == offset);
# else
    CBuf.Unlock();
    TBuf.Unlock();
# endif

    dev->SetFogState(GEngine->pcfWorld->Env.FogParams); // ugly hack: switch to fp'ed fog
    dev->Draw(RenderDevice::EPrimTriStrip, offset, 2);
    dev->SetFogState(GEngine->pcfWorld->Env.FogParams.UseVSFog());
#else
    dev->SetStream(VBuf.GetStream());
	dev->SetShader(GEngine->Video->MaterialSys->Shaders.FixedSimple,RenderDevice::EShaderVertex);
	dev->SetShader(EInvalidIndex,RenderDevice::EShaderPixel);

# ifdef TRANSGAMING
    dev->SetFogState(GEngine->pcfWorld->Env.FogParams); // ugly hack: switch to fp'ed fog
    dev->Draw(RenderDevice::EPrimTriStrip,VBuf.Unlock(),2);
    dev->SetFogState(GEngine->pcfWorld->Env.FogParams.UseVSFog());
# else
    dev->SetFogState(GEngine->World->Env.FogParams); // ugly hack: switch to fp'ed fog
    dev->Draw(RenderDevice::EPrimTriStrip,VBuf.Unlock(),2);
    dev->SetFogState(GEngine->World->Env.FogParams.UseVSFog());
# endif
#endif

	{ 
		dev->SetViewport(0,0,pViewport.ViewportWidth,pViewport.ViewportHeight,pViewport.zNear,pViewport.zFar);
	}
}
//============================================================================
void ParticleSystem::RenderSprites(const Camera& camera,
                                   const Sprite1DOF& sprite)
{
    RenderDevice* dev=GEngine->GetRenderDevice();
    Assert(dev);

    // set texture state
    TextureState st[2];
    st[0].Set(0,TextureState::ETexOpModulate,TextureState::ETexArgDiffuse,TextureState::ETexFilterBilinear);
	st[0].AlphaOp = TextureState::ETexOpModulate;
	st[0].AlphaArg = TextureState::ETexArgDiffuse;
    st[1].Set(1,TextureState::ETexOpDisable);

    dev->SetTextureState(st,ARRAY_COUNT(st));

#ifdef TRANSGAMING_PS2
    const int   count = sprite.Position.Size() * 2;
    Vector *    v = PBuf.Lock(count);
    DWORD  *    c = CBuf.Lock(count);
    TexCoord *  t = TBuf.Lock(count);
#else
    SimpleVertex* v=VBuf.Lock(sprite.Position.Size()*2);
#endif

    int dm = sprite.DrawMode;
    if (dm >= 10) dm -= 10;
    
    // fill buffer
	Vector camPos = camera.GetPosition();
    Vector Tangent;

    for (int i=0;i<sprite.Position.Size();i++)
    {
        if (i<sprite.Position.Size()-1)
            Tangent = sprite.Position[i+1]-sprite.Position[i];

        Vector add;
        if (dm == 0) // binormal
        {
	        add = (sprite.Position[i]-camPos)^Tangent;
	        add *= 0.5f*sprite.Size/add.Magnitude();
        }
        else if (dm == 1) // horizontal
        {
            add = (camera.GetRightVector()*0.5f*sprite.Size);
        }
        else // vertical
        {
            add = (camera.GetUpVector()*0.5f*sprite.Size);
        }

        float m = (float)i/(float)(sprite.Position.Size()-1);
        if (sprite.DrawMode >= 10)
            m = i%2;

#ifdef TRANSGAMING_PS2
        v[0] = sprite.Position[i] + add;
        v[1] = sprite.Position[i] - add;
        t[0].U = t[1].U = m; 
        t[0].V = 0;
        t[1].V = 1;
        c[0] = c[1] = sprite.Color;

        v += 2;
        c += 2;
        t += 2;
#else
        *v++ = SimpleVertex(sprite.Position[i]+add,sprite.Color,TexCoord(m,0));
        *v++ = SimpleVertex(sprite.Position[i]-add,sprite.Color,TexCoord(m,1));
#endif
    }

    RenderState state;
    state.SetBlendMode(sprite.BlendMode);
    state.SetCullMode(RenderState::ECullNone);
	state.SetDepthTest(true);
	state.SetAlphaTest(false);
    state.SetDepthWrite(false);
	dev->SetState(state);
	dev->SetObjectTransform(GIdentityMatrix);

    dev->SetTexture(0,sprite.TexRef);

#ifdef TRANSGAMING_PS2
    int     s[] = { PBuf.GetStream(), CBuf.GetStream(), TBuf.GetStream() };
    int     offset;


    dev->SetStreams(s, ARRAY_COUNT(s), EInvalidIndex);
    dev->SetShader(GEngine->Video->MaterialSys->Shaders.Fixed_DCTU1, RenderDevice::EShaderVertex);
    dev->SetShader(EInvalidIndex, RenderDevice::EShaderPixel);

    offset = PBuf.Unlock();
# ifdef DEBUG
    //make sure all the vertex buffers are in sync!
    Assert(CBuf.Unlock() == offset);
    Assert(TBuf.Unlock() == offset);
# else
    CBuf.Unlock();
    TBuf.Unlock();
# endif

    dev->SetFogState(GEngine->pcfWorld->Env.FogParams); // ugly hack: switch to fp'ed fog
    dev->Draw(RenderDevice::EPrimTriStrip, offset, (sprite.Position.Size() - 1) * 2);
    dev->SetFogState(GEngine->pcfWorld->Env.FogParams.UseVSFog());

    PBuf.Align(4);
    CBuf.Align(4);
    TBuf.Align(4);
#else
    dev->SetStream(VBuf.GetStream());
	dev->SetShader(GEngine->Video->MaterialSys->Shaders.FixedSimple,RenderDevice::EShaderVertex);
	dev->SetShader(EInvalidIndex,RenderDevice::EShaderPixel);

# ifdef TRANSGAMING
    dev->SetFogState(GEngine->pcfWorld->Env.FogParams); // ugly hack: switch to fp'ed fog
    dev->Draw(RenderDevice::EPrimTriStrip,VBuf.Unlock(),(sprite.Position.Size()-1)*2);
    dev->SetFogState(GEngine->pcfWorld->Env.FogParams.UseVSFog());
# else
    dev->SetFogState(GEngine->World->Env.FogParams); // ugly hack: switch to fp'ed fog
    dev->Draw(RenderDevice::EPrimTriStrip,VBuf.Unlock(),(sprite.Position.Size()-1)*2);
    dev->SetFogState(GEngine->World->Env.FogParams.UseVSFog());
# endif

    VBuf.Align(4);
#endif
}
//============================================================================
void ParticleSystem::Save(FileObject *f)
{
}
//============================================================================
void ParticleSystem::Load(FileObject *f)
{
}
//============================================================================
