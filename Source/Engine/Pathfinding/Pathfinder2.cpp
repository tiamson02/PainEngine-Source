// ---------------------------------------------------------------------------
// Copyright (c) 2003 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2003/10 Krystian Galaj
//----------------------------------------------------------------------------

#include "Engine.h"

#include "Core/HashMap.h"
#include "Core/BitSet.h"

#include "Core/FileSystem/FileManagerStandard.h"
extern ENGINE_API FileManagerStd GFileMan;

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Render/Viewport.h"
#include "Engine/Mesh.h"

#include "PointerHeap.h"
#include "PathfindingNode.h"
#include "Pathfinder2.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Render/Video.h"
#include "Engine/World.h"

#if defined( TRANSGAMING_PSP )
#include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
#include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
#include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#define LOG_PATHFINDER_ACTIONS

//-------------------------------------------------------------------------------------------------------

#ifdef TRANSGAMING
void WaypointGPath2::Empty( void )
#else
inline void WaypointGPath2::Empty( void )
#endif
{
	if( mPrecisePath )
	{
		delete mPrecisePath;
		mPrecisePath = 0;
	}

	mPortals.ClearArray();
	mDistance = 0.0f;
	mCalculatedPoints.ClearArray();
	mCalculatedSetIndices.ClearArray();
	mCalculatedWaypointIndices.ClearArray();
	mCalculatedDirs.ClearArray();
}

WaypointPath2* WaypointGPath2::CalculatePreciselyNextSet( void )
{
	if( !mInit ) return 0;

	if( mPortals.Size() > 1 )
	{
		// Get portals
		Pathfinder2Portal* start = mPortals[mPortals.Size()-1];
		Pathfinder2Portal* end = mPortals[mPortals.Size()-2];

		// Determine connecting set
		int connectingSet = ( mCurrentSet == start->LowerSetIndex )?start->HigherSetIndex:start->LowerSetIndex;
		Assert( ( end->LowerSetIndex == connectingSet ) || ( end->HigherSetIndex == connectingSet ) );

		// Calculate starting point
		Vector lastPoint;
		GetPosLast( lastPoint );
		int startPoint = mPathfinder->GetClosestInPortal( start, lastPoint, connectingSet );
		Assert( startPoint != -1 );

		// Determine target point for heuristics function
		Vector target = mPortals[mPortals.Size()-2]->Center;
		for( int portalInd = 2; portalInd <= mPortals.Size(); ++portalInd )
			target = mPathfinder->CenterCalculation( mPortals[mPortals.Size()-portalInd], lastPoint, target );

		// Calculate new precise path
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		WaypointPath2* path = new WaypointPath2( this );
		mPathfinder->GetShortestPath( startPoint, (int*)&(end->Waypoints[0]), end->Waypoints.Size(), path, false, &target,
			FLT_MAX, connectingSet );
		Assert( path->Size() != 0 );

		// Input it into structures
		mPortals.RemoveLast();
		if( mPrecisePath )
			delete mPrecisePath;
		mPrecisePath = path;
		mCurrentSet = connectingSet;
		return mPrecisePath;
	}
	else if( mPortals.Size() == 1 )	// last path segment
	{
		// Get portals
		Pathfinder2Portal* start = mPortals[mPortals.Size()-1];
		int end = mPathfinder->GetIndexOfWaypointClosestTo( mEndPoint );
		int set = mPathfinder->mWaypoints[end].Set;
		mCurrentSet = set;

		// Calculate starting point
		Vector lastPoint;
		GetPosLast( lastPoint );
		int startPoint = mPathfinder->GetClosestInPortal( start, lastPoint, set );
		Assert( ( startPoint != -1 ) && ( mPathfinder->mWaypoints[startPoint].Set == set ) );

#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		WaypointPath2* path = new WaypointPath2( this );
		mPathfinder->GetShortestPath( startPoint, &end, 1, path, false, 0, FLT_MAX, set );
		Assert( path->Size() != 0 );
		mPortals.RemoveLast();

		if( mPrecisePath )
			delete mPrecisePath;
		mPrecisePath = path;
		return mPrecisePath;
	}
	else
	{
		if( mPrecisePath )
		{
			delete mPrecisePath;
			mPrecisePath = 0;
		}
		return 0;	// end of path
	};
}

void WaypointGPath2::Step( void )
{
	Vector t;
	PeekDir( 0, t );	// make sure something is there
	mCalculatedDirs.Remove(0);
	PeekPos( 0, t );	// make sure something is there
	Vector t2;
	if( PeekPos( 1, t2) )
		mDistance -= (t2-t).Magnitude();
	mCalculatedPoints.Remove(0);
	mCalculatedSetIndices.Remove(0);
	mCalculatedWaypointIndices.Remove(0);
}

int WaypointGPath2::InternalGetNext( Vector& next, int& setIndex, int& waypointIndex )
{
	if( !mPrecisePath )
		return 0;

	while( !mPrecisePath->Size() )
	{
		if( CalculatePreciselyNextSet() )
			mPrecisePath->GetNextFromPath();	// remove first, it will be equal to curr
		else
			return 0;
	};

	setIndex = mCurrentSet;
	waypointIndex = mPrecisePath->PeekNextIndex(mPathfinder);
	next = mPrecisePath->GetNextFromPath();
	return 1;
}

int WaypointGPath2::PeekPos( int index, Vector& pos )
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
				return 0;
			}

		} while( t == lastPos );

		mCalculatedPoints.Add( t );
		mCalculatedSetIndices.Add( z );
		mCalculatedWaypointIndices.Add( w );
	};

	pos = mCalculatedPoints[index];

	return 1;
}

void WaypointGPath2::PeekDir( int index, Vector& dir )
{
	while( mCalculatedDirs.Size() <= index )
	{
		Vector t1;
		if( index == 0 )
			t1 = mPosMinus1;
		else
			PeekPos( index-1, t1 );

		Vector t2;
		if( !PeekPos( index+1, t2 ) )
			if( !PeekPos( index, t2 ) )
				t2 = t1;

		Vector t = t2 - t1;
		t.Normalize();
		mCalculatedDirs.Add( t );
	}

	dir = mCalculatedDirs[ index ];
}

void WaypointGPath2::RecalculatePath( void )
{
	Vector from;
	if( !PeekPos( 0, from ) )	// empty path
		return;

	// Check if path optimisations can be made now
	for( int i = 1; mCalculatedSetIndices[i] == mCalculatedSetIndices[0]; ++i )
		if( mPathfinder->mWaypoints[mCalculatedWaypointIndices[i]].Disabled )
		{
			mPathfinder->GetShortestPath( from, mEndPoint, *this );
			return;
		}
}

WaypointGPath2& WaypointGPath2::operator=(const WaypointGPath2& a)
{
	if( &a == this )
		return *this;

	Empty();

#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	mPrecisePath = new WaypointPath2(*a.mPrecisePath);

	mCalculatedPoints = a.mCalculatedPoints;
	mCalculatedSetIndices = a.mCalculatedSetIndices;
	mCalculatedWaypointIndices = a.mCalculatedWaypointIndices;
	mCalculatedDirs = a.mCalculatedDirs;

	mPathfinder = a.mPathfinder;
	mInit = a.mInit;
	mPortals = a.mPortals;
	mEndPoint = a.mEndPoint;
	mDistance = a.mDistance;
	mPosMinus1 = a.mPosMinus1;
	mCurrentSet = a.mCurrentSet;

	return *this;
}

#define WAYPOINTGPATH2_LOAD_SAVE_TOKEN 0x5e44

bool WaypointGPath2::Load( FileObject* f )
{
	if( !f->TestToken( WAYPOINTGPATH2_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: WaypointGPath2::Load()\n" );
		return false;
	}

	// Load info and recalculate path
	Vector start;
	Vector end;

    *f >> start;
    *f >> end;
#ifdef	TRANSGAMING
	GEngine->pcfWorld->Paths2.GetShortestPath( start, end, *this );
#else
	GEngine->World->Paths2.GetShortestPath( start, end, *this );
#endif
	return true;
}

bool WaypointGPath2::Save( FileObject* f )
{
	if( !f->PutToken( WAYPOINTGPATH2_LOAD_SAVE_TOKEN ) ) return false;

	// Save enough info to recalculate path
	Vector pos;
	PeekPos(0,pos);
	*f << pos;
	*f << mEndPoint;
	return true;
}

//-------------------------------------------------------------------------------------------------------

Pathfinder2::Pathfinder2( void ) : CurrentSet( 0 ), FirstUnusedSetNumber(1),
	AutoValid( false ), SortedWaypoints( false ), CalculatedBorders( false ),
	mDistances(0), mHowManyDistances(0), mPackedIndices(0), ViewSize(50), DrawDisabledSets(true)
{
	EnabledSets.Add( true );	// set 0 existing, enabled

#ifdef TRANSGAMING_USE_EDITOR
	SelectedColor		= 0x00ff00;
	ChosenSetColor		= 0xff0000;
	DisabledColor		= 0x404040;
	BorderColor			= 0xffff00;

	SetColors[0]		= 0xffffff;
	SetColors[1]		= 0x0000ff;
	SetColors[2]		= 0x00ffff;
	SetColors[3]		= 0xff00ff;
	SetColors[4]		= 0xffffff;
	SetColors[5]		= 0x0000ff;

	GetColorsFromScript();
#endif
}

Pathfinder2::~Pathfinder2(void)
{
	Assert( WaypointPaths.Size() == FreePathIndices.Size() );
	for( int i = 0; i < Portals.Size(); ++i )
		delete Portals[i];
	if( mDistances ) delete[] mDistances, mDistances = 0;
	if( mPackedIndices ) delete[] mPackedIndices, mPackedIndices = 0;
}

// Waypoint manipulation

int Pathfinder2::AddWaypoint( Waypoint2 waypoint )
{
	waypoint.Base = mHowManyDistances;
	waypoint.Count = 0;
	int num = mWaypoints.Add( waypoint );

	SortedWaypoints = false;
	ClearAutomaticStructures();
	return num;
}

void Pathfinder2::MoveWaypoint( int waypointIndex, Vector newPosition, bool recalculateDistances )
{
	mWaypoints[waypointIndex].Position = newPosition;
	if( recalculateDistances )
		RecalculateDistances( waypointIndex );
	SortedWaypoints = false;
	ClearAutomaticStructures();
}

void Pathfinder2::RecalculateDistances( int waypointIndex )
{
	if( !mDistances ) return;

	int size = mWaypoints.Size();
	for( int i = 0; i < size; ++i )
	{
		Waypoint2& wp = mWaypoints[i];

		if( i == waypointIndex )
		{
			for( int j = 0; j < wp.Count; ++j )
				mDistances[wp.Base+j] = mWaypoints[waypointIndex].Dist( mWaypoints[mPackedIndices[wp.Base+j]] );
			continue;
		}

		for( int j = 0; j < wp.Count; ++j )
			if( mPackedIndices[wp.Base+j] == waypointIndex )
				mDistances[wp.Base+j] = mWaypoints[i].Dist( mWaypoints[waypointIndex] );
	}

	ClearAutomaticStructures();
}

void Pathfinder2::RecalculateAllDistances( void )
{
	if( !mDistances ) return;

	int size = mWaypoints.Size();
	for( int i = 0; i < size; ++i )
	{
		Waypoint2& wp = mWaypoints[i];
		for( int j = 0; j < wp.Count; ++j )
			mDistances[wp.Base+j] = wp.Dist( mWaypoints[mPackedIndices[wp.Base+j]] );
	}

	ClearAutomaticStructures();
}

void Pathfinder2::RemoveWaypoint( Waypoint2& waypoint )
{
	int i = FastGetWaypointIndex( &waypoint );
	if( i == -1 ) return;	// not found
	RemoveWaypoint(i);
	SortedWaypoints = false;
	ClearAutomaticStructures();
}

void Pathfinder2::RemoveWaypoint( int index )
{
	ClearUndo( index );
	Select( index, false );

	int size = mWaypoints.Size();

	if( !mDistances )
	{
		for( int i = index; i < size-1; ++i )
			mWaypoints[i] = mWaypoints[i+1];
		mWaypoints.RemoveLast();
	}
	else
	{
		// Calculate new number of indices
		int newHowManyDistances = mHowManyDistances - mWaypoints[index].Count;
		for( int i = 0; i < mHowManyDistances; ++i )
			if( (mPackedIndices[i] == index) && ( ( i < mWaypoints[index].Base ) || ( i - mWaypoints[index].Base >= mWaypoints[index].Count ) ) )
				newHowManyDistances--;

		if( !newHowManyDistances )
		{
			mHowManyDistances = 0;
			delete[] mDistances;		mDistances = 0;
			delete[] mPackedIndices;	mPackedIndices = 0;

			for( int i = index; i < size-1; ++i )
				mWaypoints[i] = mWaypoints[i+1];
			mWaypoints.RemoveLast();

			for( int i = index; i < size-1; ++i )
				mWaypoints[i].Count = mWaypoints[i].Base = 0;
		}
		else
		{
#ifdef TRANSGAMING_PSP
            m_setTag("AI");
#endif
			float* newDist = new float[ newHowManyDistances ];
#ifdef TRANSGAMING_PSP
            m_setTag("AI");
#endif
			int* newPackedIndices = new int[ newHowManyDistances ];

			int newDistPtr = 0;

			// For waypoints before the one to remove
			for( int i = 0; i < index; ++i )
			{
				int base = mWaypoints[i].Base;
				mWaypoints[i].Base = newDistPtr;
				for( int j = 0; j < mWaypoints[i].Count; ++j )
				{
					int dest = mPackedIndices[base+j];
					if( dest != index )
					{
						newDist[newDistPtr] = mDistances[base+j];
						newPackedIndices[newDistPtr] = (dest>index)?dest-1:dest;
						newDistPtr++;
					}
				}
				mWaypoints[i].Count = newDistPtr-mWaypoints[i].Base;
			}

			// For waypoints after the one to remove
			for( int i = index+1; i < size; ++i )
			{
				int base = mWaypoints[i].Base;
				mWaypoints[i].Base = newDistPtr;
				for( int j = 0; j < mWaypoints[i].Count; ++j )
				{
					int dest = mPackedIndices[base+j];
					if( dest != index )
					{
						newDist[newDistPtr] = mDistances[base+j];
						newPackedIndices[newDistPtr] = (dest>index)?dest-1:dest;
						newDistPtr++;
					}
				}
				mWaypoints[i].Count = newDistPtr - mWaypoints[i].Base;
			}

			// Now change wptable
			for( int i = index; i < size-1; ++i )
				mWaypoints[i] = mWaypoints[i+1];
			mWaypoints.RemoveLast();

			mHowManyDistances = newHowManyDistances;
			delete[] mDistances;
			mDistances = newDist;
			delete[] mPackedIndices;
			mPackedIndices = newPackedIndices;
		}
	}

	CalculatedBorders = false;
	SortedWaypoints = false;
	ClearAutomaticStructures();
}

int Pathfinder2::GetWaypointIndex( Waypoint2& waypoint )
{
	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( mWaypoints[i] == waypoint )
			return i;
	return -1;
}

void Pathfinder2::ScaleContents( float scale )
{
	for( int i = 0; i < mWaypoints.Size(); ++i ) mWaypoints[i].Position *= scale;
	if( mDistances )
		for( int i = 0; i < mHowManyDistances; ++i )
		{
			mDistances[i] *= scale;
			ClearAutomaticStructures();
		}
}

void Pathfinder2::FlipZ( void )
{
	for( int i = 0; i < mWaypoints.Size(); ++i )
		mWaypoints[i].Position.Z *= -1.0;
}

// Waypoint selection

void Pathfinder2::Select_NotConnectedToAnything( void )
{
	if( !mDistances )
		Select_All( true );
	else
		for( int i = 0; i < mWaypoints.Size(); ++i )
			if( !mWaypoints[i].Count )
				Select( i,true );
}

void Pathfinder2::Select_All( bool select )
{
	if( !select )
		for( int i = 0; i < mWaypoints.Size(); ++i )
			mWaypoints[i].Selected = false;
	else
	{
		for( int i = 0; i < mWaypoints.Size(); ++i )
			if( SetEnabled( mWaypoints[i].Set ) )
				mWaypoints[i].Selected = true;
	}
	UndoDisplacement.ClearArray();
}

// Operations on selected waypoints

#ifdef TRANSGAMING_USE_EDITOR
void Pathfinder2::Selected_Displace( const Vector& displacement )
{
	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( mWaypoints[i].Selected )
		{
			MakeUndo(i,mWaypoints[i].Position);
			mWaypoints[i].Position += displacement;
			SortedWaypoints = false;
			ClearAutomaticStructures();
		}
}

void Pathfinder2::Selected_Remove( void )
{
	for( int i = mWaypoints.Size()-1; i >= 0; i-- )
		if( mWaypoints[i].Selected )
			RemoveWaypoint(i);
}

void Pathfinder2::Selected_Copy( const Vector& displacement )
{
	DynamicArray<int> indices;
	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( mWaypoints[i].Selected )
		{
			Waypoint2 wp;
			wp.Position = mWaypoints[i].Position;
			wp.Position += displacement;
			wp.Selected = false;
			wp.BigMonsters = false;
			wp.Disabled = false;
			wp.Border = false;
			wp.Set = GetValidCurrentSet();
			indices.Add( AddWaypoint( wp ) );
		};
	Select_All( false );
	for( int i = 0; i < indices.Size(); ++i )
		Select( indices[i], true );
}

void Pathfinder2::Selected_Scale( const Vector& center, float scale )
{
	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( mWaypoints[i].Selected )
		{
			Vector diff = mWaypoints[i].Position - center;
			diff *= scale;
			mWaypoints[i].Position = center + diff;
			ClearAutomaticStructures();
		}
	UndoDisplacement.ClearArray();
}

void Pathfinder2::Selected_Recalculate( void )
{
	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( mWaypoints[i].Selected )
			RecalculateDistances( i );
}

void Pathfinder2::Selected_ConnectDisconnect( float minDistance, float maxDistance, int connect, int disconnect )
{
	int size = mWaypoints.Size();
	if( !size ) return;

	DynamicArray<float> newDist;
	DynamicArray<int> newInd;

#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
	int* newBase = new int[ size ];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
	int* newCount = new int[ size ];

	int newDistPtr = 0;

	for( int i = 0; i < size; ++i )
	{
		if( !mWaypoints[i].Selected )
		{
			int base = mWaypoints[i].Base;
			newBase[i] = newDistPtr;
			for( int j = 0; j < mWaypoints[i].Count; ++j )
			{
				newDist.Add( mDistances[base+j] );
				newInd.Add( mPackedIndices[base+j] );
				newDistPtr++;
			}
			newCount[i] = newDistPtr - newBase[i];
		}
		else
		{
			newBase[i] = newDistPtr;
			for( int j = 0; j < size; ++j )
			{
				float curDist = mWaypoints[i].Dist( mWaypoints[j] );

				bool connected = false;
				for( int l = 0; l < mWaypoints[j].Count; ++l )
					if( mPackedIndices[mWaypoints[j].Base+l] == i )
					{
						connected = true;
						break;
					}

				if( !mWaypoints[j].Selected )
				{
					if( connected )
					{
						newDist.Add( curDist );
						newInd.Add( j );
						newDistPtr++;
					}
					continue;
				}

				if( ( !connect && !connected ) || ( !disconnect && connected ) )
					continue;

				if( ( curDist < minDistance ) || ( curDist > maxDistance ) )
					continue;

				newDist.Add( curDist );
				newInd.Add( j );
				newDistPtr++;
			}
			newCount[i] = newDistPtr - newBase[i];
		}
	}

	if( mDistances ) delete[] mDistances;
	if( mPackedIndices ) delete[] mPackedIndices;

	mHowManyDistances = newDist.Size();

	if( mHowManyDistances )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mDistances = new float[newDist.Size()];
		for( int i = 0; i < newDist.Size(); ++i )
			mDistances[i] = newDist[i];

#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mPackedIndices = new int[newInd.Size()];
		for( int i = 0; i < newInd.Size(); ++i )
			mPackedIndices[i] = newInd[i];
	}
	else
	{
		mDistances = 0;
		mPackedIndices = 0;
	}

	for( int i = 0; i < size; ++i )
	{
		mWaypoints[i].Base = newBase[i];
		mWaypoints[i].Count = newCount[i];
	}
	delete[] newBase;
	delete[] newCount;

	ClearAutomaticStructures();
	CalculatedBorders = false;
}
#endif

