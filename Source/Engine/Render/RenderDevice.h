
// #pragma once
#ifndef _RENDERDEVICE_
#define _RENDERDEVICE_


class Viewport;
class Texture;
class LogBuffer;

#include "Core/Math/LeMath.h"

#ifdef	TRANSGAMING

class Color_TG{
public:
    union{
	    DWORD	dwVal;
	    BYTE	C[4];
        struct
        {
            BYTE R,G,B,A;
        };
    };

	Color_TG(const DWORD val){
        dwVal = val;
    }

    Color_TG(const float* val) // init from float*
    {
	    R = ((BYTE)(val[0]*255.f));
	    G = ((BYTE)(val[1]*255.f));
	    B = ((BYTE)(val[2]*255.f));
	    A = ((BYTE)(val[3]*255.f));
    }

    Color_TG(const float r, const float g, const float b, const float a=1.0f) // init from floats
    {
    	R = ((BYTE)(r*255.f));
	    G = ((BYTE)(g*255.f));
	    B = ((BYTE)(b*255.f));
	    A = ((BYTE)(a*255.f));
    }

	Color_TG(const BYTE r, const BYTE g, const BYTE b, const BYTE a){
        R = r;
        G = g;
        B = b;
        A = a;
    }

	Color_TG(const int r, const int g, const int b, const int a){
        R = r;
        G = g;
        B = b;
        A = a;
    }

	operator DWORD () const
	{
		return dwVal;
	}

	operator Plane () const
	{
		const float inv = 1/255.f;
		return Plane(R*inv, G*inv, B*inv, A*inv);
	}

	operator Vector () const
	{
		const float inv = 1/255.f;
		return Vector(R*inv, G*inv, B*inv);
	}
    // TODO: pack/unpack methods
};

# define    tInv    (1/255.f)
# define    COLOR2PLANE(plane, tColor )     plane.set(tColor.R*tInv, tColor.G*tInv, tColor.B*tInv, tColor.A*tInv)

#else

union Color
{
	DWORD	dwVal;
	BYTE	C[4];
    struct
    {
        BYTE B,G,R,A;
    };

	Color(const DWORD val)
	:	dwVal(val)
	{}

    Color(const float* val) // init from float*
	:	R((BYTE)(val[0]*255.f))
	,	G((BYTE)(val[1]*255.f))
	,	B((BYTE)(val[2]*255.f))
	,	A((BYTE)(val[3]*255.f))
    {}

    Color(const float r, const float g, const float b, const float a=1.0f) // init from floats
	:	R((BYTE)(r*255.f))
	,	G((BYTE)(g*255.f))
	,	B((BYTE)(b*255.f))
	,	A((BYTE)(a*255.f))
    {}

	Color(const BYTE r, const BYTE g, const BYTE b, const BYTE a)
	:	dwVal((a<<24)|(r<<16)|(g<<8)|b)
	{}

	Color(const int r, const int g, const int b, const int a)
	:	dwVal((a<<24)|(r<<16)|(g<<8)|b)
	{}

	operator DWORD () const
	{
		return dwVal;
	}

	operator Plane () const
	{
		const float inv = 1/255.f;
		return Plane(R*inv, G*inv, B*inv, A*inv);
	}

	operator Vector () const
	{
		const float inv = 1/255.f;
		return Vector(R*inv, G*inv, B*inv);
	}
    // TODO: pack/unpack methods
};

#endif

// =================================================================

class RenderColor
{
public:
    RenderColor() {}
    RenderColor(const DWORD ub4) : UB4(ub4) {}

    RenderColor(const FLOAT* fv)
    :   B((BYTE)(fv[2] * 255.0f))
    ,   G((BYTE)(fv[1] * 255.0f))
    ,   R((BYTE)(fv[0] * 255.0f))
    ,   A((BYTE)(fv[3] * 255.0f))
    {}

    RenderColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a = 1.0f)
    :   B((BYTE)(b * 255.0f))
    ,   G((BYTE)(g * 255.0f))
    ,   R((BYTE)(r * 255.0f))
    ,   A((BYTE)(a * 255.0f))
    {}

    operator DWORD () const { return UB4; }

private:

    union {
        DWORD   UB4;
        BYTE    UB[4];

        struct { 
#ifdef TRANSGAMING
            BYTE R,G,B,A; 
#else
            BYTE B,G,R,A; 
#endif
        };
    };
};

#define MAX_TEXTURE_STAGES	(16)

struct TextureStageState
{
	enum ETexOp
	{
		ETexOp_Disabled,
		ETexOp_Arg1,
		ETexOp_Arg2,
		ETexOp_Modulate,
		ETexOp_Modulate_x2,
		ETexOp_Modulate_x4,
		ETexOp_Add,
		ETexOp_Add_Signed,
		ETexOp_Add_Signed_x2,
		ETexOp_Substract,
		ETexOp_Dot3,
	};

	enum ETexArg
	{
		ETexArg_Current,
		ETexArg_Texel,
		ETexArg_Color,
		ETexArg_Diffuse,
	};

	enum ETexFilter
	{
		ETexFilter_None,
		ETexFilter_Point,
		ETexFilter_Linear,
        ETexFilter_Anisotropic,
	};

	enum ETexAddr
	{
		ETexAddr_Wrap,
		ETexAddr_Clamp,
		ETexAddr_Mirror,
        ETexAddr_MirrorOnce,
        ETexAddr_Border,
	};

	enum ETexCoord
	{
		ETexCoord_Index,
		ETexCoord_CamNormal,
		ETexCoord_CamPos,
		ETexCoord_CamReflection,
		ETexCoord_SphereMap,
	};

	enum ETexTransform
	{
		ETexTransform_Disable,
		ETexTransform_1,
		ETexTransform_2,
		ETexTransform_3,
		ETexTransform_4,

		// additional bits
		ETexTransform_Project	= 0x80,
	};

