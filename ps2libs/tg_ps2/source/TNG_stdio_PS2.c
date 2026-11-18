#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sceerrno.h>
#include <sifdev.h>
#include "include/TNG_stdio_PS2.h"
#include <stdarg.h>
//#define USE_PHYSFS

#ifdef USE_DEBUGPRINT
# define DEBUGPRINT printf
#else
# define DEBUGPRINT(fmt, args...)
#endif


#ifdef TRANSGAMING_USE_PHYSFS
# include "shellCore.h"
# include "shellFile.h"
# include "physfs.h"
# include "extras/globbing.h"
#endif


//define this symbol to use PhysFS from the DVD/UMD drive.  If undefined, 
//  the file access will come from host0.
#ifdef TRANSGAMING_MASTERING
# define PHYSFS_UMD
#endif


// eric-> uncomment this symbol is the TNG_f*() family of functions should also support FILE *
//   streams.
//#define SUPPORT_STREAM

#ifndef SUPPORT_STREAM
typedef struct{
    int lastOp;     //the result of the last file read or write operation
    int eof;        //non-zero if a read or write op hut the end of file
    int fd;         //the file descriptor 
} FileData;
#endif

#ifdef TRANSGAMING_USE_PHYSFS
static int g_physfsInitialized = 0;


static void PS2InitializeFileIO()
{
    if (g_physfsInitialized)
        return;


# ifdef PHYSFS_UMD
	shellFileInit(SHELLFILE_MEDIUM_DVD, 0, "iop", NULL);
# else
	shellFileInit(SHELLFILE_MEDIUM_HOST0, 0, "IOP", NULL);
# endif

    if (!PHYSFS_init(NULL)){
        DEBUGPRINT("PS2InitializeFileIO(): ERROR-> could not initialize file system (specific error: '%s')\n", PHYSFS_getLastError());

        exit(-1);
    }


    if (!PHYSFS_addToSearchPath("PS2ROOT.ZIP", 1)){
        DEBUGPRINT("PS2InitializeFileIO(): ERROR-> could not register main data volume (specific error: '%s')\n", PHYSFS_getLastError());

        exit(-2);
    }

    if (!PHYSFS_addToSearchPath("PS2SCRPT.ZIP", 1)){
        DEBUGPRINT("PSPInitializeFileIO(): WARNING-> could not register scripts data volume (specific error: '%s')... assuming scripts are in 'root.zip'\n", PHYSFS_getLastError());
    }

#ifndef PHYSFS_UMD
    if (!PHYSFS_setWriteDir(".")){
        DEBUGPRINT("PS2InitializeFileIO(): ERROR-> could not set the writing directory (specific error: '%s')\n", PHYSFS_getLastError());

        exit(-3);
    }
#endif

    PHYSFS_setCaseSensitivity(0);


    g_physfsInitialized = 1;
}

const char *fixFilename(const char *filename){
    if (filename[0] == '.' && filename[1] == '.' && filename[2] == '/')
        return &filename[3];

    return filename;
}
#endif

// This file will allow us to redirect any stdio calls we wish.

#ifdef SUPPORT_STREAM
# define FD_BIAS 3 // avoid conflict with std fds
#endif

#define CHECK_ERROR(val, ret) \
	if ((val) < 0) { errno = -(val); return ret; }

