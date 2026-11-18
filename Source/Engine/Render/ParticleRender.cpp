#error this file is obselete

#include "Engine.h"
#include "Render/ParticleRender.h"
#include "Render/ParticleEffect.h"
#include "Render/ParticleSystem.h"
#include "Render/Video.h"
#include "Render/View.h"
#include "Render/Material.h"

//================================================================================================
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
// ===============================================================================================

static inline DWORD GetParticleColor(const Particle* particle)
{
    const DWORD A = ((const DWORD)(particle->Alpha  *255.0f))<<24;
    const DWORD R = ((const DWORD)(particle->Color.X*255.0f))<<16;
    const DWORD G = ((const DWORD)(particle->Color.Y*255.0f))<<8;
    const DWORD B = ((const DWORD)(particle->Color.Z*255.0f));

    return (A|R|G|B);
}

// ===============================================================================================

static inline void GetSpriteOrientation(int mode, const Particle* particle, Vector& Right, Vector& Up)
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
}

static inline void GetSparkOrientation(const Particle* particle, Vector& Right, Vector& Up)
{
    const Camera& camera = GEngine->Video->GetActiveView()->mCamera;

	const Vector start	= particle->Position;
    const Vector end	= start + particle->Velocity;

    Right = end - start;
    Up = Right^(start-camera.GetPosition());
    Up.Normalize();

    Up		*= particle->Thickness;
    Right	*= particle->Length;
}

//====================================================================================================

ParticleRenderTnL::ParticleRenderTnL(int InMode)
:	ParticleRenderBase(InMode)
{
}

void ParticleRenderTnL::SetState()
{
    ParticleSystem* PartSys = &GEngine->Video->ParticleSys;

    PartSys->SetState();

    RenderState state;
    state.SetDepthWrite(false);
    state.SetAlphaFunc(RenderState::EFuncGreater);
    state.SetAlphaRef(0x4);
    state.SetAlphaTest(1);

	if (Mode & View::EModeDemonFX)
		state.SetAlphaWrite(true);
}

void ParticleRenderTnL::DrawBatched(const ParticleEmitter* Emitter)
{
	//int Num = 0;
	//for (Particle* particle = emitter->ListFront; particle != NULL; particle=particle->Next) 
	//	++Num;
	const int Num = Emitter->ActiveParticleCount;

	Assert(Num<MAX_BUFFERED_PARTICLES);

    ParticleSystem* PartSys = &GEngine->Video->ParticleSys;
	SimpleVertex* V	= PartSys->GetVBuf().Lock(Num*4);

	const int VStart = PartSys->GetVBuf().GetFillPos();
	const int EStart = (VStart >> 2) * 6;

	for (Particle* particle = Emitter->ListFront; particle != NULL; particle=particle->Next) 
	{
		Vector Right, Up;

		if (Emitter->Type == ParticleEmitter::SparkEmitter)
			GetSparkOrientation(particle, Right, Up);
		else
			GetSpriteOrientation(Mode, particle, Right, Up);

		const DWORD C = GetParticleColor(particle);
		const Vector& Pos = particle->Position;

		const Vector P[4] = 
		{
			Pos-Right+Up,
			Pos+Right+Up,
			Pos+Right-Up,
			Pos-Right-Up
		};

		
		V[0] = SimpleVertex(P[0], C, TexCoord(0,1));
		V[1] = SimpleVertex(P[1], C, TexCoord(1,1));
		V[2] = SimpleVertex(P[2], C, TexCoord(1,0));
		V[3] = SimpleVertex(P[3], C, TexCoord(0,0));

		V+=4;
	}

	PartSys->GetVBuf().Unlock();

	RenderDevice* dev = GEngine->GetRenderDevice();
	const int s[] = { PartSys->GetVBuf().GetStream() };
	dev->SetStreams(s,1,PartSys->GetEBuf());
	dev->Draw(RenderDevice::EPrimTriList, EStart, Num*2, VStart, Num*4);
}				

