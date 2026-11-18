#include "Engine.h"
#include "Engine/ConvexPolygon.h"
#include "Engine/Mesh.h"

#pragma optimize( "p", on )

void ConvexMeshPolygon::operator+=(const MeshVertex& v)
{ 
	if (VertexCount<MaxPolyVerts)
		Verts[VertexCount++]=v;
}

void ConvexMeshPolygon::Reverse()
{
    const int count = VertexCount/2;
    for (int i=0; i<count; ++i)
    {
        const int swapIndex = (VertexCount-1)-i;
		MeshVertex tmp       = Verts[i];
		Verts[i]         = Verts[swapIndex];
		Verts[swapIndex] = tmp;
    }
}

int ConvexMeshPolygon::Classify(const Plane& plane, const float eps) const
{
	int front=0,back=0,close=0;
	for (int i=0; i<VertexCount; ++i) 
	{
		float d = plane.Distance(Verts[i].Pos);
		if (d > +eps) 
			++front; 
		else
		if (d < -eps) 
			++back; 
		else 
		{
			++close;
			++front;
			++back;			
		}
	}

	if (close==VertexCount) { return EOnThePlane; }
	if (front==VertexCount) { return EInFrontOf; }
	if (back ==VertexCount) { return EInBackOf; }
	return ESpanning;
}

int ConvexMeshPolygon::Split(const Plane& plane, ConvexMeshPolygon& front, ConvexMeshPolygon& back, const float eps) const
{
#define front_side      (0)
#define back_side       (1)
#define on_the_plane    (2)

    int     sides[ConvexPolygon::MaxPolyVerts]; // vertex orientation against the plane
    float   dists[ConvexPolygon::MaxPolyVerts]; // vertex distance to the plane

    front.VertexCount=0;
    back.VertexCount =0;

    // counts for (front,back,on_plane)
    int counts[3] = { 0,0,0 };

    // go through each vertex and determine its orientation
    // and distance to the splitting plane
    int i;

    for (i=0; i<VertexCount; ++i) 
	{
		float d = plane.Distance(Verts[i].Pos);
        dists[i]=d;

        if (d > +eps)
            sides[i]=front_side;
        else
        if (d < -eps)
            sides[i]=back_side;
        else
            sides[i]=on_the_plane;

        ++counts[sides[i]];
    }
    // link last with first for easy wrap
    sides[i]=sides[0];
    dists[i]=dists[0];

    if (counts[front_side]==0)
	{
        back=*this;
        return EInBackOf;
    }
    else
    if (counts[back_side]==0)
	{
        front=*this;
        return EInFrontOf;
    }
    else 
	{
        for (int i=0; i<VertexCount; ++i) 
		{
            const MeshVertex& a=Verts[i];
            // if it's on the plane, just add it to both lists
            // (front,back) and get out
            if (sides[i]==on_the_plane) 
			{
                front.Verts[front.VertexCount++] = a;
                back .Verts[back .VertexCount++] = a;
                continue;
            }

            if (sides[i]==front_side)
                front.Verts[front.VertexCount++] = a;
            else
            if (sides[i]==back_side)
                back .Verts[back .VertexCount++] = a;

            // at this point we know the vertex is either
            // in front or in back of the plane
            // if the next vertex is on the plane or on the same side
            // we don't need to split ...
            if(sides[i+1] == on_the_plane || sides[i+1] == sides[i])
                continue;

            // create a split point
            MeshVertex mid;
            MeshVertex b=Verts[(i+1)%VertexCount]; // next vert and wrap
            const float d=dists[i]/(dists[i]-dists[i+1]);
            for (int j=0; j<3; ++j) 
			{
                if (plane.Normal[j]==1.0f)
                    mid.Pos[j]=+plane.Dist;
                else
                if (plane.Normal[j]== -1.0f)
                    mid.Pos[j]=-plane.Dist;
                else
                    mid.Pos[j]= a.Pos[j] + d*(b.Pos[j]-a.Pos[j]);
            }
			mid.TU.U = a.TU.U + d*(b.TU.U-a.TU.U);
			mid.TU.V = a.TU.V + d*(b.TU.V-a.TU.V);
			mid.Normal = a.Normal + (b.Normal-a.Normal)*d;
			mid.Normal.Normalize();
            //front.Verts.Add(mid);
            //back .Verts.Add(mid);
            front.Verts[front.VertexCount++] = mid;
            back .Verts[back .VertexCount++] = mid;
        }

        // sanity check
        if (front.VertexCount<3)
		{
            return -5;
        }
        if (back.VertexCount<3) 
		{ 
            return -5;
        }
    }

    front.PlaneIndex = PlaneIndex;
    back .PlaneIndex = PlaneIndex;

    return 0;

#undef front_side
#undef back_side
#undef on_the_plane
}

