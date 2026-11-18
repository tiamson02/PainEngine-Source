// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _PATHFINDERFLOORPRODUCER_
#define _PATHFINDERFLOORPRODUCER_

#include "Core/nommgr.h"
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
#include <ext/hash_map>
using namespace::__gnu_cxx;
#else
#include <hash_map>
#endif

#include "Core/mmgr.h"

class WorldMesh;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "PathfinderFloor.h"

class PathfinderFloorProducer
{

	struct PFPoint
	{
		Vector point;
		int index;
	};

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
	PathfinderFloorProducer(void);
	~PathfinderFloorProducer(void);

	void ClearContents( void );
#ifdef TRANSGAMING_USE_EDITOR
	void AddMesh( WorldMesh* mesh, int angleDegrees );
	void ProduceFloors( DynamicArray<PathfinderFloor*>& floors );
#endif

	bool BadEdges;

protected:

/*
	// This adds the point to the object, and returns its index. It handles
	// repeating points, not adding them and returning index of previous occurrence.
	int AddPoint( Vector p )
	{
		for( int i = 0; i < mMeshPoints.Size(); i++ )
		{
			if( p == mMeshPoints[i] )
			{
				return i;
			};
		};
		mMeshPoints.Add( p );
		return mMeshPoints.Size() - 1;
	}
*/
/*
	int AddEdge( PFEdge& e, int triangleIndex, int edgeNum )
	{
		if( e.i[0] > e.i[1] )
		{
			int temp = e.i[0];
			e.i[0] = e.i[1];
			e.i[1] = temp;
		};

		for( int j = 0; j < mMeshEdges.Size(); j++ )
		{
			if( e.i[0] == mMeshEdges[j].i[0] && e.i[1] == mMeshEdges[j].i[1] )
			{
				if( mMeshEdges[j].howMany == 1 )
				{
					mMeshEdges[j].triangleIndex[1] = triangleIndex;
					mMeshEdges[j].edgeNum[1] = edgeNum;
				};
				mMeshEdges[j].howMany++;
				return j;
			};
		};
		e.howMany = 1;
		e.triangleIndex[0] = triangleIndex;
		e.edgeNum[0] = edgeNum;
		e.triangleIndex[1] = -1;
		e.edgeNum[1] = -1;
		mMeshEdges.Add( e );
		return mMeshEdges.Size() - 1;
	}
*/
	/*
	int AddTriangle( PFTriangle t )
	{
		for( int j = 0; j < mMeshTriangles.Size(); j++ )
		{
			if( t.i[0] == mMeshTriangles[j].i[0] &&
				t.i[1] == mMeshTriangles[j].i[1] &&
				t.i[2] == mMeshTriangles[j].i[2] )
			{
				return j;
			};
		};
		t.used = 0;
		mMeshTriangles.Add( t );
		return mMeshTriangles.Size() - 1;
	}
*/
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
	// Faster approach - to try
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
		{
			for( iter1 = pair1.first; iter1 != pair1.second; iter1++ )
			{
				if( iter1->second.point == p )
				{
					return iter1->second.index;
				};
			};
		};

		std::pair<ulong32,PFPoint> pair2;
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

		while( size-- ) {
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
		{
			for( iter1 = pair1.first; iter1 != pair1.second; iter1++ )
			{
				if( ( iter1->second.i[0] == e.i[0] ) &&
					( iter1->second.i[1] == e.i[1] ) )
				{
					if( iter1->second.howMany == 1 )
					{
						if( mMeshEdges[iter1->second.index].triangleIndex[0] == triangleIndex )
						{
							return iter1->second.index;
						}
						else
						{
							mMeshEdges[iter1->second.index].triangleIndex[1] = triangleIndex;
							mMeshEdges[iter1->second.index].edgeNum[1] = edgeNum;
							iter1->second.triangleIndex[1] = triangleIndex;
							iter1->second.edgeNum[1] = edgeNum;
						};
					};
					if( iter1->second.howMany > 1 )	// edge is in more than two triangles - critical error - report!!!
					{
//						Vector& p1 = mMeshPoints[iter1->second.i[0]];
//						Vector& p2 = mMeshPoints[iter1->second.i[1]];
//						GLog.Print( "CRITICAL ERROR: Edge between points (%f,%f,%f) and (%f,%f,%f) belongs to more than two triangles!!!\n",
//							p1.X,p1.Y,p1.Z,p2.X,p2.Y,p2.Z );
//						GLog.Flush();
//						BadEdges = true;
						return -1;
					}
					mMeshEdges[iter1->second.index].howMany++;
					iter1->second.howMany++;

					return iter1->second.index;
				};
			};
		};

		std::pair<ulong32,PFEdge> pair2;
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

		while( size-- ) {
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
				{
//					return iter1->second.index;
					return -1;
				};
			};
		};

		std::pair<ulong32,PFTriangle> pair2;
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
		GLog.Print("Shit Hit The Fan!!!\n");
		GLog.Flush();
		GLog.Close();
		exit(1);
	};

};



#endif //_PATHFINDERFLOORPRODUCER_

