// -------------------------------------------------------------------------------
// Copyright (c) 2004 People Can Fly. All Rights Reserved.						
// Not for public release.
//
// $Header: /Game/Source/Engine/Render/PostProcess.cpp 16    04-12-16 12:58 Ulkai $
// -------------------------------------------------------------------------------

#include "Engine.h"
#include "Engine/World.h"
#include "Render/Video.h"
#include "Render/Material.h"
#include "Render/HUD.h"
#include "Render/PostProcess.h"
#include "Render/View.h"

static FLOAT GaussianDistribution(const FLOAT X, const FLOAT Y, const FLOAT Rho);

// =========================================================================================================

FPostProcess::FPostProcess()
:	Scale(1.0F), Rho(1.0F)
{
}

void FPostProcess::CalcGauss(FLOAT InScale, FLOAT InRho)
{
	if (InScale != Scale || InRho != Rho)
	{
		// so we don't have to calc again
		Scale	= InScale;
		Rho		= InRho;

		for (INT X=0; X<7; ++X) 
			Gauss[X] = Scale * GaussianDistribution(X, 0, Rho);
	}
}

void FPostProcess::SetGaussWeights()
{
	RenderDevice* Device = GEngine->Video->RenderDev;
#ifdef TRANSGAMING
    Plane tmpPlane;


    for (int i=0; i<7; ++i){
        tmpPlane.set(Vector(Gauss[i]));
		Device->SetPixelShaderMemory(i, &tmpPlane, 1);
	}
#else
	for (int i=0; i<7; ++i)
	{
		Device->SetPixelShaderMemory(i, &Plane(Vector(Gauss[i])), 1);
	}
#endif
}

void FPostProcess::SetGaussHorizOffsets(int Width)
{
	const FLOAT T = 1.25F / (FLOAT) Width;

	Plane Offset[7];
	for (int i=1; i<7; ++i)
		Offset[i] = Plane(FLOAT(i) * T, 0, 0, 0);

	RenderDevice* Device = GEngine->Video->RenderDev;

	// compute offsets
	Device->SetShaderMemory(20, &Offset[1], 1);
	Device->SetShaderMemory(21, &Offset[2], 1);
	Device->SetShaderMemory(22, &Offset[3], 1);

	Device->SetPixelShaderMemory(7, &Offset[4], 1);
	Device->SetPixelShaderMemory(8, &Offset[5], 1);
	Device->SetPixelShaderMemory(9, &Offset[6], 1);

	Device->SetShaderMemory(40, &Offset[4], 1);
	Device->SetShaderMemory(41, &Offset[5], 1);
	Device->SetShaderMemory(42, &Offset[6], 1);
}

void FPostProcess::SetGaussVertOffsets(int Height)
{
	const FLOAT T = 1.25F / (FLOAT) Height;

	Plane Offset[7];
	for (int i=1; i<7; ++i)
		Offset[i] = Plane(0, FLOAT(i) * T, 0, 0);

	RenderDevice* Device = GEngine->Video->RenderDev;

	// compute offsets
	Device->SetShaderMemory(20, &Offset[1], 1);
	Device->SetShaderMemory(21, &Offset[2], 1);
	Device->SetShaderMemory(22, &Offset[3], 1);

	Device->SetPixelShaderMemory(7, &Offset[4], 1);
	Device->SetPixelShaderMemory(8, &Offset[5], 1);
	Device->SetPixelShaderMemory(9, &Offset[6], 1);

	Device->SetShaderMemory(40, &Offset[4], 1);
	Device->SetShaderMemory(41, &Offset[5], 1);
	Device->SetShaderMemory(42, &Offset[6], 1);
}

void FPostProcess::SetOffsets_DownSample2x2(int IndexOffset, int Width, int Height)
{
	RenderDevice* Device = GEngine->Video->RenderDev;

	const FLOAT TU = 1.0F/FLOAT(Width);
	const FLOAT TV = 1.0F/FLOAT(Height);

	INT Index = 0;
	for (INT Y=0; Y<2; ++Y)
	{
		for (INT X=0; X<2; ++X)
		{
			const Plane Pan((X - 0.5F) * TU, (Y - 0.5F) * TV, 0.0F, 0.0F);
			Device->SetPixelShaderMemory(IndexOffset + Index++, &Pan, 1);
		}
	}
}