PolygonalMesh::PolygonalMesh(const SimpleMesh& mesh)
{
#ifdef TRANSGAMING_DEINDEX
    if (mesh.isIndexed){
#endif
	    int tris = mesh.Data->Triangles.Size()/3;
	    Polygons.ResizeArray(tris);

	    for (int t=0; t<tris; ++t)
	    {
		    Polygons[t] += mesh.Data->VertList.getVert(mesh.Data->Triangles[t*3+0]);
		    Polygons[t] += mesh.Data->VertList.getVert(mesh.Data->Triangles[t*3+1]);
		    Polygons[t] += mesh.Data->VertList.getVert(mesh.Data->Triangles[t*3+2]);
	    }
#ifdef TRANSGAMING_DEINDEX
    }

    else{
        if (mesh.PrimType == RenderDevice::EPrimTriStrip){
	        int tris = mesh.Data->VertList.Size() - 2;
	        Polygons.ResizeArray(tris);

	        for (int t=0; t<tris; ++t)
	        {
                MeshVertex vtx[3];


                mesh.Data->VertList.getVert(t + 0, vtx[0]);
                mesh.Data->VertList.getVert(t + 1, vtx[1]);
                mesh.Data->VertList.getVert(t + 2, vtx[2]);


                //skip over degenerate triangles
                if (vtx[0].Pos == vtx[1].Pos || vtx[1].Pos == vtx[2].Pos){
                    t++;

                    continue;
                }
		        Polygons[t] += vtx[0];
		        Polygons[t] += vtx[1];
		        Polygons[t] += vtx[2];
	        }
        }

        else if (mesh.PrimType == RenderDevice::EPrimTriList){
	        int tris = mesh.Data->VertList.Size() / 3;
	        Polygons.ResizeArray(tris);

	        for (int t=0; t<tris; ++t)
	        {
		        Polygons[t] += mesh.Data->VertList.getVert(t * 3 + 0);
		        Polygons[t] += mesh.Data->VertList.getVert(t * 3 + 1);
		        Polygons[t] += mesh.Data->VertList.getVert(t * 3 + 2);
	        }
        }
    }
#endif

	RecomputeBox();
}

void PolygonalMesh::RecomputeBox()
{
	Box.Reset();

	for (int p=0; p<Polygons.Size(); ++p)
		for (int v=0; v<Polygons[p].VertexCount; ++v)
			Box += Polygons[p].Verts[v].Pos;
}

void PolygonalMesh::CreateMesh(WorldMesh& mesh) const
{
    int totalVerts = 0;
    int i = 0;


	mesh.Data->Triangles.ClearArray();
#ifndef TRANSGAMING
    mesh.Elems.ClearArray();
#endif


    //count up the number of vertices needed for the mesh
    for (int p = 0; p < Polygons.Size(); p++)
        totalVerts += (Polygons[p].VertexCount - 1) * 3;


    mesh.Data->VertList.setSize(totalVerts, VertexList::VTXFMT_NONE);


	for (int p=0; p<Polygons.Size(); ++p)
	{
		ConvexMeshPolygon& poly = Polygons[p];

#ifdef TRANSGAMING_DEINDEX
        for (int v = 1; v < poly.VertexCount - 1; v++){
            mesh.Data->VertList.setVert(i++, poly.Verts[0]);
            mesh.Data->VertList.setVert(i++, poly.Verts[v]);
            mesh.Data->VertList.setVert(i++, poly.Verts[v + 1]);
            /*mesh.Verts.Add(poly.Verts[0]);
            mesh.Verts.Add(poly.Verts[v]);
            mesh.Verts.Add(poly.Verts[v + 1]);//*/
        }
#else
	    int base =  0;


        for (int v=1; v<poly.VertexCount-1; ++v)
		{
            mesh.Data->VertList.setVert(i++, poly.Verts[0]);
            mesh.Data->VertList.setVert(i++, poly.Verts[v]);
            mesh.Data->VertList.setVert(i++, poly.Verts[v + 1]);
			/*mesh.Verts.Add(poly.Verts[0]);
			mesh.Verts.Add(poly.Verts[v]);
			mesh.Verts.Add(poly.Verts[v+1]);//*/
			mesh.Data->Triangles.Add(base++);
			mesh.Data->Triangles.Add(base++);
			mesh.Data->Triangles.Add(base++);
		}
#endif
/*

		for (int v=0; v<poly.VertexCount; ++v)
		{
			mesh.Verts.Add(poly.Verts[v]);
		}

		for (int t=1; t<poly.VertexCount-1; ++t)
		{
			mesh.Data->Triangles.Add(base);
			mesh.Data->Triangles.Add(base+t);
			mesh.Data->Triangles.Add(base+t+1);
		}
*/
	}

	Assert(mesh.Data->Groups.Size()==1);

	if (mesh.Data->Groups.Size()==1)
	{
		mesh.Data->Groups[0].RenderBase = 0;
#ifdef TRANSGAMING_DEINDEX
		mesh.Data->Groups[0].RenderCount = mesh.Data->VertList.Size() / 3;
        mesh.isIndexed = false;
        mesh.PrimType = RenderDevice::EPrimTriList;
#else
		mesh.Data->Groups[0].RenderCount = mesh.Data->Triangles.Size()/3;
#endif
	}
}


int PolygonalMesh::Split(const Plane& plane, PolygonalMesh& back, PolygonalMesh& front) const
{
	back.Polygons.ClearArray();
	front.Polygons.ClearArray();

    for (int t=0; t<Polygons.Size(); ++t)
	{
		ConvexMeshPolygon frontPoly, backPoly;
		ConvexMeshPolygon &tri = Polygons[t];

		int res = tri.Split(plane, frontPoly, backPoly);

		switch (res) {
			case EInBackOf:
				back.Polygons.Add(tri);
				break;
			case EInFrontOf:
				front.Polygons.Add(tri);
				break;
			case 0:
				back.Polygons.Add(backPoly);
				front.Polygons.Add(frontPoly);
				break;
		}
	}

	back.RecomputeBox();
	front.RecomputeBox();

	if (back.Polygons.Size()==0)
		return EInFrontOf;

	if (front.Polygons.Size()==0)
		return EInBackOf;

	return 0;
}

#pragma optimize( "p", off )
