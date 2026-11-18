#include "Engine.h"

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Render/Billboard.h"
#include "Render/SceneRender.h"
#include "Render/Viewport.h"
#include "Engine/Portal.h"
#include "Engine/World.h"

//int SceneRender::VisCounter=0;

#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
int HeapSortedList::mMaxElemsCount = 0;
#endif

SceneRender::SceneRender(Viewport& vp)
:	mViewport(vp), mWorld(0), EyeZone(0), CoronaCount(0), mLastEyePosition( -123456.7f, -123456.7f, -123456.7f )
,	NumReflectSurfs(0)
,	VisCounter(0)
{
}

Zone* SceneRender::GetEyeZone()
{
	// brute force
	for (int index=0; index<mWorld->Zones.Size(); ++index) 
	{
		Zone* zone=mWorld->Zones[index];
        if(zone->PointTest(mViewport.EyePosition))
            return zone;
	}

	return 0;
}

void SceneRender::CreatePortalVolume(const ConvexVolume& frustum, const Portal* portal, ConvexVolume& volume)
{	
	ConvexPolygon poly = portal->Poly;	
	for( int i = 0; i < frustum.PlaneCount; ++i )
	{
		if (!poly.VertexCount)
			break;

		// Added by Krystian - don't cut portals with ViewFrustum's near plane
		if( ( &frustum == &mViewport.ViewFrustum ) && (i == 4) )
			continue;

		ConvexPolygon front,back;
		int r = poly.Split(frustum.BoundingPlanes[i],front,back);
		if (r == EInBackOf)
			continue;

		if (r == EInFrontOf)
		{
			volume.PlaneCount = 0;
			return;
		}

		poly=back;
	}

	volume.PlaneCount = 0;
	if (mWorld->Planes[portal->PlaneIndex].Distance(mViewport.EyePosition)>=0.0f)
		volume.BoundingPlanes[volume.PlaneCount++] = mWorld->Planes[portal->PlaneIndex];
	else
    {
		volume.BoundingPlanes[volume.PlaneCount++] = mWorld->Planes[portal->PlaneIndex].Flip();
		poly.Reverse();
    }

	for (int i=0; i<poly.VertexCount; ++i) {
		volume.BoundingPlanes[volume.PlaneCount++] = Plane
        (
			mViewport.EyePosition,
            poly.Verts[i],
            poly.Verts[(i+1)%poly.VertexCount]
        );
	}

	// Add ViewFrustum's far plane
	volume.BoundingPlanes[volume.PlaneCount++] = mViewport.ViewFrustum.BoundingPlanes[5];
}

bool SceneRender::CutPortalPolygonWithAntiPortalVolumes( ConvexPolygon polygon, DynamicArray<ConvexVolumeResizable*>& aportals )
{
	static ConvexPolygon cutStack[MAX_POLYGON_CUTS];
	static int firstAntiPortalIndex[MAX_POLYGON_CUTS];
	int stackTop = 1;
	cutStack[0] = polygon;
	firstAntiPortalIndex[0] = 0;

	static ConvexPolygon front, back;

	// Get next cut
	while( stackTop )
	{
		ConvexPolygon poly = cutStack[--stackTop];
		int firstAPI = firstAntiPortalIndex[stackTop];

		int added = 0;
		while( !added )
		{
			if( firstAPI == aportals.Size() )
				return true;	// one of the portal cuts was cut with all aportals and not destroyed

			bool cutOut = true;
			ConvexVolumeResizable* apv = aportals[firstAPI];
			for( int j = 0; j < apv->Planes.Size(); ++j )
			{
				int r = poly.Split(*(apv->Planes[j]),front,back);
				if( r == EInBackOf )
					continue;

				if( r == EInFrontOf )	// this cut is out of this antiportal volume, so ignore new cuts
				{
					stackTop -= added;
					added = 0;
					cutOut = false;
					break;
				}
				else					// this cut intersects with current volume
				{
					if( stackTop >= MAX_POLYGON_CUTS ) return true;	// too complicated matter, forget it and assume they don't cut
					cutStack[stackTop] = front;
					firstAntiPortalIndex[stackTop++] = firstAPI+1;
					added++;
				}
			}
			firstAPI++;
			if( cutOut ) added++;	// so there won't be added == 0
		}
	}

	return false;	// no cut survived
}

void SceneRender::CutPortalPolygonWithFrustum( const ConvexVolume& frustum, ConvexPolygon& polygon )
{	
	for( int i = 0; i < frustum.PlaneCount; ++i )
	{
		if( !polygon.VertexCount )
			break;

		ConvexPolygon front,back;
		int r = polygon.Split(frustum.BoundingPlanes[i],front,back);
		if (r == EInBackOf)
			continue;

		if (r == EInFrontOf)
			break;

		polygon = back;
	}
}

