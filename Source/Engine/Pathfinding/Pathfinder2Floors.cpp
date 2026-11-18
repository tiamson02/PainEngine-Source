// ---------------------------------------------------------------------------
// Copyright (c) 2003 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2003/10 Krystian Galaj
//----------------------------------------------------------------------------

#include "Engine.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Render/Viewport.h"
#include "Engine/Mesh.h"
#include "Render/Video.h"
#include "Pathfinding/Pathfinder2.h"
#include "Pathfinding/Pathfinder2Floors.h"
#include "Engine/World.h"

#if defined( TRANSGAMING_PSP )
#include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
#include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
#include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

#ifdef FLOORS_ENABLED

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
			return 1;
	};
	return 0;
} 

bool Pathfinder2FloorEdge::DetermineOutsideInsideEdge( void )
{
	float sum = 0.f;
	int i=0;
	
	if( mFloorVertices.Size() < 3 ) return false;

	Vector lastVect = mFloorVertices[1] - mFloorVertices[0];
	lastVect.Y = 0.f;
	lastVect.Normalize();
	Vector curVect;
	Vector partVect;

	for( i = 2; i < mFloorVertices.Size(); ++i )
	{
		curVect = mFloorVertices[i] - mFloorVertices[i-1];
		curVect.Y = 0.f;
		curVect.Normalize();

		partVect = lastVect^curVect;

		if( partVect.Y > 1.0f )
			partVect.Y = 1.0f;
		else if( partVect.Y < -1.0f )
			partVect.Y = -1.0f;

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

	if( partVect.Y > 1.0f )
		partVect.Y = 1.0f;
	else if( partVect.Y < -1.0f )
		partVect.Y = -1.0f;

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

	if( partVect.Y > 1.0f )
		partVect.Y = 1.0f;
	else if( partVect.Y < -1.0f )
		partVect.Y = -1.0f;

	add = asin( partVect.Y );
	if( lastVect.Dot( curVect ) < 0 )
	{
		if( add > 0 )
			add = F_PI-add;
		else
			add = -F_PI-add;
	}
	sum += add;
/*
	if( sum > 0 )
		GLog.Print( "Outside edge: %f, verts: %d\n", sum, mFloorVertices.Size() );
	else
		GLog.Print( "Inside edge: %f, verts: %d\n", sum, mFloorVertices.Size() );
*/
	// we don't take into account edges that might have been wrongly calculated
	if( fabs(sum) < 0.3f )
		return false;

	outsideEdge = ( sum > 0 );

	return true;
}

// This assumes that it has been established if it's an outside edge, or an inside edge.

bool Pathfinder2FloorEdge::InsideFloorEdge( const Vector& p )
{
	int crosses = 0;
	int i=0;

	Vector lineEnd;
	lineEnd.X = p.X;
	lineEnd.Y = 0.f;
	lineEnd.Z = 1000000.f;

	for( i = 0; i < mFloorVertices.Size()-1; i++ )
		if( lineCrossesLine2D( p, lineEnd, mFloorVertices[i], mFloorVertices[i+1] ) )
			crosses++;

	if( lineCrossesLine2D( p, lineEnd, mFloorVertices[i], mFloorVertices[0] ) )
		crosses++;

	return ( crosses % 2 == 0 )?(!outsideEdge):outsideEdge;
}

bool Pathfinder2FloorEdge::Load( FILE* f, float scale )
{
	mFloorVertices.ClearArray();

	// Read number of vertices
	int size;

#ifdef TRANSGAMING
	if( TNG_fread( &size, sizeof(int), 1, f ) != 1 ) return false;
#else
	if( fread( &size, sizeof(int), 1, f ) != 1 ) return false;
#endif

	// Read vertices
	Vector v;
	for( int i = 0; i < size; i++ )
	{
#ifdef TRANSGAMING
		if( TNG_fread( &v, sizeof(Vector), 1, f ) != 1 ) return false;
#else
		if( fread( &v, sizeof(Vector), 1, f ) != 1 ) return false;
#endif
		v *= scale;
		mFloorVertices.Add( v );
	};

	// Read outside edger marker
#ifdef TRANSGAMING
	if( TNG_fread( &outsideEdge, sizeof(char), 1, f ) != 1 ) return false;
#else
	if( fread( &outsideEdge, sizeof(char), 1, f ) != 1 ) return false;
#endif

	return true;
}

bool Pathfinder2FloorEdge::Save( FILE* f, float scale )
{
	// Write number of vertices
	int size = mFloorVertices.Size();
#ifdef TRANSGAMING
	if( TNG_fwrite( &size, sizeof(int), 1, f ) != 1 ) return false;
#else
	if( fwrite( &size, sizeof(int), 1, f ) != 1 ) return false;
#endif

	// Write vertices
	for( int i = 0; i < size; i++ )
	{
		Vector t = mFloorVertices[i];
		t *= scale;
#ifdef TRANSGAMING
		if( TNG_fwrite( &t, sizeof(Vector), 1, f ) != 1 ) return false;
#else
		if( fwrite( &t, sizeof(Vector), 1, f ) != 1 ) return false;
#endif

	};

	// Write outside edge marker
#ifdef TRANSGAMING
	if( TNG_fwrite( &outsideEdge, sizeof(char), 1, f ) != 1 ) return false;
#else
	if( fwrite( &outsideEdge, sizeof(char), 1, f ) != 1 ) return false;
#endif
	return true;
}

//-------------------------------------------------------------------------------------------------------

bool Pathfinder2Floor::InsideFloor( const Vector& p )
{
	for( int i = 0; i < mFloorEdges.Size(); i++ )
		if( !mFloorEdges[i]->InsideFloorEdge( p ) )
			return false;
	return true;
}

void Pathfinder2Floor::ClearContents( void )
{
	for( int i = 0; i < mFloorEdges.Size(); ++i )
		delete mFloorEdges[i];
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
			return 0;	// outside of triangle

		p.Y = (float)(t0.Y + a*v10.Y + b* v20.Y);

		return 1;
	}
	else		// should be VERY rare, if at all
		return 0;
}

