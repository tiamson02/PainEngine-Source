// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

#include "Engine.h"
#include "PathfinderFloorProducer.h"
#include "Engine/Mesh.h"

PathfinderFloorProducer::PathfinderFloorProducer() : BadEdges(false)
{
}

PathfinderFloorProducer::~PathfinderFloorProducer()
{
	ClearContents();
}

static Vector GetNormal( Vector& v1, Vector& v2, Vector& v3 )
{
	return (v3-v1)^(v2-v1);
}

#ifdef TRANSGAMING_USE_EDITOR
void PathfinderFloorProducer::AddMesh( WorldMesh* mesh, int angleDegrees )
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
				continue;

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
			{
/*
				GLog.Print( "CRITICAL ERROR: mesh name = '%s'\n", mesh->Name.AsChar() );
				GLog.Flush();
				GLog.Close();
				exit(1);
*/
				RemoveTriangle( triangle );	// try to repair the situation
			}
			else
			{
				mMeshTriangles[triangleIndex].t[0] = e0;
				mMeshTriangles[triangleIndex].t[1] = e1;
				mMeshTriangles[triangleIndex].t[2] = e2;
			};
		};
	};

/*
	// For each section
	for( int i = 0; i < mesh->Sections.Size(); i++ )
	{
		Mesh::Section& section = mesh->Sections[i];

		int base = section.RenderBase;
		int count = section.RenderCount;

		char pt = 0;
		PFTriangle triangle;
		PFEdge edge;

		char beforeLastAdded = 0;
		char lastAdded = 0;

		Vector vertPos[3];
		vertPos[0] = mesh->VertList.getVertPos(mesh->Elems[base]);
		vertPos[1] = mesh->VertList.getVertPos(mesh->Elems[base+1]);

		int internalVert[2];

		for( int j = 2; j < count; j++ )
		{
			Vector vertPos3 = mesh->VertList.getVertPos(mesh->Elems[base+j]);
			Vector normal = GetNormal( vertPos[0], vertPos3, vertPos[1] );
			normal.Normalize();

			if( normal.Dot( up ) > (float)cos(angleDegrees*F_PI/180) )	// slope less than 45 degrees
			{
				// Prepare points to add
				if( !lastAdded )
				{
					if( !beforeLastAdded )
					{
						internalVert[pt] = AddPoint( vertPos[pt] );
					};
					internalVert[1-pt] = AddPoint( vertPos[1-pt] );
				};
				int internalVert3 = AddPoint( vertPos3 );

				triangle.i[0] = internalVert[0];
				triangle.i[1] = internalVert3;
				triangle.i[2] = internalVert[1];
				int triangleIndex = AddTriangle( triangle );

				edge.i[0] = internalVert[0];
				edge.i[1] = internalVert3;
				int e0 = AddEdge( edge, triangleIndex, 1 );
				edge.i[0] = internalVert[1];
				edge.i[1] = internalVert3;
				int e1 = AddEdge( edge, triangleIndex, 2 );
				edge.i[0] = internalVert[0];
				edge.i[1] = internalVert[1];
				int e2 = AddEdge( edge, triangleIndex, 0 );

				mMeshTriangles[triangleIndex].t[0] = e0;
				mMeshTriangles[triangleIndex].t[1] = e1;
				mMeshTriangles[triangleIndex].t[2] = e2;

				internalVert[pt] = internalVert3;
				beforeLastAdded = lastAdded;
				lastAdded = 1;
			}
			else
			{
				beforeLastAdded = lastAdded;
				lastAdded = 0;
			};

			vertPos[pt] = vertPos3;
			pt = 1 - pt;
		};
	};
*/
}
#endif

void PathfinderFloorProducer::ClearContents( void )
{
	mMeshPoints.ClearArray();
	mMeshEdges.ClearArray();
	mMeshTriangles.ClearArray();
}

#ifdef TRANSGAMING_USE_EDITOR
void PathfinderFloorProducer::ProduceFloors( DynamicArray<PathfinderFloor*>& floors )
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
		PathfinderFloor* floor = new PathfinderFloor;
		DynamicArray<int> openTriangles;
		DynamicArray<int> openEdges;
		bool badFloor = false;

		while( mMeshTriangles[firstTriangleNotOnFloor].used == 1 )
		{
			firstTriangleNotOnFloor++;
		};

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
				{
					openEdges.Add( curTri.t[j] );
				}
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
					else
					{
						if( !mMeshTriangles[curEdge->triangleIndex[0]].used )
						{
							openTriangles.Add( curEdge->triangleIndex[0] );
							mMeshTriangles[ curEdge->triangleIndex[0]].used = 1;
							--trianglesNotOnFloor;
						};
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
		PathfinderFloorEdge* floorEdge = new PathfinderFloorEdge;

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
			floorEdge->DetermineOutsideInsideEdge( /*floor->mFloorTriangles*/ );

			floor->AddEdge( floorEdge );

			if( openEdges.Size() )
			{
#ifdef TRANSGAMING_PSP
                m_setTag("AI");
#endif
				floorEdge = new PathfinderFloorEdge;
			};
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

int PathfinderFloorProducer::FindNextEdgeGoingRight( int edge )
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
