/*
  File: pixelConversion.cpp
  Author: Eric van Beurden

  provides pixel conversion functions for the (currently) 12 pixel formats supported
  by the Image class.  These conversion functions are not shared, but are instead 
  packed into an array that is indexible by the format number itself.

// ** INSTRUCTIONS: read the comment sections starting with '// **' for instructions
// ** on what needs to be done
*/

#include "pixelConversion.h"
#include <stdio.h>
#include <memory.h>

#if defined(_DEBUG)&&defined(__FIND_LEAKS__)
# include <debugMalloc.h>
#endif

ConversionFunc convert[IMG_NUM_FORMATS][IMG_NUM_FORMATS]={0}; //array of conversion functions
bool conversionTableInitialized=false;

// ** these are the pixel formats that we are supporting (at the moment at least).  The
// ** numbers indicate how many bits of R (if only one number), RA (if two numbers), 
// ** RGB (if three numbers), or RGBA (if four numbers) there are (eg: IMG_FMT_1010102 
// ** has 10 bits each of red, green, and blue, and 2 bits of alpha).
/*
#define IMG_FMT_4         0
#define IMG_FMT_8         1
#define IMG_FMT_44        2
#define IMG_FMT_88        3
#define IMG_FMT_62        4
#define IMG_FMT_124       5 
#define IMG_FMT_555       6
#define IMG_FMT_888       7
#define IMG_FMT_4444      8
#define IMG_FMT_5551      9
#define IMG_FMT_8888      10
#define IMG_FMT_1010102   11
#define IMG_NUM_FORMATS   12
//*/

/*ALPHA_GEN_DEFAULT
ALPHA_GEN_TRANSPARENT
ALPHA_GEN_BLEND_R
ALPHA_GEN_BLEND_G
ALPHA_GEN_BLEND_B
ALPHA_GEN_BLEND_I//*/

// ** Here's the functions i've done so far.  They cover the conversion from the 
// ** 8888 RGBA format to all the other 11 formats.  Don't worry about making a 
// ** function that converts a pixel to its own format.  That function will never
// ** be called (that case is checked).  I've been naming the functions conv<X>to<Y>()
// ** where <X> is the format of the <src> parameter, and <Y> is the format of the
// ** <dest> parameter.  <dest> is a pointer to the destination for the pixel (eg:
// ** dest[0]=R, dest[1]=G, dest[2]=B, dest[3]=A for the 8888 format).  <src> is 
// ** the source pixel for the conversion.  In the case of a <Y>=4 function, the 
// ** function uses the <high> parameter to determine whether to put the destination
// ** pixel value in the high bit, or the low nibble of the <dest> pointer.  In the
// ** case of an <X>=4 function, the <high> parameter is used to determine which 
// ** nibble to take the source pixel from.  In all other cases, the <high> parameter
// ** can be ignored (and will always be false for that matter).
// ** WARNING: shifting left, then right (ie: (x<<5)>>3 )  DOES NOT give the results 
// ** you want (ie: data not clipped to 8 bits).  All immediate operations are done 
// ** as ints, so when you shift back, the data the you would expect to be zeroes is
// ** actually the data that was shifted out of the first bit (i tried to simplify 
// ** the converters, and this was the problem i ran into).  You can actually get 
// ** around this by setting up an appropriate bit mast and ANDing it with the value 
// ** before you shift the second time.  I was too lazy to figure out all the rules
// ** for doing this, so i just made all of the functions use unsigned char (or short)
// ** intermediate variables to hold the results.

void conv8to4(unsigned char *dest,unsigned char *src,int high){
  if (high) dest[0]|=src[0]&0xf0;
  else      dest[0]|=src[0]>>4;
}
void conv8to44(unsigned char *dest,unsigned char *src,int high){ 
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=src[0]>>4|(src[0]?0xf0:0x00); break;
    case ALPHA_GEN_BLEND_R:     
    case ALPHA_GEN_BLEND_G:     
    case ALPHA_GEN_BLEND_B:     
    case ALPHA_GEN_BLEND_I:
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=src[0]>>4|(src[0]&0xf0); break;
  }
  //dest[0]=src[0]>>4|(src[0]&0xf0);
}

