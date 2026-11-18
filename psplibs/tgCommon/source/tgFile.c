/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgFile.c
*   Medium independent file access
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-23-08      breugelmans initial implementation
*   2005-      umd and async support added, callbacks changed
*/


//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "tgTypes.h"

#ifdef _PSPEMU
	#include <libemu.h>
#else
	#include <kernelutils.h>
	#include <iofilemgr.h>
	#include <mediaman.h>
	#include <kerror.h>
#endif

#include <string.h>
#include <ctype.h>
#include <thread.h>
#include "tgAssert.h"
#include "tgFile.h"

//-----------------------------------------------------------------------------
// M A C R O S ,   C O N S T A N T S
//-----------------------------------------------------------------------------

#define IOP_COPY_CHUNK_SIZE 2048
#define SHARED_MOD_TABLE_SIZE 16


//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------
static ETgResult _TranslateFileName( TgFileHandle *file, const char *name );

static ETgResult _OpenCore( TgFileHandle *file, const char *fileName, int mode, ETgFileBlockMode blkMode);
static ETgResult _OpenCoreMem( TgFileHandle *file, void *addr, s64 size, int mode, ETgFileBlockMode blkMode);
static ETgResult _ReadCore(  TgFileHandle *file, void *dest, s64 offset, s64 size,
							 ETgFileBlockMode blkMode );
static ETgResult _SeekCore(  TgFileHandle *file, s64 byteOffset, ETgFileWhence whence,
							ETgFileBlockMode blkMode  );
static ETgResult _WriteCore( TgFileHandle *file, void *source, s64 byteCount, 
					  ETgFileBlockMode blkMode );

static ETgFileBlockMode _CheckMediumBlocking( TgFileHandle *file, ETgFileBlockMode blkMode);
static int	 _TranslateWhence( ETgFileWhence whence, ETgFileMedium medium);
static void  _BackSlashToForwardSlash( char *str);
static void  _ForwardSlashToBackSlash( char *str);
//static void  _BackSlashToUpperForwardSlash( char *str);
//static void  _ForwardSlashToUpperBackSlash( char *str);

//static int _InitUMDdrive(void);
//static int _WaitForUMDReady(void);
static void _RegisterCallback( TgFileHandle *file, TgFileCallbackFn* callbackFn, ETgFileBlockMode blkMode );
static void _NotifyCallBack(TgFileHandle *file, ETgResult result);

#ifdef _PSPEMU
static void  _BuildModeString( char *str, int mode);
#endif
//-----------------------------------------------------------------------------
// S T A T I C   V A R I A B L E S
//-----------------------------------------------------------------------------
static int _tgFileInitCalled = 0;
static int _lastError = 0;			//see kerror.h
static ETgFileMedium _fileMedium = TG_FILE_MEDIUM_NONE;
static char _rootDir[128] = "";

//-----------------------------------------------------------------------------
// S T A T I C   F U N C T I O N S
//-----------------------------------------------------------------------------

// Convert case, backslashes/forwardslash, add root path
// Add IP address if required.
static ETgResult _TranslateFileName( TgFileHandle *file, const char *name )
{
	// Remove any trailing backslash
	if ((name[0]=='\\') || (name[0]=='/'))
		name++;

	// Check case, backslash directions
	// Prepend root directory
	switch(_fileMedium)
	{
		case TG_FILE_MEDIUM_EMU:		
			strcpy(file->fileName, _rootDir);
			strcat(file->fileName, name);
			//_ForwardSlashToBackSlash(file->fileName);
			_BackSlashToForwardSlash(file->fileName);
			break;
		case TG_FILE_MEDIUM_DEVKIT:		
			strcpy(file->fileName, "host0:");
			strcat(file->fileName, _rootDir);
			strcat(file->fileName, name);
			//_ForwardSlashToBackSlash(file->fileName);
			_BackSlashToForwardSlash(file->fileName);
			break;
		case TG_FILE_MEDIUM_UMD:		
			strcpy(file->fileName, SCE_UMD_ALIAS_NAME);
			strcat(file->fileName, _rootDir);
			strcat(file->fileName, name);
			//_ForwardSlashToBackSlash(file->fileName);
			_BackSlashToForwardSlash(file->fileName);
			break;
		default:
			return TG_FAIL;	
	}
	return TG_SUCCESS;
}

