/*
    File:   image.cpp
    Author: eric van beurden
    Date:   15/6/2006

    Namespaces:           GFX
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#include "image.h"
#include <math.h>
#include "trace.h"
#include <stdio.h>
#include <fstream>

//!
namespace GFX{

#define SWAPBYTES(x)  x = (((x) & 0x0f) << 4) | (((x) & 0xf0) >> 4)


#pragma pack(push,1)
typedef struct{
   uint8    idLen;       //ID
   uint8    mapType;     //color map type (paletted or RGB(A))
   uint8    imageType;   //tga format number
   int16    firstEntry;  //first color map entry
   int16    length;      //size of color map (usually 256)
   uint8    entrySize;   //size of each color map entry (usually 24-bit)
   int16    xOrigin;     //start x-coord of the image (usually 0)
   int16    yOrigin;     //start y-coord of the image (usually 0)
   int16    width;       //width of image
   int16    height;      //height of image
   uint8    depth;       //bit depth of image (usually 24 or 32)
   uint8    descriptor;  //reserved byte
} TgaHeader;
#pragma pack(pop)


Image::ImageDepth  formatDepths[Image::IMG_NUM_FORMATS] = {
    Image::IMG_DEPTH_4, 
    Image::IMG_DEPTH_8,
    Image::IMG_DEPTH_8, 
    Image::IMG_DEPTH_16, 
    Image::IMG_DEPTH_8,
    Image::IMG_DEPTH_16, 
    Image::IMG_DEPTH_16, 
    Image::IMG_DEPTH_24, 
    Image::IMG_DEPTH_16, 
    Image::IMG_DEPTH_16, 
    Image::IMG_DEPTH_32, 
    Image::IMG_DEPTH_32
};
int32       formatComponents[Image::IMG_NUM_FORMATS]={1, 1, 2, 2, 2, 2, 3, 3, 4, 4, 4, 4};


class TGA: public Image{
  public:
    void fillData(TgaHeader *tga){
      width = tga->width;
      height = tga->height;
      depth = (ImageDepth)tga->depth;
      bytes = width * height * (depth / 8);
      components = depth / 8;
      //memcpy(&lastHeader,tga,sizeof(lastHeader));
    }

    void TGADecompress(std::fstream *z){
      int32 pos=0;
      uint8 packet;
      int32 len;
      uint8 color[4] = {0};


      while (pos<bytes){

        z->read((char *)&packet, sizeof(uint8));
        len = (packet & 0x7F) + 1;

        if (packet & 0x80){  //RLE packet
          z->read((char *)color, sizeof(uint8) * components);

          if (components == 1){
            memset(data+pos,color[0],len);
            pos+=len;
          } 
          else{
            for (int32 i=0;i<len;i++){
              data[pos]  =color[2]; data[pos+1]=color[1];
              data[pos+2]=color[0];
              if (components==4) data[pos+3]=color[3];
              pos+=components;
            }
          }
        }

        else{  //RAW packet
          for (int32 i=0;i<len;i++){
            z->read((char *)color,sizeof(uint8)*components);

            if (components==1){
              memset(data+pos,color[0],len);
              pos+=len;
            }
            else{
              data[pos]  =color[2]; data[pos+1]=color[1];
              data[pos+2]=color[0];
              if (components==4) data[pos+3]=color[3];
              pos+=components;
            }
          }
        }
      }
    }
    bool TGARecompress(std::fstream *f) const{
      return false;
    }
    bool loader(const char *filename){
        std::fstream f(filename, std::ios::in | std::ios::binary);

        if (f.fail()){
          TRACE("loadTGA(): ERROR-> could not locate file \"%s\"\n",filename);
          valid=false;
          
          return false;
        }
        init();
        valid=true;

        TgaHeader header;
        f.read((char *)&header,sizeof(header));
        if (header.width<=0||header.height<=0){    //attempt to detect incorrect image file headers
            valid = false; 
            return false;
        }
        if (header.depth>32){ valid=false; return false; }
        if (header.idLen!=0){ valid=false; return false; }
        fillData(&header);

        data=new uint8[bytes];
        if (data==NULL){
          TRACE("loadTGA(): ERROR-> insufficient memory to allocate %ld bytes\n",bytes);
          valid=false;
          f.close();
          
          return false;
        }

      if (header.mapType==1){   //read and discard palette
        int32 bytes=(header.entrySize>>3)*header.length;
        uint8 *palette=new uint8[bytes];

        f.read((char *)palette,bytes);
        delete []palette;
      }
      //uncompressed image
      if (header.imageType==1||header.imageType==2||header.imageType==3){
        f.read((char *)data,sizeof(uint8)*bytes);

        //image is stored as BGR(A), make it RGB(A)
        if (components>=3){
            for (int32 i=0;i<bytes;i+=components){
                uint8 tmp = data[i];
                data[i] = data[i + 2];
                data[i + 2] = tmp;
            }
        }
      }
      //RLE compressed image
      else if (header.imageType==9||header.imageType==10)
        TGADecompress(&f);

      //if (header.descriptor!=32) flip();    //flip the image if it's stored upside down
      f.close();

      //fill in with NULL image here
      return valid;
    }
    bool saver(const char *filename) const{
      if (data==NULL||!valid) return false;
      int32 i;
      std::fstream f(filename, std::ios::out | std::ios::binary);
      TgaHeader header;

      if (f.fail())
        return false;
      
      //memcpy(&header,&lastHeader,sizeof(TgaHeader));
      header.idLen=0;         //dunno
      if (components==1){
        header.mapType=1;     //palette type
        header.imageType=1;   //storage format
        header.length=256;    //number of palette entries
        header.entrySize=24;  //size of each palette entry (bits)
      }
      else{
        header.mapType=0;     //palette type
        header.imageType=2;   //storage format (2,3=uncompressed RGB(A) data, 9,10=RLE compressed RGB(A) data)
        header.length=0;      //number of palette entries
        header.entrySize=0;   //size of each palette entry (bits)
      }
      header.firstEntry=0;    //first palette entry
      header.descriptor=32;   //dunno
      header.depth=depth;     //image's depth
      header.height=height;   //image's height
      header.width=width;     //image's width
      header.xOrigin=0;       //image's upper left x coord
      header.yOrigin=0;       //image's upper left y coord
      f.write((char *)&header,sizeof(TgaHeader));

      if (header.mapType==1){   //insert a grayscale palette
        int32 stride=header.entrySize>>3;
        int32 bytes=header.length*stride;
        uint8 *palette=new uint8[bytes];

        ((int32 *)palette)[0]=0xff00ff;   //set the first color index to a transparent color (magenta)
        for (int32 i=3;i<bytes;i+=stride)
          memset(&palette[i],i/stride,stride);

        f.write((char *)palette,bytes);
        delete []palette;
      }
      if (header.imageType >= 1 && header.imageType <= 3){
        if (components>=3){
            for (i=0;i<bytes;i+=components){    //reverse byte ordering for writing
                uint8 tmp = data[i];
                data[i] = data[i + 2];
                data[i + 2] = tmp;
            }
        }

        f.write((char *)data,sizeof(uint8)*bytes);

        if (components>=3){
            for (i=0;i<bytes;i+=components){    //re-reverse byte ordering for use
                uint8 tmp = data[i];
                data[i] = data[i + 2];
                data[i + 2] = tmp;
            }
        }
      }
      else if (header.imageType >= 9 && header.imageType <= 10)
          TGARecompress(&f);

      f.close();
      return true;
    }
};

void Image::init(){
  if (data)
      delete []data;

  data = NULL;
  width = height = components = bytes = 0;
  depth = IMG_DEPTH_NONE;
  format = IMG_FMT_NONE;
  valid = false;
}

Image::Image(){
  data = NULL;

  init();
}

Image::Image(const char *filename){
  data = NULL;

  init();

  load(filename, IMG_DEFAULT);
}

Image::Image(int32 w, int32 h, PixelFormat fmt){
  data = NULL;

  init();

  create(w, h, fmt);  
}

Image::~Image(){
  init();
}

//load(): loads an image from file.  The <type> parameter is can be used to
//  force the loader to use a specific loader type (ie: if the file's extension
//  isn't necessarily correct).  If the file is successfully loaded, the first
//  mipmap level (the first one smaller than or equal to the size of the image)
//  is generated automatically, and further levels can be generated using the 
//  nextLevel() function.
bool Image::load(const char *filename, int32 type){
    if (type==IMG_DEFAULT){   //check file extension/type for loader to use
      const char *ext = strrchr(filename,'.');
    	if (ext){
    		if (!strcasecmp(ext,".tga"))			type=IMG_TGA;
    	}
    }
    switch(type){
        case IMG_DEFAULT:		//first try loading as a TGA image, then a JPEG, then fail if neither works
        case IMG_TGA:		//load as a TGA image
            valid=((TGA *)this)->loader(filename);
            break;
        default:
            TRACE("Image::load(): could not locate appropriate loader for the file \"%s\"\n",filename);
            valid=false;
            break;
    }

    if (!valid){
        freeImage();

        return valid;
    }

    switch(depth){
        case 8:  format = IMG_FMT_8; break;
        case 16: format = IMG_FMT_88; break;
        case 24: format = IMG_FMT_888; break;
        case 32: format = IMG_FMT_8888; break;
    }

    components=formatComponents[format];

    return valid;
}

//saver(): dummy saving function that is overridden in format specific versions of
//  the Image class.  Returns false.
bool Image::saver(const char *filename) const{
  TRACE("saver(): ERROR: calling wrong function!\n");

  return false;
}

//loader(): dummy loading function that is overridden in format specific versions of
//  the Image class.  Returns false.
bool Image::loader(const char *filename){ 
  TRACE("loader(): ERROR: calling wrong function!\n");

  return false;
}


bool Image::save(const char *filename,int32 type) const{
    if (type==IMG_DEFAULT){
        const char *ext = strrchr(filename,'.');
        if (!strcasecmp(ext,".tga"))      type=IMG_TGA;
        else                              type=IMG_TGA;
    }

    switch(type){
        case IMG_TGA:
            return ((TGA *)this)->saver(filename);
            break;
    }

    return false;
}

//freeImage(): frees all the memory in use to hold this image.  This includes the
//  current mipmap image.
void Image::freeImage(){
  if (data)
      delete []data;

  data = NULL;
  width = height = bytes = components = 0;
  format = IMG_FMT_NONE;
  depth = IMG_DEPTH_NONE;
  valid = false;  
}


//flip(): flips the image in <data> either in the horizontal or vertical axis.
//  The first mipmap level is regenerated after the image is flipped.
void Image::flip(bool vert){
  if (vert){
    int32 storageWidth=bytes/height;
    uint8 *line=new uint8[storageWidth];
    for (int32 i=0;i<height/2;i++){
      memcpy(line,&data[i*storageWidth],storageWidth);
      memcpy(&data[i*storageWidth],&data[(height-i-1)*storageWidth],storageWidth);
      memcpy(&data[(height-i-1)*storageWidth],line,storageWidth);
    }
    delete line;
  }
  else{
    int32 storageWidth=bytes/height,pixelSize=depth/8,divider=2;
    bool flipNibbles=false;
    if (pixelSize==0){    //it's a 4 bit pixel mode
      divider=4;
      pixelSize=1;
      flipNibbles=true;
    }
    uint8 *pixel=new uint8[pixelSize];

    for (int32 i=0;i<height;i++){
      for (int32 j=0;j<width/divider;j++){
        memcpy(pixel,&data[i*storageWidth+j*pixelSize],pixelSize);
        memcpy(&data[i*storageWidth+j*pixelSize],&data[i*storageWidth+(width-j-1)*pixelSize],pixelSize);
        memcpy(&data[i*storageWidth+(width-j-1)*pixelSize],pixel,pixelSize);
      }
      if (flipNibbles){
        uint8 op;
        for (int32 i=0;i<bytes;i++){
          op=data[i];
          SWAPBYTES(op);
        }
      }
    }
    
  }
}

//create(): creates a new bitmap with the given dimensions, and format.  Returns false
//  if the <fmt> parameter is not valid, and true otherwise.
bool Image::create(int32 w, int32 h, PixelFormat fmt){
  if (data==NULL||w!=width||h!=height||format!=fmt){    //only change the memory if the size changed
    switch(fmt){        //make sure a valid format was passed in
      case IMG_FMT_4:
      case IMG_FMT_8:
      case IMG_FMT_44:
      case IMG_FMT_88:
      case IMG_FMT_62:
      case IMG_FMT_124:
      case IMG_FMT_555:
      case IMG_FMT_888:
      case IMG_FMT_4444:
      case IMG_FMT_5551:
      case IMG_FMT_8888:
      case IMG_FMT_1010102: break;
      default: return false;
    };
    
    if (data)
        delete []data;

    width = w;
    height = h;
    format = fmt;
    depth = formatDepths[format];
    components = formatComponents[format];
    bytes = (int32)(width * height * (depth / 8.0f));
    
    data = new uint8[bytes];
  }
  memset(data, 0, bytes);
  valid = true;

  return true;
}

//clear(): clears this image to the specified color value.  If <originalToo> is set
//  both the current mipmap and the original bitmap are cleared to <c>, otherwise,
//  only the current mipmap is cleared to <c>.  Returns true if the bitmap(s) are
//  cleared, and false if either the image was invalid, or if <originalToo> is cleared
//  and the mipmap image is the same as the original image.
bool Image::clear(uint32 c){
  if (!valid) return false;

  memset(data, c, bytes);    //clear out the original image

  return true;
}

void Image::drawBox(int32 x, int32 y, int32 w, int32 h, int32 c){
    int32 pitch = getPitch();
    int32 size = pitch / width;
    int32 offset;


    //nothing to draw => fail
    if (x > width || y > height)
        return;

    //clamp offscreen boxes
    if (x + w >= width)
        w = width - x;

    if (y + h >= height)
        h = height - y;


    if (x < 0){
        w += x;
        x = 0;
    }

    if (y < 0){
        h += y;
        y = 0;
    }


    switch (format){
        case IMG_FMT_8:
            for (int32 j = y; j < y + h; j++){
                for (int32 i = x; i < x + w; i++){
                    data[j * pitch + i * size] = (uint8)c;
                }
            }

            break;

        case IMG_FMT_88:
            for (int32 j = y; j < y + h; j++){
                for (int32 i = x; i < x + w; i++){
                    offset = j * pitch + i * size;

                    data[offset] = (uint8)(c & 0xff);
                    data[offset + 1] = (uint8)((c >> 8) & 0xff);
                }
            }

            break;

        case IMG_FMT_888:
            for (int32 j = y; j < y + h; j++){
                for (int32 i = x; i < x + w; i++){
                    offset = j * pitch + i * size;

                    data[offset] = (uint8)(c & 0xff);
                    data[offset + 1] = (uint8)((c >> 8) & 0xff);
                    data[offset + 2] = (uint8)((c >> 16) & 0xff);
                }
            }

            break;

        case IMG_FMT_8888:
            for (int32 j = y; j < y + h; j++){
                for (int32 i = x; i < x + w; i++){
                    offset = j * pitch + i * size;

                    data[offset] = (uint8)(c & 0xff);
                    data[offset + 1] = (uint8)((c >> 8) & 0xff);
                    data[offset + 2] = (uint8)((c >> 16) & 0xff);
                    data[offset + 3] = (uint8)((c >> 24) & 0xff);
                }
            }

            break;

    }
}

bool Image::findBox(int32 c, int32 *x, int32 *y, int32 *w, int32 *h) const{
    uint8 r = (uint8)(c & 0xff);
    uint8 g = (uint8)((c >> 8) & 0xff);
    uint8 b = (uint8)((c >> 16) & 0xff);
    uint8 a = (uint8)((c >> 24) & 0xff);
    int32 pitch = getPitch();
    int32 size = pitch / width;
    int32 offset;


    switch (format){
        case IMG_FMT_8:
            for (int32 j = 0; j < height; j++){
                for (int32 i = 0; i < width; i++){
                    if (data[j * pitch + i * size] == r){
                        *x = i;
                        *y = j;

                        break;
                    }
                }
            }

            for (int32 i = *x; i < width; i++){
                if (data[(*y) * pitch + i * size] != r)
                    *w = i;
            }

            for (int32 i = *y; i < height; i++){
                if (data[i * pitch + (*x) * size] != r)
                    *h = i;
            }

            break;

        case IMG_FMT_88:
            for (int32 j = 0; j < height; j++){
                for (int32 i = 0; i < width; i++){
                    offset = j * pitch + i * size;

                    if (data[offset] == r &&
                        data[offset + 1] == g){

                        *x = i;
                        *y = j;

                        break;
                    }
                }
            }


            for (int32 i = *x; i < width; i++){
                offset = (*y) * pitch + i * size;

                if (data[offset] != r &&
                    data[offset + 1] != g)
                    *w = i;
            }

            for (int32 i = *y; i < height; i++){
                offset = i * pitch + (*x) * size;

                if (data[offset] != r &&
                    data[offset + 1] != g)
                    *h = i;
            }

            break;

        case IMG_FMT_888:
            for (int32 j = 0; j < height; j++){
                for (int32 i = 0; i < width; i++){
                    offset = j * pitch + i * size;

                    if (data[offset] == r &&
                        data[offset + 1] == g &&
                        data[offset + 2] == b){

                        *x = i;
                        *y = j;

                        break;
                    }
                }
            }


            for (int32 i = *x; i < width; i++){
                offset = (*y) * pitch + i * size;

                if (data[offset] != r &&
                    data[offset + 1] != g &&
                    data[offset + 2] != b)
                    *w = i;
            }

            for (int32 i = *y; i < height; i++){
                offset = i * pitch + (*x) * size;

                if (data[offset] != r &&
                    data[offset + 1] != g &&
                    data[offset + 2] != b)
                    *h = i;
            }

            break;

        case IMG_FMT_8888:
            for (int32 j = 0; j < height; j++){
                for (int32 i = 0; i < width; i++){
                    offset = j * pitch + i * size;

                    if (data[offset] == r &&
                        data[offset + 1] == g &&
                        data[offset + 2] == b &&
                        data[offset + 3] == a){

                        *x = i;
                        *y = j;

                        break;
                    }
                }
            }


            for (int32 i = *x; i < width; i++){
                offset = (*y) * pitch + i * size;

                if (data[offset] != r &&
                    data[offset + 1] != g &&
                    data[offset + 2] != b &&
                    data[offset + 3] != a)
                    *w = i;
            }

            for (int32 i = *y; i < height; i++){
                offset = i * pitch + (*x) * size;

                if (data[offset] != r &&
                    data[offset + 1] != g &&
                    data[offset + 2] != b &&
                    data[offset + 3] != a)
                    *h = i;
            }

            break;
    }
    

    return true;
}


//isBoxEmpty(): checks a box in this image to test if it contains no non-black,
//  non-transparent pixels.  A black pixel is considered as only containing the
//  RGB values <0, 0, 0>.  A non-transparent pixel is considered as having an 
//  alpha value of 0.  If any pixel in the box starting at <x, y> with size
//  <w, h> contains a non-black pixel with an alpha value greater than 0, false
//  is returned.  Otherwise true is returned.
bool Image::isBoxEmpty(float32 x, float32 y, float32 w, float32 h) const{
    int32 pitch = getPitch();
    int32 size = pitch / width;
    int32 offset;
    int32 sx;
    int32 sy;
    int32 sw;
    int32 sh;


    //clamp the section to the image size and reject bad sections
    if (x > 1.0f || y > 1.0f || w < 0.0f || h < 0.0f)
        return true;


    if (x < 0.0f){
        w += x;
        x = 0.0f;
    }

    if (y < 0.0f){
        h += y;
        y = 0.0f;
    }


    if (x + w > 1.0f)
        w = 1.0f - x;

    if (y + h > 1.0f)
        h = 1.0f - y;


    sx = (int32)(x * width);
    sy = (int32)(y * height);
    sw = (int32)(w * width);
    sh = (int32)(h * height);


    for (int32 j = sy; j < sy + sh; j++){
        for (int32 i = sx; i < sx + sw; i++){
            offset = j * pitch + i * size;

            //the color is non-black or non-transparent => box is not empty
            if (!isColorEmpty(offset))
                return false;
        }
    }


    return true;
}

bool Image::isColorEmpty(int32 offset) const{
    switch (format){
        case IMG_FMT_8:
            return data[offset] == 0;

        case IMG_FMT_88:
            return  data[offset] == 0 || 
                    data[offset + 1] == 0;

        case IMG_FMT_888:
            return  data[offset] == 0 && 
                    data[offset + 1] == 0 && 
                    data[offset + 2] == 0;

        case IMG_FMT_8888:
            return  (data[offset] == 0 && 
                     data[offset + 1] == 0 && 
                     data[offset + 2] == 0) || 
                    data[offset + 3] == 0;
    }

    return false;
}


//copy(): copies a rectangular section from the source image <src>
//  to this image.  The rectangular section begins at the point <sx, sy>
//  and has the size <sw, sh>.  The section is copied to a rectangle
//  of the same size in this image beginning at the point <dx, dy>.
//  Returns true if the section was copied, and false otherwise.  The
//  source image <src> must have the same pixel format as this image
//  in order for the copy to succeed.
bool Image::copy(const Image &src, int32 sx, int32 sy, int32 sw, int32 sh, int32 dx, int32 dy){
    if (src.getFormat() != format)
        return false;


    int32 srcPitch = src.getPitch();
    int32 dstPitch = getPitch();
    //source pixel size & destination pixel size have to be the same due to the check above
    int32 size = dstPitch / width;
    /*int32 srcx;
    int32 srcy;
    int32 srcw;
    int32 srch;
    int32 dstx;
    int32 dsty;//*/


    //clamp the ranges to the source image size and reject invalid regions
    if (sx > src.width || sy > src.height || dx > width || dy > height)// || sw < width || sh < height)
        return false;


    if (sx < 0){
        sw += sx;
        sx = 0;
    }

    if (sy < 0){
        sh += sy;
        sy = 0;
    }


    if (sx + sw > src.width)
        sw = src.width - sx;

    if (sy + sh > src.height)
        sh = src.height - sy;


    if (dx + sw > width)
        sw = width - dx;

    if (dy + sh > height)
        sh = height - dy;


    /*srcx = (int32)(sx * (src.getWidth() - 1));
    srcy = (int32)(sy * (src.getHeight() - 1));
    srcw = (int32)(sw * (src.getWidth() - 1));
    srch = (int32)(sh * (src.getHeight() - 1));
    dstx = (int32)(dx * (width - 1));
    dsty = (int32)(dy * (height - 1));//*/


    for (int32 j = 0; j < sh; j++){
        memcpy( &data[(dy + j) * dstPitch + dx * size],
                &src.data[(sy + j) * srcPitch + sx * size],
                sw * size);
    }

    return true;
}


bool Image::take(Image &src){
    if (!src.valid)
        return false;


    freeImage();

    data =          src.data;
    width =         src.width;
    height =        src.height;
    depth =         src.depth;
    components =    src.components;
    format =        src.format;
    bytes =         src.bytes;
    valid =         src.valid;

    src.data =      NULL;
    src.freeImage();

    return valid;
}

void Image::swapRB(){
    int32       pitch = getPitch();
    int32       size = pitch / width;
    uint8       tmp;
    uint8 *     tmpData;


    switch (format){
        case IMG_FMT_888:
        case IMG_FMT_8888:
            for (int32 j = 0; j < height; j++){
                for (int32 i = 0; i < width; i++){
                    tmpData = &data[j * pitch + i * size];

                    tmp = tmpData[0];
                    tmpData[0] = tmpData[2];
                    tmpData[2] = tmp;
                }
            }
    }
}


};  //namespace GFX;