// Returns true if found triangle to get height.
bool Pathfinder2Floor::SetPointHeightOnFloor( Vector& p )
{
	for( int i = 0; i < mFloorTriangles.Size(); i += 3 )
		if( inTriangle2D( p, mFloorTriangles[i], mFloorTriangles[i+1], mFloorTriangles[i+2] ) )
			return true;
	return false;
}

bool Pathfinder2Floor::Load( FILE* f, float scale )
{
	ClearContents();

	// Read number of triangles
	int size;

#ifdef TRANSGAMING
	if( TNG_fread( &size, sizeof(int), 1, f ) != 1 ) return false;
#else
	if( fread( &size, sizeof(int), 1, f ) != 1 ) return false;
#endif

	// Read triangles
	Vector v;
	for( int i = 0; i < size; i++ )
	{
#ifdef TRANSGAMING
		if( TNG_fread( &v, sizeof(Vector), 1, f ) != 1 ) return false;
#else
		if( fread( &v, sizeof(Vector), 1, f ) != 1 ) return false;
#endif
		v *= scale;
		mFloorTriangles.Add( v );
	};

	// Read number of floor edges
#ifdef TRANSGAMING
	if( TNG_fread( &size, sizeof(int), 1, f ) != 1 ) return false;
#else
	if( fread( &size, sizeof(int), 1, f ) != 1 ) return false;
#endif

	// Read floor edges
	Pathfinder2FloorEdge* e;
	for( int i = 0; i < size; i++ )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		e = new Pathfinder2FloorEdge;
		if( !e->Load(f, scale) )
		{
			delete e;
			return 0;
		};
		mFloorEdges.Add( e );
	};

	return true;
}

