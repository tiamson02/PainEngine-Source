// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _WAYPOINTGPATH_
#define _WAYPOINTGPATH_


#include "Engine.h"
#include "Core/Math/LeMath.h"
#include "WaypointPath.h"
#include "WaypointSet.h"

class WaypointGPath
{
	friend class Pathfinder;
#ifdef TRANSGAMING_USE_EDITOR
	friend class LineRender;
#endif

public:
	WaypointGPath( void )
	{
		mInit = 0;
		mBigMonster = 0;
		mPrecisePath = 0;
	};

	~WaypointGPath(void)
	{
		Empty();
	};

	void Init( Pathfinder* pf, AStarHeuristicFunction heuristic, char bigMonster = 0 )
	{
		mPathfinder = pf;
		mHeuristic = heuristic;
		mInit = 1;
		mBigMonster = bigMonster;
	};

//	void Render( RenderDevice* dev, LineBuffer& VBuf );
	void Empty( void )
	{
		if( mPrecisePath )
		{
			delete mPrecisePath;
			mPrecisePath = 0;
		};
		mPortals.ClearArray();
		mDistance = 0.0f;
		mCalculatedPoints.ClearArray();
		mCalculatedZoneIndices.ClearArray();
		mCalculatedWaypointIndices.ClearArray();
		mCalculatedDirs.ClearArray();
	};

	int Finished( void )
	{
		Vector t;
//		if( !PeekPos(0, t) )
//		{
//			GLog.Print("WGP: Finished\n" );
//		};
		return !PeekPos(0, t);
	};

	Vector Next( Vector& dir )
	{
		Vector temp;
		Peek( 0, temp, dir );
		Step();
//		GLog.Print("WGP: Next: %f, %f, %f\n", temp.X, temp.Y, temp.Z );
		return temp;
	};
	int PeekPos( int index, Vector& pos );
	void Step( void );
	WaypointGPath& operator=(const WaypointGPath& a);

	int DisableWaypoint( int index );	// returns zone in which waypoint was disabled
	void EnableAllWaypointsInZone( int zoneIndex );
	void RecalculatePath( void );

protected:
	WaypointPath* CalculatePreciselyNextZone( void );

	int PeekZoneAndWaypointIndex( int index, int& zone, int& waypoint );
	int InternalGetNext( Vector& next, int& zoneIndex, int& waypointIndex );
	void GetPosLast( Vector& pos )
	{
		pos = mCalculatedPoints.Size()?mCalculatedPoints[mCalculatedPoints.Size()-1]:mPosMinus1;
	};

	void PeekDir( int index, Vector& dir );
	int Peek( int index, Vector& pos, Vector& dir );	// Returns 1 if vectors are valid, 0 if are not

	// Variables set once in object lifetime
	Pathfinder* mPathfinder;
	AStarHeuristicFunction mHeuristic;
	char mInit;
	char mBigMonster;

	// Raw paths
	WaypointPath* mPrecisePath;
	DynamicArray<PathfinderPortalNode*> mPortals;

	// Additional capabilities
	Vector mEndPoint;
	float mDistance;
	Vector mPosMinus1;
	int mCurrentZone;

	DynamicArray<Vector> mCalculatedPoints;
	DynamicArray<int> mCalculatedZoneIndices;
	DynamicArray<int> mCalculatedWaypointIndices;
	DynamicArray<Vector> mCalculatedDirs;
};


#endif //_WAYPOINTGPATH_