FILE* TNG_fopen( const char* filename, const char* mode )
{
#ifndef TRANSGAMING_USE_PHYSFS
	const char *m;
	char filename_on_host[256];
	char *rep;
	int flag = 0;
	struct sce_stat stat;
	int r, fd;
# ifndef SUPPORT_STREAM
    FileData *f;
# else
	FILE* f;
# endif

	sprintf( filename_on_host, "%s%s", "host0:", filename );

        // replace backslashes with forward slashes
	rep = filename_on_host;
	while ((rep = strchr(rep, '\\'))) *rep = '/';
	
	//printf("TNG_fopen ---> filename = %s | filename_on_host = %s\n", filename, filename_on_host);
	for (m=mode; *m; m++) {
		switch (*m) {
		case 'a': flag |= SCE_APPEND | SCE_WRONLY | SCE_CREAT; break;
		case 'r': flag |= SCE_RDONLY; break;
		case 'w': flag |= SCE_WRONLY | SCE_CREAT | SCE_TRUNC; break;
		case '+':
			if (flag & SCE_RDONLY) flag |= SCE_WRONLY;
			else flag |= SCE_RDONLY;
			break;
	        }
	}

	r = sceGetstat( filename_on_host, &stat );
	if (r < 0) {
		if ((r == -ENOENT) && (flag & SCE_CREAT)) {
			// doesn't exist? no problem, we're creating
			r = 0;
		} else {
			// somewhat unexpected error
			errno = -r;
			return NULL;
		}
	} else {
		if (stat.st_mode & SCE_STM_FDIR) {
			// sceRead behaves badly (hangs) on directories...
			errno = EISDIR;
			return NULL;
		}
	}

	fd = sceOpen( filename_on_host, flag, SCE_STM_RUGO|SCE_STM_WUGO );
	CHECK_ERROR(fd, NULL)

# ifdef SUPPORT_STREAM
	f = fdopen( fd + FD_BIAS, mode );
	return f;
# else
    f = (FileData *)malloc(sizeof(FileData));

    f->lastOp = 0;
    f->eof = 0;
    f->fd = fd;

    return (FILE *)f;
# endif
#else
	if (!g_physfsInitialized)
		PS2InitializeFileIO();


	PHYSFS_file *   fd;


    filename = fixFilename(filename);


    if(strchr(mode,'+') || strchr(mode,'r')){
        DEBUGPRINT("TNG_fopen(): opening '%s' for reading\n", filename);

		fd = PHYSFS_openRead(filename);
    }
    else{
#ifdef PHYSFS_UMD
        DEBUGPRINT("TNG_fopen(): opening '%s' for writing... this will fail\n", filename);

        return NULL;
#else
        if (strchr(mode, 'a')){
            DEBUGPRINT("TNG_fopen(): opening '%s' for appending\n", filename);
    	
            fd = PHYSFS_openAppend(filename);
        }
        else{
            DEBUGPRINT("TNG_fopen(): opening '%s' for writing\n", filename);
    	
            fd = PHYSFS_openWrite(filename);
        }
#endif
    }


# ifdef DEBUG
    if (fd == NULL){
        DEBUGPRINT("TNG_fopen(): PDK Open failed! (specific error: '%s')\n", PHYSFS_getLastError());

        return NULL;
    }
    else{
        DEBUGPRINT("TNG_fopen(): PDK Open returned %x for %s\n", (unsigned int)fd, filename);
    }
# endif

	return (FILE *)fd;
#endif
}

int TNG_fclose( FILE* stream )
{
#ifndef TRANSGAMING_USE_PHYSFS
# ifndef SUPPORT_STREAM
    FileData *  f = (FileData *)stream;
    int         r;


    r = sceClose(f->fd);
    free(f);

	CHECK_ERROR(r, EOF);

    return 0;
# else
	int r, fd = fileno( stream );
	if (fd < FD_BIAS)
		return fclose( stream );
	r = sceClose( fd - FD_BIAS);
	fclose( stream );
	CHECK_ERROR(r, EOF)
	return 0;
# endif
#else
	int ret = 0;
	int err = PHYSFS_close((PHYSFS_file *)stream);
	
    if (err == 0){
        DEBUGPRINT("TNG_fclose(): error closing the stream %x (specific error: '%s')\n", (int)stream, PHYSFS_getLastError());

		ret = EOF;
    }
	
	DEBUGPRINT("PDK Close on %x returns %d\n", (int)stream, ret);
	return ret;
#endif

}

int TNG_fprintf(FILE *stream, const char *fmt, ...){
    const int   BUFSIZE = 4096;
    char        buf[BUFSIZE];
    va_list     args;
    int         len;

    
    va_start(args, fmt);
    len = vsnprintf(buf, BUFSIZE, fmt, args);
    va_end(args);


    if (len == BUFSIZE){
        buf[BUFSIZE - 1] = 0;

        DEBUGPRINT("TNG_fprintf(): ERROR-> string output overflowed and will be truncated at %d characters\n    string = '%s'", BUFSIZE, buf);
    }


#ifndef TRANSGAMING_USE_PHYSFS
# ifndef SUPPORT_STREAM
    FileData *  f = (FileData *)stream;
    int         r;


    r = sceWrite(f->fd, buf, len);
    f->lastOp = r;
    f->eof |= (r < 0 ? 1 : 0);

    CHECK_ERROR(r, -1);
    return r;
# else
    int r, fd = fileno( stream );
    if (fd < FD_BIAS)
        return fwrite( buf, 1, len, stream );

    r = sceWrite(fd - FD_BIAS, buf, len);
	CHECK_ERROR(r, -1)
    return r;
# endif
#else
	PHYSFS_sint64 bytesWritten = PHYSFS_write((PHYSFS_file *)stream, buf, sizeof(buf[0]), len);

    //DEBUGPRINT("TNG_fwrite(): PDK Write %d bytes in file %x\n", (unsigned int)bytesWritten, (int)stream);


	return (int)bytesWritten;
#endif
}


