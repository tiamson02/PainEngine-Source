// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

#include "Engine.h"
#include "PathfinderFloor.h"

#if defined( TRANSGAMING_PSP )
#include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
#include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
#include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

#define RAY_HEIGHT 10000.f

PathfinderFloor::PathfinderFloor()
{
}

PathfinderFloor::~PathfinderFloor()
{
	ClearContents();
}

int PathfinderFloor::InsideFloor( const Vector& p )
{
	for( int i = 0; i < mFloorEdges.Size(); i++ )
	{
		if( !mFloorEdges[i]->InsideFloorEdge( p ) )
		{
			return 0;
		};
	};

	return 1;
}

void PathfinderFloor::ClearContents( void )
{
	for( int i = 0; i < mFloorEdges.Size(); i++ )
	{
		delete mFloorEdges[i];
	};
	mFloorEdges.ClearArray();
	mFloorTriangles.ClearArray();
}

// If in triangle, updates height and returns 1
// Else just returns 0

static int inTriangle2D( Vector& p, const Vector& t0, const Vector& t1, const Vector& t2 )
{
	Vector v20 = t2 - t0;
	Vector v10 = t1 - t0;
	Vector v0p = t0 - p;
	
	double det = v20.Z*v10.X - v20.X*v10.Z;

	if( det != 0.0 )
	{
		double a = ( v0p.Z*v20.X - v0p.X*v20.Z ) / det;
		double b = ( v0p.X*v10.Z - v0p.Z*v10.X ) / det;

		if( ( a < 0.0 ) || ( b < 0.0 ) || ( a + b > 1.0 ) )
		{
			return 0;	// outside of triangle
		};

		p.Y = (float)(t0.Y + a*v10.Y + b* v20.Y);

		return 1;
	}
	else		// should be VERY rare, if at all
	{
		return 0;
	};
}

// Returns 0 if not found triangle to get height, 1 otherwise.
int PathfinderFloor::SetPointHeightOnFloor( Vector& p )
{
	for( int i = 0; i < mFloorTriangles.Size(); i+=3 )
	{
		if( inTriangle2D( p, mFloorTriangles[i], mFloorTriangles[i+1], mFloorTriangles[i+2] ) )
		{
			return 1;
		}
	};
	return 0;
}
/*
void PathfinderFloor::Render( RenderDevice* dev, LineBuffer& VBuf, DWORD color )
{
    Assert(dev);

    RenderState state;
    state.Flags=RenderState::ESetConstColor
               |RenderState::ESetObjectTransform;

	state.ConstColor     = color;
	state.ObjectTransform = GIdentityMatrix;

	dev->SetState(state);
    dev->SetColorMode();
    dev->SetStream(VBuf.GetStream());

    // render all triangles
	int toDraw = mFloorTriangles.Size();
	int drawn = 0;
	while( toDraw )
	{
		int toDrawThisTime = (toDraw<1023)?toDraw:1023;
		toDraw -= toDrawThisTime;
		Vector* v = VBuf.Lock( toDrawThisTime );
	    for( int i = 0; i < toDrawThisTime; i++ )
		{
			v[i] = mFloorTriangles[drawn+i];
			v[i].Y += DISPLACEMENT_UP;
		};
		dev->Draw(RenderDevice::EPrimTriList, VBuf.Unlock(), toDrawThisTime/3 );
	};
}
*/
int PathfinderFloor::Load( FILE* f, float scale )
{
	ClearContents();

	// Read number of triangles
	int size;

#ifdef TRANSGAMING
	if( TNG_fread( &size, sizeof(int), 1, f ) != 1 )
#else
	if( fread( &size, sizeof(int), 1, f ) != 1 )
#endif
	{
		return 0;
	};

	// Read triangles
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
		mFloorTriangles.Add( v );
	};

	// Read number of floor edges
#ifdef TRANSGAMING
	if( TNG_fread( &size, sizeof(int), 1, f ) != 1 )
#else
	if( fread( &size, sizeof(int), 1, f ) != 1 )