static void _BackSlashToForwardSlash( char *str)
{
	while (*str)
	{
		if (*str == '\\') *str = '/';
		
		str++;
	}
}

static void _ForwardSlashToBackSlash( char *str)
{
	while (*str)
	{
		if (*str == '/') *str = '\\';
		
		str++;
	}
}

#ifdef _PSPEMU
static void  _BuildModeString( char *str, int mode)
{
	strcpy(str, "");
	
	if (mode & 0x0001)	strcat(str, "r");
	if (mode & 0x0002) 	strcat(str, "w");

	strcat(str, "b");
	
}
#endif

// Checks whether medium can support non-blocking
static ETgFileBlockMode _CheckMediumBlocking( TgFileHandle *file, ETgFileBlockMode blkMode)
{
	(void) file; // To remove "unused argument" warnings.
	ETgFileBlockMode ret;
	
	if (blkMode == TG_FILE_BLOCKING) return TG_FILE_BLOCKING;
	
	switch (_fileMedium)
	{
		case TG_FILE_MEDIUM_MEMORY:
			ret = TG_FILE_BLOCKING;
			break;
		case TG_FILE_MEDIUM_EMU:
			ret = TG_FILE_BLOCKING;
			break;
		case TG_FILE_MEDIUM_DEVKIT:
			ret = TG_FILE_BLOCKING;
			break;
		case TG_FILE_MEDIUM_UMD:
			ret = TG_FILE_NON_BLOCKING;
			break;
		default:
			ret = TG_FILE_BLOCKING;
			break;
	}
	return ret;
}

// Convert ETgFileWhence into medium specific whence
static int _TranslateWhence( ETgFileWhence whence, ETgFileMedium medium)
{
	int newWhence = whence;
	
	switch (medium)
	{		
#ifdef _PSPEMU
		case TG_FILE_MEDIUM_EMU:
		{
			switch (whence)
			{
				case TG_FILE_SET:
					newWhence = SCEEMU_SEEK_SET;
					break;
				case TG_FILE_CUR:
					newWhence = SCEEMU_SEEK_CUR;
					break;
				case TG_FILE_END:
					newWhence = SCEEMU_SEEK_END;
					break;
			}
			break;
		}
#else
		case TG_FILE_MEDIUM_DEVKIT:
		case TG_FILE_MEDIUM_UMD:
			switch (whence)
			{
				case TG_FILE_SET:
					newWhence = SEEK_SET;
					break;
				case TG_FILE_CUR:
					newWhence = SEEK_CUR;
					break;
				case TG_FILE_END:
					newWhence = SEEK_END;
					break;
			}
			break;
#endif
		default:
			break;		
	}
	
	return newWhence;
}

static void _RegisterCallback( TgFileHandle *file, TgFileCallbackFn* callbackFn, ETgFileBlockMode blkMode )
{	
	int *param = (int*)file;
	SceUID id= sceKernelCreateCallback("TGFILE_CB", callbackFn, param);
	if (id < 0) 
	{
		TG_ASSERT(id>0, "tgFile RegisterCallback: sceKernelCreateCallback failed");
	}

	file->kernelCallbackUID = id;

	if(blkMode==TG_FILE_NON_BLOCKING)
	{
		sceIoSetAsyncCallback( file->fp, id, (int)file);
	}
}

static void _NotifyCallBack(TgFileHandle *file, ETgResult result)
{
//	TG_OUTPUT_TEXT("tgFile Kernel notify\n");
	sceKernelNotifyCallback(file->kernelCallbackUID, (int)result);
	sceKernelCheckCallback();
}


static ETgResult _OpenCoreMem( TgFileHandle *file, void *addr, s64 size, int mode, ETgFileBlockMode blkMode)
{
	(void) mode; // To remove "unused argument" warnings.
	ETgResult result = TG_FAIL;
	
	TG_ASSERT(_tgFileInitCalled, "tgFileInit has not been called");
	TG_ASSERT((_fileMedium!=TG_FILE_MEDIUM_NONE), "No medium selected");

	file->bytePos = 0;
	file->byteSize = 0;
	file->reqOffset = 0;
	file->state=TG_FILE_FUNC_ERROR;

	if (blkMode == TG_FILE_BLOCKING)
	{	
		switch(_fileMedium)
		{
			case TG_FILE_MEDIUM_MEMORY:
				file->startAddr = (u8*)addr;
				file->curAddr = file->startAddr;
				file->byteSize = size;
				result = TG_SUCCESS;
				break;
			default:
				break;
		}
		file->state=TG_FILE_FUNC_OPEN_MEM;
		if(file->kernelCallbackUID) _NotifyCallBack(file,result);
	}

	return result;
}




