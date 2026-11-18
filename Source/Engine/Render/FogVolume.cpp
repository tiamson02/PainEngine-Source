#include "Engine.h"
#include "Render/View.h"
#include "Render/FogVolume.h"
#include "Render/Video.h"
#include "Render/Material.h"

Volume::Volume(const WorldMesh& mesh, int type) : WorldMesh(mesh, 74), VolColor(0xFFFFFF)
{
    //EVIL HACK!!  force the conversion of the mesh's vertlist so that it can be somewhat rendered.
    //  This could be removed when the MT change goes in.
    if (Data->VertList.getFormat() == VertexList::VTXFMT_NONE){
        TRACE("Volume::Volume(): converting %d vertices to '%s' format (from '%s') for object '%s' (data = %p, vertList = %p, this = %p)\n", Data->VertList.Size(), Data->VertList.getFormatName(VertexList::VTXFMT_MESH), Data->VertList.getFormatName(), Name.AsChar(), Data, &Data->VertList, this);
        Data->VertList.convert(VertexList::VTXFMT_MESH);
    }

	Lighting = ELightingNone;
	VolEnd = 20;
	VolDensity = 1;
	//WorldMesh &w = *this;
	//w = mesh;

	VolumeType = type;

	Type = ETypeVolume;
	Flags |= WorldMesh::ENoClip;
	Sort = ESortVolume;
}

Volume::~Volume(void)
{
}

void Volume::DrawBackPass(RenderDevice* dev)
{
    Viewport &pViewport = GEngine->Video->GetActiveView()->LocalViewport;

	dev->SetShaderMemory(0,(Flags & EEntMovable) ?
		&(ObjectToWorld*pViewport.WorldToScreen).GetTranspose()[0] :
		&pViewport.WorldToScreen.GetTranspose()[0],4);

    dev->SetStreams(Data->VertStream, Data->VertStreamCount, Data->ElemStream);

#ifdef TRANSGAMING_MESH_WARNINGS
    //TRACE("Volume::DrawBackPass(): drawing volume fog pass for object '%s' {verts = %d, tris = %d}\n", Name.AsChar(), VertList.Size(), Triangles.Size());
#endif
    RenderDefaultPass(BackShader);
}

void Volume::DrawFrontPass(RenderDevice* dev)
{
    Viewport &pViewport = GEngine->Video->GetActiveView()->LocalViewport;

	dev->SetShaderMemory(0,(Flags & EEntMovable) ?
		&(ObjectToWorld*pViewport.WorldToScreen).GetTranspose()[0] :
		&pViewport.WorldToScreen.GetTranspose()[0],4);

    dev->SetStreams(Data->VertStream, Data->VertStreamCount, Data->ElemStream);

#ifdef TRANSGAMING_MESH_WARNINGS
    //TRACE("Volume::DrawFrontPass(): drawing volume fog pass for object '%s' {verts = %d, tris = %d}\n", Name.AsChar(), VertList.Size(), Triangles.Size());
#endif
	RenderDefaultPass(FrontShader);
}