#endif
	{
		return 0;
	};

	// Read floor edges
	PathfinderFloorEdge* e;
	for( int i = 0; i < size; i++ )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		e = new PathfinderFloorEdge;
		if( !e->Load(f, scale) )
		{
			delete e;
			return 0;
		};
		mFloorEdges.Add( e );
	};

	return 1;
}

int PathfinderFloor::Save( FILE* f, float scale )
{
	// Write number of triangles
	int size = mFloorTriangles.Size();

#ifdef TRANSGAMING
	if( TNG_fwrite( &size, sizeof(int), 1, f ) != 1 )
#else
	if( fwrite( &size, sizeof(int), 1, f ) != 1 )
#endif
	{
		return 0;
	};

	// Write triangles
	for( int i = 0; i < size; i++ )
	{
		Vector t = mFloorTriangles[i];
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

	// Write number of edges
	size = mFloorEdges.Size();
#ifdef TRANSGAMING
	if( TNG_fwrite( &size, sizeof(int), 1, f ) != 1 )
#else
	if( fwrite( &size, sizeof(int), 1, f ) != 1 )
#endif
	{
		return 0;
	};

	// Write edges
	for( int i = 0; i < size; i++ )
	{
		if( !mFloorEdges[i]->Save(f, scale) )
		{
			return 0;
		};
	};

	return 1;
}

float PathfinderFloor::Area( void )
{
	float sum = 0.0f;

	Vector v1;
	Vector v2;
	Vector v3;

	for( int i = 0; i < mFloorTriangles.Size()/3; i++ )
	{
		v1 = mFloorTriangles[3*i+1] - mFloorTriangles[3*i];
		v2 = mFloorTriangles[3*i+2] - mFloorTriangles[3*i];
		v3 = v2^v1;
		sum += ( 0.5f * (float)fabs( v3.Magnitude() ) );
	};

	return sum;
}

void PathfinderFloor::MakeGrid( float step, DynamicArray<Vector>& arrayToFill )
{
	float xMin, yMin, xMax, yMax;

	GetHorizontalEncompassingRectangle( xMin, yMin, xMax, yMax );

	float xCenter = ( xMin + xMax ) / 2.0f;
	float yCenter = ( yMin + yMax ) / 2.0f;
	int xRange = (int)floor( (xMax - xCenter) / step );
	int yRange = (int)floor( (yMax - yCenter) / step );

	Vector v;
	for( int i = -yRange; i <= yRange; i++ )
	{
		v.Z = yCenter + step*i;
		for( int j = -xRange; j <= xRange; j++ )
		{
			v.X = xCenter + step*j;
			if( InsideFloor( v ) )
			{
				if( SetPointHeightOnFloor( v ) )
				{
					arrayToFill.Add( v );
				};
				v.Y = 0.0f;
			};
		};
	};
}

void PathfinderFloor::GetHorizontalEncompassingRectangle( float& xMin, float& yMin, float& xMax, float& yMax )
{
	Vector v;

	xMin = FLT_MAX;	yMin = FLT_MAX;	xMax = -FLT_MAX; yMax = -FLT_MAX;

	for( int i = 0; i < mFloorTriangles.Size(); i++ )
	{
		v = mFloorTriangles[i];

		if( v.X < xMin )
		{
			xMin = v.X;
		};
		if( v.X > xMax )
		{
			xMax = v.X;
		};
		if( v.Z < yMin )
		{
			yMin = v.Z;
		};
		if( v.Z > yMax )
		{
			yMax = v.Z;
		};
	};
}

void PathfinderFloor::ScaleContents( float scale )
{
	// Scale triangles
	for( int i = 0; i < mFloorTriangles.Size(); i++ )
	{
		mFloorTriangles[i] *= scale;
	};

	// Scale number of edges
	int size = mFloorEdges.Size();
	for( int i = 0; i < size; i++ )
	{
		mFloorEdges[i]->ScaleContents( scale );
	};
}

void PathfinderFloor::FlipZ( void )
{
	// Flip triangles
	for( int i = 0; i < mFloorTriangles.Size(); i++ )
	{
		mFloorTriangles[i].Z *= -1.0;
	};

	// Scale number of edges
	int size = mFloorEdges.Size();
	for( int i = 0; i < size; i++ )
	{
		mFloorEdges[i]->FlipZ();
	};
}
