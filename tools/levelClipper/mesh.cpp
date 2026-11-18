#include <stdlib.h>
#include "mesh.h"
#include "fileUtil.h"
#include "trace.h"
#include "text.h"
#include <memory.h>

//******************************************************************************************************//
//********************************** Mesh::Group Implementation ****************************************//
//******************************************************************************************************//

//*************************** constructors & destructor *******************************//
Mesh::Group::Group(){
    renderBase = 0;
    renderCount = 0;
    texName = NULL;

    memset(staticBoundingBox, 0, sizeof(staticBoundingBox));
}

Mesh::Group::~Group(){
    Text::strcopy(&texName, NULL);
}


Mesh::Group &Mesh::Group::operator = (const Group &g){
    renderBase = g.renderBase;
    renderCount = g.renderCount;

    Text::strcopy(&texName, g.texName);

    colorMap = g.colorMap;
    lightMap = g.lightMap;
    alphaMap = g.alphaMap;
    blendMap = g.blendMap;

    memcpy(staticBoundingBox, g.staticBoundingBox, sizeof(staticBoundingBox));

    return *this;
}


//**************************** accessor functions *******************************//
const Vector *Mesh::Group::getBoundingBox() const{
    return staticBoundingBox;
}

int16 Mesh::Group::getRenderBase() const{
    return renderBase;
}

int16 Mesh::Group::getRenderCount() const{
    return renderCount;
}

const char *Mesh::Group::getTexName() const{
    return texName;
}

const MaterialDef &Mesh::Group::getColorMap() const{
    return colorMap;
}

const MaterialDef &Mesh::Group::getBlendMap() const{
    return blendMap;
}

//******************************************************************************************************//
//************************************* Mesh Implementation *******************************************//
//******************************************************************************************************//

//***************************** constructors & destructor ***********************************//
Mesh::Mesh(){
    triangleCount = 0;
    triangles = NULL;

    vertexStride = 0;
    vertexCount = 0;
    vertexFormat = VTXFMT_NONE;
    voidverts = NULL;

    uvCount = 0;
    stripped = false;
    indexed = false;;
    mtformat = false;
    hasBoundingBox = false;
    hasVolumeInfo = false;

    name = NULL;
    alphaMapName = NULL;
    lightMapName = NULL;

    groups = NULL;
    groupCount = 0;

    type = TYPE_NORMAL;

    volumeParams.color = 0;
    volumeParams.end = 0;
}

Mesh::Mesh(FILE *fp, float scale){
    triangleCount = 0;
    triangles = NULL;

    vertexStride = 0;
    vertexCount = 0;
    vertexFormat = VTXFMT_NONE;
    voidverts = NULL;

    uvCount = 0;
    stripped = false;
    indexed = false;;
    mtformat = false;
    hasBoundingBox = false;
    hasVolumeInfo = false;

    name = NULL;
    alphaMapName = NULL;
    lightMapName = NULL;

    groups = NULL;
    groupCount = 0;

    type = TYPE_NORMAL;

    volumeParams.color = 0;
    volumeParams.end = 0;

    load(fp, scale);
}

Mesh::~Mesh(){
    clear();
}


Mesh &Mesh::operator = (const Mesh &m){
    clear();

    Text::strcopy(&name, m.name);
    Text::strcopy(&alphaMapName, m.alphaMapName);
    Text::strcopy(&lightMapName, m.lightMapName);

    boundingBox = m.boundingBox;
    transform = m.transform;


    //copy vertex data
    vertexStride = m.vertexStride;
    vertexFormat = m.vertexFormat;
    vertexCount = m.vertexCount;

    if (vertexCount){
        voidverts = reinterpret_cast<void *>(new uint8[vertexStride * vertexCount]);

        memcpy(voidverts, m.voidverts, vertexStride * vertexCount);
    }


    //copy triangle data
    triangleCount = m.triangleCount;
    if (triangleCount){
        triangles = new uint16[triangleCount];

        memcpy(triangles, m.triangles, sizeof(uint16) * triangleCount);
    }


    //copy flags
    uvCount = m.uvCount;
    stripped = m.stripped;
    indexed = m.indexed;
    mtformat = m.mtformat;
    hasBoundingBox = m.hasBoundingBox;
    hasVolumeInfo = m.hasVolumeInfo;


    //copy groups
    groupCount = m.groupCount;

    if (groupCount){
        groups = new Group[groupCount];

        for (int32 i = 0; i < groupCount; i++)
            groups[i] = m.groups[i];
    }


    //copy extra info
    type = m.type;


    volumeParams.color = m.volumeParams.color;
    volumeParams.end = m.volumeParams.end;

    return *this;
}