void FPostProcess::SetOffsets_DownSample4x4(int IndexOffset, int Width, int Height)
{
	RenderDevice* Device = GEngine->Video->RenderDev;

	const FLOAT TU = 1.0F/FLOAT(Width);
	const FLOAT TV = 1.0F/FLOAT(Height);

	INT Index = 0;
	for (INT Y=0; Y<4; ++Y)
	{
		for (INT X=0; X<4; ++X)
		{
			const Plane Pan((X - 1.5F) * TU, (Y - 1.5F) * TV, 0.0F, 0.0F);
			Device->SetPixelShaderMemory(IndexOffset + Index++, &Pan, 1);
		}
	}
}

void FPostProcess::ScreenQuad(Material* Shader, Texture* Src, Texture* Dst)
{
#ifdef TRANSGAMING_PS2
	printf("FPostProcess::ScreenQuad not implimented - fixme!!!\n");
#else
	RenderDevice* Device = GEngine->Video->RenderDev;
	HUD* hud = GEngine->Video->Hud;

	if (Dst)
	{
		Device->SetRenderTarget(Dst, NULL);
		Device->SetViewport(0, 0, Dst->Width, Dst->Height);
	}
	else
	{
		// automatically restores original viewport
		Device->SetFBRenderTarget();
	}

	RenderPass& pass = Shader->Passes[0];	
#ifdef TRANSGAMING
    MaterialInput tmpMat;

	pass.Activate();
	pass.SetInput(&tmpMat);
#else
	pass.Activate();
	pass.SetInput(&MaterialInput());
#endif

	Device->SetScreenTransform(GIdentityMatrix);
	Device->SetCameraTransform(GIdentityMatrix);
    Device->SetObjectTransform(GIdentityMatrix);


	const FLOAT EX = 0.5F/FLOAT(Src->Width);
	const FLOAT EY = 0.5F/FLOAT(Src->Height);

	ScreenVertexNDC* VPtr = (ScreenVertexNDC*) hud->Buf.Lock(4);
	
	VPtr[0] = ScreenVertexNDC(-1,+1,0+EX,0+EY);
	VPtr[1] = ScreenVertexNDC(+1,+1,1+EX,0+EY);
	VPtr[2] = ScreenVertexNDC(-1,-1,0+EX,1+EY);
	VPtr[3] = ScreenVertexNDC(+1,-1,1+EX,1+EY);
	
	Device->SetStream(hud->Buf.GetStream());
	Device->Draw(RenderDevice::EPrimTriStrip, hud->Buf.Unlock(), 2);
#endif
}
// =========================================================================================================

static void SetActiveMaterial(const char* Name)
{
	Material* M = GEngine->Video->MaterialSys->FindMaterial(Name);
	Assert(M);

	RenderPass& pass = M->Passes[0];	
#ifdef TRANSGAMING
    MaterialInput tmpMat;

	pass.Activate();
	pass.SetInput(&tmpMat);
#else
	pass.Activate();
	pass.SetInput(&MaterialInput());
#endif
}

static INT SetupQuadVertexBuffer(int Width, int Height)
{
#ifdef TRANSGAMING_PS2
	printf("SetupQuadVertexBuffer not implimented - fixme!!!\n");
	return(0);
#else
	// DX weirdness
	const FLOAT EX = 0.5F/FLOAT(Width);
	const FLOAT EY = 0.5F/FLOAT(Height);

	HUD* Hud = GEngine->Video->Hud;
	ScreenVertexNDC* V = (ScreenVertexNDC*) Hud->Buf.Lock(4);
	
	V[0] = ScreenVertexNDC(-1,+1,0+EX,0+EY);
	V[1] = ScreenVertexNDC(+1,+1,1+EX,0+EY);
	V[2] = ScreenVertexNDC(-1,-1,0+EX,1+EY);
	V[3] = ScreenVertexNDC(+1,-1,1+EX,1+EY);

	const INT Start = Hud->Buf.Unlock();
	
	RenderDevice* Device = GEngine->Video->RenderDev;
	Device->SetStream(Hud->Buf.GetStream());
	return Start;
#endif
}

