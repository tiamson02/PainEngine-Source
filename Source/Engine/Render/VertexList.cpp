#include "VertexList.h"
#include "FileSystem/GFileManager.h"
#ifndef MEMORY_TEST
# include <malloc.h>
#endif
#include "Core/mmgr.h"

#define STRINGIZER(x) #x

#ifdef TRANSGAMING_PS2
struct TexRange{
    TexCoord min;
    TexCoord max;

    void updateRange(const TexCoord &t){
        if (t.X < min.X) min.X = t.X;
        if (t.Y < min.Y) min.Y = t.Y;

        if (t.X > max.X) max.X = t.X;
        if (t.Y > max.Y) max.Y = t.Y;
    }
};
#endif


const char *VertexList::formatNames[VTXFMT_COUNT + 1] = {
#ifdef TRANSGAMING_PS2
        "MeshVertex",
        "Position4f | Normal3f | TexCoord2f",
# ifdef TRANSGAMING_KEEP_NORMALS
        "Position4f | Normal3f | TexCoord2f | TexCoord2f",
# else
        "Position4f | TexCoord2f | TexCoord2f",
# endif
        "Position4f | Normal3f | TexCoord2f | Weight" STRINGIZER(WEIGHT_COUNT) "f",
        "<unknown>"
#else
        "MeshVertex",
        "RenderVertexN1",
        "MTVertex",
        "BoneVertex",
        "<unknown>"
#endif
};


//************************************** Utility Functions ***********************************************//
template <typename T>
bool bufferAlloc(T **buf, size_t size){
#ifdef TRANSGAMING_PS2
	m_setTag("VertexList");
	*buf = (T *)memalign(64, sizeof(T) * size);
#else
	*buf = (T *)malloc(sizeof(T) * size);
#endif
#ifdef TRANSGAMING_MESH_WARNINGS
    TRACE("bufferAlloc(): allocated a buffer to store %d vertices at %p {sizeof(T) = %d}\n", size, *buf, sizeof(T));
#endif

    return (*buf) != NULL;
}

void bufferFree(void *buf){
    if (buf == NULL)
        return;

#ifdef TRANSGAMING_PS2
    free(buf);
#else
    free(buf);
#endif
#ifdef TRANSGAMING_MESH_WARNINGS
    TRACE("bufferFree(): destroying the vertex buffer at %p\n", buf);
#endif
}


#define COPYVBUF(dst, src, count)       memcpy(dst, src, sizeof(src[0]) * (count))
#define READVBUF(file, dst, count)      file.Read(dst, sizeof(dst[0]) * (count))
#define WRITEVBUF(file, dst, count)     file.Write(dst, sizeof(dst[0]) * (count))

//*************************************** constructors & destructor ****************************************//
VertexList::VertexList()
{
#ifdef TRANSGAMING_PS2
    bone.positions = NULL;
    bone.normals = NULL;
    bone.tex1 = NULL;
    bone.weights = NULL;
#endif

	count		= 0;
	format		= VTXFMT_NONE;
	stride		= 0;
	voidVerts	=	NULL;
}

VertexList::VertexList(FileObject& file, VertexFormat fmt)
{
#ifdef TRANSGAMING_PS2
    bone.positions = NULL;
    bone.normals = NULL;
    bone.tex1 = NULL;
    bone.weights = NULL;
#endif

	count		= 0;
	format		= VTXFMT_NONE;
	stride		= 0;
	voidVerts	=	NULL;

    load(file, fmt);
}

VertexList::~VertexList(){
    clear();
}


const char *VertexList::getFormatName() const{
    return formatNames[format];
}

const char *VertexList::getFormatName(const VertexFormat fmt) const{
    return formatNames[fmt];
}


MeshVertex VertexList::getVert(int i) const{
    MeshVertex vtx;


    switch (format){
        case VTXFMT_NONE:
            vtx = verts[i];
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            vtx.Pos = MT.positions[i];
# ifdef TRANSGAMING_KEEP_NORMALS
            vtx.Normal = MT.normals[i];
# else
            vtx.Normal.Set(0, 0, 1);
# endif
            vtx.TU1 = MT.tex1[i];
            vtx.TU2 = MT.tex2[i];
            break;

        case VTXFMT_MESH:
            vtx.Pos = RN1.positions[i];
            vtx.Normal = RN1.normals[i];
            vtx.TU = RN1.tex1[i];
            break;

        case VTXFMT_BONE:
            vtx.Pos = bone.positions[i];
            vtx.Normal = bone.normals[i];
            vtx.TU = bone.tex1[i];
            break;
#else
        case VTXFMT_MT:
            vtx = MTVerts[i];
            break;

        case VTXFMT_MESH:
            vtx = RN1Verts[i];
            break;

        case VTXFMT_BONE:
            vtx = boneVerts[i];
            break;
#endif
    }

    return vtx;
}

MeshVertex &VertexList::getVert(int i, MeshVertex &vtx) const{
    switch (format){
        case VTXFMT_NONE:
            vtx = verts[i];
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            vtx.Pos = MT.positions[i];
# ifdef TRANSGAMING_KEEP_NORMALS
            vtx.Normal = MT.normals[i];
# else
            vtx.Normal.Set(0, 0, 1);
# endif
            vtx.TU1 = MT.tex1[i];
            vtx.TU2 = MT.tex2[i];
            break;

        case VTXFMT_MESH:
            vtx.Pos = RN1.positions[i];
            vtx.Normal = RN1.normals[i];
            vtx.TU = RN1.tex1[i];
            break;

        case VTXFMT_BONE:
            vtx.Pos = bone.positions[i];
            vtx.Normal = bone.normals[i];
            vtx.TU = bone.tex1[i];
            break;
#else
        case VTXFMT_MT:
            vtx = MTVerts[i];
            break;

        case VTXFMT_MESH:
            vtx = RN1Verts[i];
            break;

        case VTXFMT_BONE:
            vtx = boneVerts[i];
            break;
#endif
    }

    return vtx;
}

#ifdef TRANSGAMING_PS2
Vector4 *VertexList::getPositions(int *_stride) const{
    switch (format){
        case VTXFMT_NONE:
            *_stride = 0;
            return NULL;

        case VTXFMT_MT:
            *_stride = sizeof(MT.positions[0]);
            return MT.positions;

        case VTXFMT_MESH:
            *_stride = sizeof(RN1.positions[0]);
            return RN1.positions;

        case VTXFMT_BONE:
            *_stride = sizeof(bone.positions[0]);
            return bone.positions;
    }

    return NULL;
}

Vector *VertexList::getNormals(int *_stride) const{
    switch (format){
        case VTXFMT_NONE:
            *_stride = 0;
            return NULL;

        case VTXFMT_MT:
# ifdef TRANSGAMING_KEEP_NORMALS
            *_stride = sizeof(MT.normals[0]);
            return MT.normals;
# else
            *_stride = 0;
            return NULL;
# endif

        case VTXFMT_MESH:
            *_stride = sizeof(RN1.normals[0]);
            return RN1.normals;

        case VTXFMT_BONE:
            *_stride = sizeof(bone.normals[0]);
            return bone.normals;
    }

    return NULL;
}

TexCoord *VertexList::getTexCoord1(int *_stride) const{
    switch (format){
        case VTXFMT_NONE:
            Assert(!"unknown texcoord position!");
            *_stride = 0;
            return NULL;

        case VTXFMT_MT:
            *_stride = sizeof(MT.tex1[0]);
            return MT.tex1;

        case VTXFMT_MESH:
            *_stride = sizeof(RN1.tex1[0]);
            return RN1.tex1;

        case VTXFMT_BONE:
            *_stride = sizeof(bone.tex1[0]);
            return bone.tex1;
    }

    *_stride = 0;
    return NULL;
}

