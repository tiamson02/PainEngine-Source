#ifndef  __MESHPACK_H__
# define __MESHPACK_H__

# include "stdTypes.h"
# include "vector.h"
# include "mesh.h"


class MeshPack{
public:
    typedef enum{
        PLATFORM_PC,
        PLATFORM_PSP,
        PLATFORM_PS2,
    } Platform;

private:
    char *      filename;
    Box         boundingBox;
    Box         geometryBoundingBox;
    Mesh *      meshes;
    int32       meshCount;
    Platform    platform;


    void        calcBoundingBox();
    bool        fixFilename();
    bool        loadPack(const char *fname, float scale);
    bool        savePack(const char *fname, float scale) const;

    Mesh *      findMesh(const char *name) const;
public:
    MeshPack();
    MeshPack(const MeshPack &m);
    MeshPack(const char *filename, float scale);
    ~MeshPack();

    MeshPack &  operator = (const MeshPack &m);

    bool        load(const char *fname, float scale);
    bool        save(const char *fname, float scale) const;
    bool        convert(Platform p);
    void        clear();

    const char *getFilename() const;
    int32       getMeshCount() const;
    Box         getBoundingBox() const;
    Box         getBoundingBox(int32 mesh) const;
    Box         getGeometryBoundingBox() const;

    bool        hasObject(const char *name) const;
    bool        setVolumeParams(const char *name, Color _color, float32 _end);

#ifdef TRANSGAMING_FULLDEBUG
    void        testMeshes() const;
#endif
};

#endif
