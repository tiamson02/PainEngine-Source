
// TIM2 file parser

// Version management information

// $Revision: 22 $
// $Date: 99/11/18 21:58 $

#include <stdio.h>

// Set structure of TIM2 file header


#ifdef R5900

// For PS2 environment

#include <eekernel.h>
#include <sifdev.h>
#include <libgraph.h>
#include "tim2.h"

// Prototype declarations

static void Tim2LoadTexture(int psm, u_int tbp, int tbw, int sx, int sy, u_long128 *pImage);
static int  Tim2CalcBufWidth(int psm, int w);
static int  Tim2CalcBufSize(int psm, int w, int h);
static int  Tim2GetLog2(int n);

#else	// R5900

// For non-PS2 environment


#ifdef WIN32
#pragma warning(push)
#pragma warning(disable: 4200)
#endif	// WIN32
#include "tim2.h"
#ifdef WIN32
#pragma warning(pop)
#endif	// WIN32

#endif	// R5900



// Check file header of TIM2 file

// Arguments

// pTim2	Starting address of TIM2-format data

// Return value

//			When 0, error 

//			When 1, normal termination (TIM2)

//			When 2, normal termination (CLUT2)

int Tim2CheckFileHeaer(void *pTim2)
{
	TIM2_FILEHEADER *pFileHdr = (TIM2_FILEHEADER *)pTim2;
	int i;

	// Check TIM2 signature

	if(pFileHdr->FileId[0]=='T' || pFileHdr->FileId[1]=='I' || pFileHdr->FileId[2]=='M' || pFileHdr->FileId[3]=='2') {
		// If TIM2

		i = 1;
	} else if(pFileHdr->FileId[0]=='C' || pFileHdr->FileId[1]=='L' || pFileHdr->FileId[2]=='T' || pFileHdr->FileId[3]=='2') {
		// If CLUT2

		i = 2;
	} else {
		// If illegal identification character

		printf("Tim2CheckFileHeaer: TIM2 is broken %02X,%02X,%02X,%02X\n",
					pFileHdr->FileId[0], pFileHdr->FileId[1], pFileHdr->FileId[2], pFileHdr->FileId[3]);
		return(0);
	}

	// Check TIM2 file format version and format ID

	if(!(pFileHdr->FormatVersion==0x03 ||
	    (pFileHdr->FormatVersion==0x04 && (pFileHdr->FormatId==0x00 || pFileHdr->FormatId==0x01)))) {
		printf("Tim2CheckFileHeaer: TIM2 is broken (2)\n");
		return(0);
	}
	return(i);
}



// Get picture header corresponding to specified number

// Arguments

// pTim2	Starting address of TIM2-format data

// imgno	Specification of the number of the picture header to reference

// Return value

//			Pointer to picture header

TIM2_PICTUREHEADER *Tim2GetPictureHeader(void *pTim2, int imgno)
{
	TIM2_FILEHEADER *pFileHdr = (TIM2_FILEHEADER *)pTim2;
	TIM2_PICTUREHEADER *pPictHdr;
	int i;

	// Check picture number

	if(imgno>=pFileHdr->Pictures) {
		printf("Tim2GetPictureHeader: Illegal image no.(%d)\n", imgno);
		return(NULL);
	}

	if(pFileHdr->FormatId==0x00) {
		// When format ID is 0x00, 16-byte alignment

		pPictHdr = (TIM2_PICTUREHEADER *)((char *)pTim2 + sizeof(TIM2_FILEHEADER));
	} else {
		// When format ID is 0x01, 128-byte alignment

		pPictHdr = (TIM2_PICTUREHEADER *)((char *)pTim2 + 0x80);
	}

	// Skip up to specified picture number

	for(i=0; i<imgno; i++) {
		pPictHdr = (TIM2_PICTUREHEADER *)((char *)pPictHdr + pPictHdr->TotalSize);
	}
	return(pPictHdr);
}


// Decide whether or not picture data is CLUT2 data

// Arguments

// ph:		Starting address of TIM2 picture header

// Return value

//			When 0, TIM2

//			When 1, CLUT2

int Tim2IsClut2(TIM2_PICTUREHEADER *ph)
{
	// Distinguish MipMapTextures member of picture header

	if(ph->MipMapTextures==0) {
		// When number of textures is 0, CLUT2 data

		return(1);
	} else {
		// When number of textures is 1, TIM2 data

		return(0);
	}
}


// Get texture size of each MIPMAP level

// Arguments

// ph:		Starting address of TIM2 picture header

// mipmap:	MIPMAP texture level

// pWidth:	Pointer to int-type variable for receiving X size

// pHeight:	Pointer to int-type variable for receiving Y size

// Return value

//			MIPMAP texture size

int Tim2GetMipMapPictureSize(TIM2_PICTUREHEADER *ph, int mipmap, int *pWidth, int *pHeight)
{
	int w, h, n;
	w = ph->ImageWidth>>mipmap;
	h = ph->ImageHeight>>mipmap;
	if(pWidth) {
		*pWidth  = w;
	}
	if(pHeight) {
		*pHeight = h;
	}

	n = w * h;
	switch(ph->ImageType) {
		case TIM2_RGB16:	n *= 2;		break;
		case TIM2_RGB24:	n *= 3;		break;
		case TIM2_RGB32:	n *= 4;		break;
		case TIM2_IDTEX4:	n /= 2;		break;
		case TIM2_IDTEX8:				break;
	}

	// Regardless of the FormatID specification of the file header, the MIPMAP texture size is

	// always aligned on a 16-byte boundary

	n = (n + 15) & ~15;
	return(n);
}


// Get MIPMAP header address and size

// Arguments

// ph:		Starting address of TIM2 picture header

// pSize:	Pointer to int-type variable for receiving MIPMAP header size

//			When size is not required, set to NULL

// Return value

//			When NULL, no MIPMAP header

//			When not NULL, starting address of MIPMAP header