	enum
	{
		ETSS_SetNothing,
		ETSS_SetColorOp,
		ETSS_SetColorArg,
		ETSS_SetAlphaOp,
		ETSS_SetAlphaArg,
		ETSS_SetFilter,
		ETSS_SetAddr,
		ETSS_SetCoord,
		ETSS_SetTransform,
		ETSS_SetTexMatrix,
		ETSS_SetConstColor,
		ETSS_SetTexture,
		ETSS_SetBumpEnvMatrix,
	};

	struct Stage
	{
		Stage() : Num(0) {}

		int	Num;
		int Sets[32];

		ETexOp			ColorOp;
		ETexArg			ColorArg1;
		ETexArg			ColorArg2;
		ETexArg			ColorArg3;
		ETexOp			AlphaOp;
		ETexArg			AlphaArg1;
		ETexArg			AlphaArg2;
		ETexArg			AlphaArg3;
		ETexFilter		MinFilter;
		ETexFilter		MagFilter;
		ETexFilter		MipFilter;
		ETexAddr		AddrU;
		ETexAddr		AddrV;
		ETexAddr		AddrW;
		ETexCoord		Coord;
		int				CoordIndex;
		ETexTransform	Transform;
		Matrix			TexMatrix;
		Matrix			BumpEnvMatrix;
		RenderColor		ConstColor;
		DWORD			TexId;
	};

	Stage	Stages[MAX_TEXTURE_STAGES];
	Stage*	ST;

	TextureStageState() { ST = &Stages[0]; }

	inline void ActiveStage(const int stage) { ST = &Stages[stage]; }

	inline void SetColorOp(const ETexOp op);
	inline void SetColorArg(const ETexArg arg1, const ETexArg arg2 = ETexArg_Current);
	inline void SetAlphaOp(const ETexOp op);
	inline void SetAlphaArg(const ETexArg arg1, const ETexArg arg2 = ETexArg_Current);
	inline void SetFilter(const ETexFilter min, const ETexFilter mag, const ETexFilter mip = ETexFilter_None);
	inline void SetAddr(const ETexAddr u, const ETexAddr v, const ETexAddr w = ETexAddr_Wrap);
	inline void SetCoord(const int coord, const ETexCoord tc = ETexCoord_Index);
    inline void SetTransform(const ETexTransform tt);
    inline void SetTexMatrix(const Matrix& m);
	inline void SetBumpEnvMatrix(const Matrix& m);
    inline void SetConstColor(const RenderColor& cc);
    inline void SetTexture(DWORD texId);
};

inline void TextureStageState::SetColorOp(const ETexOp op) 
{ 
	ST->ColorOp = op;
	ST->Sets[ST->Num++] = ETSS_SetColorOp;
}

inline void TextureStageState::SetColorArg(const ETexArg arg1, const ETexArg arg2)
{
	ST->ColorArg1 = arg1;
	ST->ColorArg2 = arg2;

	ST->Sets[ST->Num++] = ETSS_SetColorArg;
}

inline void TextureStageState::SetAlphaOp(const ETexOp op) 
{ 
	ST->AlphaOp = op;
	ST->Sets[ST->Num++] = ETSS_SetAlphaOp;
}

inline void TextureStageState::SetAlphaArg(const ETexArg arg1, const ETexArg arg2)
{
	ST->AlphaArg1 = arg1;
	ST->AlphaArg2 = arg2;

	ST->Sets[ST->Num++] = ETSS_SetAlphaArg;
}

inline void TextureStageState::SetFilter(const ETexFilter min, const ETexFilter mag, const ETexFilter mip)
{
	ST->MinFilter = min;
	ST->MagFilter = mag;
	ST->MipFilter = mip;

	ST->Sets[ST->Num++] = ETSS_SetFilter;
}

inline void TextureStageState::SetAddr(const ETexAddr u, const ETexAddr v, const ETexAddr w)
{
	ST->AddrU = u;
	ST->AddrV = v;
	ST->AddrW = w;

	ST->Sets[ST->Num++] = ETSS_SetAddr;
}

inline void TextureStageState::SetCoord(const int index, const ETexCoord tc)
{
	ST->CoordIndex = index;
	ST->Coord = tc;

	ST->Sets[ST->Num++] = ETSS_SetCoord;
}

inline void TextureStageState::SetTransform(const ETexTransform tt)
{
    ST->Transform = tt;
    ST->Sets[ST->Num++] = ETSS_SetTransform;
}

inline void TextureStageState::SetTexMatrix(const Matrix& m)
{
    ST->TexMatrix = m;
    ST->Sets[ST->Num++] = ETSS_SetTexMatrix;
}

inline void TextureStageState::SetBumpEnvMatrix(const Matrix& m)
{
    ST->BumpEnvMatrix = m;
    ST->Sets[ST->Num++] = ETSS_SetBumpEnvMatrix;
}

inline void TextureStageState::SetConstColor(const RenderColor& cc)
{
    ST->ConstColor = cc;
    ST->Sets[ST->Num++] = ETSS_SetConstColor;
}

inline void TextureStageState::SetTexture(DWORD texId)
{
    ST->TexId = texId;
    ST->Sets[ST->Num++] = ETSS_SetTexture;
}

// =================================================================

struct TextureState
{
	enum // tex/alpha op
	{
		ETexOpDisable			= 1,
		ETexOpTexture			= 2,    // use texture only
        ETexOpDiffuse   		= 3,    // use diffuse only (DOES THAT WORK?)
		ETexOpColor				= 4,    // use const color
		ETexOpModulate			= 5,    // modulate texture with arg
		ETexOpModulate2x		= 6,	// modulatex2 texture with arg
		ETexOpAdd				= 7,    // add texture with arg
		ETexOpAddSigned			= 8,    // addsigned texture with arg
		ETexOpAddSigned2x		= 9,    // addsignedx2 texture with arg
		ETexOpBlendDiffuse		=10,	// Linearly blend this texture stage, using the interpolated alpha from each vertex 
		ETexOpBlendTexture		=11,	// Linearly blend this texture stage, using alpha from this stage's texture 
		ETexOpBlendColor		=12,	// Linearly blend this texture stage, using alpha from a scalar alpha  
		ETexOpBlendPrevious		=13,	// Linearly blend this texture stage, using alpha taken from the previous texture stage  
		ETexOpModulateAlphaAdd	=14,	// Modulate the color of the second argument, using the alpha of the first argument; then add the result to argument one. 
		ETexOpPreviousModulate	=15,	// Modulate the color of the second argument, using the alpha of the first argument; then add the result to argument one. 
	};

