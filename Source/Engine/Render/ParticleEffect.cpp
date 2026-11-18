// $Header: /Game/Source/Engine/Render/ParticleEffect.cpp 47    05-06-07 13:40 Sokol $

#include "Engine.h"
#include "Render/ParticleEffect.h"
#include "Render/View.h"
#include "Physics/PhysicsObject.h"
#include "Render/ParticleSystem.h"
#include "Render/Video.h"
#include "Render/Material.h"
#include "Engine/Model.h"
#include "Engine/World.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

//============================================================================
// ParticleEffect
//============================================================================
ParticleEffect::ParticleEffect()
:   Entity(Entity::ETypeParticleFx), EmitterCount(0)
,	Active(true)
,	FixedTransform(false)
{
	updateOnce = false;
	ParentJoint  = -1;    
    ParentRotation = false;
    Sort = Entity::ESortTrans;
    IsImmortal = false;
    ParentRotationOffset = IdentityQuaternion();
    ParentOffset.Set(0.0f,0.0f,0.0f);
    CameraOffset.Set(0.0f,0.0f,0.0f);
    VisibilityTimeOut = 5.0f;
	ObjectToWorld.SetIdentity();    // = GIdentityMatrix;
}
//============================================================================
ParticleEffect::~ParticleEffect()
{
    Restart(); // free particles pool
    for (int i=0; i<EmitterCount; i++)
    {
        // free emitters pool
        Emitters[i].Emitter->Active = FALSE; 
        Emitters[i].Emitter->Effect = NULL;
    }
}
//============================================================================
static inline void RotationMatrixOptimized(Matrix& mat, float angle, float x, float y, float z)
{
    float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c, s, c;

    s = GSinFloat(angle);
    c = GCosFloat(angle);

    xx = x * x;
    yy = y * y;
    zz = z * z;
    xy = x * y;
    yz = y * z;
    zx = z * x;
    xs = x * s;
    ys = y * s;
    zs = z * s;
    one_c = 1.0f - c;

#define M(row,col)  mat.mArray[col*4+row]

    M(0,0) = (one_c * xx) + c;
    M(0,1) = (one_c * xy) - zs;
    M(0,2) = (one_c * zx) + ys;
    M(0,3) = 0.0f;

    M(1,0) = (one_c * xy) + zs;
    M(1,1) = (one_c * yy) + c;
    M(1,2) = (one_c * yz) - xs;
    M(1,3) = 0.0f;

    M(2,0) = (one_c * zx) - ys;
    M(2,1) = (one_c * yz) + xs;
    M(2,2) = (one_c * zz) + c;
    M(2,3) = 0.0f;

    M(3,0) = 0.0f;
    M(3,1) = 0.0f;
    M(3,2) = 0.0f;
    M(3,3) = 1.0f;

#undef M
}
//============================================================================
void ParticleEffect::RenderInitialize(RenderDevice* dev, MaterialSystem* matSys, const char* texSubPath)
{
    char buf[MAX_PATH];


    //TRACE("ParticleEffect::RenderInitialize(): initializing %d emitters for '%s'\n", EmitterCount, Name.AsChar());
    for (int i=0; i<EmitterCount; ++i)
    {
        ParticleEmitter* emitter = Emitters[i].Emitter;
        Assert(emitter);


        snprintf(buf, MAX_PATH, "Particles/%s", emitter->TextureName.AsChar());
        emitter->TexturePtr = matSys->CreateTexture(buf);
    }
}
//============================================================================
void ParticleEffect::RenderCleanup   (RenderDevice* dev, MaterialSystem* matSys)
{
    //for (int i=0; i<EmitterCount; ++i)
    //	matSys->ReleaseTexture(Emitters[i].Emitter->TexturePtr);
}
//============================================================================
void ParticleEffect::ComputeNewPosition()
{
	if (Parent && TransformByParent)
	{
		if (ParentJoint>-1)
		{
			Translation = Parent->AsModel()->GetJointTransform(ParentJoint).Transform(ParentOffset);
			if (CameraOffset.X != 0.0f || CameraOffset.Y != 0.0f || CameraOffset.Z != 0.0f)
			{
				const Camera& camera = GEngine->Video->GetActiveView()->mCamera;
				Vector v = Translation-camera.GetPosition();
				v.Normalize();
				Translation -= v*CameraOffset;
			}

			if (ParentRotation) // from joint
				Rotation  = ParentRotationOffset * Parent->AsModel()->GetJointTransform(ParentJoint).GetRotation();                
			else
				Rotation  = Parent->GetRotation(); 
		}
		else
		{
			if (ParentRotation)
				Rotation = ParentRotationOffset * Parent->ObjectToWorld.GetRotation();

			Translation = Parent->ObjectToWorld.GetTranslation() + Parent->ObjectToWorld.GetRotation().Transform(ParentOffset);
			// cameraoffset - oddalenie w kierunku kamery (aby sie nie wcinalo w obiekt)
			if (CameraOffset.X != 0.0f || CameraOffset.Y != 0.0f || CameraOffset.Z != 0.0f)
			{
				const Camera& camera = GEngine->Video->GetActiveView()->mCamera;
				Vector v = Translation-camera.GetPosition();
				v.Normalize();
				Translation -= v*CameraOffset;
			}
		}
	}
#ifndef TRANSGAMING_NO_HAVOK
	if (PO)
		Translation = PO->GetPosition();
#endif
}
//============================================================================
void ParticleEffect::Tick(const float deltaTime)
{
    if (Flags & Entity::EEntNoDraw){
        //TRACE("ParticleEffect::Tick(): not drawing the particle effect '%s' (%d emitters)\n", Name.AsChar(), EmitterCount);
        return;
    }

//-DDP	#ifdef TRANSGAMING
//-DDP	//the particle system wasn't rendered in the last frame => skip updating it 
//-DDP	//as well until it's visible again.
//-DDP	This was causing particles to not update and get left in memory... FIXME!!!
//-DDP	if (VisibilityTimeOut < 2.0f)
//-DDP		return;
//-DDP	#endif

    // disable tick when object was not visibled since 2 seconds (only for evolved)

    // to jeszcze nie dziala, poniewaz efekty przywiazane lub fizyczne powinny moc sie przemieszczac
    // tylko emittery nie powinny byc przeliczane

    //TRACE("ParticleEffect::Tick(): updating the particle effect '%s' (%d emitters)\n", Name.AsChar(), EmitterCount);
    if (VisibilityTimeOut > 0)
        VisibilityTimeOut -= deltaTime;
    else if (!Parent || !TransformByParent)
    {
        bool ret = true;
        for (int i=0; i<EmitterCount; ++i){
            if (!Emitters[i].Emitter->Evolve)
            {
                ret = false;
                break;
            }
        }

        if (ret)
        {
            Box.Reset();
            for (int i=0; i<EmitterCount; ++i)
                if (Emitters[i].Emitter->Active)
                    Box+= Emitters[i].Emitter->Box;
            //GScript.DoString("Game:Print('Hide:%f,%f,%f')",Box.Min.X,Box.Min.Y,Box.Min.Z);

			if (FixedTransform)
				//Box = Box.Transform(ObjectToWorld);
				Box = Box.Transform(TranslationMatrix(Translation));

			return;
        }
    }
	

    if (IsActive())
    {
		Vector lastTranslation(Translation);
		ComputeNewPosition();
        if (!(lastTranslation == Translation))
        {
			SetupEmitters(updateOnce);
			updateOnce = false;				// ulgy hack by Marek for wmodif pfx
            UpdateTransform(false);
        }

        LocalBox.Reset();
        Box.Reset();
        for (int i=0; i<EmitterCount; ++i)
		{
            if (Emitters[i].Emitter->Active)
            {
                Emitters[i].Emitter->Tick(deltaTime);
                Box+= Emitters[i].Emitter->Box;
            }
		}

		if (FixedTransform)
			//Box = Box.Transform(ObjectToWorld);
			Box = Box.Transform(TranslationMatrix(Translation));
        //GScript.DoString("Game:Print('%f,%f,%f')",Box.Min.X,Box.Min.Y,Box.Min.Z);

        if (WorldLink)
            WorldLink->UpdateEntity(this);
    }
    else
        if (WorldLink && !IsImmortal) 
            WorldLink->DeleteEntityDelayed(this);
}