void Mesh::clear(){
    if (triangles)
        delete []triangles;

    if (voidverts){
        switch (vertexFormat){
            case VTXFMT_NONE:
                delete []verts;
                break;

            case VTXFMT_PS2:
                delete []ps2verts;
                break;

            case VTXFMT_PSP:
                delete []pspverts;
                break;

            case VTXFMT_MT:
                delete []mtverts;
                break;
        }
    }

    if (groups)
        delete []groups;

    groups = NULL;
    groupCount = 0;

    triangles = NULL;
    voidverts = NULL;
    triangleCount = 0;
    vertexFormat = VTXFMT_NONE;
    vertexCount = 0;
    vertexStride = 0;

    uvCount = 0;
    stripped = false;
    indexed = false;
    mtformat = false;
    hasBoundingBox = false;
    hasVolumeInfo = false;

    type = TYPE_NORMAL;

        volumeParams.color = 0;
    volumeParams.end = 0;

    boundingBox.reset();
    transform.setIdentity();

    Text::strcopy(&name, NULL);
    Text::strcopy(&alphaMapName, NULL);
    Text::strcopy(&lightMapName, NULL);
}


//*************************************** loading & saving functions **************************************//
bool Mesh::load(FILE *fp, float scale){
    uint32  magic;
    int32   flags;
    int32   tmp;
    char    tmpBuf[_MAX_PATH];
    bool    error = false;
    size_t  size;


    INP(fp, magic);

    if (magic != 0xdeafbabe)
        return false;


    clear();

    //read name
    READSTRING(fp, tmpBuf, _MAX_PATH);
    Text::strcopy(&name, tmpBuf);


    //read transformation matrix
    INP(fp, transform);

    //make sure the transformation matrix is the identity
    /*if (!transform.isIdentity()){
        TRACE("Mesh::load():    WARNING-> the object '%s' has a non-identity transformation matrix\n", name);
        TRACE("                   [%7.5f %7.5f %7.5f %7.5f]\n"
              "                   [%7.5f %7.5f %7.5f %7.5f]\n"
              "                   [%7.5f %7.5f %7.5f %7.5f]\n"
              "                   [%7.5f %7.5f %7.5f %7.5f]\n",
                transform.x.x, transform.y.x, transform.z.x, transform.w.x,
                transform.x.y, transform.y.y, transform.z.y, transform.w.y,
                transform.x.z, transform.y.z, transform.z.z, transform.w.z,
                transform.x.w, transform.y.w, transform.z.w, transform.w.w);
    }//*/

    transform.setIdentity();


    //read UV channel count and flags
    INP(fp, flags);

    stripped =       (flags & MESH_TRISTRIP_BIT) ? true : false;
    indexed =        (flags & MESH_DEINDEXED_BIT) ? false : true;
    hasBoundingBox = (flags & MESH_BOUNDINGBOX_BIT) ? true : false;
    hasVolumeInfo =  (flags & MESH_VOLUMEPARAMS_BIT) ? true : false;

    uvCount = flags & MESH_UVCOUNT_MASK;


    //read the volume parameters if they exist
    if (hasVolumeInfo)
        INP(fp, volumeParams);


    //make sure both vertex formats aren't specified
    Assert((flags & (MESH_PS2DATA_BIT | MESH_PSPDATA_BIT)) != (MESH_PS2DATA_BIT | MESH_PSPDATA_BIT));


    //read in the vertex count
    INP(fp, vertexCount);

    //allocate memory for the vertices and set the format and size values
    if (flags & MESH_MTFORMAT_BIT){
        mtverts = new MTVertex[vertexCount];
        vertexFormat = VTXFMT_MT;
        size = sizeof(MTVertex);
        mtformat = true;
    }

    else if (flags & MESH_PSPDATA_BIT){
        pspverts = new PSPVertex[vertexCount];
        vertexFormat = VTXFMT_PSP;
        size = sizeof(PSPVertex);
    }

    else if (flags & MESH_PS2DATA_BIT){
        ps2verts = new PS2Vertex[vertexCount];
        vertexFormat = VTXFMT_PS2;
        size = sizeof(PS2Vertex);
    }

    else{
        verts = new MeshVertex[vertexCount];
        vertexFormat = VTXFMT_NONE;
        size = sizeof(MeshVertex);
    }

    //read in the vertices in their specified format
    fread(voidverts, size, vertexCount, fp);
    scaleVerts(scale);


    //read in normals (skip over them)
    INP(fp, tmp);

    if (tmp)
        fseek(fp, sizeof(Vector) * tmp, SEEK_CUR);


    //read in the bounding box
    INP(fp, boundingBox);


    //read in triangles
    INP(fp, triangleCount);

    triangles = new uint16[triangleCount];
    fread(triangles, sizeof(uint16), triangleCount, fp);


    //read in groups
    INP(fp, groupCount);
    groups = new Group[groupCount];

    for (int32 i = 0; i < groupCount; i++){
        Group &         group = groups[i];


        INP(fp, group.renderBase);
        INP(fp, group.renderCount);

        if (hasBoundingBox)
            INP(fp, group.staticBoundingBox);

        group.colorMap.load(fp);
        group.lightMap.load(fp);
        group.blendMap.load(fp);
        group.alphaMap.load(fp);

        Text::strcopy(&group.texName, group.colorMap.getName());
        if (lightMapName == NULL && group.lightMap.getName() && group.lightMap.getName()[0] != 0)
            Text::strcopy(&lightMapName, group.lightMap.getName());
        Text::strcopy(&alphaMapName, group.alphaMap.getName());
    }


    //recalculate the bounding box 
    calcBoundingBox();

    return true;
}

