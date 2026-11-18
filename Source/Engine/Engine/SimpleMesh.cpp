// ----------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.						
// Not for public release.
//
// $Header: /Game/Source/Engine/Engine/SimpleMesh.cpp 69    10/13/04 6:24p Bart $
// ----------------------------------------------------------------------------

#include "Engine.h"
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "Render/Video.h"
#include "Engine/SimpleMesh.h"
#include "Render/Material.h"
#include "Engine/World.h"
#include "Render/View.h"


//*************************************** SimpleMesh Implementation ****************************************//
SimpleMesh::SimpleMesh() :
	LightMapTex(0),
#ifndef TRANSGAMING
    NormalMapTex(0),
#endif
    CubeMapTex(0), Master(0), ReloadFlags(0),
    PrimType(EInvalidIndex),
	DefaultShader(0),DetailShader(0),BlendShader(0),BlendDetailShader(0),		
	VolShader(0),ShadowMapShader(0),BlurShader(0),GhostFXShader(0),
	ShadowPassShader(0), PointLightShader(0), SpotLightShader(0), PointTU2GlossShader(0),
    MatSpecColor(Vector(0.5f,0.5f,0.5f)), MatSpecPower(20), MatRefract(1.1f), MatFresnel(0.3f), MatRefrTint(Vector(1,1,1)), MatReflTint(Vector(1,1,1))
#ifdef TRANSGAMING_DEINDEX
    , isIndexed(true)
#endif
#ifdef TRANSGAMING_PSP
    , hasBoundingBox(false)
#endif
//	,Verts(true)
{
	Data = new SimpleMeshData;
	Data->AddRef();
}

SimpleMesh::~SimpleMesh()
{
	if (EInvalidIndex!=Data->ElemStream)
	{
		RenderDevice* dev=GEngine->GetRenderDevice();
		dev->ReleaseResource(RenderDevice::EResBuffer, &Data->ElemStream, 1);
	}

	if (Data->VertStreamCount != 0)
	{
		RenderDevice* dev=GEngine->GetRenderDevice();
		dev->ReleaseResource(RenderDevice::EResBuffer, Data->VertStream, Data->VertStreamCount);
	}

	if (EInvalidIndex!=Data->NormStream)
	{
		RenderDevice* dev=GEngine->GetRenderDevice();
		dev->ReleaseResource(RenderDevice::EResBuffer, &Data->NormStream, 1);
	}
#ifdef TRANSGAMING_PS2
	if (Data->DisplayList)
	    glDeleteLists(Data->DisplayList, 1);
#endif

	Data->DelRef();
}

SimpleMesh &SimpleMesh::operator=(const SimpleMesh &sm)
{
	Data->DelRef();

	LightMapName			= sm.LightMapName;
#ifndef TRANSGAMING
    NormalMapName			= sm.NormalMapName;
	DetailMap				= sm.DetailMap;
#endif
	CubeMapName				= sm.CubeMapName;
	AlphaMap				= sm.AlphaMap;
	ReloadFlags				= sm.ReloadFlags;
								 
	LightMapTex				= sm.LightMapTex;
#ifndef TRANSGAMING
    NormalMapTex			= sm.NormalMapTex;
#endif
	CubeMapTex				= sm.CubeMapTex;
								 
	MatSpecColor			= sm.MatSpecColor;
	MatSpecPower			= sm.MatSpecPower;
	MatRefract				= sm.MatRefract;
	MatFresnel				= sm.MatFresnel;
	MatRefrTint				= sm.MatRefrTint;
	MatReflTint				= sm.MatReflTint;
								 
	Master					= sm.Master;
								 
	DefaultShader			= sm.DefaultShader;
	DetailShader			= sm.DetailShader;
	BlendShader				= sm.BlendShader;
	BlendDetailShader		= sm.BlendDetailShader;
	VolShader				= sm.VolShader;
	ShadowMapShader			= sm.ShadowMapShader;
	BlurShader				= sm.BlurShader;
	GhostFXShader			= sm.GhostFXShader;

	ShadowPassShader		= sm.ShadowPassShader;
	PointLightShader		= sm.PointLightShader;
	SpotLightShader			= sm.SpotLightShader;
	PointTU2GlossShader		= sm.PointTU2GlossShader;
								 
	PrimType				= sm.PrimType;

#ifndef TRANSGAMING
	CloneOf					= sm.CloneOf;
#endif

#ifdef TRANSGAMING_DEINDEX
	isIndexed				= sm.isIndexed;
#endif

#ifdef TRANSGAMING_PSP
	hasBoundingBox			= sm.hasBoundingBox;
#endif

	//use data and increase 
	Data					= sm.Data;
	Data->AddRef();
	if (Data->UseCount)
		TRACE("Sharing mesh %d\n", Data->UseCount);
	return(*this);
}



