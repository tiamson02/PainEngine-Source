#include <stdlib.h>
#include "meshPack.h"
#include "fileUtil.h"
#include "text.h"
#include "stdTypes.h"
#include "trace.h"


//********************************* constructors & destructor ***********************************//
MeshPack::MeshPack(){
    meshCount = 0;
    meshes = NULL;
    filename = NULL;
    platform = PLATFORM_PC;
}

MeshPack::MeshPack(const MeshPack &m){
    *this = m;
}

MeshPack::MeshPack(const char *filename, float scale){
    meshCount = 0;
    meshes = NULL;
    filename = NULL;
    platform = PLATFORM_PC;

    load(filename, scale);
}

MeshPack::~MeshPack(){
    clear();
}


MeshPack &MeshPack::operator = (const MeshPack &m){
    clear();

    meshCount = m.meshCount;
    meshes = new Mesh[meshCount];

    for (int32 i = 0; i < meshCount; i++)
        meshes[i] = m.meshes[i];

    Text::strcopy(&filename, m.filename);
    platform = m.platform;

    boundingBox = m.boundingBox;
    geometryBoundingBox = m.geometryBoundingBox;

    return *this;
}

void MeshPack::clear(){
    if (meshes)
        delete []meshes;

    meshes = NULL;
    meshCount = 0;

    Text::strcopy(&filename, NULL);
    boundingBox.reset();
    geometryBoundingBox.reset();
}

//******************************* loading & saving functions *************************************//
bool MeshPack::fixFilename(){
    if (filename == NULL)
        return false;

    
    char    fname[_MAX_PATH];
    char *  ext;
    size_t  len;


    strncpy(fname, filename, _MAX_PATH);

    ext = strrchr(fname, '.');
    len = strlen(fname);

    if (ext == NULL)
        ext = &fname[len];
    

    //try the PSP specific format first
    strncpy(ext, ".mpp", _MAX_PATH - len);

    //check if the PSP specific version exists
    if (fexist(fname)){
        Text::strcopy(&filename, fname);
        platform = PLATFORM_PSP;

        return true;
    }

    else{
        //try the PSP specific format first
        strncpy(ext, ".mp2", _MAX_PATH - len);

        //check if the PS2 specific version exists
        if (fexist(fname)){
            Text::strcopy(&filename, fname);
            platform = PLATFORM_PS2;

            return true;
        }

        //otherwise just make sure the PC version exists
        else{
            platform = PLATFORM_PC;

            return fexist(filename);
        }
    }
}

bool MeshPack::load(const char *fname, float scale){
    Text::strcopy(&filename, fname);


    //file doesn't exist in either the PSP or PC format => fail
    if (!fixFilename())
        return false;


    bool result = loadPack(filename, scale);

    //calculate the bounding box of the level
    calcBoundingBox();


    Vector size = boundingBox.getSize();
    Vector sizeg = geometryBoundingBox.getSize();


    TRACE("\nMeshPack::load(): the map '%s' has the bounding boxes:\n"
          "     all:      {<%g, %g, %g>, <%g, %g, %g>}\n"
          "       size =  <%g, %g, %g>\n"
          "     geometry: {<%g, %g, %g>, <%g, %g, %g>}\n"
          "       size =  <%g, %g, %g>\n",
          filename,
          boundingBox.min.x, boundingBox.min.y, boundingBox.min.z,
          boundingBox.max.x, boundingBox.max.y, boundingBox.max.z,
          size.x, size.y, size.z,
          geometryBoundingBox.min.x, geometryBoundingBox.min.y, geometryBoundingBox.min.z,
          geometryBoundingBox.max.x, geometryBoundingBox.max.y, geometryBoundingBox.max.z,
          sizeg.x, sizeg.y, sizeg.z);

#ifdef TRANSGAMING_FULLDEBUG
    testMeshes();
#endif

    return result;
}

bool MeshPack::save(const char *fname, float scale) const{
    char    name[_MAX_PATH];
    char *  ext;
    size_t  len;


    strncpy(name, fname, _MAX_PATH);
    ext = strrchr(name, '.');
    len = strlen(name);

    if (ext == NULL)
        ext = &name[len];


    switch (platform){
        case PLATFORM_PC:
            strncpy(name, ".mpk", _MAX_PATH - len);
            break;

        case PLATFORM_PSP:
            strncpy(name, ".mpp", _MAX_PATH - len);
            break;

        case PLATFORM_PS2:
            strncpy(name, ".mp2", _MAX_PATH - len);
            break;
    }


    return savePack(name, scale);
}