static ETgResult _OpenCore( TgFileHandle *file, const char *fileName, int mode, ETgFileBlockMode blkMode)
{
	ETgResult result = TG_FAIL;
#ifdef _PSPEMU
	char modeString[10];
#endif
	
	TG_ASSERT(_tgFileInitCalled, "tgFileInit has not been called");
	TG_ASSERT((_fileMedium!=TG_FILE_MEDIUM_NONE), "No medium selected");
	TG_ASSERT((_fileMedium!=TG_FILE_MEDIUM_MEMORY),"Should have used _OpenCoreMem");

	_TranslateFileName(file, fileName); // Translate filename etc

	file->bytePos = 0;
	file->byteSize = 0;
	file->reqOffset = 0;
	file->state=TG_FILE_FUNC_ERROR;

	if (blkMode == TG_FILE_BLOCKING)
	{	
		switch(_fileMedium)
		{
#ifdef _PSPEMU
			case TG_FILE_MEDIUM_EMU:
				_BuildModeString(modeString, mode);
				file->fp = sceEmuFopen(file->fileName, modeString);
				if (file->fp!=-1) 
				{
					file->byteSize = sceEmuFilelength(file->fp);
					if (file->byteSize != -1)
						result = TG_SUCCESS;
				}
				break;
#else
			case TG_FILE_MEDIUM_DEVKIT:
				file->fp = sceIoOpen(file->fileName, mode, 0);
				if (file->fp >= 0)
				{
					SceOff ret;
					ret = sceIoLseek(file->fp, 0, SEEK_END);
					if (ret < 0)
					{ 
						sceIoClose(file->fp);
						TG_OUTPUT_TEXT("file open(seek end) failed on:%s\n", file->fileName);
						result = TG_FAIL;
					}
					else
					{
						file->byteSize = ret;
						ret = sceIoLseek(file->fp, 0, SEEK_SET);
						result = TG_SUCCESS;
					}
				}
				break;

			case TG_FILE_MEDIUM_UMD:
				file->fp = sceIoOpen(file->fileName, mode, 0644);
				if (file->fp >= 0)
				{
					SceOff ret;
					ret = sceIoLseek(file->fp, 0, SEEK_END);
					if (ret < 0)
					{ 
						sceIoClose(file->fp);
						TG_OUTPUT_TEXT("file open(seek end) failed on:%s\n", file->fileName);
						result = TG_FAIL;
					}
					else
					{
						file->byteSize = ret;
						ret = (int)sceIoLseek(file->fp, 0, SEEK_SET);
						result = TG_SUCCESS;
					}
				}				
				break;
#endif
			default:
				break;
		}
		if(result==TG_SUCCESS)
		{
			file->state=TG_FILE_FUNC_OPEN;
			if(file->kernelCallbackUID) _NotifyCallBack(file, result);
		}
	}
	else 
	{
		file->state=TG_FILE_FUNC_OPEN;
		file->fp = sceIoOpenAsync(file->fileName, mode, 0644);
		if (file->fp>=0)
		{
			//TODO: remove blocking seek for size
			ETgResult res = tgFileSync( file, TG_FILE_BLOCKING );
			if(res==TG_SUCCESS)
			{
				SceOff ret;
				ret = sceIoLseek(file->fp, 0, SEEK_END);
				if (ret < 0)
				{ 
					sceIoClose(file->fp);
					TG_OUTPUT_TEXT("file open(seek end) failed on:%s\n", file->fileName);
					result = TG_FAIL;
				}
				else
				{
					file->byteSize = ret;
					ret = (int)sceIoLseek(file->fp, 0, SEEK_SET);
					result = TG_SUCCESS;
				}
			}
		}
	}
	return result;
}