TexCoord *VertexList::getTexCoord2(int *_stride) const{
    switch (format){
        case VTXFMT_NONE:
            Assert(!"unknown texcoord position!");
            *_stride = 0;
            return NULL;

        case VTXFMT_MT:
            *_stride = sizeof(MT.tex2[0]);
            return MT.tex2;

        default:
        case VTXFMT_MESH:
        case VTXFMT_BONE:
            *_stride = 0;
            return NULL;
    }

    return NULL;
}

WEIGHT_TYPE *VertexList::getWeights(int *_stride) const{
    switch (format){
        default:
        case VTXFMT_NONE:
        case VTXFMT_MT:
        case VTXFMT_MESH:
            *_stride = 0;
            return NULL;

        case VTXFMT_BONE:
            *_stride = sizeof(bone.weights[0]) * WEIGHT_COUNT;
            return bone.weights;
    }

    return NULL;
}
#endif


MeshVertex VertexList::operator[](int i) const{
    MeshVertex vtx;


    switch (format){
        case VTXFMT_NONE:
            vtx = verts[i];
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            vtx.Pos = MT.positions[i];
# ifdef TRANSGAMING_KEEP_NORMALS
            vtx.Normal = MT.normals[i];
# else
            vtx.Normal.Set(0, 0, 1);
# endif
            vtx.TU1 = MT.tex1[i];
            vtx.TU2 = MT.tex2[i];
            break;

        case VTXFMT_MESH:
            vtx.Pos = RN1.positions[i];
            vtx.Normal = RN1.normals[i];
            vtx.TU = RN1.tex1[i];
            break;

        case VTXFMT_BONE:
            vtx.Pos = bone.positions[i];
            vtx.Normal = bone.normals[i];
            vtx.TU = bone.tex1[i];
            break;
#else
        case VTXFMT_MT:
            vtx = MTVerts[i];
            break;

        case VTXFMT_MESH:
            vtx = RN1Verts[i];
            break;

        case VTXFMT_BONE:
            vtx = boneVerts[i];
            break;
#endif
    }

    return vtx;
}

void VertexList::setSize(int cnt, VertexFormat fmt){
    clear();

    switch (fmt){
        case VTXFMT_NONE:
            stride = sizeof(MeshVertex);
            bufferAlloc(&verts, cnt);
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            stride = 0;
            bufferAlloc(&MT.positions, cnt);
# ifdef TRANSGAMING_KEEP_NORMALS
            bufferAlloc(&MT.normals, cnt);
# endif
            bufferAlloc(&MT.tex1, cnt);
            bufferAlloc(&MT.tex2, cnt);
            break;

        case VTXFMT_MESH:
            stride = 0;
            bufferAlloc(&RN1.positions, cnt);
            bufferAlloc(&RN1.normals, cnt);
            bufferAlloc(&RN1.tex1, cnt);
            break;

        case VTXFMT_BONE:
            stride = 0;
            bufferAlloc(&bone.positions, cnt);
            bufferAlloc(&bone.normals, cnt);
            bufferAlloc(&bone.tex1, cnt);
            bufferAlloc(&bone.weights, cnt * WEIGHT_COUNT);
            break;
#else
        case VTXFMT_MT:
            stride = sizeof(MTVertex);
            bufferAlloc(&MTVerts, cnt);
            break;

        case VTXFMT_MESH:
            stride = sizeof(RenderVertexN1);
            bufferAlloc(&RN1Verts, cnt);
            break;

        case VTXFMT_BONE:
            stride = sizeof(BoneVertex4);
            bufferAlloc(&boneVerts, cnt);
            break;
#endif
    }

    format = fmt;
    count = cnt;
}

void VertexList::clear(){
    switch (format){
        case VTXFMT_NONE:
            bufferFree(verts);
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            bufferFree(MT.positions);
# ifdef TRANSGAMING_KEEP_NORMALS
            bufferFree(MT.normals);
# endif
            bufferFree(MT.tex1);
            bufferFree(MT.tex2);
            break;

        case VTXFMT_MESH:
            bufferFree(RN1.positions);
            bufferFree(RN1.normals);
            bufferFree(RN1.tex1);
            break;

        case VTXFMT_BONE:
            bufferFree(bone.positions);
            bufferFree(bone.normals);
            bufferFree(bone.tex1);
            bufferFree(bone.weights);
            break;
#else
        case VTXFMT_MT:
            bufferFree(MTVerts);
            break;

        case VTXFMT_MESH:
            bufferFree(RN1Verts);
            break;

        case VTXFMT_BONE:
            bufferFree(boneVerts);
            break;
#endif
    }

    count = 0;
    stride = 0;
    format = VTXFMT_NONE;
    voidVerts = NULL;

#ifdef TRANSGAMING_PS2
    bone.positions = NULL;
    bone.normals = NULL;
    bone.tex1 = NULL;
    bone.weights = NULL;
#endif
}

VertexList &VertexList::operator = (const VertexList &v){
    clear();


    count = v.count;

    switch (v.format){
        case VTXFMT_NONE:
            bufferAlloc(&verts, count);

            memcpy(verts, v.verts, sizeof(MeshVertex) * count);
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            bufferAlloc(&MT.positions, count);
# ifdef TRANSGAMING_KEEP_NORMALS
            bufferAlloc(&MT.normals, count);
# endif
            bufferAlloc(&MT.tex1, count);
            bufferAlloc(&MT.tex2, count);

            COPYVBUF(MT.positions, v.MT.positions, count);
# ifdef TRANSGAMING_KEEP_NORMALS
            COPYVBUF(MT.normals, v.MT.normals, count);
# endif
            COPYVBUF(MT.tex1, v.MT.tex1, count);
            COPYVBUF(MT.tex2, v.MT.tex2, count);
            break;

        case VTXFMT_MESH:
            bufferAlloc(&RN1.positions, count);
            bufferAlloc(&RN1.normals, count);
            bufferAlloc(&RN1.tex1, count);

            COPYVBUF(RN1.positions, v.RN1.positions, count);
            COPYVBUF(RN1.normals, v.RN1.normals, count);
            COPYVBUF(RN1.tex1, v.RN1.tex1, count);
            break;

        case VTXFMT_BONE:
            bufferAlloc(&bone.positions, count);
            bufferAlloc(&bone.normals, count);
            bufferAlloc(&bone.tex1, count);
            bufferAlloc(&bone.weights, count * WEIGHT_COUNT);

            COPYVBUF(bone.positions, v.bone.positions, count);
            COPYVBUF(bone.normals, v.bone.normals, count);
            COPYVBUF(bone.tex1, v.bone.tex1, count);
            COPYVBUF(bone.weights, v.bone.weights, count * WEIGHT_COUNT);
            break;
#else
        case VTXFMT_MT:
            bufferAlloc(&MTVerts, count);

            COPYVBUF(MTVerts, v.MTVerts, count);
            break;

        case VTXFMT_MESH:
            bufferAlloc(&RN1Verts, count);

            COPYVBUF(RN1Verts, v.RN1Verts, count);
            break;

        case VTXFMT_BONE:
            bufferAlloc(&boneVerts, count);

            COPYVBUF(boneVerts, v.boneVerts, count);
            break;
#endif
    }

    stride = v.stride;
    format = v.format;

    return *this;
}

void VertexList::copy(void *dst, VertexFormat fmt) const{
    Assert(format == fmt);
    Assert(verts);

    //!!// possibly extend this to support converting the array on the fly if
    //!!// a different vertex format is requested.

    switch (format){
        case VTXFMT_NONE:
            COPYVBUF(dst, verts, count);
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
        case VTXFMT_MESH:
        case VTXFMT_BONE:
            Assert(!"can't copy deinterleaved verts!");
            break;
#else
        case VTXFMT_MT:
            COPYVBUF(dst, MTVerts, count);
            break;

        case VTXFMT_MESH:
            COPYVBUF(dst, RN1Verts, count);
            break;

        case VTXFMT_BONE:
            COPYVBUF(dst, boneVerts, count);
            break;
#endif
    }
}