int Pathfinder2::GetIndexOfWaypointClosestTo( const Vector& point, float toleranceDistance )
{
	if( !mWaypoints.Size() ) return -1;

	SortWaypoints();

	// 1. Find out the index of waypoint closest to us in X
	int low = 0;
	int high = mWaypoints.Size();

	while( low+1 != high )
	{
		int center = (low+high)>>1;
		if( mWaypoints[center].Position.X >= point.X )
			low = center;
		else
			high = center;
	}
	int closestIndex = low;
	int sizeMinusClosestIndex = mWaypoints.Size() - low;

	// 1.1. Setup current closest wp data
	float distanceSq;
	int wpIndex;
	if( mWaypoints[closestIndex].Disabled )
	{
		distanceSq = toleranceDistance*toleranceDistance*2;
		wpIndex = -1;
	}
	else
	{
		distanceSq = (mWaypoints[closestIndex].Position - point).MagnitudeSquared();
		wpIndex = closestIndex;
	}

	// 2. Go up-down from it, searching for closest. Remember closest distance, and when Xdown and Xup go over it, stop search
	int add = 1;
	bool goDown = ( add <= closestIndex );
	bool goUp = ( add < sizeMinusClosestIndex );

	while( goDown || goUp )
	{
		if( goDown && !mWaypoints[closestIndex-add].Disabled )
		{
			Vector& pos = mWaypoints[closestIndex-add].Position;
			float newDistSq = (pos-point).MagnitudeSquared();

			if( newDistSq < distanceSq )
			{
				distanceSq = newDistSq;
				wpIndex = closestIndex-add;
			}

			float diff = point.X - pos.X;
			if( ( diff*diff > distanceSq ) || ( add == closestIndex ) )
				goDown = false;
		}

		if( goUp && !mWaypoints[closestIndex+add].Disabled )
		{
			Vector& pos = mWaypoints[closestIndex+add].Position;
			float newDistSq = (pos-point).MagnitudeSquared();

			if( newDistSq < distanceSq )
			{
				distanceSq = newDistSq;
				wpIndex = closestIndex+add;
			}

			float diff = pos.X - point.X;
			if( ( diff*diff > distanceSq ) || ( add == sizeMinusClosestIndex-1 ) )
				goUp = false;
		}

		add++;
	}

	return ( distanceSq < toleranceDistance*toleranceDistance )?wpIndex:-1;
}

Waypoint2* Pathfinder2::GetNextAvailableFrom( int from, int& searchCount, int& searchPtr, float& dist )
{
	int count = mWaypoints[from].Count;
	if( searchCount >= count ) return 0;
	int index = mWaypoints[from].Base + searchCount;
	searchCount++;
	searchPtr = mPackedIndices[index];
	dist = mDistances[index];
	return &mWaypoints[searchPtr];
}

float Pathfinder2::GetShortestPath( int from, int* to, int toHowMany, WaypointPath2* path, bool bigMonsters, Vector* targetForHeur, float maxLenToTry, int restrictToZone )
{
	// Safeguards
	if( !mWaypoints.Size() || !mDistances || !toHowMany )
		return FLT_MAX;

	Waypoint2 targetWaypointForHeur;
	targetWaypointForHeur.Position = ( targetForHeur == 0 )?mWaypoints[to[0]].Position:*targetForHeur;

	BitSet inClosed;
	inClosed.Resize( mWaypoints.Size() );
	BitSet inOpen;
	inOpen.Resize( mWaypoints.Size() );

	PointerHeap<PathfindingNode> openNodes;
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	PathfindingNode* nodes = new PathfindingNode[ mWaypoints.Size() ];
	PathfindingNode* current;

	// Insert the starting node
	current = nodes + from;
	current->mCost = 0;
	current->mFromIndex = -1;
	current->mHeuristic = targetWaypointForHeur.Dist( mWaypoints[from] )*1.1f;
	openNodes.Insert( current );
	inOpen.Set( from );

	char endReached = 0;
	// While there are still nodes to visit
	while( openNodes.Size() )
	{
		current = openNodes.Remove();
		inOpen.Clear( current-nodes );
		inClosed.Set( current-nodes );

		// End reached?
		for( int i = 0; i < toHowMany; i++ )
			if( current-nodes == to[i] )
				endReached = 1;

		if( endReached || ( current->mCost > 1.2f*maxLenToTry ) )
			break;

		// For each neighbour of our node
		int searchCount = 0;
		int wpIndex;
		float dist;
		while( GetNextAvailableFrom( current-nodes, searchCount, wpIndex, dist ) != 0 )
		{
			if( mWaypoints[wpIndex].Disabled || ( bigMonsters && (!mWaypoints[wpIndex].BigMonsters) ) )
				continue;	// This one can't be accessed by a big monster or it may be disabled

			if( ( restrictToZone != -1 ) && ( mWaypoints[wpIndex].Set != restrictToZone ) )
				continue;	// This one is not in the zone we're restricted to

			if( path && !path->DistanceOk(dist) )
				continue;	// This one does not meet requirements as for the distances

			if( ( inClosed.Test( wpIndex ) || inOpen.Test( wpIndex ) ) )
			{
				if( nodes[wpIndex].mCost > current->mCost + dist )
				{
					nodes[wpIndex].mFromIndex = current - nodes;
					nodes[wpIndex].mCost = current->mCost + dist;
					inClosed.Clear( wpIndex );
					if( inOpen.Test( wpIndex ) )
					{
						// Adjust open node in heap
						openNodes.Remove( nodes + wpIndex );
						openNodes.Insert( nodes + wpIndex );
					}
					else	// Add node to heap
					{
						openNodes.Insert( nodes + wpIndex );
						inOpen.Set( wpIndex );
					}
				}		// else node does not need changes
			}
			else	// new node; not in open or close
			{
				nodes[wpIndex].mFromIndex = current - nodes;
				nodes[wpIndex].mCost = current->mCost + dist;
				nodes[wpIndex].mHeuristic = targetWaypointForHeur.Dist( mWaypoints[wpIndex] )*1.1f;
				openNodes.Insert( nodes + wpIndex );
				inOpen.Set( wpIndex );
			}
		}
	}

	if( endReached )	// create path list(backward, like stack) and return it
	{
		float cost = current->mCost;
		if( path )
		{
			while( current->mFromIndex != -1 )
			{
				path->AddAtEnd( &(mWaypoints[current-nodes]) );
				current = nodes + current->mFromIndex;
			};
			path->AddAtEnd( &(mWaypoints[current-nodes]) );
		}
		delete[] nodes;
		return cost;
	}
	else
	{
		delete[] nodes;
		return FLT_MAX;
	}
}

int Pathfinder2::PickWaypoint( Viewport& viewport, int xm, int ym, float toleranceDistance, bool pickDisabled )
{
	POINT pt;
	pt.x = xm; pt.y = ym;

	float x = float(pt.x);
	float y = float(pt.y);

	// Compute the vector of the pick ray in screen space
	Vector v;
	v.X =  ( ( ( 2.0f * x ) / viewport.Width  ) - 1 ) / viewport.CameraToScreen.A00;
	v.Y = -( ( ( 2.0f * y ) / viewport.Height ) - 1 ) / viewport.CameraToScreen.A11;
	v.Z =  1.0f;

	// Transform the screen space pick ray into 3D space
	RaySegment ray;
	Matrix CameraToWorld = viewport.WorldToCamera.Inverse();
	ray.Dir    = CameraToWorld.TransformNormal(v);
	ray.Origin = CameraToWorld.GetTranslation();

	float dist = toleranceDistance;
	int waypointIndex = -1;
	float newDist;

	for( int i = 0; i < mWaypoints.Size(); ++i )
	{
		if( !pickDisabled && !SetEnabled( mWaypoints[i].Set ) ) continue;
		newDist = ray.DistanceFromPoint( mWaypoints[i].Position );
		if( newDist < dist )
		{
			dist = newDist;
			waypointIndex = i;
		};
	};

	return waypointIndex;
}

