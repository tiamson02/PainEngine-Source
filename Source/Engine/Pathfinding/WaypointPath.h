// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _WAYPOINTPATH_
#define _WAYPOINTPATH_


class Pathfinder;

#include "Engine.h"
#include "Core/Math/LeMath.h"
#include "Waypoint.h"

class ENGINE_API WaypointPath
{
#ifdef TRANSGAMING_USE_EDITOR
	friend class LineRender;
#endif
public:
	WaypointPath(void) {};
	WaypointPath( const WaypointPath& path ) { *this = path; };
	~WaypointPath(void) {};

	void AddAtEnd( Waypoint* wp )
	{
		mPath.Add(wp);
	};

	void Empty( void )
	{
		mPath.ClearArray();
	};

	int  Size( void )
	{
		return mPath.Size();
	};

	Vector GetNextFromPath( void )
	{
		Vector v = mPath.Size()?mPath[mPath.Size()-1]->mPosition:Vector(0,0,0);
		mPath.RemoveLast();
		return v;
	};

	Vector PeekNextFromPath( void )
	{
		return mPath.Size()?mPath[mPath.Size()-1]->mPosition:Vector(0,0,0);
	};

	WaypointPath& operator=( const WaypointPath& path )
	{
		mPath = path.mPath;
		return *this;
	};

	int PeekNextIndex( Pathfinder* pf, int zoneIndex );
	int PeekIndex( Pathfinder* pf, int zoneIndex, int index );

protected:
	// Variables
	DynamicArray<Waypoint*> mPath;
};



#endif //_WAYPOINTPATH_



