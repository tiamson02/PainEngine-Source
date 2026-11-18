#include "Engine.h"
#include "Billboard.h"
#include "Render/View.h"
#include "Physics/PhysicsWorld.h"
#include "Render/Video.h"
#include "Render/Material.h"
#include "Engine/World.h"
#ifdef TRANSGAMING
# include "Render/ParticleSystem.h"
#endif

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

/* min and max macros */
#ifndef NOMINMAX
#ifndef max
#define max(a,b)   (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)   (((a) < (b)) ? (a) : (b))
#endif
#endif  /* NOMINMAX */

Billboard::Billboard() 
: Entity(Entity::ETypeBillboard), BillboardType(0), TexPtr(0), Timer(0), 
  Alpha(0), Color(1,1,1), BlendMode(RenderState::EBlendAlpha)
{
	TimeDelta = 0;
	AlphaMax	= 0.4f;
	FadeInTime	= 0.5f;
	FadeOutTime = 0.5f;
	MinRadius	= 0.5f;
	MaxRadius	= 4.f;
	MinDistance   	= 1.0f;
	MaxDistance	= 10.f;
	Distance = 1;
	OffDistance = 80;
	TraceMargin = 1;
	IsVisible = false;
	HitStatus = true;
	TraceInterval = CORONAS_TRACE_INTERVAL;
	Sprite.Size = MaxRadius;
	LocalBox.Expand(0);
	Sort = ESortTrans;
}

void Billboard::SetCorona(bool enable)
{
	if (enable)
	{
		BillboardType |= EBillboardCorona;
		Sort = ESortCorona;
	}
	else
	{
		BillboardType &= ~EBillboardCorona;
		Sort = ESortTrans;
	}
}

void Billboard::FadeTick(BOOL visible, float delta)
{
	if (FadeOutTime<=0)  FadeOutTime = 0.0001f;
	if (FadeInTime<=0)   FadeInTime  = 0.0001f;
	
	if (AlphaMax<=0)    AlphaMax  = 0.0001f;

	if (visible != IsVisible)
		Timer = visible? (Alpha/AlphaMax) * FadeInTime : (Alpha/AlphaMax) * FadeOutTime;

	if (visible) 
	{ 		
		Timer += delta;
		Alpha = AlphaMax * (Timer/FadeInTime);
		
		if (Timer>=FadeInTime)
		{
			Alpha = AlphaMax;
			Timer = FadeInTime;
		}
	}
	else 
	{
		Timer -= delta;
		Alpha = AlphaMax * (Timer/FadeOutTime);
		
		if (Timer<=0)
		{
			Alpha = 0;
			Timer = 0;
		}
	}

	IsVisible = visible;
}

void Billboard::RenderInitialize(RenderDevice* dev, MaterialSystem* matSys, const char* texSubPath)
{
	TexPtr = matSys->CreateTexture(TexName.AsChar());
}

void Billboard::RenderCleanup(RenderDevice* dev, MaterialSystem* matSys)
{
	if (TexPtr)
		matSys->ReleaseTexture(TexPtr);

	TexPtr = 0;
}

