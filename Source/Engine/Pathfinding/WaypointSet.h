// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _WAYPOINTSET_
#define _WAYPOINTSET_


class Viewport;
class World;
class Zone;

#include <float.h>
#include "Engine.h"
#include "Core/BitSet.h"
#include "Core/Math/LeMath.h"
#include "Waypoint.h"
#include "WaypointPath.h"
#include "PathfinderFloorSet.h"

class PathfinderPortalNode;

typedef float (*AStarHeuristicFunction)( Waypoint& from, Waypoint& to, Waypoint& current );

class ENGINE_API WaypointSet
{
#ifdef TRANSGAMING_USE_EDITOR
	friend class LineRender;
#endif
	friend class Pathfinder2;

public:
	WaypointSet(void);
	~WaypointSet(void);

	int AddWaypoint( Waypoint waypoint );	// returns index
	void MoveWaypoint( Waypoint& waypoint, Vector newPosition, int recalculateDistances = 0 )
	{
		MoveWaypoint( GetWaypointIndex( waypoint ), newPosition, recalculateDistances );
	};

	void MoveWaypoint( int waypointIndex, Vector newPosition, int recalculateDistances = 0 );
	void RecalculateDistances( int waypointIndex );
	void RemoveWaypoint( Waypoint& waypoint );
	void RemoveWaypoint( int index );
	int NumberOfWaypoints( void )
	{
		return mWaypoints.Size();
	};

	int GetWaypointIndex( Waypoint& waypoint );
	int FastGetWaypointIndex( Waypoint* waypoint ) { return (waypoint-&mWaypoints[0]); };

//	int Load( const char* filename, float scale = 1.0f );
//	int Save( Pathfinder* pf, const char* filename, float scale = 1.0f );
//	int LoadContents( FILE* f, float scale = 1.0f );
//	int SaveContents( Pathfinder* pf, FILE* f, float scale = 1.0f );

	int LoadContents( FileObject* f, DynamicArray<int>* portalTranslator = 0, float scale = 1.0f );
	int SaveContents( Pathfinder* pf, FileObject* f, float scale = 1.0f );

	int LoadFloors( FILE* f, float scale = 1.0f );
	int SaveFloors( FILE* f, float scale = 1.0f );

	void ScaleContents( float scale );
	void FlipZ( void );
	void ScaleFloors( float scale )
	{
		if( mFloors )
		{
			mFloors->ScaleContents( scale );
		};
	};

	void FlipFloorZ( void )
	{
		if( mFloors )
		{
			mFloors->FlipZ();
		};
	};

	void ClearContents( void );
	void ClearDistances( void );
	void ClearFloors( void );
	int HasFloors( void )
	{
		return ( mFloors != 0 );
	};

	void GetWaypointIndicesInBox( BoundingBox box, DynamicArray<int>& list );
	int GetIndexOfWaypointClosestTo( const Vector& point, float toleranceDistance = FLT_MAX );
	Waypoint& operator[](const int index) const
	{
		return mWaypoints[index];
	};

	// A-Star algorithm - returns 0 waypoints in arr if not found
	// Returns cost of the path, FLT_MAX if not found
	float GetShortestPath( int from, int* to, int toHowMany, AStarHeuristicFunction heuristic, WaypointPath& path, int bigMonsters = 0, Vector* targetForHeur = 0, float maxLenToTry = FLT_MAX );
//	void Render( RenderDevice* dev, LineBuffer& VBuf );

	// Portal connections methods
	void ClearConnections( void );
	void GeneratePortalConnections( DynamicArray<PathfinderPortalNode*>& portals, DynamicArray<int>& zoneIndices );
	void GetPortalNodesOrderedByDistanceFromPoint( const Vector& point, DynamicArray<PathfinderPortalNode*>& nodes, int firstToZone = -1 );
	int HowManyPortals( void )
	{ 
		return mPortalNodes.Size();
	};
	PathfinderPortalNode* GetPortalNode( int i )
	{ 
		return mPortalNodes[i];
	};

	PathfinderPortalNode* GetConnectionFromPortal( PathfinderPortalNode* node1, int number, float& distance, int bigMonster );
	void Test_InitWithWaypointGrid( int howMany = 10 );
	int PickWaypoint( Viewport& viewport, int xm, int ym, float toleranceDistance );

#ifdef TRANSGAMING_USE_EDITOR
	void GenerateFloors( World* world, int zoneIndex )
	{
		if( mFloors )
		{
			delete mFloors;
		};
#ifdef TRANSGAMING
        m_setTag("AI");
#endif
		mFloors = new PathfinderFloorSet;
		mFloors->GenerateFloors( world, zoneIndex );
	}
#endif