void conv8to88(unsigned char *dest,unsigned char *src,int high){
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=src[0]; dest[1]=src[0]?0xff:0x00;break;
    case ALPHA_GEN_BLEND_R:     
    case ALPHA_GEN_BLEND_G:     
    case ALPHA_GEN_BLEND_B:     
    case ALPHA_GEN_BLEND_I:
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=src[0]; dest[1]=src[0];break;
  }
  //dest[0]=src[0]; dest[1]=src[1];
}
void conv8to62(unsigned char *dest,unsigned char *src,int high){
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=(src[0]>>2)|(src[0]?0xc0:0x00); break;
    case ALPHA_GEN_BLEND_R:     
    case ALPHA_GEN_BLEND_G:     
    case ALPHA_GEN_BLEND_B:     
    case ALPHA_GEN_BLEND_I:
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=(src[0]>>2)|(src[0]&0xc0); break;
  }
  //dest[0]=(src[0]>>2)|(src[0]&0xc0);
}
void conv8to124(unsigned char *dest,unsigned char *src,int high){
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=(src[0]<<4)&0xff; dest[1]=((src[0]>>4)&0x0f)|(src[0]?0xf0:0x00); break;
    case ALPHA_GEN_BLEND_R:     
    case ALPHA_GEN_BLEND_G:     
    case ALPHA_GEN_BLEND_B:     
    case ALPHA_GEN_BLEND_I:
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=(src[0]<<4)&0xff; dest[1]=((src[0]>>4)&0x0f)|(src[0]&0xf0); break;
  }
  //dest[0]=(src[0]<<4)&0xff; dest[1]=((src[0]>>4)&0x0f)|(src[0]&0xf0);
}
void conv8to888(unsigned char *dest,unsigned char *src,int high){ dest[0]=src[0]; dest[1]=src[0]; dest[2]=src[0]; }
void conv8to555(unsigned char *dest,unsigned char *src,int high){
  unsigned char r=src[0]>>3;
  dest[0]=r|(r<<5);
  dest[1]=(r>>3)|(r<<2);
}
void conv8to4444(unsigned char *dest,unsigned char *src,int high){
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=(src[0]>>4)|(src[0]&0xf0); dest[1]=(src[0]>>4)|(src[0]?0xf0:0x00); break;
    case ALPHA_GEN_BLEND_R:     
    case ALPHA_GEN_BLEND_G:     
    case ALPHA_GEN_BLEND_B:     
    case ALPHA_GEN_BLEND_I:
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=(src[0]>>4)|(src[0]&0xf0); dest[1]=(src[0]>>4)|(src[0]&0xf0); break;
  }
  //dest[0]=(src[0]>>4)|(src[0]&0xf0);
  //dest[1]=(src[0]>>4)|(src[0]&0xf0);
}
void conv8to5551(unsigned char *dest,unsigned char *src,int high){
  unsigned char r=src[0]>>3;
  dest[0]=r|(r<<5);
  dest[1]=(r>>3)|(r<<2)|(src[0]?0x80:0x00);
  
}
void conv8to8888(unsigned char *dest,unsigned char *src,int high){
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=src[0]; dest[1]=src[0]; dest[2]=src[0]; dest[3]=src[0]?0xff:0x00; break;
    case ALPHA_GEN_BLEND_R:     
    case ALPHA_GEN_BLEND_G:     
    case ALPHA_GEN_BLEND_B:     
    case ALPHA_GEN_BLEND_I:
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=src[0]; dest[1]=src[0]; dest[2]=src[0]; dest[3]=src[0]; break;
  }

  //dest[0]=src[0]; dest[1]=src[0]; dest[2]=src[0]; dest[3]=src[0]; 
}
void conv8to1010102(unsigned char *dest,unsigned char *src,int high){
  unsigned short r=src[0]<<2,a=0;
  switch (high){
    case ALPHA_GEN_TRANSPARENT: a=src[0]?0xc0:0x00; break;
    case ALPHA_GEN_BLEND_R:     
    case ALPHA_GEN_BLEND_G:     
    case ALPHA_GEN_BLEND_B:     
    case ALPHA_GEN_BLEND_I:
    case ALPHA_GEN_DEFAULT:
    default:                     a=src[0]&0xc0; break;
  }

  dest[0]=r&0xff;
  dest[1]=(r>>8)|((r<<2)&0xff);
  dest[2]=(r>>6)|((r<<4)&0xff);
  dest[3]=(r>>4)|a;
}

void conv8888to4(unsigned char *dest,unsigned char *src,int high){
  if (high) dest[0]|=src[0]&0xf0;
  else      dest[0]|=src[0]>>4;
}

