#include "D3Dev.h"
#include "dxerr9.h"

#include "Render/Material.h"

static void ClassifyMemoryStats(Texture* texture)
{
	char *names[] = {
		"decals", "hud", "items", "levels", "models", "particles", "skies", "special", 0
	};

	for (int i=0; names[i]; i++) 
	{
		if (texture->FileName.Find(names[i])) 
		{
			texture->MemoryStats = RenderDevice::EMemTexDecals+i;

			if (texture->MemoryStats == RenderDevice::EMemTexModels &&  
				texture->LevelOfDetail == RenderDevice::ETexLODWeapons) 
			{
				texture->MemoryStats = RenderDevice::EMemTexWeapons;
			}
		}
	}
}

static inline D3DFORMAT FlagsToFormat(DWORD Flags)
{
	D3DFORMAT Format = D3DFMT_X8R8G8B8;
	if (Flags & Texture::ETexRaw8)
		Format = D3DFMT_P8;
	else if (Flags & Texture::ETexRaw32Alpha)
		Format = D3DFMT_A8R8G8B8;

	return Format;
}

static inline D3DFORMAT GetD3DFormat(BYTE Format)
{
	D3DFORMAT Fmt = D3DFMT_A8R8G8B8;
	switch (Format)
	{
	case Texture::ETexFmt_A8R8G8B8:			return D3DFMT_A8R8G8B8;
	case Texture::ETexFmt_A8:				return D3DFMT_A8;
	case Texture::ETexFmt_A16B16G16R16:		return D3DFMT_A16B16G16R16;
	// floating point
	case Texture::ETexFmt_F16_ABGR:			return D3DFMT_A16B16G16R16F;
	case Texture::ETexFmt_F16_R:			return D3DFMT_R16F;
	case Texture::ETexFmt_F16_GR:			return D3DFMT_G16R16F;
	case Texture::ETexFmt_F32_ARGB:			return D3DFMT_A32B32G32R32F;
	case Texture::ETexFmt_F32_R:			return D3DFMT_R32F;
	case Texture::ETexFmt_F32_GR:			return D3DFMT_G32R32F;
	}	

	return Fmt;
}

// ==========================================================================
// internal
// create texture for use as a render target
// ==========================================================================

void D3DRenderDevice::CreateRenderTarget(Texture* texture)
{
    LPDIRECT3DTEXTURE9 pTex=0;
    HRESULT hr;

	if (texture->Width<= 0 || texture->Height<= 0)
    {
		D3DLog.Print("ERROR: invalid W,H for render target (%d,%d)\n",
                     texture->Width,texture->Height);
        return;
	}

    hr = d3dev->CreateTexture
    (
		texture->Width,
        texture->Height,
        1, // levels
        D3DUSAGE_RENDERTARGET/*|D3DUSAGE_AUTOGENMIPMAP*/,
        //D3DFMT_A8R8G8B8,
		GetD3DFormat(texture->Format),
        D3DPOOL_DEFAULT,
        &pTex,
		NULL
	);

    if (SUCCEEDED(hr))
	{	
	    int texBytes=0;
		int w=0,h=0;

		for (unsigned int i=0; i<pTex->GetLevelCount(); ++i) 
        {
		    D3DSURFACE_DESC Desc;
			if (SUCCEEDED(pTex->GetLevelDesc(i,&Desc))) 
            {
				int size = Desc.Width*Desc.Height*SurfFormatToBPP(Desc.Format)/8;
			    TexBytes+=size;
				texBytes+=size;
				if (i==0) 
                {
				    w=Desc.Width;
					h=Desc.Height;
			    }
            }
        }

		D3DTexture texDef(texture,pTex);
		const int index = Textures.Add(texDef);
		ResTextures.Add(index);

#ifdef ANNOYING_GRAPHICS_LOGS
		D3DLog.Print("RenderTargetTex[%d]:\t%.2f MB/%.2f MB free (%dx%d mip %d)\n",
                     index,(float)texBytes/(1024.f*1024.f),
					 d3dev->GetAvailableTextureMem()/(1024.f*1024.f),
					 w,h,pTex->GetLevelCount());
#endif

		texture->OriginalWidth  = w;
		texture->OriginalHeight = h;
		texture->Width			= w;
		texture->Height			= h;
		texture->PixelBytes		= texBytes;
		texture->DeviceId		= index;
	}
    else
		D3DLog.Print("ERROR: could not create %s target %dx%d (%s) %.2f MB free\n",
                     ((texture->Type == Texture::ETexRenderTarget) ? "render" : "depth"),
                     texture->Width,texture->Height,
					 DXGetErrorDescription9A(hr),
					 d3dev->GetAvailableTextureMem()/(1024.f*1024.f));
}
// ==========================================================================
// internal
// create texture for use as a render target
// ==========================================================================