TIM2_MIPMAPHEADER *Tim2GetMipMapHeader(TIM2_PICTUREHEADER *ph, int *pSize)
{
	TIM2_MIPMAPHEADER *pMmHdr;

	static const char mmsize[] = {
		0,		// Texture number 0 (for CLUT2 data)

		0,		// LV0 texture only (no MIPMAP header)

		32,		// Up to LV1 MipMap

		32,		// Up to LV2 MipMap

		32,		// Up to LV3 MipMap

		48,		// Up to LV4 MipMap

		48,		// Up to LV5 MipMap

		48		// Up to LV6 MipMap

	};

	if(ph->MipMapTextures>1) {
		pMmHdr = (TIM2_MIPMAPHEADER *)((char *)ph + sizeof(TIM2_PICTUREHEADER));
	} else {
		pMmHdr = NULL;
	}

	if(pSize) {
		// When there was no extended header

		*pSize = mmsize[ph->MipMapTextures];
	}
	return(pMmHdr);
}


// Get user space address and size

// Arguments

// ph:		Starting address of TIM2 picture header

// pSize:	Pointer to int-type variable for receiving user space size

//			When size is not required, set to NULL

// Return value

//			When NULL, no user space

//			When not NULL, starting address of user space

void *Tim2GetUserSpace(TIM2_PICTUREHEADER *ph, int *pSize)
{
	void *pUserSpace;

	static const char mmsize[] = {
		sizeof(TIM2_PICTUREHEADER),			// Texture number 0 (for CLUT2 data)

		sizeof(TIM2_PICTUREHEADER),			// LV0 texture only (no MIPMAP header)

		sizeof(TIM2_PICTUREHEADER) + 32,	// Up to LV1 MipMap

		sizeof(TIM2_PICTUREHEADER) + 32,	// Up to LV2 MipMap

		sizeof(TIM2_PICTUREHEADER) + 32,	// Up to LV3 MipMap

		sizeof(TIM2_PICTUREHEADER) + 48,	// Up to LV4 MipMap

		sizeof(TIM2_PICTUREHEADER) + 48,	// Up to LV5 MipMap

		sizeof(TIM2_PICTUREHEADER) + 48		// Up to LV6 MipMap

	};

	// Check header size

	if(ph->HeaderSize==mmsize[ph->MipMapTextures]) {
		// When no user space exists

		if(pSize) *pSize = 0;
		return(NULL);
	}

	// When user space exists

	pUserSpace = (void *)((char *)ph + mmsize[ph->MipMapTextures]);
	if(pSize) {
		// Calculate user space size

		// When there is an extended header, get title size from it

		TIM2_EXHEADER *pExHdr;

		pExHdr = (TIM2_EXHEADER *)pUserSpace;
		if(pExHdr->ExHeaderId[0]!='e' ||
			pExHdr->ExHeaderId[1]!='X' ||
			pExHdr->ExHeaderId[2]!='t' ||
			pExHdr->ExHeaderId[3]!=0x00) {

			// When there was no extended header

			*pSize = (ph->HeaderSize - mmsize[ph->MipMapTextures]);
		} else {
			// When there was an extended header

			*pSize = (int)pExHdr->UserSpaceSize;
		}
	}
	return(pUserSpace);
}


// Get user data address and size

// Arguments

// ph:		Starting address of TIM2 picture header

// pSize:	Pointer to int-type variable for receiving user data size

//			When size is not required, set to NULL

// Return value

//			When NULL, no user data

//			When not NULL, starting address of user data

void *Tim2GetUserData(TIM2_PICTUREHEADER *ph, int *pSize)
{
	void *pUserSpace;
	TIM2_EXHEADER *pExHdr;

	pUserSpace = Tim2GetUserSpace(ph, pSize);
	if(pUserSpace==NULL) {
		// When no user space existed

		return(NULL);
	}

	// Check whether or not there is an extended header in the user space

	pExHdr = (TIM2_EXHEADER *)pUserSpace;
	if(pExHdr->ExHeaderId[0]!='e' ||
		pExHdr->ExHeaderId[1]!='X' ||
		pExHdr->ExHeaderId[2]!='t' ||
		pExHdr->ExHeaderId[3]!=0x00) {

		// When no extended header was found

		return(pUserSpace);
	}

	// When an extended header was found

	if(pSize) {
		// Return size of user data part

		*pSize = (int)pExHdr->UserDataSize;
	}
	return((char *)pUserSpace + sizeof(TIM2_EXHEADER));
}


// Get starting address of comment character string that was stored in user space

// Arguments

// ph:		Starting address of TIM2 picture header

// Return value

//			When NULL, no comment character string

//			When not NULL, starting address of comment character string (ASCIZ)

char *Tim2GetComment(TIM2_PICTUREHEADER *ph)
{
	void *pUserSpace;
	TIM2_EXHEADER *pExHdr;

	pUserSpace = Tim2GetUserSpace(ph, NULL);
	if(pUserSpace==NULL) {
		// When no user space existed

		return(NULL);
	}

	// Check whether or not there is an extended header in the user space

	pExHdr = (TIM2_EXHEADER *)pUserSpace;
	if(pExHdr->ExHeaderId[0]!='e' ||
		pExHdr->ExHeaderId[1]!='X' ||
		pExHdr->ExHeaderId[2]!='t' ||
		pExHdr->ExHeaderId[3]!=0x00) {

		// When no extended header was found

		return(NULL);
	}

	// When an extended header had existed

	if(pExHdr->UserSpaceSize==((sizeof(TIM2_EXHEADER) + pExHdr->UserDataSize))) {
		// When intended size of user space was equal to total size of extended header and user data

		// You can decide that no comment character string exists

		return(NULL);
	}

	// When no comment was found

	return((char *)pUserSpace + sizeof(TIM2_EXHEADER) + pExHdr->UserDataSize);
}



// Get starting address of image data for specified MIPMAAP level

// Arguments

// ph:		Starting address of TIM2 picture header

// mipmap:	MIPMAP texture level

// Return value

//			When NULL, no relevant image data

//			When not NULL, starting address of image data

void *Tim2GetImage(TIM2_PICTUREHEADER *ph, int mipmap)
{
	void *pImage;
	TIM2_MIPMAPHEADER *pm;
	int i;

	if(mipmap>=ph->MipMapTextures) {
		// No MIPMAP texture corresponding to specified level exists

		return(NULL);
	}

	// Calculate starting address of image data

	pImage = (void *)((char *)ph + ph->HeaderSize);
	if(ph->MipMapTextures==1) {
		// For only LV0 texture

		return(pImage);
	}

	// When MIPMAP texture exists

	pm = (TIM2_MIPMAPHEADER *)((char *)ph + sizeof(TIM2_PICTUREHEADER));
	for(i=0; i<mipmap; i++) {
		pImage = (void *)((char *)pImage + pm->MMImageSize[i]);
	}
	return(pImage);
}


