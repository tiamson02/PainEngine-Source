// ---------------------------------------------------------------------------
// Copyright (c) 2003 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2003/10 Krystian Galaj
//----------------------------------------------------------------------------

#include "Engine.h"
#include "Pathfinding/Pathfinder2.h"
#include "Pathfinder2Waypoints.h"

WaypointPath2::WaypointPath2( WaypointGPath2* path )
{
	SetDistances( path );
}

void WaypointPath2::SetDistances( WaypointGPath2* path )
{
	minDist = path->mMinimalDistance;
	maxDist = path->mMaximalDistance;
}

int WaypointPath2::PeekNextIndex( Pathfinder2* pf )
{
	if( !mPath.Size() )
		return 0;
	return pf->FastGetWaypointIndex( mPath[mPath.Size()-1] );
}

int WaypointPath2::PeekIndex( Pathfinder2* pf, int index )
{
	return pf->FastGetWaypointIndex( mPath[mPath.Size()-1-index] );
}