void D3DRenderDevice::CreateDepthSurface(Texture* texture)
{
    LPDIRECT3DSURFACE9 pSurf=0;
    HRESULT hr;

	if (texture->Width<= 0 || texture->Height<= 0)
    {
		D3DLog.Print("ERROR: invalid W,H for depth surface (%d,%d)\n",
                     texture->Width,texture->Height);
        return;
	}

    hr = d3dev->CreateDepthStencilSurface
    (
		texture->Width,
        texture->Height,
		D3DFMT_D24S8,
		D3DMULTISAMPLE_NONE,
		0,			// multisample quality
		TRUE,		// discardable
		&pSurf,
		NULL
	);

    if (SUCCEEDED(hr))
	{	
	    int texBytes=0;
		int w=0,h=0;

		D3DSURFACE_DESC Desc;
		if (SUCCEEDED(pSurf->GetDesc(&Desc))) 
        {
			int size = Desc.Width*Desc.Height*SurfFormatToBPP(Desc.Format)/8;
			TexBytes+=size;
			texBytes+=size;
			w=Desc.Width;
			h=Desc.Height;
        }

		D3DTexture texDef(texture,pSurf);
		const int index = Textures.Add(texDef);
		ResTextures.Add(index);

#ifdef ANNOYING_GRAPHICS_LOGS
		D3DLog.Print("DepthTargetSurface[%d]:\t%.2f MB/%.2f MB free (%dx%d)\n",
                     index,(float)texBytes/(1024.f*1024.f),
					 d3dev->GetAvailableTextureMem()/(1024.f*1024.f),
					 w,h);
#endif

		texture->OriginalWidth  = w;
		texture->OriginalHeight = h;
		texture->Width			= w;
		texture->Height			= h;
		texture->PixelBytes		= texBytes;
		texture->DeviceId		= index;
	}
    else
		D3DLog.Print("ERROR: could not create depth target %dx%d (%s) %.2f MB free\n",
                     texture->Width,texture->Height,
					 DXGetErrorDescription9A(hr),
					 d3dev->GetAvailableTextureMem()/(1024.f*1024.f));
}

// ==========================================================================
// internal
// create regular texture from file
// ==========================================================================
void D3DRenderDevice::CreateRegularTextureFromFile(Texture* texture)
{
    LPDIRECT3DTEXTURE9 pTex=0;
    HRESULT hr;

	D3DXIMAGE_INFO info;	// image info

	hr=D3DXGetImageInfoFromFileInMemory(
		texture->Data,
		texture->Size,
		&info
	);

	int w = info.Width;
	int h = info.Height;

	if (!(texture->Flags & Texture::ETexNoLOD))
	{
		// apply texture lod
		int lod = TexLOD[texture->LevelOfDetail];
		w >>= lod;
		h >>= lod;

		const int MinSize = 32;
		if (w<MinSize && info.Width>MinSize)  w = MinSize;
		if (h<MinSize && info.Height>MinSize) h = MinSize;
	}

	D3DFORMAT internalFormat = D3DFMT_UNKNOWN; // use file format 
/*
	if ((info.Format == D3DFMT_DXT1) && (HWClass == EHWClassNV20)) {
		// Nvidia bug workaround: enforce internal DXT3 for DXT1 textures on GF3
		internalFormat = D3DFMT_DXT3;
	}
*/
	hr=D3DXCreateTextureFromFileInMemoryEx
	(
		d3dev,
		texture->Data,
		texture->Size,
		w,	
		h,	
		(texture->Flags & Texture::ETexNoMipMaps) ? 1 : D3DX_DEFAULT,
		0, 
		internalFormat,		
		D3DPOOL_MANAGED, 
		D3DX_DEFAULT,
		D3DX_FILTER_BOX|D3DX_FILTER_DITHER,
		0, 
		&info, 
		NULL,
		&pTex
	);

	if (SUCCEEDED(hr)) 
	{
	    int texBytes=0;
		int w=0,h=0;

		for (unsigned int i=0; i<pTex->GetLevelCount(); ++i) 
        {
		    D3DSURFACE_DESC Desc;
			if (SUCCEEDED(pTex->GetLevelDesc(i,&Desc))) 
            {
				int size = Desc.Width*Desc.Height*SurfFormatToBPP(Desc.Format)/8;
			    TexBytes+=size;
				texBytes+=size;
				if (i==0) 
                {
				    w=Desc.Width;
					h=Desc.Height;
			    }
            }
        }

		D3DTexture texDef(texture,pTex);
		const int index = Textures.Add(texDef);

#ifdef ANNOYING_GRAPHICS_LOGS
		D3DLog.Print("RegularTex[%d]:\t%.2f MB/%.2f MB free %s (%dx%d mip %d)->(%dx%d mip %d)\n",
                     index,(float)texBytes/(1024.f*1024.f),
					 d3dev->GetAvailableTextureMem()/(1024.f*1024.f),
					 texture->FileName.AsChar(),
					 info.Width,info.Height,info.MipLevels,
					 w,h,pTex->GetLevelCount());
#endif

		ClassifyMemoryStats(texture);

		TextureMemory[texture->MemoryStats] += (float)texBytes/(1024.f*1024.f);

		texture->OriginalWidth  = info.Width;
		texture->OriginalHeight = info.Height;
		texture->Width			= w;
		texture->Height			= h;
		texture->PixelBytes		= texBytes;
		texture->DeviceId		= index;
    }
}

