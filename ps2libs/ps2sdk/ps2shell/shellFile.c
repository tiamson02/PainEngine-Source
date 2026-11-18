//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// shellFile - medium-independent file access
//
// $Workfile: shellFile.c $
// $Author: Mbreugel $
// $Date: 1/12/03 16:21 $
// $Revision: 55 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "shellCore.h"
#include <libver.h>
#include <malloc.h>

//-----------------------------------------------------------------------------
// M A C R O S ,   C O N S T A N T S
//-----------------------------------------------------------------------------

#define IOP_COPY_CHUNK_SIZE 2048
#define SHARED_MOD_TABLE_SIZE 16


//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

typedef struct _FileInfo
{
	int  lsn;
	int  bytes;
	char nativeName[128];

} FileInfo;

typedef struct _SharedMod
{
	int id;
	int users;
	int result;

} SharedMod;


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

static void _GetIopImagePath( char *path );
static int _GetIopModulePath( const char *modName, char *result );
#ifdef DEBUG
static void _CheckIopName( const char *name );
#endif
static void _NativeName( const char *src, char *dest);

static EShellResult _GetInfo( const char *name, FileInfo *pFileInfo );
static EShellResult _SyncReturn1( EShellFileBlockingMode blkMode );
static EShellResult _SyncReadCore( EShellFileBlockingMode blkMode );
static EShellResult _FileRead( const char *name, EShellFileBlockingMode blkMode, void *eeAddr,
				  int sectorOffset, int sectors );
static EShellResult _FileReadIOPm( const char *name, EShellFileBlockingMode blkMode, 
								int iopAddr, int sectorOffset, int sectors );

static EShellResult _ReadCore( const FileInfo *pFileInfo, EShellFileBlockingMode blkMode, void *eeIopAddr,
					  int offset, int sectors );
static EShellResult _SyncReadIOPm( EShellFileBlockingMode blkMode );

static void _CopyAndAlignMem(void);
static EShellResult _SyncReadChain( EShellFileBlockingMode blkMode );         // New
static EShellResult _SyncReadChainHandle( EShellFileBlockingMode blkMode );   // New


//-----------------------------------------------------------------------------
// S T A T I C   V A R I A B L E S
//-----------------------------------------------------------------------------

static int _shellFileInitCalled = 0;
static const char *_iopModPath = "";
static char *_iopImageFile = IOP_IMAGE_FILE;
static EShellFileMedium _medium;
static int _isLibcdvdMedium = 0;
static SharedMod _sharedModTable[SHARED_MOD_TABLE_SIZE];
static int _file = 0;
static sceCdRMode _fileMode =
{
	0,              // try count (0 = try 256 times)
	SCECdSpinNom,   // spindle control
	SCECdSecS2048,  // data pattern
	0               // padding
};

static sceCdRMode _streamMode =
{
	0,              // try count (0 = try 256 times)
	SCECdSpinStm,   // spindle control
	SCECdSecS2048,  // data pattern
	0               // padding
};
static EShellFileStat _kernelStatus;
static EShellFileError _kernelError;
static EShellResult (*_fileSyncFunc)     ( EShellFileBlockingMode blkMode ) = _SyncReturn1;
static EShellResult (*_realFileSyncFunc) ( EShellFileBlockingMode blkMode );
static unsigned int _loadChainTag[64 * 3];

static char _namesCpy[64][128];
static void *_eeIopAddrsCpy[64];
static int  _sectorOffsetsCpy[64];
static int  _sectorCountsCpy[64];

static int _curTagInd = 0;
static int _amountRead = 0;
static int _iopAddr = 0;
static char _iopCopyBuf[IOP_COPY_CHUNK_SIZE] data_align(64);
static int _fileSizeBytes = 0;
static int _stLsn = 0;
static int _stBufSize = 0;
static int _stLength = 0;
static int _stPos = 0;
static char _kernelName[64];    // "host0:" or "atfile:host[!port],"
static u_int _kernelNameLen = 0;


static char _startBuffer[2048] data_align(64);     // New
static char _endBuffer[2048] data_align(64);       // New

static void *_eeIopDestAddr = NULL;        // new
static int  _eeIopTransferSize = 0;     // new

static int  _startBufferOffset = 0; // new
static int	_startBufferSize   = 0; // new
static int  _numMiddleSectors  = 0; // new
static int	_endBufferSize     = 0; // new

static int  _lastSectorLoaded  = -1;// New
static int 	_reUseSector=0;			//new
static ShellFileHandle* pActiveFileHandle = NULL;

//-----------------------------------------------------------------------------
// S T A T I C   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// _GetIopImagePath()
// Description: obtains the path of the expected location of the IOP image file
// Parameters: path - pointer to string to store result
// Notes: The directory used for the IOP image file is the first one
//        specified in the IOP search path.
//-----------------------------------------------------------------------------
static void _GetIopImagePath( char *path )
{
	char *s, *d;
	
	if(_isLibcdvdMedium)
	{
		strcpy(path, "cdrom0:\\");
		d = &path[8];
		s = (char *) _iopModPath;
		
		while(*s && (*s != ';')) *(d++) = (char) toupper(*(s++));

		// Directory-delimiting slash, only if necessary		
		if(*_iopModPath && (*_iopModPath != ';')) *(d++) = '\\';

		s = _iopImageFile;
		while(*s) *(d++) = (char) toupper(*(s++));
		
		*(d++) = ';';
		*(d++) = '1';
		*d = 0;	
	}
	
	else
	{
		strcpy(path, "host0:");
		d = &path[6];
		s = (char *) _iopModPath;
		
		while(*s && (*s != ';')) *(d++) = *(s++);

		// Directory-delimiting slash, only if necessary		
		if(*_iopModPath && (*_iopModPath != ';')) *(d++) = '/';

		s = _iopImageFile;
		while(*s) *(d++) = *(s++);

		*d = 0;	
	}
}

