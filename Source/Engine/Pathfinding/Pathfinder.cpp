// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

#include "Engine.h"

#include "Core/FileSystem/FileManagerStandard.h"
extern ENGINE_API FileManagerStd GFileMan;

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Render/Viewport.h"

#include "Pathfinder.h"
#include "PathfinderAStarNode.h"
#include "PointerHeap.h"
#include "Engine/Portal.h"
#include "Render/Video.h"
#include "Engine/World.h"

#if defined( TRANSGAMING_PSP )
#include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
#include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
#include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

Pathfinder::Pathfinder( void )
{
	mWaypointSets = 0;
	mPortalNodes = 0;
	mHowManyPortalNodes = 0;
	mHowManyWaypointSets = 0;
}

Pathfinder::~Pathfinder(void)
{
	ClearContents();
}

#ifdef TRANSGAMING_USE_EDITOR
int Pathfinder::PreparePortalNodes( float portalThickness )
{
	int sum = 0;

	ClearPortalNodes();
	CleanStructures();

	// Safeguard - make sure waypoint sets are initialized
	if( ( mWaypointSets == 0 ) && ( mWorld->Zones.Size() ) )
	{
		mHowManyWaypointSets = mWorld->Zones.Size();
        m_setTag("AI");
		mWaypointSets = new WaypointSet[mHowManyWaypointSets];
	};

	// Remove all waypoints outside of zones or portals
//	RemoveWaypointsOutsideOfZones( portalThickness );

	// Prepare contents of portal nodes ( this may add new nodes to WaypointSets )
	mHowManyPortalNodes = mWorld->Portals.Size();
	if( mHowManyPortalNodes )
	{
        m_setTag("AI");
		mPortalNodes = new PathfinderPortalNode[mHowManyPortalNodes];
		for( int i = 0; i < mHowManyPortalNodes; ++i )
			mPortalNodes[i].mPortalNumber = i;
	};
	for( int i = 0; i < mHowManyPortalNodes; i++ )
	{
		sum += mPortalNodes[i].PrepareContents( this, i, portalThickness );
	};

	// Calculate distances of connections between portals in WaypointSets
	for( int i = 0; i < mHowManyWaypointSets; i++ )
	{
		// Determine which portals connect to zone
		DynamicArray<PathfinderPortalNode*> portals;
		DynamicArray<int> zoneIndices;
		for( int j = 0; j < mWorld->Portals.Size(); j++ )
		{
			for( int k = 0; k < mWorld->Portals[j]->ZoneRefs.Size(); k++ )
			{
				if( mWorld->Portals[j]->ZoneRefs[k] == i )
				{
					portals.Add( &mPortalNodes[j] );
					zoneIndices.Add( k );
				};
			};
		};

		mWaypointSets[i].GeneratePortalConnections( portals, zoneIndices );
	};

	UnselectAllWaypointsInPathfinder();
	SelectWaypointsNotConnectedToAnything();
	DeterminePortalNodesWideness();

	return sum;
}

void Pathfinder::PrepareFloors( void )
{
	// Safeguard - make sure waypoint sets are initialized
	if( ( mWaypointSets == 0 ) && ( mWorld->Zones.Size() ) )
	{
		mHowManyWaypointSets = mWorld->Zones.Size();
        m_setTag("AI");
		mWaypointSets = new WaypointSet[mHowManyWaypointSets];
	};

#ifdef TRANSGAMING
	float  time = GEngine->SysDriver.GetCurrentTime();
#else
	double time = GEngine->SysDriver.GetCurrentTime();
#endif
	GLog.Print( "Prepare Floors: start\n" );

	if( mWorld->Zones.Size() )
	{
		for( int i = 0; i < mHowManyWaypointSets; i++ )
		{
			mWaypointSets[i].GenerateFloors( mWorld, i );
		};
	}
	else
	{
		mNoZonesWaypointSet.GenerateFloors( mWorld, -1 );
	};
	GLog.Print( "Prepare Floors: end\n" );
	GLog.Print( "Prepare Floors time: %f\n", GEngine->SysDriver.GetCurrentTime() - time );
}
#endif

void Pathfinder::ClearContents( void )
{
	ClearWaypointSets();
	ClearPortalNodes();
}

void Pathfinder::ClearWaypointSets( void )
{
	mNoZonesWaypointSet.ClearContents();

	for( int i = 0; i < mHowManyWaypointSets; i++ )
	{
		mWaypointSets[i].ClearDistances();
	};
	if( mWaypointSets )
	{
		delete[] mWaypointSets;
	};
	mWaypointSets = 0;
	mHowManyWaypointSets = 0;
}

void Pathfinder::ClearPortalNodes( void )
{
	for( int i = 0; i < mHowManyPortalNodes; i++ )
	{
		mPortalNodes[i].ClearContents();
	};
	if( mPortalNodes )
	{
		delete[] mPortalNodes;
	};
	mPortalNodes = 0;
	mHowManyPortalNodes = 0;
}