// ==========================================================================
// internal
// create dudv texture from file
// ==========================================================================
void D3DRenderDevice::CreateDuDvTextureFromFile(Texture* texture)
{
    LPDIRECT3DTEXTURE9 pTexSrc=0, pTex=0;
    HRESULT hr;

	D3DXIMAGE_INFO info;	// image info
	hr=D3DXCreateTextureFromFileInMemoryEx
	(
		d3dev,
		texture->Data,
		texture->Size,
		D3DX_DEFAULT,		// taken from file
		D3DX_DEFAULT,		// taken from file
		(texture->Flags & Texture::ETexNoMipMaps) ? 1 : D3DX_DEFAULT,
		0, 
		D3DFMT_UNKNOWN,		// use file format 
		D3DPOOL_MANAGED, 
		D3DX_DEFAULT,
		D3DX_FILTER_BOX|D3DX_FILTER_DITHER,
		0, 
		&info, 
		NULL,
		&pTexSrc
	);

	if (FAILED(hr)) 
		return;

	D3DLOCKED_RECT LockedSrc, Locked;
	D3DSURFACE_DESC ddsdDesc;

	pTexSrc->GetLevelDesc(0, &ddsdDesc);

	hr = d3d->CheckDeviceFormat
	(
		AdapterOrdinal,
		D3DDEVTYPE_HAL,
		D3DFMT_X8R8G8B8,
		0, // usage
		D3DRTYPE_TEXTURE,
		D3DFMT_V8U8
	);

	if (FAILED(hr))
	{
		D3DLog.Print("ERROR:\tCheckDeviceFormat on dudv texture returned \"%s\" (%s).\n",
			DXGetErrorDescription9A(hr),
			texture->FileName.AsChar());
		pTex = pTexSrc;
	}
	else
	{
		hr = d3dev->CreateTexture
		(
			ddsdDesc.Width,
			ddsdDesc.Height,
			(texture->Flags & Texture::ETexNoMipMaps) ? 1 : 0,	// levels
			0, // usage
			D3DFMT_V8U8,
			D3DPOOL_MANAGED,
			&pTex,
			NULL
		);

		pTexSrc->LockRect(0, &LockedSrc, NULL, 0);
		pTex->LockRect(0, &Locked, NULL, 0);

		for (unsigned int y = 0; y < ddsdDesc.Height; ++y)
		{
			for (unsigned int x = 0; x < ddsdDesc.Width; ++x)
			{
				UCHAR* pBitsSrc = (UCHAR*)((DWORD*)((BYTE*)LockedSrc.pBits + (y * LockedSrc.Pitch)) + x);

				UCHAR G = pBitsSrc[1]; 
				UCHAR R = pBitsSrc[2]; 

				CHAR* pBits = (CHAR*)((WORD*)((BYTE*)Locked.pBits + (y * Locked.Pitch)) + x);

				pBits[0] = *((CHAR*)&R);
				pBits[1] = *((CHAR*)&G);
			}
		}

		pTex->UnlockRect(0);
		pTexSrc->UnlockRect(0);

		SAFE_RELEASE(pTexSrc);
	}
	        
	D3DTexture texDef(texture,pTex);
	const int index = Textures.Add(texDef);

	int texBytes=0;
	int w=0,h=0;

	for (unsigned int i=0; i<pTex->GetLevelCount(); ++i) 
    {
		D3DSURFACE_DESC Desc;
		if (SUCCEEDED(pTex->GetLevelDesc(i,&Desc))) 
        {
			int size = Desc.Width*Desc.Height*SurfFormatToBPP(Desc.Format)/8;
			TexBytes+=size;
			texBytes+=size;
			if (i==0) 
            {
				w=Desc.Width;
				h=Desc.Height;
			}
        }
    }

#ifdef ANNOYING_GRAPHICS_LOGS
	D3DLog.Print("RegularTex[%d]:\t%.2f MB/%.2f MB free %s (%dx%d mip %d)->(%dx%d mip %d)\n",
                    index,(float)texBytes/(1024.f*1024.f),
					d3dev->GetAvailableTextureMem()/(1024.f*1024.f),
					texture->FileName.AsChar(),
					info.Width,info.Height,info.MipLevels,
					w,h,pTex->GetLevelCount());
#endif

	ClassifyMemoryStats(texture);

	TextureMemory[texture->MemoryStats] += (float)texBytes/(1024.f*1024.f);
		
	texture->OriginalWidth  = info.Width;
	texture->OriginalHeight = info.Height;
	texture->Width			= w;
	texture->Height			= h;
	texture->PixelBytes		= texBytes;
	texture->DeviceId		= index;
}

// ==========================================================================