bool MeshPack::loadPack(const char *fname, float scale){
    FILE *  fp;
    int32   portals = 0;
    int32   antiportals = 0;
    int32   zones = 0;
    int32   volumes = 0;
    int32   glasses = 0;
    int32   barriers = 0;
    uint32  magic;
    int32   pos;
    uint32  offsets[4096];



    TRACE("MeshPack::load(): loading a mesh pack from the file '%s'\n", fname);

    fp = fopen(fname, "rb");

    //file couldn't be opened => fail
    if (fp == NULL)
        return false;


    TRACE("MeshPack::load():    opened the pack file '%s'\n", fname);


    //read in the file signature
    fseek(fp, 0, SEEK_END);
    pos = ftell(fp);
    pos -= sizeof(magic);
    fseek(fp, pos, SEEK_SET);

    INP(fp, magic);


    //file signature doesn't match => fail
    if (magic != 0xdeadbeef){
        TRACE("MeshPack::load(): ERROR-> invalid file signature: 0x%08X\n", magic);
        fclose(fp);

        return false;
    }


    pos -= sizeof(uint32);
    fseek(fp, pos, SEEK_SET);
    INP(fp, meshCount);

    TRACE("MeshPack::load():    reading %d meshes total from '%s'\n", meshCount, fname);

    //sanity check
    Assert(meshCount < 4096);


    if (meshCount > 0) {
        pos -= meshCount * sizeof(uint32);
        fseek(fp, pos, SEEK_SET);

        //read in the mesh index
        fread(offsets, sizeof(uint32), meshCount, fp);


        //allocate some space to store the meshes
        meshes = new Mesh[meshCount];


        //start reading the meshes in
        for (int32 i = 0; i < meshCount; i++){
            Mesh &mesh = meshes[i];


            //seek to the mesh's location in the file
            fseek(fp, offsets[i], SEEK_SET);

            //attempt to load the mesh
            if (!mesh.load(fp, scale)){
                TRACE("MeshPack::load(): ERROR-> file access error while loading mesh %d from '%s'\n", i, fname);
                fclose(fp);

                return false;
            }


            //classify the mesh
            if (strstr(mesh.getName(), "portal")){
#ifdef TRANSGAMING_FULLDEBUG
                TRACE("MeshPack::load():    loaded a portal: {id = %d, name = '%s', verts = %d}\n", portals, mesh.getName(), mesh.getVertCount());
#endif
                mesh.setType(Mesh::TYPE_PORTAL);

                portals++;
            }
            else if (strstr(mesh.getName(), "antyp")){
#ifdef TRANSGAMING_FULLDEBUG
                TRACE("MeshPack::load():    loaded an anti-portal: {id = %d, name = '%s', verts = %d}\n", antiportals, mesh.getName(), mesh.getVertCount());
#endif
                mesh.setType(Mesh::TYPE_ANTIPORTAL);

                if (strstr(mesh.getName(), "antypswitch"))
                    mesh.setType(Mesh::TYPE_ANTIPORTALSWITCH);

                antiportals++;
            }
            else if (strstr(mesh.getName(), "zone")){
                if (strstr(mesh.getName(), "death") || strstr(mesh.getName(), "DEATH")){
#ifdef TRANSGAMING_FULLDEBUG
                    TRACE("MeshPack::load():    loaded a death zone: {id = %d, name = '%s', verts = %d}\n", zones, mesh.getName(), mesh.getVertCount());
#endif
                    mesh.setType(Mesh::TYPE_DEATHZONE);
                }

                else if (strstr(mesh.getName(), "ice") || strstr(mesh.getName(), "ICE")){
#ifdef TRANSGAMING_FULLDEBUG
                    TRACE("MeshPack::load():    loaded an ice zone: {id = %d, name = '%s', verts = %d}\n", zones, mesh.getName(), mesh.getVertCount());
#endif
                    mesh.setType(Mesh::TYPE_ICEZONE);
                }

                else if (strstr(mesh.getName(), "ladder") || strstr(mesh.getName(), "LADDER")){
#ifdef TRANSGAMING_FULLDEBUG
                    TRACE("MeshPack::load():    loaded a ladder zone: {id = %d, name = '%s', verts = %d}\n", zones, mesh.getName(), mesh.getVertCount());
#endif
                    mesh.setType(Mesh::TYPE_LADDERZONE);
                }

                else if (strstr(mesh.getName(), "switch") || strstr(mesh.getName(), "SWITCH")){
#ifdef TRANSGAMING_FULLDEBUG
                    TRACE("MeshPack::load():    loaded a switch zone: {id = %d, name = '%s', verts = %d}\n", zones, mesh.getName(), mesh.getVertCount());
#endif
                    mesh.setType(Mesh::TYPE_SWITCHZONE);
                }

                else{
#ifdef TRANSGAMING_FULLDEBUG
                    TRACE("MeshPack::load():    loaded a zone: {id = %d, name = '%s', verts = %d}\n", zones, mesh.getName(), mesh.getVertCount());
#endif
                    mesh.setType(Mesh::TYPE_ZONE);
                }

                zones++;
            }
            else if (strstr(mesh.getName(), "vollight")){
#ifdef TRANSGAMING_FULLDEBUG
                TRACE("MeshPack::load():    loaded a volume light: {id = %d, name = '%s', verts = %d}\n", volumes, mesh.getName(), mesh.getVertCount());
#endif
                mesh.setType(Mesh::TYPE_VOLUMELIGHT);

                volumes++;
            }
            else if (strstr(mesh.getName(), "volfog")){
#ifdef TRANSGAMING_FULLDEBUG
                TRACE("MeshPack::load():    loaded a volume fog: {id = %d, name = '%s', verts = %d}\n", volumes, mesh.getName(), mesh.getVertCount());
#endif
                mesh.setType(Mesh::TYPE_VOLUMEFOG);

                volumes++;
            }
            else if (strstr(mesh.getName(), "glass")){
#ifdef TRANSGAMING_FULLDEBUG
                TRACE("MeshPack::load():    loaded a glass object: {id = %d, name = '%s', verts = %d}\n", glasses, mesh.getName(), mesh.getVertCount());
#endif
                mesh.setType(Mesh::TYPE_GLASS);

                glasses++;
            }
            else{
                bool traced = false;


                if (strstr(mesh.getName(), "barrier")){
#ifdef TRANSGAMING_FULLDEBUG
                    TRACE("MeshPack::load():    loaded a barrier object: {id = %d, name = '%s', verts = %d}\n", barriers, mesh.getName(), mesh.getVertCount());
#endif
                    mesh.setType(Mesh::TYPE_GLASS);

                    barriers++;
                    traced = true;
                }

                const char *actgrpIndex = strstr(mesh.getName(), "actgrp");


                if (actgrpIndex){
                    int32 g = 0;


                    sscanf(actgrpIndex, "actgrp%d", &g);
#ifdef TRANSGAMING_FULLDEBUG
                    TRACE("MeshPack::load():    loaded an active group member: {group = %d, name = '%s', verts = %d}\n", g, mesh.getName(), mesh.getVertCount());
#endif
                }

#ifdef TRANSGAMING_FULLDEBUG
                else if (!traced)
                    TRACE("MeshPack::load():    loaded a normal mesh: {name = '%s', verts = %d}\n", mesh.getName(), mesh.getVertCount());
#endif
            }
        }
    }


    TRACE("\nMeshPack::load(): loaded the mesh pack '%s'.  Meshes:\n"
          "         %2d portals\n"
          "         %2d antiportals\n"
          "         %2d zones\n"
          "         %2d volumes\n"
          "         %2d glasses\n"
          "         %2d barriers\n"
          "         %2d meshes\n",
          fname,
          portals,
          antiportals,
          zones,
          volumes,
          glasses,
          barriers,
          meshCount - (portals + antiportals + zones + volumes + glasses + barriers));

    fclose(fp);

    return true;
}