//============================================================================

#define INLINE	__forceinline

static inline void GetSpriteOrientation(int mode, const Particle* particle, Vector& Right, Vector& Up, Vector* P)
{
    const Camera& camera = GEngine->Video->GetActiveView()->mCamera;

	if (mode & View::EModeRenderRTCubemap)
	{
		Right = camera.GetCubemapRightVector() * particle->Size;
		Up    = camera.GetCubemapUpVector()    * particle->Size;
	}
	else
	{
		Right = camera.GetRightVector() * particle->Size;
		Up    = camera.GetUpVector()    * particle->Size;
	}

    if (particle->Rotation) 
	{
        Matrix mat;
		Vector r;
		if (mode & View::EModeRenderRTCubemap)
	        r = camera.GetCubemapForwardVector();
		else
			r = camera.GetForwardVector();  

        RotationMatrixOptimized(mat, particle->Rotation, r.X, r.Y, r.Z);

        Right = mat.TransformNormal(Right);
        Up    = mat.TransformNormal(Up);
    }

	P[0] = particle->Position-Right+Up;
	P[1] = particle->Position+Right+Up;
	P[2] = particle->Position+Right-Up;
	P[3] = particle->Position-Right-Up;
}

static inline void GetSparkOrientation(const Particle* particle, Vector& Right, Vector& Up, Vector* P)
{
    const Camera& camera = GEngine->Video->GetActiveView()->mCamera;

	const Vector start	= particle->Position;
    const Vector end	= start + particle->Velocity;

    Right = end - start;
    Up = Right^(start-camera.GetPosition());
    Up.Normalize();

    Up		*= particle->Thickness;
    Right	*= particle->Length;

	P[0] = particle->Position+Up;
	P[1] = particle->Position+Right+Up;
	P[2] = particle->Position+Right;
	P[3] = particle->Position;
}