static HRESULT CreateNormalizationCubeMap(LPDIRECT3DDEVICE9 pD3DDev, DWORD dwWidth, WORD dwMipmaps, LPDIRECT3DCUBETEXTURE9* pTextureReturn)
{
	HRESULT hr;
	LPDIRECT3DCUBETEXTURE9 pCubeTexture;

	*pTextureReturn = NULL;

	hr = D3DXCreateCubeTexture
    (
        pD3DDev, 
        dwWidth, 
        dwMipmaps, 
        0, 
        D3DFMT_X8R8G8B8, 
        D3DPOOL_MANAGED, 
        &pCubeTexture
    );

	if(FAILED(hr))
	{
		return hr;
	}

	for (int i = 0; i < 6; ++i)
	{
		D3DLOCKED_RECT Locked;
		D3DXVECTOR3 Normal;
		float w,h;
		D3DSURFACE_DESC ddsdDesc;
		
		pCubeTexture->GetLevelDesc(0, &ddsdDesc);

		pCubeTexture->LockRect((D3DCUBEMAP_FACES)i, 0, &Locked, NULL, 0);

		for (unsigned int y = 0; y < ddsdDesc.Height; ++y)
		{
			h = (float)y / ((float)(ddsdDesc.Height - 1));
			h *= 2.0f;
			h -= 1.0f;

			for (unsigned int x = 0; x < ddsdDesc.Width; ++x)
			{
				w = (float)x / ((float)(ddsdDesc.Width - 1));
				w *= 2.0f;
				w -= 1.0f;

				DWORD* pBits = (DWORD*)((BYTE*)Locked.pBits + (y * Locked.Pitch));
				pBits += x;

				switch((D3DCUBEMAP_FACES)i)
				{
					case D3DCUBEMAP_FACE_POSITIVE_X:
						Normal = D3DXVECTOR3(1.0f, -h, -w);
						break;
					case D3DCUBEMAP_FACE_NEGATIVE_X:
						Normal = D3DXVECTOR3(-1.0f, -h, w);
						break;
					case D3DCUBEMAP_FACE_POSITIVE_Y:
						Normal = D3DXVECTOR3(w, 1.0f, h);
						break;
					case D3DCUBEMAP_FACE_NEGATIVE_Y:
						Normal = D3DXVECTOR3(w, -1.0f, -h);
						break;
					case D3DCUBEMAP_FACE_POSITIVE_Z:
						Normal = D3DXVECTOR3(w, -h, 1.0f);
						break;
					case D3DCUBEMAP_FACE_NEGATIVE_Z:
						Normal = D3DXVECTOR3(-w, -h, -1.0f);
						break;
					default:
						break;
				}

				D3DXVec3Normalize(&Normal, &Normal);

				// Scale to be a color from 0 to 255 (127 is 0)
				Normal += D3DXVECTOR3(1.0f, 1.0f, 1.0f);
				Normal *= 127.0f;

				// Store the color
				*pBits = (DWORD)(((DWORD)Normal.x << 16) | ((DWORD)Normal.y << 8) | ((DWORD)Normal.z << 0));

			}
		}
		pCubeTexture->UnlockRect((D3DCUBEMAP_FACES)i, 0);
	}

	if ((dwMipmaps == 0) || (dwMipmaps > 1))
		D3DXFilterCubeTexture(pCubeTexture, NULL, 0, D3DX_FILTER_LINEAR);

	*pTextureReturn = pCubeTexture;

	return S_OK;
}

// ==========================================================================
// internal
// create cubemap texture from file
// ==========================================================================
void D3DRenderDevice::CreateCubeMapTextureFromFile(Texture* texture)
{
	LPDIRECT3DCUBETEXTURE9 pCubeTex=0;
    HRESULT hr;

	D3DXIMAGE_INFO info;	// image info

	hr = D3DXGetImageInfoFromFileInMemory
	(
		texture->Data,
		texture->Size,
		&info
	);

	int w = info.Width;

	if (!(texture->Flags & Texture::ETexNoLOD))
	{
		// apply texture lod
		int lod = TexLOD[texture->LevelOfDetail];
		w >>= lod;

		const int MinSize = 32;
		if (w<MinSize && info.Width>MinSize)  w = MinSize;
	}

	D3DFORMAT internalFormat = D3DFMT_UNKNOWN; // use file format 
/*
	if ((info.Format == D3DFMT_DXT1) && (HWClass == EHWClassNV20)) {
		// Nvidia bug workaround: enforce internal DXT3 for DXT1 textures on GF3
		internalFormat = D3DFMT_DXT3;
	}
*/
	hr=D3DXCreateCubeTextureFromFileInMemoryEx
	(
		d3dev,
		texture->Data,
		texture->Size,
		w, 
		(texture->Flags & Texture::ETexNoMipMaps) ? 1 : D3DX_DEFAULT,
		0, 
		internalFormat, 
		D3DPOOL_MANAGED, 
		D3DX_DEFAULT,
		D3DX_FILTER_BOX|D3DX_FILTER_DITHER,
		0, 
		&info, 
		NULL,
		&pCubeTex
	);

	if (SUCCEEDED(hr))
	{	
	    int texBytes=0;
		int w=0,h=0;

		for (unsigned int i=0; i<pCubeTex->GetLevelCount(); ++i) 
        {
		    D3DSURFACE_DESC Desc;
			if (SUCCEEDED(pCubeTex->GetLevelDesc(i,&Desc))) 
            {
				int size = Desc.Width*Desc.Height*SurfFormatToBPP(Desc.Format)/8;
				TexBytes+=size;
				texBytes+=size;
				if (i==0) 
                {
				    w=Desc.Width;
					h=Desc.Height;
			    }
            }
        }

		//pTex->SetLOD(3);

           
		D3DTexture texDef(texture,pCubeTex);
		const int index = Textures.Add(texDef);

#ifdef ANNOYING_GRAPHICS_LOGS
		D3DLog.Print("CubemapTex[%d]:\t%.2f MB/%.2f MB free %s (%dx%d mip %d)->(%dx%d mip %d)\n",
                     index,(float)texBytes/(1024.f*1024.f),
					 d3dev->GetAvailableTextureMem()/(1024.f*1024.f),
					 texture->FileName.AsChar(),
					 info.Width,info.Height,info.MipLevels,
					 w,h,pCubeTex->GetLevelCount());
#endif

		ClassifyMemoryStats(texture);

		TextureMemory[texture->MemoryStats] += (float)texBytes/(1024.f*1024.f);

		texture->OriginalWidth  = info.Width;
		texture->OriginalHeight = info.Height;
		texture->Width			= w;
		texture->Height			= h;
		texture->PixelBytes		= texBytes;
		texture->DeviceId		= index;
	}
}

