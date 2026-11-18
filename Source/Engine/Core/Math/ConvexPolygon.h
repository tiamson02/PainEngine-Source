
// ================================================================

#define HUGE_NUMBER 999999.0f

enum 
{
	EOnThePlane	    = 1,
	EInFrontOf		= 2,
	EInBackOf		= 3,
	ESpanning		= 4
};

// ================================================================

class ConvexVolumeResizable;

class ENGINE_API ConvexPolygon
{
public:
    static const int    MaxPolyVerts=32;
    Vector              Verts[MaxPolyVerts];
    int                 VertexCount;
	int					PlaneIndex;

public:
    ConvexPolygon() : VertexCount(0), PlaneIndex(-1) {}

	void	Reverse();
	int		Classify(const Plane& plane, const float eps=0.001f) const;
	int     Split(const Plane& plane, ConvexPolygon& front, ConvexPolygon& back, const float eps=0.001f) const;
    BOOL    CreateFromPlane(const Plane& plane, float extent=HUGE_NUMBER);

	ConvexVolume Extrude( float depth );
	bool ConvexVolumeTest( const ConvexVolumeResizable& volume, float extrusionDepth ) const;
	bool BoxTest( const BoundingBox& box, float extrusionDepth ) const;
	bool BoxTestForBoxTypeZonePortalInclusion( const BoundingBox& box, float extrusionDepth ) const;
	
	void operator+=(const Vector& v);	

	ConvexPolygon& operator=(const ConvexPolygon& rhs)
    {
        PlaneIndex  = rhs.PlaneIndex;
        VertexCount = rhs.VertexCount;
        GMemCopy(&Verts[0],&rhs.Verts[0],VertexCount*sizeof(Vector));
        return (*this);
    }

	// serialization
   	friend FileObject& operator<<(FileObject& file, const ConvexPolygon& poly)
    {
        //return file << poly.Verts;
        file << poly.VertexCount;
        if (poly.VertexCount>0)
            file.Write(&poly.Verts[0],sizeof(Vector)*poly.VertexCount);
        return file;
    }

    friend FileObject& operator>>(FileObject& file, ConvexPolygon& poly)
    {
        //return file >> poly.Verts;
        file >> poly.VertexCount;
        if (poly.VertexCount>0)
            file.Read(&poly.Verts[0],sizeof(Vector)*poly.VertexCount);
        return file;
    }
};
