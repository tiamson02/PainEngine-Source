
// #pragma once
#ifndef _SIMPLEMESH_
#define _SIMPLEMESH_


#include "Render/Vertex.h"
#include "Render/VertexBuffer.h"
#include "Render/MaterialDef.h"
#include "Render/VertexList.h"

#ifdef TRANSGAMING_PS2
#include <GL/gl.h>
#include <GL/ps2gl.h>
#endif

class WorldMesh;
class Material;

class ENGINE_API SimpleMesh
{
public:
    // inner classes
#ifdef TRANSGAMING
    enum MeshFlags{
        MESH_TRISTRIP_BIT =     0x80000000,     //set if the incoming data is in tristrip format
        MESH_DEINDEXED_BIT =    0x40000000,     //set if the incoming data is deindexed
        MESH_PSPDATA_BIT =      0x20000000,     //set if the incoming data is already in the PSP native format
        MESH_PS2DATA_BIT =      0x10000000,     //set if the incoming data is already in the PS2 native format
        MESH_BOUNDINGBOX_BIT =  0x08000000,     //set if the stream contains a bounding box
        MESH_MTFORMAT_BIT =     0x04000000,     //set if the incoming data is in MT format
        MESH_VOLUMEPARAMS_BIT = 0x02000000,     //set if the mesh contains volume information (color, distance)

        MESH_UVCOUNT_MASK =     0x00000003,     //support up to 3 UV channels

        MESH_ALL_MASK =         MESH_TRISTRIP_BIT | 
                                MESH_DEINDEXED_BIT |
                                MESH_PSPDATA_BIT |
                                MESH_PS2DATA_BIT |
                                MESH_BOUNDINGBOX_BIT |
                                MESH_MTFORMAT_BIT |
                                MESH_VOLUMEPARAMS_BIT,
    };
#endif

	class Group
	{
	public:
	    String      TexName;
        Texture*    TexPtr;
		Texture*	TexEmPtr;
		Texture*	TexGlossPtr;

#ifdef TRANSGAMING_PSP
        Vector      staticBoundingBox[8];
#endif

		int	        RenderBase;
		int	        RenderCount;
		int			VertexMin;
		int			VertexCount;

		MaterialDef	ColorMap,BlendMap;

        Group() 
        :   TexName("notex"), RenderBase(0), RenderCount(0), TexPtr(0), TexEmPtr(0), TexGlossPtr(0)
		,	VertexMin(0), VertexCount(0)
        {}
		~Group() {}

#ifdef TRANSGAMING_PSP
        FileObject &    load(FileObject &file, bool readBoundingBox = false);
        FileObject &    save(FileObject &file, bool writeBoundingBox = false);
#else
    	friend FileObject& operator<<(FileObject& file, const Group& grp);
    	friend FileObject& operator>>(FileObject& file, Group& grp);
#endif
	};

#ifndef TRANSGAMING
	struct Edge
	{
		WORD	V0, V1;
		WORD	E0, E1;
	};
#endif

    // mesh flags <- this is crap it shouldn't touch entity's Flags
    enum
    {
        EVertFormatNTU  = (1<<10), // vertex stream has Normal+TU (otherwise TU1+TU2)
#ifndef TRANSGAMING
        EPackedFormat	= (1<<11),
#endif
        ENoLight	    = (1<<20), // no lighting
#ifndef TRANSGAMING
        ENormTangent    = (1<<21), // normals are tangent vectors+TU, else only Normal
#endif
        ENoClip         = (1<<22),
        ETeleport       = (1<<23), // a teleport mesh
        EPhysics        = (1<<24),
		EClone			= (1<<25),
		EBlendMap		= (1<<26),
		EWater			= (1<<27),  // bumpy shiny surface ;)
		E2Sided			= (1<<28),
		EATest			= (1<<29),
		ENoDepth		= (1<<30),	// disable ztest & zwrite
        EInvalid        = (1<<31),  // cannot be rendered, invalid format or data
    };

	enum 
	{
		ETexAlphaMap		= (1<<0),
		ETexNormalMap		= (1<<1),
		ETexCubeMap			= (1<<2),
		ETexDetailMap		= (1<<3),
		ETexLightMap		= (1<<4),
		ETexBaseMap			= (1<<5),
		ETexBlendMap		= (1<<6),
	};


