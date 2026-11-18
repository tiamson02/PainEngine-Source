// ----------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.						
// Not for public release.
//
// $Header: /Game/Source/D3Dev/D3Dev.cpp 282   04-12-16 12:59 Ulkai $
// ----------------------------------------------------------------------------

#include "D3Dev.h"
#include "D3DEffect.h"
#include "dxerr9.h"
#include "Render/Viewport.h"
#include "Render/View.h"
#include "Render/Video.h"
#include "Render/Material.h"

//#define NVPERFHUD_ENABLE

LogBuffer D3DRenderDevice::D3DLog;

static void DumpDecl(LogBuffer& Log, const D3DVERTEXELEMENT9* Decl);

// DLL interface
RenderDevice* CreateDevice()
{
	D3DRenderDevice::D3DLog.Open("D3Dev.log");
    D3DRenderDevice::D3DLog.Print("Creating DirectX 9.0 Renderer\n");
    m_setTag("D3DVideo");
    return new D3DRenderDevice();
}

void DestroyDevice(RenderDevice* rdevPtr)
{
    D3DRenderDevice::D3DLog.Print("Destroying DirectX 9.0 Renderer\n");
    if(rdevPtr) delete rdevPtr;
}

// ===========================================================

D3DRenderDevice::D3DRenderDevice()
:	RenderDevice()
,	d3d(NULL), d3dev(NULL)
,   IsFullScreen(FALSE), FullScreenHWND(0), FullScreenWidth(0), FullScreenHeight(0)
,   CurrentStreamSize(0), CurrentStreamIsIndexed(FALSE)
,   CurrentVertexShader(0), CurrentPixelShader(0), CachedSWVP(FALSE), CachedFVF(0)
,   TexBytes(0), FrameTris(0), FrameDIPs(0)
,   OriginalBackBuffer(0), OriginalDepthBuffer(0), CurrentBackBuffer(0), CurrentDepthBuffer(0)
,	AdapterOrdinal(D3DADAPTER_DEFAULT), ViewportWidth(0), ViewportHeight(0), StencilSurfaceTouched(FALSE)
,	CullFlip(0)
{
    GMemZero(&d3dpp,sizeof(D3DPRESENT_PARAMETERS));
    GMemZero(&d3dCaps,sizeof(D3DCAPS9));

    Textures.Add(D3DTexture());
	GetOriginalGamma();
}

D3DRenderDevice::~D3DRenderDevice()
{
	RestoreOriginalGamma();
    Close();
    Textures.ClearArray(0);
}

// =============================================================
// RenderDevice interface
// =============================================================

BOOL D3DRenderDevice::Open(HWND fullScreenHWND)
{
    FullScreenHWND=fullScreenHWND;
    Close();

    BOOL success=FALSE;
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if(d3d) {
		D3DADAPTER_IDENTIFIER9 d3dIdent;

		if (SUCCEEDED(d3d->GetAdapterIdentifier(AdapterOrdinal,0,&d3dIdent))) {
			D3DLog.Print("Driver: %s\n", d3dIdent.Driver);
			D3DLog.Print("Description: %s\n", d3dIdent.Description);
			int Product = HIWORD(d3dIdent.DriverVersion.HighPart),
				Version = LOWORD(d3dIdent.DriverVersion.HighPart),
				SubVer  = HIWORD(d3dIdent.DriverVersion.LowPart),
				Build   = LOWORD(d3dIdent.DriverVersion.LowPart);
			D3DLog.Print("Version: %d.%d.%d.%d\n",Product,Version,SubVer,Build);

		}

        if(SUCCEEDED(d3d->GetDeviceCaps(AdapterOrdinal,D3DDEVTYPE_HAL,&d3dCaps))) {
            // we only support TnL devices
            if(d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
                D3DLog.Print("Successfully opened DirectX 9.0 Renderer\n");
                DumpCaps(d3dCaps);

				HWClass = EHWClassGenericTnL;

		        if(d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1,1)
					&& d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1,1))
				{
					HWClass = EHWClassNV20;

					if (d3dCaps.PixelShaderVersion == D3DPS_VERSION(1,3))
						HWClass = EHWClassNV25;

					if (d3dCaps.PixelShaderVersion == D3DPS_VERSION(1,4))
						HWClass = EHWClassR200;

					if (d3dCaps.VertexShaderVersion >= D3DVS_VERSION(2,0)
						&& d3dCaps.PixelShaderVersion >= D3DPS_VERSION(2,0))
					{
						HWClass = EHWClassNV30;
					}

					if (d3dCaps.VertexShaderVersion >= D3DVS_VERSION(3,0)
						&& d3dCaps.PixelShaderVersion >= D3DPS_VERSION(3,0))
					{
						HWClass = EHWClassNV40;
					}
				}

				if (GEngine->EnforcedHWClass && GEngine->EnforcedHWClass<HWClass)
				{
					D3DLog.Print("HWClass enforced by command line param ");
					HWClass = GEngine->EnforcedHWClass;
				}

				D3DLog.Print("HWClass: ");
				switch (HWClass) {
				case EHWClassUnsupported:	D3DLog.Print("Unsupported\n"); break;
				case EHWClassGenericTnL:	D3DLog.Print("GenericTnL\n"); break;
				case EHWClassNV20:			D3DLog.Print("NV20\n"); break;
				case EHWClassNV25:			D3DLog.Print("NV25\n"); break;
				case EHWClassR200:			D3DLog.Print("R200\n"); break;
				case EHWClassNV30:			D3DLog.Print("NV30 (yes, it's possible)\n"); break;
				case EHWClassNV40:			D3DLog.Print("NV40 (goddamn!)\n"); break;
				}

				if (!(d3dCaps.TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP_POW2))
				{
					DevCaps |= EDevCaps_NonPow3DTex;
					D3DLog.Print("3D Textures NONPOW2 supported!\n");
				}

				if (!(d3dCaps.TextureCaps & D3DPTEXTURECAPS_NOPROJECTEDBUMPENV))
					DevCaps |= EDevCaps_BumpEnvProj;

	            D3DDISPLAYMODE d3ddm;
	            if(SUCCEEDED(d3d->GetAdapterDisplayMode(AdapterOrdinal, &d3ddm))) 
                {
                    D3DLog.Print("Desktop Mode: %dx%d @ %d\n",d3ddm.Width,d3ddm.Height,d3ddm.RefreshRate);
                    DesktopMode=d3ddm;
                    DesktopHWND=GetDesktopWindow();
                    success=TRUE;
	            }

				RenderDisplayMode* lastMode=0;
				DisplayModeNum = 0;

				D3DFORMAT format = D3DFMT_X8R8G8B8;

				for (UINT idx=0; idx<d3d->GetAdapterModeCount(AdapterOrdinal,format); ++idx) {
					HRESULT hr;
					D3DDISPLAYMODE mode;
					
					hr = d3d->EnumAdapterModes(AdapterOrdinal,format,idx,&mode);
					if (SUCCEEDED(hr)) {
						if (mode.Format == D3DFMT_X8R8G8B8) {

							if (lastMode && (mode.Width == lastMode->XRes && mode.Height == lastMode->YRes)) {
								if (lastMode->RefreshRateNum < MAX_REFRESH_RATES)
									lastMode->RefreshRates[lastMode->RefreshRateNum++] = mode.RefreshRate;

							} else {
								if (DisplayModeNum < MAX_DISPLAY_MODES) {
									DisplayModes[DisplayModeNum].XRes = mode.Width;
									DisplayModes[DisplayModeNum].YRes = mode.Height;
									DisplayModes[DisplayModeNum].RefreshRates[0] = mode.RefreshRate;
									DisplayModes[DisplayModeNum].RefreshRateNum = 1;

									lastMode = &DisplayModes[DisplayModeNum++];
								}
							}
						}
					}
				}

				for (int i=0; i<DisplayModeNum; ++i) {
					D3DLog.Print("mode[%02d]: %dx%d @ ",i,DisplayModes[i].XRes,DisplayModes[i].YRes);
					for (int j=0; j<DisplayModes[i].RefreshRateNum; ++j) {
						D3DLog.Print("%d ",DisplayModes[i].RefreshRates[j]);
					}
					D3DLog.Print("\n");
				}
            }
        }
    }

    if(!success) {
        Close();
    }

    return success;
}

void D3DRenderDevice::Close()
{
    if(d3dev) {
        D3DLog.Print("Closing DirectX 9 Render Device\n");

        Flush();
        SAFE_RELEASE(d3dev);
        SAFE_RELEASE(d3d);
    }

    D3DLog.Flush();
}

BOOL D3DRenderDevice::SetMode(UINT width, UINT height, BOOL fullscreen, int multisampling)
{
    BOOL success=FALSE;
    if(d3d) 
    {
        // pure device maybe?
		DWORD Behavior;
		if (HWClass==EHWClassGenericTnL) {
			Behavior = D3DCREATE_MIXED_VERTEXPROCESSING;
		}
		else {
			Behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
			if(d3dCaps.DevCaps & D3DDEVCAPS_PUREDEVICE) {
				Behavior|=D3DCREATE_PUREDEVICE;
			}
        }

        UINT FX,FY;
        D3DFORMAT format;
		UINT refreshRate;
		HWND hwnd;

        if (fullscreen) 
        {
            // validate xres,yres
            if(width<=0 || height<=0) {
                D3DLog.Print("SetMode(): invalid W,H passed (%d,%d) for fullscreen\n",width,height);
                return FALSE;
            }

			FX=FY=0;
			refreshRate = D3DPRESENT_RATE_DEFAULT;

			for (int i=0; i<DisplayModeNum; ++i) {
				if (width == DisplayModes[i].XRes && height == DisplayModes[i].YRes) {
					refreshRate = DisplayModes[i].RefreshRates[DisplayModes[i].RefreshRateNum-1];
		            FX=width;
		            FY=height;
					break;
				}
			}

			if (!FX || !FY) {
				D3DLog.Print("WARNING: fullscreen mode (%d,%d) INVALID, falling back to safe mode\n",
							  width,height);
				FX = 640;
				FY = 480;
			}
            
            format = D3DFMT_A8R8G8B8;
            hwnd = FullScreenHWND;
        }
        else 
        { 
            hwnd = DesktopHWND;
            format = D3DFMT_A8R8G8B8;
            FX = DesktopMode.Width;
            FY = DesktopMode.Height;
			refreshRate = D3DPRESENT_RATE_DEFAULT;
        }


        d3dpp.BackBufferWidth                   = FX;
        d3dpp.BackBufferHeight                  = FY;
        d3dpp.BackBufferFormat                  = format;
		d3dpp.BackBufferCount                   = 1;

		const D3DMULTISAMPLE_TYPE multi[17] = 
		{
			D3DMULTISAMPLE_NONE,
			D3DMULTISAMPLE_NONE,
			D3DMULTISAMPLE_2_SAMPLES,
			D3DMULTISAMPLE_3_SAMPLES,
			D3DMULTISAMPLE_4_SAMPLES,
			D3DMULTISAMPLE_5_SAMPLES,
			D3DMULTISAMPLE_6_SAMPLES,
			D3DMULTISAMPLE_7_SAMPLES,
			D3DMULTISAMPLE_8_SAMPLES,
			D3DMULTISAMPLE_9_SAMPLES,
			D3DMULTISAMPLE_10_SAMPLES,
			D3DMULTISAMPLE_11_SAMPLES,
			D3DMULTISAMPLE_12_SAMPLES,
			D3DMULTISAMPLE_13_SAMPLES,
			D3DMULTISAMPLE_14_SAMPLES,
			D3DMULTISAMPLE_15_SAMPLES,
			D3DMULTISAMPLE_16_SAMPLES,
		};
		
		d3dpp.MultiSampleType					= multi[Clamp(multisampling, 0, 16)];

		d3dpp.SwapEffect                        = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow                     = hwnd;
        d3dpp.Windowed                          = !fullscreen;
        d3dpp.EnableAutoDepthStencil            = TRUE;
        d3dpp.AutoDepthStencilFormat            = D3DFMT_D24S8;
        d3dpp.Flags                             = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

		d3dpp.FullScreen_RefreshRateInHz        = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval				= D3DPRESENT_INTERVAL_IMMEDIATE;

		if( !SUCCEEDED(d3d->CheckDeviceMultiSampleType(AdapterOrdinal, D3DDEVTYPE_HAL,
													d3dpp.BackBufferFormat,
													d3dpp.Windowed,
													d3dpp.MultiSampleType, NULL)))
		{
			d3dpp.MultiSampleType=D3DMULTISAMPLE_NONE;
		}

		if (d3dev) D3DLog.Print("Available texmem %d MB\n", d3dev->GetAvailableTextureMem()/1024/1024);

        ReleaseDevice();

		if (d3dev) D3DLog.Print("Available texmem %d MB\n", d3dev->GetAvailableTextureMem()/1024/1024);

        HRESULT hr;
		
		if (d3dev) {
			D3DLog.Print("Reset device\n");
			hr = d3dev->Reset(&d3dpp);
		}
		else {
			D3DLog.Print("Create device\n");

#ifdef NVPERFHUD_ENABLE
			hr = d3d->CreateDevice(d3d->GetAdapterCount()-1, D3DDEVTYPE_REF,
                                   FullScreenHWND, Behavior, &d3dpp, &d3dev);
#else
			hr = d3d->CreateDevice(AdapterOrdinal, D3DDEVTYPE_HAL,
                                   FullScreenHWND, Behavior, &d3dpp, &d3dev);
#endif
		}


		if (d3dev) D3DLog.Print("Available texmem %d MB\n", d3dev->GetAvailableTextureMem()/1024/1024);


		if (SUCCEEDED(hr) && d3dev) 
        {
            RestoreDevice();
			
			if (d3dev) D3DLog.Print("Available texmem %d MB\n", d3dev->GetAvailableTextureMem()/1024/1024);

			success=TRUE;

            IsFullScreen=fullscreen;
            if(IsFullScreen)
            {
                FullScreenWidth  = FX;
                FullScreenHeight = FY;
            }

			// remember render targets
	        if (FAILED(d3dev->GetRenderTarget(0,&OriginalBackBuffer)))
                D3DLog.Print("WARNING: GetRenderTarget(BackBuffer) FAILED\n");
	        if (FAILED(d3dev->GetDepthStencilSurface(&OriginalDepthBuffer)))
                D3DLog.Print("WARNING: GetRenderTarget(DepthBuffer) FAILED\n");

			D3DSURFACE_DESC desc;
			OriginalBackBuffer->GetDesc(&desc);

			D3DLog.Print("SetMode: %d,%d%@%dHz (%s) (fmt:%d, req:%d)",FX,FY,refreshRate,
					fullscreen?"Fullscreen":"Windowed",
					desc.Format, format);

			if (desc.MultiSampleType!=D3DMULTISAMPLE_NONE)
				D3DLog.Print(" AAx%d\n",desc.MultiSampleType);
			else
				D3DLog.Print("\n");

			//SetFBRenderTarget();

			D3DLog.Print("Available texmem %d MB\n", d3dev->GetAvailableTextureMem()/1024/1024);
        }
    }

    return success;
}

BOOL D3DRenderDevice::SetGamma(float contrast, float gamma, float brightness)
{
    // TODO: check caps

	D3DGAMMARAMP Ramp;
	Ramp.red[0] = 1;
    for(int i=0; i<256; ++i) {
        int x=GFloatRound((contrast+0.5f)*(float)pow(i/255.f,1.0f/gamma)*65535.f + (brightness-0.5f)*32768.f - contrast*32768.f+16384.f);
        WORD val=x<0 
			? WORD(0)
			: x<65535 
			? WORD(x) 
			: WORD(65535);

		Ramp.red[i]		= val;
        Ramp.green[i]	= val;
        Ramp.blue[i]	= val;
    }

	if (IsFullScreen && d3dev)
	{
		// ATI fix hack, ATI has messed up Gamma Calibration in Cat 4.7
		Ramp.red[0] += 1;
		d3dev->SetGammaRamp(0,D3DSGR_CALIBRATE,&Ramp);
		Ramp.red[0] -= 1;
		d3dev->SetGammaRamp(0,D3DSGR_CALIBRATE,&Ramp);
	}
	else
	{
		HDC hdc = GetDC(GetDesktopWindow()); 
		SetDeviceGammaRamp(hdc, &Ramp);
		ReleaseDC(GetDesktopWindow(), hdc);
	}

    return TRUE;
}