void Pathfinder2::RemoveConnection( int i, int j )
{
	int size = mWaypoints.Size();

	if( !size || !mDistances ) return;

	int start = mWaypoints[i].Base;
	int end = start + mWaypoints[i].Count;

	for( int a = start; a < end; ++a )
	{
		if( mPackedIndices[a] == j )
		{
			mHowManyDistances--;
#ifdef TRANSGAMING_PSP
            m_setTag("AI");
#endif
			float* newDist = new float[mHowManyDistances];
#ifdef TRANSGAMING_PSP
            m_setTag("AI");
#endif
			int* newInd = new int[mHowManyDistances];

			for( int k = 0; k < a; ++k )
			{
				newDist[k] = mDistances[k];
				newInd[k] = mPackedIndices[k];
			}

			for( int k = a; k < mHowManyDistances; ++k )
			{
				newDist[k] = mDistances[k+1];
				newInd[k] = mPackedIndices[k+1];
			}

			delete[] mDistances;		mDistances = newDist;
			delete[] mPackedIndices;	mPackedIndices = newInd;

			mWaypoints[i].Count--;
			for( int k = i+1; k < size; ++k )
				mWaypoints[k].Base--;

			ClearAutomaticStructures();
			CalculateBorder(i);
			CalculateBorder(j);
			return;
		}
	}
}

#pragma optimize( "g", off )

void Pathfinder2::AddConnection( int i, int j )
{
	int size = mWaypoints.Size();

	if( ( size <= i ) || ( size <= j ) ) return;

	int a = 0;
	int k;

	// Check if connection exists
	for( a = mWaypoints[i].Base; a < mWaypoints[i].Base + mWaypoints[i].Count; ++a )
		if( mPackedIndices[a] == j )
			return;

	mHowManyDistances++;
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	float* newDist = new float[mHowManyDistances];
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	int* newInd = new int[mHowManyDistances];

	for( k = 0; k < a; ++k )
	{
		newDist[k] = mDistances[k];
		newInd[k] = mPackedIndices[k];
	};

	for( k = mHowManyDistances-1; k > a; --k )
	{
		newDist[k] = mDistances[k-1];
		newInd[k] = mPackedIndices[k-1];
	};

	newDist[a] = (mWaypoints[i].Position - mWaypoints[j].Position).Magnitude();
	newInd[a] = j;

	if( mDistances ) delete[] mDistances;
	mDistances = newDist;

	if( mPackedIndices ) delete[] mPackedIndices;
	mPackedIndices = newInd;

	mWaypoints[i].Count++;
	for( int l = i+1; l < size; ++l )
		mWaypoints[l].Base++;

	CalculateBorder(i);
	CalculateBorder(j);
	ClearAutomaticStructures();
}

#pragma optimize( "", on )

#ifdef TRANSGAMING_USE_EDITOR
int Pathfinder2::ConnectionCollidingWithGeometry( const Vector& source, const Vector& destination )
{
	RaySegment ray;
	ray.Dir    = destination - source;
	ray.Origin = source;
	float len = ray.Dir.Magnitude();
	ray.Dir.Normalize();

#ifdef	TRANSGAMING
	for( Entity* entity = GEngine->pcfWorld->EntityList; entity; entity = entity->Next )
#else
	for( Entity* entity = GEngine->World->EntityList; entity; entity = entity->Next )
#endif
	{
		WorldMesh* ptr = entity->AsMesh();
		if( !ptr ) continue;

		WorldMesh& mesh = *ptr;

		if( ray.Intersect( mesh.Box ) )
		{
#ifdef TRANSGAMING_DEINDEX
            if (!mesh.isIndexed){
                if (mesh.PrimType == RenderDevice::EPrimTriList){
                    const int triCount = mesh.VertList.Size() / 3;

			        for( int j = 0; j < triCount; ++j )
			        {
				        const Vector v0 = mesh.VertList.getVertPos(j * 3 + 0);
				        const Vector v1 = mesh.VertList.getVertPos(j * 3 + 1);
				        const Vector v2 = mesh.VertList.getVertPos(j * 3 + 2);
				        float d;
				        if( ray.IntersectTriangle(v0,v1,v2,d ) && (d > 0 ) && ( d < len ) ) 
					        return 1;
			        }
                }

                else{
                    const int triCount = mesh.VertList.Size() - 2;

			        for( int j = 0; j < triCount; ++j )
			        {
				        const Vector v0 = mesh.VertList.getVertPos(j + 0);
				        const Vector v1 = mesh.VertList.getVertPos(j + 1);
				        const Vector v2 = mesh.VertList.getVertPos(j + 2);
				        float d;
				        if( ray.IntersectTriangle(v0,v1,v2,d ) && (d > 0 ) && ( d < len ) ) 
					        return 1;
			        }
                }
            }

            else{
#endif
			    const int triCount = mesh.Triangles.Size()/3;
			    for( int j = 0; j < triCount; ++j )
			    {
				    const WORD e0 = mesh.Triangles[j*3+0];
				    const WORD e1 = mesh.Triangles[j*3+1];
				    const WORD e2 = mesh.Triangles[j*3+2];

				    const Vector v0 = mesh.VertList.getVertPos(e0);
				    const Vector v1 = mesh.VertList.getVertPos(e1);
				    const Vector v2 = mesh.VertList.getVertPos(e2);
				    float d;
				    if( ray.IntersectTriangle(v0,v1,v2,d ) && (d > 0 ) && ( d < len ) ) 
					    return 1;
			    }
#ifdef TRANSGAMING_DEINDEX
            }
#endif
		}
	}
	return 0;
}

void Pathfinder2::Selected_RemoveConnectionsCollidingWithGeometry( float threshold )
{
	int size = mWaypoints.Size();

	for( int i = 0; i < size; ++i )
	{
		if( !mWaypoints[i].Selected ) continue;
		for( int j = i+1; j < size; ++j )
		{
			if( !mWaypoints[j].Selected ) continue;
			bool a = Connected( i, j );
			bool b = Connected( j, i );
			if( !a && !b ) continue;

			Vector p1 = mWaypoints[i].Position;
			Vector p2 = mWaypoints[j].Position;

			if( !ConnectionCollidingWithGeometry( p1, p2 ) )
			{
				p1.X += threshold; p2.X += threshold;
				if( !ConnectionCollidingWithGeometry( p1, p2 ) )
				{
					p1.X -= 2*threshold; p2.X -= 2*threshold;
					if( !ConnectionCollidingWithGeometry( p1, p2 ) )
					{
						p1.X += threshold; p2.X += threshold;
						p1.Z += threshold; p2.Z += threshold;
						if( !ConnectionCollidingWithGeometry( p1, p2 ) )
						{
							p1.Z -= 2*threshold; p2.Z -= 2*threshold;
							if( !ConnectionCollidingWithGeometry( p1, p2 ) )
								continue;
						}
					}
				}
			}

			if( a ) RemoveConnection( i, j );
			if( b ) RemoveConnection( j, i );
		}
	}
}
#endif

void Pathfinder2::ClearContents( void )
{
	if( mDistances )
	{
		mHowManyDistances = 0;
		delete[] mDistances;
		mDistances = 0;
		delete[] mPackedIndices;
		mPackedIndices = 0;
	}

	mWaypoints.ClearArray();
	SortedWaypoints = true;
	ClearAutomaticStructures();
}

bool Pathfinder2::LoadContents( const char* filename, float scale, bool dontLoadPortals )
{
	if( !strlen( filename ) ) return false;

	FileObject* f = GFileMan.CreateFileReader( filename );
	if( !f ) return false;

	// Read number of waypoints
	int size;
	if( !f->Read( &size, sizeof(int) ) ) { delete f; return false; };

	ClearContents();
	SortedWaypoints = false;
	CalculatedBorders = false;
	UndoDisplacement.ClearArray();

	unsigned short maxUsedSetNumber = 0;

	// Read waypoints
	Waypoint2 wp;
	EnabledSets.ClearArray();
	for( int i = 0; i < size; ++i )
	{
		if( !wp.LoadFrom(f) ) { delete f; return false; };
		wp.Position *= scale;
		if( wp.Set > maxUsedSetNumber )
			maxUsedSetNumber = wp.Set;
		mWaypoints.Add( wp );
	}
	FirstUnusedSetNumber = maxUsedSetNumber+1;
	for( int i = 0; i < FirstUnusedSetNumber; ++i )
		EnabledSets.Add(true);

	// Read distances
	if( !f->Read( &mHowManyDistances, sizeof(int) ) )
	{
		mHowManyDistances = 0;
		delete f;
		return false;
	}

	if( mHowManyDistances )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mDistances = new float[mHowManyDistances];
		if( !f->Read( mDistances, sizeof(float)*mHowManyDistances ) )
		{
			mHowManyDistances = 0;
			delete[] mDistances;
			mDistances = 0;
			for( int i = 0; i < size; ++i )
			{
				mWaypoints[i].Base = 0;
				mWaypoints[i].Count = 0;
			}
			delete f;
			return false;
		}

		for( int i = 0; i < mHowManyDistances; i++ )
			mDistances[i] *= scale;

#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mPackedIndices = new int[mHowManyDistances];
		if( !f->Read( mPackedIndices, sizeof(int)*mHowManyDistances ) )
		{
			mHowManyDistances = 0;
			delete[] mDistances; mDistances = 0;
			delete[] mPackedIndices; mPackedIndices = 0;
			for( int i = 0; i < size; ++i )
			{
				mWaypoints[i].Base = 0;
				mWaypoints[i].Count = 0;
			}
			delete f;
			return false;
		}
	}

	// Read marker if portal connections are here
	bool are;
	if( !f->Read( &are, sizeof(bool) ) ) { delete f; return false; };

	if( !are || dontLoadPortals )
	{
		// Close file
		delete f;
		return true;
	}

	ClearAutomaticStructures();

	// Load sets
	int setsCount;
	if( !f->Read( &setsCount, sizeof(int) ) ) { delete f; return true; };

	Assert(FirstUnusedSetNumber == setsCount);
    Sets.ResizeArray(FirstUnusedSetNumber);

	for( int i = 0; i < FirstUnusedSetNumber; ++i )
	{
		Pathfinder2Set& set = Sets[i];

		int waypointsCount;
		if( !f->Read( &waypointsCount, sizeof(int) ) ) { ClearAutomaticStructures(); delete f; return true; };

		for( int j = 0; j < waypointsCount; ++j )
		{
			int wp;
			if( !f->Read( &wp, sizeof(int) ) ) { ClearAutomaticStructures(); delete f; return true; };
			set.Waypoints.Add( wp );
		}

		int portalIndicesCount;
		if( !f->Read( &portalIndicesCount, sizeof(int) ) ) { ClearAutomaticStructures(); delete f; return true; };

		for( int j = 0; j < portalIndicesCount; ++j )
		{
			int pi;
			if( !f->Read( &pi, sizeof(int) ) ) { ClearAutomaticStructures(); delete f; return true; };
			set.PortalIndices.Add( pi );
		}

		if( portalIndicesCount )
		{
#ifdef TRANSGAMING_PSP
            m_setTag("AI");
#endif
			set.PortalDistances = new float[portalIndicesCount*portalIndicesCount];
			if( !f->Read( set.PortalDistances, sizeof(float)*portalIndicesCount*portalIndicesCount ) ) { ClearAutomaticStructures(); delete f; return false; };
		}
	}

	// Load portals
	int portalsCount;
	if( !f->Read( &portalsCount, sizeof(int) ) ) { ClearAutomaticStructures(); delete f; return true; };

	for( int i = 0; i < portalsCount; ++i )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		Pathfinder2Portal* portal = new Pathfinder2Portal;

		int waypointsCount;
		if( !f->Read( &waypointsCount, sizeof(int) ) ) { delete portal; ClearAutomaticStructures(); delete f; return true; };

		for( int j = 0; j < waypointsCount; ++j )
		{
			int wp;
			if( !f->Read( &wp, sizeof(int) ) ) { delete portal; ClearAutomaticStructures(); delete f; return true; };
			portal->Waypoints.Add( wp );
		}

		if( !f->Read( &portal->LowerSetIndex, sizeof(int) ) ) { delete portal; ClearAutomaticStructures(); delete f; return true; };
		if( !f->Read( &portal->HigherSetIndex, sizeof(int) ) ) { delete portal; ClearAutomaticStructures(); delete f; return true; };
		if( !f->Read( &portal->Center.X, sizeof(float) ) ) { delete portal; ClearAutomaticStructures(); delete f; return true; };
		if( !f->Read( &portal->Center.Y, sizeof(float) ) ) { delete portal; ClearAutomaticStructures(); delete f; return true; };
		if( !f->Read( &portal->Center.Z, sizeof(float) ) ) { delete portal; ClearAutomaticStructures(); delete f; return true; };
		if( !f->Read( &portal->Width, sizeof(float) ) ) { delete portal; ClearAutomaticStructures(); delete f; return true; };
		if( !f->Read( &portal->BorderPoint1, sizeof(int) ) ) { delete portal; ClearAutomaticStructures(); delete f; return true; };
		if( !f->Read( &portal->BorderPoint2, sizeof(int) ) ) { delete portal; ClearAutomaticStructures(); delete f; return true; };

		Portals.Add( portal );
	}

	AutoValid = true;
	SortedWaypoints = true;
	CalculatedBorders = false;
//	CalculateBorders();

	// Close file
	delete f;
	return true;
}

