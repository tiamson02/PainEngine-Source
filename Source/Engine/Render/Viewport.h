
// #pragma once
#ifndef _VIEWPORT_
#define _VIEWPORT_


class Camera;

#include "Render/RenderDevice.h"

class ENGINE_API Viewport
{
public:
    enum
    {
        EViewPerspective    = 1,
        EViewOrthoXY        = 2,
        EViewOrthoXZ        = 3,
    };

    // viewport params for render device
    DWORD   ClearFlags;
    DWORD   ClearColor;
    float   MinZ; // near clip plane
    float   MaxZ; // far clip plane
    int     Width;	// Width of projection matrix
    int     Height;	// Height of projection matrix
	int		ViewportWidth;	// Width of viewport
	int		ViewportHeight;	// Height of Viewport
	float	zNear; // zRange min
	float	zFar;  // zRange max
    int     XOffset;
    int     YOffset;
	float	Fov;

    DWORD   ViewType;

    // scene params
    Matrix          WorldToCamera;
    Matrix          CameraToScreen;
    Matrix          WorldToScreen;
	Matrix			ProjTweak;
    Vector          EyePosition;
    ConvexVolume    ViewFrustum;

    Viewport() 
    :   ClearFlags(RenderDevice::EClearAll), ClearColor(0), MinZ(1.0f), MaxZ(32768.0f)
    ,   Width(0), Height(0), ViewportWidth(0), ViewportHeight(0), XOffset(0), YOffset(0), Fov(90), zNear(0), zFar(1)
    ,   ViewType(EViewPerspective)
    {
        WorldToScreen.SetIdentity();
        CameraToScreen.SetIdentity();
        WorldToCamera.SetIdentity();
        ProjTweak.SetIdentity();
    }

    void Resize(HWND hwnd);
	void Resize(int w, int h);
	void Update(const Camera& camera);
	Matrix GetCameraToScreen() const;
	void SetCameraToScreen(float scaleZ=1, bool ProjOnFarZ=false);
	//void SetCameraToScreen(const Matrix& ProjMatrix);
};



#endif //_VIEWPORT_