static ETgResult _ReadCore(  TgFileHandle *file, void *dest, s64 offset, s64 size,
							 ETgFileBlockMode blkMode )
{
	int ret;
	ETgResult result = TG_FAIL;
	file->state=TG_FILE_FUNC_ERROR;

	if (blkMode == TG_FILE_BLOCKING)
	{
		if (offset!=file->bytePos)
			_SeekCore(file, offset, TG_FILE_SET, TG_FILE_BLOCKING);

		// Perform blocking read
		switch(_fileMedium)
		{
			case TG_FILE_MEDIUM_MEMORY:
				if ((file->bytePos + size) > file->byteSize )
				{
					result = TG_FAIL;
				}
				else
				{					
					memcpy(dest, file->curAddr, size);
					file->curAddr += size;
					file->bytePos += size;
					result = TG_SUCCESS;
				}
				break;
#ifdef _PSPEMU
			case TG_FILE_MEDIUM_EMU:
				//ret = sceEmuFread(dest, size, 1, file->fp);
				if (sceEmuFread(dest, size, 1, file->fp);)
				{
					ret = sceEmuFtell(file->fp);
					if(ret!=-1)
					{
						file->bytePos = ret;
						result = TG_SUCCESS;
					}	
				}
				break;
#else
			case TG_FILE_MEDIUM_DEVKIT:
			case TG_FILE_MEDIUM_UMD:
				ret = sceIoRead(file->fp, dest, size);
				if (ret)
				{
					TG_WARNING(ret==size,"eof, ret=%d size=%lld",ret,size);
					file->bytePos += ret;
					sceKernelDcacheWritebackAll();
					result = TG_SUCCESS;					
				}

				break;
#endif
			default:
				break;
		}
		file->state=TG_FILE_FUNC_READ;
		if(file->kernelCallbackUID>0) _NotifyCallBack(file, result);
	}
	else 
	{
		if (offset!=file->bytePos)		//TODO: async seek
			_SeekCore(file, offset, TG_FILE_SET, TG_FILE_BLOCKING);

		file->state=TG_FILE_FUNC_READ;
		int ret=sceIoReadAsync(file->fp, dest, size);
		if (ret==SCE_KERNEL_ERROR_OK)
		{
			result = TG_SUCCESS;
			file->reqOffset=size;
		}
		else
			_lastError=ret;
	}
	
	return result;
}


// _SeekCore performs blocking/non blocking seek to a file position
// At a later stage this Can consider sector sizes and internal cache to seek to aligned
// addresses internally
static ETgResult _SeekCore( TgFileHandle *file,	s64 byteOffset, ETgFileWhence whence,
							ETgFileBlockMode blkMode )
{

	ETgResult result = TG_FAIL;
	int mediumWhence;
	s64 oldBytePos = file->bytePos;
	file->state=TG_FILE_FUNC_ERROR;

//#ifndef _PSPEMU
	s64 ret;
//#endif
	mediumWhence = _TranslateWhence( whence, _fileMedium);
	
	if (blkMode == TG_FILE_BLOCKING)
	{
		// Perform blocking read
		switch(_fileMedium)
		{
#ifdef _PSPEMU
			case TG_FILE_MEDIUM_EMU:
				if(sceEmuFseek(file->fp, byteOffset, mediumWhence )==0)
				{
					ret = sceEmuFtell(file->fp);
					if(ret!=-1)
					{
						file->bytePos = ret;
						result = TG_SUCCESS;
					}					
				}
				break;
#else
			case TG_FILE_MEDIUM_DEVKIT:
			case TG_FILE_MEDIUM_UMD:
				ret = sceIoLseek(file->fp, byteOffset, mediumWhence );
				if(ret!=-1)
				{
					file->bytePos = ret;
					result = TG_SUCCESS;
				}
				break;
#endif
			case TG_FILE_MEDIUM_MEMORY:
			{
				switch(whence)
				{
					case TG_FILE_SET:
						file->bytePos = byteOffset;
						break;
					case TG_FILE_CUR:
						file->bytePos += byteOffset;
						break;
					case TG_FILE_END:
						file->bytePos = (file->byteSize - 1) - byteOffset;
						break;							
				}
				if ( (file->bytePos < file->byteSize) && (file->bytePos >= 0) )
				{
					file->curAddr = file->startAddr + file->bytePos;
					result = TG_SUCCESS;
				}
				else
				{
					file->bytePos = oldBytePos;
				}
				break;			
			}
			default:
				break;
		}
		file->state=TG_FILE_FUNC_SEEK;
		if(file->kernelCallbackUID) _NotifyCallBack(file, result);
	}
	else 
	{
		file->state=TG_FILE_FUNC_SEEK;
		int ret=sceIoLseekAsync(file->fp, byteOffset, mediumWhence);
		if (ret==SCE_KERNEL_ERROR_OK)
		{
			result = TG_SUCCESS;
			switch(whence)
			{
				case TG_FILE_SET:
					file->reqOffset = byteOffset;
					break;
				case TG_FILE_CUR:
					file->reqOffset = file->bytePos + byteOffset;
					break;
				case TG_FILE_END:
					file->reqOffset = file->byteSize + byteOffset;
					break;							
			}
		}
		else
			_lastError=ret;
	}
	
	return result;

}

