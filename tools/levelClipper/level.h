#ifndef  __LEVEL_H__
# define __LEVEL_H__

# include "meshPack.h"


class Level{
public:
    enum{
        FLAG_NONE =         0x0000,

        FLAG_FIXSLASHES =   0x0001,
        FLAG_MERGEVOLUMES = 0x0002,

        FLAG_ENTITY_MASK =  FLAG_MERGEVOLUMES,
        FLAG_OBJECT_MASK =  FLAG_FIXSLASHES,
    };

private:
    MeshPack    map;
    char *      path;
    float       scale;


    bool isFolderIgnored(const char *filename) const;
    bool loadObject(const char *filename);
    bool clipCObjects(const char *pth, const char *pattern, int32 *clipped, int32 *total, int32 options);
    bool clipEObjects(const char *pth, const char *pattern, int32 *clipped, int32 *total, int32 options);

public:
    Level();
    Level(const Level &l);
    Level(const char *filename, float scale);
    ~Level();


    void        clear();
    Level &     operator = (const Level &l);

    bool        load(const char *filename);
    bool        save(const char *filename) const;

    bool        convert(MeshPack::Platform platform);
    bool        clipObjects(int32 *clipped, int32 *total, int32 options = FLAG_NONE);

    const char *getMapName() const;

    void        setPath(const char *pth);
    const char *getPath() const;

    void        setScale(float f);
    float       getScale() const;
};

#endif