int Pathfinder::LoadContents( const char* filename, float scale, bool dontLoadPortals )
{
	if( !strlen( filename ) )
	{
		return 0;
	};

	FileObject* f = GFileMan.CreateFileReader( filename );
	if( !f )
	{
		return 0;
	};

	// Read number of waypoint sets
	int howMany;

	if( !f->Read( &howMany, sizeof(int) ) )
	{
		delete f;
		return 0;
	};

	// Read number of portal nodes
	int howMany2;
	if( !f->Read( &howMany2, sizeof(int) ) )
	{
		delete f;
		return 0;
	};

	// Addition; new format puts strings here
	bool newFormat = false;
	DynamicArray<int> zoneTranslator;
	DynamicArray<int> portalTranslator;

	if( ( howMany == -1 ) && ( howMany2 == -1 ) )	// new format
	{
		int realHowMany;
		if( !f->Read( &realHowMany, sizeof(int) ) )
		{
			delete f;
			return 0;
		};

		int realHowMany2;
		if( !f->Read( &realHowMany2, sizeof(int) ) )
		{
			delete f;
			return 0;
		};

		String tempName;
		for( int i = 0; i < realHowMany; ++i )
		{
			*f >> tempName;
			int translated = -1;
			for( int j = 0; j < mWorld->Zones.Size(); ++j )
			{
				if( mWorld->Zones[j]->Name == tempName )
				{
					Assert( translated == -1 );
					translated = j;
				}
			}
			if( translated != zoneTranslator.Size() )
				GLog.Print( "Pathfinder warning: Zone %d translated to %d!\n", zoneTranslator.Size(), translated );
			zoneTranslator.Add( translated );
		}

		for( int i = 0; i < realHowMany2; ++i )
		{
			*f >> tempName;
			int translated = -1;
			for( int j = 0; j < mWorld->Portals.Size(); ++j )
			{
				if( mWorld->Portals[j]->Name == tempName )
				{
					Assert( translated == -1 );
					translated = j;
				}
			}
			if( translated != portalTranslator.Size() )
				GLog.Print( "Pathfinder warning: Portal %d translated to %d!\n", portalTranslator.Size(), translated );
			portalTranslator.Add( translated );
		}

		howMany = realHowMany;
		howMany2 = realHowMany2;
		newFormat = true;
	}

	ClearContents();
	int hm;

	if( howMany )
	{
		// Allocate table for waypoint sets
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		WaypointSet* wpset = new WaypointSet[ howMany ];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mWaypointSets = new WaypointSet[ mWorld->Zones.Size() ];

		// Read them
		hm = 0;
		for( int i = 0; i < howMany; i++ )
		{
			if( newFormat )
			{
				if( !wpset[i].LoadContents( f, &portalTranslator, scale ) )
					break;
			}
			else
			{
				if( !wpset[i].LoadContents( f, 0, scale ) )
					break;
			}
			hm++;
		};

		if( howMany > hm )
		{
			mHowManyWaypointSets = 0;
			mHowManyPortalNodes = 0;
			delete[] wpset;
			delete[] mWaypointSets;
			mWaypointSets = 0;
			delete f;
			return 0;
		}

		if( newFormat )
		{
			for( int i = 0; i < howMany; ++i )
			{
				if( zoneTranslator[i] == -1 )
					continue;
				wpset[i].CopyTo( mWaypointSets[zoneTranslator[i]] );
			}
			mHowManyWaypointSets = mWorld->Zones.Size();
			delete[] wpset;
		}
		else
		{
			delete[] mWaypointSets;
			mWaypointSets = wpset;
			wpset = 0;
			mHowManyWaypointSets = hm;
		}
	};

	if( dontLoadPortals )
	{
		mHowManyPortalNodes = 0;
		delete f;

		// Remove node indices from waypoint sets
		for( int i = 0; i < mHowManyWaypointSets; i++ )
			mWaypointSets[i].ClearConnections();

		GLog.Flush();
		return 1;
	}

	if( howMany2 )
	{
		// Allocate table for portal nodes
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mPortalNodes = new PathfinderPortalNode[ howMany2 ];
		for( int i = 0; i < howMany2; ++i )
			mPortalNodes[i].mPortalNumber = i;

		// Read them
		hm = 0;
		for( int i = 0; i < howMany2; i++ )
		{
			if( newFormat )
			{
				if( portalTranslator[i] == -1 )
				{
					PathfinderPortalNode dump;
					if( !dump.LoadPortalNode( f, *mWorld, 0 ) )
					{
						mHowManyPortalNodes = hm;
						delete f;
						return 0;
					}
				}
				else if( !mPortalNodes[hm].LoadPortalNode( f, *mWorld, portalTranslator[i] ) )
				{
					mHowManyPortalNodes = hm;
					delete f;
					return 0;
				}
				else
					hm++;
				Assert( (portalTranslator[i] == -1 ) || ( portalTranslator[i] == hm-1 ) );
			}
			else if( !mPortalNodes[hm].LoadPortalNode( f, *mWorld, i ) )
			{
				mHowManyPortalNodes = hm;
				delete f;
				return 0;
			}
			else
				hm++;
		};

		mHowManyPortalNodes = hm;
	};

	delete f;

	// Transform node indices in waypoints into node pointers
	for( int i = 0; i < mHowManyWaypointSets; i++ )
	{
		mWaypointSets[i].InLoadTransformNodeIndicesIntoPointers( this );
	};

	GLog.Flush();

	return 1;
}

int Pathfinder::SaveContents( const char* filename, float scale )
{
	if( !strlen( filename ) )
	{
		return 0;
	};

	FileObject* f = GFileMan.CreateFileWriter( filename );
	if( !f )
	{
		return 0;
	};

	// Safeguard - make sure waypoint sets are initialized to be saved
	if( ( mWaypointSets == 0 ) && ( mWorld->Zones.Size() ) )
	{
		mHowManyWaypointSets = mWorld->Zones.Size();
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mWaypointSets = new WaypointSet[mHowManyWaypointSets];
	};

	// New format saving: write strings with names

	// Write -1 and -1
	int i = -1;
	if( !f->Write( &i, sizeof(int) ) ) { delete f; return 0; };
	if( !f->Write( &i, sizeof(int) ) ) { delete f; return 0; };

	// Write number of waypoint sets
	if( !f->Write( &mHowManyWaypointSets, sizeof(int) ) )
	{
		delete f;
		return 0;
	};

	// Write number of portal nodes
	if( !f->Write( &mHowManyPortalNodes, sizeof(int) ) )
	{
		delete f;
		return 0;
	};

	// Write zone strings
	for( int i = 0; i < mHowManyWaypointSets; ++i )
		*f << mWorld->Zones[i]->Name;

	// Write portal strings
	for( int i = 0; i < mHowManyPortalNodes; ++i )
		*f << mWorld->Portals[mPortalNodes[i].mPortalNumber]->Name;

	// Write waypoint sets
	for( int i = 0; i < mHowManyWaypointSets; i++ )
	{
		if( !mWaypointSets[i].SaveContents( this, f, scale ) )
		{
			delete f;
			return 0;
		};
	};

	// Write portal nodes
	for( int i = 0; i < mHowManyPortalNodes; i++ )
	{
		if( !mPortalNodes[i].SavePortalNode( f ) )
		{
			delete f;
			return 0;
		};
	};

	delete f;
	return 1;
}

