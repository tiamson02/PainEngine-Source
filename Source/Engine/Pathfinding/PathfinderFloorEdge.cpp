// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

#include "Engine.h"
#include "PathfinderFloorEdge.h"

#if defined( TRANSGAMING_PSP )
#include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
#include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
#include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

#define LINE_END 1000000.f

PathfinderFloorEdge::PathfinderFloorEdge()
{
}

PathfinderFloorEdge::~PathfinderFloorEdge()
{

}

void PathfinderFloorEdge::DetermineOutsideInsideEdge( void /*DynamicArray<Vector>& floorPoints*/ )
{
	/*
	outsideEdge = 1;
	for( int i = 0; i < floorPoints.Size(); i++ )
	{
		if( InsideFloorEdge( floorPoints[i] ) )
		{
			return;
		};
	};
	outsideEdge = 0;
*/
	float sum = 0.f;
	int i = 0;
	
	if( mFloorVertices.Size() < 3 )
	{
		return;
	};

	Vector lastVect = mFloorVertices[1] - mFloorVertices[0];
	lastVect.Y = 0.f;
	lastVect.Normalize();
	Vector curVect;
	Vector partVect;

	for( i = 2; i < mFloorVertices.Size(); i++ )
	{
		curVect = mFloorVertices[i] - mFloorVertices[i-1];
		curVect.Y = 0.f;
		curVect.Normalize();

		partVect = lastVect^curVect;

		float add = asin( partVect.Y );
		if( lastVect.Dot( curVect ) < 0 )
		{
			if( add > 0 )
				add = F_PI-add;
			else
				add = -F_PI-add;
		}
		sum += add;

		lastVect = curVect;
	};

	curVect = mFloorVertices[0] - mFloorVertices[i-1];
	curVect.Y = 0.f;
	curVect.Normalize();

	partVect = lastVect^curVect;

	float add = asin( partVect.Y );
	if( lastVect.Dot( curVect ) < 0 )
	{
		if( add > 0 )
			add = F_PI-add;
		else
			add = -F_PI-add;
	}
	sum += add;

	lastVect = curVect;

	curVect = mFloorVertices[1] - mFloorVertices[0];
	curVect.Y = 0.f;
	curVect.Normalize();

	partVect = lastVect^curVect;

	add = asin( partVect.Y );
	if( lastVect.Dot( curVect ) < 0 )
	{
		if( add > 0 )
			add = F_PI-add;
		else
			add = -F_PI-add;
	}
	sum += add;

	outsideEdge = ( sum > 0 );
}

static int lineCrossesLine2D( const Vector& line1start, const Vector& line1end, const Vector& line2start, const Vector& line2end )
{
	double d1x, d1y, dpx, dpy, d2x, d2y;
	double dt1, dt2, dtd;
	double t1,t2;

	d1x = line1end.X - line1start.X;
	d1y = line1end.Z - line1start.Z;
	d2x = line2end.X - line2start.X;
	d2y = line2end.Z - line2start.Z;
	dpx = line2start.X - line1start.X;
	dpy = line2start.Z - line1start.Z;

	dt1 = d2x*dpy-d2y*dpx;
	dt2 = d1x*dpy-d1y*dpx;
	dtd = d2x*d1y-d2y*d1x;

	if( dtd != 0.0 )
	{
		t1 = dt1/dtd;
		t2 = dt2/dtd;
		if( ( t1 >= 0.0 ) && ( t1 <= 1.0 ) && ( t2 >= 0.0 ) && ( t2 <= 1.0 ) )
		{
			return 1;
		};
	};
	return 0;
} 

// This assumes that it has been established if it's an outside edge, or an inside edge.

int PathfinderFloorEdge::InsideFloorEdge( const Vector& p )
{
	int crosses = 0;
	int i = 0;
	
	Vector lineEnd;
	lineEnd.X = p.X;
	lineEnd.Y = 0.f;
	lineEnd.Z = LINE_END;

	for( i = 0; i < mFloorVertices.Size()-1; i++ )
	{
		if( lineCrossesLine2D( p, lineEnd, mFloorVertices[i], mFloorVertices[i+1] ) )
		{
			crosses++;
		};
	};
	if( lineCrossesLine2D( p, lineEnd, mFloorVertices[i], mFloorVertices[0] ) )
	{
		crosses++;
	};

	return ( crosses % 2 == 0 )?(!outsideEdge):outsideEdge;
}

int PathfinderFloorEdge::Load( FILE* f, float scale )
{
	mFloorVertices.ClearArray();

	// Read number of vertices
	int size;

#ifdef TRANSGAMING
	if( TNG_fread( &size, sizeof(int), 1, f ) != 1 )
#else
	if( fread( &size, sizeof(int), 1, f ) != 1 )
#endif
	{
		return 0;
	};

	// Read vertices
	Vector v;
	for( int i = 0; i < size; i++ )
	{
#ifdef TRANSGAMING
		if( TNG_fread( &v, sizeof(Vector), 1, f ) != 1 )
#else
		if( fread( &v, sizeof(Vector), 1, f ) != 1 )
#endif
		{
			return 0;
		};
		v *= scale;
		mFloorVertices.Add( v );
	};

	// Read outside edger marker
#ifdef TRANSGAMING
	if( TNG_fread( &outsideEdge, sizeof(char), 1, f ) != 1 )
#else
	if( fread( &outsideEdge, sizeof(char), 1, f ) != 1 )
#endif
	{
		return 0;
	};

	return 1;
}

int PathfinderFloorEdge::Save( FILE* f, float scale )
{
	// Write number of vertices
	int size = mFloorVertices.Size();
#ifdef TRANSGAMING
	if( TNG_fwrite( &size, sizeof(int), 1, f ) != 1 )
#else
	if( fwrite( &size, sizeof(int), 1, f ) != 1 )
#endif
	{
		return 0;
	};

	// Write vertices
	for( int i = 0; i < size; i++ )
	{
		Vector t = mFloorVertices[i];
		t *= scale;
#ifdef TRANSGAMING
		if( TNG_fwrite( &t, sizeof(Vector), 1, f ) != 1 )
#else
		if( fwrite( &t, sizeof(Vector), 1, f ) != 1 )
#endif
		{
			return 0;
		};
	};

	// Write outside edge marker
#ifdef TRANSGAMING
	if( TNG_fwrite( &outsideEdge, sizeof(char), 1, f ) != 1 )
#else
	if( fwrite( &outsideEdge, sizeof(char), 1, f ) != 1 )
#endif
	{
		return 0;
	};

	return 1;
}

void PathfinderFloorEdge::ScaleContents( float scale )
{
	int size = mFloorVertices.Size();
	for( int i = 0; i < size; i++ )
	{
		mFloorVertices[i] *= scale;
	};
}

void PathfinderFloorEdge::FlipZ( void )
{
	int size = mFloorVertices.Size();
	for( int i = 0; i < size; i++ )
	{
		mFloorVertices[i].Z *= -1.0;
	};
}
