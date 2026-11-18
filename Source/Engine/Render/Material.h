
// #pragma once
#ifndef _MATERIAL_
#define _MATERIAL_


#include "Core/HashMap.h"
#include "Render/Vertex.h"
#include "Engine/Entity.h"
//#include "Engine/World.h"
#include "Render/MaterialDef.h"

class Entity;

// ==================================================================

struct StreamDef
{
    StreamDef(DWORD s1) 
    :   S1(s1), StreamCount(1) 
    {}
    StreamDef(DWORD s1, DWORD s2)
    :   S1(s1), S2(s2), StreamCount(2) 
    {}
    StreamDef(DWORD s1, DWORD s2, DWORD s3)
    :   S1(s1), S2(s2), S3(s3), StreamCount(3) 
    {}
    StreamDef(DWORD s1, DWORD s2, DWORD s3, DWORD s4)
    :   S1(s1), S2(s2), S3(s3), S4(s4), StreamCount(4) 
    {}

    
#ifdef TRANSGAMING
    void set(DWORD s1){
        S1 = s1;
        StreamCount = 1;
    }
    void set(DWORD s1, DWORD s2){
        S1 = s1;
        S2 = s2;

        StreamCount = 2;
    }
    void set(DWORD s1, DWORD s2, DWORD s3){
        S1 = s1;
        S2 = s2;
        S3 = s3;

        StreamCount = 3;
    }
    void set(DWORD s1, DWORD s2, DWORD s3, DWORD s4){
        S1 = s1;
        S2 = s2;
        S3 = s3;
        S4 = s4;

        StreamCount = 4;
    }
#endif

    union
    {
        DWORD       Streams[4];
        struct {    DWORD S1,S2,S3,S4; };
    };

    int     StreamCount;
};

// ==================================================================

#ifdef TRANSGAMING
# define MAX_TEXTURE_FRAMES  (6)
#else
# define MAX_TEXTURE_FRAMES  (32)
#endif

class Texture
{
private:
    //private flags
    enum{
    };

public:
	// flags
	enum
	{
		ETexInvalid		    = (1<<0),
		ETexNoMipMaps	    = (1<<1),
		ETexDuDv			= (1<<2),
		ETexNoLOD			= (1<<3),
		ETexRaw32			= (1<<4),
		ETexRaw8			= (1<<5),
        ETexAnim            = (1<<6),
		ETexSystem			= (1<<7), // system texture are never released!
		ETexRaw32Alpha		= (1<<8),
		ETexDynamic         = (1<<9),
        ETexDataValid       = (1<<10),
        ETexDataEmpty       = (1<<11),
#ifndef TRANSGAMING
        ETexAnim3D			= (1<<12),	// use 3D texture for animated frames
#endif
	};

	// texture type
	enum
	{
		ETexRegular			= 0,
		ETexCubeMap			= 1,
#ifndef TRANSGAMING
        ETexVolume			= 2,
#endif
		
		// special textures
		ETexRenderTargetCubeMap	= 19,
		ETexRenderTarget		= 20,
		ETexDepthTarget			= 21,		
		ETexNormCubeMap			= 22,
	};

	// texture format
	enum
	{
		// use file format 
		ETexFmt_Default			= 0,
		// byte formats
		ETexFmt_A8R8G8B8		= 1,
		ETexFmt_A8				= 2,
		ETexFmt_A16B16G16R16	= 3,
		// floating point
		ETexFmt_F16_ABGR		= 20,
		ETexFmt_F16_R			= 21,
		ETexFmt_F16_GR			= 22,
		ETexFmt_F32_ARGB		= 23,
		ETexFmt_F32_R			= 24,
		ETexFmt_F32_GR			= 25,
	};

	enum
	{
		ETexAnimNone		= 0,
		ETexAnimAdvance		= 1,
	};