void Billboard::Draw(RenderDevice* dev, int mode)
{
    Camera& cam = GEngine->Video->GetActiveView()->mCamera;

	Vector trans = ObjectToWorld.GetTranslation();

	if (BillboardType & EBillboardCorona)
	{
		// trace tutaj poniewaz draw nie jest wywolywany dla obiekotw niewidocznych przez portale
		// a tick jest wywolywany zawsze
		BOOL show = TRUE;
		Distance  = (trans - cam.GetPosition()).Magnitude();
		if( !GEngine->Video->CoronasEnabled )
			Distance = OffDistance;
		if (Distance < OffDistance)
		{
			TraceInterval -= TimeDelta;
			if (TraceInterval <= 0)
			{
				Vector dir   = (trans-cam.GetPosition()).GetNormal();
				Vector start = cam.GetPosition()+dir*0.3f; // ugly hack: to avoid tracing from within player body
				Vector end   = trans-dir*TraceMargin;

                //TRACE("Billboard::Draw(): checking billboard visibility (physics = %p)\n", GEngine->pcfWorld->Physics);
				if (!GEngine->Video->GetActiveView()->LocalViewport.ViewFrustum.BoxTest(end,Vector(0,0,0)))
					HitStatus = true;
#ifdef	TRANSGAMING
                else if( GEngine->pcfWorld->Physics ){
                    //TRACE("Billboard::Draw(): performing a line trace to the billboard\n");
# ifdef TRANSGAMING_NO_HAVOK
					HitStatus = true;
# else // TRANSGAMING_NO_HAVOK
					HitStatus = GEngine->pcfWorld->Physics->LineTraceFirstHit(start,end);
# endif // TRANSGAMING_NO_HAVOK
                }
#else
				else if( GEngine->World->Physics )
					HitStatus = GEngine->World->Physics->LineTraceFirstHit(start,end);
#endif // TRANSGAMING
				else
					HitStatus = true;

				TraceInterval = CORONAS_TRACE_INTERVAL;
			}
			show = !HitStatus;
		}
		else
			show = FALSE;

		FadeTick(show,TimeDelta);

		if (Alpha>0)
		{
			if	(Distance >= MaxDistance)
			{
				// oddala sie razem z kamera
				Sprite.Size = MaxRadius;
			}
			else if	(Distance <= MinDistance)
			{			
				//float a = Distance / MinDistance; // trzyma staly rozmiar na ekranie w minimalnym zasiegu
				float a = 1; // oddala sie z kamera
				Sprite.Size = MinRadius * a;
			}
			else
			{
				// skaluje sie miedzy min, a max - oddalajac sie razem z kamera
				float a = (MaxRadius - MinRadius) / (MaxDistance - MinDistance);
				Sprite.Size = MinRadius + a * (Distance - MinDistance);
			}
			UpdateBBox();
		}
	}
	else
		Alpha = AlphaMax;


	if (Alpha > 0)
	{
		Sprite.Position = trans;
		//const FLOAT S = GEngine->World->BloomFX.Enabled ? GEngine->World->BloomFX.Multiplier : 1.0F;
		//const float c[] = { Color.R/(255.0f*S),Color.G/(255.0f*S),Color.B/(255.0f*S),Alpha };

		//const float c[] = { Color.R/255.0f,Color.G/255.0f,Color.B/255.0f,Alpha };
		//Sprite.Color = ::Color(c).dwVal;			
		float color[] = { Color.R/255.0f,Color.G/255.0f,Color.B/255.0f,Alpha };
#ifdef	TRANSGAMING
		if (GEngine->Video->BloomEnabled && (GEngine->pcfWorld->BloomFX.Multiplier > 0.0F))
		{
			color[0] *= GEngine->pcfWorld->BloomFX.DimScale;
			color[1] *= GEngine->pcfWorld->BloomFX.DimScale;
			color[2] *= GEngine->pcfWorld->BloomFX.DimScale;
		}
				
		Sprite.Color = ::Color_TG(color).dwVal;
#else
		if (GEngine->Video->BloomEnabled && (GEngine->World->BloomFX.Multiplier > 0.0F))
		{
			color[0] *= GEngine->World->BloomFX.DimScale;
			color[1] *= GEngine->World->BloomFX.DimScale;
			color[2] *= GEngine->World->BloomFX.DimScale;
		}
				
		Sprite.Color = ::Color(color).dwVal;
#endif
		Sprite.TexRef = TexPtr ? TexPtr->DeviceId : EInvalidIndex;
		
		dev->SetObjectTransform(GIdentityMatrix);

		RenderDevice* dev=GEngine->GetRenderDevice();
		Assert(dev);

		// set texture state
		TextureState st[2];
		st[0].Set(0,TextureState::ETexOpModulate,TextureState::ETexArgDiffuse,TextureState::ETexFilterBilinear);
		st[0].AlphaOp = TextureState::ETexOpModulate;
		st[0].AlphaArg = TextureState::ETexArgDiffuse;
		st[1].Set(1,TextureState::ETexOpDisable);

		dev->SetTextureState(st,ARRAY_COUNT(st));

		// fill buffer
		Vector Right, Up;

		if (mode & View::EModeRenderRTCubemap)
		{
			Right=cam.GetCubemapRightVector() * Sprite.Size;
			Up   =cam.GetCubemapUpVector()    * Sprite.Size;
		}
		else
		{
			Right=cam.GetRightVector() * Sprite.Size;
			Up   =cam.GetUpVector()    * Sprite.Size;
		}

		Vector cp = Right^Up;
		Matrix zr = RotationMatrix(Sprite.Rotation,cp.X,cp.Y,cp.Z);

		Up = zr.Transform(Up);
		Right = zr.Transform(Right);

#ifdef TRANSGAMING_PS2
        ParticleSystem &partSys = GEngine->Video->ParticleSys;
        Vector *        v = partSys.PBuf.Lock(4);
        DWORD *         c = partSys.CBuf.Lock(4);
        TexCoord *      t = partSys.TBuf.Lock(4);
        int             offset;
        const Vector    left = Sprite.Position - Right;
        const Vector    right = Sprite.Position + Right;
        static const TexCoord T[4] = {
            TexCoord(0, 0),
            TexCoord(1, 0),
            TexCoord(0, 1),
            TexCoord(1, 1)
        };

        v[0] = left + Up;
        v[1] = right + Up;
        v[2] = left - Up;
        v[3] = right - Up;
        memcpy(t, T, sizeof(TexCoord) * 4);
        c[0] = c[1] = c[2] = c[3] = Sprite.Color;


        dev->SetTexture(0, Sprite.TexRef);
        int s[] = { partSys.PBuf.GetStream(), partSys.CBuf.GetStream(), partSys.TBuf.GetStream() };
        dev->SetStreams(s, ARRAY_COUNT(s), EInvalidIndex);
        dev->SetShader(GEngine->Video->MaterialSys->Shaders.Fixed_DCTU1, RenderDevice::EShaderVertex);
        dev->SetShader(EInvalidIndex, RenderDevice::EShaderPixel);
     
        dev->SetFogState(GEngine->pcfWorld->Env.FogParams); // ugly hack: switch to fp'ed fog

        offset = partSys.PBuf.Unlock();

# ifdef DEBUG
        //make sure all the vertex buffers are in sync!
        Assert(partSys.CBuf.Unlock() == offset);
        Assert(partSys.TBuf.Unlock() == offset);
# else
        partSys.CBuf.Unlock();
        partSys.TBuf.Unlock();
# endif
#else
		SimpleVertex* v=GEngine->Video->DecalSys.VBuf.Lock(6);
		v[0]=SimpleVertex(Sprite.Position-Right+Up, Sprite.Color, TexCoord(0,0));
		v[1]=SimpleVertex(Sprite.Position+Right+Up, Sprite.Color, TexCoord(1,0));
		v[2]=SimpleVertex(Sprite.Position+Right-Up, Sprite.Color, TexCoord(1,1));
		v[3]=SimpleVertex(Sprite.Position+Right-Up, Sprite.Color, TexCoord(1,1));
		v[4]=SimpleVertex(Sprite.Position-Right-Up, Sprite.Color, TexCoord(0,1));
		v[5]=SimpleVertex(Sprite.Position-Right+Up, Sprite.Color, TexCoord(0,0));


		dev->SetTexture(0,Sprite.TexRef);
		dev->SetStream(GEngine->Video->DecalSys.VBuf.GetStream());
		dev->SetShader(GEngine->Video->MaterialSys->Shaders.FixedSimple,RenderDevice::EShaderVertex);
		dev->SetShader(EInvalidIndex,RenderDevice::EShaderPixel);
      
# ifdef TRANSGAMING
        dev->SetFogState(GEngine->pcfWorld->Env.FogParams); // ugly hack: switch to fp'ed fog
# else
        dev->SetFogState(GEngine->World->Env.FogParams); // ugly hack: switch to fp'ed fog
# endif
#endif

		RenderState state;
		state.SetCullMode(RenderState::ECullNone);
		state.SetDepthTest(BillboardType & EBillboardCorona ? false : true);
		state.SetDepthWrite(false);
		state.SetBlendMode(BlendMode);
		dev->SetState(state);

#ifdef TRANSGAMING_PS2
        dev->Draw(RenderDevice::EPrimTriStrip, offset, 2);
        dev->SetFogState(GEngine->pcfWorld->Env.FogParams.UseVSFog());
#else
		dev->Draw(RenderDevice::EPrimTriList,GEngine->Video->DecalSys.VBuf.Unlock(),2);
# ifdef TRANSGAMING
        dev->SetFogState(GEngine->pcfWorld->Env.FogParams.UseVSFog());
# else
        dev->SetFogState(GEngine->World->Env.FogParams.UseVSFog());
# endif
#endif
	}
}