// Get starting address of CLUT data

// Arguments

// ph:		Starting address of TIM2 picture header

// Return value

//			When NULL, no relevant CLUT data exists

//			When not NULL, starting address of CLUT data

void *Tim2GetClut(TIM2_PICTUREHEADER *ph)
{
	void *pClut;
	if(ph->ClutColors==0) {
		// When number of colors constituting CLUT data part is 0

		pClut = NULL;
	} else {
		// When CLUT data exists

		pClut = (void *)((char *)ph + ph->HeaderSize + ph->ImageSize);
	}
	return(pClut);
}


// Get CLUT color

// Arguments

// ph:		Starting address of TIM2 picture header

// clut:	CLUT set specification

// no:		Specifies the number of the index to be obtained

// Return value

//			A color in RGBA32 format is returned

//			If there is an error in the specifications of clut, no, etc., 0x00000000 (black) is returned

unsigned int Tim2GetClutColor(TIM2_PICTUREHEADER *ph, int clut, int no)
{
	unsigned char *pClut;
	int n;
	unsigned char r, g, b, a;

	pClut = Tim2GetClut(ph);
	if(pClut==NULL) {
		// When there was no CLUT data

		return(0);
	}

	// First, calculate which number color data it is

	switch(ph->ImageType) {
		case TIM2_IDTEX4:	n = clut*16 + no;	break;
		case TIM2_IDTEX8:	n = clut*256 + no;	break;
		default:			return(0);					// For invalid pixel color

	}
	if(n>ph->ClutColors) {
		// When color data of specified CLUT number or index did not exist

		return(0);
	}

	// Placement may be rearranged according to format of CLUT part

	switch((ph->ClutType<<8) | ph->ImageType) {
		case (((TIM2_RGB16 | 0x40)<<8) | TIM2_IDTEX4):	// 16 colors, CSM1, 16 bits, rearranged

		case (((TIM2_RGB24 | 0x40)<<8) | TIM2_IDTEX4):	// 16 colors, CSM1, 24 bits, rearranged

		case (((TIM2_RGB32 | 0x40)<<8) | TIM2_IDTEX4):	// 16 colors, CSM1, 32 bits, rearranged

		case (( TIM2_RGB16        <<8) | TIM2_IDTEX8):	// 256 colors, CSM1, 16 bits, rearranged

		case (( TIM2_RGB24        <<8) | TIM2_IDTEX8):	// 256 colors, CSM1, 24 bits, rearranged

		case (( TIM2_RGB32        <<8) | TIM2_IDTEX8):	// 256 colors, CSM1, 32 bits, rearranged

			if((n & 31)>=8) {
				if((n & 31)<16) {
					n += 8;				// +8 - 15 to +16 - 23

				} else if((n & 31)<24) {
					n -= 8;				// +16 - 23 to +8 - 15

				}
			}
			break;
	}

	// Get color data based on CLUT part pixel format

	switch(ph->ClutType & 0x3F) {
		case TIM2_RGB16:
			// For 16-bit color

			r = (unsigned char)((((pClut[n*2 + 1]<<8) | pClut[n*2])<<3) & 0xF8);
			g = (unsigned char)((((pClut[n*2 + 1]<<8) | pClut[n*2])>>2) & 0xF8);
			b = (unsigned char)((((pClut[n*2 + 1]<<8) | pClut[n*2])>>7) & 0xF8);
			a = (unsigned char)((((pClut[n*2 + 1]<<8) | pClut[n*2])>>8) & 0x80);
			break;

		case TIM2_RGB24:
			// For 24-bit color

			r = pClut[n*3];
			g = pClut[n*3 + 1];
			b = pClut[n*3 + 2];
			a = 0x80;
			break;

		case TIM2_RGB32:
			// For 32-bit color

			r = pClut[n*4];
			g = pClut[n*4 + 1];
			b = pClut[n*4 + 2];
			a = pClut[n*4 + 3];
			break;

		default:
			// For invalid pixel format

			r = 0;
			g = 0;
			b = 0;
			a = 0;
			break;
	}
	return (u_int)((a<<24) | (g<<16) | (b<<8) | r);
}


// Set CLUT color

// Arguments

// ph:		Starting address of TIM2 picture header

// clut:	CLUT set specification

// no:		Specification of the number of the index to set

// color:	Color to be set  (RGB32 format)

// Return value

//			The oldest color in RGBA32 format is returned

//			If there is an error in the specifications of clut, no, etc., 0x00000000 (black) is returned