bool Pathfinder2Floor::Save( FILE* f, float scale )
{
	// Write number of triangles
	int size = mFloorTriangles.Size();
#ifdef TRANSGAMING
	if( TNG_fwrite( &size, sizeof(int), 1, f ) != 1 ) return false;
#else
	if( fwrite( &size, sizeof(int), 1, f ) != 1 ) return false;
#endif

	// Write triangles
	for( int i = 0; i < size; ++i )
	{
		Vector t = mFloorTriangles[i];
		t *= scale;
#ifdef TRANSGAMING
		if( TNG_fwrite( &t, sizeof(Vector), 1, f ) != 1 ) return false;
#else
		if( fwrite( &t, sizeof(Vector), 1, f ) != 1 ) return false;
#endif
	};

	// Write number of edges
	size = mFloorEdges.Size();
#ifdef TRANSGAMING
	if( TNG_fwrite( &size, sizeof(int), 1, f ) != 1 ) return false;
#else
	if( fwrite( &size, sizeof(int), 1, f ) != 1 ) return false;
#endif

	// Write edges
	for( int i = 0; i < size; i++ )
		if( !mFloorEdges[i]->Save(f, scale) ) return false;

	return true;
}

float Pathfinder2Floor::Area( void )
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
	}

	return sum;
}

void Pathfinder2Floor::MakeGrid( float step, DynamicArray<Vector>& arrayToFill )
{
	float xMin, yMin, xMax, yMax;

	//	GetHorizontalEncompassingRectangle( xMin, yMin, xMax, yMax );
	{
		Vector v;
		xMin = FLT_MAX;	yMin = FLT_MAX;	xMax = -FLT_MAX; yMax = -FLT_MAX;
		for( int i = 0; i < mFloorTriangles.Size(); i++ )
		{
			v = mFloorTriangles[i];
			if( v.X < xMin ) xMin = v.X;
			if( v.X > xMax ) xMax = v.X;
			if( v.Z < yMin ) yMin = v.Z;
			if( v.Z > yMax ) yMax = v.Z;
		}
	}

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
				if( !SetPointHeightOnFloor( v ) )
				{
//					GLog.Print( "Point in floor, couldn't find triangle!\n" );
					v.Y = mFloorTriangles[0].Y;
					Vector dist = v-mFloorTriangles[0];
					dist.Y = 0;
					float distance = dist.MagnitudeSquared();
					for( int k = 1; k < mFloorTriangles.Size(); ++k )
					{
						dist = v-mFloorTriangles[k];
						dist.Y = 0;
						float newDist = dist.MagnitudeSquared();
						if( newDist < distance )
						{
							distance = newDist;
							v.Y = mFloorTriangles[k].Y;
						}
					}

				}
				arrayToFill.Add( v );
				v.Y = 0.0f;
			}
		}
	}
}

void Pathfinder2Floor::ScaleContents( float scale )
{
	for( int i = 0; i < mFloorTriangles.Size(); ++i ) mFloorTriangles[i] *= scale;
	for( int i = 0; i < mFloorEdges.Size(); ++i ) mFloorEdges[i]->ScaleContents( scale );
}

void Pathfinder2Floor::FlipZ( void )
{
	for( int i = 0; i < mFloorTriangles.Size(); ++i ) mFloorTriangles[i].Z *= -1.0;
	for( int i = 0; i < mFloorEdges.Size(); ++i ) mFloorEdges[i]->FlipZ();
}

//-------------------------------------------------------------------------------------------------------

static Vector GetNormal( Vector& v1, Vector& v2, Vector& v3 ) { return (v3-v1)^(v2-v1); };