int32 Mesh::save(FILE *fp, float scale) const{
    if (fp == NULL || ferror(fp))
        return -1;

    uint32  magic = 0xdeafbabe;
    int32   flags = 0;
    int32   tmp;
    int32   pos = ftell(fp);


    //write file signature
    OUTP(fp, magic);


    //write name
    WRITESTRING(fp, name);

    //write transformation matrix
    OUTP(fp, transform);


    //update mesh flags
    if (stripped)
        flags |= MESH_TRISTRIP_BIT;

    if (!indexed)
        flags |= MESH_DEINDEXED_BIT;

    if (hasBoundingBox)
        flags |= MESH_BOUNDINGBOX_BIT;

    if (hasVolumeInfo)
        flags |= MESH_VOLUMEPARAMS_BIT;

    switch (vertexFormat){
        case VTXFMT_PS2:
            flags |= MESH_PS2DATA_BIT;
            break;

        case VTXFMT_PSP:
            flags |= MESH_PSPDATA_BIT;
            break;

        case VTXFMT_MT:
            flags |= MESH_MTFORMAT_BIT;
            break;
    }

    flags |= uvCount & MESH_UVCOUNT_MASK;

    //write out flags
    OUTP(fp, flags);


    //write out the volume parameters if they exist
    if (hasVolumeInfo)
        OUTP(fp, volumeParams);


    //write out the vertices
    OUTP(fp, vertexCount);
    fwrite(voidverts, vertexStride, vertexCount, fp);


    //write out an empty normals array
    tmp = 0;
    OUTP(fp, tmp);


    //read in the bounding box
    OUTP(fp, boundingBox);


    //write out triangles
    OUTP(fp, triangleCount);

    fwrite(triangles, sizeof(uint16), triangleCount, fp);


    //write out groups
    OUTP(fp, groupCount);

    for (int32 i = 0; i < groupCount; i++){
        Group &         group = groups[i];


        OUTP(fp, group.renderBase);
        OUTP(fp, group.renderCount);

        if (hasBoundingBox)
            OUTP(fp, group.staticBoundingBox);

        group.colorMap.save(fp);
        group.lightMap.save(fp);
        group.blendMap.save(fp);
        group.alphaMap.save(fp);
    }


    return pos;
}


//**************************************** format conversion functions ***************************************//
bool Mesh::convert(VertexFormat fmt){
    //!!// Implement Me!!!
    return false;
}

void Mesh::scaleVerts(float scale){
    int i;


    switch (vertexFormat){
        case VTXFMT_NONE:
            for (i = 0; i < vertexCount; i++)
                verts[i].pos *= scale;
            
            break;

        case VTXFMT_MT:
            for (i = 0; i < vertexCount; i++)
                mtverts[i].pos *= scale;

            break;

        case VTXFMT_PSP:
            for (i = 0; i < vertexCount; i++)
                pspverts[i].pos *= scale;

            break;

        case VTXFMT_PS2:
            for (i = 0; i < vertexCount; i++)
                ps2verts[i].pos *= scale;

            break;
    }
}


//***************************************** accessor functions ***************************************//
const char *Mesh::getName() const{
    return name;
}

bool Mesh::isIndexed() const{
    return indexed;
}

bool Mesh::isStripped() const{
    return stripped;
}

bool Mesh::isMTFormat() const{
    return mtformat;
}

int32 Mesh::getUVCount() const{
    return uvCount;
}


const uint16 *Mesh::getTriangles() const{
    return triangles;
}

const int32 Mesh::getTriangleCount() const{
    return triangleCount;
}


Mesh::VertexFormat Mesh::getVertFormat() const{
    return vertexFormat;
}

const int32 Mesh::getVertCount() const{
    return vertexCount;
}

const size_t Mesh::getVertStride() const{
    return vertexStride;
}

Box Mesh::getBoundingBox() const{
    return boundingBox;
}