static inline DWORD GetParticleColor(const Particle* particle)
{
#ifdef	TRANSGAMING
	const FLOAT S = (GEngine->Video->BloomEnabled && (GEngine->pcfWorld->BloomFX.Multiplier > 0.0F )) 
				  ? 255.0F * GEngine->pcfWorld->BloomFX.DimScale 
				  : 255.0F;

    const DWORD R = ((const DWORD)(particle->Color.X * S));
    const DWORD G = ((const DWORD)(particle->Color.Y * S))<<8;
    const DWORD B = ((const DWORD)(particle->Color.Z * S))<<16;
    const DWORD A = ((const DWORD)(particle->Alpha   * 255.0F))<<24;
#else
	const FLOAT S = (GEngine->Video->BloomEnabled && (GEngine->World->BloomFX.Multiplier > 0.0F )) 
				  ? 255.0F * GEngine->World->BloomFX.DimScale 
				  : 255.0F;

    const DWORD A = ((const DWORD)(particle->Alpha   * 255.0F))<<24;
    const DWORD R = ((const DWORD)(particle->Color.X * S))<<16;
    const DWORD G = ((const DWORD)(particle->Color.Y * S))<<8;
    const DWORD B = ((const DWORD)(particle->Color.Z * S));
#endif	

    return (A|R|G|B);
}

// ==========================================================================================

#ifdef TRANSGAMING_PS2
class ParticleRender
{
public:
    ParticleRender(int InMode, ParticleSystem* InPartSys)
        :	Mode(InMode), PartSys(InPartSys)
    {}

    void DrawBatched(const ParticleEmitter* emitter);

    inline void DrawAnimated(const ParticleEmitter* emitter, const int *FrameIds, int& texId)
    {
        const int               s[] = { PartSys->PBuf.GetStream(), PartSys->CBuf.GetStream(), PartSys->TBuf.GetStream() };
        RenderDevice*           dev = GEngine->GetRenderDevice();
        Vector *                v;
        DWORD *                 c;
        TexCoord *              t;
        int                     offset;
        Vector                  Right, Up;
        Vector                  P[4];
        static const TexCoord   T[4] = {
            TexCoord(0, 1),
            TexCoord(1, 1),
            TexCoord(0, 0),
            TexCoord(1, 0)
        };


        dev->SetStreams(s, ARRAY_COUNT(s), EInvalidIndex);

        for (Particle* particle = emitter->ListFront; particle != NULL; particle=particle->Next){
            if (emitter->Type == ParticleEmitter::SparkEmitter)
                GetSparkOrientation(particle, Right, Up, P);
            else
                GetSpriteOrientation(Mode, particle, Right, Up, P);

            const DWORD C = GetParticleColor(particle);


            if (texId != FrameIds[particle->TexFrame])
                dev->SetTexture(0, texId = FrameIds[particle->TexFrame]);

            v = PartSys->PBuf.Lock(4);
            c = PartSys->CBuf.Lock(4);
            t = PartSys->TBuf.Lock(4);

            memcpy(v, P, sizeof(Vector) * 2);
            memcpy(t, T, sizeof(TexCoord) * 4);
            v[2] = P[3];
            v[3] = P[2];
            c[0] = c[1] = c[2] = c[3] = C;

            offset = PartSys->PBuf.Unlock();
# ifdef DEBUG
            Assert(PartSys->CBuf.Unlock() == offset);
            Assert(PartSys->TBuf.Unlock() == offset);
# else
            PartSys->CBuf.Unlock();
            PartSys->TBuf.Unlock();
# endif

            dev->Draw(RenderDevice::EPrimTriStrip, offset, 2);		
        }
    }

private:
    int	Mode;
    ParticleSystem* PartSys;
};