void SceneRender::CreateAntiportalVolume( const AntiPortal& antiportal, ConvexVolumeResizable& volume )
{
	volume.Clear();

	// find out which tris of antiportal are visible to us; and add planes from those visible to volume
	for( int i = 0; i < antiportal.SceneRenderVisible.Size(); ++i )
	{
		if( antiportal.DoublePlaneChecker[i] != i )
			antiportal.SceneRenderVisible[i] = antiportal.SceneRenderVisible[ antiportal.DoublePlaneChecker[i] ];
		else if( antiportal.Convex.Planes[i]->Distance( mViewport.EyePosition ) >= 0.0f )	// visible
		{
			antiportal.SceneRenderVisible[i] = true;
            m_setTag("PortalAnti");
			Plane* p = new Plane(*(antiportal.Convex.Planes[i]));
			volume.Planes.Add(p);
		}
		else
			antiportal.SceneRenderVisible[i] = false;
	}

	// find the edges between visible and non-visible triangles, and add planes from eye position to those edges
	for( int i = 0; i < antiportal.SceneRenderVisible.Size(); ++i )
	{
		if( antiportal.SceneRenderVisible[i] )
		{
			for( int j = 0; j < 3; ++j )
			{
				if( !antiportal.SceneRenderVisible[antiportal.TriangleAdjacencies[3*i+j]] )
				{
                    m_setTag("PortalAnti");
					Plane* p = new Plane( mViewport.EyePosition,
							antiportal.Vertices[antiportal.Triangles[3*i+((j+1)%3)]],
                            antiportal.Vertices[antiportal.Triangles[3*i+j]]);
					volume.Planes.Add( p );
				}
			}
		}
	}
}

