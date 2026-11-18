
// #pragma once
#ifndef _PORTAL_
#define _PORTAL_


#include "Core/Debug.h"
#include "Core/AbstractFactory.h"
#include "Core/Object.h"
#include "Engine/Entity.h"

class Portal;
class AntiPortal;

// ==================================================================

class ENGINE_API Zone : public Object
{
public:
    DECLARE_OBJECT_CLASS(Zone);

	BoundingBox					BBox;
	ConvexVolumeResizable		Convex;

	bool						IsBox:1;
	bool						WasIn:1;
	bool						OccludeEnteredInLastFrame:1;
    bool						Enabled:1;

	// Representation
	DynamicArray<Vector>		Vertices;
	DynamicArray<WORD>			Triangles;

    DynamicArray<int>			PortalRefs; // TODO: compatibility
	DynamicArray<Portal*>		Portals;
	DynamicArray<AntiPortal*>	AntiPortals;
	DynamicArray<Zone*>			VisibleZones;	// Potentially Visible Set, like in Quake, perhaps. Calculated just after level load.

    Entity*     EntityList;
    int         Index;

public:

    Zone() : EntityList(0), Index(EInvalidIndex), IsBox(true), Enabled(true) , WasIn(false), OccludeEnteredInLastFrame(false) {}

    inline void		AddEntity(Entity* entity);
    inline void		RemoveEntity(Entity* entity);
	inline void		ClearEntityList();
    inline Entity*	NextEntity(Entity* entity) const;

	// serialization
    void Load(FileObject* file)
    {
        Object::Load(file);
        *file >> BBox;
		IsBox = true;
		Convex.Clear();
		Vertices.ClearArray();
		Triangles.ClearArray();
    }

    void Save(FileObject* file)
    {
        Object::Save(file);
        *file << BBox;
    }

	void BuildBox( void )
	{
		BBox.Reset();
		for(int	i=0; i<Vertices.Size(); ++i )
			BBox += Vertices[i];
	}

	void BuildConvex( void )
	{
		Convex.Clear();
		for( int i = 0; i < Triangles.Size(); i += 3 )
		{
            m_setTag("Portal");
            Plane* x = new Plane( Vertices[Triangles[i]], Vertices[Triangles[i+2]], Vertices[Triangles[i+1]] );
			int j;
			for( j = 0; j < Convex.Planes.Size(); ++j )
				if( x->Equal( *(Convex.Planes[j]) ) )
					break;

			Convex.Planes.Add( x );
		}
	}

	void Scale( float scale )
	{
		if( IsBox && !Triangles.Size() )
			BBox = BBox.Transform(ScaleMatrix(scale));
		else
		{
			if( scale != 1.0f )
			{
				BBox.Reset();
				for( int i = 0; i < Vertices.Size(); ++i )
				{
	                Vertices[i] *= scale;
					BBox += Vertices[i];
				}
			}
			BuildConvex();
		}
	}

	bool IsABoxReally( void )
	{
		for( int i = 0; i < Convex.Planes.Size(); ++i )
			if( !Convex.Planes[i]->InMainAxis() )
				return false;
		return true;
	}

	// returns true if zone encompasses or intersects box
	bool BoxTest( const BoundingBox& box )
	{
		if( IsBox )
			return ( BBox.Intersect( box ) != 0 );
		else
			return ( Convex.BoxTest( box.GetCenter(), box.GetExtent() ) != 0 );
	}

	inline bool PortalTest( const Portal& portal, float tolerance );

	// returns 0 if box is outside of zone, 1 if zone intersects box, 2 if zone encompasses box
	int BoxTestExact( const BoundingBox& box )
	{
		if( IsBox )
			return BBox.EncompassOrIntersect( box );
		else
			return Convex.BoxTest( box.GetCenter(), box.GetExtent() );
	}

	bool PointTest( const Vector& point )
	{
		if( IsBox )
			return ( BBox.Intersect( point ) != 0 );
		else
			return Convex.PointTest( point );
	}

	char *GetSwitchZoneName(char *meshName, size_t len)
	{
		strncpy( meshName, Name.AsChar(), len);

        char *  start = strstr( meshName, "_" );
		Assert( start-meshName < Name.Len()-1 );
		start++;
		char* end = strstr( start, "_" );
		if( end ) *end = 0;
		return start;
	}

#define HAS_PLANE_TOLERANCE 0.0001f

