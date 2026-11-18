#error This file is obselete
#include "Engine.h"

IMPLEMENT_OBJECT_CLASS(Zone);
IMPLEMENT_OBJECT_CLASS(Portal);
IMPLEMENT_OBJECT_CLASS(WorldMesh);

Level::Level()
{
}

Level::~Level()
{
    Close();
}

BOOL Level::Load(FileObject* file)
{
    guard(Level::Load);
    Assert(file);
    return TRUE;
    unguard;
}

BOOL Level::Save(FileObject* )
{
    return FALSE;
}

inline static void DumpBox(const BoundingBox& box)
{
	if(box.Valid())
		GLog.Print("(%.2f,%.2f,%.2f) (%.2f,%.2f,%.2f)",
		    		box.Min.X,box.Min.Y,box.Min.Z,
					box.Max.X,box.Max.Y,box.Max.Z);
    else
		GLog.Print("(Box Invalid)");
}

BOOL Level::LoadPackage(const char* name)
{
	guard(Level::LoadPackage);
	ObjectPackage* package = ObjectPackage::GFindPackage(name);
	if(package)
	{
		GLog.Print("LoadPackage(): %s loaded...\n",name);
        package->Dump(GLog);

        for(ClassIterator<Zone> i(package); i; ++i)
        {
            Zone* zone = i.GetInstance();
			Zones.Add(zone);

			GLog.Print("Zone Box\t:");
			DumpBox(zone->Box);
			GLog.Print("\n");
        }

        for(ClassIterator<Portal> i(package); i; ++i)
        {
            Portal* portal = i.GetInstance();
			GLog.Print("Portal Box\t:");
			DumpBox(portal->Box);
			GLog.Print("\n");

			// create portal plane
			portal->Poly.PlaneIndex=CreatePlane(portal->Poly.Verts[0],
			                                    portal->Poly.Verts[1],
				                                portal->Poly.Verts[2]);



			// set up reverse poly, since portals are really two sided
			portal->BuildOtherPoly();     
			portal->OtherPoly.PlaneIndex=CreatePlane(portal->OtherPoly.Verts[0],
													 portal->OtherPoly.Verts[1],
													 portal->OtherPoly.Verts[2]);

			Portals.Add(portal);
        }
        
        for(ClassIterator<WorldMesh> i(package); i; ++i)
        {
            WorldMesh* mesh = i.GetInstance();
			Meshes.Add(mesh);

			GLog.Print("mesh: %s [L:%s] (%d, %d, %d) CloneOf: %s",
						mesh->Name.AsChar(),mesh->LightMapName.AsChar(),
						mesh->Verts.Size(),mesh->Triangles.Size()/3,mesh->Elems.Size(),
						mesh->CloneOf.AsChar());

			DumpBox(mesh->Box);
			GLog.Print("\n");
        }
        
        return TRUE;
	}

    return FALSE;
	unguard;
}

void Level::InitZones()
{
    guard(Level::InitZones);

	for(int i=0; i<Portals.Size(); ++i)
		Portals[i]->Zones.ClearArray();

	for(int i=0; i<Zones.Size(); ++i)
	{
		Zones[i]->Portals.ClearArray();
		Zones[i]->ClearEntityList();
	}

	// first, deal with explicit portal links
    for(int portalIndex=0; portalIndex<Portals.Size(); ++portalIndex) {
        Portal* portal=Portals[portalIndex];
        // see which zones are referenced
        for(int zoneIndex=0; zoneIndex<Zones.Size(); ++zoneIndex) {
            Zone* zone=Zones[zoneIndex];
            if(portal->Name.Find(zone->Name)) {
                portal->Zones.AddUnique(zone);
                zone->Portals.AddUnique(portal);
            }
        }
    }

	// build a mesh and portal ref list for each zone
	for(int zoneIndex=0; zoneIndex<Zones.Size(); ++zoneIndex) {
		Zone* zone=Zones[zoneIndex];

		for(int portalIndex=0; portalIndex<Portals.Size(); ++portalIndex) {
			Portal* portal=Portals[portalIndex];

    		// add two fake vertices, to grow the box along the plane
	    	// in both directions, making it thicker for zone intersection
            const float portalThicknessTolerance=4.f;
			BoundingBox box=portal->Box;
			box+=portal->Poly.Verts[0]+Planes[portal->Poly.PlaneIndex].Normal*portalThicknessTolerance;
			box+=portal->Poly.Verts[0]-Planes[portal->Poly.PlaneIndex].Normal*portalThicknessTolerance;

			if(zone->Box.Intersect(box)) {                
                portal->Zones.AddUnique(zone);
                zone->Portals.AddUnique(portal);
			}
		}

	}

    unguard;
}