int Pathfinder2FloorProducer::FindNextEdgeGoingRight( int edge )
{
	const int nextEdge[] = {1,2,0};

	static int lastEdges[128];
	int howManyLastEdges = 1;
	lastEdges[0] = edge;

	int tri = 0;
	int eNum;

	do
	{
		if( mMeshEdges[edge].howMany == 1 )
		{
			tri = mMeshEdges[edge].triangleIndex[0];
			eNum = mMeshEdges[edge].edgeNum[0];
		}
		else
		{
			if( tri == mMeshEdges[edge].triangleIndex[0] )
			{
				tri = mMeshEdges[edge].triangleIndex[1];
				eNum = mMeshEdges[edge].edgeNum[1];
			}
			else
			{
				tri = mMeshEdges[edge].triangleIndex[0];
				eNum = mMeshEdges[edge].edgeNum[0];
			};
		};

		eNum = nextEdge[eNum];
		edge = mMeshTriangles[tri].t[eNum];

		for( int i = 0; i < howManyLastEdges; ++i )
			if( edge == lastEdges[i] )
			{
				GLog.Print( "Faulty open edge; forgetting one floor!\n" );
				return -1;	// error!
			}
		lastEdges[howManyLastEdges++] = edge;
		Assert(howManyLastEdges<128);
	}
	while( mMeshEdges[edge].howMany != 1 );

	return edge;
}