bool MeshPack::savePack(const char *fname, float scale) const{
    FILE *  fp;
    uint32  magic = 0xdeadbeef;
    uint32  offsets[4096];



    TRACE("MeshPack::save(): saving a mesh pack to the file '%s'\n", fname);

    fp = fopen(fname, "wb");

    //file couldn't be opened => fail
    if (fp == NULL)
        return false;


    TRACE("MeshPack::save():    opened the pack file '%s'\n", fname);


    //sanity check
    Assert(meshCount < 4096);


    //write out all the meshes
    for (int32 i = 0; i < meshCount; i++){
        int32 offset = meshes[i].save(fp, scale);


        //mesh couldn't be written to the file => fail
        if (offset < 0){
            TRACE("MeshPack::save(): ERROR-> could not save the mesh '%s' to the file '%s'\n", meshes[i].getName(), fname);
            fclose(fp);

            //file is corrupted => delete it
            unlink(fname);

            return false;
        }

        offsets[i] = (unsigned)offset;
    }


    //write the mesh index to the file
    fwrite(offsets, sizeof(uint32), meshCount, fp);

    //write out the mesh count
    OUTP(fp, meshCount);

    //write out the file signature
    OUTP(fp, magic);


    TRACE("MeshPack::save(): saved the mesh pack to '%s'\n", fname);
    fclose(fp);

    return true;
}