#ifdef TRANSGAMING_PS2
void VertexList::copyPositions(void *dst, VertexFormat fmt) const{
    Assert(format == fmt);
    Assert(verts);


    //!!// possibly extend this to support converting the array on the fly if
    //!!// a different vertex format is requested.

    switch (format){
        case VTXFMT_NONE:
            Assert(!"can't copy interleaved positions!");
            break;

        case VTXFMT_MT:
            COPYVBUF(dst, MT.positions, count);
            break;

        case VTXFMT_MESH:
            COPYVBUF(dst, RN1.positions, count);
            break;

        case VTXFMT_BONE:
            COPYVBUF(dst, bone.positions, count);
            break;
    }
}

void VertexList::copyNormals(void *dst, VertexFormat fmt) const{
    Assert(format == fmt);
    Assert(verts);


    //!!// possibly extend this to support converting the array on the fly if
    //!!// a different vertex format is requested.

    switch (format){
        case VTXFMT_NONE:
            Assert(!"can't copy interleaved positions!");
            break;

        case VTXFMT_MT:
# ifdef TRANSGAMING_KEEP_NORMALS
            COPYVBUF(dst, MT.normals, count);
# else
            Assert(!"no normals!");
# endif
            break;

        case VTXFMT_MESH:
            COPYVBUF(dst, RN1.normals, count);
            break;

        case VTXFMT_BONE:
            COPYVBUF(dst, bone.normals, count);
            break;
    }
}

void VertexList::copyTex1(void *dst, VertexFormat fmt) const{
    Assert(format == fmt);
    Assert(verts);


    //!!// possibly extend this to support converting the array on the fly if
    //!!// a different vertex format is requested.

    switch (format){
        case VTXFMT_NONE:
            Assert(!"can't copy interleaved positions!");
            break;

        case VTXFMT_MT:
            COPYVBUF(dst, MT.tex1, count);
            break;

        case VTXFMT_MESH:
            COPYVBUF(dst, RN1.tex1, count);
            break;

        case VTXFMT_BONE:
            COPYVBUF(dst, bone.tex1, count);
            break;
    }
}

void VertexList::copyTex2(void *dst, VertexFormat fmt) const{
    Assert(format == fmt);
    Assert(verts);


    //!!// possibly extend this to support converting the array on the fly if
    //!!// a different vertex format is requested.

    switch (format){
        case VTXFMT_NONE:
            Assert(!"can't copy interleaved positions!");
            break;

        case VTXFMT_MT:
            COPYVBUF(dst, MT.tex2, count);
            break;

        case VTXFMT_MESH:
        case VTXFMT_BONE:
            Assert(!"no tex2!");
            break;
    }
}

void VertexList::copyWeights(void *dst, VertexFormat fmt) const{
    Assert(format == fmt);
    Assert(verts);


    //!!// possibly extend this to support converting the array on the fly if
    //!!// a different vertex format is requested.

    switch (format){
        case VTXFMT_NONE:
            Assert(!"can't copy interleaved positions!");
            break;

        case VTXFMT_MT:
        case VTXFMT_MESH:
            Assert(!"no weights!");
            break;

        case VTXFMT_BONE:
            COPYVBUF(dst, bone.weights, count * WEIGHT_COUNT);
            break;
    }
}
#endif

bool VertexList::load(FileObject &file, VertexFormat fmt){
    clear();

    switch (fmt){
        case VTXFMT_NONE:
            file >> count;

            bufferAlloc(&verts, count);
            READVBUF(file, verts, count);

            stride =    sizeof(MeshVertex);
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            file >> count;

            bufferAlloc(&MT.positions, count);
# ifdef TRANSGAMING_KEEP_NORMALS
            bufferAlloc(&MT.normals, count);
# endif
            bufferAlloc(&MT.tex1, count);
            bufferAlloc(&MT.tex2, count);

            READVBUF(file, MT.positions, count);
# ifdef TRANSGAMING_KEEP_NORMALS
            READVBUF(file, MT.normals, count);
# endif
            READVBUF(file, MT.tex1, count);
            READVBUF(file, MT.tex2, count);

            stride = 0;
            break;

        case VTXFMT_MESH:
            file >> count;

            bufferAlloc(&RN1.positions, count);
            bufferAlloc(&RN1.normals, count);
            bufferAlloc(&RN1.tex1, count);

            READVBUF(file, RN1.positions, count);
            READVBUF(file, RN1.normals, count);
            READVBUF(file, RN1.tex1, count);

            stride = 0;
            break;

        case VTXFMT_BONE:
            file >> count;

            bufferAlloc(&bone.positions, count);
            bufferAlloc(&bone.normals, count);
            bufferAlloc(&bone.tex1, count);
            bufferAlloc(&bone.weights, count * WEIGHT_COUNT);

            READVBUF(file, bone.positions, count);
            READVBUF(file, bone.normals, count);
            READVBUF(file, bone.tex1, count);
            READVBUF(file, bone.weights, count * WEIGHT_COUNT);

            stride = 0;
            break;
#else
        case VTXFMT_MT:
            file >> count;

            bufferAlloc(&MTVerts, count);
            READVBUF(file, MTVerts, count);

            stride =    sizeof(MTVertex);
            break;

        case VTXFMT_MESH:
            file >> count;

            bufferAlloc(&RN1Verts, count);
            READVBUF(file, RN1Verts, count);

            stride =    sizeof(RenderVertexN1);
            break;

        case VTXFMT_BONE:
            file >> count;

            bufferAlloc(&boneVerts, count);
            READVBUF(file, boneVerts, count);

            stride =    sizeof(BoneVertex4);
            break;
#endif
    }

    format = fmt;
    Assert(count != 0);
    Assert(stride != 0);

    return true;
}

bool VertexList::save(FileObject &file) const{
    switch (format){
        case VTXFMT_NONE:
            file << count;
            file.Write(verts, sizeof(MeshVertex) * count);
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            file << count;

            WRITEVBUF(file, MT.positions, count);
# ifdef TRANSGAMING_KEEP_NORMALS
            WRITEVBUF(file, MT.normals, count);
# endif
            WRITEVBUF(file, MT.tex1, count);
            WRITEVBUF(file, MT.tex2, count);
            break;

        case VTXFMT_MESH:
            file << count;

            WRITEVBUF(file, RN1.positions, count);
            WRITEVBUF(file, RN1.normals, count);
            WRITEVBUF(file, RN1.tex1, count);
            break;

        case VTXFMT_BONE:
            file << count;

            WRITEVBUF(file, bone.positions, count);
            WRITEVBUF(file, bone.normals, count);
            WRITEVBUF(file, bone.tex1, count);
            WRITEVBUF(file, bone.weights, count * WEIGHT_COUNT);
            break;
#else
        case VTXFMT_MT:
            file << count;
            WRITEVBUF(file, MTVerts, count);
            break;

        case VTXFMT_MESH:
            file << count;
            WRITEVBUF(file, RN1Verts, count);
            break;

        case VTXFMT_BONE:
            file << count;
            WRITEVBUF(file, boneVerts, count);
            break;
#endif
    }

    return true;
}