void Volume::RenderInitialize(RenderDevice* dev, MaterialSystem* matSys, const char* texSubPath)
{
    Assert(dev);
    Assert(matSys);
    void *  ptr;
    int     stride;


    // basic init
	LocalBox = Box;

    for (int i = 0; i < VertexList::MaxStreamCount; i++)
        Data->VertStream[i] = EInvalidIndex;

	Data->NormStream = EInvalidIndex;
    Data->ElemStream = EInvalidIndex;
    Data->VertStreamCount = 0;

    // assume the worst
    Flags|=EInvalid;


    Assert(Data->VertList.getFormat() == VertexList::VTXFMT_MESH);

    // vertex stream
#ifdef TRANSGAMING_PS2
    //positions stream
    ptr = Data->VertList.getPositions(&stride);
    Assert(ptr && stride);
    TRACE("FogVolume::RenderInitialize(): allocating a position buffer for %d verts for '%s' (size = %d, type = '%s')\n", Data->VertList.Size(), Name.AsChar(), stride, Data->VertList.getFormatName());
    Data->VertStream[Data->VertStreamCount++] = dev->CreateStream(ptr, 0, Data->VertList.Size(), stride);

    //positions stream
    ptr = Data->VertList.getNormals(&stride);
    Assert(ptr && stride);
    TRACE("FogVolume::RenderInitialize(): allocating a normal buffer for %d verts for '%s' (size = %d, type = '%s')\n", Data->VertList.Size(), Name.AsChar(), stride, Data->VertList.getFormatName());
    Data->VertStream[Data->VertStreamCount++] = dev->CreateStream(ptr, 0, Data->VertList.Size(), stride);

    //positions stream
    ptr = Data->VertList.getTexCoord1(&stride);
    Assert(ptr && stride);
    TRACE("FogVolume::RenderInitialize(): allocating a tex coord buffer for %d verts for '%s' (size = %d, type = '%s')\n", Data->VertList.Size(), Name.AsChar(), stride, Data->VertList.getFormatName());
    Data->VertStream[Data->VertStreamCount++] = dev->CreateStream(ptr, 0, Data->VertList.Size(), stride);
#else
    ptr = Data->VertList.getVerts(&stride);
    TRACE("FogVolume::RenderInitialize(): allocating a vertex buffer for %d verts for '%s' (size = %d, type = '%s')\n", Data->VertList.Size(), Name.AsChar(), stride, Data->VertList.getFormatName());
    Data->VertStream[Data->VertStreamCount++] = dev->CreateStream(ptr, 0, Data->VertList.Size(), stride);
#endif

    if (Data->VertStreamCount == 0 || Data->VertStream[0] == EInvalidIndex)
        return;


    // indices
#ifndef TRANSGAMING
    if (Elems.Size())
    {
        // index stream
        ElemStream = dev->CreateStream
        (
            &Elems[0],
            RenderDevice::EIndex16,
            Elems.Size(),
            sizeof(WORD)
        );

        if (EInvalidIndex == ElemStream)
		    return;

# ifndef TRANSGAMING
        PrimType = RenderDevice::EPrimTriStrip;
# endif
    }
    else
#endif
    if (Data->Triangles.Size())
    {
        Data->ElemStream = dev->CreateStream
        (
            &Data->Triangles[0],
            RenderDevice::EIndex16,
            Data->Triangles.Size(),
            sizeof(WORD)
        );

        if (EInvalidIndex == Data->ElemStream)
		    return;

#ifndef TRANSGAMING
        PrimType = RenderDevice::EPrimTriList;
#endif
    }


    // init shading group materials
    char tmpName[_MAX_PATH];

	for (int i=0; i<Data->Groups.Size(); ++i) {
        snprintf(tmpName, _MAX_PATH, "%s%s", texSubPath ? texSubPath : "", Data->Groups[i].TexName.AsChar());

        Data->Groups[i].TexPtr =
			(Data->Groups[i].TexName == "notex" || Data->Groups[i].TexName.IsNull()) ?
			matSys->Textures.Gray :
			matSys->CreateTexture(tmpName);
			//matSys->CreateTexture((String(texSubPath?texSubPath:"")+Data->Groups[i].TexName).AsChar());
	}


    // all good, clear the bad flag
    Flags&=~EInvalid;
	BackShader		= matSys->CreateMaterial("volback");

	switch (VolumeType)
	{
		case EVolLight:
			FrontShader		= matSys->CreateMaterial("vollightfront");
			break;

		case EVolFog:
			FrontShader		= matSys->CreateMaterial("volfogfront");
			break;
	}
}

void Volume::RenderCleanup(RenderDevice* dev, MaterialSystem* matSys)
{
    Assert(dev);
    Assert(matSys);

    if(Flags & EInvalid)
        return;

    // remove buffers
    dev->ReleaseResource(RenderDevice::EResBuffer, Data->VertStream, Data->VertStreamCount);
    dev->ReleaseResource(RenderDevice::EResBuffer, &Data->ElemStream, 1);

    for (int i = 0; i < VertexList::MaxStreamCount; i++)
        Data->VertStream[i] = EInvalidIndex;

    Data->ElemStream = EInvalidIndex;
    Data->VertStreamCount = 0;


    // remove textures
    for(int i=0; i<Data->Groups.Size(); ++i)
		matSys->ReleaseTexture(Data->Groups[i].TexPtr);
    //    matSys->ReleaseMaterial(Data->Groups[i].Material);

	matSys->ReleaseMaterial(BackShader);
	matSys->ReleaseMaterial(FrontShader);
}
