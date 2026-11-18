
// #pragma once
#ifndef _VERTEX_
#define _VERTEX_


//#include "Core/FileSystem/FileManager.h"
//#include "Render/RenderDevice.h"

#ifdef	TRANSGAMING

class BaseTexCoord
{
public:
	union 
	{
		struct { float S, T; };
		struct { float U, V; };
		struct { float X, Y; };
		float  Array[2];
	};

    BOOL operator==(const BaseTexCoord& tu) const
    {
        return U==tu.U && V==tu.V;
    }

    friend FileObject& operator>>(FileObject& file, BaseTexCoord& tu)
	{
		return file >> tu.S >> tu.T;
	}

    friend FileObject& operator<<(FileObject& file, BaseTexCoord& tu)
	{
		return file << tu.S << tu.T;
	}

};

class TexCoord: public BaseTexCoord
{
public:
    TexCoord() {}
    TexCoord(float x, float y)
    {
    	X = x;
    	Y = y;
    }
    TexCoord(const BaseTexCoord& tu)
    {
    	X = tu.X;
    	Y = tu.Y;
    }
};

#else

class ENGINE_API TexCoord
{
public:
	union 
	{
		struct { float S, T; };
		struct { float U, V; };
		struct { float X, Y; };
		float  Array[2];
	};

    TexCoord() {}
    TexCoord(float x, float y) : X(x), Y(y) {}

    BOOL operator==(const TexCoord& tu) const
    {
        return U==tu.U && V==tu.V;
    }

    friend FileObject& operator>>(FileObject& file, TexCoord& tu)
	{
		return file >> tu.S >> tu.T;
	}

    friend FileObject& operator<<(FileObject& file, TexCoord& tu)
	{
		return file << tu.S << tu.T;
	}
};

#endif	//TRANSGAMING