bool VertexList::convert(VertexFormat fmt, DynamicArray<Vector> *normals){
    if (format != VTXFMT_NONE){
        Assert(!"Attempt to reconvert verts!");
        return false;
    }

    //already converted to the correct format => succeed
    if (format == fmt)
        return true;

    //make sure there are some vertices to convert
    Assert(count);


    switch (fmt){
#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:{
            MTVertices tmp;


            bufferAlloc(&tmp.positions, count);
# ifdef TRANSGAMING_KEEP_NORMALS
            bufferAlloc(&tmp.normals, count);
# endif
            bufferAlloc(&tmp.tex1, count);
            bufferAlloc(&tmp.tex2, count);

# ifdef TRANSGAMING_KEEP_NORMALS
            Assert(normals);
            Assert(normals.Size() >= count);
            
            for (int i = 0; i < count; i++){
                tmp.positions[i] = verts[i].Pos;
                tmp.positions[i].W = 1.0f;
                tmp.normals[i] = verts[i].Normal;
                tmp.tex1[i] = verts[i].TU1;
                tmp.tex2[i] = verts[i].TU2;
            }
# else
            for (int i = 0; i < count; i++){
                tmp.positions[i] = verts[i].Pos;
                tmp.positions[i].W = 1.0f;
                tmp.tex1[i] = verts[i].TU1;
                tmp.tex2[i] = verts[i].TU2;
            }
# endif

            //change the stored format
            format = fmt;
            stride = 0;

            //destroy the source vertex array
            bufferFree(verts);

            //take the new vertex data from the temp array
            MT.positions = tmp.positions;
# ifdef TRANSGAMING_KEEP_NORMALS
            MT.normals= tmp.normals;
# endif
            MT.tex1 = tmp.tex1;
            MT.tex2 = tmp.tex2;

            break;
        }

        case VTXFMT_MESH:{
            RN1Vertices tmp;


            bufferAlloc(&tmp.positions, count);
            bufferAlloc(&tmp.normals, count);
            bufferAlloc(&tmp.tex1, count);


            for (int i = 0; i < count; i++){
                tmp.positions[i] = verts[i].Pos;
                tmp.positions[i].W = 1.0f;
                tmp.normals[i] = verts[i].Normal;
                tmp.tex1[i] = verts[i].TU;
            }


            //change the stored format
            format = fmt;
            stride = 0;

            //destroy the source vertex array
            bufferFree(verts);

            //take the new vertex data from the temp array
            RN1.positions = tmp.positions;
            RN1.normals = tmp.normals;
            RN1.tex1 = tmp.tex1;

            break;
        }

        case VTXFMT_BONE:{
            BoneVertices tmp;


            bufferAlloc(&tmp.positions, count);
            bufferAlloc(&tmp.normals, count);
            bufferAlloc(&tmp.tex1, count);
            bufferAlloc(&tmp.weights, count * WEIGHT_COUNT);


            for (int i = 0; i < count; i++){
                tmp.positions[i] = verts[i].Pos;
                tmp.positions[i].W = 1.0f;
                tmp.normals[i] = verts[i].Normal;
                tmp.tex1[i] = verts[i].TU;

                //!!// figure out a way to pass the weights into this function!
                tmp.weights[i * WEIGHT_COUNT] = WEIGHT_SCALE;

                for (int j = 0; j < WEIGHT_COUNT; j++)
                    tmp.weights[i * WEIGHT_COUNT + j] = 0;
            }


            //change the stored format
            format = fmt;
            stride = 0;

            //destroy the source vertex array
            bufferFree(verts);

            //take the new vertex data from the temp array
            bone.positions = tmp.positions;
            bone.normals = tmp.normals;
            bone.tex1 = tmp.tex1;
            bone.weights = tmp.weights;

            break;
        }
#else   //TRANSGAMING_PS2
        case VTXFMT_MT:{
            MTVertex *    tmp;


            bufferAlloc(&tmp, count);

# ifdef TRANSGAMING_KEEP_NORMALS
            Assert(normals);
            Assert(normals.Size() >= count);
            for (int i = 0; i < count; i++)
                tmp[i] = MTVertex(verts[i], (*normals)[i]);
# else
            for (int i = 0; i < count; i++)
                tmp[i] = MTVertex(verts[i]);
# endif

            //change the stored format
            format = fmt;
            stride = sizeof(MTVertex);

            //destroy the source vertex array
            bufferFree(verts);

            //take the new vertex data from the temp array
            MTVerts = tmp;

            break;
        }

        case VTXFMT_MESH:{
            RenderVertexN1 *    tmp;


            bufferAlloc(&tmp, count);

            for (int i = 0; i < count; i++)
                tmp[i] = RenderVertexN1(verts[i]);


            //change the stored format
            format = fmt;
            stride = sizeof(RenderVertexN1);

            //destroy the source vertex array
            bufferFree(verts);

            //take the new vertex data from the temp array
            RN1Verts = tmp;

            break;
        }

        case VTXFMT_BONE:{
            BoneVertex4 *   tmp;


            bufferAlloc(&tmp, count);

            for (int i = 0; i < count; i++){
                tmp[i] = BoneVertex4(verts[i]);
                //!!// figure out a way to pass the weights into this function!
                tmp[i].Weights[0] = 1;
            }


            //change the stored format
            format = fmt;
            stride = sizeof(BoneVertex4);

            //destroy the source vertex array
            bufferFree(verts);

            //take the new vertex data from the temp array
            boneVerts = tmp;

            break;
        }
#endif
    }

    return true;
}

void VertexList::getClosestPoint(const Vector &sourcePoint, const Vector &start, float& distance, Vector& closestPoint) const{
    int     i;


    Assert(count);

    switch (format){
        case VTXFMT_NONE:
            closestPoint = start;
            distance = (closestPoint - sourcePoint).MagnitudeSquared();

            for (i = 1; i < count; ++i){
                float newDist = (verts[i].Pos - sourcePoint).MagnitudeSquared();
                if (newDist < distance){
                    distance = newDist;
                    closestPoint = verts[i].Pos;
                }
            }
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            closestPoint = start;
            distance = (closestPoint - sourcePoint).MagnitudeSquared();

            for (i = 1; i < count; ++i){
                float newDist = (MT.positions[i] - sourcePoint).MagnitudeSquared();
                if (newDist < distance){
                    distance = newDist;
                    closestPoint = MT.positions[i];
                }
            }
            break;

        case VTXFMT_MESH:
            closestPoint = start;
            distance = (closestPoint - sourcePoint).MagnitudeSquared();

            for (i = 1; i < count; ++i){
                float newDist = (RN1.positions[i] - sourcePoint).MagnitudeSquared();
                if (newDist < distance){
                    distance = newDist;
                    closestPoint = RN1.positions[i];
                }
            }
            break;

        case VTXFMT_BONE:
            closestPoint = start;
            distance = (closestPoint - sourcePoint).MagnitudeSquared();

            for (i = 1; i < count; ++i){
                float newDist = (bone.positions[i] - sourcePoint).MagnitudeSquared();
                if (newDist < distance){
                    distance = newDist;
                    closestPoint = bone.positions[i];
                }
            }
            break;
#else
        case VTXFMT_MT:
            closestPoint = start;
            distance = (closestPoint - sourcePoint).MagnitudeSquared();

            for (i = 1; i < count; ++i){
                float newDist = (MTVerts[i].Pos - sourcePoint).MagnitudeSquared();
                if (newDist < distance){
                    distance = newDist;
                    closestPoint = MTVerts[i].Pos;
                }
            }
            break;

        case VTXFMT_MESH:
            closestPoint = start;
            distance = (closestPoint - sourcePoint).MagnitudeSquared();

            for (i = 1; i < count; ++i){
                float newDist = (RN1Verts[i].Pos - sourcePoint).MagnitudeSquared();
                if (newDist < distance){
                    distance = newDist;
                    closestPoint = RN1Verts[i].Pos;
                }
            }
            break;

        case VTXFMT_BONE:
            closestPoint = start;
            distance = (closestPoint - sourcePoint).MagnitudeSquared();

            for (i = 1; i < count; ++i){
                float newDist = (boneVerts[i].Pos - sourcePoint).MagnitudeSquared();
                if (newDist < distance){
                    distance = newDist;
                    closestPoint = boneVerts[i].Pos;
                }
            }
            break;
#endif
    }

    distance = (float)sqrt(distance);
}