//-----------------------------------------------------------------------------
// _GetIopModulePath()
// Description: scans through the IOP search path for a filename
// Parameters: modName - filename with no directory, eg. 'libsd.irx'
//             result  - pointer to string to store result
// Returns: 1 = filename found (stored in result), otherwise zero.
//-----------------------------------------------------------------------------
static int _GetIopModulePath( const char *modName, char *result )
{
	int fd;
	char *d;
	char *s = (char *) _iopModPath;
	char *modNameC;
	
	while(1)
	{
		d = result;
	
		if(_isLibcdvdMedium)
		{
			strcpy(d, "cdrom0:\\");
			d += 8;
			
			while(*s && (*s != ';'))
			{
				*(d++) = (char) toupper(*(s++));
				if(*(d-1) == '/') *(d-1) = '\\';
			}

			// Only add the \ if the directory is not empty: the empty
			// string means cdrom0:\<modName>;1, not cdrom0:\\<modName>;1
			if(d != result + 8)
			{
				*(d++) = '\\';
				*d = 0;
			}
		
			modNameC = (char *) modName;
			while(*modNameC) *(d++) = (char) toupper(*(modNameC++));
			
			*(d++) = ';';
			*(d++) = '1';
			*d = 0;
		}

		else
		{
			strcpy(d, _kernelName);
			d += _kernelNameLen;
			
			while(*s && (*s != ';')) *(d++) = *(s++);
			
			*d = 0;
		
			// Only add the / if the directory is not empty: the empty
			// string means host0:<modName>;1, not host0:/<modName>;1
			if(d != result + _kernelNameLen)
			{
				*(d++) = '/';
				*d = 0;
			}

			strcat(d, modName);
		}
		
		if((fd = sceOpen(result, SCE_RDONLY, 0)) >= 0)
		{
			sceClose(fd);
			return 1;
		}
		
		if(!*s++) break;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// _CheckIopName()
// Description: verifies that the parameter is in the correct format for
//              use in a shellFile load IOP module function.
// Notes: only defined for debug builds; is an empty statement otherwise
//-----------------------------------------------------------------------------
#ifdef DEBUG
static void _CheckIopName( const char *name )
{
	int count = 0;
	const char *n = name;
	
	while(*n)
    {
		switch(*(n++))
		{			
		case 'a' ... 'z':
		case 'A' ... 'Z':
		case '0' ... '9':
		case '_':
			break;
			
		default:
			goto fail;
		}
		
		if(++count>8) goto fail;
    }
	
	return;
	
 fail:	
	WARNING(0, "Format of IOP module name '%s' not suitable.", name);
}
#else
#define _CheckIopName(x) ;
#endif

//-----------------------------------------------------------------------------
// _NativeName()
// Description: converts 'src' to a filename native to the medium specified
//              in shellFileInit. Stores result in 'dest'.
//-----------------------------------------------------------------------------
static void _NativeName( const char *src, char *dest )
{
	// "dir/file" (libcdvd) -> "\DIR\FILE;1"
	// "dir/file" (kernel)  -> "host0:dir/file"
	//                          or "atfile:host[!port],dir/file"
	
	if(_isLibcdvdMedium)
	{
		char *d = dest;
		
		*(d++) = '\\';
		do
		{
			*d = (char) toupper(*(src++));
			if(*d=='/') *d = '\\';
		}
		while(*(d++));
		
		strcat(dest, ";1");
	}

	else  // Kernel filename
	{
		strcpy(dest, _kernelName);
		strcpy(dest + _kernelNameLen, src);
	}
}

//-----------------------------------------------------------------------------
// _GetInfo()
// Description: fills in a fileInfo structure
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
// Notes: Result contains the filename converted to a medium-native format,
// and if the target is CD, the sector number and filesize are read from the
// TOC.
//-----------------------------------------------------------------------------
static EShellResult _GetInfo( const char *name, FileInfo *pFileInfo )
{
	_NativeName(name, pFileInfo->nativeName);

	if(_isLibcdvdMedium)
    {      
		sceCdlFILE fp;
		
		if(!sceCdSearchFile(&fp, pFileInfo->nativeName))
		{
			WARNING(0, "sceCdSearchFile failed for '%s'",
					pFileInfo->nativeName);
			return SHELL_FAIL;
		}
		
		pFileInfo->lsn   = (int) fp.lsn;
		pFileInfo->bytes = (int) fp.size;
    }
    
    return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// _FileRead()
// Description: read from a file (gets info from specified name)
//              Checks for non-blocking read already in progress must have been
//              done before calling this function.
// Parameters:
//   name           - filename in shellFile format
//   blkMode        - 0 = non-blocking
//                    1 = blocking
//   eeAddr         - target EE address
//   sectorOffset   - offset within file to start reading, in 2048-byte units
//   sectors        - amount of data to read, in 2048-byte units(-1=whole file)
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
// Notes: SPR cannot be the destination buffer
//-----------------------------------------------------------------------------
static EShellResult _FileRead( const char *name, EShellFileBlockingMode blkMode, void *eeAddr,
				  int sectorOffset, int sectors )
{
	FileInfo fileInfo;
	
	if(!_GetInfo(name, &fileInfo)) return SHELL_FAIL;
	return _ReadCore(&fileInfo, blkMode, eeAddr, sectorOffset, sectors)?SHELL_SUCCESS:SHELL_FAIL;
}


//-----------------------------------------------------------------------------
// _FileReadIOPm()
// Description: read from a file to IOP memory (gets info from specified name)
//              Checks for non-blocking read already in progress must have been
//              done before calling this function.
// Parameters:
//   name           - filename in shellFile format
//   blkMode        - 0 = non-blocking
//                    1 = blocking
//   iopAddr        - Target IOP address
//   sectorOffset   - offset within file to start reading, in 2048-byte units
//   sectors        - amount of data to read, in 2048-byte units(-1=whole file)
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
static EShellResult _FileReadIOPm( const char *name, EShellFileBlockingMode blkMode, 
								int iopAddr, int sectorOffset, int sectors )
{
	FileInfo fileInfo;
	
	if(_isLibcdvdMedium)
    {
		if(! _GetInfo(name, &fileInfo)) return SHELL_FAIL;

		if(sectors==-1) 
		{
			sectors = BYTES_TO_SECTORS(fileInfo.bytes); // whole file
			sectorOffset = 0;
		}
		
		if(!sceCdReadIOPm((u_int) fileInfo.lsn + sectorOffset, (u_int) sectors,
						  (void *) iopAddr, &_fileMode)) return SHELL_FAIL;
				
		if(blkMode) sceCdSync(0x00);
		
		else _fileSyncFunc = _SyncReadIOPm;
		
		return SHELL_SUCCESS;
    }
	
	else
    {
		if(!_GetInfo(name, &fileInfo)) return SHELL_FAIL;
		
		if((_file = sceOpen(fileInfo.nativeName, SCE_RDONLY)) < 0)
		{
			WARNING(0, "sceOpen failed (returned %d) for '%s'",
					_file, fileInfo.nativeName);
			return SHELL_FAIL;
		}
		
		if(sectors==-1)
		{
			sectors = BYTES_TO_SECTORS(sceLseek(_file, 0, SCE_SEEK_END));
			sectorOffset = 0;
		}

		_fileSizeBytes = sectors*2048; //_fileSizeBytes = sceLseek(_file, 0, SCE_SEEK_END);
		
		sceLseek(_file, sectorOffset*2048, SCE_SEEK_SET);
		
		if(sceRead(_file, _iopCopyBuf, IOP_COPY_CHUNK_SIZE) < 0)
		{
			WARNING(0, "sceRead error occurred in '%s'",
					fileInfo.nativeName);
			sceClose(_file);
			return SHELL_FAIL;
		}
		
		_fileSyncFunc = _SyncReadIOPm; // This is OK to use for shellFileReadIOPm
		
		_iopAddr = iopAddr;
		_amountRead = 0;
		
		if(blkMode) return shellFileSync(SHELLFILE_BLOCKING);
		
		return SHELL_SUCCESS;
    }
}


//-----------------------------------------------------------------------------
// _SyncReturn1()
// Description: dummy function substituted to _fileSyncFunc to always report
//              that a shellFile operation has completed.
// Returns: SHELL_SUCCESS
//-----------------------------------------------------------------------------
static EShellResult _SyncReturn1( EShellFileBlockingMode blkMode )
{
	// No operation is known to be in progress, hence vacuously we've completed
	(void) blkMode;    // To prevent unused parameter warnings
	return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// _SyncReadCore()
// Description: performs shellFileSync when the command to be synced was
//              (perhaps indirectly) _ReadCore
// Returns:  SHELL_INDETERMINATE = not completed
//           SHELL_FAIL          = error occurred
//           SHELL_SUCCESS       = completed
//-----------------------------------------------------------------------------
static EShellResult _SyncReadCore( EShellFileBlockingMode blkMode )
{
	int stillExecuting;
	
	if(_isLibcdvdMedium)
    {
		if(blkMode)
		{
			sceCdSync(0x00);
			stillExecuting = 0;
		}		
		else stillExecuting = (sceCdSync(0x01) != 0);
		
		if(stillExecuting) return SHELL_INDETERMINATE;    
		else
		{
			_fileSyncFunc = _SyncReturn1;
			return SHELL_SUCCESS;
		}
    }
	
	else
    {
		do sceIoctl(_file, SCE_FS_EXECUTING, &stillExecuting);
		while(stillExecuting && blkMode);
		
		if(stillExecuting) return SHELL_INDETERMINATE;
		else
		{
			sceClose(_file);
			_fileSyncFunc = _SyncReturn1;
			_kernelStatus = SHELLFILE_STAT_PAUSE;
			_kernelError = SHELLFILE_ER_NO;
			return SHELL_SUCCESS;
		}     
    }
}


//-----------------------------------------------------------------------------
// _SyncWrite()
// Description: performs shellFileSync when the command to be synced was
//              shellFileHandleWrite
// Returns:  SHELL_INDETERMINATE = not completed
//           SHELL_FAIL          = error occurred
//           SHELL_SUCCESS       = completed
//-----------------------------------------------------------------------------
static EShellResult _SyncWrite( EShellFileBlockingMode blkMode )
{
	int stillExecuting;
	
	if(_isLibcdvdMedium) return SHELL_FAIL;
	else
    {
		do sceIoctl(_file, SCE_FS_EXECUTING, &stillExecuting);
		while(stillExecuting && blkMode);
		
		if(stillExecuting) return SHELL_INDETERMINATE;
		else
		{
			sceClose(_file);
			_fileSyncFunc = _SyncReturn1;
			_kernelStatus = SHELLFILE_STAT_PAUSE;
			_kernelError = SHELLFILE_ER_NO;
			return SHELL_SUCCESS;
		}     
    }
}


//-----------------------------------------------------------------------------
// _ReadCore()
// Description: loads a file from the target medium
// Parameters:
//   fileInfo  - location of file
//   blkMode   - 0 = non-blocking
//               1 = blocking
//   eeIopAddr - pointer to memory buffer to store file data.
//   offset    - starting point in file from which to load, in units of
//               2048 bytes
//   sectors   - number of 2048-byte sectors to read.
//               size == -1 means read the whole file
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = completed
//-----------------------------------------------------------------------------
static EShellResult _ReadCore( const FileInfo *pFileInfo, EShellFileBlockingMode blkMode, 
								void *eeIopAddr, int offset, int sectors )
{		
	if(_isLibcdvdMedium)
    {
		if(sectors==-1) sectors = BYTES_TO_SECTORS(pFileInfo->bytes);
		
		if(!sceCdRead((u_int) (pFileInfo->lsn+offset),(u_int) sectors, eeIopAddr, &_fileMode))
		{
			WARNING(0, "sceCdRead failed in '%s'", pFileInfo->nativeName);
			return SHELL_FAIL;
		}
		
		if(blkMode) sceCdSync(0x00);
		else _fileSyncFunc = _SyncReadCore;
		
		return SHELL_SUCCESS;
    }
	
	else  
    {
		if(blkMode)
		{
			if((_file = sceOpen(pFileInfo->nativeName, SCE_RDONLY)) < 0)
			{
				WARNING(0, "sceOpen failed (returned %d) for '%s'",
						_file, pFileInfo->nativeName);
				_kernelStatus = SHELLFILE_STAT_EMG;
				return SHELL_FAIL;
			}
			
			if(sectors==-1)
				sectors = BYTES_TO_SECTORS(sceLseek(_file, 0, SCE_SEEK_END));
			
			sceLseek(_file, offset*2048, SCE_SEEK_SET);
		}
		
		else
		{
			int stillExecuting;
			int err;
			
			if(sectors==-1) // Calc file size
			{
				if((_file = sceOpen(pFileInfo->nativeName, SCE_RDONLY)) < 0)
				{
					_kernelStatus = SHELLFILE_STAT_EMG;
					WARNING(0, "sceOpen failed (returned %d) for '%s'",
							_file, pFileInfo->nativeName);
					return SHELL_FAIL;
				}
				
				sectors = BYTES_TO_SECTORS(sceLseek(_file, 0, SCE_SEEK_END));
				sceClose(_file);
			}	

			if((_file = sceOpen(pFileInfo->nativeName,
								SCE_RDONLY|SCE_NOWAIT)) < 0)
			{
				_kernelStatus = SHELLFILE_STAT_EMG;
				WARNING(0, "sceOpen failed (returned %d) for '%s'",
						_file, pFileInfo->nativeName);
				return SHELL_FAIL;
			}

			do
			{
				if((err = sceIoctl(_file, SCE_FS_EXECUTING, &stillExecuting))<0)
				{
					WARNING(0, "sceOpen failed (returned %d) for '%s'",
						err, pFileInfo->nativeName);
					return SHELL_FAIL;
				}
			}			
			while(stillExecuting);

			

			if(offset) sceLseek(_file, offset*2048, SCE_SEEK_SET);


			do
			{
				if((err = sceIoctl(_file, SCE_FS_EXECUTING, &stillExecuting))<0)
				{
					WARNING(0, "sceLseek failed (returned %d) for '%s'",
						err, pFileInfo->nativeName);
					return SHELL_FAIL;
				}
			}			
			while(stillExecuting);


		}

		// We now have an opened file, _file, seeked to the right place

		if (sceRead(_file, eeIopAddr, sectors*2048) <= 0)
		{
			_kernelStatus = SHELLFILE_STAT_EMG;
			WARNING(0, "sceRead failed for '%s'", pFileInfo->nativeName);
			sceClose(_file);
			return SHELL_FAIL;
		}

		if(blkMode)
		{
			_fileSyncFunc = _SyncReturn1;
			sceClose(_file);
			return SHELL_SUCCESS;
		}

		else
		{			
			_fileSyncFunc = _SyncReadCore;
			_kernelStatus = SHELLFILE_STAT_READ;
			_kernelError = SHELLFILE_ER_NORDY;
			return SHELL_SUCCESS;
		}
    }
}

//-----------------------------------------------------------------------------
// _CopyToIopMem()
// Description: Copies memory from ee to iop
//-----------------------------------------------------------------------------
static void _CopyToIopMem(void *dest, void *src, int size)
{
	sceSifDmaData toIopDMA;
	int queID;

	toIopDMA.data = (unsigned int)src;
	toIopDMA.addr = (unsigned int)(dest)&0xfffffffe;
	toIopDMA.size = (unsigned int)size;
	toIopDMA.mode = 0;

	FlushCache(0);

	while((queID = (int) sceSifSetDma(&toIopDMA, 1)) == 0) {}
	while(sceSifDmaStat((unsigned int)queID) >= 0) {}
}

//-----------------------------------------------------------------------------
// _SyncCdSync()
// Description: performs shellFileSync when medium is _isLibcdvdMedium
// Returns:  SHELL_INDETERMINATE = not completed
//           SHELL_FAIL          = error occurred
//           SHELL_SUCCESS       = completed
//-----------------------------------------------------------------------------
static EShellResult _SyncCdSync( EShellFileBlockingMode blkMode )
{
	int stillExecuting;
	
	if(blkMode)
	{
		sceCdSync(0x00);
		stillExecuting = 0;
	}
	else stillExecuting = (sceCdSync(0x01) != 0);
	
	if(stillExecuting) return SHELL_INDETERMINATE;    
	else
	{
		_fileSyncFunc = _SyncReturn1;
		if(SHELLFILE_ER_NO==shellFileGetError()) return SHELL_SUCCESS;
		else return SHELL_FAIL;
	}
}

//-----------------------------------------------------------------------------
// _SyncReadIOPm()
// Description: performs shellFileSync when the command to be synced was 
//              (perhaps indirectly) _FileReadIOPm
// Returns:  SHELL_INDETERMINATE = not completed
//           SHELL_FAIL          = error occurred
//           SHELL_SUCCESS       = completed
//-----------------------------------------------------------------------------
static EShellResult _SyncReadIOPm( EShellFileBlockingMode blkMode )
{
	int stillExecuting;
	
	if(_isLibcdvdMedium) return _SyncCdSync(blkMode);
	else
    {	
		do
		{
			do
			{
				sceIoctl(_file, SCE_FS_EXECUTING, &stillExecuting);
			} while(stillExecuting && blkMode);
			
			if(stillExecuting) return SHELL_INDETERMINATE;

			_CopyToIopMem((char*)_iopAddr + _amountRead, _iopCopyBuf, IOP_COPY_CHUNK_SIZE);
						
			_amountRead += IOP_COPY_CHUNK_SIZE;
			if(_amountRead >= _fileSizeBytes)
			{
				sceClose(_file);
				_fileSyncFunc = _SyncReturn1;
				_kernelStatus = SHELLFILE_STAT_PAUSE;
				return SHELL_SUCCESS;
			}
			else
			{
				// Queue up the next chunk
				if(sceRead(_file, _iopCopyBuf, IOP_COPY_CHUNK_SIZE) < 0)
				{
					WARNING(0, "sceRead error occurred in '%s'", __FUNCTION__);
					sceClose(_file);
					return SHELL_FAIL;
				}
			}
		} while(blkMode);
		
		return SHELL_INDETERMINATE;
    }
}


//-----------------------------------------------------------------------------
// _SyncReadChain()
// Description: performs shellFileSync when the command to be synced was
//              shellFileReadChain or shellFileLoadChain
// Returns:  SHELL_INDETERMINATE = not completed
//           SHELL_FAIL          = error occurred
//           SHELL_SUCCESS       = completed
//-----------------------------------------------------------------------------
static EShellResult _SyncReadChain( EShellFileBlockingMode blkMode )
{
	if(_isLibcdvdMedium) return _SyncCdSync(blkMode);

	else // Sync of host0 needs to kick off next file read
    {
		do
		{
			EShellResult r=_realFileSyncFunc(blkMode); // sync
			if(r!=SHELL_SUCCESS) return r; // return if not yet complete
			
			_curTagInd++; // get next file
			
			if(_namesCpy[_curTagInd][0]==0) // return if end of list
			{
				_kernelError = SHELLFILE_ER_NO;
				_kernelStatus = SHELLFILE_STAT_PAUSE;
				_fileSyncFunc = _SyncReturn1;
				return SHELL_SUCCESS;
			}
			
			if((int) _eeIopAddrsCpy[_curTagInd] & 1) // iop mem file load
			{
				if(!_FileReadIOPm(_namesCpy[_curTagInd], blkMode,
									 (int) _eeIopAddrsCpy[_curTagInd] & ~1,
									 _sectorOffsetsCpy[_curTagInd],
									 _sectorCountsCpy[_curTagInd]))
				{
					_fileSyncFunc = _SyncReturn1;
					_kernelError = SHELLFILE_ER_READ;
					_kernelStatus = SHELLFILE_STAT_PAUSE;
					return SHELL_FAIL;
				}
			}
			
			else // ee mem file load
			{
				if(!_FileRead(_namesCpy[_curTagInd], blkMode,
								 (char*) _eeIopAddrsCpy[_curTagInd],// 0, -1))
									 _sectorOffsetsCpy[_curTagInd],
									 _sectorCountsCpy[_curTagInd]))
				{
					_fileSyncFunc = _SyncReturn1;
					_kernelError = SHELLFILE_ER_READ;
					_kernelStatus = SHELLFILE_STAT_PAUSE;
					return SHELL_FAIL;
				}
			}
			
			// Reinstate sync function after the earlier shellFileReadIOPm or
			// shellFileRead would have altered it.
			_realFileSyncFunc = _fileSyncFunc;
			_fileSyncFunc = _SyncReadChain;
		}
		while(blkMode);
		
		return SHELL_INDETERMINATE;
    }
}

//-----------------------------------------------------------------------------
// _CopyAndAlignMem()
// Description: Performs alignment of non sector sized reads at completion of
//              shellFileHandleRead by copying regions from start and end
//              sector buffers. Also updates current file position
//-----------------------------------------------------------------------------
static void _CopyAndAlignMem(void)
{
	char *destAddr = _eeIopDestAddr;
	char *destAddr2;
	
	destAddr2 = ((char*)_eeIopDestAddr) + _eeIopTransferSize - _endBufferSize;

	// Perform end of chain tidy up to handle non sector sized reads
	if ((u_int)_eeIopDestAddr & 0x1)
	{
		if ((_startBufferSize>0) && (_reUseSector==0)) // If sector was reused then this was already copied 
			_CopyToIopMem(destAddr, ((unsigned char *)_startBuffer) + _startBufferOffset, _startBufferSize);

		if (_endBufferSize>0)
			_CopyToIopMem(destAddr2, _endBuffer, _endBufferSize);
	}
	else
	{
		if ((_startBufferSize>0) && (_reUseSector==0)) // If sector was reused then this was already copied
			memcpy(destAddr, ((unsigned char*)_startBuffer) + _startBufferOffset, (unsigned int)_startBufferSize);

		if (_endBufferSize>0)
			memcpy(destAddr2, _endBuffer, (unsigned int)_endBufferSize);			
	}
	
	
	// Allow transfers <2048 bytes to be cached
	if ((_endBufferSize==0)&&(_lastSectorLoaded!=-1)) memcpy(_endBuffer, _startBuffer, 2048);
	
	
	// Update position
	pActiveFileHandle->bytePos+= (_startBufferSize + _endBufferSize + _numMiddleSectors*2048);
	
}


//-----------------------------------------------------------------------------
// _SyncReadChainHandle()
// Description: performs shellFileSync when the command to be synced was
//              shellHandleRead (non sector aligned read function)
//              At completion this function does memcpys to align start and end
//              as required
// Returns:  SHELL_INDETERMINATE = not completed
//           SHELL_FAIL          = error occurred
//           SHELL_SUCCESS       = completed
//-----------------------------------------------------------------------------
static EShellResult _SyncReadChainHandle( EShellFileBlockingMode blkMode )
{
	if(_isLibcdvdMedium) // Sync of CD syncs until whole chain is transfered
    {
		int stillExecuting;
		
		if(blkMode)
		{
			sceCdSync(0x00); // block till complete
			stillExecuting = 0;
		}	
		else stillExecuting = (sceCdSync(0x01) != 0);
		
		if(stillExecuting) return SHELL_INDETERMINATE;
		else
		{				
			// Perform end of chain tidy up to handle non sector sized reads
			_CopyAndAlignMem();

			_fileSyncFunc = _SyncReturn1;
			return SHELL_SUCCESS;
		}
    }  
	
	else // Sync for host0 needs to kick off next file read
    {
		do
		{
			EShellResult r=_realFileSyncFunc(blkMode); // sync
			if(r!=SHELL_SUCCESS) return r; // return if not yet complete
			
			_curTagInd++; // next file
			
			if(_namesCpy[_curTagInd][0]==0) // return if end of list
			{
				_kernelError = SHELLFILE_ER_NO;
				_kernelStatus = SHELLFILE_STAT_PAUSE;
				_fileSyncFunc = _SyncReturn1;

				// Perform end of chain tidy up to handle non sector sized reads
				_CopyAndAlignMem();

				return SHELL_SUCCESS;
			}
			
			if((int) _eeIopAddrsCpy[_curTagInd] & 1) // iop mem file load
			{
				if(!_FileReadIOPm(_namesCpy[_curTagInd], blkMode,
									 (int) _eeIopAddrsCpy[_curTagInd] & ~1,
									 _sectorOffsetsCpy[_curTagInd],
									 _sectorCountsCpy[_curTagInd]))
				{
					_fileSyncFunc = _SyncReturn1;
					_kernelError = SHELLFILE_ER_READ;
					_kernelStatus = SHELLFILE_STAT_PAUSE;
					return SHELL_FAIL;
				}
			}
			
			else // ee mem file load
			{
				if(!_FileRead(_namesCpy[_curTagInd], blkMode,
								 (char*) _eeIopAddrsCpy[_curTagInd],// 0, -1))
									 _sectorOffsetsCpy[_curTagInd],
									 _sectorCountsCpy[_curTagInd]))
				{
					_fileSyncFunc = _SyncReturn1;
					_kernelError = SHELLFILE_ER_READ;
					_kernelStatus = SHELLFILE_STAT_PAUSE;
					return SHELL_FAIL;
				}
			}
			
			// Reinstate sync function after the earlier shellFileReadIOPm or
			// shellFileRead would have altered it.
			
			_realFileSyncFunc = _fileSyncFunc;
			_fileSyncFunc = _SyncReadChainHandle; 
		}
		while(blkMode);
		
		return SHELL_INDETERMINATE;
    }
}


//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// shellFileSetIOPImage()
// Description: sets the name of the IOP image used to reboot 
// Parameters:
//   image - The name of IOP image (eg. dnas300.img)
// Notes: Call this function before calling shellFileInit if the IOP is to be rebooted
// 	  with given image. The default image used is IOP_IMAGE_NAME
//-----------------------------------------------------------------------------
void shellFileSetIOPImage(char *image) {

  if ( image != NULL )
    _iopImageFile = image;

  return;
}

//-----------------------------------------------------------------------------
// shellFileInit()
// Description: initialises shellFile module
// Parameters:
//   medium        - SHELLFILE_MEDIUM_HOST0    = host0
//                   SHELLFILE_MEDIUM_CD       = CD
//                   SHELLFILE_MEDIUM_DVD      = DVD
//                   SHELLFILE_MEDIUM_SCEEDEMO = autoselect CD or DVD, only for
//                                               SCEE-region demo segments
//                   SHELLFILE_MEDIUM_ATMON    = atmon
//   rebootIop     - 0 = don't reboot IOP
//                 - 1 = reboot IOP
//   iopModPath    - string of directories to search for IOP modules,
//                   separated by semicolons
//   medParam      - parameter specific to choice of medium
//
// Notes: If the IOP is to be rebooted, the expected location of the image file
//        is within the first directory specified in iopModPath
//
//        When medium is SHELLFILE_MEDIUM_ATMON, the medParam parameter should
//        be a string pointer which specifies the location of the remote file
//        server. The string should take the format "atfile:host[!port],",
//        where host must be an IP address and port is an optional port number.
//
//        eg. "atfile:192.168.0.19," 
//            "atfile:192.168.0.19!12345," 
//
//        This format was chosen to allow the argv[0] parameter, as passed to
//        main() when compiled with the atmon crt0.s, to be passed in directly.
//
//        For all other medium types, medParam is undefined.
//-----------------------------------------------------------------------------
void shellFileInit( EShellFileMedium medium, int rebootIop,
					const char *iopModPath, void *medParam )
{  
	int n;
	
	_medium = medium;

	switch(medium)
	{
		case SHELLFILE_MEDIUM_HOST0:
			strcpy(_kernelName, "host0:");
			_kernelNameLen = strlen(_kernelName);
			_isLibcdvdMedium = 0;
			break;

		case SHELLFILE_MEDIUM_ATMON:
			ASSERT(medParam, "Null pointer argument %s", "medParam");

			for(n = 0; ((char *) medParam)[n] != ','; n++)
			{
				ASSERT(((char *) medParam)[n], "medParam string missing comma");
				_kernelName[n] = ((char *) medParam)[n];
			}

			_kernelName[n++] = ',';
			_kernelName[n++] = 0;
			_kernelNameLen = strlen(_kernelName);

			_isLibcdvdMedium = 0;
			break;

		default:
			_isLibcdvdMedium = 1;
	}

	ASSERT(iopModPath, "Null pointer for IOP search path");
	_iopModPath = iopModPath;

	_kernelStatus = SHELLFILE_STAT_PAUSE;
	_kernelError = SHELLFILE_ER_NO;
	
	for(n=0; n<SHARED_MOD_TABLE_SIZE; n++)
	{
		_sharedModTable[n].id = -1;
		_sharedModTable[n].users = 0;
	}	

	if(rebootIop)
	{
		char iopImgPath[128];
	
		if(_isLibcdvdMedium) sceCdInit(SCECdINIT);

		_GetIopImagePath(iopImgPath);
		while(!sceSifRebootIop(iopImgPath))
			;
		while(!sceSifSyncIop())
			;
	}

	sceSifInitRpc(0);
	sceSifLoadFileReset();
	sceFsReset();	

	if(_isLibcdvdMedium)
    {
    	int m;
    	
    	if(_medium == SHELLFILE_MEDIUM_CD) m = SCECdCD;
    	else if(_medium == SHELLFILE_MEDIUM_DVD) m = SCECdDVD;
    	else
    	{
    		WARNING(0, "SHELLFILE_MEDIUM_SCEEDEMO selected. This option "
    			"should not be used except for within SCEE-region demo segments");
    	
    		m = (sceCdGetDiskType() == SCECdPS2CD) ? SCECdCD : SCECdDVD;
    	}
    
		sceCdInit(SCECdINIT);
		sceCdMmode(m);
    }
	
	_shellFileInitCalled = 1;
}

//-----------------------------------------------------------------------------
// shellFileNativeName()
// Description: converts from shellFile name format to kernel I/O format.
// Parameters:
//   src   - source filename
//   dest  - buffer to hold destination filename
//-----------------------------------------------------------------------------
void shellFileNativeName( const char *src, char *dest )
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(src, "Null pointer argument %s", "src");
	ASSERT(dest, "Null pointer argument %s", "dest");

	if(_isLibcdvdMedium)
	{
		strcpy(dest, "cdrom0:");
		_NativeName(src, dest + 7);
	}
	else _NativeName(src, dest);
}

//-----------------------------------------------------------------------------
// shellFileLoadStartModule()
// Description: loads and starts an IOP module unless already started
// Parameters:
//   name    - name of IOP module, stripped of all path and suffix,
//             eg. "sio2man"
//   args    - the args parameter (refer to documentation of sceSifLoadModule)
//   argp    - the argp parameter (refer to documentation of sceSifLoadModule)
//   result  - a pointer to a variable for storing the value returned by the
//             module's initialisation routine. Can be null.
// Returns: module ID
// Notes: If the module was already loaded, 'result' will return a copy of the
//        result that was obtained when the module was first started.
//-----------------------------------------------------------------------------
int shellFileLoadStartModule(const char *name, int args, const char *argp,
							int *pResult )
{
	char modifiedModName[128];
	const char *s;
	char *d;
	int ind = 0;
	int id;

	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(name, "Null pointer argument %s", "name");

	if (shellFileSync(SHELLFILE_NON_BLOCKING)!=SHELL_SUCCESS)
		WARNING(0, "Sync needed for non-blocking function in progress");

	_CheckIopName(name);
	
	for(s=name, d=modifiedModName; *s; s++, d++) *d = (char) tolower(*s);

	*d = 0;
	
	id = sceSifSearchModuleByName(modifiedModName);
		
	if(id<0)  // Module not already loaded
	{
		char fullModPath[128];
	
		for(ind=0; ind<SHARED_MOD_TABLE_SIZE; ind++)
			if(_sharedModTable[ind].id == -1) break;
		
		ASSERT(ind<SHARED_MOD_TABLE_SIZE, "No more entries available in "
			   "shared IOP module table");

		strcpy(modifiedModName, name);
		strcat(modifiedModName, ".irx");

		if(_GetIopModulePath(modifiedModName, fullModPath))
		{	
			if((id = sceSifLoadStartModule(fullModPath, args, argp,
										   &_sharedModTable[ind].result)) < 0)
			{
				WARNING(0, "Couldn't load IOP module %s", fullModPath);
				return id;
			}
			
			PRINT("Loaded IOP module %s", fullModPath);
			
			_sharedModTable[ind].id = id;
			_sharedModTable[ind].users = 1;
		}
			
		else WARNING(0, "Couldn't find IOP module %s.irx", name);
	}

	else    // Module already loaded
	{
		for(ind=0; ind<SHARED_MOD_TABLE_SIZE; ind++)
		{
			if(_sharedModTable[ind].id == id)
			{
				_sharedModTable[ind].users++;
				break;
			}
		}
		
		// Module was loaded outside of shellFile. This shouldn't ordinarily
		// happen unless running under atmon which leaves a number of
		// modules installed (sio2man, padman, mcman, etc.)
		
		WARNING(ind<SHARED_MOD_TABLE_SIZE,
				"Module %s already loaded outside of shellFile", name);

		// Create a new entry in the shared mod table for it.

		for(ind=0; ind<SHARED_MOD_TABLE_SIZE; ind++)
			if(_sharedModTable[ind].id == -1) break;
		
		ASSERT(ind<SHARED_MOD_TABLE_SIZE, "No more entries available in "
			   "shared IOP module table");

		_sharedModTable[ind].id = id;
		_sharedModTable[ind].users = 1;
	}
	
	if(pResult) *pResult = _sharedModTable[ind].result;
	return id;
}

//-----------------------------------------------------------------------------
// shellFileStopUnloadModule()
// Description: stops and unloads an IOP module unless used elsewhere
// Parameters:
//   id      - module ID returned by shellFileLoadStartModule.
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
// Notes: passing args and argp and obtaining a result from closing the module
//        is not currently supported as these lose usefulness in a shared
//        module system.
//-----------------------------------------------------------------------------
EShellResult shellFileStopUnloadModule( int id )
{
	int ind;
	int ret;
	int args = 0;
	const char *argp = "";
	int res;

	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");

	for(ind=0; ind<SHARED_MOD_TABLE_SIZE; ind++)
	{
		if((_sharedModTable[ind].id == id) && _sharedModTable[ind].users > 0)
		{
			if(--_sharedModTable[ind].users == 0)
			{
				ret = sceSifStopModule(id, args, argp, &res);
				
				if(ret<0) return SHELL_FAIL;

				_sharedModTable[ind].id = -1;

				if((ret=sceSifUnloadModule(id)) < 0)
				{
					WARNING(0, "Couldn't unload stopped module %d (%d)",
							id, ret);
					return SHELL_FAIL;
				}
			}
			
			break;
		}
	}
	
	ASSERT(ind<SHARED_MOD_TABLE_SIZE, "Module not known to shared IOP "
		   "module table");

	return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileDiskReady()
// Description: checks drive status
// Parameters:
//   blkMode - 0 = non-blocking
//             1 = blocking
// Returns:  0       = drive not ready
//           nonzero = drive not ready
//-----------------------------------------------------------------------------
int shellFileDiskReady( EShellFileBlockingMode blkMode )
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium)
	{
		if(blkMode==SHELLFILE_NON_BLOCKING) _fileSyncFunc = _SyncCdSync;
		return sceCdDiskReady(blkMode ? 0 : 1) == SCECdComplete;
	}
	else return 1;
}

//-----------------------------------------------------------------------------
// shellFileGetError()
// Description: determines cause of last error
// Returns:  SHELLFILE_ER_FAIL    = shellFileGetError function failed 
//           SHELLFILE_ER_NO      = No error
//           SHELLFILE_ER_EOM     = Outermost track reached during playback
//           SHELLFILE_ER_TRMOPN  = Cover opened during playback
//           SHELLFILE_ER_READ    = Problem occurred during read
//           SHELLFILE_ER_CUD     = Not appropriate for disc in drive
//           SHELLFILE_ER_NORDY   = Processing command
//           SHELLFILE_ER_ABRT    = Abort command received
//-----------------------------------------------------------------------------
EShellFileError shellFileGetError( void )
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	
	return _isLibcdvdMedium ? (EShellFileError) sceCdGetError() : _kernelError;
}

//-----------------------------------------------------------------------------
// shellFileGetSize()
// Description: determines size of file
// Parameters:
//   name - filename in shellFile format
// Returns: size of file in bytes
//-----------------------------------------------------------------------------
unsigned int shellFileGetSize( const char *name )
{
	FileInfo fileInfo;
	
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(!_GetInfo(name, &fileInfo)) return 0;
	
	if(_isLibcdvdMedium) return (unsigned int) fileInfo.bytes;
	else
    {
		unsigned int size;
		
		if((_file = sceOpen(fileInfo.nativeName, SCE_RDONLY)) < 0) return 0;
		
		size = (unsigned int) sceLseek(_file, 0, SCE_SEEK_END);      
		sceClose(_file);
		
		return size;
    }
}

//-----------------------------------------------------------------------------
// shellFileRead()
// Description: read from a file
// Parameters:
//   name           - filename in shellFile format
//   blkMode        - 0 = non-blocking
//                    1 = blocking
//   eeAddr         - target EE address
//   sectorOffset   - offset within file to start reading, in 2048-byte units
//   sectors        - amount of data to read, in 2048-byte units
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
// Notes: SPR cannot be the destination buffer
//-----------------------------------------------------------------------------
EShellResult shellFileRead( const char *name, EShellFileBlockingMode blkMode, void *eeAddr,
				  int sectorOffset, int sectors )
{	
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(name, "Null pointer argument %s", "name");
	ASSERT(eeAddr, "Null pointer argument %s", "eeAddr");
	
	WARNING(!((int) eeAddr%64),
			"Load address is not 64-byte aligned; performance may suffer");
	
	if (shellFileSync(SHELLFILE_NON_BLOCKING)!=SHELL_SUCCESS)
		WARNING(0, "Sync needed for non-blocking function in progress");

	return _FileRead(name, blkMode, eeAddr, sectorOffset, sectors);
}


//-----------------------------------------------------------------------------
// shellFileLoad()
// Description: reads a file into memory
// Parameters:
//   name      - name of the file
//   alignment - how to align the memory allocated for the file data
//   length    - pointer to a variable which will receive the file length
// Returns: pointer to the memory allocated for the file
//-----------------------------------------------------------------------------
void *shellFileLoad( const char *name, unsigned int alignment, unsigned int *length)
{
	unsigned int len;
	unsigned int blocks;
	void *data;

	data=0;
	len=(unsigned int)shellFileGetSize(name);
	if(length) *length=len;
	if(len)
	{
		blocks=(len+2047)>>11;

		// TG hack until we're using the shell memory allocator
		// shell memory not available in this build, so we just use
		// malloc, and we don't get data aligned to the apps' spec,
		// though we do get aligned data
		data=memalign (alignment, blocks<<11);
		if(data)
		{
			if(shellFileRead(name, 
							SHELLFILE_BLOCKING,
							data,
							0,
							(int) blocks)==SHELL_FAIL)
				data=0;
		}
	}
	return data;
}

//-----------------------------------------------------------------------------
// shellFileGetLoadPos()
// Description: returns amount of file read
// Returns: file read position in 2048-byte units, or zero if file loading
//          is complete
//-----------------------------------------------------------------------------
int shellFileGetLoadPos( void )
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	
	// Non-committal, ambiguous, meaningless answer. (Could cause problems)
	if(_medium == SHELLFILE_MEDIUM_HOST0) return 0;	
	else return (int) sceCdGetReadPos() * 16;
}


