#include <stdio.h>
#include <stdlib.h>
#include "include/TNG_stdio_PSP.h"
#include <string.h>
#include "tgFile.h"
#include <stdarg.h>

//define  this symbol to allow files to be opened for writing using the normal
//  stdio method.  If undefined, physfs will attempt to open the files for
//  writing.
//#define PHYSFS_STDIO_FILE_WRITES

//define this symbol to use PhysFS from the DVD/UMD drive.  If undefined, 
//  the file access will come from host0.
#ifdef TRANSGAMING_MASTERING
#define PHYSFS_UMD
#endif

//#define USE_DEBUGPRINT
#ifdef USE_DEBUGPRINT
# define DEBUGPRINT printf
#else
# define DEBUGPRINT(fmt, args...) {}
#endif

#ifdef TRANSGAMING_USE_PHYSFS
#include "physfs.h"
#include "extras/globbing.h"
#endif


/*#ifdef PHYSFS_STDIO_FILE_WRITES
typedef struct{
    union{
        PHYSFS_file *   physfs_file;
        FILE *          stdio_file;
    };
    int                 use_stdio;
} FileDescriptor;
#endif//*/


#ifdef TRANSGAMING_USE_PHYSFS
static int g_physfsInitialized = 0;


static int PSPInitializeFileIO()
{
    if (g_physfsInitialized)
        return 1;

# ifdef PHYSFS_UMD
	tgFileInit( "/PSP_GAME/USRDIR", NULL, 0);
	tgFileSelectMedium(TG_FILE_MEDIUM_UMD);
# else
	tgFileInit( NULL, NULL, 0);
	tgFileSelectMedium(TG_FILE_MEDIUM_DEVKIT);
# endif

    if (!PHYSFS_init(NULL)){
        DEBUGPRINT("PSPInitializeFileIO(): ERROR-> could not initialize file system (specific error: '%s')\n", PHYSFS_getLastError());

        exit(-1);
    }


    if (!PHYSFS_addToSearchPath("PSPROOT.ZIP", 1)){
        DEBUGPRINT("PSPInitializeFileIO(): ERROR-> could not register main data volume (specific error: '%s')\n", PHYSFS_getLastError());

        exit(-2);
    }
    if (!PHYSFS_addToSearchPath("PSPSCRPT.ZIP", 1)){
        DEBUGPRINT("PSPInitializeFileIO(): WARNING-> could not register scripts data volume (specific error: '%s')... assuming scripts are in 'root.zip'\n", PHYSFS_getLastError());
    }

#ifndef PHYSFS_UMD

    if (!PHYSFS_setWriteDir(".")){
        DEBUGPRINT("PSPInitializeFileIO(): ERROR-> could not set the writing directory (specific error: '%s')\n", PHYSFS_getLastError());

        exit(-3);
    }
#endif

    PHYSFS_setCaseSensitivity(0);


    g_physfsInitialized = 1;
    return 1;
}

const char *fixFilename(const char *filename){
    if (filename[0] == '.' && filename[1] == '.' && filename[2] == '/')
        return &filename[3];

    return filename;
}

#endif

// This file will allow us to redirect any stdio calls we wish.