	enum // tex/alpha arg
	{
        ETexArgPrevious 	= 1,    // previous stage
		ETexArgDiffuse		= 2,
		ETexArgColor		= 3,
		ETexArgTexture		= 4,
		ETexArgPrevious_AR	= 5,	// previous, alpha replicate
	};

	enum // tex filter
	{
		ETexFilterPoint				= 1,
		ETexFilterBilinearNoMips	= 2,
		ETexFilterBilinear			= 3,
		ETexFilterTrilinear			= 4,
		ETexFilterAnisotropic		= 5,
	};

	enum // tex addressing/tex coord index
	{
        ETexAddrWrap        = 1,
        ETexAddrClamp       = 2,
        ETexAddrMirror      = 3,
	};

    enum    // transform
    {
        ETexTransformNone	= 1,
		ETexTransform1		= 2,
		ETexTransform2		= 3,
		ETexTransform3		= 4,
		ETexTransform4		= 5,
        ETexTransformProj   = 0x20,
    };

	enum
	{
		ETexCoordPassThru			= 0,
		ETexCoordWorldSpace			= 0x08,
		ETexCoordWorldOrient		= 0x10,
		ETexCoordCameraSpaceNormal	= 0x20,
		ETexCoordCameraSpacePos		= 0x40,
		ETexCoordCameraSpaceReflect	= 0x80,
	};

    union
    {
		struct
		{
			union
			{
				struct
				{
					BYTE    Op;
					BYTE    Arg;
					BYTE	AlphaOp;
					BYTE	AlphaArg;
				};
				DWORD	V1;
			};

			union
			{
				struct
				{
					BYTE    Filter;
					BYTE	AddrMode;
					BYTE    Transform;
					BYTE	CoordIndex;	// +1
				};
				DWORD	V2;
			};
		};

        QWORD   QVal;
    };

	// texture xform
	enum 
	{
		EXformNone		= 0,	// vs does not use texture xform 
		EXformIdentity	= 1,	// vs uses texture xform, but set it to identity
		EXformUseShader	= 2,	// use texture xform defined in shader
		EXformInBlend	= 3,	// use texture const xform for terrain shader's colormap
		EXformInAlpha	= 4,	// use texture const xform for terrain shader's blendmap
		EXformInDetail	= 5,	// use texture const xform for detail texture shader
	};

	enum 
	{
		EXformUseMat	= 1<<0,	// xform uses explicit mat
	};

    int     Stage;
    int     TexId;
    Matrix* BumpPtr;
	float	TileU, TileV;
	float	PanU, PanV;
	Matrix	Mat;

	float	Rot;
	Matrix	MatStore;
	int		Xform;
	int		Flags;

    TextureState(int stage=0) 
    :   QVal(0), Stage(stage), BumpPtr(0), TileU(1), TileV(1), PanU(0), PanV(0), Rot(0)
	,	Xform(EXformNone), Flags(0), TexId(0)
    {
        MatStore.SetIdentity();
        Mat.SetIdentity();
    }
    TextureState(int stage, int texId) 
    :   QVal(0), Stage(stage), BumpPtr(0), TileU(1), TileV(1), PanU(0), PanV(0), Rot(0)
	,	Xform(EXformNone), Flags(0), TexId(texId)
    {
        MatStore.SetIdentity();
        Mat.SetIdentity();    
    }

	inline void TickMatStore(float time=0)
	{
		if (Xform != EXformNone)
		{
			MatStore.SetIdentity(); // = GIdentityMatrix;

			if (Flags & EXformUseMat)
				MatStore = Mat;
			
			if (Rot)
				MatStore *= TextureRotMatrix(Rot*time);
			if (PanU!=0 || PanV!=0)
				MatStore *= TexturePanMatrix(PanU*time,PanV*time);
			if (TileU!=1 || TileV!=1)
				MatStore *= ScaleMatrix(Vector(TileU,TileV,1));

			MatStore.Transpose();
		}
	}

    inline void Set(int stage, BYTE op, BYTE arg=0, BYTE filter=0, BYTE addr=0)
    {
        Stage		= stage;
        Op			= op;
        Arg			= arg;
        Filter		= filter;
		AddrMode	= addr;
    }

	inline void SetTexGen(const int mode)
	{
		CoordIndex = (BYTE) ((Stage+1)|mode);
	}

	inline void SetCoordIndex(const int index)
	{
		CoordIndex = (BYTE) (index + 1);
	}
};

// =================================================================

struct FogState
{
	enum
	{
		EFogNone,
		EFogExp,
		EFogExp2,
		EFogLinear,
	};

	BYTE	Mode;
	float	Start;
	float	End;
	float	Density;
	DWORD	Color;
	float	VolStart;
	float	VolEnd;
	DWORD	VolColor;

	FogState() 
		: Mode(EFogNone), Start(0), End(50.0f), Density(0), Color(0xFFFFFF), VolStart(0), VolEnd(50.0f), VolColor(0xFFFFFF) {}
    FogState(BYTE mode) 
		: Mode(mode), Start(0), End(50.0f), Density(0), Color(0xFFFFFF), VolStart(0), VolEnd(50.0f), VolColor(0xFFFFFF) {}
	FogState UseVSFog() const;
};

// =================================================================

struct LightState
{
    enum
    {
        ELightOff           = 0,        // turn the light off
        ELightPoint         = 1,
        ELightDirectional   = 2,
        ELightSpot          = 3,
    };
    
