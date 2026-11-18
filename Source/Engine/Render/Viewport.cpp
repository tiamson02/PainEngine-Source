#include "Engine.h"
#include "Render/Viewport.h"
#include "Render/Camera.h"
#include "Render/VertexBuffer.h"
#include "Render/Vertex.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Render/Video.h"

void Viewport::Resize(HWND hwnd)
{
    //Assert(hwnd);

    RECT r;
    GetClientRect(hwnd,&r);

    Width =r.right-r.left;
    Height=r.bottom-r.top;
/*
	if( ( ViewportWidth != Width ) || ( ViewportHeight != Height ) )
	{
		GLog.Print( "Resizing viewport from (%d,%d) to (%d,%d)\n", ViewportWidth, ViewportHeight, Width, Height );
		if( hwnd == GEngine->Video->FullScreenHWND() )
			GLog.Print( "Fullscreen HWND\n" );
	}
*/

	ViewportWidth   = Width;
	ViewportHeight  = Height;

#ifdef TRANSGAMING
    ScreenVertex::FX = Width;
    ScreenVertex::FY = Height;
#endif

    XOffset=YOffset=0;
}

void Viewport::Resize(int w, int h)
{
	ViewportWidth   = w;
	ViewportHeight  = h;
	XOffset = YOffset = 0;
}

Matrix Viewport::GetCameraToScreen() const
{
	return CameraToScreen;
}

void Viewport::SetCameraToScreen(float scaleZ, bool ProjOnFarZ)
{
	// setup projection
	float FovRadians = (Fov*0.5f) * F_PI / 180.0f;
	//CameraToScreen = ProjectionMatrix(FovRadians,float(Width),float(Height),MinZ*scaleZ,MaxZ*scaleZ);
	CameraToScreen = ProjectionMatrix(FovRadians,float(Width),float(Height),MinZ*scaleZ,MaxZ*scaleZ)
				   * ProjTweak;

	WorldToScreen = WorldToCamera * CameraToScreen;
}

/*
void Viewport::SetCameraToScreen(const Matrix& ProjMatrix)
{
	CameraToScreen = ProjMatrix;
    WorldToScreen = WorldToCamera * CameraToScreen;
}
*/

void Viewport::Update(const Camera& camera)
{
    // update screen size
#ifndef TRANSGAMING
    ScreenVertex::FX=(float)Width;
    ScreenVertex::FY=(float)Height;
#endif

    // setup matrices
	Fov			  = camera.GetFOV();
    EyePosition   = camera.GetPosition();
    WorldToCamera = camera.GetViewMatrix();

	CameraToScreen = GetCameraToScreen();

    WorldToScreen = WorldToCamera*CameraToScreen;

	ViewFrustum.PlaneCount = 6;

    // Left clipping plane
    ViewFrustum.BoundingPlanes[0].X = -WorldToScreen.A03 - WorldToScreen.A00;
    ViewFrustum.BoundingPlanes[0].Y = -WorldToScreen.A13 - WorldToScreen.A10;
    ViewFrustum.BoundingPlanes[0].Z = -WorldToScreen.A23 - WorldToScreen.A20;
    ViewFrustum.BoundingPlanes[0].W = +WorldToScreen.A33 + WorldToScreen.A30;
	ViewFrustum.BoundingPlanes[0].Normalize();

    // Right clipping plane
    ViewFrustum.BoundingPlanes[1].X = -WorldToScreen.A03 + WorldToScreen.A00;
    ViewFrustum.BoundingPlanes[1].Y = -WorldToScreen.A13 + WorldToScreen.A10;
    ViewFrustum.BoundingPlanes[1].Z = -WorldToScreen.A23 + WorldToScreen.A20;
    ViewFrustum.BoundingPlanes[1].W = +WorldToScreen.A33 - WorldToScreen.A30;
	ViewFrustum.BoundingPlanes[1].Normalize();

    // Top clipping plane
    ViewFrustum.BoundingPlanes[2].X = -WorldToScreen.A03 + WorldToScreen.A01;
    ViewFrustum.BoundingPlanes[2].Y = -WorldToScreen.A13 + WorldToScreen.A11;
    ViewFrustum.BoundingPlanes[2].Z = -WorldToScreen.A23 + WorldToScreen.A21;
    ViewFrustum.BoundingPlanes[2].W = +WorldToScreen.A33 - WorldToScreen.A31;
	ViewFrustum.BoundingPlanes[2].Normalize();

    // Bottom clipping plane
    ViewFrustum.BoundingPlanes[3].X = -WorldToScreen.A03 - WorldToScreen.A01;
    ViewFrustum.BoundingPlanes[3].Y = -WorldToScreen.A13 - WorldToScreen.A11;
    ViewFrustum.BoundingPlanes[3].Z = -WorldToScreen.A23 - WorldToScreen.A21;
    ViewFrustum.BoundingPlanes[3].W = +WorldToScreen.A33 + WorldToScreen.A31;
	ViewFrustum.BoundingPlanes[3].Normalize();

    // Near clipping plane
    ViewFrustum.BoundingPlanes[4].X = -WorldToScreen.A02;
    ViewFrustum.BoundingPlanes[4].Y = -WorldToScreen.A12;
    ViewFrustum.BoundingPlanes[4].Z = -WorldToScreen.A22;
	ViewFrustum.BoundingPlanes[4].W = +WorldToScreen.A32;
	ViewFrustum.BoundingPlanes[4].Normalize();
/*
	Vector backVector = -camera.GetForwardVector();
	backVector.Normalize();
	ViewFrustum.BoundingPlanes[4] = Plane( backVector, EyePosition + backVector );
*/
	// far clipping planes
    ViewFrustum.BoundingPlanes[5].X = -WorldToScreen.A03 + WorldToScreen.A02;
    ViewFrustum.BoundingPlanes[5].Y = -WorldToScreen.A13 + WorldToScreen.A12;
    ViewFrustum.BoundingPlanes[5].Z = -WorldToScreen.A23 + WorldToScreen.A22;
	ViewFrustum.BoundingPlanes[5].W = +WorldToScreen.A33 - WorldToScreen.A32;
	ViewFrustum.BoundingPlanes[5].Normalize();
}