void ParticleRenderTnL::DrawAnimated(const ParticleEmitter* Emitter)
{
    ParticleSystem* PartSys = &GEngine->Video->ParticleSys;

	const int s[] = { PartSys->GetVBuf().GetStream() };
	RenderDevice* dev=GEngine->GetRenderDevice();
	dev->SetStreams(s,1,EInvalidIndex);

	int* FrameIds = Emitter->TexturePtr->FrameIds;
	int texId = EInvalidIndex;

	for (Particle* particle = Emitter->ListFront; particle != NULL; particle=particle->Next) 
	{
		Vector Right, Up;

		if (Emitter->Type == ParticleEmitter::SparkEmitter)
			GetSparkOrientation(particle, Right, Up);
		else
			GetSpriteOrientation(Mode, particle, Right, Up);

		const DWORD C = GetParticleColor(particle);
		const Vector& Pos = particle->Position;

		if (texId != FrameIds[particle->TexFrame])
			dev->SetTexture(0, texId = FrameIds[particle->TexFrame]);

		const Vector P[4] = 
		{
			Pos-Right+Up,
			Pos+Right+Up,
			Pos+Right-Up,
			Pos-Right-Up
		};
		
		SimpleVertex* v = PartSys->GetVBuf().Lock(4);

		v[0] = SimpleVertex(P[0], C, TexCoord(0,1));
		v[1] = SimpleVertex(P[1], C, TexCoord(1,1));
		v[2] = SimpleVertex(P[3], C, TexCoord(0,0));
		v[3] = SimpleVertex(P[2], C, TexCoord(1,0));

		dev->Draw(RenderDevice::EPrimTriStrip, PartSys->GetVBuf().Unlock(), 2);		
	}
}

void ParticleRenderTnL::Draw(ParticleEffect* Effect)
{
	RenderDevice* dev = GEngine->GetRenderDevice();

	Matrix ObjectToWorld;   // = GIdentityMatrix;

    ObjectToWorld.SetIdentity();

    if (Effect->FixedTransform)
        ObjectToWorld *= TranslationMatrix(Effect->GetPosition());

    for (int i=0; i<Effect->EmitterCount; ++i)
    {
        ParticleEmitter* Emitter = Effect->Emitters[i].Emitter;
        Assert(Emitter);

        if (0 == Emitter->ActiveParticleCount || !Emitter->Active)
            continue;

	    RenderState state;
        state.SetDepthTest(Emitter->DepthTest);			// TODO: redundant states when material is used!!!
		state.SetBlendMode(Emitter->BlendMode);
        dev->SetState(state);

		const Matrix EmitterTransform = Effect->Emitters[i].Transform*ObjectToWorld;

		if (Emitter->MaterialPtr)
		{
			const Viewport& ActiveViewport = GEngine->Video->GetActiveView()->LocalViewport;
			dev->SetShaderMemory(0, &(EmitterTransform * ActiveViewport.WorldToScreen).GetTranspose()[0], 4);

			Material* M = Emitter->MaterialPtr;
			for (int i=0; i<M->PassCount; ++i)
			{
				// NOTE: material input sets everything EXCEPT diffuse map
				// which for particles is defined in emitter ini file
				// sampler 0 is always set to particle diffuse texture

				RenderPass& Pass = M->Passes[i];
				MaterialInput Input;
				Input.WarpMap = Emitter->WarpTexPtr;
				Pass.Activate();
				Pass.SetInput(&Input);

				if (Emitter->TexturePtr->NumFrames == 0) 
				{
					dev->SetTexture(0, Emitter->TexturePtr->DeviceId);
					DrawBatched(Emitter);
				}
				else
				{
					DrawAnimated(Emitter);
				}
			}
		}
		else
		{
	        dev->SetObjectTransform(&EmitterTransform);
			dev->SetShader(GEngine->Video->MaterialSys->Shaders.FixedSimple,RenderDevice::EShaderVertex);
			dev->SetShader(EInvalidIndex,RenderDevice::EShaderPixel);

			TextureState st[1];
			st[0].Stage = 1;
			st[0].Op = TextureState::ETexOpDisable;
			st[0].AlphaOp = TextureState::ETexOpDisable;

			dev->SetTextureState(st,ARRAY_COUNT(st));

			if (Emitter->TexturePtr->NumFrames == 0) 
			{
				dev->SetTexture(0, Emitter->TexturePtr->DeviceId);
				DrawBatched(Emitter);
			}
			else
			{
				DrawAnimated(Emitter);
			}
		}
	}
}

// =====================================================================================