void ParticleRender::DrawBatched(const ParticleEmitter* emitter)
{
        static const TexCoord T[6] = {
            TexCoord(0, 1),
            TexCoord(1, 1),
            TexCoord(1, 0),
            TexCoord(0, 1),
            TexCoord(1, 0),
            TexCoord(0, 0)
        };

        int Num = emitter->ActiveParticleCount;

        //clamp the particle count to the rendering limit
        if (Num > MAX_BUFFERED_PARTICLES)
            Num = MAX_BUFFERED_PARTICLES;

        Assert(Num <= MAX_BUFFERED_PARTICLES);

        const int   vertCount = Num * 6;
        Vector *    v = PartSys->PBuf.Lock(vertCount);
        DWORD *     c = PartSys->CBuf.Lock(vertCount);
        TexCoord *  t = PartSys->TBuf.Lock(vertCount);
        const int   VStart = PartSys->PBuf.GetFillPos();
        int         count = 0;
        Vector      Right, Up;
        Vector      P[4];

# ifdef DEBUG
        //make sure all the streams are in sync!
        Assert(PartSys->CBuf.GetFillPos() == VStart);
        Assert(PartSys->TBuf.GetFillPos() == VStart);
# endif

		float ConstantColourRGB = 0.0f;
		float ConstantColourA = 0.0f;

        for (Particle *particle = emitter->ListFront; particle != NULL; particle = particle->Next)
		{

            if (emitter->Type == ParticleEmitter::SparkEmitter)
                GetSparkOrientation(particle, Right, Up, P);
            else
                GetSpriteOrientation(Mode, particle, Right, Up, P);

			float alpha = particle->Alpha;
			if (alpha <0.3f)
				alpha = 0.3f;

			ConstantColourRGB += particle->Color.X + particle->Color.Y + particle->Color.Z;
			ConstantColourA += alpha;

            const DWORD C = GetParticleColor(particle);

            memcpy(v, P, sizeof(Vector) * 3);
            memcpy(&v[4], &P[2], sizeof(Vector) * 2);
            memcpy(t, T, sizeof(TexCoord) * 6);
            v[3] = P[0];
            c[0] = c[1] = c[2] = c[3] = c[4] = c[5] = C;
            
            v += 6;
            c += 6;
            t += 6;

            //make sure the particle count doesn't go above the buffering limit!
            count++;

            if (count >= Num)
                break;
        }

        PartSys->PBuf.Unlock();
        PartSys->CBuf.Unlock();
        PartSys->TBuf.Unlock();

		ConstantColourRGB /= (float)count*3.0f;
		ConstantColourA /= (float)count;

		DWORD	CCol =	((int)(ConstantColourA*255.0f))<<24 |
						((int)(ConstantColourRGB*255.0f))<<16 | 
						((int)(ConstantColourRGB*255.0f))<< 8 | 
						((int)(ConstantColourRGB*255.0f));

        RenderDevice* dev=GEngine->GetRenderDevice();
        const int s[] = { PartSys->PBuf.GetStream(), PartSys->CBuf.GetStream(), PartSys->TBuf.GetStream() };
        dev->SetStreams(s, ARRAY_COUNT(s), EInvalidIndex);
        dev->SetConstColor(CCol);
        dev->Draw(RenderDevice::EPrimTriList, VStart, Num * 2);
}


#else
class ParticleRender
{
public:
	ParticleRender(int InMode, ParticleSystem* InPartSys)
	:	Mode(InMode), PartSys(InPartSys)
	{}

	inline void DrawBatched(const ParticleEmitter* emitter)
	{
		//int Num = 0;
		//for (Particle* particle = emitter->ListFront; particle != NULL; particle=particle->Next) 
		//	++Num;
		int Num = emitter->ActiveParticleCount;
		Assert(Num<MAX_BUFFERED_PARTICLES);

#ifdef TRANSGAMING_PSP
		SimpleVertex* V	= PartSys->VBuf.Lock(Num*6);
        static const TexCoord T[6] = {
            TexCoord(0, 1),
            TexCoord(1, 1),
            TexCoord(1, 0),
            TexCoord(0, 1),
            TexCoord(1, 0),
            TexCoord(0, 0)
        };

		const int   VStart = PartSys->VBuf.GetFillPos();

        //TRACE("ParticleRender::DrawBatched(): rendering particles starting from vertex %d, and index %d (limits: v = %d, i = %d)\n", VStart, EStart, PartSys->VBuf.GetMaxElems(), MAX_BUFFERED_PARTICLES*6);

		for (Particle* particle = emitter->ListFront; particle != NULL; particle=particle->Next) 
		{
			Vector Right, Up;
			Vector P[4];

			if (emitter->Type == ParticleEmitter::SparkEmitter)
				GetSparkOrientation(particle, Right, Up, P);
			else
				GetSpriteOrientation(Mode, particle, Right, Up, P);

			const DWORD C = GetParticleColor(particle);
			
            //v: 0 1 2 3 4 5
            //q: 0 1 2 0 2 3
            V[0].Pos = V[3].Pos = P[0];
            V[1].Pos = P[1];
            V[2].Pos = V[4].Pos = P[2];
            V[5].Pos = P[3];
            V[0].Color = V[1].Color = V[2].Color = V[3].Color = V[4].Color = V[5].Color = C;
            V[0].TU = T[0];
            V[1].TU = T[1];
            V[2].TU = T[2];
            V[3].TU = T[3];
            V[4].TU = T[4];
            V[5].TU = T[5];

			V+=6;
		}

		

		RenderDevice* dev=GEngine->GetRenderDevice();
		const int s[] = { PartSys->VBuf.GetStream() };
		dev->SetStreams(s, 1, EInvalidIndex);
		dev->Draw(RenderDevice::EPrimTriList, PartSys->VBuf.Unlock(), Num * 2);
#else
		SimpleVertex* V	= PartSys->VBuf.Lock(Num*4);

		const int   VStart = PartSys->VBuf.GetFillPos();
		const int   EStart = (VStart >> 2) * 6;

        //TRACE("ParticleRender::DrawBatched(): rendering particles starting from vertex %d, and index %d (limits: v = %d, i = %d)\n", VStart, EStart, PartSys->VBuf.GetMaxElems(), MAX_BUFFERED_PARTICLES*6);

		for (Particle* particle = emitter->ListFront; particle != NULL; particle=particle->Next) 
		{
			Vector Right, Up;
			Vector P[4];

			if (emitter->Type == ParticleEmitter::SparkEmitter)
				GetSparkOrientation(particle, Right, Up, P);
			else
				GetSpriteOrientation(Mode, particle, Right, Up, P);

			const DWORD C = GetParticleColor(particle);
			
			V[0] = SimpleVertex(P[0], C, TexCoord(0,1));
			V[1] = SimpleVertex(P[1], C, TexCoord(1,1));
			V[2] = SimpleVertex(P[2], C, TexCoord(1,0));
			V[3] = SimpleVertex(P[3], C, TexCoord(0,0));

			V+=4;
		}

		PartSys->VBuf.Unlock();

		RenderDevice* dev=GEngine->GetRenderDevice();
		const int s[] = { PartSys->VBuf.GetStream() };
		dev->SetStreams(s,1,PartSys->EBuf);
		dev->Draw(RenderDevice::EPrimTriList, EStart, Num * 2, VStart, Num * 4);
#endif
	}