int TNG_fread( void* buffer, size_t size, size_t count, FILE* stream )
{
#ifndef TRANSGAMING_USE_PHYSFS
# ifndef SUPPORT_STREAM
    FileData *  f = (FileData *)stream;
    int         r;


    r = sceRead(f->fd, buffer, size * count);
    f->lastOp = r;
    f->eof |= (r < 0 ? 1 : 0);

    CHECK_ERROR(r, -1);

    return r / size;
# else
	int r, fd = fileno( stream );
	if (fd < FD_BIAS)
		// read stdin from host
		return fread( buffer, size, count, stream );
	if (size == 0) return 0;
	r = sceRead( fd - FD_BIAS, buffer, size * count );
	CHECK_ERROR(r, -1)
	return r / size;
# endif
#else
    PHYSFS_sint64 bytesRead = PHYSFS_read((PHYSFS_file *)stream, buffer, size, count);

	//DEBUGPRINT("PDK Read %d bytes in file %x\n", bytesRead, (int)stream);
    
    
    return (int)bytesRead;
#endif
}

size_t TNG_fwrite( const void* buffer, size_t size, size_t count, FILE* stream )
{
#ifndef TRANSGAMING_USE_PHYSFS
# ifndef SUPPORT_STREAM
    FileData *  f = (FileData *)stream;
    int         r;


    r = sceWrite(f->fd, buffer, size * count);
    f->lastOp = r;
    f->eof |= (r < 0 ? 1 : 0);

    CHECK_ERROR(r, -1);

    return r / size;
# else
	int r, fd = fileno( stream );
	if (fd < FD_BIAS)
		// write stdout to host
		return fwrite( buffer, size, count, stream );
	if (size == 0) return 0;
	r = sceWrite( fd - FD_BIAS, buffer, size * count );
	CHECK_ERROR(r, -1)
	return r / size;
# endif
#else
	PHYSFS_sint64 bytesWritten = PHYSFS_write((PHYSFS_file *)stream, buffer, size, count);

	//DEBUGPRINT("PDK Write %d bytes in file %x\n", bytesWritten, (int)stream);


	return (int)bytesWritten;
#endif
}

int TNG_fflush( FILE* stream )
{
#ifndef TRANSGAMING_USE_PHYSFS
# ifndef SUPPORT_STREAM
    (void)stream;
    //nothing to do!
    return 0;
# else
	int fd = fileno( stream );
	if (fd < FD_BIAS)
		return fflush( stream );
	// nothing to do
	return 0;
# endif
#else
	return PHYSFS_flush((PHYSFS_file*)stream);
#endif
}

int TNG_fseek( FILE* stream, long offset, int origin )
{
#ifndef TRANSGAMING_USE_PHYSFS
# ifndef SUPPORT_STREAM
    FileData *  f = (FileData *)stream;
    int         r;


	r = sceLseek64( f->fd, offset, origin );
	if (r == -EUNSUP) {
		// try 32-bit version
		r = sceLseek( f->fd, offset, origin );
	}

    f->lastOp = r;
	CHECK_ERROR(r, -1)
	return 0;
# else
	int fd = fileno( stream );
	long r;
	if (fd < FD_BIAS)
		return fseek( stream, offset, origin );
	r = sceLseek64( fd - FD_BIAS, offset, origin );
	if (r == -EUNSUP) {
		// try 32-bit version
		r = sceLseek( fd - FD_BIAS, offset, origin );
	}
	CHECK_ERROR(r, -1)
	return 0;
# endif
#else
	long position = 0;
	switch (origin)
	{
	case SEEK_SET:
		position = 0;
		position += offset;
		break;
	case SEEK_CUR:
		position = PHYSFS_tell((PHYSFS_file *)stream);
		position += offset;
		break;
	case SEEK_END:
		position = PHYSFS_fileLength((PHYSFS_file*)stream);
		position -= offset;
		break;
	}

    int err = PHYSFS_seek((PHYSFS_file *)stream, position);
    if (err == 0)
		err = -1;
	else
	   err = PHYSFS_tell((PHYSFS_file *)stream);


    DEBUGPRINT("TNG_fseek(): PDK Seek in file %x at pos %ld gives result %d\n", (int)stream, position, err);
	
    return err;
#endif
}

