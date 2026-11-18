// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _PATHFINDER_
#define _PATHFINDER_


class Viewport;

#include "Engine.h"
#include "Core/Math/LeMath.h"
#include "WaypointSet.h"
#include "PathfinderPortalNode.h"
#include "WaypointGPath.h"

// 1. Every zone has one instance of WaypointSet class associated with it. This class
// manages all waypoints inside this zone, and helps find the way inside it, and to the
// portal waypoints.

// 2. The whole level has one instance of class Pathfinder, which helps to find the way
// between zones. 

// 3. The single node in this class is a set of waypoints contained in a single portal.
// The node contains information, which zones are connected to the portal, and
// which indices the waypoints have in each zone (WaypointSet). Of course the node
// may contain other information, which can influence heuristics of the search.

// 4. The connections between nodes are the paths between portals. The path may not exist,
// may exist in one direction, or in both. The costs of travelling it can be different
// in both directions. The path may also contain other information influencing
// heuristics.

// 5. At the beginning, Pathfinder class has to fill in the information from its fields,
// either by loading it from a prepared file, or by calculating it ( in the editor ) from
// information contained in the zones and portals.

// Structures required: WaypointSet for each zone, filled with waypoints.

// 1) Find out sets of waypoints contained in the portals for each zone. For each portal,
// if the waypoint is in one zone in the portal, but is not in some other in the same
// portal, create this waypoint in the other zone. Calculate distances in zones and so on.
// Every waypoint in the portal has to be in all zones containing the portal.

// We have filled portal nodes filled with their indices in zones and zone references

// 2) Provide additional information for the portals ( in shadows, close to water,
// anything you want ) now.

// 3) Calculate paths between portals ( costs and other values for heuristics ).
// Each path can be calculated in the zone connecting the portals.

// Now we have full information needed for the master algorithm to find the path between
// any two points.

class ENGINE_API Pathfinder
{
	friend class Pathfinder2;
public:
    // Klaudiusz:  I put it here to avoid duplication of pointer to Viewport in World class
    // maybe it should be placed in some more 'globally accessible' class, like World?
	Viewport* mViewport;

public:
	Pathfinder( void );
	~Pathfinder( void );
	void Init( World* world )
	{
		mWorld = world;
	};
	void SetViewport( Viewport* viewport )
	{
		mViewport = viewport;
	};

	int PreparePortalNodes( float portalThickness = 10.0f );
	// returns number of waypoints added to waypoint sets when
	// synchronizing waypoints in portals

#ifdef TRANSGAMING_USE_EDITOR
	void PrepareFloors( void );
#endif

	void ClearContents( void );
	void ClearWaypointSets( void );
	void ClearPortalNodes( void );
	int LoadContents( const char* filename, float scale = 1.0f, bool dontLoadPortals = false );
	int SaveContents( const char* filename, float scale = 1.0f );

	int LoadFloors( const char* filename, float scale = 1.0f );
	int SaveFloors( const char* filename, float scale = 1.0f );

	void ScaleContents( float scale );
	void FlipZ( void );

#ifdef TRANSGAMING_USE_EDITOR
	void Render( int renderWaypoints, int renderFloors );
#endif

	// Floor manipulation methods
	void FloorSelection(RECT* rect, int select );
	void SelectUnselectFloorsOfAreaLowerHigherThan( int select, int lower, float area );
	void RemoveSelectedFloors( void );
	void FloorSelectedSetClearAll( int set );
	void AddGridOnSelectedFloors( float step );

	// Waypoint manipulation methods
	int AddWaypoint( Waypoint& waypoint );	// returns index
	void MoveWaypoint( Waypoint& waypoint, Vector newPosition, int recalculateDistances = 0 );
	void MoveWaypoint( int waypointIndex, Vector newPosition, int recalculateDistances = 0 );
	void RecalculateDistances( int waypointIndex );
	void RemoveWaypoint( Waypoint& waypoint );
	void RemoveWaypoint( int index );
	int PickWaypoint( int xm, int ym, float toleranceDistance );
	int Selection( float toleranceDistance, RECT* rect, int select );
	void DisplaceSelected( const Vector& displacement );
	void CopySelected( void );
	void ScaleSelected( const Vector& center, float scale );
	void RecalculateSelected( void );
	void RemoveSelected( void );
	void MoveSelectedToZoneTheyreIn( void );
	void WaypointSelectedSetClearAll( int set );
	void ConnectDisconnectSelected( float minDistance, float maxDistance, int connect, int disconnect );
	void ConnectSelected( void );
	void DisconnectSelected( void );
#ifdef TRANSGAMING_USE_EDITOR
	void RemoveConnectionsCollidingWithGeometryInSelected( void );
#endif
	void UnselectAllWaypointsInPathfinder( void );
#ifdef TRANSGAMING_USE_EDITOR
	void SelectWaypointsNotConnectedToAnything( void );
	void SelectWaypointsNotConnectedToAnythingInCurrentRoom( void );
#endif
	void SelectWaypointsOutsideOfCurrentZone( float portalThickness );
	void SetSelectedAsForSmallMonstersOnly( void );
	void SetSelectedAsForAllMonsters( void );
	void InvertSelection( void );

	// Waypoint and floors manipulation methods
	void LevelWaypointsWithFloors( float upDisplacement );
#ifdef TRANSGAMING_USE_EDITOR
	void SelectUnselectWaypointsOnSelectedFloors( int select );
#endif

	// Structures cleaner method
#ifdef TRANSGAMING_USE_EDITOR
	void CleanStructures( void );
#endif
	int TestStructuresCorectness( void );

	int HowMany( void )
	{
		return mHowManyPortalNodes;
	};

	WaypointSet& GetWaypointsInZone( int zoneIndex )
	{
		if( zoneIndex == -1 )
			return mNoZonesWaypointSet;
		return mWaypointSets[zoneIndex];
	};

	PathfinderPortalNode& GetNodeForPortal( int portalIndex )
	{
		return mPortalNodes[portalIndex];
	};

	World* GetWorld( void ) { return mWorld; };
	WaypointSet* GetCurrentWaypointSet( void );

	// New methods for Marek
	void GetClosestWaypoint( Vector point, int& zone, int& index, float& distance );
#ifdef TRANSGAMING
	Vector GetWaypointPosition( int zone, int index );
#else
	inline Vector GetWaypointPosition( int zone, int index );
#endif
	int GetWaypointPaths( int zone, int index );
	void GetWaypointPath( int zone, int index, int pathIndex, int& targetIndex, float& distance );

	float GetShortestPath( Vector from, Vector to, AStarHeuristicFunction heuristic, WaypointGPath& path, int bigMonster = 0 );
	float LocalGetShortestPath( int from, int* to, int toHowMany, AStarHeuristicFunction heuristic, WaypointPath& path );

	void Test_InitWithWaypointGrid( int howMany );

protected:
	void DeterminePortalNodesWideness( void );
#ifdef TRANSGAMING_USE_EDITOR
	void RemoveWaypointsOutsideOfZones( float portalThickness = 10.0f );
#endif
	float GetShortestPortalPath( PathfinderPortalNode* from, PathfinderPortalNode* to, DynamicArray<PathfinderPortalNode*>& list, int startZoneIndex, int bigMonster );
	PathfinderFloorSet* GetCurrentFloorSet( void );

	// Variables
	World* mWorld;

	WaypointSet* mWaypointSets;
	int mHowManyWaypointSets;

	WaypointSet mNoZonesWaypointSet;

	PathfinderPortalNode* mPortalNodes;
	int mHowManyPortalNodes;
};



#endif //_PATHFINDER_