int Pathfinder::LoadFloors( const char* filename, float scale )
{
	if( !strlen( filename ) )
	{
		return 0;
	};

#ifdef TRANSGAMING
	FILE* f = TNG_fopen(filename, "rb" );
#else
	FILE* f = fopen(filename, "rb" );
#endif
	if( !f )
	{
		return 0;
	};

	// Read number of floors
	int howMany;
#ifdef TRANSGAMING
	if( TNG_fread( &howMany, sizeof(int), 1, f ) != 1 )
	{
		TNG_fclose(f);
		return 0;
	};
#else
	if( fread( &howMany, sizeof(int), 1, f ) != 1 )
	{
		fclose(f);
		return 0;
	};
#endif

	// New format check
	bool newFormat = false;
	DynamicArray<int> zoneTranslator;
	if( howMany == -1 )
	{
		int realHowMany;
#ifdef TRANSGAMING
		if( TNG_fread( &realHowMany, sizeof(int), 1, f ) != 1 )
		{
			delete f;
			return 0;
		};
#else
		if( fread( &realHowMany, sizeof(int), 1, f ) != 1 )
		{
			delete f;
			return 0;
		};
#endif

		String tempName;
		for( int i = 0; i < realHowMany; ++i )
		{
			unsigned int strlen;
			char str[1024];
#ifdef TRANSGAMING
			if( TNG_fread( &strlen, sizeof(unsigned int), 1, f ) != 1 ) { delete f; return 0; };
			if( TNG_fread( &str, sizeof(char), strlen, f ) != strlen ) { delete f; return 0; };
#else
			if( fread( &strlen, sizeof(unsigned int), 1, f ) != 1 ) { delete f; return 0; };
			if( fread( &str, sizeof(char), strlen, f ) != strlen ) { delete f; return 0; };
#endif
			tempName = str;
			int translated = -1;
			for( int j = 0; j < mWorld->Zones.Size(); ++j )
			{
				if( mWorld->Zones[j]->Name == tempName )
				{
					Assert( translated == -1 );
					translated = j;
				}
			}
			zoneTranslator.Add( translated );
		}

		howMany = realHowMany;
		newFormat = true;
	}

	if( !howMany )
	{
#ifdef TRANSGAMING
		TNG_fclose(f);
#else
		fclose(f);
#endif
		return 1;
	};

	// Read them
	int lower = (howMany < mHowManyWaypointSets)?howMany:mHowManyWaypointSets;
	for( int i = 0; i < lower; i++ )
	{
		if( newFormat )
		{
			if( zoneTranslator[i] != -1 )
			{
				if( !mWaypointSets[zoneTranslator[i]].LoadFloors( f, scale ) )
				{
					for( int j = 0; i < i; j++ )
					{
						if( zoneTranslator[j] != -1 )
							mWaypointSets[zoneTranslator[j]].ClearFloors();
					};
#ifdef TRANSGAMING
					TNG_fclose(f);
#else
					fclose(f);
#endif
					return 0;
				};
			}
			else
			{
				WaypointSet dump;
				dump.LoadFloors( f, scale );
			}
		}
		else
		{
			if( !mWaypointSets[i].LoadFloors( f, scale ) )
			{
				for( int j = 0; i < i; j++ )
					mWaypointSets[j].ClearFloors();
#ifdef TRANSGAMING
				TNG_fclose(f);
#else
				fclose(f);
#endif
				return 0;
			};
		}
	};

#ifdef TRANSGAMING
	TNG_fclose(f);
#else
	fclose(f);
#endif
	return 1;
}

