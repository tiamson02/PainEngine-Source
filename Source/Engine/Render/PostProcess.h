// ----------------------------------------------------------------------------
// Copyright (c) 2004 People Can Fly. All Rights Reserved.						
// Not for public release.
//
// $Header: /Game/Source/Engine/Render/PostProcess.h 6     10/13/04 6:24p Bart $
// ----------------------------------------------------------------------------

#ifndef __PostProcess_h__
#define __PostProcess_h__

class Material;
class Texture;

class FPostProcess
{
public:
	FPostProcess();

	void CalcGauss(FLOAT Scale, FLOAT Rho);
	void SetGaussWeights();
	void SetGaussHorizOffsets(int Width);
	void SetGaussVertOffsets(int Height);	

	void ScreenQuad(Material* Shader, Texture* Src, Texture* Dst);

	void SetOffsets_DownSample2x2(int IndexOffset, int Width, int Height);
	void SetOffsets_DownSample4x4(int IndexOffset, int Width, int Height);

	void RenderGauss();

	void RenderBloom();
	void RenderBloomPS11();

public:
	FLOAT	Scale, Rho;
	FLOAT	Gauss[7];
};

#endif // __PostProcess_h__


