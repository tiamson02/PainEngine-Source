#ifndef  __OBJECT_H__
# define __OBJECT_H__

# include "stdTypes.h"
# include "vector.h"


class Object{
private:
    char *  filename;
    Vector  size;
    Vector  pos;
    Box     boundingBox;
    Color   fogColor;
    float32 fogEnd;
    bool    valid;


    static const char *STR_POS;
    static const char *STR_WIDTH;
    static const char *STR_HEIGHT;
    static const char *STR_DEPTH;
    static const char *STR_COLOR;
    static const char *STR_END;


    void calcBoundingBox();

public:
    Object();
    Object(const char *fname);
    ~Object();


    bool            isValid() const;

    bool            load(const char *fname);
    bool            save(const char *fname) const;
    Object &        operator = (const Object &o);
    void            clear();

    const Box &     getBoundingBox() const;
    const Vector &  getSize() const;
    const Vector &  getPosition() const;
    const Color &   getFogColor() const;
    const float32   getFogEnd() const;
    const char *    getFilename() const;
};


#endif