unsigned int Tim2SetClutColor(TIM2_PICTUREHEADER *ph, int clut, int no, unsigned int newcolor)
{
	unsigned char *pClut;
	unsigned char r, g, b, a;
	int n;

	pClut = Tim2GetClut(ph);
	if(pClut==NULL) {
		// When there was no CLUT data

		return(0);
	}

	// First, calculate which number color data it is

	switch(ph->ImageType) {
		case TIM2_IDTEX4:	n = clut*16 + no;	break;
		case TIM2_IDTEX8:	n = clut*256 + no;	break;
		default:			return(0);					// For invalid pixel color

	}
	if(n>ph->ClutColors) {
		// When color data of specified CLUT number or index did not exist

		return(0);
	}

	// Placement may be rearranged according to format of CLUT part

	switch((ph->ClutType<<8) | ph->ImageType) {
		case (((TIM2_RGB16 | 0x40)<<8) | TIM2_IDTEX4):	// 16 colors, CSM1, 16 bits, rearranged

		case (((TIM2_RGB24 | 0x40)<<8) | TIM2_IDTEX4):	// 16 colors, CSM1, 24 bits, rearranged

		case (((TIM2_RGB32 | 0x40)<<8) | TIM2_IDTEX4):	// 16 colors, CSM1, 32 bits, rearranged

		case (( TIM2_RGB16        <<8) | TIM2_IDTEX8):	// 256 colors, CSM1, 16 bits, rearranged

		case (( TIM2_RGB24        <<8) | TIM2_IDTEX8):	// 256 colors, CSM1, 24 bits, rearranged

		case (( TIM2_RGB32        <<8) | TIM2_IDTEX8):	// 256 colors, CSM1, 32 bits, rearranged

			if((n & 31)>=8) {
				if((n & 31)<16) {
					n += 8;				// +8 - 15 to +16 - 23

				} else if((n & 31)<24) {
					n -= 8;				// +16 - 23 to +8 - 15

				}
			}
			break;
	}

	// Get color data based on CLUT part pixel format

	switch(ph->ClutType & 0x3F) {
		case TIM2_RGB16:
			// For 16-bit color

			{
				unsigned char rr, gg, bb, aa;
				r = (unsigned char)((((pClut[n*2 + 1]<<8) | pClut[n*2])<<3) & 0xF8);
				g = (unsigned char)((((pClut[n*2 + 1]<<8) | pClut[n*2])>>2) & 0xF8);
				b = (unsigned char)((((pClut[n*2 + 1]<<8) | pClut[n*2])>>7) & 0xF8);
				a = (unsigned char)((((pClut[n*2 + 1]<<8) | pClut[n*2])>>8) & 0x80);

				rr = (unsigned char)((newcolor>>3)  & 0x1F);
				gg = (unsigned char)((newcolor>>11) & 0x1F);
				bb = (unsigned char)((newcolor>>19) & 0x1F);
				aa = (unsigned char)((newcolor>>31) & 1);

				pClut[n*2]     = (unsigned char)((((aa<<15) | (bb<<10) | (gg<<5) | rr))    & 0xFF);
				pClut[n*2 + 1] = (unsigned char)((((aa<<15) | (bb<<10) | (gg<<5) | rr)>>8) & 0xFF);
			}
			break;

		case TIM2_RGB24:
			// For 24-bit color

			r = pClut[n*3];
			g = pClut[n*3 + 1];
			b = pClut[n*3 + 2];
			a = 0x80;
			pClut[n*3]     = (unsigned char)((newcolor)     & 0xFF);
			pClut[n*3 + 1] = (unsigned char)((newcolor>>8)  & 0xFF);
			pClut[n*3 + 2] = (unsigned char)((newcolor>>16) & 0xFF);
			break;

		case TIM2_RGB32:
			// For 32-bit color

			r = pClut[n*4];
			g = pClut[n*4 + 1];
			b = pClut[n*4 + 2];
			a = pClut[n*4 + 3];
			pClut[n*4]     = (unsigned char)((newcolor)     & 0xFF);
			pClut[n*4 + 1] = (unsigned char)((newcolor>>8)  & 0xFF);
			pClut[n*4 + 2] = (unsigned char)((newcolor>>16) & 0xFF);
			pClut[n*4 + 3] = (unsigned char)((newcolor>>24) & 0xFF);
			break;

		default:
			// For invalid pixel format

			r = 0;
			g = 0;
			b = 0;
			a = 0;
			break;
	}
	return (u_int)((a<<24) | (g<<16) | (b<<8) | r);
}


// Get texel (not limited to color information) data

// Arguments

// ph:		Starting address of TIM2 picture header

// mipmap:	MIPMAP texture level

// x:		Texel X-coordinate for obtaining texel data

// y:		Texel Y-coordinate for obtaining texel data

// Return value

//			Color information (4-bit or 8-bit index number or 16-bit, 24-bit, or 32-bit direct color)

unsigned int Tim2GetTexel(TIM2_PICTUREHEADER *ph, int mipmap, int x, int y)
{
	unsigned char *pImage;
	int t;
	int w, h;

	pImage = Tim2GetImage(ph, mipmap);
	if(pImage==NULL) {
		// When there was no texture data corresponding to the specified level

		return(0);
	}
	Tim2GetMipMapPictureSize(ph, mipmap, &w, &h);
	if(x>w || y>h) {
		// When texel coordinates are invalid

		return(0);
	}

	t = y*w + x;
	switch(ph->ImageType) {
		case TIM2_RGB16:
			return(u_int)((pImage[t*2 + 1]<<8) | pImage[t*2]);

		case TIM2_RGB24:
			return(u_int)((pImage[t*3 + 2]<<16) | (pImage[t*3 + 1]<<8) | pImage[t*3]);

		case TIM2_RGB32:
			return(u_int)((pImage[t*4 + 3]<<24) | (pImage[t*4 + 2]<<16) | (pImage[t*4 + 1]<<8) | pImage[t*4]);

		case TIM2_IDTEX4:
			if(x & 1) {
				return(u_int)(pImage[t/2]>>4);
			} else {
				return(u_int)(pImage[t/2] & 0x0F);
			}
		case TIM2_IDTEX8:
			return(pImage[t]);
	}

	// When pixel format was invalid

	return(0);
}



// Set texel (not limited to color information) data

// Arguments

// ph:		Starting address of TIM2 picture header

// mipmap:	MIPMAP texture level

// x:		Texel X-coordinate for obtaining texel data

// y:		Texel Y-coordinate for obtaining texel data

// Return value

//			Color information (4-bit or 8-bit index number or 16-bit, 24-bit, or 32-bit direct color)

unsigned int Tim2SetTexel(TIM2_PICTUREHEADER *ph, int mipmap, int x, int y, unsigned int newtexel)
{
	unsigned char *pImage;
	int t;
	int w, h;
	unsigned int oldtexel;

	pImage = Tim2GetImage(ph, mipmap);
	if(pImage==NULL) {
		// When there was no texture data corresponding to the specified level

		return(0);
	}
	Tim2GetMipMapPictureSize(ph, mipmap, &w, &h);
	if(x>w || y>h) {
		// When texel coordinates are invalid

		return(0);
	}

	t = y*w + x;
	switch(ph->ImageType) {
		case TIM2_RGB16:
			oldtexel = (u_int)(pImage[t*2 + 1]<<8) | pImage[t*2];
			pImage[t*2]     = (unsigned char)((newtexel)    & 0xFF);
			pImage[t*2 + 1] = (unsigned char)((newtexel>>8) & 0xFF);
			return(oldtexel);

		case TIM2_RGB24:
			oldtexel = (u_int)(pImage[t*3 + 2]<<16) | (pImage[t*3 + 1]<<8) | pImage[t*3];
			pImage[t*3]     = (unsigned char)((newtexel)     & 0xFF);
			pImage[t*3 + 1] = (unsigned char)((newtexel>>8)  & 0xFF);
			pImage[t*3 + 2] = (unsigned char)((newtexel>>16) & 0xFF);
			return(oldtexel);

		case TIM2_RGB32:
			oldtexel = (u_int)(pImage[t*4 + 3]<<24) | (pImage[t*4 + 2]<<16) | (pImage[t*4 + 1]<<8) | pImage[t*4];
			pImage[t*4]     = (unsigned char)((newtexel)     & 0xFF);
			pImage[t*4 + 1] = (unsigned char)((newtexel>>8)  & 0xFF);
			pImage[t*4 + 2] = (unsigned char)((newtexel>>16) & 0xFF);
			pImage[t*4 + 3] = (unsigned char)((newtexel>>24) & 0xFF);
			return(oldtexel);

		case TIM2_IDTEX4:
			if(x & 1) {
				oldtexel =(u_int) pImage[t/2]>>4;
				pImage[t/2] = (unsigned char)((newtexel<<4) | oldtexel);
			} else {
				oldtexel = (u_int) pImage[t/2] & 0x0F;
				pImage[t/2] = (unsigned char)((oldtexel<<4) | newtexel);
			}
			return(oldtexel);
		case TIM2_IDTEX8:
			oldtexel = pImage[t];
			pImage[t] = (unsigned char)newtexel;
			return(oldtexel);
	}

	// When pixel format was invalid

	return(0);
}


