// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

#ifdef TRANSGAMING

class BasePlane
{
public:
	union {
		struct { float X, Y, Z, W; };
		struct {
			BaseVector	Normal;
			float	Dist;
		};
	};

	FLOAT&          operator[](INT i)			{ return (&Normal.X)[i]; }
	const FLOAT&    operator[](INT i) const		{ return (&Normal.X)[i]; }


	BasePlane operator+(const BasePlane& p) const
	{
		BasePlane bp;
		bp.X = X+p.X; bp.Y = Y+p.Y; bp.Z = Z+p.Z; bp.W = W+p.W;
		return bp;
	}

	// arithmetic assignment
	BasePlane& operator*=(const float a) 
	{
		X *= a; Y *= a; Z *= a; W *= a;
		return *this;
	}

	BasePlane& operator/=(const float a)
	{
		const float inva = 1.0f/a;
		X *= inva; Y *= inva; Z *= inva; W *= inva;
		return *this;
	}

	BasePlane operator-(const BasePlane& p) const
	{
		BasePlane bp;
		bp.X = X-p.X; bp.Y = Y-p.Y; bp.Z = Z-p.Z; bp.W = W-p.W;
		return bp;
	}

	BasePlane operator-() const
	{
		BasePlane bp;
		bp.X = -X; bp.Y = -Y; bp.Z = -Z; bp.W = -W;
		return bp;
		//return BasePlane(-X, -Y, -Z, -W);
	}

	float Dot3(const BasePlane& p) const
	{
		return X*p.X+Y*p.Y+Z*p.Z;
	}

	float Distance(const Vector& a) const
	{
		return Normal.Dot(a) - Dist;
	}

	void SnapPoint( Vector& a ) const
	{
		a-= (Normal*(Normal.Dot(a) - Dist));
	}

	BasePlane Flip() const
	{
		BasePlane bp;
		bp.X = -X; bp.Y = -Y; bp.Z = -Z; bp.W = -W;
		return bp;
		//return BasePlane(-X,-Y,-Z,-W);
	}

    float Normalize()
    {
        float mag = sqrtf(X*X + Y*Y + Z*Z);
		float invmag = 1.0f/mag;
        X *= invmag;
        Y *= invmag;
        Z *= invmag;
        W *= invmag;
		return mag;
    }

	bool InMainAxis( void )
	{
		if( ( fabs( X ) > 0.99f ) || ( fabs( Y ) > 0.99f ) || ( fabs( Z ) > 0.99f ) ) return true;
		return false;
	}

	// 0 - entirely outside
	// 1 - partially inside
	// 2 - entirely inside
	enum
	{
		EBoxOutside		= 0,
		EBoxIntersect	= 1,
		EBoxInside		= 2,
	};

	int BoxTest( const Vector& center, const Vector& extent )
	{
		float NP = extent.X*fabsf(X) + extent.Y*fabsf(Y) + extent.Z*fabsf(Z);
		float MP = Distance(center);

		if (MP > NP)
			return EBoxOutside;

		else if (MP > -NP)
			return EBoxIntersect;

		else 
			return EBoxInside;
	}

#define	NORMAL_EPSILON	0.0001f
#define	DIST_EPSILON	0.01f

    BOOL Equal(const BasePlane& rhs) const
    {
	    if(		fabs(Normal.X-rhs.Normal.X) < NORMAL_EPSILON
		    &&	fabs(Normal.Y-rhs.Normal.Y) < NORMAL_EPSILON
		    &&	fabs(Normal.Z-rhs.Normal.Z) < NORMAL_EPSILON
		    &&	fabs(Dist-rhs.Dist) < DIST_EPSILON)
	    {
		    return TRUE;
	    }

	    return FALSE;
    }

    void Snap()
    {
	    for(int i=0; i<3; ++i) {
		    if(fabs(Normal[i] - +1) < NORMAL_EPSILON) {
			    Normal=Vector(0,0,0);
			    Normal[i] = +1;
			    break;
		    }
		    if(fabs(Normal[i] - -1) < NORMAL_EPSILON) {
			    Normal=Vector(0,0,0);
			    Normal[i] = -1;
			    break;
		    }
	    }

	    if(fabs(Dist-floor(Dist+0.5f)) < DIST_EPSILON) {
		    Dist = (float)floor(Dist+0.5f);
	    }
    }
#undef	NORMAL_EPSILON
#undef	DIST_EPSILON

};

class Plane : public BasePlane
{
public:
	// constructors
	Plane() {Normal.X = 0;}
	Plane(const BasePlane& plane)
	//:	Normal(plane.Normal), Dist(plane.Dist)
	{
		Normal = plane.Normal;
		Dist = plane.Dist;
	}

	Plane(const BaseVector& vec)
	//:	W(1.0f)
	{
		Normal = vec;
		W = 1.0f;
	}

	Plane(const BaseVector& normal, const float dist)
	//:	Dist(dist)
	{
		Normal = normal;
		Dist = dist;
	}

	Plane(const BaseVector& normal, const BaseVector& point)
	{ 
		Normal = normal;
		Dist = normal.Dot(point); 
	}

	Plane(const float x, const float y,
		  const float z, const float w)
	//:	X(x), Y(y), Z(z), W(w)
	{
		X = x; Y= y; Z = z; W = w;
	}
	Plane(const BaseVector& a, const BaseVector& b, const BaseVector& c)
	//:	Normal(((b-a)^(c-a)).GetNormal()), Dist(Normal.Dot(a))
	{
		Normal = ((b-a)^(c-a)).GetNormal();
		Dist = Normal.Dot(a);
	}
	Plane(const Vector4 &a, const Vector4 &b, const Vector4 &c){
		Normal = ((b-a)^(c-a)).GetNormal();
		Dist = Normal.Dot(a);
	}