bool Pathfinder2::MergeContents( const char* filename, float scale )
{
	if( !strlen( filename ) )
		return false;

	FileObject* f = GFileMan.CreateFileReader( filename );
	if( !f )
		return false;

	// Read number of waypoints
	int size;
	if( !f->Read( &size, sizeof(int) ) )
	{
		delete f;
		return false;
	}

	ClearAutomaticStructures();
	SortedWaypoints = false;
	CalculatedBorders = false;
	UndoDisplacement.ClearArray();

	int numOfWaypoints = mWaypoints.Size();
	unsigned short maxUsedSetNumber = 0;

	// Determine first unused set number
	unsigned short firstUnusedSetNumber = 0;
	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( mWaypoints[i].Set >= firstUnusedSetNumber )
			firstUnusedSetNumber = mWaypoints[i].Set+1;

	DynamicArray<Waypoint2> newWaypoints;

	// Read waypoints
	Waypoint2 wp;
	EnabledSets.ClearArray();
	for( int i = 0; i < size; ++i )
	{
		if( !wp.LoadFrom(f) ) { delete f; return false; };
		wp.Position *= scale;
		wp.Set += firstUnusedSetNumber;
		if( wp.Set > maxUsedSetNumber )
			maxUsedSetNumber = wp.Set;
		wp.Base += mHowManyDistances;
		wp.Selected = true;
		newWaypoints.Add( wp );
	}
	FirstUnusedSetNumber = maxUsedSetNumber+1;
	for( int i = 0; i < FirstUnusedSetNumber; ++i )
		EnabledSets.Add(true);

	// Read distances
	int howManyDistances;
	if( !f->Read( &howManyDistances, sizeof(int) ) )
	{
		delete f;
		return false;
	}

	if( howManyDistances )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		float* newDistances = new float[mHowManyDistances+howManyDistances];
		::memcpy( newDistances, mDistances, mHowManyDistances*sizeof(float) );
		if( !f->Read( newDistances+mHowManyDistances, sizeof(float)*howManyDistances ) )
		{
			delete[] newDistances;
			delete f;
			return false;
		}

		for( int i = mHowManyDistances; i < mHowManyDistances+howManyDistances; ++i )
			newDistances[i] *= scale;

#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		int* newPackedIndices = new int[mHowManyDistances+howManyDistances];
		::memcpy( newPackedIndices, mPackedIndices, mHowManyDistances*sizeof(int) );
		if( !f->Read( newPackedIndices+mHowManyDistances, sizeof(int)*howManyDistances ) )
		{
			delete[] newDistances;
			delete[] newPackedIndices;
			delete f;
			return false;
		}

		for( int i = mHowManyDistances; i < mHowManyDistances+howManyDistances; ++i )
			newPackedIndices[i] += numOfWaypoints;

		delete[] mDistances;
		mDistances = newDistances;
		delete[] mPackedIndices;
		mPackedIndices = newPackedIndices;
		mHowManyDistances += howManyDistances;
	}

	delete f;

	for( int i = 0; i < newWaypoints.Size(); ++i )
		mWaypoints.Add( newWaypoints[i] );

	return true;
}

bool Pathfinder2::SaveContents( const char* filename, float scale, bool generateAutoStructsFirst )
{
	if( generateAutoStructsFirst )
	{
		ClearAutomaticStructures();
		GenerateAutomaticStructures();
	}

	if( !strlen( filename ) ) return false;

	FileObject* f = GFileMan.CreateFileWriter( filename );
	if( !f ) return false;

	// Write waypoint number
	int size = mWaypoints.Size();
	if( !f->Write( &size, sizeof(int) ) ) { delete f; return false; };

	// Write waypoints
	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( !mWaypoints[i].SaveTo(f, scale ) )
		{
			delete f;
			return false;
		}

	// Write distances
	if( !f->Write( &mHowManyDistances, sizeof(int) ) ) { delete f; return false; };

#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	float* t = new float[mHowManyDistances];
	::memcpy(t, mDistances, mHowManyDistances*sizeof(float) );
	for( int i = 0; i < mHowManyDistances; ++i ) t[i] *= scale;
	if( !f->Write( t, sizeof(float)*mHowManyDistances ) ) { delete[] t; delete f; return false; };
	delete[] t;

	if( !f->Write( mPackedIndices, sizeof(int)*mHowManyDistances ) ) { delete f; return false; };

	// Write marker if portal connections are here
	if( !f->Write( &AutoValid, sizeof(bool) ) ) { delete f; return false; };

	if( AutoValid )
	{
		// Save sets
		int setsCount = Sets.Size();
		if( !f->Write( &setsCount, sizeof(int) ) ) { delete f; return false; };

		for( int i = 0; i < setsCount; ++i )
		{
			Pathfinder2Set& set = Sets[i];

			int waypointsCount = set.Waypoints.Size();
			if( !f->Write( &waypointsCount, sizeof(int) ) ) { delete f; return false; };
			if( waypointsCount )
				if( !f->Write( (int*)&(set.Waypoints[0]), sizeof(int)*waypointsCount ) ) { delete f; return false; };

			int portalIndicesCount = set.PortalIndices.Size();
			if( !f->Write( &portalIndicesCount, sizeof(int) ) ) { delete f; return false; };
			if( portalIndicesCount )
				if( !f->Write( (int*)&(set.PortalIndices[0]), sizeof(int)*portalIndicesCount ) ) { delete f; return false; };

			if( portalIndicesCount )
				if( !f->Write( set.PortalDistances, sizeof(float)*portalIndicesCount*portalIndicesCount ) ) { delete f; return false; };
		}

		// Save portals
		int portalsCount = Portals.Size();
		if( !f->Write( &portalsCount, sizeof(int) ) ) { delete f; return false; };

		for( int i = 0; i < portalsCount; ++i )
		{
			Pathfinder2Portal* portal = Portals[i];

			int waypointsCount = portal->Waypoints.Size();
			if( !f->Write( &waypointsCount, sizeof(int) ) ) { delete f; return false; };
			if( waypointsCount )
				if( !f->Write( (int*)&(portal->Waypoints[0]), sizeof(int)*waypointsCount ) ) { delete f; return false; };

			if( !f->Write( &portal->LowerSetIndex, sizeof(int) ) ) { delete f; return false; };
			if( !f->Write( &portal->HigherSetIndex, sizeof(int) ) ) { delete f; return false; };
			if( !f->Write( &portal->Center.X, sizeof(float) ) ) { delete f; return false; };
			if( !f->Write( &portal->Center.Y, sizeof(float) ) ) { delete f; return false; };
			if( !f->Write( &portal->Center.Z, sizeof(float) ) ) { delete f; return false; };
			if( !f->Write( &portal->Width, sizeof(float) ) ) { delete f; return false; };
			if( !f->Write( &portal->BorderPoint1, sizeof(int) ) ) { delete f; return false; };
			if( !f->Write( &portal->BorderPoint2, sizeof(int) ) ) { delete f; return false; };
		}
	}

	delete f;
	return true;
}

int Pathfinder2::GetWaypointByPosition( Vector& position )
{
	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( position == mWaypoints[i].Position )
			return i;
	return -1;
}

void Pathfinder2::ImportFromOldPathfinder( void )
{
#ifdef	TRANSGAMING
	if( !GEngine || !GEngine->pcfWorld ) return;

	Pathfinder& oldPF = GEngine->pcfWorld->Paths;
#else
	if( !GEngine || !GEngine->World ) return;

	Pathfinder& oldPF = GEngine->World->Paths;
#endif

	ClearContents();

	GLog.Print( "Importing waypoints from No Zones Waypoint Set\n" );
	ImportFromOldPathfinderFromWpSet( &oldPF.mNoZonesWaypointSet, 0 );

	GLog.Print( "There are %d zones to import from\n", oldPF.mHowManyWaypointSets );
	for( int i = 0; i < oldPF.mHowManyWaypointSets; ++i )
		ImportFromOldPathfinderFromWpSet( &oldPF.mWaypointSets[i], i+1 );

	FirstUnusedSetNumber = oldPF.mHowManyWaypointSets;
	EnabledSets.ClearArray();
	for( int i = 0; i < FirstUnusedSetNumber+1; ++i )
	EnabledSets.Add(true);

	SortedWaypoints = false;
	CalculatedBorders = false;

	GLog.Print( "Making sure all paths are two-way\n" );
	AssertTwoWayPaths();
	CalculateBorders();
	ClearAutomaticStructures();
	ValidateSets();
	GLog.Print( "Cleaned, finished.\n" );
}

void Pathfinder2::ImportFromOldPathfinderFromWpSet( WaypointSet* wpset, int set )
{
	GLog.Print( "Importing waypoints from zone %d\n", set );

	// 1. Import waypoints and set mapping
	if( !wpset->mWaypoints.Size() ) return;

	int bothSizes = mWaypoints.Size()+wpset->mWaypoints.Size();

#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	int* indexMapping = new int[wpset->mWaypoints.Size()];
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	int* reverseMapping = new int[bothSizes];
	for( int j = 0; j < bothSizes; ++j )
		reverseMapping[j] = -1;

	GLog.Print( " - Importing waypoints\n" );

	for( int j = 0; j < wpset->mWaypoints.Size(); ++j )
	{
		Waypoint& wp = wpset->mWaypoints[j];
		int index = GetWaypointByPosition( wp.mPosition );
		if( index != -1 )
		{
			indexMapping[j] = index;
			reverseMapping[index] = j;
		}
		else
		{
			Waypoint2 wp2;
			wp2.Position = wp.mPosition;
			wp2.Disabled = ( wpset->IsDisabled(j) != 0 );
			wp2.BigMonsters = ( wpset->mBigMonsters.Test(j) != 0 );
			wp2.Selected = false;
			wp2.Border = false;
			wp2.Set = set;
			indexMapping[j] = AddWaypoint( wp2 );
			reverseMapping[indexMapping[j]] = j;
		}
	}

	GLog.Print( " - Importing connections\n" );
	GLog.Print( "   - Making list of connections to import\n" );

	// 2. Import connections - they have to be sorted from lowest to highest 'from'
	DynamicArray<int> from;
	DynamicArray<int> to;
	int searchPtr = 0;
	float dist;
	for( int j = 0; j < bothSizes; ++j )
	{
		if( reverseMapping[j] == -1 ) continue;

		searchPtr = 0;
		while( wpset->GetNextAvailableFrom( reverseMapping[j], searchPtr, dist ) )
		{
			from.Add( j );
			to.Add( indexMapping[searchPtr-1] );
		}
	}

	delete[] indexMapping;
	delete[] reverseMapping;

	GLog.Print( "   - Mass importing connections\n" );

	MassAddConnection( from, to );

	SortedWaypoints = false;
	ClearAutomaticStructures();
}

bool Pathfinder2::LoadFloors( const char* filename, float scale )
{
#ifdef FLOORS_ENABLED
	if( !strlen( filename ) ) return false;

#ifdef TRANSGAMING
	FILE* f = TNG_fopen(filename, "rb" );
#else
	FILE* f = fopen(filename, "rb" );
#endif
	if( !f ) return false;

	bool ok = Floors.Load( f, scale );
	if( !ok )
		Floors.ClearContents();

#ifdef TRANSGAMING
	TNG_fclose(f);
#else
	fclose(f);
#endif
	return ok;
#else
	return true;
#endif
}

bool Pathfinder2::SaveFloors( const char* filename, float scale )
{
#ifdef FLOORS_ENABLED
	if( !strlen( filename ) ) return false;

#ifdef TRANSGAMING
	FILE* f = TNG_fopen(filename, "wb" );
#else
	FILE* f = fopen(filename, "wb" );
#endif
	if( !f ) return false;

	bool ok = Floors.Save( f, scale );

#ifdef TRANSGAMING
	TNG_fclose(f);
#else
	fclose(f);
#endif
	return ok;
#else
	return true;
#endif
}

int Pathfinder2::Selection( Viewport& viewport, float toleranceDistance, RECT* rect, bool select )
{
	if( rect == NULL )
	{
		Select_All( select );
		return select?0:-1;
	}
	else if( ( rect->left == rect->right ) && ( rect->top == rect->bottom ) )
	{
		int wp = PickWaypoint( viewport, rect->left, rect->top, toleranceDistance );
		if( wp != -1 )
			Select( wp, select );
		return select?wp:-1;
	}
	// else

	Matrix WTC = viewport.WorldToCamera;

	int sel = -1;

	for( int i = 0; i < mWaypoints.Size(); i++ )
	{
		Vector newVec = viewport.WorldToScreen.TransformCoord( mWaypoints[i].Position );
		if( newVec.Z <= 0 )
			continue;

		newVec = viewport.WorldToScreen.Transform( mWaypoints[i].Position );

		newVec.X = ( newVec.X + 1 ) * viewport.Width / 2;
		newVec.Y = ( -newVec.Y + 1 ) * viewport.Height / 2;

		int x = (int)(newVec.X);
		int y = (int)(newVec.Y);

		if( ( x >= rect->left ) && ( x < rect->right ) &&
			( y >= rect->top ) && ( y < rect->bottom ) )
		{
			Select( i, select );
			if( select )
				sel = i;
		}
	}
	return sel;
}

void Pathfinder2::SelectedFloors_AddGrid( float step )
{
#ifdef FLOORS_ENABLED
	DynamicArray<Vector> newPoints;
	Floors.Selected_MakeGrid( step, newPoints );

	for( int i = 0; i < newPoints.Size(); ++i )
	{
		Waypoint2 wp;
		wp.Position = newPoints[i];
		wp.BigMonsters = false;
		wp.Disabled = false;
		wp.Selected = true;
		wp.Border = false;
		wp.Set = GetValidCurrentSet();
		AddWaypoint( wp );
	}
	UndoDisplacement.ClearArray();
#endif
}

void Pathfinder2::Select( int index, bool select )
{
	Assert( index >= 0 );
	if( !( select && !SetEnabled(mWaypoints[index].Set) ) )
		mWaypoints[index].Selected = select;
	UndoDisplacement.ClearArray();
}

void Pathfinder2::Select_Invert( void )
{
	for( int i = 0; i < mWaypoints.Size(); ++i )
	{
		if( !SetEnabled( mWaypoints[i].Set ) )
			mWaypoints[i].Selected = false;
		else
			mWaypoints[i].Selected = !mWaypoints[i].Selected;
	}
	UndoDisplacement.ClearArray();
}

void Pathfinder2::Select_OnSelectedFloors( bool select )
{
#ifdef FLOORS_ENABLED
	int floor = 0;
	Pathfinder2Floor* fl;
	while( ( fl = Floors.GetNextSelectedFloor( floor ) ) != 0 )
		for( int j = 0; j < mWaypoints.Size(); ++j )
		{
			Vector v = mWaypoints[j].Position;
			if( fl->InsideFloor( v ) )
				Select( j, select );
		}
#endif
}

#ifdef TRANSGAMING_USE_EDITOR
void Pathfinder2::Selected_MoveToCurrentSet( void )
{
	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( mWaypoints[i].Selected )
			mWaypoints[i].Set = GetValidCurrentSet();
	ClearAutomaticStructures();
	CalculatedBorders = false;
}
#endif

void Pathfinder2::ExpandCurrentSet( void )
{
	DynamicArray<int> indicesToAdd;

	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( mWaypoints[i].Set == CurrentSet )
		{
			int searchCount = 0;
			int wpIndex;
			float dist;
			while( GetNextAvailableFrom( i, searchCount, wpIndex, dist ) != 0 )
				if( mWaypoints[wpIndex].Set != CurrentSet )
					indicesToAdd.AddUnique( wpIndex );
		}

	for( int i = 0; i < indicesToAdd.Size(); ++i )
		mWaypoints[indicesToAdd[i]].Set = CurrentSet;
	ClearAutomaticStructures();
	CalculatedBorders = false;
	ValidateSets();
}