void Billboard::Tick(const float delta)
{
	TimeDelta = delta;
}

void Billboard::UpdateBBox()
{
	LocalBox.Reset();
	LocalBox.Expand(max(0.2, Sprite.Size/2));
    Box=LocalBox.Transform(ObjectToWorld);
}

void Billboard::SetTexture(const char* fileName, int flags)
{
    if (fileName)
    {
        TexName = fileName;
		TexPtr = GEngine->Video->MaterialSys->CreateTexture(TexName.AsChar(), flags);
    }
    else
    {
        if (TexPtr)
        {
            GEngine->Video->MaterialSys->ReleaseTexture(TexPtr);
            TexPtr = 0;
        }
    }
}

#define BILLBOARD_LOAD_SAVE_TOKEN 0x5e90

bool Billboard::LoadEntity( FileObject* f )
{
    if( !f->TestToken( BILLBOARD_LOAD_SAVE_TOKEN ) )
    {
        GLog.Print( "Token test failure: Billboard::LoadEntity()\n" );
        return false;
    }
    
    Entity::LoadEntity( f );

    *f >> Translation >> Rotation;

    *f >> BillboardType;
    *f >> BlendMode;

    *f >> TexName;
    *f >> (DWORD&)Color;

    *f >> Timer;
    *f >> FadeInTime;
    *f >> FadeOutTime;

    *f >> Alpha;
    *f >> AlphaMax;    

    *f >> Distance;
    *f >> MaxDistance;
    *f >> MinDistance;
    *f >> MaxRadius;
    *f >> MinRadius;
    *f >> OffDistance;
    *f >> TraceMargin;
    *f >> IsVisible;
    *f >> HitStatus;
    *f >> TimeDelta;
    *f >> TraceInterval;

    Sprite.Size = MaxRadius;
    UpdateTransform();
    SetTexture(TexName.AsChar());
    UpdateBBox();
    return true;
}