// Get texture color

// Arguments

// ph:		Starting address of TIM2 picture header

// mipmap:	MIPMAP texture level

// clut:	CLUT number used for index color conversion

// x:		Texel X-coordinate for obtaining texel data

// y:		Texel Y-coordinate for obtaining texel data

// Return value

//			A color in RGBA32 format is returned

//			If there is an error in the clut specification, 0x00000000 (black) is returned

//			If there was an error in the x and y specifications, the operation is not guaranteed

unsigned int Tim2GetTextureColor(TIM2_PICTUREHEADER *ph, int mipmap, int clut, int x, int y)
{
	unsigned int t;
	if(Tim2GetImage(ph, mipmap)==NULL) {
		// When there was no texture data corresponding to the specified level

		return(0);
	}
	t = Tim2GetTexel(ph, mipmap, (x>>mipmap), (y>>mipmap));
	switch(ph->ImageType) {
		case TIM2_RGB16:
			{
				unsigned char r, g, b, a;
				r = (unsigned char)((t<<3) & 0xF8);
				g = (unsigned char)((t>>2) & 0xF8);
				b = (unsigned char)((t>>7) & 0xF8);
				a = (unsigned char)((t>>8) & 0x80);
				return (u_int)((a<<24) | (g<<16) | (b<<8) | r);
			}

		case TIM2_RGB24:
			return((0x80<<24) | (t & 0x00FFFFFF));

		case TIM2_RGB32:
			return(t);

		case TIM2_IDTEX4:
		case TIM2_IDTEX8:
			return (u_int)(Tim2GetClutColor(ph, clut, (int)t));
	}
	return(0);
}






// Subsequent functions can only be used with the PS2 ee-gcc

#ifdef R5900

// Read TIM2 picture data to GS local memory

// Arguments

// ph:		Starting address of TIM2 picture header

// tbp:		Destination texture buffer base address (when -1, value in header is used)

// cbp:		Destination CLUT buffer page base address (when -1, value in header is used)

// Return value

//			When NULL, error

//			When not NULL, next buffer address

// Note

//			Even if CSM2 had been specified as the CLUT storage mode, it is forcibly converted to CSM1 and sent to the GS

unsigned int Tim2LoadPicture(TIM2_PICTUREHEADER *ph, unsigned int tbp, unsigned int cbp)
{
	// Transfer CLUT data

	tbp = Tim2LoadImage(ph, tbp);
	Tim2LoadClut(ph, cbp);
	return(tbp);
}


// Read image data part of TIM2 picture into GS local memory

// Arguments

// ph:		Starting address of TIM2 picture header

// tbp:		Destination texture buffer base address (when -1, value in header is used)

// Return value

//			When NULL, error

//			When not NULL, next texture buffer address

// Note

//			Even if CSM2 had been specified as the CLUT storage mode, it is forcibly converted to CSM1 and sent to the GS

unsigned int Tim2LoadImage(TIM2_PICTUREHEADER *ph, unsigned int tbp)
{
	// Send image data

	if(ph->MipMapTextures>0) {
		static const int psmtbl[] = {
			SCE_GS_PSMCT16,
			SCE_GS_PSMCT24,
			SCE_GS_PSMCT32,
			SCE_GS_PSMT4,
			SCE_GS_PSMT8
		};
		int i;
		int psm;
		u_long128 *pImage;
		int w, h;
		int tbw;

		psm = psmtbl[ph->ImageType - 1];		// Get pixel format

		((sceGsTex0 *)&ph->GsTex0)->PSM  = psm;

		w = ph->ImageWidth;						// Image X size

		h = ph->ImageHeight;					// Image Y size


		// Calculate starting address of image data

		pImage = (u_long128 *)((char *)ph + ph->HeaderSize);
		if(tbp==-1) {
			// When tbp specification is -1, get tbp and tbw from GsTex0 member specified in picture header

			tbp = ((sceGsTex0 *)&ph->GsTex0)->TBP0;
			tbw = (int)((sceGsTex0 *)&ph->GsTex0)->TBW;

			Tim2LoadTexture(psm, tbp, tbw, w, h, pImage);		// Transfer texture data to GS

		} else {
			// When tbp value was specified, overwrite tbp and tbw in GsTex0 member of picture header

			tbw = Tim2CalcBufWidth(psm, w);
			// Update value set for TEX0 register of GS

			((sceGsTex0 *)&ph->GsTex0)->TBP0 = tbp;
			((sceGsTex0 *)&ph->GsTex0)->TBW  = tbw;
			Tim2LoadTexture(psm, tbp, tbw, w, h, pImage);		// Transfer texture data to GS

			tbp += (u_int)Tim2CalcBufSize(psm, w, h);		// Update tbp value

		}

		if(ph->MipMapTextures>1) {
			// When there is a MIPMAP texture

			TIM2_MIPMAPHEADER *pm;

			pm = (TIM2_MIPMAPHEADER *)(ph + 1);		// MIPMAP header immediately following picture header


			// Get LV0 texture size

			// When tbp was specified in argument, miptbp in picture header is ignored and automatically calculated

			if(tbp!=-1) {
				pm->GsMiptbp1 = 0;
				pm->GsMiptbp2 = 0;
			}

			pImage = (u_long128 *)((char *)ph + ph->HeaderSize);
			// Transfer image for each MIPMAP level

			for(i=1; i<ph->MipMapTextures; i++) {
				// If there is a MIPMAP level, texture size is halved

				w = w / 2;
				h = h / 2;

				pImage = (u_long128 *)((char *)pImage + pm->MMImageSize[i - 1]);
				if(tbp==-1) {
					// When texture page specification is -1, tbp and tbw specified in MIPMAP header are used

					int miptbp;
					if(i<4) {
						// For MIPMAP levels 1, 2, 3

						miptbp = (int)(pm->GsMiptbp1>>((i-1)*20)) & 0x3FFF;
						tbw    = (int)(pm->GsMiptbp1>>((i-1)*20 + 14)) & 0x3F;
					} else {
						// For MIPMAP levels 4, 5, 6

						miptbp = (int)(pm->GsMiptbp2>>((i-4)*20)) & 0x3FFF;
						tbw    = (int)(pm->GsMiptbp2>>((i-4)*20 + 14)) & 0x3F;
					}
					Tim2LoadTexture(psm, (u_int)miptbp, tbw, w, h, pImage);
				} else {
					// When texture page is specified, set MIPMAP header again

					tbw = Tim2CalcBufWidth(psm, w);		// Calculate texture width

					if(i<4) {
						// For MIPMAP levels 1, 2, 3

						pm->GsMiptbp1 |= ((u_long)tbp)<<((i-1)*20);
						pm->GsMiptbp1 |= ((u_long)tbw)<<((i-1)*20 + 14);
					} else {
						// For MIPMAP levels 4, 5, 6

						pm->GsMiptbp2 |= ((u_long)tbp)<<((i-4)*20);
						pm->GsMiptbp2 |= ((u_long)tbw)<<((i-4)*20 + 14);
					}
					Tim2LoadTexture(psm, tbp, tbw, w, h, pImage);
					tbp += (u_int)Tim2CalcBufSize(psm, w, h);			// Update tbp value

				}
			}
		}
	}
	return(tbp);
}