void FPostProcess::RenderBloomPS11()
{
	Material* M;
	Texture* Src, *Dst;

#ifdef	TRANSGAMING
    Plane tmpPlane;
	const World::TBloomFX& BloomFX = GEngine->pcfWorld->BloomFX;
#else
	const World::TBloomFX& BloomFX = GEngine->World->BloomFX;
#endif

	CalcGauss(BloomFX.Multiplier, 4.0F);

	RenderDevice* Device = GEngine->Video->RenderDev;
	//HUD* hud = GEngine->Video->Hud;

	RenderState BlendNone;
	BlendNone.SetBlendMode(RenderState::EBlendNone);

	RenderState BlendAdd;
	BlendAdd.SetBlendMode(RenderState::EBlendAdd);

	// bright pass
	M = GEngine->Video->MaterialSys->FindMaterial("bright_pass");
	Assert(M);

#ifdef	TRANSGAMING
	Src = GEngine->pcfWorld->TexFB[0];
	Dst = GEngine->pcfWorld->TexFB[1];
#else
	Src = GEngine->World->TexFB[0];
	Dst = GEngine->World->TexFB[1];
#endif

	//Device->SetPixelShaderMemory(0, &Plane(BloomFX.LuminanceThreshold, 0, 0, 0), 1);
#ifdef TRANSGAMING
    tmpPlane.set(0.3F, 0.6F, 0.11F, BloomFX.LuminanceThreshold);
	Device->SetPixelShaderMemory(0, &tmpPlane, 1);
#else
	Device->SetPixelShaderMemory(0, &Plane(0.3F, 0.6F, 0.11F, BloomFX.LuminanceThreshold), 1);
#endif
	ScreenQuad(M, Src, Dst);

	// render 
#ifdef	TRANSGAMING
	Src = GEngine->pcfWorld->TexFB[1];
	Dst = GEngine->pcfWorld->TexFB[2];
#else
	Src = GEngine->World->TexFB[1];
	Dst = GEngine->World->TexFB[2];
#endif

	// setup render target
	Device->SetRenderTarget(Dst, NULL);
	Device->SetViewport(0, 0, Dst->Width, Dst->Height);

	const INT VStart = SetupQuadVertexBuffer(Src->Width, Src->Height);

	// setup GTexelWeight
	for (int i=0; i<5; ++i)
	{
#ifdef TRANSGAMING
        tmpPlane.set(Vector(Gauss[i]));

		Device->SetPixelShaderMemory(i, &tmpPlane, 1);
		Device->SetShaderMemory(i+40, &tmpPlane, 1);
#else
		Device->SetPixelShaderMemory(i, &Plane(Vector(Gauss[i])), 1);
		Device->SetShaderMemory(i+40, &Plane(Vector(Gauss[i])), 1);
#endif
	}

	// setup GTapOffset
	const FLOAT TU = 1.0F/FLOAT(Src->Width);
	const FLOAT TV = 1.0F/FLOAT(Src->Height);

	SetActiveMaterial("gauss_accum_fb1");

	// accum right (plus)
#ifdef TRANSGAMING
    for (int i=0; i<4; ++i){
        tmpPlane.set((i+1) * TU, 0, 0, 0);
		Device->SetShaderMemory(20+i, &tmpPlane, 1);
    }
#else
    for (int i=0; i<4; ++i)
		Device->SetShaderMemory(20+i, &Plane((i+1) * TU, 0, 0, 0), 1);
#endif

	Device->Draw(RenderDevice::EPrimTriStrip, VStart, 2);

	// add left side
	Device->SetState(BlendAdd);

	// accum left (minus)
#ifdef TRANSGAMING
    for (int i=0; i<4; ++i){
        tmpPlane.set((i-1) * TU, 0, 0, 0);
		Device->SetShaderMemory(20+i, &tmpPlane, 1);
    }
#else
	for (int i=0; i<4; ++i)
		Device->SetShaderMemory(20+i, &Plane((i-1) * TU, 0, 0, 0), 1);
#endif

	Device->Draw(RenderDevice::EPrimTriStrip, VStart, 2);

	// accum center
	SetActiveMaterial("gauss_center_fb1");
	Device->SetState(BlendAdd);
	Device->Draw(RenderDevice::EPrimTriStrip, VStart, 2);

	// -----------------------
	// switch render targets
	Device->SetRenderTarget(Src, NULL);
	Device->SetViewport(0, 0, Src->Width, Src->Height);

	SetActiveMaterial("gauss_accum_fb2");

	// accum bottom (plus)
#ifdef TRANSGAMING
    for (int i=0; i<4; ++i){
        tmpPlane.set(0, (i+1) * TV, 0, 0);
		Device->SetShaderMemory(20+i, &tmpPlane, 1);
    }
#else
	for (int i=0; i<4; ++i)
		Device->SetShaderMemory(20+i, &Plane(0, (i+1) * TV, 0, 0), 1);
#endif

	Device->Draw(RenderDevice::EPrimTriStrip, VStart, 2);

	// add top side
	Device->SetState(BlendAdd);

	// accum top (minus)
#ifdef TRANSGAMING
    for (int i=0; i<4; ++i){
        tmpPlane.set(0, (i-1) * TV, 0, 0);
		Device->SetShaderMemory(20+i, &tmpPlane, 1);
    }
#else
	for (int i=0; i<4; ++i)
		Device->SetShaderMemory(20+i, &Plane(0, (i-1) * TV, 0, 0), 1);
#endif

	Device->Draw(RenderDevice::EPrimTriStrip, VStart, 2);

	// accum center
	SetActiveMaterial("gauss_center_fb2");
	Device->SetState(BlendAdd);
	Device->Draw(RenderDevice::EPrimTriStrip, VStart, 2);

	// reset back to normal
	Device->SetState(BlendNone);
	Device->SetFBRenderTarget();
}