	inline void DrawAnimated(const ParticleEmitter* emitter, const int *FrameIds, int& texId)
	{
		const int s[] = { PartSys->VBuf.GetStream() };
		RenderDevice* dev=GEngine->GetRenderDevice();
		dev->SetStreams(s,1,EInvalidIndex);

		for (Particle* particle = emitter->ListFront; particle != NULL; particle=particle->Next) 
		{
			Vector Right, Up;
			Vector P[4];

			if (emitter->Type == ParticleEmitter::SparkEmitter)
				GetSparkOrientation(particle, Right, Up, P);
			else
				GetSpriteOrientation(Mode, particle, Right, Up, P);

			const DWORD C = GetParticleColor(particle);

			if (texId != FrameIds[particle->TexFrame])
				dev->SetTexture(0,texId = FrameIds[particle->TexFrame]);
			
			SimpleVertex* v = PartSys->VBuf.Lock(4);

			v[0] = SimpleVertex(P[0], C, TexCoord(0,1));
			v[1] = SimpleVertex(P[1], C, TexCoord(1,1));
			v[2] = SimpleVertex(P[3], C, TexCoord(0,0));
			v[3] = SimpleVertex(P[2], C, TexCoord(1,0));

			dev->Draw(RenderDevice::EPrimTriStrip, PartSys->VBuf.Unlock(), 2);		
		}
	}

private:
	int	Mode;
	ParticleSystem* PartSys;
};
#endif