// Transfer CLUT data part of TIM2 picture to GS local memory

// Arguments

// ph:		Starting address of TIM2 picture header

// cbp:		Destination CLUT buffer page base address (when -1, value in header is used)

// Return value

//			When 0, error 

//			When not 0, processing succeeded

// Note

//			Even if CSM2 had been specified as the CLUT storage mode, it is forcibly converted to CSM1 and sent to the GS

unsigned int Tim2LoadClut(TIM2_PICTUREHEADER *ph, unsigned int cbp)
{
	short i;
	sceGsLoadImage li;
	u_long128 *pClut;
	int	cpsm;

	// Get CLUT pixel format

	if(ph->ClutType==TIM2_NONE) {
		// When no CLUT data exists

		return(1);
	} else if((ph->ClutType & 0x3F)==TIM2_RGB16) {
		cpsm = SCE_GS_PSMCT16;
	} else if((ph->ClutType & 0x3F)==TIM2_RGB24) {
		cpsm = SCE_GS_PSMCT24;
	} else {
		cpsm = SCE_GS_PSMCT32;
	}
	((sceGsTex0 *)&ph->GsTex0)->CPSM = cpsm;	// CLUT part pixel format setup

	((sceGsTex0 *)&ph->GsTex0)->CSM  = 0;		// CLUT storage mode (always CSM1)

	((sceGsTex0 *)&ph->GsTex0)->CSA  = 0;		// CLUT entry offset (always 0)

	((sceGsTex0 *)&ph->GsTex0)->CLD  = 1;		// CLUT buffer load control (always load)


	if(cbp==-1) {
		// If there is no cbp specification, get value from GsTex0 member of picture header

		cbp = (u_int)((sceGsTex0 *)&ph->GsTex0)->CBP;
	} else {
		// If cbp was specified, overwrite value in GsTex0 member of picture header

		((sceGsTex0 *)&ph->GsTex0)->CBP = cbp;
	}

	// Calculate starting address of CLUT data

	pClut = (u_long128 *)((char *)ph + ph->HeaderSize + ph->ImageSize);

	// Send CLUT data to GS local memory

	// CLUT data format is determined according to CLUT format and texture format

	switch((ph->ClutType<<8) | ph->ImageType) {
		case (((TIM2_RGB16 | 0x40)<<8) | TIM2_IDTEX4):	// 16 colors, CSM1, 16 bits, rearranged

		case (((TIM2_RGB24 | 0x40)<<8) | TIM2_IDTEX4):	// 16 colors, CSM1, 24 bits, rearranged

		case (((TIM2_RGB32 | 0x40)<<8) | TIM2_IDTEX4):	// 16 colors, CSM1, 32 bits, rearranged

		case (( TIM2_RGB16        <<8) | TIM2_IDTEX8):	// 256 colors, CSM1, 16 bits, rearranged

		case (( TIM2_RGB24        <<8) | TIM2_IDTEX8):	// 256 colors, CSM1, 24 bits, rearranged

		case (( TIM2_RGB32        <<8) | TIM2_IDTEX8):	// 256 colors, CSM1, 32 bits, rearranged

			// When the CLUT is a 256-color CLUT and the CLUT storage mode is CSM1

			// When the CLUT is a 16-color CLUT, the CLUT storage mode is CSM1, and the rearranged flag is ON

			// Since the data is placed with the pixels already rearranged, it can be transferred asis

			Tim2LoadTexture(cpsm, cbp, 1, 16, (ph->ClutColors / 16), pClut);
			break;

		case (( TIM2_RGB16        <<8) | TIM2_IDTEX4):	// 16 colors, CSM1, 16 bits, linear placement

		case (( TIM2_RGB24        <<8) | TIM2_IDTEX4):	// 16 colors, CSM1, 24 bits, linear placement

		case (( TIM2_RGB32        <<8) | TIM2_IDTEX4):	// 16 colors, CSM1, 32 bits, linear placement

		case (((TIM2_RGB16 | 0x80)<<8) | TIM2_IDTEX4):	// 16 colors, CSM2, 16 bits, linear placement

		case (((TIM2_RGB24 | 0x80)<<8) | TIM2_IDTEX4):	// 16 colors, CSM2, 24 bits, linear placement

		case (((TIM2_RGB32 | 0x80)<<8) | TIM2_IDTEX4):	// 16 colors, CSM2, 32 bits, linear placement

		case (((TIM2_RGB16 | 0x80)<<8) | TIM2_IDTEX8):	// 256 colors, CSM2, 16 bits, linear placement

		case (((TIM2_RGB24 | 0x80)<<8) | TIM2_IDTEX8):	// 256 colors, CSM2, 24 bits, linear placement

		case (((TIM2_RGB32 | 0x80)<<8) | TIM2_IDTEX8):	// 256 colors, CSM2, 32 bits, linear placement

			// When the CLUT is a 16-color CLUT, the CLUT storage mode is CSM1, and the rearranged flag is OFF

			// When the CLUT is a 16-color CLUT and the CLUT storage mode is CSM2

			// When the CLUT is a 256-color CLUT and the CLUT storage mode is CSM2


			// Since the performance is bad for CSM2, the data is rearranged as CSM1 and transferred

			for(i=0; i<(ph->ClutColors/16); i++) {
				sceGsSetDefLoadImage(&li, (short)cbp, 1, (short)cpsm, (short)((i & 1)*8), (short)((i>>1)*2), 8, 2);
				FlushCache(WRITEBACK_DCACHE);			// Flush D cache

				sceGsExecLoadImage(&li, pClut);			// Transfer CLUT data to GS

				sceGsSyncPath(0, 0);					// Wait until data transfer ends


				// To next 16 colors, update address

				if((ph->ClutType & 0x3F)==TIM2_RGB16) {
					pClut = (u_long128 *)((char *)pClut + 2*16);	// For 16-bit color

				} else if((ph->ClutType & 0x3F)==TIM2_RGB24) {
					pClut = (u_long128 *)((char *)pClut + 3*16);	// For 24-bit color

				} else {
					pClut = (u_long128 *)((char *)pClut + 4*16);	// For 32-bit color

				}
			}
			break;

		default:
			printf("Illegal clut and texture combination. ($%02X,$%02X)\n", ph->ClutType, ph->ImageType);
			return(0);
	}
	return(1);
}


