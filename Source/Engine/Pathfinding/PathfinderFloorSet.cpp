// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

#include "Engine.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Render/Viewport.h"

#include "PathfinderFloorSet.h"
#include "PathfinderFloorProducer.h"
#include "Engine/Portal.h"
#include "Render/Video.h"
#include "Engine/World.h"

#if defined( TRANSGAMING_PSP )
#include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
#include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
#include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

PathfinderFloorSet::PathfinderFloorSet()
{
}

PathfinderFloorSet::~PathfinderFloorSet()
{
	ClearContents();
}

void PathfinderFloorSet::ClearContents( void )
{
	for( int i = 0; i < mFloors.Size(); i++ )
	{
		delete mFloors[i];
	};
}

#ifdef TRANSGAMING_USE_EDITOR
void PathfinderFloorSet::Render( void )
{
	const DWORD colorNormal = 0xa0a0ff;
	const DWORD colorSelected = 0xa00000;
	const DWORD colorWarning = 0xffa0a0;

	Assert( GEngine && GEngine->Video );
	LineRender& lineBuf = GEngine->Video->LineBuf;

	int displayed;

	for( int i = 0; i < mFloors.Size(); i++ )
	{
		displayed = 0;
		for( int j = 0; j < mSelectedFloors.Size(); j++ )
		{
			if( i == mSelectedFloors[j] )
			{
				lineBuf.DrawFloor( *mFloors[i], colorSelected );
				displayed = 1;
				break;
			};
		};
		if( displayed )
		{
			continue;
		};

		if( !mFloors[i]->HowManyEdges() )
		{
			lineBuf.DrawFloor( *mFloors[i], colorWarning );
		}
		else
		{
			lineBuf.DrawFloor( *mFloors[i], colorNormal );
		};
	};
}
#endif

#ifdef TRANSGAMING_USE_EDITOR
void PathfinderFloorSet::GenerateFloors( World* world, int zoneIndex )
{
	const int slideAngle = 45;
	PathfinderFloorProducer producer;

	GLog.Print("Floor generation start\n");
#ifdef TRANSGAMING
	float  time1 = GEngine->SysDriver.GetCurrentTime();
	float  time2;
	float  time3;
#else
	double time1 = GEngine->SysDriver.GetCurrentTime();
	double time2;
	double time3;
#endif

	if( world->Zones.Size() == 0 )
	{
		GLog.Print("No zones\n");
		ClearContents();
		GLog.Print("Adding %d meshes...\n", world->Meshes.Size() );
		for( int i = 0; i < world->Meshes.Size(); i++ )
		{
			producer.AddMesh( world->Meshes[i], slideAngle );
		};

		if( producer.BadEdges )
		{
			GLog.Print( "CRITICAL ERROR: bad edges\n" );
			GLog.Flush();
			GLog.Close();
			exit(1);
		}

		GLog.Print("Producing floors\n" );
		time2 = GEngine->SysDriver.GetCurrentTime();
		producer.ProduceFloors( mFloors );
		time3 = GEngine->SysDriver.GetCurrentTime();

		GLog.Print("Finished producing floors\n" );
		GLog.Print("Mesh addition time: %d\n", time2 - time1 );
		GLog.Print("Produce time: %d\n", time3 - time2 );
		GLog.Flush();

		return;
	}
	else if( (zoneIndex == -1 ) || ( zoneIndex >= world->Zones.Size() ) )
	{
		return;
	};

	GLog.Print("Zone: %d\n", zoneIndex );
	ClearContents();

	Zone& zone = *world->Zones[ zoneIndex ];

    /*
    for(TEntityPtr* p=zone.EntityList; p; p=p->Next)
    {
        WorldMesh* mesh = (*p)->AsMesh();
    */

    for(Entity* entity = zone.EntityList; entity; entity=zone.NextEntity(entity))
    {
        WorldMesh* mesh = entity->AsMesh();

        if(!mesh) 
            continue;
        
        producer.AddMesh( mesh, slideAngle );
    }
    /*
	for( int i = 0; i < zone.MeshRefs.Size(); i++ )
	{
		producer.AddMesh( world->Meshes[zone.MeshRefs[i]], slideAngle );
	};
    */
	if( producer.BadEdges )
	{
		GLog.Print( "CRITICAL ERROR: bad edges\n" );
		GLog.Flush();
		GLog.Close();
		exit(1);
	}

	time2 = GEngine->SysDriver.GetCurrentTime();
	producer.ProduceFloors( mFloors );
	time3 = GEngine->SysDriver.GetCurrentTime();

	GLog.Print("Finished producing floors\n" );
	GLog.Print("Mesh addition time: %f\n", time2 - time1 );
	GLog.Print("Produce time: %f\n", time3 - time2 );
	GLog.Flush();
}
#endif