void VertexList::getClosestPoint2(const Vector &hitPoint, float &distance, Vector &closestPoint) const{
    Vector4 closestFromLine4;
    Vector  closestFromLine;
    int     i, j;
    Vector4 hitPoint4 = hitPoint;

    
    switch (format){
        case VTXFMT_NONE:
            for (i = 0; i < count; i++){
                Vector &pos = verts[i].Pos;
                Vector  toHP2 = hitPoint - pos;


                for (j = i + 1; j < count; j++){
                    Vector &    pos2 = verts[j].Pos;
                    Vector      line = pos - pos2;
                    Vector      toHP1 = hitPoint - pos2;


                    if ((line.Dot(toHP1) > 0 ) && (line.Dot(toHP2) < 0)){
                        closestFromLine.SetToClosestFromLine(hitPoint, pos, pos2);
                        float magn = (closestFromLine - hitPoint).MagnitudeSquared();

                        if (magn < distance){
                            distance = magn;
                            closestPoint = closestFromLine;
                        }
                    }
                }
            }
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MESH:
            for (i = 0; i < count; i++){
                Vector4 &pos = RN1.positions[i];
                Vector4  toHP2 = hitPoint4 - pos;


                for( int j = i+1; j < count; j++){
                    Vector4 &    pos2 = RN1.positions[j];
                    Vector4      line = pos - pos2;
                    Vector4      toHP1 = hitPoint4 - pos2;


                    if ((line.Dot(toHP1) > 0 ) && (line.Dot(toHP2) < 0)){
                        closestFromLine4.SetToClosestFromLine(hitPoint4, pos, pos2);
                        float magn = (closestFromLine4 - hitPoint4).MagnitudeSquared();


                        if (magn < distance){
                            distance = magn;
                            closestPoint = closestFromLine4;
                        }
                    }
                }
            }
            break;

        case VTXFMT_MT:
            for (i = 0; i < count; i++){
                Vector4 &pos = MT.positions[i];
                Vector4  toHP2 = hitPoint4 - pos;


                for( int j = i+1; j < count; j++){
                    Vector4 &    pos2 = MT.positions[j];
                    Vector4      line = pos - pos2;
                    Vector4      toHP1 = hitPoint4 - pos2;


                    if ((line.Dot(toHP1) > 0 ) && (line.Dot(toHP2) < 0)){
                        closestFromLine4.SetToClosestFromLine(hitPoint4, pos, pos2);
                        float magn = (closestFromLine4 - hitPoint4).MagnitudeSquared();


                        if (magn < distance){
                            distance = magn;
                            closestPoint = closestFromLine4;
                        }
                    }
                }
            }
            break;

        case VTXFMT_BONE:
            for (i = 0; i < count; i++){
                Vector4 &pos = bone.positions[i];
                Vector4  toHP2 = hitPoint4 - pos;


                for( int j = i+1; j < count; j++){
                    Vector4 &    pos2 = bone.positions[j];
                    Vector4      line = pos - pos2;
                    Vector4      toHP1 = hitPoint4 - pos2;


                    if ((line.Dot(toHP1) > 0 ) && (line.Dot(toHP2) < 0)){
                        closestFromLine4.SetToClosestFromLine(hitPoint4, pos, pos2);
                        float magn = (closestFromLine4 - hitPoint4).MagnitudeSquared();


                        if (magn < distance){
                            distance = magn;
                            closestPoint = closestFromLine4;
                        }
                    }
                }
            }
            break;
#else
        case VTXFMT_MESH:
            for (i = 0; i < count; i++){
                Vector &pos = RN1Verts[i].Pos;
                Vector  toHP2 = hitPoint - pos;


                for (j = i + 1; j < count; j++){
                    Vector &    pos2 = RN1Verts[j].Pos;
                    Vector      line = pos - pos2;
                    Vector      toHP1 = hitPoint - pos2;


                    if ((line.Dot(toHP1) > 0 ) && (line.Dot(toHP2) < 0)){
                        closestFromLine.SetToClosestFromLine(hitPoint, pos, pos2);
                        float magn = (closestFromLine - hitPoint).MagnitudeSquared();


                        if (magn < distance){
                            distance = magn;
                            closestPoint = closestFromLine;
                        }
                    }
                }
            }
            break;

        case VTXFMT_MT:
            for (i = 0; i < count; i++){
                Vector &pos = MTVerts[i].Pos;
                Vector  toHP2 = hitPoint - pos;


                for (j = i + 1; j < count; j++){
                    Vector &    pos2 = MTVerts[j].Pos;
                    Vector      line = pos - pos2;
                    Vector      toHP1 = hitPoint - pos2;


                    if ((line.Dot(toHP1) > 0 ) && (line.Dot(toHP2) < 0)){
                        closestFromLine.SetToClosestFromLine(hitPoint, pos, pos2);
                        float magn = (closestFromLine - hitPoint).MagnitudeSquared();


                        if (magn < distance){
                            distance = magn;
                            closestPoint = closestFromLine;
                        }
                    }
                }
            }
            break;

        case VTXFMT_BONE:
            for (i = 0; i < count; i++){
                Vector &pos = boneVerts[i].Pos;
                Vector  toHP2 = hitPoint - pos;


                for (j = i + 1; j < count; j++){
                    Vector &    pos2 = boneVerts[j].Pos;
                    Vector      line = pos - pos2;
                    Vector      toHP1 = hitPoint - pos2;


                    if ((line.Dot(toHP1) > 0 ) && (line.Dot(toHP2) < 0)){
                        closestFromLine.SetToClosestFromLine(hitPoint, pos, pos2);
                        float magn = (closestFromLine - hitPoint).MagnitudeSquared();


                        if (magn < distance){
                            distance = magn;
                            closestPoint = closestFromLine;
                        }
                    }
                }
            }
            break;
#endif
    }
}


void VertexList::scaleVerts(float scale, BoundingBox *box){
    int i;


    switch (format){
        case VTXFMT_NONE:
		    for (i = 0; i < count; i++){
                verts[i].Pos *= scale;
                if (box)
                    (*box) += verts[i].Pos;
            }
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
		    for (i = 0; i < count; i++){
                MT.positions[i] *= scale;
                if (box)
                    (*box) += MT.positions[i];
            }
            break;

        case VTXFMT_MESH:
		    for (i = 0; i < count; i++){
                RN1.positions[i] *= scale;
                if (box)
                    (*box) += RN1.positions[i];
            }
            break;

        case VTXFMT_BONE:
		    for (i = 0; i < count; i++){
                bone.positions[i] *= scale;
                if (box)
                    (*box) += bone.positions[i];
            }
            break;

#else
        case VTXFMT_MT:
		    for (i = 0; i < count; i++){
                verts[i].Pos *= scale;
                if (box)
                    (*box) += verts[i].Pos;
            }
            break;

        case VTXFMT_MESH:
		    for (i = 0; i < count; i++){
                RN1Verts[i].Pos *= scale;
                if (box)
                    (*box) += RN1Verts[i].Pos;
            }
            break;

        case VTXFMT_BONE:
		    for (i = 0; i < count; i++){
                boneVerts[i].Pos *= scale;
                if (box)
                    (*box) += boneVerts[i].Pos;
            }
            break;
#endif
    }
}