void D3DRenderDevice::GetOriginalGamma()
{
	HDC hdc = GetDC(GetDesktopWindow()); 
	GetDeviceGammaRamp(hdc, &OrigianalGammaRamp);
	ReleaseDC(GetDesktopWindow(), hdc);  
}

BOOL D3DRenderDevice::RestoreOriginalGamma()
{
	SetGamma(0.5f, 1.0f,0.5f);
	return TRUE;
}

BOOL D3DRenderDevice::ClearZ(float z)
{
    Assert(d3dev);
    return SUCCEEDED(d3dev->Clear(0,0,D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,0,z,0));
}

BOOL D3DRenderDevice::ClearFB(DWORD argb)
{
    Assert(d3dev);
    return SUCCEEDED(d3dev->Clear(0,0,D3DCLEAR_TARGET,argb,0,0));
}

BOOL D3DRenderDevice::SetViewport(int x, int y, int w, int h, float zNear, float zFar)
{
	D3DVIEWPORT9 VP;
	VP.X		= x;
	VP.Y		= y;
    VP.Width	= w;
    VP.Height	= h;
	VP.MinZ		= zNear;
	VP.MaxZ		= zFar;

//    D3DLog.Print("SetViewport(%d,%d,%d,%d, %g,%g)\n",x,y,w,h,zNear,zFar);

	if (SUCCEEDED(d3dev->SetViewport(&VP)))
	{
		ViewportWidth  = w;
		ViewportHeight = h;
		return TRUE;
	}
	else
	{
        D3DLog.Print("SetViewport() FAILED\n");
		return FALSE;
	}
}

BOOL D3DRenderDevice::SetScissor(int x1, int y1, int x2, int y2)
{
	RECT R;

	R.left		= x1;
	R.right		= x2;
	R.top		= y1;
	R.bottom	= y2;

	return SUCCEEDED(d3dev->SetScissorRect(&R));
}

BOOL D3DRenderDevice::Lock(Viewport& viewport)
{
    if(d3dev) 
    {
        HRESULT hr=d3dev->TestCooperativeLevel();
        if(D3D_OK==hr) 
        {              

            /*
            D3DLog.Print("W,H=%d,%d (%d,%d)\n",
                         d3dpp.BackBufferWidth,d3dpp.BackBufferHeight,
                         FullScreenWidth,FullScreenHeight);
            */

			/* store original viewport */
			OriginalViewport = viewport;

		    if(SUCCEEDED(SetViewport(
					OriginalViewport.XOffset,OriginalViewport.YOffset, 
					OriginalViewport.Width,OriginalViewport.Height,
					OriginalViewport.zNear,OriginalViewport.zFar))) 
            {
                if(SUCCEEDED(d3dev->BeginScene())) 
                {          
					//static int frame = 0;
					//D3DLog.Print("Frame %d\n", frame++);
					// determine clear flags
					DWORD clearFlags = 0;
					if (viewport.ClearFlags == EClearAll)
						clearFlags = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL;
					else
					if (viewport.ClearFlags & EClearColor)
						clearFlags |= D3DCLEAR_TARGET;
					else
					if (viewport.ClearFlags & EClearZ)
						clearFlags |= D3DCLEAR_ZBUFFER;
					else
					if (viewport.ClearFlags & EClearStencil)
						clearFlags |= D3DCLEAR_STENCIL;	

					//if (!StencilSurfaceTouched)
					//	clearFlags &= ~D3DCLEAR_STENCIL;


					// clear buffers
					if (clearFlags)
					{
          				d3dev->Clear
						(
							0,0,
							clearFlags, 
							viewport.ClearColor,
							1.0f,
							0
						);
					}

                    d3dev->SetTransform(D3DTS_VIEW,       _DXMAT(viewport.WorldToCamera));
                    d3dev->SetTransform(D3DTS_PROJECTION, _DXMAT(viewport.CameraToScreen));    

					// no shaders set (this is a fix, driver can change shader id between present and lock - NvPerfHUD)
					CurrentVertexShader = (LPDIRECT3DVERTEXSHADER9) EInvalidIndex;
					CurrentPixelShader  = (LPDIRECT3DPIXELSHADER9) EInvalidIndex;

					StencilSurfaceTouched = FALSE;

					for (int i=0; i<96; i++)
						CachedVSConst[i] = Plane(F_MAX,F_MAX,F_MAX,F_MAX);

					for (int i=0; i<8; i++)
						CachedPSConst[i] = Plane(F_MAX,F_MAX,F_MAX,F_MAX);

                    return TRUE;
                }
                else 
                    D3DLog.Print("BeginScene() FAILED\n");
            }
            else 
                D3DLog.Print("Lock->SetViewport() FAILED\n");

        }
        else                        
        if(D3DERR_DEVICELOST==hr) 
        {
		    D3DLog.Print("D3DERR_DEVICELOST...\n");
            Sleep(100);
        }
        else
        if(D3DERR_DEVICENOTRESET==hr) 
        {
            D3DLog.Print("D3DERR_DEVICENOTRESET...\n");
            ReleaseDevice();
			D3DLog.Print("Reset device\n");
            if (SUCCEEDED(d3dev->Reset(&d3dpp))) 
            {
                RestoreDevice();
				// remember render targets
				if (FAILED(d3dev->GetRenderTarget(0,&OriginalBackBuffer)))
					D3DLog.Print("WARNING: GetRenderTarget(BackBuffer) FAILED\n");
				if (FAILED(d3dev->GetDepthStencilSurface(&OriginalDepthBuffer)))
					D3DLog.Print("WARNING: GetRenderTarget(DepthBuffer) FAILED\n");

				SetFBRenderTarget();
            }
            else
                D3DLog.Print("FAILED!\n");
        }
    }

    return FALSE;
}

void D3DRenderDevice::Unlock()
{
	if ((CurrentBackBuffer == OriginalBackBuffer) && (CurrentDepthBuffer == OriginalDepthBuffer))
	{
		// reset counters only if it's unlock for FB rendering (not render to texture stuff)
		LastFrameTris = FrameTris;
		LastFrameDIPs = FrameDIPs;
		//TextureMemory[EMemTexAvailable] = d3dev->GetAvailableTextureMem()/1024/1024;
		FrameTris = 0;
		FrameDIPs = 0;
	}

    d3dev->EndScene();
}

void D3DRenderDevice::Flip(HWND hwnd)
{
    if (IsFullScreen)
	{
        d3dev->Present(0,0,0,0);
	}
	else
    {        
        RECT r;
        GetClientRect(hwnd,&r);
        d3dev->Present(&r,0,hwnd,0);
    }
}

void D3DRenderDevice::TakeScreenshot(const char* filename)
{
	// Obtain adapter number and device window
	D3DDEVICE_CREATION_PARAMETERS dcp;
	d3dev->GetCreationParameters(&dcp);

	// Obtain width and height for the front buffer surface
	D3DDISPLAYMODE dm;
	d3d->GetAdapterDisplayMode(dcp.AdapterOrdinal, &dm);

	// Create the front buffer surface
	IDirect3DSurface9 *pSurface;
	HRESULT hr = d3dev->CreateOffscreenPlainSurface(
		dm.Width, dm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pSurface, NULL);
	if (FAILED(hr))
		return;

	// Get a copy of the front buffer
	d3dev->GetFrontBufferData(0,pSurface); //FIXME

	// Obtain the rectangle into which rendering is drawn
	// It's the client rectangle of the focus window in screen coordinates
	HWND hwnd = GEngine->Video->GetActiveView()->GetActiveWindow();

	RECT rc;
	GetClientRect(hwnd, &rc);

	ClientToScreen(hwnd, LPPOINT(&rc.left));
	ClientToScreen(hwnd, LPPOINT(&rc.right));

	// Use D3DX to save the front buffer
	D3DXSaveSurfaceToFile(filename, D3DXIFF_BMP, pSurface, 0, &rc);
}

void D3DRenderDevice::ReleaseShaders()
{
//  d3dev->SetVertexShader(0);
//  d3dev->SetPixelShader(0);
	static D3DVERTEXELEMENT9 declEnd = D3DDECL_END();

    for (int i=0; i<Shaders.Size(); ++i)
    {
        D3DShader& shader = Shaders[i];
		if (shader.Type == EShaderVertex && !D3DShader::DeclEnd(shader.Decl[0])) 
		{
			if (shader.VSh)
			{
				SAFE_RELEASE(shader.VSh);
				SAFE_RELEASE(shader.VDecl);
			}
		}
		else if (shader.Type == EShaderPixel) 
		{
			SAFE_RELEASE(shader.PSh);
		}
    }
}

void D3DRenderDevice::RestoreShaders()
{
	int usage = 0;
	if (HWClass == EHWClassGenericTnL)
		usage = D3DUSAGE_SOFTWAREPROCESSING;

    for (int index=0; index<Shaders.Size(); ++index)
    {
    	HRESULT hr = E_FAIL;
        D3DShader& shader = Shaders[index];

        switch (shader.Type)
        {
        case EShaderVertex:
			if (D3DShader::DeclEnd(shader.Decl[0]))
				continue;

			hr = d3dev->CreateVertexDeclaration(shader.Decl, &shader.VDecl);

			if (FAILED(hr))
				D3DLog.Print("FAILED to restore vertex declaration (%s)\n",DXGetErrorDescription9A(hr));

			hr = d3dev->CreateVertexShader((shader.Source.Size() ? &shader.Source[0] : 0),&shader.VSh);

			if (FAILED(hr))
				D3DLog.Print("FAILED to restore vertex shader (%s)\n",DXGetErrorDescription9A(hr));

            break;
        case EShaderPixel:
			hr = d3dev->CreatePixelShader(&shader.Source[0],&shader.PSh);
			if (FAILED(hr))
				D3DLog.Print("FAILED to restore pixel shader (%s)\n",DXGetErrorDescription9A(hr));
			break;
        }


    }
}

void D3DRenderDevice::Flush()
{
    if (d3dev) 
    {
        // release device stuff
        ReleaseDevice();

        Shaders.ClearArray();

		// release buffers
        int totalBytes=0;
        for (int i=0; i<Buffers.Size(); ++i) 
        {
			if (!Buffers[i].Ptr)
				continue;

			if (Buffers[i].Type & D3DBuffer::EIndexStream)
			{
				D3DINDEXBUFFER_DESC Desc;
				Buffers[i].IBuf->GetDesc(&Desc);
				totalBytes+=Desc.Size;
				SAFE_RELEASE(Buffers[i].IBuf);
			}
			else if (Buffers[i].Type & D3DBuffer::EVertexStream)
			{
				D3DVERTEXBUFFER_DESC Desc;
				Buffers[i].VBuf->GetDesc(&Desc);
				totalBytes+=Desc.Size;
				SAFE_RELEASE(Buffers[i].VBuf);
			}
        }
        Buffers.ClearArray();

        D3DLog.Print("All vertex buffers released\n");

		for (int i=0; i<(int)d3dCaps.MaxSimultaneousTextures; i++)
			d3dev->SetTexture(i, NULL);

        // release textures
        totalBytes=0;
        for (int i=0; i<Textures.Size(); ++i) 
        {
			if (!Textures[i].Ptr)
				continue;

            LPDIRECT3DTEXTURE9 tex=Textures[i].pTex;
            if (tex) 
            {
			    for (unsigned int j=0; j<tex->GetLevelCount(); ++j) 
                {
				    D3DSURFACE_DESC Desc;
				    if (SUCCEEDED(tex->GetLevelDesc(j,&Desc))) {
						int size = Desc.Width*Desc.Height*SurfFormatToBPP(Desc.Format)/8;
					    totalBytes+=size;
                    }
                }

                SAFE_RELEASE(tex);
            }
        }

        Textures.ClearArray();
        Textures.Add(D3DTexture());

        D3DLog.Print("All textures released\n");
		TexBytes=0;
    }
}


// RenderDevice interface
void D3DRenderDevice::CreateTexture(Texture* texture)
{
	Assert(texture);

	// init with invalid values
	texture->DeviceId		= EInvalidIndex;
	texture->OriginalWidth  = -1;
	texture->OriginalHeight	= -1;
	//texture->Width			= -1;
	//texture->Height			= -1;
	texture->PixelBytes		= -1;

	// is it from raw bytes? otherwise from file
	int rawBytes = texture->Flags & (Texture::ETexRaw32|Texture::ETexRaw8);
	int special = (texture->Type == Texture::ETexRenderTarget)
				| (texture->Type == Texture::ETexRenderTargetCubeMap)
				| (texture->Type == Texture::ETexDepthTarget)
				| (texture->Type == Texture::ETexNormCubeMap);

	// need data/size for non-special textures
	if (!special && (!texture->Data || texture->Size<=0))
		return;

	// dispatch
	switch (texture->Type)
	{
	case Texture::ETexRegular:
		if (rawBytes)
		{
			if( texture->Flags & Texture::ETexDynamic )
				CreateDynamicTexture(texture);
			else
				CreateRegularTexture(texture);
		}
		else if (texture->Flags & Texture::ETexDuDv)
			CreateDuDvTextureFromFile(texture);
		else
			CreateRegularTextureFromFile(texture);
		break;

	case Texture::ETexRenderTargetCubeMap:
		CreateRenderTargetCubeMapTextureFromFile(texture);
		break;

	case Texture::ETexCubeMap:
		if (rawBytes)
			CreateCubeMapTexture(texture);
		else
			CreateCubeMapTextureFromFile(texture);
		break;

	case Texture::ETexVolume:
		if (rawBytes || (texture->Flags & Texture::ETexAnim3D))
			CreateVolumeTexture(texture);
		else
			CreateVolumeTextureFromFile(texture);
		break;

	case Texture::ETexRenderTarget:
		CreateRenderTarget(texture);
		break;

	case Texture::ETexDepthTarget:
		CreateDepthSurface(texture);
		break;

	case Texture::ETexNormCubeMap:
		if (texture->Width)
			CreateNormCubeMap(texture);
		break;
	}
}

void D3DRenderDevice::SetFBRenderTarget()
{
    HRESULT hr;
	hr = d3dev->SetRenderTarget(0,OriginalBackBuffer);
	if (FAILED(hr)) D3DLog.Print("SetRenderTarget() FAILED with %x\n", hr);

    hr = d3dev->SetDepthStencilSurface(OriginalDepthBuffer);
	if (FAILED(hr)) D3DLog.Print("SetDepthStencilSurface() FAILED with %x\n", hr);

	CurrentBackBuffer  = OriginalBackBuffer;
	CurrentDepthBuffer = OriginalDepthBuffer;

	if (FAILED(hr))
        D3DLog.Print("SetRenderTarget() FAILED with %x\n", hr);

	SetViewport(0,0,OriginalViewport.ViewportWidth,OriginalViewport.ViewportHeight,OriginalViewport.zNear,OriginalViewport.zFar);
}