//============================================================================
void ParticleEffect::Draw(RenderDevice* /*dev*/, int mode)
{
	if (!(mode & View::EModeMultiPass))
		return;

	const bool demonFX = (mode & View::EModeDemonFX)!=0;
	if (demonFX ^ Demonic)
		return;

    VisibilityTimeOut = 2.0f;
    ParticleSystem* partSys = &GEngine->Video->ParticleSys;
    //const Camera& camera = GEngine->Video->GetActiveView()->mCamera;
    ParticleEffect &effect = *this;

    if (!effect.IsActive())
        return;

    //TRACE("ParticleEffect::Draw(): rendering the particle system '%s' (%d emitters)\n", Name.AsChar(), effect.EmitterCount);
    RenderDevice* dev=GEngine->GetRenderDevice();
    Assert(dev);

    partSys->SetState();

    RenderState state;
    state.SetCullMode(RenderState::ECullNone);
    state.SetDepthWrite(false);
    state.SetAlphaFunc(RenderState::EFuncGreater);
    state.SetAlphaRef(0x4);
    state.SetAlphaTest(1);

	if (mode&View::EModeDemonFX)
		state.SetAlphaWrite(true);

	/*
    //Matrix ObjectToWorld = ScaleMatrix(effect.GetScale());
    Matrix ObjectToWorld = GIdentityMatrix;
    if (effect.FixedTransform)
        ObjectToWorld *= TranslationMatrix(effect.GetPosition());
	*/

	Matrix ObjectToWorld;// = GIdentityMatrix;
    

    ObjectToWorld.SetIdentity();

    if (effect.FixedTransform)
		ObjectToWorld = TranslationMatrix(effect.GetPosition());

	ParticleRender render(mode, partSys);

    for (int i=0; i<effect.EmitterCount; ++i)
    {
        ParticleEmitter* emitter = effect.Emitters[i].Emitter;
        Assert(emitter);

        if (0 == emitter->ActiveParticleCount || !emitter->Active)
            continue;

		/*
		const Matrix EmitterTransform = effect.Emitters[i].Transform*ObjectToWorld;
		if (emitter->KillDist > 0.0F)
		{
			const Vector EmitterPos = EmitterTransform.TransformCoord(Vector(0.0F));
			const FLOAT DistSquared = (camera.GetPosition() - EmitterPos).MagnitudeSquared();
			if (DistSquared > emitter->KillDist)
				continue;
		}
		*/

		
		state.SetDepthTest(emitter->DepthTest);			// TODO: redundant states when material is used!!!
		state.SetBlendMode(emitter->BlendMode);
        dev->SetState(state);

		// setup emitter transform
		/*
		Matrix EmitterTransform = effect.Emitters[i].Transform;
		if (effect.Emitters[i].FixedTransform)
			EmitterTransform *= TranslationMatrix(effect.GetPosition());
		*/
		const Matrix EmitterTransform = effect.Emitters[i].Transform * ObjectToWorld;

		if (emitter->MaterialPtr)
		{
			const Viewport& ActiveViewport = GEngine->Video->GetActiveView()->LocalViewport;
			dev->SetShaderMemory(0, &(EmitterTransform * ActiveViewport.WorldToScreen).GetTranspose()[0], 4);

			Material* M = emitter->MaterialPtr;
			for (int i=0; i<M->PassCount; ++i)
			{
				// NOTE: material input sets everything EXCEPT diffuse map
				// which for particles is defined in emitter ini file
				// sampler 0 is always set to particle diffuse texture

				RenderPass& Pass = M->Passes[i];
				MaterialInput Input;
				Input.WarpMap = emitter->WarpTexPtr;
				Pass.Activate();
				Pass.SetInput(&Input);

				int texId = EInvalidIndex;
				int *FrameIds = emitter->TexturePtr->FrameIds;

				if (emitter->TexturePtr->NumFrames == 0) 
				{
					dev->SetTexture(0,emitter->TexturePtr->DeviceId);
					FrameIds = 0;

                    //TRACE("ParticleEffect::Draw():      rendering batched with a material\n");
					render.DrawBatched(emitter);
				}
				else
				{
                    //TRACE("ParticleEffect::Draw():      rendering animated with a material\n");
					render.DrawAnimated(emitter, FrameIds, texId);
				}
			}
		}
		else
		{
	        //dev->SetObjectTransform(&EmitterTransform);
            if (effect.FixedTransform){
                TranslationMatrix tm(effect.GetPosition());

				dev->SetObjectTransform(&tm);
            }
			else
				dev->SetObjectTransform(GIdentityMatrix);

#ifdef TRANSGAMING_PS2
            dev->SetShader(GEngine->Video->MaterialSys->Shaders.Fixed_DCTU1, RenderDevice::EShaderVertex);
            dev->SetShader(EInvalidIndex, RenderDevice::EShaderPixel);
#else
			dev->SetShader(GEngine->Video->MaterialSys->Shaders.FixedSimple,RenderDevice::EShaderVertex);
			dev->SetShader(EInvalidIndex,RenderDevice::EShaderPixel);
#endif

			TextureState st[1];
			st[0].Stage = 1;
			st[0].Op = TextureState::ETexOpDisable;
			st[0].AlphaOp = TextureState::ETexOpDisable;

			dev->SetTextureState(st,ARRAY_COUNT(st));

			int texId = EInvalidIndex;
			int *FrameIds = emitter->TexturePtr->FrameIds;

			if (emitter->TexturePtr->NumFrames == 0) 
			{
				dev->SetTexture(0,emitter->TexturePtr->DeviceId);
				FrameIds = 0;

                //TRACE("ParticleEffect::Draw():      rendering batched without a material\n");
				render.DrawBatched(emitter);
			}
			else
			{
                //TRACE("ParticleEffect::Draw():      rendering animated without a material\n");
				render.DrawAnimated(emitter, FrameIds, texId);
			}
		}
	}

	partSys->ResetState();
}