int TNG_fsetpos( FILE* stream, const fpos_t* position )
{
#ifndef TRANSGAMING_USE_PHYSFS
# ifndef SUPPORT_STREAM
    FileData *  f = (FileData *)stream;
    int         r;


	r = sceLseek64( f->fd, *position, SEEK_SET );
	if (r == -EUNSUP) {
		// try 32-bit version
		r = sceLseek( f->fd, *position, SEEK_SET );
	}

    f->lastOp = r;
	CHECK_ERROR(r, -1)
	return 0;
# else
	int fd = fileno( stream );
	long r;
	if (fd < FD_BIAS)
		return fsetpos( stream, position );
	r = sceLseek64( fd - FD_BIAS, *position, SEEK_SET );
	if (r == -EUNSUP) {
		// try 32-bit version
		r = sceLseek( fd - FD_BIAS, *position, SEEK_SET );
	}
	CHECK_ERROR(r, -1)
	return 0;
# endif
#else
    DEBUGPRINT("TNG_fsetpos(): PDK setting file position for %x to %d\n", (unsigned int)stream, (int)*position);
	if ( TNG_fseek(stream,(long)*position, SEEK_SET))
		return 0;
	else
		return -1;
#endif
}

long TNG_ftell( FILE* stream )
{
#ifndef TRANSGAMING_USE_PHYSFS
# ifndef SUPPORT_STREAM
	FileData *  f = (FileData *)stream;
    int         r;

    
    r = sceLseek64( f->fd, 0, SEEK_CUR );
	if (r == -EUNSUP) {
		// try 32-bit version
		r = sceLseek( f->fd, 0, SEEK_CUR );
	}

    f->lastOp = r;
	CHECK_ERROR(r, -1)
	return r;
# else
	int fd = fileno( stream );
	long r;
	if (fd < FD_BIAS)
		return ftell( stream );
	r = sceLseek64( fd - FD_BIAS, 0, SEEK_CUR );
	if (r == -EUNSUP) {
		// try 32-bit version
		r = sceLseek( fd - FD_BIAS, 0, SEEK_CUR );
	}
	CHECK_ERROR(r, -1)
	return r;
# endif
#else
    int pos = PHYSFS_tell((PHYSFS_file *)stream);
	
    DEBUGPRINT("TNG_ftell(): PDK Tell for file %x is %d\n", (int)stream, pos);
    
    
    return pos;
#endif
}

char *TNG_fgets( char *str, int len, FILE *stream ){
    int i = 0;


    if (str == NULL || stream == NULL || len <= 0)
        return NULL;

    while (i < len){

        //read a byte and terminate the stream if an error occurred
        if (TNG_fread(&str[i], sizeof(str[0]), 1, stream) <= 0){
            str[i] = 0;

            return NULL;
        }

        
        if (str[i] == '\n'){
            i++;

            break;
        }

        i++;
    }

    str[i] = 0;
    str[len - 1] = 0;

    return str;
}

int TNG_fputs( const char *str, FILE *stream ){
    size_t  len;
    int     count;


    if (str == NULL || stream == NULL)
        return EOF;

    len = strlen(str);
    count = TNG_fwrite(str, sizeof(str[0]), len, stream);

    if (count > 0)
        return count;

    else
        return EOF;
}

int TNG_getc( FILE* stream )
{
#ifndef TRANSGAMING_USE_PHYSFS
# ifndef SUPPORT_STREAM
    FileData *  f = (FileData *)stream;
    int         r;
    int         ch = 0;


	r = sceRead( f->fd, &ch, 1 );
    f->lastOp = r;
    f->eof |= (r < 0 ? 1 : 0);

	CHECK_ERROR(r, EOF)
	return ch;
# else
	int r, fd = fileno( stream );
	char ch;
	if (fd < FD_BIAS)
		return getc( stream );
	r = sceRead( fd - FD_BIAS, &ch, 1 );
	CHECK_ERROR(r, EOF)
	return ch;
# endif
#else
	int retvalue = 0;
	int err = TNG_fread( &retvalue, 1, 1, stream );
	if (err == EOF)
		return EOF;
	else
		return retvalue;
#endif
}

int TNG_fgetc( FILE *stream ){
#ifndef TRANSGAMING_USE_PHYSFS
# ifndef SUPPORT_STREAM
    FileData *  f = (FileData *)stream;
    int         r;
    int         ch = 0;


	r = sceRead( f->fd, &ch, 1 );
    f->lastOp = r;
    f->eof |= (r < 0 ? 1 : 0);

	CHECK_ERROR(r, EOF)
	return ch;
# else
	int r, fd = fileno( stream );
	char ch;
	if (fd < FD_BIAS)
		return getc( stream );
	r = sceRead( fd - FD_BIAS, &ch, 1 );
	CHECK_ERROR(r, EOF)
	return ch;
# endif
#else
	int retvalue = 0;
	int err = TNG_fread( &retvalue, 1, 1, stream );
	if (err == EOF)
		return EOF;
	else
		return retvalue;
#endif
}

