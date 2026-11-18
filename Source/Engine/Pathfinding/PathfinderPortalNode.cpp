// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

#include "Engine.h"
#include "PathfinderPortalNode.h"
#include "Pathfinder.h"
#include "Engine/Portal.h"
#include "Engine/World.h"

PathfinderPortalNode::PathfinderPortalNode( void )
{
	mWaypointIndicesInZones = 0;
	mHowManyWaypointsInPortal = 0;
	mWide = 0;
	mPortalNumber = 0;
}

PathfinderPortalNode::~PathfinderPortalNode()
{
	ClearContents();
}

void PathfinderPortalNode::ClearContents()
{
	if( mHowManyWaypointsInPortal )
	{
		delete[] mWaypointIndicesInZones;
		mWaypointIndicesInZones = 0;
		mHowManyWaypointsInPortal = 0;
	};
}

int PathfinderPortalNode::LoadPortalNode( FileObject* f, World& world, int portalNum )
{
	ClearContents();

	mPortal = world.Portals[ portalNum ];
	mPortalNumber = portalNum;

	int howManyZones = mPortal->ZoneRefs.Size();

	// Read number of waypoints in portal
	if( !f->Read( &mHowManyWaypointsInPortal, sizeof(int) ) )
	{
		delete f;
		return 0;
	};

	// Allocate and read waypoint indices in zones table
	int tableSize = howManyZones*mHowManyWaypointsInPortal;
	if( tableSize )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mWaypointIndicesInZones = new int[ tableSize ];
		if( !f->Read( mWaypointIndicesInZones, sizeof(int)*tableSize ) )
		{
			delete f;
			return 0;
		};
	};

	// Read wideness marker
	if( !f->Read( &mWide, 1 ) )
	{
		return 0;
	};

	return 1;
}

int PathfinderPortalNode::SavePortalNode( FileObject* f )
{
	// Write number of waypoints in portal
	if( !f->Write( &mHowManyWaypointsInPortal, sizeof(int) ) )
	{
		delete f;
		return 0;
	};

	// Write waypoint indices in zones table
	int tableSize = mPortal->ZoneRefs.Size()*mHowManyWaypointsInPortal;
	if( tableSize )
	{
		if( !f->Write( mWaypointIndicesInZones, sizeof(int)*tableSize ) )
		{
			delete f;
			return 0;
		};
	};

	// Write wideness marker
	if( !f->Write( &mWide, 1 ) )
	{
		return 0; 
	};

	return 1;
}

int PathfinderPortalNode::PrepareContents( Pathfinder* pf, int portalNum, float portalThickness )
{
	int zoneIndex = 0;
	World* world = pf->GetWorld();
	mPortal = world->Portals[ portalNum ];

	BoundingBox box = mPortal->Box;

	// if horizontal portal, decimate portal thinckness by 100 ( only points almost on portal get transferred )
	if( ( world->Planes[mPortal->PlaneIndex].Normal.X == 0.0f ) &&
		( world->Planes[mPortal->PlaneIndex].Normal.Z == 0.0f ) )
		portalThickness /= 100.0f;

	box += mPortal->Poly.Verts[0]+world->Planes[mPortal->PlaneIndex].Normal*portalThickness;
	box += mPortal->Poly.Verts[0]-world->Planes[mPortal->PlaneIndex].Normal*portalThickness;

	int sum = 0;
	int numZones = mPortal->ZoneRefs.Size();

	DynamicArray<int>* waypointIndicesInPortalPerZone;
	DynamicArray<Waypoint> waypointsInPortal;

#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	waypointIndicesInPortalPerZone = new DynamicArray<int>[numZones];

	for( zoneIndex = 0; zoneIndex < numZones; zoneIndex++ )
	{
		WaypointSet& wpSet = pf->GetWaypointsInZone( mPortal->ZoneRefs[zoneIndex] );
		DynamicArray<int> waypointsInPortalForZone;

		wpSet.GetWaypointIndicesInBox( box, waypointsInPortalForZone );

		for( int i = 0; i < waypointsInPortalForZone.Size(); i++ )
		{
			Waypoint& wp = wpSet[ waypointsInPortalForZone[i] ];

			int found = 0;
			for( int j = 0; j < waypointsInPortal.Size(); j++ )
			{
				if( waypointsInPortal[j].mPosition == wp.mPosition )
				{
					found = 1;
					waypointIndicesInPortalPerZone[zoneIndex][j] = waypointsInPortalForZone[i];
					break;
				};
			};

			if( !found )
			{
				waypointsInPortal.Add( wp );
				for( int j = 0; j < numZones; j++ )
				{
					if( j == zoneIndex )
					{
						waypointIndicesInPortalPerZone[j].Add( waypointsInPortalForZone[i] );
					}
					else
					{
						waypointIndicesInPortalPerZone[j].Add( -1 );
					};
				};
			};
		};
	};

	// Now we have a table of waypoints in the portal, containing all the waypoints
	// that have been there from all zones, and a set of tables of indices, some of which
	// may be equal to -1, meaning that this waypoint is not present in the appropriate zone

	// We must now add all needed waypoints to the appropriate zones.

	for( zoneIndex = 0; zoneIndex < numZones; zoneIndex++ )
	{
		WaypointSet& wpSet = pf->GetWaypointsInZone( mPortal->ZoneRefs[zoneIndex] );

		for( int i = 0; i < waypointIndicesInPortalPerZone[zoneIndex].Size(); i++ )
		{
			if( waypointIndicesInPortalPerZone[zoneIndex][i] == -1 )
			{
				waypointIndicesInPortalPerZone[zoneIndex][i] = wpSet.AddWaypoint( waypointsInPortal[i] );
				wpSet.SelectUnique( waypointIndicesInPortalPerZone[zoneIndex][i] );
				sum++;
			};
		};
	};

	// Now all the tables are most surely full :))
	// We must copy their contents to the internal structures, and release tables

	mHowManyWaypointsInPortal = waypointsInPortal.Size();
	if( mHowManyWaypointsInPortal )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mWaypointIndicesInZones = new int[mHowManyWaypointsInPortal*numZones];

		int count = 0;
		for( zoneIndex = 0; zoneIndex < numZones; zoneIndex++ )
		{
			for( int i = 0; i < mHowManyWaypointsInPortal; i++, count++ )
			{
				mWaypointIndicesInZones[count] = waypointIndicesInPortalPerZone[zoneIndex][i];
			};
		};
	}
	else
	{
		if( mWaypointIndicesInZones )
		{
			delete[] mWaypointIndicesInZones;
			mWaypointIndicesInZones = 0;
		};
	};

	delete[] waypointIndicesInPortalPerZone;

	return sum;
}

