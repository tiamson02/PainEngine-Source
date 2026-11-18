
class Portal;
class AntiPortal;
class World;
class Zone;
class Billboard;

#include "Engine/Entity.h"

// ==================================================================

struct FrontToBackCompare
{
    bool operator()(const float d1, const float d2) const
    {
        return d1 > d2;
    }
};

struct BackToFrontCompare
{
    bool operator()(const float d1, const float d2) const
    {
        return d1 < d2;
    }
};

// ==================================================================

template <const int Size, class CompareFunc = FrontToBackCompare>
class SortedList
{
public:
    struct Elem
    {
        Entity* EntPtr;
        float   Distance;
        Elem*   Next;
        Elem*   Prev;
    };

    Elem*   Head;
    Elem*   Tail;
    int     Count;
    Elem    Elems[Size];

    SortedList() { Reset(); }
    void Reset() { Head=Tail=0; Count=0; }

    operator Elem*() const { return Head; }

    inline void Insert(Entity* entity, float dist)
    {
        Assert(Count<Size);
        Elem* e=&Elems[Count++];

        e->EntPtr   = entity;
        e->Distance = dist;

        for (Elem* before=Head; before; before=before->Next)
        {
            if (before->Distance > dist)
            //if (CompareFunc()(before->Distance,dist))
            {
	            e->Next = before;
	            e->Prev = before->Prev;
                if (before->Prev)
	                before->Prev->Next = e;

	            before->Prev = e;
    
                if (before == Head)
		            Head = e;

                return;
            }
        }

	    // insert at back
	    e->Prev = Tail;
	    e->Next = 0;
        if (Head == 0)  // list was empty
            Head = e;
        else            // list was nonempty
		    Tail->Next = e;

    	Tail = e;
    }
};
// ==================================================================

template <const int Size, class CompareFunc = FrontToBackCompare>
class HeapSortedList
{
public:
    struct Elem
    {
        Entity* EntPtr;
        float   Distance;
    };

    int     Count;
    Elem    Elems[Size];
#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
    static int mMaxElemsCount;
#endif

    HeapSortedList() { Reset(); }
    void Reset() { Count=0; }

	void FullSort( void )
	{
		Elem tempElem;
		int tempCount = Count;
		while( Count )
		{
			tempElem = RemoveNext();
			Elems[Count] = tempElem;
		}

		Count = tempCount;
	}

    inline void Insert(Entity* entity, float dist)
    {
        Assert(Count<Size);

		// add element at the end of queue
		int index = Count;
		Count++;

		while( ( index != 0 ) && ( dist > Elems[Father(index)].Distance ) )
		{
			int father = Father(index);
			Elems[index] = Elems[father];
			index = father;
		};
		Elems[index].EntPtr = entity;
		Elems[index].Distance = dist;

#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
        if (Count > mMaxElemsCount){
            mMaxElemsCount = Count;
            TRACE("@@@ HeapSortedList::Insert(): peak EntityList heap usage = %d\n", mMaxElemsCount);
        }
#endif
	}

//	inline int Size( void ) { return Count; }
//	inline Elem* operator[](const int index) { return &(Elems[index]); }
	inline Elem RemoveNext( void )
	{
		Assert( Count > 0 );
		Elem toGive = Elems[0];
		Elem toHeap = Elems[Count-1];
		Count--;

		int index = 0;
		int largest = LeftSon(index);
		int size = Count;
		bool inserted = false;
		while( largest <= size )
		{
			if( largest < size )	// two sons
				if( Elems[largest].Distance < Elems[largest+1].Distance )	// choose smaller son
					largest++;

			if( toHeap.Distance < Elems[largest].Distance )
			{
				Elems[index] = Elems[largest];
				index = largest;
				largest = LeftSon( index );
			} else
			{
				Elems[index] = toHeap;
				inserted = true;
				largest = size + 1;	// to exit the loop
			}
		}
		if( !inserted )
			Elems[index] = toHeap;

		return toGive;
	}

protected:
	inline int Father( int index ) { return (index-1)>>1; };
	inline int LeftSon( int index ) { return (index<<1)+1; };
	inline int RightSon( int index ) { return (index<<1)+2; };
};

// ==================================================================

#define MAX_POLYGON_CUTS 100

class ENGINE_API SceneRender
{
public:	
    typedef SortedList<4096,FrontToBackCompare>			TSortedList;
    typedef HeapSortedList<4096,FrontToBackCompare>		THeapSortedList;

	Viewport&			mViewport;      //584 bytes (packed)
	World*				mWorld;
	Zone*				EyeZone;

	int	StatZones;
	int StatPortals;
	int StatEntities;

    THeapSortedList		EntityList;

    static const int MaxCoronas=256;
    Billboard*  Coronas[MaxCoronas];
    int			CoronaCount;

	ConvexVolume	Volumes[32];
	int				VolumeCount;

#define MAX_REFLECT_SURFS	16
	WorldMesh*		ReflectSurfs[MAX_REFLECT_SURFS];
	int				NumReflectSurfs;

public:
	SceneRender(Viewport& vp);
	void UpdateDistanceFromCam();
	void Occlude( bool thruPortals = true, bool noAntiportals = false );
	void OccludeReflection(const DynamicArray<WorldMesh*>& FixedList, const Plane& ReflectionPlane, const BoundingBox& ReflectionBox);
	void Dump();
	inline bool IsVisible(const Entity* e) const { return e->VisCounter==VisCounter; }

	Zone*	GetEyeZone();

	// Entity visibility
	static bool EntityVisible( Entity* entity, Vector& eyePosition );
	static void CreateBoxVolume( const BoundingBox& box, const Vector& point, ConvexVolumeResizable& volume );
private:
	static bool EntityVisible( const Vector& point, Entity* entity, ConvexVolumeResizable& volume, int zoneIndex, unsigned char* zonesVisited );
	static bool CreateEntityVisibilityVolume( const Vector& point, const ConvexVolumeResizable& frustum, const Portal* portal, ConvexVolumeResizable& volume );

public:
	// Potentially Visible Set
	static void RecalculateWorldPVS( void );
private:
	static void RecalculateZonePVS( int zoneIndex );
	static void CreatePortalToPortalVolume( int startZone, Portal* sourcePortal, ConvexPolygon& destPortalPoly, ConvexVolumeResizable& volume );
	static void RecalculateZonePVSRecurrent( int startZone, Portal* startPortal, ConvexVolumeResizable& volume, int zoneIndex, Portal* sourcePortal );

protected:
	SceneRender& operator=(const SceneRender&);

private:
	void	RecursiveWalkPortals(Portal* thisPortal, Zone* zone, const ConvexVolume& frustum, bool noAntiportals );	
    void    CreatePortalVolume(const ConvexVolume& frustum, const Portal* portal, ConvexVolume& volume);
	void	CreateAntiportalVolume( const AntiPortal& antiportal, ConvexVolumeResizable& volume );
	bool	CutPortalPolygonWithAntiPortalVolumes( ConvexPolygon polygon, DynamicArray<ConvexVolumeResizable*>& aportals );
	void	CutPortalPolygonWithFrustum( const ConvexVolume& frustum, ConvexPolygon& polygon );

	DynamicArray<ConvexVolumeResizable*> mAntiPortalsBlock;
	Vector mLastEyePosition;

public:
	int VisCounter;
};
