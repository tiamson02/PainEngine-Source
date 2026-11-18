#include "Engine.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Render/Decal.h"
#include "Render/View.h"
#include "Engine/Mesh.h"
#include "Render/FogVolume.h"
#include "Render/Video.h"
#include "Engine/Model.h"
#include "Engine/World.h"
#include "Render/Material.h"
#include "Physics/PhysicsObject.h"
#include "Physics/PhysicsEngine.h"
#include "Physics/PhysicsWorld.h"

//#define DEBUG_SCISSOR

#ifdef DEBUG_SCISSOR
#include "Render/HID.h"
#endif

/* min and max macros */
#ifndef NOMINMAX
#ifndef max
#define max(a,b)   (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)   (((a) < (b)) ? (a) : (b))
#endif
#endif  /* NOMINMAX */

#define UBER_SHADER
#define COLLAPSE_SHADOWS
#define COLLAPSE_TU2SPEC

WorldMesh::WorldMesh() 
:   Entity(Entity::ETypeMesh), MPKIndex(INVALID_MPKINDEX), FixedMeshIndex(65535), ActiveGroup(0), Time(0)
,	IsNTUGlass(0), UseWaterEnv(0), mtformat(false), finalformat(false), hasVolumeInfo(false)
{
    //TRACE("WorldMesh::WorldMesh(): creating a new world mesh at %p\n", this);
	Flags |= Entity::EEntDestroyOnReload;

	SpecularLights[0] = NULL;
	SpecularLights[1] = NULL;

	SpecularPower = 8.0F;
	NumShadowCasters = 0;
	memset(ShadowCasters,0,sizeof(ShadowCasters));
#ifdef TRANSGAMING_NON_TNL
	memset(ShadowShaders,0,sizeof(ShadowShaders));
#endif
}


WorldMesh::WorldMesh(const WorldMesh& mesh)
: Entity(Entity::ETypeMesh), MPKIndex(INVALID_MPKINDEX), ActiveGroup(0), FixedMeshIndex(65535), Time(0)
{
    //TRACE("WorldMesh::WorldMesh(): copying the world mesh '%s' from %p to %p\n", mesh.Name.AsChar(), &mesh, this);
	Flags |= Entity::EEntDestroyOnReload;			
	Name			= mesh.Name;
	ObjectToWorld	= mesh.ObjectToWorld;
	Flags			= mesh.Flags;
	//Verts			= mesh.Verts;
	Box				= mesh.Box;
	LightMapName	= mesh.LightMapName;
	AlphaMap		= mesh.AlphaMap;
    mtformat        = mesh.mtformat;
    finalformat     = mesh.finalformat;
    hasVolumeInfo   = mesh.hasVolumeInfo;
	Data			= mesh.Data;
	Data->AddRef();


    // remember speculars
	SpecularLights[0] = mesh.SpecularLights[0];
	SpecularLights[1] = mesh.SpecularLights[1];

	SpecularPower	= mesh.SpecularPower;
}

WorldMesh::WorldMesh(const WorldMesh &mesh, int evilHack)
: Entity(Entity::ETypeMesh), MPKIndex(INVALID_MPKINDEX), ActiveGroup(0), FixedMeshIndex(65535), Time(0)
{
    //TRACE("WorldMesh::WorldMesh(evil): copying the world mesh '%s' from %p to %p\n", mesh.Name.AsChar(), &mesh, this);
	Flags |= Entity::EEntDestroyOnReload;			
	Name			= mesh.Name;
	ObjectToWorld	= mesh.ObjectToWorld;
	Flags			= mesh.Flags;
	//Verts			= mesh.Verts;
	Box				= mesh.Box;
	LightMapName	= mesh.LightMapName;
	AlphaMap		= mesh.AlphaMap;
    mtformat        = mesh.mtformat;
    finalformat     = mesh.finalformat;
    hasVolumeInfo   = mesh.hasVolumeInfo;
	Data			= mesh.Data;
	Data->AddRef();


    // remember speculars
	SpecularLights[0] = mesh.SpecularLights[0];
	SpecularLights[1] = mesh.SpecularLights[1];

	SpecularPower	= mesh.SpecularPower;
}


WorldMesh::~WorldMesh( void )
{
	// Check if we're a glass fragment and delete us from this glass

	if( PO )
	{
#ifdef TRANSGAMING_NO_HAVOK
		Glass* glass = NULL;
#else
		Glass* glass = PO->GetGlass();
#endif
		if( glass )
			glass->ForgetFragment( this );
	}
}


// Entity serialization interface
void WorldMesh::Load(FileObject* file)
{
    guard(WorldMesh::Load);
    Entity::Load(file);
    *file >> *this;

	ObjectToWorld.SetIdentity();    // = GIdentityMatrix;

    unguard;
}

void WorldMesh::Save(FileObject* file)
{
    guard(WorldMesh::Save);
	Entity::Save(file);
    *file << *this;
    unguard;
}


BOOL WorldMesh::LoadMesh(FileObject* file, VolumeParams *params)
{
	DWORD magic;
	int uvSetCount,groupCount;
	bool isTriStrip;

	*file >> magic;
	if (magic != 0xDeafBabe)
		return FALSE;

	*file >> Name >> ObjectToWorld >> uvSetCount;

#ifdef TRANSGAMING
	// Did we set the top bit to tri-strip-mode?
	isTriStrip =    (uvSetCount & MESH_TRISTRIP_BIT) ? true : false;

# ifdef TRANSGAMING_DEINDEX
    // is this data in an indexed format?  We can't trust the size of the Triangles array since it isn't always checked
    isIndexed =     (uvSetCount & MESH_DEINDEXED_BIT) ? false : true;
# endif

    mtformat =      (uvSetCount & MESH_MTFORMAT_BIT) ? true : false;
# ifdef TRANSGAMING_PSP
    hasBoundingBox = (uvSetCount & MESH_BOUNDINGBOX_BIT) ? true : false;
    finalformat =   (uvSetCount & MESH_PSPDATA_BIT) ? true : false;

    //make sure we're not trying to load a PS2 specific mesh
    Assert((uvSetCount & MESH_PS2DATA_BIT) == 0);
# elif defined(TRANSGAMING_PS2)
    finalformat =   (uvSetCount & MESH_PS2DATA_BIT) ? true : false;

    //make sure we're not trying to use a PSP specific mesh
    Assert((uvSetCount & MESH_PSPDATA_BIT) == 0);
# endif

    hasVolumeInfo = (uvSetCount & MESH_VOLUMEPARAMS_BIT) ? true : false;


    //make sure both formats aren't specified simultaneously
    Assert(!(mtformat && finalformat));


    // Get the original uvSetCount...
	uvSetCount &= MESH_UVCOUNT_MASK;


	// Determine the PrimType for the mesh.
	if ( isTriStrip )
		PrimType = RenderDevice::EPrimTriStrip;
	else
		PrimType = RenderDevice::EPrimTriList;
#endif

	ObjectToWorld.SetIdentity();  // = GIdentityMatrix;

    if (uvSetCount == 1){
		Flags |= EVertFormatNTU;
        Assert(!mtformat);
    }
	

    //read the volume information
    if (hasVolumeInfo)
        file->Read(params, sizeof(VolumeParams));


    //read the vertices
#ifdef TRANSGAMING
    if (mtformat){
        Data->VertList.load(*file, VertexList::VTXFMT_MT);
        Assert(uvSetCount == 2);
    }

    else if (finalformat){
        Data->VertList.load(*file, VertexList::VTXFMT_MESH);
        Assert(uvSetCount == 1);
    }

    else{
        Data->VertList.load(*file, VertexList::VTXFMT_NONE);

        switch (uvSetCount){
            case 1:
                TRACE("WorldMesh::LoadMesh(): converting %d vertices to '%s' format (from '%s') for object '%s' (data = %p, vertList = %p, this = %p)\n", Data->VertList.Size(), Data->VertList.getFormatName(VertexList::VTXFMT_MESH), Data->VertList.getFormatName(), Name.AsChar(), Data, &Data->VertList, this);
                Data->VertList.convert(VertexList::VTXFMT_MESH);
                break;

            case 2:
                TRACE("WorldMesh::LoadMesh(): NOT converting %d vertices to '%s' format (from '%s') for object '%s' (data = %p, vertList = %p, this = %p)\n", Data->VertList.Size(), Data->VertList.getFormatName(VertexList::VTXFMT_MESH), Data->VertList.getFormatName(), Name.AsChar(), Data, &Data->VertList, this);
                //Data->VertList.convert(VertexList::VTXFMT_MT);
                break;

            default:
                TRACE("WorldMesh::LoadMesh(): ERROR-> invalid UV set count: %d\n", uvSetCount);
                Assert(!"Bad UV set count!");
                break;
        }
    }

#else
    *file >> Verts;
#endif
    *file >> Data->Normals >> Box;
    *file >> Data->Triangles;


#ifdef TRANSGAMING_PS2
    TRACE("WorldMesh::LoadMesh(): fixing up the texture coordinates for the mesh '%s'\n", Name.AsChar());
    Data->VertList.fixTexCoords(&Data->Triangles, uvSetCount, PrimType);
#endif

	AlphaMap.Name.Clear();
	LightMapName.Clear();

	// parse groups
	*file >> groupCount;
#ifdef TRANSGAMING_MESH_WARNINGS
# ifdef TRANSGAMING_DEINDEX
    TRACE("WorldMesh::LoadMesh(): loaded the mesh '%s' {indexed = %d, primType = %s, uvCount = %d, verts.size = %d, normals.size = %d, triangles.size = %d, groupCount = %d}\n", Name.AsChar(), isIndexed, isTriStrip ? "strip" : " list", uvSetCount, Data->VertList.Size(), Normals.Size(), Triangles.Size(), groupCount);
# else
    TRACE("WorldMesh::LoadMesh(): loaded the mesh '%s' {indexed = %d, primType = %s, uvCount = %d, verts.size = %d, normals.size = %d, triangles.size = %d, groupCount = %d}\n", Name.AsChar(), Triangles.Size() != 0, isTriStrip ? "strip" : " list", uvSetCount, Data->VertList.Size(), Normals.Size(), Triangles.Size(), groupCount);
# endif
#endif
	Data->Groups.ResizeArray(groupCount);

	for (int i=0; i<groupCount; ++i) {
		Group& group = Data->Groups[i];
		WORD start,count;

		*file >> start >> count;
		group.RenderBase = start;
		group.RenderCount = count;

#ifdef TRANSGAMING_PSP
        if (hasBoundingBox)
            file->Read(group.staticBoundingBox, sizeof(group.staticBoundingBox));
#endif

		MaterialDef colorMap,lightMap,blendMap,alphaMap;
		*file >> colorMap >> lightMap >> blendMap >> alphaMap;

		group.TexName = colorMap.Name;
		if (!LightMapName.Size() && lightMap.Name.Size())
			LightMapName = lightMap.Name;

		AlphaMap = alphaMap;

		group.ColorMap = colorMap;
		group.BlendMap = blendMap;

#ifdef TRANSGAMING_MESH_WARNINGS
        /*TRACE("         group %d)\n"
              "             renderBase =  %d\n"
              "             renderCount = %d\n"
              "             colorMap =    '%s'\n"
              "             lightMap =    '%s'\n"
              "             blendMap =    '%s'\n"
              "             alphaMap =    '%s'\n",
              i,
              group.RenderBase,
              group.RenderCount,
              group.ColorMap.Name.AsChar(),
              LightMapName.AsChar(),
              group.BlendMap.Name.AsChar(),
              AlphaMap.Name.AsChar());//*/
#endif
	}

	Init();

#ifdef TRANSGAMING_STRIPIFY
        //Stripify();
#endif

	return TRUE;
}

BOOL WorldMesh::SaveMesh(FileObject* file)
{
	int uvSetCount = Flags & EVertFormatNTU ? 1 : 2;
	int groupCount = Data->Groups.Size();

	*file << 0xDeafBabe;

#ifdef TRANSGAMING
    //this mesh used tri strips => set the tristrip bit
    if (PrimType == RenderDevice::EPrimTriStrip)
        uvSetCount |= MESH_TRISTRIP_BIT;

# ifdef TRANSGAMING_DEINDEX
    //this mesh has deindexed data => set the deindexed bit
    if (!isIndexed)
        uvSetCount |= MESH_DEINDEXED_BIT;
# endif

    if (mtformat)
        uvSetCount |= MESH_MTFORMAT_BIT;

    if (hasVolumeInfo && isVolume())
        uvSetCount |= MESH_VOLUMEPARAMS_BIT;

# ifdef TRANSGAMING_PSP
    if (finalformat)
        uvSetCount |= MESH_PSPDATA_BIT;
# elif defined(TRANSGAMING_PS2)
    if (finalformat)
        uvSetCount |= MESH_PS2DATA_BIT;
# endif
#endif

	*file << Name << ObjectToWorld << uvSetCount;


    //write out the volume information
    if (hasVolumeInfo && isVolume()){
        Volume *        volume = reinterpret_cast<Volume *>(this);
        VolumeParams    params(volume->VolColor, volume->VolEnd);

        file->Write(&params, sizeof(params));
    }


#ifdef TRANSGAMING
# ifdef DEBUG
    if (mtformat){
        Assert(Data->VertList.getFormat() == VertexList::VTXFMT_MT);
    }

    else if (finalformat){
        Assert(Data->VertList.getFormat() == VertexList::VTXFMT_MESH);
    }

    else{
        Assert(Data->VertList.getFormat() == VertexList::VTXFMT_NONE);
    }
# endif

    Data->VertList.save(*file);
#else
    *file << Verts;
#endif

    *file << Data->Normals << Box;
	*file << Data->Triangles;


	// parse groups
	*file << groupCount;

	MaterialDef lightMap;

	lightMap.Name = LightMapName;

	for (int i=0; i<groupCount; ++i) 
	{
		Group& group = Data->Groups[i];

		*file << (WORD)group.RenderBase << (WORD)group.RenderCount;
		*file << group.ColorMap << lightMap << group.BlendMap << AlphaMap;
	}

	Init();

	return TRUE;
}

void WorldMesh::Tick(const float timeDelta)
{
	Entity::Tick(timeDelta);
	// water animation
	Time += timeDelta;
	Time = fmod(Time,256);
}

void WorldMesh::RecomputeBox()
{
    Data->VertList.computeBox(Box);
	/*Box.Reset();

    for(int i=0; i<Data->VertList.Size(); ++i)
        Box += Data->VertList.getVertPos(i);
	    //Box+=Verts[i].Pos;

	LocalBox = Box;//*/
}

void WorldMesh::TransformToObjectSpace()
{
	Box.Reset();
    Data->VertList.transformToObjectSpace(ObjectToWorld, &Box);
    /*//transformation matrix is the identity -> nothing to do => succeed
    if (ObjectToWorld.IsIdentity())
        return;
    
	Matrix WorldToObject = ObjectToWorld.Inverse();
	Box.Reset();

    for (int i=0; i<Data->VertList.Size(); ++i)
    {
	    //Verts[i].Pos = WorldToObject.TransformCoord(Verts[i].Pos);
	    //Box+=Verts[i].Pos;
        Vector &pos = Data->VertList.getVertPos(i);

        pos = WorldToObject.TransformCoord(pos);
        Box += pos;
    }//*/
}

void WorldMesh::PreScale(float scale)
{
    if (1.0f == scale)
        return;

    Box.Reset();

    Data->VertList.scaleVerts(scale, &Box);
    /*for (int i=0; i<Data->VertList.Size(); ++i){
	    //Verts[i].Pos *= scale;
        Vector &pos = Data->VertList.getVertPos(i);

        pos *= scale;
        Box += pos;
    }//*/

	//RecomputeBox();
}

