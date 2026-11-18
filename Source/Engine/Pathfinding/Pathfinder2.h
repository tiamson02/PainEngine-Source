// ---------------------------------------------------------------------------
// Copyright (c) 2002-2003 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2003/10 Krystian Galaj
//----------------------------------------------------------------------------

// No definitions in this file anymore - KG

// #pragma once
#ifndef _PATHFINDER2_
#define _PATHFINDER2_


class Viewport;
class WaypointSet;

#define FLOORS_ENABLED

#include "Engine.h"
#include "Pathfinder2Waypoints.h"

#ifdef FLOORS_ENABLED
#include "Pathfinder2Floors.h"
#endif

#define NUM_WPSET_COLORS 6

//-------------------------------------------------------------------------------------------------------
class Pathfinder2;
class Pathfinder2Portal;

#ifdef TRANSGAMING_WINDOWS
class ENGINE_API WaypointGPath2
#else
class WaypointGPath2
#endif
{
	friend class Pathfinder2;
	friend class WaypointPath2;
#ifdef TRANSGAMING_USE_EDITOR
	friend class LineRender;
#endif

public:
	WaypointGPath2( void ) : mInit(0), mPrecisePath(0), mMinimalDistance(0.0f), mMaximalDistance(1000.0f) {};
	~WaypointGPath2(void) { Empty(); };

	inline void Init( Pathfinder2* pf );
#ifdef TRANSGAMING
	void Empty( void );
#else
	inline void Empty( void );
#endif
	inline int Finished( void );
	inline Vector Next( Vector& dir );
	int PeekPos( int index, Vector& pos );
	void Step( void );
	WaypointGPath2& operator=(const WaypointGPath2& a);
	void RecalculatePath( void );
	void SetMoveConstraints( float minimalDistance, float maximalDistance )
	{
		mMinimalDistance = minimalDistance;
		mMaximalDistance = maximalDistance;
	};

	// For savegames
	bool Load( FileObject* f );
	bool Save( FileObject* f );

protected:
	WaypointPath2* CalculatePreciselyNextSet( void );
	int InternalGetNext( Vector& next, int& setIndex, int& waypointIndex );
	inline void GetPosLast( Vector& pos );

	void PeekDir( int index, Vector& dir );
	inline int Peek( int index, Vector& pos, Vector& dir );	// Returns 1 if vectors are valid, 0 if are not

	// Variables set once in object lifetime
	Pathfinder2* mPathfinder;
	char mInit;

	// Raw paths
	WaypointPath2* mPrecisePath;
	DynamicArray<Pathfinder2Portal*> mPortals;

	// Additional capabilities
	Vector mEndPoint;
	float mDistance;
	Vector mPosMinus1;
	int mCurrentSet;

	// Constraints for point-to-point distance

	float mMinimalDistance;
	float mMaximalDistance;

	DynamicArray<Vector> mCalculatedPoints;
	DynamicArray<int> mCalculatedSetIndices;
	DynamicArray<int> mCalculatedWaypointIndices;
	DynamicArray<Vector> mCalculatedDirs;
};

//-------------------------------------------------------------------------------------------------------

class Pathfinder2Set
{
public:
	// Methods
	Pathfinder2Set() : PortalDistances( 0 ), Color( 0 ) {}
	~Pathfinder2Set() { if( PortalDistances ) delete[] PortalDistances, PortalDistances = 0; }

	// Topology
	DynamicArray<int> Waypoints;
	DynamicArray<int> PortalIndices;

	// Data helping in AStar value tracing
	float* PortalDistances;

	// Display
	unsigned char Color:4;
};

class Pathfinder2Portal
{
public:
	// Methods
	Pathfinder2Portal() {};
	~Pathfinder2Portal() {};

	// Topology
	DynamicArray<int> Waypoints;
	int LowerSetIndex;
	int HigherSetIndex;

	// Data helping in AStar value tracing
	Vector Center;
	float Width;
	int BorderPoint1;
	int BorderPoint2;
};

class Pathfinder2AStarNode
{
public:

	// Variables
	Pathfinder2Portal* Portal;
	int From;
	int LastSet;
	float Cost;
	float Heuristic;

	Pathfinder2AStarNode( Pathfinder2Portal* portal = 0 ) : Portal(portal), From(0), Cost(0), Heuristic(0) {}
	Pathfinder2AStarNode( const Pathfinder2AStarNode& node ) : Portal(node.Portal), From(node.From), Cost(node.Cost), Heuristic(node.Heuristic) {}
	~Pathfinder2AStarNode( void ) {}