#ifdef TRANSGAMING_USE_EDITOR
void Pathfinder2FloorProducer::AddMesh( WorldMesh* mesh, int angleDegrees )
{
	// Pull all triangles out of mesh. Add their edges into hash.
	// If an edge already is in hash, remove it from there.

	// On the exit, we'll get all the edges of floors in hash.

	// We need pointers to adjacent triangles and adjacent edges to determine the sets
	// of triangles to be used inside each floor line.

	// Then, we'll get the triples: set of points, set of triangles indexed at those points,
	// and the set of edge lines bordering the triangles, also indexed at points.

	// Each of those triples is a single floor.

	// ------------------------------------------------

	const Vector up = Vector(0,1,0);

	// For each triangle
	Vector vertPos[3];
	int vertIndex[3];
	PFTriangle triangle;
	PFEdge edge;
#ifdef TRANSGAMING_DEINDEX
    int increment, limit;


    if (!mesh->isIndexed){
        if (mesh->PrimType == RenderDevice::EPrimTriStrip){
            increment = 1;
            limit = mesh->VertList.Size() - 2;
        }

        else{
            increment = 3;
            limit = mesh->VertList.Size();
        }
    }

    else{
        increment = 3;
        limit = mesh->Triangles.Size();
    }


	for( int i = 0; i < limit; i += increment )
#else
	for( int i = 0; i < mesh->Triangles.Size(); i += 3 )
#endif
	{
#ifdef TRANSGAMING_DEINDEX
        if (!mesh->isIndexed){
		    vertPos[0] = mesh->VertList.getVertPos(i + 0);
		    vertPos[1] = mesh->VertList.getVertPos(i + 1);
		    vertPos[2] = mesh->VertList.getVertPos(i + 2);
        }

        else{
#endif
		    int t = mesh->Triangles[i + 0];
		    if( t >= mesh->VertList.Size() )
		    {
			    GLog.Print( "CRITICAL ERROR: vertex index in triangle out of vertices range! Mesh: '%s'\n", mesh->Name.AsChar() );
			    GLog.Flush();
			    GLog.Close();
			    exit(1);
		    }
		    vertPos[0] = mesh->VertList.getVertPos(t);

		    t = mesh->Triangles[i + 1];
		    if( t >= mesh->VertList.Size() )
		    {
			    GLog.Print( "CRITICAL ERROR: vertex index in triangle out of vertices range! Mesh: '%s'\n", mesh->Name.AsChar() );
			    GLog.Flush();
			    GLog.Close();
			    exit(1);
		    }
		    vertPos[1] = mesh->VertList.getVertPos(t);

		    t = mesh->Triangles[i + 2];
		    if( t >= mesh->VertList.Size() )
		    {
			    GLog.Print( "CRITICAL ERROR: vertex index in triangle out of vertices range! Mesh: '%s'\n", mesh->Name.AsChar() );
			    GLog.Flush();
			    GLog.Close();
			    exit(1);
		    }
		    vertPos[2] = mesh->VertList.getVertPos(t);

#ifdef TRANSGAMING_DEINDEX
        }
#endif

		Vector normal = GetNormal( vertPos[0], vertPos[2], vertPos[1] );
		normal.Normalize();

		if( normal.Dot( up ) < -(float)cos(angleDegrees*F_PI/180) )	// slope less than 45 degrees
		{
			triangle.i[0] = vertIndex[0] = AddPoint( vertPos[0] );
			triangle.i[1] = vertIndex[1] = AddPoint( vertPos[1] );
			triangle.i[2] = vertIndex[2] = AddPoint( vertPos[2] );

			// Revert triangle so first index is lowest
			Assert( triangle.i[0] != triangle.i[1] );
			if( triangle.i[0] > triangle.i[1] )
			{
				Assert( triangle.i[2] != triangle.i[1] );
				if( triangle.i[2] > triangle.i[1] )	// 1 lowest
				{
#ifdef TRANSGAMING
                    int f;
#else
					float f;
#endif
					f = triangle.i[0];
					vertIndex[0] = triangle.i[0] = triangle.i[1];
					vertIndex[1] = triangle.i[1] = triangle.i[2];
					vertIndex[2] = triangle.i[2] = f;
				}
				else	// 2 lowest
				{
#ifdef TRANSGAMING
                    int f;
#else
					float f;
#endif
					f = triangle.i[0];
					vertIndex[0] = triangle.i[0] = triangle.i[2];
					vertIndex[2] = triangle.i[2] = triangle.i[1];
					vertIndex[1] = triangle.i[1] = f;
				}
			}
			else
			{
				Assert( triangle.i[2] != triangle.i[0] );
				if( triangle.i[2] > triangle.i[0] )	// 0 lowest
					; // Do nothing
				else	// 2 lowest
				{
#ifdef TRANSGAMING
                    int f;
#else
					float f;
#endif
					f = triangle.i[0];
					vertIndex[0] = triangle.i[0] = triangle.i[2];
					vertIndex[2] = triangle.i[2] = triangle.i[1];
					vertIndex[1] = triangle.i[1] = f;
				}
			}

			int triangleIndex = AddTriangle( triangle );

			if( triangleIndex == -1 )	// We have this triangle in the database, so forget it
			{
				GLog.Print( "Triangle { (%f,%f,%f), (%f,%f,%f), (%f,%f,%f) } found twice!\n",
					vertPos[0].X, vertPos[0].Y, vertPos[0].Z, 
					vertPos[1].X, vertPos[1].Y, vertPos[1].Z, 
					vertPos[2].X, vertPos[2].Y, vertPos[2].Z
					);
				continue;
			}

			edge.i[0] = vertIndex[0];
			edge.i[1] = vertIndex[1];
			int e0 = AddEdge( edge, triangleIndex, 1 );
			int e1 = -1, e2 = -1;
			if( e0 != -1 )
			{
				edge.i[0] = vertIndex[1];
				edge.i[1] = vertIndex[2];
				e1 = AddEdge( edge, triangleIndex, 2 );
				if( e1 != -1 )
				{
					edge.i[0] = vertIndex[2];
					edge.i[1] = vertIndex[0];
					e2 = AddEdge( edge, triangleIndex, 0 );
				}
			}

			if( e0 == -1 || e1 == -1 || e2 == -1 )
				RemoveTriangle( triangle );	// try to repair the situation
			else
			{
				mMeshTriangles[triangleIndex].t[0] = e0;
				mMeshTriangles[triangleIndex].t[1] = e1;
				mMeshTriangles[triangleIndex].t[2] = e2;
			}
		}
	}
}

