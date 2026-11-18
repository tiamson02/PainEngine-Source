/*
    File:   font.h
    Author: eric van beurden
    Date:   16/6/2006

    Namespaces:           GFX
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#ifndef  __fontSave_H_
# define __fontSave_H_

# include "stdTypes.h"
# include "image.h"


//!
namespace GFX{

#define MAKEFCC(c1, c2, c3, c4)     (((c1) << 24) | ((c2) << 16) | ((c3) << 8) | (c4))


class Font{
public:
    typedef enum{
        PACK_ERROK,
        PACK_ERROVERFLOW,
        PACK_ERRTOOSMALL,
    } PackStatus;

private:
    typedef uint32 FCC;

    static const FCC    FCC_FONT = MAKEFCC('F', 'O', 'N', 'T');
    static const FCC    FCC_SIZE = MAKEFCC('S', 'I', 'Z', 'E');
    static const FCC    FCC_NAME = MAKEFCC('N', 'A', 'M', 'E');
    static const FCC    FCC_FORE = MAKEFCC('F', 'O', 'R', 'E');
    static const FCC    FCC_TEXC = MAKEFCC('T', 'E', 'X', 'C');
    static const FCC    FCC_IMG  = MAKEFCC('_', 'I', 'M', 'G');
    static const FCC    FCC_END  = MAKEFCC('_', 'E', 'N', 'D');
    
    static const size_t BUFSIZE = 1024;
    static const size_t TEXCOORD_COUNT = 256;

    static const char * SECTION_PARAMS;
    static const char * SECTION_COORDS;


    bool        valid;
    Image       img;
    int32       charHeight;
    int32       vertSpace;
    int32       size;
    int32       texCoordCount;
    float32     texCoords[TEXCOORD_COUNT][4];
    char        name[BUFSIZE];
    char        foreTex[BUFSIZE];


    void init();
    void clear();
    bool exportSettings(const char *filename) const;
    bool loadSettings(const char *filename);

    int32   findArea() const;
    void    removeBlankChars();
    float32 getCharHeight(int32 c) const;
    float32 getCharWidth(int32 c) const;
    float32 getCharX(int32 c) const;
    float32 getCharY(int32 c) const;

    float32 rescaleVal(float32 val, int32 oldW, int32 newW) const;
    bool    findTheoreticalSize(int32 *width, int32 *height) const;
public:
    Font();
    Font(const char *filename);
    ~Font();


    bool        isValid() const;

    bool        load(const char *filename);
    bool        load(const char *filename, const char *settings);
    bool        save(const char *filename) const;
    bool        split(const char *filename) const;

    bool        saveMask(const char *filename) const;
    bool        updateTexCoords(const Image &mask);

    bool        swapColors();

    PackStatus  packCharacters(int32 width, int32 height);
};

};  //namespace GFX;


#endif