	class SimpleMeshData
	{
	public:

		SimpleMeshData()
		{
			UseCount		= 0;

			ElemStream		= EInvalidIndex;
			NormStream		= EInvalidIndex;
			VertStreamCount = 0;
			for (int i = 0; i < VertexList::MaxStreamCount; i++)
				VertStream[i] = EInvalidIndex;

#ifdef TRANSGAMING_PS2
			DisplayList		= 0;
			RenderVertCount	= 0;
			TexturesCached	= false;
			CurrentFrame	= -1;
			RenderInstance	=  0;

#endif
		}

		void						AddRef(void)	{ UseCount++; }
		void						DelRef(void)
		{
			if (--UseCount==0)
				delete this;
		}

		int							UseCount;

		DynamicArray<Group>			Groups;
		DynamicArray<WORD>			Triangles;
#ifndef TRANSGAMING
		DynamicArray<Vector>		TriNormals;
	//	DynamicArray<Edge>			Edges;			//unused
	//	DynamicArray<WORD>			Elems;			//unused
#endif

		VertexList					VertList;
		//DynamicArray<MeshVertex>    Verts;
		DynamicArray<Vector>        Normals;

		int							VertStreamCount;
		int							VertStream[VertexList::MaxStreamCount];
		int							NormStream;
		int							ElemStream;

#ifdef TRANSGAMING_PS2

		unsigned int				DisplayList;
		unsigned int				RenderVertCount;
		bool						TexturesCached;
		int							CurrentFrame;
		int							RenderInstance;

#define MAXSKINMESHRENDERS 6

		typedef VertexBuffer<Vector4,  RenderDevice::EVertexPos4>	PS2PosBuffer;
		typedef VertexBuffer<Vector,   RenderDevice::EVertexNormal>	PS2NormBuffer;
		typedef VertexBuffer<TexCoord, RenderDevice::EVertexTU1>	PS2TexBuffer;

		PS2PosBuffer				ps2_Verts[MAXSKINMESHRENDERS];
		PS2NormBuffer				ps2_Normals[MAXSKINMESHRENDERS];
		PS2TexBuffer				ps2_UVs[MAXSKINMESHRENDERS];

#endif
	};

public:
    // data
    String          LightMapName;
#ifndef TRANSGAMING
	String			NormalMapName;
#endif
	String			CubeMapName;
	MaterialDef		AlphaMap;
#ifndef TRANSGAMING
    MaterialDef		DetailMap;
#endif
	int				ReloadFlags;
	
	Texture*		LightMapTex;
#ifndef TRANSGAMING
    Texture*		NormalMapTex;
#endif
	Texture*		CubeMapTex;

	Vector			MatSpecColor;
	float			MatSpecPower;
	float			MatRefract;
	float			MatFresnel;
	Vector			MatRefrTint;
	Vector			MatReflTint;

#ifndef TRANSGAMING
	String			CloneOf;
#endif
	WorldMesh*		Master;

	Material*		DefaultShader;
	Material*		DetailShader;
	Material*		BlendShader;
	Material*		BlendDetailShader;
	Material*		VolShader;
	Material*		ShadowMapShader;
	Material*		BlurShader;
	Material*		GhostFXShader;

	Material*		ShadowPassShader;
	Material*		PointLightShader;
	Material*		SpotLightShader;
	Material*		PointTU2GlossShader;

    int PrimType;
#ifdef TRANSGAMING_DEINDEX
    bool isIndexed:1;
#endif
#ifdef TRANSGAMING_PSP
    bool    hasBoundingBox:1;
#endif

	SimpleMeshData	*Data;

public:

	SimpleMesh();
	virtual ~SimpleMesh();

	void Init();

	friend FileObject& operator>>(FileObject& file, SimpleMesh& sm);
	friend FileObject& operator<<(FileObject& file, const SimpleMesh& sm);
	SimpleMesh &operator=(const SimpleMesh &sm);

	void SetTextureName(int texType, const char *texName);
	void SetTexturesPriority(DWORD priority);

    void RenderDefaultPass(Material* shader, int numTexSets = 1);

#ifdef TRANSGAMING_STRIPIFY
    void Stripify(void);
#endif

#ifndef TRANSGAMING
protected:
	void CalcTangents(DynamicArray<Plane>& TanVecs);
#endif
};


#endif //_SIMPLEMESH_