    unsigned    Type: 6;
    unsigned    Format: 6;
    unsigned    Flags: 12;
    unsigned    AnimType: 2;
    unsigned    NumFrames: 5;
    float	    Fps;
    int		    RefCount;
    BYTE*       DataFile;
    int         Size;
    int         DeviceId;
    int         FrameIds[MAX_TEXTURE_FRAMES];
    int		    OriginalWidth;
    int		    OriginalHeight;
    int		    Width;
    int		    Height;
    int		    Depth;
    int		    PixelBytes;
    String      FileName;
#ifndef TRANSGAMING
    int		    LevelOfDetail;
    int		    MemoryStats;
#endif

    bool	    IsLocked;

    Texture() 
	:	Type(ETexRegular), Format(0), AnimType(ETexAnimNone), Flags(0), RefCount(0), DataFile(0)
	,	Size(0), DeviceId(EInvalidIndex), NumFrames(0), Fps(0), IsLocked(false)
#ifndef TRANSGAMING
    , LevelOfDetail(0)
    , MemoryStats(0)
#endif
	{}

	Texture(int type, DWORD flags=0)
	:	Type(type), Format(0), AnimType(ETexAnimNone), Flags(flags), RefCount(0), DataFile(0)
	,	Size(0), DeviceId(EInvalidIndex), NumFrames(0), Fps(0), IsLocked(false)
#ifndef TRANSGAMING
    , LevelOfDetail(0)
    , MemoryStats(0)
#endif
	{}

	~Texture();

    BOOL Load(const char* name);
	void ReleaseData();
    void FreeData();

    bool isDataValid() const{ return (Flags & ETexDataValid) != 0; }
    bool wasCreatedEmpty() const{ return (Flags & ETexDataEmpty) != 0; }

	// utility functions
#ifndef TRANSGAMING
	void GenerateAttenDst2(int size);
	void GenerateAttenExp(int size);
#endif
};

// ==================================================================

struct MaterialInput
{
	enum 
	{
		EInputColor			= 1<<0,
		EInputLight			= 1<<1,
#ifndef TRANSGAMING
		EInputNormal		= 1<<2,
		EInputDetail		= 1<<3,
#endif
		EInputCube			= 1<<4,
		EInputBlend			= 1<<5,
		EInputAlpha			= 1<<6,
		EInputEmissive  	= 1<<7,
		EInputGloss			= 1<<8,
		EInputEnvCubeMap	= 1<<9,
		EInputWarpMap		= 1<<10,
		EDefBlend			= 1<<11,
		EDefAlpha			= 1<<12,
#ifndef TRANSGAMING
		EDefDetail      	= 1<<13,
#endif
	};

    Texture*    ColorMap;
    Texture*    LightMap;
#ifndef TRANSGAMING
    Texture*    NormalMap;
	Texture*	DetailMap;
#endif
	Texture*	CubeMap;
	Texture*	BlendMap;
	Texture*	AlphaMap;
	Texture*	EmissiveMap;
	Texture*	GlossMap;
	Texture*	EnvCubeMap;
	Texture*	WarpMap;

	MaterialDef*	BlendDef;
	MaterialDef*	AlphaDef;
#ifndef TRANSGAMING
	MaterialDef*	DetailDef;
#endif

	// convenience constructors
    MaterialInput() 
    {}
    MaterialInput(Texture* colorMap) 
    :   ColorMap(colorMap) 
    {}
    MaterialInput(Texture* colorMap, Texture* lightMap)
    :   ColorMap(colorMap), LightMap(lightMap)
    {}
    MaterialInput(Texture* colorMap, Texture* lightMap, Texture* normalMap)
#ifdef TRANSGAMING
        :   ColorMap(colorMap), LightMap(lightMap)
#else
        :   ColorMap(colorMap), LightMap(lightMap), NormalMap(normalMap)
#endif
    {}
	MaterialInput(int input, Texture** maps);
	MaterialInput(int input, Texture** maps, MaterialDef** mats);
};

// ==================================================================