Matrix Mesh::getTransform() const{
    return transform;
}

int32 Mesh::getGroupCount() const{
    return groupCount;
}

const Mesh::Group *Mesh::getGroup(int32 i) const{
    if (i < 0 || i >= groupCount)
        return NULL;

    if (groups == NULL)
        return NULL;

    return &groups[i];
}


Mesh::MeshType Mesh::getType() const{
    return type;
}


//*************************************** manipulation functions ***************************************//
void Mesh::setType(MeshType t){
    type = t;
}


void Mesh::calcBoundingBox(){
    Box original = boundingBox;


    boundingBox.reset();


    switch (vertexFormat){
        case VTXFMT_NONE:
            for (int32 i = 0; i < vertexCount; i++)
                boundingBox += verts[i].pos;

            break;

        case VTXFMT_PS2:
            for (int32 i = 0; i < vertexCount; i++)
                boundingBox += ps2verts[i].pos;

            break;

        case VTXFMT_PSP:
            for (int32 i = 0; i < vertexCount; i++)
                boundingBox += pspverts[i].pos;

            break;

        case VTXFMT_MT:
            for (int32 i = 0; i < vertexCount; i++)
                boundingBox += mtverts[i].pos;

            break;
    }

    
    calcStaticBoundingBoxes();

#ifdef TRANSGAMING_FULLDEBUG
    if (original != boundingBox){
        TRACE("Mesh::calcBoundingBox(): bounding box has changed!  {original = {<%g, %g, %g>, <%g, %g, %g>}, boundingBox = {<%g, %g, %g>, <%g, %g, %g>}}\n",
            original.min.x, original.min.y, original.min.z,
            original.max.x, original.max.y, original.max.z,
            boundingBox.min.x, boundingBox.min.y, boundingBox.min.z,
            boundingBox.max.x, boundingBox.max.y, boundingBox.max.z);
    }
#endif
}

void Mesh::calcStaticBoundingBoxes(){
    if (hasBoundingBox)
        return;


    Box bb;


    for (int32 i = 0; i < groupCount; i++){
        int32 base = groups[i].renderBase;
        int32 end = stripped ? 
                        (groups[i].renderCount + 2) :
                        (groups[i].renderCount * 3);


        bb.reset();

        switch (vertexFormat){
            case VTXFMT_NONE:
                //renderBase & renderCount have different meanings with indexed vs non-indexed data
                if (indexed){
                    for (int32 i = 0; i < end; i++)
                        bb += verts[triangles[i]].pos;
                }

                else{
                    for (int32 i = 0; i < end; i++)
                        bb += verts[i].pos;
                }

                break;

            case VTXFMT_PS2:
                //renderBase & renderCount have different meanings with indexed vs non-indexed data
                if (indexed){
                    for (int32 i = 0; i < end; i++)
                        bb += ps2verts[triangles[i]].pos;
                }

                else{
                    for (int32 i = 0; i < end; i++)
                        bb += ps2verts[i].pos;
                }

                break;

            case VTXFMT_PSP:
                //renderBase & renderCount have different meanings with indexed vs non-indexed data
                if (indexed){
                    for (int32 i = 0; i < end; i++)
                        bb += pspverts[triangles[i]].pos;
                }

                else{
                    for (int32 i = 0; i < end; i++)
                        bb += pspverts[i].pos;
                }

                break;

            case VTXFMT_MT:
                //renderBase & renderCount have different meanings with indexed vs non-indexed data
                if (indexed){
                    for (int32 i = 0; i < end; i++)
                        bb += mtverts[triangles[i]].pos;
                }

                else{
                    for (int32 i = 0; i < end; i++)
                        bb += mtverts[i].pos;
                }

                break;
        }


        Assert(bb.isValid());
        groups[i].staticBoundingBox[0].set(bb.min.x, bb.min.y, bb.min.z);
        groups[i].staticBoundingBox[1].set(bb.min.x, bb.min.y, bb.max.z);
        groups[i].staticBoundingBox[2].set(bb.min.x, bb.max.y, bb.min.z);
        groups[i].staticBoundingBox[3].set(bb.min.x, bb.max.y, bb.max.z);
        groups[i].staticBoundingBox[4].set(bb.max.x, bb.min.y, bb.min.z);
        groups[i].staticBoundingBox[5].set(bb.max.x, bb.min.y, bb.max.z);
        groups[i].staticBoundingBox[6].set(bb.max.x, bb.max.y, bb.min.z);
        groups[i].staticBoundingBox[7].set(bb.max.x, bb.max.y, bb.max.z);
    }

    hasBoundingBox = true;
}


void Mesh::setVolumeParams(Color _color, float32 _end){
    hasVolumeInfo = true;

    volumeParams.color = _color;
    volumeParams.end = _end;
}