//============================================================================
int ParticleEffect::AddEmitter(ParticleEmitter* emitter)
{
    if (EmitterCount == MaxEmitters-1){
        //TRACE("ParticleEffect::AddEmitter(): effect '%s' is full!\n", Name.AsChar());

        return -1;
    }

    //TRACE("ParticleEffect::AddEmitter(): adding an emitter to the effect '%s'\n", Name.AsChar());
    const int index = EmitterCount++;
    EmitterDef* e = &Emitters[index];
    e->Emitter				= emitter; 
    e->Emitter->Effect		= this; 
    //e->Transform			= GIdentityMatrix;
    e->Pos					= Vector(0,0,0);
    e->Rotation				= Vector(0,0,0);
    e->Scale				= Scale;

    e->Transform.SetIdentity();

	// handle special materials
	if (emitter->MaterialPtr)
	{
        //TRACE("ParticleEffect::AddEmitter(): emitter has a material!  This might not render correctly...\n");
		Material* M = emitter->MaterialPtr;
		if (M->Flags & EMF_WarpScreen)
		{
			WarpScreen = true;
			Sort = ESortWarp;

			// ugly hack: no render on puny cards
			if (GEngine->GetRenderDevice()->HWClass < RenderDevice::EHWClassNV30)
				EnableDraw(false);
		}
	}

	if (emitter->KillDist > 0.0F)
		this->KillDist = emitter->KillDist;

    return index;
}
//============================================================================
void ParticleEffect::RemoveEmitter(int index)
{
    //TRACE("ParticleEffect::RemoveEmitter(): removing emitter %d from effect '%s'\n", index, Name.AsChar());
    if (index>=0 && index<EmitterCount) {
		
		Emitters[index].Emitter->Active = FALSE;
        Emitters[index].Emitter->Effect = NULL;

#ifdef TRANSGAMING
        memmove(&Emitters[index], &Emitters[index + 1], (EmitterCount - index - 1) * sizeof(Emitters[0]));
#else
        for (int i=index+1; i<EmitterCount; ++i)
            Emitters[i-1] = Emitters[i];
#endif
        --EmitterCount;	
    }

}
//============================================================================
void ParticleEffect::SetPosition(const Vector& pos)
{
    Entity::SetPosition(pos);
    SetupEmitters();
}
//============================================================================
void ParticleEffect::SetScale(float scale)
{
    Entity::SetScale(scale);
    SetupEmitters();
}
//============================================================================
void ParticleEffect::SetRotation(const Quaternion& rotation)
{
    Entity::SetRotation(rotation);
    SetupEmitters();

}
//============================================================================
void ParticleEffect::SetupEmitters(bool forceLastPosition)
{
	ComputeNewPosition();
    for (int i=0; i<EmitterCount; ++i)
    {
        Emitters[i].Emitter->LastPosition = Emitters[i].Emitter->Position;
        Emitters[i].SetupTransform();
        if (Emitters[i].Emitter->LastPosition.MagnitudeSquared() == 0 || forceLastPosition)
            Emitters[i].Emitter->LastPosition = Emitters[i].Emitter->Position;
    }
}
//============================================================================
void ParticleEffect::EmitterDef::SetupTransform()
{
    Matrix r;
    Vector rV(FDeg2Rad(Rotation.X),FDeg2Rad(Rotation.Y),FDeg2Rad(Rotation.Z));
    r.FromEulerAnglesXYZ(rV);

    //if (!Emitter->Effect->FixedTransform)  to do - per emitterdef

	/*
    if (Emitter->Effect && !Emitter->Effect->FixedTransform)
    {
        Emitter->RotQuat = Emitter->Effect->Rotation * Quaternion(r);	
        Emitter->Position = Emitter->Effect->Translation + Pos * Emitter->Effect->Scale;
    }
	*/
    if (Emitter->Effect)
    {
        Emitter->RotQuat = Emitter->Effect->Rotation * Quaternion(r);	
		if (Emitter->Effect->FixedTransform)
			Emitter->Position = Pos;
		else
			Emitter->Position = Emitter->Effect->Translation + Pos * Emitter->Effect->Scale;
    
        Transform.SetIdentity();    // = IdentityMatrix();

        Emitter->SetScale(Emitter->Effect->Scale * Scale); // scale emitter's properties
    }
    else
    {
        Emitter->RotQuat = Quaternion(r);	
        Emitter->Position = Pos;
    
        Transform.SetIdentity();    // = IdentityMatrix();

        Emitter->SetScale(Scale); // scale emitter's properties
    }
}
//============================================================================
BOOL ParticleEffect::IsActive() const
{
    if (EmitterCount == 0) return TRUE; // for empty object

    for (int i=0; i<EmitterCount; ++i){
        if (Emitters[i].Emitter->Active)
            return TRUE;
    }

    return FALSE;
}
//============================================================================
void ParticleEffect::Deactivate(BOOL instantKill)
{
    for (int i=0; i<EmitterCount; ++i) {
        Emitters[i].Emitter->Deactivate(instantKill);
    }

    Active = false;
}
//============================================================================
void ParticleEffect::Restart()
{
    SetupEmitters();
    for (int i=0; i<EmitterCount; ++i)
        Emitters[i].Emitter->Restart();

    Active = true;
    LocalBox.Reset();
    LocalBox.Expand(1);
	VisibilityTimeOut = 5.0f;
}
//============================================================================
bool ParticleEffect::OnUnregister()
{
    if (EmitterCount == 1 && Emitters[0].Emitter->Immortal) // flarki maja od razu znikac
        return true;
    Deactivate();
    return false;
}
//============================================================================
void ParticleEffect::SetParentOffset(const Vector &offset, int joint, const Vector &camoffset)
{	
    ParentOffset = offset;
    CameraOffset = camoffset;
    if (Parent && Parent->AsModel())
        ParentJoint = joint;

	ComputeNewPosition();
}

#define PARTICLEFX_LOAD_SAVE_TOKEN 0x5e93

