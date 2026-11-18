//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// shellFile header file
//
// $Workfile: shellFile.h $
// $Author: Mdean $
// $Date: 9/09/03 16:12 $
// $Revision: 11 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

#ifndef _SHELLCORE_FILE_H_
#define _SHELLCORE_FILE_H_

//-----------------------------------------------------------------------------
// M A C R O S ,   C O N S T A N T S
//-----------------------------------------------------------------------------


#define SHELLFILE_READ   SCE_RDONLY
#define SHELLFILE_WRITE  SCE_WRONLY
#define SHELLFILE_RDWR   SCE_RDWR
#define SHELLFILE_APPEND SCE_APPEND
#define SHELLFILE_CREATE SCE_CREAT
#define SHELLFILE_TRUNC  SCE_TRUNC
#define SHELLFILE_EXCL   SCE_EXCL

// These correspond to libcdvd's SCECdEr definitions

typedef enum
{
	SHELLFILE_ER_FAIL    = -1,
	SHELLFILE_ER_NO      = 0x00,
	SHELLFILE_ER_EOM     = 0x32,
	SHELLFILE_ER_TRMOPN  = 0x31,
	SHELLFILE_ER_READ    = 0x30,
	SHELLFILE_ER_CUD     = 0x14,
	SHELLFILE_ER_NORDY   = 0x13,
	SHELLFILE_ER_ABRT    = 0x01
	
} EShellFileError;

// These correspond to libcdvd's SCECdStat definitions

typedef enum
{
	SHELLFILE_STAT_SHELLOPEN  = 0x01,
	SHELLFILE_STAT_STOP       = 0x00,
	SHELLFILE_STAT_SPIN       = 0x02,
	SHELLFILE_STAT_READ       = 0x06,
	SHELLFILE_STAT_PAUSE      = 0x0a,
	SHELLFILE_STAT_SEEK       = 0x12,
	SHELLFILE_STAT_EMG        = 0x20
	
} EShellFileStat;

typedef enum
{
	SHELLFILE_NON_BLOCKING = 0,
	SHELLFILE_BLOCKING = 1
	
} EShellFileBlockingMode;

typedef enum
{
	SHELLFILE_MEDIUM_HOST0,
	SHELLFILE_MEDIUM_CD,
	SHELLFILE_MEDIUM_DVD,
	SHELLFILE_MEDIUM_SCEEDEMO,
	SHELLFILE_MEDIUM_ATMON
	
} EShellFileMedium;

typedef enum
{
	SHELLFILE_SET,
	SHELLFILE_CUR,
	SHELLFILE_END
	
} EShellFileWhence;


// Recently added structure
typedef struct _ShellFileHandle
{
	char fileName[128];
	int mode;        // open for read/write?
	int fd;          // for sceOpen type handles
	int bytePos;     // current file position
	int byteSize;
	int lsn;         // Logical sector number
} ShellFileHandle; 

#define BYTES_TO_SECTORS(x) (((x)+2047)/2048)


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif


// All these functions were recently added
EShellResult   shellFileHandleOpen( char *fileName, ShellFileHandle *fileHandle, int mode ); // new
EShellResult   shellFileHandleRead( ShellFileHandle *fileHandle, void *addr, int bytes, EShellFileBlockingMode blkMode ); // new
EShellResult   shellFileHandleSeek( ShellFileHandle *fileHandle, int byteOffset, EShellFileWhence whence ); // new
EShellResult   shellFileHandleSync( ShellFileHandle *fileHandle, EShellFileBlockingMode blkMode ); // new
EShellResult   shellFileHandleClose( ShellFileHandle *fileHandle ); // new

EShellResult   shellFileHandleWrite( ShellFileHandle *fileHandle, void *source, int byteCount, EShellFileBlockingMode blkMode);

EShellResult   shellFileReadChain( const char **names, EShellFileBlockingMode blkMode, int *sectorOffsets, int *sectorCounts, void **eeIopAddrs ); // new
EShellResult   shellFileReadIOPm( const char *name, EShellFileBlockingMode blkMode, int iopAddr, // new
									int sectorOffset, int sectors );
// End of recently added functions

void 	       shellFileSetIOPImage(char *image);
void           shellFileInit( EShellFileMedium medium, int rebootIop,
							  const char *iopModPath, void *medParam );
void           shellFileNativeName( const char *src, char *dest );
int            shellFileLoadStartModule(const char *name, int args, const char *argp,
										int *pResult );
EShellResult    shellFileStopUnloadModule( int id );
int            shellFileDiskReady( EShellFileBlockingMode blkMode );
EShellFileError shellFileGetError( void );
unsigned int   shellFileGetSize( const char *name );
EShellResult    shellFileRead( const char *name, EShellFileBlockingMode blkMode, void *eeAddr,
							int sectorOffset, int sectors );
void          *shellFileLoad( const char *name, unsigned int alignment, unsigned int *length);
int            shellFileGetLoadPos( void );
EShellResult    shellFileLoadChain( const char **names, EShellFileBlockingMode blkMode, void **eeIopAddrs );
EShellResult    shellFileLoadIOPm( const char *name, EShellFileBlockingMode blkMode, int iopAddr );
EShellResult    shellFilePause( void );
EShellResult    shellFileSeek( const char *name, int sectorOffset );
EShellResult    shellFileStandby( EShellFileBlockingMode blkMode );
EShellFileStat  shellFileStatus( void );
EShellResult    shellFileStop( EShellFileBlockingMode blkMode );
EShellResult    shellFileStInit( int sectors, int partitions, int iopAddr );
EShellResult    shellFileStStart( char *name, int sectorOffset );
int            shellFileStRead( void *eeAddr, int sectors, EShellFileBlockingMode blkMode );
EShellResult    shellFileStStop( void );
EShellResult    shellFileStSeek( int sectors );
int            shellFileStStat( void );
EShellResult    shellFileSync( EShellFileBlockingMode blkMode );
EShellResult    shellFileTrayReq( int command, int *pTrayCnt );

//-------------------------------------
struct sce_stat;
struct sce_dirent;

int shellStatFile(const char *fileName, struct sce_stat *statbuffer);

int shellDopen(const char *dirname);
int shellDclose(int fd);
int shellDread(int fd, struct sce_dirent *buf);

int shellIoOpen(const char *filename, int flags, int mode);


#ifdef __cplusplus
}
#endif

#endif // _SHELLCORE_FILE_H_