    BYTE    Flags;
    Vector  Pos;  
	Vector	Dir;
    Vector  Specular;
    Vector  Diffuse;
	Vector	Ambient;
    float   Radius;

    LightState() {}
    LightState(BYTE flags) : Flags(flags) {}
};

// =================================================================

class RenderState
{
	friend class D3DRenderDevice;
	friend class VideoSystemPS2;
	friend class VideoSystemPSP;

public:
    enum // depth/alpha/stencil func
    {
        EFuncNever          = 0,
        EFuncLess           = 1,
        EFuncEqual          = 2,
        EFuncLessEqual      = 3,
        EFuncGreater        = 4,
        EFuncNotEqual       = 5,
        EFuncGreaterEqual   = 6,
        EFuncAlways         = 7,
    };

    enum // stencil ops
    {
        EStencilOpKeep      = 0,
        EStencilOpZero      = 1,
        EStencilOpReplace   = 2,
        EStencilOpIncrSat   = 3,
        EStencilOpDecrSat   = 4,
        EStencilOpInvert    = 5,
        EStencilOpIncr      = 6,
        EStencilOpDecr      = 7,
    };

    enum // blend modes
    {
        EBlendNone          	= 0,
        EBlendAlpha         	= 1,
        EBlendAdd           	= 2,
        EBlendModulate      	= 3,
		EBlendFilter			= 4,
        EBlendTranslucent   	= 5,
		EBlendInvModulate		= 6,
		EBlendSubtract			= 7,
		EBlendRevSubtract		= 8,
		EBlendDestTranslucent	= 9,
		EBlendDestAlpha			= 10,
        EBlendModulate2x      	= 11,
    };

    enum // fill modes
    {
        EFillSolid      = 0,
        EFillLines      = 1,
        EFillPoints     = 2,
    };

    enum // cull modes
    {
        ECullNone       = 0,
        ECullCCW        = 1,
        ECullCW         = 2,
    };

protected:
    BYTE    DepthTest:1;
    BYTE    DepthWrite:1;
    BYTE    ColorWrite:1;
    BYTE    AlphaWrite:1;
    BYTE    StencilTest:1;
    BYTE    AlphaTest:1;
	BYTE	ScissorTest:1;
    BYTE    Lighting:1;
    BYTE    NormalizeNormals:1;
	BYTE	SpecularEnable:1;

    BYTE    DepthFunc;
    BYTE    AlphaFunc;
    BYTE    AlphaRef;
	BYTE	StencilFunc;
	BYTE	StencilRef;
	BYTE	StencilMask;
    BYTE    StencilFailOp;
    BYTE    StencilZFailOp;
    BYTE    StencilPassOp;
    BYTE    BlendMode;
    BYTE    CullMode;
    BYTE    FillMode;

    DWORD   ConstColor;
	DWORD	AmbientColor;
	DWORD	FogColor;	// must not be set

public:
    RenderState() 
	:	DepthTest(1),DepthWrite(1),ColorWrite(1),AlphaWrite(0),StencilTest(0),AlphaTest(0)
	,	ScissorTest(0), Lighting(0),NormalizeNormals(1),SpecularEnable(0)
	,	DepthFunc(EFuncLessEqual),AlphaFunc(EFuncAlways),AlphaRef(0)
	,	StencilFunc(EFuncAlways),StencilRef(0),StencilMask(0xFF),StencilFailOp(EStencilOpKeep),StencilZFailOp(EStencilOpKeep),StencilPassOp(EStencilOpKeep)
	,	BlendMode(EBlendNone),CullMode(ECullCCW),FillMode(EFillSolid)
	,	ConstColor(0), AmbientColor(0), FogColor(0)
	{}

	RenderState& operator=(const RenderState& state)
	{
		GMemCopy(this,&state,sizeof(RenderState));
		return (*this);
	}

    inline void Reset() { *this = RenderState(); }
#ifndef TRANSGAMING
	void DumpState(LogBuffer& MLog) const;
#endif
    // util
    inline void SetDepthTest(bool val)  		{ DepthTest=val;		}
    inline void SetDepthWrite(bool val) 		{ DepthWrite=val;		}
    inline void SetColorWrite(bool val) 		{ ColorWrite=val;		}
    inline void SetAlphaWrite(bool val) 		{ AlphaWrite=val;		}
	inline void SetStencilTest(bool val)		{ StencilTest=val; 	    }
    inline void SetAlphaTest(bool val)  		{ AlphaTest=val;        }
	inline void SetScissorTest(bool val)		{ ScissorTest=val;		}
    inline void SetLighting(bool val)			{ Lighting=val;			}
	inline void SetNormalizeNormals(bool val)	{ NormalizeNormals=val;	}
	inline void SetSpecularEnable(bool val)		{ SpecularEnable=val;	}

	inline void SetDepthFunc(int func)		{ DepthFunc=BYTE(func); }
	inline void SetAlphaFunc(BYTE func)		{ AlphaFunc=func;		}
    inline void SetAlphaRef(BYTE ref)		{ AlphaRef=ref;			}

	inline void SetStencilFunc(BYTE func)	{ StencilFunc=func;		}
	inline void SetStencilRef(BYTE ref)		{ StencilRef=ref; 		}
	inline void SetStencilMask(BYTE mask)	{ StencilMask=mask;		}
	inline void SetStencilFailOp(BYTE op)	{ StencilFailOp=op;		}
	inline void SetStencilZFailOp(BYTE op)	{ StencilZFailOp=op;	}
	inline void SetStencilPassOp(BYTE op)	{ StencilPassOp=op;		}

	inline void SetBlendMode(int mode) 	{ BlendMode=BYTE(mode); }
    inline void SetCullMode (int mode) 	{ CullMode =BYTE(mode); }
	inline void SetFillMode (int mode) 	{ FillMode =BYTE(mode); }

	inline void SetConstColor(DWORD val)  	{ ConstColor=val;		}
    inline void SetAmbientColor(DWORD val)	{ AmbientColor=val;		}