//-----------------------------------------------------------------------------
// shellFileReadChain()
// Description: Loads sectors from a sequence of files, sector offsets and 
//              sector counts
//
// Parameters:
//   names      - array of filenames (shellFile format), terminated by a null
//                pointer
//   blkMode    - 0 = non-blocking
//                1 = blocking
//   sectorOffsets - array of sector offsets into each file section being loaded
//
//   sectorCounts  - array of sector countrs for each file section being loaded
//
//   eeIopAddrs - array of addresses to which files should be loaded. For
//                each address, LSB = 1 means IOP memory is the destination.
//                Array is terminated implicitly be the length of 'names'.
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------

EShellResult shellFileReadChain( const char **names, EShellFileBlockingMode blkMode, 
								int *sectorOffsets, int *sectorCounts, void **eeIopAddrs )
{
	int n;

	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(names, "Null pointer argument %s", "names");
	ASSERT(eeIopAddrs, "Null pointer argument %s", "eeIopAddrs");
	ASSERT(sectorOffsets, "Null pointer argument %s", "sectorOffsets");
	ASSERT(sectorCounts, "Null pointer argument %s", "sectorCounts");

	
	if (shellFileSync(SHELLFILE_NON_BLOCKING)!=SHELL_SUCCESS)
		WARNING(0, "Sync needed for non-blocking function in progress");

	for(n = 0; n<64; n++)
    {
		if(_isLibcdvdMedium) // Build loadChainTag table
		{
			FileInfo fileInfo;
			
			if(names[n]==0) // NULL terminate if item is end of list
			{
				_loadChainTag[(n*3)+0] = 0xffffffff; 
				_loadChainTag[(n*3)+1] = 0xffffffff;
				_loadChainTag[(n*3)+2] = 0xffffffff;
				break;
			}

			ASSERT(!((int) eeIopAddrs[n]%64),"Dest address is not 64-byte aligned");

			
			if(!_GetInfo(names[n], &fileInfo)) return SHELL_FAIL;
						
			if (sectorCounts[n]==-1)
			{
				_loadChainTag[(n*3)+0] = (u_int) fileInfo.lsn;
				_loadChainTag[(n*3)+1] = (u_int) BYTES_TO_SECTORS(fileInfo.bytes);
				_loadChainTag[(n*3)+2] = (u_int) eeIopAddrs[n];
			}
			else
			{
				_loadChainTag[(n*3)+0] = (u_int) fileInfo.lsn + (u_int) sectorOffsets[n]; 			
				_loadChainTag[(n*3)+1] = (u_int) sectorCounts[n];  
				_loadChainTag[(n*3)+2] = (u_int) eeIopAddrs[n];
			}
		}
		
		else // Or build filename list + sector offset and counts list
		{
			if(names[n]==0)
			{
				_namesCpy[n][0] = 0;
				break;
			}
			
			ASSERT(!((int) eeIopAddrs[n]%64),"Dest address is not 64-byte aligned");

			strcpy(_namesCpy[n], names[n]);

			if (sectorCounts[n]==-1)
			{
				_eeIopAddrsCpy[n] = eeIopAddrs[n];
				_sectorOffsetsCpy[n] = 0; 
				_sectorCountsCpy[n] = -1; 
			}
			else
			{
				_eeIopAddrsCpy[n] = eeIopAddrs[n];
				_sectorOffsetsCpy[n] = sectorOffsets[n];
				_sectorCountsCpy[n] = sectorCounts[n];
			}
		}
    }
	
	if(_isLibcdvdMedium) // Startload and sync(block) if required
    {
		if(!sceCdReadChain(_loadChainTag, &_fileMode)) return SHELL_FAIL;
		
		if(blkMode) sceCdSync(0x00);
		else _fileSyncFunc = _SyncReadChain;
		
		return SHELL_SUCCESS;
    }
	
	else // Startload and sync(block) if required
    {
		_curTagInd = -1;
		_realFileSyncFunc = _SyncReturn1;
		_fileSyncFunc = _SyncReadChain;
		
		return _SyncReadChain(blkMode);
    }
}