void D3DRenderDevice::SetRenderTarget(Texture* colorBuf, Texture* depthBuf, int cubeFace)
{
    IDirect3DSurface9* NewBackBuffer=0;
    IDirect3DSurface9* NewDepthBuffer=0;

    if (colorBuf && colorBuf->DeviceId>=0)
    {
        Assert(colorBuf->DeviceId<=Textures.Size());

		if (colorBuf->Type==Texture::ETexRenderTarget)
		{
			LPDIRECT3DTEXTURE9 pTex = Textures[colorBuf->DeviceId].pTex;
			if (pTex)
			{
				if (FAILED(pTex->GetSurfaceLevel(0, &NewBackBuffer)))
				{
					D3DLog.Print("GetSurfaceLevel(0) color FAILED\n");
					return;
				}
			}
		}
		else if (colorBuf->Type==Texture::ETexRenderTargetCubeMap)
		{
			static const D3DCUBEMAP_FACES CubeMapFaces[] = 
			{
				D3DCUBEMAP_FACE_POSITIVE_X,
				D3DCUBEMAP_FACE_NEGATIVE_X,
				D3DCUBEMAP_FACE_POSITIVE_Y,
				D3DCUBEMAP_FACE_NEGATIVE_Y,
				D3DCUBEMAP_FACE_POSITIVE_Z,
				D3DCUBEMAP_FACE_NEGATIVE_Z,
			};

			LPDIRECT3DCUBETEXTURE9 pCube = Textures[colorBuf->DeviceId].pCube;
			if (pCube)
			{
				if (FAILED(pCube->GetCubeMapSurface(CubeMapFaces[cubeFace], 0, &NewBackBuffer)))
				{
					D3DLog.Print("GetCubeMapSurface(0) color FAILED\n");
					return;
				}
			}
		}
    }

    if (depthBuf && depthBuf->DeviceId>=0)
    {
        Assert(depthBuf->DeviceId<=Textures.Size());

		if (depthBuf->Type==Texture::ETexDepthTarget)
		{
			LPDIRECT3DSURFACE9 pSurf = Textures[depthBuf->DeviceId].pSurf;
			if (pSurf)
			{
				NewDepthBuffer = pSurf;
			}
		}
    }

    HRESULT hr;
	hr = d3dev->SetRenderTarget(0,NewBackBuffer);
	if (FAILED(hr)) D3DLog.Print("SetRenderTarget() FAILED with %x\n", hr);

	ResetTextureStages();

    hr = d3dev->SetDepthStencilSurface(NewDepthBuffer);
	if (FAILED(hr)) D3DLog.Print("SetDepthStencilSurface() FAILED with %x\n", hr);

	if (NewBackBuffer!=CurrentBackBuffer)
	{
		NewBackBuffer->Release();
	}

	CurrentBackBuffer  = NewBackBuffer;
	CurrentDepthBuffer = NewDepthBuffer;


	if (!colorBuf)
		SetViewport(0,0,OriginalViewport.ViewportWidth,OriginalViewport.ViewportHeight,OriginalViewport.zNear,OriginalViewport.zFar);
}

BOOL D3DRenderDevice::SetRenderTarget(int Index, Texture* ColorBuf, int CubeFace)
{
    IDirect3DSurface9* NewBackBuffer=0;

    if (ColorBuf && ColorBuf->DeviceId>=0)
    {
        Assert(ColorBuf->DeviceId<=Textures.Size());

		if (ColorBuf->Type == Texture::ETexRenderTarget)
		{
			LPDIRECT3DTEXTURE9 pTex = Textures[ColorBuf->DeviceId].pTex;
			if (pTex)
			{
				if (FAILED(pTex->GetSurfaceLevel(0, &NewBackBuffer)))
				{
					D3DLog.Print("GetSurfaceLevel(0) color FAILED\n");
					return 0;
				}
			}
		}
		else if (ColorBuf->Type == Texture::ETexRenderTargetCubeMap)
		{
			static const D3DCUBEMAP_FACES CubeMapFaces[] = 
			{
				D3DCUBEMAP_FACE_POSITIVE_X,
				D3DCUBEMAP_FACE_NEGATIVE_X,
				D3DCUBEMAP_FACE_POSITIVE_Y,
				D3DCUBEMAP_FACE_NEGATIVE_Y,
				D3DCUBEMAP_FACE_POSITIVE_Z,
				D3DCUBEMAP_FACE_NEGATIVE_Z,
			};

			LPDIRECT3DCUBETEXTURE9 pCube = Textures[ColorBuf->DeviceId].pCube;
			if (pCube)
			{
				Assert(CubeFace>=0 && CubeFace <6);
				if (FAILED(pCube->GetCubeMapSurface(CubeMapFaces[CubeFace], 0, &NewBackBuffer)))
				{
					D3DLog.Print("GetCubeMapSurface(0) color FAILED\n");
					return 0;
				}
			}
		}
    }

	HRESULT hr;
	if (!ColorBuf && Index == 0)
	{
		hr = d3dev->SetRenderTarget(0, OriginalBackBuffer);
		if (SUCCEEDED(hr))
		{
			SetViewport
			(
				0, 0,
				OriginalViewport.ViewportWidth, OriginalViewport.ViewportHeight,
				OriginalViewport.zNear, OriginalViewport.zFar
			);
		}
	}
	else
	{
		hr = d3dev->SetRenderTarget(Index, NewBackBuffer);
		if (NewBackBuffer)
			NewBackBuffer->Release();
	}

	if (FAILED(hr)) 
		D3DLog.Print("SetRenderTarget() FAILED with %x\n", hr);

	return SUCCEEDED(hr);
}

BOOL D3DRenderDevice::SetDepthTarget(Texture* DepthBuf)
{
    IDirect3DSurface9* NewDepthBuffer=0;

    if (DepthBuf && DepthBuf->DeviceId>=0)
    {
        Assert(DepthBuf->DeviceId<=Textures.Size());

		if (DepthBuf->Type==Texture::ETexDepthTarget)
		{
			LPDIRECT3DSURFACE9 pSurf = Textures[DepthBuf->DeviceId].pSurf;
			if (pSurf)
			{
				NewDepthBuffer = pSurf;
			}
		}
    }

    HRESULT hr = d3dev->SetDepthStencilSurface(NewDepthBuffer);
	if (FAILED(hr)) 
		D3DLog.Print("SetDepthStencilSurface() FAILED with %x\n", hr);

	return SUCCEEDED(hr);
}

inline static DWORD MapVertexTypeToFVF(DWORD type)
{
    // check predefined types first
    if (type == RenderDevice::EStreamNone)
        return 0;
    if (type == RenderDevice::EMeshVertex)
        return D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2;
    if (type == RenderDevice::EMeshVertexNTU)
        return D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
    if (type == RenderDevice::EScreenVertex || type == RenderDevice::ESimpleVertex)
        return D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;

    DWORD FVF=0;
    if(type & RenderDevice::EVertexPos)         FVF|=D3DFVF_XYZ;
    if(type & RenderDevice::EVertexDiffuse)     FVF|=D3DFVF_DIFFUSE;            
    if(type & RenderDevice::EVertexNormal)      FVF|=D3DFVF_NORMAL;
    if(type & RenderDevice::EVertexTU1)         FVF|=D3DFVF_TEX1;

    return FVF;
}

int D3DRenderDevice::CreateStream(void* sourceData, DWORD type, int count, int stride)
{
	//type |= EUsageSWVP;

    int index=EInvalidIndex;
	DWORD FVF=0;

    HRESULT hr;
    LPDIRECT3DVERTEXBUFFER9 VBuf=NULL;
    LPDIRECT3DINDEXBUFFER9  IBuf=NULL;


	DWORD Usage = D3DUSAGE_WRITEONLY;

	if (!sourceData)
		Usage |= D3DUSAGE_DYNAMIC;
	if (type & EUsageSWVP)
		Usage |= D3DUSAGE_SOFTWAREPROCESSING;

	D3DFORMAT format = D3DFMT_INDEX16;

	if (type & RenderDevice::EIndex16)
	{
        format = D3DFMT_INDEX16;
		stride = sizeof(WORD);
	}
	else if  (type & RenderDevice::EIndex32)
	{
        format = D3DFMT_INDEX32;
		stride = sizeof(DWORD);
	}

    if (type & RenderDevice::EIndex16 || type & RenderDevice::EIndex32) 
	{
       	hr=d3dev->CreateIndexBuffer
	    (
		    static_cast<UINT>(count*stride),
            Usage,
            format,
		    D3DPOOL_DEFAULT,
		    &IBuf,
			NULL
	    );

    }
    else 
	{
        hr=d3dev->CreateVertexBuffer
        (
            static_cast<UINT>(count*stride),
            Usage,
            0,
            D3DPOOL_DEFAULT,
            &VBuf,
			NULL
        );
    }
   
	if (FAILED(hr))
	{		
		D3DLog.Print("ERROR:\tCreateBuffer failed (%s), falling back to SYSTEMMEM!\n",
			DXGetErrorDescription9A(hr));

		if (type & RenderDevice::EIndex16 || type & RenderDevice::EIndex32) 
		{
       		hr=d3dev->CreateIndexBuffer
			(
				static_cast<UINT>(count*stride),
				Usage,
				format,
				D3DPOOL_SYSTEMMEM,
				&IBuf,
				NULL
			);
		}
		else 
		{
			hr=d3dev->CreateVertexBuffer
			(
				static_cast<UINT>(count*stride),
				Usage,
				0,
				D3DPOOL_SYSTEMMEM,
				&VBuf,
				NULL
			);
		}

		if (FAILED(hr))
		{		
			// this is realy bad!
			D3DLog.Print("ERROR:\tCreateBuffer in SYSTEMMEM failed (%s), bailing out!\n",
				DXGetErrorDescription9A(hr));

			exit(EXIT_FAILURE);

			return EInvalidIndex;
		}
	}

    // find unused slot
	D3DBuffer* buf=0;
	for(int i=0; i<Buffers.Size(); ++i)
	{
        if(Buffers[i].Type==D3DBuffer::EUnusedStream)
		{
			buf=&Buffers[i];
			index=i;
			break;
		}
	}

	if(!buf)
	{
		index=Buffers.Size();
		buf=Buffers.Create();
		Assert(buf);
	}

    buf->FVF=FVF;

    if(type & RenderDevice::EIndex16 || type & RenderDevice::EIndex32) 
	{
        buf->Type=D3DBuffer::EIndexStream;
        buf->IBuf=IBuf;

        if(sourceData)
            D3DStaticBufferFill(buf->IBuf,sourceData,count*stride);

		D3DINDEXBUFFER_DESC desc;
		buf->IBuf->GetDesc(&desc);

		TextureMemory[EMemIBs] += (float)desc.Size/(1024.f*1024.f);

    } 
	else 
	{
        buf->Type=D3DBuffer::EVertexStream;
        buf->VBuf=VBuf;

        if(sourceData)
            D3DStaticBufferFill(buf->VBuf,sourceData,count*stride);

		D3DVERTEXBUFFER_DESC desc;
		buf->VBuf->GetDesc(&desc);

		TextureMemory[EMemVBs] += (float)desc.Size/(1024.f*1024.f);
    }

    buf->ElemCount  = count;
    buf->Stride     = stride;
    buf->SourceData = sourceData;

	if (type & EUsageSWVP)
		buf->Type	|= D3DBuffer::EUsageSWVP;

	return index;
}


BYTE* D3DRenderDevice::LockStream(int streamId, int pos, int count)
{
    const D3DBuffer& buf=Buffers[streamId];
    // only dynamic buffers can be locked
    if (NULL == buf.SourceData) 
    {
        // user can pass zero count...
        if(pos>0 && count==0)
            count=buf.ElemCount-pos;

        if(buf.Type & D3DBuffer::EVertexStream) 
            return D3DDynamicBufferLock(buf.VBuf,pos*buf.Stride,count*buf.Stride);

        if(buf.Type & D3DBuffer::EIndexStream)  
            return D3DDynamicBufferLock(buf.IBuf,pos*buf.Stride,count*buf.Stride);
    }

    return 0;
}

void D3DRenderDevice::UnlockStream(int streamId)
{
    const D3DBuffer& buf=Buffers[streamId];
    if(buf.Type & D3DBuffer::EVertexStream) buf.VBuf->Unlock(); else
    if(buf.Type & D3DBuffer::EIndexStream)  buf.IBuf->Unlock();
}