void SceneRender::RecursiveWalkPortals(Portal* thisPortal, Zone* zone, const ConvexVolume& frustum, bool noAntiportals )
{
	// include this zone's meshes that are in current volume
	int i,j;
	++StatZones;

	if( zone->WasIn ) return;
	zone->WasIn = true;
	zone->OccludeEnteredInLastFrame = true;

	GProfiler.SetState( CLIENT_RENDER_OCCLUDE_ANTIPORTALS_CALCULATION );

	// First, find this zone's antiportals in frustrum and sort them by center distance
	DynamicArray<AntiPortal*> antiPortals;
	DynamicArray<float> antiPortalDistances;
	DynamicArray<ConvexVolumeResizable*> antiPortalVolumes;

	if( !noAntiportals )
	{
		antiPortalVolumes.ResizeArray( zone->AntiPortals.Size() );
		for( i = 0; i < zone->AntiPortals.Size(); ++i )
		{
			AntiPortal* ap = zone->AntiPortals[i];
			if( ap->Disabled ) continue;
			Vector boxCenter = ap->Box.GetCenter();
			Vector boxExtent = ap->Box.GetExtent();
			if( !frustum.BoxTest( boxCenter, boxExtent ) )
				continue;	// this antiportal is not visible in the frustum

			bool vis = true;
#ifdef TRANSGAMING
			// Because 'i' has been declared before this scope ( and the one of the for loop before this )
			// both loops are manipulating it ( i.e. this loop was screwing up the loop above ).  By
			// switching i -> j, we are using a variable that gets reset when entering the scope below.
			// This fixes the infinite loop we were experiencing in portal levels.
			for( j = 0; j < mAntiPortalsBlock.Size(); ++j )
			{
				if( ( mAntiPortalsBlock[j] == ap->SceneRenderConvexVolume ) || ( mAntiPortalsBlock[j]->BoxTest( boxCenter, boxExtent ) == ConvexVolumeResizable::EBoxInside ) )
				{
					vis = false;
					break;
				}
			}
#else
			for( i = 0; i < mAntiPortalsBlock.Size(); ++i )
			{
				if( ( mAntiPortalsBlock[i] == ap->SceneRenderConvexVolume ) || ( mAntiPortalsBlock[i]->BoxTest( boxCenter, boxExtent ) == ConvexVolumeResizable::EBoxInside ) )
				{
					vis = false;
					break;
				}
			}
#endif
			if( !vis )
				continue;

			// calculate distance to antiportal
			float distance = (mViewport.EyePosition - boxCenter).Magnitude();

			// find where to put it and put it there
			for( j = 0; j < antiPortals.Size(); ++j )
				if( antiPortalDistances[j] > distance )
					break;
			antiPortals.Insert(ap,j);
			antiPortalDistances.Insert(distance,j);
		}

		// Next, determine which antiportals are in the other antiportals' volumes, and forget them
		for( i = 0; i < antiPortals.Size(); ++i )
		{
			AntiPortal* ap = antiPortals[i];

			// If we're for the first time in zone, create volume
			if( ap->SceneRenderConvexVolume )
				antiPortalVolumes[i] = ap->SceneRenderConvexVolume;
			else
			{
                m_setTag("Portal");
				ConvexVolumeResizable* cv = new ConvexVolumeResizable();
				antiPortalVolumes[i] = cv;
				CreateAntiportalVolume( *ap, *cv );
				ap->SceneRenderConvexVolume = cv;
			}

			for( j = i+1; j < antiPortals.Size(); ++j )
			{
				AntiPortal* ap2 = antiPortals[j];
				if( ap->SceneRenderConvexVolume->BoxTest( ap2->Box.GetCenter(), ap2->Box.GetExtent() ) == ConvexVolumeResizable::EBoxInside )
				{
					// This antiportal is covered by ap; we can forget it
					antiPortals.Remove(j);
					antiPortalDistances.Remove(j);
					--j;
				}
			}
		}
		// Ok, now we have a set of convex antiportal volumes; we use it to forget about portals and entities
	}

	GProfiler.SetState( CLIENT_RENDER_OCCLUDE_ENTITY_VISIBILITY );

    for (Entity* entity = zone->EntityList; entity; entity=zone->NextEntity(entity))
	{
		if (entity->Flags & Entity::EEntAlwaysVisible) {

			Vector boxCenter=entity->Box.GetCenter();
			entity->VisCounter=VisCounter;
			EntityList.Insert(entity,(mViewport.EyePosition-boxCenter).MagnitudeSquared());
		}
		else if (entity->Flags & Entity::EEntNoDraw) {

			continue;
		}
		if (entity->VisCounter!=VisCounter)
		{			            
            const Vector boxCenter = entity->Box.GetCenter();
            const Vector boxExtent = entity->Box.GetExtent();

			if( frustum.BoxTest( boxCenter, boxExtent ) )
			{
				bool vis = true;

				GProfiler.SetState( CLIENT_RENDER_OCCLUDE_ANTIP_BOXTEST );
				
				if (entity->KillDist > 0.0F)
				{
					const FLOAT DistSquared = (mViewport.EyePosition - boxCenter).MagnitudeSquared();
					if (DistSquared > entity->KillDist)
						vis = false;						
				}
				
				for( i = 0; i < mAntiPortalsBlock.Size(); ++i )
				{
					if( mAntiPortalsBlock[i]->BoxTest( boxCenter, boxExtent ) == ConvexVolumeResizable::EBoxInside )
					{
						vis = false;
						break;
					}
				}

				if (vis)
				{
					for( i = 0; i < antiPortals.Size(); ++i )
					{
						if( antiPortalVolumes[i]->BoxTest( boxCenter, boxExtent ) == ConvexVolumeResizable::EBoxInside )
						{
							vis = false;
							break;
						}
					}
				}

				GProfiler.SetState( CLIENT_RENDER_OCCLUDE_ENTITY_VISIBILITY );

				if( vis )
				{
					entity->VisCounter = VisCounter;

		            Billboard* billboard = entity->AsBillboard();
					if (billboard && (billboard->Type & Billboard::EBillboardCorona))
		                Coronas[CoronaCount++] = billboard;
		            else
		                EntityList.Insert(entity,(mViewport.EyePosition-boxCenter).MagnitudeSquared());

					//if ((zone == EyeZone) && entity->ReflectSurf)
					if (entity->ReflectSurf)
					{
						Assert(entity->AsMesh());
						//ReflectFlags |= entity->AsMesh()->ReflectSurf;
						if (NumReflectSurfs < MAX_REFLECT_SURFS) {
							entity->Distance = (mViewport.EyePosition-boxCenter).MagnitudeSquared();
							ReflectSurfs[NumReflectSurfs++] = entity->AsMesh();
						}
					}

					// update stats
					Model* model = entity->AsModel();
					if (model)
					{
						++StatEntities;
					}
					WorldMesh* mesh = entity->AsMesh();
					if (mesh) {
						++StatEntities;
					}
				}
			}
		}
	}

	// go thru this node's visible portals
    // we set the portal to 'visited' once we did view volume extrusion

	GProfiler.SetState( CLIENT_RENDER_OCCLUDE_PORTAL_VISIBILITY );
	for (int index=0; index<zone->Portals.Size(); ++index) 
	{
		Portal* portal=zone->Portals[index];

		if( portal == thisPortal )
			continue;

		if( portal->IsDisabled )
			continue;

		Vector boxCenter = portal->Box.GetCenter();
		Vector boxExtent = portal->Box.GetExtent();
		if( boxExtent.X < 0.05f ) boxExtent.X = 0.05f;
		if( boxExtent.Y < 0.05f ) boxExtent.Y = 0.05f;
		if( boxExtent.Z < 0.05f ) boxExtent.Z = 0.05f;
		if( frustum.BoxTest( boxCenter, boxExtent ) )
		{
			bool vis = true;
			for( i = 0; i < mAntiPortalsBlock.Size(); ++i )
			{
				if( mAntiPortalsBlock[i]->BoxTest( boxCenter, boxExtent ) == ConvexVolumeResizable::EBoxInside )
				{
					vis = false;
					break;
				}
			}

			float distFromPortal = fabs(mWorld->Planes[portal->PlaneIndex].Distance( mViewport.EyePosition ));

			int added = 0;
			if( vis && ( distFromPortal > 0.1f ) )
			{
				ConvexPolygon cutPolygon = portal->Poly;
				CutPortalPolygonWithFrustum( frustum, cutPolygon );

				for( i = 0; i < antiPortals.Size(); ++i )
				{
					int result = antiPortalVolumes[i]->BoxTest( boxCenter, boxExtent );

					if( result == ConvexVolumeResizable::EBoxInside )
					{
						vis = false;
						break;
					}
					else if( result == ConvexVolumeResizable::EBoxIntersect )
					{
						mAntiPortalsBlock.Add( antiPortalVolumes[i] );
						added++;
					}
				}

				if( vis && mAntiPortalsBlock.Size() )
					vis = CutPortalPolygonWithAntiPortalVolumes( cutPolygon, mAntiPortalsBlock );
			}

			if( vis )
			{
				ConvexVolume volume;
				if( distFromPortal > 0.1f )
				{
					CreatePortalVolume( frustum, portal, volume );

					if (!volume.PlaneCount)
					{
						while( added )
						{
							mAntiPortalsBlock.RemoveLast();
							added--;
						}
						continue;
					}
				}
				else
					volume = frustum;

	            // mark the portal as already extruded and seen through for this vis pass
				++StatPortals;
				portal->OccludeEnteredInLastFrame = true;

				GProfiler.SetState( CLIENT_RENDER_OCCLUDE );
	            // visit all connected zones
				for ( i=0; i<portal->Zones.Size(); ++i) 
				{
	                Zone* otherZone=portal->Zones[i];
					if( otherZone != zone )
						RecursiveWalkPortals(portal,otherZone,volume,noAntiportals);
				}

				GProfiler.SetState( CLIENT_RENDER_OCCLUDE_PORTAL_VISIBILITY );
			}

			while( added )
			{
				mAntiPortalsBlock.RemoveLast();
				added--;
			}
		}
	}
	GProfiler.SetState( CLIENT_RENDER_OCCLUDE );

	zone->WasIn = false;
}

