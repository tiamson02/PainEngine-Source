// ---------------------------------------------------------------------------
// Copyright (c) 2002-2003 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2003/10 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _PATHFINDER2FLOORS_
#define _PATHFINDER2FLOORS_


#include "Core/nommgr.h"
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
#include <ext/hash_map>
using namespace::__gnu_cxx;
#else
#include <hash_map>
#endif
#include "Core/mmgr.h"
class WorldMesh;
class Viewport;

#include "Engine.h"

// Edge is always closed. When looking down (-y), if floor vertices turn counterclockwise,
// inside is in the floor, else outside is in the floor.

class Pathfinder2FloorEdge
{
public:
	Pathfinder2FloorEdge( void ) {};
	~Pathfinder2FloorEdge( void ) {};

	bool Load( FILE* f, float scale = 1.0f );
	bool Save( FILE* f, float scale = 1.0f );

	void ScaleContents( float scale ) { int size = mFloorVertices.Size(); for( int i = 0; i < size; ++i ) mFloorVertices[i] *= scale; };
	void FlipZ( void ) { int size = mFloorVertices.Size(); for( int i = 0; i < size; ++i ) mFloorVertices[i].Z *= -1.0; };

	// To be used when creating edge
	void AddVertex( Vector& v ) { mFloorVertices.Add( v ); };

	bool InsideFloorEdge( const Vector& p );
	bool DetermineOutsideInsideEdge( void );

protected:
	// Variables
	DynamicArray<Vector> mFloorVertices;
	bool outsideEdge;	// if false, it's inside edge. if true, outside edge
};

//-------------------------------------------------------------------------------------------------------
class Pathfinder2Floor
{
	friend class Pathfinder2FloorProducer;
#ifdef TRANSGAMING_USE_EDITOR
	friend class LineRender;
#endif

public:
	Pathfinder2Floor( void ) {};
	~Pathfinder2Floor( void ) { ClearContents(); };

	bool Load( FILE* f, float scale = 1.0f );
	bool Save( FILE* f, float scale = 1.0f );

	void ScaleContents( float scale );
	void FlipZ( void );

	// Floor creation helpers
	void AddEdge( Pathfinder2FloorEdge* newEdge ) { mFloorEdges.Add( newEdge ); };
	void AddTriangle( const Vector& p1, const Vector& p2, const Vector& p3 ) { mFloorTriangles.Add( p1 ); mFloorTriangles.Add( p2 ); mFloorTriangles.Add( p3 ); };

	bool InsideFloor( const Vector& p );

	int HowManyTriangles( void ) { return mFloorTriangles.Size()/3; };
	int HowManyEdges( void ) { return mFloorEdges.Size(); };

	const Vector& TrianglePoint( int i ) { return mFloorTriangles[i]; };

	// Give valid x and z to this point, expect valid y.
	// For better performance, check that the point IS on floor first!
	// It sets point on 0.f when point is not on floor.
	// Returns false when point is not on floor, and true if it is anyway, but it
	// takes much more time than for InsideFloor()
	bool SetPointHeightOnFloor( Vector& p );

	void ClearContents( void );
	float Area( void );
	void MakeGrid( float step, DynamicArray<Vector>& arrayToFill );

protected:
	// Variables
	DynamicArray<Pathfinder2FloorEdge*> mFloorEdges;
	DynamicArray<Vector> mFloorTriangles;
};

//-------------------------------------------------------------------------------------------------------

class Pathfinder2FloorProducer
{
	struct PFPoint { Vector point; int index; };

	struct PFEdge
	{
		int i[2];

		int howMany;
		int triangleIndex[2];	// edge of which triangles it is ( -1 if not present )
		int edgeNum[2];			// 0 - edge 0-1, 1 - edge 0-2, 2 - edge 1-2 in triangle
		int index;
	};

	struct PFTriangle
	{
		int i[3];

		int t[3];				// index of edges of the triangle
		char used;
		int index;
	};

public:
	Pathfinder2FloorProducer(void) : BadEdges(false) {};
	~Pathfinder2FloorProducer(void) { ClearContents(); };