int D3DRenderDevice::CreateShader(int type, const DWORD* source, const DWORD* streamTypeList, int listCount)
{
	// create a single declarator for all data streams
	D3DVERTEXELEMENT9 decl[MAXD3DDECLLENGTH];

	if (HWClass == EHWClassGenericTnL && type==EShaderPixel)
		return EInvalidIndex;

	// process vertex shaders - either programmable or fixed
	bool showDecl = false;
	if (EShaderVertex == type)
	{
		static D3DVERTEXELEMENT9 declEnd = D3DDECL_END();

		if (!source)	// it's a fixed stream
		{
			if (1 == listCount)	// 1 component = use FVF
			{
				int index = EInvalidIndex;
				DWORD fvf = MapVertexTypeToFVF(streamTypeList[0]);
				if (fvf)
				{
					index = Shaders.Size();
					D3DShader* shader	= Shaders.Create();
					shader->Type		= EShaderVertex;
					shader->VSh			= 0;
					shader->FVF			= fvf;
					shader->Decl[0]		= declEnd;
					shader->Source.ClearArray();

					//D3DLog.Print("FVF shader[%d]\n",index);
				}

				return index;
			}

			// multi stream - create FVF vertex shader

			int c=0,stream=0;
			int texBase=0;
	
			for (int i=0; i<listCount; ++i) 
			{
				const DWORD streamType = streamTypeList[i];
				//D3DLog.Print("adding Type: %08x\n",streamType);

				WORD offset = 0;

				D3DVERTEXELEMENT9 vdecl;
				vdecl.Stream 	= stream++;
				vdecl.Offset 	= offset;
				vdecl.Type   	= 0;
				vdecl.Method 	= D3DDECLMETHOD_DEFAULT;
				vdecl.Usage  	= 0;
				vdecl.UsageIndex = 0;
     
				if (streamType & EVertexPos) 
				{
					decl[c] = vdecl;
					vdecl.Offset += sizeof(float)*3;

					decl[c].Type = D3DDECLTYPE_FLOAT3;
					decl[c].Usage = D3DDECLUSAGE_POSITION;
					c++;
				}
				if (streamType & EVertexDiffuse)	
				{
					decl[c] = vdecl;
					vdecl.Offset += 4;

					decl[c].Type = D3DDECLTYPE_D3DCOLOR;
					decl[c].Usage = D3DDECLUSAGE_COLOR;
					c++;
				}
				if (streamType & EVertexNormal)		
				{
					decl[c] = vdecl;
					vdecl.Offset += sizeof(float)*3;

					decl[c].Type = D3DDECLTYPE_FLOAT3;
					decl[c].Usage = D3DDECLUSAGE_NORMAL;
					c++;
				}

				int texCount=(streamType & ETexCountMask)>>ETexCountShift;
				for (int j=0; j<texCount; ++j)
				{
					decl[c] = vdecl;
					vdecl.Offset += sizeof(float)*2;

					decl[c].Type = D3DDECLTYPE_FLOAT2;
					decl[c].Usage = D3DDECLUSAGE_TEXCOORD;
					decl[c].UsageIndex = j;
					c++;
				}

				texBase+=texCount;
			}            
			decl[c++]=declEnd;
		}
		else // they passed in source = programmable shader
		{
			int c=0,stream=0;

			for (int i=0; i<listCount; ++i) 
			{
				const DWORD streamDef = streamTypeList[i];
				//D3DLog.Print("adding stream: %08x\n",streamDef);

				WORD offset = 0;

				D3DVERTEXELEMENT9 vdecl;
				vdecl.Stream 	= stream++;
				vdecl.Offset 	= offset;
				vdecl.Type   	= 0;
				vdecl.Method 	= D3DDECLMETHOD_DEFAULT;
				vdecl.Usage  	= 0;
				vdecl.UsageIndex = 0;

				if (streamDef==MeshVertex::ShaderStreamNTU)
				{
					decl[c] = vdecl;
					decl[c].Type = D3DDECLTYPE_FLOAT3;
					decl[c].Usage = D3DDECLUSAGE_POSITION;

					vdecl.Offset += sizeof(float)*3;
					c++;

					decl[c] = vdecl;
					decl[c].Type = D3DDECLTYPE_FLOAT3;
					decl[c].Usage = D3DDECLUSAGE_NORMAL;

					vdecl.Offset += sizeof(float)*3;
					c++;

					decl[c] = vdecl;
					decl[c].Type = D3DDECLTYPE_FLOAT2;
					decl[c].Usage = D3DDECLUSAGE_TEXCOORD;

					vdecl.Offset += sizeof(float)*2;
					c++;
				}
				else if (streamDef==MeshVertex::ShaderStreamN)
				{
					decl[c] = vdecl;
					decl[c].Type = D3DDECLTYPE_FLOAT3;
					decl[c].Usage = D3DDECLUSAGE_NORMAL;

					vdecl.Offset += sizeof(float)*3;
					c++;
				}
				else if (streamDef==MeshVertex::ShaderStreamTU2)
				{
					decl[c] = vdecl;
					decl[c].Type = D3DDECLTYPE_FLOAT3;
					decl[c].Usage = D3DDECLUSAGE_POSITION;

					vdecl.Offset += sizeof(float)*4; // stride!!!
					c++;

					decl[c] = vdecl;
					decl[c].Type = D3DDECLTYPE_FLOAT2;
					decl[c].Usage = D3DDECLUSAGE_TEXCOORD;
					decl[c].UsageIndex = 0;

					vdecl.Offset += sizeof(float)*2;
					c++;

					decl[c] = vdecl;
					decl[c].Type = D3DDECLTYPE_FLOAT2;
					decl[c].Usage = D3DDECLUSAGE_TEXCOORD;
					decl[c].UsageIndex = 1;

					vdecl.Offset += sizeof(float)*2;
					c++;
				}
				else if (streamDef==MeshVertex::ShaderStreamWI)
				{
					decl[c] = vdecl;
					decl[c].Type = D3DDECLTYPE_FLOAT3;
					decl[c].Usage = D3DDECLUSAGE_BLENDWEIGHT;

					vdecl.Offset += sizeof(float)*3;
					c++;

					decl[c] = vdecl;
					decl[c].Type = D3DDECLTYPE_D3DCOLOR;
					decl[c].Usage = D3DDECLUSAGE_BLENDINDICES;
					decl[c].UsageIndex = 0;

					vdecl.Offset += sizeof(DWORD)*1;
					c++;
				}
				else if (streamDef==MeshVertex::ShaderStreamCTU)
				{
					decl[c] = vdecl;
					decl[c].Type = D3DDECLTYPE_FLOAT3;
					decl[c].Usage = D3DDECLUSAGE_POSITION;

					vdecl.Offset += sizeof(float)*3;
					c++;

					decl[c] = vdecl;
					decl[c].Type = D3DDECLTYPE_D3DCOLOR;
					decl[c].Usage = D3DDECLUSAGE_COLOR;

					vdecl.Offset += sizeof(DWORD)*1;
					c++;

					decl[c] = vdecl;
					decl[c].Type = D3DDECLTYPE_FLOAT2;
					decl[c].Usage = D3DDECLUSAGE_TEXCOORD;

					vdecl.Offset += sizeof(float)*2;
					c++;
				}
				else
				{
					//Assert(false);
					int tcIndex = 0;
					for (int j=0; j<8; ++j)
					{
						const DWORD val = GetShaderComponent(j,streamDef);
						if (0 == val)
							break;

						static const DWORD table [] =
						{
							0,
							D3DDECLTYPE_FLOAT1,			// 1
							D3DDECLTYPE_FLOAT2,			// 2
							D3DDECLTYPE_FLOAT3,			// 3
							D3DDECLTYPE_FLOAT4,			// 4
							D3DDECLTYPE_D3DCOLOR,		// 5
							D3DDECLTYPE_UBYTE4,			// 6
							D3DDECLTYPE_SHORT2,			// 7
							D3DDECLTYPE_SHORT4,			// 8
							D3DDECLTYPE_SHORT2N,		// 9
							D3DDECLTYPE_SHORT4N,		// 10
							D3DDECLTYPE_USHORT2N,		// 11
							D3DDECLTYPE_USHORT4N,		// 12
							D3DDECLTYPE_UBYTE4N,		// 13
							D3DDECLTYPE_UDEC3,			// 14
							D3DDECLTYPE_DEC3N			// 15
						};
						static const DWORD sizes [] =
						{
							0,
							sizeof(float),
							sizeof(float)*2,
							sizeof(float)*3,
							sizeof(float)*4,
							4,
							4,
							sizeof(short)*2,
							sizeof(short)*4,
							sizeof(short)*2,
							sizeof(short)*4,
							sizeof(short)*2,
							sizeof(short)*4,
							4,
							4,
							4
						};

						// figure out usage
						BYTE usage = D3DDECLUSAGE_TEXCOORD;
						if (j == 0 && (table[val] == D3DDECLTYPE_FLOAT3 || table[val] == D3DDECLTYPE_FLOAT4))
							usage = D3DDECLUSAGE_POSITION;
						else if ((j == 1 || j == 2) && (table[val] == D3DDECLTYPE_FLOAT3 || table[val] == D3DDECLTYPE_FLOAT4))
							usage = D3DDECLUSAGE_NORMAL;
						else if (table[val] == D3DDECLTYPE_D3DCOLOR)
							usage = D3DDECLUSAGE_COLOR;
							
						decl[c] = vdecl;
						decl[c].Type = table[val];
						decl[c].Usage = usage;

						if (usage == D3DDECLUSAGE_TEXCOORD)
							decl[c].UsageIndex = tcIndex++;

						vdecl.Offset += sizes[val];
						c++;

						showDecl = true;
					}
				}

			}
			decl[c++]=declEnd;
		}
	}

	if (showDecl)
		DumpDecl(D3DLog, decl);

	// check cached shaders
	int index = EInvalidIndex;
    for (int i=0; i<Shaders.Size(); ++i) 
	{
		if (Shaders[i].Type != type)
			continue;

        BOOL same=TRUE;
		// for fixed or vertex shader - compare declarator
		if (type == EShaderVertex)
		{
			for (int j=0; j<MAXD3DDECLLENGTH; ++j) 
			{
				if (!D3DShader::CmpDecls(decl[j],Shaders[i].Decl[j])) 
				{
					same=FALSE;
					break;
				}

				if (D3DShader::DeclEnd(Shaders[i].Decl[j]))
					break;
			}
		}

        if (same) 
		{
			if (source)
			{
                for (int t=0; D3DVS_END()!=source[t]; ++t)
                    if (source[t]!=Shaders[i].Source[t])
                    {
                        same = FALSE;
                        break;
                    }
			}

			if (same)
			{
				Assert(Shaders[i].Type==type);
                //D3DLog.Print("CreateShader(): cached (%d): %d\n",type,Shaders[i].Handle);
				index = i;
				break;
			}
        }
    }

	if (EInvalidIndex == index) // need to create a new one
	{
		HRESULT hr = E_FAIL;
		DWORD usage=0;

		if (HWClass == EHWClassGenericTnL && source)
			usage = D3DUSAGE_SOFTWAREPROCESSING;

		D3DShader* shader=NULL;
		index = Shaders.Size();

		if (type == EShaderVertex) 
		{
			LPDIRECT3DVERTEXSHADER9 VSh;
			LPDIRECT3DVERTEXDECLARATION9 VDecl;
			hr = d3dev->CreateVertexDeclaration(decl, &VDecl);
			if (FAILED(hr))
			{
				D3DLog.Print("FAILED to create vertex declaration (%s)\n",DXGetErrorDescription9A(hr));
				return EInvalidIndex;
			}
			hr = d3dev->CreateVertexShader(source,&VSh);
			if (FAILED(hr))
			{
				D3DLog.Print("FAILED to create vertex shader (%s)\n",DXGetErrorDescription9A(hr));
				return EInvalidIndex;
			}


			shader = Shaders.Create();
			shader->VSh		= VSh;
			shader->VDecl	= VDecl;
			shader->Type	= EShaderVertex;

		}
		else if (type == EShaderPixel)
		{
			LPDIRECT3DPIXELSHADER9 PSh;
			hr = d3dev->CreatePixelShader(source,&PSh);

			if (FAILED(hr))
			{
				D3DLog.Print("FAILED to create pixel shader (%s)\n",DXGetErrorDescription9A(hr));
				return EInvalidIndex;
			}

			shader = Shaders.Create();
			shader->PSh		= PSh;
			shader->Type	= EShaderPixel;
		}
		else
		{
			Assert(false);
			return EInvalidIndex;
		}


        // remember decl and source for programmable shaders
		if (type == EShaderVertex)
		{
			for (int i=0; i<MAXD3DDECLLENGTH; ++i) 
			{
				shader->Decl[i]=decl[i];
				if (D3DShader::DeclEnd(decl[i]))
					break;
			}
        }

        if (source)
        {
			int i=0;
			do {
				shader->Source.Add(source[i]);
			}
			while (source[i++] != D3DVS_END());

			/*
            for (int i=0; i<1024; ++i)
            {
                shader->Source.Add(source[i]);
				if (source[i]==D3DVS_END()) {
					D3DLog.Print("Source: %d tokens\n",i);
                    break;
				}
            }
			*/
		}
	}


	/*
    D3DLog.Print("CreateShader[%d]: %s (%d)\n",
                index, (type==EShaderVertex ? "vertex" : "pixel"),
                Shaders[index].Handle);
	*/

	return index;
}

void D3DRenderDevice::SetState(const RenderState& state)
{
    // local translation RenderDevice->D3DRenderDevice tables
    static const D3DCMPFUNC CompareFuncMap[] = 
    {
        D3DCMP_NEVER,           // EFuncNever
        D3DCMP_LESS,            // EFuncLess
        D3DCMP_EQUAL,           // EFuncEqual
        D3DCMP_LESSEQUAL,       // EFuncLessEqual
        D3DCMP_GREATER,         // EFuncGreater
        D3DCMP_NOTEQUAL,        // EFuncNotEqual
        D3DCMP_GREATEREQUAL,    // EFuncGreaterEqual
        D3DCMP_ALWAYS           // EFuncAlways
    };

    static const D3DSTENCILOP StencilOpMap[] =
    {
        D3DSTENCILOP_KEEP,      // EStencilOpKeep      = 0,
        D3DSTENCILOP_ZERO,      // EStencilOpZero      = 1,
        D3DSTENCILOP_REPLACE,   // EStencilOpReplace   = 2,
        D3DSTENCILOP_INCRSAT,   // EStencilOpIncrSat   = 3,
        D3DSTENCILOP_DECRSAT,   // EStencilOpDecrSat   = 4,
        D3DSTENCILOP_INVERT,    // EStencilOpInvert    = 5,
        D3DSTENCILOP_INCR,      // EStencilOpIncr      = 6,
        D3DSTENCILOP_DECR       // EStencilOpDecr      = 7,
    };

    static const D3DFILLMODE FillModeMap[] =
    {
        D3DFILL_SOLID,          // EFillSolid      = 0,
        D3DFILL_WIREFRAME,      // EFillLines      = 1,
        D3DFILL_POINT           // EFillPoints     = 2,
    };

    static const D3DCULL CullModeMap[] =
    {
        D3DCULL_NONE,           // ECullNone       = 0,
        D3DCULL_CCW,            // ECullCCW        = 1,
        D3DCULL_CW              // ECullCW         = 2,
    };

    // Depth buffer/function related
    if(CachedState.DepthFunc!=state.DepthFunc)
    {
        d3dev->SetRenderState(D3DRS_ZFUNC,CompareFuncMap[state.DepthFunc]);
        CachedState.DepthFunc=state.DepthFunc;
    }

    if(CachedState.DepthTest!=state.DepthTest)
    {
        d3dev->SetRenderState(D3DRS_ZENABLE, state.DepthTest ? D3DZB_TRUE : D3DZB_FALSE);
        CachedState.DepthTest=state.DepthTest;
    }

    if(CachedState.DepthWrite!=state.DepthWrite)
    {
        d3dev->SetRenderState(D3DRS_ZWRITEENABLE, state.DepthWrite);
        CachedState.DepthWrite=state.DepthWrite;
    }

    if(CachedState.ColorWrite!=state.ColorWrite || CachedState.AlphaWrite!=state.AlphaWrite)
    {
        const UINT setColor=D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE;
		const UINT setAlpha=D3DCOLORWRITEENABLE_ALPHA;
        d3dev->SetRenderState(D3DRS_COLORWRITEENABLE, (state.ColorWrite ? setColor : 0) | (state.AlphaWrite ? setAlpha : 0));
        CachedState.ColorWrite=state.ColorWrite;
        CachedState.AlphaWrite=state.AlphaWrite;
    }

    // stencil buffer/function related
    if(CachedState.StencilTest!=state.StencilTest)
    {
        d3dev->SetRenderState(D3DRS_STENCILENABLE, state.StencilTest ? TRUE : FALSE);
		if (state.StencilTest)
			StencilSurfaceTouched = TRUE;
        CachedState.StencilTest=state.StencilTest;
    }

    if(CachedState.StencilRef!=state.StencilRef)
    {
        d3dev->SetRenderState(D3DRS_STENCILREF, state.StencilRef);
        CachedState.StencilRef=state.StencilRef;
    }

    if(CachedState.StencilMask!=state.StencilMask)
    {
        d3dev->SetRenderState(D3DRS_STENCILMASK, state.StencilMask);
        CachedState.StencilMask=state.StencilMask;
    }

    if(CachedState.StencilFunc!=state.StencilFunc)
    {
        d3dev->SetRenderState(D3DRS_STENCILFUNC, CompareFuncMap[state.StencilFunc]);
        CachedState.StencilFunc=state.StencilFunc;
    }

    if(CachedState.StencilFailOp!=state.StencilFailOp)
    {
        d3dev->SetRenderState(D3DRS_STENCILFAIL, StencilOpMap[state.StencilFailOp]);
        CachedState.StencilFailOp=state.StencilFailOp;
    }

    if(CachedState.StencilZFailOp!=state.StencilZFailOp)
    {
        d3dev->SetRenderState(D3DRS_STENCILZFAIL, StencilOpMap[state.StencilZFailOp]);
        CachedState.StencilZFailOp=state.StencilZFailOp;
    }

    if(CachedState.StencilPassOp!=state.StencilPassOp)
    {
        d3dev->SetRenderState(D3DRS_STENCILPASS, StencilOpMap[state.StencilPassOp]);
        CachedState.StencilPassOp=state.StencilPassOp;
    }

    // alpha test
    if(CachedState.AlphaTest!=state.AlphaTest)
    {
        d3dev->SetRenderState(D3DRS_ALPHATESTENABLE, state.AlphaTest ? TRUE : FALSE);
        CachedState.AlphaTest=state.AlphaTest;
    }

    if(CachedState.AlphaFunc!=state.AlphaFunc)
    {
        d3dev->SetRenderState(D3DRS_ALPHAFUNC,CompareFuncMap[state.AlphaFunc]);
        CachedState.AlphaFunc=state.AlphaFunc;
    }

    if (CachedState.AlphaRef!=state.AlphaRef)
    {
        d3dev->SetRenderState(D3DRS_ALPHAREF,DWORD(state.AlphaRef));
        CachedState.AlphaRef=state.AlphaRef;
    }

    // scissor test
    if (CachedState.ScissorTest!=state.ScissorTest)
    {
        d3dev->SetRenderState(D3DRS_SCISSORTESTENABLE, state.ScissorTest ? TRUE : FALSE);
        CachedState.ScissorTest = state.ScissorTest;
    }

	// blend modes (inflict fogColor!)
	DWORD fogColor = CachedState.FogColor;

    switch(state.BlendMode)
    {
    case RenderState::EBlendNone:				fogColor = SavedFogColor;   break;
    case RenderState::EBlendAlpha:				fogColor = 0;				break;
    case RenderState::EBlendAdd:				fogColor = 0;				break;
    case RenderState::EBlendTranslucent:		fogColor = SavedFogColor;   break;
    case RenderState::EBlendModulate:			fogColor = (DWORD) -1;      break;
    case RenderState::EBlendModulate2x:			fogColor = (DWORD) -1;      break;
    case RenderState::EBlendFilter:				fogColor = (DWORD) -1;      break;
	case RenderState::EBlendInvModulate:		fogColor = 0;				break;
	case RenderState::EBlendSubtract:			fogColor = 0;				break;
	case RenderState::EBlendRevSubtract:		fogColor = 0;				break;
    case RenderState::EBlendDestTranslucent:	fogColor = SavedFogColor;	break;
    case RenderState::EBlendDestAlpha:			fogColor = 0;				break;	
	}

	if(CachedState.FogColor!=fogColor)
	{
		d3dev->SetRenderState(D3DRS_FOGCOLOR, fogColor);
		CachedState.FogColor=fogColor;
	}

    if(CachedState.BlendMode!=state.BlendMode) 
    {
        switch(state.BlendMode)
        {
        case RenderState::EBlendNone:
            d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
            break;

        case RenderState::EBlendAlpha:
            d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			d3dev->SetRenderState(D3DRS_BLENDOP,	D3DBLENDOP_ADD);
            d3dev->SetRenderState(D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA);
            d3dev->SetRenderState(D3DRS_DESTBLEND,	D3DBLEND_ONE);
            break;

        case RenderState::EBlendAdd:
            d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			d3dev->SetRenderState(D3DRS_BLENDOP,	D3DBLENDOP_ADD);
            d3dev->SetRenderState(D3DRS_SRCBLEND,	D3DBLEND_ONE);
            d3dev->SetRenderState(D3DRS_DESTBLEND,	D3DBLEND_ONE);
            break;

        case RenderState::EBlendTranslucent:
            d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			d3dev->SetRenderState(D3DRS_BLENDOP,	D3DBLENDOP_ADD);
            d3dev->SetRenderState(D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA);
            d3dev->SetRenderState(D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA);
            break;

        case RenderState::EBlendModulate:
            d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			d3dev->SetRenderState(D3DRS_BLENDOP,	D3DBLENDOP_ADD);
			d3dev->SetRenderState(D3DRS_SRCBLEND,   D3DBLEND_DESTCOLOR);
			d3dev->SetRenderState(D3DRS_DESTBLEND,  D3DBLEND_ZERO);
            break;

        case RenderState::EBlendModulate2x:
            d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			d3dev->SetRenderState(D3DRS_BLENDOP,	D3DBLENDOP_ADD);
			d3dev->SetRenderState(D3DRS_SRCBLEND,   D3DBLEND_DESTCOLOR);
			d3dev->SetRenderState(D3DRS_DESTBLEND,  D3DBLEND_SRCCOLOR);
            break;

        case RenderState::EBlendFilter:
            d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			d3dev->SetRenderState(D3DRS_BLENDOP,	D3DBLENDOP_ADD);
            d3dev->SetRenderState(D3DRS_SRCBLEND,	D3DBLEND_ZERO);
            d3dev->SetRenderState(D3DRS_DESTBLEND,	D3DBLEND_SRCCOLOR);
            break;

		case RenderState::EBlendInvModulate:
			d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			d3dev->SetRenderState(D3DRS_BLENDOP,	D3DBLENDOP_ADD);
			d3dev->SetRenderState(D3DRS_SRCBLEND,   D3DBLEND_ZERO);
			d3dev->SetRenderState(D3DRS_DESTBLEND,  D3DBLEND_INVSRCCOLOR);
			break;

		case RenderState::EBlendSubtract:
			d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			d3dev->SetRenderState(D3DRS_BLENDOP,	D3DBLENDOP_SUBTRACT);
			d3dev->SetRenderState(D3DRS_SRCBLEND,   D3DBLEND_ONE);
			d3dev->SetRenderState(D3DRS_DESTBLEND,  D3DBLEND_ONE);
			break;

		case RenderState::EBlendRevSubtract:
			d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			d3dev->SetRenderState(D3DRS_BLENDOP,	D3DBLENDOP_REVSUBTRACT);
			d3dev->SetRenderState(D3DRS_SRCBLEND,   D3DBLEND_ONE);
			d3dev->SetRenderState(D3DRS_DESTBLEND,  D3DBLEND_ONE);
			break;

        case RenderState::EBlendDestTranslucent:
            d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			d3dev->SetRenderState(D3DRS_BLENDOP,	D3DBLENDOP_ADD);
            d3dev->SetRenderState(D3DRS_SRCBLEND,	D3DBLEND_DESTALPHA);
            d3dev->SetRenderState(D3DRS_DESTBLEND,	D3DBLEND_INVDESTALPHA);
			fogColor = SavedFogColor;
            break;

        case RenderState::EBlendDestAlpha:
            d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			d3dev->SetRenderState(D3DRS_BLENDOP,	D3DBLENDOP_ADD);
            d3dev->SetRenderState(D3DRS_SRCBLEND,	D3DBLEND_DESTALPHA);
            d3dev->SetRenderState(D3DRS_DESTBLEND,	D3DBLEND_ONE);
            break;
		}

        CachedState.BlendMode=state.BlendMode;
    }

    if(CachedState.Lighting!=state.Lighting)
    {
        d3dev->SetRenderState(D3DRS_LIGHTING, state.Lighting);
        CachedState.Lighting=state.Lighting;
    }

    if(CachedState.AmbientColor!=state.AmbientColor)
    {
		d3dev->SetRenderState(D3DRS_AMBIENT, state.AmbientColor);
        CachedState.AmbientColor=state.AmbientColor;
    }

    if(CachedState.NormalizeNormals!=state.NormalizeNormals)
    {
        d3dev->SetRenderState(D3DRS_NORMALIZENORMALS, state.NormalizeNormals);
        CachedState.NormalizeNormals=state.NormalizeNormals;
    }

	if(CachedState.SpecularEnable!=state.SpecularEnable)
	{
		d3dev->SetRenderState(D3DRS_SPECULARENABLE, state.SpecularEnable);
		CachedState.SpecularEnable=state.SpecularEnable;
	}

    if(CachedState.FillMode!=state.FillMode)
    {
        d3dev->SetRenderState(D3DRS_FILLMODE,FillModeMap[state.FillMode]);
        CachedState.FillMode=state.FillMode;
    }

    if (CachedState.CullMode!=state.CullMode)
    {
		int CullMode = state.CullMode;
		if (CullFlip && CullMode != RenderState::ECullNone)		
			CullMode = (CullMode == RenderState::ECullCCW) ? RenderState::ECullCW : RenderState::ECullCCW;
			
        d3dev->SetRenderState(D3DRS_CULLMODE,CullModeMap[CullMode]);
        CachedState.CullMode = state.CullMode;

        //d3dev->SetRenderState(D3DRS_CULLMODE,CullModeMap[state.CullMode]);
        //CachedState.CullMode=state.CullMode;
    }

    if(CachedState.ConstColor!=state.ConstColor)
    {
        CachedState.ConstColor=state.ConstColor;
        d3dev->SetRenderState(D3DRS_TEXTUREFACTOR, CachedState.ConstColor);
    }
}