void Level::InitRender (RenderManager* render)
{
	guard(Level::InitRender);
	MaterialSystem& matSys=render->GetMaterialSystem();
	for(int i=0; i<Meshes.Size(); ++i)
	{
		WorldMesh* mesh=Meshes[i];
		for(int j=0; j<mesh->Groups.Size(); ++j)
			//mesh->Groups[j].MaterialIndex=matSys.GetTexture(mesh->Groups[j].MaterialName.AsChar());
			mesh->Groups[j].MaterialIndex=matSys.FindMaterial(mesh->Groups[j].MaterialName.AsChar());

		mesh->LightMapIndex=matSys.GetTexture(mesh->LightMapName.AsChar());
		mesh->Precache(render->GetDevice());
	}
	unguard;
}

void Level::InitPhysics(PhysicsEngine* physics)
{
}

int Level::CreatePlane(const Vector& a, const Vector& b, const Vector& c)
{
	Plane plane(a,b,c);
	plane.Snap();

 	for(int i=0; i<Planes.Size(); ++i) {
		if(plane.Equal(Planes[i])) {
			return i;
		}
	}

    return Planes.Add(plane);
}

void Level::Close()
{
    guard(Level::Close);
    unguard;
}

// ========================================================================

void FWorld::Init()
{	
	mLevel.InitZones();
	for(int i=0; i<mLevel.Meshes.Size(); ++i)
		EntityList.Insert(TEntityPtr(mLevel.Meshes[i]));

    for(TEntityPtr* p=EntityList; p; p=p->Next)
	{
		for(int zoneIndex=0; zoneIndex<mLevel.Zones.Size(); ++zoneIndex) 
		{
			Zone* zone=mLevel.Zones[zoneIndex];

			Entity* entity=p->Get();
			if(entity->Box.Valid() && zone->Box.Intersect(entity->Box))
				zone->AddEntity(entity);
		}
	}

	InitPhysics();

	//if(!GEngine->IsDedicated())
	InitRender();
}

void FWorld::InitRender()
{
	RenderManager* render=GEngine->GetRender();
	mLevel.InitRender(render);
	// models, etc
}

void FWorld::InitPhysics()
{
}

void FWorld::Dump()
{
	// dump info
	DynamicArray<Zone*>& zones=mLevel.Zones;
    for(int i=0; i<zones.Size(); ++i)
    {
        const Zone* zone=zones[i];
        GLog.Print("Zone '%s' -> entity list:\n",zone->Name.AsChar());

        for(TEntityPtr* p=zone->EntityList; p; p=p->Next)
            GLog.Print("\t%s\n",(*p)->Name.AsChar());
    }

	DynamicArray<Portal*>& portals=mLevel.Portals;
	for(int i=0; i<portals.Size(); ++i)
	{
		const Portal* portal=portals[i];
		GLog.Print("Portal '%s' -> zone list:\n",portal->Name.AsChar());

		for(int j=0; j<portal->Zones.Size(); ++j)
			GLog.Print("\t%s\n",portal->Zones[j]->Name.AsChar());
	}
}

void FWorld::Tick(float timeDelta)
{
}