static ETgResult _WriteCore( TgFileHandle *file, void *source, s64 byteCount, 
					  ETgFileBlockMode blkMode )
{
	ETgResult result = TG_FAIL;
	file->state=TG_FILE_FUNC_ERROR;
	
	if (blkMode == TG_FILE_BLOCKING)
	{
		// Perform blocking write
		switch(_fileMedium)
		{
			case TG_FILE_MEDIUM_MEMORY:
				result = TG_FAIL; // Not supported
#ifdef _PSPEMU
			case TG_FILE_MEDIUM_EMU:
				if (sceEmuFwrite(source, byteCount, 1, file->fp))
					result = TG_SUCCESS;
				else
					result = TG_FAIL;
				break;
#else
			case TG_FILE_MEDIUM_DEVKIT:
				if (sceIoWrite(file->fp, source, byteCount))
					result = TG_SUCCESS;
				else
					result = TG_FAIL;
				break;
			case TG_FILE_MEDIUM_UMD:
				result = TG_FAIL;
				break;
#endif			
			default:
				break;
		}
		if (result == TG_SUCCESS)
		{
			file->bytePos += byteCount;
			file->byteSize = file->bytePos;
		}

		file->state=TG_FILE_FUNC_WRITE;
		if(file->kernelCallbackUID) _NotifyCallBack(file, result);
	}
	else 
	{
		result = TG_FAIL;
	}
	
	return result;
}


//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// tgFileInit
// Description: Initialise the tgFile module
// Parameters:
//	 rootDir - Root directory
//   cacheArea - (optional) Pointer to buffer for caching
//   cacheSize - Size of buffer for caching
// Returns:  void
// Notes:
//-----------------------------------------------------------------------------
void tgFileInit( const char *rootDir, void *cacheArea, int cacheSize )
{
	(void) cacheArea; // To remove "unused argument" warnings.
	(void) cacheSize; // To remove "unused argument" warnings.
	(void) _tgFileInitCalled; // To remove "unused argument" warnings.
	(void) _lastError; // To remove "unused argument" warnings.
	
	int pathLength;

	_fileMedium = TG_FILE_MEDIUM_NONE;
	_tgFileInitCalled = 1;
	*_rootDir='\0';

	if (rootDir)
	{
		pathLength = strlen(rootDir);
		if (pathLength)
		{
			strcpy(_rootDir, rootDir);
			_ForwardSlashToBackSlash(_rootDir);

			if (_rootDir[pathLength-1]!='\\') strcat(_rootDir, "\\");
		}
	}

}

//-----------------------------------------------------------------------------
// tgFileSelectMedium()
// Description: Selects a file medium
// Parameters:
//	 medium - FileMedium type
// Returns: void
// Notes:
//-----------------------------------------------------------------------------
void tgFileSelectMedium( ETgFileMedium medium)
{
	_fileMedium = medium;

	//TODO- move init medium from here- 
	//maybe use thread + set up in tgSetMediumParams
	if( medium==TG_FILE_MEDIUM_UMD)
	{
		int ret=0;
		ret = sceUmdActivate(SCE_UMD_MODE_POWERON, SCE_UMD_ALIAS_NAME); // <0 means error
		if(ret<0) TG_ASSERT(0,"failed UMD init");

		if (!sceUmdCheckMedium()) // Check disc is inserted
		{ 
			TG_PRINT("\nInsert disc check...");
			sceUmdWaitDriveStat(SCE_UMD_MEDIA_IN);
			TG_PRINT("disc is inserted.\n");
		}

		sceUmdWaitDriveStat(SCE_UMD_READABLE);
		TG_PRINT("UMD readable\n");
	}
}