void Pathfinder2::ContractCurrentSet( void )
{
	DynamicArray<int> indicesToAdd;
	DynamicArray<int> setsToChangeTo;

	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( mWaypoints[i].Set == CurrentSet )
		{
			int searchCount = 0;
			int wpIndex;
			float dist;
			while( GetNextAvailableFrom( i, searchCount, wpIndex, dist ) != 0 )
				if( mWaypoints[wpIndex].Set != CurrentSet )
				{
					indicesToAdd.Add( i );
					setsToChangeTo.Add( mWaypoints[wpIndex].Set );
					break;
				}
		}

	for( int i = 0; i < indicesToAdd.Size(); ++i )
		mWaypoints[indicesToAdd[i]].Set = setsToChangeTo[i];
	ClearAutomaticStructures();
	CalculatedBorders = false;
	ValidateSets();
}

void Pathfinder2::GetCurrentSetFromSelected( void )
{
	bool currentGot = false;
	int current = FirstUnusedSetNumber;

	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( mWaypoints[i].Selected )
		{
			if( !currentGot )
			{
				currentGot = true;
				current = mWaypoints[i].Set;
			}
			else if( current != mWaypoints[i].Set )
			{
				CurrentSet = FirstUnusedSetNumber;
				return;
			}
		}
	CurrentSet = current;
}

void Pathfinder2::MakeNewSetFromSelected( void )
{
	unsigned short ourNumber = FirstUnusedSetNumber++;
	EnabledSets.Add(true);
	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( mWaypoints[i].Selected )
			mWaypoints[i].Set = ourNumber;
	ClearAutomaticStructures();
	CalculatedBorders = false;
}

void Pathfinder2::ValidateSet( unsigned short set )
{
	DynamicArray<int> setIndices;

	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( mWaypoints[i].Set == set )
			setIndices.Add( i );

	bool firstSet = true;
	bool toClear = false;

	while( setIndices.Size() )
	{
		if( !firstSet )
			toClear = true;

		DynamicArray<int> openWP;
		DynamicArray<int> closedWP;

		openWP.Add( setIndices[0] );
		setIndices.FastRemove( 0 );

		while( openWP.Size() )
		{
			int searchCount = 0;
			int wpIndex;
			float dist;
			while( GetNextAvailableFrom( openWP[0], searchCount, wpIndex, dist ) != 0 )
			{
				if( mWaypoints[wpIndex].Set == set )
				{
					int ind = setIndices.Find( wpIndex );
					if( ind != -1 )
					{
						openWP.Add( wpIndex );
						setIndices.FastRemove( ind );
					}
				}
			}
			closedWP.Add( openWP[0] );
			openWP.FastRemove(0);
		}

		if( !firstSet )
		{
			for( int i = 0; i < closedWP.Size(); ++i )
				mWaypoints[closedWP[i]].Set = FirstUnusedSetNumber;
			FirstUnusedSetNumber++;
			EnabledSets.Add(true);
		}

		firstSet = false;
	}

	if( toClear )
	{
	    ClearAutomaticStructures();
		CalculatedBorders = false;
	}
}

void Pathfinder2::MergeSetsFromSelected( void )
{
	DynamicArray<int> selectedSets;

	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( mWaypoints[i].Selected )
			selectedSets.AddUnique( mWaypoints[i].Set );

	if( selectedSets.Size() <= 1 )
		return;

	for( int i = 0; i < mWaypoints.Size(); ++i )
		for( int j = 1; j < selectedSets.Size(); ++j )
			if( mWaypoints[i].Set == selectedSets[j] )
			{
				mWaypoints[i].Set = selectedSets[0];
				break;
			}
	ClearAutomaticStructures();
	CalculatedBorders = false;
}

void Pathfinder2::ValidateSets( void )
{
#ifdef LOG_PATHFINDER_ACTIONS
	GLog.Print( "Validating sets..." );
#endif

	// Determine first unused set number
	FirstUnusedSetNumber = 0;
	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( mWaypoints[i].Set >= FirstUnusedSetNumber )
			FirstUnusedSetNumber = mWaypoints[i].Set+1;

	for( int i = 0; i < FirstUnusedSetNumber; ++i )
		ValidateSet( i );

	if( FirstUnusedSetNumber <= 1 )
		return;

#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	int* setNumbers = new int[FirstUnusedSetNumber];

	while( true )
	{
		for( int i = 0; i < FirstUnusedSetNumber; ++i )	setNumbers[i] = 0;
		for( int i = 0; i < mWaypoints.Size(); ++i )	setNumbers[mWaypoints[i].Set]++;

		// Find last full
		int lastFull;
		for( lastFull = FirstUnusedSetNumber-1; lastFull >= 0; --lastFull )
			if( setNumbers[lastFull] != 0 )
				break;

		// Adjust other structs to it
		while( lastFull < EnabledSets.Size()-1 ) EnabledSets.RemoveLast();
		FirstUnusedSetNumber = lastFull+1;
		if( CurrentSet > FirstUnusedSetNumber ) CurrentSet = FirstUnusedSetNumber;

		// Find first empty
		int firstEmpty;
		for( firstEmpty = 0; firstEmpty < FirstUnusedSetNumber; ++firstEmpty )
			if( setNumbers[firstEmpty] == 0 )
				break;

		// Check if there's something to change
		if( firstEmpty == FirstUnusedSetNumber )	break;

		// Change it
		for( int i = 0; i < mWaypoints.Size(); ++i )
			if( mWaypoints[i].Set == lastFull )
				mWaypoints[i].Set = firstEmpty;

		// Update structs after change
		if( CurrentSet == lastFull ) CurrentSet = firstEmpty;
		if( CurrentSet > FirstUnusedSetNumber ) CurrentSet = FirstUnusedSetNumber;
		EnabledSets[firstEmpty] = EnabledSets[lastFull];
		EnabledSets.RemoveLast();
		FirstUnusedSetNumber--;
	}
	ClearAutomaticStructures();

#ifdef LOG_PATHFINDER_ACTIONS
	GLog.Print( "validated.\n" );
#endif
}

void Pathfinder2::ClearAutomaticStructures( void )
{
	if( AutoValid )
	{
		AutoValid = false;
		Sets.ClearArray();
		for( int i = 0; i < Portals.Size(); ++i )
			delete Portals[i];
		Portals.ClearArray();
	}
}

void Pathfinder2::GenerateAutomaticStructures( void )
{
	if( AutoValid ) return;	// All is ok
	SortWaypoints();
	RecalculateAllDistances();
	AssertTwoWayPaths();
	ValidateSets();

#ifdef LOG_PATHFINDER_ACTIONS
	GLog.Print( "Generating automatic structures..." );
#endif

	// Generate sets
    Sets.ResizeArray(FirstUnusedSetNumber);

	for( int i = 0; i < FirstUnusedSetNumber; ++i )
	{
		for( int j = 0; j < mWaypoints.Size(); ++j )
			if( mWaypoints[j].Set == i )
				Sets[i].Waypoints.Add(j);
		Sets[i].PortalDistances = 0;
	}

	// Generate portals
	for( int i = 0; i < FirstUnusedSetNumber; ++i )
	{
		Pathfinder2Set& set = Sets[i];
		for( int j = i+1; j < FirstUnusedSetNumber; ++j )
		{
			DynamicArray<int> ConnectionStart;
			DynamicArray<int> ConnectionEnd;

			for( int k = 0; k < set.Waypoints.Size(); ++k )
			{
				int searchCount = 0;
				int wpIndex;
				float dist;
				while( GetNextAvailableFrom( set.Waypoints[k], searchCount, wpIndex, dist ) != 0 )
					if( mWaypoints[wpIndex].Set == j )
					{
						ConnectionStart.Add( set.Waypoints[k] );
						ConnectionEnd.Add( wpIndex );
					}
			}

			while( ConnectionStart.Size() )
			{
#ifdef TRANSGAMING_PSP
                m_setTag("AI");
#endif
				Pathfinder2Portal* portal = new Pathfinder2Portal;
				portal->LowerSetIndex = i;
				portal->HigherSetIndex = j;

				// Generate a set of points from it ( they have to be connected on both sides )
				GeneratePortal( ConnectionStart[0], ConnectionEnd[0], portal->Waypoints );

				// Add this set as portal to the portal list
				int portalIndex = Portals.Add( portal );

				// Generate additional portal data, such as portal center and portal width and portal most border indices
				// - Find two most distant points in the portal
				float dist = 0;
				int bp1 = -1;
				int bp2 = -1;
				for( int k = 0; k < portal->Waypoints.Size(); ++k )
					for( int l = k+1; l < portal->Waypoints.Size(); ++l )
					{
						float ms = (mWaypoints[portal->Waypoints[k]].Position-mWaypoints[portal->Waypoints[l]].Position).MagnitudeSquared();
						if( ms > dist )
						{
							dist = ms;
							bp1 = k;
							bp2 = l;
						}
					}
				portal->BorderPoint1 = portal->Waypoints[bp1];
				portal->BorderPoint2 = portal->Waypoints[bp2];
				portal->Center = mWaypoints[portal->BorderPoint1].Position + mWaypoints[portal->BorderPoint2].Position / 2.0f;
				portal->Width = ( mWaypoints[portal->BorderPoint1].Position - mWaypoints[portal->BorderPoint2].Position ).Magnitude();

				// Remove connections from portal from the list of all connections from this set
				ConnectionStart.FastRemove(0);
				ConnectionEnd.FastRemove(0);

				for( int k = 0; k < ConnectionStart.Size(); ++k )
					if( ( portal->Waypoints.Find( ConnectionStart[k] ) != -1 ) && ( portal->Waypoints.Find( ConnectionEnd[k] ) != -1 ) )
					{
						ConnectionStart.FastRemove(k);
						ConnectionEnd.FastRemove(k);
						k--;
					}

				// Add portal to both sets as info
				set.PortalIndices.Add( portalIndex );
				Sets[j].PortalIndices.Add( portalIndex );
			}
		}
	}

	// Find best paths from portal to portal in each set
	for( int i = 0; i < FirstUnusedSetNumber; ++i )
	{
		Pathfinder2Set& set = Sets[i];
		if( !set.PortalIndices.Size() )
			set.PortalDistances = 0;

#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		set.PortalDistances = new float[set.PortalIndices.Size()*set.PortalIndices.Size()];
		// indexed [from*size+to]

		for( int j = 0; j < set.PortalIndices.Size(); ++j )		// from
		{
			Pathfinder2Portal* startPortal = Portals[set.PortalIndices[j]];
			for( int k = 0; k < set.PortalIndices.Size(); ++k )	// to
			{
				if( k == j )
				{
					set.PortalDistances[j*set.PortalIndices.Size()+k] = FLT_MAX;
					continue;
				}

				Pathfinder2Portal* endPortal = Portals[set.PortalIndices[k]];

				// Determine "from"
				float dist = FLT_MAX;
				int waypointClosestToCenter = -1;
				for( int l = 0; l < startPortal->Waypoints.Size(); ++l )
				{
					float ms = (mWaypoints[startPortal->Waypoints[l]].Position-startPortal->Center).MagnitudeSquared();
					if( ms < dist )
					{
						dist = ms;
						waypointClosestToCenter = l;
					}
				}

				// "to" is a set of target portal waypoints, distance is measured to center

				if( ( waypointClosestToCenter == -1 ) || !endPortal->Waypoints.Size() )
				{
					set.PortalDistances[j*set.PortalIndices.Size()+k] = FLT_MAX;
					continue;
				}

				set.PortalDistances[j*set.PortalIndices.Size()+k] = GetShortestPath(
					startPortal->Waypoints[waypointClosestToCenter], (int*)&(endPortal->Waypoints[0]), endPortal->Waypoints.Size(),
					0, false, &endPortal->Center, FLT_MAX, i );
			}
		}
	}

	AutoValid = true;

#ifdef LOG_PATHFINDER_ACTIONS
	GLog.Print( "generated.\n" );
#endif
}

void Pathfinder2::GeneratePortal( int startIndex, int endIndex, DynamicArray<int>& portalPoints )
{
	int startSet = mWaypoints[startIndex].Set;
	int endSet = mWaypoints[endIndex].Set;

	DynamicArray<int> forbiddenStartPoints;	// empty yet
	DynamicArray<int> forbiddenEndPoints;	// empty yet
	DynamicArray<int> startPoints;
	DynamicArray<int> endPoints;

	DynamicArray<int> openWP;				// temp for set generation
	bool forbiddenChanged;

	do
	{
		forbiddenChanged = false;
		startPoints.ClearArray();
		endPoints.ClearArray();

		// Generate a set of start points
		openWP.Add( startIndex );
		while( openWP.Size() )
		{
			int searchCount = 0;
			int wpIndex;
			float dist;
			bool bordered = false;
			while( GetNextAvailableFrom( openWP[0], searchCount, wpIndex, dist ) != 0 )
				if( mWaypoints[wpIndex].Set == endSet )
					bordered = true;

			if( !bordered )
			{
				openWP.FastRemove(0);
				continue;
			}

			searchCount = 0;
			while( GetNextAvailableFrom( openWP[0], searchCount, wpIndex, dist ) != 0 )
				if( ( mWaypoints[wpIndex].Set == startSet ) &&
					( startPoints.Find( wpIndex ) == -1 ) &&
					( forbiddenStartPoints.Find( wpIndex ) == -1 ) )
					openWP.AddUnique( wpIndex );

			startPoints.Add( openWP[0] );
			openWP.FastRemove(0);
		}

		// Generate a set of end points
		openWP.Add( endIndex );
		while( openWP.Size() )
		{
			int searchCount = 0;
			int wpIndex;
			float dist;
			bool bordered = false;
			while( GetNextAvailableFrom( openWP[0], searchCount, wpIndex, dist ) )
				if( mWaypoints[wpIndex].Set == startSet )
					bordered = true;

			if( !bordered )
			{
				openWP.FastRemove(0);
				continue;
			}

			searchCount = 0;
			while( GetNextAvailableFrom( openWP[0], searchCount, wpIndex, dist ) )
				if( ( mWaypoints[wpIndex].Set == endSet ) &&
					( endPoints.Find( wpIndex ) == -1 ) &&
					( forbiddenEndPoints.Find( wpIndex ) == -1 ) )
					openWP.AddUnique( wpIndex );

			endPoints.Add( openWP[0] );
			openWP.FastRemove(0);
		}

		// Check start points for validity
		for( int i = 0; i < startPoints.Size(); ++i )
		{
			int searchCount = 0;
			int wpIndex;
			float dist;
			bool borderedToEndSet = false;
			while( GetNextAvailableFrom( startPoints[i], searchCount, wpIndex, dist ) != 0 )
				if( ( mWaypoints[wpIndex].Set == endSet ) && ( endPoints.Find( wpIndex ) != -1 ) )
				{
					borderedToEndSet = true;
					break;
				}

			if( !borderedToEndSet )
			{
				forbiddenStartPoints.AddUnique( startPoints[i] );
				forbiddenChanged = true;
			}
		}

		// Check end points for validity
		for( int i = 0; i < endPoints.Size(); ++i )
		{
			int searchCount = 0;
			int wpIndex;
			float dist;
			bool borderedToStartSet = false;
			while( GetNextAvailableFrom( endPoints[i], searchCount, wpIndex, dist ) != 0 )
				if( ( mWaypoints[wpIndex].Set == startSet ) && ( startPoints.Find( wpIndex ) != -1 ) )
				{
					borderedToStartSet = true;
					break;
				}

			if( !borderedToStartSet )
			{
				forbiddenEndPoints.AddUnique( endPoints[i] );
				forbiddenChanged = true;
			}
		}
		// If anything has been added to forbidden, repeat the procedure
	}
	while( forbiddenChanged );

	for( int i = 0; i < startPoints.Size(); ++i )
		portalPoints.Add( startPoints[i] );

	for( int i = 0; i < endPoints.Size(); ++i )
		portalPoints.Add( endPoints[i] );
}