// ==========================================================================
// internal
// create render target cubemap texture from file
// ==========================================================================
void D3DRenderDevice::CreateRenderTargetCubeMapTextureFromFile(Texture* texture)
{
	LPDIRECT3DCUBETEXTURE9 pCubeTex=0;
    HRESULT hr;

	hr = D3DXCreateCubeTexture
    (
        d3dev, 
        texture->Width, 
        1, 
        D3DUSAGE_RENDERTARGET, 
        D3DFMT_X8R8G8B8, 
        D3DPOOL_DEFAULT, 
        &pCubeTex
    );

	if (SUCCEEDED(hr)) {
	    int texBytes=0;
		int w=0,h=0;

		for (unsigned int i=0; i<pCubeTex->GetLevelCount(); ++i) 
        {
		    D3DSURFACE_DESC Desc;
			if (SUCCEEDED(pCubeTex->GetLevelDesc(i,&Desc))) 
            {
				int size = Desc.Width*Desc.Height*SurfFormatToBPP(Desc.Format)/8;
				TexBytes+=size;
				texBytes+=size;
				if (i==0) 
                {
				    w=Desc.Width;
					h=Desc.Height;
			    }
            }
        }

		D3DTexture texDef(texture,pCubeTex);
		const int index = Textures.Add(texDef);
		ResTextures.Add(index);


		D3DLog.Print("RenderTargetCube[%d]:\t%.2f MB/%.2f MB free (%dx%d mip %d)\n",
                     index,(float)texBytes/(1024.f*1024.f),
					 d3dev->GetAvailableTextureMem()/(1024.f*1024.f),
					 w,h,pCubeTex->GetLevelCount());

		texture->OriginalWidth  = w;
		texture->OriginalHeight = h;
		texture->Width			= w;
		texture->Height			= h;
		texture->PixelBytes		= texBytes;
		texture->DeviceId		= index;
	}
    else
		D3DLog.Print("ERROR: could not create %s target cubemap (%d,%d)\n",
                     ((texture->Type == Texture::ETexRenderTarget) ? "render" : "depth"),
                     texture->Width,texture->Height);
}

// ==========================================================================
// internal
// create regular texture from raw bytes
// ==========================================================================
void D3DRenderDevice::CreateRegularTexture(Texture* texture)
{
    LPDIRECT3DTEXTURE9 pTex=0;
    HRESULT hr;

	if (texture->Width<= 0 || texture->Height<= 0)
    {
		D3DLog.Print("ERROR: invalid W,H for CreateRegularTexture (raw) (%d,%d)\n",
                     texture->Width,texture->Height);
        return;
	}

	D3DFORMAT format = D3DFMT_X8R8G8B8;
	if (texture->Flags & Texture::ETexRaw8)
		format = D3DFMT_P8;
	else if( texture->Flags & Texture::ETexRaw32Alpha )
		format = D3DFMT_A8R8G8B8;

    hr = d3dev->CreateTexture
    (
		texture->Width,
        texture->Height,
		(texture->Flags & Texture::ETexNoMipMaps) ? 1 : 0,	// levels
        0, // usage
        format,
        D3DPOOL_MANAGED,
        &pTex,
		NULL
	);



	if (SUCCEEDED(hr) && pTex)
	{
		D3DLOCKED_RECT Locked;
		D3DSURFACE_DESC ddsdDesc;

		pTex->GetLevelDesc(0, &ddsdDesc);
		pTex->LockRect(0, &Locked, NULL, 0);

		for (unsigned int y = 0; y < ddsdDesc.Height; ++y)
		{
			for (unsigned int x = 0; x < ddsdDesc.Width; ++x)
			{
				DWORD* pBits = (DWORD*)((BYTE*)Locked.pBits + (y * Locked.Pitch));
				pBits += x;
				//*pBits = *( ( DWORD *)( texture->Data + y * texture->Height + texture->Width ) );	// WTF? :)
				*pBits = *((DWORD*)(texture->Data) + y * texture->Height + x);
			}
		}

		pTex->UnlockRect(0);

		if (!(texture->Flags & Texture::ETexNoMipMaps))
		{
			hr = D3DXFilterTexture
			(
				pTex,
				NULL,				// CONST PALETTEENTRY *pPalette,
				0,					// UINT SrcLevel,
				D3DX_FILTER_BOX		// DWORD MipFilter
			);
		}

		D3DTexture texDef(texture,pTex);
		const int index = Textures.Add(texDef);
		texture->DeviceId = index;

#ifdef ANNOYING_GRAPHICS_LOGS
		D3DLog.Print("RegularTex: [%d] %s (%d) (%d,%d)\n",
                     index,texture->FileName.AsChar(),
					 texture->Size,texture->Width,texture->Height);
#endif
	}
	else
	{
		D3DLog.Print( "Error creating regular texture %s\n", texture->FileName );
	}
}