//******************************* format conversion functions ***********************************//
bool MeshPack::convert(Platform p){
    bool    result = true;
    int32   i;


    //already in the requested format => succeed
    if (platform == p)
        return true;


    switch (p){
        case PLATFORM_PC:
            for (i = 0; i < meshCount; i++){
                if (!meshes[i].convert(Mesh::VTXFMT_NONE)){
                    result = false;

                    break;
                }
            }

            break;

        case PLATFORM_PS2:
            for (i = 0; i < meshCount; i++){
                if (!meshes[i].convert(Mesh::VTXFMT_PS2)){
                    result = false;

                    break;
                }
            }

            break;
        case PLATFORM_PSP:
            for (i = 0; i < meshCount; i++){
                if (!meshes[i].convert(Mesh::VTXFMT_PSP)){
                    result = false;

                    break;
                }
            }
    }

    return result;
}


//******************************* accessor functions *************************************//
const char *MeshPack::getFilename() const{
    return filename;
}

int32 MeshPack::getMeshCount() const{
    return meshCount;
}

Box MeshPack::getBoundingBox() const{
    return boundingBox;
}

Box MeshPack::getGeometryBoundingBox() const{
    return geometryBoundingBox;
}

Box MeshPack::getBoundingBox(int32 mesh) const{
    if (mesh >= 0 && mesh < meshCount)
        return meshes[mesh].getBoundingBox();

    return Box();
}


Mesh *MeshPack::findMesh(const char *name) const{
    for (int32 i = 0; i < meshCount; i++){
        if (!strcasecmp(meshes[i].getName(), name))
            return &meshes[i];
    }

    return NULL;
}

bool MeshPack::hasObject(const char *name) const{
    return findMesh(name) != NULL;
}


//********************************* manipulation & query functions *************************************//
void MeshPack::calcBoundingBox(){
    boundingBox.reset();
    geometryBoundingBox.reset();


    for (int32 i = 0; i < meshCount; i++){
        boundingBox += meshes[i].getBoundingBox();

        if (meshes[i].getType() == Mesh::TYPE_NORMAL || meshes[i].getType() == Mesh::TYPE_ACTIVE)
            geometryBoundingBox += meshes[i].getBoundingBox();
    }
}

bool MeshPack::setVolumeParams(const char *name, Color _color, float32 _end){
    Mesh *mesh = findMesh(name);


    if (mesh){
        mesh->setVolumeParams(_color, _end);

        return true;
    }

    return false;
}

#ifdef TRANSGAMING_FULLDEBUG
void MeshPack::testMeshes() const{
    TRACE("MeshPack::testMeshes(): testing the meshes in '%s' to see if any are outside the geometry bb = {<%g, %g, %g>, <%g, %g, %g>}\n", 
            filename,
            geometryBoundingBox.min.x, geometryBoundingBox.min.y, geometryBoundingBox.min.z, 
            geometryBoundingBox.max.x, geometryBoundingBox.max.y, geometryBoundingBox.max.z);


    for (int32 i = 0; i < meshCount; i++){
        Box bb = meshes[i].getBoundingBox();


        if (bb.outside(geometryBoundingBox, Box::AXIS_Y))
            TRACE("MeshPack::testMeshes():   mesh '%s' is outside {bb = {<%g, %g, %g>, <%g, %g, %g>}}\n",
                    meshes[i].getName(),
                    bb.min.x, bb.min.y, bb.min.z,
                    bb.max.x, bb.max.y, bb.max.z);

        if (!bb.inside(geometryBoundingBox))
            TRACE("MeshPack::testMeshes():   mesh '%s' is not inside {bb = {<%g, %g, %g>, <%g, %g, %g>}}\n",
                    meshes[i].getName(),
                    bb.min.x, bb.min.y, bb.min.z,
                    bb.max.x, bb.max.y, bb.max.z);
    }
}
#endif