float Pathfinder2::GetShortestPortalPath( int from, int to, DynamicArray<int>& list, int startSetIndex, float noLongerThan )
{
	Assert( Portals.Size() );
	Vector fromCenter = Portals[from]->Center;
	Vector toCenter = Portals[to]->Center;

	BitSet inClosed;
	inClosed.Resize( Portals.Size() );
	BitSet inOpen;
	inOpen.Resize( Portals.Size() );

	PointerHeap<Pathfinder2AStarNode> openNodes;
    m_setTag("AI");
	Pathfinder2AStarNode* nodes = new Pathfinder2AStarNode[ Portals.Size() ];
	Pathfinder2AStarNode* current;

	// Insert the starting node
	current = nodes + from;
	current->Portal = Portals[from];
	current->Cost = 0;
	current->From = -1;
	current->LastSet = startSetIndex;
	current->Heuristic = ( fromCenter - toCenter ).Magnitude()*1.05f;
	openNodes.Insert( current );
	inOpen.Set( from );

	bool endReached = false;

	// While there are still nodes to visit
	while( openNodes.Size() )
	{
		current = openNodes.Remove();
		int currentNum = current-nodes;
		inOpen.Clear( currentNum );
		inClosed.Set( currentNum );

		// End reached?
		if( currentNum == to )
		{
			endReached = true;
			break;
		}

		// Max search len reached?
		if( current->Cost > 1.2f*noLongerThan )
			break;

		// For each neighbour of our node
		Pathfinder2AStarNode* searchPtr = 0;

		int setIndex = ( current->Portal->LowerSetIndex == current->LastSet )?current->Portal->HigherSetIndex:current->Portal->LowerSetIndex;
		Pathfinder2Set& set = Sets[setIndex];

		int portalInSet = -1;
		for( int i = 0; i < set.PortalIndices.Size(); ++i )
			if( set.PortalIndices[i] == currentNum )
			{
				portalInSet = i;
				break;
			}

		Assert( portalInSet != -1 );

		for( int i = 0; i < set.PortalIndices.Size(); ++i )
		{
			float dist = set.PortalDistances[portalInSet*set.PortalIndices.Size()+i];
			if( dist == FLT_MAX )	// no connection
				continue;

			int portalNum = set.PortalIndices[i];
			searchPtr = nodes + portalNum;

			if( ( inClosed.Test( portalNum ) || inOpen.Test( portalNum ) ) )
			{
				if( searchPtr->Cost > current->Cost + dist )
				{
					searchPtr->From = currentNum;
					searchPtr->LastSet = setIndex;
					searchPtr->Cost = current->Cost + dist;
					searchPtr->Heuristic = (searchPtr->Portal->Center - toCenter).Magnitude()*1.05f;
					inClosed.Clear( portalNum );
					if( inOpen.Test( portalNum ) )
					{
						// Adjust open node in heap
						openNodes.Remove( searchPtr );
						openNodes.Insert( searchPtr );
					}
					else	// Add node to heap
					{
						inOpen.Set( portalNum );
						openNodes.Insert( searchPtr );
					}
				}			// else node does not need changes
			}
			else			// new node; not in open or close
			{
				searchPtr->From = currentNum;
				searchPtr->Portal = Portals[portalNum];
				searchPtr->LastSet = setIndex;
				searchPtr->Cost = current->Cost + dist;
				searchPtr->Heuristic = (searchPtr->Portal->Center - toCenter).Magnitude()*1.05f;
				openNodes.Insert( searchPtr );
				inOpen.Set( portalNum );
			}
		}
	}

	if( endReached )	// create path list ( first node at end ) and return cost
	{
		float cost = current->Cost;
		while( current->From != -1 )
		{
			list.Add( current-nodes );
			current = nodes + current->From;
		}
		list.Add( current-nodes );
		delete[] nodes;
		return cost;
	}
	else
	{
		delete[] nodes;
		return 0;
	}
}

float Pathfinder2::GetShortestPath( Vector from, Vector to, WaypointGPath2& path )
{
	GenerateAutomaticStructures();

	path.Empty();
	path.mEndPoint = to;
	path.mPrecisePath = 0;
	path.mDistance = 0;
	path.mPosMinus1 = to;

	Vector toEnd = to-from;
	if( toEnd.Magnitude() < 0.0001f ) return 0;
	toEnd.Normalize();

	int profState = GProfiler.GetState();
	GProfiler.SetState( PATHFINDING );

	int closestToStart = GetIndexOfWaypointClosestTo( from );
	if( closestToStart == -1 ) { GProfiler.SetState( profState ); return 0; };
	int closestToEnd = GetIndexOfWaypointClosestTo( to );

	path.mEndPoint = to;
	path.mPrecisePath = 0;
	path.mDistance = FLT_MAX;

	int setFrom = mWaypoints[closestToStart].Set;
	int setTo = mWaypoints[closestToEnd].Set;
	Pathfinder2Set& startSet = Sets[setFrom];
	Pathfinder2Set& endSet = Sets[setTo];
	path.mCurrentSet = setFrom;

	// If waypoints in same set, check for path between them
	if( setFrom == setTo )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		WaypointPath2* onlyPath = new WaypointPath2( &path );
		float onlyDistance = GetShortestPath( closestToStart, &closestToEnd, 1, onlyPath, false, 0, FLT_MAX, setFrom );

		if( onlyPath->Size() )	// path found
		{
			path.mPrecisePath = onlyPath;
			path.mDistance = onlyDistance;
			path.mPosMinus1 = path.mPrecisePath->GetNextFromPath();
			GProfiler.SetState( profState );
			return path.mDistance;
		}
		else
			delete onlyPath;
	}

//	Find portals from starting zone. And immediately leading to the end zone, for special case.
	DynamicArray<Pathfinder2Portal*> immediatePortals;
	DynamicArray<Pathfinder2Portal*> startPortals;
	DynamicArray<int> startPortalIndices;

	for( int i = 0; i < startSet.PortalIndices.Size(); ++i )
	{
		Pathfinder2Portal* p = Portals[startSet.PortalIndices[i]];
		if( ( p->HigherSetIndex == setTo ) || ( p->LowerSetIndex == setTo ) )
			immediatePortals.Add( p );
		else
		{
			startPortals.Add( p );
			startPortalIndices.Add(startSet.PortalIndices[i]);
		}
	}

	float farthestLengthToSearch = FLT_MAX;
	WaypointPath2 immediatePath(&path);
	int shortestImmediatePath = -1;

	//-------------------------------------------------------------------------
	// Special case - first test paths going through one portal only, if u can
	//-------------------------------------------------------------------------

	if( immediatePortals.Size() )
	{
		// Get immediate portal closest by its sum of distances from start and end
		int closest = 0;
		float dist = (immediatePortals[0]->Center-from).Magnitude()+(immediatePortals[0]->Center-to).Magnitude();
		for( int i = 1; i < immediatePortals.Size(); ++i )
		{
			float dist2 = (immediatePortals[i]->Center-from).Magnitude()+(immediatePortals[i]->Center-to).Magnitude();
			if( dist2 < dist )
			{
				dist = dist2;
				closest = i;
			}
		}

		// Move closest to end of array
		Pathfinder2Portal* p = immediatePortals[closest];
		immediatePortals.FastRemove( closest );
		immediatePortals.Add( p );

		// Find best path in immediate portals
		for( int i = immediatePortals.Size()-1; i >= 0; --i )
		{
			WaypointPath2 tempPath(&path);
			Pathfinder2Portal* portal = immediatePortals[i];
			Vector target = CenterCalculation( portal, from, to );

			float startDistance = GetShortestPath( closestToStart, (int*)&(portal->Waypoints[0]), portal->Waypoints.Size(),
				&tempPath, false, &target, farthestLengthToSearch, setFrom );

			if( startDistance == FLT_MAX )
				continue;

			if( target == to )
				target = from;

			WaypointPath2 tempPath2(&path);
			float endDistance = GetShortestPath( closestToEnd, (int*)&(portal->Waypoints[0]), portal->Waypoints.Size(),
				&tempPath2, false, &target, farthestLengthToSearch, setTo );

			if( endDistance == FLT_MAX )
				continue;

			if( startDistance + endDistance < farthestLengthToSearch )
			{
				farthestLengthToSearch = startDistance + endDistance;
				shortestImmediatePath = i;
				immediatePath = tempPath;
			}
		}
	}

	// If we have some start portals, check longer paths
	float* startDistances = 0;
	WaypointPath2* startPaths = 0;
	float* endDistances = 0;
	WaypointPath2* endPaths = 0;
	DynamicArray<Pathfinder2Portal*> endPortals;
	DynamicArray<int> endPortalIndices;

	DynamicArray<int> shortestPortalPath;
	int shortestPathStartPortal = -1;
	int shortestPathEndPortal = -1;

	if( startPortals.Size() )
	{
		// Find portals from ending zone.
		for( int i = 0; i < endSet.PortalIndices.Size(); ++i )
		{
			int index = endSet.PortalIndices[i];
			endPortalIndices.Add(index);
			endPortals.Add( Portals[index] );
		}

		if( endPortals.Size() )
		{
			// All is ok - now calculate paths
#ifdef TRANSGAMING_PSP
            m_setTag("AI");
#endif
			startDistances = new float[startPortals.Size()];
			for( int i = 0; i < startPortals.Size(); ++i ) startDistances[i] = -1.0f;
#ifdef TRANSGAMING_PSP
            m_setTag("AI");
#endif
			startPaths = new WaypointPath2[startPortals.Size()];
			for( int i = 0; i < startPortals.Size(); ++i ) startPaths[i].SetDistances(&path);
#ifdef TRANSGAMING_PSP
            m_setTag("AI");
#endif
			endDistances = new float[endPortals.Size()];
			for( int i = 0; i < endPortals.Size(); ++i ) endDistances[i] = -1.0f;
#ifdef TRANSGAMING_PSP
            m_setTag("AI");
#endif
			endPaths = new WaypointPath2[endPortals.Size()];
			for( int i = 0; i < endPortals.Size(); ++i ) endPaths[i].SetDistances(&path);

			// Find distances and paths between pairs of portals (should be quite quick)
			float tempDist;
			DynamicArray<int> tempPortalPath;

			// This is only an approximate measurement, so we don't worry about center/border points here

			for( int i = 0; i < startPortals.Size(); ++i )
				for( int j = 0; j < endPortals.Size(); ++j )
				{
					tempPortalPath.ClearArray();
					tempDist = GetShortestPortalPath( startPortalIndices[i], endPortalIndices[j], tempPortalPath, setFrom, farthestLengthToSearch );
					if( !tempPortalPath.Size() || ( tempDist > farthestLengthToSearch ) )
						continue;

					if( startDistances[i] == -1.0f )
						startDistances[i] = GetShortestPath( closestToStart, (int*)&(startPortals[i]->Waypoints[0]),
							startPortals[i]->Waypoints.Size(), &startPaths[i], false, &startPortals[i]->Center, farthestLengthToSearch, setFrom );

					if( tempDist+startDistances[i] > farthestLengthToSearch )
						continue;

					if( endDistances[j] == -1.0f )
						endDistances[j] = GetShortestPath( closestToEnd, (int*)&(endPortals[j]->Waypoints[0]),
							endPortals[j]->Waypoints.Size(), &endPaths[j], false, &endPortals[j]->Center, farthestLengthToSearch, setTo );

					if( tempDist+startDistances[i]+endDistances[j] > farthestLengthToSearch )
						continue;

					// Update shortestPortalPath nad farthest length to search

					shortestImmediatePath = -1;	// immediates are no longer best
					shortestPortalPath = tempPortalPath;
					farthestLengthToSearch = tempDist+startDistances[i]+endDistances[j];
					shortestPathStartPortal = i;
					shortestPathEndPortal = j;
				}
		}
	}

	if( shortestImmediatePath != -1 )	// one of immediates was the best
	{
		path.mPortals.Add(immediatePortals[shortestImmediatePath]);
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		path.mPrecisePath = new WaypointPath2(immediatePath);
	}
	else if( shortestPathStartPortal != -1 )
	{
		for( int i = 0; i < shortestPortalPath.Size(); ++i )
			path.mPortals.Add( Portals[shortestPortalPath[i]] );
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		path.mPrecisePath = new WaypointPath2(startPaths[shortestPathStartPortal]);
	}

	// Clean up
	if( startPaths ) delete[] startPaths;
	if( startDistances ) delete[] startDistances;
	if( endPaths ) delete[] endPaths;
	if( endDistances ) delete[] endDistances;

	if( farthestLengthToSearch < FLT_MAX )
	{
		if( path.mPrecisePath->Size() > 1 )
			path.mPosMinus1 = path.mPrecisePath->GetNextFromPath();
		else
			path.mPosMinus1 = path.mPrecisePath->PeekNextFromPath();

		// Additional check for position on the line 1-2
		RaySegment ray;
		ray.Origin = path.mPosMinus1;
		Vector point2;
		if( path.PeekPos( 0, point2 ) )
		{
			if( ( point2 == ray.Origin ) && !path.PeekPos( 1, point2 ) )
				;	// Do nothing
			else
			{
				ray.Dir = point2 - ray.Origin;
				ray.Dir.Normalize();
				if( ray.DistanceFromPoint( from ) < 0.1f )
					path.Step();	// forget about the first point
			}
		}

		path.mDistance = farthestLengthToSearch;
		GProfiler.SetState( profState );
		return farthestLengthToSearch;
	}
	else
	{
		GProfiler.SetState( profState );
		return 0;
	}
}

