#include "D3DEffect.h"
#include "D3Dev.h"

D3DEffect::D3DEffect(LPD3DXEFFECT InEffect)
:	Effect(InEffect)
{
	Assert(Effect);
}

D3DEffect::~D3DEffect()
{
	SAFE_RELEASE(Effect);
}

BOOL D3DEffect::EnumTechniques(DynamicArray<RenderTechnique>& Out)
{
	HRESULT hr;

	D3DXHANDLE Handle = NULL;		

	for (D3DXHANDLE Technique = NULL; ; Handle = Technique)
	{
		hr = Effect->FindNextValidTechnique(Handle, &Technique);
		if (!Technique)
			break;

		D3DXTECHNIQUE_DESC Desc;
		hr = Effect->GetTechniqueDesc(Technique,&Desc);
		if (FAILED(hr))
			return 0;
		
		Assert(Desc.Passes<=MAX_EFFECT_PASSES);

		RenderTechnique* T = Out.Create();

		T->Index		= (DWORD) Technique;
		T->Name			= Desc.Name;
		T->NumPasses	= Desc.Passes;

		for (UINT Index=0; Index<Desc.Passes; ++Index)
		{
			D3DXHANDLE Pass = Effect->GetPass(Technique, Index);
			D3DXPASS_DESC PassDesc;
			hr = Effect->GetPassDesc(Pass, &PassDesc);
			if (SUCCEEDED(hr))
			{
				EffectPass& P = T->Passes[Index];
				P.Name			= PassDesc.Name;
				P.VertShader	= PassDesc.pVertexShaderFunction;
				P.FragShader	= PassDesc.pPixelShaderFunction;
			}
			else
			{
				//D3DLog->Print("ERROR: could not get DESC for Pass %d of Technique %s\n", Index, T->Name.AsChar());
				return 0;
			}
		}
	}    

	return 1;
}

DWORD D3DEffect::GetParamIndex(const char* Name)
{
	D3DXHANDLE Handle = Effect->GetParameterByName(NULL, Name);
	return (DWORD) Handle;
}

DWORD D3DEffect::GetTextureIndex(const char* Name)
{
	return 0;
}

BOOL D3DEffect::GetTechniqueString(DWORD Technique, const char* Name, String& Value)
{
	Assert(Name);
	D3DXHANDLE Handle = Effect->GetAnnotationByName((D3DXHANDLE) Technique, Name);
	if (Handle) 
	{
		const char* Temp = 0;
		HRESULT hr = Effect->GetString(Handle, &Temp);
		if (SUCCEEDED(hr) && Temp)
		{
			Value = Temp;
			return 1;
		}
	}

	return 0;
}

BOOL D3DEffect::SetParam(const char* Name, const void* Data, DWORD Size)
{
	return 0;
}

BOOL D3DEffect::SetParam(DWORD Index, const void* Data, DWORD Size)
{
	HRESULT hr = Effect->SetValue
	(
		(D3DXHANDLE) Index,
		Data,
		(UINT) Size
	);

	return SUCCEEDED(hr);
}

BOOL D3DEffect::SetTexture(const char* Name, Texture* Tex)
{
	return 0;
}

BOOL D3DEffect::SetTexture(DWORD Index, Texture* Tex)
{
	/*
	D3DTexture* BaseTex = static_cast<D3DTexture*>(Texture);
	HRESULT hr = Effect->SetTexture
	(
		(D3DXHANDLE) Index,
		*BaseTex
	);

	return SUCCEEDED(hr);
	*/
	return 0;
}

BOOL D3DEffect::SetTechnique(DWORD Index)
{
	//LPDIRECT3DDEVICE9 Device;
	//Effect->GetDevice(&Device);
	//Device->SetFVF(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1);

	HRESULT hr = Effect->SetTechnique((D3DXHANDLE) Index);
	return SUCCEEDED(hr);
}

UINT D3DEffect::Begin()
{
	DWORD Flags = D3DXFX_DONOTSAVESTATE|D3DXFX_DONOTSAVESAMPLERSTATE|D3DXFX_DONOTSAVESHADERSTATE;
	UINT NumPasses;

	HRESULT hr = Effect->Begin(&NumPasses, Flags);
    if (SUCCEEDED(hr))
		return NumPasses;

	return 0;
}

BOOL D3DEffect::BeginPass(UINT Num)
{
	return SUCCEEDED(Effect->BeginPass(Num));
}

BOOL D3DEffect::EndPass()
{
	return SUCCEEDED(Effect->EndPass());
}

BOOL D3DEffect::End()
{
	return SUCCEEDED(Effect->End());
}