void Pathfinder2FloorProducer::ProduceFloors( DynamicArray<Pathfinder2Floor*>& floors )
{
	int trianglesNotOnFloor = mMeshTriangles.Size();
	int firstTriangleNotOnFloor = 0;
	PFEdge* curEdge;
	int erroneousFloors = 0;

	while( trianglesNotOnFloor )
	{
		// 1. Make a set of triangles, starting with the first one from the list, and
		// filling other like in A*. This will give you also a set of edge indices of edges
		// that led to no triangles. Remember to set 'used' markers on taken triangles,
		// to be able to find triangles not used easier.

#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		Pathfinder2Floor* floor = new Pathfinder2Floor;
		DynamicArray<int> openTriangles;
		DynamicArray<int> openEdges;
		bool badFloor = false;

		while( mMeshTriangles[firstTriangleNotOnFloor].used == 1 )
			firstTriangleNotOnFloor++;

		openTriangles.Add( firstTriangleNotOnFloor );

		mMeshTriangles[firstTriangleNotOnFloor].used = 1;
		--trianglesNotOnFloor;

		while( openTriangles.Size() )
		{
			int triangleIndex = openTriangles[openTriangles.Size()-1];
			openTriangles.RemoveLast();
			PFTriangle& curTri = mMeshTriangles[triangleIndex];
			floor->AddTriangle( mMeshPoints[curTri.i[0]], mMeshPoints[curTri.i[1]], mMeshPoints[curTri.i[2]] );

			for( int j = 0; j < 3; j++ )
			{
				curEdge = &mMeshEdges[curTri.t[j]];
				if( curEdge->howMany == 1 )
					openEdges.Add( curTri.t[j] );
				else
				{
					if( curEdge->triangleIndex[0] == triangleIndex )
					{
						if( !mMeshTriangles[curEdge->triangleIndex[1]].used )
						{
							openTriangles.Add( curEdge->triangleIndex[1] );
							mMeshTriangles[ curEdge->triangleIndex[1]].used = 1;
							--trianglesNotOnFloor;
						};
					}
					else if( !mMeshTriangles[curEdge->triangleIndex[0]].used )
					{
						openTriangles.Add( curEdge->triangleIndex[0] );
						mMeshTriangles[ curEdge->triangleIndex[0]].used = 1;
						--trianglesNotOnFloor;
					};
				};
			};
		};

		// 2. Make floor edges. For each floor edge:
		//  - Take the first edge. Remove it from list.
		//  - Move through mesh to find next edge to given.
		//  - Repeat until you find the first edge again.
		// Repeat this until you have no more edges on the list.
		// Remember to input new PathfinderEdges into floor.

#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		Pathfinder2FloorEdge* floorEdge = new Pathfinder2FloorEdge;

		while( openEdges.Size() )
		{
			int startEdge = openEdges[openEdges.Size()-1];
			openEdges.RemoveLast();
			int edge = startEdge;

			bool infiniteLoop = false;
			for( ;; )
			{
				int newEdge = FindNextEdgeGoingRight( edge );
				if( newEdge == -1 )
				{
					// I have to assume that the geometry is somehow erroneous, and forget about this floor
					erroneousFloors++;
					badFloor = true;
					break;
				}
				int vIndex = GetCommonVertexIndex( edge, newEdge );
				floorEdge->AddVertex( mMeshPoints[vIndex] );

				bool openEdgeRemoved = false;
				for( int i = 0; i < openEdges.Size(); i++ )
				{
					if( openEdges[i] == newEdge )
					{
						openEdges.FastRemove(i);
						openEdgeRemoved = true;
						break;
					};
				};
				edge = newEdge;
				if( edge == startEdge )
					break;
				if( !openEdgeRemoved || infiniteLoop )
				{
					// I have to assume that the geometry is somehow erroneous, and forget about this floor
					erroneousFloors++;
					badFloor = true;
					break;
				}
				Assert( openEdgeRemoved );
				Assert( !infiniteLoop );
				if( openEdges.Size() == 0 )
					infiniteLoop = true;
			};

			if( badFloor )	// bailing out - next stage
			{
				delete floorEdge;
				break;
			}

			if( !floorEdge->DetermineOutsideInsideEdge() )
			{
				delete floorEdge;
				break;
			}

			floor->AddEdge( floorEdge );

            if( openEdges.Size() ){
#ifdef TRANSGAMING_PSP
                m_setTag("AI");
#endif
				floorEdge = new Pathfinder2FloorEdge;
            }
		};

		if( badFloor )
		{
			delete floor;
			badFloor = false;
		}
		else
			floors.Add( floor );
	};

	if( erroneousFloors )
		GLog.Print( "Erroneous floors: %d\n", erroneousFloors );
}
#endif

