
#ifndef _TIM2_H_INCLUDED
#define _TIM2_H_INCLUDED


// Type definition for size unification

typedef unsigned char  TIM2_UCHAR8;
typedef unsigned int   TIM2_UINT32;
typedef unsigned short TIM2_UINT16;

#ifdef R5900
typedef unsigned long  TIM2_UINT64;		// PS2
#else	// R5900
#ifdef WIN32
typedef unsigned __int64 TIM2_UINT64;	// Win32
#else	// WIN32
typedef unsigned long long TIM2_UINT64;	// GNU-C
#endif	// WIN32
#endif	// R5900


// Definition of constants that are used in ClutType and ImageType of the picture header

enum TIM2_gattr_type {
	TIM2_NONE = 0,			// Used by ClutType when there is no CLUT data

	TIM2_RGB16,				// 16-bit color (used with both ClutType and ImageType)

	TIM2_RGB24,				// 24-bit color (used only with ImageType)

	TIM2_RGB32,				// 32-bit color (used with both ClutType and ImageType)

	TIM2_IDTEX4,			// 16-color texture (used only with ImageType)

	TIM2_IDTEX8				// 16-color texture (used only with ImageType)

};

// TIM2 file header

typedef struct {
	TIM2_UCHAR8 FileId[4];				// File ID ("T", "I", "M", "2"  or "C", "L", "T", "2")

	TIM2_UCHAR8 FormatVersion;			// File format version

	TIM2_UCHAR8 FormatId;				// Format ID

	TIM2_UINT16 Pictures;				// Number of picture data parts

	TIM2_UCHAR8 pad[8];					// 16-byte alignment

} TIM2_FILEHEADER;


// TIM2 picture header

typedef struct {
	TIM2_UINT32 TotalSize;				// Total size in bytes of all picture data

	TIM2_UINT32 ClutSize;				// Size in bytes of CLUT data part

	TIM2_UINT32 ImageSize;				// Size in bytes of image data part

	TIM2_UINT16 HeaderSize;				// Image header size

	TIM2_UINT16 ClutColors;				// CLUT size (number of colors in CLUT part)

	TIM2_UCHAR8 PictFormat;				// Image ID

	TIM2_UCHAR8 MipMapTextures;			// Number of MIPMAPs

	TIM2_UCHAR8 ClutType;				// CLUT part type

	TIM2_UCHAR8 ImageType;				// Image part type

	TIM2_UINT16 ImageWidth;				// Image width (not bit count)

	TIM2_UINT16 ImageHeight;			// Image height (not bit count)


	TIM2_UINT64 GsTex0;					// TEX0
	TIM2_UINT64 GsTex1;					// TEX1
	TIM2_UINT32 GsTexaFbaPabe;			// Combination of TEXA, FBA, and PABE bits

	TIM2_UINT32 GsTexClut;				// TEXCLUT (low-order 32 bits directly)

} TIM2_PICTUREHEADER;


// TIM2 MIPMAP header

typedef struct {
	TIM2_UINT64 GsMiptbp1;				// MIPTBP1 (64 bits directly)

	TIM2_UINT64 GsMiptbp2;				// MIPTBP2  (64 bits directly)

	TIM2_UINT32 MMImageSize[0];			// Size in bytes of image for MIPMAP number [?]

} TIM2_MIPMAPHEADER;


// TIM2 extended header

typedef struct {
	TIM2_UCHAR8 ExHeaderId[4];			// Extended comment header  ID("e", "X", "t", "\x00")

	TIM2_UINT32 UserSpaceSize;			// User space size

	TIM2_UINT32 UserDataSize;			// User data size

	TIM2_UINT32 Reserved;				// Reserved

} TIM2_EXHEADER;


// TIM2 file loader prototype declaration

int   Tim2CheckFileHeaer(void *p);								// Check to determine whether or not file is TIM2 file

TIM2_PICTUREHEADER *Tim2GetPictureHeader(void *p, int imgno);	// Obtain specified number picture header 

int   Tim2IsClut2(TIM2_PICTUREHEADER *ph);						// Distinguish between TIM2 and CLUT2


int   Tim2GetMipMapPictureSize(TIM2_PICTUREHEADER *ph,
					int mipmap, int *pWidth, int *pHeight);		// Obtain picture size for each MIPMAP level


TIM2_MIPMAPHEADER *Tim2GetMipMapHeader(TIM2_PICTUREHEADER *ph, int *pSize);
																// Obtain starting address and size of MIPMAP header


void *Tim2GetUserSpace(TIM2_PICTUREHEADER *ph, int *pSize);		// Obtain starting address and size of user space

void *Tim2GetUserData(TIM2_PICTUREHEADER *ph, int *pSize);		// Obtain starting address and size of user data

char *Tim2GetComment(TIM2_PICTUREHEADER *ph);					// Obtain starting address of comment character string

void *Tim2GetImage(TIM2_PICTUREHEADER *ph, int mipmap);			// Obtain starting address of image data of specified level

void *Tim2GetClut(TIM2_PICTUREHEADER *ph);						// Obtain starting address of CLUT data


unsigned int Tim2GetClutColor(TIM2_PICTUREHEADER *ph,
					int clut, int no);							// Obtain CLUT color data of specified CLUT set and index

unsigned int Tim2SetClutColor(TIM2_PICTUREHEADER *ph,
					int clut, int no, unsigned int newcolor);	// Set CLUT color data of specified CLUT set and index


unsigned int Tim2GetTexel(TIM2_PICTUREHEADER *ph,
					int mipmap, int x, int y);					// Obtain pixel data from specified texel coordinates of specified MIPMAP level

unsigned int Tim2SetTexel(TIM2_PICTUREHEADER *ph,
					int mipmap, int x, int y, unsigned int newtexel);	// Set pixel data at specified texel coordinates of specified level


unsigned int Tim2GetTextureColor(TIM2_PICTUREHEADER *ph,
					int mipmap, int clut, int x, int y);		// Use specified CLUT to obtain texel color from specified MIPMAP level



#ifdef R5900

unsigned int Tim2LoadPicture(TIM2_PICTUREHEADER *ph,
					unsigned int tbp, unsigned int cbp);		// Transfer image data or CLUT data to GS local memory
unsigned int Tim2LoadImage(TIM2_PICTUREHEADER *ph, unsigned int tbp);	// Transfer image data to GS local memory

unsigned int Tim2LoadClut(TIM2_PICTUREHEADER *ph, unsigned int cbp);	// Transfer CLUT data to GS local memory


int Tim2TakeSnapshot(sceGsDispEnv *d0, sceGsDispEnv *d1,
					char *pszFname);							// Write snapshot image in TIM2 file on hard disk drive of host 

#endif	// R5900

#endif /* _TIM2_H_INCLUDED */