//-----------------------------------------------------------------------------
// shellFileLoadChain()
// Description: loads a sequence of files
// Parameters:
//   names      - array of filenames (shellFile format), terminated by a null
//                pointer
//   blkMode    - 0 = non-blocking
//                1 = blocking
//   eeIopAddrs - array of addresses to which files should be loaded. For
//                each address, LSB = 1 means IOP memory is the destination.
//                Array is terminated implicitly be the length of 'names'.
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileLoadChain( const char **names, EShellFileBlockingMode blkMode, void **eeIopAddrs )
{
	int n;

	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(names, "Null pointer argument %s", "names");
	ASSERT(eeIopAddrs, "Null pointer argument %s", "eeIopAddrs");
	
	if (shellFileSync(SHELLFILE_NON_BLOCKING)!=SHELL_SUCCESS)
		WARNING(0, "Sync needed for non-blocking function in progress");

	for(n = 0; n<64; n++)
    {

		if(_isLibcdvdMedium)
		{
			FileInfo fileInfo;
			
			if(names[n]==0)
			{
				_loadChainTag[(n*3)+0] = 0xffffffff;
				_loadChainTag[(n*3)+1] = 0xffffffff;
				_loadChainTag[(n*3)+2] = 0xffffffff;
				break;
			}
			
			// Added: Warning on 64byte non alignment of destination addresses
			ASSERT(!((int) eeIopAddrs[n]%64),"Dest address is not 64-byte aligned");

			if(!_GetInfo(names[n], &fileInfo)) return SHELL_FAIL;
			
			_loadChainTag[(n*3)+0] = (u_int) fileInfo.lsn;
			_loadChainTag[(n*3)+1] = (u_int) BYTES_TO_SECTORS(fileInfo.bytes);
			_loadChainTag[(n*3)+2] = (u_int) eeIopAddrs[n];
		}
		else
		{
			if(names[n]==0)
			{
				_namesCpy[n][0] = 0;
				break;
			}

			// Added: Warning on 64byte non alignment of destination addresses
			ASSERT(!((int) eeIopAddrs[n]%64),"Dest address is not 64-byte aligned");
			
			strcpy(_namesCpy[n], names[n]);
			_eeIopAddrsCpy[n] = eeIopAddrs[n];
			_sectorOffsetsCpy[n] = 0; // Added for _syncReadChain function
			_sectorCountsCpy[n] = -1; // Added for _syncReadChain function
		}
    }
	
	if(_isLibcdvdMedium)
    {
		if(!sceCdReadChain(_loadChainTag, &_fileMode)) return SHELL_FAIL;
		
		if(blkMode) sceCdSync(0x00);
		else _fileSyncFunc = _SyncReadChain;
		
		return SHELL_SUCCESS;
    }
	
	else
    {
		_curTagInd = -1;
		_realFileSyncFunc = _SyncReturn1;
		_fileSyncFunc = _SyncReadChain;
		
		return _SyncReadChain(blkMode);
    }
}