//-------------------------------------------------------------------------------------------------------

#ifdef TRANSGAMING_USE_EDITOR
void Pathfinder2FloorSet::Render( void )
{
	const DWORD colorNormal = 0xa0a0ff;
	const DWORD colorSelected = 0xa00000;
	const DWORD colorWarning = 0xffa0a0;

	Assert( GEngine && GEngine->Video );
	LineRender& lineBuf = GEngine->Video->LineBuf;

	int displayed;

	for( int i = 0; i < mFloors.Size(); ++i )
	{
		displayed = 0;
		for( int j = 0; j < mSelectedFloors.Size(); ++j )
		{
			if( i == mSelectedFloors[j] )
			{
				lineBuf.DrawFloor( *mFloors[i], colorSelected );
				displayed = 1;
				break;
			};
		};

		if( displayed )
			continue;

		if( !mFloors[i]->HowManyEdges() )
			lineBuf.DrawFloor( *mFloors[i], colorWarning );
		else
			lineBuf.DrawFloor( *mFloors[i], colorNormal );
	};
}
#endif

int Pathfinder2FloorSet::PickFloor( const Viewport& viewport, int xm, int ym )
{
	POINT pt;
	pt.x = xm; pt.y = ym;

	float x = float( pt.x );
	float y = float( pt.y );

    // Compute the vector of the pick ray in screen space
    Vector v;
    v.X =  ( ( ( 2.0f * x ) / viewport.Width  ) - 1 ) / viewport.CameraToScreen.A00;
    v.Y = -( ( ( 2.0f * y ) / viewport.Height ) - 1 ) / viewport.CameraToScreen.A11;
    v.Z =  1.0f;

    // Transform the screen space pick ray into 3D space
    RaySegment ray;
    Matrix CameraToWorld = viewport.WorldToCamera.Inverse();
    ray.Dir    = CameraToWorld.TransformNormal( v );
    ray.Origin = CameraToWorld.GetTranslation();

    float dist=FLT_MAX;
    int floorNum = -1;

    for( int i = 0; i < mFloors.Size(); i++ )
	{
		Pathfinder2Floor* floor = mFloors[i];

		const int triCount = floor->HowManyTriangles();
		for( int j = 0; j < triCount; ++j )
		{
			const Vector& v0 = floor->TrianglePoint( 3*j );
			const Vector& v1 = floor->TrianglePoint( 3*j+1 );
			const Vector& v2 = floor->TrianglePoint( 3*j+2 );

			float d = FLT_MAX;
			if( ray.IntersectTriangle( v0, v1, v2, d ) )
			{
				if( d < 0 )
					continue;

				if( d < dist )
				{
					dist = d;
					floorNum = i;
				}
			}
		}
	}

	return floorNum;
}

void Pathfinder2FloorSet::Selection( const Viewport& viewport, RECT* rect, bool select )
{
	if( rect == NULL )
		Select_All( select );
	else
		Select( PickFloor( viewport, rect->left, rect->top ), select );
}

void Pathfinder2FloorSet::Select( int floorIndex, bool select )
{
	if( floorIndex == -1 ) return;

	for( int i = 0; i < mSelectedFloors.Size(); i++ )
		if( mSelectedFloors[i] == floorIndex )
		{
			if( !select )
				mSelectedFloors.FastRemove(i);
			return;
		}
	if( select )
		mSelectedFloors.Add( floorIndex );
}

