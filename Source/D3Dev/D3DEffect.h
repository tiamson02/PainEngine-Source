#ifndef __D3DEffect_h__
#define __D3DEffect_h__

#include "D3Dev.h"
#include <d3dx9.h>

class D3DEffect : public RenderEffect
{
public:
	D3DEffect(LPD3DXEFFECT InEffect);
	~D3DEffect();

	BOOL	EnumTechniques(DynamicArray<RenderTechnique>& Out);
	DWORD	GetParamIndex(const char* Name);
	DWORD	GetTextureIndex(const char* Name);
	BOOL	GetTechniqueString(DWORD Technique, const char* Name, String& Value);

	BOOL	SetParam(const char* Name, const void* Data, DWORD Size);
	BOOL	SetParam(DWORD Index, const void* Data, DWORD Size);
	BOOL	SetTexture(const char* Name, Texture* Tex);
	BOOL	SetTexture(DWORD Index, Texture* Tex);

	BOOL	SetTechnique(DWORD Index);
	UINT	Begin();
	BOOL	BeginPass(UINT Num);
	BOOL	EndPass();
	BOOL	End();

private:
	LPD3DXEFFECT	Effect;
};

#endif // __D3DEffect_h__