int Pathfinder::SaveFloors( const char* filename, float scale )
{
	if( !strlen( filename ) )
	{
		return 0;
	};

	if( !mHowManyWaypointSets )
	{
		return 1;
	};

	// Check if floors exist in first waypoint set
	if( !mWaypointSets[0].HasFloors() )
	{
		return 1;
	};

#ifdef TRANSGAMING
	FILE* f = TNG_fopen(filename, "wb" );
#else
	FILE* f = fopen(filename, "wb" );
#endif
	if( !f )
	{
		return 0;
	};

	// New format writing
	int marker = -1;
#ifdef TRANSGAMING
	if( TNG_fwrite( &marker, sizeof(int), 1, f ) != 1 ) { TNG_fclose(f); return 0; };
#else
	if( fwrite( &marker, sizeof(int), 1, f ) != 1 ) { fclose(f); return 0; };
#endif

	// Write number of waypoint sets
#ifdef TRANSGAMING
	if( TNG_fwrite( &mHowManyWaypointSets, sizeof(int), 1, f ) != 1 )
	{
		TNG_fclose(f);
#else
	if( fwrite( &mHowManyWaypointSets, sizeof(int), 1, f ) != 1 )
	{
		fclose(f);
#endif
		return 0;
	};

	// Write zone names for floors
	for( int i = 0; i < mHowManyWaypointSets; i++ )
	{
		char str[1024];
		sprintf( str, "%s", mWorld->Zones[i]->Name.AsChar() );
		unsigned int strlen = mWorld->Zones[i]->Name.Size()+1;
#ifdef TRANSGAMING
		if( TNG_fwrite( &strlen, sizeof(unsigned int), 1, f ) != 1 ) { TNG_fclose(f); return 0; };
		if( TNG_fwrite( &str, sizeof(char), strlen, f ) != strlen ) { TNG_fclose(f); return 0; };
#else
		if( fwrite( &strlen, sizeof(unsigned int), 1, f ) != 1 ) { fclose(f); return 0; };
		if( fwrite( &str, sizeof(char), strlen, f ) != strlen ) { fclose(f); return 0; };
#endif
	}

	// Write waypoint set floors
	for( int i = 0; i < mHowManyWaypointSets; i++ )
	{
		if( !mWaypointSets[i].SaveFloors( f, scale ) )
		{
#ifdef TRANSGAMING
			TNG_fclose(f);
#else
			fclose(f);
#endif
			return 0;
		};
	};
#ifdef TRANSGAMING
	TNG_fclose(f);
#else
	fclose(f);
#endif
	return 1;
}

void Pathfinder::GetClosestWaypoint( Vector point, int& zone, int& index, float& distance )
{
	zone = mWorld->FindZone( point );
	if( zone != EInvalidIndex )
	{
		index = mWaypointSets[zone].GetIndexOfWaypointClosestTo( point );
		distance = (mWaypointSets[zone][index].mPosition - point).Magnitude();
	}
}

#ifdef TRANSGAMING
Vector Pathfinder::GetWaypointPosition( int zone, int index )
#else
inline Vector Pathfinder::GetWaypointPosition( int zone, int index )
#endif
{
	return mWaypointSets[zone][index].mPosition;
}

int Pathfinder::GetWaypointPaths( int zone, int index )
{
	int searchPtr = 0;
	float dist = 0.0f;
	int numberOfWaypoints = 0;
	Waypoint* temp;
	while( ( temp = mWaypointSets[zone].GetNextAvailableFrom( index, searchPtr, dist ) ) != 0 )
		numberOfWaypoints++;

	return numberOfWaypoints;
}

void Pathfinder::GetWaypointPath( int zone, int index, int pathIndex, int& targetIndex, float& distance )
{
	int searchPtr = 0;
	float dist = 0.0f;
	int numberOfWaypoints = 0;
	Waypoint* temp;
	do
	{
		temp = mWaypointSets[zone].GetNextAvailableFrom( index, searchPtr, dist );
		numberOfWaypoints++;
	}
	while( numberOfWaypoints < pathIndex );

	Assert( temp );
	targetIndex = mWaypointSets[zone].FastGetWaypointIndex( temp );
	distance = dist;
}

float Pathfinder::GetShortestPath( Vector from, Vector to, AStarHeuristicFunction heuristic, WaypointGPath& path, int bigMonster )
{
	int profState = GProfiler.GetState();
	GProfiler.SetState( PATHFINDING );

	// Bulletproof algorithm for pathfinding. Not very fast, but a base one
	// for all optimalisations. Should works in EVERY case, too.

	path.Empty();

	path.mEndPoint = to;
	path.mPrecisePath = 0;
	path.mDistance = FLT_MAX;

	// 1. Find the zone vector 'from' is in
	int zoneFrom = mWorld->FindZone( from );
	path.mCurrentZone = zoneFrom;

	// 2. Find the zone vector 'to' is in
	int zoneTo = mWorld->FindZone( to );

	// 3. Find the waypoint closest to vector 'from'
	int closestStartWP;
	if( zoneFrom != EInvalidIndex )
	{
		if( mWaypointSets )
			closestStartWP = mWaypointSets[zoneFrom].GetIndexOfWaypointClosestTo( from );
		else
			closestStartWP = -1;
	}
	else
		closestStartWP = mNoZonesWaypointSet.GetIndexOfWaypointClosestTo( from );

	if( closestStartWP == -1 )
	{
		GProfiler.SetState( profState );
		return 0;
	};

	// 3,1. Find the waypoint closest to vector 'to'
	int closestEndWP;
	if( zoneTo != EInvalidIndex )
	{
		if( mWaypointSets )
			closestEndWP = mWaypointSets[zoneTo].GetIndexOfWaypointClosestTo( to );
		else
			closestEndWP = -1;
	}
	else
		closestEndWP = mNoZonesWaypointSet.GetIndexOfWaypointClosestTo( to );

	if( closestEndWP == -1 )
	{
		GProfiler.SetState( profState );
		return 0;
	};

	// 3,2. If waypoints in same zone, check for path between them
	if( zoneFrom == zoneTo )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		WaypointPath* onlyPath = new WaypointPath;
		float onlyDistance;
		if( zoneFrom == EInvalidIndex )
			onlyDistance = mNoZonesWaypointSet.GetShortestPath( closestStartWP, &closestEndWP, 1, heuristic, *onlyPath, bigMonster );
		else
			onlyDistance = mWaypointSets[zoneFrom].GetShortestPath( closestStartWP, &closestEndWP, 1, heuristic, *onlyPath, bigMonster );

		if( onlyPath->Size() )	// path found
		{
			path.mPrecisePath = onlyPath;
			path.mDistance = onlyDistance;
			path.mPosMinus1 = path.mPrecisePath->GetNextFromPath();
			return path.mDistance;
		}
		else
			delete onlyPath;
	};

//	3,5. Find portals from starting zone. Get them in the list from closest to target to
//		farthest to target.
	DynamicArray<PathfinderPortalNode*> startPortals;
	mWaypointSets[zoneFrom].GetPortalNodesOrderedByDistanceFromPoint( to, startPortals );
	if( !startPortals.Size() ) { GProfiler.SetState( profState ); return FLT_MAX; }

	// 3,75. Find portals from ending zone. Get them in the list from closest to 'from' to
	//       farthest to 'from'.
	DynamicArray<PathfinderPortalNode*> endPortals;
	mWaypointSets[zoneTo].GetPortalNodesOrderedByDistanceFromPoint( from, endPortals );
	if( !endPortals.Size() ) { GProfiler.SetState( profState ); return FLT_MAX; }

	// 3,8. Prepare variables
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
	float* startDistances = new float[startPortals.Size()];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
	WaypointPath* startPaths = new WaypointPath[startPortals.Size()];
	bool startPathExists = false;
	int startPathExistsTo = -1;
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
	float* endDistances = new float[endPortals.Size()];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
	WaypointPath* endPaths = new WaypointPath[endPortals.Size()];
	bool endPathExists = false;
	int endPathExistsTo = -1;

	//----------------------------------------------------------------------
	// Special case - path between two closest portals length test
	//----------------------------------------------------------------------

	float farthestLengthToSearch = FLT_MAX;

	// 3,81. Find first start path
	{
		PathfinderPortalNode* startPortal = startPortals[0];

		int zoneInd = 0;
		for( int j = 0; j < startPortal->GetPortal()->ZoneRefs.Size(); j++ )
		{
			if( startPortal->GetPortal()->ZoneRefs[j] == zoneFrom )
			{
				zoneInd = j;
				break;
			};
		};

		Vector center = startPortal->IsWide()?to:startPortal->GetPortal()->Box.GetCenter();

		startDistances[0] = mWaypointSets[zoneFrom].GetShortestPath( closestStartWP, startPortal->GetWaypointsForZone(zoneInd),
			startPortal->NumberOfWaypoints(), heuristic, startPaths[0], bigMonster, &center );

		if( startDistances[0] == FLT_MAX )
			startPortals.Remove(0);
		else
		{
			startPathExists = true;

			int size = mWaypointSets[zoneFrom].HowManyPortals();
			for( int j = 0; j < size; ++j )
				if( startPortal == mWaypointSets[zoneFrom].GetPortalNode(j) )
				{
					startPathExistsTo = j;
					break;
				};
			Assert( startPathExistsTo != -1 );
		}
	}

	if( ( startDistances[0] != FLT_MAX ) && ( startPortals[0] == endPortals[0] ) )
	{
		// 3,82. Find first end path
		PathfinderPortalNode* endPortal = endPortals[0];

		int zoneInd = 0;
		for( int j = 0; j < endPortal->GetPortal()->ZoneRefs.Size(); j++ )
		{
			if( endPortal->GetPortal()->ZoneRefs[j] == zoneTo )
			{
				zoneInd = j;
				break;
			}
		}

		// Fix for portals
		if( *endPortal->GetWaypointsForZone(zoneInd) >= mWaypointSets[zoneTo].NumberOfWaypoints() )
		{
			GLog.Print("VERY IMPORTANT ERROR: This portal's zones are switched!!!\n");
			GLog.Flush();
			// This portal's zones are switched!!!
			if( endPortal->GetPortal()->ZoneRefs.Size() == 2 )
			{
				GLog.Print("Trying to resolve the problem...\n");
				GLog.Flush();
				endPortal->SwitchZones( 0, 1 );
			}
			else
			{
				GLog.Close();
				exit(1);
			}
		}

//		Warning:: it's a fix for opera - but it assumes that all paths are two-way!!!!!
		Vector center = endPortal->IsWide()?from:endPortal->GetPortal()->Box.GetCenter();
		endDistances[0] = mWaypointSets[zoneTo].GetShortestPath( closestEndWP, endPortal->GetWaypointsForZone(zoneInd),
			endPortal->NumberOfWaypoints(), heuristic, endPaths[0], bigMonster, &center );

		if( endDistances[0] == FLT_MAX )
			endPortals.Remove(0);
		else
		{
			endPathExists = true;

			int size = mWaypointSets[zoneTo].HowManyPortals();
			for( int j = 0; j < size; ++j )
				if( endPortal == mWaypointSets[zoneTo].GetPortalNode(j) )
				{
					endPathExistsTo = j;
					break;
				};
			Assert( endPathExistsTo != -1 );

			farthestLengthToSearch = startDistances[0] + endDistances[0];
		}
	}

	// 4. Find rest of start paths
	int i = 0;
	if( startPathExists ) i = 1;

	for( ; i < startPortals.Size(); i++ )
	{
		PathfinderPortalNode* startPortal = startPortals[i];

		if( startPathExists )
		{
			mWaypointSets[zoneFrom].GetConnectionFromPortal( startPortal, startPathExistsTo, startDistances[i], bigMonster );
			if( ( startDistances[i] == FLT_MAX ) || ( startDistances[i] > farthestLengthToSearch ) )
			{
				startPortals.Remove(i);
				for( int k = i; k < startPortals.Size(); ++k ) startDistances[k] = startDistances[k+1];
				i--;
				continue;
			}
		}

		int zoneInd = 0;
		for( int j = 0; j < startPortal->GetPortal()->ZoneRefs.Size(); j++ )
		{
			if( startPortal->GetPortal()->ZoneRefs[j] == zoneFrom )
			{
				zoneInd = j;
				break;
			};
		};

		Vector center = startPortal->IsWide()?to:startPortal->GetPortal()->Box.GetCenter();

		startDistances[i] = mWaypointSets[zoneFrom].GetShortestPath( closestStartWP, startPortal->GetWaypointsForZone(zoneInd),
			startPortal->NumberOfWaypoints(), heuristic, startPaths[i], bigMonster, &center, farthestLengthToSearch );

		if( startDistances[i] == FLT_MAX )
		{
			startPortals.Remove(i);
			for( int k = i; k < startPortals.Size(); ++k ) startDistances[k] = startDistances[k+1];
			i--;
			continue;
		}

		if( !startPathExists && ( startDistances[i] != FLT_MAX ) )
		{
			startPathExists = true;

			int size = mWaypointSets[zoneFrom].HowManyPortals();
			for( int j = 0; j < size; ++j )
				if( startPortal == mWaypointSets[zoneFrom].GetPortalNode(j) )
				{
					startPathExistsTo = j;
					break;
				};
			Assert( startPathExistsTo != -1 );
		}
	};

	// 5. Find all end paths
	i = 0;
	if( endPathExists ) i = 1;

	for( ; i < endPortals.Size(); i++ )
	{
		PathfinderPortalNode* endPortal = endPortals[i];

		if( endPathExists )
		{
			mWaypointSets[zoneTo].GetConnectionFromPortal( endPortal, endPathExistsTo, endDistances[i], bigMonster );
			if( ( endDistances[i] == FLT_MAX ) || ( endDistances[i] > farthestLengthToSearch ) )
			{
				endPortals.Remove(i);
				for( int k = i; k < endPortals.Size(); ++k ) endDistances[k] = endDistances[k+1];
				i--;
				continue;
			}
		}

		int zoneInd = 0;
		for( int j = 0; j < endPortal->GetPortal()->ZoneRefs.Size(); j++ )
		{
			if( endPortal->GetPortal()->ZoneRefs[j] == zoneTo )
			{
				zoneInd = j;
				break;
			};
		};

		// Fix for portals
		if( *endPortal->GetWaypointsForZone(zoneInd) >= mWaypointSets[zoneTo].NumberOfWaypoints() )
		{
			GLog.Print("VERY IMPORTANT ERROR: This portal's zones are switched!!!\n");
			GLog.Flush();
			// This portal's zones are switched!!!
			if( endPortal->GetPortal()->ZoneRefs.Size() == 2 )
			{
				GLog.Print("Trying to resolve the problem...\n");
				GLog.Flush();
				endPortal->SwitchZones( 0, 1 );
			}
			else
			{
				GLog.Close();
				exit(1);
			}
		}

//		Warning:: it's a fix for opera - but it assumes that all paths are two-way!!!!!
		Vector center = endPortal->IsWide()?from:endPortal->GetPortal()->Box.GetCenter();
		endDistances[i] = mWaypointSets[zoneTo].GetShortestPath( closestEndWP, endPortal->GetWaypointsForZone(zoneInd),
			endPortal->NumberOfWaypoints(), heuristic, endPaths[i], bigMonster, &center, farthestLengthToSearch );

//		endDistances[i] = mWaypointSets[zoneTo].GetShortestPath( 
//			*endPortal->GetWaypointsForZone(zoneInd), &closestEndWP, 1, heuristic, endPaths[i], bigMonster );

		if( endDistances[i] == FLT_MAX )
		{
			endPortals.Remove(i);
			for( int k = i; k < endPortals.Size(); ++k ) endDistances[k] = endDistances[k+1];
			i--;
			continue;
		}

		if( !endPathExists && ( endDistances[i] != FLT_MAX ) )
		{
			endPathExists = true;

			int size = mWaypointSets[zoneTo].HowManyPortals();
			for( int j = 0; j < size; ++j )
				if( endPortal == mWaypointSets[zoneTo].GetPortalNode(j) )
				{
					endPathExistsTo = j;
					break;
				};
			Assert( endPathExistsTo != -1 );
		}
	};

	// 6. Find distances and paths between pairs of portals (should be quite quick)
	int tableSize =  startPortals.Size()*endPortals.Size();
	if( tableSize == 0 )
	{
		GProfiler.SetState( profState );
		return 0;
	};

#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	float* distances = new float[ tableSize ];
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	DynamicArray<PathfinderPortalNode*>* portalPaths = new DynamicArray<PathfinderPortalNode*>[ tableSize ];

	int indPtr = 0;
	for( int i = 0; i < startPortals.Size(); i++ )
	{
		for( int j = 0; j < endPortals.Size(); j++ )
		{
			if( startPortals[i] != endPortals[j] )
			{
				distances[indPtr] = GetShortestPortalPath( startPortals[i], endPortals[j], portalPaths[indPtr], zoneFrom, bigMonster );
				if( !portalPaths[indPtr].Size() )
				{
					distances[indPtr] = FLT_MAX;
				};
			}
			else
			{
				distances[indPtr] = 0.0f;
			}
			indPtr++;
		};
	};

	float minDist = FLT_MAX;
	int minI = 0;
	int minJ = 0;
	int minIndPtr = 0;
	indPtr = 0;
	for( int i = 0; i < startPortals.Size(); i++ )
	{
		for( int j = 0; j < endPortals.Size(); j++ )
		{
			float newDist = startDistances[i]+distances[indPtr]+endDistances[j];
			if( newDist < minDist )
			{
				minDist = newDist;
				minI = i;
				minJ = j;
				minIndPtr = indPtr;
			};
			indPtr++;
		};
	};

	if( minDist < FLT_MAX )
	{
		if( portalPaths[minIndPtr].Size() == 0 )
		{
			portalPaths[minIndPtr].Add( endPortals[minJ] );
		};
		path.mPortals = portalPaths[minIndPtr];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		path.mPrecisePath = new WaypointPath(startPaths[minI]);
	};

	delete[] endPaths;
	delete[] endDistances;
	delete[] startPaths;
	delete[] startDistances;
	delete[] portalPaths;
	delete[] distances;

	if( minDist < FLT_MAX )
	{
		if( path.mPrecisePath->Size() > 1 )
		{
			path.mPosMinus1 = path.mPrecisePath->GetNextFromPath();
		}
		else
		{
			path.mPosMinus1 = path.mPrecisePath->PeekNextFromPath();
		};

		// Additional check for position on the line 1-2
		RaySegment ray;
		ray.Origin = path.mPosMinus1;
		Vector point2;
		if( path.PeekPos( 0, point2 ) )
		{
			if( ( point2 == ray.Origin ) && !path.PeekPos( 1, point2 ) )
			{
				// Do nothing
			}
			else
			{
				ray.Dir = point2 - ray.Origin;
				ray.Dir.Normalize();
				if( ray.DistanceFromPoint( from ) < 0.1f )
				{
					path.Step();	// forget about the first point
				};
			};
		};

		path.mDistance = minDist;
		GProfiler.SetState( profState );
		return minDist;
	}
	else
	{
		GProfiler.SetState( profState );
		return 0;
	};
}