bool Billboard::SaveEntity( FileObject* f )
{
    // Data for World to recreate us
    *f << (int)ETypeBillboard << FileName << Name << Scale << (char) (0);

    if( !f->PutToken( BILLBOARD_LOAD_SAVE_TOKEN ) ) return false;

    Entity::SaveEntity( f );

    *f << Translation << Rotation;

    *f << BillboardType;
    *f << BlendMode;

    *f << TexName;
    *f << (DWORD) Color;

    *f << Timer;
    *f << FadeInTime;
    *f << FadeOutTime;

    *f << Alpha;
    *f << AlphaMax;    

    *f << Distance;
    *f << MaxDistance;
    *f << MinDistance;
    *f << MaxRadius;
    *f << MinRadius;
    *f << OffDistance;
    *f << TraceMargin;
    *f << IsVisible;
    *f << HitStatus;
    *f << TimeDelta;
    *f << TraceInterval;

    return true;
}

//============================================================================
#ifndef TRANSGAMING
bool Billboard::DemoReadEntity( FileObject *f )
{
	Entity::DemoReadEntity( f );
	return true;
}


bool Billboard::DemoWriteEntity( FileObject *f )
{
	Entity::DemoWriteEntity( f );
	return true;
}


bool Billboard::DemoReadFull( FileObject *f )
{
	Entity::DemoReadFull( f );

	*f >> BillboardType;
    *f >> BlendMode;

    *f >> TexName;
    *f >> (DWORD&)Color;

    *f >> Timer;
    *f >> FadeInTime;
    *f >> FadeOutTime;

    *f >> Alpha;
    *f >> AlphaMax;    

    *f >> Distance;
    *f >> MaxDistance;
    *f >> MinDistance;
    *f >> MaxRadius;
    *f >> MinRadius;
    *f >> OffDistance;
    *f >> TraceMargin;
    *f >> IsVisible;
    *f >> HitStatus;
    *f >> TimeDelta;
    *f >> TraceInterval;

    Sprite.Size = MaxRadius;
    UpdateTransform();
    SetTexture(TexName.AsChar());
    UpdateBBox();

	return true;
}


bool Billboard::DemoWriteFull( FileObject *f )
{
	Entity::DemoWriteFull( f );
	*f << BillboardType;
    *f << BlendMode;

    *f << TexName;
    *f << (DWORD) Color;

    *f << Timer;
    *f << FadeInTime;
    *f << FadeOutTime;

    *f << Alpha;
    *f << AlphaMax;    

    *f << Distance;
    *f << MaxDistance;
    *f << MinDistance;
    *f << MaxRadius;
    *f << MinRadius;
    *f << OffDistance;
    *f << TraceMargin;
    *f << IsVisible;
    *f << HitStatus;
    *f << TimeDelta;
    *f << TraceInterval;

	return true;
}
#endif