	bool HasPlane( Plane& plane )
	{
		if( IsBox )
		{
			Vector a(0,0,0);
			plane.SnapPoint(a);
			if( plane.Normal.X != 0.0f )
			{
				if( ( fabs( BBox.Min.X - a.X ) < HAS_PLANE_TOLERANCE ) || ( fabs( BBox.Max.X - a.X ) < HAS_PLANE_TOLERANCE ) )
					return true;
				else
					return false;
			}
			else if( plane.Normal.Y != 0.0f )
			{
				if( ( fabs( BBox.Min.Y - a.Y ) < HAS_PLANE_TOLERANCE ) || ( fabs( BBox.Max.Y - a.Y ) < HAS_PLANE_TOLERANCE ) )
					return true;
				else
					return false;
			}
			else
			{
				if( ( fabs( BBox.Min.Z - a.Z ) < HAS_PLANE_TOLERANCE ) || ( fabs( BBox.Max.Z - a.Z ) < HAS_PLANE_TOLERANCE ) )
					return true;
				else
					return false;
			}
		}
		else
		{
			for( int i = 0; i < Convex.Planes.Size(); ++i )
				if( Convex.Planes[i]->Equal( plane ) )
					return true;
			return false;
		}
	}
};

// ========================================================================

inline void Zone::AddEntity(Entity* entity)
{
	Assert(Index!=EInvalidIndex);

    Entity::TZoneLink* link = &entity->ZoneLinks[Index];
    if (link->ZoneLink != this)
    {
		if(EntityList)
			EntityList->ZoneLinks[Index].Prev = &link->Next;

		link->Next = EntityList;
		link->Prev = &EntityList;
		EntityList = entity;

		// remember zone
        link->ZoneLink = this;
    }
}

inline void Zone::RemoveEntity(Entity* entity)
{
	Assert(Index!=EInvalidIndex);

    Entity::TZoneLink* link = &entity->ZoneLinks[Index];
    if (link->ZoneLink == this)
    {
		if(link->Next)
			link->Next->ZoneLinks[Index].Prev = link->Prev;

		*link->Prev = link->Next;

		// not linked to a zone
        link->ZoneLink = 0;
    }
}

inline void Zone::ClearEntityList()
{
	Assert(Index!=EInvalidIndex);
    for (Entity* e = EntityList; e; e=e->ZoneLinks[Index].Next)
		RemoveEntity(e);
}

inline Entity* Zone::NextEntity(Entity* entity) const 
{
	Assert(Index!=EInvalidIndex);
	Assert(entity->ZoneLinks[Index].ZoneLink==this);
    return entity->ZoneLinks[Index].Next;
}

// ==================================================================

class ENGINE_API Portal : public Object
{
public:
    DECLARE_OBJECT_CLASS(Portal);

	BoundingBox				Box;
	ConvexPolygon			Poly;
	ConvexPolygon			OtherPoly;

    int PlaneIndex;

	DynamicArray<int>		ZoneRefs;   // TODO: compatibility
	DynamicArray<Zone*>	    Zones;

	int						VisCounter;
	bool					IsDisabled:1;
	bool					OccludeEnteredInLastFrame:1;

	Portal() : VisCounter(0), IsDisabled(false), OccludeEnteredInLastFrame(false) {}

	void BuildBox()
	{
		Box.Reset();
		for(int	i=0; i<Poly.VertexCount; ++i) {
			Box+=Poly.Verts[i];
		}
	}

    void BuildOtherPoly()
    {
        OtherPoly = Poly;
        OtherPoly.Reverse();
    }

	void Scale(float scale)
	{
		if (scale!=1.0f)
		{
			Box.Reset();
			for (int i=0; i<Poly.VertexCount; ++i)
			{
				Poly.Verts[i]*=scale;
				Box+=Poly.Verts[i];
			}
		}
	}

	// serialization
    void Load(FileObject* file)
    {
		Object::Load(file);
        *file >> Box >> Poly;
    }

    void Save(FileObject* file)
    {
		Object::Save(file);
        *file << Box << Poly;
    }

	void FixVertexOrder( void )
	{
		ConvexPolygon tempPoly;
		DynamicArray<Vector> vertsToGet;

		tempPoly.VertexCount = 2;
		tempPoly.Verts[0] = Poly.Verts[0];
		tempPoly.Verts[1] = Poly.Verts[1];

		for( int i = 2; i < Poly.VertexCount; ++i )
			vertsToGet.Add( Poly.Verts[i] );

		while( vertsToGet.Size() )
		{
			Vector dir = tempPoly.Verts[tempPoly.VertexCount-1] - tempPoly.Verts[tempPoly.VertexCount-2];
			dir.Normalize();

			float maxDotVal = -1;
			int maxIndex = -1;

			for( int i = 0; i < vertsToGet.Size(); ++i )
			{
				Vector dir2 = vertsToGet[i] - tempPoly.Verts[tempPoly.VertexCount-1];
				dir2.Normalize();
				float dotVal = dir.Dot(dir2);
				if( dotVal > maxDotVal )
				{
					maxDotVal = dotVal;
					maxIndex = i;
				}
			}

			Assert( maxIndex != -1 );

			tempPoly.Verts[tempPoly.VertexCount++] = vertsToGet[maxIndex];
			vertsToGet.FastRemove(maxIndex);
		}

		Poly = tempPoly;
	}