/*
struct ENGINE_API SimpleVertex
{
    enum 
    { 
        ShaderStream    = SetShaderComponent(0,RenderDevice::ERegFloat4)
                        | SetShaderComponent(1,RenderDevice::ERegFloat2),
		ShaderStreamCol = SetShaderComponent(0,RenderDevice::ERegFloat3)
						| SetShaderComponent(1,RenderDevice::ERegColor)
                        | SetShaderComponent(2,RenderDevice::ERegFloat2),
		ShaderStream4T	= SetShaderComponent(0,RenderDevice::ERegFloat3)
						| SetShaderComponent(1,RenderDevice::ERegColor)
						| SetShaderComponent(2,RenderDevice::ERegUShort4N),
    };

#ifdef TRANSGAMING_PSP
    TexCoord    TU;
    DWORD       Color;
    Vector      Pos;
#else
    Vector      Pos;
    DWORD       Color;
    TexCoord    TU;
#endif

    SimpleVertex() {}
    SimpleVertex(const Vector& pos, const DWORD color=-1, const TexCoord& tu=TexCoord(0,0))
    :   Pos(pos), Color(color), TU(tu)
    {}
    SimpleVertex(float x, float y, float z, DWORD color, float s, float t)
    :   Pos(x,y,z), Color(color), TU(s,t)
    {}
};

struct ENGINE_API SimplePackedVertex
{
	Vector	Pos;
	DWORD	Color;
	short	S[4];

	SimplePackedVertex(const Vector& InPos, const DWORD InColor, const FLOAT A[4], FLOAT Range)
	:	Pos(InPos), Color(InColor)
	{
			S[0] = (short) (A[0] * Range);
			S[1] = (short) (A[1] * Range);
			S[2] = (short) (A[2] * Range);
			S[3] = (short) (A[3] * Range);
	}
};

struct ENGINE_API ScreenVertex : public SimpleVertex
{
    static float FX;
    static float FY;

	ScreenVertex(float x, float y, float s, float t, DWORD color=0xffffffff)
	:	SimpleVertex((2.0f*x)/FX - 1.0f,-((2.0f*y)/FY - 1.0f),0,color,s,t)
	{}
};

struct ENGINE_API ScreenVertexNDC : public SimpleVertex
{
	ScreenVertexNDC(FLOAT X, FLOAT Y, FLOAT S, FLOAT T)
	:	SimpleVertex(Vector(X,Y,0.1F), 0xFFFFFFFF, TexCoord(S,T))
	{}
};

struct ENGINE_API MeshVertex
{
    // definitions of the stream for programmable pipe
    enum 
    { 
        ShaderStreamNTU = SetShaderComponent(0,RenderDevice::ERegFloat3)
                        | SetShaderComponent(1,RenderDevice::ERegFloat3)
                        | SetShaderComponent(2,RenderDevice::ERegFloat2),

        ShaderStreamTU2 = SetShaderComponent(0,RenderDevice::ERegFloat4)
                        | SetShaderComponent(1,RenderDevice::ERegFloat2)
                        | SetShaderComponent(2,RenderDevice::ERegFloat2),

        ShaderStreamWI  = SetShaderComponent(0,RenderDevice::ERegFloat3)
                        | SetShaderComponent(1,RenderDevice::ERegColor),

        ShaderStreamCTU = SetShaderComponent(0,RenderDevice::ERegFloat3)
                        | SetShaderComponent(1,RenderDevice::ERegColor)
                        | SetShaderComponent(2,RenderDevice::ERegFloat2),

		ShaderStreamN   = SetShaderComponent(0,RenderDevice::ERegFloat3),
	};

    // vertex components
    Vector		Pos;
#ifdef TRANSGAMING
	union {
		struct
		{
			DWORD		Color;
			BaseTexCoord	TU1;
			BaseTexCoord	TU2;
		};
		struct
		{
			BaseVector      Normal;
			BaseTexCoord    TU;
		};
	};
#else
	union {
		struct
		{
			DWORD		Color;
			TexCoord	TU1;
			TexCoord	TU2;
		};
		struct
		{
			Vector      Normal;
			TexCoord    TU;
		};
	};
#endif

    MeshVertex() {}
    MeshVertex(const Vector& pos, 
               DWORD color=-1, 
               const TexCoord& tu1=TexCoord(0,0),
               const TexCoord& tu2=TexCoord(0,0))
    :   
        Pos(pos), Color(color), TU1(tu1), TU2(tu2)
    {}

    MeshVertex(const Vector& pos,
               const Vector& normal,
               const TexCoord& tu)
    :   Pos(pos), Normal(normal), TU(tu)
    {}
};

static inline DWORD PACK_DEC3N(const Vector& N)
{
   // Convert to signed integer -511 to +511 range
   int iX = (int) (N.X * 511.0f);
   int iY = (int) (N.Y * 511.0f);
   int iZ = (int) (N.Z * 511.0f);

   return (iX & 0x3FF) | ((iY & 0x3FF) << 10) | ((iZ & 0x3FF) << 20);
}

#define PACK_MINUS1TO1(X)  ((BYTE)((X)*127.5F + 127.5F))

static inline DWORD PACK_VEC4(const Plane& A)
{
	BYTE B[4];

	B[3] = PACK_MINUS1TO1(A[0]);
	B[2] = PACK_MINUS1TO1(A[1]);
	B[1] = PACK_MINUS1TO1(A[2]);
	B[0] = PACK_MINUS1TO1(A[3]);

	return *(DWORD*) &B[0];
}

struct ENGINE_API MeshPackVertNTU
{
    enum 
    { 
        ShaderStream = SetShaderComponent(0,RenderDevice::ERegFloat3)
                     | SetShaderComponent(1,RenderDevice::ERegDec3N)
                     | SetShaderComponent(2,RenderDevice::ERegColor)
					 | SetShaderComponent(3,RenderDevice::ERegFloat2)
    };

	Vector		Pos;
	DWORD		Normal;
	DWORD		Tangent;
	DWORD		Pad;
	TexCoord	TU;

	MeshPackVertNTU() {}

	MeshPackVertNTU(const MeshVertex& A, const Plane& T)
	{
		Pos			= A.Pos;
		Normal		= PACK_DEC3N(A.Normal);
		Tangent		= PACK_VEC4(T);
		Pad			= 0;
		TU			= A.TU;
	}
};

struct ENGINE_API MeshPackVertTU2
{
	Vector		Pos;
	DWORD		Normal;
	DWORD		Tangent;	// float4, w = sign
	TexCoord	TU1;
	short		TU2[2];
		
	MeshPackVertTU2() {}
	MeshPackVertTU2(const MeshVertex& A, const Vector& N)
	{
		Pos			= A.Pos;
		Normal		= PACK_VEC4(N);
		Tangent		= 0;
		TU1			= A.TU1;
		TU2[0]		= short(A.TU2.X * 32767.0F);
		TU2[1]		= short(A.TU2.Y * 32767.0F);
	}
};

struct ENGINE_API TangentBasis
{
    enum 
    { 
        ShaderStream = SetShaderComponent(0,RenderDevice::ERegFloat3)
                     | SetShaderComponent(1,RenderDevice::ERegFloat3)
                     | SetShaderComponent(2,RenderDevice::ERegFloat2)
    };

    Vector      Tangent;
    Vector      Binormal;
    TexCoord    TU;

    TangentBasis() {}
    TangentBasis(const Vector& tangent,
                 const Vector& binormal,
                 const TexCoord& tu)
    :   Tangent(tangent), Binormal(binormal), TU(tu)
    {}
};

struct ENGINE_API VertexWeights
{
    enum 
    { 
        ShaderStream = SetShaderComponent(0,RenderDevice::ERegFloat3)
                     | SetShaderComponent(1,RenderDevice::ERegColor)
    };

	float Wgh0, Wgh1, Wgh2;
	BYTE  Ind[4];

    VertexWeights() {}
    VertexWeights(const float w0, const float w1, const float w2, 
				  const WORD i0, const WORD i1, const WORD i2, const WORD i3)
    :   Wgh0(w0), Wgh1(w1), Wgh2(w2)
    {
		Ind[0] = BYTE(i0);
		Ind[1] = BYTE(i1);
		Ind[2] = BYTE(i2);
		Ind[3] = BYTE(i3);
	}
};

#ifdef TRANSGAMING
struct ENGINE_API RenderVertexN1 // used by NTU (and sometimes TU2 stage 1)
{
#ifdef TRANSGAMING_PSP
    TexCoord    TU;
    Vector      Normal;
    Vector      Pos;
#else
    Vector      Pos;
    Vector      Normal;
    TexCoord    TU;
#endif

    RenderVertexN1() {}
    RenderVertexN1(const MeshVertex& vtx)
    :   Pos(vtx.Pos), Normal(vtx.Normal), TU(vtx.TU)
    {}
    RenderVertexN1(const MeshVertex& vtx, const Vector& normal)
    :   Pos(vtx.Pos), Normal(normal), TU(vtx.TU1)
    {}
};

struct ENGINE_API RenderVertexN2 // used sometimes by TU2 stage 2
{
#ifdef TRANSGAMING_PSP
    TexCoord    TU;
    Vector      Normal;
    Vector      Pos;
#else
    Vector      Pos;
    Vector      Normal;
    TexCoord    TU;
#endif

    RenderVertexN2() {}
    RenderVertexN2(const MeshVertex& vtx)
    :   Pos(vtx.Pos), Normal(vtx.Normal), TU(vtx.TU)
    {}
    RenderVertexN2(const MeshVertex& vtx, const Vector& normal)
    :   Pos(vtx.Pos), Normal(normal), TU(vtx.TU2)
    {}
};

struct ENGINE_API RenderVertexNC1 // used by TU2 stage 1
{
#ifdef TRANSGAMING_PSP
    TexCoord    TU;
    DWORD       Color;
    Vector      Normal;
    Vector      Pos;
#else
    Vector      Pos;
    Vector      Normal;
    DWORD       Color;
    TexCoord    TU;
#endif

    RenderVertexNC1() {}
    RenderVertexNC1(const MeshVertex& vtx, const Vector& normal)
    :   Pos(vtx.Pos), Normal(normal), Color(vtx.Color), TU(vtx.TU1)
    {}
};

struct ENGINE_API RenderVertexNC2 // used by TU2 stage 2
{
#ifdef TRANSGAMING_PSP
    TexCoord    TU;
    DWORD       Color;
    Vector      Normal;
    Vector      Pos;
#else
    Vector      Pos;
    Vector      Normal;
    DWORD       Color;
    TexCoord    TU;
#endif

    RenderVertexNC2() {}
    RenderVertexNC2(const MeshVertex& vtx, const Vector& normal)
    :   Pos(vtx.Pos), Normal(normal), Color(vtx.Color), TU(vtx.TU2)
    {}
};

struct ENGINE_API RenderVertexC1 // not used right now
{
#ifdef TRANSGAMING_PSP
    TexCoord    TU;
    DWORD       Color;
    Vector      Pos;
#else
    Vector      Pos;
    DWORD       Color;
    TexCoord    TU;
#endif

    RenderVertexC1() {}
    RenderVertexC1(const MeshVertex& vtx)
    :   Pos(vtx.Pos), Color(vtx.Color), TU(vtx.TU1)
    {}
};

struct ENGINE_API RenderVertexC2 // not used right now
{
#ifdef TRANSGAMING_PSP
    TexCoord    TU;
    DWORD       Color;
    Vector      Pos;
#else
    Vector      Pos;
    DWORD       Color;
    TexCoord    TU;
#endif

    RenderVertexC2() {}
    RenderVertexC2(const MeshVertex& vtx)
    :   Pos(vtx.Pos), Color(vtx.Color), TU(vtx.TU2)
    {}
};
#endif

template <> struct ComplexType<TexCoord>        { enum { Value=false }; };
template <> struct ComplexType<SimpleVertex>    { enum { Value=false }; };
template <> struct ComplexType<MeshVertex>      { enum { Value=false }; };
template <> struct ComplexType<TangentBasis>    { enum { Value=false }; };
template <> struct ComplexType<VertexWeights>   { enum { Value=false }; };
#ifdef TRANSGAMING
template <> struct ComplexType<RenderVertexN1>  { enum { Value=false }; };
template <> struct ComplexType<RenderVertexN2>  { enum { Value=false }; };
template <> struct ComplexType<RenderVertexNC1> { enum { Value=false }; };
template <> struct ComplexType<RenderVertexNC2> { enum { Value=false }; };
template <> struct ComplexType<RenderVertexC1>  { enum { Value=false }; };
template <> struct ComplexType<RenderVertexC2>  { enum { Value=false }; };
#endif


*/
#endif //_VERTEX_