    inline bool GetDepthTest()  		{ return DepthTest;			}
    inline bool GetDepthWrite() 		{ return DepthWrite;		}
    inline bool GetColorWrite() 		{ return ColorWrite;		}
    inline bool GetAlphaWrite() 		{ return AlphaWrite;		}
	inline bool GetStencilTest()		{ return StencilTest; 	    }
    inline bool GetAlphaTest()  		{ return AlphaTest;			}
	inline bool GetScissorTest()		{ return ScissorTest;		}
    inline bool GetLighting()			{ return Lighting;			}
	inline bool GetNormalizeNormals()	{ return NormalizeNormals;	}
	inline bool GetSpecularEnable()		{ return SpecularEnable;	}

	inline int GetDepthFunc()			{ return DepthFunc;		}
	inline BYTE GetAlphaFunc()			{ return AlphaFunc;		}
    inline BYTE GetAlphaRef()			{ return AlphaRef;		}

	inline BYTE GetStencilFunc()		{ return StencilFunc;	}
	inline BYTE GetStencilRef()			{ return StencilRef; 	}
	inline BYTE GetStencilMask()		{ return StencilMask;	}
	inline BYTE GetStencilFailOp()		{ return StencilFailOp;	}
	inline BYTE GetStencilZFailOp()		{ return StencilZFailOp;}
	inline BYTE GetStencilPassOp()		{ return StencilPassOp;	}

	inline int GetBlendMode() 			{ return BlendMode;		}
    inline int GetCullMode () 			{ return CullMode;		}
	inline int GetFillMode () 			{ return FillMode;		}

	inline DWORD GetConstColor()  		{ return ConstColor;	}
    inline DWORD GetAmbientColor()		{ return AmbientColor;	}
};

// =================================================================

class RenderStateHUD : public RenderState
{
public:
	RenderStateHUD()
	{
		DepthTest = 0;
		DepthWrite = 0;
	}
};

#define MAX_DISPLAY_MODES	(32)
#define MAX_REFRESH_RATES	(8)

struct RenderDisplayMode // only 32bpp
{
	UINT	XRes;
	UINT	YRes;

	UINT	RefreshRates[MAX_REFRESH_RATES];
	int		RefreshRateNum;
};

// =================================================================

struct EffectPass
{
	String			Name;
	const DWORD*	VertShader;
	const DWORD*	FragShader;

	EffectPass() : VertShader(0), FragShader(0) {}
};

#define MAX_EFFECT_PASSES	4

struct RenderTechnique
{
	String	Name;
	UINT	NumPasses;
	DWORD	Index;

	EffectPass	Passes[MAX_EFFECT_PASSES];
};

class RenderEffect
{
public:
	virtual ~RenderEffect() {}

	virtual BOOL	EnumTechniques(DynamicArray<RenderTechnique>& Out) = 0;
	virtual DWORD	GetParamIndex(const char* Name) = 0;
	virtual DWORD	GetTextureIndex(const char* Name) = 0;
	virtual	BOOL	GetTechniqueString(DWORD Technique, const char* Name, String& Value) = 0;

	virtual BOOL	SetParam(const char* Name, const void* Data, DWORD Size) = 0;
	virtual BOOL	SetParam(DWORD Index, const void* Data, DWORD Size) = 0;
	virtual BOOL	SetTexture(const char* Name, Texture* Tex) = 0;
	virtual BOOL	SetTexture(DWORD Index, Texture* Tex) = 0;

	virtual BOOL	SetTechnique(DWORD Index) = 0;
	virtual UINT	Begin() = 0;
	virtual BOOL	BeginPass(UINT Num) = 0;
	virtual BOOL	EndPass() = 0;
	virtual BOOL	End() = 0;
};

// =================================================================

#ifndef TRANSGAMING
struct VertexComponent
{
	BYTE	Stream;
	BYTE	Offset;
	BYTE	Type;
	BYTE	Usage;

	VertexComponent() : Type(0) {}
	VertexComponent(BYTE InStream, BYTE InOffset, BYTE InType, BYTE InUsage)
	:	Stream(InStream), Offset(InOffset), Type(InType), Usage(InUsage)
	{}
};
#endif

// =================================================================

class RenderDevice
{
public:

	enum
	{
		EHWClassUnsupported	= 0,
		EHWClassGenericTnL	= 1,	// TnL, no shaders
		EHWClassNV20		= 2,	// vs_1_1, ps_1_1
		EHWClassNV25		= 3,	// vs_1_1, ps_1_3
		EHWClassR200		= 4,	// vs_1_1, ps_1_4
		EHWClassNV30		= 5,	// vs_2_0, ps_2_0
		EHWClassNV40		= 6,
	};

    // vertex type
	enum
	{
		ETexCountShift	= 8,
		ETexCountMask	= 0x00000f00,

        // bit shift and mask for the texture coordinate type in a vertex format bit
        // flag.  The type is embedded as an enum in the bit flag.  The default texture
        // coordinate format is EVertexTUFloat if none is explicitly specified.  Unlike
        // with the bone weights these type flags do not determine the presence of 
        // texture coordinates in the vertex format.  The EVertexTU* bits determine
        // that instead.  EVertexTUNone does not need to be specified if texture
        // coordinates do not exist in a vertex format; it only exists for completeness.
        // However, explicitly specifying EVertexTUNone will override the texture count
        // and create a vertex format with NO texture coordinates.
        ETexTypeShift   = 12,
        ETexTypeMask    = 0x00003000,

        // bit shift and mask for the bone weight count in a vertex format bit flag.
        // The count itself is embedded in the bit flag as <weightCount> - 1.  The
        // presence of bone weights is determined by the bone type bits.  1-8 bone
        // weights are supported in a single vertex format.
        EBoneCountShift = 16,
        EBoneCountMask  = 0x00070000,