void SimpleMesh::Init()
{
	DynamicArray<WORD> *IndexArr; 

#ifndef TRANSGAMING
	if (Elems.Size()) {
		IndexArr = &Elems;
	}
	else
#endif
    if (Data->Triangles.Size()) {
		IndexArr = &Data->Triangles;
	}
    else{
        TRACE("SimpleMesh::Init(): don't care about vertex ranges for non-indexed data!\n");

		return;
    }

	for (int i=0; i<Data->Groups.Size(); i++)
	{
		int emin = Data->Groups[i].RenderBase;
		int emax = (i+1==Data->Groups.Size()) ? (*IndexArr).Size() : Data->Groups[i+1].RenderBase;
		int vmin = Data->VertList.Size();
		int vmax = 0;

		for (int e=emin; e<emax; e++)
		{
			if (vmin > (*IndexArr)[e])
				vmin = (*IndexArr)[e];
			if (vmax < (*IndexArr)[e])
				vmax = (*IndexArr)[e];
		}

		Data->Groups[i].VertexMin   = vmin;
		Data->Groups[i].VertexCount = vmax-vmin+1;
	}
}

// serialization
FileObject& operator>>(FileObject& file, SimpleMesh& sm)
{
#ifdef TRANSGAMING
    //extra formatting bit flags are added in the place where the <CloneOf> string is supposed to be
    int                         flags;
    int                         count;
    DynamicArray<WORD>	        Elems;
    VertexList::VertexFormat    fmt = VertexList::VTXFMT_NONE;
    

    //read the <CloneOf> data.  This will contain a single 32-bit int followed by that many bytes.
    //We've hijacked the first 32-bits that are guaranteed to be there in order to store various
    //flags relating to the state of the incoming mesh.  We'll extract the flags from the value then
    //skip over the string that is supposed to be there (it's never used anyway).  
    file >> flags;

# ifdef TRANSGAMING_DEINDEX
    sm.isIndexed = !(flags & SimpleMesh::MESH_DEINDEXED_BIT);
# endif
    sm.PrimType = (flags & SimpleMesh::MESH_TRISTRIP_BIT) ? RenderDevice::EPrimTriStrip : RenderDevice::EPrimTriList;

# ifdef TRANSGAMING_PSP
    sm.hasBoundingBox = (flags & SimpleMesh::MESH_BOUNDINGBOX_BIT) ? true : false;
# endif


    //make sure we're not specifying MT format AND a platform specific format
    Assert(!((flags & SimpleMesh::MESH_MTFORMAT_BIT) && (flags & (SimpleMesh::MESH_PSPDATA_BIT | SimpleMesh::MESH_PS2DATA_BIT))));

    if (flags & SimpleMesh::MESH_MTFORMAT_BIT)
        fmt = VertexList::VTXFMT_MT;

# ifdef TRANSGAMING_PSP
    else if (flags & SimpleMesh::MESH_PSPDATA_BIT)
        fmt = VertexList::VTXFMT_MESH;
        
    Assert(!(flags & SimpleMesh::MESH_PS2DATA_BIT));
# elif defined(TRANSGAMING_PS2)
    else if (flags & SimpleMesh::MESH_PS2DATA_BIT)
        fmt = VertexList::VTXFMT_MESH;

    Assert(!(flags & SimpleMesh::MESH_PSPDATA_BIT));
# endif


    flags &= ~SimpleMesh::MESH_ALL_MASK;

    //this data was supposed to be a string containing the name of the object that this mesh was
    //a clone of.  Now we've hijacked it and embedded our extra flags in it, so we have to make
    //sure the data stream is still in sync with old versions.
    if (flags > 0)
        file.Seek(flags, SEEK_CUR);


    //load the rest of the mesh's data
#ifdef TRANSGAMING
    //skip reading the normal map's name
    int     tmp;
    char    buf[MAX_PATH];


    file >> sm.LightMapName >> tmp;
    Assert(tmp <= MAX_PATH);
    file.Read(buf, tmp);
#else
    file >> sm.LightMapName >> sm.NormalMapName;
#endif
    
#ifdef TRANSGAMING_PSP
    //read in the groups array
    file >> count;

    sm.Data->Groups.ResizeArray(count);

    for (int i = 0; i < count; i++)
        sm.Data->Groups[i].load(file, sm.hasBoundingBox);
    
#else
    file >> sm.Data->Groups;
#endif
        
    file >> sm.Data->Triangles >> Elems;

    sm.Data->VertList.load(file, fmt);

    TRACE("SimpleMesh::operator>>(): converting %d vertices to '%s' format (from '%s') for the mesh from '%s' (data = 0x%08X, vertlist = 0x%08X, this = 0x%08X)\n", sm.Data->VertList.Size(), sm.Data->VertList.getFormatName(VertexList::VTXFMT_MESH), sm.Data->VertList.getFormatName(), file.GetName(), sm.Data, &sm.Data->VertList, &sm);
    sm.Data->VertList.convert(VertexList::VTXFMT_MESH);
    Assert(sm.Data->VertList.getFormat() == VertexList::VTXFMT_MESH);

    file >> sm.Data->Normals >> count;
    

    //make sure no tangents were specified
    Assert(count == 0);

    //skip over any tangents that were specified
    if (count)
        file.Seek((sizeof(Vector) + sizeof(Vector) + sizeof(TexCoord)) * count, SEEK_CUR);


    //respect the [obselete] tri stripped data path and set the primitive type 
    //if a set of primitive elements are loaded in.  Also output a deprecated
    //message.
    if (Elems.Size()){
        TRACE("SimpleMesh::operator>>(): ERROR-> a mesh was loaded with Elems.Size() > 0 from the file '%s'\n", file.GetName());
        Assert(!"mesh has Elems!");

        sm.PrimType = RenderDevice::EPrimTriStrip;
    }

# ifdef TRANSGAMING_MESH_WARNINGS
#  ifdef TRANSGAMING_DEINDEX
    TRACE("SimpleMesh::operator>>(): loaded an object from '%s' with %d verts, %d elems, %d triangles, %d normals, and %d tangents (indexed = %d, primType = %s)\n", 
            file.GetName(), sm.VertList.Size(), Elems.Size(), sm.Triangles.Size(), sm.Normals.Size(), 0, sm.isIndexed, sm.PrimType == RenderDevice::EPrimTriList ? "list" : "strip");
#  else
    TRACE("SimpleMesh::operator>>(): loaded an object from '%s' with %d verts, %d elems, %d triangles, %d normals, and %d tangents (indexed = %d, primType = %s)\n", 
            file.GetName(), sm.VertList.Size(), Elems.Size(), sm.Triangles.Size(), sm.Normals.Size(), 0, sm.Triangles.Size() != 0, sm.PrimType == RenderDevice::EPrimTriList ? "list" : "strip");
#  endif
# endif

    char tmpBuf[MAX_PATH];

    sm.LightMapName  = String::StripBaseName(sm.LightMapName.AsChar(), tmpBuf, MAX_PATH);
	//sm.NormalMapName = String::StripBaseName(sm.NormalMapName.AsChar(), tmpBuf, MAX_PATH);
#else
	FileObject &f = file >> sm.CloneOf >> sm.LightMapName >> sm.NormalMapName
			>> sm.Groups >> sm.Triangles >> sm.Elems
			>> sm.Verts >> sm.Normals >> sm.Tangents;

    sm.LightMapName  = String::StripExtension(String::BaseName(sm.LightMapName));
	sm.NormalMapName = String::StripExtension(String::BaseName(sm.NormalMapName));
#endif
	sm.Init();
	return file;
}