void SceneRender::Occlude(bool thruPortals, bool noAntiportals )
{
	int j=0;
	if( !(mViewport.EyePosition == mLastEyePosition) )
	{
		for( int i = 0; i < mWorld->AntiPortals.Size(); ++i )
		{
			if( mWorld->AntiPortals[i]->SceneRenderConvexVolume )
			{
				delete mWorld->AntiPortals[i]->SceneRenderConvexVolume;
				mWorld->AntiPortals[i]->SceneRenderConvexVolume = 0;
			}
		}
	}
	mLastEyePosition = mViewport.EyePosition;

	++VisCounter;
    if (VisCounter>0x7fffffff-1)
        VisCounter=0;

	// reset stats
	StatPortals  = 0;
	StatZones	 = 0;
	StatEntities = 0;

	VolumeCount = 0;
	CoronaCount = 0;

	NumReflectSurfs = 0;

    EntityList.Reset();
	EyeZone=GetEyeZone();

	for( int i = 0; i < mWorld->Zones.Size(); ++i )
		mWorld->Zones[i]->OccludeEnteredInLastFrame = false;
	for( int i = 0; i < mWorld->Portals.Size(); ++i )
		mWorld->Portals[i]->OccludeEnteredInLastFrame = false;


	if (EyeZone && thruPortals)
		RecursiveWalkPortals(0,EyeZone,mViewport.ViewFrustum, noAntiportals );
    else
    {
		// First, find antiportals in frustrum and sort them by center distance
		DynamicArray<AntiPortal*> antiPortals;
		DynamicArray<float> antiPortalDistances;
		DynamicArray<ConvexVolumeResizable*> antiPortalVolumes;

		if( !noAntiportals )
		{
			antiPortalVolumes.ResizeArray( mWorld->AntiPortals.Size() );
			for( int i = 0; i < mWorld->AntiPortals.Size(); ++i )
			{
				AntiPortal* ap = mWorld->AntiPortals[i];
				if( ap->Disabled ) continue;
				Vector boxCenter = ap->Box.GetCenter();
				if( !mViewport.ViewFrustum.BoxTest( boxCenter,ap->Box.GetExtent()) )
					continue;	// this antiportal is not visible in the frustum

				// calculate distance to antiportal
				float distance = (mViewport.EyePosition - boxCenter).Magnitude();

				// find where to put it and put it there
				for( j = 0; j < antiPortals.Size(); ++j )
					if( antiPortalDistances[j] > distance )
						break;
				antiPortals.Insert(ap,j);
				antiPortalDistances.Insert(distance,j);
			}

			// Next, determine which antiportals are in the other antiportals' volumes, and forget them
			for( int i = 0; i < antiPortals.Size(); ++i )
			{
				AntiPortal* ap = antiPortals[i];

				// If we're for the first time in zone, create volume
				if( ap->SceneRenderConvexVolume )
					antiPortalVolumes[i] = ap->SceneRenderConvexVolume;
				else
				{
                    m_setTag("Portal");
					ConvexVolumeResizable* cv = new ConvexVolumeResizable();
					antiPortalVolumes[i] = cv;
					CreateAntiportalVolume( *ap, *cv );
					ap->SceneRenderConvexVolume = cv;
				}

				for( j = i+1; j < antiPortals.Size(); ++j )
				{
					AntiPortal* ap2 = antiPortals[j];
					if( ap->SceneRenderConvexVolume->BoxTest( ap2->Box.GetCenter(), ap2->Box.GetExtent() ) == ConvexVolumeResizable::EBoxInside )
					{
						// This antiportal is covered by ap; we can forget it
						antiPortals.Remove(j);
						antiPortalDistances.Remove(j);
						--j;
					}
				}
			}
			// Ok, now we have a set of convex antiportal volumes; we use it to forget about portals and entities
		}

        for (Entity* entity=mWorld->EntityList; entity; entity=entity->Next)
        {
			if (entity->Flags & Entity::EEntAlwaysVisible) {

				Vector boxCenter=entity->Box.GetCenter();
				entity->VisCounter=VisCounter;
				EntityList.Insert(entity,(mViewport.EyePosition-boxCenter).MagnitudeSquared());
			}
			else if (entity->Flags & Entity::EEntNoDraw) {

				continue;
			}
			else if (entity->VisCounter != VisCounter) {			            

                Vector boxCenter = entity->Box.GetCenter();
				Vector boxExtent = entity->Box.GetExtent();

				if( mViewport.ViewFrustum.BoxTest( boxCenter, boxExtent ) )
			    {
					bool vis = true;
					for( int i = 0; i < antiPortals.Size(); ++i )
						if( antiPortalVolumes[i]->BoxTest( boxCenter, boxExtent ) == ConvexVolumeResizable::EBoxInside )
						{
							vis = false;
							break;
						}

					if( vis )
					{
					    entity->VisCounter=VisCounter;

						Billboard* billboard = entity->AsBillboard();
						if (billboard && (billboard->Type & Billboard::EBillboardCorona))
							Coronas[CoronaCount++] = billboard;
						else
							EntityList.Insert(entity,(mViewport.EyePosition-boxCenter).MagnitudeSquared());

						if (entity->ReflectSurf)
						{
							Assert(entity->AsMesh());
							//ReflectSurf = entity->AsMesh();
							if (NumReflectSurfs < MAX_REFLECT_SURFS)
								ReflectSurfs[NumReflectSurfs++] = entity->AsMesh();
						}

	   					++StatEntities;
					}
				}
			}
        }
    }

	Assert( mAntiPortalsBlock.Size() == 0 );

	// Sort the data in the entity list now
	EntityList.FullSort();
	UpdateDistanceFromCam();
}

