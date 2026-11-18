// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _PATHFINDERFLOOR_
#define _PATHFINDERFLOOR_


#include "Engine.h"
#include "Core/Math/LeMath.h"
#include "PathfinderFloorEdge.h"

class PathfinderFloor
{
	friend class PathfinderFloorProducer;
#ifdef TRANSGAMING_USE_EDITOR
	friend class LineRender;
#endif

public:
	PathfinderFloor(void);
	~PathfinderFloor(void);

	int Load( FILE* f, float scale = 1.0f );
	int Save( FILE* f, float scale = 1.0f );

	void ScaleContents( float scale );
	void FlipZ( void );

	// To be used when creating floor
	void AddEdge( PathfinderFloorEdge* newEdge )
	{
		mFloorEdges.Add( newEdge );
	};

	// To be used when creating floor
	void AddTriangle( const Vector& p1, const Vector& p2, const Vector& p3 )
	{
		mFloorTriangles.Add( p1 );
		mFloorTriangles.Add( p2 );
		mFloorTriangles.Add( p3 );
	};

	int InsideFloor( const Vector& p );

	int HowManyTriangles( void )
	{
		return mFloorTriangles.Size()/3;
	};

	int HowManyEdges( void )
	{
		return mFloorEdges.Size();
	};

	const Vector& TrianglePoint( int i )
	{
		return mFloorTriangles[i];
	};

	// Give valid x and z to this point, expect valid y.
	// For better performance, check that the point IS on floor first!
	// It sets point on 0.f when point is not on floor.
	// Returns 0 when point is not on floor, and 1 if it is anyway, but it
	// takes much more time than for InsideFloor()
	int SetPointHeightOnFloor( Vector& p );

	void ClearContents( void );
//	void Render( RenderDevice* dev, LineBuffer& VBuf, DWORD color );

	float Area( void );
	void MakeGrid( float step, DynamicArray<Vector>& arrayToFill );

protected:
	void GetHorizontalEncompassingRectangle( float& xMin, float& yMin, float& xMax, float& yMax );

	// Variables
	DynamicArray<PathfinderFloorEdge*> mFloorEdges;
	DynamicArray<Vector> mFloorTriangles;
};


#endif //_PATHFINDERFLOOR_