FileObject& operator<<(FileObject& file, const SimpleMesh& sm)
{
#ifdef TRANSGAMING
    int empty = 0;
    int flags = 0;
    int tmp;


#ifdef TRANSGAMING_PSP
    if (sm.hasBoundingBox)
        flags |= SimpleMesh::MESH_BOUNDINGBOX_BIT;
#endif

#ifdef TRANSGAMING_DEINDEX
    if (sm.isIndexed)
        flags |= SimpleMesh::MESH_DEINDEXED_BIT;
#endif

    if (sm.PrimType == RenderDevice::EPrimTriStrip)
        flags |= SimpleMesh::MESH_TRISTRIP_BIT;



    //write out a 0 instead of the <CloneOf> name here.  We've changed the purpose of the
    //data at that position, so make sure it's still compatible with older versions of the
    //data files.
    file << flags;


#ifdef TRANSGAMING
    //skip writing out the normal map name
    tmp = 0;


    file << sm.LightMapName << tmp;
#else
    file << sm.LightMapName << sm.NormalMapName;
#endif
         

#ifdef TRANSGAMING_PSP
    //write out the group list
    tmp = sm.Data->Groups.Size();
    file << tmp;

    for (int i = 0; i < tmp; i++)
        sm.Data->Groups[i].save(file, sm.hasBoundingBox);
#else
    file << sm.Data->Groups;
#endif

    file << sm.Data->Triangles << empty;       //sm.Elems

    sm.Data->VertList.save(file);

    file << sm.Data->Normals << empty;
#else
	file << sm.CloneOf << sm.LightMapName << sm.NormalMapName
         << sm.Data->Groups << sm.Data->Triangles << sm.Data->Elems
         << sm.Data->Verts << sm.Data->Normals << sm.Data->Tangents;
#endif

	return file;
}