void D3DRenderDevice::SetObjectTransform(const Matrix* matPtr)
{
	d3dev->SetTransform(D3DTS_WORLD,_DXMAT(matPtr));
}

void D3DRenderDevice::SetCameraTransform(const Matrix* matPtr)
{
	d3dev->SetTransform(D3DTS_VIEW,_DXMAT(matPtr));
}

void D3DRenderDevice::SetScreenTransform(const Matrix* matPtr)
{
	d3dev->SetTransform(D3DTS_PROJECTION,_DXMAT(matPtr));
}

void D3DRenderDevice::SetSWVP(BOOL enable)
{
	if (CachedSWVP != enable)
	{
		d3dev->SetSoftwareVertexProcessing(enable);
		CachedSWVP = enable;
	}
}

void D3DRenderDevice::SetFVF(DWORD FVF)
{
	if (CachedFVF != FVF)
	{
		d3dev->SetFVF(FVF);
		CachedFVF = FVF;
	}
}

void D3DRenderDevice::SetVertexDeclaration(LPDIRECT3DVERTEXDECLARATION9 VDecl)
{
	if (CurrentVDecl != VDecl)
	{
		d3dev->SetVertexDeclaration(VDecl);
		CurrentVDecl = VDecl;
	}
}


void D3DRenderDevice::SetStreams(const int* streams, int streamCount, int indexStream, int streamOffset)
{
	if (Buffers[streams[0]].Type & D3DBuffer::EUsageSWVP)
		SetSWVP(TRUE);
	else 
		SetSWVP(FALSE);


    for (int index=0; index<streamCount; ++index)
    {
        const D3DBuffer& buf=Buffers[streams[index]];
        Assert(buf.ElemCount==Buffers[streams[(index+1)%streamCount]].ElemCount);

		if (CachedStreams[index].VBuf!=buf.VBuf || (int)CachedStreams[index].Offset!=streamOffset)
		{
			d3dev->SetStreamSource(index,buf.VBuf,streamOffset,buf.Stride);
			CachedStreams[index].VBuf=buf.VBuf;
			CachedStreams[index].Offset=streamOffset;
		}
    }

    if (indexStream!=EInvalidIndex)
    {
        d3dev->SetIndices(Buffers[indexStream].IBuf);
        CurrentStreamIsIndexed=TRUE;
        CurrentStreamSize=Buffers[streams[0]].ElemCount;
    }
    else
        CurrentStreamIsIndexed=FALSE;
}

void D3DRenderDevice::SetShaderMemory(DWORD addr, const void* data, int count)
{
	Assert(data);

	d3dev->SetVertexShaderConstantF(addr, (const float*)data, count);

#if 0
	for (int i=0; i<count; i++)
	{
		const float *F = ((const float*)data)+4*i;
		if (CachedVSConst[addr].X!=F[0] || CachedVSConst[addr].Y!=F[1] || 
			CachedVSConst[addr].Z!=F[2] || CachedVSConst[addr].W!=F[3] )
		{
			d3dev->SetVertexShaderConstantF(addr,F,1);
			CachedVSConst[addr].X=F[0];
			CachedVSConst[addr].Y=F[1];
			CachedVSConst[addr].Z=F[2];
			CachedVSConst[addr].W=F[3];
		}
		addr++;
	}
#endif
}

void D3DRenderDevice::SetPixelShaderMemory(DWORD addr, const void* data, int count)
{
	// suspicious behaviour with ATI 9200, 
	// looks like driver internally sets every unitialized ps const even when unused
	// effectively screwing up constants chacheing
	// for safety lets reset cached constants

	Assert(data);

	d3dev->SetPixelShaderConstantF(addr, ((const float*)data), count);
}

void D3DRenderDevice::SetPixelShaderMemoryB(DWORD addr, const BOOL* data, int count)
{
	// suspicious behaviour with ATI 9200, 
	// looks like driver internally sets every unitialized ps const even when unused
	// effectively screwing up constants chacheing
	// for safety lets reset cached constants

	Assert(data);

	d3dev->SetPixelShaderConstantB(addr, data, count);
}

void D3DRenderDevice::SetShader(int shaderIndex, int shaderType)
{
	switch (shaderType)
	{
	case EShaderVertex:
		LPDIRECT3DVERTEXSHADER9 VSh;
		if (shaderIndex>=0 && shaderIndex<Shaders.Size())
		{
			VSh = Shaders[shaderIndex].VSh;
			Assert(Shaders[shaderIndex].Type == EShaderVertex);
		}
		else
			VSh = 0;

        if (CurrentVertexShader != VSh)
        {
		    d3dev->SetVertexShader(VSh);
            CurrentVertexShader = VSh;
        }
		if (CurrentVertexShader == 0)
		{
			d3dev->SetFVF(Shaders[shaderIndex].FVF);
		}
		else 
		{
			d3dev->SetVertexDeclaration(Shaders[shaderIndex].VDecl);
		}
		break;

	case EShaderPixel:
		LPDIRECT3DPIXELSHADER9 PSh;
		if (shaderIndex>=0 && shaderIndex<Shaders.Size())
		{
			PSh = Shaders[shaderIndex].PSh;
			Assert(Shaders[shaderIndex].Type == EShaderPixel);
		}
		else
			PSh = 0;

        if (CurrentPixelShader != PSh)
        {
		    d3dev->SetPixelShader(PSh);
            CurrentPixelShader = PSh;
        }
		break;
	}
}