// For finding path between portals

float Pathfinder::GetShortestPortalPath( PathfinderPortalNode* from, PathfinderPortalNode* to, DynamicArray<PathfinderPortalNode*>& list, int startZoneIndex, int bigMonster )
{
	// Safeguards
	if( mHowManyPortalNodes <= 0 )
	{
		return 0;
	};

	Vector toCenter = to->GetPortal()->Box.GetCenter();

	BitSet inClosed;
	inClosed.Resize( mHowManyPortalNodes );
	BitSet inOpen;
	inOpen.Resize( mHowManyPortalNodes );

	PointerHeap<PathfinderAStarNode> openNodes;
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	PathfinderAStarNode* nodes = new PathfinderAStarNode[ mHowManyPortalNodes ];
	PathfinderAStarNode* current;

	// Insert the starting node
	current = nodes + (from-mPortalNodes);
	current->mPortal = from;
	current->mCost = 0;
	current->mFromIndex = -1;
	current->mLastZoneIndex = startZoneIndex;
	current->mHeuristic = (from->GetPortal()->Box.GetCenter() - toCenter).Magnitude()*1.05f;
	openNodes.Insert( current );
	inOpen.Set( from - mPortalNodes );

	char endReached = 0;

	// While there are still nodes to visit
	while( openNodes.Size() )
	{
		current = openNodes.Remove();
		inOpen.Clear( current-nodes );
		inClosed.Set( current-nodes );

		// End reached?
		if( current->mPortal == to )
		{
			endReached = 1;
			break;
		};

		// For each neighbour of our node
		PathfinderAStarNode* searchPtr = 0;
		for( int j = 0; j < current->mPortal->GetPortal()->ZoneRefs.Size(); j++ )
		{
			if( current->mPortal->GetPortal()->ZoneRefs[j] == current->mLastZoneIndex )
			{
				continue;
			};

			WaypointSet& ws = mWaypointSets[current->mPortal->GetPortal()->ZoneRefs[j]];
			for( int k = 0; k < ws.HowManyPortals(); k++ )
			{
				float dist;
				PathfinderPortalNode* pn = ws.GetConnectionFromPortal( current->mPortal, k, dist, bigMonster );
				searchPtr = nodes + (pn-mPortalNodes);

				if( dist == FLT_MAX )	// no connection
				{
					continue;
				};

				if( ( inClosed.Test( searchPtr-nodes ) ||
					inOpen.Test( searchPtr-nodes ) ) )
				{
					if( searchPtr->mCost > current->mCost + dist )
					{
						searchPtr->mFromIndex = current - nodes;
						searchPtr->mLastZoneIndex = current->mPortal->GetPortal()->ZoneRefs[j];
						searchPtr->mCost = current->mCost + dist;
						searchPtr->mHeuristic = (searchPtr->mPortal->GetPortal()->Box.GetCenter() - toCenter).Magnitude()*1.05f;
						inClosed.Clear( searchPtr-nodes );
						if( inOpen.Test( searchPtr-nodes ) )
						{
							// Adjust open node in heap
							openNodes.Remove( searchPtr );
							openNodes.Insert( searchPtr );
						}
						else	// Add node to heap
						{
							openNodes.Insert( searchPtr );
							inOpen.Set( searchPtr-nodes );
						};
					};		// else node does not need changes
				}
				else	// new node; not in open or close
				{
					searchPtr->mFromIndex = current - nodes;
					searchPtr->mPortal = pn;
					searchPtr->mLastZoneIndex = current->mPortal->GetPortal()->ZoneRefs[j];
					searchPtr->mCost = current->mCost + dist;
					searchPtr->mHeuristic = (searchPtr->mPortal->GetPortal()->Box.GetCenter() - toCenter).Magnitude()*1.05f;
					openNodes.Insert( searchPtr );
					inOpen.Set( searchPtr-nodes );
				};
			};
		};
	};

	if( endReached )	// create path list ( first node at end ) and return cost
	{
		float cost = current->mCost;
		while( current->mFromIndex != -1 )
		{
			list.Add( current->mPortal );
			current = nodes + current->mFromIndex;
		};
		list.Add( current->mPortal );
		delete[] nodes;
		return cost;
	}
	else
	{
		delete[] nodes;
		return 0;
	};

/*	// OLD VERSION, POSSIBLY FLAWED IN MANY WAYS

	PointerHeap<PathfinderAStarNode> openNodes;
	DynamicArray<PathfinderAStarNode*> closedNodes;
	PathfinderAStarNode* current;

	Vector toCenter = to->GetPortal()->Box.GetCenter();

	// Insert the starting node
	current = new PathfinderAStarNode;
	current->mPortal = from;
	current->mCost = 0;
	current->mFromIndex = -1;
	current->mLastZoneIndex = startZoneIndex;
	current->mHeuristic = (from->GetPortal()->Box.GetCenter() - toCenter).Magnitude()*1.05f;
	openNodes.Insert( current );

	char endReached = 0;
	// While there are still nodes to visit
	while( openNodes.Size() )
	{
		current = openNodes.Remove();
		closedNodes.Add( current );

		// End reached?
		if( current->mPortal == to )
		{
			endReached = 1;
			break;
		};

		// For each neighbour of our node
		PathfinderPortalNode* searchPtr = 0;
		for( int j = 0; j < current->mPortal->GetPortal()->ZoneRefs.Size(); j++ )
		{
			if( current->mPortal->GetPortal()->ZoneRefs[j] == current->mLastZoneIndex )
			{
				continue;
			};

			WaypointSet& ws = mWaypointSets[current->mPortal->GetPortal()->ZoneRefs[j]];
			for( int k = 0; k < ws.HowManyPortals(); k++ )
			{
				float dist;
				searchPtr = ws.GetConnectionFromPortal( current->mPortal, k, dist );

				if( dist == FLT_MAX )	// no connection
				{
					continue;
				};

				char inList = 0;
				for( int i = 0; i < closedNodes.Size(); i++ )
				{
					if( closedNodes[i]->mPortal == searchPtr )
					{
						inList = 1;
					};
				};

				if( !inList )
				{					// not visited yet
					// is it in the list of open nodes?
					int nodeNum = -1;
					for( int i = 0; i < openNodes.Size(); i++ )
					{
						if( openNodes[i]->mPortal == searchPtr )
						{
							nodeNum = i;
						};
					};

					PathfinderAStarNode* newOpen;
					if( nodeNum != -1 )
					{
						newOpen = openNodes[nodeNum];
					}
					else
					{
						newOpen = new PathfinderAStarNode(searchPtr);
					};

					newOpen->mFromIndex = closedNodes.Size()-1;	// index of current in closedNodes
					newOpen->mLastZoneIndex = current->mPortal->GetPortal()->ZoneRefs[j];
					newOpen->mCost = current->mCost + dist;
					newOpen->mHeuristic = (searchPtr->GetPortal()->Box.GetCenter() - toCenter).Magnitude()*1.05f;

					if( nodeNum == -1 )
					{
						openNodes.Insert( newOpen );
					};
				};
			};
		};
	};

	if( endReached )	// create path list ( first node at end ) and return cost
	{
		float cost = current->mCost;
		while( current->mFromIndex != -1 )
		{
			list.Add( current->mPortal );
			current = closedNodes[current->mFromIndex];
		};
		list.Add( current->mPortal );

		for( int i = 0; i < closedNodes.Size(); i++ )
		{
			delete closedNodes[i];
		};
		openNodes.ClearContents();
		return cost;
	}
	else
	{
		for( int i = 0; i < closedNodes.Size(); i++ )
		{
			delete closedNodes[i];
		};
		openNodes.ClearContents();
		return 0;
	};
	*/
}

