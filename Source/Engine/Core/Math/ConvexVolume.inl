
inline ConvexVolume::ConvexVolume(const Matrix& ClipMat)
:	PlaneCount(6)
{
    // Left clipping plane
    BoundingPlanes[0].X = -ClipMat.A03 - ClipMat.A00;
    BoundingPlanes[0].Y = -ClipMat.A13 - ClipMat.A10;
    BoundingPlanes[0].Z = -ClipMat.A23 - ClipMat.A20;
    BoundingPlanes[0].W = +ClipMat.A33 + ClipMat.A30;
	BoundingPlanes[0].Normalize();

    // Right clipping plane
    BoundingPlanes[1].X = -ClipMat.A03 + ClipMat.A00;
    BoundingPlanes[1].Y = -ClipMat.A13 + ClipMat.A10;
    BoundingPlanes[1].Z = -ClipMat.A23 + ClipMat.A20;
    BoundingPlanes[1].W = +ClipMat.A33 - ClipMat.A30;
	BoundingPlanes[1].Normalize();

    // Top clipping plane
    BoundingPlanes[2].X = -ClipMat.A03 + ClipMat.A01;
    BoundingPlanes[2].Y = -ClipMat.A13 + ClipMat.A11;
    BoundingPlanes[2].Z = -ClipMat.A23 + ClipMat.A21;
    BoundingPlanes[2].W = +ClipMat.A33 - ClipMat.A31;
	BoundingPlanes[2].Normalize();

    // Bottom clipping plane
    BoundingPlanes[3].X = -ClipMat.A03 - ClipMat.A01;
    BoundingPlanes[3].Y = -ClipMat.A13 - ClipMat.A11;
    BoundingPlanes[3].Z = -ClipMat.A23 - ClipMat.A21;
    BoundingPlanes[3].W = +ClipMat.A33 + ClipMat.A31;
	BoundingPlanes[3].Normalize();

    // Near clipping plane
    BoundingPlanes[4].X = -ClipMat.A02;
    BoundingPlanes[4].Y = -ClipMat.A12;
    BoundingPlanes[4].Z = -ClipMat.A22;
	BoundingPlanes[4].W = +ClipMat.A32;
	BoundingPlanes[4].Normalize();

	// far clipping planes
    BoundingPlanes[5].X = -ClipMat.A03 + ClipMat.A02;
    BoundingPlanes[5].Y = -ClipMat.A13 + ClipMat.A12;
    BoundingPlanes[5].Z = -ClipMat.A23 + ClipMat.A22;
	BoundingPlanes[5].W = +ClipMat.A33 - ClipMat.A32;
	BoundingPlanes[5].Normalize();
}

ConvexVolume& ConvexVolume::operator=(const ConvexVolume& v)
{
	PlaneCount = v.PlaneCount;
	if (PlaneCount>0)
		GMemCopy(&BoundingPlanes[0],&v.BoundingPlanes[0],sizeof(Plane)*PlaneCount);

	return (*this);
}

int ConvexVolume::BoxTest(const Vector& center, const Vector& extent) const
{
	int result = EBoxInside;

	for (int i=0; i<PlaneCount; ++i) 
	{
		const Plane& p = BoundingPlanes[i];

		float NP = extent.X*fabsf(p.X) + extent.Y*fabsf(p.Y) + extent.Z*fabsf(p.Z);
		float MP = p.Distance(center);

		if (MP > NP)
			return EBoxOutside;

		if (MP > -NP)
			result = EBoxIntersect;
	}

	return result;
}

int ConvexVolume::BoxTest(const Vector& center) const
{
	int result = EBoxInside;

	for (int i=0; i<PlaneCount; ++i) 
	{
		const Plane& p = BoundingPlanes[i];

		float MP = p.Distance(center);
		const float DIST_EPSILON = 0.01f;

		if (MP > DIST_EPSILON)
			return EBoxOutside;

		if (MP > -DIST_EPSILON)
			result = EBoxIntersect;
	}

	return result;
}

bool ConvexVolume::PointTest( const Vector& point ) const
{
	for( int i = 0; i < PlaneCount; ++i )
		if( BoundingPlanes[i].Distance( point ) > 0 )
			return false;
	return true;
}

void ConvexVolume::Transform(const Matrix& m)
{
	Matrix itm = m.Inverse();
	itm.Transpose();

	for (int i=0; i<PlaneCount; ++i)
		BoundingPlanes[i] = itm.TransformPlane(BoundingPlanes[i]);    
}

void ConvexVolumeResizable::Clear( void )
{
	while( Planes.Size() )
	{
		delete Planes[Planes.Size()-1];
		Planes.RemoveLast();
	}
}

ConvexVolumeResizable& ConvexVolumeResizable::operator=( const ConvexVolumeResizable& v )
{
	Clear();
	for( int i = 0; i < v.Planes.Size(); ++i )
	{
        m_setTag("Math");
		Plane* p = new Plane(*(v.Planes[i]));
		Planes.Add(p);
	}

	return (*this);
}

int ConvexVolumeResizable::BoxTest(const Vector& center, const Vector& extent) const
{
	int result = EBoxInside;

	for( int i = 0; i < Planes.Size(); ++i )
	{
		Plane* p = Planes[i];

		float NP = extent.X*fabsf(p->X) + extent.Y*fabsf(p->Y) + extent.Z*fabsf(p->Z);
		float MP = p->Distance( center );

		if (MP > NP)
			return EBoxOutside;

		if (MP > -NP)
			result = EBoxIntersect;
	}
	return result;
}

bool ConvexVolumeResizable::PointTest( const Vector& point ) const
{
	for( int i = 0; i < Planes.Size(); ++i )
		if( Planes[i]->Distance( point ) > 0 )
			return false;
	return true;
}

bool ConvexVolumeResizable::PointTestExclusive( const Vector& point ) const
{
	for( int i = 0; i < Planes.Size(); ++i )
		if( Planes[i]->Distance( point ) >= 0 )
			return false;
	return true;
}