        // bit shift and mask for the bone weight type in a vertex format bit flag.
        // The type is embedded as an enum in the bit flag.  Specifying a weight type
        // other than EVertexBoneNone will also expect a weight count (default is 1
        // weight if no EVertexBone* flag is explicitly specified).
        EBoneTypeShift  = 19,
        EBoneTypeMask   = 0x00180000,
	};

    enum 
    {   
        EStreamNone         = 0,    // non FVF stream
	    
        // unused (bit 0)

        // basic single component types (bits 1-5)
	    EVertexPos          = (1 << 1),
	    EVertexDiffuse      = (1 << 2),
	    EVertexNormal       = (1 << 3),
	    EVertexPos4         = (1 << 4),	//PS2 specific as EVertexPos, but has 4 floats

        // unused (bits 6-7)

        // texture coordinate set counts (bits 8-11)
        // eric-> These values were incorrect.  From the way they are used, they actually 
        //   identified 1, 2, 4, 8, 0, 0, 0, or 0 sets of texture coordinate instead
        //   of the intended 1-8 sets.  These have been corrected to specify numerical
        //   values instead of single bits.  The dependency on the value of ETexCountShift
        //   has also been removed.
	    EVertexTU1          = (1 << (ETexCountShift)),
	    EVertexTU2          = (2 << (ETexCountShift)),
	    EVertexTU3          = (3 << (ETexCountShift)),
	    EVertexTU4          = (4 << (ETexCountShift)),
	    EVertexTU5          = (5 << (ETexCountShift)),
	    EVertexTU6          = (6 << (ETexCountShift)),
	    EVertexTU7          = (7 << (ETexCountShift)),
	    EVertexTU8          = (8 << (ETexCountShift)),

        // texture coordinate type (bits 12-13)
        EVertexTUFloat      = (0 << ETexTypeShift),
        EVertexTUUByte      = (1 << ETexTypeShift),
        EVertexTUUShort     = (2 << ETexTypeShift),
        EVertexTUNone       = (3 << ETexTypeShift),

        // unused (bits 14-15)

        // bone weight counts (bits 16-18)
        EVertexBone1        = (0 << EBoneCountShift),
        EVertexBone2        = (1 << EBoneCountShift),
        EVertexBone3        = (2 << EBoneCountShift),
        EVertexBone4        = (3 << EBoneCountShift),
        EVertexBone5        = (4 << EBoneCountShift),
        EVertexBone6        = (5 << EBoneCountShift),
        EVertexBone7        = (6 << EBoneCountShift),
        EVertexBone8        = (7 << EBoneCountShift),

        // bone type names (bits 19-20)
        EVertexBoneNone     = (0 << EBoneTypeShift),
        EVertexBoneFloat    = (1 << EBoneTypeShift),
        EVertexBoneUByte    = (2 << EBoneTypeShift),
        EVertexBoneUShort   = (3 << EBoneTypeShift),

        // unused (bits 21-23)

        // index buffer component types (bits 24-25)
	    EIndex16            = (1 << 24),
	    EIndex32            = (1 << 25),

        // unused (bits 26-27)

        // special treatment flags (bits 28-29)
        EVertexForceClip    = (1 << 28),
        EVertexMultitexture = (1 << 29),

        // unused (bit 30)

		// stream's usage 
		EUsageSWVP			= (1 << 31),
    };

	//	shader component types
	//  WARNING!!! order is important -- see D3Dev.cpp
	enum
	{
		ERegFloat1			= 1,
		ERegFloat2			= 2,
		ERegFloat3			= 3,
		ERegFloat4			= 4,
		ERegColor			= 5,	// 4D packed unsigned bytes mapped to 0. to 1. range
		ERegUByte4			= 6,	// 4D unsigned byte
		ERegShort2			= 7,	// Normalized, 2-D signed short, expanded to (first byte/32767.0, second byte/32767.0, 0, 1)
		ERegShort4			= 8,	// Normalized, 4-D signed short, expanded to (first byte/32767.0, second byte/32767.0, third byte/32767.0, fourth byte/32767.0)
		ERegShort2N			= 9,	// Normalized, 2-D signed short, expanded to (first byte/32767.0, second byte/32767.0, 0, 1)
		ERegShort4N			= 10,	// Normalized, 4-D signed short, expanded to (first byte/32767.0, second byte/32767.0, third byte/32767.0, fourth byte/32767.0)
		ERegUShort2N		= 11,	// Normalized, 2-D unsigned short, expanded to (first byte/65535.0, second byte/65535.0, 0, 1)
		ERegUShort4N		= 12,	// Normalized 4-D unsigned short, expanded to (first byte/65535.0, second byte/65535.0, third byte/65535.0, fourth byte/65535.0)
		ERegUByte4N			= 13,	// Normalized, 4-D unsigned byte. Each of the four bytes is normalized by dividing to 255.0
		ERegUDec3			= 14,	// 3-D unsigned 10 10 10 format expanded to (value, value, value, 1)
		ERegDec3N			= 15,	// 3-D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
	};

	#define SetShaderComponent(x,c)	(((c)&0xf)<<(4*(x)))
	#define GetShaderComponent(x,a)	(((a)>>(4*(x)))&0xf)

	// vertex component usage
	enum
	{
		EUsagePosition,
		EUsageNormal,
		EUsageTangent,
		EUsageColor,
		EUsageTexCoord0,
		EUsageTexCoord1,
		EUsageTexCoord2,
		EUsageTexCoord3,
		EUsageTexCoord4,
		EUsageTexCoord5,
		EUsageTexCoord6,
		EUsageTexCoord7,
	};

    // composite/known vertex formats
    enum
    {
#ifndef TRANSGAMING
        EMeshVertex         = (EVertexPos|EVertexDiffuse|EVertexTU2),
        EMeshVertexNTU      = (EVertexPos|EVertexNormal|EVertexTU1),
#endif
        ESimpleVertex       = (EVertexPos|EVertexDiffuse|EVertexTU1),
        EScreenVertex       = ESimpleVertex,
#ifdef TRANSGAMING
# ifdef TRANSGAMING_PSP
        ERenderVertexN1     = (EVertexPos|EVertexNormal|EVertexTU1),
# endif
        //ERenderVertexNC1    = (EVertexPos|EVertexNormal|EVertexDiffuse|EVertexTU1),
        ERenderVertexNC     = (EVertexPos | EVertexNormal | EVertexDiffuse | EVertexTU2),
# ifdef TRANSGAMING_PS2
        ERenderVertexC2     = (EVertexPos|EVertexDiffuse|EVertexTU2),
# endif
#endif
    };

