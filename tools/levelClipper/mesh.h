#ifndef  __MESH_H__
# define __MESH_H__

# include <stdio.h>
# include "stdTypes.h"
# include "vector.h"
# include "material.h"


class Mesh{
public:
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

    typedef enum{
        //major formats
        VTXFMT_NONE,
        VTXFMT_MT,
        VTXFMT_PSP,
        VTXFMT_PS2,
    } VertexFormat;


    typedef enum{
        TYPE_NORMAL,
        TYPE_PORTAL,
        TYPE_ANTIPORTAL,
        TYPE_ANTIPORTALSWITCH,
        TYPE_ZONE,
        TYPE_DEATHZONE,
        TYPE_ICEZONE,
        TYPE_LADDERZONE,
        TYPE_SWITCHZONE,
        TYPE_VOLUMELIGHT,
        TYPE_VOLUMEFOG,
        TYPE_GLASS,
        TYPE_BARRIER,
        TYPE_ACTIVE,
    } MeshType;


    class Group{
    private:
        friend class Mesh;

        Vector          staticBoundingBox[8];
        int16           renderBase;
        int16           renderCount;
        char *          texName;
        MaterialDef     colorMap;
        MaterialDef     lightMap;
        MaterialDef     blendMap;
        MaterialDef     alphaMap;

    public:
        Group();
        ~Group();

        Group &             operator = (const Group &g);
        const Vector *      getBoundingBox() const;
        int16               getRenderBase() const;
        int16               getRenderCount() const;
        const char *        getTexName() const;
        const MaterialDef & getColorMap() const;
        const MaterialDef & getBlendMap() const;
    };


private:
    char *              name;
    char *              alphaMapName;
    char *              lightMapName;
    
    Box                 boundingBox;
    Matrix              transform;
    
    union{
        MeshVertex *    verts;
        PS2Vertex *     ps2verts;
        PSPVertex *     pspverts;
        MTVertex *      mtverts;
        void *          voidverts;
    };
    size_t              vertexStride;
    VertexFormat        vertexFormat;
    int32               vertexCount;

    uint16 *            triangles;
    int32               triangleCount;

    int32               uvCount;
    bool                stripped;
    bool                indexed;
    bool                mtformat;
    bool                hasBoundingBox;
    bool                hasVolumeInfo;

    Group *             groups;
    int32               groupCount;

    MeshType            type;

    struct VolumeParams{
        Color           color;
        float32         end;
    } volumeParams;


    void calcBoundingBox();
    void calcStaticBoundingBoxes();
    void scaleVerts(float scale);
public:
    Mesh();
    Mesh(FILE *fp, float scale);
    ~Mesh();


    Mesh &              operator = (const Mesh &m);
    void                clear();

    bool                load(FILE *fp, float scale);
    int32               save(FILE *fp, float scale) const;
    bool                convert(VertexFormat fmt);

    const char *        getName() const;

    bool                isIndexed() const;
    bool                isStripped() const;
    bool                isMTFormat() const;
    int32               getUVCount() const;

    const uint16 *      getTriangles() const;
    const int32         getTriangleCount() const;

    VertexFormat        getVertFormat() const;
    const int32         getVertCount() const;
    const size_t        getVertStride() const;

    Box                 getBoundingBox() const;
    Matrix              getTransform() const;

    int32               getGroupCount() const;
    const Group *       getGroup(int32 i) const;

    MeshType            getType() const;
    void                setType(MeshType t);

    void                setVolumeParams(Color _color, float32 _end);
};


#endif