//-----------------------------------------------------------------------------
// shellFileReadIOPm()
// Description: Read sectors from a file to IOP memory
// Parameters:
//   name           - filename in shellFile format
//   blkMode        - 0 = non-blocking
//                    1 = blocking
//   iopAddr        - Target IOP address
//   sectorOffset   - offset within file to start reading, in 2048-byte units
//   sectors        - amount of data to read, in 2048-byte units
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileReadIOPm( const char *name, EShellFileBlockingMode blkMode, 
								int iopAddr, int sectorOffset, int sectors )
{	
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(name, "Null pointer argument %s", "name");
	
	if (shellFileSync(SHELLFILE_NON_BLOCKING)!=SHELL_SUCCESS)
		WARNING(0, "Sync needed for non-blocking function in progress");


	return _FileReadIOPm( name, blkMode, iopAddr, sectorOffset, sectors);
}

//-----------------------------------------------------------------------------
// shellFileLoadIOPm()
// Description: loads a file to IOP memory
// Parameters:
//   name     - filename in shellFile format
//   blkMode  - 0 = non-blocking
//              1 = blocking
//   iopAddr  - Target IOP address
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileLoadIOPm( const char *name, EShellFileBlockingMode blkMode, int iopAddr )
{	
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(name, "Null pointer argument %s", "name");
	
	if (shellFileSync(SHELLFILE_NON_BLOCKING)!=SHELL_SUCCESS)
		WARNING(0, "Sync needed for non-blocking function in progress");

	return _FileReadIOPm( name, blkMode, iopAddr, 0, -1);
}