    enum    // NOTE: the order and values are important!
    {
        EPointList      = 0,
        EPrimTriList    = 1,
        EPrimTriStrip   = 2,
        EPrimTriFan     = 3,
        EPrimLineList   = 4,
        EPrimLineStrip  = 5,
    };

    enum // Lock flags
    {
        ELockNone               = 0,
        ELockNoResize           = (1<<1),   // don't resize backbuffer based on window size
        ELockForceReset         = (1<<2),
    };

	enum
	{
        //basic vertex shader type
        EShaderVertexBase = 1,

        //eric-> these shader type names correspond to the shader used for texture stages 0 through n within
        //a single pass of a material.  If no shader is set for stage <i>, shader 0 will be used instead.
        //There is support for a maximum of 8 texture stages in the engine, each of which can have its own
        //shader now.  Note that EShaderVertex and EShaderVertex0 are pseudonyms for each other.  
        //Be sure to set the shader for EShaderVertex0 (or EShaderVertex) first because doing so will clear
        //out all other vertex shaders.
        //The base shader will specify the 'clip' flag for the entire object.  If it is not set in EShaderVertex0
        //the object will render through the unclipped path.  Otherwise, it will be clipped to the view
        //frustum.
		EShaderVertex	= EShaderVertexBase,
        EShaderVertex0  = EShaderVertexBase,
        EShaderVertex1  = EShaderVertexBase + 1,
        EShaderVertex2  = EShaderVertexBase + 2,
        EShaderVertex3  = EShaderVertexBase + 3,
        EShaderVertex4  = EShaderVertexBase + 4,
        EShaderVertex5  = EShaderVertexBase + 5,
        EShaderVertex6  = EShaderVertexBase + 6,
        EShaderVertex7  = EShaderVertexBase + 7,

		EShaderPixel	= 9,
	};

	enum
	{
		EResTexture	= 1,
		EResBuffer	= 2,
		EResShader  = 3,
	};

    // clear target flags
	enum
	{
		EClearColor		= (1<<0),
		EClearZ			= (1<<1),
		EClearStencil	= (1<<2),

		EClearAll		= (EClearColor|EClearZ|EClearStencil),
	};

	// texture quality sets
	enum 
	{
		ETexLODArchitecture,
		ETexLODWeapons,
		ETexLODModels,
		ETexLODSky,

		ETexLODNumEntries,
	};

	enum
	{
		EMemUnclassified,

		EMemTexDecals,
		EMemTexHUD,
		EMemTexItems,
		EMemTexLevels,
		EMemTexModels,
		EMemTexParticles,
		EMemTexSkies,
		EMemTexSpecial,
		EMemTexWeapons,

		EMemTexEntries,

		EMemTexAvailable,

		EMemVBs,
		EMemIBs,

		EMemNumEntries,
	};

	enum
	{
		ERenderQuery_Occlusion	= 1,
	};

	int	HWClass;
	int LastFrameTris;
	int LastFrameDIPs;
	float TextureMemory[EMemNumEntries];
	int	TexLOD[ETexLODNumEntries];

	enum
	{
		EDevCaps_NonPow2DTex	= (1<<1),
		EDevCaps_NonPow3DTex	= (1<<2),
		EDevCaps_BumpEnvProj	= (1<<3),
	};

	DWORD	DevCaps;

	RenderDisplayMode	DisplayModes[MAX_DISPLAY_MODES];
	int					DisplayModeNum;

public:
	RenderDevice() 
	:	HWClass(EHWClassUnsupported), DisplayModeNum(0), LastFrameTris(0), LastFrameDIPs(0)
	,	DevCaps(0)
	{
		GMemZero(&TexLOD,sizeof(TexLOD));
		for (int i=0; i<EMemNumEntries; i++)
			TextureMemory[i] = 0;
	}

	virtual ~RenderDevice() {}

    // open device (mDeviceInfo should get filled with info)
	virtual BOOL Open(HWND fullScreenHWND = 0) = 0;
	virtual void Close() = 0;

    virtual BOOL SetMode(UINT width, UINT height, BOOL fullScreen=FALSE, int multisampling=0) = 0;

    virtual BOOL SetGamma(float contrast, float gamma, float brightness) = 0;
	
    // lock device for rendering
    virtual BOOL Lock(Viewport& viewport) = 0;

    // unlock (end scene)
	virtual void Unlock() = 0;

	// present the back buffer
	virtual void Flip(HWND hwnd=0) = 0;

    // release all resources
	virtual void Flush() = 0;

	// take screenshot from current FB
	virtual void	TakeScreenshot(const char* filename) = 0;

	// set viewport
	virtual BOOL SetViewport(int x, int y, int w, int h, float zNear=0.0f, float zFar=1.0f) = 0;
	virtual BOOL SetScissor(int x1, int y1, int x2, int y2) = 0;

    virtual BOOL ClearZ(float z = 1.0f) = 0;
    virtual BOOL ClearFB(DWORD argb = 0) = 0;

	// queries
	virtual int		CreateQuery(int Type) = 0;
	virtual BOOL	BeginQuery(int Query) = 0;
	virtual void	EndQuery(int Query) = 0;
	virtual BOOL	GetQueryData(int Query, void* OutData, int Size) = 0;

