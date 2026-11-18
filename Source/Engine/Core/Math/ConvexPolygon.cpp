#include "Engine.h"

#include "Core/Math/LeMath.h"

#pragma optimize( "p", on )

void ConvexPolygon::operator+=(const Vector& v)
{ 
	if (VertexCount<MaxPolyVerts)
		Verts[VertexCount++]=v;
}

void ConvexPolygon::Reverse()
{
    const int count = VertexCount/2;
    for (int i=0; i<count; ++i)
    {
        const int swapIndex = (VertexCount-1)-i;
		Vector tmp       = Verts[i];
		Verts[i]         = Verts[swapIndex];
		Verts[swapIndex] = tmp;
    }
}

int ConvexPolygon::Classify(const Plane& plane, const float eps) const
{
	int front=0,back=0,close=0;
	for (int i=0; i<VertexCount; ++i) 
	{
		float d = plane.Distance(Verts[i]);
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

int ConvexPolygon::Split(const Plane& plane, ConvexPolygon& front, ConvexPolygon& back, const float eps) const
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
		float d = plane.Distance(Verts[i]);
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
            const Vector& a=Verts[i];
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
            Vector mid;
            Vector b=Verts[(i+1)%VertexCount]; // next vert and wrap
            const float d=dists[i]/(dists[i]-dists[i+1]);
            for (int j=0; j<3; ++j) 
			{
                if (plane.Normal[j]==1.0f)
                    mid[j]=+plane.Dist;
                else
                if (plane.Normal[j]== -1.0f)
                    mid[j]=-plane.Dist;
                else
                    mid[j]= a[j] + d*(b[j]-a[j]);
            }
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


BOOL ConvexPolygon::CreateFromPlane(const Plane& plane, float extent)
{
	float v, max = -HUGE_NUMBER;
	int x = -1;

    // find the major axis
	for (int i=0; i<3; ++i)
	{
		v = fabsf(plane.Normal[i]);
		if (v > max)
		{
			x = i;
			max = v;
		}
	}

	if (x==-1)
    {
        VertexCount = 0;
		return FALSE;
    }
		
    Vector up(0,0,0);

	switch (x)
	{
	case 0:
	case 1:
		up[2] = 1;
		break;		
	case 2:
		up[0] = 1;
		break;		
	}

	//v = DotProduct (vup, normal);
    v = up.Dot(plane.Normal);
	//VectorMA (vup, -v, normal, vup);
    up = up + plane.Normal*(-v);
	//VectorNormalize (vup, vup);
    up.Normalize();

	//VectorScale (normal, dist, org);
    Vector org = plane.Normal*plane.Dist;
	//CrossProduct (vup, normal, vright);
	Vector right = up^plane.Normal;
	
	//VectorScale (vup, MAX_WORLD_COORD, vup);
    up *= extent;
	//VectorScale (vright, MAX_WORLD_COORD, vright);
    right *= extent;

/*
// project a really big	axis aligned box onto the plane
	w = AllocWinding (4);
	
	VectorSubtract (org, vright, w->p[0]);
	VectorAdd (w->p[0], vup, w->p[0]);
	
	VectorAdd (org, vright, w->p[1]);
	VectorAdd (w->p[1], vup, w->p[1]);
	
	VectorAdd (org, vright, w->p[2]);
	VectorSubtract (w->p[2], vup, w->p[2]);
	
	VectorSubtract (org, vright, w->p[3]);
	VectorSubtract (w->p[3], vup, w->p[3]);
	
	w->numpoints = 4;
*/

    Verts[0] = org - right + up;
    Verts[1] = org + right + up;
    Verts[2] = org + right - up;
    Verts[3] = org - right - up;

    VertexCount = 4;
    return TRUE;
}

ConvexVolume ConvexPolygon::Extrude( float depth )
{
	ConvexVolume volume;
	Assert( depth > 0.0f );
	Assert( VertexCount > 2 );

	// Calculate normal
	Plane tempPlane;
	tempPlane = Plane( Verts[0], Verts[1], Verts[2] );
	Vector extrusionNormal = tempPlane.Normal;
	extrusionNormal.Normalize();
	extrusionNormal *= depth;

	// Create front plane from 3 points
	Vector tempPoint1, tempPoint2, tempPoint3;
	tempPoint1 = Verts[0] + extrusionNormal;
	tempPoint2 = Verts[1] + extrusionNormal;
	tempPoint3 = Verts[2] + extrusionNormal;
	volume.BoundingPlanes[volume.PlaneCount++] = Plane( tempPoint1, tempPoint2, tempPoint3 );

	// Create back plane from 3 points
	tempPoint1 = Verts[0] - extrusionNormal;
	tempPoint2 = Verts[1] - extrusionNormal;
	tempPoint3 = Verts[2] - extrusionNormal;
	volume.BoundingPlanes[volume.PlaneCount++] = Plane( tempPoint2, tempPoint1, tempPoint3 );

	for( int i = 0; i < VertexCount; ++i )
	{
		tempPoint1 = Verts[i] - extrusionNormal;
		tempPoint2 = Verts[( i+1 < VertexCount )?i+1:0] - extrusionNormal;
		tempPoint3 = Verts[( i+1 < VertexCount )?i+1:0] + extrusionNormal;
		volume.BoundingPlanes[volume.PlaneCount++] = Plane( tempPoint1, tempPoint2, tempPoint3 );
	}
	return volume;
}

bool ConvexPolygon::ConvexVolumeTest( const ConvexVolumeResizable& volume, float extrusionDepth ) const
{
	Assert( extrusionDepth > 0.0f );
	Assert( VertexCount > 2 );
	// Calculate normal
	Plane tempPlane;
	tempPlane = Plane( Verts[0], Verts[1], Verts[2] );
	Vector extrusionNormal = tempPlane.Normal;
	extrusionNormal.Normalize();
	extrusionNormal *= extrusionDepth;

	if( !VertexCount ) return false;

	Vector center(0,0,0);
	for( int i = 0; i < VertexCount; ++i ) center += Verts[i];
	center /= VertexCount;

	return ( volume.PointTest( center+extrusionNormal ) || volume.PointTest( center-extrusionNormal ) );
/*
	for( int i = 0; i < VertexCount; ++i )
		if( volume.PointTest( Verts[i]+extrusionNormal ) || volume.PointTest( Verts[i]-extrusionNormal ) )
			return true;
	return false;
*/
}

bool ConvexPolygon::BoxTest( const BoundingBox& box, float extrusionDepth ) const
{
	Assert( extrusionDepth > 0.0f );
	Assert( VertexCount > 2 );
	// Calculate normal
	Plane tempPlane;
	tempPlane = Plane( Verts[0], Verts[1], Verts[2] );
	Vector extrusionNormal = tempPlane.Normal;
	extrusionNormal.Normalize();
	extrusionNormal *= extrusionDepth;

	for( int i = 0; i < VertexCount; ++i )
		if( box.PointTest( Verts[i]+extrusionNormal ) || box.PointTest( Verts[i]-extrusionNormal ) )
			return true;
	return false;
}

bool ConvexPolygon::BoxTestForBoxTypeZonePortalInclusion( const BoundingBox& box, float extrusionDepth ) const
{
	Assert( extrusionDepth > 0.0f );
	Assert( VertexCount > 2 );
	// Calculate normal
	Plane tempPlane;
	tempPlane = Plane( Verts[0], Verts[1], Verts[2] );
	Vector extrusionNormal = tempPlane.Normal;
	extrusionNormal.Normalize();
	extrusionNormal *= extrusionDepth;

	BoundingBox portalBox;
	portalBox.Reset();

	for( int i = 0; i < VertexCount; ++i )
	{
		portalBox += Verts[i]+extrusionNormal;
		portalBox += Verts[i]-extrusionNormal;
	}
	if( portalBox.IntersectInclusive( box ) )
		return true;
	return false;
}

#pragma optimize( "p", off )
