// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _WAYPOINT_
#define _WAYPOINT_


#include "Engine.h"
#include "Core/Math/LeMath.h"

class Waypoint
{
public:

	BOOL operator==( const Waypoint& wp ) const
	{
		return (mPosition==wp.mPosition);
	};
/*
	int ReadFrom( FILE* f )
	{
		return( fread(&mPosition, sizeof(Vector),1,f) == 1 );
	};
*/
	bool ReadFrom( FileObject* f )
	{
        return f->Read( &mPosition, sizeof(Vector) )>0?true:false;
	};
/*
	int WriteTo( FILE* f, float scale = 1.0f )
	{
		Vector t = mPosition;
		t *= scale;
		return (fwrite(&t, sizeof(Vector),1,f) == 1 );
	};
*/
	bool WriteTo( FileObject* f, float scale = 1.0f )
	{
		Vector t = mPosition;
		t *= scale;
		return f->Write(&t, sizeof(Vector) )>0?true:false;
	};

	float DistanceFrom( Waypoint& other )
	{
		return (mPosition-other.mPosition).Magnitude();
	};

	float DistanceFromSquared( Waypoint& other )
	{
		return (mPosition-other.mPosition).MagnitudeSquared(); 
	};

	void MakeBox( BoundingBox& b, const float size = 0.1f )
	{
		b.Min = b.Max = mPosition;
        b.Min-=Vector(size);
        b.Max+=Vector(size);
	};

	// Variables

	Vector mPosition;
};



#endif //_WAYPOINT_