// Write snapshot in TIM2 file

// Arguments

// d0:		Even raster line frame buffer

// d1:		Odd raster line frame buffer (when NULL, non-interlaced)

// pszFname:  TIM2 file name

// Return value

//			When 0, error 

//			When not 0, processing succeeded

int Tim2TakeSnapshot(sceGsDispEnv *d0, sceGsDispEnv *d1, char *pszFname)
{
	int i;

	int h;					// File handle

	int nWidth, nHeight;	// Image dimensions

	int nImageType;			// Image part type

	int psm;				// Pixel format

	int nBytes;				// Number of bytes constituting one raster line


	// Get image size and pixel format

	nWidth  = d0->display.DW / (d0->display.MAGH + 1);
	nHeight = (int)d0->display.DH + 1;
	psm     = (int)d0->dispfb.PSM;

	// Obtain number of bytes per raster line and TIM2 pixel type from pixel format

	switch(psm) {
		case SCE_GS_PSMCT32:	nBytes = nWidth*4;	nImageType = TIM2_RGB32;	break;
		case SCE_GS_PSMCT24:	nBytes = nWidth*3;	nImageType = TIM2_RGB24;	break;
		case SCE_GS_PSMCT16:	nBytes = nWidth*2;	nImageType = TIM2_RGB16;	break;
		case SCE_GS_PSMCT16S:	nBytes = nWidth*2;	nImageType = TIM2_RGB16;	break;
		default:
			// When pixel format is unknown, terminate with error

			// GS_PSGPU24 format is not supported

			printf("Illegal pixel format.\n");
			return(0);
	}


	// Open TIM2 file

	h = sceOpen(pszFname, SCE_WRONLY | SCE_CREAT);
	if(h==-1) {
		// Processing for opening file failed

		printf("file create failure.\n");
		return(0);
	}

	// Write file header

	{
		TIM2_FILEHEADER fhdr;

		fhdr.FileId[0] = 'T';				// Set file ID

		fhdr.FileId[1] = 'I';
		fhdr.FileId[2] = 'M';
		fhdr.FileId[3] = '2';
		fhdr.FormatVersion = 3;				// Format version 4

		fhdr.FormatId  = 0;					// 16-byte alignment mode

		fhdr.Pictures  = 1;					// Number of pictures is 1

		for(i=0; i<8; i++) {
			fhdr.pad[i] = 0x00;				// Clear padding member with 0x00

		}

		sceWrite(h, &fhdr, sizeof(TIM2_FILEHEADER));	// Write file header

	}

	// Write picture header

	{
		TIM2_PICTUREHEADER phdr;
		int nImageSize;

		nImageSize = nBytes * nHeight;
		phdr.TotalSize   = sizeof(TIM2_PICTUREHEADER) + nImageSize;	// Total size

		phdr.ClutSize    = 0;							// No CLUT part

		phdr.ImageSize   = (unsigned int)nImageSize;					// Image part size

		phdr.HeaderSize  = sizeof(TIM2_PICTUREHEADER);	// Header part size

		phdr.ClutColors  = 0;							// Number of CLUT colors

		phdr.PictFormat  = 0;							// Picture format

		phdr.MipMapTextures = 1;						// Number of MIPMAP textures

		phdr.ClutType    = TIM2_NONE;					// No CLUT part

		phdr.ImageType   = (u_char)nImageType;					// Image type

		phdr.ImageWidth  = (u_short)nWidth;						// Image width

		phdr.ImageHeight = (u_short)nHeight;						// Image height


		// Set all GS register settings to 0

		phdr.GsTex0        = 0;
		((sceGsTex0 *)&phdr.GsTex0)->TBW = Tim2CalcBufWidth(psm, nWidth);
		((sceGsTex0 *)&phdr.GsTex0)->PSM = psm;
		((sceGsTex0 *)&phdr.GsTex0)->TW  = Tim2GetLog2(nWidth);
		((sceGsTex0 *)&phdr.GsTex0)->TH  = Tim2GetLog2(nHeight);
		phdr.GsTex1        = 0;
		phdr.GsTexaFbaPabe = 0;
		phdr.GsTexClut     = 0;

		sceWrite(h, &phdr, sizeof(TIM2_PICTUREHEADER));	// Write picture header

	}


	// Write image data

	for(i=0; i<nHeight; i++) {
		u_char buf[4096];			// Reserve 4KB raster buffer

		sceGsStoreImage si;

		if(d1) {
			// When interlaced

			if(!(i & 1)) {
				// When there is an even number of raster lines in interlaced display

				sceGsSetDefStoreImage(&si, (short)(d0->dispfb.FBP*32), (short)d0->dispfb.FBW, (short)psm,
									(short)d0->dispfb.DBX, (short)(d0->dispfb.DBY + (short)((short)i/2)),
									(short)nWidth, 1);
			} else {
				// When there is an odd number of raster lines in interlaced display

				sceGsSetDefStoreImage(&si, (short)(d1->dispfb.FBP*32), (short)d1->dispfb.FBW, (short)psm,
									(short)d1->dispfb.DBX, (short)(d1->dispfb.DBY + (short)(i/2)),
									(short)nWidth, 1);
			}
		} else {
			// When non-interlaced

			sceGsSetDefStoreImage(&si, (short)(d0->dispfb.FBP*32), (short)d0->dispfb.FBW, (short)psm,
									(short)d0->dispfb.DBX, (short)(d0->dispfb.DBY + (short)i),
									(short)nWidth, 1);
		}
		FlushCache(WRITEBACK_DCACHE);				// Flush D cache


		sceGsExecStoreImage(&si, (u_long128 *)buf);	// Start transfer to VRAM

		sceGsSyncPath(0, 0);						// Wait until data transfer ends


		sceWrite(h, buf, nBytes);					// Write data for one raster line

	}

	// File writing completed

	sceClose(h);						// Close file

	return(1);
}