#ifdef TRANSGAMING_PSP
FileObject &SimpleMesh::Group::load(FileObject &file, bool readBoundingBox){
    file >> TexName >> RenderBase >> RenderCount;

    if (readBoundingBox)
        file.Read(staticBoundingBox, sizeof(staticBoundingBox));


    char tmpBuf[MAX_PATH];

    TexName = String::StripBaseName(TexName.AsChar(), tmpBuf, MAX_PATH);

    return file;
}

FileObject &SimpleMesh::Group::save(FileObject &file, bool writeBoundingBox){
    file << TexName << RenderBase << RenderCount;

    if (writeBoundingBox)
        file.Write(staticBoundingBox, sizeof(staticBoundingBox));


    return file;
}
#else
FileObject& operator<<(FileObject& file, const SimpleMesh::Group& grp)
{
    return file << grp.TexName << grp.RenderBase << grp.RenderCount;
}

FileObject& operator>>(FileObject& file, SimpleMesh::Group& grp)
{
    FileObject& fo = file >> grp.TexName >> grp.RenderBase >> grp.RenderCount;

#ifdef TRANSGAMING
    char tmpBuf[MAX_PATH];

	grp.TexName = String::StripBaseName(grp.TexName.AsChar(), tmpBuf, MAX_PATH);
#else
	grp.TexName = String::StripExtension(String::BaseName(grp.TexName));
#endif
	return fo;
}
#endif

