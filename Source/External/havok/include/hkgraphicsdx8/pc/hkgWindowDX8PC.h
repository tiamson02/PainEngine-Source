/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_WINDOW_DX8_PC_H
#define HK_GRAPHICS_WINDOW_DX8_PC_H

#include <hkgraphics/hkgWindow.h>
#include <hkgraphicsdx8/hkgWindowDX8.h>

//-----------------------------------------------------------------------------
// Error codes
//-----------------------------------------------------------------------------
typedef unsigned short APPMSGTYPE;
#define MSG_NONE 0
#define MSGERR_APPMUSTEXIT 1
#define MSGWARN_SWITCHEDTOREF 2

#define D3DAPPERR_NODIRECT3D          0x82000001
#define D3DAPPERR_NOWINDOW            0x82000002
#define D3DAPPERR_NOCOMPATIBLEDEVICES 0x82000003
#define D3DAPPERR_NOWINDOWABLEDEVICES 0x82000004
#define D3DAPPERR_NOHARDWAREDEVICE    0x82000005
#define D3DAPPERR_HALNOTCOMPATIBLE    0x82000006
#define D3DAPPERR_NOWINDOWEDHAL       0x82000007
#define D3DAPPERR_NODESKTOPHAL        0x82000008
#define D3DAPPERR_NOHALTHISMODE       0x82000009
#define D3DAPPERR_NONZEROREFCOUNT     0x8200000a
#define D3DAPPERR_MEDIANOTFOUND       0x8200000b
#define D3DAPPERR_RESIZEFAILED        0x8200000c
#define D3DAPPERR_NULLREFDEVICE       0x8200000d

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


struct D3DModeInfo
{
    DWORD      Width;      // Screen width in this mode
    DWORD      Height;     // Screen height in this mode
    D3DFORMAT  Format;     // Pixel format in this mode
    DWORD      dwBehavior; // Hardware / Software / Mixed vertex processing
    D3DFORMAT  DepthStencilFormat; // Which depth/stencil format to use with this mode
};

struct D3DDeviceInfo
{
    // Device data
    D3DDEVTYPE   DeviceType;      // Reference, HAL, etc.
    D3DCAPS8     d3dCaps;         // Capabilities of this device
    const TCHAR* strDesc;         // Name of this device
    BOOL         bCanDoWindowed;  // Whether this device can work in windowed mode

    // Modes for this device
    DWORD        dwNumModes;
    D3DModeInfo  modes[150];

    // Current state
    DWORD        dwCurrentMode;
    BOOL         bWindowed;
    D3DMULTISAMPLE_TYPE MultiSampleTypeWindowed;
    D3DMULTISAMPLE_TYPE MultiSampleTypeFullscreen;
};

struct D3DAdapterInfo
{
    // Adapter data
    D3DADAPTER_IDENTIFIER8 d3dAdapterIdentifier;
    D3DDISPLAYMODE d3ddmDesktop;      // Desktop display mode for this adapter

    // Devices for this adapter
    DWORD          dwNumDevices;
    D3DDeviceInfo  devices[5];

    // Current state
    DWORD          dwCurrentDevice;
};




class hkgDirectJoystickPC;

class hkgWindowDX8PC : public hkgWindowDX8
{
public:

	static hkgWindow* createWindowDX8PC();
	
	virtual bool initialize( HKG_WINDOW_CREATE_FLAG flags, HKG_WINDOW_BUFFERS buffers, int w, int h, const char* name, void* windowHandle = HK_NULL, void* parentHandle = HK_NULL ); // returns success
	virtual bool initializeContext();

	virtual	bool setAsCurrent(); // only need to call this if you have > 1 window
	virtual bool clearBuffers();
	virtual bool swapBuffers();

	virtual bool updateSize( unsigned int w, unsigned int h);
	virtual bool updatePos( unsigned int x, unsigned int y);

	virtual HKG_WINDOW_MSG_STATUS peekMessages(bool inViewportResize = false); // will loop on windows messages if any. Will return if idle.
	virtual void waitMessages(); // block until there is messages

	inline HINSTANCE getInstanceHandle() const;

	inline bool getActiveState() const;
	inline void setActiveState(bool a);
	
	// Internal
	bool updateJoystick();


protected:

	hkgWindowDX8PC( HINSTANCE instance, WORD menuID = 0);
	~hkgWindowDX8PC();

	HRESULT					initializeD3D();
	void					cleanupD3D();
	HRESULT					resizeD3D();
	bool					updateSizeD3D();

	HRESULT					buildDeviceList();
	

	D3DAdapterInfo			m_Adapters[10];
    DWORD					m_dwNumAdapters;
    DWORD					m_dwAdapter;

	DWORD					m_dwWindowStyle;     // Saved window style for mode switches
    RECT					m_rcWindowBounds;    // Saved window bounds for mode switches
    RECT					m_rcWindowClient;    // Saved client area size for mode switches
	
	BOOL					m_bUseDepthBuffer;   // Whether to autocreate depthbuffer
    DWORD					m_dwMinDepthBits;    // Minimum number of bits needed in depth buffer
    DWORD					m_dwMinStencilBits;  // Minimum number of bits needed in stencil buffer
 
	D3DCAPS8				m_d3dCaps;           // Caps for the device
    D3DSURFACE_DESC			m_d3dsdBackBuffer;   // Surface desc of the backbuffer
    DWORD					m_dwCreateFlags;     // Indicate sw or hw vertex processing
	TCHAR					m_strDeviceStats[90];// String to hold D3D device stats
  

	BOOL					findDepthStencilFormat(	UINT iAdapter, D3DDEVTYPE DeviceType, D3DFORMAT TargetFormat, D3DFORMAT* pDepthStencilFormat );
	INT_PTR CALLBACK		selectDeviceProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	HRESULT					userSelectNewDevice();

	HRESULT					toggleFullscreen();
	HRESULT					forceWindowed();
	HRESULT					adjustWindowForChange();

	HRESULT 				displayErrorMsg( HRESULT hr, DWORD dwType );

	HRESULT ConfirmDevice(D3DCAPS8*,DWORD,D3DFORMAT)   { return S_OK; }
    HRESULT OneTimeSceneInit()                         { return S_OK; }
    HRESULT InitDeviceObjects()                        { return S_OK; }
    HRESULT RestoreDeviceObjects()                     { return S_OK; }
    HRESULT InvalidateDeviceObjects()                  { return S_OK; }
    HRESULT DeleteDeviceObjects()                      { return S_OK; }

	HINSTANCE				m_hinstance;

	bool					m_ownHWND; // did it create the hwnd?

	hkgDirectJoystickPC*	m_directJoystick;
						
};

// WndProc prototype
LRESULT CALLBACK hkgWindowProcDX8PC(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#include <hkgraphicsdx8/pc/hkgWindowDX8PC.inl>

#endif //HK_GRAPHICS_WINDOW_PC_H

/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