	inline bool operator<( Pathfinder2AStarNode& node ) const { return ( Cost + Heuristic < node.Cost + node.Heuristic ); }
	inline bool operator>( Pathfinder2AStarNode& node ) const { return ( Cost + Heuristic > node.Cost + node.Heuristic ); }
};

//-------------------------------------------------------------------------------------------------------

struct PathfinderUndo
{
	int Index;
	Vector Position;
};

#ifdef TRANSGAMING_WINDOWS
class ENGINE_API Pathfinder2
#else
class Pathfinder2
#endif
{
	friend class WaypointGPath2;
#ifdef TRANSGAMING_USE_EDITOR
	friend class LineRender;
#endif
public:
	Pathfinder2( void );
	~Pathfinder2( void );
#ifdef TRANSGAMING_USE_EDITOR
	void GetColorsFromScript( void );
#endif
	// ************* Methods *******************
	inline float Dist(const int index1, const int index2);

	// Waypoint manipulation - basic
	int AddWaypoint( Waypoint2 waypoint );	// returns index
	void MoveWaypoint( int waypointIndex, Vector newPosition, bool recalculateDistances = false );
	void RecalculateDistances( int waypointIndex );
	void RecalculateAllDistances( void );
	void RemoveWaypoint( Waypoint2& waypoint );
	void RemoveWaypoint( int index );
	int GetWaypointIndex( Waypoint2& waypoint );
	int GetWaypointByPosition( Vector& position );
	Waypoint2& operator[]( const int index ) const { return mWaypoints[index]; };

	// Path information - basic
	int GetWaypointPaths( int index ) { return mWaypoints[index].Count; }
	void GetWaypointPath( int index, int pathIndex, int& targetIndex, float& distance );

	// Global
	void ScaleContents( float scale );
	void FlipZ( void );

	// Set manipulation
	void ExpandCurrentSet( void );
	void ContractCurrentSet( void );
	void GetCurrentSetFromSelected( void );
	void MakeNewSetFromSelected( void );
	void ValidateSet( unsigned short set );
	void ValidateSets( void );
	void MergeSetsFromSelected( void );

	inline int FastGetWaypointIndex( Waypoint2* waypoint );
	inline void MoveWaypoint( Waypoint2& waypoint, Vector newPosition, bool recalculateDistances = false );
	inline int NumberOfWaypoints( void );

	// Waypoint selection
	inline void Select( int index, bool select );
#ifndef TRANSGAMING
	inline 
#endif
    void Select_All( bool select );
	inline void Select_AllInSet( bool select, unsigned short set );
	inline void Select_Invert( void );
	void Select_NotConnectedToAnything( void );
	void Select_OnSelectedFloors( bool select );

	// Waypoint manipulation - selected
	int Selection( Viewport& viewport, float toleranceDistance, RECT* rect, bool select );
#ifdef TRANSGAMING_USE_EDITOR
	void Selected_Displace( const Vector& displacement );
	void Selected_Copy( const Vector& displacement );
	void Selected_Scale( const Vector& center, float scale );
	void Selected_Recalculate( void );
	void Selected_Remove( void );
	void Selected_MoveToCurrentSet( void );
	void Selected_ConnectDisconnect( float minDistance, float maxDistance, int connect, int disconnect );
	inline void Selected_Connect( void );
	inline void Selected_Disconnect( void );
	void Selected_RemoveConnectionsCollidingWithGeometry( float threshold );
#endif

	// Work methods
	int GetIndexOfWaypointClosestTo( const Vector& point, float toleranceDistance = FLT_MAX );
	void GetClosestWaypoint( Vector point, int& index, float& distance );
	float GetShortestPath( Vector from, Vector to, WaypointGPath2& path );
	float GetShortestPath( int from, int* to, int toHowMany, WaypointPath2* path, bool bigMonsters = false, Vector* targetForHeur = 0, float maxLenToTry = FLT_MAX, int restrictToZone = -1 );
	int PickWaypoint( Viewport& viewport, int xm, int ym, float toleranceDistance, bool pickDisabled = false );
	void GenerateAutomaticStructures( void );
	void ClearAutomaticStructures( void );

	// For savegames
	bool LoadPathfinding( FileObject* f );
	bool SavePathfinding( FileObject* f );

#ifdef TRANSGAMING_USE_EDITOR
	void Render( int renderWaypoints, int renderFloors );
#endif

	// Temporary disabling/enabling waypoints
	inline void EnableAllWaypoints( bool enable );
	inline void EnableWaypoint( int number, bool enable );
	inline bool WaypointEnabled( int number );
	inline void EnableInBox( BoundingBox& box, bool enable );

	inline bool Connected( int i, int j );
	void AddConnection( int i, int j );
	void MassAddConnection( DynamicArray<int>& from, DynamicArray<int>& to );
	void RemoveConnection( int i, int j );
	void MassRemoveConnection( DynamicArray<int>& from, DynamicArray<int>& to );
#ifdef TRANSGAMING_USE_EDITOR
	int ConnectionCollidingWithGeometry( const Vector& source, const Vector& destination );
#endif

	// Load/Save
	bool LoadContents( const char* filename, float scale = 1.0f, bool dontLoadPortals = false );
	bool SaveContents( const char* filename, float scale = 1.0f, bool generateAutoStructsFirst = false );
	bool MergeContents( const char* filename, float scale = 1.0f );

	bool LoadFloors( const char* filename, float scale = 1.0f );
	bool SaveFloors( const char* filename, float scale = 1.0f );

	// One time methods
	void ImportFromOldPathfinder( void );

	// Floors methods
	void SelectedFloors_AddGrid( float step );
#ifdef FLOORS_ENABLED
	void Select_FloorsOfAreaLowerHigherThan( bool select, bool lower, float area ) { Floors.Select_FloorsOfAreaLowerHigherThan( select, lower, area ); };
	void SelectedFloors_Remove( void ) { Floors.Selected_Remove(); };
# ifdef TRANSGAMING_USE_EDITOR
	void PrepareFloors( void ) { Floors.GenerateFloors(); };
# endif
	void FloorSelection( const Viewport& viewport, RECT* rect, bool select ) { Floors.Selection( viewport, rect, select ); };
#endif

	int HowManySets( void ) { return Sets.Size(); };
	void ChangeViewLimits( bool expand );
	float GetViewLimits( void ) { return ViewSize; };

	void ClearContents( void );
	void ApplyUndo( void );

	// Enable/disable sets; draw disabled sets
	bool DrawDisabledSets;
	void EnableSet( unsigned short setNr, bool enable ) { EnabledSets[setNr] = enable; };
	void SwitchSetEnabling( Viewport& viewport, int xm, int ym, float toleranceDistance )
	{
		int wpindex = PickWaypoint( viewport, xm, ym, toleranceDistance, true );
		if( wpindex != -1 )
		{
			unsigned short set = mWaypoints[wpindex].Set;
			bool enable = !SetEnabled(set);
			if( !enable )
			{
				Select_AllInSet( false, set );
				if( CurrentSet == set )
					CurrentSet = FirstUnusedSetNumber;
			}
			EnableSet( set, enable );
		}
	}
	void EnableSets( bool enable )
	{
		if( !enable ) { Select_All(false); CurrentSet = FirstUnusedSetNumber; };
		for( int i = 0; i < EnabledSets.Size(); ++i ) EnabledSets[i] = enable;
	}

	// Fast pick current set
	void FastPickCurrentSet( Viewport& viewport, int xm, int ym, float toleranceDistance )
	{
		int wpindex = PickWaypoint( viewport, xm, ym, toleranceDistance );
		if( wpindex != -1 )
			CurrentSet = mWaypoints[wpindex].Set;
		else
			CurrentSet = FirstUnusedSetNumber;
	}

	void MergeWaypointsBelowDistance( float distance );
	void RandomizeSets( void );

protected:

	// ************* Methods *******************
	float GetShortestPortalPath( int from, int to, DynamicArray<int>& list, int startSetIndex, float noLongerThan = FLT_MAX );
	void ImportFromOldPathfinderFromWpSet( WaypointSet* wpset, int set );
	Waypoint2* GetNextAvailableFrom( int from, int& searchCount, int& searchPtr, float& dist );
	void GeneratePortal( int startIndex, int endIndex, DynamicArray<int>& portalPoints );
	int GetClosestInPortal( Pathfinder2Portal* portal, Vector& point, int setIndex = -1 );
	Vector CenterCalculation( Pathfinder2Portal* portal, Vector& from, Vector& to );
	void SortWaypoints( void );
	void CalculateBorder( int i );
	void CalculateBorders( void );
	void AssertTwoWayPaths( void );

	// ************* Members *******************

	// Waypoints
	DynamicArray<Waypoint2> mWaypoints;
	bool SortedWaypoints;
	bool CalculatedBorders;

	// Connections - packed representation
	float* mDistances;
	int mHowManyDistances;
	int* mPackedIndices;	// as many as there are distances - destination for each distance

	// Sets
	unsigned short FirstUnusedSetNumber;
	DynamicArray<bool> EnabledSets;
	unsigned short CurrentSet;

	// Floors
#ifdef FLOORS_ENABLED
	Pathfinder2FloorSet Floors;
#endif

	// Automatically generated data
	bool AutoValid;
	DynamicArray<Pathfinder2Set> Sets;
	DynamicArray<Pathfinder2Portal*> Portals;

	// View param
	float ViewSize;

	// Undo for displacements
	DynamicArray<PathfinderUndo> UndoDisplacement;
	void MakeUndo( int index, Vector& position );
	void ClearUndo( int index );

public:
	bool SetEnabled( int setNr ) { Assert( setNr < EnabledSets.Size() ); return EnabledSets[setNr]; };
	short GetCurrentSet( void ) {return CurrentSet; };
	short GetValidCurrentSet( void )
	{
		Assert( CurrentSet <= FirstUnusedSetNumber );
		if( CurrentSet == FirstUnusedSetNumber )
		{
			FirstUnusedSetNumber++;
			EnabledSets.Add( true );
		}
		return CurrentSet;
	}

	// Colors
#ifdef TRANSGAMING_USE_EDITOR
	DWORD SelectedColor;
	DWORD ChosenSetColor;
	DWORD DisabledColor;
	DWORD BorderColor;
	DWORD SetColors[NUM_WPSET_COLORS];
#endif

	// Waypoint paths table
	DynamicArray<WaypointGPath2*> WaypointPaths;
	DynamicArray<int> FreePathIndices;
};