//-----------------------------------------------------------------------------
// shellFilePause()
// Description: pause device head
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFilePause( void )
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium) 
	{
		_fileSyncFunc = _SyncCdSync;
		return sceCdPause() ? SHELL_SUCCESS : SHELL_FAIL;
	}
	else
	{
    	_kernelStatus=SHELLFILE_STAT_PAUSE;
		_fileSyncFunc = _SyncReturn1;
		return SHELL_SUCCESS;
	}
}

//-----------------------------------------------------------------------------
// shellFileSeek()
// Description: move device head to file position
// Parameters:
//   name         - filename in shellFile format
//   sectorOffset - offset within file to seek to, in 2048-byte units
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileSeek( const char *name, int sectorOffset )
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(name, "Null pointer argument %s", "name");
	
	if(shellFileSync(SHELLFILE_NON_BLOCKING)!=SHELL_SUCCESS)
		WARNING(0, "Sync needed for non-blocking function in progress");

	if(_isLibcdvdMedium)
	{
		FileInfo fileInfo;
		
		if(!_GetInfo(name, &fileInfo)) return SHELL_FAIL;
		
		_fileSyncFunc = _SyncCdSync;
		
		return sceCdSeek((u_int)(fileInfo.lsn + sectorOffset)) ? SHELL_SUCCESS : SHELL_FAIL;
	}

	else
	{
		_fileSyncFunc = _SyncReturn1;
		return SHELL_SUCCESS;
	}
}

//-----------------------------------------------------------------------------
// shellFileStandby()
// Description: starts rotation of media
// Parameters:
//   blkMode   - 0 = non-blocking
//               1 = blocking
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileStandby( EShellFileBlockingMode blkMode )
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium)
    {
		if(!sceCdStandby()) return SHELL_FAIL;		
		if(blkMode) sceCdSync(0x00);
		else _fileSyncFunc = _SyncCdSync;		
    }
    
    return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileStatus()
// Description: get drive status
// Returns:  SHELLFILE_STAT_SHELLOPEN = Tray is open
//           SHELLFILE_STAT_STOP      = Stopped
//           SHELLFILE_STAT_SPIN      = Spindle is spinning
//           SHELLFILE_STAT_READ      = Reading
//           SHELLFILE_STAT_PAUSE     = Paused
//           SHELLFILE_STAT_SEEK      = Seeking
//           SHELLFILE_STAT_EMG       = Abnormal termination
//-----------------------------------------------------------------------------
EShellFileStat shellFileStatus( void )
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium) return (EShellFileStat) sceCdStatus();
	else return _kernelStatus;
}

