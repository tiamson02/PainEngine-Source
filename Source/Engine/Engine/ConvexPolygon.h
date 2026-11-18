
// #pragma once
#ifndef _CONVEXPOLYGON_
#define _CONVEXPOLYGON_


class WorldMesh;

// ================================================================

#define HUGE_NUMBER 999999.0f

// ================================================================

#include "Render/Vertex.h"
#include "Core/DynArray.h"

class ConvexVolumeResizable;
class SimpleMesh;

class ENGINE_API ConvexMeshPolygon
{
public:
    static const int    MaxPolyVerts=32;
    MeshVertex          Verts[MaxPolyVerts];
    int                 VertexCount;
	int					PlaneIndex;

public:
    ConvexMeshPolygon() : VertexCount(0), PlaneIndex(-1) {}

	void	Reverse();
	int		Classify(const Plane& plane, const float eps=0.001f) const;
	int     Split(const Plane& plane, ConvexMeshPolygon& front, ConvexMeshPolygon& back, const float eps=0.001f) const;
//	BOOL    CreateFromPlane(const Plane& plane, float extent=HUGE_NUMBER);

//	ConvexVolume Extrude( float depth );
//	bool ConvexVolumeTest( const ConvexVolumeResizable& volume, float extrusionDepth ) const;
//	bool BoxTest( const BoundingBox& box, float extrusionDepth ) const;
	
	void operator+=(const MeshVertex& v);	

	ConvexMeshPolygon& operator=(const ConvexMeshPolygon& rhs)
    {
        PlaneIndex  = rhs.PlaneIndex;
        VertexCount = rhs.VertexCount;
        GMemCopy(&Verts[0],&rhs.Verts[0],VertexCount*sizeof(Vector));
        return (*this);
    }
};

class ENGINE_API PolygonalMesh
{
public:
	DynamicArray<ConvexMeshPolygon>		Polygons;
	BoundingBox			Box;

	PolygonalMesh( void ) {}
	PolygonalMesh(const SimpleMesh& mesh);

	int Split(const Plane& plane, PolygonalMesh& back, PolygonalMesh& front) const;

	void CreateMesh(WorldMesh& mesh) const;

	void RecomputeBox();
};

#endif //_CONVEXPOLYGON_