bool Pathfinder2FloorSet::Load( FILE* f, float scale )
{
	ClearContents();

	// Read floor number
	int size;
#ifdef TRANSGAMING
	if( TNG_fread( &size, sizeof(int), 1, f ) != 1 ) return false;
#else
	if( fread( &size, sizeof(int), 1, f ) != 1 ) return false;
#endif

	for( int i = 0; i < size; ++i )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mFloors.Add( new Pathfinder2Floor );
		if( !mFloors[i]->Load(f, scale) )
		{
			delete mFloors[i];
			mFloors.RemoveLast();
			return false;
		}
	}
	return true;
}

bool Pathfinder2FloorSet::Save( FILE* f, float scale )
{
	// Write floor number
	int size = mFloors.Size();
#ifdef TRANSGAMING
	if( TNG_fwrite( &size, sizeof(int), 1, f ) != 1 ) return false;
#else
	if( fwrite( &size, sizeof(int), 1, f ) != 1 ) return false;
#endif

	for( int i = 0; i < size; ++i )
		if( !mFloors[i]->Save(f,scale) )
			return false;
	return true;
}

void Pathfinder2FloorSet::Select_FloorsOfAreaLowerHigherThan( bool select, bool lower, float area )
{
	if( lower )
	{
		for( int i = 0; i < mFloors.Size(); ++i )
			if( mFloors[i]->Area() < area )
				Select( i, select );
	}
	else
	{
		for( int i = 0; i < mFloors.Size(); ++i )
			if( mFloors[i]->Area() > area )
				Select( i, select );
	}
}

void Pathfinder2FloorSet::Selected_Remove( void )
{
	for( int i = mFloors.Size()-1; i >= 0; --i )
		for( int j = 0; j < mSelectedFloors.Size(); ++j )
			if( mSelectedFloors[j] == i )
			{
				delete mFloors[i];
				mFloors.FastRemove(i);
				mSelectedFloors.FastRemove(j);
				break;
			}
}

#ifdef TRANSGAMING_USE_EDITOR
void Pathfinder2FloorSet::GenerateFloors( void )
{
	int i=0;
#ifdef	TRANSGAMING
	if( !GEngine || !GEngine->pcfWorld ) return;
#else
	if( !GEngine || !GEngine->World ) return;
#endif

	const int slideAngle = 45;
	Pathfinder2FloorProducer producer;

	GLog.Print("Floor generation start\n");
#ifdef TRANSGAMING
	float  time1 = GEngine->SysDriver.GetCurrentTime();
	float  time2;
	float  time3;
#else
	double time1 = GEngine->SysDriver.GetCurrentTime();
	double time2;
	double time3;
#endif

	ClearContents();
#ifdef	TRANSGAMING
	GLog.Print("Adding %d meshes...\n", GEngine->pcfWorld->Meshes.Size() );
	for( i = 0; i < GEngine->pcfWorld->Meshes.Size(); i++ )
		producer.AddMesh( GEngine->pcfWorld->Meshes[i], slideAngle );
#else
	GLog.Print("Adding %d meshes...\n", GEngine->World->Meshes.Size() );
	for( i = 0; i < GEngine->World->Meshes.Size(); i++ )
		producer.AddMesh( GEngine->World->Meshes[i], slideAngle );
#endif

	if( producer.BadEdges )
	{
		GLog.Print( "CRITICAL ERROR: bad edges\n" );
		GLog.Flush();
		GLog.Close();
		exit(1);
	}

	GLog.Print("Producing floors\n" );
	time2 = GEngine->SysDriver.GetCurrentTime();
	producer.ProduceFloors( mFloors );
	time3 = GEngine->SysDriver.GetCurrentTime();

	GLog.Print("Finished producing floors\n" );
	GLog.Print("Mesh addition time: %d\n", time2 - time1 );
	GLog.Print("Produce time: %d\n", time3 - time2 );
	GLog.Flush();
}
#endif

#endif		// FLOORS_ENABLED
