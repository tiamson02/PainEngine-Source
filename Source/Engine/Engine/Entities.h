// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created:	2002/04	Bart Sekura
//----------------------------------------------------------------------------
#error This file is obselete

// TEMP SHIT
class Effect
{
public:
	String	Name;
	//Vector  Position;
	BoundingBox	Box;

	DynamicArray<int>	Emitters;
};

class Material
{
public:
	String			DiffuseMap;
	//TexCoord		  Pan;
	//TexCoord		  Tile;

	int				RefCnt;
	int				TexRef;

	Material() : RefCnt(0),	TexRef(-1) {}
};

class Mesh
{
public:
	struct Section
	{
		int	MaterialRef;
		int	RenderBase;
		int	RenderCount;

		Section() :	MaterialRef(-1)	{}
	};

	DynamicArray<Section>		Sections;
	DynamicArray<MeshVertex>	Verts;
	DynamicArray<Vector>		Normals;
	DynamicArray<WORD>			Triangles;
	DynamicArray<WORD>			Elems;
	String						Name;

	Matrix						ObjectMatrix;
	BoundingBox					Box;
	int							LMapIndex;
	int							Visible;
	DWORD						Flags;

	int							FrameCount;
	DynamicArray<Vector>		FrameVerts;
	DynamicArray<Vector>		FrameNorms;

	int							FrameVertStream;
	int							FrameNormStream;

	// renderer	sort help
	float	ViewDistance;

	// render device specific
	int	VertexStream;
	int IndexStream;
};

struct Light
{
    Vector  Position;
    Vector  Color;
    float   Intensity;
};