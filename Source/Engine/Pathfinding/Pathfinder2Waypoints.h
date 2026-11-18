// ---------------------------------------------------------------------------
// Copyright (c) 2002-2003 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2003/10 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _PATHFINDER2WAYPOINTS_
#define _PATHFINDER2WAYPOINTS_


class Pathfinder2;

#include "Engine.h"
#include "Core/Math/LeMath.h"

class ENGINE_API Waypoint2
{
public:
	// Members
	Vector			Position;
	unsigned short	Set:12;
	bool			Disabled:1;
	bool			Selected:1;
	bool			BigMonsters:1;
	bool			Border:1;

	int				Base;
	int				Count;

	// Methods - mainly shortcuts
	inline float Dist( Waypoint2& other ) const { return (Position-other.Position).Magnitude(); };
	inline float DistSquared( Waypoint2& other ) const { return (Position-other.Position).MagnitudeSquared(); };

	inline void MakeBox( BoundingBox& b, const float size = 0.1f ) const
	{
		b.Min = b.Max = Position;
		b.Min -= Vector( size );
		b.Max += Vector( size );
	};

	inline bool InBox( const BoundingBox& b ) const { return ( b.Intersect( Position ) != 0 ); };

	bool operator==( const Waypoint2& wp ) const { return ( Position == wp.Position ) != 0; };

	bool SaveTo( FileObject* f, float scale = 1.0f )
	{
		Vector t = Position;
		t *= scale;
		if( !f->Write(&t, sizeof(Vector) ) ) return false;

		unsigned short set = Set;
		if( !f->Write(&set, sizeof(unsigned short) ) ) return false;

		bool b = BigMonsters;
		if( !f->Write(&b, sizeof(bool) ) ) return false;

		int base = Base;
		if( !f->Write(&base, sizeof(int) ) ) return false;

		int count = Count;
		if( !f->Write(&count, sizeof(int) ) ) return false;

		return true;
	};

	bool LoadFrom( FileObject* f )
	{
		if( !f->Read( &Position, sizeof(Vector) ) ) return false;

		unsigned short set;
		if( !f->Read( &set, sizeof(unsigned short) ) ) return false;
		Set = set;

		bool b;
		if( !f->Read( &b, sizeof(bool) ) ) return false;
		BigMonsters = b;

		int base;
		if( !f->Read( &base, sizeof(int) ) ) return false;
		Base = base;

		int count;
		if( !f->Read( &count, sizeof(int) ) ) return false;
		Count = count;

		Disabled = false;
		Selected = false;
		Border = false;

		return true;
	};
};

//-------------------------------------------------------------------------------------------------------

class WaypointGPath2;

class WaypointPath2
{
#ifdef TRANSGAMING_USE_EDITOR
	friend class LineRender;
#endif

public:
	WaypointPath2( void ) : minDist(0.0f), maxDist(10000.0f) {};
	WaypointPath2( const WaypointPath2& path ) { *this = path; };
	WaypointPath2( WaypointGPath2* path );

	~WaypointPath2( void ) {};

	void AddAtEnd( Waypoint2* wp ) { mPath.Add( wp ); };
	void Empty( void ) { mPath.ClearArray(); };
	int  Size( void ) { return mPath.Size(); };

	Vector GetNextFromPath( void )
	{
		Vector v = mPath.Size()?mPath[mPath.Size()-1]->Position:Vector(0,0,0);
		mPath.RemoveLast();
		return v;
	};

	Vector PeekNextFromPath( void ) { return mPath.Size()?mPath[mPath.Size()-1]->Position:Vector(0,0,0); };
	WaypointPath2& operator=( const WaypointPath2& path )
	{
		mPath = path.mPath;
		minDist = path.minDist;
		maxDist = path.maxDist;
		return *this;
	};

	int PeekNextIndex( Pathfinder2* pf );
	int PeekIndex( Pathfinder2* pf, int index );

	void SetDistances( WaypointGPath2* path );
	inline bool DistanceOk( float distance ) { return ( distance > minDist ) && ( distance < maxDist ); };

protected:
	// Variables
	DynamicArray<Waypoint2*> mPath;
	float minDist;
	float maxDist;
};



#endif  //_PATHFINDER2WAYPOINTS_
