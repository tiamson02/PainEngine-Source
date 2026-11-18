// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//          2002/06 Krystian added stuff
//----------------------------------------------------------------------------

class RaySegment
{
public:
    RaySegment() {}
    RaySegment(const Vector& origin, const Vector& dir)
    :   Origin(origin), Dir(dir)
    {}

    inline bool Intersect(const BoundingBox& box) const;
	inline bool IntersectTriangle(const Vector& v0, 
                                  const Vector& v1, 
                                  const Vector& v2, 
                                  float& dist, 
                                  Vector* point=0) const;

	inline bool IntersectQuad(const Vector& v0, 
                              const Vector& v1, 
                              const Vector& v2, 
							  const Vector& v3,
                              float& dist, 
                              Vector* point=0 ) const;
	inline bool IntersectConvexPolygon( ConvexPolygon& cp, float& dist, Vector* point ) const;

	float DistanceFromPoint( const Vector& point ) const
    {
	    Vector vec1 = point - Origin;
	    Vector vec2 = Dir;
	    vec2.Normalize();
	    vec2 *= vec1.Dot( vec2 );
	    Vector vec3 = Origin;
	    vec3 += vec2;

	    return (point-vec3).Magnitude();
    }

public:
    Vector  Origin;
    Vector  Dir;
};


inline bool RaySegment::Intersect(const BoundingBox& box) const
{
    /* 
    Fast Ray-Box Intersection
    by Andrew Woo
    from "Graphics Gems", Academic Press, 1990
    */

#define RIGHT	0
#define LEFT	1
#define MIDDLE	2

    Vector coord; // intersection point

	bool inside = true;
	int quadrant[3];
	int whichPlane;
	float maxT[3];
	float candidatePlane[3];

	/* Find candidate planes; this loop can be avoided if
   	rays cast all from the eye(assume perpsective view) */
	for (int i=0; i<3; ++i)
		if(Origin[i] < box.Min[i]) {
			quadrant[i] = LEFT;
			candidatePlane[i] = box.Min[i];
			inside = false;
		}else if (Origin[i] > box.Max[i]) {
			quadrant[i] = RIGHT;
			candidatePlane[i] = box.Max[i];
			inside = false;
		}else	{
			quadrant[i] = MIDDLE;
		}

	/* Ray origin inside bounding box */
	if(inside)	{
		coord = Origin;
		return true;
	}


	/* Calculate T distances to candidate planes */
	for (int i=0; i<3; ++i)
		if (quadrant[i] != MIDDLE && Dir[i] !=0.f)
			maxT[i] = (candidatePlane[i]-Origin[i]) / Dir[i];
		else
			maxT[i] = -1.;

	/* Get largest of the maxT's for final choice of intersection */
	whichPlane = 0;
	//int i;
	for (int i=1; i<3; ++i)
		if (maxT[whichPlane] < maxT[i])
			whichPlane = i;

	/* Check final candidate actually inside box */
	if (maxT[whichPlane] < 0.f) 
        return false;

	for (int i=0; i<3; ++i)
		if (whichPlane != i) {
			coord[i] = Origin[i] + maxT[whichPlane] *Dir[i];
			if (coord[i] < box.Min[i] || coord[i] > box.Max[i])
				return false;
		} else {
			coord[i] = candidatePlane[i];
		}
	return true;				/* ray hits box */

#undef RIGHT
#undef LEFT
#undef MIDDLE
}

inline bool RaySegment::IntersectTriangle(const Vector& v0, 
                                          const Vector& v1, 
                                          const Vector& v2, 
                                          float& dist, 
                                          Vector* point) const
{
    float t,u,v;

    // find vectors for two edges sharing v0
    Vector edge1 = v1 - v0;
    Vector edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    Vector pvec=Dir^edge2;

    // If determinant is near zero, ray lies in plane of triangle
    float det = edge1.Dot(pvec);

    Vector tvec;
    if( det > 0 ) {
        tvec = Origin - v0;
    }
    else {
        tvec = v0 - Origin;
        det = -det;
    }

    if( det < 0.0001f ) {
        return false;
    }

    // Calculate U parameter and test bounds
    u = tvec.Dot(pvec);
    if( u < 0.0f || u > det ) {
        return false;
    }

    // Prepare to test V parameter
    Vector qvec=tvec^edge1;

    // Calculate V parameter and test bounds
    v = Dir.Dot(qvec);
    if( v < 0.0f || u + v > det ) {
        return false;
    }

    // Calculate t, scale parameters, ray intersects triangle
    t = edge2.Dot(qvec);
    float invDet = 1.0f / det;
    t *= invDet;
    u *= invDet;
    v *= invDet;

    if(point)
    {
	    *point = v0;
	    Vector temp = v1*u; 
        *point += temp; 
        temp = v2*v; 
        *point += temp;
    }

    dist=t;
    return true;
}

inline bool RaySegment::IntersectQuad(const Vector& v0, 
                                      const Vector& v1, 
                                      const Vector& v2, 
				        			  const Vector& v3,
                                      float& dist, 
                                      Vector* point ) const
{
	if( !IntersectTriangle( v0,v1,v3, dist, point ) )
	{
		return IntersectTriangle( v1,v2,v3, dist, point );
	}
	else
	{
		return true;
	};
}

inline bool RaySegment::IntersectConvexPolygon( ConvexPolygon& cp, float& dist, Vector* point ) const
{
	if( cp.VertexCount < 3 )
		return false;

	const Vector& v0 = cp.Verts[0];
	int i = 1;
	while( !IntersectTriangle( v0,cp.Verts[i],cp.Verts[i+1], dist, point ) )
	{
		i++;
		if( i == cp.VertexCount-1 )
			return false;
	};
	return true;
}
