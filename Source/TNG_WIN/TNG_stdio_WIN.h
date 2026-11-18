#ifndef TNG_STDIO_WIN_H_
#define TNG_STDIO_WIN_H_

#ifdef __cplusplus
extern "C" {
#endif

FILE *          TNG_fopen( const char* filename, const char* mode );
int             TNG_fclose( FILE* stream );
int             TNG_fread( void* buffer, size_t size, size_t count, FILE* stream );
size_t          TNG_fwrite( const void* buffer, size_t size, size_t count, FILE* stream );
int             TNG_fflush( FILE* stream );
int             TNG_fseek( FILE* stream, long32 offset, int origin );
int             TNG_fsetpos( FILE* stream, const fpos_t* position );
long32            TNG_ftell( FILE* stream );
char *          TNG_fgets( char *str, int len, FILE *stream );
int             TNG_fputs( const char *str, FILE *stream );
int             TNG_fgetc( FILE *stream );
int             TNG_getc( FILE* stream );
int             TNG_ungetc( int c, FILE *stream );
int             TNG_feof( FILE *stream );
int             TNG_ferror( FILE* stream );
void            TNG_clearerr( FILE *stream );
int             TNG_fprintf( FILE *stream, const char *fmt, ... );


//file searching functions
#ifdef TRANSGAMING_USE_PHYSFS
char **         TNG_findOpen(const char *dirName, char *buf, size_t len);
int             TNG_findClose(void *list);
ulong32 TNG_fileSize(const char *filename);
int             TNG_isDirectory(const char *dir);
int             TNG_exists(const char *filename);
int             TNG_oneExists(const char *filename, char *buf, size_t len);
#endif

#ifdef __cplusplus
}
#endif

#endif // TNG_STDIO_WIN_H_