	// Temporary disabling/enabling waypoints
	void EnableAllWaypoints( void ) { mDisabledWaypoints.ClearAll(); };
	void DisableWaypoint( int number ) { mDisabledWaypoints.Set(number); };
	void EnableWaypoint( int number ) { mDisabledWaypoints.Clear(number); };
	int IsDisabled( int number ) { return mDisabledWaypoints.Test( number ); };

	// To work on selected waypoints
	int Selection( Viewport& viewport, float toleranceDistance, RECT* rect, int select );
	void DisplaceSelected( const Vector& displacement );
	void CopySelected( const Vector displacement );
	void ScaleSelected( const Vector& center, float scale );
	void RecalculateSelected( void );
	void RemoveSelected( void );
	void MoveSelectedToTheZoneTheyreIn( void );
	void SelectedSetClearAll( int set )
	{
		mSelected.ClearAll();

		if( set )
		{
			for( int i = 0; i < mWaypoints.Size(); i++ )
			{
				mSelected.Set( i );
			};
		};
	};
	void ConnectDisconnectSelected( float minDistance, float maxDistance, int connect, int disconnect );
	void ConnectSelected( void );
	void DisconnectSelected( void );
#ifdef TRANSGAMING_USE_EDITOR
	void RemoveConnectionsCollidingWithGeometryInSelected( World* world, Zone& zone );
#endif

	void LevelWaypointsWithFloors( float upDisplacement );
	void AddGridOnSelectedFloors( float step );
#ifdef TRANSGAMING_USE_EDITOR
	void SelectUnselectWaypointsOnSelectedFloors( int select );
	void SelectWaypointsNotConnectedToAnything( void );
#endif
	void SelectUnique( int waypointIndex );

	void SelectedForSmallMonstersOnly( void );
	void SelectedForAllMonsters( void );
	void InvertSelection( void )
	{
		for( int i = 0; i < mWaypoints.Size(); i++ )
		{
			if( mSelected.Test( i ) )
			{
				mSelected.Clear( i );
			}
			else
			{
				mSelected.Set( i );
			};
		};
	}

	PathfinderFloorSet* GetFloorSet( void )
	{
		return mFloors;
	};

	// Cleaner methods
#ifdef TRANSGAMING_USE_EDITOR
	void SelectWaypointsOutsideOfZoneOrItsPortals( World* world, int zoneNum, float portalThickness );
	void RemoveWaypointsOutsideOfZoneOrItsPortals( World* world, int zoneNum, float portalThickness )
	{
		SelectWaypointsOutsideOfZoneOrItsPortals( world, zoneNum, portalThickness );
		RemoveSelected();
	};
	void MergeWaypointsAtSamePosition( float positionTolerance = 0.1f );
	void RemoveWaypointsNotConnectedToAnything( void );
#endif


	// Internal method in load of pathfinder; to be used only there
	void InLoadTransformNodeIndicesIntoPointers( Pathfinder* pf );

	// Warning: copies nodes pointers, should be used ONLY during load when those are indices really
	void CopyTo( WaypointSet& wpset );

	// Enumeration of waypoints available from given
	Waypoint* GetNextAvailableFrom( int from, int& searchPtr, float& dist );	// till returns 0;

protected:
	int Connected( int i, int j );
	void AddConnection( int i, int j );
	void RemoveConnection( int i, int j );
#ifdef TRANSGAMING_USE_EDITOR
	int ConnectionCollidingWithGeometry( const Vector& source, const Vector& destination, World* world, Zone& zone );
#endif

	void PackDistances( void );
	void UnpackDistances( void );
	float DistanceBetween(const int index1, const int index2)
	{
		return mWaypoints[index1].DistanceFrom(mWaypoints[index2]);
	};

	void UnselectUnique( int waypointIndex );

	DynamicArray<Waypoint> mWaypoints;

	float* mDistances;	// used in both representations

	// Packed representation
	int mHowManyDistances;
	int* mPackedIndices;// as many as there are distances - destination for each distance
	int* mPackedBase;	// as many as there are waypoints - base of destinations for each waypoint
	int* mPackedCount;	// as many as there are waypoints - count of destinations for each waypoint

	BitSet mBigMonsters;

	float* mPortalConnections;
	BitSet mPortalConnectionsForBigMonsters;	// if 1 in this table, big monsters can use those
	BitSet mDisabledWaypoints;					// for temporarily disabling certain waypoints
	DynamicArray<PathfinderPortalNode*> mPortalNodes;

	PathfinderFloorSet* mFloors;

	BitSet mSelected;
};

float ENGINE_API GAStarDistanceHeuristic( Waypoint& from, Waypoint& to, Waypoint& current );



#endif //_WAYPOINTSET_