#ifdef TRANSGAMING_USE_EDITOR
void Pathfinder::Render( int renderWaypoints, int renderFloors )
{
	LineRender& lineBuf=GEngine->Video->LineBuf;

	WaypointSet* ws = GetCurrentWaypointSet();
	if( ws )
	{
		if( renderWaypoints )
		{
			lineBuf.DrawWaypointSet( *ws );
		};

		if( renderFloors )
		{
			PathfinderFloorSet* fs = ws->GetFloorSet();
			if( fs )
			{
				fs->Render();
			};
		};
	};
}
#endif

void Pathfinder::FloorSelection(RECT* rect, int select )
{
	PathfinderFloorSet* fs = GetCurrentFloorSet();
	if( fs )
	{
		fs->Selection(*mViewport, rect, select );
	};
}

void Pathfinder::SelectUnselectFloorsOfAreaLowerHigherThan( int select, int lower, float area )
{
	PathfinderFloorSet* fs = GetCurrentFloorSet();
	if( fs )
	{
		fs->SelectUnselectFloorsOfAreaLowerHigherThan( select, lower, area );
	};
}

void Pathfinder::RemoveSelectedFloors( void )
{
	PathfinderFloorSet* fs = GetCurrentFloorSet();
	if( fs )
	{
		fs->RemoveSelected();
	};
}