class RenderPass
{
public:
	// input types
	enum
	{
        EMatNone        	= 0,    // no texture will be set
        EMatUseTexture  	= 1,    // use material's own texture
		EMatInColorMap		= 2,    // use input color map
		EMatInLightMap		= 3,
		EMatInNormalMap		= 4,
		EMatInDetailMap		= 5,
		EMatInCubeMap		= 6,
		EMatInBlendMap		= 7,
		EMatInAlphaMap		= 8,
		EMatInEmissiveMap	= 9,
		EMatInGlossMap		= 10,
		EMatInEnvCubeMap	= 11,
		EMatInWarpMap		= 12,
		EMatInFBTex0		= 13,	// use captured framebuffer
		EMatInFBTex1		= 14,
		EMatInFBTex2		= 15,
	};

	struct TextureInput
	{
        int         Type;
		Texture*	TexPtr;

		TextureInput() : Type(EMatNone), TexPtr(0) {}
	};

	// misc flags
	// NOTE: ugly hack (tm) with pass render target changes
	//		 if one pass changes a render target, it must be followed
	//		 by another pass that resets render target back to framebuffer
	//		 use flags below to do that
	enum 
	{
		EPass_RTargetFB			= (1<<20),
		EPass_RTargetTexFB1		= (1<<21),
		EPass_RTargetTexFB2		= (1<<22),
	};

public:
	static const int	MaxTextures=8;
	TextureInput		TexMaps [MaxTextures];
	TextureState		TexState[MaxTextures];
	int					TexCount;

    static const int    MaxShaderCount = 8;
	RenderState			State;
	int					VertShader[MaxShaderCount];
	int					FragShader;
	DWORD				Flags;

public:
	RenderPass() 
	:	TexCount(0), FragShader(EInvalidIndex), Flags(0)
	{
        for (int i = 0; i < MaxShaderCount; i++)
            VertShader[i] = EInvalidIndex;
    }

	void Activate();
	void SetInput(MaterialInput* matIn);
};

// ==================================================================

class MaterialScript;

enum
{
	EMF_WarpScreen	= (1<<1),
	EMF_SortTrans	= (1<<2),
	EMF_Reflect		= (1<<3),
	EMF_Refract		= (1<<4),
	EMF_Water		= (1<<5),
};

class Material
{
public:
#ifdef TRANSGAMING
	static const int	MaxPasses=2;
#else
	static const int	MaxPasses=4;
#endif

	DWORD				Flags;
	RenderPass			Passes[MaxPasses];
	int					PassCount;
	int					RefCount;

    Material();
};

struct FXEntry
{
	RenderEffect*					FX;
	DynamicArray<RenderTechnique>	Techniques;
};

// ==================================================================