void conv8888to8(unsigned char *dest,unsigned char *src,int high){
  short intensity=(src[0]+src[1]+src[2])/3;
  dest[0]=intensity&0xff;
}
void conv8888to44(unsigned char *dest,unsigned char *src,int high){
  short intensity=(src[0]+src[1]+src[2])/3;
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=(intensity>>4)|(src[3]?0xf0:0x00); break;
    case ALPHA_GEN_BLEND_R:     dest[0]=(intensity>>4)|(src[0]&0xf0); break;
    case ALPHA_GEN_BLEND_G:     dest[0]=(intensity>>4)|(src[1]&0xf0); break;
    case ALPHA_GEN_BLEND_B:     dest[0]=(intensity>>4)|(src[2]&0xf0); break;
    case ALPHA_GEN_BLEND_I:     dest[0]=(intensity>>4)|(intensity&0xf0); break;
    case ALPHA_GEN_DEFAULT:     
    default:                    dest[0]=(intensity>>4)|(src[3]&0xf0); break;
  }
  //dest[0]=(src[0]>>4)|(src[3]&0xf0);
}
void conv8888to88(unsigned char *dest,unsigned char *src,int high){
  short intensity=(src[0]+src[1]+src[2])/3;
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=intensity&0xff; dest[1]=src[3]; break;  //?0xff:0x00; break;
    case ALPHA_GEN_BLEND_R:     dest[0]=intensity&0xff; dest[1]=src[3]; break;  //src[0]; break;
    case ALPHA_GEN_BLEND_G:     dest[0]=intensity&0xff; dest[1]=src[3]; break;  //src[1]; break;
    case ALPHA_GEN_BLEND_B:     dest[0]=intensity&0xff; dest[1]=src[3]; break;  //src[2]; break;
    case ALPHA_GEN_BLEND_I:     dest[0]=intensity&0xff; dest[1]=src[3]; break;  //(src[0]+src[1]+src[2])/3; break;
    case ALPHA_GEN_DEFAULT:     
    default:                    dest[0]=intensity&0xff; dest[1]=src[3]; break;
  }
  //dest[0]=src[0]; dest[1]=src[3];
}
void conv8888to62(unsigned char *dest,unsigned char *src,int high){
  short intensity=(src[0]+src[1]+src[2])/3;
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=(intensity>>2)|(src[3]?0xc0:0x00); break;
    case ALPHA_GEN_BLEND_R:     dest[0]=(intensity>>2)|(src[0]&0xc0); break;
    case ALPHA_GEN_BLEND_G:     dest[0]=(intensity>>2)|(src[1]&0xc0); break;
    case ALPHA_GEN_BLEND_B:     dest[0]=(intensity>>2)|(src[2]&0xc0); break;
    case ALPHA_GEN_BLEND_I:     dest[0]=(intensity>>2)|(((src[0]+src[1]+src[2])/3)&0xc0); break;
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=(intensity>>2)|(src[3]&0xc0); break;
  }
  //dest[0]=(src[0]>>2)|(src[3]&0xc0);
}
void conv8888to124(unsigned char *dest,unsigned char *src,int high){
  short intensity=(src[0]+src[1]+src[2])/3;
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=(intensity<<4)&0xff; dest[1]=((intensity>>4)&0x0f)|(src[3]?0xf0:0x00); break;
    case ALPHA_GEN_BLEND_R:     dest[0]=(intensity<<4)&0xff; dest[1]=((intensity>>4)&0x0f)|(src[0]&0xf0); break;
    case ALPHA_GEN_BLEND_G:     dest[0]=(intensity<<4)&0xff; dest[1]=((intensity>>4)&0x0f)|(src[1]&0xf0); break;
    case ALPHA_GEN_BLEND_B:     dest[0]=(intensity<<4)&0xff; dest[1]=((intensity>>4)&0x0f)|(src[2]&0xf0); break;
    case ALPHA_GEN_BLEND_I:     dest[0]=(intensity<<4)&0xff; dest[1]=((intensity>>4)&0x0f)|(((src[0]+src[1]+src[2])/3)&0xf0); break;
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=(intensity<<4)&0xff; dest[1]=((intensity>>4)&0x0f)|(src[3]&0xf0); break;
  }

  //dest[0]=(src[0]<<4)&0xff; dest[1]=((src[0]>>4)&0x0f)|(src[3]&0xf0);
}

void conv8888to888(unsigned char *dest,unsigned char *src,int high){ dest[0]=src[0]; dest[1]=src[1]; dest[2]=src[2]; }

void conv8888to555(unsigned char *dest,unsigned char *src,int high){ 
  unsigned char r=src[0]>>3,g=src[1]>>3,b=src[2]>>3;
  dest[0]=r|(g<<5);
  dest[1]=(g>>3)|(b<<2);
}

