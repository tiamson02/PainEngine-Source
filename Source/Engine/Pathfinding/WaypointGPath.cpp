// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

#include "Engine.h"
#include "WaypointGPath.h"
#include "PathfinderPortalNode.h"
#include "Pathfinder.h"
#include "Engine/Portal.h"
#include "Engine/World.h"

/*
void WaypointGPath::Render( RenderDevice* dev, LineBuffer& VBuf )
{
	if( !mInit )
	{
		return;
	};

	Assert(dev);

	if( mPrecisePath )
	{
		mPrecisePath->Render( dev, VBuf );
	};

	int size = mPortals.Size();
	if( size < 2 )
	{
		return;
	};

    RenderState state;
    state.Flags=RenderState::ESetConstColor
               |RenderState::ESetObjectTransform;
	state.ConstColor = 0xa000;
	state.ObjectTransform=GIdentityMatrix;

	dev->SetState(state);
    dev->SetColorMode();
    dev->SetStream(VBuf.GetStream());


	// render all connections
	dev->SetState(state);
	Vector* v = VBuf.Lock( size );
	for( int i = 0; i < size; i++ )
	{
		v[i] = mPortals[i]->GetPortal()->Box.GetCenter();
	};
	dev->Draw(RenderDevice::EPrimLineStrip, VBuf.Unlock(), size-1 );
}
*/

WaypointPath* WaypointGPath::CalculatePreciselyNextZone( void )
{
	Vector lastPoint;

	if( !mInit )
	{
		return 0;
	};

	if( mPortals.Size() > 1 )
	{
		PathfinderPortalNode* start = mPortals[mPortals.Size()-1];
		PathfinderPortalNode* end = mPortals[mPortals.Size()-2];

		// Zones connecting portal 1 and portal 2
		DynamicArray<int> wpSets;
		DynamicArray<int> wpIndices1;
		DynamicArray<int> wpIndices2;

		DynamicArray<int>& z1 = start->GetPortal()->ZoneRefs;
		DynamicArray<int>& z2 = end->GetPortal()->ZoneRefs;
		int s1 = start->GetPortal()->ZoneRefs.Size();
		int s2 = end->GetPortal()->ZoneRefs.Size();

		for( int i = 0; i < s1; i++ )
		{
			for( int j = 0; j < s2; j++ )
			{
				if( z1[i] == z2[j] )
				{
					wpSets.Add( z1[i] );
					wpIndices1.Add( i );
					wpIndices2.Add( j );
					break;
				};
			};
		};

		// Determine target point for heuristics function
		Vector center;
		int portalInd = 2;
		for(;;)
		{
			PathfinderPortalNode* checkPortal = mPortals[mPortals.Size()-portalInd];

			if( !checkPortal->IsWide() )
			{
				center = checkPortal->GetPortal()->Box.GetCenter();
				break;
			};

			portalInd++;
			if( portalInd > mPortals.Size() )
			{
				center = mEndPoint;
				break;
			};
		};

		int index = 0;
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		WaypointPath* path = new WaypointPath;
		do
		{
			WaypointSet& wpset = mPathfinder->GetWaypointsInZone( wpSets[index] );
			int* w2 = end->GetWaypointsForZone( wpIndices2[index] );
			GetPosLast( lastPoint );
			int startPoint = start->GetIndexOfWaypointClosestTo( lastPoint, wpset, wpIndices1[index] );

			wpset.GetShortestPath( startPoint, w2, end->NumberOfWaypoints(), mHeuristic, *path, mBigMonster, &center );
			index++;
		}
		while( (path->Size() == 0) && ( index < wpSets.Size() ) );

		mPortals.RemoveLast();

		if( path->Size() == 0 )	// failure!!! strange...
		{
			delete path;
			return 0;
		};

		if( mPrecisePath )
		{
			delete mPrecisePath;
		};
		mPrecisePath = path;
		mCurrentZone = wpIndices2[index-1];
		return mPrecisePath;
	}
	else if( mPortals.Size() == 1 )	// last path segment
	{
		PathfinderPortalNode* start = mPortals[mPortals.Size()-1];
		int zoneIndex = mPathfinder->GetWorld()->FindZone( mEndPoint );
		mCurrentZone = zoneIndex;
		WaypointSet& wpset = mPathfinder->GetWaypointsInZone( zoneIndex );

		int i;
		for( i = 0; i < start->GetPortal()->ZoneRefs.Size(); i++ )
		{
			if( zoneIndex == start->GetPortal()->ZoneRefs[i] )
			{
				break;
			};
		};

		GetPosLast( lastPoint );
		int startPoint = start->GetIndexOfWaypointClosestTo( lastPoint, wpset, i );
		int target = wpset.GetIndexOfWaypointClosestTo( mEndPoint );

#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		WaypointPath* path = new WaypointPath;
		wpset.GetShortestPath( startPoint, &target, 1, mHeuristic, *path );

		mPortals.RemoveLast();

		if( path->Size() == 0 )	// failure!!! strange...
		{
			delete path;
			return 0;
		};

		if( mPrecisePath )
		{
			delete mPrecisePath;
		};
		mPrecisePath = path;
		return mPrecisePath;
	}
	else
	{
		if( mPrecisePath )
		{
			delete mPrecisePath;
			mPrecisePath = 0;
		};
		return 0;	// end of path
	};
}