    void set(const BaseVector &vec){
        Normal = vec;
        W = 1.0f;
    }
    void set(const BaseVector &normal, const float dist){
        Normal = normal;
        Dist = dist;
    }
    void set(const BaseVector& normal, const BaseVector& point){
        Normal = normal;
        Dist = normal.Dot(point);
    }
    void set(const float x, const float y, const float z, const float w){
        X = x;
        Y = y;
        Z = z;
        W = w;
    }
    void set(const BaseVector& a, const BaseVector& b, const BaseVector& c){
        Normal = ((b - a) ^ (c - a)).GetNormal();
        Dist = Normal.Dot(a);
    }
    void set(const Vector4 &a, const Vector4 &b, const Vector4 &c){
        Normal = ((b - a) ^ (c - a)).GetNormal();
        Dist = Normal.Dot(a);
    }
};


#else

class Plane
{
public:
	union {
		struct { float X, Y, Z, W; };
		struct {
			Vector	Normal;
			float	Dist;
		};
	};

	FLOAT&          operator[](INT i)			{ return (&Normal.X)[i]; }
	const FLOAT&    operator[](INT i) const		{ return (&Normal.X)[i]; }

	// constructors
	Plane() {}
	Plane(const Plane& plane)
	:	Normal(plane.Normal), Dist(plane.Dist)
	{}
    Plane(const Vector& vec)
    :   Normal(vec), W(1.0f)
    {}
	Plane(const Vector& normal, const float dist)
	:	Normal(normal), Dist(dist)
	{}
	Plane(const Vector& normal, const Vector& point)
	:	Normal(normal)
	{ Dist = Normal.Dot(point); }
	Plane(const float x, const float y,
		  const float z, const float w)
	:	X(x), Y(y), Z(z), W(w)
	{}
	Plane(const Vector& a, const Vector& b, const Vector& c)
	:	Normal(((b-a)^(c-a)).GetNormal()), Dist(Normal.Dot(a))
	{}

	Plane operator+(const Plane& p) const
	{
		return Plane(X+p.X, Y+p.Y, Z+p.Z, W+p.W);
	}

	// arithmetic assignment
	Plane& operator*=(const float a) 
	{
		X *= a; Y *= a; Z *= a; W *= a;
		return *this;
	}

	Plane& operator/=(const float a)
	{
		const float inva = 1/a;
		X *= inva; Y *= inva; Z *= inva; W *= inva;
		return *this;
	}

	Plane operator-(const Plane& p) const
	{
		return Plane(X-p.X, Y-p.Y, Z-p.Z, W-p.W);
	}

	Plane operator-() const
	{
		return Plane(-X, -Y, -Z, -W);
	}

	float Dot3(const Plane& p) const
	{
		return X*p.X+Y*p.Y+Z*p.Z;
	}

	float Distance(const Vector& a) const
	{
		return Normal.Dot(a) - Dist;
	}

	void SnapPoint( Vector& a ) const
	{
		a-= (Normal*(Normal.Dot(a) - Dist));
	}

	Plane Flip() const
	{
		return Plane(-X,-Y,-Z,-W);
	}

    float Normalize()
    {
        float mag = sqrtf(X*X + Y*Y + Z*Z);
		float invmag = 1/mag;
        X *= invmag;
        Y *= invmag;
        Z *= invmag;
        W *= invmag;
		return mag;
    }

	bool InMainAxis( void )
	{
		if( ( fabs( X ) > 0.99f ) || ( fabs( Y ) > 0.99f ) || ( fabs( Z ) > 0.99f ) ) return true;
		return false;
	}

	// 0 - entirely outside
	// 1 - partially inside
	// 2 - entirely inside
	enum
	{
		EBoxOutside		= 0,
		EBoxIntersect	= 1,
		EBoxInside		= 2,
	};

	int BoxTest( const Vector& center, const Vector& extent )
	{
		float NP = extent.X*fabsf(X) + extent.Y*fabsf(Y) + extent.Z*fabsf(Z);
		float MP = Distance(center);

		if (MP > NP)
			return EBoxOutside;

		else if (MP > -NP)
			return EBoxIntersect;

		else 
			return EBoxInside;
	}

#define	NORMAL_EPSILON	0.0001f
#define	DIST_EPSILON	0.01f

    BOOL Equal(const Plane& rhs) const
    {
	    if(		fabs(Normal.X-rhs.Normal.X) < NORMAL_EPSILON
		    &&	fabs(Normal.Y-rhs.Normal.Y) < NORMAL_EPSILON
		    &&	fabs(Normal.Z-rhs.Normal.Z) < NORMAL_EPSILON
		    &&	fabs(Dist-rhs.Dist) < DIST_EPSILON)
	    {
		    return TRUE;
	    }

	    return FALSE;
    }

    void Snap()
    {
	    for(int i=0; i<3; ++i) {
		    if(fabs(Normal[i] - +1) < NORMAL_EPSILON) {
                Normal=Vector(0,0,0);
			    Normal[i] = +1;
			    break;
		    }
		    if(fabs(Normal[i] - -1) < NORMAL_EPSILON) {
                Normal=Vector(0,0,0);
			    Normal[i] = -1;
			    break;
		    }
	    }

	    if(fabs(Dist-floor(Dist+0.5f)) < DIST_EPSILON) {
		    Dist = (float)floor(Dist+0.5f);
	    }
    }
    
#undef	NORMAL_EPSILON
#undef	DIST_EPSILON

};

#endif // TRANSGAMING