// ==========================================================================
// internal
// create dynamic texture
// ==========================================================================
void D3DRenderDevice::CreateDynamicTexture(Texture* texture)
{
    LPDIRECT3DTEXTURE9 pTex=0;
    HRESULT hr;

	if (texture->Width<= 0 || texture->Height<= 0)
    {
		D3DLog.Print("ERROR: invalid W,H for CreateDynamicTexture (raw) (%d,%d)\n",
                     texture->Width,texture->Height);
        return;
	}

	D3DFORMAT format = D3DFMT_X8R8G8B8;
	if (texture->Flags & Texture::ETexRaw8)
		format = D3DFMT_P8;
	else if( texture->Flags & Texture::ETexRaw32Alpha )
		format = D3DFMT_A8R8G8B8;

    hr = d3dev->CreateTexture
    (
		texture->Width,
        texture->Height,
		(texture->Flags & Texture::ETexNoMipMaps) ? 1 : 0,	// levels
        D3DUSAGE_DYNAMIC, // usage
        format,
        D3DPOOL_DEFAULT,
        &pTex,
		NULL
	);

	if( FAILED( hr ) )
	{
		D3DLog.Print( "Error creating dynamic texture, reverting to normal" );
		hr = d3dev->CreateTexture
		(
			texture->Width,
			texture->Height,
			(texture->Flags & Texture::ETexNoMipMaps) ? 1 : 0,	// levels
			0, // usage
			format,
			D3DPOOL_MANAGED,
			&pTex,
			NULL
		);
	}

	if (SUCCEEDED(hr) && pTex)
	{
		D3DLOCKED_RECT Locked;
		D3DSURFACE_DESC ddsdDesc;

		pTex->GetLevelDesc(0, &ddsdDesc);
		pTex->LockRect(0, &Locked, NULL, 0);

		for (unsigned int y = 0; y < ddsdDesc.Height; ++y)
		{
			for (unsigned int x = 0; x < ddsdDesc.Width; ++x)
			{
				DWORD* pBits = (DWORD*)((BYTE*)Locked.pBits + (y * Locked.Pitch));
				pBits += x;
				//*pBits = *( ( DWORD *)( texture->Data + y * texture->Height + texture->Width ) );
				*pBits = *((DWORD*)(texture->Data) + y * texture->Height + x);
			}
		}

		pTex->UnlockRect(0);

		D3DTexture texDef(texture,pTex);
		const int index = Textures.Add(texDef);
		texture->DeviceId = index;
		ResTextures.Add(index);

		D3DLog.Print("DynamicTex: [%d] %s (%d) (%d,%d)\n",
                     index,texture->FileName.AsChar(),
					 texture->Size,texture->Width,texture->Height);
	}
	else
	{
		D3DLog.Print( "Error creating dynamic texture %s\n", texture->FileName );
	}
}

// ==========================================================================
// internal
// create 3d texture
// ==========================================================================