Vector WorldMesh::CenterGeometry()
{
    Vector tr = Data->VertList.centerGeometry(Box);

	/*RecomputeBox();
	Vector tr = Box.Max - (Box.Max-Box.Min)/2;

    Box.Reset();

    for (int i=0;i<Data->VertList.Size();i++){
		//Verts[i].Pos-=tr;

        Vector &pos = Data->VertList.getVertPos(i);
        
        pos -= tr;
        Box += pos;
    }

	//RecomputeBox();//*/
	
	Flags|=EEntCenteredGeometry;

	return tr;
}

void WorldMesh::SetupFlags()
{ 
	if (strstr(Name.AsChar(),"trans"))
	{
		Flags|=EEntTranslucent;
		Sort = ESortTrans;
	}

	if (strstr(Name.AsChar(),"phys"))
		Flags|=(EPhysics|EEntMovable);

	if (strstr(Name.AsChar(),"water"))
	{
		Flags|=EWater;
#ifdef	TRANSGAMING
		GEngine->pcfWorld->WaterMesh = this;
#else
		GEngine->World->WaterMesh = this;
#endif
	}

	if (strstr(Name.AsChar(),"tele"))
		Flags|=ETeleport;

	if (strstr(Name.AsChar(),"noclip"))
		Flags|=ENoClip;

	if (strstr(Name.AsChar(),"2sided"))
		Flags|=E2Sided;

	if (strstr(Name.AsChar(),"atest"))
		Flags|=EATest;

	if (strstr(Name.AsChar(), "decal")) 
		Sort = ESortDecal;

	if (strstr(Name.AsChar(), "nolight"))
		Flags|=SimpleMesh::ENoLight;

	bool gotAlpha = AlphaMap.Name.Size() ? true : false;
	if (gotAlpha && !strlen(AlphaMap.Name.AsChar()))
		gotAlpha = false;

	if (Flags & EVertFormatNTU)
		gotAlpha = false;

	if (gotAlpha)
		Flags |= EBlendMap;	
/*
	GLog.Print("%c%c%c%c%c%c%c%c %s\n", 
		Flags&EVertFormatNTU	?'N':'L',
		Flags&EEntTranslucent	?'t':' ',
		Flags&EPhysics			?'p':' ',
		Flags&EWater			?'w':' ',
		Flags&ENoClip			?'n':' ',
		Flags&E2Sided			?'2':' ',
		Flags&EATest			?'a':' ',
		Sort==ESortDecal		?'d':' ',
		Name.AsChar());
*/	/*
	bool gotBlend = BlendMap.Name.Size() ? true : false;
	if (gotBlend && !strlen(BlendMap.Name.AsChar()))
		gotBlend = false;

	if (gotAlpha && gotBlend)
		Flags|=EBlendMap;
	*/
}

void WorldMesh::SetupMaterials(const char* texSubPath)
{
    MaterialSystem* matSys = GEngine->Video->MaterialSys;

    TRACE("WorldMesh::SetupMaterials(): setting up materials for the mesh '%s'\n", Name.AsChar());
    if (!(Flags & EVertFormatNTU))
    {
		Lighting = ELightingDLights;
    }
	else
	{
		Lighting = ELightingAllLights;
	}

	if (Flags & ENoDepth)
	{
		String name = "sky";

		if (Flags & EVertFormatNTU)		name += "ntu";
		if (Flags & EEntTranslucent)	name += "trans";

		DefaultShader = matSys->CreateMaterial(name.AsChar());
#ifdef TRANSGAMING
        TRACE("WorldMesh::SetupMaterials(): %p Mesh '%s' sky default shader created with %s\n", this, Name.AsChar(), name.AsChar());
#endif
	}
	else if (Flags & EBlendMap) 
	{
		// create default shader
		DefaultShader = matSys->FindMaterial(Name.LowerCase().AsChar());
		if (!DefaultShader)
		{
			if (Flags & EEntTranslucent)
				DefaultShader = matSys->CreateMaterial("terraintu2trans");
			else
				DefaultShader = matSys->CreateMaterial("terraintu2");
		}
#ifdef TRANSGAMING
        TRACE("WorldMesh::SetupMaterials(): %p Mesh '%s' blendmap created with terraintu2%s\n", this, Name.AsChar(), (Flags & EEntTranslucent) ? "trans" : "");
#endif
	} 
	else if (Flags & EWater)
	{
		DefaultShader = matSys->FindMaterial(Name.LowerCase().AsChar());
#ifdef TRANSGAMING
		if (DefaultShader)
			TRACE("WorldMesh::SetupMaterials(): %p Mesh water default shader created with %s\n", this, Name.AsChar());
#endif

		if (!DefaultShader) 
		{
			const char* s = "water";

#ifdef TRANSGAMING_NON_TNL
			if (GEngine->GetRenderDevice()->HWClass >= RenderDevice::EHWClassNV30)
			{
#ifdef	TRANSGAMING
				ReflectSurf = GEngine->pcfWorld->Water.ReflectFlags;
#else
				ReflectSurf = GEngine->World->Water.ReflectFlags;
#endif

				if (ReflectSurf & (Entity::EReflectScene|Entity::ERefractScene))
					s = "water2_refl_refr";
				else if (ReflectSurf & Entity::EReflectScene)
					s = "water2_refl";
				else if (ReflectSurf & Entity::ERefractScene)
					s = "water2_refr";

			}
#endif  //TRANSGAMING_NON_TNL

			DefaultShader	= matSys->CreateMaterial(s);

#ifndef TRANSGAMING
            NormalMapName	= "special/ripples_00";
#endif
			CubeMapName		= "special/cube_wenecja";

			/*
			DefaultShader	= matSys->CreateMaterial("water");

			NormalMapName	= "special/ripples_00";
			CubeMapName		= "special/cube_wenecja";

			ReflectSurf = Entity::EReflectScene;
			*/	
#ifdef TRANSGAMING
			TRACE("WorldMesh::SetupMaterials(): %p Mesh '%s' water shader created with %s\n", this, Name.AsChar(), s);
#endif
		}
	}
	else
	{
		// create default shader
		DefaultShader = matSys->FindMaterial(Name.LowerCase().AsChar());
#ifdef TRANSGAMING
		if (DefaultShader)
			TRACE("WorldMesh::SetupMaterials(): %p Mesh default shader created with '%s'\n", this, Name.AsChar());
#endif

		if (!DefaultShader) 
		{
			String matName = "";
			String matBlend = "blend";
			String matDefault = "default";


			if (Flags & EVertFormatNTU) {
				matName += "ntu";
				//-DDP - HACK!!!! - not supported currentlly
				//-DDP - HACK!!!! - if (Data->Groups.Size()>0 && Data->Groups[0].TexEmPtr)
				//-DDP - HACK!!!! - 	matName += "emissive";
			}
			else
			{
#ifdef	TRANSGAMING
				if (GEngine->pcfWorld->Overbright)
#else
				if (GEngine->World->Overbright)
#endif

					matName += "tu2x2";
				else
					matName += "tu2";
			}

			if (Flags & EEntTranslucent)
				matName += "trans";
			
			if (Flags & EATest)
				matName += "atest";

			if (Flags & E2Sided)
				matName += "2sided";

			// set default material
			DefaultShaderName = matDefault + matName;
			DefaultShader = matSys->CreateMaterial(DefaultShaderName.AsChar());
			
			// set detail
#ifndef TRANSGAMING_NON_TNL
            //make sure the detail shader isn't set up
            BlendDetailShader = DetailShader = NULL;
#else
			if (matSys->FindMaterial((matDefault+matName+"detail").AsChar())) {
                DetailShader = matSys->CreateMaterial((matDefault+matName+"detail").AsChar());
				DetailMap.Tile.U = 8.2f;
				DetailMap.Tile.V = 7.1f;
			}

			if (matSys->FindMaterial((matBlend+matName+"detail").AsChar())) {
				BlendDetailShader = matSys->CreateMaterial((matBlend+matName+"detail").AsChar());
				DetailMap.Tile.U = 8.2f;
				DetailMap.Tile.V = 7.1f;
			}
			else 
			{
				BlendDetailShader = DetailShader;
			}			
#endif

			if (matSys->FindMaterial((matBlend+matName).AsChar())) {
				BlendShader = matSys->CreateMaterial((matBlend+matName).AsChar());
			}
			else {
				BlendShader = DefaultShader;
			}

			if (!stricmp(matName.AsChar(), "ntutrans") && strstr(Name.AsChar(),"glass")) {
				//GLog.Print("mesh: %s - %s\n", Name.AsChar(), matName.AsChar());
				if (GEngine->Video->WarpEffectsEnabled) {
					DefaultShader = matSys->CreateMaterial("ntu_trans_warp");
					DetailShader = NULL;
					Sort = ESortWarp;
					IsNTUGlass = 1;
				}
			}

#ifdef TRANSGAMING
			TRACE("WorldMesh::SetupMaterials(): %p Mesh '%s' created with '%s'\n", this, Name.AsChar(), DefaultShaderName.AsChar());
#endif
		}
#ifdef ANNOYING_GRAPHICS_LOGS
		else
			GLog.Print("Mesh: using global shader '%s'\n",Name.AsChar());
#endif
	}

	VolShader = matSys->CreateMaterial("volobj");

	SetupShaders();
    TRACE("WorldMesh::SetupMaterials(): done setting up materials for '%s'\n", Name.AsChar());
}