int WaypointGPath::Peek( int index, Vector& pos, Vector& dir )
{
	PeekDir( index, dir );
	return PeekPos( index, pos );
}

void WaypointGPath::Step( void )
{
	Vector t;
	PeekDir( 0, t );	// make sure something is there
	mCalculatedDirs.Remove(0);
	PeekPos( 0, t );	// make sure something is there
	Vector t2;
	if( PeekPos( 1, t2) )
	{
		mDistance -= (t2-t).Magnitude();
	};
	mCalculatedPoints.Remove(0);
	mCalculatedZoneIndices.Remove(0);
	mCalculatedWaypointIndices.Remove(0);
}

int WaypointGPath::InternalGetNext( Vector& next, int& zoneIndex, int& waypointIndex )
{
	if( !mPrecisePath )
	{
		return 0;
	};

	while( !mPrecisePath->Size() )
	{
		if( CalculatePreciselyNextZone() )
		{
			mPrecisePath->GetNextFromPath();	// remove first, it will be equal to curr
		}
		else
		{
			return 0;
		};
	};
	zoneIndex = mCurrentZone;
	waypointIndex = mPrecisePath->PeekNextIndex(mPathfinder,mCurrentZone);
	next = mPrecisePath->GetNextFromPath();
	return 1;
}

int WaypointGPath::PeekPos( int index, Vector& pos )
{
	while( mCalculatedPoints.Size() <= index )
	{
		Vector lastPos;
		GetPosLast( lastPos );
		Vector t;
		int z, w;
		do
		{
			if( !InternalGetNext( t, z, w ) )
			{
				pos = lastPos;
//				pos = mEndPoint;
				return 0;
			};
		} while( t == lastPos );
		mCalculatedPoints.Add( t );
		mCalculatedZoneIndices.Add( z );
		mCalculatedWaypointIndices.Add( w );
	};
	pos = mCalculatedPoints[index];

	return 1;
}

int WaypointGPath::PeekZoneAndWaypointIndex( int index, int& zone, int& waypoint )
{
	Vector pos;
	if( !PeekPos( index, pos ) )
	{
		return 0;
	};
	zone = mCalculatedZoneIndices[index];
	waypoint = mCalculatedWaypointIndices[index];
	return 1;
}

void WaypointGPath::PeekDir( int index, Vector& dir )
{
	while( mCalculatedDirs.Size() <= index )
	{
		Vector t1;
		if( index == 0 )
		{
			t1 = mPosMinus1;
		}
		else
		{
			PeekPos( index-1, t1 );
		};

		Vector t2;
		if( !PeekPos( index+1, t2 ) )
		{
			if( !PeekPos( index, t2 ) )
			{
				t2 = t1;
			};
		};

		Vector t = t2 - t1;
		t.Normalize();
		mCalculatedDirs.Add( t );
	};
	dir = mCalculatedDirs[ index ];
}

int WaypointGPath::DisableWaypoint( int index )
{
	int zone;
	int wpindex;
	if( !PeekZoneAndWaypointIndex( index, zone, wpindex ) )
		return -1;
	WaypointSet& ws = mPathfinder->GetWaypointsInZone( zone );
	ws.DisableWaypoint( wpindex );
	return zone;
}

void WaypointGPath::EnableAllWaypointsInZone( int zoneIndex )
{
	WaypointSet& ws = mPathfinder->GetWaypointsInZone( zoneIndex );
	ws.EnableAllWaypoints();
}

void WaypointGPath::RecalculatePath( void )
{
	Vector from;
	if( !PeekPos( 0, from ) )	// empty path
		return;

	// Check if path optimisations can be made now
	int zoneNow = mCalculatedZoneIndices[0];
	WaypointSet& ws = mPathfinder->GetWaypointsInZone( zoneNow );
	for( int i = 1; mCalculatedZoneIndices[i] == zoneNow; i++ )
	{
		if( ws.IsDisabled( mCalculatedWaypointIndices[i] ) )
		{
			mPathfinder->GetShortestPath( from, mEndPoint, GAStarDistanceHeuristic, *this, mBigMonster );
			return;
		};
	};
}

WaypointGPath& WaypointGPath::operator=(const WaypointGPath& a)
{
	if( &a == this )
		return *this;
	Empty();
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	mPrecisePath = new WaypointPath(*a.mPrecisePath);
	mCalculatedPoints = a.mCalculatedPoints;
	mCalculatedZoneIndices = a.mCalculatedZoneIndices;
	mCalculatedWaypointIndices = a.mCalculatedWaypointIndices;
	mCalculatedDirs = a.mCalculatedDirs;

	mPathfinder = a.mPathfinder;
	mHeuristic = a.mHeuristic;
	mInit = a.mInit;
	mBigMonster = a.mBigMonster;
	mPortals = a.mPortals;
	mEndPoint = a.mEndPoint;
	mDistance = a.mDistance;
	mPosMinus1 = a.mPosMinus1;
	mCurrentZone = a.mCurrentZone;
	return *this;
}