	// textures
	virtual void	CreateTexture(Texture* tex) = 0;
    virtual void    SetRenderTarget(Texture* colorBuf, Texture* depthBuf, int cubeFace=0) = 0;
	virtual BOOL	SetRenderTarget(int Index, Texture* ColorBuf, int CubeFace = 0) = 0;
	virtual BOOL	SetDepthTarget(Texture* DepthBuf) = 0;
    virtual void    SetFBRenderTarget() = 0;
	virtual BOOL	GrabBackBuffer(Texture* Tex, HWND hwnd) = 0;
	virtual BOOL	GrabZStencilBuffer(Texture* Tex) = 0;
	virtual void	GetBackBufferSize(UINT& SizeX, UINT& SizeY) = 0;
	virtual BYTE*	LockTexture(Texture* tex, int &pitch) = 0;
	virtual void	UnlockTexture(Texture* tex) = 0;
	virtual void	SetTexturePriority(Texture *tex, DWORD priority) = 0;
	virtual void	PreLoadTexture(Texture *tex) = 0;

	// stream interface
	virtual int 	CreateStream(void* sourceData, DWORD type, int count, int stride) = 0;
	virtual BYTE* 	LockStream  (int streamId, int pos, int count) = 0;
	virtual void 	UnlockStream(int streamId) = 0;

#ifndef TRANSGAMING
    // this function is obselete even in the windows version
	virtual int		CreateVertexShader(const DWORD* Source, const VertexComponent* ComponentList) = 0;
#endif
    virtual int     CreateShader(int type, const DWORD* source, const DWORD* streamTypeList, int listCount) = 0;
	virtual void	SetShaderMemory(DWORD addr, const void* data, int count) = 0;
	virtual void	SetPixelShaderMemory(DWORD addr, const void* data, int count) = 0;
	virtual void	SetPixelShaderMemoryB(DWORD addr, const BOOL* data, int count) = 0;
	virtual void	SetShader(int shaderIndex, int shaderType) = 0;

    virtual void    SetBoneMatrix(int index, const Matrix *matrix) = 0;
	
	virtual RenderEffect* CreateEffect(DWORD Flags, const BYTE* EffectData, UINT Size) = 0;

    virtual void    SetState(const RenderState& state) = 0;
    virtual void    SetStreams(const int* streams, int streamCount, int indexStream, int streamOffset=0) = 0;
    virtual void    SetTextureState(const TextureState* stages, int stageCount) = 0;
	virtual void	ResetTextureStages() = 0;
    virtual void    SetFogState(const FogState& state) = 0;
    virtual void    SetFog(bool isFog) = 0;
    virtual void    SetLight(int number, const LightState& state)  = 0;

#ifdef TRANSGAMING_PS2
    virtual void    SetPolygonMode(int mode){}
#endif

#ifdef TRANSGAMING
    // material specular color
    virtual void    SetSpecular(const Vector& color, const float power) = 0;

    // the lighting and ambient in the RenderState is not used, and rewriting
    // the engine to set those properly is not trivial, so I'll be handling
    // the necessary state using these function calls instead.
    virtual void    SetLighting(bool isLighting) = 0;
    virtual void    SetAmbientLight(const Vector& color) = 0;

    // these avoid the overhead of setting up a LightState structure,
    // since the engine's dynamic light system is designed to always provide
    // simple directional lights to the vertex shader, nothing more.
    virtual void    SetNoLight(int number) = 0;
    virtual void    SetDirLight(int number, const Plane& dir, const Vector& color) = 0;

    virtual void 	Draw(const int primType, int base, int count, int vbase=EInvalidIndex, int vcount=EInvalidIndex, int basevindex=0, bool swapPath = false) = 0;
#else
	virtual void 	Draw(const int primType, int base, int count, int vbase=EInvalidIndex, int vcount=EInvalidIndex, int basevindex=0) = 0;
#endif
	virtual void	ReleaseResource(int resourceType, const int* ids, int count) = 0;

	virtual void SetObjectTransform(const Matrix* matPtr) = 0;
	virtual void SetCameraTransform(const Matrix* matPtr) = 0;
	virtual void SetScreenTransform(const Matrix* matPtr) = 0;

	virtual void SetColorWrite(BOOL ColorWrite, BOOL AlphaWrite) = 0;
	virtual void SetCullFlip(BOOL Flip) = 0;
#if defined(TRANSGAMING_PSP) || defined(TRANSGAMING_PS2)
    virtual void SetVBlankState(bool state) = 0;
    virtual bool GetVBlankState() const = 0;
#endif

	// util shortcuts
    inline void SetTexture(int stage, int texId)
    {
    	#ifdef	TRANSGAMING
    	TextureState tmpState = TextureState(stage,texId);
        SetTextureState(&tmpState,1);
    	#else
        SetTextureState(&TextureState(stage,texId),1);
       #endif
    }
    inline void SetColorMode()
    {
        TextureState st[2];
        st[0].Set(0,TextureState::ETexOpColor);
		st[1].Set(1,TextureState::ETexOpDisable);
        SetTextureState(st,ARRAY_COUNT(st));
    }
	inline void SetStream(int stream)
    {
        SetStreams(&stream,1,EInvalidIndex);
    }
    inline void SetIndexedStream(int stream, int indexStream)
    {
        SetStreams(&stream,1,indexStream);
    }

	inline void SetVertexShader(int shaderId)
	{
		SetShader(shaderId, EShaderVertex);
	}

	inline void SetPixelShader(int shaderId)
	{
		SetShader(shaderId, EShaderPixel);
	}

	virtual void SetConstColor(DWORD color) = 0;

};

// =================================================================

class RenderDeviceManager
{
public:
    // device types
    enum {
        ENullRenderDevice,
        ED3DRenderDevice,
    };

    // create device
    static RenderDevice* CreateRenderDevice(int deviceType);
    static void          DestroyRenderDevice(RenderDevice* rdevPtr);

private:
    typedef RenderDevice* (*CreateDeviceProc)();
    typedef void          (*DestroyDeviceProc)(RenderDevice* rdevPtr);

#ifndef TRANSGAMING
    static HMODULE       hDeviceModule;
#endif
};


#endif //_RENDERDEVICE_