int TNG_ungetc( int c, FILE *stream ){
    if (c == EOF)
        return EOF;

    
    TNG_fseek(stream, -1, SEEK_CUR);

    return c;
}

int TNG_feof( FILE *stream ){
#ifndef TRANSGAMING_USE_PHYSFS
# ifndef SUPPORT_STREAM
    FileData *  f = (FileData *)stream;

    return f->eof != 0;
# else
	int fd = fileno( stream );
	if (fd < FD_BIAS)
		return feof( stream );

    //no way to detect the error!
    return 0;
# endif
#else
    return PHYSFS_eof((PHYSFS_file *)stream);
#endif
}

int TNG_ferror( FILE* stream )
{
#ifndef TRANSGAMING_USE_PHYSFS
# ifndef SUPPORT_STREAM
    FileData *  f = (FileData *)stream;

    return f->lastOp < 0;
# else
	int fd = fileno( stream );
	if (fd < FD_BIAS)
		return ferror( stream );

    //no way to detect the error!
	return 0;
# endif
#else
    (void)stream;
    DEBUGPRINT("TNG_ferror(): PDK ferror called for file %x (specific error: '%s')\n", (int)stream, PHYSFS_getLastError());
	return 0;
#endif
}

void TNG_clearerr( FILE *stream ){
#ifndef TRANSGAMING_USE_PHYSFS
# ifndef SUPPORT_STREAM
    FileData *  f = (FileData *)stream;

    f->lastOp = 0;
    f->eof = 0;
# else
 	int fd = fileno( stream );
	if (fd < FD_BIAS)
		clearerr( stream );

    //nothing to do!
# endif
#else
    (void)stream;
    DEBUGPRINT("TNG_clearerr(): PDK clearerr called for file %x (current error: '%s')\n", (int)stream, PHYSFS_getLastError());
#endif
}

/*********************** File searching/enumeration ****************************/
#ifdef TRANSGAMING_USE_PHYSFS

# ifndef _MAX_PATH
#  define _MAX_PATH 260
# endif

const char *getfilename(const char *filename){
    int i = strlen(filename) - 1;


    for (; i >= 0; i--){
        if (filename[i] == '\\' || filename[i] == '/')
            return &filename[i + 1];
    }

    return filename;
}


char **TNG_findOpen(const char *dirName, char *dir, size_t len){
    char *  wildcard;


	if (!g_physfsInitialized)
		PS2InitializeFileIO();


    dirName = fixFilename(dirName);
    strncpy(dir, dirName, len);

    wildcard = (char *)getfilename(dir);


    //no wildcard => assume '*'
    if (wildcard == NULL){
        wildcard = "*.*";
    }
    else{
        //wildcard exists => terminate the directory name at the path seperator
        char *tmp = (char *)wildcard;

        tmp--;
        tmp[0] = 0;
    }
    

    return PHYSFSEXT_enumerateFilesWildcard(dir, wildcard, 0);
}

int TNG_findClose(void *list){
    if (list)
        PHYSFS_freeList(list);

    return 1;
}

int TNG_isDirectory(const char *dir){
	if (!g_physfsInitialized)
		PS2InitializeFileIO();


    dir = fixFilename(dir);

    return PHYSFS_isDirectory(dir);
}

int TNG_exists(const char *filename){
	if (!g_physfsInitialized)
		PS2InitializeFileIO();


    filename = fixFilename(filename);

    return PHYSFS_exists(filename);
}

unsigned long TNG_fileSize(const char *filename){
    PHYSFS_file *   fp;
    PHYSFS_sint64   size = 0;


    filename = fixFilename(filename);
    fp = PHYSFS_openRead(filename);

    if (fp){
        size = PHYSFS_fileLength(fp);

        PHYSFS_close(fp);
    }

    return (unsigned long)size;
}

int TNG_oneExists(const char *filename, char *buf, size_t len){
    char      dir[_MAX_PATH];
    char *    wildcard;


	if (!g_physfsInitialized)
		PS2InitializeFileIO();


    filename = fixFilename(filename);
    strncpy(dir, filename, _MAX_PATH);

    wildcard = (char *)getfilename(dir);


    //no wildcard => assume '*'
    if (wildcard == NULL){
        wildcard = "*.*";
    }
    else{
        //wildcard exists => terminate the directory name at the path seperator
        char *tmp = (char *)wildcard;

        tmp--;
        tmp[0] = 0;
    }
    

    return PHYSFSEXT_findFirstWildcardMatch(dir, wildcard, 0, buf, len);
}

#endif