//============================================================================
bool ParticleEffect::LoadEntity( FileObject* f )
{
    if( !f->TestToken( PARTICLEFX_LOAD_SAVE_TOKEN ) )
    {
        GLog.Print( "Token test failure: ParticleFX::LoadEntity()\n" );
        return false;
    }

    Entity::LoadEntity( f );
    *f >> Flags >> Translation >> Rotation;

    *f >> ParentOffset;
    *f >> ParentRotation;
    *f >> ParentRotationOffset;

    *f >> CameraOffset;

    *f >> Active;
    *f >> IsImmortal;

	*f >> FixedTransform;

    int ecount;
    *f >> ecount;
    for (int i=0; i < ecount; ++i)
    {
        String ename;
        *f >> ename;
        AddEmitter(GEngine->Video->ParticleSys.CreateEmitter(ename.AsChar()));

        //int     ActiveParticleCount;
        //int     SpawnedParticleCount;

        *f >> Emitters[i].Emitter->RotQuat;
        *f >> Emitters[i].Emitter->Position;
        *f >> Emitters[i].Emitter->LastPosition;

        char tmp;
        *f >> tmp; Emitters[i].Emitter->Active   = tmp;
        *f >> tmp; Emitters[i].Emitter->Respawn  = tmp;
        *f >> tmp; Emitters[i].Emitter->Evolve   = tmp;
        *f >> tmp; Emitters[i].Emitter->Immortal = tmp;
        *f >> tmp; Emitters[i].Emitter->Dying    = tmp;

        *f >> Emitters[i].Pos;
        *f >> Emitters[i].Rotation;
        *f >> Emitters[i].Scale;
    }

    SetupEmitters();
    return true;
}
//============================================================================
bool ParticleEffect::SaveEntity( FileObject* f )
{
    // Data for World to recreate us
    *f << (int)ETypeParticleFx << FileName << Name << Scale << (char) (0);

    if( !f->PutToken( PARTICLEFX_LOAD_SAVE_TOKEN ) ) return false;

    Entity::SaveEntity( f );

    *f << Flags << Translation << Rotation;

    *f << ParentOffset;
    *f << ParentRotation;
    *f << ParentRotationOffset;

    *f << CameraOffset;

    *f << Active;
    *f << IsImmortal;

	*f << FixedTransform;

    *f << EmitterCount;
    for (int i=0; i<EmitterCount; ++i)
    {
        *f << Emitters[i].Emitter->Name;
        
        //int     ActiveParticleCount;
        //int     SpawnedParticleCount;

        *f << Emitters[i].Emitter->RotQuat;
        *f << Emitters[i].Emitter->Position;
        *f << Emitters[i].Emitter->LastPosition;

        *f << (char) (Emitters[i].Emitter->Active   ?1:0);
        *f << (char) (Emitters[i].Emitter->Respawn  ?1:0);
        *f << (char) (Emitters[i].Emitter->Evolve   ?1:0);
        *f << (char) (Emitters[i].Emitter->Immortal ?1:0);
        *f << (char) (Emitters[i].Emitter->Dying    ?1:0);

        *f << Emitters[i].Pos;
        *f << Emitters[i].Rotation;
        *f << Emitters[i].Scale;
    }
        
    return true;
}
//============================================================================
#ifndef TRANSGAMING
bool ParticleEffect::DemoReadEntity( FileObject *f )
{
	Entity::DemoReadEntity( f );
	return true;
}


bool ParticleEffect::DemoWriteEntity( FileObject *f )
{
	Entity::DemoWriteEntity( f );
	return true;
}


bool ParticleEffect::DemoReadFull( FileObject *f )
{
	Entity::DemoReadFull( f );

	*f >> ParentOffset;
    *f >> ParentRotation;
    *f >> ParentRotationOffset;

    *f >> CameraOffset;

    *f >> Active;
    *f >> IsImmortal;

    int ecount;
    *f >> ecount;
    for (int i=0; i < ecount; ++i)
    {
        String ename;
        *f >> ename;
        AddEmitter(GEngine->Video->ParticleSys.CreateEmitter(ename.AsChar()));

        //int     ActiveParticleCount;
        //int     SpawnedParticleCount;

        *f >> Emitters[i].Emitter->RotQuat;
        *f >> Emitters[i].Emitter->Position;
        *f >> Emitters[i].Emitter->LastPosition;

        char tmp;
        *f >> tmp; Emitters[i].Emitter->Active   = tmp;
        *f >> tmp; Emitters[i].Emitter->Respawn  = tmp;
        *f >> tmp; Emitters[i].Emitter->Evolve   = tmp;
        *f >> tmp; Emitters[i].Emitter->Immortal = tmp;
        *f >> tmp; Emitters[i].Emitter->Dying    = tmp;

        *f >> Emitters[i].Pos;
        *f >> Emitters[i].Rotation;
        *f >> Emitters[i].Scale;
    }

    SetupEmitters();

	return true;
}


bool ParticleEffect::DemoWriteFull( FileObject *f )
{
	Entity::DemoWriteFull( f );

	*f << ParentOffset;
    *f << ParentRotation;
    *f << ParentRotationOffset;

    *f << CameraOffset;

    *f << Active;
    *f << IsImmortal;

    *f << EmitterCount;
    for (int i=0; i<EmitterCount; ++i)
    {
        *f << Emitters[i].Emitter->Name;
        
        //int     ActiveParticleCount;
        //int     SpawnedParticleCount;

        *f << Emitters[i].Emitter->RotQuat;
        *f << Emitters[i].Emitter->Position;
        *f << Emitters[i].Emitter->LastPosition;

        *f << (char) (Emitters[i].Emitter->Active   ?1:0);
        *f << (char) (Emitters[i].Emitter->Respawn  ?1:0);
        *f << (char) (Emitters[i].Emitter->Evolve   ?1:0);
        *f << (char) (Emitters[i].Emitter->Immortal ?1:0);
        *f << (char) (Emitters[i].Emitter->Dying    ?1:0);

        *f << Emitters[i].Pos;
        *f << Emitters[i].Rotation;
        *f << Emitters[i].Scale;
    }
	return true;
}
#endif