	void BuildFromMesh(World* world, const WorldMesh* mesh);
};

// ==================================================================

class ENGINE_API AntiPortal : public Object
{
public:
    DECLARE_OBJECT_CLASS(AntiPortal);

	static int   				Counter;

	// Static optimisations
	BoundingBox					Box;
	ConvexVolumeResizable		Convex;
	DynamicArray<int>			DoublePlaneChecker;
	DynamicArray<WORD>			TriangleAdjacencies;

	// SceneRender modified optimisations
	DynamicArray<bool>			SceneRenderVisible;
	ConvexVolumeResizable*		SceneRenderConvexVolume;

	// Representation
    DynamicArray<Vector>		Vertices;
	DynamicArray<WORD>			Triangles;

	bool						Disabled;

	AntiPortal() : SceneRenderConvexVolume(0), Disabled(false) {}
	AntiPortal( WorldMesh* mesh );

	~AntiPortal() { if( SceneRenderConvexVolume ) { delete SceneRenderConvexVolume; SceneRenderConvexVolume = 0; } }

	void BuildBox( void )
	{
		Box.Reset();
		for(int	i=0; i<Vertices.Size(); ++i )
			Box += Vertices[i];
	}

	void BuildConvex( void )
	{
		Convex.Clear();
		DoublePlaneChecker.ClearArray();
		for( int i = 0; i < Triangles.Size(); i += 3 )
		{
            m_setTag("Portal");
			Plane* x = new Plane( Vertices[Triangles[i]], Vertices[Triangles[i+2]], Vertices[Triangles[i+1]] );
			int j;
			for( j = 0; j < Convex.Planes.Size(); ++j )
				if( x->Equal( *(Convex.Planes[j]) ) )
					break;

			DoublePlaneChecker.Add(j);
			Convex.Planes.Add( x );
		}
	}

	bool BuildTriangleAdjacencies( void )
	{
		if( !Triangles.Size() ) return false;
		Assert( Triangles.Size() % 3 == 0 );

        SceneRenderVisible.ResizeArray( Triangles.Size()/3 );
		TriangleAdjacencies.ResizeArray( Triangles.Size() );

		// Additionally check that all adjacencies are full
		for( int i = 0; i < Triangles.Size(); ++i )
			TriangleAdjacencies[i] = (WORD)-1;

		// For each triangle
		for( int i = 0; i < SceneRenderVisible.Size(); ++i )
		{
			// For each triangle edge
			for( int j = 0; j < 3; ++j )
			{
				WORD index1 = Triangles[3*i+j];
				WORD index2 = (j==2)?Triangles[3*i]:Triangles[3*i+j+1];
				// For each other triangle
				for( int k = i+1; k < SceneRenderVisible.Size(); ++k )
				{
					for( int l = 0; l < 3; ++l )
					{
						WORD index3 = Triangles[3*k+l];
						WORD index4 = (l==2)?Triangles[3*k]:Triangles[3*k+l+1];
						if( ( index1 == index4 ) && ( index2 == index3 ) )
						{
							TriangleAdjacencies[3*i+j] = k;
							TriangleAdjacencies[3*k+l] = i;
						}
					}
				}
			}
		}

		// Additionally check that all adjacencies are full
		for( int i = 0; i < Triangles.Size(); ++i )
			if( TriangleAdjacencies[i] == (WORD)-1 )
				return false;
		return true;
	}

	void Scale( float scale )
	{
		if( scale != 1.0f )
		{
			Box.Reset();
			for( int i = 0; i < Vertices.Size(); ++i )
			{
                Vertices[i] *= scale;
				Box += Vertices[i];
			}
		}
		BuildConvex();
	}

	// serialization
    void Load(FileObject* file)
    {
		Object::Load(file);
        *file >> Vertices >> Triangles;
		BuildBox();
		BuildConvex();
		BuildTriangleAdjacencies();
    }

    void Save(FileObject* file)
    {
		Object::Save(file);
        *file << Vertices << Triangles;
    }
};

// returns true if zone encompasses or intersects portal
inline bool Zone::PortalTest( const Portal& portal, float tolerance )
{
	if( IsBox )
//		return portal.Poly.BoxTest( BBox, tolerance );
		return portal.Poly.BoxTestForBoxTypeZonePortalInclusion( BBox, tolerance );
	else
		return portal.Poly.ConvexVolumeTest( Convex, tolerance );
}


#endif //_PORTAL_

