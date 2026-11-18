/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
 */

#ifndef _STRFILE_H_
#define _STRFILE_H_

#include <psptypes.h>
#include <kernel.h>

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif  /* defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus) */

typedef struct {
	SceUID fd;
	SceChar8 assignName[10];
} StrFile;

SceInt32  strFileOpen (StrFile *file, const SceChar8 *filename, SceInt32 flag);
SceInt32  strFileClose(StrFile *file);
SceInt32  strFileRead (StrFile *file, SceVoid* buff, SceInt32 size);
SceInt32  strFileWrite(StrFile *file, SceVoid* buff, SceInt32 size);
SceOff    strFileLseek(StrFile *file, SceOff offset, SceInt32 whence);
SceInt32  strFileAssign(StrFile* file, const SceChar8* filename);
	SceInt32  strFileUnassign (StrFile* file);

SceInt32 strGetFileSize( const char *file );

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif  /* defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus) */

#endif	/* _STRFILE_H_ */