void D3DRenderDevice::SetTextureState(const TextureState* stages, int stageCount)
{
    if(!stages)
        return;

    for (int i=0; i<stageCount; ++i)
    {
        const TextureState& t=stages[i]; 

        if (t.Stage>=0 && t.Stage<(int)d3dCaps.MaxTextureBlendStages)
        {

			if (t.Stage<(int)d3dCaps.MaxSimultaneousTextures && t.TexId && CachedTextureStates[t.Stage].TexId!=t.TexId)
			{
				//				int TexId = t.TexId;
				//				if (TexId!=15 && TexId!=106)	TexId=1;

				//                LPDIRECT3DTEXTURE9 pTex = (TexId>=0 && TexId<Textures.Size() ? Textures[TexId].pTex : NULL);
				LPDIRECT3DTEXTURE9 pTex = (t.TexId>0 && t.TexId<Textures.Size() ? Textures[t.TexId].pTex : NULL);
				///				D3DLog.Print("texture(%d) =  %d\n", t.Stage, t.TexId);

				d3dev->SetTexture(t.Stage,pTex);
				CachedTextureStates[t.Stage].TexId=t.TexId;
			}

			if (t.QVal && t.QVal != CachedTextureStates[t.Stage].QVal)
			{
				// compare first part of tex state settings
				if (t.V1 && t.V1 != CachedTextureStates[t.Stage].V1)
				{
					// color op
					if(t.Op && t.Op!=CachedTextureStates[t.Stage].Op)
					{
						switch(t.Op)
						{
						case TextureState::ETexOpDisable:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLOROP,D3DTOP_DISABLE);
							break;
						case TextureState::ETexOpTexture:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG1, D3DTA_TEXTURE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
							break;
						case TextureState::ETexOpDiffuse:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG1, D3DTA_DIFFUSE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
							break;
						case TextureState::ETexOpColor:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG1, D3DTA_TFACTOR);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
							break;
						case TextureState::ETexOpModulate:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG1, D3DTA_TEXTURE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLOROP,   D3DTOP_MODULATE);
							break;
						case TextureState::ETexOpAdd:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG1, D3DTA_TEXTURE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLOROP,   D3DTOP_ADD);
							break;
						case TextureState::ETexOpAddSigned:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG1, D3DTA_TEXTURE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLOROP,   D3DTOP_ADDSIGNED);
							break;
						case TextureState::ETexOpAddSigned2x:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG1, D3DTA_TEXTURE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLOROP,   D3DTOP_ADDSIGNED2X);
							break;
						case TextureState::ETexOpModulate2x:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG1, D3DTA_TEXTURE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
							break;
						case TextureState::ETexOpBlendDiffuse:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG1, D3DTA_TEXTURE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLOROP,   D3DTOP_BLENDDIFFUSEALPHA);
							break;
						case TextureState::ETexOpBlendTexture:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG1, D3DTA_TEXTURE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLOROP,   D3DTOP_BLENDTEXTUREALPHA);
							break;
						case TextureState::ETexOpBlendColor:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG1, D3DTA_TEXTURE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLOROP,   D3DTOP_BLENDFACTORALPHA);
							break;
						case TextureState::ETexOpBlendPrevious:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG1, D3DTA_TEXTURE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA);
							break;
						case TextureState::ETexOpModulateAlphaAdd:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG1, D3DTA_CURRENT);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLOROP,   D3DTOP_MODULATEALPHA_ADDCOLOR);
							break;
						case TextureState::ETexOpPreviousModulate:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG1, D3DTA_CURRENT);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLOROP,   D3DTOP_MODULATE);
							break;
						}

						CachedTextureStates[t.Stage].Op=t.Op;
					}

					// tex arg
					if(t.Arg && t.Arg!=CachedTextureStates[t.Stage].Arg)
					{
						switch(t.Arg)
						{
						case TextureState::ETexArgPrevious:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG2, D3DTA_CURRENT);
							break;
						case TextureState::ETexArgDiffuse:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG2, D3DTA_DIFFUSE);
							break;
						case TextureState::ETexArgColor:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG2, D3DTA_TFACTOR);
							break;
						case TextureState::ETexArgTexture:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG2, D3DTA_TEXTURE);
							break;
						case TextureState::ETexArgPrevious_AR:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_COLORARG2, D3DTA_CURRENT | D3DTA_ALPHAREPLICATE);
							break;
						}

						CachedTextureStates[t.Stage].Arg=t.Arg;
					}

					// alpha op
					if(t.AlphaOp && t.AlphaOp!=CachedTextureStates[t.Stage].AlphaOp)
					{
						switch(t.AlphaOp)
						{
						case TextureState::ETexOpDisable:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
							break;
						case TextureState::ETexOpTexture:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
							break;
						case TextureState::ETexOpDiffuse:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
							break;
						case TextureState::ETexOpColor:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
							break;
						case TextureState::ETexOpModulate:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
							break;
						case TextureState::ETexOpAdd:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAOP,   D3DTOP_ADD);
							break;
						case TextureState::ETexOpAddSigned:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAOP,   D3DTOP_ADDSIGNED);
							break;
						case TextureState::ETexOpAddSigned2x:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAOP,   D3DTOP_ADDSIGNED2X);
							break;
						}

	                    CachedTextureStates[t.Stage].AlphaOp=t.AlphaOp;
                    }

					// alpha arg
					if(t.AlphaArg && t.AlphaArg!=CachedTextureStates[t.Stage].AlphaArg)
					{
						switch(t.AlphaArg)
						{
						case TextureState::ETexArgPrevious:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAARG2, D3DTA_CURRENT);
							break;
						case TextureState::ETexArgDiffuse:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
							break;
						case TextureState::ETexArgColor:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
							break;
						case TextureState::ETexArgTexture:
							d3dev->SetTextureStageState(t.Stage,D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
							break;
						}

						CachedTextureStates[t.Stage].AlphaArg=t.AlphaArg;
					}
				}

				// compare second part of tex state settings
				if (t.V2 && t.V2 != CachedTextureStates[t.Stage].V2)
				{
					// filter state
					if(t.Filter && t.Filter!=CachedTextureStates[t.Stage].Filter)
					{
						switch(t.Filter)
						{
						case TextureState::ETexFilterPoint:
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MINFILTER, D3DTEXF_POINT);							
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
							break;

						case TextureState::ETexFilterBilinear:
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
							break;

						case TextureState::ETexFilterBilinearNoMips:
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
							break;

						case TextureState::ETexFilterTrilinear:
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
							break;

						case TextureState::ETexFilterAnisotropic:
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
							d3dev->SetSamplerState(t.Stage, D3DSAMP_MAXANISOTROPY, d3dCaps.MaxAnisotropy > 8 ? 8 : d3dCaps.MaxAnisotropy);
							break;
						}

	                    CachedTextureStates[t.Stage].Filter=t.Filter;
					}

					// addr mode
					if (t.AddrMode && t.AddrMode!=CachedTextureStates[t.Stage].AddrMode)
					{
						switch (t.AddrMode)
						{
						case TextureState::ETexAddrWrap:
							d3dev->SetSamplerState(t.Stage, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
							d3dev->SetSamplerState(t.Stage, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
							d3dev->SetSamplerState(t.Stage, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
							break;

						case TextureState::ETexAddrClamp:
							d3dev->SetSamplerState(t.Stage, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
							d3dev->SetSamplerState(t.Stage, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
							d3dev->SetSamplerState(t.Stage, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
							break;

						case TextureState::ETexAddrMirror:
							d3dev->SetSamplerState(t.Stage, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
	                        d3dev->SetSamplerState(t.Stage, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
							d3dev->SetSamplerState(t.Stage, D3DSAMP_ADDRESSW, D3DTADDRESS_MIRROR);
		                    break;
						}

	                    CachedTextureStates[t.Stage].AddrMode=t.AddrMode;
					}


					if (t.Transform!=CachedTextureStates[t.Stage].Transform)
					{
						int val = D3DTTFF_DISABLE;

						if (HWClass==EHWClassGenericTnL)
							switch (t.Transform & 0xf) 
							{
								case TextureState::ETexTransformNone:
									val = D3DTTFF_DISABLE;
									break;
								case TextureState::ETexTransform1:
									val = D3DTTFF_COUNT1;
									break;
								case TextureState::ETexTransform2:
									val = D3DTTFF_COUNT2;
									break;
								case TextureState::ETexTransform3:
									val = D3DTTFF_COUNT3;
									break;
								case TextureState::ETexTransform4:
									val = D3DTTFF_COUNT4;
									break;
							}

						if (t.Transform & TextureState::ETexTransformProj)
							val |= D3DTTFF_PROJECTED;

						d3dev->SetTextureStageState(t.Stage, D3DTSS_TEXTURETRANSFORMFLAGS, val);
						CachedTextureStates[t.Stage].Transform = t.Transform;
					}

					if (t.CoordIndex && t.CoordIndex!=CachedTextureStates[t.Stage].CoordIndex)
					{
						int val = (t.CoordIndex-1) & 0x7;

						if (t.CoordIndex >= 0x10) {
							switch (t.CoordIndex & 0xe0) {
								case TextureState::ETexCoordCameraSpaceNormal: 
									val |= D3DTSS_TCI_CAMERASPACENORMAL;
									break;

								case TextureState::ETexCoordCameraSpacePos:
									val |= D3DTSS_TCI_CAMERASPACEPOSITION;
									break;

								case TextureState::ETexCoordCameraSpaceReflect:
									val |= D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
									break;
							}
						}

						d3dev->SetTextureStageState(t.Stage,D3DTSS_TEXCOORDINDEX,val);
						CachedTextureStates[t.Stage].CoordIndex = t.CoordIndex;
					}
				}
			}

			if (HWClass==EHWClassGenericTnL)
			{
				if ((t.Transform & 0xf) != TextureState::ETexTransformNone)
				{
					Matrix m = t.MatStore.GetTranspose();

					// compensation for unbeliveble behavior of D3D (http://www.flipcode.org/cgi-bin/fcmsg.cgi?thread_show=10662)
					// we _always_ use 2 component tex coords, unless we use texgen
					switch (t.Transform & 0xf) 
					{
					case TextureState::ETexTransform1:
						m.Y = m.W;	
						break;
					case TextureState::ETexTransform2:
						m.Z = m.W;	
						break;
					}

					if (t.CoordIndex >= 0x08)
					{
						Matrix WorldToCamera = GEngine->Video->GetActiveView()->LocalViewport.WorldToCamera;

						switch (t.CoordIndex&0x18) 
						{
						case TextureState::ETexCoordWorldSpace:
							m = WorldToCamera.Inverse() * m;
							break;
						case TextureState::ETexCoordWorldOrient:
							m = m * WorldToCamera.GetRotation().GetTranspose();
							break;
						}
					}

					d3dev->SetTransform(D3DTRANSFORMSTATETYPE (D3DTS_TEXTURE0+t.Stage), _DXMAT(m));
					CachedTextureStates[t.Stage].Xform = t.Xform;
				}
				else
				{
					d3dev->SetTransform(D3DTRANSFORMSTATETYPE (D3DTS_TEXTURE0+t.Stage), _DXMAT(GIdentityMatrix));
				}
			}

			if(t.BumpPtr) {
				d3dev->SetTextureStageState(t.Stage, D3DTSS_BUMPENVMAT00, F2DW(t.BumpPtr->m4x4[0][0]));
				d3dev->SetTextureStageState(t.Stage, D3DTSS_BUMPENVMAT01, F2DW(t.BumpPtr->m4x4[0][1]));
				d3dev->SetTextureStageState(t.Stage, D3DTSS_BUMPENVMAT10, F2DW(t.BumpPtr->m4x4[1][0]));
				d3dev->SetTextureStageState(t.Stage, D3DTSS_BUMPENVMAT11, F2DW(t.BumpPtr->m4x4[1][1]));
			}
        }
    }
}

void D3DRenderDevice::ResetTextureStages()
{
	for (unsigned int Stage=0; Stage<d3dCaps.MaxSimultaneousTextures; ++Stage)
	{
		CachedTextureStates[Stage].TexId = -1;
		d3dev->SetTexture(Stage, NULL);
	}
}

void D3DRenderDevice::SetFog(bool isFog)
{
	d3dev->SetRenderState(D3DRS_FOGENABLE, isFog);
}

void D3DRenderDevice::SetFogState(const FogState& state)
{
	const D3DRENDERSTATETYPE type = D3DRS_FOGVERTEXMODE;

	switch(state.Mode)
	{
	case FogState::EFogNone:
		d3dev->SetRenderState(type,D3DFOG_NONE);
		break;
	case FogState::EFogExp:
		d3dev->SetRenderState(type,D3DFOG_EXP);
		break;
	case FogState::EFogExp2:
		d3dev->SetRenderState(type,D3DFOG_EXP2);
		break;
	case FogState::EFogLinear:
		d3dev->SetRenderState(type,D3DFOG_LINEAR);
		break;
    default:
        return;
	}
	
	d3dev->SetRenderState(D3DRS_FOGSTART,	F2DW(state.Start));
	d3dev->SetRenderState(D3DRS_FOGEND,		F2DW(state.End));
	d3dev->SetRenderState(D3DRS_FOGDENSITY, F2DW(state.Density));
	d3dev->SetRenderState(D3DRS_FOGCOLOR, state.Color);

	CachedState.FogColor=state.Color;
	SavedFogColor=state.Color;
}

void D3DRenderDevice::SetLight(int number, const LightState& state)
{
    if(state.Flags==LightState::ELightOff)
    {
        d3dev->LightEnable(number,FALSE);
	    return;
    }

    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(light));

//	const float fadeOutLin = 48.0f/256.0f;
//	const float fadeOutSqr = 64.0f/256.0f;
	// funky equations to compute linear and squared att based on:
	// r     - max light range
	// att   - required att at range eg. 32/256
	// d     - range at which att is max, that is att = 1
	// 
	// Att(r) = att;
	// Att(d) = 1;
	const float r   = state.Radius;
	const float att = 16.0f/256.0f;
	const float d   = r*0.2f;

	const float A2 = (1/(att*r) - 1/d) / (r-d);
	const float A1 = 1/d - A2*d;

	switch (state.Flags) 
    {		
	case LightState::ELightPoint:
        light.Type=D3DLIGHT_POINT;
	    light.Position=D3DXVECTOR3(state.Pos.X,state.Pos.Y,state.Pos.Z);
        light.Range      = state.Radius;
		light.Attenuation0=0;
		//light.Attenuation1=1.0f/(fadeOutLin*state.Radius);
		//light.Attenuation2=1.0f/(fadeOutSqr*state.Radius*state.Radius);
		light.Attenuation1=A1;
		light.Attenuation2=A2;
        break;
			
    case LightState::ELightDirectional:
        light.Type=D3DLIGHT_DIRECTIONAL;
	    light.Direction=D3DXVECTOR3(state.Dir.X,state.Dir.Y,state.Dir.Z);
        break;

    default:
        return;
    }

	light.Specular.r  = state.Specular.X;
	light.Specular.g  = state.Specular.Y;
	light.Specular.b  = state.Specular.Z;

	light.Diffuse.r  = state.Diffuse.X;
	light.Diffuse.g  = state.Diffuse.Y;
	light.Diffuse.b  = state.Diffuse.Z;

	light.Ambient.r  = state.Ambient.X;
	light.Ambient.g  = state.Ambient.Y;
	light.Ambient.b  = state.Ambient.Z;

	d3dev->SetLight   (number, &light);
    d3dev->LightEnable(number,TRUE);
}

void D3DRenderDevice::Draw(const int primType, int base, int count, int vbase, int vcount, int basevindex)
{
    static const D3DPRIMITIVETYPE D3DPrimType[]=
    { 
        D3DPT_POINTLIST,        // EPointList       [0]
        D3DPT_TRIANGLELIST,     // EPrimTriList     [1]
        D3DPT_TRIANGLESTRIP,    // EPrimTriStrip    [2]
        D3DPT_TRIANGLEFAN,      // EPrimTriFan      [3]
        D3DPT_LINELIST,         // EPrimLineList    [4]
        D3DPT_LINESTRIP         // EPrimLineStrip   [5]
    };

	FrameTris += count;
	FrameDIPs ++;

	if(CurrentStreamIsIndexed) {
		if (vbase==EInvalidIndex)
			d3dev->DrawIndexedPrimitive(D3DPrimType[primType],basevindex,0,CurrentStreamSize,base,count);
		else
			d3dev->DrawIndexedPrimitive(D3DPrimType[primType],basevindex,vbase,vcount,base,count);
	}
    else
        d3dev->DrawPrimitive(D3DPrimType[primType],base,count);
}

// ======================================================
// internal
// ======================================================

#define PK_MIPMAPLODBIAS	(-0.4F)
//#define PK_MIPMAPLODBIAS	(-0.0F)

void D3DRenderDevice::RestoreDevice()
{
	D3DLog.Print("Restore device\n");
    Assert(d3dev);
    d3dev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
    d3dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    d3dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    d3dev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
    d3dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    d3dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	d3dev->SetTextureStageState(0, D3DTSS_BUMPENVMAT00, F2DW(0.1f));
	d3dev->SetTextureStageState(0, D3DTSS_BUMPENVMAT01, F2DW(0.0f));
	d3dev->SetTextureStageState(0, D3DTSS_BUMPENVMAT10, F2DW(0.0f));
	d3dev->SetTextureStageState(0, D3DTSS_BUMPENVMAT11, F2DW(0.1f));
    d3dev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	d3dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

    d3dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    d3dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    d3dev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    d3dev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    d3dev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    d3dev->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
    d3dev->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, F2DW(PK_MIPMAPLODBIAS));

    // defaults for texture unit 1
    // NOTE: disabled upon startup

    // defaults for alpha op: 
    // modulate alpha from texture with diffuse

    for (int i=1; i<(int)d3dCaps.MaxSimultaneousTextures; ++i)
    {
		d3dev->SetTextureStageState(i, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		d3dev->SetTextureStageState(i, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		d3dev->SetTextureStageState(i, D3DTSS_COLORARG2, D3DTA_CURRENT);
		d3dev->SetTextureStageState(i, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		d3dev->SetTextureStageState(i, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		d3dev->SetTextureStageState(i, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		d3dev->SetTextureStageState(i, D3DTSS_BUMPENVMAT00, F2DW(0.1f));
		d3dev->SetTextureStageState(i, D3DTSS_BUMPENVMAT01, F2DW(0.0f));
		d3dev->SetTextureStageState(i, D3DTSS_BUMPENVMAT10, F2DW(0.0f));
		d3dev->SetTextureStageState(i, D3DTSS_BUMPENVMAT11, F2DW(0.1f));
        d3dev->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, i);
		d3dev->SetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

        d3dev->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        d3dev->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        d3dev->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
        d3dev->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        d3dev->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
        d3dev->SetSamplerState(i, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	    d3dev->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, /*F2DW(PK_MIPMAPLODBIAS)*/0.0F);
    }

    d3dev->SetRenderState(D3DRS_TEXTUREFACTOR, (D3DCOLOR)-1);


    // no lights, no culling
    d3dev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA
												| D3DCOLORWRITEENABLE_RED
												| D3DCOLORWRITEENABLE_GREEN
												| D3DCOLORWRITEENABLE_BLUE);

    d3dev->SetRenderState(D3DRS_LIGHTING, FALSE);
    d3dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
    d3dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   


    // z-buffer
    d3dev->SetRenderState(D3DRS_ZENABLE, TRUE);
    d3dev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
//	d3dev->SetRenderState(D3DRS_ZBIAS, 0);
	d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    d3dev->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    d3dev->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
    d3dev->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

    d3dev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    d3dev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
    d3dev->SetRenderState(D3DRS_ALPHAREF, 0x0);

    d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    // default material
    D3DMATERIAL9 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
    mtrl.Specular.r = mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Specular.g = mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Specular.b = mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
    mtrl.Specular.a = mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    d3dev->SetMaterial( &mtrl );

	// default global ambient
#ifdef TRANSGAMING
	d3dev->SetRenderState(D3DRS_AMBIENT, Color_TG(0.0f,0.0f,0.0f));
#else
	d3dev->SetRenderState(D3DRS_AMBIENT, Color(0.0f,0.0f,0.0f));
#endif

    d3dev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

    d3dev->SetTransform(D3DTS_WORLD,_DXMAT(GIdentityMatrix));
    d3dev->SetTransform(D3DTS_VIEW, _DXMAT(GIdentityMatrix));
    d3dev->SetTransform(D3DTS_PROJECTION, _DXMAT(GIdentityMatrix));                        

	d3dev->SetSoftwareVertexProcessing(FALSE);	

    d3dev->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    d3dev->SetRenderState( D3DRS_LOCALVIEWER, TRUE );


	// restore releasable (not managed) textures
	for (int i=0; i<ResTextures.Size(); ++i)
	{
		D3DTexture &tex = Textures[ResTextures[i]];

		if (tex.pTex)
			continue;

        HRESULT hr;

		switch (tex.TexDef->Type) 
		{
		case Texture::ETexDepthTarget:
			hr = d3dev->CreateDepthStencilSurface
			(
				tex.TexDef->Width,
				tex.TexDef->Height,
				D3DFMT_D24S8,
				D3DMULTISAMPLE_NONE,
				0,
				TRUE, //FIXME: Zbuf discard?
				&tex.pSurf,
				NULL
			);
			if(!SUCCEEDED(hr))
			{
				D3DLog.Print("ERROR: could not restore depth target %dx%d (%s) %.2f MB free\n",
							tex.TexDef->Width,tex.TexDef->Height,
							DXGetErrorDescription9A(hr),
							d3dev->GetAvailableTextureMem()/(1024.f*1024.f));
			}
			break;

		case Texture::ETexRenderTarget:

			hr = d3dev->CreateTexture
			(
				tex.TexDef->Width,
				tex.TexDef->Height,
				1, // levels
				D3DUSAGE_RENDERTARGET,
				D3DFMT_A8R8G8B8,
				D3DPOOL_DEFAULT,
				&tex.pTex,
				NULL
			);
			if(!SUCCEEDED(hr))
			{
				D3DLog.Print("ERROR: could not restore render target[%d] %dx%d (%s) %.2f MB free\n",
							ResTextures[i],	tex.TexDef->Width,tex.TexDef->Height,
							DXGetErrorDescription9A(hr),
							d3dev->GetAvailableTextureMem()/(1024.f*1024.f));
			}
			break;

		case Texture::ETexRenderTargetCubeMap:

			hr = D3DXCreateCubeTexture
			(
				d3dev, 
				tex.TexDef->Width, 
				1, 
				D3DUSAGE_RENDERTARGET, 
				D3DFMT_X8R8G8B8, 
				D3DPOOL_DEFAULT, 
				&tex.pCube
			);
			if(!SUCCEEDED(hr))
			{
				D3DLog.Print("ERROR: could not restore %s target %dx%d (%s) %.2f MB free\n",
							((tex.TexDef->Type == Texture::ETexRenderTarget) ? "render" : "depth"),
							tex.TexDef->Width,tex.TexDef->Height,
							DXGetErrorDescription9A(hr),
							d3dev->GetAvailableTextureMem()/(1024.f*1024.f));
			}
			break;
		}
	}


	for (int i=0; i<Textures.Size(); ++i)
	{
		D3DTexture &tex = Textures[i];

		if (!tex.pTex)
			continue;
		switch (tex.TexDef->Type) 
		{
		case Texture::ETexDepthTarget:
		case Texture::ETexRenderTarget:
		case Texture::ETexRenderTargetCubeMap:
			continue;
		default:
			tex.pTex->PreLoad();
			break;
		}
	}
	
	
	// restore each mesh
    // restore all buffers
    for(int i=0; i<Buffers.Size(); ++i) 
    {
        D3DBuffer& buf=Buffers[i];
		DWORD Usage = D3DUSAGE_WRITEONLY;

		if (!buf.SourceData)
			Usage |= D3DUSAGE_DYNAMIC;
		if (buf.Type & D3DBuffer::EUsageSWVP)
			Usage |= D3DUSAGE_SOFTWAREPROCESSING;
        
        if ((buf.Type & D3DBuffer::EIndexStream) && (NULL == buf.IBuf)) 
        {
            HRESULT hr;
       	    hr=d3dev->CreateIndexBuffer
	        (
		        static_cast<UINT>(buf.ElemCount*buf.Stride),
		        Usage,
                sizeof(DWORD)==buf.Stride ? D3DFMT_INDEX32 : D3DFMT_INDEX16,
		        D3DPOOL_DEFAULT,
		        &buf.IBuf,
				NULL
	        );

			if (FAILED(hr)) 
			{ // fallback to sysmem
       			hr=d3dev->CreateIndexBuffer
				(
					static_cast<UINT>(buf.ElemCount*buf.Stride),
					Usage,
					sizeof(DWORD)==buf.Stride ? D3DFMT_INDEX32 : D3DFMT_INDEX16,
					D3DPOOL_SYSTEMMEM,
					&buf.IBuf,
					NULL
				);
			}

            // restore data (static buffers)
            if(SUCCEEDED(hr) && buf.IBuf && buf.SourceData) 
                D3DStaticBufferFill(buf.IBuf,buf.SourceData,buf.ElemCount*buf.Stride);
        }
        else
        if ((buf.Type & D3DBuffer::EVertexStream) && (NULL == buf.VBuf))
        {
            HRESULT hr;
            hr=d3dev->CreateVertexBuffer
            (
                static_cast<UINT>(buf.ElemCount*buf.Stride),
                Usage,
                buf.FVF,
                D3DPOOL_DEFAULT,
                &buf.VBuf,
				NULL
            );

			if (FAILED(hr))
			{
				hr=d3dev->CreateVertexBuffer
				(
					static_cast<UINT>(buf.ElemCount*buf.Stride),
					Usage,
					buf.FVF,
					D3DPOOL_SYSTEMMEM,
					&buf.VBuf,
					NULL
				);
			}
            // restore data (static buffers)
            if(SUCCEEDED(hr) && buf.VBuf && buf.SourceData)  
                D3DStaticBufferFill(buf.VBuf,buf.SourceData,buf.ElemCount*buf.Stride);
        }
    }

    // no shaders set
    CurrentVertexShader = (LPDIRECT3DVERTEXSHADER9) EInvalidIndex;
    CurrentPixelShader  = (LPDIRECT3DPIXELSHADER9) EInvalidIndex;
	CurrentVDecl		= (LPDIRECT3DVERTEXDECLARATION9) EInvalidIndex;
	CachedFVF			= (DWORD)EInvalidIndex;
	CachedSWVP			= FALSE;
	SavedFogColor		= 0;

    // restore shaders
    RestoreShaders();

    // setup cached state
    CachedState.DepthTest=1;
    CachedState.DepthWrite=1;
    CachedState.ColorWrite=1;
    CachedState.AlphaWrite=1;
    CachedState.StencilTest=0;
	CachedState.StencilFunc=RenderState::EFuncAlways;
	CachedState.StencilRef=0x0;
	CachedState.StencilMask=0xFF;
	CachedState.StencilFailOp=RenderState::EStencilOpKeep;
	CachedState.StencilZFailOp=RenderState::EStencilOpKeep;
	CachedState.StencilPassOp=RenderState::EStencilOpKeep;
    CachedState.AlphaTest=0;
    CachedState.Lighting=0;
    CachedState.NormalizeNormals=1;
	CachedState.AmbientColor=0;
	CachedState.SpecularEnable=0;

    CachedState.DepthFunc=RenderState::EFuncLessEqual;
    CachedState.AlphaFunc=RenderState::EFuncAlways;
    CachedState.AlphaRef=0;
    CachedState.StencilFailOp=RenderState::EStencilOpKeep;
    CachedState.StencilZFailOp=RenderState::EStencilOpKeep;
    CachedState.StencilPassOp=RenderState::EStencilOpKeep;

	CachedState.FillMode = RenderState::EFillSolid;
	CachedState.BlendMode= RenderState::EBlendNone;
	CachedState.CullMode = RenderState::ECullCCW;
    CachedState.ConstColor=(DWORD)-1;


	for (int i=0; i<96; i++)
		CachedVSConst[i] = Plane(F_MAX,F_MAX,F_MAX,F_MAX);

	for (int i=0; i<8; i++)
		CachedPSConst[i] = Plane(F_MAX,F_MAX,F_MAX,F_MAX);

    memset(&CachedTextureStates[0],0,sizeof(CachedTextureStates));
	memset(&CachedStreams[0],0,sizeof(CachedStreams));
}

void D3DRenderDevice::ReleaseDevice()
{
    if (d3dev) 
    {
		D3DLog.Print("Release device\n");

		for (int i=0; i<(int)d3dCaps.MaxSimultaneousTextures; i++)
			d3dev->SetTexture(i, NULL);

		// release buffers
        for (int i=0; i<Buffers.Size(); ++i) 
        {
            D3DBuffer& buf=Buffers[i];
			if (!buf.Ptr)
				continue;

            if (buf.Type & D3DBuffer::EIndexStream)   { SAFE_RELEASE(buf.IBuf) } else
            if (buf.Type & D3DBuffer::EVertexStream)  { SAFE_RELEASE(buf.VBuf) }
        }

		//release releasable (not managed) textures
		for (int i=0; i<ResTextures.Size(); ++i)
		{
			D3DTexture &tex = Textures[ResTextures[i]];

			if (!tex.Ptr)
				continue;
			
			if (tex.TexDef->Type==Texture::ETexRenderTarget)		{ SAFE_RELEASE(tex.pTex)  } else
			if (tex.TexDef->Type==Texture::ETexDepthTarget)			{ SAFE_RELEASE(tex.pSurf) } else
			if (tex.TexDef->Type==Texture::ETexRenderTargetCubeMap)	{ SAFE_RELEASE(tex.pCube) } 
		}

        // release shaders
        ReleaseShaders();

        // release render targets
		if (OriginalBackBuffer != CurrentBackBuffer)
	        SAFE_RELEASE(CurrentBackBuffer);
		if (OriginalDepthBuffer != CurrentDepthBuffer)
	        SAFE_RELEASE(CurrentDepthBuffer);
        
        CurrentBackBuffer=NULL;
        CurrentDepthBuffer=NULL;

        SAFE_RELEASE(OriginalBackBuffer);
        SAFE_RELEASE(OriginalDepthBuffer);
    }
}

void D3DRenderDevice::ReleaseResource(int resourceType, const int* ids, int count)
{
	Assert(ids);
	Assert(count>0);

	switch(resourceType)
	{
	case EResTexture:
		for (int i=0; i<count; ++i)
        {
            const int index = ids[i];
			for (int r=0; r<ResTextures.Size(); r++)
				if (ResTextures[r]==index)
				{
					ResTextures.FastRemove(r);
					break;
				}

		    if (index<0 || index>=Textures.Size())
				return;

            if (!Textures[index].pTex)
				return;

			int totalBytes=0;

			switch (Textures[index].TexDef->Type)
			{
			case Texture::ETexDepthTarget: 
				{
					LPDIRECT3DSURFACE9 surf = Textures[index].pSurf;

					D3DSURFACE_DESC Desc;
					if (SUCCEEDED(surf->GetDesc(&Desc))) 
					{
						int size = Desc.Width*Desc.Height*SurfFormatToBPP(Desc.Format)/8;
						totalBytes+=size;
						TexBytes-=size;
					}
					surf->Release();
					break;
				}
			case Texture::ETexRegular: 
			case Texture::ETexRenderTarget: 
				{
					LPDIRECT3DTEXTURE9 tex = Textures[index].pTex;

					for (unsigned int j=0; j<tex->GetLevelCount(); ++j) 
					{
						D3DSURFACE_DESC Desc;
						if (SUCCEEDED(tex->GetLevelDesc(j,&Desc))) 
						{
							int size = Desc.Width*Desc.Height*SurfFormatToBPP(Desc.Format)/8;
							totalBytes+=size;
							TexBytes-=size;
						}
					}
					tex->Release();
					break;
				}

			case Texture::ETexCubeMap:
			case Texture::ETexNormCubeMap:
			case Texture::ETexRenderTargetCubeMap:
				{
					LPDIRECT3DCUBETEXTURE9 tex = Textures[index].pCube;

					for (unsigned int j=0; j<tex->GetLevelCount(); ++j) 
					{
						D3DSURFACE_DESC Desc;
						if (SUCCEEDED(tex->GetLevelDesc(j,&Desc))) 
						{
							int size = Desc.Width*Desc.Height*SurfFormatToBPP(Desc.Format)/8;
							totalBytes+=size*6;
							TexBytes-=size*6;
						}
					}
					tex->Release();
					break;
				}

			case Texture::ETexVolume:
				{
					LPDIRECT3DVOLUMETEXTURE9 tex = Textures[index].pVol;

					for (unsigned int j=0; j<tex->GetLevelCount(); ++j) 
					{
						D3DVOLUME_DESC Desc;
						if (SUCCEEDED(tex->GetLevelDesc(j,&Desc))) 
						{
							int size = Desc.Width*Desc.Height*SurfFormatToBPP(Desc.Format)/8;
							totalBytes+=size;
							TexBytes-=size;
						}
					}
					tex->Release();
					break;
				}
			}


			Textures[index].pTex=0;
#ifdef ANNOYING_GRAPHICS_LOGS
			D3DLog.Print("Texture [%d] released - %.2f MB/%.2f MB free (%s)\n",
							index,(float)totalBytes/(1024.f*1024.f),
							d3dev->GetAvailableTextureMem()/(1024.f*1024.f),
							Textures[index].TexDef->FileName.AsChar());
#endif

			if (Textures[index].TexDef->Type == Texture::ETexRegular)
				TextureMemory[Textures[index].TexDef->MemoryStats] -= (float)totalBytes/(1024.f*1024.f);
        }
		break;

	case EResBuffer:
		for(int i=0; i<count; ++i)
		{
            if(ids[i] == EInvalidIndex)
                continue;

            D3DBuffer& buf=Buffers[ids[i]];
			if (!buf.Ptr)
				continue;

            if(buf.Type & D3DBuffer::EIndexStream)   
			{ 
				D3DINDEXBUFFER_DESC desc;
				buf.IBuf->GetDesc(&desc);
				TextureMemory[EMemIBs] -= (float)desc.Size/(1024.f*1024.f);
				SAFE_RELEASE(buf.IBuf) 
			} 
			else if (buf.Type & D3DBuffer::EVertexStream)  
			{ 
				D3DVERTEXBUFFER_DESC desc;
				buf.VBuf->GetDesc(&desc);
				TextureMemory[EMemVBs] -= (float)desc.Size/(1024.f*1024.f);
				SAFE_RELEASE(buf.VBuf) 
			};
            buf.Type=D3DBuffer::EUnusedStream;
            buf.SourceData=0;
		}
		break;
	};
}

RenderEffect* D3DRenderDevice::CreateEffect(DWORD Flags, const BYTE* EffectData, UINT Size)
{
	HRESULT hr;

	LPD3DXEFFECT Effect = NULL;
	LPD3DXBUFFER Errors = NULL;

	hr = D3DXCreateEffect
	(
		d3dev,
		EffectData,
		Size,
		NULL,		//const D3DXMACRO *pDefines,
		NULL,		//LPD3DXINCLUDE pInclude,
		0,			//DWORD Flags,
		NULL,		//LPD3DXEFFECTPOOL pPool,
		&Effect,
		&Errors
	);

    if (SUCCEEDED(hr)){	
        m_setTag("D3DVideo");
        return new D3DEffect(Effect);	
    }


	if (Errors)
	{
		//
		Errors->Release();
	}

	return NULL;
}

BOOL D3DRenderDevice::GrabBackBuffer(Texture* Tex, HWND hwnd)
{
	Assert(Tex);
	Assert(Tex->DeviceId>=0);
    Assert(Tex->DeviceId<=Textures.Size());
	Assert(Tex->Type==Texture::ETexRenderTarget);
		
	LPDIRECT3DTEXTURE9 pTex = Textures[Tex->DeviceId].pTex;
	Assert(pTex);

	IDirect3DSurface9* TexSurf = NULL;
	IDirect3DSurface9* BackSurf = NULL;

	HRESULT hr;
	BOOL Result = 0;

	if (SUCCEEDED(pTex->GetSurfaceLevel(0, &TexSurf)))
	{
		hr = d3dev->GetBackBuffer
		(
			0,							// UINT iSwapChain,
			0,							// UINT BackBuffer,
			D3DBACKBUFFER_TYPE_MONO,	// D3DBACKBUFFER_TYPE Type,
			&BackSurf
		);

		if (SUCCEEDED(hr))
		{
			const RECT* SourceRect = NULL;
			
			RECT WindowRect;
			if (!IsFullScreen && hwnd)
			{
				GetClientRect(hwnd, &WindowRect);
				SourceRect = &WindowRect;
			}			

			hr = d3dev->StretchRect
			(          
				BackSurf,		// IDirect3DSurface9 *pSourceSurface,
				SourceRect,		// CONST RECT *pSourceRect,
				TexSurf,		// IDirect3DSurface9 *pDestSurface,
				NULL,			// CONST RECT *pDestRect,
				D3DTEXF_NONE	// D3DTEXTUREFILTERTYPE Filter
				//D3DTEXF_LINEAR
			);

			Result = SUCCEEDED(hr);
		}
	}

	SAFE_RELEASE(TexSurf);
	SAFE_RELEASE(BackSurf);

	return Result;
}

BOOL D3DRenderDevice::GrabZStencilBuffer(Texture* Tex)
{
	Assert(Tex);
	Assert(Tex->DeviceId>=0);
    Assert(Tex->DeviceId<=Textures.Size());
	Assert(Tex->Type==Texture::ETexDepthTarget);
		
	IDirect3DSurface9* TexSurf = Textures[Tex->DeviceId].pSurf;
	Assert(TexSurf);

	IDirect3DSurface9* ZStencilSurf = NULL;

	HRESULT hr;
	BOOL Result = 0;

	hr = d3dev->GetDepthStencilSurface(&ZStencilSurf);
	
	if (SUCCEEDED(hr)) 
	{
		hr = d3dev->StretchRect
		(          
			ZStencilSurf,	// IDirect3DSurface9 *pSourceSurface,
			NULL,			// CONST RECT *pSourceRect,
			TexSurf,		// IDirect3DSurface9 *pDestSurface,
			NULL,			// CONST RECT *pDestRect,
			D3DTEXF_NONE	// D3DTEXTUREFILTERTYPE Filter
		);

		Result = SUCCEEDED(hr);
	}

	SAFE_RELEASE(TexSurf);
	SAFE_RELEASE(ZStencilSurf);

	return Result;
}

void D3DRenderDevice::GetBackBufferSize(UINT& SizeX, UINT& SizeY)
{
	SizeX = d3dpp.BackBufferWidth;
    SizeY = d3dpp.BackBufferHeight;
}

// ==========================================================================================================

static BYTE GetD3DType(BYTE Type)
{
	switch (Type)
	{
	case RenderDevice::ERegFloat1:		return D3DDECLTYPE_FLOAT1;
	case RenderDevice::ERegFloat2:		return D3DDECLTYPE_FLOAT2;
	case RenderDevice::ERegFloat3:		return D3DDECLTYPE_FLOAT3;
	case RenderDevice::ERegFloat4:		return D3DDECLTYPE_FLOAT4;
	case RenderDevice::ERegColor:		return D3DDECLTYPE_D3DCOLOR;
	case RenderDevice::ERegUByte4:		return D3DDECLTYPE_UBYTE4;
	case RenderDevice::ERegShort2:		return D3DDECLTYPE_SHORT2;
	case RenderDevice::ERegShort4:		return D3DDECLTYPE_SHORT4;
	case RenderDevice::ERegShort2N:		return D3DDECLTYPE_SHORT2N;
	case RenderDevice::ERegShort4N:		return D3DDECLTYPE_SHORT4N;
	case RenderDevice::ERegUShort2N:	return D3DDECLTYPE_USHORT2N;
	case RenderDevice::ERegUShort4N:	return D3DDECLTYPE_USHORT4N;
	case RenderDevice::ERegUByte4N:		return D3DDECLTYPE_UBYTE4N;
	case RenderDevice::ERegUDec3:		return D3DDECLTYPE_UDEC3;
	case RenderDevice::ERegDec3N:		return D3DDECLTYPE_DEC3N;
	}

	return 0;
}

int D3DRenderDevice::CreateVertexShader(const DWORD* Source, const VertexComponent* ComponentList)
{
	HRESULT hr;
	D3DVERTEXELEMENT9 Elems[MAXD3DDECLLENGTH];

	D3DVERTEXELEMENT9* E = &Elems[0];
	for (const VertexComponent* C = ComponentList; C->Type; ++C, ++E)
	{
		BYTE Type = GetD3DType(C->Type);

		E->Stream		= C->Stream;
		E->Offset		= C->Offset;
		E->Type			= Type;
		E->UsageIndex	= 0;
		E->Method		= D3DDECLMETHOD_DEFAULT;

		if (C->Usage >= RenderDevice::EUsageTexCoord0)
		{
			E->Usage = D3DDECLUSAGE_TEXCOORD;
			E->UsageIndex = (RenderDevice::EUsageTexCoord7 - C->Usage);
		}
		else if (C->Usage == RenderDevice::EUsagePosition)
		{
			E->Usage = D3DDECLUSAGE_POSITION;
		}
		else if (C->Usage == RenderDevice::EUsageNormal)
		{
			E->Usage = D3DDECLUSAGE_NORMAL;
		}
		else if (C->Usage == RenderDevice::EUsageColor)
		{
			E->Usage = D3DDECLUSAGE_COLOR;
		}
	}

	const D3DVERTEXELEMENT9 DeclEnd = D3DDECL_END();
	E[0] = 	DeclEnd;

	LPDIRECT3DVERTEXSHADER9 VSh = 0;
	LPDIRECT3DVERTEXDECLARATION9 VDecl = 0;

	hr = d3dev->CreateVertexDeclaration(Elems, &VDecl);
	if (SUCCEEDED(hr))
	{
		hr = d3dev->CreateVertexShader(Source, &VSh);
		if (SUCCEEDED(hr))
		{
			const int Index = Shaders.Size();

			D3DShader* Shader	= Shaders.Create();
			Shader->VSh			= VSh;
			Shader->VDecl		= VDecl;
			Shader->Type		= EShaderVertex;
			
			return Index;
		}
		else
			D3DLog.Print("FAILED to create vertex shader (%s)\n", DXGetErrorDescription9A(hr));
	}
	else
		D3DLog.Print("FAILED to create vertex declaration (%s)\n", DXGetErrorDescription9A(hr));

	// shit hit the fan
	SAFE_RELEASE(VSh);
	SAFE_RELEASE(VDecl);

	return EInvalidIndex;
}

// ==========================================================================================================

int D3DRenderDevice::CreateQuery(int Type)
{
	if (Type == ERenderQuery_Occlusion)
	{
		IDirect3DQuery9* pQuery = 0;
		HRESULT hr = d3dev->CreateQuery(D3DQUERYTYPE_OCCLUSION, &pQuery);
		if (SUCCEEDED(hr))		
			return Queries.Add(pQuery);		
	}

	return -1;
}

BOOL D3DRenderDevice::BeginQuery(int Query)
{
	Assert(Query>=0 && Query<Queries.Size());
	IDirect3DQuery9* pQuery = Queries[Query];
	HRESULT hr = pQuery->Issue(D3DISSUE_BEGIN);
	return SUCCEEDED(hr);
}

void D3DRenderDevice::EndQuery(int Query)
{
	Assert(Query>=0 && Query<Queries.Size());
	IDirect3DQuery9* pQuery = Queries[Query];
	pQuery->Issue(D3DISSUE_END);
}

BOOL D3DRenderDevice::GetQueryData(int Query, void* OutData, int Size)
{
	Assert(Query>=0 && Query<Queries.Size());
	IDirect3DQuery9* pQuery = Queries[Query];
	HRESULT hr = pQuery->GetData(OutData, Size, 0);
	return SUCCEEDED(hr);
}

void D3DRenderDevice::SetCullFlip(BOOL Flip)
{
	CullFlip = Flip;
	CachedState.CullMode = 0xFF;
}

void D3DRenderDevice::SetColorWrite(BOOL ColorWrite, BOOL AlphaWrite)
{
    if (CachedState.ColorWrite != ColorWrite || CachedState.AlphaWrite != AlphaWrite)
    {
        const UINT setColor=D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE;
		const UINT setAlpha=D3DCOLORWRITEENABLE_ALPHA;
        d3dev->SetRenderState(D3DRS_COLORWRITEENABLE, (ColorWrite ? setColor : 0) | (AlphaWrite ? setAlpha : 0));
        CachedState.ColorWrite = ColorWrite;
        CachedState.AlphaWrite = AlphaWrite;
    }
}


#ifdef TRANSGAMING

void D3DRenderDevice::SetSpecular(const Vector& color, const float power)
{
}
void D3DRenderDevice::SetLighting(bool isLighting)
{
}
void D3DRenderDevice::SetAmbientLight(const Vector& color)
{
}
void D3DRenderDevice::SetNoLight(int number)
{
}
void D3DRenderDevice::SetDirLight(int number, const Plane& dir, const Vector& color)
{
}

#endif

// ==========================================================================================================

static void DumpCaps(D3DCAPS9& Caps)
{
	LogBuffer &log = D3DRenderDevice::D3DLog;
	log.Print("MaxTextureSize: %dx%d\n", 
		Caps.MaxTextureWidth, Caps.MaxTextureHeight);
	log.Print("VertexShaderVersion: %d.%d\n",
		D3DSHADER_VERSION_MAJOR(Caps.VertexShaderVersion),
		D3DSHADER_VERSION_MINOR(Caps.VertexShaderVersion));
	log.Print("PixelShaderVersion: %d.%d\n",
		D3DSHADER_VERSION_MAJOR(Caps.PixelShaderVersion),
		D3DSHADER_VERSION_MINOR(Caps.PixelShaderVersion));
//	log.Print("PixelShader1xMaxValue: %d\n",
//		Caps.PixelShader1xMaxValue);		
	log.Print("MaxTextureBlendStages: %d\n",
		Caps.MaxTextureBlendStages);
	log.Print("MaxSimultaneousTextures: %d\n",
		Caps.MaxSimultaneousTextures);
}

// ============================================================================================

static inline DWORD TMapOp(const TextureStageState::ETexOp op)
{
    switch (op) {
        case TextureStageState::ETexOp_Disabled:     	return D3DTOP_DISABLE;
        case TextureStageState::ETexOp_Arg1:         	return D3DTOP_SELECTARG1;
        case TextureStageState::ETexOp_Arg2:         	return D3DTOP_SELECTARG2;
        case TextureStageState::ETexOp_Modulate:     	return D3DTOP_MODULATE;
        case TextureStageState::ETexOp_Modulate_x2:  	return D3DTOP_MODULATE2X;
        case TextureStageState::ETexOp_Modulate_x4:  	return D3DTOP_MODULATE4X;
        case TextureStageState::ETexOp_Add:          	return D3DTOP_ADD;
        case TextureStageState::ETexOp_Add_Signed:    	return D3DTOP_ADDSIGNED;
        case TextureStageState::ETexOp_Add_Signed_x2: 	return D3DTOP_ADDSIGNED2X;
        case TextureStageState::ETexOp_Substract:    	return D3DTOP_SUBTRACT;
        case TextureStageState::ETexOp_Dot3:         	return D3DTOP_DOTPRODUCT3;
    }

    return D3DTOP_DISABLE;
}

static inline DWORD TMapArg(const TextureStageState::ETexArg arg)
{
    switch (arg) {
        case TextureStageState::ETexArg_Current:    return D3DTA_CURRENT;
        case TextureStageState::ETexArg_Texel:      return D3DTA_TEXTURE;
        case TextureStageState::ETexArg_Color:      return D3DTA_TFACTOR;
        case TextureStageState::ETexArg_Diffuse:    return D3DTA_DIFFUSE;
    }

    return D3DTA_CURRENT;
}

static inline DWORD TMapFilter(const TextureStageState::ETexFilter filter)
{
    switch (filter) {
        case TextureStageState::ETexFilter_None:        return D3DTEXF_NONE;
        case TextureStageState::ETexFilter_Point:       return D3DTEXF_POINT;
        case TextureStageState::ETexFilter_Linear:      return D3DTEXF_LINEAR;
        case TextureStageState::ETexFilter_Anisotropic: return D3DTEXF_ANISOTROPIC;
    }

    return D3DTEXF_NONE;
}

static inline DWORD TMapAddr(const TextureStageState::ETexAddr addr)
{
    switch (addr) {
        case TextureStageState::ETexAddr_Wrap:          return D3DTADDRESS_WRAP;
        case TextureStageState::ETexAddr_Clamp:         return D3DTADDRESS_CLAMP;
        case TextureStageState::ETexAddr_Mirror:        return D3DTADDRESS_MIRROR;
        case TextureStageState::ETexAddr_MirrorOnce:    return D3DTADDRESS_MIRRORONCE;
        case TextureStageState::ETexAddr_Border:        return D3DTADDRESS_BORDER;
    }

    return D3DTADDRESS_WRAP;
}

static inline DWORD TMapCoord(const TextureStageState::ETexCoord coord)
{
    switch (coord) {
        case TextureStageState::ETexCoord_Index:            return D3DTSS_TCI_PASSTHRU;
        case TextureStageState::ETexCoord_CamNormal:        return D3DTSS_TCI_CAMERASPACENORMAL;
        case TextureStageState::ETexCoord_CamPos:           return D3DTSS_TCI_CAMERASPACEPOSITION;
        case TextureStageState::ETexCoord_CamReflection:    return D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
        case TextureStageState::ETexCoord_SphereMap:        return D3DTSS_TCI_PASSTHRU; // not supported under DX8.1
    }

    return D3DTSS_TCI_PASSTHRU;
}

static inline DWORD TMapTransform(const TextureStageState::ETexTransform xform)
{
    DWORD ttff = 0;

    switch (xform) {
        case TextureStageState::ETexTransform_Disable:  return D3DTTFF_DISABLE;
        case TextureStageState::ETexTransform_1:        ttff = D3DTTFF_COUNT1; break;
        case TextureStageState::ETexTransform_2:        ttff = D3DTTFF_COUNT2; break;
        case TextureStageState::ETexTransform_3:        ttff = D3DTTFF_COUNT3; break;
        case TextureStageState::ETexTransform_4:        ttff = D3DTTFF_COUNT4; break;
    }

    if (xform & TextureStageState::ETexTransform_Project)
        ttff |= D3DTTFF_PROJECTED;

    return ttff;
}

// ========================================================================================================

static const char* GetTypeString(BYTE Type)
{
	switch (Type)
	{
	case D3DDECLTYPE_FLOAT1:		return "D3DDECLTYPE_FLOAT1";
	case D3DDECLTYPE_FLOAT2:		return "D3DDECLTYPE_FLOAT2";
	case D3DDECLTYPE_FLOAT3:		return "D3DDECLTYPE_FLOAT3";
	case D3DDECLTYPE_FLOAT4:		return "D3DDECLTYPE_FLOAT4";
	case D3DDECLTYPE_D3DCOLOR:		return "D3DDECLTYPE_D3DCOLOR";
	case D3DDECLTYPE_UBYTE4:		return "D3DDECLTYPE_UBYTE4";
	case D3DDECLTYPE_SHORT2:		return "D3DDECLTYPE_SHORT2";
	case D3DDECLTYPE_SHORT4:		return "D3DDECLTYPE_SHORT4";
	case D3DDECLTYPE_SHORT2N:		return "D3DDECLTYPE_SHORT2N";
	case D3DDECLTYPE_SHORT4N:		return "D3DDECLTYPE_SHORT4N";
	case D3DDECLTYPE_USHORT2N:		return "D3DDECLTYPE_USHORT2N";
	case D3DDECLTYPE_USHORT4N:		return "D3DDECLTYPE_USHORT4N";
	case D3DDECLTYPE_UBYTE4N:		return "D3DDECLTYPE_UBYTE4N";
	case D3DDECLTYPE_UDEC3:			return "D3DDECLTYPE_UDEC3";
	case D3DDECLTYPE_DEC3N:			return "D3DDECLTYPE_DEC3N";
	}

	return "ERROR";
}

static const char* GetUsageString(BYTE Usage)
{
	switch (Usage)
	{
	case D3DDECLUSAGE_POSITION:		return "D3DDECLUSAGE_POSITION";
	case D3DDECLUSAGE_COLOR:		return "D3DDECLUSAGE_COLOR";
	case D3DDECLUSAGE_NORMAL:		return "D3DDECLUSAGE_NORMAL";
	case D3DDECLUSAGE_TEXCOORD:		return "D3DDECLUSAGE_TEXCOORD";
	}

	return "ERROR";
}

static void DumpDecl(LogBuffer& Log, const D3DVERTEXELEMENT9* Decl)
{
	for (int i=0; i<MAXD3DDECLLENGTH; ++i)
	{
		const D3DVERTEXELEMENT9 End = D3DDECL_END();
		if (!memcmp(&Decl[i],&End,sizeof(D3DVERTEXELEMENT9)))
		{
			Log.Print("END\n");
			return;
		}
		else
		{
			Log.Print("[%d]: %d %d %s %s %d\n",
				i, Decl[i].Stream, Decl[i].Offset, 
				GetTypeString(Decl[i].Type), GetUsageString(Decl[i].Usage), Decl[i].UsageIndex);
		}
	}
}