// Transfer texture data

// Arguments

// psm:		Texture pixel format

// tbp:		Texture buffer base point

// tbw:		Texture buffer width

// w:		Transfer area width

// h:		Transfer area line count

// pImage:	Address where texture image is stored

static void Tim2LoadTexture(int psm, u_int tbp, int tbw, int w, int h, u_long128 *pImage)
{
	sceGsLoadImage li;
	int i, l, n;
	u_long128 *p;

	switch(psm) {
		case SCE_GS_PSMZ32:
		case SCE_GS_PSMCT32:
			n = w*4;
			break;

		case SCE_GS_PSMZ24:
		case SCE_GS_PSMCT24:
			n = w*3;
			break;

		case SCE_GS_PSMZ16:
		case SCE_GS_PSMZ16S:
		case SCE_GS_PSMCT16:
		case SCE_GS_PSMCT16S:
			n = w*2;
			break;

		case SCE_GS_PSMT8H:
		case SCE_GS_PSMT8:
			n = w;
			break;

		case SCE_GS_PSMT4HL:
		case SCE_GS_PSMT4HH:
		case SCE_GS_PSMT4:
			n = w/2;
			break;

		default:
			return;
	}

	// Send while dividing data so that it does not exceed the maximum DMA transfer size of 512KB

	l = 32764 * 16 / n;
	for(i=0; i<h; i+=l) {
		p = (u_long128 *)((char *)pImage + n*i);
		if((i+l)>h) {
			l = h - i;
		}

		sceGsSetDefLoadImage(&li, (short)tbp, (short)tbw, (short)psm, 0, (short)i, (short)w, (short)l);
		FlushCache(WRITEBACK_DCACHE);		// Flush D cache

		sceGsExecLoadImage(&li, p);			// Start transfer to GS local memory

		sceGsSyncPath(0, 0);				// Wait until data transfer ends

	}
	return;
}



// Calculate buffer size from specified pixel format and texture size

// Arguments

// psm:		Texture pixel format

// w:		Width

// Return value

//			Buffer size consumed by one line

//			Units:  256 bytes (64 words)

static int Tim2CalcBufWidth(int psm, int w)
{
//	return(w / 64);

	switch(psm) {
		case SCE_GS_PSMT8H:
		case SCE_GS_PSMT4HL:
		case SCE_GS_PSMT4HH:
		case SCE_GS_PSMCT32:
		case SCE_GS_PSMCT24:
		case SCE_GS_PSMZ32:
		case SCE_GS_PSMZ24:
		case SCE_GS_PSMCT16:
		case SCE_GS_PSMCT16S:
		case SCE_GS_PSMZ16:
		case SCE_GS_PSMZ16S:
			return((w+63) / 64);

		case SCE_GS_PSMT8:
		case SCE_GS_PSMT4:
			w = (w+63) / 64;
			if(w & 1) {
				w++;
			}
			return(w);
	}
	return(0);
}



// Calculate buffer size from specified pixel format and texture size

// Arguments

// psm:		Texture pixel format

// w:		Width

// h:		Number of lines

// Return value

//			Buffer size consumed by one line

//			Units:  256 bytes (64 words)

// Note

//			When storing a pixel format for which the following texture differs,

//			the alignment of tbp must be adjusted to a 2KB-page boundary.

static int Tim2CalcBufSize(int psm, int w, int h)
{
	(void)psm;
	return(w * h / 64);
/*
	switch(psm) {
		case SCE_GS_PSMT8H:
		case SCE_GS_PSMT4HL:
		case SCE_GS_PSMT4HH:
		case SCE_GS_PSMCT32:
		case SCE_GS_PSMCT24:
		case SCE_GS_PSMZ32:
		case SCE_GS_PSMZ24:
			// 1 word is consumed per pixel
			return(((w+63)/64) * ((h+31)/32));

		case SCE_GS_PSMCT16:
		case SCE_GS_PSMCT16S:
		case SCE_GS_PSMZ16:
		case SCE_GS_PSMZ16S:
			// 1/2 word is consumed per pixel
			return(((w+63)/64) * ((h+63)/64));

		case SCE_GS_PSMT8:
			// 1/4 word is consumed per pixel
			return(((w+127)/128) * ((h+63)/64));

		case SCE_GS_PSMT4:
			// 1/8 word is consumed per pixel
			return(((w+127)/128) * ((h+127)/128));
	}
	// Error?
	return(0);

*/
}



// Get bit width
static int Tim2GetLog2(int n)
{
	int i;
	for(i=31; i>0; i--) {
		if(n & (1<<i)) {
			break;
		}
	}
	if(n>(1<<i)) {
		i++;
	}
	return(i);
}


#endif	// R5900


