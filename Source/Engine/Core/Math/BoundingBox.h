// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------
#ifndef  __BOUNDINGBOX_H__
# define __BOUNDINGBOX_H__

// AABB

class BoundingBox
{
public:
    Vector  Min;
    Vector  Max;

    // constructors    
    BoundingBox() 
    :   Min(Vector(F_MAX,F_MAX,F_MAX)), Max(Vector(-F_MAX,-F_MAX,-F_MAX)) 
    {}
    BoundingBox(const Vector& min, const Vector& max)
    :   Min(min), Max(max)
    {}

    void Reset()
    {
        Min=Vector(+F_MAX,+F_MAX,+F_MAX);
        Max=Vector(-F_MAX,-F_MAX,-F_MAX);
    }

    bool Valid() const
    {
        return Min.X<F_MAX && Min.Y<F_MAX && Min.Z<F_MAX;
    }

    bool Good() const
    {
		const float SANE = 10000.f;
        return Max.X<SANE && Max.Y<SANE && Max.Z<SANE
			&& Min.X>-SANE && Min.Y>-SANE && Min.Z>-SANE;
    }

    BoundingBox& operator+=(const Vector& rhs)
    {
        Min.Minimize(rhs);
        Max.Maximize(rhs);
        return *this;
    }
#ifdef TRANSGAMING
    BoundingBox& operator+=(const Vector4& rhs){
        Min.Minimize(rhs);
        Max.Maximize(rhs);
        return *this;
    }
#endif
    BoundingBox& operator+=(const BoundingBox& rhs)
    {
        Min.Minimize(rhs.Min);
        Max.Maximize(rhs.Max);
        return *this;
    }
	Vector GetCenter() const
	{
		return Vector((Min+Max)*0.5f);
	}
	Vector GetExtent() const
	{
		return (Max-Min)*0.5f;
	}
	Vector GetCorner(int num) const
	{
		return Vector( (num&1) ? Max.X : Min.X, (num&2) ? Max.Y : Min.Y, (num&4) ? Max.Z : Min.Z);
	}

	void Expand(float a)
	{
        //Min-=Vector(a);
        //Max+=Vector(a);
		const Vector v(a);
		(*this)+= v;
		(*this)+=-v;
	}

    BoundingBox Transform(const Matrix& mat) const
    {
        BoundingBox box;
  
        box+=mat.TransformCoord(Vector(Min.X,Min.Y,Min.Z));
        box+=mat.TransformCoord(Vector(Max.X,Min.Y,Min.Z));
        box+=mat.TransformCoord(Vector(Min.X,Max.Y,Min.Z));
        box+=mat.TransformCoord(Vector(Max.X,Max.Y,Min.Z));
        box+=mat.TransformCoord(Vector(Min.X,Min.Y,Max.Z));
        box+=mat.TransformCoord(Vector(Max.X,Min.Y,Max.Z));
        box+=mat.TransformCoord(Vector(Min.X,Max.Y,Max.Z));
        box+=mat.TransformCoord(Vector(Max.X,Max.Y,Max.Z));

        return box;
    }

	BOOL Intersect(const BoundingBox& rhs) const
	{
		if(Min.X > rhs.Max.X || rhs.Min.X > Max.X) return FALSE;
		if(Min.Y > rhs.Max.Y || rhs.Min.Y > Max.Y) return FALSE;
		if(Min.Z > rhs.Max.Z || rhs.Min.Z > Max.Z) return FALSE;

		return TRUE;
	}

	BOOL Intersect(const BoundingBox& rhs, float epsilon) const
	{
		if(Min.X > rhs.Max.X+epsilon || rhs.Min.X > Max.X+epsilon) return FALSE;
		if(Min.Y > rhs.Max.Y+epsilon || rhs.Min.Y > Max.Y+epsilon) return FALSE;
		if(Min.Z > rhs.Max.Z+epsilon || rhs.Min.Z > Max.Z+epsilon) return FALSE;

		return TRUE;
	}

	BOOL IntersectInclusive(const BoundingBox& rhs) const
	{
		if(Min.X >= rhs.Max.X || rhs.Min.X >= Max.X) return FALSE;
		if(Min.Y >= rhs.Max.Y || rhs.Min.Y >= Max.Y) return FALSE;
		if(Min.Z >= rhs.Max.Z || rhs.Min.Z >= Max.Z) return FALSE;

		return TRUE;
	}

	BOOL PointTest(const Vector& point ) const
	{
		if(Min.X > point.X || point.X > Max.X) return FALSE;
		if(Min.Y > point.Y || point.Y > Max.Y) return FALSE;
		if(Min.Z > point.Z || point.Z > Max.Z) return FALSE;

		return TRUE;
	}

	int EncompassOrIntersect(const BoundingBox& rhs) const
	{
		if(Min.X > rhs.Max.X || rhs.Min.X > Max.X) return 0;
		if(Min.Y > rhs.Max.Y || rhs.Min.Y > Max.Y) return 0;
		if(Min.Z > rhs.Max.Z || rhs.Min.Z > Max.Z) return 0;

		if( ( Max.X >= rhs.Max.X ) && ( Min.X <= rhs.Min.X ) &&
			( Max.Y >= rhs.Max.Y ) && ( Min.Y <= rhs.Min.Y ) &&
			( Max.Z >= rhs.Max.Z ) && ( Min.Z <= rhs.Min.Z ) ) return 2;

		return 1;
	}

    BOOL Intersect(const Vector& point) const
    {
        if(point.X < Min.X || point.X > Max.X) return FALSE;
        if(point.Y < Min.Y || point.Y > Max.Y) return FALSE;
        if(point.Z < Min.Z || point.Z > Max.Z) return FALSE;

        return TRUE;
    }

    // serialization
  	friend FileObject& operator<<(FileObject& file, const BoundingBox& box)
    {
        return file << box.Min << box.Max;
    }

  	friend FileObject& operator>>(FileObject& file, BoundingBox& box)
    {
        return file >> box.Min >> box.Max;
    }

};

#endif