//-----------------------------------------------------------------------------
// tgSetMediumParams()
// Description: So parameters specific to each medium type
// Parameters:
//	 mediumParams - Medium specific parameters
// Returns:  void
// Notes:
//-----------------------------------------------------------------------------
void tgFileSetMediumParams( void *mediumParams)
{
	(void) mediumParams; // To remove "unused argument" warnings.

}

//-----------------------------------------------------------------------------
// tgFileOpenMem()
// Description: Opens a file from memory
// Parameters:
//	 file - File handle
//	 addr - Location of the file in memory
//	 size - Size of file in memory (for overrun checks etc)
//   accessMode - Whether file is open for read, write etc.
//   blkMode - Blocking mode must be TG_FILE_BLOCKING
//   callbackFn - Optional function called on completion
// Returns:  TG_FAIL, TG_SUCCESS or TG_INDETERMINATE
// Notes: this function is mainly for test purposes.
//-----------------------------------------------------------------------------
ETgResult	tgFileOpenMem( TgFileHandle *file, void *addr, s64 size, int accessMode, 
						 ETgFileBlockMode blkMode, TgFileCallbackFn* callbackFn )
{
	ETgResult result = TG_FAIL;
	
	TG_ASSERT(_tgFileInitCalled, "tgFileInit has not been called");

	blkMode = _CheckMediumBlocking( file, blkMode ); 

	file->callbackFn = NULL;
	file->kernelCallbackUID = 0;
	if(callbackFn) _RegisterCallback(file, callbackFn, blkMode); 
	result  = _OpenCoreMem( file, addr, size, accessMode, blkMode);

	return result;
}

//-----------------------------------------------------------------------------
// tgFileOpen()
// Description: Opens a file
// Parameters:
//	 file - File handle
//	 fileName - Location of the file
//   accessMode - Whether file is open for read, write etc. see defines:TG_FILE_O_RDONLY etc
//   blkMode - Blocking mode: TG_FILE_NON_BLOCKING, TG_FILE_BLOCKING
//   callbackFn - Optional function called on completion
// Returns:  TG_FAIL, TG_SUCCESS or TG_INDETERMINATE
// Notes: this function is always blocking at the moment.
//-----------------------------------------------------------------------------
ETgResult tgFileOpen( TgFileHandle *file, const char *fileName, int accessMode, 
				ETgFileBlockMode blkMode, TgFileCallbackFn* callbackFn )
{
	ETgResult result = TG_FAIL;
	
	TG_ASSERT(_tgFileInitCalled, "tgFileInit has not been called");

	blkMode = _CheckMediumBlocking( file, blkMode ); 
	file->callbackFn = NULL;
	file->kernelCallbackUID = 0;
	if(callbackFn) _RegisterCallback(file, callbackFn, blkMode); 
	result  = _OpenCore( file, fileName, accessMode, blkMode);

//	if (result == TG_FAIL)
//		TG_OUTPUT_TEXT("TG file open fail %s\n", file->fileName);
//	else
//		TG_OUTPUT_TEXT("TG file open success %s\n", file->fileName);
	

	return result;
}

//-----------------------------------------------------------------------------
// tgFileSize()
// Description: Get the size of the file
// Parameters:
//	 file - File handle
// Returns: Size in bytes
// Notes:
//-----------------------------------------------------------------------------
s64 tgFileGetSize( TgFileHandle  *file)
{
	TG_ASSERT(file, "tgFileSize: file=NULL");

	return file->byteSize;
}

//-----------------------------------------------------------------------------
// tgFileSeek()
// Description: Seeks to a position in the file
// Parameters:
//	 file - File handle
//   byteOffset - Number of bytes to seek
//   whence - Seek start point: TG_FILE_SET,TG_FILE_CUR, TG_FILE_END
//   blkMode - Blocking mode: TG_FILE_NON_BLOCKING, TG_FILE_BLOCKING
// Returns:  TG_FAIL, TG_SUCCESS or TG_INDETERMINATE
// Notes:
//-----------------------------------------------------------------------------
ETgResult tgFileSeek( TgFileHandle *file, s64 byteOffset, ETgFileWhence whence,
				ETgFileBlockMode blkMode )
{
	ETgResult result = TG_FAIL;
	
	TG_ASSERT(file, "tgFileSeek: file=NULL");

	blkMode = _CheckMediumBlocking( file, blkMode ); 
	result  = _SeekCore( file, byteOffset, whence, blkMode);

	return result;
}