void SceneRender::UpdateDistanceFromCam()
{
	Matrix C2WInvTran = mViewport.WorldToCamera;
	C2WInvTran.Transpose();
	Plane camLocalPlane(0,0,1,0);
	Plane camPlane = C2WInvTran.TransformPlane(camLocalPlane);
	int code = ~camPlane.Normal.Sign();

    for( int i = 0; i < EntityList.Count; ++i )
	{
		Entity *ent = EntityList.Elems[i].EntPtr;
		const BoundingBox& box = ent->Box;

		Vector corner = box.GetCorner(code);
		ent->DistanceFromCamera = camPlane.Distance(corner);

//		GLog.Print("entity[%d]: %s %.4f\n", i, ent->Name.AsChar(), ent->DistanceFromCamera );
	}
}


void SceneRender::Dump()
{
	GLog.Print(" ====================== FRAME ======================= (%d)\n",EntityList.Count);   
    for( int i = 0; i < EntityList.Count; ++i )
	{
		THeapSortedList::Elem& e = EntityList.Elems[i];
		GLog.Print("entity[%d]: %s %.4f\n", i, e.EntPtr->Name.AsChar(), sqrt(e.Distance) );
	}
}

void SceneRender::CreateBoxVolume( const BoundingBox& box, const Vector& point, ConvexVolumeResizable& volume )
{
	volume.Clear();

	// find out which tris of box are visible to us; and add planes from those visible to volume
	bool visible[6];

	for( int i = 0; i < 6; ++i ) visible[i] = true;

	// Add -X plane
	if( box.Min.X < point.X )
	{
        m_setTag("VolumeBox");
		Plane* p = new Plane( Vector( -1, 0, 0 ), box.Min );
		volume.Planes.Add(p);
		visible[0] = false;
	}


	// Add +X plane
	if( box.Max.X > point.X )
	{
        m_setTag("VolumeBox");
		Plane* p = new Plane( Vector( 1, 0, 0 ), box.Max );
		volume.Planes.Add(p);
		visible[1] = false;
	}

	// Add -Y plane
	if( box.Min.Y < point.Y )
	{
        m_setTag("VolumeBox");
		Plane* p = new Plane( Vector( 0, -1, 0 ), box.Min );
		volume.Planes.Add(p);
		visible[2] = false;
	}

	// Add +Y plane
	if( box.Max.Y > point.Y )
	{
        m_setTag("VolumeBox");
		Plane* p = new Plane( Vector( 0, 1, 0 ), box.Max );
		volume.Planes.Add(p);
		visible[3] = false;
	}

	// Add -Z plane
	if( box.Min.Z < point.Z )
	{
        m_setTag("VolumeBox");
		Plane* p = new Plane( Vector( 0, 0, -1 ), box.Min );
		volume.Planes.Add(p);
		visible[4] = false;
	}

	// Add +Z plane
	if( box.Max.Z > point.Z )
	{
        m_setTag("VolumeBox");
		Plane* p = new Plane( Vector( 0, 0, 1 ), box.Max );
		volume.Planes.Add(p);
		visible[5] = false;
	}

	// Add planes generated from -X sides
	if( visible[0] )
	{
		if( !visible[2] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Min.X, box.Min.Y, box.Min.Z ),
			Vector( box.Min.X, box.Min.Y, box.Max.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[3] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Min.X, box.Max.Y, box.Max.Z ),
			Vector( box.Min.X, box.Max.Y, box.Min.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[4] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Min.X, box.Max.Y, box.Min.Z ),
			Vector( box.Min.X, box.Min.Y, box.Min.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[5] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Min.X, box.Min.Y, box.Max.Z ),
			Vector( box.Min.X, box.Max.Y, box.Max.Z ) );
			volume.Planes.Add( p );
		}
	}

	// Add planes generated from +X sides
	if( visible[1] )
	{
		if( !visible[2] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Max.X, box.Min.Y, box.Max.Z ),
			Vector( box.Max.X, box.Min.Y, box.Min.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[3] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Max.X, box.Max.Y, box.Min.Z ),
			Vector( box.Max.X, box.Max.Y, box.Max.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[4] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Max.X, box.Min.Y, box.Min.Z ),
			Vector( box.Max.X, box.Max.Y, box.Min.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[5] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Max.X, box.Max.Y, box.Max.Z ),
			Vector( box.Max.X, box.Min.Y, box.Max.Z ) );
			volume.Planes.Add( p );
		}
	}

	// Add planes generated from -Y sides
	if( visible[2] )
	{
		if( !visible[0] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Min.X, box.Min.Y, box.Max.Z ),
			Vector( box.Min.X, box.Min.Y, box.Min.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[1] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Min.X, box.Min.Y, box.Min.Z ),
			Vector( box.Min.X, box.Min.Y, box.Max.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[4] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Min.X, box.Min.Y, box.Min.Z ),
			Vector( box.Max.X, box.Min.Y, box.Min.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[5] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Max.X, box.Min.Y, box.Max.Z ),
			Vector( box.Min.X, box.Min.Y, box.Max.Z ) );
			volume.Planes.Add( p );
		}
	}

	// Add planes generated from +Y sides
	if( visible[3] )
	{
		if( !visible[0] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Min.X, box.Max.Y, box.Min.Z ),
			Vector( box.Min.X, box.Max.Y, box.Max.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[1] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Max.X, box.Max.Y, box.Max.Z ),
			Vector( box.Max.X, box.Max.Y, box.Min.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[4] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Max.X, box.Max.Y, box.Min.Z ),
			Vector( box.Min.X, box.Max.Y, box.Min.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[5] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Min.X, box.Max.Y, box.Max.Z ),
			Vector( box.Max.X, box.Max.Y, box.Max.Z ) );
			volume.Planes.Add( p );
		}
	}

	// Add planes generated from -Z sides
	if( visible[4] )
	{
		if( !visible[0] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Min.X, box.Min.Y, box.Min.Z ),
			Vector( box.Min.X, box.Max.Y, box.Min.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[1] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Max.X, box.Max.Y, box.Min.Z ),
			Vector( box.Max.X, box.Min.Y, box.Min.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[2] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Max.X, box.Min.Y, box.Min.Z ),
			Vector( box.Min.X, box.Min.Y, box.Min.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[3] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Min.X, box.Max.Y, box.Min.Z ),
			Vector( box.Max.X, box.Max.Y, box.Min.Z ) );
			volume.Planes.Add( p );
		}
	}

	// Add planes generated from +Z sides
	if( visible[5] )
	{
		if( !visible[0] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Min.X, box.Max.Y, box.Max.Z ),
			Vector( box.Min.X, box.Min.Y, box.Max.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[1] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Max.X, box.Min.Y, box.Max.Z ),
			Vector( box.Max.X, box.Max.Y, box.Max.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[2] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Min.X, box.Min.Y, box.Max.Z ),
			Vector( box.Max.X, box.Min.Y, box.Max.Z ) );
			volume.Planes.Add( p );
		}

		if( !visible[3] )
		{
            m_setTag("VolumeBox");
			Plane* p = new Plane( point,
			Vector( box.Max.X, box.Max.Y, box.Max.Z ),
			Vector( box.Min.X, box.Max.Y, box.Max.Z ) );
			volume.Planes.Add( p );
		}
	}
}