class ENGINE_API MaterialSystem
{
public:
    struct
    {
		int FixedPos;
		int FixedSimple;
		int Fixed_TU2;
        //expects vertex data in the following format on the PSP:
        //      TexCoord2f | Normal3f | Position3f      <-- stream 0
        //expects vertex data in the following format on the PS2:
        //      Position3f      <-- stream 0
        //      Normal3f        <-- stream 1
        //      TexCoord2f      <-- stream 2
		int Fixed_NTU;
#ifdef TRANSGAMING_PS2
        //expects vertex data in the following format:
        //      Position3f      <-- stream 0
        //      Color4ub        <-- stream 1
        //      TexCoord2f      <-- stream 2
        int Fixed_DCTU1;
        //expects vertex data in the following format:
        //      Position3f      <-- stream 0
        //      TexCoord2f      <-- stream 1
        int Fixed_DTU1;
        //expects vertex data in the following format:
        //      Position3f | Color4ub | TexCoord2f | TexCoord2f     <-- stream 0
        //      Normal3f                                            <-- stream 1
        int Fixed_CTU2;
        //expects vertex data in the following format:
        //      Position4f      <-- stream 0
        //      TexCoord2f      <-- stream 1
        int Fixed_4DTU1;
        //expects vertex data in the following format:
        //      Position3f | Color4ub | TexCoord2f | TexCoord2f     <-- stream 0
        //      Normal3f                                            <-- stream 1
        int Fixed_4DCTU1;
        ////expects vertex data in the following format:
        ////      Position4f      <-- stream 0
        ////      Color4ub        <-- stream 1
        ////      TexCoord2f      <-- stream 2
        //int Fixed_4NTU;
#endif
#ifdef TRANSGAMING_KEEP_NORMALS
        //expects vertex data in the following format on the PSP:
        //      Weight1f | TexCoord2f | Color4ub | Normal3f | Vertex3f  <-- stream 0
        //  NOTE: the color specified in this vertex format will be ignored when using
        //    the SCEGU_REPLACE texture application mode.  This vertex format is used
        //    when performing multi-pass, single-data multitexturing.  This format
        //    represents the base texture pass.  The second set of texture coordinates
        //    are actually hidden in the spot where the diffuse color is specified.
        //    In order to render in this format bone matrix 0 must be set to the identity.
        //expects vertex data in the following format on the PS2:
        //      Position4f      <-- stream 0
        //      Normal3f        <-- stream 1
        //      TexCoord2f      <-- stream 2
        //      TexCoord2f      <-- stream 3
        int FixedMT0_TU2;
        //expects vertex data in the following format:
        //      Weight3f | TexCoord2us | Normal3f | Vertex3f  <-- stream 0
        //  NOTE: This vertex format is used when performing multi-pass, single-data 
        //    multitexturing.  This format represents the second texture pass.  The 
        //    first set of texture coordinates are actually hidden in the spot where
        //    bone weights 2 and 3 are specified.  In order to render in this format
        //    bone matrix 0 must be set to the identity.
        //expects vertex data in the following format on the PS2:
        //      Position4f      <-- stream 0
        //      Normal3f        <-- stream 1
        //      TexCoord2f      <-- stream 2
        //      TexCoord2f      <-- stream 3
        //  NOTE: on the PS2 this value will identify the same shader generated by 
        //    the FixedMT0_TU2 shader.
        int FixedMT1_TU2;
#else
        //expects vertex data in the following format on the PSP:
        //      Weight1f | TexCoord2f | Color4ub | Vertex3f  <-- stream 0
        //  NOTE: the color specified in this vertex format will be ignored when using
        //    the SCEGU_REPLACE texture application mode.  This vertex format is used
        //    when performing multi-pass, single-data multitexturing.  This format
        //    represents the base texture pass.  The second set of texture coordinates
        //    are actually hidden in the spot where the diffuse color is specified.
        //    In order to render in this format bone matrix 0 must be set to the identity.
        //expects vertex data in the following format on the PS2:
        //      Position4f      <-- stream 0
        //      TexCoord2f      <-- stream 2
        //      TexCoord2f      <-- stream 3
        int FixedMT0_TU2;
        //expects vertex data in the following format on the PSP:
        //      Weight3f | TexCoord2us | Vertex3f  <-- stream 0
        //  NOTE: This vertex format is used when performing multi-pass, single-data 
        //    multitexturing.  This format represents the second texture pass.  The 
        //    first set of texture coordinates are actually hidden in the spot where
        //    bone weights 2 and 3 are specified.  In order to render in this format
        //    bone matrix 0 must be set to the identity.
        //expects vertex data in the following format on the PS2:
        //      Position4f      <-- stream 0
        //      TexCoord2f      <-- stream 2
        //      TexCoord2f      <-- stream 3
        //  NOTE: on the PS2 this value will identify the same shader generated by 
        //    the FixedMT0_TU2 shader.
        int FixedMT1_TU2;
#endif


		int	VS_NTU;
		int	VS_TU2;

		int SKIN[Entity::MaxInfluences][Entity::MaxLights+1];
    }
    Shaders;

    struct
    {
		Texture*	NoTex;
		Texture*	White;
		Texture*	Gray;
		Texture*	Black;

        Texture*    AttenMap;
        Texture*    IllumMap;
#ifndef TRANSGAMING
		Texture*	NormCube;
#endif
		Texture*	Flashlight;
		Texture*	FlashFalloff;
		Texture*	ShadowFalloff;
    }
    Textures;

#ifndef TRANSGAMING
# define MAX_VERTEX_COMPONENTS	8

	struct
	{
		VertexComponent	PackedNTU[MAX_VERTEX_COMPONENTS];
		VertexComponent	PackedTU2[MAX_VERTEX_COMPONENTS];
	}
	Decls;
#endif