void D3DRenderDevice::CreateVolumeTexture(Texture* texture)
{
	HRESULT hr;
	IDirect3DVolumeTexture9* pTex = NULL;

	D3DLog.Print("VOLUME: %d %d %d\n", texture->Width, texture->Height, texture->Depth);

	D3DFORMAT Format = FlagsToFormat(texture->Flags);
	if (texture->Flags & Texture::ETexAnim3D)
	{
		const int texId = texture->FrameIds[0];
        LPDIRECT3DTEXTURE9 SrcTex = (texId>0 && texId<Textures.Size() ? Textures[texId].pTex : NULL);
		Assert(SrcTex);

		D3DSURFACE_DESC SrcDesc;
		SrcTex->GetLevelDesc(0, &SrcDesc);
		Format = SrcDesc.Format;

		D3DLog.Print("FRAME[0]= %d %d\n", SrcDesc.Width, SrcDesc.Height);
	}

	hr = d3dev->CreateVolumeTexture
	(
		texture->Width,					// UINT Width,
		texture->Height,				// UINT Height,
		texture->Depth,					// UINT Depth,
		1,								// UINT Levels,
		0,								// DWORD Usage,
		Format,							// D3DFORMAT Format,
		D3DPOOL_MANAGED,				// D3DPOOL Pool,
		&pTex,							// IDirect3DVolumeTexture9** ppVolumeTexture,
		NULL							// HANDLE* pSharedHandle
	);

	if (SUCCEEDED(hr))
	{
		D3DTexture TexDef(texture,pTex);
		const int Index = Textures.Add(TexDef);
		texture->DeviceId = Index;
		ResTextures.Add(Index);

		D3DLog.Print("3DTex: [%d] %s (%d) (%d,%d,%d)\n",
                     Index,texture->FileName.AsChar(),
					 texture->Size,texture->Width,texture->Height,texture->Depth);

		if (texture->Flags & Texture::ETexAnim3D)
		{
			// lock volume texture
			D3DLOCKED_BOX DstLocked;
			D3DBOX DstBox;

			DstBox.Left = 0;
			DstBox.Right = texture->Width;
			DstBox.Top = 0;
			DstBox.Bottom = texture->Height;
			DstBox.Front = 0;
			DstBox.Back = texture->NumFrames;

			hr = pTex->LockBox
			(
				0,
				&DstLocked,
				NULL,//&DstBox,
				0
			);

			if (FAILED(hr))
				return;

			D3DVOLUME_DESC DstDesc;
			pTex->GetLevelDesc(0, &DstDesc);

			int PitchSize = 1;
			switch (Format)
			{
			case D3DFMT_DXT1:	
				PitchSize = 8; break;
			case D3DFMT_DXT2:	
			case D3DFMT_DXT3:
			case D3DFMT_DXT4:
			case D3DFMT_DXT5:
				PitchSize = 16; break;
			}

			for (int i=0; i<texture->NumFrames; ++i)
			{
				const int texId = texture->FrameIds[i];
                LPDIRECT3DTEXTURE9 SrcTex = (texId>0 && texId<Textures.Size() ? Textures[texId].pTex : NULL);
				Assert(SrcTex);

				D3DLOCKED_RECT SrcLocked;
				D3DSURFACE_DESC SrcDesc;

				SrcTex->GetLevelDesc(0, &SrcDesc);
				SrcTex->LockRect(0, &SrcLocked, NULL, 0);

				for (unsigned int y = 0; y < SrcDesc.Height; ++y)
				{
					BYTE* Dst = (BYTE*) DstLocked.pBits + (i * DstLocked.SlicePitch) + (y * (DstLocked.RowPitch/PitchSize));

					const BYTE* Src = (BYTE*) SrcLocked.pBits + (y * (SrcLocked.Pitch/PitchSize));
					for (unsigned int x = 0; x < SrcDesc.Width; ++x)
					{
						Dst[x] = Src[x];
					}
				}

				SrcTex->UnlockRect(0);
			}

			pTex->UnlockBox(0);
		}
	}
}

void D3DRenderDevice::CreateVolumeTextureFromFile(Texture* texture)
{
	Assert(0==1);
}

// ==========================================================================
// internal
// create cubemap texture from raw bytes
// ==========================================================================
void D3DRenderDevice::CreateCubeMapTexture(Texture* texture)
{
}

void D3DRenderDevice::CreateNormCubeMap(Texture* texture)
{
	HRESULT hr;
	LPDIRECT3DCUBETEXTURE9 pTex=0;
	hr = CreateNormalizationCubeMap(d3dev, texture->Width, 1, &pTex);
	if (SUCCEEDED(hr)) 
	{
		D3DTexture texDef(texture,pTex);
		const int index = Textures.Add(texDef);
		texture->DeviceId = index;
        texture->Height = texture->Width;
	}
}

// ==========================================================================
//	Lock texture
// ==========================================================================
BYTE *D3DRenderDevice::LockTexture( Texture *tex, int &pitch )
{
	if( tex->DeviceId < 0 )
		return NULL;

	D3DTexture texDef = Textures[tex->DeviceId];
	LPDIRECT3DTEXTURE9 pTex = texDef.pTex;
	if( !pTex )
		return NULL;

	BYTE *data = NULL;
	D3DLOCKED_RECT locked;

	HRESULT hr = pTex->LockRect( 0, &locked, NULL, 0 );
	if( SUCCEEDED( hr ) )
	{
		data = ( BYTE *)locked.pBits;
		pitch = locked.Pitch;
	}

	tex->IsLocked = true;

	return data;
}


void D3DRenderDevice::UnlockTexture( Texture *tex )
{
	if( tex->DeviceId < 0 )
		return;

	D3DTexture texDef = Textures[tex->DeviceId];
	LPDIRECT3DTEXTURE9 pTex = texDef.pTex;
	if( !pTex )
		return;

	pTex->UnlockRect( 0 );

	tex->IsLocked = false;
}

void D3DRenderDevice::SetTexturePriority(Texture *tex, DWORD priority)
{
	if( tex->DeviceId < 0 )
		return;

	D3DTexture texDef = Textures[tex->DeviceId];
	LPDIRECT3DTEXTURE9 pTex = texDef.pTex;
	if( !pTex )
		return;

	pTex->SetPriority(priority);
//	D3DLog.Print("SetPriority[%d]: %d\n", tex->DeviceId, priority);
}

