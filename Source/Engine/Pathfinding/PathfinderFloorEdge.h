// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _PATHFINDERFLOOREDGE_
#define _PATHFINDERFLOOREDGE_


#include "Engine.h"
#include "Core/Math/LeMath.h"

// Edge is always closed. When looking down (-y), if floor vertices turn counterclockwise,
// inside is in the floor, else outside is in the floor.

class PathfinderFloorEdge
{
public:
	PathfinderFloorEdge(void);
	~PathfinderFloorEdge(void);

	int Load( FILE* f, float scale = 1.0f );
	int Save( FILE* f, float scale = 1.0f );

	void ScaleContents( float scale );
	void FlipZ( void );

	// To be used when creating edge
	void AddVertex( Vector& v )
	{
		mFloorVertices.Add( v );
	}

	int InsideFloorEdge( const Vector& p );
	void DetermineOutsideInsideEdge( void/*DynamicArray<Vector>& floorPoints*/ );

protected:
	// Variables
	DynamicArray<Vector> mFloorVertices;
	char outsideEdge;	// if 0, it's inside edge. if 1, outside edge
};



#endif //_PATHFINDERFLOOREDGE_