	void ClearContents( void )
	{
		mMeshPoints.ClearArray();
		mMeshEdges.ClearArray();
		mMeshTriangles.ClearArray();
	};

#ifdef TRANSGAMING_USE_EDITOR
	void AddMesh( WorldMesh* mesh, int angleDegrees );
	void ProduceFloors( DynamicArray<Pathfinder2Floor*>& floors );
#endif

	bool BadEdges;

protected:

	int FindNextEdgeGoingRight( int edge );

	int GetCommonVertexIndex( int edge1, int edge2 )
	{
		PFEdge& e1 = mMeshEdges[edge1];
		PFEdge& e2 = mMeshEdges[edge2];
		return ( e1.i[0] == e2.i[0] )?e1.i[0]:(( e1.i[0] == e2.i[1] )?e1.i[0]:e1.i[1]);
	}

	// Variables
	DynamicArray<Vector> mMeshPoints;
	DynamicArray<PFTriangle> mMeshTriangles;
	DynamicArray<PFEdge> mMeshEdges;

	// AddPoint hashed
	// ********************************************************************
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
	hash_multimap<ulong32,PFPoint> mMeshPointsHash;
#else
	stdext::hash_multimap<ulong32,PFPoint> mMeshPointsHash;
#endif

	// This assumes that unsigned long has 32 bits.
	ulong32 makeKey( Vector* element )
	{
		ulong32 size = sizeof(Vector);
		unsigned char* data = (unsigned char *)element;
		ulong32 hash = 0;
		ulong32 wrap = 0;

		while( size-- ) {
			hash = (hash << 4) + *data++;
			wrap = hash & 0xf0000000;
			if( wrap != 0 )
				hash ^= wrap >> 24;
		};
		return hash;
	}

	int AddPoint( Vector p )
	{
		ulong32 key = makeKey(&p);

#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
		hash_multimap<ulong32,PFPoint>::const_iterator iter1;
		pair<hash_multimap<ulong32,PFPoint>::const_iterator,
			hash_multimap<ulong32,PFPoint>::const_iterator> pair1;
#else
		stdext::hash_multimap<ulong32,PFPoint>::const_iterator iter1;
		std::pair<stdext::hash_multimap<ulong32,PFPoint>::const_iterator,
			stdext::hash_multimap<ulong32,PFPoint>::const_iterator> pair1;
#endif

		pair1 = mMeshPointsHash.equal_range( key );

		if( pair1.first != mMeshPointsHash.end() )
			for( iter1 = pair1.first; iter1 != pair1.second; iter1++ )
				if( iter1->second.point == p )
					return iter1->second.index;

#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
		pair<ulong32,PFPoint> pair2;
#else
		std::pair<ulong32,PFPoint> pair2;
#endif
		mMeshPoints.Add( p );
		pair2.first = key;
		pair2.second.point = p;
		pair2.second.index = mMeshPoints.Size() - 1;
		mMeshPointsHash.insert( pair2 );
		return mMeshPoints.Size() - 1;
	};

	// *****************************************************************

#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
	hash_multimap<ulong32,PFEdge> mMeshEdgesHash;
#else
	stdext::hash_multimap<ulong32,PFEdge> mMeshEdgesHash;
#endif

	// This assumes that unsigned long has 32 bits.
	ulong32 makeKeyFromEdge( int* element )
	{
		ulong32 size = 2*sizeof(int);
		unsigned char* data = (unsigned char *)element;
		ulong32 hash = 0;
		ulong32 wrap = 0;

		while( size-- )
		{
			hash = (hash << 4) + *data++;
			wrap = hash & 0xf0000000;
			if( wrap != 0 )
				hash ^= wrap >> 24;
		};
		return hash;
	}