//-----------------------------------------------------------------------------
// shellFileStop()
// Description: stops rotation of media
// Parameters:
//   blkMode   - 0 = non-blocking
//               1 = blocking
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileStop( EShellFileBlockingMode blkMode )
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium)
	{
		sceCdStop();
		if(blkMode) sceCdSync(0x00);
		else _fileSyncFunc = _SyncCdSync;		
    }
    _kernelStatus=SHELLFILE_STAT_STOP;
    
    return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileStInit()
// Description: initialise file streaming functions
// Parameters:
//   sectors     - capaciy of IOP stream buffer, in 2048-byte units
//   partitions  - number of stream buffer partitions
//   iopAddr     - IOP pointer to stream buffer
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileStInit( int sectors, int partitions, int iopAddr )
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(iopAddr, "Null pointer argument %s", "iopAddr");
	
	if(SCE_LIBRARY_VERSION >= 0x2600)
	{
		// Streaming functions devolved to a separate module from 2.6.0

		if(!shellFileLoadStartModule("cdvdstm", 0, "", NULL))
			ASSERT(0, "Couldn't load IOP module");
	}

	if(_isLibcdvdMedium)
		sceCdStInit((u_int) sectors,(u_int) partitions, (u_int) iopAddr);

	else _stBufSize = sectors;
	
	return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileStStart()
// Description: begin streaming from a file
// Parameters:
//   name          - filename in shellFile format
//   sectorOffset  - offset from start of file to begin streaming, in
//                   2048-byte units
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileStStart( char *name, int sectorOffset )
{
	FileInfo fileInfo;
	
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(name, "Null pointer argument %s", "name");
	
	if(!_GetInfo(name, &fileInfo)) return SHELL_FAIL;
	
	if(_isLibcdvdMedium)
    {
		_stLsn = fileInfo.lsn;
		sceCdStStart((u_int)  (_stLsn + sectorOffset), &_streamMode);
		return SHELL_SUCCESS;
    }
	
	else
    {
		if((_file = sceOpen(fileInfo.nativeName, SCE_RDONLY)) < 0) return SHELL_FAIL;
		
		_stLength = (sceLseek(_file, 0, SCE_SEEK_END) + 2047) / 2048;
		sceLseek(_file, sectorOffset*2048, SCE_SEEK_SET);
		_stPos = sectorOffset;
    }
	
	return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileStRead()
// Description: read stream data
// Parameters:
//   eeAddr   - target EE address
//   sectors  - amount of data to read in 2048-byte units
//   blkMode  - 0 = non-blocking
//              1 = blocking
// Returns: number of sectors read
//-----------------------------------------------------------------------------
int shellFileStRead( void *eeAddr, int sectors, EShellFileBlockingMode blkMode )
{
	int n;

	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(eeAddr, "Null pointer argument %s", "eeAddr");
	
	if(_isLibcdvdMedium)
    {
		u_int err;
		
		return sceCdStRead((u_int) sectors, (u_int *) eeAddr,
						   (u_int) (blkMode ? STMBLK : STMNBLK), &err);
    }
	
	else
	{
		_stPos += n = (sceRead(_file, eeAddr, sectors*2048)+2047)/2048;
		
		if(_stPos >= _stLength) return sectors;
		else return n;
	}
}

//-----------------------------------------------------------------------------
// shellFileStStop()
// Description: stop streaming
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileStStop( void )
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium)
		return sceCdStStop() ? SHELL_SUCCESS : SHELL_FAIL;
	
	else return (sceClose(_file) == -1) ? SHELL_FAIL : SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileStSeek()
// Description: change stream position
// Parameters:
//   sectors - offset from start of file to resume streaming, in 2048-byte
//             units
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileStSeek( int sectors )
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium)
		return sceCdStSeek((u_int) (_stLsn + sectors)) ? SHELL_SUCCESS : SHELL_FAIL;
	
	else
	{
		if(sceLseek(_file, sectors*2048, SCE_SEEK_SET) == -1) return SHELL_FAIL;
		else _stPos = sectors;
	}
	
	return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileStStat()
// Description: get stream data read status
// Returns: amount of data accumulated in stream buffer, in 2048-byte units
//-----------------------------------------------------------------------------
int shellFileStStat( void )
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium) return sceCdStStat();
	
	// We claim the entire pretend buffer has been filled, just in case
	// the caller was waiting for it to be completely full before performing
	// some action.
	
	else return _stBufSize;
}

//-----------------------------------------------------------------------------
// shellFileSync()
// Description: wait for or detect command completion
// Parameters:
//   blkMode - 0 = non-blocking
//             1 = blocking
// Returns:  SHELL_INDETERMINATE = not completed
//           SHELL_FAIL          = error occurred
//           SHELL_SUCCESS       = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileSync( EShellFileBlockingMode blkMode )
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	
	return _fileSyncFunc(blkMode);
}

//-----------------------------------------------------------------------------
// shellFileTrayReq()
// Description: open, close or detect status of tray
// Parameters:
//   command   - 0 = open tray
//               1 = close tray
//               2 = get tray state change
//   pTrayCnt  - pointer to hold tray state change result
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileTrayReq( int command, int *pTrayCnt )
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(pTrayCnt, "Null pointer argument %s", "pTrayCnt");
	
	if(_isLibcdvdMedium)
		return sceCdTrayReq(command, (unsigned int *) pTrayCnt) ? SHELL_SUCCESS : SHELL_FAIL;
	
	else
    {     
		*pTrayCnt = 0;
		return SHELL_SUCCESS;
    }
}



//-----------------------------------------------------------------------------
// shellFileHandleOpen
// Description: Open specified file
// Parameters:
//   fileName       - filename in shellFile format
//	 fileHandle		- structure holding file info
//	 mode			- specifies read/write/create file modes
//
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileHandleOpen(char *fileName, ShellFileHandle *fileHandle, int mode)
{
	FileInfo file;
	
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");

	// Init file structure
	fileHandle->fd=0;
	fileHandle->bytePos=0;
	fileHandle->mode=mode;
	strcpy(fileHandle->fileName, fileName);
	
	// Get the size of this file in bytes and the disc LSN
	if(!_GetInfo(fileName, &file)) return SHELL_FAIL;


	if (_isLibcdvdMedium)
    {
		fileHandle->byteSize = file.bytes;
		fileHandle->lsn      = file.lsn;
	}
	else
	{ 
		unsigned int size;
		
		if((_file = sceOpen(file.nativeName, fileHandle->mode)) < 0) return SHELL_FAIL;
		size = (unsigned int) sceLseek(_file, 0, SCE_SEEK_END);      
		sceClose(_file);
		
		fileHandle->byteSize = (int)size;
		fileHandle->lsn      = 0;
    }

	return SHELL_SUCCESS;

}
//-----------------------------------------------------------------------------
// shellFileHandleRead
// Description: Read bytes from specified file from current position
// Parameters:
//	 fileHandle		- structure holding file info
//	 addr			- destination for data (must be 64byte aligned)
//	 bytes			- number of bytes to read (must be in multiples of 64bytes)
//   blkMode		- 0 = non-blocking
//            		  1 = blocking
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileHandleRead(ShellFileHandle *fileHandle, void *addr, int sizeInBytes, 
									EShellFileBlockingMode blkMode)
{
	// Performs open and read each occasion it is called
	int  startSectorPos, middleSectorPos, endSectorPos;
	int chainIndex=0;
	_reUseSector=0;

	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(!((int) addr%64),"Dest address is not 64-byte aligned");
	ASSERT(!((int) sizeInBytes%64),"Size is not a multiple of 64-bytes");
	ASSERT(addr,"destination pointer 'addr'=NULL");

	// Check for non-blocking reads in progress
	if (shellFileSync(SHELLFILE_NON_BLOCKING)!=SHELL_SUCCESS)
		WARNING(0, "Sync needed for non-blocking function in progress");

	_eeIopDestAddr = addr;
	_eeIopTransferSize = sizeInBytes;
	
	// Start off a shellFileReadChain of upto 3 parts for start, middle, end

	// Part1
	_startBufferOffset = (fileHandle->bytePos%2048);
	if ((_startBufferOffset+sizeInBytes)<2048) 
		_startBufferSize  = sizeInBytes;
	else 
		_startBufferSize  = 2048 - _startBufferOffset;

	startSectorPos = fileHandle->lsn + (fileHandle->bytePos>>11);

	if (_startBufferSize==2048) _startBufferSize=0;

	// Part2
	_numMiddleSectors = ((sizeInBytes - _startBufferSize)>>11);
	if (_startBufferSize>0)
		middleSectorPos=startSectorPos+1;
	else
		middleSectorPos=startSectorPos;

	// Part3
	endSectorPos = middleSectorPos + _numMiddleSectors;
	_endBufferSize = sizeInBytes - (_startBufferSize + (_numMiddleSectors<<11));
	if (_endBufferSize<0) _endBufferSize = 0;


	if ((_medium==SHELLFILE_MEDIUM_CD)||(_medium==SHELLFILE_MEDIUM_DVD)) // host0 sectors aren't cached
		_reUseSector = (_lastSectorLoaded==startSectorPos);

	_lastSectorLoaded = -1; // reset

	// Part 1 loads 1st section to buffer (for copy at sync stage)
	if (_startBufferSize>0)
	{
		if (_reUseSector)
		{
			if ((u_int)addr & 0x1) // iop addr
			{
				_CopyToIopMem(addr, ((unsigned char *)_endBuffer) + _startBufferOffset, 
								_startBufferSize);
			}
			else // ee addr
			{
				memcpy(addr, ((char*)_endBuffer) + _startBufferOffset, 
						(unsigned int)_startBufferSize);
			}																				
		}
		else // Add to chain
		{		
			_sectorOffsetsCpy[chainIndex] = startSectorPos;
			_eeIopAddrsCpy[chainIndex]     = _startBuffer;			
			strcpy(_namesCpy[chainIndex], fileHandle->fileName);
			_sectorCountsCpy[chainIndex++] = 1;
		}
		_lastSectorLoaded = startSectorPos;
	}
	
	// Part 2 loads	middle section directly (if there is one)
	if (_numMiddleSectors>0)
	{
		_sectorOffsetsCpy[chainIndex] = middleSectorPos;
		_eeIopAddrsCpy[chainIndex]     = ((char*)addr) + _startBufferSize;
		strcpy(_namesCpy[chainIndex], fileHandle->fileName);
		_sectorCountsCpy[chainIndex++] = _numMiddleSectors;
	}

	// Part 3 loads end section to buffer (for copy at sync stage)
	if (_endBufferSize>0)
	{
		_sectorOffsetsCpy[chainIndex] = endSectorPos;
		_eeIopAddrsCpy[chainIndex]     = _endBuffer;
		strcpy(_namesCpy[chainIndex], fileHandle->fileName);
		_sectorCountsCpy[chainIndex++] = 1;
		_lastSectorLoaded              = endSectorPos;
	}

	// Terminate chain
	_namesCpy[chainIndex][0] = 0;
	pActiveFileHandle = fileHandle;
	
	if(_isLibcdvdMedium) 
    {
		int n;

		// Build list for sceCdReadChain		
		for(n = 0; n<chainIndex; n++)
	    {									
			_loadChainTag[(n*3)+0] = (u_int) _sectorOffsetsCpy[n]; 
			_loadChainTag[(n*3)+1] = (u_int) _sectorCountsCpy[n];  
			_loadChainTag[(n*3)+2] = (u_int) _eeIopAddrsCpy[n];
		}

		// Terminate list
		_loadChainTag[(n*3)+0] = 0xffffffff; 
		_loadChainTag[(n*3)+1] = 0xffffffff;
		_loadChainTag[(n*3)+2] = 0xffffffff;

		if(!sceCdReadChain(_loadChainTag, &_fileMode)) return SHELL_FAIL;
		
		if(blkMode) _SyncReadChainHandle(SHELLFILE_BLOCKING);
		else _fileSyncFunc = _SyncReadChainHandle;
		 
		return SHELL_SUCCESS;
    }
	
	else // Startload and sync(block) if required
    {
		_curTagInd = -1;
		_realFileSyncFunc = _SyncReturn1;
		_fileSyncFunc = _SyncReadChainHandle;
		
		return _SyncReadChainHandle(blkMode);
    }

}