//-----------------------------------------------------------------------------
// tgFileRead()
// Description: Reads data from a file into memory
// Parameters:
//	 file - File handle
//   dest - Destination address in memory for the data read in
//   size - Size in bytes to read
//   blkMode - Blocking mode: TG_FILE_NON_BLOCKING, TG_FILE_BLOCKING
// Returns:  TG_FAIL, TG_SUCCESS or TG_INDETERMINATE
// Notes:
//-----------------------------------------------------------------------------
ETgResult tgFileRead( TgFileHandle *file, void *dest, s64 size, ETgFileBlockMode blkMode )
{
	ETgResult result = TG_FAIL;
	
	TG_ASSERT(file, "tgFileRead: file=NULL");

	blkMode = _CheckMediumBlocking( file, blkMode ); 
	result  = _ReadCore( file, dest, file->bytePos, size, blkMode );

	return result;
}

//-----------------------------------------------------------------------------
// tgFileGetPos()
// Description: Get the current file position
// Parameters:
//	 file - File handle
// Returns: Position in bytes from the start of the file
// Notes:
//-----------------------------------------------------------------------------
s64 tgFileGetPos(  TgFileHandle *file)
{
	TG_ASSERT(file, "tgFileGetPos: file=NULL");
	return file->bytePos;
}


//-----------------------------------------------------------------------------
// tgFileReadOffset()
// Description: Reads data from a particle offset in a file into memory
// Parameters:
//	 file - File handle
//   dest - Destination address in memory for the data read in
//   offset - Offset to start reading from
//   size - Size in bytes to read
//   blkMode - Blocking mode: TG_FILE_NON_BLOCKING, TG_FILE_BLOCKING
// Returns:  TG_FAIL, TG_SUCCESS or TG_INDETERMINATE
// Notes: at the moment this function always performs blocking seek if seeking is required
//-----------------------------------------------------------------------------
ETgResult tgFileReadOffset( TgFileHandle *file, void *dest, s64 offset, s64 size, 
					  ETgFileBlockMode blkMode )
{
	ETgResult result = TG_FAIL;
	
	TG_ASSERT(file, "tgFileRead: file=NULL");

	blkMode = _CheckMediumBlocking( file, blkMode ); 
	result  = _ReadCore( file, dest, offset, size, blkMode );

	return result;
}


//-----------------------------------------------------------------------------
// tgFileWrite
// Description: Writes data to a file
// Parameters:
//	 file - File handle
//   source - Source address of the data to write
//   byteCount - Number of bytes to write
//   blkMode - Blocking mode: TG_FILE_NON_BLOCKING, TG_FILE_BLOCKING
// Returns:  TG_FAIL, TG_SUCCESS or TG_INDETERMINATE
// Notes: only blocking writes with EMU or DEVKIT are supported 
//-----------------------------------------------------------------------------
ETgResult tgFileWrite( TgFileHandle *file, void *source, s64 byteCount, 
				 ETgFileBlockMode blkMode )
{
	ETgResult result = TG_FAIL;
	
	TG_ASSERT(file, "tgFileWrite: file=NULL");

	blkMode = _CheckMediumBlocking( file, blkMode ); 
	result  = _WriteCore( file, source, byteCount, blkMode);

	return result;
}