void Pathfinder::RemoveSelected( void )
{
	WaypointSet* ws = GetCurrentWaypointSet();
	if( ws )
	{
		ws->RemoveSelected();
	};
}

void Pathfinder::MoveSelectedToZoneTheyreIn( void )
{
	WaypointSet* ws = GetCurrentWaypointSet();
	if( ws )
		ws->MoveSelectedToTheZoneTheyreIn();
}

WaypointSet* Pathfinder::GetCurrentWaypointSet( void )
{
	if( !mWorld->Zones.Size() )
	{
		return &mNoZonesWaypointSet;
	};

	// XXX: get rid of that
	Vector eye = mViewport->EyePosition;
	// find the zone we're in
	int zoneIndex = mWorld->FindZone( eye );

	if( zoneIndex != EInvalidIndex )
	{
		if( mHowManyWaypointSets > zoneIndex )
		{
			return &mWaypointSets[zoneIndex];
		};
	};
	return 0;
}

PathfinderFloorSet* Pathfinder::GetCurrentFloorSet( void )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		PathfinderFloorSet* fs = ws->GetFloorSet();
		if( fs )
		{
			return fs;
		};
	};
	return 0;
}

int Pathfinder::AddWaypoint( Waypoint& waypoint )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		return ws->AddWaypoint( waypoint );
	};
	return -1;
}

void Pathfinder::MoveWaypoint( Waypoint& waypoint, Vector newPosition, int recalculateDistances )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->MoveWaypoint( waypoint, newPosition, recalculateDistances );
	};
}

void Pathfinder::MoveWaypoint( int waypointIndex, Vector newPosition, int recalculateDistances )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->MoveWaypoint( waypointIndex, newPosition, recalculateDistances );
	};
}

void Pathfinder::RecalculateDistances( int waypointIndex )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->RecalculateDistances( waypointIndex );
	};
}

void Pathfinder::RemoveWaypoint( Waypoint& waypoint )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->RemoveWaypoint( waypoint );
	};
}

void Pathfinder::RemoveWaypoint( int index )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->RemoveWaypoint( index );
	};
}

int Pathfinder::PickWaypoint( int xm, int ym, float toleranceDistance )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		return ws->PickWaypoint( *mViewport, xm, ym, toleranceDistance );
	};
	return -1;
}

int Pathfinder::Selection( float toleranceDistance, RECT* rect, int select )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		return ws->Selection( *mViewport, toleranceDistance, rect, select );
	};
	return -1;
}

void Pathfinder::DisplaceSelected( const Vector& displacement )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->DisplaceSelected( displacement );
	};
}

void Pathfinder::CopySelected( void )
{
	const Vector displacement = Vector( 0.5f, 0.0f, 0.0f );
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->CopySelected( displacement );
	};
}