void conv8888to4444(unsigned char *dest,unsigned char *src,int high){
  short intensity=(src[0]+src[1]+src[2])/3;
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=(src[0]>>4)|(src[1]&0xf0); dest[1]=(src[2]>>4)|(src[3]?0xf0:0x00); break;
    case ALPHA_GEN_BLEND_R:     dest[0]=(src[0]>>4)|(src[1]&0xf0); dest[1]=(src[2]>>4)|(src[0]&0xf0); break;
    case ALPHA_GEN_BLEND_G:     dest[0]=(src[0]>>4)|(src[1]&0xf0); dest[1]=(src[2]>>4)|(src[1]&0xf0); break;
    case ALPHA_GEN_BLEND_B:     dest[0]=(src[0]>>4)|(src[1]&0xf0); dest[1]=(src[2]>>4)|(src[2]&0xf0); break;
    case ALPHA_GEN_BLEND_I:     dest[0]=(src[0]>>4)|(src[1]&0xf0); dest[1]=(src[2]>>4)|(intensity&0xf0); break;
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=(src[0]>>4)|(src[1]&0xf0); dest[1]=(src[2]>>4)|(src[3]&0xf0); break;
  }


  //dest[0]=(src[0]>>4)|(src[1]&0xf0); dest[1]=(src[2]>>4)|(src[3]&0xf0);
  //dest[1]=(src[2]>>4)|(src[3]&0xf0);
}

void conv8888to5551(unsigned char *dest,unsigned char *src,int high){
  unsigned char r=src[0]>>3,g=src[1]>>3,b=src[2]>>3;
  dest[0]=r|(g<<5);
  dest[1]=(g>>3)|(b<<2)|(src[3]?0x80:0x00);
}

void conv8888to1010102(unsigned char *dest,unsigned char *src,int high){
  unsigned short r=src[0]<<2,g=src[1]<<2,b=src[2]<<2,a=src[3]&0xc0;
  switch (high){
    case ALPHA_GEN_TRANSPARENT: a=src[3]?0xc0:0x00; break;
    case ALPHA_GEN_BLEND_R:     a=src[0]&0xc0; break;
    case ALPHA_GEN_BLEND_G:     a=src[1]&0xc0; break;
    case ALPHA_GEN_BLEND_B:     a=src[2]&0xc0; break;
    case ALPHA_GEN_BLEND_I:     a=((src[0]=src[1]+src[2])/3)&0xc0; break;
    case ALPHA_GEN_DEFAULT:
    default:                    a=src[0]&0xc0; break;
  }

  dest[0]=r&0xff;
  dest[1]=(r>>8)|((g<<2)&0xff);
  dest[2]=(g>>6)|((b<<4)&0xff);
  dest[3]=(b>>4)|a;
}

void conv888to4(unsigned char *dest,unsigned char *src,int high){
  short intensity=(src[0]+src[1]+src[2])/3;
  if (high) dest[0]|=intensity&0x0f0;
  else      dest[0]|=intensity>>4;
}

void conv888to8(unsigned char *dest,unsigned char *src,int high){
  short intensity=(src[0]+src[1]+src[2])/3;
  dest[0]=intensity&0xff;
}