void SimpleMesh::RenderDefaultPass(Material* shader, int numTexSets)
{
    Assert(shader);			

#ifdef TRANSGAMING_PS2_DISP_LIST
	bool CountRenderVerts = false;
	if (Data->DisplayList)
	{
		glCallList(Data->DisplayList);
		return;
	}
	else if (Data->TexturesCached)
	{
		// We've run through once without a display list, ensuring
		// that textures that have been used are properly set up.  Now
		// record the display list.
		Data->DisplayList = glGenLists(1);
		if (Data->DisplayList)
		{
			// Note that we're using the mode field to pass the expected number 
			// of verts in this list.  It's not otherwise used by ps2gl
			glNewList(Data->DisplayList, Data->RenderVertCount);
		}
	}
	else
	{
	    Data->TexturesCached = true;
	    CountRenderVerts = true;
	}
#endif

    RenderDevice* dev = GEngine->Video->RenderDev;

	if (Data->Groups.Size()>0)
	{
		for (int index=0; index<shader->PassCount; ++index)
		{
			RenderPass& pass = shader->Passes[index];	
			pass.Activate();

			for (int i=0; i<Data->Groups.Size(); ++i) 
			{
				SimpleMesh::Group& grp = Data->Groups[i];

#ifdef TRANSGAMING
				Texture *textures[] = 
				{	
					grp.TexPtr,LightMapTex,CubeMapTex,
					grp.BlendMap.TexPtr,AlphaMap.TexPtr,grp.TexEmPtr,grp.TexGlossPtr
				};
				MaterialDef *materialDefs[] = { &grp.ColorMap, &grp.BlendMap };
				const int flags 
					= MaterialInput::EInputColor
					| MaterialInput::EInputLight
					| MaterialInput::EInputCube
					| MaterialInput::EInputBlend
					| MaterialInput::EInputAlpha
					| MaterialInput::EInputEmissive
					| MaterialInput::EInputGloss
					| MaterialInput::EDefAlpha
					| MaterialInput::EDefBlend;


                MaterialInput mi(flags,textures,materialDefs);


				pass.SetInput(&mi);

# ifdef TRANSGAMING_DEINDEX
#  ifdef TRANSGAMING_MESH_WARNINGS
                /*TRACE("SimpleMesh::Render(): rendering group %d: {base = %d, count = %d, vertexMin = %d, vertexCount = %d}\n", i, grp.RenderBase, grp.RenderCount, grp.VertexMin, grp.VertexCount);
                TRACE("SimpleMesh::Render(): textures:\n"
                      "    Color =      '%s'\n"
                      "    LightMap =   '%s'\n"
                      "    Cube =       '%s'\n"
                      "    Blend =      '%s'\n"
                      "    Alpha =      '%s'\n"
                      "    Emissive =   '%s'\n"
                      "    Gloss =      '%s'\n",
                        grp.TexPtr ?            grp.TexPtr->FileName.AsChar() :             "<none>",
                        LightMapTex ?           LightMapTex->FileName.AsChar() :            "<none>",
                        CubeMapTex ?            CubeMapTex->FileName.AsChar() :             "<none>",
					    grp.BlendMap.TexPtr ?   grp.BlendMap.TexPtr->FileName.AsChar() :    "<none>",
                        AlphaMap.TexPtr ?       AlphaMap.TexPtr->FileName.AsChar() :        "<none>",
                        grp.TexEmPtr ?          grp.TexEmPtr->FileName.AsChar() :           "<none>",
                        grp.TexGlossPtr ?       grp.TexGlossPtr->FileName.AsChar() :        "<none>");//*/
#  endif
                if (!isIndexed){
                    //TRACE("SimpleMesh::Render(): drawing non-indexed\n");
				    dev->Draw(PrimType, grp.RenderBase, grp.RenderCount);
                }

                else{
# endif
                    //TRACE("SimpleMesh::Render(): drawing indexed\n");
				    dev->Draw(PrimType,grp.RenderBase,grp.RenderCount,grp.VertexMin,grp.VertexCount);
# ifdef TRANSGAMING_DEINDEX
                }
# endif
#else
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

                pass.SetInput(&MaterialInput(flags,textures,materialDefs));

				dev->Draw(PrimType,grp.RenderBase,grp.RenderCount,grp.VertexMin,grp.VertexCount);
#endif
				// Assume trilists for count
#ifdef TRANSGAMING_PS2
				Data->RenderVertCount += (3*grp.RenderCount) * numTexSets;
#endif
			}

			//dev->ResetTextureStages();
		}
	}

#ifdef TRANSGAMING_PS2_DISP_LIST
	if (Data->DisplayList)
	{
		// End the list and run it. PS2GL doesn't support COMPILE_AND_EXECUTE
		glEndList();
		glCallList(Data->DisplayList);
		return;
	}
#endif
}

void SimpleMesh::SetTextureName(int texType, const char *texName)
{
	switch(texType)
	{
	case ETexAlphaMap:
		AlphaMap.Name = texName;
		break;

	case ETexNormalMap:
#ifdef TRANSGAMING
        return;
#else
		NormalMapName = texName;
		break;
#endif

	case ETexCubeMap:
		CubeMapName = texName;
		break;

	case ETexDetailMap:
#ifdef TRANSGAMING
        return;
#else
		DetailMap.Name = texName;
		break;
#endif

	default:
		return;
	}

	ReloadFlags |= texType;
}

