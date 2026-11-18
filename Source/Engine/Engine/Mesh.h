
// #pragma once
#ifndef _MESH_
#define _MESH_


#include "Core/AbstractFactory.h"
#include "Engine/SimpleMesh.h"
#include "Engine/Entity.h"

#ifdef TRANSGAMING
# define MAX_MESH_SHADOWS			(2)
# ifdef TRANSGAMING_NON_TNL
#  define MAX_MESH_SHADOWS_PER_PASS	(7)
# endif
#else
# define MAX_MESH_SHADOWS			(16)
# define MAX_MESH_SHADOWS_PER_PASS	(7)
#endif

struct TWater;
#define INVALID_MPKINDEX    31
#define MAX_MPKINDEX        30


class ENGINE_API WorldMesh : public Entity, public SimpleMesh
{
	friend class SkyLayer;
public:
    DECLARE_OBJECT_CLASS(WorldMesh);


    class VolumeParams{
    public:
#ifdef TRANSGAMING
        Color_TG    color;
        float       end;

# ifdef TRANSGAMING_PS2
        VolumeParams(): color(0x00000000u), end(0.0f){}
# else
        VolumeParams(): color(0x00000000ul), end(0.0f){}
# endif
        VolumeParams(Color_TG _color, float _end): color(_color), end(_end){}
#else
        Color       color;
        float       end;

        VolumeParams(): color(0ul), end(0.0f){}
        VolumeParams(Color _color, float _end): color(_color), end(_end){}
#endif
    };

    BYTE			ActiveGroup;
	float			Time;
	Light*			SpecularLights[2];
	FLOAT			SpecularPower;
    unsigned      	FixedMeshIndex:16;
    unsigned        MPKIndex:5;
	bool			IsNTUGlass:1;
	bool			UseWaterEnv:1;
    bool            mtformat:1;
    bool            finalformat:1;
    bool            hasVolumeInfo:1;
	String			DefaultShaderName;

#ifdef TRANSGAMING_NON_TNL
	Material*		ShadowShaders[MAX_MESH_SHADOWS_PER_PASS];
#endif
	Model*			ShadowCasters[MAX_MESH_SHADOWS];
	int				NumShadowCasters;

	WorldMesh();
	WorldMesh(const WorldMesh& mesh);
    WorldMesh(const WorldMesh &mesh, int evilHack);
	~WorldMesh();

	void RecomputeBox();
	void TransformToObjectSpace();
    void PreScale(float scale);
	Vector CenterGeometry();
	void GetClosestPoint( Vector sourcePoint, float& distance, Vector& closestPoint );
    Vector GetRandomPoint();

    // Entity serialization interface
    void Load(FileObject* file);
    void Save(FileObject* file);
	void Tick(const float timeDelta);
	void UpdateVolEntity();

	// Load/save for the savegames
	virtual bool LoadEntity( FileObject* f );
	virtual bool SaveEntity( FileObject* f );

    // Entity interface
    void RenderInitialize(RenderDevice* dev, MaterialSystem* matSys, const char* texSubPath=0);
    void RenderCleanup   (RenderDevice* dev, MaterialSystem* matSys);

    void InitializeVBs(RenderDevice* dev);

	void ReloadTextures();
	void SetupShaders();
	void InitializeTextures(RenderDevice* dev, MaterialSystem* matSys, const char* texSubPath=0);
    void CleanupTextures(RenderDevice* dev, MaterialSystem* matSys);

	void Draw(RenderDevice* dev, int mode);
	void DrawZ(const Matrix& WorldToScreen);
	void DrawShadow(Model* Caster);
	void DrawShadows();

	// new file format
	BOOL LoadMesh(FileObject* file, VolumeParams *params = NULL);
	BOOL SaveMesh(FileObject* file);

    virtual bool isVolume() const{ return false; }

	// parse mesh name and setup flags 
	void SetupFlags();
	void SetDefaultMaterial(const char* Name);
	void SetupWaterShader();
	void SetupGlassShader();
	void SetSpecular(float Power);
	void AddSpecularLight(Light* L);

	Plane GetReflectionPlane() const;

#ifdef TRANSGAMING
public:
    //vertex array used for non-lightmapped static world geometry
    //DynamicArray<RenderVertexN1> RN1Verts;

    //DynamicArray<RenderVertexNC1> RNC1Verts;
#endif

private:
    void SetupMaterials(const char* texSubPath);

	void RenderWater(const TWater& Water);
	void RenderNTU(Material* shader);
	void RenderLightPass(const Light* light);
	void RenderShadowPass(Model* model);
#ifdef TRANSGAMING_NON_TNL
	void RenderShadows(Model** Casters, int NumCasters);
	void RenderUberLightPass(const Light* Lights[], int NumLights);
    void RenderTU2Specular();
#endif
};


#endif //_MESH_