void conv888to44(unsigned char *dest,unsigned char *src,int high){
  short intensity=(src[0]+src[1]+src[2])/3;
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=(intensity>>4)|(intensity?0xf0:0x00); break;
    case ALPHA_GEN_BLEND_R:     dest[0]=(intensity>>4)|(src[0]&0xf0); break;
    case ALPHA_GEN_BLEND_G:     dest[0]=(intensity>>4)|(src[1]&0xf0); break;
    case ALPHA_GEN_BLEND_B:     dest[0]=(intensity>>4)|(src[2]&0xf0); break;
    case ALPHA_GEN_BLEND_I:
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=(intensity>>4)|(intensity&0xf0); break;
  }

  //dest[0]=(src[0]>>4)|(intensity&0xf0);
}
void conv888to88(unsigned char *dest,unsigned char *src,int high){
  short intensity=(src[0]+src[1]+src[2])/3;
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=intensity&0xff; dest[1]=intensity?0xff:0x00; break;
    case ALPHA_GEN_BLEND_R:     dest[0]=intensity&0xff; dest[1]=src[0]&0xff; break;
    case ALPHA_GEN_BLEND_G:     dest[0]=intensity&0xff; dest[1]=src[1]&0xff; break;
    case ALPHA_GEN_BLEND_B:     dest[0]=intensity&0xff; dest[1]=src[2]&0xff; break;
    case ALPHA_GEN_BLEND_I:
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=intensity&0xff; dest[1]=intensity&0xff; break;
  }
  //dest[0]=src[0]; dest[1]=intensity&0xf0;
}
void conv888to62(unsigned char *dest,unsigned char *src,int high){
  short intensity=(src[0]+src[1]+src[2])/3;
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=(intensity>>2)|(intensity?0xc0:0x00); break;
    case ALPHA_GEN_BLEND_R:     dest[0]=(intensity>>2)|(src[0]&0xc0); break;
    case ALPHA_GEN_BLEND_G:     dest[0]=(intensity>>2)|(src[1]&0xc0); break;
    case ALPHA_GEN_BLEND_B:     dest[0]=(intensity>>2)|(src[2]&0xc0); break;
    case ALPHA_GEN_BLEND_I:
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=(intensity>>2)|(intensity&0xc0); break;
  }
  //dest[0]=(src[0]>>2)|(intensity&0xc0);
}
void conv888to124(unsigned char *dest,unsigned char *src,int high){
  short intensity=(src[0]+src[1]+src[2])/3;
  short r=src[0]<<4;
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=(r&0xff); dest[1]=(r>>8)|(intensity?0xf0:0x00); break;
    case ALPHA_GEN_BLEND_R:     dest[0]=(r&0xff); dest[1]=(r>>8)|(src[0]&0xf0); break;
    case ALPHA_GEN_BLEND_G:     dest[0]=(r&0xff); dest[1]=(r>>8)|(src[1]&0xf0); break;
    case ALPHA_GEN_BLEND_B:     dest[0]=(r&0xff); dest[1]=(r>>8)|(src[2]&0xf0); break;
    case ALPHA_GEN_BLEND_I:
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=(r&0xff); dest[1]=(r>>8)|(intensity&0xf0); break;
  }
  //dest[0]=(r&0xff); dest[1]=(r>>8)|(intensity&0xf0);
}
void conv888to8888(unsigned char *dest,unsigned char *src,int high){
  short intensity=(src[0]+src[1]+src[2])/3;
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=src[0]; dest[1]=src[1]; dest[2]=src[2]; dest[3]=intensity?0xff:0x00; break;
    case ALPHA_GEN_BLEND_R:     dest[0]=src[0]; dest[1]=src[1]; dest[2]=src[2]; dest[3]=src[0]&0xff; break;
    case ALPHA_GEN_BLEND_G:     dest[0]=src[0]; dest[1]=src[1]; dest[2]=src[2]; dest[3]=src[1]&0xff; break;
    case ALPHA_GEN_BLEND_B:     dest[0]=src[0]; dest[1]=src[1]; dest[2]=src[2]; dest[3]=src[2]&0xff; break;
    case ALPHA_GEN_BLEND_I:
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=src[0]; dest[1]=src[1]; dest[2]=src[2]; dest[3]=intensity&0xff; break;
  }
  //dest[0]=src[0]; dest[1]=src[1]; dest[2]=src[2]; dest[3]=intensity&0xff;
}

void conv888to555(unsigned char *dest,unsigned char *src,int high){
  unsigned char r=src[0]>>3,g=src[1]>>3,b=src[2]>>3;
  dest[0]=r|(g<<5); dest[1]=(g>>3)|(b<<2);
}

void conv888to4444(unsigned char *dest,unsigned char *src,int high){
  short intensity=(src[0]+src[1]+src[2])/3;
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=(src[0]>>4)|(src[1]&0xf0); dest[1]=(src[2]>>4)|(intensity?0xf0:0x00); break;
    case ALPHA_GEN_BLEND_R:     dest[0]=(src[0]>>4)|(src[1]&0xf0); dest[1]=(src[2]>>4)|(src[0]&0xf0); break;
    case ALPHA_GEN_BLEND_G:     dest[0]=(src[0]>>4)|(src[1]&0xf0); dest[1]=(src[2]>>4)|(src[1]&0xf0); break;
    case ALPHA_GEN_BLEND_B:     dest[0]=(src[0]>>4)|(src[1]&0xf0); dest[1]=(src[2]>>4)|(src[2]&0xf0); break;
    case ALPHA_GEN_BLEND_I:
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=(src[0]>>4)|(src[1]&0xf0); dest[1]=(src[2]>>4)|(intensity&0xf0); break;
  }
  //dest[0]=(src[0]>>4)|(src[1]&0xf0);
  //dest[1]=(src[2]>>4)|(intensity&0xf0);
}

void conv888to5551(unsigned char *dest,unsigned char *src,int high){
  short intensity=(src[0]+src[1]+src[2])/3;
  unsigned char r=src[0]>>3,g=src[1]>>3,b=src[2]>>3;
  dest[0]=r|(g<<5); dest[1]=(g>>3)|(b<<2)|(intensity?0x80:0x00);
}