void FPostProcess::RenderGauss()
{
#ifdef TRANSGAMING_PS2
	printf("FPostProcess::RenderGauss not implimented - fixme!!!\n");
#else
	RenderDevice* Device = GEngine->Video->RenderDev;
	HUD* hud = GEngine->Video->Hud;
	
#ifdef	TRANSGAMING
	Texture* Src = GEngine->pcfWorld->TexFB[1];
	Texture* Dst = GEngine->pcfWorld->TexFB[2];
#else
	Texture* Src = GEngine->World->TexFB[1];
	Texture* Dst = GEngine->World->TexFB[2];
#endif

	// setup render target
	Device->SetRenderTarget(Dst, NULL);
	Device->SetViewport(0, 0, Dst->Width, Dst->Height);

	// fill and set buffer
	const FLOAT EX = 0.5F/FLOAT(Src->Width);
	const FLOAT EY = 0.5F/FLOAT(Src->Height);

	ScreenVertexNDC* VPtr = (ScreenVertexNDC*) hud->Buf.Lock(4);
	
	VPtr[0] = ScreenVertexNDC(-1,+1,0+EX,0+EY);
	VPtr[1] = ScreenVertexNDC(+1,+1,1+EX,0+EY);
	VPtr[2] = ScreenVertexNDC(-1,-1,0+EX,1+EY);
	VPtr[3] = ScreenVertexNDC(+1,-1,1+EX,1+EY);

	const INT VStart = hud->Buf.Unlock();

	Device->SetStream(hud->Buf.GetStream());

	SetActiveMaterial("gauss_fb1");

	SetGaussWeights();
	SetGaussHorizOffsets(Src->Width);

	Device->Draw(RenderDevice::EPrimTriStrip, VStart, 2);

	// switch render target
#ifdef	TRANSGAMING
	Src = GEngine->pcfWorld->TexFB[2];
	Dst = GEngine->pcfWorld->TexFB[1];
#else
	Src = GEngine->World->TexFB[2];
	Dst = GEngine->World->TexFB[1];
#endif

	Device->SetRenderTarget(Dst, NULL);
	Device->SetViewport(0, 0, Dst->Width, Dst->Height);

	SetActiveMaterial("gauss_fb2");

	SetGaussVertOffsets(Src->Height);

	Device->Draw(RenderDevice::EPrimTriStrip, VStart, 2);
#endif
}