void SimpleMesh::SetTexturesPriority(DWORD priority)
{
	RenderDevice *dev = GEngine->GetRenderDevice();

	for (int i=0; i<Data->Groups.Size(); ++i)
	{
		if (Data->Groups[i].TexPtr)
			dev->SetTexturePriority(Data->Groups[i].TexPtr, priority);
	}

#ifndef TRANSGAMING
	if (NormalMapTex)
		dev->SetTexturePriority(NormalMapTex, priority);
#endif
	if (LightMapTex)
		dev->SetTexturePriority(LightMapTex, priority);
	if (CubeMapTex)
		dev->SetTexturePriority(CubeMapTex, priority);
}

#ifndef TRANSGAMING
void SimpleMesh::CalcTangents(DynamicArray<Plane>& TanVecs)
{
	const int NumVerts = Verts.Size();
    m_setTag("Mesh");
	Vector* Tan1 = new Vector[NumVerts * 2];
	Vector* Tan2 = Tan1 + NumVerts;
	memset(Tan1, 0, NumVerts * sizeof(Vector) * 2);
	
	for (int a=0; a<Triangles.Size(); ++a)
	{
		const int v1 = Triangles[a];
		const int v2 = Triangles[a];
		const int v3 = Triangles[a];
		
		const Vector& p1 = Verts[v1].Pos;
		const Vector& p2 = Verts[v2].Pos;
		const Vector& p3 = Verts[v3].Pos;

#ifdef	TRANSGAMING
		TexCoord tmpw1;
		TexCoord tmpw2;
		TexCoord tmpw3;
		tmpw1 = Verts[v1].TU1;
		tmpw2 = Verts[v2].TU1;
		tmpw3 = Verts[v3].TU1;
		const TexCoord& w1 = tmpw1;
		const TexCoord& w2 = tmpw2;
		const TexCoord& w3 = tmpw3;
#else
		const TexCoord& w1 = Verts[v1].TU1;
		const TexCoord& w2 = Verts[v2].TU1;
		const TexCoord& w3 = Verts[v3].TU1;
#endif

		float x1 = p2.X - p1.X;
		float x2 = p3.X - p1.X;
		float y1 = p2.Y - p1.Y;
		float y2 = p3.Y - p1.Y;
		float z1 = p2.Z - p1.Z;
		float z2 = p3.Z - p1.Z;
		
		float s1 = w2.X - w1.X;
		float s2 = w3.X - w1.X;
		float t1 = w2.Y - w1.Y;
		float t2 = w3.Y - w1.Y;
		
		float r = 1.0F / (s1 * t2 - s2 * t1);
		Vector sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
		Vector tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);
		
		Tan1[v1] += sdir;
		Tan1[v2] += sdir;
		Tan1[v3] += sdir;
		
		Tan2[v1] += tdir;
		Tan2[v2] += tdir;
		Tan2[v3] += tdir;
	}

	TanVecs.ResizeArray(NumVerts);

	for (int a=0; a<NumVerts; ++a)
	{
		const Vector& N = Verts[a].Normal;
		const Vector& T = Tan1[a];
		
		#ifdef	TRANSGAMING
		// Gram-Schmidt orthogonalize
		TanVecs[a].Normal.Set( (T - N * (N * T)).Normalize() );
		#else
		// Gram-Schmidt orthogonalize
		TanVecs[a].Normal = (T - N * (N * T)).Normalize();
		#endif
		
		// Calculate handedness
		TanVecs[a].W = ((N ^ T).Dot(Tan2[a]) < 0.0F) ? -1.0F : 1.0F;
	}

	delete Tan1;
}
#endif

#ifdef TRANSGAMING_STRIPIFY

# include <Core/nommgr.h>
# include "Engine/tri_stripper/stdafx.h"
# include "Engine/tri_stripper/tri_stripper.h"
# include <Core/mmgr.h>

# define STITCH