void conv888to1010102(unsigned char *dest,unsigned char *src,int high){
  short r=src[0]<<2,g=src[1]<<2,b=src[2]<<2;
  short intensity=(src[0]+src[1]+src[2])/3;
  switch (high){
    case ALPHA_GEN_TRANSPARENT: dest[0]=r&0xff; dest[1]=(r>>8)|(g<<2); dest[2]=(g>>6)|(b<<4); dest[3]=(b>>4)|(intensity?0xc0:0x00); break;
    case ALPHA_GEN_BLEND_R:     dest[0]=r&0xff; dest[1]=(r>>8)|(g<<2); dest[2]=(g>>6)|(b<<4); dest[3]=(b>>4)|(src[0]&0xc0); break;
    case ALPHA_GEN_BLEND_G:     dest[0]=r&0xff; dest[1]=(r>>8)|(g<<2); dest[2]=(g>>6)|(b<<4); dest[3]=(b>>4)|(src[1]&0xc0); break;
    case ALPHA_GEN_BLEND_B:     dest[0]=r&0xff; dest[1]=(r>>8)|(g<<2); dest[2]=(g>>6)|(b<<4); dest[3]=(b>>4)|(src[2]&0xc0); break;
    case ALPHA_GEN_BLEND_I:
    case ALPHA_GEN_DEFAULT:
    default:                    dest[0]=r&0xff; dest[1]=(r>>8)|(g<<2); dest[2]=(g>>6)|(b<<4); dest[3]=(b>>4)|(intensity&0xc0); break;
  }
  //dest[0]=r&0xff; dest[1]=(r>>8)|(g<<2); dest[2]=(g>>6)|(b<<4); dest[3]=(b>>4)|(intensity&0xc0);
}