void VertexList::transformToObjectSpace(const Matrix &m, BoundingBox *box) const{

    //transformation matrix is the identity -> nothing to do => succeed
    if (m.IsIdentity())
        return;
    

	Matrix  WorldToObject = m.Inverse();
    int     i;


    switch (format){
        case VTXFMT_NONE:
            for (i = 0; i < count; i++){
                verts[i].Pos = WorldToObject.TransformCoord(verts[i].Pos);
                if (box)
                    (*box) += verts[i].Pos;
            }
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            for (i = 0; i < count; i++){
                MT.positions[i] = WorldToObject.TransformCoord(MT.positions[i]);
                if (box)
                    (*box) += MT.positions[i];
            }
            break;

        case VTXFMT_MESH:
            for (i = 0; i < count; i++){
                RN1.positions[i] = WorldToObject.TransformCoord(RN1.positions[i]);
                if (box)
                    (*box) += RN1.positions[i];
            }
            break;

        case VTXFMT_BONE:
            for (i = 0; i < count; i++){
                bone.positions[i] = WorldToObject.TransformCoord(bone.positions[i]);
                if (box)
                    (*box) += bone.positions[i];
            }
            break;

#else
        case VTXFMT_MT:
            for (i = 0; i < count; i++){
                MTVerts[i].Pos = WorldToObject.TransformCoord(MTVerts[i].Pos);
                if (box)
                    (*box) += MTVerts[i].Pos;
            }
            break;

        case VTXFMT_MESH:
            for (i = 0; i < count; i++){
                RN1Verts[i].Pos = WorldToObject.TransformCoord(RN1Verts[i].Pos);
                if (box)
                    (*box) += RN1Verts[i].Pos;
            }
            break;

        case VTXFMT_BONE:
            for (i = 0; i < count; i++){
                boneVerts[i].Pos = WorldToObject.TransformCoord(boneVerts[i].Pos);
                if (box)
                    (*box) += boneVerts[i].Pos;
            }
            break;
#endif
    }
}


Vector VertexList::centerGeometry(BoundingBox &box){
    int         i;
    Vector      tr3;
    Vector4     tr4;


    computeBox(box);
	tr3 = box.Max - (box.Max - box.Min) / 2;
    tr4 = tr3;
    box.Reset();

    switch (format){
        case VTXFMT_NONE:
            for (i = 0; i < count; i++){
                verts[i].Pos -= tr3;
                box += verts[i].Pos;
            }
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            for (i = 0; i < count; i++){
                MT.positions[i] -= tr4;
                box += MT.positions[i];
            }
            break;

        case VTXFMT_MESH:
            for (i = 0; i < count; i++){
                RN1.positions[i] -= tr4;
                box += RN1.positions[i];
            }
            break;

        case VTXFMT_BONE:
            for (i = 0; i < count; i++){
                bone.positions[i] -= tr4;
                box += bone.positions[i];
            }
            break;

#else
        case VTXFMT_MT:
            for (i = 0; i < count; i++){
                MTVerts[i].Pos -= tr3;
                box += MTVerts[i].Pos;
            }
            break;

        case VTXFMT_MESH:
            for (i = 0; i < count; i++){
                RN1Verts[i].Pos -= tr3;
                box += RN1Verts[i].Pos;
            }
            break;

        case VTXFMT_BONE:
            for (i = 0; i < count; i++){
                boneVerts[i].Pos -= tr3;
                box += boneVerts[i].Pos;
            }
            break;
#endif
    }

    return tr3;
}

/*
    switch (format){
        case VTXFMT_NONE:
#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
        case VTXFMT_MESH:
        case VTXFMT_BONE:
#else
        case VTXFMT_MT:
        case VTXFMT_MESH:
        case VTXFMT_BONE:
#endif
    }

*/

void VertexList::computeBox(BoundingBox &box) const{
    int i;


    box.Reset();


    switch (format){
        case VTXFMT_NONE:
		    for (i = 0; i < count; i++)
                box += verts[i].Pos;
            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
		    for (i = 0; i < count; i++)
                box += MT.positions[i];
            break;

        case VTXFMT_MESH:
		    for (i = 0; i < count; i++)
                box += RN1.positions[i];
            break;

        case VTXFMT_BONE:
		    for (i = 0; i < count; i++)
                box += bone.positions[i];
            break;

#else
        case VTXFMT_MT:
		    for (i = 0; i < count; i++)
                box += verts[i].Pos;
            break;

        case VTXFMT_MESH:
		    for (i = 0; i < count; i++)
                box += RN1Verts[i].Pos;
            break;

        case VTXFMT_BONE:
		    for (i = 0; i < count; i++)
                box += boneVerts[i].Pos;
            break;
#endif
    }
}


Vector VertexList::getRandomPoint() const{
    Vector point(0,0,0);

    switch (format){
        case VTXFMT_NONE:
            if (count > 0 )
                point = verts[rand() % count].Pos;

            break;

#ifdef TRANSGAMING_PS2
        case VTXFMT_MT:
            if (count > 0 )
                point = MT.positions[rand() % count];

            break;

        case VTXFMT_MESH:
            if (count > 0 )
                point = RN1.positions[rand() % count];

            break;

        case VTXFMT_BONE:
            if (count > 0 )
                point = bone.positions[rand() % count];

            break;
#else
        case VTXFMT_MT:
            if (count > 0 )
                point = MTVerts[rand() % count].Pos;

            break;

        case VTXFMT_MESH:
            if (count > 0 )
                point = RN1Verts[rand() % count].Pos;

            break;

        case VTXFMT_BONE:
            if (count > 0 )
                point = boneVerts[rand() % count].Pos;

            break;
#endif
    }

    return point;
}

Plane VertexList::getReflectionPlane(const DynamicArray<WORD> *triangles) const{
    if (triangles == NULL){
        switch (format){
            case VTXFMT_NONE:{
                const Vector& P1 = verts[2].Pos;
                const Vector& P2 = verts[1].Pos;
                const Vector& P3 = verts[0].Pos;

                return Plane(P1,P2,P3);
            }

#ifdef TRANSGAMING_PS2
            case VTXFMT_MT:{
                const Vector4& P1 = MT.positions[2];
                const Vector4& P2 = MT.positions[1];
                const Vector4& P3 = MT.positions[0];

                return Plane(P1,P2,P3);
            }

            case VTXFMT_MESH:{
                const Vector4& P1 = RN1.positions[2];
                const Vector4& P2 = RN1.positions[1];
                const Vector4& P3 = RN1.positions[0];

                return Plane(P1,P2,P3);
            }

            case VTXFMT_BONE:{
                const Vector4& P1 = bone.positions[2];
                const Vector4& P2 = bone.positions[1];
                const Vector4& P3 = bone.positions[0];

                return Plane(P1,P2,P3);
            }
#else
            case VTXFMT_MT:{
                const Vector& P1 = MTVerts[2].Pos;
                const Vector& P2 = MTVerts[1].Pos;
                const Vector& P3 = MTVerts[0].Pos;

                return Plane(P1,P2,P3);
            }

            case VTXFMT_MESH:{
                const Vector& P1 = RN1Verts[2].Pos;
                const Vector& P2 = RN1Verts[1].Pos;
                const Vector& P3 = RN1Verts[0].Pos;

                return Plane(P1,P2,P3);
            }

            case VTXFMT_BONE:{
                const Vector& P1 = boneVerts[2].Pos;
                const Vector& P2 = boneVerts[1].Pos;
                const Vector& P3 = boneVerts[0].Pos;

                return Plane(P1,P2,P3);
            }
#endif
        }
    }

    else{
        Assert(triangles->Size() >= 3);


        switch (format){
            case VTXFMT_NONE:{
                const Vector& P1 = verts[(*triangles)[2]].Pos;
                const Vector& P2 = verts[(*triangles)[1]].Pos;
                const Vector& P3 = verts[(*triangles)[0]].Pos;

                return Plane(P1,P2,P3);
            }

#ifdef TRANSGAMING_PS2
            case VTXFMT_MT:{
                const Vector& P1 = MT.positions[(*triangles)[2]];
                const Vector& P2 = MT.positions[(*triangles)[1]];
                const Vector& P3 = MT.positions[(*triangles)[0]];

                return Plane(P1,P2,P3);
            }

            case VTXFMT_MESH:{
                const Vector& P1 = RN1.positions[(*triangles)[2]];
                const Vector& P2 = RN1.positions[(*triangles)[1]];
                const Vector& P3 = RN1.positions[(*triangles)[0]];

                return Plane(P1,P2,P3);
            }

            case VTXFMT_BONE:{
                const Vector& P1 = bone.positions[(*triangles)[2]];
                const Vector& P2 = bone.positions[(*triangles)[1]];
                const Vector& P3 = bone.positions[(*triangles)[0]];

                return Plane(P1,P2,P3);
            }
#else
            case VTXFMT_MT:{
                const Vector& P1 = MTVerts[(*triangles)[2]].Pos;
                const Vector& P2 = MTVerts[(*triangles)[1]].Pos;
                const Vector& P3 = MTVerts[(*triangles)[0]].Pos;

                return Plane(P1,P2,P3);
            }

            case VTXFMT_MESH:{
                const Vector& P1 = RN1Verts[(*triangles)[2]].Pos;
                const Vector& P2 = RN1Verts[(*triangles)[1]].Pos;
                const Vector& P3 = RN1Verts[(*triangles)[0]].Pos;

                return Plane(P1,P2,P3);
            }

            case VTXFMT_BONE:{
                const Vector& P1 = boneVerts[(*triangles)[2]].Pos;
                const Vector& P2 = boneVerts[(*triangles)[1]].Pos;
                const Vector& P3 = boneVerts[(*triangles)[0]].Pos;

                return Plane(P1,P2,P3);
            }
#endif
        }
    }

    return Plane(0, 0, 0, 0);
}

