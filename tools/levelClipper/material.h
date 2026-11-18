#ifndef  __MATERIAL_H__
# define __MATERIAL_H__

# include <stdio.h>
# include "vector.h"


class MaterialDef{
private:
    friend class Mesh;

    char *      name;
    TexCoord    pan;
    TexCoord    tile;

public:
    MaterialDef();
    ~MaterialDef();

    MaterialDef &   operator = (const MaterialDef &m);

    bool            load(FILE *fp);
    bool            save(FILE *fp) const;

    const char *    getName() const;
    const TexCoord &getPan() const;
    const TexCoord &getTile() const;

    void            setPan(const TexCoord &p);
    void            setTile(const TexCoord &t);

    /*friend FileObject& operator>>(FileObject& file, MaterialDef& mat)
    {
    return file >> mat.Name >> mat.Pan >> mat.Tile;
    }

    friend FileObject& operator<<(FileObject& file, MaterialDef& mat)
    {
    return file << mat.Name << mat.Pan << mat.Tile;
    }//*/
};

#endif