#ifndef TRANSGAMING
ParticleRenderVS::ParticleRenderVS(int InMode)
:	ParticleRenderBase(InMode)
{
}

void ParticleRenderVS::SetState()
{
    ParticleSystem* PartSys = &GEngine->Video->ParticleSys;

    PartSys->SetState();

    RenderState state;
    state.SetDepthWrite(false);
    state.SetAlphaFunc(RenderState::EFuncGreater);
    state.SetAlphaRef(0x4);
    state.SetAlphaTest(1);

	if (Mode & View::EModeDemonFX)
		state.SetAlphaWrite(true);

}

void ParticleRenderVS::Draw(ParticleEffect* Effect)
{
	RenderDevice* dev=GEngine->GetRenderDevice();
    ParticleSystem* PartSys = &GEngine->Video->ParticleSys;

	Matrix ObjectToWorld;   // = GIdentityMatrix;


    ObjectToWorld.SetIdentity();

    if (Effect->FixedTransform)
        ObjectToWorld *= TranslationMatrix(Effect->GetPosition());

    for (int i=0; i<Effect->EmitterCount; ++i)
    {
        ParticleEmitter* Emitter = Effect->Emitters[i].Emitter;
        Assert(Emitter);

        if (0 == Emitter->ActiveParticleCount || !Emitter->Active)
            continue;

		// set transform
		const Matrix EmitterTransform = Effect->Emitters[i].Transform * ObjectToWorld;

		// fill buffer
		const int Num = Emitter->ActiveParticleCount;
		Assert(Num<MAX_BUFFERED_PARTICLES);

		SimpleVertex* V	= PartSys->GetVBuf().Lock(Num*4);

		const int VStart = PartSys->GetVBuf().GetFillPos();
		const int EStart = (VStart >> 2) * 6;

		if (Emitter->Type == ParticleEmitter::SpriteEmitter)
		{
			SetSpriteConstMemory();

			for (Particle* particle = Emitter->ListFront; particle != NULL; particle=particle->Next) 
			{
				const Vector& Pos	= particle->Position;
				const FLOAT Size	= particle->Size;
				const DWORD C		= GetParticleColor(particle);

				V[0] = SimpleVertex(Pos, C, TexCoord(0,Size));
				V[1] = SimpleVertex(Pos, C, TexCoord(1,Size));
				V[2] = SimpleVertex(Pos, C, TexCoord(2,Size));
				V[3] = SimpleVertex(Pos, C, TexCoord(3,Size));

				V += 4;
			}
		}
		else if (Emitter->Type == ParticleEmitter::SparkEmitter)
		{
			SetSparkConstMemory();

			for (Particle* particle = Emitter->ListFront; particle != NULL; particle=particle->Next) 
			{
				//const Vector& Pos	= particle->Position;

				// TODO fill spark
				//dev->Draw(RenderDevice::EPrimTriStrip, PartSys->GetVBuf().Unlock(), 2);		
			}
		}

		PartSys->GetVBuf().Unlock();

		const int s[] = { PartSys->GetVBuf().GetStream() };
		dev->SetStreams(s,1,PartSys->GetEBuf());
		dev->Draw(RenderDevice::EPrimTriList, EStart, Num*2, VStart, Num*4);
	}
}

void ParticleRenderVS::SetSpriteConstMemory()
{
    const Camera& camera = GEngine->Video->GetActiveView()->mCamera;

	const Vector Right	= (Mode & View::EModeRenderRTCubemap) ? camera.GetCubemapRightVector() : camera.GetRightVector();
	const Vector Up		= (Mode & View::EModeRenderRTCubemap) ? camera.GetCubemapUpVector() : camera.GetUpVector();

	const Vector RightVec[4] =
	{
		-Right,
		 Right,
		 Right,
		-Right
	};

	const Vector UpVec[4] =
	{
		 Up,
		 Up,
		-Up,
		-Up
	};

	const TexCoord Tex[4] =
	{
		TexCoord(0,1),
		TexCoord(1,1),
		TexCoord(1,0),
		TexCoord(0,0)
	};
	
}

void ParticleRenderVS::SetSparkConstMemory()
{
	const TexCoord Tex[4] =
	{
		TexCoord(0,1),
		TexCoord(1,1),
		TexCoord(1,0),
		TexCoord(0,0)
	};	
}
#endif