#ifdef TRANSGAMING_PS2
void VertexList::fixTexCoords(DynamicArray<WORD> *triangles, int uvSetCount, int primType){
    TexCoord    min, max;
    TexCoord    range;
    TexCoord *  tu;
    int         x;
    int         y;
    int         stride;


    Assert(format == VTXFMT_MESH || format == VTXFMT_MT || format == VTXFMT_NONE);

    //can't deal with interleaved vertex data for now
    if (format == VTXFMT_NONE){
        fixTexCoordsNone(triangles, uvSetCount, primType);

        return;
    }


    tu = getTexCoord1(&stride);
    Assert(tu);

    min.X = min.Y = 2048.0f;
    max.X = max.Y = -2048.0f;

    if (uvSetCount == 1){
        for (int i = 0; i < count; i++){
            if (tu[i].X < min.X)  min.X = tu[i].X;
            if (tu[i].Y < min.Y)  min.Y = tu[i].Y;
            if (tu[i].X > max.X)  max.X = tu[i].X;
            if (tu[i].Y > max.Y)  max.Y = tu[i].Y;
        }


        range.X = max.X - min.X;
        range.Y = max.Y - min.Y;

        if (range.X < 16.0f && range.Y < 16.0f){
            x = (int)((range.X * 0.5f) + min.X);
            y = (int)((range.Y * 0.5f) + min.Y);


            for (int i = 0; i < count; i++){
                tu[i].X -= x;
                tu[i].Y -= y;
            }
        }

        else{
            TRACE("VertexList::fixTexCoords(): WARNING-> the texture coordinate range is too large!  Attempting to rescale... (fmt = TU1, range = <%g, %g>)\n", range.X, range.Y);

            if (primType == RenderDevice::EPrimTriList){
                Assert(triangles);

                DynamicArray<TexRange>  tc(count);
                DynamicArray<WORD> &    tris = *triangles;


                //initialize the neighbour ranges
                for (int i = 0; i < count; i++){
                    tc[i].min.X = tc[i].min.Y = 2048.0f;
                    tc[i].max.X = tc[i].max.Y = -2048.0f;
                }


                //loop through the index and find the range for each vertex
                for (int i = 0; i < tris.Size(); i += 3){
                    tc[tris[i + 0]].updateRange(tu[tris[i + 0]]);
                    tc[tris[i + 0]].updateRange(tu[tris[i + 1]]);
                    tc[tris[i + 0]].updateRange(tu[tris[i + 2]]);

                    tc[tris[i + 1]].updateRange(tu[tris[i + 0]]);
                    tc[tris[i + 1]].updateRange(tu[tris[i + 1]]);
                    tc[tris[i + 1]].updateRange(tu[tris[i + 2]]);

                    tc[tris[i + 2]].updateRange(tu[tris[i + 0]]);
                    tc[tris[i + 2]].updateRange(tu[tris[i + 1]]);
                    tc[tris[i + 2]].updateRange(tu[tris[i + 2]]);
                }


                //loop through the verticies and update the 
                for (int i = 0; i < count; i++){
                    x = (int)(((tc[i].max.X - tc[i].min.X) * 0.5f) + tc[i].min.X);
                    y = (int)(((tc[i].max.Y - tc[i].min.Y) * 0.5f) + tc[i].min.Y);
                    
                    tu[i].X -= x;
                    tu[i].Y -= y;
                }
            }

            else if (primType == RenderDevice::EPrimTriStrip)
                TRACE("VertexList::fixTexCoords(): WARNING-> texcoords fixup isn't implemented for tristrips! (fmt = TU1)\n");
        }
    }

    else{
        for (int i = 0; i < count; i++){
            if (tu[i].X < min.X)  min.X = tu[i].X;
            if (tu[i].Y < min.Y)  min.Y = tu[i].Y;
            if (tu[i].X > max.X)  max.X = tu[i].X;
            if (tu[i].Y > max.Y)  max.Y = tu[i].Y;
        }


        range.X = max.X - min.X;
        range.Y = max.Y - min.Y;

        if (range.X < 16.0f && range.Y < 16.0f){
            x = (int)((range.X * 0.5f) + min.X);
            y = (int)((range.Y * 0.5f) + min.Y);


            for (int i = 0; i < count; i++){
                tu[i].X -= x;
                tu[i].Y -= y;
            }
        }

        else{
            TRACE("VertexList::fixTexCoords(): ERROR-> the texture coordinate range is too large!  Attempting to rescale... (fmt = TU2, range = <%g, %g>)\n", range.X, range.Y);

            if (primType == RenderDevice::EPrimTriList){
                Assert(triangles);

                DynamicArray<TexRange>  tc(count);
                DynamicArray<WORD> &    tris = *triangles;


                TRACE("VertexList::fixTexCoords(): verts.size() = %d, tc.size() = %d\n", count, tc.Size());

                //initialize the neighbour ranges
                for (int i = 0; i < count; i++){
                    tc[i].min.X = tc[i].min.Y = 2048.0f;
                    tc[i].max.X = tc[i].max.Y = -2048.0f;
                }


                TRACE("VertexList::fixTexCoords(): finding coordinate range for %d triangles\n", tris.Size());
                //loop through the index and find the range for each vertex
                for (int i = 0; i < tris.Size(); i += 3){
                    tc[tris[i + 0]].updateRange(tu[tris[i + 0]]);
                    tc[tris[i + 0]].updateRange(tu[tris[i + 1]]);
                    tc[tris[i + 0]].updateRange(tu[tris[i + 2]]);

                    tc[tris[i + 1]].updateRange(tu[tris[i + 0]]);
                    tc[tris[i + 1]].updateRange(tu[tris[i + 1]]);
                    tc[tris[i + 1]].updateRange(tu[tris[i + 2]]);

                    tc[tris[i + 2]].updateRange(tu[tris[i + 0]]);
                    tc[tris[i + 2]].updateRange(tu[tris[i + 1]]);
                    tc[tris[i + 2]].updateRange(tu[tris[i + 2]]);
                }


                TRACE("VertexList::fixTexCoords(): updating coordinates for %d verts\n", count);
                //loop through the verticies and update the 
                for (int i = 0; i < count; i++){
                    x = (int)(((tc[i].max.X - tc[i].min.X) * 0.5f) + tc[i].min.X);
                    y = (int)(((tc[i].max.Y - tc[i].min.Y) * 0.5f) + tc[i].min.Y);

                    TRACE(" %4d) <x, y> = <%d, %d>, min = <%g, %g>, max = <%g, %g>, dT = <%g, %g> -> <%g, %g>\n", i, x, y, tc[i].min.X, tc[i].min.Y, tc[i].max.X, tc[i].max.Y, tu[i].X, tu[i].Y, tu[i].X - x, tu[i].Y - y);
                    tu[i].X -= x;
                    tu[i].Y -= y;
                }
            }

            else if (primType == RenderDevice::EPrimTriStrip)
                TRACE("VertexList::fixTexCoords(): ERROR-> texcoords fixup isn't implemented for tristrips! (fmt = TU2)\n");
        }
    }
}