bool SceneRender::EntityVisible( Entity* entity, Vector& eyePosition )
{
	// 1. Get eye zone from eyePosition. If no zone, all entities visible
#ifdef	TRANSGAMING
	int zoneIndex = GEngine->pcfWorld->FindZone( eyePosition );
	if( zoneIndex == EInvalidIndex ) return true;

	Zone* zone = GEngine->pcfWorld->Zones[zoneIndex];
#else
	int zoneIndex = GEngine->World->FindZone( eyePosition );
	if( zoneIndex == EInvalidIndex ) return true;

	Zone* zone = GEngine->World->Zones[zoneIndex];
#endif

	// 2. Check if entity is in the same zone we are; return true if that is so.
	if( entity->ZoneLinks[zoneIndex].ZoneLink == zone )
		return true;

	// 3. Prepare structures
	unsigned char EntityVisibleZonesVisited[64];
	::memset( EntityVisibleZonesVisited, 0, 64 );

	// 4. Create frustum from point to entity box, covering entity box.
	ConvexVolumeResizable volume;
	CreateBoxVolume( entity->Box, eyePosition, volume );

	// 5. Call recurrent method that will tell you if your entity is visible in any direction
	return EntityVisible( eyePosition, entity, volume, zoneIndex, EntityVisibleZonesVisited );
}