void SimpleMesh::Stripify(void)
{
    DynamicArray<WORD> *IndexArr; 

#ifndef TRANSGAMING
    if (Elems.Size())
        // already stripified
        return;
#endif

    if (!Triangles.Size())
        // cannot handle
        return;

    TRACE("Stripifying mesh %p\n", this);

    // First, we're supposed to eliminate duplicate vertices,
    // but for now I'll assume that this isn't necessary
    // (i.e. hoping the modelling tools aren't crap)

    DynamicArray<Group> ResultGroups;
    DynamicArray<WORD> ResultElems;

    // Stripify each group separately.
    for (unsigned i=0; i<Groups.Size(); ++i) 
    {
        Group& grp = Groups[i];
        unsigned ibse = grp.RenderBase;
        unsigned icnt = grp.RenderCount * 3;

        // Build index list for group
        triangle_stripper::tri_stripper::indices indices(icnt);
        for (unsigned e=0; e<icnt; e++) {
            indices[e] = Triangles[ibse+e];
        }
        // Initialize stripper
        triangle_stripper::tri_stripper stripper(indices);
        stripper.SetCacheSize(0);
        stripper.SetMinStripSize(2);
        // Perform stripping
        triangle_stripper::tri_stripper::primitives_vector prims;
        stripper.Strip(&prims);

#ifdef STITCH
        Group ngrp(grp);
        ngrp.RenderBase = ResultElems.Size();
        int stitch_c = 0, stitch_i = -1;
#endif
        // Process each resulting tristrip/trilist
        for (unsigned p=0; p<prims.size(); p++)
        {
             triangle_stripper::tri_stripper::primitives& prim = prims[p];
             unsigned ecnt = prim.m_Indices.size();
             if (prim.m_Type ==
                 triangle_stripper::tri_stripper::PT_Triangle_Strip) {
                 // It's a tristrip, yay
#ifndef STITCH
                 Group ngrp(grp);
                 ngrp.RenderBase = ResultElems.Size();
                 ngrp.RenderCount = ecnt-2;
                 ResultGroups.Add(ngrp);
#else
                 if (stitch_c) {
                     for (unsigned e=0; e<stitch_c; e++)
                         ResultElems.Add(stitch_i);
                     ResultElems.Add(prim.m_Indices[0]);
                 }
#endif
                 for (unsigned e=0; e<ecnt; e++)
                     ResultElems.Add(prim.m_Indices[e]);
#ifdef STITCH
                 stitch_c = 1 + (ecnt & 1);
                 stitch_i = prim.m_Indices[ecnt-1];
#endif
             } else {
                 // It's a list of stray triangles
                 // Since we can't mix tristrips and trilists,
                 // we must convert each triangle in the list to its
                 // own single-triangle tristrip, unless we stitch them
                 unsigned tcnt = ecnt/3;
                 for (unsigned t=0; t<tcnt; t++) {
#ifndef STITCH
                     Group ngrp(grp);
                     ngrp.RenderBase = ResultElems.Size();
                     ngrp.RenderCount = 1;
                     ResultGroups.Add(ngrp);
#else
                     if (stitch_c) {
                         for (unsigned e=0; e<stitch_c; e++)
                             ResultElems.Add(stitch_i);
                         ResultElems.Add(prim.m_Indices[t*3+0]);
                     }
#endif
                     ResultElems.Add(prim.m_Indices[t*3+0]);
                     ResultElems.Add(prim.m_Indices[t*3+1]);
                     ResultElems.Add(prim.m_Indices[t*3+2]);
#ifdef STITCH
                     // it'd probably be great if we could stitch
                     // some of these triangles with less than
                     // 3 dummy vertices, but it's not trivial...
                     stitch_c = 2;
                     stitch_i = prim.m_Indices[t*3+2];
#endif
                 }
             }
        }
#ifdef STITCH
        ngrp.RenderCount = ResultElems.Size() - ngrp.RenderBase - 2;
        if (ngrp.RenderCount > 0)
            ResultGroups.Add(ngrp);
#endif
    }

    // Install stripified mesh
    //Triangles.Clear();
    Groups = ResultGroups;
#ifndef TRANSGAMING
    Elems = ResultElems;
#endif

    // Recalculate vertex ranges
    Init();
}
#endif