void Pathfinder::ScaleSelected( const Vector& center, float scale )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->ScaleSelected( center, scale );
	};
}

void Pathfinder::RecalculateSelected( void )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->RecalculateSelected();
	};
}

void Pathfinder::FloorSelectedSetClearAll( int set )
{
	PathfinderFloorSet* fs = GetCurrentFloorSet();
	if( fs )
	{
		fs->SelectedSetClearAll( set );
	};
}

void Pathfinder::WaypointSelectedSetClearAll( int set )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->SelectedSetClearAll( set );
	};
}

void Pathfinder::ConnectDisconnectSelected( float minDistance, float maxDistance, int connect, int disconnect )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->ConnectDisconnectSelected( minDistance, maxDistance, connect, disconnect );
	};
}

void Pathfinder::ConnectSelected( void )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->ConnectSelected();
	};
}

void Pathfinder::DisconnectSelected( void )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->DisconnectSelected();
	};
}

void Pathfinder::LevelWaypointsWithFloors( float upDisplacement )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->LevelWaypointsWithFloors( upDisplacement );
	};
}

void Pathfinder::AddGridOnSelectedFloors( float step )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->AddGridOnSelectedFloors( step );
	};
}

void Pathfinder::Test_InitWithWaypointGrid( int howMany )
{
	if( !mWorld->Zones.Size() )
	{
		mNoZonesWaypointSet.Test_InitWithWaypointGrid( howMany );
	};
}

float Pathfinder::LocalGetShortestPath( int from, int* to, int toHowMany, AStarHeuristicFunction heuristic, WaypointPath& path )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		return ws->GetShortestPath( from, to, toHowMany, heuristic, path );
	};
	return FLT_MAX;
}

#ifdef TRANSGAMING_USE_EDITOR
void Pathfinder::SelectUnselectWaypointsOnSelectedFloors( int select )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->SelectUnselectWaypointsOnSelectedFloors( select );
	};
}

void Pathfinder::RemoveConnectionsCollidingWithGeometryInSelected( void )
{
	int zoneIndex;
	if( !mWorld->Zones.Size() )
	{
		zoneIndex= -1;
	}
	else
	{
		// XXX: get rid of that
		Vector eye = mViewport->EyePosition;
		// find the zone we're in
		zoneIndex = mWorld->FindZone( eye );
	};

	if( zoneIndex != EInvalidIndex )
	{
		if( mHowManyWaypointSets > zoneIndex )
		{
			mWaypointSets[zoneIndex].RemoveConnectionsCollidingWithGeometryInSelected( mWorld, *mWorld->Zones[zoneIndex] );
		};
	};
}
#endif

#ifdef TRANSGAMING_USE_EDITOR
void Pathfinder::RemoveWaypointsOutsideOfZones( float portalThickness )
{
	for( int i = 0; i < mHowManyWaypointSets; i++ )
	{
		mWaypointSets[i].RemoveWaypointsOutsideOfZoneOrItsPortals( mWorld, i, portalThickness );
	};
}

void Pathfinder::SelectWaypointsOutsideOfCurrentZone( float portalThickness )
{
	if( !mWorld->Zones.Size() )
	{
		return;
	};

	// XXX: get rid of that
	Vector eye = mViewport->EyePosition;
	// find the zone we're in
	int zoneIndex = mWorld->FindZone( eye );

	if( zoneIndex == EInvalidIndex )
	{
		return;
	};

	mWaypointSets[zoneIndex].SelectWaypointsOutsideOfZoneOrItsPortals( mWorld, zoneIndex, portalThickness );
}
#endif

void Pathfinder::UnselectAllWaypointsInPathfinder( void )
{
	mNoZonesWaypointSet.SelectedSetClearAll(0);
	for( int i = 0; i < mHowManyWaypointSets; i++ )
	{
		mWaypointSets[i].SelectedSetClearAll(0);
	};
};

#ifdef TRANSGAMING_USE_EDITOR
void Pathfinder::CleanStructures( void )
{
	UnselectAllWaypointsInPathfinder();

	mNoZonesWaypointSet.RemoveWaypointsNotConnectedToAnything();
	mNoZonesWaypointSet.MergeWaypointsAtSamePosition();
	for( int i = 0; i < mHowManyWaypointSets; i++ )
	{
		mWaypointSets[i].RemoveWaypointsNotConnectedToAnything();
		mWaypointSets[i].MergeWaypointsAtSamePosition();
	};
}
#endif

void Pathfinder::ScaleContents( float scale )
{
	// Scale waypoint sets
	mNoZonesWaypointSet.ScaleContents( scale );
	mNoZonesWaypointSet.ScaleFloors( scale );
	for( int i = 0; i < mHowManyWaypointSets; i++ )
	{
		mWaypointSets[i].ScaleContents( scale );
		mWaypointSets[i].ScaleFloors( scale );
	};
}

void Pathfinder::FlipZ( void )
{
	// Scale waypoint sets
	mNoZonesWaypointSet.FlipZ();
	mNoZonesWaypointSet.FlipFloorZ();
	for( int i = 0; i < mHowManyWaypointSets; i++ )
	{
		mWaypointSets[i].FlipZ();
		mWaypointSets[i].FlipFloorZ();
	};
}

#ifdef TRANSGAMING_USE_EDITOR
void Pathfinder::SelectWaypointsNotConnectedToAnything( void )
{
	mNoZonesWaypointSet.SelectWaypointsNotConnectedToAnything();
	for( int i = 0; i < mHowManyWaypointSets; i++ )
	{
		mWaypointSets[i].SelectWaypointsNotConnectedToAnything();
	};
}

void Pathfinder::SelectWaypointsNotConnectedToAnythingInCurrentRoom( void )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->SelectWaypointsNotConnectedToAnything();
	};
}
#endif

int Pathfinder::TestStructuresCorectness( void )
{
	for( int i = 0; i < mHowManyPortalNodes; i ++ )
	{
		if( !mPortalNodes[i].TestNodeCorrectness( this ) )
		{
			return 0;	// incorrect
		};
	};
	return 1;	// correct
}

void Pathfinder::SetSelectedAsForSmallMonstersOnly( void )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->SelectedForSmallMonstersOnly();
	};
}

void Pathfinder::SetSelectedAsForAllMonsters( void )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->SelectedForAllMonsters();
	};
}

void Pathfinder::InvertSelection( void )
{
	WaypointSet* ws = GetCurrentWaypointSet();

	if( ws )
	{
		ws->InvertSelection();
	};
}

void Pathfinder::DeterminePortalNodesWideness( void )
{
	for( int i = 0; i < mHowManyPortalNodes; i++ )
	{
		mPortalNodes[i].DetermineWideness( this, i );
	};
}