	int AddEdge( PFEdge& e, int triangleIndex, int edgeNum )
	{
		if( e.i[0] > e.i[1] )
		{
			int temp = e.i[0];
			e.i[0] = e.i[1];
			e.i[1] = temp;
		};

		ulong32 key = makeKeyFromEdge( e.i );

#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
		hash_multimap<ulong32,PFEdge>::iterator iter1;
		pair<hash_multimap<ulong32,PFEdge>::iterator,
			hash_multimap<ulong32,PFEdge>::iterator> pair1;
#else
		stdext::hash_multimap<ulong32,PFEdge>::iterator iter1;
		std::pair<stdext::hash_multimap<ulong32,PFEdge>::iterator,
			stdext::hash_multimap<ulong32,PFEdge>::iterator> pair1;
#endif

		pair1 = mMeshEdgesHash.equal_range( key );

		if( pair1.first != mMeshEdgesHash.end() )
			for( iter1 = pair1.first; iter1 != pair1.second; iter1++ )
				if( ( iter1->second.i[0] == e.i[0] ) &&
					( iter1->second.i[1] == e.i[1] ) )
				{
					if( iter1->second.howMany == 1 )
					{
						if( mMeshEdges[iter1->second.index].triangleIndex[0] == triangleIndex )
							return iter1->second.index;
						else
						{
							mMeshEdges[iter1->second.index].triangleIndex[1] = triangleIndex;
							mMeshEdges[iter1->second.index].edgeNum[1] = edgeNum;
							iter1->second.triangleIndex[1] = triangleIndex;
							iter1->second.edgeNum[1] = edgeNum;
						};
					};
					if( iter1->second.howMany > 1 )	// edge is in more than two triangles - critical error - report!!!
						return -1;
					mMeshEdges[iter1->second.index].howMany++;
					iter1->second.howMany++;

					return iter1->second.index;
				};

#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
		pair<ulong32,PFEdge> pair2;
#else
		std::pair<ulong32,PFEdge> pair2;
#endif
		e.howMany = 1;
		e.triangleIndex[0] = triangleIndex;
		e.edgeNum[0] = edgeNum;
		e.triangleIndex[1] = -1;
		e.edgeNum[1] = -1;
		pair2.second.i[0] = e.i[0];
		pair2.second.i[1] = e.i[1];
		mMeshEdges.Add( e );
		pair2.first = key;
		pair2.second.howMany = 1;
		pair2.second.triangleIndex[0] = triangleIndex;
		pair2.second.edgeNum[0] = edgeNum;
		pair2.second.triangleIndex[1] = -1;
		pair2.second.edgeNum[1] = -1;
		pair2.second.index = mMeshEdges.Size() - 1;
		mMeshEdgesHash.insert( pair2 );
		return mMeshEdges.Size() - 1;
	};

	// *****************************************************************

#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
	hash_multimap<ulong32,PFTriangle> mMeshTrianglesHash;
#else
	stdext::hash_multimap<ulong32,PFTriangle> mMeshTrianglesHash;
#endif

	// This assumes that unsigned long has 32 bits.
	ulong32 makeKeyFromTriangle( int* element )
	{
		ulong32 size = 3*sizeof(int);
		unsigned char* data = (unsigned char *)element;
		ulong32 hash = 0;
		ulong32 wrap = 0;

		while( size-- )
		{
			hash = (hash << 4) + *data++;
			wrap = hash & 0xf0000000;
			if( wrap != 0 )
				hash ^= wrap >> 24;
		};
		return hash;
	}

	int AddTriangle( PFTriangle t )
	{
		ulong32 key = makeKeyFromTriangle( t.i );

#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
		hash_multimap<ulong32,PFTriangle>::const_iterator iter1;
		pair<hash_multimap<ulong32,PFTriangle>::const_iterator,
			hash_multimap<ulong32,PFTriangle>::const_iterator> pair1;
#else
		stdext::hash_multimap<ulong32,PFTriangle>::const_iterator iter1;
		std::pair<stdext::hash_multimap<ulong32,PFTriangle>::const_iterator,
			stdext::hash_multimap<ulong32,PFTriangle>::const_iterator> pair1;
#endif

		pair1 = mMeshTrianglesHash.equal_range( key );

		if( pair1.first != mMeshTrianglesHash.end() )
		{
			for( iter1 = pair1.first; iter1 != pair1.second; iter1++ )
			{
				if( t.i[0] == iter1->second.i[0] &&
					t.i[1] == iter1->second.i[1] &&
					t.i[2] == iter1->second.i[2] )
					return -1;
			};
		};

#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
		pair<ulong32,PFTriangle> pair2;
#else
		std::pair<ulong32,PFTriangle> pair2;
#endif
		t.used = 0;
		mMeshTriangles.Add( t );
		pair2.first = key;
		pair2.second = t;
		pair2.second.index = mMeshTriangles.Size() - 1;
		mMeshTrianglesHash.insert( pair2 );
		return mMeshTriangles.Size() - 1;
	};