int Pathfinder2::GetClosestInPortal( Pathfinder2Portal* portal, Vector& point, int setIndex )
{
	float dist = FLT_MAX;
	int chosen = -1;
	for( int i = 0; i < portal->Waypoints.Size(); ++i )
	{
		Waypoint2& wp = mWaypoints[portal->Waypoints[i]];

		if( ( setIndex != -1 ) && ( wp.Set != setIndex ) )
			continue;

		float dist2 = (wp.Position - point).MagnitudeSquared();

		if( dist2 < dist )
		{
			dist = dist2;
			chosen = i;
		}
	}
	Assert( chosen != -1 );
	return portal->Waypoints[chosen];
}

Vector Pathfinder2::CenterCalculation( Pathfinder2Portal* portal, Vector& from, Vector& to )
{
	Vector target;
	Vector& bp1 = mWaypoints[portal->BorderPoint1].Position;
	Vector& bp2 = mWaypoints[portal->BorderPoint2].Position;
	Vector FromToOne = bp1-from;
	Vector FromOneUp = FromToOne^(bp1-bp2);
	Vector OutOne = FromOneUp^FromToOne;
	Vector OneToTo = to-bp1;
	if( OneToTo.Dot( OutOne ) > 0 )
		target = bp1;
	else
	{
		Vector FromToTwo = bp2-from;
		Vector FromTwoUp = (bp2-bp1)^FromToTwo;
		Vector OutTwo = FromToTwo^FromTwoUp;
		Vector TwoToTo = to-bp2;
		if( TwoToTo.Dot( OutTwo ) > 0 )
			target = bp2;
		else
			target = to;
	}
	return target;
}

void Pathfinder2::SortWaypoints( void )
{
	if( SortedWaypoints ) return;
	if( !mWaypoints.Size() ) { SortedWaypoints = true ; return; };
	UndoDisplacement.ClearArray();

#ifdef LOG_PATHFINDER_ACTIONS
	GLog.Print( "Sorting waypoints..." );
#endif

	ClearAutomaticStructures();

	// 1. Create mapping from old waypoints to new waypoints
	DynamicArray<int> mapping;
	mapping.GrowArray( mWaypoints.Size() );

	DynamicArray<int> from;
	for( int i = 0; i < mWaypoints.Size(); ++i )
		from.Add(i);

	DynamicArray<Waypoint2> newWaypoints;
	newWaypoints.GrowArray( mWaypoints.Size() );

	for( int i = 0; i < mWaypoints.Size(); ++i )
	{
		float currentLowest = 100000.0;
		int currentWP = -1;
		for( int j = 0; j < from.Size(); ++j )
		{
			Waypoint2& wp = mWaypoints[from[j]];
			if( wp.Position.X < currentLowest )
			{
				currentLowest = wp.Position.X;
				currentWP = j;
			}
		}
		newWaypoints[i] = mWaypoints[from[currentWP]];
		mapping[from[currentWP]] = i;
		from.FastRemove(currentWP);
	}
	Assert( !from.Size() );
	mWaypoints = newWaypoints;

	// 2. For each waypoint connection - read it, convert and add to new table
	if( mDistances )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		float* mNewDistances = new float[mHowManyDistances];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		int* mNewPackedIndices = new int[mHowManyDistances];

		int distanceCounter = 0;

		for( int i = 0; i < mWaypoints.Size(); ++i )
		{
			int base = mWaypoints[i].Base;
			mWaypoints[i].Base = distanceCounter;
			for( int j = 0; j < mWaypoints[i].Count; ++j )
			{
				mNewDistances[distanceCounter] = mDistances[base+j];
				mNewPackedIndices[distanceCounter++] = mapping[mPackedIndices[base+j]];
			}
		}

		delete[] mDistances;		mDistances = mNewDistances;
		delete[] mPackedIndices;	mPackedIndices = mNewPackedIndices;
	}

	SortedWaypoints = true;

#ifdef LOG_PATHFINDER_ACTIONS
	GLog.Print( "sorted.\n" );
#endif
}

#ifdef TRANSGAMING_USE_EDITOR
void Pathfinder2::Render( int renderWaypoints, int renderFloors )
{
	LineRender& lineBuf = GEngine->Video->LineBuf;

	if( renderWaypoints )
	{
		CalculateBorders();
		lineBuf.DrawWaypointSet2();
	}
#ifdef FLOORS_ENABLED
	if( renderFloors )
		Floors.Render();
#endif
}

void Pathfinder2::GetColorsFromScript( void )
{
	unsigned int r,g,b;

	ScriptObject PathfinderColors = GScript.Globals()["Tweak"]["PathfinderColors"];

	r = (unsigned int)PathfinderColors["SelectedR"].GetInt(); if( r > 255 ) r = 255;
	g = (unsigned int)PathfinderColors["SelectedG"].GetInt(); if( g > 255 ) g = 255;
	b = (unsigned int)PathfinderColors["SelectedB"].GetInt(); if( b > 255 ) b = 255;
#ifdef TRANSGAMING
    SelectedColor = (b<<16)+(g<<8)+r;
#else
    SelectedColor = (r<<16)+(g<<8)+b;
#endif

	r = (unsigned int)PathfinderColors["ChosenSetR"].GetInt(); if( r > 255 ) r = 255;
	g = (unsigned int)PathfinderColors["ChosenSetG"].GetInt(); if( g > 255 ) g = 255;
	b = (unsigned int)PathfinderColors["ChosenSetB"].GetInt(); if( b > 255 ) b = 255;
#ifdef TRANSGAMING
	ChosenSetColor = (b<<16)+(g<<8)+r;
#else
	ChosenSetColor = (r<<16)+(g<<8)+b;
#endif

	r = (unsigned int)PathfinderColors["DisabledR"].GetInt(); if( r > 255 ) r = 255;
	g = (unsigned int)PathfinderColors["DisabledG"].GetInt(); if( g > 255 ) g = 255;
	b = (unsigned int)PathfinderColors["DisabledB"].GetInt(); if( b > 255 ) b = 255;
#ifdef TRANSGAMING
	DisabledColor = (b<<16)+(g<<8)+r;
#else
	DisabledColor = (r<<16)+(g<<8)+b;
#endif

	r = (unsigned int)PathfinderColors["BorderR"].GetInt(); if( r > 255 ) r = 255;
	g = (unsigned int)PathfinderColors["BorderG"].GetInt(); if( g > 255 ) g = 255;
	b = (unsigned int)PathfinderColors["BorderB"].GetInt(); if( b > 255 ) b = 255;
#ifdef TRANSGAMING
	BorderColor = (b<<16)+(g<<8)+r;
#else
	BorderColor = (r<<16)+(g<<8)+b;
#endif

    const int BUFSIZE = 16;
    char tmpBuf[BUFSIZE];

	for( int i = 0; i < NUM_WPSET_COLORS; ++i )
	{
        snprintf(tmpBuf, BUFSIZE, "SetColor%dR", i + 1);
		r = (unsigned int)PathfinderColors[tmpBuf].GetInt(); if( r > 255 ) r = 255;

        snprintf(tmpBuf, BUFSIZE, "SetColor%dR", i + 1);
		g = (unsigned int)PathfinderColors[tmpBuf].GetInt(); if( g > 255 ) g = 255;

        snprintf(tmpBuf, BUFSIZE, "SetColor%dR", i + 1);
		b = (unsigned int)PathfinderColors[tmpBuf].GetInt(); if( b > 255 ) b = 255;

#ifdef TRANSGAMING
		SetColors[i] = (b<<16)+(g<<8)+r;
#else
		SetColors[i] = (r<<16)+(g<<8)+b;
#endif
	}

}
#endif

void Pathfinder2::CalculateBorder( int i )
{
	int ourSet = mWaypoints[i].Set;

	int searchCount = 0;
	int wpIndex;
	float dist;
	bool bordered = false;
	while( GetNextAvailableFrom( i, searchCount, wpIndex, dist ) != 0 )
		if( mWaypoints[wpIndex].Set != ourSet )
		{
			bordered = true;
			break;
		}
	mWaypoints[i].Border = bordered;
}

void Pathfinder2::CalculateBorders( void )
{
	if( CalculatedBorders ) return;

#ifdef LOG_PATHFINDER_ACTIONS
	GLog.Print( "Recalculating borders..." );
#endif

	for( int i = 0; i < mWaypoints.Size(); ++i )
		CalculateBorder(i);

	CalculatedBorders = true;

#ifdef LOG_PATHFINDER_ACTIONS
	GLog.Print( "recalculated.\n" );
#endif
}

void Pathfinder2::GetClosestWaypoint( Vector point, int& index, float& distance )
{
	index = GetIndexOfWaypointClosestTo( point );
	if( index != -1 )
		distance = (mWaypoints[index].Position - point).Magnitude();
}

void Pathfinder2::GetWaypointPath( int index, int pathIndex, int& targetIndex, float& distance )
{
	Assert( pathIndex < mWaypoints[index].Count );
	int ind = mWaypoints[index].Base + pathIndex;
	targetIndex = mPackedIndices[ind];
	distance = mDistances[ind];
}

void Pathfinder2::AssertTwoWayPaths( void )
{
	DynamicArray<int> from;
	DynamicArray<int> to;
	int searchCount = 0;
	int wpIndex;
	float dist = 0.0f;

	for( int i = 0; i < mWaypoints.Size(); ++i )
	{
		searchCount = 0;
		while( GetNextAvailableFrom( i, searchCount, wpIndex, dist ) )
		{
			from.Add( wpIndex );
			to.Add( i );
		}
	}

	// Sort lists
	int temp;
	for( int i = 0; i < from.Size(); ++i )
	{
		int lowestI = from[i];
		int lowestIndex = i;
		for( int j = i+1; j < from.Size(); ++j )
			if( from[j] < lowestI )
			{
				lowestI = from[j];
				lowestIndex = j;
			}

		if( lowestIndex != i )
		{
			temp = from[i];		from[i] = from[lowestIndex];		from[lowestIndex] = temp;
			temp = to[i];		to[i] = to[lowestIndex];			to[lowestIndex] = temp;
		}
	}

	MassAddConnection( from, to );
}

void Pathfinder2::ChangeViewLimits( bool expand )
{
	if( expand )
	{
		ViewSize *= 1.1f;
		if( ViewSize > 1500 ) ViewSize = 1500;
	}
	else
	{
		ViewSize *= 0.9f;
		if( ViewSize < 10 ) ViewSize = 10;
	}
}

void Pathfinder2::MassAddConnection( DynamicArray<int>& from, DynamicArray<int>& to )
{
	Assert( from.Size() == to.Size() );
	if( !from.Size() ) return;

	// Make sure the list 'from' is sorted from lowest to highest

	int connWalker = 0;
	int newDistCount = 0;
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
	float* newDist = new float[mHowManyDistances+from.Size()];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
	int* newInd = new int[mHowManyDistances+from.Size()];

	int size = mWaypoints.Size();
	for( int i = 0; i < size; ++i )
	{
		// Add to new table connections we have
		int newBase = newDistCount;
		for( int j = 0; j < mWaypoints[i].Count; ++j )
		{
			newDist[newDistCount] = mDistances[mWaypoints[i].Base+j];
			newInd[newDistCount++] = mPackedIndices[mWaypoints[i].Base+j];
		}

		if( connWalker < from.Size() )
		{
			while( from[connWalker] == i )
			{
				int j = to[connWalker];
				if( !Connected( i, j ) )
				{
					newDist[newDistCount] = (mWaypoints[i].Position - mWaypoints[j].Position).Magnitude();
					newInd[newDistCount++] = j;
					mWaypoints[i].Count++;
				}
				connWalker++;
				if( connWalker == from.Size() ) break;
			}
		}
		mWaypoints[i].Base = newBase;
	}

	if( mDistances ) delete[] mDistances;
	if( mPackedIndices ) delete[] mPackedIndices;

#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	mDistances = new float[newDistCount];
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	mPackedIndices = new int[newDistCount];
	::memcpy( mDistances, newDist, newDistCount*sizeof(float) );
	::memcpy( mPackedIndices, newInd, newDistCount*sizeof(int) );
	delete[] newDist;
	delete[] newInd;
	mHowManyDistances = newDistCount;

	CalculateBorders();
	ClearAutomaticStructures();
}

