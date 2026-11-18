#ifndef STDIO_MULTISTREAM_H
#define STDIO_MULTISTREAM_H


#ifdef  __cplusplus
extern "C" {
#endif

typedef struct 
{
	unsigned int mPosition;
	unsigned int mSize;
	unsigned int mID;
} ms_FILE;

extern ms_FILE*		ms_stderr;

extern void			ms_init(char *);
extern char*		ms_getdeviceprefix();

extern ms_FILE *	ms_fopen(const char *, const char *);
extern int			ms_fclose(ms_FILE *);
extern unsigned int	ms_fread(void *, unsigned int, unsigned int, ms_FILE *);
extern unsigned int	ms_fwrite(void *, unsigned int, unsigned int, ms_FILE *);
extern int			ms_fseek(ms_FILE *, long , int );
extern long			ms_ftell(ms_FILE *);

extern void			ms_printf(const char *msg,...);
extern void			ms_fprintf(ms_FILE *,const char *msg,...);

// support for other prototypes that would exist in <stdio.h>
#if !defined(STDIO_MS_IMPLEMENTATION) && !defined(USE_SYSTEM_STDIO)
extern int vsprintf(char *,const char*, void*);
extern int sprintf(char *,const char *, ...);
extern int sscanf(const char *,const char *, ...);
#define feof(_stream)     ((_stream)->_flag & _IOEOF)
#define getchar()         getc(stdin)
#define putchar(_c)       putc((_c),stdout)
#endif //!STDIO_MS_IMPLEMENTATION && USE_SYSTEM_STDIO

#ifdef  __cplusplus
}
#endif


#if !defined(STDIO_MS_IMPLEMENTATION) && !defined(USE_SYSTEM_STDIO)
#define FILE	                            ms_FILE
#define stderr	                            ms_stderr
#define fopen	                            ms_fopen
#define fclose	                            ms_fclose
#define fseek	                            ms_fseek
#define ftell								ms_ftell
#define fread								ms_fread
#define fwrite								ms_fwrite  
#define printf								ms_printf
#define fprintf								ms_fprintf
#endif

#ifndef USE_SYSTEM_STDIO
#define	FOPEN_MAX	20	/* must be <= OPEN_MAX <sys/syslimits.h> */
#define EOF     (-1)
#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0
#endif


#endif  /* STDIO_MULTISTREAM_H */