bool SceneRender::EntityVisible( const Vector& point, Entity* entity, ConvexVolumeResizable& volume, int zoneIndex, unsigned char* zonesVisited )
{
#ifdef	TRANSGAMING
	Zone* zone = GEngine->pcfWorld->Zones[zoneIndex];
#else
	Zone* zone = GEngine->World->Zones[zoneIndex];
#endif
	zonesVisited[zoneIndex] = 1;

	// For each portal in zone:
	for( int index = 0; index < zone->Portals.Size(); ++index )
	{
		Portal* portal = zone->Portals[index];

		if( portal->IsDisabled ) continue;

		// 1. Check if it leads to visited zone; if so, continue
		int otherZoneIndex = -1;
		bool visited = false;
		for( int i = 0; i < portal->ZoneRefs.Size(); ++i) 
		{
			int temp = portal->ZoneRefs[i];
			if( temp != zoneIndex )
			{
				otherZoneIndex = temp;
				if( zonesVisited[otherZoneIndex] )
				{
					visited = true;
					break;
				}
			}
		}

		if( visited )
			continue;

		// 2. Cut frustum with it - we'll get a new frustum or an empty object. If it's empty, continue.
		ConvexVolumeResizable newVolume;
		if( !CreateEntityVisibilityVolume( point, volume, portal, newVolume ) )
			continue;

		// 3. If zone that frustum leads to is a zone the entity is in, return true.
		Assert( otherZoneIndex != -1 );
#ifdef	TRANSGAMING
		if( entity->ZoneLinks[otherZoneIndex].ZoneLink == GEngine->pcfWorld->Zones[otherZoneIndex] )
			return true;
#else
		if( entity->ZoneLinks[otherZoneIndex].ZoneLink == GEngine->World->Zones[otherZoneIndex] )
			return true;
#endif

		// 4. return the data from recurrent call of this method, with new frustum, in new zone.
		return EntityVisible( point, entity, newVolume, otherZoneIndex, zonesVisited );
	}
	return false;
}

bool SceneRender::CreateEntityVisibilityVolume( const Vector& point, const ConvexVolumeResizable& frustum, const Portal* portal, ConvexVolumeResizable& volume )
{
	ConvexPolygon poly = portal->Poly;	

	for( int i = 0; i < frustum.Planes.Size(); ++i )
	{
		ConvexPolygon front, back;
		int r = poly.Split( *frustum.Planes[i], front, back );
		if( r == EInBackOf ) continue;
		if( r == EInFrontOf ) return false;
		poly = back;
	}

	volume = frustum;

	// Repair orientation of poly, if wrong
#ifdef	TRANSGAMING
	if( GEngine->pcfWorld->Planes[portal->PlaneIndex].Distance( point ) < 0.0f )
		poly.Reverse();
#else
	if( GEngine->World->Planes[portal->PlaneIndex].Distance( point ) < 0.0f )
		poly.Reverse();
#endif

	// Add new planes to volume; planes come from new cut portal
	for( int i = 0; i < poly.VertexCount; ++i )
	{
        m_setTag("Volume");
		Plane* p = new Plane( point, poly.Verts[i], poly.Verts[(i+1)%poly.VertexCount] );
		volume.Planes.Add( p );
	}
	return true;
}

// Potentially Visible Set
void SceneRender::RecalculateWorldPVS( void )
{

#ifdef	TRANSGAMING
	Assert( GEngine->pcfWorld );

	for( int i = 0; i < GEngine->pcfWorld->Zones.Size(); ++i )
		RecalculateZonePVS( i );
#else
	Assert( GEngine->World );

	for( int i = 0; i < GEngine->World->Zones.Size(); ++i )
		RecalculateZonePVS( i );
#endif
}

void SceneRender::RecalculateZonePVS( int zoneIndex )
{
#ifdef	TRANSGAMING
	Zone* zone = GEngine->pcfWorld->Zones[zoneIndex];
#else
	Zone* zone = GEngine->World->Zones[zoneIndex];
#endif

	// 1. Our zone is visible from our zone
	zone->VisibleZones.Add( zone );

	// 2. All directly adjacent zones are visible from our zone
	// 3. For all exits from them, create portal volumes and check by recursive entering.
	DynamicArray<Zone*> directlyConnectedZones;

	for( int index = 0; index < zone->Portals.Size(); ++index )
	{
		Portal* portal = zone->Portals[index];
		for( int i = 0; i < portal->ZoneRefs.Size(); ++i) 
			if( portal->ZoneRefs[i] != zoneIndex )
			{
#ifdef	TRANSGAMING
				Zone* otherZone = GEngine->pcfWorld->Zones[portal->ZoneRefs[i]];
#else
				Zone* otherZone = GEngine->World->Zones[portal->ZoneRefs[i]];
#endif
				directlyConnectedZones.AddUnique( otherZone );
				zone->VisibleZones.AddUnique( otherZone );

				// For each portal not on same plane as source, create PortalToPortalVolume and use it to
				// find further zones

				for( int j = 0; j < otherZone->Portals.Size(); ++j )
				{
					Portal* otherPortal = otherZone->Portals[j];
					if( portal->PlaneIndex == otherPortal->PlaneIndex )
						continue;	// TO DO - do this elimination better

					int thirdZone = -1;
					for( int k = 0; k < otherPortal->ZoneRefs.Size(); ++k) 
						if( otherPortal->ZoneRefs[k] != portal->ZoneRefs[i] )
						{
							thirdZone = k;
							break;
						}

					Assert( thirdZone != -1 );
					ConvexVolumeResizable volume;
					CreatePortalToPortalVolume( zoneIndex, portal, otherPortal->Poly, volume );
					RecalculateZonePVSRecurrent( zoneIndex, portal, volume, thirdZone, otherPortal );
				}
				break;
			}
	}
}

