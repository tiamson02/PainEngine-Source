#include "Engine.h"

#include "Engine/Mesh.h"
#include "Portal.h"
#include "Engine/World.h"
#include "Physics/PhysicsEngine.h"
#include "Core/FileSystem/LogBuffer.h"

extern ENGINE_API LogBuffer GLog;

int AntiPortal::Counter = 0;

AntiPortal::AntiPortal( WorldMesh* mesh ) : SceneRenderConvexVolume(0), Disabled(false)
{
	Name = String::Sprintf( "%s_antyp%d", mesh->Name.AsChar(), Counter++ );

	// 1. Transfer verts from mesh to new table
	DynamicArray<Vector> vertices;

	if( mesh->GetPosition().MagnitudeSquared() )
	{
		Matrix& otw = mesh->ObjectToWorld;
		for( int i = 0; i < mesh->Data->VertList.Size(); ++i )
            vertices.Add( otw.TransformCoord( mesh->Data->VertList.getVertPos(i) * Vector(1,1,0.9f)) ); // thinner then mesh
	}
	else
	{
		for( int i = 0; i < mesh->Data->VertList.Size(); ++i )
			vertices.Add( mesh->Data->VertList.getVertPos(i) );
	}
#ifndef TRANSGAMING_NO_HAVOK
	// 2. Create convex geometry on it ( ie. new verts & triangles )
	// 3. Incorporate new verts and triangles into antiportal
	if( !CreateConvexGeometry( vertices, Vertices, Triangles ) )
	{
		GLog.Print( "WARNING: degenerate mesh '%s' used to make an antiportal!!!!\n", mesh->Name.AsChar() );
		return;
	}
#endif
	// 4. Build the rest of data
	Box = mesh->Box;
	BuildConvex();
}

/*
AntiPortal::AntiPortal( WorldMesh* mesh ) : SceneRenderConvexVolume(0), Disabled(false)
{
	static int counter = 0;
	Name = String::Sprintf( "%s_antyp%d", mesh->Name.AsChar(), counter++ );

	DynamicArray<int> vertexTranslator;
	if( mesh->GetPosition().MagnitudeSquared() )
	{
		Matrix& otw = mesh->ObjectToWorld;
		for( int i = 0; i < mesh->Data->VertList.Size(); ++i )
		{
            Vector v = otw.TransformCoord( mesh->Data->VertList.getVertPos(i) * Vector(1,1,0.9f)) ; // thinner then mesh
			int j;
			for( j = 0; j < Vertices.Size(); ++j )
				if( Vertices[j] == v )
					break;
			vertexTranslator.Add( j );
			if( j == Vertices.Size() )
				Vertices.Add( v );
		}
	}
	else
	{
		for( int i = 0; i < mesh->Data->VertList.Size(); ++i )
		{
            Vector v = mesh->Data->VertList.getVertPos(i);
			int j;
			for( j = 0; j < Vertices.Size(); ++j )
				if( Vertices[j] == v )
					break;
			vertexTranslator.Add( j );
			if( j == Vertices.Size() )
				Vertices.Add( v );
		}
	}

	for( int i = 0; i < mesh->Data->Triangles.Size(); ++i )
		Data->Triangles.Add( vertexTranslator[mesh->Data->Triangles[i]] );

	Box = mesh->Box;
	BuildConvex();
}
*/

void Portal::BuildFromMesh(World* world, const WorldMesh* mesh)
{
	Assert(mesh);

	Name = mesh->Name;
	Box = mesh->Box;

	Poly.VertexCount = 0;

#ifdef TRANSGAMING_DEINDEX
    if (!mesh->isIndexed){
	    int triCount;
            
        if (mesh->PrimType == RenderDevice::EPrimTriList)
            triCount = mesh->Data->VertList.Size()/3;

        else
            triCount = mesh->Data->VertList.Size() - 2;


	    for (int i=0; i<triCount; ++i) {
		    for (int ei=0; ei<3; ++ei) {
			    Vector e0[2];
			    int found = 0;


                if (mesh->PrimType == RenderDevice::EPrimTriList){
		            e0[0] = mesh->Data->VertList.getVertPos(i * 3 + ei);
		            e0[1] = mesh->Data->VertList.getVertPos(i * 3 + ((ei + 1) % 3));
                }

                else{
		            e0[0] = mesh->Data->VertList.getVertPos(i + ei);
		            e0[1] = mesh->Data->VertList.getVertPos(i + ((ei + 1) % 3));
                }


			    for (int j=0; j<triCount; ++j) {
				    if (i == j)
					    continue;

				    for (int ej=0; ej<3; ++ej) {
					    Vector e1[2];


                        if (mesh->PrimType == RenderDevice::EPrimTriList){
					        e1[0] = mesh->Data->VertList.getVertPos(j * 3 + ej);
					        e1[1] = mesh->Data->VertList.getVertPos(j * 3 + ((ej + 1) % 3));
                        }

                        else{
					        e1[0] = mesh->Data->VertList.getVertPos(j + ej);
					        e1[1] = mesh->Data->VertList.getVertPos(j + ((ej + 1) % 3));
                        }


                        //check if these two pairs represent a shared edge in the mesh
					    if ((e0[0] == e1[0] && e0[1] == e1[1])
						    || (e0[0] == e1[1] && e0[1] == e1[0])) 
					    {
						    found = 1;
						    j = triCount;
						    break;
					    }

				    }
			    }

			    if (!found) {
				    Poly.Verts[Poly.VertexCount++] = e0[0];
			    }
		    }
	    }
    }

    else{
#endif
        //!!// NOTE: this does NOT handle tristrips!
	    const int triCount = mesh->Data->Triangles.Size()/3;
	    for (int i=0; i<triCount; ++i) {
		    for (int ei=0; ei<3; ++ei) {
			    int e0[2];

			    e0[0] = mesh->Data->Triangles[i*3+ei];
			    e0[1] = mesh->Data->Triangles[i*3+((ei+1)%3)];

			    int found = 0;

			    for (int j=0; j<triCount; ++j) {
				    if (i == j)
					    continue;

				    for (int ej=0; ej<3; ++ej) {
					    int e1[2];

					    e1[0] = mesh->Data->Triangles[j*3+ej];
					    e1[1] = mesh->Data->Triangles[j*3+((ej+1)%3)];

					    if ((e0[0] == e1[0] && e0[1] == e1[1])
						    || (e0[0] == e1[1] && e0[1] == e1[0])) 
					    {
						    found = 1;
						    j = triCount;
						    break;
					    }

				    }
			    }

			    if (!found) {
				    Poly.Verts[Poly.VertexCount++] = mesh->Data->VertList.getVertPos(e0[0]);
			    }
		    }
	    }
#ifdef TRANSGAMING_DEINDEX
    }
#endif

	FixVertexOrder();

	PlaneIndex = world->CreatePlane
	(
		Poly.Verts[0],
		Poly.Verts[1],
		Poly.Verts[2]
	);

	BuildOtherPoly();     
}