//initConversionTable(): fills in the conversion table with the function names.
void initConversionTable(){
  if (conversionTableInitialized) return;
  memset(convert,0,sizeof(ConversionFunc)*(IMG_NUM_FORMATS)*(IMG_NUM_FORMATS));

// ** when you finish a set of functions, uncomment the affected section below
// ** (the one that assigns the functions, that you just made, to the array).

  /*convert[IMG_FMT_4][IMG_FMT_8]=      conv4to8; 
  convert[IMG_FMT_4][IMG_FMT_44]=     conv4to44;
  convert[IMG_FMT_4][IMG_FMT_88]=     conv4to88;
  convert[IMG_FMT_4][IMG_FMT_62]=     conv4to62;
  convert[IMG_FMT_4][IMG_FMT_124]=    conv4to124;
  convert[IMG_FMT_4][IMG_FMT_555]=    conv4to555;
  convert[IMG_FMT_4][IMG_FMT_888]=    conv4to888;
  convert[IMG_FMT_4][IMG_FMT_4444]=   conv4to4444;
  convert[IMG_FMT_4][IMG_FMT_5551]=   conv4to5551;
  convert[IMG_FMT_4][IMG_FMT_8888]=   conv4to8888;
  convert[IMG_FMT_4][IMG_FMT_1010102]=conv4to1010102;//*/

  convert[IMG_FMT_8][IMG_FMT_4]=      conv8to4;
  convert[IMG_FMT_8][IMG_FMT_44]=     conv8to44;
  convert[IMG_FMT_8][IMG_FMT_88]=     conv8to88;
  convert[IMG_FMT_8][IMG_FMT_62]=     conv8to62;
  convert[IMG_FMT_8][IMG_FMT_124]=    conv8to124;
  convert[IMG_FMT_8][IMG_FMT_555]=    conv8to555;
  convert[IMG_FMT_8][IMG_FMT_888]=    conv8to888;
  convert[IMG_FMT_8][IMG_FMT_4444]=   conv8to4444;
  convert[IMG_FMT_8][IMG_FMT_5551]=   conv8to5551;
  convert[IMG_FMT_8][IMG_FMT_8888]=   conv8to8888;
  convert[IMG_FMT_8][IMG_FMT_1010102]=conv8to1010102;//*/

  /*convert[IMG_FMT_44][IMG_FMT_4]=      conv44to4;
  convert[IMG_FMT_44][IMG_FMT_8]=      conv44to8; 
  convert[IMG_FMT_44][IMG_FMT_88]=     conv44to88;
  convert[IMG_FMT_44][IMG_FMT_62]=     conv44to62;
  convert[IMG_FMT_44][IMG_FMT_124]=    conv44to124;
  convert[IMG_FMT_44][IMG_FMT_555]=    conv44to555;
  convert[IMG_FMT_44][IMG_FMT_888]=    conv44to888;
  convert[IMG_FMT_44][IMG_FMT_4444]=   conv44to4444;
  convert[IMG_FMT_44][IMG_FMT_5551]=   conv44to5551;
  convert[IMG_FMT_44][IMG_FMT_8888]=   conv44to8888;
  convert[IMG_FMT_44][IMG_FMT_1010102]=conv44to1010102;//*/

  /*convert[IMG_FMT_88][IMG_FMT_4]=      conv88to4;
  convert[IMG_FMT_88][IMG_FMT_8]=      conv88to8; 
  convert[IMG_FMT_88][IMG_FMT_44]=     conv88to44;
  convert[IMG_FMT_88][IMG_FMT_62]=     conv88to62;
  convert[IMG_FMT_88][IMG_FMT_124]=    conv88to124;
  convert[IMG_FMT_88][IMG_FMT_555]=    conv88to555;
  convert[IMG_FMT_88][IMG_FMT_888]=    conv88to888;
  convert[IMG_FMT_88][IMG_FMT_4444]=   conv88to4444;
  convert[IMG_FMT_88][IMG_FMT_5551]=   conv88to5551;
  convert[IMG_FMT_88][IMG_FMT_8888]=   conv88to8888;
  convert[IMG_FMT_88][IMG_FMT_1010102]=conv88to1010102;//*/

  /*convert[IMG_FMT_62][IMG_FMT_4]=      conv62to4;
  convert[IMG_FMT_62][IMG_FMT_8]=      conv62to8;
  convert[IMG_FMT_62][IMG_FMT_44]=     conv62to44;
  convert[IMG_FMT_62][IMG_FMT_88]=     conv62to88;
  convert[IMG_FMT_62][IMG_FMT_124]=    conv62to124;
  convert[IMG_FMT_62][IMG_FMT_555]=    conv62to555;
  convert[IMG_FMT_62][IMG_FMT_888]=    conv62to888;
  convert[IMG_FMT_62][IMG_FMT_4444]=   conv62to4444;
  convert[IMG_FMT_62][IMG_FMT_5551]=   conv62to5551;
  convert[IMG_FMT_62][IMG_FMT_8888]=   conv62to8888;
  convert[IMG_FMT_62][IMG_FMT_1010102]=conv62to1010102;//*/

  /*convert[IMG_FMT_124][IMG_FMT_4]=      conv124to4;
  convert[IMG_FMT_124][IMG_FMT_8]=      conv124to8; 
  convert[IMG_FMT_124][IMG_FMT_44]=     conv124to44;
  convert[IMG_FMT_124][IMG_FMT_88]=     conv124to88;
  convert[IMG_FMT_124][IMG_FMT_62]=     conv124to62;
  convert[IMG_FMT_124][IMG_FMT_555]=    conv124to555;
  convert[IMG_FMT_124][IMG_FMT_888]=    conv124to888;
  convert[IMG_FMT_124][IMG_FMT_4444]=   conv124to4444;
  convert[IMG_FMT_124][IMG_FMT_5551]=   conv124to5551;
  convert[IMG_FMT_124][IMG_FMT_8888]=   conv124to8888;
  convert[IMG_FMT_124][IMG_FMT_1010102]=conv124to1010102;//*/

  /*convert[IMG_FMT_555][IMG_FMT_4]=      conv555to4;
  convert[IMG_FMT_555][IMG_FMT_8]=      conv555to8; 
  convert[IMG_FMT_555][IMG_FMT_44]=     conv555to44;
  convert[IMG_FMT_555][IMG_FMT_88]=     conv555to88;
  convert[IMG_FMT_555][IMG_FMT_62]=     conv555to62;
  convert[IMG_FMT_555][IMG_FMT_124]=    conv555to124;
  convert[IMG_FMT_555][IMG_FMT_888]=    conv555to888;
  convert[IMG_FMT_555][IMG_FMT_4444]=   conv555to4444;
  convert[IMG_FMT_555][IMG_FMT_5551]=   conv555to5551;
  convert[IMG_FMT_555][IMG_FMT_8888]=   conv555to8888;
  convert[IMG_FMT_555][IMG_FMT_1010102]=conv555to1010102;//*/

  convert[IMG_FMT_888][IMG_FMT_4]=      conv888to4;
  convert[IMG_FMT_888][IMG_FMT_8]=      conv888to8; 
  convert[IMG_FMT_888][IMG_FMT_44]=     conv888to44;
  convert[IMG_FMT_888][IMG_FMT_88]=     conv888to88;
  convert[IMG_FMT_888][IMG_FMT_62]=     conv888to62;
  convert[IMG_FMT_888][IMG_FMT_124]=    conv888to124;
  convert[IMG_FMT_888][IMG_FMT_555]=    conv888to555;
  convert[IMG_FMT_888][IMG_FMT_4444]=   conv888to4444;
  convert[IMG_FMT_888][IMG_FMT_5551]=   conv888to5551;
  convert[IMG_FMT_888][IMG_FMT_8888]=   conv888to8888;
  convert[IMG_FMT_888][IMG_FMT_1010102]=conv888to1010102;//*/

  /*convert[IMG_FMT_4444][IMG_FMT_4]=      conv4444to4;
  convert[IMG_FMT_4444][IMG_FMT_8]=      conv4444to8; 
  convert[IMG_FMT_4444][IMG_FMT_44]=     conv4444to44;
  convert[IMG_FMT_4444][IMG_FMT_88]=     conv4444to88;
  convert[IMG_FMT_4444][IMG_FMT_62]=     conv4444to62;
  convert[IMG_FMT_4444][IMG_FMT_124]=    conv4444to124;
  convert[IMG_FMT_4444][IMG_FMT_555]=    conv4444to555;
  convert[IMG_FMT_4444][IMG_FMT_888]=    conv4444to888;
  convert[IMG_FMT_4444][IMG_FMT_5551]=   conv4444to5551;
  convert[IMG_FMT_4444][IMG_FMT_8888]=   conv4444to8888;
  convert[IMG_FMT_4444][IMG_FMT_1010102]=conv4444to1010102;//*/

  /*convert[IMG_FMT_5551][IMG_FMT_4]=      conv5551to4;
  convert[IMG_FMT_5551][IMG_FMT_8]=      conv5551to8; 
  convert[IMG_FMT_5551][IMG_FMT_44]=     conv5551to44;
  convert[IMG_FMT_5551][IMG_FMT_88]=     conv5551to88;
  convert[IMG_FMT_5551][IMG_FMT_62]=     conv5551to62;
  convert[IMG_FMT_5551][IMG_FMT_124]=    conv5551to124;
  convert[IMG_FMT_5551][IMG_FMT_555]=    conv5551to555;
  convert[IMG_FMT_5551][IMG_FMT_888]=    conv5551to888;
  convert[IMG_FMT_5551][IMG_FMT_4444]=   conv5551to4444;
  convert[IMG_FMT_5551][IMG_FMT_8888]=   conv5551to8888;
  convert[IMG_FMT_5551][IMG_FMT_1010102]=conv5551to1010102;//*/

  convert[IMG_FMT_8888][IMG_FMT_4]=      conv8888to4;
  convert[IMG_FMT_8888][IMG_FMT_8]=      conv8888to8;
  convert[IMG_FMT_8888][IMG_FMT_44]=     conv8888to44;
  convert[IMG_FMT_8888][IMG_FMT_88]=     conv8888to88;
  convert[IMG_FMT_8888][IMG_FMT_62]=     conv8888to62;
  convert[IMG_FMT_8888][IMG_FMT_124]=    conv8888to124;
  convert[IMG_FMT_8888][IMG_FMT_555]=    conv8888to555;
  convert[IMG_FMT_8888][IMG_FMT_888]=    conv8888to888;
  convert[IMG_FMT_8888][IMG_FMT_4444]=   conv8888to4444;
  convert[IMG_FMT_8888][IMG_FMT_5551]=   conv8888to5551;
  convert[IMG_FMT_8888][IMG_FMT_1010102]=conv8888to1010102;//*/

  /*convert[IMG_FMT_1010102][IMG_FMT_4]=      conv1010102to4;
  convert[IMG_FMT_1010102][IMG_FMT_8]=      conv1010102to8; 
  convert[IMG_FMT_1010102][IMG_FMT_44]=     conv1010102to44;
  convert[IMG_FMT_1010102][IMG_FMT_88]=     conv1010102to88;
  convert[IMG_FMT_1010102][IMG_FMT_62]=     conv1010102to62;
  convert[IMG_FMT_1010102][IMG_FMT_124]=    conv1010102to124;
  convert[IMG_FMT_1010102][IMG_FMT_555]=    conv1010102to555;
  convert[IMG_FMT_1010102][IMG_FMT_888]=    conv1010102to888;
  convert[IMG_FMT_1010102][IMG_FMT_4444]=   conv1010102to4444;
  convert[IMG_FMT_1010102][IMG_FMT_5551]=   conv1010102to5551;
  convert[IMG_FMT_1010102][IMG_FMT_8888]=   conv1010102to8888;//*/

  conversionTableInitialized=true;
}

