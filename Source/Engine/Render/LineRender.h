// $Header: /Game/Source/Engine/Render/LineRender.h 37    8/19/04 9:39p Bart $
#ifdef TRANSGAMING_USE_EDITOR

// #pragma once
#ifndef _LINERENDER_
#define _LINERENDER_


class PathfinderFloor;
class Pathfinder2Floor;
class WaypointPath;
class WaypointGPath;
class WorldMesh;
class Zone;
class AntiPortal;
class WaypointSet;

#include "Render/VertexBuffer.h"
#include "Render/RenderDevice.h"
#include "Render/Vertex.h"

class ENGINE_API LineRender
{
    typedef VertexBuffer<Vector,RenderDevice::EVertexPos>  LineBuffer;
    void SetState(RenderDevice* dev);

public:
    BOOL Init();
	void RenderCleanup();

	void DrawLine(const Vector& p1, const Vector& p2, DWORD color, bool ztest=false);
    void DrawBox(const BoundingBox& box, DWORD color);
    void DrawTranslucentBox(const BoundingBox& box, DWORD color);
	void DrawFilledBox( const BoundingBox& box, DWORD color, bool ztest=true);
	void DrawPoly(const ConvexPolygon& poly, DWORD color);
	void DrawTri(const Vector tri[3], DWORD color);
    void DrawFilledPoly(const ConvexPolygon& poly, DWORD color);
    void DrawMesh(const WorldMesh* mesh, const DWORD color);
	void DrawWireframe(const DynamicArray<SimpleVertex>& verts, const Matrix& m, const DWORD color);
	void DrawTranslucentZones(const DynamicArray<Zone*>& zones, const DWORD color);
	void DrawAntiPortals(const DynamicArray<AntiPortal*>& anti, const DWORD color);
    void DrawSphere(float radius, DWORD color, Vector* center = NULL, bool ztest = true);
    void DrawSphereCone(float radius, DWORD color, float angle, Matrix* m = NULL);
	void DrawEllipse(DWORD color, const Vector& pos, const Vector& xAxis, const Vector& yAxis);
    void DrawDirLight(DWORD color, const Vector& pos, const Vector& dir);
	void DrawPlane(const Plane& p);

	void DrawWaypointSet( const WaypointSet& wps );
	void DrawWaypointSet2( void );
	void DrawFloor( const PathfinderFloor& floor, DWORD color );
	void DrawFloor( const Pathfinder2Floor& floor, DWORD color );
	void DrawWaypointPath( const WaypointPath& path );
	void DrawGlobalWaypointPath( const WaypointGPath& path );

private:
	friend class Model;
    LineBuffer  VBuf;
};



#endif //_LINERENDER_

#endif