// Inlines and shorts
inline void WaypointGPath2::Init( Pathfinder2* pf ) { mPathfinder = pf; mInit = 1; }
inline int WaypointGPath2::Finished( void ) { Vector t; return !PeekPos(0, t); }
inline Vector WaypointGPath2::Next( Vector& dir ) { Vector temp; Peek( 0, temp, dir ); Step(); return temp; }
inline void WaypointGPath2::GetPosLast( Vector& pos ) { pos = mCalculatedPoints.Size()?mCalculatedPoints[mCalculatedPoints.Size()-1]:mPosMinus1; }
inline int WaypointGPath2::Peek( int index, Vector& pos, Vector& dir ) { PeekDir( index, dir ); return PeekPos( index, pos ); }

inline float Pathfinder2::Dist( const int index1, const int index2 ) { return mWaypoints[index1].Dist(mWaypoints[index2]); }
inline int Pathfinder2::FastGetWaypointIndex( Waypoint2* waypoint ) { return (waypoint-&mWaypoints[0]); }
inline void Pathfinder2::MoveWaypoint( Waypoint2& waypoint, Vector newPosition, bool recalculateDistances ) { MoveWaypoint( FastGetWaypointIndex( &waypoint ), newPosition, recalculateDistances ); }
inline int Pathfinder2::NumberOfWaypoints( void ) { return mWaypoints.Size(); }
inline void Pathfinder2::Select_AllInSet( bool select, unsigned short set ) { if( select && !SetEnabled(set) ) return; for( int i = 0; i < mWaypoints.Size(); ++i ) if( mWaypoints[i].Set == set ) mWaypoints[i].Selected = select; UndoDisplacement.ClearArray(); };
#ifdef TRANSGAMING_USE_EDITOR
inline void Pathfinder2::Selected_Connect( void ) { Selected_ConnectDisconnect( -0.1f, FLT_MAX, 1, 0); }
inline void Pathfinder2::Selected_Disconnect( void ) { Selected_ConnectDisconnect( 1.0f, 0.0f, 0, 1); }
#endif
inline void Pathfinder2::EnableAllWaypoints( bool enable ) { for( int i = 0; i < mWaypoints.Size(); ++i ) mWaypoints[i].Disabled = !enable; }
inline void Pathfinder2::EnableWaypoint( int number, bool enable ) { mWaypoints[number].Disabled = !enable; }
inline bool Pathfinder2::WaypointEnabled( int number ) { return !mWaypoints[number].Disabled; }
inline void Pathfinder2::EnableInBox( BoundingBox& box, bool enable ) { for( int i = 0; i < mWaypoints.Size(); ++i ) if( mWaypoints[i].InBox( box ) ) mWaypoints[i].Disabled = !enable; }
inline bool Pathfinder2::Connected( int i, int j ) { if( mDistances ) for( int a = 0; a < mWaypoints[i].Count; ++a ) if( mPackedIndices[mWaypoints[i].Base+a] == j ) return true; return false; }



#endif //_PATHFINDER2_

