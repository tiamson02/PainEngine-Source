// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _PATHFINDERFLOORSET_
#define _PATHFINDERFLOORSET_


class Viewport;
class World;

#include "Engine.h"
#include "Core/Math/LeMath.h"
#include "PathfinderFloor.h"

class PathfinderFloorSet
{
public:
	PathfinderFloorSet(void);
	~PathfinderFloorSet(void);

	int Load( FILE* f, float scale = 1.0f );
	int Save( FILE* f, float scale = 1.0f );

	void ScaleContents( float scale );
	void FlipZ( void );

#ifdef TRANSGAMING_USE_EDITOR
	void Render( void );
#endif
	void ClearContents( void );

	int PickFloor( const Viewport& viewport, int xm, int ym );
	void Selection( const Viewport& viewport, RECT* rect, int select );
	void SelectUnselectFloorsOfAreaLowerHigherThan( int select, int lower, float area );
	void RemoveSelected( void );
	void SelectedSetClearAll( int set )
	{
		mSelectedFloors.ClearArray();

		if( set )
		{
			for( int i = 0; i < mFloors.Size(); i++ )
			{
				mSelectedFloors.Add( i );
			};
		};
	};
	void MakeGridOnSelected( float step, DynamicArray<Vector>& arrayToFill );

#ifdef TRANSGAMING_USE_EDITOR
	void GenerateFloors( World* world, int zoneIndex );
#endif

	// To start iteration, put 0 in floor. Iterate until returns 0.
	PathfinderFloor* GetNextSelectedFloor( int& floor )
	{
		if( floor < mSelectedFloors.Size() )
		{
			return mFloors[mSelectedFloors[floor++]];
		};
		return 0;
	};

protected:
	void SelectUnique( int floorIndex );
	void UnselectUnique( int floorIndex );

	// Variables
	DynamicArray<PathfinderFloor*> mFloors;
	DynamicArray<int> mSelectedFloors;
};



#endif //_PATHFINDERFLOORSET_