int PathfinderPortalNode::GetIndexOfWaypointClosestTo( const Vector& point, WaypointSet& wpset, int index )
{
	float distance = FLT_MAX;
	int wpIndex = -1;
	int *wps = GetWaypointsForZone( index );

	for( int i = 0; i < mHowManyWaypointsInPortal; i++ )
	{
		float t = (wpset[wps[i]].mPosition - point).Magnitude();
		if( t < distance )
		{
			distance = t;
			wpIndex = i;
		};
	};

	return wps[wpIndex];
}

int PathfinderPortalNode::TestNodeCorrectness( Pathfinder* pf )
{
	if( !mHowManyWaypointsInPortal )
	{
		return 1;
	};

	int numZones = mPortal->ZoneRefs.Size();

	for( int i = 0; i < numZones; i++ )
	{
		WaypointSet& wpset = pf->GetWaypointsInZone( mPortal->ZoneRefs[i] );
		int* wp = GetWaypointsForZone( i );
		for( int j = 0; j < mHowManyWaypointsInPortal; j++ )
		{
			if( (wp[i] >= wpset.NumberOfWaypoints()) || (wp[i] < 0) )
			{
				return 0;	// incorrect
			};
		};
	};
	return 1;
}

void PathfinderPortalNode::DetermineWideness( Pathfinder* pf, int portalNum )
{
	World* world = pf->GetWorld();
	Portal* portal = pf->GetWorld()->Portals[ portalNum ];
	int numZones = portal->ZoneRefs.Size();
	BoundingBox box = portal->Box;

	// Determine dominant axis
	char isX = 0;
	if( ( box.Max.X - box.Min.X ) > ( box.Max.Z - box.Min.Z ) )
	{
		isX = 1;
	};

	mWide = 0;

	for( int zoneIndex = 0; zoneIndex < numZones; zoneIndex++ )
	{
		BoundingBox zoneBox = world->Zones[portal->ZoneRefs[zoneIndex]]->BBox;
		if( isX )
			if( ( box.Max.X - box.Min.X ) / ( zoneBox.Max.X - zoneBox.Min.X ) > 0.6f )
				mWide = 1;
		else
			if( ( box.Max.Z - box.Min.Z ) / ( zoneBox.Max.Z - zoneBox.Min.Z ) > 0.6f )
				mWide = 1;
	};
}

void PathfinderPortalNode::SwitchZones( int zone1, int zone2 )
{
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	int* temp = new int[mHowManyWaypointsInPortal];
	::memcpy( temp, mWaypointIndicesInZones + mHowManyWaypointsInPortal*zone1, sizeof(int)*mHowManyWaypointsInPortal );
	::memcpy( mWaypointIndicesInZones + mHowManyWaypointsInPortal*zone1, 
		mWaypointIndicesInZones + mHowManyWaypointsInPortal*zone2, sizeof(int)*mHowManyWaypointsInPortal );
	::memcpy( mWaypointIndicesInZones + mHowManyWaypointsInPortal*zone2, temp, sizeof(int)*mHowManyWaypointsInPortal );
	delete[] temp;
}
