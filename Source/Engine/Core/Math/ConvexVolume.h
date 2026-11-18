// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

class ConvexVolume
{
    static const int MaxBoundingPlanes  = 16;

public:
	
	Plane	BoundingPlanes[MaxBoundingPlanes];
	int		PlaneCount;

	ConvexVolume() : PlaneCount(0) {};
	ConvexVolume(const BoundingBox& bbox)
	{
		PlaneCount = 6;
		BoundingPlanes[0] = Plane(-1, 0, 0,-bbox.Min.X);
		BoundingPlanes[1] = Plane( 1, 0, 0, bbox.Max.X);
		BoundingPlanes[2] = Plane( 0,-1, 0,-bbox.Min.Y);
		BoundingPlanes[3] = Plane( 0, 1, 0, bbox.Max.Y);
		BoundingPlanes[4] = Plane( 0, 0,-1,-bbox.Min.Z);
		BoundingPlanes[5] = Plane( 0, 0, 1, bbox.Max.Z);
	}
	inline ConvexVolume(const Matrix& ClipMat);

	static int GetMaxBoundingPlanes(void) 
	{ 
		return MaxBoundingPlanes; 
	};

	inline ConvexVolume& operator=(const ConvexVolume& v);

	// 0 - entirely outside
	// 1 - partially inside
	// 2 - entirely inside
	enum
	{
		EBoxOutside		= 0,
		EBoxIntersect	= 1,
		EBoxInside		= 2,
	};

	inline int BoxTest(const Vector& center, const Vector& extent) const;
	inline int BoxTest(const Vector& center) const;
	inline bool PointTest( const Vector& point ) const;
	inline void Transform(const Matrix& m);
};

class ConvexVolumeResizable
{
public:
	DynamicArray<Plane*> Planes;

	~ConvexVolumeResizable() { Clear(); }
	inline void Clear( void );
	inline ConvexVolumeResizable& operator=( const ConvexVolumeResizable& v );

	// 0 - entirely outside
	// 1 - partially inside
	// 2 - entirely inside
	enum
	{
		EBoxOutside		= 0,
		EBoxIntersect	= 1,
		EBoxInside		= 2,
	};

	inline int BoxTest(const Vector& center, const Vector& extent) const;
	inline bool PointTest( const Vector& point ) const;
	inline bool PointTestExclusive( const Vector& point ) const;
};