	void RemoveTriangle( PFTriangle t )
	{
		ulong32 key = makeKeyFromTriangle( t.i );

#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
		hash_multimap<ulong32,PFTriangle>::iterator iter1;
		pair<hash_multimap<ulong32,PFTriangle>::iterator,
			hash_multimap<ulong32,PFTriangle>::iterator> pair1;
#else
		stdext::hash_multimap<ulong32,PFTriangle>::iterator iter1;
		std::pair<stdext::hash_multimap<ulong32,PFTriangle>::iterator,
			stdext::hash_multimap<ulong32,PFTriangle>::iterator> pair1;
#endif

		pair1 = mMeshTrianglesHash.equal_range( key );

		if( pair1.first != mMeshTrianglesHash.end() )
		{
			for( iter1 = pair1.first; iter1 != pair1.second; iter1++ )
			{
				if( t.i[0] == iter1->second.i[0] &&
					t.i[1] == iter1->second.i[1] &&
					t.i[2] == iter1->second.i[2] )
				{
					Assert( iter1->second.index == mMeshTriangles.Size() - 1 );
					mMeshTriangles.RemoveLast();
					mMeshTrianglesHash.erase( iter1 );
					return;
				};
			};
		};
//		GLog.Print("Shit Hit The Fan!!!\n");
//		GLog.Flush();
//		GLog.Close();
		exit(1);
	};

};

//-------------------------------------------------------------------------------------------------------
class Pathfinder2FloorSet
{
public:
	// Creation
	Pathfinder2FloorSet(void) {};
	~Pathfinder2FloorSet(void) { ClearContents(); };

	// Management
	void ClearContents( void ) {
		for( int i = 0; i < mFloors.Size(); ++i )
			delete mFloors[i];
		mFloors.ClearArray();
	};

	// Content filling
#ifdef TRANSGAMING_USE_EDITOR
	void GenerateFloors( void );
#endif
	bool Load( FILE* f, float scale = 1.0f );
	bool Save( FILE* f, float scale = 1.0f );

	// Operations
	void ScaleContents( float scale ) { for( int i = 0; i < mFloors.Size(); ++i ) mFloors[i]->ScaleContents( scale ); };
	void FlipZ( void ) { for( int i = 0; i < mFloors.Size(); ++i ) mFloors[i]->FlipZ(); };

	// Drawing
#ifdef TRANSGAMING_USE_EDITOR
	void Render( void );
#endif

	// User selection
	int PickFloor( const Viewport& viewport, int xm, int ym );
	void Selection( const Viewport& viewport, RECT* rect, bool select );

	// Selection
	void Select( int index, bool select );
	void Select_All( int select )
	{
		mSelectedFloors.ClearArray();
		if( select )
			for( int i = 0; i < mFloors.Size(); i++ )
				mSelectedFloors.Add( i );
	}
	void Select_FloorsOfAreaLowerHigherThan( bool select, bool lower, float area );

	// Operations on sleected
	void Selected_Remove( void );
	void Selected_MakeGrid( float step, DynamicArray<Vector>& arrayToFill )
		{ for( int i = 0; i < mSelectedFloors.Size(); ++i ) mFloors[mSelectedFloors[i]]->MakeGrid( step, arrayToFill ); };

	// To start iteration, put 0 in floor. Iterate until returns 0.
	Pathfinder2Floor* GetNextSelectedFloor( int& floor ) { return ( floor < mSelectedFloors.Size() )?mFloors[mSelectedFloors[floor++]]:0; };

protected:
	// Variables
	DynamicArray<Pathfinder2Floor*> mFloors;
	DynamicArray<int> mSelectedFloors;
};



#endif  //_PATHFINDER2FLOORS_