void WorldMesh::SetupWaterShader()
{
	Assert(Flags & EWater);

	if (!GEngine->Video)
		return;

	if (WaterSurf)
		return;

	const char* s = "water";
	ReflectSurf = 0;

#ifdef TRANSGAMING_NON_TNL
#ifdef	TRANSGAMING
	if (GEngine->GetRenderDevice()->HWClass >= RenderDevice::EHWClassNV30 && GEngine->pcfWorld->Water.ReflectFlags)
	{
		ReflectSurf = GEngine->pcfWorld->Water.ReflectFlags;
#else
	if (GEngine->GetRenderDevice()->HWClass >= RenderDevice::EHWClassNV30 && GEngine->World->Water.ReflectFlags)
	{
		ReflectSurf = GEngine->World->Water.ReflectFlags;
#endif
		if ((ReflectSurf & (Entity::EReflectScene|Entity::ERefractScene)) == (Entity::EReflectScene|Entity::ERefractScene))
			s = "water2_refl_refr";
		else if (ReflectSurf & Entity::EReflectScene)
			s = "water2_refl";
		else if (ReflectSurf & Entity::ERefractScene)
			s = "water2_refr";
	}
#endif  //TRANSGAMING_NON_TNL

	DefaultShader = GEngine->Video->MaterialSys->FindMaterial(s);
}

void WorldMesh::SetupGlassShader()
{
	if (IsNTUGlass) {
		if (GEngine->Video->WarpEffectsEnabled) {
			DefaultShader = GEngine->Video->MaterialSys->CreateMaterial("ntu_trans_warp");
			Sort = ESortWarp;
		} else {
			DefaultShader = GEngine->Video->MaterialSys->CreateMaterial("defaultntutrans");
			Sort = ESortTrans;
		}
	}
}

void WorldMesh::SetupShaders()
{
	char ShadowShader[256] = "";
	char PointShader[256] = "";
	char SpotShader[256] = "";

	if (Flags & EVertFormatNTU) {
		const char* pref = "ntu_";
		strcat(ShadowShader,pref);
		strcat(PointShader,pref);
		strcat(SpotShader,pref);
	} else {
		const char* pref = "tu2_";
		strcat(ShadowShader,pref);
		strcat(PointShader,pref);
		strcat(SpotShader,pref);
	}

	strcat(ShadowShader,"shadowpass");

#ifdef	TRANSGAMING
	if (GEngine->pcfWorld->HighQualityLights) {
#else
	if (GEngine->World->HighQualityLights) {
#endif

#ifdef TRANSGAMING_NON_TNL
#ifdef UBER_SHADER
		if (GEngine->GetRenderDevice()->HWClass >= RenderDevice::EHWClassNV40)
            strcat(PointShader,"uber_fx_");
        else
            strcat(PointShader,"fx_");
#else
			strcat(PointShader,"fx_");
#endif // UBER_SHADER
#else   //TRANSGAMING_NON_TNL
        strcat(PointShader,"fx_");
#endif  //TRANSGAMING_NON_TNL
	}

	if (Flags & EBlendMap)
		strcat(PointShader,"blend_");		
	
	strcat(PointShader,"pointpass");
	strcat(SpotShader,"spotpass");

	ShadowPassShader	= GEngine->Video->MaterialSys->CreateMaterial(ShadowShader);
	PointLightShader	= GEngine->Video->MaterialSys->CreateMaterial(PointShader);
	SpotLightShader		= GEngine->Video->MaterialSys->CreateMaterial(SpotShader);
	PointTU2GlossShader = NULL;

#ifdef TRANSGAMING_NON_TNL
# ifdef	TRANSGAMING
	if (GEngine->pcfWorld->HighQualityLights && !(Flags & EVertFormatNTU)) {
# else
	if (GEngine->World->HighQualityLights && !(Flags & EVertFormatNTU)) {
# endif
		strcpy(PointShader,"tu2_fx_gloss_");
		if (Flags & EBlendMap)
			strcat(PointShader,"blend_");

		strcat(PointShader,"pointpass");
		PointTU2GlossShader = GEngine->Video->MaterialSys->CreateMaterial(PointShader);
	}
#endif

#ifdef TRANSGAMING_NON_TNL
	memset(ShadowShaders, 0, sizeof(ShadowShaders));

    if (GEngine->GetRenderDevice()->HWClass >= RenderDevice::EHWClassNV30) {
		
		char* p = &ShadowShader[strlen(ShadowShader)];
		p[1] = 0;

		for (int index=0; index<MAX_MESH_SHADOWS_PER_PASS; ++index) {
			p[0] = '1' + index;
			ShadowShaders[index] = GEngine->Video->MaterialSys->FindMaterial(ShadowShader);
			Assert(ShadowShaders[index]);
		}
	}
#endif  //TRANSGAMING_NON_TNL

	SetupGlassShader();

	if ((Flags & EWater) && WaterSurf) {
#ifdef	TRANSGAMING
		if (GEngine->pcfWorld->WaterQuality == 0) {
#else
		if (GEngine->World->WaterQuality == 0) {
#endif

			DefaultShader = GEngine->Video->MaterialSys->FindMaterial("water_ntu");
			ReflectSurf = 0;
		} else {

			if ((ReflectSurf & Entity::ERefractScene) && !(ReflectSurf & Entity::EReflectScene)) {
				DefaultShader = GEngine->Video->MaterialSys->FindMaterial("water_ntu_refl");
			}	
			else if ((ReflectSurf & Entity::ERefractScene) && (ReflectSurf & Entity::EReflectScene)) {
				DefaultShader = GEngine->Video->MaterialSys->FindMaterial("water_ntu_rr");
			} else {
				//GLog.Print("WARNING: water surf \"%s\" with no reflection NOR refraction?!\n", Name.AsChar());
				DefaultShader = GEngine->Video->MaterialSys->FindMaterial(DefaultShaderName.AsChar());
				if (DefaultShader->Flags & (EMF_Reflect|EMF_Refract))
				{
					ReflectSurf = 0;
					
					if (DefaultShader->Flags & EMF_Reflect)
						ReflectSurf |= Entity::EReflectScene;
					if (DefaultShader->Flags & EMF_Refract)
						ReflectSurf |= Entity::ERefractScene;

					DetailShader = NULL;
					Flags |= ENoLight;
				}
			}
		}
	}
}

void WorldMesh::RenderInitialize(RenderDevice* dev, MaterialSystem* matSys, const char* texSubPath)
{
    guard(WorldMesh::RenderInitialize);
    Assert(dev);
    Assert(matSys);

    // basic init
	LocalBox = Box;

    for (int i = 0; i < VertexList::MaxStreamCount; i++)
        Data->VertStream[i] = EInvalidIndex;
	Data->NormStream = EInvalidIndex;
    Data->ElemStream = EInvalidIndex;
    Data->VertStreamCount = 0;

#ifdef	TRANSGAMING
	if (GEngine->pcfWorld->LateVBs)
		GEngine->pcfWorld->LateVBsList.Add(this);
#else
	if (GEngine->World->LateVBs)
		GEngine->World->LateVBsList.Add(this);
#endif
	else
		InitializeVBs(dev);


    // assume the worst
    SetupFlags();

#ifndef TRANSGAMING
	DetailMap.Name = "special/detail";
#endif

	ReloadFlags = ~0;

	InitializeTextures(dev, matSys, texSubPath);

    SetupMaterials(texSubPath);

    unguard;
}

void WorldMesh::InitializeVBs(RenderDevice* dev) 
{
    Flags|=EInvalid;

    TRACE("WorldMesh::InitializeVBs(): initializing the object '%s'\n", Name.AsChar());


    // some sanity checks
    if( Data->VertList.Size() == 0 || ( Data->VertList.Size() > 65535 ) )
    {
        GLog.Print("ERROR: mesh '%s' has no verts or has more than 65536 of them\n",Name.AsChar());
        return;
    }


#ifdef TRANSGAMING_DEINDEX
    if (!Triangles.Size() && isIndexed)
#else
# ifdef TRANSGAMING
    if (!Data->Triangles.Size())
# else
    if (!Data->Elems.Size() && !Data->Triangles.Size())
# endif
#endif
    {
        GLog.Print("ERROR: mesh '%s' has no tris\n",Name.AsChar());
        return;
    }

#ifndef TRANSGAMING
    // fix W component for vertex shaders
    if (!(Flags & EVertFormatNTU))
    {
        for (int i=0; i<Data->Verts.Size(); ++i)
	    {
			const float x=1.0f;
			Verts[i].Color = *(DWORD*) &x;
			//Verts[i].Color = (DWORD) -1;
	    }
    }
#endif

#ifdef TRANSGAMING
    DWORD usage = 0;

    if (Flags & EVertFormatNTU)
    {
        void *  ptr;
        int     stride;


        //create a vertex stream
        Assert(Data->VertStreamCount == 0);
# ifdef TRANSGAMING_PS2
        if (Data->VertStreamCount == 0 || Data->VertStream[0] == EInvalidIndex){
            //position stream
            ptr = Data->VertList.getPositions(&stride);
            Assert(ptr && stride);
            TRACE("WorldMesh::InitializeVBs(): allocating a position buffer for %d verts for '%s' (size = %d, type = '%s')\n", Data->VertList.Size(), Name.AsChar(), stride, Data->VertList.getFormatName());
            Data->VertStream[Data->VertStreamCount++] = dev->CreateStream(ptr, usage, Data->VertList.Size(), stride);

            //normal stream
            ptr = Data->VertList.getNormals(&stride);
            Assert(ptr && stride);
            TRACE("WorldMesh::InitializeVBs(): allocating a normal buffer for %d verts for '%s' (size = %d, type = '%s')\n", Data->VertList.Size(), Name.AsChar(), stride, Data->VertList.getFormatName());
            Data->VertStream[Data->VertStreamCount++] = dev->CreateStream(ptr, usage, Data->VertList.Size(), stride);

            //tex coord stream
            ptr = Data->VertList.getTexCoord1(&stride);
            Assert(ptr && stride);
            TRACE("WorldMesh::InitializeVBs(): allocating a tex coord buffer for %d verts for '%s' (size = %d, type = '%s')\n", Data->VertList.Size(), Name.AsChar(), stride, Data->VertList.getFormatName());
            Data->VertStream[Data->VertStreamCount++] = dev->CreateStream(ptr, usage, Data->VertList.Size(), stride);
        }
# else
        if (Data->VertStreamCount == 0 || Data->VertStream[0] == EInvalidIndex){
            ptr = Data->VertList.getVerts(&stride);
            TRACE("WorldMesh::InitializeVBs(): allocating a vertex buffer for %d verts for '%s' (size = %d, type = '%s')\n", Data->VertList.Size(), Name.AsChar(), stride, Data->VertList.getFormatName());
            Data->VertStream[Data->VertStreamCount++] = dev->CreateStream(ptr, usage, Data->VertList.Size(), stride);
        }
# endif
    }
    else
    {
        Assert(Data->VertStreamCount == 0);
        void *  vtxPtr;
        int     vtxStride;


        //create a vertex stream 
# ifdef TRANSGAMING_PS2
        //!!// until the MT change is in, make sure we're still dealing with interleaved vertex data for multitextured surfaces
        Assert(Data->VertList.getFormat() == VertexList::VTXFMT_NONE);


        if (Data->VertStreamCount == 0 || Data->VertStream[0] == EInvalidIndex){
            vtxPtr = Data->VertList.getVerts(&vtxStride);
            TRACE("WorldMesh::InitializeVBs(): allocating a vertex buffer for %d verts for '%s' (size = %d, type = '%s')\n", Data->VertList.Size(), Name.AsChar(), vtxStride, Data->VertList.getFormatName());
            Data->VertStream[Data->VertStreamCount++] = dev->CreateStream(vtxPtr, usage, Data->VertList.Size(), vtxStride);
            //!!// allocate more streams from deinterleaved data for multitexturing
        }
# else
        if (Data->VertStreamCount == 0 || Data->VertStream[0] == EInvalidIndex){
            vtxPtr = Data->VertList.getVerts(&vtxStride);
            TRACE("WorldMesh::InitializeVBs(): allocating a vertex buffer for %d verts for '%s' (size = %d, type = '%s')\n", Data->VertList.Size(), Name.AsChar(), vtxStride, Data->VertList.getFormatName());
            Data->VertStream[Data->VertStreamCount++] = dev->CreateStream(vtxPtr, usage, Data->VertList.Size(), vtxStride);
        }
# endif
        //!!// normal streams are needed (expected) on multitextured surfaces until this is streamlined a little (MT change)
        if (Data->NormStream == EInvalidIndex){
            TRACE("WorldMesh::InitializeVBs(): allocating a normal buffer for %d verts for '%s' (size = %d, type = 'Vector')\n", Data->Normals.Size(), Name.AsChar(), sizeof(Vector));
            Data->NormStream = dev->CreateStream(&Data->Normals[0], usage, Data->Normals.Size(), sizeof(Vector));
        }//*/
    }
#else   //ifdef TRANSGAMING
    // vertex stream
	DWORD type, usage = 0;

	if (Flags & EVertFormatNTU) 
	{
        type  = RenderDevice::EMeshVertexNTU;
		if (dev->HWClass == RenderDevice::EHWClassGenericTnL)
			usage = (DWORD)RenderDevice::EUsageSWVP;
	}
	else 
	{
        type  = RenderDevice::EMeshVertex;
	}		
	
# if 0
	// use packed format?
	if (Flags & EPackedFormat)
	{
		if (Flags & EVertFormatNTU)
		{
			DynamicArray<MeshPackVertNTU> PackVerts;
			DynamicArray<Plane> Tan;
			memset(&Tan[0],0,sizeof(Plane)*Tan.Size());

			PackVerts.ResizeArray(Verts.Size());
			for (int i=0; i<Verts.Size(); ++i)			
				PackVerts[i] = MeshPackVertNTU(Verts[i], Tan[i]);			

			VertStream = dev->CreateStream(&PackVerts[0], type|usage, PackVerts.Size(), sizeof(MeshPackVertNTU));
		}
		else
		{
			DynamicArray<MeshPackVertTU2> PackVerts;

			PackVerts.ResizeArray(Verts.Size());
			for (int i=0; i<Verts.Size(); ++i)			
				PackVerts[i] = MeshPackVertTU2(Verts[i], Normals[i]);			
		}
	}
# endif

	if (VertStream == EInvalidIndex)
	    VertStream = dev->CreateStream(&Verts[0], type|usage, Verts.Size(), sizeof(MeshVertex));

    if (EInvalidIndex == VertStream)
        return;

    if (Flags & ENormTangent)
    {
        if (!Tangents.Size())
        {
            GLog.Print("ERROR: mesh '%s' has no tangents (but flags says it shoulda)\n",Name.AsChar());
            return;
        }

        // only used in programmable mode
        NormStream = dev->CreateStream
		(
		    &Tangents[0],
			RenderDevice::EStreamNone,
			Tangents.Size(),
			sizeof(TangentBasis)
		);

        if (EInvalidIndex == NormStream)
		    return;
    }
    else
    {
        if (!(Flags & EVertFormatNTU))
        {
	        if (!Normals.Size())
            {
                GLog.Print("ERROR: mesh '%s' has no normals\n",Name.AsChar());
                return;
            }

	        NormStream = dev->CreateStream
	        (
		        &Normals[0],
		        RenderDevice::EVertexNormal,
		        Normals.Size(),
		        sizeof(Vector)
	        );
			Assert(EInvalidIndex != NormStream);

			if (EInvalidIndex == NormStream) {
		        return;
			}
        }
    }
#endif

    // indices
#ifdef TRANSGAMING
    if (Data->Triangles.Size())
    {
        if (Data->ElemStream == EInvalidIndex){
            TRACE("WorldMesh::InitializeVBs(): allocating an index buffer for %d verts for '%s' (size = %d, type = 'WORD')\n", Data->Triangles.Size(), Name.AsChar(), sizeof(WORD));

            Data->ElemStream = dev->CreateStream
            (
                &Data->Triangles[0],
                RenderDevice::EIndex16|usage,
                Data->Triangles.Size(),
                sizeof(WORD)
            );
		    Assert(EInvalidIndex != Data->ElemStream);
        }

        if (EInvalidIndex == Data->ElemStream)
		    return;
    }

# ifdef TRANSGAMING_DEINDEX
    else if (!isIndexed)
        Data->ElemStream = EInvalidIndex;
# endif

#else
    if (Data->Elems.Size())
    {
        // index stream
        Data->ElemStream = dev->CreateStream
        (
            &Data->Elems[0],
            RenderDevice::EIndex16|usage,
            Data->Elems.Size(),
            sizeof(WORD)
        );
		Assert(EInvalidIndex != ElemStream);

        if (EInvalidIndex == ElemStream)
		    return;

        PrimType = RenderDevice::EPrimTriStrip;
    }

    else
    {
        ElemStream = dev->CreateStream
        (
            &Triangles[0],
            RenderDevice::EIndex16|usage,
            Triangles.Size(),
            sizeof(WORD)
        );
		Assert(EInvalidIndex != ElemStream);

        if (EInvalidIndex == ElemStream)
		    return;

        PrimType = RenderDevice::EPrimTriList;
    }
#endif

    // all good, clear the bad flag
    Flags&=~EInvalid;
}

void WorldMesh::ReloadTextures()
{
    if (!GEngine->Video) return;

    String subPath;
#ifdef	TRANSGAMING
    if (GEngine->pcfWorld->Name.Size())
        subPath = String("Levels/")+GEngine->pcfWorld->Name+"/";
#else
    if (GEngine->World->Name.Size())
        subPath = String("Levels/")+GEngine->World->Name+"/";
#endif

	RenderDevice* dev = GEngine->Video->RenderDev;
	MaterialSystem* matSys = GEngine->Video->MaterialSys;
	CleanupTextures(dev, matSys);
	InitializeTextures(dev, matSys, subPath.AsChar());
}

void WorldMesh::InitializeTextures(RenderDevice* dev, MaterialSystem* matSys, const char* texSubPath)
{
	String TexSubPath(texSubPath?texSubPath:"");
	int texLOD = (Flags & EBlendMap) ? RenderDevice::ETexLODSky : RenderDevice::ETexLODArchitecture;

#ifndef TRANSGAMING
	if (NormalMapName.Len() && (ReloadFlags & ETexNormalMap)) {
		NormalMapTex	= matSys->CreateTexture(NormalMapName.AsChar());
	}
#endif

	if (CubeMapName.Len() && (ReloadFlags & ETexCubeMap)) {
		CubeMapTex		= matSys->CreateCubeMap(CubeMapName.AsChar());
	}

#ifndef TRANSGAMING
	if (DetailMap.Name.Len() && (ReloadFlags & ETexDetailMap)) {
		DetailMap.TexPtr	= matSys->CreateTexture(DetailMap.Name.AsChar(),Texture::ETexNoLOD);
	}
#endif

	if (AlphaMap.Name.Len() && (ReloadFlags & ETexAlphaMap)) {
		AlphaMap.TexPtr = matSys->CreateTexture((TexSubPath+AlphaMap.Name).AsChar(),0,texLOD);
	}


    if (!(Flags & EVertFormatNTU) && (ReloadFlags & ETexLightMap)) {
		LightMapTex		= matSys->CreateTexture((TexSubPath+LightMapName).AsChar(),Texture::ETexNoMipMaps,texLOD);

		if (LightMapTex==matSys->Textures.NoTex) {
			LightMapTex = matSys->Textures.White;
		}

		if (LightMapTex->Flags & Texture::ETexAnim)	{
			matSys->SetTextureAnimAdvance((TexSubPath+LightMapName).AsChar(), 2);
		}
	}


	if (ReloadFlags & ETexBaseMap) 
	{	
		bool hasEmissive = false;

		for (int i=0; i<Data->Groups.Size(); ++i) 
		{
			Data->Groups[i].TexGlossPtr = NULL;

			if (Data->Groups[i].TexName == "notex") 
			{
				Data->Groups[i].TexPtr = matSys->Textures.NoTex;
			}
			else 
			{
				Data->Groups[i].TexPtr = matSys->CreateTexture((TexSubPath+Data->Groups[i].TexName).AsChar(),0,texLOD);
				if (matSys->IsTextureOnDisk((Data->Groups[i].TexName+"_add").AsChar())) 
				{
					Data->Groups[i].TexEmPtr = matSys->CreateTexture((Data->Groups[i].TexName+"_add").AsChar());
					hasEmissive = true;
				}
				// load specular maps
#ifdef TRANSGAMING_NON_TNL
#ifdef	TRANSGAMING
				if (GEngine->pcfWorld->HighQualityLights 
#else
				if (GEngine->World->HighQualityLights 
#endif
					&& GEngine->GetRenderDevice()->HWClass >= RenderDevice::EHWClassNV30
					&& matSys->IsTextureOnDisk((Data->Groups[i].TexName+"_s").AsChar()))
				//if (matSys->IsTextureOnDisk((Data->Groups[i].TexName+"_s").AsChar()))
				{
					Data->Groups[i].TexGlossPtr = matSys->CreateTexture((Data->Groups[i].TexName+"_s").AsChar());
				}
#endif  //TRANSGAMING_NON_TNL
			}
		
		}

		if (hasEmissive) 
		{
			for (int i=0; i<Data->Groups.Size(); ++i)
				if (!Data->Groups[i].TexEmPtr)
					Data->Groups[i].TexEmPtr = matSys->Textures.Black;
		}

		// set diffuse maps as specular maps if none specified
		for (int i=0; i<Data->Groups.Size(); ++i)
		{
			if (!Data->Groups[i].TexGlossPtr)
				Data->Groups[i].TexGlossPtr = Data->Groups[i].TexPtr;
		}
	}

	if (ReloadFlags & ETexBlendMap)

		for (int i=0; i<Data->Groups.Size(); ++i) {

			Group& grp = Data->Groups[i];

			if (grp.BlendMap.Name.Len()) {
				grp.BlendMap.TexPtr = matSys->CreateTexture((TexSubPath+grp.BlendMap.Name).AsChar(),0,texLOD);
			}
		}


	ReloadFlags = 0;
}

void WorldMesh::CleanupTextures(RenderDevice* dev, MaterialSystem* matSys)
{
	if (ReloadFlags & ETexBaseMap)
		for(int i=0; i<Data->Groups.Size(); ++i) 
		{
			matSys->ReleaseTexture(Data->Groups[i].TexPtr);
			matSys->ReleaseTexture(Data->Groups[i].TexEmPtr);
			if (Data->Groups[i].TexGlossPtr != Data->Groups[i].TexPtr)
				matSys->ReleaseTexture(Data->Groups[i].TexGlossPtr);
		}

	if (ReloadFlags & ETexBlendMap)
		for(int i=0; i<Data->Groups.Size(); ++i) {
			matSys->ReleaseTexture(Data->Groups[i].BlendMap.TexPtr);
		}

	if (ReloadFlags & ETexLightMap)
		matSys->ReleaseTexture(LightMapTex);

#ifndef TRANSGAMING
	if (ReloadFlags & ETexNormalMap)
		matSys->ReleaseTexture(NormalMapTex);
#endif

#ifndef TRANSGAMING
	if (ReloadFlags & ETexDetailMap)
		matSys->ReleaseTexture(DetailMap.TexPtr);
#endif

	if (ReloadFlags & ETexCubeMap)
		matSys->ReleaseTexture(CubeMapTex);

	if (ReloadFlags & ETexAlphaMap)
		matSys->ReleaseTexture(AlphaMap.TexPtr);
}

void WorldMesh::RenderCleanup(RenderDevice* dev, MaterialSystem* matSys)
{
    Assert(dev);
    Assert(matSys);

    if(Flags & EInvalid)
        return;

    // remove buffers
    int ids[] = { Data->ElemStream, Data->NormStream };
    dev->ReleaseResource(RenderDevice::EResBuffer,ids,ARRAY_COUNT(ids));
    dev->ReleaseResource(RenderDevice::EResBuffer, Data->VertStream, Data->VertStreamCount);

    for (int i = 0; i < VertexList::MaxStreamCount; i++)
	    Data->VertStream[i] = EInvalidIndex;

	Data->ElemStream = EInvalidIndex;
	Data->NormStream = EInvalidIndex;
    Data->VertStreamCount = 0;

	matSys->ReleaseMaterial(DefaultShader);
#ifdef TRANSGAMING_NON_TNL
    matSys->ReleaseMaterial(DetailShader);
#endif

	if (BlendShader!=DefaultShader)
		matSys->ReleaseMaterial(BlendShader);
#ifdef TRANSGAMING_NON_TNL
	if (BlendDetailShader!=DetailShader)
		matSys->ReleaseMaterial(BlendDetailShader);
#endif

	matSys->ReleaseMaterial(VolShader);

	DefaultShader = NULL;
	DetailShader = NULL;
	BlendShader = NULL;
	BlendDetailShader = NULL;
	VolShader = NULL;

    // remove textures
	ReloadFlags = ~0;
	CleanupTextures(dev, matSys);
}

/*
void WorldMesh::RenderWater()
{
    RenderDevice* dev = GEngine->Video->RenderDev;
	dev->SetIndexedStream(VertStream,ElemStream);


	Matrix WorldToObject = ObjectToWorld.Inverse();
	Vector eyeVec = GEngine->Video->GetActiveView()->LocalViewport.EyePosition;
	eyeVec = WorldToObject.TransformCoord(eyeVec);

	const float wavesScale = GEngine->World->Water.BumpHeight;

	//Matrix trans(1,0,0, 0,0,1, 0,-1,0);
	Matrix m;

	if (Flags & EWater)
		m = ScaleMatrix(Vector(wavesScale,1,wavesScale));
	else 
		m = ScaleMatrix(Vector(wavesScale,wavesScale,1));

	dev->SetShaderMemory(4, &Plane(m.X.Normal+1,eyeVec.X), 1);
	dev->SetShaderMemory(5, &Plane(m.Y.Normal+1,eyeVec.Y), 1);
	dev->SetShaderMemory(6, &Plane(m.Z.Normal+1,eyeVec.Z), 1);

	const float fresnelBias  = GEngine->World->Water.FresnelBias;
	const float fresnelExp  = GEngine->World->Water.FresnelExponent;
	const float a = GEngine->World->Water.WaveAmplitude;
	const float f = GEngine->World->Water.WaveFreq;
	const float s = GEngine->World->Water.WaveSpeed;
	const float refAmount = GEngine->World->Water.ReflectionAmount;
	const float waterAmount = GEngine->World->Water.WaterAmount;
	const Vector refrTint = GEngine->World->Water.ShallowWaterColor*waterAmount;
	const Vector reflTint = GEngine->World->Water.DeepWaterColor*waterAmount;
	const float time = Time*s;

	dev->SetShaderMemory(10, &Plane(eyeVec,0.05f), 1);			
	dev->SetShaderMemory(11, &Plane(1, 1, refAmount, fresnelExp), 1);
	dev->SetShaderMemory(12, &Plane(1-fresnelBias, fresnelBias, 1, 1), 1);


	struct TWave
	{
		float Freq;
		float Amp;
		float Phase;
		float DirX;
		float DirZ;
	} Wave[2] = {
		{ 1.0f, 1.0f, 0.5f, -1.0f, 0.0f },
		{ 2.0f, 0.5f, 1.3f, -0.7f, 0.7f }	
	};


	dev->SetShaderMemory(13, &Plane(Wave[0].DirX, 0, Wave[0].DirZ, 0), 1);
	dev->SetShaderMemory(14, &Plane(Wave[1].DirX, 0, Wave[1].DirZ, 0), 1);
	dev->SetShaderMemory(15, &Plane(time*Wave[0].Phase, time*Wave[1].Phase, 0, 0), 1);
	dev->SetShaderMemory(16, &Plane(f*Wave[0].Freq,  f*Wave[1].Freq,  0, 0), 1);
	dev->SetShaderMemory(17, &Plane(a*Wave[0].Amp, a*Wave[1].Amp,   0, 0), 1);

	dev->SetShaderMemory(18, &Plane(F_PI, F_TWOPI, 1.0f/F_TWOPI, 0.5f), 1);
	dev->SetShaderMemory(19, &Plane(1.0f, -1.0f / 6.0f, 1.0f / 120.0f, -1.0f / 5040.0f), 1);
	dev->SetShaderMemory(20, &Plane(1.0f, -1.0f / 2.0f, 1.0f / 24.0f, -1.0f / 720.0f), 1);

	dev->SetShaderMemory(21, &Plane(reflTint-refrTint,1), 1);
	dev->SetShaderMemory(22, &Plane(refrTint,0), 1);

	if (ReflectSurf)
	{
		View *view = GEngine->Video->GetActiveView();
		const Matrix T1 = view->LocalViewport.WorldToScreen
						* ScaleMatrix(Vector(0.5F,0.5F,0.5F))	
						* TranslationMatrix(Vector(0.5F, 0.5F, 0.5F));
		const Matrix T2 = view->LocalViewport.WorldToScreen
						* ScaleMatrix(Vector(0.5F,-0.5F,0.5F))	
						* TranslationMatrix(Vector(0.5F, 0.5F, 0.5F));

		dev->SetShaderMemory(40, &T1.GetTranspose()[0], 4);
		dev->SetShaderMemory(44, &T2.GetTranspose()[0], 4);
	}

	dev->SetPixelShaderMemory(0, &Plane(1,1,1,1), 1);

	Render(DefaultShader);
}
*/

void WorldMesh::RenderWater(const TWater& Water)
{
#ifdef TRANSGAMING
    Plane tmpPlane;
#endif

    RenderDevice* dev = GEngine->Video->RenderDev;

    dev->SetStreams(Data->VertStream, Data->VertStreamCount,  Data->ElemStream);

	Matrix WorldToObject = ObjectToWorld.Inverse();
	Vector eyeVec = GEngine->Video->GetActiveView()->LocalViewport.EyePosition;
	eyeVec = WorldToObject.TransformCoord(eyeVec);

	const float wavesScale = Water.BumpHeight;

	//Matrix trans(1,0,0, 0,0,1, 0,-1,0);
	Matrix m;

	if (Flags & EWater)
		m = ScaleMatrix(Vector(wavesScale,1,wavesScale));
	else 
		m = ScaleMatrix(Vector(wavesScale,wavesScale,1));


#ifdef TRANSGAMING
    tmpPlane.set(m.X.Normal + 1, eyeVec.X);
	dev->SetShaderMemory(4, &tmpPlane, 1);

    tmpPlane.set(m.Y.Normal + 1, eyeVec.Y);
	dev->SetShaderMemory(5, &tmpPlane, 1);

    tmpPlane.set(m.Z.Normal + 1, eyeVec.Z);
	dev->SetShaderMemory(6, &tmpPlane, 1);
#else
	dev->SetShaderMemory(4, &Plane(m.X.Normal+1,eyeVec.X), 1);
	dev->SetShaderMemory(5, &Plane(m.Y.Normal+1,eyeVec.Y), 1);
	dev->SetShaderMemory(6, &Plane(m.Z.Normal+1,eyeVec.Z), 1);
#endif

	const float fresnelBias  = Water.FresnelBias;
	const float fresnelExp  = Water.FresnelExponent;
	const float a = Water.WaveAmplitude;
	const float f = Water.WaveFreq;
	const float s = Water.WaveSpeed;
	const float refAmount = Water.ReflectionAmount;
	const float waterAmount = Water.WaterAmount;
	const Vector refrTint = Water.ShallowWaterColor*waterAmount;
	const Vector reflTint = Water.DeepWaterColor*waterAmount;
	const float time = Time*s;


#ifdef TRANSGAMING
    tmpPlane.set(eyeVec, 0.05f);
    dev->SetShaderMemory(10, &tmpPlane, 1);			

    tmpPlane.set(1, 1, refAmount, fresnelExp);
	dev->SetShaderMemory(11, &tmpPlane, 1);

    tmpPlane.set(1-fresnelBias, fresnelBias, 1, 1);
	dev->SetShaderMemory(12, &tmpPlane, 1);
#else
	dev->SetShaderMemory(10, &Plane(eyeVec,0.05f), 1);			
	dev->SetShaderMemory(11, &Plane(1, 1, refAmount, fresnelExp), 1);
	dev->SetShaderMemory(12, &Plane(1-fresnelBias, fresnelBias, 1, 1), 1);
#endif


	struct TWave
	{
		float Freq;
		float Amp;
		float Phase;
		float DirX;
		float DirZ;
	} Wave[2] = {
		{ 1.0f, 1.0f, 0.5f, -1.0f, 0.0f },
		{ 2.0f, 0.5f, 1.3f, -0.7f, 0.7f }	
	};


#ifdef TRANSGAMING
    tmpPlane.set(Wave[0].DirX, 0, Wave[0].DirZ, 0);
	dev->SetShaderMemory(13, &tmpPlane, 1);

    tmpPlane.set(Wave[1].DirX, 0, Wave[1].DirZ, 0);
	dev->SetShaderMemory(14, &tmpPlane, 1);

    tmpPlane.set(time*Wave[0].Phase, time*Wave[1].Phase, 0, 0);
	dev->SetShaderMemory(15, &tmpPlane, 1);

    tmpPlane.set(f*Wave[0].Freq,  f*Wave[1].Freq,  0, 0);
	dev->SetShaderMemory(16, &tmpPlane, 1);

    tmpPlane.set(a*Wave[0].Amp, a*Wave[1].Amp,   0, 0);
	dev->SetShaderMemory(17, &tmpPlane, 1);


    tmpPlane.set(F_PI, F_TWOPI, 1.0f/F_TWOPI, 0.5f);
	dev->SetShaderMemory(18, &tmpPlane, 1);

    tmpPlane.set(1.0f, -1.0f / 6.0f, 1.0f / 120.0f, -1.0f / 5040.0f);
	dev->SetShaderMemory(19, &tmpPlane, 1);

    tmpPlane.set(1.0f, -1.0f / 2.0f, 1.0f / 24.0f, -1.0f / 720.0f);
	dev->SetShaderMemory(20, &tmpPlane, 1);


    tmpPlane.set(reflTint-refrTint,1);
	dev->SetShaderMemory(21, &tmpPlane, 1);

    tmpPlane.set(refrTint,0);
	dev->SetShaderMemory(22, &tmpPlane, 1);
#else
	dev->SetShaderMemory(13, &Plane(Wave[0].DirX, 0, Wave[0].DirZ, 0), 1);
	dev->SetShaderMemory(14, &Plane(Wave[1].DirX, 0, Wave[1].DirZ, 0), 1);
	dev->SetShaderMemory(15, &Plane(time*Wave[0].Phase, time*Wave[1].Phase, 0, 0), 1);
	dev->SetShaderMemory(16, &Plane(f*Wave[0].Freq,  f*Wave[1].Freq,  0, 0), 1);
	dev->SetShaderMemory(17, &Plane(a*Wave[0].Amp, a*Wave[1].Amp,   0, 0), 1);

	dev->SetShaderMemory(18, &Plane(F_PI, F_TWOPI, 1.0f/F_TWOPI, 0.5f), 1);
	dev->SetShaderMemory(19, &Plane(1.0f, -1.0f / 6.0f, 1.0f / 120.0f, -1.0f / 5040.0f), 1);
	dev->SetShaderMemory(20, &Plane(1.0f, -1.0f / 2.0f, 1.0f / 24.0f, -1.0f / 720.0f), 1);

	dev->SetShaderMemory(21, &Plane(reflTint-refrTint,1), 1);
	dev->SetShaderMemory(22, &Plane(refrTint,0), 1);
#endif

	if (ReflectSurf)
	{
		View *view = GEngine->Video->GetActiveView();
		const Matrix T1 = view->LocalViewport.WorldToScreen
						* ScaleMatrix(Vector(0.5F,0.5F,0.5F))	
						* TranslationMatrix(Vector(0.5F, 0.5F, 0.5F));
		const Matrix T2 = view->LocalViewport.WorldToScreen
						* ScaleMatrix(Vector(0.5F,-0.5F,0.5F))	
						* TranslationMatrix(Vector(0.5F, 0.5F, 0.5F));

		dev->SetShaderMemory(40, &T1.GetTranspose()[0], 4);
		dev->SetShaderMemory(44, &T2.GetTranspose()[0], 4);

		const View* V = GEngine->Video->GetActiveView();
		const Plane A(V->RTAspect[0]*0.5F, V->RTAspect[1]*0.5F, 0, 0);
		dev->SetShaderMemory(45, &A, 1);

#ifdef TRANSGAMING
        tmpPlane.set(Water.ReflectScale, Water.RefractScale, 0, 0);
		dev->SetPixelShaderMemory(1, &tmpPlane, 1);
#else
		dev->SetPixelShaderMemory(1, &Plane(Water.ReflectScale, Water.RefractScale, 0, 0), 1);
#endif
	}

	for (int i=0; i<2; ++i) {
		Matrix T = TexturePanMatrix(Water.Pan[i].X*Time, Water.Pan[i].Y*Time) 
					* ScaleMatrix(Vector(Water.Tile[i].X, Water.Tile[i].Y, 1));

		T.Transpose();

		dev->SetShaderMemory(24+i*3,&T.X,3);
	}

	if (WaterSurf) {

		/*
		{
			TextureState st[1];
			st[0].MatStore = GEngine->Video->GetActiveView()->LocalViewport.WorldToCamera.Inverse();
			st[0].Stage = 0;

			dev->SetTextureState(st,ARRAY_COUNT(st));
		}
		*/

#ifdef TRANSGAMING_NON_TNL
		if (GEngine->GetRenderDevice()->HWClass == RenderDevice::EHWClassNV20
			|| GEngine->GetRenderDevice()->HWClass == RenderDevice::EHWClassNV25)
		{
			TextureState st[1];
			//const float w = 0.0F;
			//const float w = 0.1F;
			const float w = Water.ReflectScale;
#ifdef TRANSGAMING
            ScaleMatrix tmpMat(Vector(w,w,0));


			st[0].BumpPtr = &tmpMat;
#else
			st[0].BumpPtr = &ScaleMatrix(Vector(w,w,0));
#endif
			st[0].Stage = 1;
			dev->SetTextureState(st,ARRAY_COUNT(st));

#ifdef TRANSGAMING
            tmpPlane.set(1,0.2F,0.2F,0.6F);
			dev->SetPixelShaderMemory(4, &tmpPlane, 1);
#else
			dev->SetPixelShaderMemory(4, &Plane(1,0.2F,0.2F,0.6F), 1);
#endif
		}
#endif //TRANSGAMING_NON_TNL
	}


#ifdef TRANSGAMING
    tmpPlane.set(1,1,1,1);
	dev->SetPixelShaderMemory(0, &tmpPlane, 1);
#else
	dev->SetPixelShaderMemory(0, &Plane(1,1,1,1), 1);
#endif

	if (dev->HWClass == RenderDevice::EHWClassGenericTnL) {
#ifdef	TRANSGAMING
		dev->SetFogState(GEngine->pcfWorld->Env.FogParams); 
#else
		dev->SetFogState(GEngine->World->Env.FogParams); 
#endif


		//RenderState state;
		//state.SetConstColor((DWORD) ::Color(&Water.DeepWaterColor.X));
		//state.SetConstColor(0xFF);
		//dev->SetState(state);
	}

#ifdef TRANSGAMING_MESH_WARNINGS
    //TRACE("WorldMesh::RenderWater(): drawing water for from object '%s' {verts = %d, tris = %d}\n", Name.AsChar(), Data->VertList.Size(), Triangles.Size());
#endif
	RenderDefaultPass(DefaultShader);

	if (dev->HWClass == RenderDevice::EHWClassGenericTnL)
#ifdef	TRANSGAMING
		dev->SetFogState(GEngine->pcfWorld->Env.FogParams.UseVSFog()); 
#else
		dev->SetFogState(GEngine->World->Env.FogParams.UseVSFog()); 
#endif
}

void WorldMesh::RenderNTU(Material* shader)
{
    RenderDevice* dev = GEngine->Video->RenderDev;

    dev->SetStreams(Data->VertStream, Data->VertStreamCount, Data->ElemStream);
	dev->SetShaderMemory(4,&ObjectToWorld.GetTranspose()[0],3);

	/*
	Material* M = GEngine->Video->MaterialSys->FindMaterial("fxambientntu");

	Plane ambient = Plane(GetEnvironmentAmbient());
	dev->SetShaderMemory(11,&ambient,1);
	Render(M);
	*/

#ifdef TRANSGAMING
    dev->SetLighting(true);
#endif

	if (Flags & ENoLight)
	{
#ifdef TRANSGAMING
        Plane tmpPlane(0, 0, 0, 1);

        dev->SetShaderMemory(10,&tmpPlane,1);

        tmpPlane.set(1,1,1,1);
		dev->SetShaderMemory(11,&tmpPlane,1);
        dev->SetSpecular(Vector(0,0,0), 0.0f);
        dev->SetAmbientLight(Vector(1,1,1));

		for (int iLight=0; iLight<Model::MaxLights; iLight++){
            tmpPlane.set(0,0,0,1);
			dev->SetShaderMemory(12+3*iLight,&tmpPlane,1);
            tmpPlane.set(0,0,0,1);
			dev->SetShaderMemory(13+3*iLight,&tmpPlane,1);
            tmpPlane.set(0,0,0,1);
			dev->SetShaderMemory(14+3*iLight,&tmpPlane,1);
            dev->SetNoLight(iLight);
		}
#else
		Plane ambient = Plane(1,1,1,1);

		dev->SetShaderMemory(10,&Plane(0,0,0,1),1);
		dev->SetShaderMemory(11,&Plane(1,1,1,1),1);


		for (int iLight=0; iLight<Model::MaxLights; iLight++)
		{
			dev->SetShaderMemory(12+3*iLight,&Plane(0,0,0,1),1);
			dev->SetShaderMemory(13+3*iLight,&Plane(0,0,0,1),1);
			dev->SetShaderMemory(14+3*iLight,&Plane(0,0,0,1),1);
		}
#endif

	}
#ifdef	TRANSGAMING
	else if (GEngine->pcfWorld->HighQualityLights)
	{
		Plane ambient = Plane(BasePlane(GetEnvironmentAmbient()));
#else
	else if (GEngine->World->HighQualityLights)
	{
		Plane ambient = Plane(GetEnvironmentAmbient());
#endif
		ComputeVSLights();

#ifdef	TRANSGAMING
		dev->SetShaderMemory(10,&GEngine->pcfWorld->DynamicSpecular,1);
        dev->SetSpecular(GEngine->pcfWorld->DynamicSpecular.Normal, GEngine->pcfWorld->DynamicSpecular.Dist);
#else
		dev->SetShaderMemory(10,&GEngine->World->DynamicSpecular,1);
#endif
		//dev->SetShaderMemory(10,&Plane(0,0,0,0),1);
		dev->SetShaderMemory(11,&ambient,1);
#ifdef	TRANSGAMING
        dev->SetAmbientLight(ambient.Normal);
#endif

		for (int iLight=0; iLight<Model::MaxLights; iLight++)
		{
			// only do directionals
			if (iLight<LightCount && (Lights[iLight]->GetType() != Light::ELightDir))
			{
				VSLights[iLight].Color = Plane(0,0,0,0);
			}

			float unScale = 1/GetWorldScale();
			VSLights[iLight].Dir.Normal *= unScale;
			VSLights[iLight].HalfDir.Normal *= unScale;
			
			dev->SetShaderMemory(12+3*iLight,&VSLights[iLight].Color,1);
			dev->SetShaderMemory(13+3*iLight,&VSLights[iLight].Dir,1);
			dev->SetShaderMemory(14+3*iLight,&VSLights[iLight].HalfDir,1);
#ifdef TRANSGAMING
            dev->SetDirLight(iLight, VSLights[iLight].Dir, VSLights[iLight].Color.Normal);
#endif
		}
	}
	else
	{
#ifdef	TRANSGAMING
		Plane ambient = Plane( BasePlane(GetEnvironmentAmbient()) );
#else
		Plane ambient = Plane(GetEnvironmentAmbient());
#endif
		ComputeVSLights();

#ifdef	TRANSGAMING
		dev->SetShaderMemory(10,&GEngine->pcfWorld->DynamicSpecular,1);
        dev->SetSpecular(GEngine->pcfWorld->DynamicSpecular.Normal, GEngine->pcfWorld->DynamicSpecular.Dist);
#else
		dev->SetShaderMemory(10,&GEngine->World->DynamicSpecular,1);
#endif
		dev->SetShaderMemory(11,&ambient,1);
#ifdef	TRANSGAMING
        dev->SetAmbientLight(ambient.Normal);
#endif

		for (int iLight=0; iLight<Model::MaxLights; iLight++)
		{
			if (iLight<LightCount && (Lights[iLight]->Flags & Light::ELightDynamic))
			{
				VSLights[iLight].Color = Plane(0,0,0,0);
			}

			float unScale = 1/GetWorldScale();
			VSLights[iLight].Dir.Normal *= unScale;
			VSLights[iLight].HalfDir.Normal *= unScale;
			
			dev->SetShaderMemory(12+3*iLight,&VSLights[iLight].Color,1);
			dev->SetShaderMemory(13+3*iLight,&VSLights[iLight].Dir,1);
			dev->SetShaderMemory(14+3*iLight,&VSLights[iLight].HalfDir,1);
#ifdef TRANSGAMING
            dev->SetDirLight(iLight, VSLights[iLight].Dir, VSLights[iLight].Color.Normal);
#endif
		}
	}

	if (dev->HWClass == RenderDevice::EHWClassGenericTnL)
#ifdef	TRANSGAMING
		dev->SetFogState(GEngine->pcfWorld->Env.FogParams); 

#ifdef TRANSGAMING_MESH_WARNINGS
    TRACE("WorldMesh::RenderNTU(): drawing mesh for object '%s' {verts = %d, tris = %d}\n", Name.AsChar(), Data->VertList.Size(), Triangles.Size());
#endif
	RenderDefaultPass(shader);

	if (dev->HWClass == RenderDevice::EHWClassGenericTnL)
		dev->SetFogState(GEngine->pcfWorld->Env.FogParams.UseVSFog()); 
#else
		dev->SetFogState(GEngine->World->Env.FogParams); 

	Render(shader);

	if (dev->HWClass == RenderDevice::EHWClassGenericTnL)
		dev->SetFogState(GEngine->World->Env.FogParams.UseVSFog()); 
#endif

#ifdef TRANSGAMING
    dev->SetLighting(false);
#endif
}

#if 0

static void GetScissor(const Viewport& vp, const BoundingBox& Box, Vector& Min, Vector& Max)
{
	BoundingBox ScreenBox;
	for (int i=0; i<8; ++i)
	{
		Vector Corner = vp.WorldToScreen.Transform(Box.GetCorner(i));
		//Vector Corner = Mult(ClipMat,Box.GetCorner(i));
		ScreenBox += Corner;
	}

	Min.X = (ScreenBox.Min.X + 1.0F) * (vp.ViewportWidth/2.0F);
	Min.Y = -(ScreenBox.Min.Y - 1.0F) * (vp.ViewportHeight/2.0F);
	Min.Z = 0;

	Max.X = (ScreenBox.Max.X + 1.0F) * (vp.ViewportWidth/2.0F);
	Max.Y = -(ScreenBox.Max.Y - 1.0F) * (vp.ViewportHeight/2.0F);
	Max.Z = 0;
}

#endif

#ifdef TRANSGAMING_NON_TNL
void WorldMesh::RenderTU2Specular()
{
	RenderDevice* dev = GEngine->Video->RenderDev;

	Material* shader = GEngine->Video->MaterialSys->FindMaterial((Flags & EBlendMap) ? "tu2_fx_gloss_blend" : "tu2_fx_gloss");
	Assert(shader);

	Light* L1 = SpecularLights[0];
	Light* L2 = SpecularLights[1];
#ifdef TRANSGAMING
    Plane tmpPlane;
#endif

    tmpPlane.set(L1 ? L1->GetWorldPosition() : Vector(0,0,0), 1.0F);
	dev->SetShaderMemory(20, &tmpPlane, 1);
    tmpPlane.set(GEngine->Video->GetActiveView()->LocalViewport.EyePosition,1);
	dev->SetShaderMemory(21, &tmpPlane, 1);
    tmpPlane.set(L2 ? L2->GetWorldPosition() : Vector(0,0,0), 1.0F);
	dev->SetShaderMemory(22, &tmpPlane, 1);

    tmpPlane.set(Vector(0,0,0),L1 ? 1.0f/L1->GetRadius() : 0.0F);
	dev->SetPixelShaderMemory(0, &tmpPlane, 1);
#ifdef	TRANSGAMING
	Vector tmpV = BaseVector(L1->Color);
	tmpV = tmpV*L1->Intensity;
    tmpPlane.set(L1 ? tmpV : Vector(0,0,0), SpecularPower);
	dev->SetPixelShaderMemory(1, &tmpPlane, 1);

    tmpPlane.set(Vector(0,0,0),L2 ? 1.0f/L2->GetRadius() : 0.0F);
	dev->SetPixelShaderMemory(2, &tmpPlane, 1);
	
	
	tmpV = BaseVector(L2->Color);
	tmpV = tmpV*L2->Intensity;
    tmpPlane.set(L2 ? tmpV : Vector(0,0,0), SpecularPower);
	dev->SetPixelShaderMemory(3, &tmpPlane, 1);	
#else
	dev->SetPixelShaderMemory(1, &Plane(L1 ? Vector(L1->Color)*L1->Intensity : Vector(0,0,0), SpecularPower), 1);

	dev->SetPixelShaderMemory(2, &Plane(Vector(0,0,0),L2 ? 1.0f/L2->GetRadius() : 0.0F), 1);
	dev->SetPixelShaderMemory(3, &Plane(L2 ? Vector(L2->Color)*L2->Intensity : Vector(0,0,0), SpecularPower), 1);	
#endif

    VertStream[VertStreamCount] = NormStream;
    dev->SetStreams(VertStream, VertStreamCount + 1, ElemStream); // need normals for lighting


#ifdef	TRANSGAMING
    tmpPlane.set(GEngine->pcfWorld->Overbright ? 2.0F : 1.0F, 0,0,0);
	dev->SetPixelShaderMemory(10, &tmpPlane, 1);
#else
	dev->SetPixelShaderMemory(10, &Plane(GEngine->World->Overbright ? 2.0F : 1.0F, 0,0,0), 1);
#endif

	//Render(Shader);

	for (int index=0; index<shader->PassCount; ++index) {
		RenderPass& pass = shader->Passes[index];	
		pass.Activate();

		RenderState state;
		int override = 0;
		if (Flags & EEntTranslucent) {
			state.SetBlendMode(RenderState::EBlendTranslucent);
			++override;
		}
		
		if (Flags & EATest) {
			state.SetAlphaTest(1);
			++override;
		}

		if (Flags & E2Sided) {
			state.SetCullMode(RenderState::ECullNone);
			++override;
		}

		if (override)
			dev->SetState(state);

		for (int i=0; i<Data->Groups.Size(); ++i) {
			SimpleMesh::Group& grp = Data->Groups[i];

			Texture *textures[] = 
			{	
				grp.TexPtr,LightMapTex,NormalMapTex,DetailMap.TexPtr,CubeMapTex,
				grp.BlendMap.TexPtr,AlphaMap.TexPtr,grp.TexEmPtr,grp.TexGlossPtr
			};
			MaterialDef *materialDefs[] = { &grp.ColorMap, &grp.BlendMap, &DetailMap };
			const int flags 
				= MaterialInput::EInputColor
				| MaterialInput::EInputLight
				| MaterialInput::EInputNormal
				| MaterialInput::EInputDetail
				| MaterialInput::EInputCube
				| MaterialInput::EInputBlend
				| MaterialInput::EInputAlpha
				| MaterialInput::EInputEmissive
				| MaterialInput::EInputGloss
				| MaterialInput::EDefAlpha
				| MaterialInput::EDefBlend
				| MaterialInput::EDefDetail;


            MaterialInput tmpMaterial(flags,textures,materialDefs);
			pass.SetInput(&tmpMaterial);

#ifdef TRANSGAMING_DEINDEX
            if (!isIndexed)
			    dev->Draw(PrimType,grp.RenderBase,grp.RenderCount);

            else
#endif
			    dev->Draw(PrimType,grp.RenderBase,grp.RenderCount,grp.VertexMin,grp.VertexCount);
		}
	}
}
#endif

void WorldMesh::RenderLightPass(const Light* light)
{
    RenderDevice* dev = GEngine->Video->RenderDev;
	Matrix WorldToObject = ObjectToWorld.Inverse();
	Vector OSLightPos = WorldToObject.TransformCoord(light->GetWorldPosition());
#ifdef TRANSGAMING
    Plane tmpPlane;
#endif

	if (dev->HWClass == RenderDevice::EHWClassGenericTnL)
	{
		if (Flags & EVertFormatNTU)			
			return;

#ifdef	TRANSGAMING
		dev->SetFogState(GEngine->pcfWorld->Env.FogParams); 
#else
		dev->SetFogState(GEngine->World->Env.FogParams); 
#endif

		Matrix m 
			= TranslationMatrix(Vector(0.5f,0.5f,0.5f)).GetTranspose()
			* ScaleMatrix(0.5f)
			* ScaleMatrix(1/light->GetRadius()) 
			* TranslationMatrix(-light->GetWorldPosition()).GetTranspose();

		PointLightShader->Passes[0].TexState[0].MatStore = m;

		m.X = m.Z;
		m.Y = Plane(0,0,0,0.5f);
		PointLightShader->Passes[0].TexState[1].MatStore = m;

#ifdef	TRANSGAMING
//		Color_TG c(&(Plane(Vector(BaseVector(light->Color))*min(light->Intensity*0.5f,1.0f)).X));
		
		Vector tmpV = Vector(BaseVector(light->Color));
		tmpV = tmpV*min(light->Intensity*0.5f,1.0f);
        tmpPlane.set(tmpV);
		Color_TG c(&tmpPlane.X);
		
		PointLightShader->Passes[1].State.SetConstColor(c);
#else
		Color c(&(Plane(Vector(light->Color)*min(light->Intensity*0.5f,1.0f)).X));
		PointLightShader->Passes[1].State.SetConstColor(c);
#endif

		PointLightShader->Passes[0].TexState[0].Flags = TextureState::EXformUseMat;
		PointLightShader->Passes[0].TexState[1].Flags = TextureState::EXformUseMat;
	}

#ifdef	TRANSGAMING
	if (GEngine->pcfWorld->HighQualityLights)
	{
        //Matrix tobj2world = 
		dev->SetShaderMemory(4,&ObjectToWorld.GetTranspose()[0],4);
		dev->SetShaderMemory(16,&ObjectToWorld[0],4);
        tmpPlane.set(light->GetWorldPosition(),1.0F);
		dev->SetShaderMemory(20,&tmpPlane, 1);
        tmpPlane.set(GEngine->Video->GetActiveView()->LocalViewport.EyePosition,1);
		dev->SetShaderMemory(21, &tmpPlane, 1);

        tmpPlane.set(Vector(0,0,0),1.0f/light->GetRadius());
		dev->SetPixelShaderMemory(0, &tmpPlane, 1);
        tmpPlane.set(Vector(BaseVector(light->Color))*light->Intensity, SpecularPower);
		dev->SetPixelShaderMemory(1, &tmpPlane, 1);
	}
#else
	if (GEngine->World->HighQualityLights)
	{
		dev->SetShaderMemory(4,&ObjectToWorld.GetTranspose()[0],4);
		dev->SetShaderMemory(16,&ObjectToWorld[0],4);
		dev->SetShaderMemory(20,&Plane(light->GetWorldPosition(),1.0F), 1);
		dev->SetShaderMemory(21, &Plane(GEngine->Video->GetActiveView()->LocalViewport.EyePosition,1), 1);

		dev->SetPixelShaderMemory(0, &Plane(Vector(0,0,0),1.0f/light->GetRadius()), 1);
		dev->SetPixelShaderMemory(1, &Plane(Vector(light->Color)*light->Intensity, SpecularPower), 1);
	}
#endif

#ifdef	TRANSGAMING
    tmpPlane.set(OSLightPos,1);
	dev->SetShaderMemory(13,&tmpPlane,1);
    tmpPlane.set(Vector(BaseVector(light->Color))*light->Intensity*0.5f,1.0f/light->GetRadius());
	dev->SetShaderMemory(12,&tmpPlane,1);
//	dev->SetDirLight(0, OSLightPos, Vector(BaseVector(light->Color))*light->Intensity*0.5f);
//	for (int iLight=1; iLight<Model::MaxLights; iLight++)
//	   dev->SetNoLight(iLight);
#else
	dev->SetShaderMemory(13,&Plane(OSLightPos,1),1);
	dev->SetShaderMemory(12,&Plane(Vector(light->Color)*light->Intensity*0.5f,1.0f/light->GetRadius()),1);
#endif

	// -------------------------------------
	Material* mat;
	switch (light->GetType())
    {
    case Light::ELightPoint:
#ifdef COLLAPSE_TU2SPEC
# ifdef TRANSGAMING_MESH_WARNINGS
        //TRACE("WorldMesh::RenderLightPass(): drawing point light pass for object '%s' {verts = %d, tris = %d}\n", Name.AsChar(), Data->VertList.Size(), Triangles.Size());
# endif
		RenderDefaultPass(PointLightShader);
#else
		
		if (GEngine->World->HighQualityLights)
		{
			if (Flags & EVertFormatNTU)			
			{
# ifdef TRANSGAMING_MESH_WARNINGS
                //TRACE("WorldMesh::RenderLightPass(): drawing point light pass for object '%s' {verts = %d, tris = %d}\n", Name.AsChar(), Data->VertList.Size(), Triangles.Size());
# endif
				RenderDefaultPass(PointLightShader);
			}
			else
			{
				//RenderDefaultPass((light->Flags & Light::ELightDynamic) ? PointLightShader : PointLightGlossShader);
# ifdef TRANSGAMING_MESH_WARNINGS
                //TRACE("WorldMesh::RenderLightPass(): drawing dynamic point light pass for object '%s' {verts = %d, tris = %d}\n", Name.AsChar(), Data->VertList.Size(), Triangles.Size());
# endif
				if (light->Flags & Light::ELightDynamic)
					RenderDefaultPass(PointLightShader);
				else if (light == SpecularLights[0] || light == SpecularLights[1])
					RenderDefaultPass(PointTU2GlossShader);
			}
		}
		else
		{
# ifdef TRANSGAMING_MESH_WARNINGS
            //TRACE("WorldMesh::RenderLightPass(): drawing point light shader pass for object '%s' {verts = %d, tris = %d}\n", Name.AsChar(), Data->VertList.Size(), Triangles.Size());
# endif
			RenderDefaultPass(PointLightShader);
		}
#endif // COLLAPSE_TU2SPEC		

        break;

	case Light::ELightSpot:
		mat = SpotLightShader;
		Matrix m = (ObjectToWorld*light->GetProjMatrix()).GetTranspose();
		mat->Passes[0].TexState[0].MatStore = m;
		mat->Passes[0].TexState[0].Xform = TextureState::EXformUseShader;
		if (light->Projector)
			mat->Passes[0].TexMaps[0].TexPtr = light->Projector;
		else
			mat->Passes[0].TexMaps[0].TexPtr = GEngine->Video->MaterialSys->Textures.NoTex;

		Vector dir = WorldToObject.TransformNormal(light->GetDirection());		
		Plane z = Plane(dir*Scale*Scale/light->GetRadius(),OSLightPos);
		z.Dist = -z.Dist;
		dev->SetShaderMemory(14,&z,1);
       	
#ifdef TRANSGAMING_MESH_WARNINGS
        //TRACE("WorldMesh::RenderLightPass(): drawing spotlight pass for object '%s' {verts = %d, tris = %d}\n", Name.AsChar(), Data->VertList.Size(), Triangles.Size());
#endif
        RenderDefaultPass(mat);
		break;
    }

	if (dev->HWClass == RenderDevice::EHWClassGenericTnL)
	{
#ifdef	TRANSGAMING
		dev->SetFogState(GEngine->pcfWorld->Env.FogParams.UseVSFog()); 
#else
		dev->SetFogState(GEngine->World->Env.FogParams.UseVSFog()); 
#endif

	}
}

void WorldMesh::RenderShadowPass(Model* model)
{
#ifdef	TRANSGAMING
    Plane tmpPlane(0, 0, 0, 0);

	if (model->ShadowUpdateFrame != GEngine->pcfWorld->Frame)
#else
	if (model->ShadowUpdateFrame != GEngine->World->Frame)
#endif
		return;

	Material* mat = ShadowPassShader;

	//Matrix m = model->ShadowDecal->GetSpawnMatrix().Inverse();
	Matrix m = model->ShadowMatrix.Inverse();
	m = m*TranslationMatrix(Vector(0.5f,0.5f,1));
	m.Transpose();

	mat->Passes[0].TexState[0].MatStore = m;
	mat->Passes[0].TexState[0].Xform = TextureState::EXformUseShader;

    RenderDevice* dev = GEngine->Video->RenderDev;
	Texture* notex = GEngine->Video->MaterialSys->Textures.NoTex;
	dev->SetShaderMemory(12,&tmpPlane,1);

	//Vector n = model->ShadowDecal->GetSpawnMatrix().Z.Normal.GetNormal();
	Vector n = model->ShadowMatrix.Z.Normal.GetNormal();
	Vector boxoffset = model->Box.GetCorner(Vector(-n.X,n.Y,-n.Z).Sign());
	boxoffset *= boxoffset.Dot(n);


	Plane z = Plane(n/(8*model->Box.GetExtent().Y), model->Box.GetCenter() + model->Box.GetExtent());
	z.Dist = -z.Dist;
	dev->SetShaderMemory(14,&z,1);


	//dev->SetPixelShaderMemory(0, &m, 2);
	//dev->SetPixelShaderMemory(16, &z, 1);

	for (int index=0; index<mat->PassCount; ++index)
	{
		RenderPass& pass = mat->Passes[index];	
		pass.Activate();

		for (int i=0; i<Data->Groups.Size(); ++i) 
		{
			const SimpleMesh::Group& grp=Data->Groups[i];
            MaterialInput tmpMaterial(model->ShadowMap,notex,notex);


			pass.SetInput(&tmpMaterial);
#ifdef TRANSGAMING_MESH_WARNINGS
            //TRACE("WorldMesh::RenderShadowPass(): rendering shadow pass %d for the object '%s'\n", index, Name.AsChar());
#endif
			dev->Draw(PrimType,grp.RenderBase,grp.RenderCount);
		}
	}
}

#ifdef TRANSGAMING_NON_TNL
void WorldMesh::RenderShadows(Model** Casters, int NumCasters)
{
	Plane transforms[MAX_MESH_SHADOWS_PER_PASS*2];
	Plane zprojs[MAX_MESH_SHADOWS_PER_PASS];

	for (int i=0; i<NumCasters; ++i) {
		const Model* model = Casters[i];

		Matrix m = model->ShadowMatrix.Inverse();
		m = m*TranslationMatrix(Vector(0.5f,0.5f,1));
		m.Transpose();

		transforms[i*2+0] = m.X;
		transforms[i*2+1] = m.Y;

		Vector n = model->ShadowMatrix.Z.Normal.GetNormal();
		Plane z = Plane(n/(8*model->Box.GetExtent().Y), model->Box.GetCenter() + model->Box.GetExtent());
		z.Dist = -z.Dist;

		zprojs[i] = z;
	}

    RenderDevice* dev = GEngine->Video->RenderDev;

	//dev->SetShaderMemory(40, &transforms[0], MAX_MESH_SHADOWS_PER_PASS*2);
	//dev->SetShaderMemory(14, &zprojs[0], MAX_MESH_SHADOWS_PER_PASS);

	dev->SetShaderMemory(40, &transforms[0], NumCasters*2);
	dev->SetShaderMemory(60, &zprojs[0], NumCasters);

	//dev->SetPixelShaderMemory(0, &transforms[0], MAX_MESH_SHADOWS_PER_PASS*2);
	//dev->SetPixelShaderMemory(16, &zprojs[0], MAX_MESH_SHADOWS_PER_PASS);

	Material* M = ShadowShaders[NumCasters-1];
	Assert(M);

	for (int index=0; index<M->PassCount; ++index)
	{
		RenderPass& Pass = M->Passes[index];	
#ifdef TRANSGAMING
        MaterialInput tmpMaterial(NULL);

        Pass.Activate();

        // NULL - we don't set colormap, but shader script references it
		Pass.SetInput(&tmpMaterial);
#else
        Pass.Activate();

        // NULL - we don't set colormap, but shader script references it
		Pass.SetInput(&MaterialInput(NULL));
#endif
		

		for (int k=0; k<NumCasters; ++k)
			dev->SetTexture(k+1, Casters[k]->ShadowMap->DeviceId);

#ifdef TRANSGAMING_DEINDEX
		// Until we decide whether we want to enable the Elems path...
        if (!isIndexed){
		    const int NumPrims = (PrimType == RenderDevice::EPrimTriList) ? Data->VertList.Size()/3 : Data->VertList.Size()-2;

            dev->Draw(PrimType, 0, NumPrims);
        }

        else{
		    const int NumPrims = (PrimType == RenderDevice::EPrimTriList) ? Triangles.Size()/3 : Triangles.Size()-2;

            dev->Draw(PrimType, 0, NumPrims, 0, Data->VertList.Size());
        }
#else
# ifdef TRANSGAMING
		const int NumPrims = (PrimType == RenderDevice::EPrimTriList) ? Triangles.Size()/3 : Triangles.Size()-2;
# else
		const int NumPrims = (PrimType == RenderDevice::EPrimTriList) ? Triangles.Size()/3 : Elems.Size()-2;
# endif
		
        dev->Draw(PrimType, 0, NumPrims, 0, Data->VertList.Size());
#endif
	}
}
#endif

void WorldMesh::DrawShadows()
{
	RenderDevice* Device = GEngine->Video->RenderDev;

	View *view = GEngine->Video->GetActiveView();
	Viewport &pViewport = view->LocalViewport;
    Device->SetObjectTransform((Flags & EEntMovable) ? &ObjectToWorld : GIdentityMatrix);
	Device->SetShaderMemory(0,(Flags & EEntMovable) ? 
								&(ObjectToWorld*pViewport.WorldToScreen).GetTranspose()[0] : 
								&pViewport.WorldToScreen.GetTranspose()[0],4);

	if (!(Flags & EVertFormatNTU))
	{
        Data->VertStream[Data->VertStreamCount] = Data->NormStream;
        Device->SetStreams(Data->VertStream, Data->VertStreamCount + 1, Data->ElemStream); // need normals for lighting
	}
    else
        Device->SetStreams(Data->VertStream, Data->VertStreamCount, Data->ElemStream);

#ifdef COLLAPSE_SHADOWS

# ifdef TRANSGAMING_NON_TNL
    if (GEngine->GetRenderDevice()->HWClass >= RenderDevice::EHWClassNV30) {
		Model** Casters = &ShadowCasters[0];
		const int nloop = NumShadowCasters/MAX_MESH_SHADOWS_PER_PASS;
		if (nloop) {	
			for (int i=0; i<nloop; ++i, Casters += MAX_MESH_SHADOWS_PER_PASS) {
				RenderShadows(Casters, MAX_MESH_SHADOWS_PER_PASS);
			}
		}

		int n2 = (NumShadowCasters%MAX_MESH_SHADOWS_PER_PASS);
		if (n2)
			RenderShadows(Casters, n2);
		
		return;
	}
	else 
	{
# endif //TRANSGAMING_NON_TNL
		for (int i=0; i<NumShadowCasters; ++i) {
			RenderShadowPass(ShadowCasters[i]);
		}
# ifdef TRANSGAMING_NON_TNL
	}
# endif //TRANSGAMING_NON_TNL
#else
	for (int i=0; i<NumShadowCasters; ++i) {
		RenderShadowPass(ShadowCasters[i]);
	}
#endif // COLLAPSE_SHADOWS
}

void WorldMesh::Draw(RenderDevice* dev, int mode)
{
    if (Flags & EInvalid)
        return;

	if( !( mode & View::EModeShowTextures ) )
		return;

#ifdef TRANSGAMING_USE_EDITOR
    if( ( mode & View::EModeShowTris ) && ( mode & View::EModeZPass ) )
	{
		GEngine->Video->LineBuf.DrawMesh(this,0xff0000);
		return;
	}
#endif

	View *view = GEngine->Video->GetActiveView();
	Viewport &pViewport = view->LocalViewport;
	dev->SetObjectTransform((Flags & EEntMovable) ? 
		&ObjectToWorld: 
		GIdentityMatrix);
	dev->SetShaderMemory(0,(Flags & EEntMovable) ? 
		&(ObjectToWorld*pViewport.WorldToScreen).GetTranspose()[0] : 
		&pViewport.WorldToScreen.GetTranspose()[0],4);

/*	
	// render reflection?
	if (ReflectSurf && !(Flags & EWater))
	{
		if (mode & View::EModeZPass)
			return;

		extern Matrix ReflectionTexProj;
		extern Matrix RefractionTexProj;

		const Matrix T1 = view->LocalViewport.WorldToScreen
		//const Matrix T1 = ReflectionTexProj
						* ScaleMatrix(Vector(0.5F,0.5F,0.5F))	
						* TranslationMatrix(Vector(0.5F, 0.5F, 0.5F));
		const Matrix T2 = view->LocalViewport.WorldToScreen
		//const Matrix T2 = RefractionTexProj
						* ScaleMatrix(Vector(0.5F,-0.5F,0.5F))	
						* TranslationMatrix(Vector(0.5F, 0.5F, 0.5F));

		dev->SetShaderMemory(40, &T1.GetTranspose()[0], 4);
		dev->SetShaderMemory(44, &T2.GetTranspose()[0], 4);

		static FLOAT MTime = 0;
				
		//dev->SetShaderMemory(50, &Plane(0,0,0,MTime*10), 1);
		const FLOAT WaveTime = MTime * 4;
		dev->SetShaderMemory(50, &Plane(2.0F, 0.05F, 0.5F, WaveTime), 1);	// freq,amp,phase,time
		dev->SetShaderMemory(51, &Plane(3.0F, 0.1F, 1.3F, WaveTime), 1);	// freq,amp,phase,time

		dev->SetShaderMemory(52, &Plane(-1,0,0,0), 1);
		dev->SetShaderMemory(53, &Plane(-0.7F,0.7F,0,0), 1);

		MTime += GEngine->LastDelta;

		dev->SetShaderMemory(14, &Plane(GEngine->Video->GetActiveView()->LocalViewport.EyePosition,0),1);
		
		// pixel params
		dev->SetPixelShaderMemory(0, &Plane(GEngine->World->Water.FresnelBias,GEngine->World->Water.FresnelExponent,0,0),1);

		const float WaterAmount = GEngine->World->Water.WaterAmount;
		const Vector RefrTint = GEngine->World->Water.ShallowWaterColor*WaterAmount;
		const Vector ReflTint = GEngine->World->Water.DeepWaterColor*WaterAmount;

		dev->SetPixelShaderMemory(1, &Plane(RefrTint,0),1);
		dev->SetPixelShaderMemory(2, &Plane(ReflTint,0),1);
		dev->SetPixelShaderMemory(3, &Plane(GetEnvironmentDirLight()->GetDirection(), 1), 1);

		dev->SetIndexedStream(VertStream,ElemStream);
		RenderDefaultPass(DefaultShader);

		return;
	}
*/	

	// Render volumetric pass
	
	if (mode & View::EModeVolPass)
	{
        dev->SetStreams(Data->VertStream, Data->VertStreamCount, Data->ElemStream);

#ifdef TRANSGAMING_MESH_WARNINGS
        TRACE("WorldMesh::Draw(): drawing volume mesh for object '%s' {verts = %d, tris = %d}\n", Name.AsChar(), Data->VertList.Size(), Triangles.Size());
#endif
        RenderDefaultPass(VolShader);
		return;
	}

	// Setup fading materials

	if (BlendShader && BlendShader->PassCount>=1)
	{
		DWORD col = 0xFF;
		if (TimeToDelete!=-1 && TimeToDelete<FadeTime)
		{
			col = BYTE(max(0,255*(TimeToDelete/FadeTime)));
		}
		BlendShader->Passes[0].State.SetConstColor(col<<24);
#ifdef TRANSGAMING_NON_TNL
        if (BlendDetailShader)
            BlendDetailShader->Passes[0].State.SetConstColor(col<<24);
#endif
	}

	Material* shader;
#ifndef TRANSGAMING_NON_TNL
	if (TimeToDelete!=-1 && TimeToDelete<FadeTime)	{
		shader = BlendShader;
		Sort = ESortTrans;
	}
	else {
		shader = DefaultShader;
	}
#else
	bool detail = DetailShader && DetailMap.Name.Len() && DistanceFromCamera<view->DetailDistance;

	if (TimeToDelete!=-1 && TimeToDelete<FadeTime)	{
		shader = detail?BlendDetailShader:BlendShader;
		Sort = ESortTrans;
	}
	else {
		shader = detail?DetailShader:DefaultShader;
	}
#endif

	// Render regular pass

	if (
		((mode & View::EModeZPass) && (Sort == Entity::ESortOpaque))
	||  ((mode & View::EModeMultiPass) && (Sort != Entity::ESortOpaque))
		)
	{
		//if (WaterSurf && dev->HWClass != RenderDevice::EHWClassGenericTnL)
		if (WaterSurf)
		{
#ifdef	TRANSGAMING
			const bool UseEnv = Env.Link && (UseWaterEnv || (GEngine->pcfWorld->WaterQuality == 1));
			//RenderWater((Env.Link && GEngine->World->WaterQuality == 1) ? Env.Link->Water : GEngine->World->Water);
			RenderWater(UseEnv ? Env.Link->Water : GEngine->pcfWorld->Water);
#else
			const bool UseEnv = Env.Link && (UseWaterEnv || (GEngine->World->WaterQuality == 1));
			//RenderWater((Env.Link && GEngine->World->WaterQuality == 1) ? Env.Link->Water : GEngine->World->Water);
			RenderWater(UseEnv ? Env.Link->Water : GEngine->World->Water);
#endif
		}
		else
		if (Flags & EVertFormatNTU)		// NTU mesh
		{
            
			RenderNTU(shader);
		}

#ifdef TRANSGAMING_NON_TNL
		/*
		else if (Flags & EWater && dev->HWClass != RenderDevice::EHWClassGenericTnL)		// TU2 water
		{
			RenderWater(GEngine->World->Water);
			//RenderWater(Env.Link ? Env.Link->Water : GEngine->World->Water);
		}

		else if (Flags & ETeleport && dev->HWClass != RenderDevice::EHWClassGenericTnL)		// TU2 teleport
		{
			RenderWater(GEngine->World->Water);
		}

		else if (Flags & EBlendMap && dev->HWClass != RenderDevice::EHWClassGenericTnL)		// TU2 blendmap
		{
   	        dev->SetStreams(VertStream, VertStreamCount, ElemStream);
			RenderDefaultPass(DefaultShader);
		}
		*/
		else if ((Flags & EWater) && !WaterSurf && dev->HWClass != RenderDevice::EHWClassGenericTnL)		// TU2 water
		{
#ifdef	TRANSGAMING
			RenderWater(GEngine->pcfWorld->Water);
#else
			RenderWater(GEngine->World->Water);
#endif
			//RenderWater(Env.Link ? Env.Link->Water : GEngine->World->Water);
		}

		else if (Flags & ETeleport && dev->HWClass != RenderDevice::EHWClassGenericTnL)		// TU2 teleport
		{
#ifdef	TRANSGAMING
			RenderWater(GEngine->pcfWorld->Water);
#else
			RenderWater(GEngine->World->Water);
#endif
		}

		else if (Flags & EBlendMap && dev->HWClass != RenderDevice::EHWClassGenericTnL)		// TU2 blendmap
		{
#ifdef COLLAPSE_TU2SPEC
#ifdef	TRANSGAMING
				if (GEngine->pcfWorld->HighQualityLights && (SpecularLights[0] || SpecularLights[1])) {
#else
				if (GEngine->World->HighQualityLights && (SpecularLights[0] || SpecularLights[1])) {
#endif

					RenderTU2Specular();
					return;
				}
#endif // COLLAPSE_TU2SPEC

            dev->SetStreams(VertStream, VertStreamCount, ElemStream);
			RenderDefaultPass(DefaultShader);
		}
#endif   //TRANSGAMING_NON_TNL
		else							// TU2 default
		{
#ifdef TRANSGAMING
            Data->VertStream[Data->VertStreamCount] = Data->NormStream;
# ifdef TRANSGAMING_MESH_WARNINGS
            //TRACE("WorldMesh::Draw(): setting streams {verts = %d, norms = %d, size = %d, ElemStream = %d}\n", VertStream[0], NormStream, ARRAY_COUNT(streams), ElemStream);
# endif
            dev->SetStreams(Data->VertStream, Data->VertStreamCount + 1, Data->ElemStream);
#else
			const int streams[] = { Data->VertStream };
			dev->SetStreams(streams,ARRAY_COUNT(streams),Data->ElemStream);
#endif

			if (dev->HWClass == RenderDevice::EHWClassGenericTnL)
			{
#ifdef	TRANSGAMING
# ifdef TRANSGAMING_DEINDEX
#  ifdef TRANSGAMING_MESH_WARNINGS
                TRACE("WorldMesh::Draw(): drawing the object '%s' {indexed = %d, verts = %d, triangles = %d, format = '%s', groups = %d}\n", Name.AsChar(), isIndexed, Data->VertList.Size(), Triangles.Size(), PrimType == RenderDevice::EPrimTriList ? "list" : "strip", Data->Groups.Size());
#  endif
# else
#  ifdef TRANSGAMING_MESH_WARNINGS
                TRACE("WorldMesh::Draw(): drawing the object '%s' {indexed = %d, verts = %d, triangles = %d, format = '%s', groups = %d}\n", Name.AsChar(), Triangles.Size() != 0, Data->VertList.Size(), Triangles.Size(), PrimType == RenderDevice::EPrimTriList ? "list" : "strip", Groups.Size());
#  endif
# endif
				dev->SetFogState(GEngine->pcfWorld->Env.FogParams); 
				// [alex] We have 2 tex coordinates sets, used for DispLists size calculation.
				RenderDefaultPass(DefaultShader, 2);
				dev->SetFogState(GEngine->pcfWorld->Env.FogParams.UseVSFog()); 
#else
				dev->SetFogState(GEngine->World->Env.FogParams); 
				Render(DefaultShader);
				dev->SetFogState(GEngine->World->Env.FogParams.UseVSFog()); 
#endif
			}
#ifdef TRANSGAMING_NON_TNL
			else 
			{
#ifdef COLLAPSE_TU2SPEC
#ifdef	TRANSGAMING
				if (GEngine->pcfWorld->HighQualityLights && (SpecularLights[0] || SpecularLights[1])) {
#else
				if (GEngine->World->HighQualityLights && (SpecularLights[0] || SpecularLights[1])) {
#endif

					RenderTU2Specular();
					return;
				}
#endif // COLLAPSE_TU2SPEC

				Render(shader);
			}
#endif   //TRANSGAMING_NON_TNL
		}
	}

	if (Flags & EWater)		// TU2 water
		return;

	// Render lighting/shadowing passes
    if ((mode & View::EModeMultiPass))
	{
		if (LightCount!=0)
		{
			if (!(Flags & EVertFormatNTU))
			{
                Data->VertStream[Data->VertStreamCount] = Data->NormStream;
                dev->SetStreams(Data->VertStream, Data->VertStreamCount + 1, Data->ElemStream); // need normals for lighting
			}
            else
                dev->SetStreams(Data->VertStream, Data->VertStreamCount, Data->ElemStream);


#ifdef TRANSGAMING_NON_TNL
#ifdef UBER_SHADER			
#ifdef	TRANSGAMING
			if (GEngine->pcfWorld->HighQualityLights && GEngine->GetRenderDevice()->HWClass >= RenderDevice::EHWClassNV40)
#else
			if (GEngine->World->HighQualityLights && GEngine->GetRenderDevice()->HWClass >= RenderDevice::EHWClassNV40)
#endif
			{
				// only collapse point lights
				const Light* L1[4],*L2[4];
				int N1 = 0, N2 = 0;

				int i;
				for (i=0; i<LightCount; ++i) {
#ifdef	TRANSGAMING
					if ((Lights[i]->Flags & Light::ELightDynamic) 
						&& !(GEngine->pcfWorld->DrawDynLights || Lights[i]->ImportantDynamic))
#else
					if ((Lights[i]->Flags & Light::ELightDynamic) 
						&& !(GEngine->World->DrawDynLights || Lights[i]->ImportantDynamic))
#endif
						continue;

#ifdef COLLAPSE_TU2SPEC
					if (Lights[i] == SpecularLights[0] || Lights[i] == SpecularLights[1])
						continue;
#endif // COLLAPSE_TU2SPEC

					if (Lights[i]->GetType() == Light::ELightPoint)
						L1[N1++] = Lights[i];
					else
						L2[N2++] = Lights[i];
				}

				if (N2 > 0) {
					for(int i=0; i<N2; ++i)
						RenderLightPass(L2[i]);
				}

				if (N1 > 0)
					RenderUberLightPass(L1,N1);

				// finito
				return;
			}
#endif // UBER_SHADER
#endif  //TRANSGAMING_NON_TNL
#ifdef TRANSGAMING_MESH_WARNINGS
            TRACE("WorldMesh::Draw(): rendering TU2 light pass for '%s' for %d lights\n", Name.AsChar(), LightCount);
#endif
			for (int i=0; i<LightCount; ++i) 
			{
				Light *l = Lights[i];

#ifdef COLLAPSE_TU2SPEC
				if (l == SpecularLights[0] || l == SpecularLights[1])
					continue;
#endif // COLLAPSE_TU2SPEC

#ifdef	TRANSGAMING
				if ((GEngine->pcfWorld->HighQualityLights && l->GetType() == Light::ELightPoint)	// TODO: add spot!
					|| (l->Flags & Light::ELightDynamic 
						&& (GEngine->pcfWorld->DrawDynLights || l->GetType() == Light::ELightSpot || l->ImportantDynamic))
					)
#else
				if ((GEngine->World->HighQualityLights && l->GetType() == Light::ELightPoint)	// TODO: add spot!
					|| (l->Flags & Light::ELightDynamic 
						&& (GEngine->World->DrawDynLights || l->GetType() == Light::ELightSpot || l->ImportantDynamic))
					)
#endif
				{
					RenderLightPass(l);
				}
			}
		}
	}
}

#ifdef TRANSGAMING_NON_TNL
void WorldMesh::RenderUberLightPass(const Light* Lights[], int NumLights)
{
	RenderDevice* Device = GEngine->Video->RenderDev;

	BOOL LightSwitch[Entity::MaxLights];
#ifdef TRANSGAMING
    Plane tmpPlane;
#endif

	for (int i=0; i<Entity::MaxLights; ++i)
	{
		if (i<NumLights)
		{
			LightSwitch[i] = TRUE;

			// set light params
#ifdef TRANSGAMING
            tmpPlane.set(Lights[i]->GetWorldPosition(),1.0F);
			Device->SetShaderMemory(41+i, &tmpPlane, 1);
#else
			Device->SetShaderMemory(41+i, &Plane(Lights[i]->GetWorldPosition(),1.0F), 1);
#endif

			Vector LightParams(0,0,0);
			if (!(Flags & EVertFormatNTU))
			{
				if (Lights[i]->Flags & Light::ELightDynamic)
					LightParams = Vector(1,1,2);
				else
					LightParams = Vector(0,0,4);

			}

#ifdef	TRANSGAMING
            tmpPlane.set(LightParams, 1.0f/Lights[i]->GetRadius());
			Device->SetPixelShaderMemory(0+i, &tmpPlane, 1);
            tmpPlane.set(Vector(BaseVector(Lights[i]->Color))*Lights[i]->Intensity, SpecularPower);
			Device->SetPixelShaderMemory(4+i, &tmpPlane, 1);
#else
			Device->SetPixelShaderMemory(0+i, &Plane(LightParams, 1.0f/Lights[i]->GetRadius()), 1);
			Device->SetPixelShaderMemory(4+i, &Plane(Vector(Lights[i]->Color)*Lights[i]->Intensity, SpecularPower), 1);
#endif
		}
		else
		{
			// turn it off
			LightSwitch[i] = FALSE;
		}
	}

	Device->SetShaderMemory(4,&ObjectToWorld.GetTranspose()[0],4);
	Device->SetShaderMemory(16,&ObjectToWorld[0],4);


#ifdef	TRANSGAMING
    tmpPlane.set(GEngine->Video->GetActiveView()->LocalViewport.EyePosition,1);
	Device->SetShaderMemory(40,&tmpPlane, 1);
    tmpPlane.set(GEngine->pcfWorld->UseFog ? 0.0F : 1.0F,0,0,0);
	Device->SetShaderMemory(38, &tmpPlane, 1);
#else
	Device->SetShaderMemory(40,&Plane(GEngine->Video->GetActiveView()->LocalViewport.EyePosition,1), 1);
	Device->SetShaderMemory(38, &Plane(GEngine->World->UseFog ? 0.0F : 1.0F,0,0,0), 1);
#endif

	/*
	if (GEngine->World->UseFog) {
		Device->SetShaderMemory(38, &Plane(0,0,0,0), 1);
		Device->SetShaderMemory(39, &Plane(Color(GEngine->World->Env.FogParams.Color), 1), 1);	
	} else {
		Device->SetShaderMemory(38, &Plane(1,0,0,0), 1);
		Device->SetShaderMemory(39, &Plane(1,1,1,1), 1);
	}
	*/

	//Device->SetShaderMemory(39, &Color(GEngine->World->Env.FogParams.Color), 1);

	Device->SetPixelShaderMemoryB(8, LightSwitch, Entity::MaxLights);

	Render(PointLightShader);
}
#endif

void WorldMesh::DrawShadow(Model* Caster)
{
	RenderDevice* Device = GEngine->Video->RenderDev;

	View *view = GEngine->Video->GetActiveView();
	Viewport &pViewport = view->LocalViewport;
	Device->SetObjectTransform((Flags & EEntMovable) ? &ObjectToWorld : GIdentityMatrix);
	Device->SetShaderMemory(0,(Flags & EEntMovable) ? 
								&(ObjectToWorld*pViewport.WorldToScreen).GetTranspose()[0] : 
								&pViewport.WorldToScreen.GetTranspose()[0],4);

	if (!(Flags & EVertFormatNTU))
	{
        Data->VertStream[Data->VertStreamCount] = Data->NormStream;
        Device->SetStreams(Data->VertStream, Data->VertStreamCount + 1, Data->ElemStream); // need normals for lighting
	}
    else
        Device->SetStreams(Data->VertStream, Data->VertStreamCount, Data->ElemStream);

	RenderShadowPass(Caster);
}

void WorldMesh::UpdateVolEntity() 
{
	if (!WorldLink || !(Flags & EEntMovable))
		return;

	for (int i=0; i<WorldLink->Volumes.Size(); ++i)
	{
		Volume* vol = WorldLink->Volumes[i];
		if (vol->Box.Intersect(Box))
			vol->DEntities.Add(this);
	}
}

void WorldMesh::GetClosestPoint( Vector sourcePoint, float& distance, Vector& closestPoint )
{
	Matrix  WorldToObject = ObjectToWorld.Inverse();
	Vector  sourcePointLocal = WorldToObject.TransformCoord( sourcePoint );

#ifdef TRANSGAMING
    Data->VertList.getClosestPoint(sourcePointLocal, Data->VertList.getVertPos(0), distance, closestPoint);
#else
	Assert( Verts.Size() );

    closestPoint = Verts[0].Pos;
    distance = (Verts[0].Pos - sourcePointLocal).MagnitudeSquared();

    for (int i = 1; i < Verts.Size(); ++i){
        float newDist = (Verts[i].Pos - sourcePointLocal).MagnitudeSquared();
        if (newDist < distance){
            distance = newDist;
            closestPoint = Verts[i].Pos;
        }
    }

	distance = (float)sqrt( distance );
#endif
    closestPoint = ObjectToWorld.TransformCoord( closestPoint );
}

Vector WorldMesh::GetRandomPoint()
{
    Vector point(0,0,0);


#ifdef TRANSGAMING
    point = Data->VertList.getRandomPoint();
#else
    if (Verts.Size() > 0 )
        point = Verts[rand() % Verts.Size()].Pos;
#endif

    point *= Scale;
    //point = ObjectToWorld.TransformCoord( point ); worldspace
    return point;
}

#define WORLDMESH_LOAD_SAVE_TOKEN 0x5e54

bool WorldMesh::LoadEntity( FileObject* f )
{
	if( !f->TestToken( WORLDMESH_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: WorldMesh::LoadEntity()\n" );
		return false;
	}

	Entity::LoadEntity( f );

	// This method loads only meshes that were not loaded from MPK. Therefore, we can safely assume that activeMeshIndex of PO
	// is equal to -1.

	Vector position;
	Quaternion orientation;

	BYTE hasPO;

	*f >> position >> orientation >> hasPO;

#ifdef TRANSGAMING
    //skip reading the normal map name
    int tmp;
    char buf[MAX_PATH];


    //skip over the detail map name
	*f >> tmp;
    Assert(tmp <= MAX_PATH);
    f->Read(buf, tmp);
    
    //skip over the normal map name
    *f >> tmp;
    Assert(tmp <= MAX_PATH);
    f->Read(buf, tmp);
    
    *f >> CubeMapName;

#else
	*f >> DetailMap >> NormalMapName >> CubeMapName;
#endif

	SetPositionAndRotation( position, orientation );

	// Load PO?
	Assert( !PO );
	if( hasPO )
	{
		DWORD bodyType;
		float bodyScale;
		*f >> bodyType >> bodyScale;	// this data was saved by PO
#ifndef TRANSGAMING_NO_HAVOK
		CreatePhysicsObject( bodyType, bodyScale, -1, true );
		return PO->LoadPO( f );
#else
		return true;
#endif
	}
	else
		return true;
}

bool WorldMesh::SaveEntity( FileObject* f )
{
	BYTE hasPO = ( PO != 0 );
#ifdef _DEBUG
	int activeMeshIndex = -1;
#ifndef TRANSGAMING_NO_HAVOK
	if( hasPO ) activeMeshIndex = PO->GetActiveMeshIndex();
#endif // TRANSGAMING_NO_HAVOK
	Assert( activeMeshIndex == -1 );
#endif
	Vector position = GetPosition();
	Quaternion orientation = GetRotation();

	*f << (int)ETypeMesh << FileName << Name << GetScale() << (char) (Flags&EEntCenteredGeometry);	// data for world, so it knows how to make us

	if( !f->PutToken( WORLDMESH_LOAD_SAVE_TOKEN ) ) return false;

	Entity::SaveEntity( f );

	*f << position << orientation << hasPO;	// data for us, so we know what to set
#ifdef TRANSGAMING
    //skip writing out the normal map and detail map names
    int tmp = 0;


	*f << tmp << tmp << CubeMapName;
#else
	*f << DetailMap << NormalMapName << CubeMapName;
#endif

#ifndef TRANSGAMING_NO_HAVOK
	return hasPO?PO->SavePO( f ): true;
#else
	return true;
#endif
}

void WorldMesh::SetDefaultMaterial(const char* Name)
{
	if (!GEngine->Video)
		return;

	Assert(Name);
	MaterialSystem* MatSys = GEngine->Video->MaterialSys;	
	String matName = String(Name).ToLower();

	// HACK!!!!
	DefaultShaderName = matName;

#ifdef	TRANSGAMING
	if (GEngine->pcfWorld->WaterQuality == 0 && (!stricmp(matName.AsChar(), "water_ntu_rr") || !stricmp(matName.AsChar(), "water_ntu_refl"))) {
#else
	if (GEngine->World->WaterQuality == 0 && (!stricmp(matName.AsChar(), "water_ntu_rr") || !stricmp(matName.AsChar(), "water_ntu_refl"))) {
#endif
		matName = "water_ntu";
	}

	Material* M = MatSys->FindMaterial(matName.AsChar());
	if (M)
	{
		DefaultShader = M;

        TRACE("WorldMesh::SetDefaultMaterial(): setting default material name to '%s'\n", DefaultShaderName.AsChar());
		
		if (M->Flags & EMF_WarpScreen)
		{
			WarpScreen = true;
			DetailShader = NULL;
			Sort = ESortWarp;
		}

		if (M->Flags & EMF_SortTrans)
		{
			Flags|=EEntTranslucent;
			Sort = ESortTrans;
		}
		else if (!WarpScreen)
		{
			Flags &= ~EEntTranslucent;
			Sort = ESortOpaque;
		}

		if (M->Flags & (EMF_Reflect|EMF_Refract))
		{
			ReflectSurf = 0;
			
			if (M->Flags & EMF_Reflect)
				ReflectSurf |= Entity::EReflectScene;
			if (M->Flags & EMF_Refract)
				ReflectSurf |= Entity::ERefractScene;

			DetailShader = NULL;
			Flags |= ENoLight;
			Flags &= ~EWater;
		}

		//RenderDevice* dev = GEngine->GetRenderDevice();
		//if ((M->Flags & EMF_Water) && (dev->HWClass != RenderDevice::EHWClassGenericTnL)) {
		if (M->Flags & EMF_Water) {
			Flags |= EWater;
			WaterSurf = true;
#ifdef	TRANSGAMING
#ifndef TRANSGAMING_NO_HAVOK
			GEngine->pcfWorld->Physics->SetEntityCollisionGroup(this, false, HCGNoncolliding);
#endif // TRANSGAMING_NO_HAVOK
#else
			GEngine->World->Physics->SetEntityCollisionGroup(this, false, HCGNoncolliding);			
#endif

			if (DefaultShaderName == "water_ntu")
				UseWaterEnv = 1;
		}
	}
	else
        TRACE("WorldMesh::SetDefaultMaterial(): ERROR-> material '%s' not found!\n", DefaultShaderName.AsChar());
}

Plane WorldMesh::GetReflectionPlane() const
{
#ifdef TRANSGAMING
# ifdef TRANSGAMING_DEINDEX
    if (!isIndexed)
        return Data->VertList.getReflectionPlane();

    else
        return Data->VertList.getReflectionPlane(&Data->Triangles);
# else
    return Data->VertList.getReflectionPlane(&Data->Triangles);
# endif
#else
    const Vector& P1 = Verts[Triangles[2]].Pos;
    const Vector& P2 = Verts[Triangles[1]].Pos;
    const Vector& P3 = Verts[Triangles[0]].Pos;

    return Plane(P1,P2,P3);
 
    //return Plane(Vector(0,1,0), P1.Dot(Vector(0,1,0)));
#endif
}

void WorldMesh::SetSpecular(float Power)
{	
	if ((Flags & ENoLight) || !GEngine->Video)
		return;

	BOOL Enabled = (SpecularLights[0] || SpecularLights[1]);

	if (GEngine->GetRenderDevice()->HWClass < RenderDevice::EHWClassNV30)
		Enabled = 0;

    if (!(Flags & EVertFormatNTU))
    {
#ifdef	TRANSGAMING
		Lighting = Enabled ? (GEngine->pcfWorld->HighQualityLights ? ELightingAllLights : ELightingDLights)
						   : ELightingDLights;
#else
		Lighting = Enabled ? (GEngine->World->HighQualityLights ? ELightingAllLights : ELightingDLights)
						   : ELightingDLights;
#endif
    }	

	SpecularPower = Power;

	MaterialSystem* MatSys = GEngine->Video->MaterialSys;	

#ifdef TRANSGAMING_NON_TNL
#ifdef	TRANSGAMING
	if (GEngine->pcfWorld->HighQualityLights && GEngine->GetRenderDevice()->HWClass >= RenderDevice::EHWClassNV30)
#else
	if (GEngine->World->HighQualityLights && GEngine->GetRenderDevice()->HWClass >= RenderDevice::EHWClassNV30)
#endif
	{
		for(int i=0; i<Groups.Size(); ++i) 
		{
			Groups[i].TexGlossPtr = NULL;

			const String GlossTexName = Groups[i].TexName + "_s";
			if (MatSys->IsTextureOnDisk(GlossTexName.AsChar()))
				Groups[i].TexGlossPtr = MatSys->CreateTexture(GlossTexName.AsChar());
			
			if (!Groups[i].TexGlossPtr)
				Groups[i].TexGlossPtr = Groups[i].TexPtr;
		}
	}
	else
	{
#endif  //TRANSGAMING_NON_TNL
		for(int i=0; i<Data->Groups.Size(); ++i) 
		{
			//if (Data->Groups[i].TexGlossPtr && (Data->Groups[i].TexGlossPtr != Data->Groups[i].TexPtr))
			//	MatSys->ReleaseTexture(Data->Groups[i].TexGlossPtr);
			if (Data->Groups[i].TexGlossPtr) {
				Texture* T = Data->Groups[i].TexGlossPtr;
				if (T->FileName.AsChar() && strstr(T->FileName.AsChar(), "_s.")) {
					//GLog.Print("Mesh: releasing specular mask: %s\n", T->FileName.AsChar());
					MatSys->ReleaseTexture(Data->Groups[i].TexGlossPtr);
				}
			}
		}
#ifdef TRANSGAMING_NON_TNL
	}
#endif  //TRANSGAMING_NON_TNL
}

void WorldMesh::AddSpecularLight(Light* L)
{
	if (Flags & ENoLight)
		return;

	if (!L)
		return;

	if (SpecularLights[0] == NULL)
		SpecularLights[0] = L;
	else if (SpecularLights[1] == NULL)
		SpecularLights[1] = L;
}

void WorldMesh::DrawZ(const Matrix& WorldToScreen)
{
    RenderDevice* Device = GEngine->Video->RenderDev;

	if ((Flags & (EWater|ETeleport)) || WaterSurf)
		return;

#ifdef TRANSGAMING_NON_TNL
	if (Flags & EEntMovable)
	{
		Device->SetShaderMemory(0, &(ObjectToWorld*WorldToScreen).GetTranspose()[0], 4);
		Device->SetShaderMemory(4, &ObjectToWorld.GetTranspose()[0], 4);
	}
	else
	{
		Device->SetShaderMemory(0, &WorldToScreen.GetTranspose()[0], 4);
		Device->SetShaderMemory(4, GIdentityMatrix, 4);
	}
#endif

	/*
	if (!(Flags & EVertFormatNTU))
	{
        VertStream[VertStreamCount] = NormStream;
		Device->SetStreams(VertStream, VertStreamCount + 1, ElemStream); // need normals for lighting
	}
	else 
		Device->SetStreams(VertStream, VertStreamCount, ElemStream);
	*/

#ifdef TRANSGAMING_DEINDEX
    if (!isIndexed){
	    Device->SetStreams(VertStream, VertStreamCount, EInvalidIndex);

	    const int NumPrims = (PrimType == RenderDevice::EPrimTriList) ? Data->VertList.Size()/3 : Data->VertList.Size()-2;
# ifdef TRANSGAMING_MESH_WARNINGS
        //TRACE("WorldMesh::DrawZ(): rendering Z pass for deindexed data for the object '%s'\n", Name.AsChar());
# endif
	    Device->Draw(PrimType, 0, NumPrims);
    }

    else{
	    Device->SetStreams(VertStream, VertStreamCount, ElemStream);

        Assert(PrimType == RenderDevice::EPrimTriList);

	    const int NumPrims = Triangles.Size() / 3;

# ifdef TRANSGAMING_MESH_WARNINGS
        //TRACE("WorldMesh::DrawZ(): rendering Z pass for indexed data for the object '%s'\n", Name.AsChar());
# endif
	    Device->Draw(PrimType, 0, NumPrims, 0, Data->VertList.Size());
    }
#else
	Device->SetStreams(Data->VertStream, Data->VertStreamCount, Data->ElemStream);

# ifdef TRANSGAMING
	const int NumPrims = (PrimType == RenderDevice::EPrimTriList) ? Data->Triangles.Size()/3 : Data->Triangles.Size()-2;
# else
	const int NumPrims = (PrimType == RenderDevice::EPrimTriList) ? Data->Triangles.Size()/3 : Data->Elems.Size()-2;
# endif

# ifdef TRANSGAMING_MESH_WARNINGS
    //TRACE("WorldMesh::DrawZ(): rendering Z pass for the object '%s'\n", Name.AsChar());
# endif
	Device->Draw(PrimType, 0, NumPrims, 0, Data->VertList.Size());
#endif
}