void VertexList::fixTexCoordsNone(DynamicArray<WORD> *triangles, int uvSetCount, int primType){
    TexCoord    min, max;
    TexCoord    range;
    int         x;
    int         y;
    int         stride;


    Assert(format == VTXFMT_NONE);


    min.X = min.Y = 2048.0f;
    max.X = max.Y = -2048.0f;

    if (uvSetCount == 1){
        for (int i = 0; i < count; i++){
            if (verts[i].TU.X < min.X)  min.X = verts[i].TU.X;
            if (verts[i].TU.Y < min.Y)  min.Y = verts[i].TU.Y;
            if (verts[i].TU.X > max.X)  max.X = verts[i].TU.X;
            if (verts[i].TU.Y > max.Y)  max.Y = verts[i].TU.Y;
        }


        range.X = max.X - min.X;
        range.Y = max.Y - min.Y;

        if (range.X < 16.0f && range.Y < 16.0f){
            x = (int)((range.X * 0.5f) + min.X);
            y = (int)((range.Y * 0.5f) + min.Y);


            for (int i = 0; i < count; i++){
                verts[i].TU.X -= x;
                verts[i].TU.Y -= y;
            }
        }

        else{
            TRACE("VertexList::fixTexCoords(): WARNING-> the texture coordinate range is too large!  Attempting to rescale... (fmt = TU1, range = <%g, %g>)\n", range.X, range.Y);

            if (primType == RenderDevice::EPrimTriList){
                Assert(triangles);

                DynamicArray<TexRange>  tc(count);
                DynamicArray<WORD> &    tris = *triangles;


                //initialize the neighbour ranges
                for (int i = 0; i < count; i++){
                    tc[i].min.X = tc[i].min.Y = 2048.0f;
                    tc[i].max.X = tc[i].max.Y = -2048.0f;
                }


                //loop through the index and find the range for each vertex
                for (int i = 0; i < tris.Size(); i += 3){
                    tc[tris[i + 0]].updateRange(verts[tris[i + 0]].TU);
                    tc[tris[i + 0]].updateRange(verts[tris[i + 1]].TU);
                    tc[tris[i + 0]].updateRange(verts[tris[i + 2]].TU);

                    tc[tris[i + 1]].updateRange(verts[tris[i + 0]].TU);
                    tc[tris[i + 1]].updateRange(verts[tris[i + 1]].TU);
                    tc[tris[i + 1]].updateRange(verts[tris[i + 2]].TU);

                    tc[tris[i + 2]].updateRange(verts[tris[i + 0]].TU);
                    tc[tris[i + 2]].updateRange(verts[tris[i + 1]].TU);
                    tc[tris[i + 2]].updateRange(verts[tris[i + 2]].TU);
                }


                //loop through the verticies and update the 
                for (int i = 0; i < count; i++){
                    x = (int)(((tc[i].max.X - tc[i].min.X) * 0.5f) + tc[i].min.X);
                    y = (int)(((tc[i].max.Y - tc[i].min.Y) * 0.5f) + tc[i].min.Y);
                    
                    verts[i].TU.X -= x;
                    verts[i].TU.Y -= y;
                }
            }

            else if (primType == RenderDevice::EPrimTriStrip)
                TRACE("VertexList::fixTexCoords(): WARNING-> texcoords fixup isn't implemented for tristrips! (fmt = TU1)\n");
        }
    }

    else{
        for (int i = 0; i < count; i++){
            if (verts[i].TU1.X < min.X)  min.X = verts[i].TU1.X;
            if (verts[i].TU1.Y < min.Y)  min.Y = verts[i].TU1.Y;
            if (verts[i].TU1.X > max.X)  max.X = verts[i].TU1.X;
            if (verts[i].TU1.Y > max.Y)  max.Y = verts[i].TU1.Y;
        }


        range.X = max.X - min.X;
        range.Y = max.Y - min.Y;

        if (range.X < 16.0f && range.Y < 16.0f){
            x = (int)((range.X * 0.5f) + min.X);
            y = (int)((range.Y * 0.5f) + min.Y);


            for (int i = 0; i < count; i++){
                verts[i].TU1.X -= x;
                verts[i].TU1.Y -= y;
            }
        }

        else{
            TRACE("VertexList::fixTexCoords(): ERROR-> the texture coordinate range is too large!  Attempting to rescale... (fmt = TU2, range = <%g, %g>)\n", range.X, range.Y);

            if (primType == RenderDevice::EPrimTriList){
                Assert(triangles);

                DynamicArray<TexRange>  tc(count);
                DynamicArray<WORD> &    tris = *triangles;


                TRACE("VertexList::fixTexCoords(): verts.size() = %d, tc.size() = %d\n", count, tc.Size());

                //initialize the neighbour ranges
                for (int i = 0; i < count; i++){
                    tc[i].min.X = tc[i].min.Y = 2048.0f;
                    tc[i].max.X = tc[i].max.Y = -2048.0f;
                }


                TRACE("VertexList::fixTexCoords(): finding coordinate range for %d triangles\n", tris.Size());
                //loop through the index and find the range for each vertex
                for (int i = 0; i < tris.Size(); i += 3){
                    tc[tris[i + 0]].updateRange(verts[tris[i + 0]].TU1);
                    tc[tris[i + 0]].updateRange(verts[tris[i + 1]].TU1);
                    tc[tris[i + 0]].updateRange(verts[tris[i + 2]].TU1);

                    tc[tris[i + 1]].updateRange(verts[tris[i + 0]].TU1);
                    tc[tris[i + 1]].updateRange(verts[tris[i + 1]].TU1);
                    tc[tris[i + 1]].updateRange(verts[tris[i + 2]].TU1);

                    tc[tris[i + 2]].updateRange(verts[tris[i + 0]].TU1);
                    tc[tris[i + 2]].updateRange(verts[tris[i + 1]].TU1);
                    tc[tris[i + 2]].updateRange(verts[tris[i + 2]].TU1);
                }


                TRACE("VertexList::fixTexCoords(): updating coordinates for %d verts\n", count);
                //loop through the verticies and update the 
                for (int i = 0; i < count; i++){
                    x = (int)(((tc[i].max.X - tc[i].min.X) * 0.5f) + tc[i].min.X);
                    y = (int)(((tc[i].max.Y - tc[i].min.Y) * 0.5f) + tc[i].min.Y);

                    TRACE(" %4d) <x, y> = <%d, %d>, min = <%g, %g>, max = <%g, %g>, dT = <%g, %g> -> <%g, %g>\n", i, x, y, tc[i].min.X, tc[i].min.Y, tc[i].max.X, tc[i].max.Y, verts[i].TU1.X, verts[i].TU1.Y, verts[i].TU1.X - x, verts[i].TU1.Y - y);
                    verts[i].TU1.X -= x;
                    verts[i].TU1.Y -= y;
                }
            }

            else if (primType == RenderDevice::EPrimTriStrip)
                TRACE("VertexList::fixTexCoords(): ERROR-> texcoords fixup isn't implemented for tristrips! (fmt = TU2)\n");
        }
    }
}
#endif
