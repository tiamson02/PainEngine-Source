/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgFile.h
*   Medium independant file access
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-23-08      breugelmans initial implementation
*/

#ifndef _TG_FILE_H_
#define _TG_FILE_H_

#include <kerneltypes.h>
#include "tgTypes.h"

//-----------------------------------------------------------------------------
// M A C R O S ,   C O N S T A N T S
//-----------------------------------------------------------------------------

#define TG_FILE_O_RDONLY  (0x0001)	//read only
#define TG_FILE_O_WRONLY  (0x0002)  //write only 
#define TG_FILE_O_RDWR    (0x0003)  //read and write   
#define TG_FILE_O_APPEND  (0x0100)    /* append (writes guaranteed at the end) */
#define TG_FILE_O_CREAT   (0x0200)    /* open with file create */
#define TG_FILE_O_TRUNC   (0x0400)    /* open with truncation */
#define TG_FILE_O_EXCL    (0x0800)    /* exclusive create */

//file access type of function
typedef enum
{
	TG_FILE_FUNC_OPEN,	//open file function
	TG_FILE_FUNC_SEEK,	//seek file function
	TG_FILE_FUNC_READ,	//read file function
	TG_FILE_FUNC_WRITE,	//write file function
	TG_FILE_FUNC_CLOSE,	//close file function
	TG_FILE_FUNC_OPEN_MEM, //open file in memory function
	TG_FILE_FUNC_ERROR,	//error
} ETgFileFunc;

//file access blocking mode
typedef enum
{
	TG_FILE_NON_BLOCKING = 0,	//non blocking mode
	TG_FILE_BLOCKING = 1		//blocking mode
	
} ETgFileBlockMode;

//file storage medium
typedef enum
{
	TG_FILE_MEDIUM_NONE,	//none specified
	TG_FILE_MEDIUM_EMU,		//emulator
	TG_FILE_MEDIUM_UMD,		//UMD
	TG_FILE_MEDIUM_MEMORY,	//memory
	TG_FILE_MEDIUM_DEVKIT	//devkit memory
	
} ETgFileMedium;

//file position
typedef enum
{
	TG_FILE_SET,	//set file pointer relative to start
	TG_FILE_CUR,	//set relative to current file pointer position
	TG_FILE_END		//set file pointer relative to end of file
	
} ETgFileWhence;

struct _TgFileHandle;

// callback function type to register user code with file functions
typedef int TgFileCallbackFn(int count, int arg, void *param);

// file handling structure
typedef struct _TgFileHandle
{
	char fileName[128];	// file name
	u8 *startAddr;		// start address for TG_FILE_MEDIUM_MEMORY
	u8 *curAddr;	 // current address for TG_FILE_MEDIUM_MEMORY
	int fp;          // for sceOpen type handles
	s64 bytePos;     // current file position
	s64 byteSize;	 // total file size in bytes
//	s64 lsn;         // Logical sector number
	TgFileCallbackFn *callbackFn; // callback function on completion
	int state;		// reading, writing, seeking etc (for sync functions/callbacks) 
	s64 reqOffset;		// target offset for non blocking functions
	SceUID  kernelCallbackUID; // kernel callback ID
} TgFileHandle; 


//convert bytes to sectors
#define BYTES_TO_SECTORS(x) (((x)+2047)/2048)


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//--- PSP file functions
void	 tgFileInit( const char *rootDir, void *cacheArea, int cacheSize );
void	 tgFileSelectMedium( ETgFileMedium medium ); 
void	 tgFileSetMediumParams( void *mediumParam ); //---param is: IP addr, file root, ...


//--- Open functions
ETgResult	tgFileOpenMem( TgFileHandle *file, void *addr, s64 size, int accessMode, 
						 ETgFileBlockMode blkMode, TgFileCallbackFn* callbackFn ); 
ETgResult	tgFileOpen(  TgFileHandle *file, const char *fileName, int accessMode, 
						 ETgFileBlockMode blkMode, TgFileCallbackFn* callbackFn ); 

s64			tgFileGetSize(  TgFileHandle *file ); //---Get Size in bytes

//--- Read functions
ETgResult	tgFileSeek(  TgFileHandle *file, s64 byteOffset, ETgFileWhence whence, 
						 ETgFileBlockMode blkMode );
ETgResult	tgFileRead(  TgFileHandle *file, void *dest, s64 size, 
						 ETgFileBlockMode blkMode );
ETgResult	tgFileReadOffset(  TgFileHandle *file, void *dest, s64 offset, s64 size,
							   ETgFileBlockMode blkMode );
s64			tgFileGetPos(  TgFileHandle *file);

//--- Write functions
ETgResult	tgFileWrite( TgFileHandle *file, void *source, s64 byteCount, ETgFileBlockMode blkMode );

//--- Close
ETgResult	tgFileClose( TgFileHandle *file, ETgFileBlockMode blkMode); 

//--- Status and Sync
ETgResult	tgFileSync( TgFileHandle *file, ETgFileBlockMode blkMode); 

//---------------------------------------

//-------------------------------------
struct SceIoStat;
struct SceIoDirent;

int tgStatFile(const char * fileName, struct SceIoStat * statbuffer);

SceUID tgDopen(	const char *dirname );
int tgDclose( SceUID fd );
int tgDread( SceUID fd,	struct SceIoDirent *buf);

int tgIoOpen(const char *filename, int flags, int mode);

int tgIoMkdir(const char *filename);


#ifdef __cplusplus
}
#endif

#endif // _TG_FILE_H_