	float		Time;
	bool		FreezeRefs;

	Material*	NullMat;

private:
    String  TexturePath;

    HashMap<String,Texture*>		TexHash;
    HashMap<String,Material*>		MatHash;
	HashMap<String,FXEntry*>		FXHash;
	DynamicArray<FXEntry>			FXList;
	DynamicArray<Texture*>			AniTex;	

	friend class MaterialScript;

public:
    MaterialSystem();
    ~MaterialSystem();

    BOOL        Init();

	void		Tick(float timeDelta);
#ifndef TRANSGAMING
	int			SetTextureLOD();
#endif
	int			ReloadTextures();

	void		SetTexFiltering();

    Material*   CreateMaterial(const char* name);
    Material*   FindMaterial(const char* name) const;
	FXEntry*	FindEffect(const char* name) const;
    void		ReleaseMaterial(Material* material, bool forceRelease=false);

    Texture*    CreateTexture(const char* name, DWORD flags=0, int lodset=0);
	Texture*	ReplaceTexture(const char* oldTexName, const char* newTexName); // temporarily replaces texture with new one, Texture* and hash stays unmodified!!!
	Texture*	CreateEmptyTexture(const char* name, int width, int height, int bpp, DWORD flags=0);
    Texture*    CreateCubeMap(const char* name, DWORD flags=0, int lodset=0);
	Texture*	FindTexture(const char* name) const;
	bool		IsTextureOnDisk(const char *name) const;
#ifndef TRANSGAMING
    void		SetTextureAnimNone(const char* name);
#endif
	void		SetTextureAnimAdvance(const char* name, float fps);
    void		ReleaseTexture(Texture* texture, bool forceRelease=false);
#ifdef DEBUG
    void        EnumerateTextures();
#endif
	void		CleanupFrozenStuff();

	int			CreateShader(const char* name, int type);

#ifndef TRANSGAMING
	void		PreLoadAll() const;
#endif

private:
	void LoadScripts();
	void LoadEffects();
	void ReleaseEffects();
	void FreeTexture(Texture* texture);

#ifdef TRANSGAMING
	bool TextureOnDiskExt(const char *name, char *retname, size_t len) const;
	bool TextureOnDisk(const char *name, char *retname, size_t len) const;
#else
	bool TextureOnDiskExt(const char *name, String& retname) const;
	bool TextureOnDisk(const char *name, String& retname) const;
#endif

	void CreateWarpTexture();

    int  LoadShader(const char* fileName, 
                    int type = RenderDevice::EShaderVertex, 
                    StreamDef* streams = 0);

	RenderEffect* LoadEffect(const char* FileName);
};

class MaterialScript
{
public:
	MaterialScript();
    MaterialScript(MaterialSystem* matSys);
    ~MaterialScript();

	BOOL Load(const char* fileName);
	BOOL ParseMaterial();
	BOOL ParsePassToken(RenderPass* pass, const char* token);

	int		ParseTexArg();
	int		ParseTexOp();
	int		ParseTexGen(const char* token);
	int		ParseTexAddr(const char* token);
	int		ParseTexFilter(const char* token);
	int		ParseCompareFunc();
	int		ParseStencilOp();
	int		ParseCullMode();
	int		ParseFillMode();
	int		ParseBlendMode();

	void	Rewind();

	void Parse();
	BOOL EndToken();
	virtual BOOL NextToken();

#ifdef TRANSGAMING
    char *  strBuf;
    size_t  strBufLen;
#else
	String	StrBuf;
#endif

	int  CurPos;
	int	 LastToken;
	char LastDelim;
	char CurDelim;
	char CurToken[256];
	int	 CharNum;
	bool InQuote;

	MaterialSystem*	MaterialSys;
};

// external parsing 
class TokenParser : public MaterialScript
{
public:
	TokenParser(const char* token) : MaterialScript(NULL) {
		strncpy(CurToken, token, sizeof(CurToken)-1);
	}

	BOOL NextToken() { 
		return TRUE;
	}
};



#endif //_MATERIAL_