//-----------------------------------------------------------------------------
// tgFileClose()
// Description: Closes an open file
// Parameters:
//	 file - The file handle
// Returns:  TG_FAIL, TG_SUCCESS or TG_INDETERMINATE
// Notes:
//-----------------------------------------------------------------------------
ETgResult tgFileClose( TgFileHandle *file, ETgFileBlockMode blkMode )
{
	ETgResult result = TG_FAIL;
	
	TG_ASSERT(file, "tgFileClose: file=NULL");
	blkMode = _CheckMediumBlocking( file, blkMode ); 

	if (blkMode == TG_FILE_BLOCKING)
	{
		switch( _fileMedium )
		{
	#ifdef _PSPEMU
			case TG_FILE_MEDIUM_EMU:
				if (sceEmuFclose( file->fp)==-1)
					result = TG_FAIL;
				else
					result = TG_SUCCESS;
				break;
	#else
			case TG_FILE_MEDIUM_DEVKIT:
			case TG_FILE_MEDIUM_UMD:
				if (sceIoClose( file->fp)==-1)
					result = TG_FAIL;
				else
					result = TG_SUCCESS;
				break;
	#endif
			case TG_FILE_MEDIUM_MEMORY:
				//Do nothing
				result = TG_SUCCESS;
				break;
				
			default:
				result = TG_FAIL;
				break;
		}
	}
	else
	{
		//non-blocking close
		file->state=TG_FILE_FUNC_CLOSE;
		int ret=sceIoCloseAsync( file->fp);
		if(ret==SCE_KERNEL_ERROR_OK)
			result=TG_SUCCESS;
		else
			_lastError=ret;
	}

	return result;
}


//-----------------------------------------------------------------------------
// tgFileSync()
// Description: Used to check the status or wait for a non blocking command 
//				to finish
// Parameters:
//	 file - File handle
//   blkMode - Blocking mode: TG_FILE_NON_BLOCKING, TG_FILE_BLOCKING
// Returns:  TG_FAIL, TG_SUCCESS or TG_INDETERMINATE
// Notes:
//-----------------------------------------------------------------------------
ETgResult tgFileSync( TgFileHandle *file, ETgFileBlockMode blkMode )
{
	TG_ASSERT(file, "tgFileSync: file=NULL");

	blkMode = _CheckMediumBlocking( file, blkMode ); 
	
	SceIores iores=0;
	int active=-1;
	
	if(blkMode==TG_FILE_NON_BLOCKING)
		active = sceIoPollAsync(file->fp,&iores);
	else
		active = sceIoWaitAsync(file->fp,&iores);

	if(active==1) return TG_INDETERMINATE;

	//operation is complete, get results & update
	ETgResult result=TG_FAIL;
	switch(file->state)
	{
	case TG_FILE_FUNC_CLOSE:
	case TG_FILE_FUNC_OPEN:
		{
			if(active==0) 
				result=TG_SUCCESS;
			else 
				_lastError=active;
		}
		file->state=TG_FILE_FUNC_ERROR;
		break;
	case TG_FILE_FUNC_SEEK:
		{
			if(active==0) 
			{
				if(file->reqOffset==iores)
				{
					file->bytePos=iores;
					file->reqOffset=0;
					result=TG_SUCCESS;
				}
			}
			else 
				_lastError=active;
		}
		file->state=TG_FILE_FUNC_ERROR;
		break;
	case TG_FILE_FUNC_READ:
		{
			if(active==0) 
			{
				if(file->reqOffset==iores)
				{
					file->bytePos+=iores;
					file->reqOffset=0;
					result=TG_SUCCESS;
				}
			}
			else 
				_lastError=active;
		}
		file->state=TG_FILE_FUNC_ERROR;
		break;
	case TG_FILE_FUNC_ERROR:
		TG_WARNING(0, "tgFileSync: nothing to sync");
		_lastError=active;
		TG_WARNING(0, "last error(see kernel.h): %d",_lastError);
		break;
	default:
		break;
	}
	return result;
}


int tgStatFile(const char * fileName, struct SceIoStat * statbuffer)
{
	TgFileHandle file;
	_TranslateFileName(&file, fileName); // Translate filename etc

	return sceIoGetstat(file.fileName , statbuffer);	

}

SceUID tgDopen(	const char *dirname )
{
	TgFileHandle file;
	_TranslateFileName(&file, dirname); // Translate filename etc

	return sceIoDopen(file.fileName);	

}

int tgDclose( SceUID fd )
{
	return sceIoDclose(fd);
}

int tgDread( SceUID fd,	struct SceIoDirent *buf)
{
	return sceIoDread(fd, buf);
}

int tgIoOpen(const char *filename, int flags, int mode){
    TgFileHandle file;


	_TranslateFileName(&file, filename); // Translate filename etc

    return sceIoOpen(file.fileName, flags, mode);
}

int tgIoMkdir(const char *filename){
    TgFileHandle file;


    _TranslateFileName(&file, filename); // Translate filename etc

    return sceIoMkdir(file.fileName, 0x0fff);
}
