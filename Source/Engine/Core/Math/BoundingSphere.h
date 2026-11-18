
// ===================================================================================

class BoundingSphere
{
public:
	Vector	Center;
	float	Radius2;	// radius squared

public:
	BoundingSphere()
	{}
	BoundingSphere(const Vector& center, const float radius)
	:	Center(center), Radius2(radius*radius)
	{}

	inline void SetRadius(const float radius);
	inline bool Intersect(const Vector& boxCenter, const Vector& boxExtent);
	inline bool Intersect(const BoundingBox& box);
};

// ===================================================================================

inline void BoundingSphere::SetRadius(const float radius)
{
	Radius2 = radius * radius;
}

inline bool BoundingSphere::Intersect(const Vector& boxCenter, const Vector& boxExtent)
{
	float tmp,s;

	float d = 0.0f;

	tmp = Center.X - boxCenter.X;
	s = tmp + boxExtent.X;

	if(s<0.0f)
	{
		d += s*s;
		if(d>Radius2)
			return false;
	}
	else
	{
		s = tmp - boxExtent.X;
		if(s>0.0f)
		{
			d += s*s;
			if(d>Radius2)
				return false;
		}
	}

	tmp = Center.Y - boxCenter.Y;
	s = tmp + boxExtent.Y;

	if(s<0.0f)
	{
		d += s*s;
		if(d>Radius2)
			return false;
	}
	else
	{
		s = tmp - boxExtent.Y;
		if(s>0.0f)
		{
			d += s*s;
			if(d>Radius2)
				return false;
		}
	}

	tmp = Center.Z - boxCenter.Z;
	s = tmp + boxExtent.Z;

	if(s<0.0f)
	{
		d += s*s;
		if(d>Radius2)
			return false;
	}
	else
	{
		s = tmp - boxExtent.Z;
		if(s>0.0f)
		{
			d += s*s;
			if(d>Radius2)
				return false;
		}
	}

	return d <= Radius2;
}

inline bool BoundingSphere::Intersect(const BoundingBox& box)
{
	int i;
	float dmin;

	dmin = 0;
    for(i=0; i<3; ++i) {
		const float a = Center[i] - box.Min[i];
		const float b = Center[i] - box.Max[i];

        if(Center[i] < box.Min[i]) dmin += a*a; else
        if(Center[i] > box.Max[i]) dmin += b*b;
    }
    if (dmin <= Radius2) 
		return true;

	return false;
}