void D3DRenderDevice::PreLoadTexture(Texture *tex)
{
	if( tex->DeviceId < 0 )
		return;

	for (int i=0; i<tex->NumFrames; i++)
	{
		if (tex->FrameIds[i]==EInvalidIndex)
			continue;
		D3DTexture texDef = Textures[tex->FrameIds[i]];
		LPDIRECT3DTEXTURE9 pTex = texDef.pTex;
		if( !pTex )
			continue;

		pTex->PreLoad();

	}

	if (tex->DeviceId==EInvalidIndex)
		return;

	D3DTexture texDef = Textures[tex->DeviceId];
	LPDIRECT3DTEXTURE9 pTex = texDef.pTex;
	if (!pTex)
		return;

	pTex->PreLoad();
}

int D3DRenderDevice::SurfFormatToBPP(const D3DFORMAT &Format) const
{
	int formatBPP = 0;

	switch (Format)
	{
	case D3DFMT_R8G8B8:				formatBPP = 24; break;
	case D3DFMT_A8R8G8B8: 			formatBPP = 32; break;
	case D3DFMT_X8R8G8B8: 			formatBPP = 32; break;
	case D3DFMT_R5G6B5:				formatBPP = 16; break;
	case D3DFMT_X1R5G5B5: 			formatBPP = 16; break;
	case D3DFMT_A1R5G5B5: 			formatBPP = 16; break;
	case D3DFMT_A4R4G4B4: 			formatBPP = 16; break;
	case D3DFMT_R3G3B2:				formatBPP = 8; break;
	case D3DFMT_A8:					formatBPP = 8; break;
	case D3DFMT_A8R3G3B2:			formatBPP = 16; break;
	case D3DFMT_X4R4G4B4:			formatBPP = 16; break;
	case D3DFMT_A2B10G10R10:		formatBPP = 32; break;
	case D3DFMT_A8B8G8R8:			formatBPP = 32; break;
	case D3DFMT_X8B8G8R8:			formatBPP = 32; break;
	case D3DFMT_G16R16:				formatBPP = 32; break;
	case D3DFMT_A2R10G10B10:		formatBPP = 32; break;
	case D3DFMT_A16B16G16R16:		formatBPP = 64; break;

	case D3DFMT_A8P8:				formatBPP = 16; break;
	case D3DFMT_P8:					formatBPP = 8; break;

	case D3DFMT_L8:					formatBPP = 8; break;
	case D3DFMT_A8L8:				formatBPP = 16; break;
	case D3DFMT_A4L4:				formatBPP = 16; break;

	case D3DFMT_V8U8:				formatBPP = 16; break;
	case D3DFMT_L6V5U5: 			formatBPP = 16; break;
	case D3DFMT_X8L8V8U8:			formatBPP = 32; break;
	case D3DFMT_Q8W8V8U8:			formatBPP = 32; break;
	case D3DFMT_V16U16:				formatBPP = 32; break;
	case D3DFMT_A2W10V10U10:		formatBPP = 32; break;

	case D3DFMT_UYVY:				formatBPP = 16; break; // I suppose?
	case D3DFMT_R8G8_B8G8:			formatBPP = 16; break;
	case D3DFMT_YUY2:				formatBPP = 16; break;
	case D3DFMT_G8R8_G8B8:			formatBPP = 16; break;
	case D3DFMT_DXT1:				formatBPP = 4; break;
	case D3DFMT_DXT2:				formatBPP = 8; break;
	case D3DFMT_DXT3: 				formatBPP = 8; break;
	case D3DFMT_DXT4: 				formatBPP = 8; break;
	case D3DFMT_DXT5: 				formatBPP = 8; break;

	case D3DFMT_D16_LOCKABLE:		formatBPP = 16; break;
	case D3DFMT_D32:				formatBPP = 32; break;
	case D3DFMT_D15S1: 				formatBPP = 16; break;
	case D3DFMT_D24S8: 				formatBPP = 32; break;
	case D3DFMT_D24X8: 				formatBPP = 32; break;
	case D3DFMT_D24X4S4:			formatBPP = 32; break;
	case D3DFMT_D16:				formatBPP = 16; break;

	case D3DFMT_D32F_LOCKABLE:		formatBPP = 32; break;
	case D3DFMT_D24FS8:				formatBPP = 32; break;

	case D3DFMT_L16:				formatBPP = 16; break;

	case D3DFMT_VERTEXDATA: 		formatBPP = 0; break; //depends on format
	case D3DFMT_INDEX16:			formatBPP = 16; break;
	case D3DFMT_INDEX32:			formatBPP = 32; break;

	case D3DFMT_Q16W16V16U16:		formatBPP = 64; break;

	case D3DFMT_MULTI2_ARGB8:		formatBPP = 32; break;

	// Floating point surface formats
	case D3DFMT_R16F:				formatBPP = 16; break;
	case D3DFMT_G16R16F:			formatBPP = 32; break;
	case D3DFMT_A16B16G16R16F:		formatBPP = 64; break;

	case D3DFMT_R32F:				formatBPP = 32; break;
	case D3DFMT_G32R32F:			formatBPP = 64; break;
	case D3DFMT_A32B32G32R32F:		formatBPP = 128; break;
	case D3DFMT_CxV8U8:				formatBPP = 16; break;
	}


	return formatBPP;
}