int PathfinderFloorSet::PickFloor( const Viewport& viewport, int xm, int ym )
{
	POINT pt;
	pt.x = xm; pt.y = ym;

	float x=float(pt.x);
	float y=float(pt.y);

    // Compute the vector of the pick ray in screen space
    Vector v;
    v.X =  ( ( ( 2.0f * x ) / viewport.Width  ) - 1 ) / viewport.CameraToScreen.A00;
    v.Y = -( ( ( 2.0f * y ) / viewport.Height ) - 1 ) / viewport.CameraToScreen.A11;
    v.Z =  1.0f;

    // Transform the screen space pick ray into 3D space
    RaySegment ray;
    Matrix CameraToWorld = viewport.WorldToCamera.Inverse();
    ray.Dir    = CameraToWorld.TransformNormal( v );
    ray.Origin = CameraToWorld.GetTranslation();

    float dist=FLT_MAX;
    int floorNum = -1;

    for( int i = 0; i < mFloors.Size(); i++ )
	{
		PathfinderFloor* floor = mFloors[i];

		const int triCount = floor->HowManyTriangles();
		for( int j = 0; j < triCount; ++j )
		{
			const Vector& v0 = floor->TrianglePoint( 3*j );
			const Vector& v1 = floor->TrianglePoint( 3*j+1 );
			const Vector& v2 = floor->TrianglePoint( 3*j+2 );

			float d = FLT_MAX;
			if( ray.IntersectTriangle( v0, v1, v2, d ) )
			{
				if( d < 0 )
				{
					continue;
				};

				if( d < dist )
				{
					dist = d;
					floorNum = i;
				};
			};
		};
	};

	return floorNum;
}

void PathfinderFloorSet::Selection( const Viewport& viewport, RECT* rect, int select )
{
	if( rect == NULL )
	{
		SelectedSetClearAll( select );
	}
	else
	{
		int wp = PickFloor( viewport, rect->left, rect->top );
		if( select )
		{
			SelectUnique( wp );
		}
		else
		{
			UnselectUnique( wp );
		};
	};
}

void PathfinderFloorSet::SelectUnique( int floorIndex )
{
	if( floorIndex == -1 )
	{
		return;
	};

	for( int i = 0; i < mSelectedFloors.Size(); i++ )
	{
		if( mSelectedFloors[i] == floorIndex )
		{
			return;
		};
	};
	mSelectedFloors.Add( floorIndex );
}

void PathfinderFloorSet::UnselectUnique( int floorIndex )
{
	for( int i = 0; i < mSelectedFloors.Size(); i++ )
	{
		if( mSelectedFloors[i] == floorIndex )
		{
			mSelectedFloors.FastRemove(i);
			return;
		};
	};
}

int PathfinderFloorSet::Load( FILE* f, float scale )
{
	ClearContents();

	// Read floor number
	int size;

#ifdef TRANSGAMING
	if( TNG_fread( &size, sizeof(int), 1, f ) != 1 )
#else
	if( fread( &size, sizeof(int), 1, f ) != 1 )
#endif
	{
		return 0;
	};

	for( int i = 0; i < size; i++ )
	{
#ifdef TRANSGAMING_PSP
        m_setTag("AI");
#endif
		mFloors.Add( new PathfinderFloor );
		if( !mFloors[i]->Load(f, scale) )
		{
			delete mFloors[i];
			mFloors.RemoveLast();
			return 0;
		};
	};
	return 1;
}

int PathfinderFloorSet::Save( FILE* f, float scale )
{
	// Write floor number
	int size = mFloors.Size();
#ifdef TRANSGAMING
	if( TNG_fwrite( &size, sizeof(int), 1, f ) != 1 )
#else
	if( fwrite( &size, sizeof(int), 1, f ) != 1 )
#endif
	{
		return 0;
	};

	for( int i = 0; i < mFloors.Size(); i++ )
	{
		if( !mFloors[i]->Save(f,scale) )
		{
			return 0;
		};
	};
	return 1;
}

void PathfinderFloorSet::SelectUnselectFloorsOfAreaLowerHigherThan( int select, int lower, float area )
{
	if( lower )
	{
		if( select )
		{
			for( int i = 0; i < mFloors.Size(); i++ )
			{
				if( mFloors[i]->Area() < area )
				{
					SelectUnique( i );
				};
			};
		}
		else
		{
			for( int i = 0; i < mFloors.Size(); i++ )
			{
				if( mFloors[i]->Area() < area )
				{
					UnselectUnique( i );
				};
			};
		};
	}
	else
	{
		if( select )
		{
			for( int i = 0; i < mFloors.Size(); i++ )
			{
				if( mFloors[i]->Area() > area )
				{
					SelectUnique( i );
				};
			};
		}
		else
		{
			for( int i = 0; i < mFloors.Size(); i++ )
			{
				if( mFloors[i]->Area() > area )
				{
					UnselectUnique( i );
				};
			};
		};
	};
}

void PathfinderFloorSet::RemoveSelected( void )
{
	for( int i = mFloors.Size()-1; i >= 0; --i )
	{
		for( int j = 0; j < mSelectedFloors.Size(); j++ )
		{
			if( mSelectedFloors[j] == i )
			{
				delete mFloors[i];
				mFloors.FastRemove(i);
				mSelectedFloors.FastRemove(j);
				break;
			};
		};
	};
}

void PathfinderFloorSet::MakeGridOnSelected( float step, DynamicArray<Vector>& arrayToFill )
{
	for( int i = 0; i < mSelectedFloors.Size(); i++ )
	{
		mFloors[mSelectedFloors[i]]->MakeGrid( step, arrayToFill );
	};
}

void PathfinderFloorSet::ScaleContents( float scale )
{
	for( int i = 0; i < mFloors.Size(); i++ )
	{
		mFloors[i]->ScaleContents( scale );
	};
}

void PathfinderFloorSet::FlipZ( void )
{
	for( int i = 0; i < mFloors.Size(); i++ )
	{
		mFloors[i]->FlipZ();
	};
}
