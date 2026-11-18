#error This file is obselete
class ENGINE_API FWorld;

class ENGINE_API Level
{
public:
    Level();
    ~Level();

	BOOL Load(FileObject* file);
	BOOL Save(FileObject* file);

	BOOL LoadPackage(const char* name);
	void InitZones();

    void Close();
	void Dump();

	void InitRender (RenderManager* render);
	void InitPhysics(PhysicsEngine* physics);

private:
    int  CreatePlane(const Vector& a, const Vector& b, const Vector& c);

private:
    DynamicArray<Plane>         Planes;

	DynamicArray<Zone*>		    Zones;
	DynamicArray<Portal*>	    Portals;
    DynamicArray<WorldMesh*>    Meshes;

	friend class FWorld;
};

// =================================================================

class ENGINE_API FWorld
{
public:
	void Init();
	void Dump();
	void Tick(float timeDelta);

	void InitRender();
	void InitPhysics();

	Level& GetLevel() { return mLevel; }
	DynamicArray<Zone*>& GetZones() { return mLevel.Zones; }
	DynamicArray<Portal*>& GetPortals() { return mLevel.Portals; }
	DynamicArray<Plane>& GetPlanes() { return mLevel.Planes; }
	TEntityList& GetEntityList() { return EntityList; }

private:
	Level			mLevel;
	TEntityList		EntityList;
};
