/*
    File:   image.h
    Author: eric van beurden
    Date:   15/6/2006

    Namespaces:           GFX
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#ifndef  __image_H_
# define __image_H_

# include "stdTypes.h"


//!
namespace GFX{

class Image{
public:
    //file format specifiers
    enum ImageType{
        IMG_DEFAULT =   0,
        IMG_TGA =       1,
    };

    //pixel formats
    enum PixelFormat{
        IMG_FMT_NONE =    -1,
        IMG_FMT_4 =       0,     //4 bpc, 1 component (R)
        IMG_FMT_8 =       1,     //8 bpc, 1 component (R)
        IMG_FMT_44 =      2,     //4 bpc, 2 components (RA)
        IMG_FMT_88 =      3,     //8 bpc, 2 components (RA)
        IMG_FMT_62 =      4,     //6 bits R, 2 bits A
        IMG_FMT_124 =     5,     //12 bits R, 2 bits A
        IMG_FMT_555 =     6,     //5 bpc, 3 components
        IMG_FMT_888 =     7,     //8 bpc, 3 components
        IMG_FMT_4444 =    8,     //4 bpc, 4 components
        IMG_FMT_5551 =    9,     //5 bits each R, G, B, 1 bit A
        IMG_FMT_8888 =    10,    //8 bpc, 4 components
        IMG_FMT_1010102 = 11,    //10 bits each R, G, B, 2 bits A
        IMG_NUM_FORMATS = 12,        
    };

    enum AlphaGenMode{
        ALPHA_GEN_DEFAULT =     0x40000001,
        ALPHA_GEN_FIRST =       ALPHA_GEN_DEFAULT,
        ALPHA_GEN_TRANSPARENT = 0x40000002,
        ALPHA_GEN_BLEND_R =     0x40000003,
        ALPHA_GEN_BLEND_G =     0x40000004,
        ALPHA_GEN_BLEND_B =     0x40000005,
        ALPHA_GEN_BLEND_I =     0x40000006,
        ALPHA_GEN_LAST =        ALPHA_GEN_BLEND_I,
    };

    enum ImageDepth{
        IMG_DEPTH_NONE =  0,
        IMG_DEPTH_4 =     4,
        IMG_DEPTH_8 =     8,
        IMG_DEPTH_16 =    16,
        IMG_DEPTH_24 =    24,
        IMG_DEPTH_32 =    32,
    };

protected:
    int32       width;
    int32       height;
    ImageDepth  depth;
    int32       components;
    PixelFormat format;
    int32       bytes;
    uint8 *     data;
    bool        valid;


    void init();
    bool isColorEmpty(int32 offset) const;

public:
    Image();
    Image(const char *filename);
    Image(int32 w, int32 h, PixelFormat fmt);
    ~Image();

    //**************** image loading functions ************************
    bool            load(const char *filename, int32 type = IMG_DEFAULT);
    bool            save(const char *filename, int32 type = IMG_DEFAULT) const;
    bool            saver(const char *filename) const;
    bool            loader(const char *filename);

    //**************** image manipulation functions *******************
    void            freeImage();
    bool            create(int32 w, int32 h, PixelFormat fmt);
    bool            clear(uint32 c);
    void            flip(bool vert = true);
    void            invert();

    void            drawBox(int32 x, int32 y, int32 w, int32 h, int32 c);

    //copy(): copies a rectangular section from the source image <src>
    //  to this image.  The rectangular section begins at the point <sx, sy>
    //  and has the size <sw, sh>.  The section is copied to a rectangle
    //  of the same size in this image beginning at the point <dx, dy>.
    //  Returns true if the section was copied, and false otherwise.  The
    //  source image <src> must have the same pixel format as this image
    //  in order for the copy to succeed.
    bool            copy(   const Image &src, 
                            int32 sx, int32 sy, int32 sw, int32 sh, 
                            int32 dx, int32 dy);

    bool            take(Image &src);

    void            swapRB();

    //**************** Image Search Functions *************************
    bool            findBox(int32 c, int32 *x, int32 *y, int32 *w, int32 *h) const;

    //isBoxEmpty(): checks a box in this image to test if it contains no non-black,
    //  non-transparent pixels.  A black pixel is considered as only containing the
    //  RGB values <0, 0, 0>.  A non-transparent pixel is considered as having an 
    //  alpha value of 0.  If any pixel in the box starting at <x, y> with size
    //  <w, h> contains a non-black pixel with an alpha value greater than 0, false
    //  is returned.  Otherwise true is returned.
    bool            isBoxEmpty(float32 x, float32 y, float32 w, float32 h) const;

    //**************** accessor functions *****************************
    bool            isValid() const{ return valid; }

    int32           getWidth() const{ return width; }
    int32           getHeight() const{ return height; }
    ImageDepth      getDepth() const{ return depth; }
    PixelFormat     getFormat() const{ return format; }
    int32           getComponents() const{ return components; }
    int32           getPitch() const{ return width * (depth / (sizeof(uint8) * 8)); }
    uint8 *         getImage() const{ return data; }
};

};  //namespace GFX;

#endif