void Pathfinder2::MassRemoveConnection( DynamicArray<int>& from, DynamicArray<int>& to )
{
	Assert( from.Size() == to.Size() );
	if( !from.Size() ) return;
	if( !mDistances ) return;

	// Assuming that 'from' are from lowest to highest

	int connWalker = 0;
	int newDistCount = 0;

	int size = mWaypoints.Size();
	for( int i = 0; i < size; ++i )
	{
		int base = mWaypoints[i].Base;

		if( connWalker < from.Size() )
		{
			while( from[connWalker] == i )
			{
				int baseEnd = base+mWaypoints[i].Count;
				int j = to[connWalker];
				for( int k = base; k < baseEnd; ++k )
					if( mPackedIndices[k] == j )
					{
						mDistances[k] = mDistances[baseEnd-1];
						mPackedIndices[k] = mPackedIndices[baseEnd-1];
						mWaypoints[i].Count--;
						connWalker++;
						break;
					}
				if( connWalker == from.Size() ) break;
			}
		}

		// Add to new table connections we have
		for( int j = 0; j < mWaypoints[i].Count; ++j )
		{
			mDistances[newDistCount] = mDistances[mWaypoints[i].Base+j];
			mPackedIndices[newDistCount++] = mPackedIndices[mWaypoints[i].Base+j];
		}

		mWaypoints[i].Base = base;
	}

	if( newDistCount == mHowManyDistances )
		return;

	float* newDist = 0;
	int* newInd = 0;

	if( newDistCount )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		newDist = new float[newDistCount];
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		newInd = new int[newDistCount];
		::memcpy( newDist, mDistances, newDistCount*sizeof(float) );
		::memcpy( newInd, mPackedIndices, newDistCount*sizeof(int) );
	}

	delete[] mDistances;		mDistances = newDist;
	delete[] mPackedIndices;	mPackedIndices = newInd;
	mHowManyDistances = newDistCount;

	::memcpy( mDistances, newDist, newDistCount*sizeof(float) );
	::memcpy( mPackedIndices, newInd, newDistCount*sizeof(int) );
	delete[] newDist;
	delete[] newInd;

	CalculateBorders();
	ClearAutomaticStructures();
}

void Pathfinder2::MakeUndo( int index, Vector& position )
{
	for( int i = 0; i < UndoDisplacement.Size(); ++i )
		if( UndoDisplacement[i].Index == index )
			return;
	PathfinderUndo u;
	u.Index = index;
	u.Position = position;
	UndoDisplacement.Add( u );
}

void Pathfinder2::ClearUndo( int index )
{
	for( int i = 0; i < UndoDisplacement.Size(); ++i )
		if( UndoDisplacement[i].Index == index )
		{
			UndoDisplacement.FastRemove( i );
			return;
		}
}

void Pathfinder2::ApplyUndo( void )
{
	for( int i = 0; i < UndoDisplacement.Size(); ++i )
	{
		mWaypoints[UndoDisplacement[i].Index].Position = UndoDisplacement[i].Position;
		RecalculateDistances(UndoDisplacement[i].Index);
	}
	UndoDisplacement.ClearArray();
}

void Pathfinder2::MergeWaypointsBelowDistance( float distance )
{
	if( !mWaypoints.Size() ) return;
	if( distance <= 0 ) return;
	ClearAutomaticStructures();
	SortWaypoints();
	float distSqrd = distance*distance;


	// Prepare merge numbers
	int size = mWaypoints.Size();
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	int* mergeNumbers = new int[size];
	for( int i = 0; i < size; ++i ) mergeNumbers[i] = -1;

	bool noSelected = true;
	for( int start = 0; start < size; ++start )
		if( mWaypoints[start].Selected )
		{
			noSelected = false;
			break;
		}

	int howManyMergeNumbers = 0;

	int end = 0;
	for( int start = 0; start < size; ++start )
	{
		float startX = mWaypoints[start].Position.X;
		Vector& pos = mWaypoints[start].Position;
		while( ( end < size ) && ( mWaypoints[end].Position.X - startX < distance ) ) end++;

		DynamicArray<int> okSet;

		if( !noSelected && !mWaypoints[start].Selected )
			continue;

		int toGive = start;
		if( ( mergeNumbers[start] != -1 ) && ( mergeNumbers[start] < toGive ) ) toGive = mergeNumbers[start];

		for( int i = start+1; i < end; ++i )
		{
			if( !noSelected && !mWaypoints[i].Selected )
				continue;
			if( (mWaypoints[i].Position - pos).MagnitudeSquared() < distSqrd )
			{
				if( ( mergeNumbers[i] != -1 ) && ( mergeNumbers[i] < toGive ) ) toGive = mergeNumbers[i];
				okSet.Add( i );
			}
		}

		for( int i = 0; i < okSet.Size(); ++i ) mergeNumbers[okSet[i]] = toGive;

		if( okSet.Size() )
		{
			mergeNumbers[start] = toGive;
			if( toGive == start ) howManyMergeNumbers++;
		}
	}

	// Now merge all sets of points

	// 1. for each set, calculate the set the target wp will be in, and its average position
#ifdef TRANSGAMING
    //@@@// eric-> reduced the size of these hash tables for the PSP and PS2
	HashMap<int,int,256> targetSet;
	HashMap<int,Vector,256> targetPos;
# ifdef FIND_HASH_PEAK
    static int maxTargetSetUsage = 0;
    static int maxTargetPosUsage = 0;
# endif
#else
	HashMap<int,int,256> targetSet;
	HashMap<int,Vector,256> targetPos;
#endif

	for( int end = size-1; end >= 0; --end )
	{
		if( mergeNumbers[end] == -1 ) continue;
		if( targetSet.Find( mergeNumbers[end] ) ) continue;

		int howMany = 1;
		Vector pos = mWaypoints[end].Position;
		DynamicArray<int> setNumbers;
		DynamicArray<int> setCount;

		setNumbers.Add( mWaypoints[end].Set );
		setCount.Add( 1 );

		int mn = mergeNumbers[end];

		// Calculate target pos
		for( int i = end-1; i >= mn; --i )
			if( mergeNumbers[i] == mn )
			{
				howMany++;
				pos += mWaypoints[i].Position;
				int set = mWaypoints[i].Set;
				int where = setNumbers.Find( set );
				if( where == -1 )
				{
					setNumbers.Add( set );
					setCount.Add( 1 );
				}
				else
					setCount[where]++;
			}

		// Calculate target set
		int maxSet = 0;
		int maxCount = setCount[0];
		for( int i = 0; i < setNumbers.Size(); ++i )
			if( setCount[i] > maxCount )
			{
				maxCount = setCount[i];
				maxSet = i;
			}

		// fill hashes
		targetSet.Insert(mn, setNumbers[maxSet] );
		pos /= howMany;
		targetPos.Insert( mn, pos );
#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
        if (targetSet.Size() > maxTargetSetUsage){
            maxTargetSetUsage = targetSet.Size();
            TRACE("@@@ Pathfinder2::MergeWaypointsBelowDistance(): peak targetSet hash usage = %d\n", maxTargetSetUsage);
        }

        if (targetPos.Size() > maxTargetPosUsage){
            maxTargetPosUsage = targetPos.Size();
            TRACE("@@@ Pathfinder2::MergeWaypointsBelowDistance(): peak targetPos hash usage = %d\n", maxTargetPosUsage);
        }
#endif
	}

	// 2. establish the table of mappings from old wp number to new one
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	int* wpMapping = new int[size];
	int wpMappingWalker = 0;
	for( int i = 0; i < size; ++i )
	{
		if( ( mergeNumbers[i] == -1 ) || ( mergeNumbers[i] == i ) )
		{
			wpMapping[i] = wpMappingWalker;
			wpMappingWalker++;
		}
		else
			wpMapping[i] = wpMapping[mergeNumbers[i]];
	}

	// 3. make temp mWaypoints table, mDistances, mPackedIndices
	DynamicArray<Waypoint2> newWaypoints;
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	float* newDist = new float[mHowManyDistances];
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	int* newInd = new int[mHowManyDistances];

	// 4. pass through old tables, transferring changes to new ones
	int realHowManyDist = 0;

	for( int i = 0; i < size; ++i )
	{
		// Check if it is to be merged
		if( mergeNumbers[i] == -1 )
		{
			// no -> tranfer it like it should be
			Waypoint2 wp;
			wp.BigMonsters = mWaypoints[i].BigMonsters;
			wp.Border = mWaypoints[i].Border;
			wp.Disabled = mWaypoints[i].Disabled;
			wp.Selected = false;
			wp.Set = mWaypoints[i].Set;
			wp.Position = mWaypoints[i].Position;
			wp.Base = realHowManyDist;

			int start = mWaypoints[i].Base;
			int end = start + mWaypoints[i].Count;
			for( int j = start; j < end; ++j )
			{
				int mn = mergeNumbers[mPackedIndices[j]];
				if( mn == -1 )
					newDist[realHowManyDist] = mDistances[j];
				else if( mn == mergeNumbers[i] )
					continue;
				else
				{
					Vector* pos = targetPos.Find( mergeNumbers[mPackedIndices[j]] );
					Assert( pos );
					newDist[realHowManyDist] = (mWaypoints[i].Position - *pos).Magnitude();
				}
				newInd[realHowManyDist++] = wpMapping[mPackedIndices[j]];
			}

			wp.Count = realHowManyDist - wp.Base;
			newWaypoints.Add(wp);
		}
		else if( mergeNumbers[i] == i )
		{
			// yes -> is it first of a set?
			// yes -> transfer it with new pos and set
			Waypoint2 wp;

			wp.BigMonsters = mWaypoints[i].BigMonsters;
			wp.Border = mWaypoints[i].Border;
			wp.Disabled = mWaypoints[i].Disabled;
			wp.Selected = true;
			int* setPtr = targetSet.Find(mergeNumbers[i]);
			Assert( setPtr );
			wp.Set = *setPtr;
			Vector* pos = targetPos.Find(mergeNumbers[i]);
			Assert( pos );
			wp.Position = *pos;
			wp.Base = realHowManyDist;

			// 1. insert connections from this first waypoint

			int start = mWaypoints[i].Base;
			int end = start + mWaypoints[i].Count;
			for( int j = start; j < end; ++j )
			{
				int mn = mergeNumbers[mPackedIndices[j]];
				if( mn == mergeNumbers[i] )
					continue;
				Vector* pos = ( mn == -1 )?&mWaypoints[mPackedIndices[j]].Position:targetPos.Find( mn );
				Assert( pos );
				newDist[realHowManyDist] = (wp.Position - *pos).Magnitude();
				newInd[realHowManyDist++] = wpMapping[mPackedIndices[j]];
			}

			// 2. insert connections from other waypoints, that double with our waypoint
			for( int j = i+1; j < size; ++j )
				if( mergeNumbers[j] == mergeNumbers[i] )
				{
					int start = mWaypoints[j].Base;
					int end = start + mWaypoints[j].Count;
					for( int k = start; k < end; ++k )
					{
						int mn = mergeNumbers[mPackedIndices[k]];
						if( mn == mergeNumbers[i] )
							continue;

						int endTargetWpIndex = wpMapping[mPackedIndices[k]];
						bool weHaveIt = false;
						for( int l = wp.Base; l < realHowManyDist; ++l )
							if( newInd[l] == endTargetWpIndex )
							{
								weHaveIt = true;
								break;
							}

						if( !weHaveIt )
						{
							Vector* pos = ( mn == -1 )?&mWaypoints[mPackedIndices[j]].Position:targetPos.Find( mn );
							Assert( pos );
							newDist[realHowManyDist] = (wp.Position - *pos).Magnitude();
							newInd[realHowManyDist++] = endTargetWpIndex;
						}
					}
				}

			wp.Count = realHowManyDist - wp.Base;
			newWaypoints.Add(wp);
		}
	}

	// 5. destroy old tables and replace them with new ones

	mWaypoints = newWaypoints;

	if( mDistances ) delete[] mDistances;
	if( mPackedIndices ) delete[] mPackedIndices;

#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	mDistances = new float[realHowManyDist];
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	mPackedIndices = new int[realHowManyDist];
	::memcpy( mDistances, newDist, realHowManyDist*sizeof(float) );
	::memcpy( mPackedIndices, newInd, realHowManyDist*sizeof(int) );
	delete[] newDist;
	delete[] newInd;
	mHowManyDistances = realHowManyDist;
	delete[] wpMapping;

	AssertTwoWayPaths();
	ValidateSets();
	SortWaypoints();
	CalculateBorders();
}

void Pathfinder2::RandomizeSets( void )
{
#ifdef LOG_PATHFINDER_ACTIONS
	GLog.Print( "Randomizing sets..." );
#endif
	ClearAutomaticStructures();

	// Determine first unused set number
	FirstUnusedSetNumber = 0;
	for( int i = 0; i < mWaypoints.Size(); ++i )
		if( mWaypoints[i].Set >= FirstUnusedSetNumber )
			FirstUnusedSetNumber = mWaypoints[i].Set+1;

	if( FirstUnusedSetNumber <= 1 ) return;

	// Determine randomize table
#ifdef TRANSGAMING_PSP
    m_setTag("AI");
#endif
	int* setNumbers = new int[FirstUnusedSetNumber];
	DynamicArray<int> Numbers;
	for( int i = 0; i < FirstUnusedSetNumber; ++i )
		Numbers.Add(i);
	for( int i = 0; i < FirstUnusedSetNumber; ++i )
	{
		int index = rand()%Numbers.Size();
		setNumbers[i] = Numbers[index];
		Numbers.FastRemove(index);
	}

	// Change waypoints
	for( int i = 0; i < mWaypoints.Size(); ++i )
		mWaypoints[i].Set = setNumbers[mWaypoints[i].Set];

	// Change current set to preserve it
	CurrentSet = setNumbers[CurrentSet];

	// Change enabled sets to preserve them
	DynamicArray<bool> enabled = EnabledSets;
	for( int i = 0; i < enabled.Size(); ++i ) enabled[i] = EnabledSets[setNumbers[i]];
	EnabledSets = enabled;

#ifdef LOG_PATHFINDER_ACTIONS
	GLog.Print( "randomized.\n" );
#endif
}

bool Pathfinder2::LoadPathfinding( FileObject* f )
{
	// No need to clear waypoint paths; I assume the new waypoints have just loaded
	Assert( WaypointPaths.Size() == FreePathIndices.Size() );
	WaypointPaths.ClearArray();
	FreePathIndices.ClearArray();

	int count;
	char ok;
	*f >> count;

	for( int i = 0; i < count; ++i )
	{
		*f >> ok;
		if( ok )
		{
#ifdef TRANSGAMING_PSP
            m_setTag("AI");
#endif
			WaypointGPath2* path = new WaypointGPath2;
			path->Init( this );
			if( !path->Load( f ) )
				return false;
			WaypointPaths.Add( path );
		}
		else
		{
			int ind = WaypointPaths.Add( 0 );
			FreePathIndices.Add( ind );
		}

	}

	return true;
}

bool Pathfinder2::SavePathfinding( FileObject* f )
{
	*f << WaypointPaths.Size();

	for( int i = 0; i < WaypointPaths.Size(); ++i )
		if( WaypointPaths[i] != 0 )
		{
			*f << (char)1;
			if( !WaypointPaths[i]->Save( f ) )
				return false;
		}
		else
			*f << (char)0;

	return true;
}