//-----------------------------------------------------------------------------
// shellFileHandleSeek
// Description: seek file to specified position (non blocking)
// Parameters:
//	 fileHandle			- structure holding file info
//   byteOffset		- ammount to seek in bytes (must be in multiples of 64bytes)
//   whence			- SHELLFILE_SET
//					  SHELLFILE_CUR
//					  SHELLFILE_END
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileHandleSeek(ShellFileHandle *fileHandle, int byteOffset, EShellFileWhence whence)
{
	int newPos;

	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(!((int) byteOffset%64),"Seek is not a multiple of 64-bytes");
	
	switch (whence)
	{
	    default:
		case SHELLFILE_SET: // Start + offset
			newPos = byteOffset;
			break;
		case SHELLFILE_CUR: // Current Pos + offset
			newPos = fileHandle->bytePos + byteOffset;
			break;
		case SHELLFILE_END: // End - offset
			newPos = fileHandle->byteSize - byteOffset;
			break;
	}


	if (shellFileSync(SHELLFILE_NON_BLOCKING)!=SHELL_SUCCESS)
		WARNING(0, "Sync needed for non-blocking function in progress");

	if ((newPos>=0) && (newPos<=fileHandle->byteSize))
	{
		fileHandle->bytePos = newPos;
		return SHELL_SUCCESS;
	}
	else
	{
		WARNING(0, "shellFileSeek outside range of file '%s'",
					fileHandle->fileName);
		return SHELL_FAIL;
	}	
}

//-----------------------------------------------------------------------------
// shellFileHandleSync
// Description: wait for or detect command completion
// Parameters:
//	 fileHandle		- structure holding file info
//   blkMode		- 0 = non-blocking
//            		  1 = blocking
// Returns:  SHELL_INDETERMINATE = not completed
//           SHELL_FAIL          = error occurred
//           SHELL_SUCCESS       = completed
//-----------------------------------------------------------------------------
EShellResult shellFileHandleSync(ShellFileHandle *fileHandle, EShellFileBlockingMode blkMode)
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(fileHandle, "fileHandle=NULL");

	(void) fileHandle;    // To prevent unused parameter warnings

	// Use shellFileSync system. 
	// Currently fileHandle is unused as last file being read is the one synced on

	return shellFileSync(blkMode);
}

//-----------------------------------------------------------------------------
// shellFileHandleClose
// Description: Closes specified file 
// Parameters:
//	 fileHandle		- structure holding file info
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult   shellFileHandleClose(ShellFileHandle *fileHandle)
{
	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(fileHandle, "fileHandle=NULL");

	(void) fileHandle;    // To prevent unused parameter warnings

	if (shellFileSync(SHELLFILE_NON_BLOCKING)!=SHELL_SUCCESS)
		WARNING(0, "Sync needed before closing file with non-blocking read in progress");

	return SHELL_SUCCESS;
}


//-----------------------------------------------------------------------------
// shellFileHandleWrite
// Description: Write data to a specified file
// Parameters:
//	 fileHandle		- structure holding file info
//	 source			- source address for data (must be 64byte aligned)
//	 byteCount		- number of bytes to write (must be in multiples of 64bytes)
//   blkMode		- 0 = non-blocking
//            		  1 = blocking
// Returns:  SHELL_FAIL    = error occurred
//           SHELL_SUCCESS = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileHandleWrite( ShellFileHandle *fileHandle, void *source, int byteCount,
									EShellFileBlockingMode blkMode)
{
	int err, stillExecuting;
	char nativeName[128];

	ASSERT(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERT(fileHandle, "fileHandle=NULL");
	ASSERT(source,"source pointer=NULL");


	ASSERT(!((int) source%64),"Source address is not 64-byte aligned");
	ASSERT(!((int) byteCount%64),"Size is not a multiple of 64-bytes");


	shellFileNativeName(fileHandle->fileName, nativeName);

	if (blkMode)
	{
		_file = sceOpen(nativeName, fileHandle->mode, 777);
		sceLseek(_file, fileHandle->bytePos, SEEK_SET);
		sceWrite(_file, source, byteCount );
		sceClose(_file);

		_fileSyncFunc = _SyncReturn1;
		return SHELL_SUCCESS;
	}
	else
	{

		if (shellFileSync(SHELLFILE_NON_BLOCKING)!=SHELL_SUCCESS)
			WARNING(0, "Sync needed for non-blocking function in progress");
		
		_file = sceOpen(nativeName, fileHandle->mode | SCE_NOWAIT);

		// wait for open to complete
		do
		{
			if((err = sceIoctl(_file, SCE_FS_EXECUTING, &stillExecuting))<0)
			{
				WARNING(0, "sceOpen failed (returned %d) for '%s'", err, nativeName);
				return SHELL_FAIL;
			}
		}			
		while(stillExecuting);


		if (fileHandle->bytePos!=0)
		{
			sceLseek(_file, fileHandle->bytePos, SEEK_SET);

			// wait for seek to complete
			do
			{
				if((err = sceIoctl(_file, SCE_FS_EXECUTING, &stillExecuting))<0)
				{
					WARNING(0, "sceOpen failed (returned %d) for '%s'", err, nativeName);
					return SHELL_FAIL;
				}
			}
			while(stillExecuting);		
		}

		// Non blocking write
		sceWrite(_file, source, byteCount );

		_fileSyncFunc = _SyncWrite;
		_kernelStatus = SHELLFILE_STAT_READ;
		_kernelError = SHELLFILE_ER_NORDY;
	}

	return SHELL_SUCCESS;
}


/* ------------------------------------------------------------------- */
int shellStatFile(const char *fileName, struct sce_stat *statbuffer){
    char fname[256];



    /* !!!! WARNING-> this does not check buffer lengths */
    shellFileNativeName(fileName, fname);

    return sceGetstat(fname, statbuffer);
}

int shellDopen(const char *dirname){
    char fname[256];


    /* !!!! WARNING-> this does not check buffer lengths */
    shellFileNativeName(dirname, fname);

    return sceDopen(fname);
}

int shellDclose(int fd){
    return sceDclose(fd);
}

int shellDread(int fd, struct sce_dirent *buf){
    return sceDread(fd, buf);
}


int shellIoOpen(const char *filename, int flags, int mode){
    char fname[256];


    /* !!!! WARNING-> this does not check buffer lengths */
    shellFileNativeName(filename, fname);

    return sceOpen(fname, flags, mode);
}