FILE* TNG_fopen( const char* filename, const char* mode )
{
#ifndef TRANSGAMING_USE_PHYSFS
	char filename_on_host[256];
	sprintf( filename_on_host, "%s%s", "host0:", filename );
	//printf("TNG_fopen ---> filename = %s | filename_on_host = %s\n", filename, filename_on_host);

	return fopen( filename_on_host, mode );
#else
	if (!g_physfsInitialized)
		PSPInitializeFileIO();
	

	PHYSFS_file *fd;
    

    filename = fixFilename(filename);


    if (strchr(mode,'+') || strchr(mode,'r')){
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
	return fclose( stream );
#else
	int ret = 0;
	int err = PHYSFS_close((PHYSFS_file *)stream);
	

    if (err == 0){
        DEBUGPRINT("TNG_fclose(): error closing the stream %x (specific error: '%s')\n", (int)stream, PHYSFS_getLastError());

		ret = EOF;
    }
	
    DEBUGPRINT("TNG_fclose(): PDK Close on %x returns %d\n", (int)stream, ret);
	return ret;
#endif
}

int TNG_fprintf(FILE *stream, const char *fmt, ...){
#ifndef TRANSGAMING_USE_PHYSFS
    va_list args;


    va_start(args, fmt);
    int result = vfprintf(stream, fmt, args);
    va_end(args);

    return result;
#else
    const int   BUFSIZE = 4096;
    char        buf[BUFSIZE];
    va_list     args;
    int         len;

    
    va_start(args, fmt);
    len = vsnprintf(buf, BUFSIZE, fmt, args);
    va_end(args);


    if (len < 0){
        len = BUFSIZE;

#ifdef USE_DEBUGPRINT
        char brake = buf[BUFSIZE - 1];

        buf[BUFSIZE - 1] = 0;
        DEBUGPRINT("TNG_fprintf(): ERROR-> string output overflowed and will be truncated at %d characters\n    string = '%s'", BUFSIZE, buf);
        buf[BUFSIZE - 1] = brake;
#endif
    }


	PHYSFS_sint64 bytesWritten = PHYSFS_write((PHYSFS_file *)stream, buf, sizeof(buf[0]), len);

    //DEBUGPRINT("TNG_fwrite(): PDK Write %d bytes in file %x\n", (unsigned int)bytesWritten, (int)stream);


	return (int)bytesWritten;
#endif
}



int TNG_fread( void* buffer, size_t size, size_t count, FILE* stream )
{
#ifndef TRANSGAMING_USE_PHYSFS
	return fread( buffer, size, count, stream );
#else
    PHYSFS_sint64 bytesRead = PHYSFS_read((PHYSFS_file *)stream, buffer, size, count);

    //DEBUGPRINT("TNG_fread(): PDK Read %d bytes in file %x\n", ((unsigned int)bytesRead), (unsigned int)stream);


    return (int)bytesRead;
#endif
}

size_t TNG_fwrite( const void* buffer, size_t size, size_t count, FILE* stream )
{
#ifndef TRANSGAMING_USE_PHYSFS
	return fwrite( buffer, size, count, stream );
#else
	PHYSFS_sint64 bytesWritten = PHYSFS_write((PHYSFS_file *)stream, buffer, size, count);

    //DEBUGPRINT("TNG_fwrite(): PDK Write %d bytes in file %x\n", (unsigned int)bytesWritten, (int)stream);


	return (int)bytesWritten;
#endif
}

int TNG_fflush( FILE* stream )
{
#ifndef TRANSGAMING_USE_PHYSFS
	return fflush( stream );
#else
	return PHYSFS_flush((PHYSFS_file*)stream);
#endif
}

int TNG_fseek( FILE* stream, long offset, int origin )
{
#ifndef TRANSGAMING_USE_PHYSFS
	return fseek( stream, offset, origin );
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
	return fsetpos( stream, position );
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
	return ftell( stream );
#else
    int pos = PHYSFS_tell((PHYSFS_file *)stream);

    DEBUGPRINT("TNG_ftell(): PDK Tell for file %x is %d\n", (int)stream, pos);


    return pos;
#endif
}

char *TNG_fgets( char *str, int len, FILE *stream ){
#ifndef TRANSGAMING_USE_PHYSFS
    return fgets(str, len, stream);
#else
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
#endif
}

int TNG_fputs( const char *str, FILE *stream ){
#ifndef TRANSGAMING_USE_PHYSFS
    return fputs(str, stream);
#else
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
#endif
}

int TNG_getc( FILE* stream )
{
#ifndef TRANSGAMING_USE_PHYSFS
	return getc( stream );
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
	return fgetc( stream );
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
#ifndef TRANSGAMING_USE_PHYSFS
    return ungetc(c, stream);
#else
    if (c == EOF)
        return EOF;

    
    TNG_fseek(stream, -1, SEEK_CUR);

    return c;
#endif
}

int TNG_feof( FILE *stream ){
#ifndef TRANSGAMING_USE_PHYSFS
    return feof(stream);
#else
    return PHYSFS_eof((PHYSFS_file *)stream);
#endif
}

int TNG_ferror( FILE* stream )
{
#ifndef TRANSGAMING_USE_PHYSFS
	return ferror( stream );
#else
    (void)stream;
    DEBUGPRINT("TNG_ferror(): PDK ferror called for file %x (specific error: '%s')\n", (int)stream, PHYSFS_getLastError());
	return 0;
#endif
}

void TNG_clearerr( FILE *stream ){
#ifndef TRANSGAMING_USE_PHYSFS
    return clearerr(stream);
#else
    (void)stream;
    DEBUGPRINT("TNG_clearerr(): PDK clearerr called for file %x (current error: '%s')\n", (int)stream, PHYSFS_getLastError());
#endif
}

/*********************** File searching/enumeration ****************************/
#ifdef TRANSGAMING_USE_PHYSFS

#ifndef _MAX_PATH
# define _MAX_PATH 260
#endif

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
		PSPInitializeFileIO();


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
		PSPInitializeFileIO();


    dir = fixFilename(dir);

    return PHYSFS_isDirectory(dir);
}

int TNG_exists(const char *filename){
	if (!g_physfsInitialized)
		PSPInitializeFileIO();


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
		PSPInitializeFileIO();


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
