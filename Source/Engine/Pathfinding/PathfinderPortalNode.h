// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _PATHFINDERPORALNODE_
#define _PATHFINDERPORALNODE_


class Portal;
class World;
class Pathfinder;
class WaypointSet;

#include "Engine.h"
#include "Core/Math/LeMath.h"

class PathfinderPortalNode
{

public:
	PathfinderPortalNode(void);
	~PathfinderPortalNode(void);

	void SwitchZones( int zone1, int zone2 );

//	int LoadPortalNode( FILE* f, World& world, int portalNum );
//	int SavePortalNode( FILE* f );

	int LoadPortalNode( FileObject* f, World& world, int portalNum );
	int SavePortalNode( FileObject* f );

	void ClearContents( void );
	int PrepareContents( Pathfinder* pf, int portalNum, float portalThickness );

	int* GetWaypointsForZone( int zoneIndex )
	{
		return mWaypointIndicesInZones+mHowManyWaypointsInPortal*zoneIndex;
	};

	int NumberOfWaypoints( void )
	{
		return mHowManyWaypointsInPortal;
	};

	Portal* GetPortal( void )
	{
		return mPortal;
	};

	int IsWide( void )
	{
		return mWide;
	};

	int GetIndexOfWaypointClosestTo( const Vector& point, WaypointSet& wpset, int index );
	int TestNodeCorrectness( Pathfinder* pf );
	void DetermineWideness( Pathfinder* pf, int portalNum );

	int mPortalNumber;			// to remember
protected:
	// Variables
	int* mWaypointIndicesInZones;	// pointer to a table[][] of indices of waypoints in zones
	int mHowManyWaypointsInPortal;	// how many waypoints are contained in portal
	Portal* mPortal;				// the portal to which the node is connected

	char mWide;
};



#endif //_PATHFINDERPORALNODE_