void SceneRender::CreatePortalToPortalVolume( int startZone, Portal* sourcePortal, ConvexPolygon& destPortalPoly, ConvexVolumeResizable& volume )
{
	// get source portal center
	Vector sourceCenter = sourcePortal->Box.GetCenter();
	ConvexPolygon& sourcePoly = sourcePortal->Poly;

	// for each line of destination portal polygon
	for( int i = 0; i < destPortalPoly.VertexCount; ++i )
	{
		Vector& first = destPortalPoly.Verts[i];
		Vector& second = destPortalPoly.Verts[(i+1)%destPortalPoly.VertexCount];

		// 1. Create plane from two points of this line, and the center
		Plane temp( sourceCenter, first, second );

		// 2. Find out the point of source portal that has the lowest Distance from this polygon
		float minDist = 10000000.0f;
		Vector bestPoint( 0, 0, 0 );

		for( int j = 0; j < sourcePoly.VertexCount; ++j )
		{
			float newDist = temp.Distance( sourcePoly.Verts[j] );
			if( newDist < minDist )
			{
				minDist = newDist;
				bestPoint = sourcePoly.Verts[j];
			}
		}

		// 3. Create another plane from those two points, and found point. Add this plane to volume.
        m_setTag("Portal");
		Plane* p = new Plane( first, second, bestPoint );
		volume.Planes.Add( p );
	}
}

void SceneRender::RecalculateZonePVSRecurrent( int startZone, Portal* startPortal, ConvexVolumeResizable& volume, int zoneIndex, Portal* sourcePortal )
{
#ifdef	TRANSGAMING
	Zone* zone = GEngine->pcfWorld->Zones[zoneIndex];

	// Insert zone into visibility list
	Zone* startZ = GEngine->pcfWorld->Zones[startZone];
#else
	Zone* zone = GEngine->World->Zones[zoneIndex];

	// Insert zone into visibility list
	Zone* startZ = GEngine->World->Zones[startZone];
#endif
	startZ->VisibleZones.AddUnique( zone );

	// For each portal in zone:
	for( int index = 0; index < zone->Portals.Size(); ++index )
	{
		Portal* portal = zone->Portals[index];

		if( portal->IsDisabled || ( portal == sourcePortal ) || ( portal->PlaneIndex == sourcePortal->PlaneIndex ) )
			continue;	// TO DO - do this elimination better

		// 1. Check if it leads to visited zone; if so, continue
		int otherZoneIndex = -1;
		for( int i = 0; i < portal->ZoneRefs.Size(); ++i) 
		{
			int temp = portal->ZoneRefs[i];
			if( temp != zoneIndex )
				otherZoneIndex = temp;
		}

		Assert( otherZoneIndex != -1 );

		// 2. Cut portal with current volume. If the result is empty, continue.

		ConvexPolygon poly = portal->Poly;	
		for( int i = 0; i < volume.Planes.Size(); ++i )
		{
			ConvexPolygon front, back;
			int r = poly.Split( *volume.Planes[i], front, back );
			if( r == EInBackOf ) continue;
			if( r == EInFrontOf ) { poly.VertexCount = 0; break; }
			poly = back;
		}
		if( !poly.VertexCount ) continue;

		// Repair orientation of poly, if wrong
#ifdef	TRANSGAMING
		if( GEngine->pcfWorld->Planes[portal->PlaneIndex].Distance( startPortal->Box.GetCenter() ) < 0.0f )
			poly.Reverse();
#else
		if( GEngine->World->Planes[portal->PlaneIndex].Distance( startPortal->Box.GetCenter() ) < 0.0f )
			poly.Reverse();
#endif

		// 3. Create new volume, made from our original portal, and new cut portal
		ConvexVolumeResizable newVolume;
		CreatePortalToPortalVolume( startZone, startPortal, poly, newVolume );

		// 4. execute the recurrent call in next zone
		RecalculateZonePVSRecurrent( startZone, startPortal, newVolume, otherZoneIndex, portal );
	}
}

void SceneRender::OccludeReflection(const DynamicArray<WorldMesh*>& FixedList, 
									const Plane& ReflectionPlane,
									const BoundingBox& ReflectionBox)
{
	++VisCounter;
    if (VisCounter>0x7fffffff-1)
        VisCounter=0;

	for (int i=0; i<FixedList.Size(); ++i) {
		Entity* entity = (Entity*) FixedList[i];
		Assert(entity);

		const Vector boxCenter = entity->Box.GetCenter();
		const Vector boxExtent = entity->Box.GetExtent();

		if ( mViewport.ViewFrustum.BoxTest( boxCenter, boxExtent ) ) {
			EntityList.Insert(entity,(mViewport.EyePosition-boxCenter).MagnitudeSquared());
		}
	}

    for (Entity* entity=mWorld->EntityList; entity; entity=entity->Next) {
		if (entity->Flags & Entity::EEntAlwaysVisible) {

			const Vector boxCenter = entity->Box.GetCenter();
			EntityList.Insert(entity,(mViewport.EyePosition-boxCenter).MagnitudeSquared());

		} else if (entity->Flags & Entity::EEntNoDraw) {
			continue;

		} else {
			if ((entity->AsMesh() && (entity->Flags & Entity::EEntMovable)) || !entity->AsMesh()) {

				if (!ReflectionBox.Intersect(entity->Box))
					continue;

				const Vector boxCenter=entity->Box.GetCenter();
				const Vector boxExtent = entity->Box.GetExtent();

				if (entity->AsModel() && ReflectionPlane.Distance(boxCenter) > 10)
					continue;

				if ( mViewport.ViewFrustum.BoxTest( boxCenter, boxExtent ) ) {
					EntityList.Insert(entity,(mViewport.EyePosition-boxCenter).MagnitudeSquared());
				}
			}
		}
	}
}
