/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*/

#include <stdio.h>
#include <string.h>

#include <kerror.h>

#include "strfile.h"
#include <fatms.h>
#include <umddevctl.h>
#include <mediaman.h>

SceInt32 strFileAssign (StrFile* file, const SceChar8* filename) {
	SceInt32 ret;
	
	if (memcmp(filename, "host0:", 6) == 0) {
		/*J hostからの読みこみ */
		strcpy(file->assignName, "host0:");

	} else if (memcmp (filename, SCE_UMD_ALIAS_NAME, 6) == 0) {
		/*J DISCからの読みこみ */
		strcpy(file->assignName, SCE_UMD_ALIAS_NAME);

		/*J media検出チェック */
		 if (!sceUmdCheckMedium()) { 
	 
			 /*J メディアが挿入イベント待ち */
			 sceUmdWaitDriveStat(SCE_UMD_MEDIA_IN);
		 } 
		
		/*J アクティベート */
		ret = sceUmdActivate(SCE_UMD_MODE_POWERON, SCE_UMD_ALIAS_NAME);
		if (ret < 0) {
			printf ("fatal error : sceUmdActivate() 0x%x\n", ret);
			return -1;
		}

		sceUmdWaitDriveStat(SCE_UMD_READABLE);

	} else if (memcmp (filename, SCE_FATMS_ALIAS_NAME, 4) == 0) {
		/*J Memory Stickからの読みこみ */
		strcpy (file->assignName, SCE_FATMS_ALIAS_NAME);

	} else {
		printf ("fatal error : not supported device\n");
		return -1;
	}

	return 1;

}

SceInt32 strFileOpen(StrFile *file, const SceChar8 *filename, SceInt32 flags)
{

	if (memcmp(filename, "host0:", 6) == 0 || 
		memcmp(filename, SCE_UMD_ALIAS_NAME, 6) == 0 ||
		memcmp(filename, SCE_FATMS_ALIAS_NAME, 4) == 0 ) {

			//file->fd = sceIoOpen (filename, flags, 0);
			file->fd = sceIoOpen (filename, flags, 00777);
		if (file->fd < 0)
		{
			printf ("fatal error : sceIoOpen() 0x%x\n", (SceInt32)file->fd);
			return -1;
		}
//		else
//			printf("sceIoOpen() 0x%x\n", (SceInt32)file->fd);

	} else {
		printf ("fatal error : not supported device\n");
		return -1;
	}

	return 1;
}


SceInt32 strFileClose(StrFile *file)
{
	SceInt32 ret;

	if (file->fd >= 0) {
		ret = sceIoClose(file->fd);
	} else {
		return -1;
	}

	return ret;
}

SceInt32 strFileUnassign(StrFile* file) {
	SceInt32 ret = 1;
	
	if (strcmp(file->assignName, SCE_UMD_ALIAS_NAME) == 0) {
		/*J ディアクティベート処理 */
		ret = sceUmdDeactivate(SCE_UMD_MODE_POWERCUR, SCE_UMD_ALIAS_NAME);
	}

	return ret;
}

SceInt32 strFileRead(StrFile *file, SceVoid *buff, SceInt32 size)
{
	SceInt32 readsize;

    readsize = sceIoRead(file->fd, (SceChar8*)buff, size);

	return readsize;
}

SceInt32 strFileWrite(StrFile *file, SceVoid *buff, SceInt32 size)
{
    return sceIoWrite(file->fd, (SceChar8*)buff, size);
}

SceOff strFileLseek (StrFile *file, SceOff offset, SceInt32 whence)
{
    return sceIoLseek (file->fd, offset, whence);
}

SceInt32 strGetFileSize( const char *file )
{
        SceIoStat statusForFile;

        if( sceIoGetstat( file, &statusForFile ) != 0 )
                return -1;

        return statusForFile.st_size;

}
