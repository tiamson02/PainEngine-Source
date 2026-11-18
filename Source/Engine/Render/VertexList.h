#ifndef  __VERTEXLIST_H__
# define __VERTEXLIST_H__

# include "Engine.h"
# include "Vertex.h"
# include "Core/Debug.h"
# include "Core/DynArray.h"


class FileObject;


# ifdef TRANSGAMING_PS2
#  define WEIGHT_TYPE     float
#  define WEIGHT_COUNT    1
#  define WEIGHT_SCALE    1.0f
# else
#  define WEIGHT_TYPE     unsigned char
#  define WEIGHT_COUNT    4
#  define WEIGHT_SCALE    0xff
# endif


class VertexList{
public:
    typedef enum{
        VTXFMT_NONE,
        VTXFMT_MESH,
        VTXFMT_MT,
        VTXFMT_BONE,

        VTXFMT_COUNT,
    } VertexFormat;


#ifdef TRANSGAMING_PSP
    typedef BoneVertex<WEIGHT_TYPE, WEIGHT_COUNT> BoneVertex4;
#endif

    //this value is the maximum number of separate streams that will be needed for any vertex format.
    //It also includes one extra stream just for padding (in case another stream needs to be manually
    //added later - ie: a normal stream).
    static const int MaxStreamCount = 5;


private:
#ifdef TRANSGAMING_PS2
        typedef struct{
            Vector4 *       positions;
# ifdef TRANSGAMING_KEEP_NORMALS
            Vector *        normals;
# endif
            TexCoord *      tex1;
            TexCoord *      tex2;
        } MTVertices;

        //RN1 vertices
        typedef struct{
            Vector4 *       positions;
            Vector *        normals;
            TexCoord *      tex1;
        } RN1Vertices;

        //Bone vertices
        typedef struct{
            Vector4 *       positions;
            Vector *        normals;
            TexCoord *      tex1;
            WEIGHT_TYPE *   weights;
        } BoneVertices;
#endif


    union{
        MeshVertex *        verts;
#ifdef TRANSGAMING_PSP
        //MT vertices
        MTVertex *          MTVerts;

        //RN1 vertices
        RenderVertexN1 *    RN1Verts;

        //Bone vertices
        BoneVertex4 *       boneVerts;
#elif defined(TRANSGAMING_PS2)
        //MT vertices
        MTVertices          MT;

        //RN1 vertices
        RN1Vertices         RN1;

        //Bone vertices
        BoneVertices        bone;
#endif
        void *              voidVerts;
    };

    int                     count;
    VertexFormat            format;
    int                     stride;


    static const char *     formatNames[VTXFMT_COUNT + 1];


    void clear();
#ifdef TRANSGAMING_PS2
    void fixTexCoordsNone(DynamicArray<WORD> *triangles, int uvSetCount, int primType);
#endif

public:
    VertexList();
    VertexList(FileObject& file, VertexFormat fmt);
    ~VertexList();

    //vertex data access functions
    inline int              Size() const{       return count; }
    inline VertexFormat     getFormat() const{  return format; }
//#ifdef TRANSGAMING_PSP
    inline void *           getVerts(int *_stride) const;
//#endif
    inline Vector           getVertPos(int i) const;
    inline void *           getVertPos(int i, int *_stride) const;
    inline void             setVertPos(int i, const Vector &v);
    const char *            getFormatName() const;
    const char *            getFormatName(const VertexFormat fmt) const;
    MeshVertex              getVert(int i) const;
    MeshVertex              operator[](int i) const;
    MeshVertex &            getVert(int i, MeshVertex &v) const;

    VertexList &            operator = (const VertexList &v);

    void                    setSize(int cnt, VertexFormat fmt);


    inline void setVert(int i, const MeshVertex &vtx);
#ifdef TRANSGAMING_PSP
    //inline void setVert(int i, const RenderVertexN1 &vtx);
    //inline void setVert(int i, const BoneVertex4 &vtx);
    //inline void setVert(int i, const MTVertex &vtx);
#endif

#ifdef TRANSGAMING_PS2
    //!!// HACK!! temporarily allow direct access to the verts in various formats
    //!!//   until HW skinning is implemented on both systems
    Vector4 *               getPositions(int *stride) const;
    Vector *                getNormals(int *stride) const;
    TexCoord *              getTexCoord1(int *stride) const;
    TexCoord *              getTexCoord2(int *stride) const;
    WEIGHT_TYPE *           getWeights(int *stride) const;
#endif

#ifdef TRANSGAMING_PSP
    //!!// HACK!! temporarily allow direct access to the verts in various formats
    //!!//   until HW skinning is implemented on both systems
    //const MeshVertex &      getMeshVert(int i) const{   Assert(format == VTXFMT_NONE);  Assert(i >= 0 && i < count); return verts[i];     }
    const RenderVertexN1 &  getRNVert(int i) const{     Assert(format == VTXFMT_MESH);  Assert(i >= 0 && i < count); return RN1Verts[i];  }
    //const MTVertex &        getMTVert(int i) const{     Assert(format == VTXFMT_MT);    Assert(i >= 0 && i < count); return MTVerts[i];   }
    //const BoneVertex4 &     getBoneVert(int i) const{   Assert(format == VTXFMT_BONE);  Assert(i >= 0 && i < count); return boneVerts[i]; }
#endif


    //copy this vertex array to another source that is in the named format
    void        copy(void *dst, VertexFormat fmt) const;
# ifdef TRANSGAMING_PS2
    void        copyPositions(void *dst, VertexFormat fmt) const;
    void        copyNormals(void *dst, VertexFormat fmt) const;
    void        copyTex1(void *dst, VertexFormat fmt) const;
    void        copyTex2(void *dst, VertexFormat fmt) const;
    void        copyWeights(void *dst, VertexFormat fmt) const;
# endif

    //vertex loading and saving functions
    bool        load(FileObject &file, VertexFormat fmt);
    bool        save(FileObject &file) const;

    //vertex format conversion function.  This should only be used until it exists in the exporter
    bool        convert(VertexFormat fmt, DynamicArray<Vector> *normals = NULL);

    //utility functions for the vertex list
    void        transformToObjectSpace(const Matrix &m, BoundingBox *box = NULL) const;
    Vector      centerGeometry(BoundingBox &box);
    void        computeBox(BoundingBox &box) const;
    void        scaleVerts(float scale, BoundingBox *box = NULL);
    void        getClosestPoint(const Vector &sourcePoint, const Vector &start, float& distance, Vector& closestPoint) const;
    void        getClosestPoint2(const Vector &hitPoint, float &distance, Vector &closestPoint) const;
    Vector      getRandomPoint() const;
    Plane       getReflectionPlane(const DynamicArray<WORD> *triangles = NULL) const;
# ifdef TRANSGAMING_PS2
    void        fixTexCoords(DynamicArray<WORD> *triangles, int uvSetCount, int primType);
# endif
};

# include "VertexList.inl"


#endif