#ifdef	TRANSGAMING
void FPostProcess::RenderBloom()
{
	Material* M;
	Texture* Src, *Dst;
	
	const World::TBloomFX& BloomFX = GEngine->pcfWorld->BloomFX;
	RenderDevice* Device = GEngine->Video->RenderDev;
    Plane tmpPlane(0.3f, 0.6f, 0.11f, BloomFX.LuminanceThreshold);

	M = GEngine->Video->MaterialSys->FindMaterial("bright_pass");

	Src = GEngine->pcfWorld->TexFB[0];
	Dst = GEngine->pcfWorld->TexFB[1];

	//Device->SetPixelShaderMemory(0, &Plane(BloomFX.LuminanceThreshold, 0, 0, 0), 1);
	Device->SetPixelShaderMemory(0, &tmpPlane, 1);
	ScreenQuad(M, Src, Dst);

	M = GEngine->Video->MaterialSys->FindMaterial("gauss_fb1");

	Src = GEngine->pcfWorld->TexFB[1];
	Dst = GEngine->pcfWorld->TexFB[2];

	CalcGauss(BloomFX.Multiplier, 4.0F);

	SetGaussWeights();
	SetGaussHorizOffsets(Src->Width);

	ScreenQuad(M, Src, Dst);

	M = GEngine->Video->MaterialSys->FindMaterial("gauss_fb2");

	Src = GEngine->pcfWorld->TexFB[2];
	Dst = GEngine->pcfWorld->TexFB[1];

	SetGaussWeights();
	SetGaussVertOffsets(Src->Height);

	ScreenQuad(M, Src, Dst);

	Device->SetFBRenderTarget();
}
#else
void FPostProcess::RenderBloom()
{
	Material* M;
	Texture* Src, *Dst;
	
	const World::TBloomFX& BloomFX = GEngine->World->BloomFX;
	RenderDevice* Device = GEngine->Video->RenderDev;

	M = GEngine->Video->MaterialSys->FindMaterial("bright_pass");

	Src = GEngine->World->TexFB[0];
	Dst = GEngine->World->TexFB[1];

	//Device->SetPixelShaderMemory(0, &Plane(BloomFX.LuminanceThreshold, 0, 0, 0), 1);
	Device->SetPixelShaderMemory(0, &Plane(0.3F, 0.6F, 0.11F, BloomFX.LuminanceThreshold), 1);
	ScreenQuad(M, Src, Dst);

	M = GEngine->Video->MaterialSys->FindMaterial("gauss_fb1");

	Src = GEngine->World->TexFB[1];
	Dst = GEngine->World->TexFB[2];

	CalcGauss(BloomFX.Multiplier, 4.0F);

	SetGaussWeights();
	SetGaussHorizOffsets(Src->Width);

	ScreenQuad(M, Src, Dst);

	M = GEngine->Video->MaterialSys->FindMaterial("gauss_fb2");

	Src = GEngine->World->TexFB[2];
	Dst = GEngine->World->TexFB[1];

	SetGaussWeights();
	SetGaussVertOffsets(Src->Height);

	ScreenQuad(M, Src, Dst);

	Device->SetFBRenderTarget();
}
#endif

// =========================================================================================================

static FLOAT GaussianDistribution(const FLOAT X, const FLOAT Y, const FLOAT Rho)
{
    FLOAT G = 1.0F / sqrtf(2.0F * F_PI * Rho * Rho);
    G *= expf(-(X*X + Y*Y)/(2.0F*Rho*Rho));

    return G;
}
