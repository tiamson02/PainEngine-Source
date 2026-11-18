/*
 * Posix-esque support routines for PhysicsFS.
 *
 * Please see the file LICENSE in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#if (!defined WIN32)

#if (defined __STRICT_ANSI__)
#define __PHYSFS_DOING_STRICT_ANSI__
#endif

/*
 * We cheat a little: I want the symlink version of stat() (lstat), and
 *  GCC/Linux will not declare it if compiled with the -ansi flag.
 *  If you are really lacking symlink support on your platform,
 *  you should #define __PHYSFS_NO_SYMLINKS__ before compiling this
 *  file. That will open a security hole, though, if you really DO have
 *  symlinks on your platform; it renders PHYSFS_permitSymbolicLinks(0)
 *  useless, since every symlink will be reported as a regular file/dir.
 */
#if (defined __PHYSFS_DOING_STRICT_ANSI__)
#undef __STRICT_ANSI__
#endif
#include <stdio.h>
#if (defined __PHYSFS_DOING_STRICT_ANSI__)
#define __STRICT_ANSI__
#endif

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#if defined (TARGET_PS2)
# include <sifdev.h>
# include <shellCore.h>
# include <shellFile.h>
#elif defined (TARGET_PSP)
# include <iofilemgr.h>
# include <iofilemgr_stat.h>
# include <kerror.h>
# include "tgFile.h"
#else
# include <dirent.h>
#endif

#include <errno.h>
#include <fcntl.h>

#ifdef PHYSFS_HAVE_LLSEEK
#include <linux/unistd.h>
#endif

#define __PHYSICSFS_INTERNAL__
#include "physfs_internal.h"


#ifndef TRANSGAMING
char *__PHYSFS_platformCopyEnvironmentVariable(const char *varname)
{
#if !defined (TARGET_PS2) && !defined(TARGET_PSP)
    const char *envr = getenv(varname);
    char *retval = NULL;

    if (envr != NULL)
    {
        retval = (char *) malloc(strlen(envr) + 1);
        if (retval != NULL)
            strcpy(retval, envr);
    } /* if */
	
    return(retval);
#else
    (void)varname;

    return NULL;
#endif
} /* __PHYSFS_platformCopyEnvironmentVariable */
#endif


static char *getUserNameByUID(void)
{
#if !defined (TARGET_PS2) && !defined(TARGET_PSP)
    uid_t uid = getuid();
    struct passwd *pw;
    char *retval = NULL;

    pw = getpwuid(uid);
    if ((pw != NULL) && (pw->pw_name != NULL))
    {
        retval = (char *) malloc(strlen(pw->pw_name) + 1);
        if (retval != NULL)
            strcpy(retval, pw->pw_name);
    } /* if */

    return(retval);
#else
    char *name = "SonyPS2User";
    char *retval = (char *)malloc(strlen(name) + 1);


    strncpy(retval, name, strlen(name) + 1);

    return retval;
#endif
} /* getUserNameByUID */


static char *getUserDirByUID(void)
{
#if !defined (TARGET_PS2) && !defined(TARGET_PSP)
    uid_t uid = getuid();
    struct passwd *pw;
    char *retval = NULL;

    pw = getpwuid(uid);
    if ((pw != NULL) && (pw->pw_dir != NULL))
    {
        retval = (char *) malloc(strlen(pw->pw_dir) + 1);
        if (retval != NULL)
            strcpy(retval, pw->pw_dir);
    } /* if */
    
    return(retval);
#else
    char *name = "SonyPS2Dir";
    char *retval = (char *)malloc(strlen(name) + 1);


    strncpy(retval, name, strlen(name) + 1);

    return retval;
#endif
} /* getUserDirByUID */


char *__PHYSFS_platformGetUserName(void)
{
#ifdef TRANSGAMING
    return getUserNameByUID();
#else
    char *retval = getUserNameByUID();
    if (retval == NULL)
        retval = __PHYSFS_platformCopyEnvironmentVariable("USER");
    return(retval);
#endif
} /* __PHYSFS_platformGetUserName */


char *__PHYSFS_platformGetUserDir(void)
{
#ifdef TRANSGAMING
    return getUserDirByUID();
#else
    char *retval = __PHYSFS_platformCopyEnvironmentVariable("HOME");
    if (retval == NULL)
        retval = getUserDirByUID();
    return(retval);
#endif
} /* __PHYSFS_platformGetUserDir */


/* -ansi and -pedantic flags prevent use of strcasecmp() on Linux. */
int __PHYSFS_platformStricmp(const char *x, const char *y)
{
    int ux, uy;

    do
    {
        ux = toupper((int) *x);
        uy = toupper((int) *y);
        if (ux != uy)
            return((ux > uy) ? 1 : -1);
        x++;
        y++;
    } while ((ux) && (uy));

    return(0);
} /* __PHYSFS_platformStricmp */


int __PHYSFS_platformStrnicmp(const char *x, const char *y, PHYSFS_uint32 len)
{
    int ux, uy;

    if (!len)
        return(0);

    do
    {
        ux = toupper((int) *x);
        uy = toupper((int) *y);
        if (ux != uy)
            return((ux > uy) ? 1 : -1);
        x++;
        y++;
        len--;
    } while ((ux) && (uy) && (len));

    return(0);
} /* __PHYSFS_platformStrnicmp */


#if (defined __PHYSFS_NO_SYMLINKS__)
#define doStat stat
#elif defined(TARGET_PS2) || defined (TARGET_PSP)
#define __PHYSFS_NO_SYMLINKS__
#else
#define doStat lstat
#endif

int __PHYSFS_platformExists(const char *fname)
{
#ifdef TARGET_PS2
    struct sce_stat statbuf;
    int exists = shellStatFile(fname, &statbuf);
    DEBUGPRINT("__PHYSFS_platformExists{PS2}(): '%s' %s (exists = %d)\n", fname, exists >= 0 ? "exists" : "doesn't exist", exists);
    BAIL_IF_MACRO(exists < 0, strerror(errno), 0);
#elif defined (TARGET_PSP)
    struct SceIoStat statbuf;
    int exists = tgStatFile(fname, &statbuf);
    //int exists = sceIoGetstat(fname, &statbuf);
    DEBUGPRINT("__PHYSFS_platformExists{PSP}(): '%s' %s (exists = %d)\n", fname, exists >= 0 ? "exists" : "doesn't exist", exists);
    BAIL_IF_MACRO(exists < 0, strerror(errno), 0);
#else
    struct stat statbuf;
    BAIL_IF_MACRO(doStat(fname, &statbuf) == -1, strerror(errno), 0);
#endif
    return(1);
} /* __PHYSFS_platformExists */


int __PHYSFS_platformIsSymLink(const char *fname)
{
#if (defined __PHYSFS_NO_SYMLINKS__)
    (void)fname;

    return(0);
#else
    struct stat statbuf;
    BAIL_IF_MACRO(lstat(fname, &statbuf) == -1, strerror(errno), 0);
    return( (S_ISLNK(statbuf.st_mode)) ? 1 : 0 );
#endif
} /* __PHYSFS_platformIsSymlink */


int __PHYSFS_platformIsDirectory(const char *fname)
{
#ifdef TARGET_PS2
    struct sce_stat statbuf;
    int err = shellStatFile(fname, &statbuf);
    DEBUGPRINT("__PHYSFS_platformIsDirectory{PS2}(): '%s' %s a directory (err = %d)\n", fname, SCE_STM_ISDIR(statbuf.st_mode) ? "is" : "is not", err);
    BAIL_IF_MACRO(err < 0, strerror(errno), 0);
    return (SCE_STM_ISDIR(statbuf.st_mode));
#elif defined (TARGET_PSP)
    struct SceIoStat statbuf;
    int err = tgStatFile(fname, &statbuf);
    //int err = sceIoGetstat(fname, &statbuf);
    DEBUGPRINT("__PHYSFS_platformIsDirectory{PSP}(): '%s' %s a directory (err = %d)\n", fname, SCE_STM_ISDIR(statbuf.st_mode) ? "is" : "is not", err);
    BAIL_IF_MACRO(err < 0, strerror(errno), 0);
    return (SCE_STM_ISDIR(statbuf.st_mode));
#else
	struct stat statbuf;
    BAIL_IF_MACRO(stat(fname, &statbuf) == -1, strerror(errno), 0);
    return( (S_ISDIR(statbuf.st_mode)) ? 1 : 0 );
#endif
} /* __PHYSFS_platformIsDirectory */


char *__PHYSFS_platformCvtToDependent(const char *prepend,
                                      const char *dirName,
                                      const char *append)
{
    int len = ((prepend) ? strlen(prepend) : 0) +
              ((append) ? strlen(append) : 0) +
              strlen(dirName) + 1;
    char *retval = (char *) malloc(len);

    BAIL_IF_MACRO(retval == NULL, ERR_OUT_OF_MEMORY, NULL);

    /* platform-independent notation is Unix-style already.  :)  */

    if (prepend)
        strcpy(retval, prepend);
    else
        retval[0] = '\0';

    strcat(retval, dirName);

    if (append)
        strcat(retval, append);

    return(retval);
} /* __PHYSFS_platformCvtToDependent */



LinkedStringList *__PHYSFS_platformEnumerateFiles(const char *dirname,
                                                  int omitSymLinks)
{
    LinkedStringList *retval = NULL, *p = NULL;
#ifdef TARGET_PS2

    int dir;
    struct sce_dirent  localent;
    struct sce_dirent *ent = &localent;
#elif defined (TARGET_PSP)
    SceUID dir;
    struct SceIoDirent  localent;
    struct SceIoDirent *ent = &localent;
#else
	DIR *dir;
    struct dirent *ent;
#endif

    int bufsize = 0;
    char *buf = NULL;
    int dlen = 0;

    if (omitSymLinks)
    {
        dlen = strlen(dirname);
        bufsize = dlen + 256;
        buf = (char *) malloc(bufsize);
        BAIL_IF_MACRO(buf == NULL, ERR_OUT_OF_MEMORY, NULL);
        strcpy(buf, dirname);
        if (buf[dlen - 1] != '/')
        {
            buf[dlen++] = '/';
            buf[dlen] = '\0';
        } /* if */
    } /* if */

    errno = 0;
#ifdef TARGET_PS2
    dir = shellDopen(dirname);
    if (dir < 0)
#elif defined (TARGET_PSP)
    dir = tgDopen(dirname);
    //dir = sceIoDopen(dirname);
    if (dir < 0)
#else
	dir = opendir(dirname);
    if (dir == NULL)
#endif
    {
        if (buf != NULL)
            free(buf);
        BAIL_IF_MACRO(1, strerror(errno), NULL);
    } /* if */

#ifdef TARGET_PS2
    while (shellDread(dir, ent) > 0)
#elif defined (TARGET_PSP)
    while (tgDread(dir, ent) > 0)
    //while (sceIoDread(dir, ent) > 0)
#else
	while ((ent = readdir(dir)) != NULL)
#endif
    {
        if (strcmp(ent->d_name, ".") == 0)
            continue;

        if (strcmp(ent->d_name, "..") == 0)
            continue;

        if (omitSymLinks)
        {
            char *p;
            int len = strlen(ent->d_name) + dlen + 1;
            if (len > bufsize)
            {
                p = realloc(buf, len);
                if (p == NULL)
                    continue;
                buf = p;
                bufsize = len;
            } /* if */

            strcpy(buf + dlen, ent->d_name);
            if (__PHYSFS_platformIsSymLink(buf))
                continue;
        } /* if */

        retval = __PHYSFS_addToLinkedStringList(retval, &p, ent->d_name, -1);
    } /* while */

    if (buf != NULL)
        free(buf);

#ifdef TARGET_PSP
    tgDclose(dir);
    //sceIoDclose(dir);
#elif defined (TARGET_PS2)
    shellDclose(dir);
#else
	closedir(dir);
#endif
    return(retval);
} /* __PHYSFS_platformEnumerateFiles */


char *__PHYSFS_platformCurrentDir(void)
{
#if !defined ( TARGET_PS2) && !defined(TARGET_PSP)
    int allocSize = 0;
    char *retval = NULL;
    char *ptr;

    do
    {
        allocSize += 100;
        ptr = (char *) realloc(retval, allocSize);
        if (ptr == NULL)
        {
            if (retval != NULL)
                free(retval);
            BAIL_MACRO(ERR_OUT_OF_MEMORY, NULL);
        } /* if */

        retval = ptr;
        ptr = getcwd(retval, allocSize);
    } while (ptr == NULL && errno == ERANGE);

    if (ptr == NULL && errno)
    {
            /*
             * getcwd() failed for some reason, for example current
             * directory not existing.
             */
        if (retval != NULL)
            free(retval);
        BAIL_MACRO(ERR_NO_SUCH_FILE, NULL);
    } /* if */

    return(retval);
#else
    BAIL_MACRO(ERR_NO_SUCH_FILE, NULL);
#endif
} /* __PHYSFS_platformCurrentDir */



int __PHYSFS_platformMkDir(const char *path)
{
#if !defined ( TARGET_PS2) && !defined(TARGET_PSP)
    int rc;
    errno = 0;
    rc = mkdir(path, S_IRWXU);
    BAIL_IF_MACRO(rc == -1, strerror(errno), 0);
    return(1);
#else
    (void)path;

    return (0);
#endif
} /* __PHYSFS_platformMkDir */

typedef struct {
/*#ifdef TARGET_PSP
TgFileHandle fh;
#else*/
int    fd;
/*#endif
int    filesize;*/
int    curpos;
int    bufstart;
int    bufend;
char   *buf;
} ps2FileBuf;
#define kPS2CacheSize     8192

static void *doOpen(const char *filename, int mode)
{
    int fd;
    int *retval;
    errno = 0;

#if !defined ( TARGET_PS2) && !defined (TARGET_PSP)
    fd = open(filename, mode, S_IRUSR | S_IWUSR);
    BAIL_IF_MACRO(fd < 0, strerror(errno), NULL);

    retval = (int *) malloc(sizeof (int));
#else
    fd = -1;
    if (! __PHYSFS_platformIsDirectory(filename))
    {
# ifdef TARGET_PS2
        fd = shellIoOpen(filename, mode, 0666);
# else
		//fd = sceIoOpen(filename, mode, 0666);
        fd = tgIoOpen(filename, mode, 0666);
# endif
        DEBUGPRINT("doOpen(): opened '%s' through handle %d\n", filename, fd);
    }
    BAIL_IF_MACRO(fd < 0, strerror(errno), NULL);

    retval = (int *) calloc(sizeof (ps2FileBuf), 1);
    if (retval)
    {
        ((ps2FileBuf*)retval)->fd =         fd;
        ((ps2FileBuf*)retval)->curpos =     0;
        ((ps2FileBuf*)retval)->bufstart =   0;
        ((ps2FileBuf*)retval)->bufend =     0;
        ((ps2FileBuf*)retval)->buf =        calloc(kPS2CacheSize, 1);
    }
#endif
    if (retval == NULL)
    {
#ifdef TARGET_PS2
		sceClose(fd);
#elif defined (TARGET_PSP)
		sceIoClose(fd);
#else
		close(fd);
#endif
        BAIL_MACRO(ERR_OUT_OF_MEMORY, NULL);
    } /* if */

#if !defined (TARGET_PS2) && !defined (TARGET_PSP)
    *retval = fd;
#endif
    return((void *) retval);
} /* doOpen */


void *__PHYSFS_platformOpenRead(const char *filename)
{

#if defined (TARGET_PS2)
    return(doOpen(filename, SCE_RDONLY));
#elif defined(TARGET_PSP)
	return(doOpen(filename, SCE_O_RDONLY));
#else
	return(doOpen(filename, O_RDONLY));
#endif
} /* __PHYSFS_platformOpenRead */


void *__PHYSFS_platformOpenWrite(const char *filename)
{

#if defined ( TARGET_PS2) 
    return (doOpen(filename, SCE_WRONLY | SCE_CREAT | SCE_TRUNC));
#elif defined(TARGET_PSP)
    return (doOpen(filename, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC));
#else
	return(doOpen(filename, O_WRONLY | O_CREAT | O_TRUNC));
#endif
} /* __PHYSFS_platformOpenWrite */


void *__PHYSFS_platformOpenAppend(const char *filename)
{
#if defined ( TARGET_PS2)
    return(doOpen(filename, SCE_WRONLY | SCE_CREAT | SCE_APPEND));
#elif defined (TARGET_PSP)
    return(doOpen(filename, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND));
#else
    return(doOpen(filename, O_WRONLY | O_CREAT | O_APPEND));
#endif
} /* __PHYSFS_platformOpenAppend */


PHYSFS_sint64 __PHYSFS_platformRead(void *opaque, void *buffer,
                                    PHYSFS_uint32 size, PHYSFS_uint32 count)
{
    int fd = *((int *) opaque);
    int max = size * count;
#if !defined ( TARGET_PS2) && !defined(TARGET_PSP)
    int rc = read(fd, buffer, max);
#else
/* The caching mechanism is very simple (despite what it looks). If there is a caching buffer
 * allocated, then we try to use it. If what we need is entirely in the buffer, we read it there.
 * If not entirely in the cache but it can fit entirely in the cache, then we read it in the cache
 * and copy it back to the caller's buffer. If it's bigger than the cache, we entirely bypass the cache
 * and read it straight into the caller's buffer.
 * Seek and Tell operations are entirely cached until a real seek is required (ie a real read or write).
 * I currently don't cache the physical filepos, but that might help preventing some of those seeks on 
 * sequential read or writes. This should be investigated later.
 * */
    if (max < 0){
        DEBUGPRINT("__PHYSFS_platformRead(): ERROR-> invalid read count! (max = %d, size = %d, count = %d)\n", max, size, count);

        return 0;
    }


    int rc = 0;
    /* first we look to see if it's all in the cache */
    ps2FileBuf* file = (ps2FileBuf*) opaque;
    if ((file->buf != NULL) && (file->curpos >= file->bufstart) && ( (file->curpos + max) <= file->bufend) )
    {
        memcpy(buffer, file->buf + file->curpos - file->bufstart, max);
        file->curpos += max;
        rc = max;
        VERYDEBUGPRINT("__PHYSFS_platformRead used cache and returned %d bytes\n", rc);
    } else
    {
        /* make sure the physical spot matches where it should be */
#ifdef TARGET_PS2
        sceLseek(fd, file->curpos, SEEK_SET);
#else
		sceIoLseek(fd, file->curpos, SEEK_SET);
#endif
        /* if not entirely in the cache, we need to read from the media. 
	 * if the read is too big to fit the cache, entirely bypass the cache, 
	 * which will keep old data in the cache
	 */
        if ((file->buf != NULL) && (max < kPS2CacheSize))
        {
            DEBUGPRINT("__PHYSFS_platformRead refilling the cache for %d\n", fd);
            file->bufstart = file->curpos;
#ifdef TARGET_PS2
            int readbytes = sceRead(fd, file->buf, kPS2CacheSize);
#else
            int readbytes = sceIoRead(fd, file->buf, kPS2CacheSize);
#endif
			file->bufend = file->bufstart + readbytes;
			rc = max;
			if (max > readbytes)
                rc = readbytes;
			memcpy(buffer, file->buf, rc);
            DEBUGPRINT("__PHYSFS_platformRead used the new cache content of %d for %d bytes\n", fd, rc);
        } else
        {
            DEBUGPRINT("__PHYSFS_platformRead(): attempting to read %d bytes (size = %d, count = %d) from stream %d\n", max, size, count, fd);
#ifdef TARGET_PS2
			rc = sceRead(fd, buffer, max);
#else
			rc = sceIoRead(fd, buffer, max);
#endif
            DEBUGPRINT("__PHYSFS_platformRead bypassed all caching on %d and read  %d bytes\n", fd, rc);
        }
        file->curpos += rc;
        DEBUGPRINT("__PHYSFS_platformRead on %d returned %d\n", fd, rc);
    }
#endif

    BAIL_IF_MACRO(rc < 0, strerror(errno), rc);
    assert(rc <= max);

#ifndef TARGET_PS2
    if ((rc < max) && (size > 1))
        lseek(fd, -(rc % size), SEEK_CUR); /* rollback to object boundary. */
#endif

    return(rc / size);
} /* __PHYSFS_platformRead */


PHYSFS_sint64 __PHYSFS_platformWrite(void *opaque, const void *buffer,
                                     PHYSFS_uint32 size, PHYSFS_uint32 count)
{
    int fd = *((int *) opaque);
    int max = size * count;
#ifdef TARGET_PS2
    ps2FileBuf* file = (ps2FileBuf*) opaque;
    /* first we make sure the physical spot matches where it should be */
    sceLseek(fd, file->curpos, SEEK_SET);
    
    int rc = sceWrite(fd, (void *) buffer, max);
    file->curpos += rc;
    VERYDEBUGPRINT("__PHYSFS_platformWrite on %d returned %d\n", fd, rc);
#elif defined (TARGET_PSP)
    ps2FileBuf* file = (ps2FileBuf*) opaque;
    /* first we make sure the physical spot matches where it should be */
    sceIoLseek(fd, file->curpos, SEEK_SET);
    
    int rc = sceIoWrite(fd, (void *) buffer, max);
    file->curpos += rc;
    VERYDEBUGPRINT("__PHYSFS_platformWrite on %d returned %d\n", fd, rc);
#else
	int rc = write(fd, (void *) buffer, max);
#endif
    
    BAIL_IF_MACRO(rc < 0, strerror(errno), rc);
    assert(rc <= max);

    if ((rc < max) && (size > 1))
        lseek(fd, -(rc % size), SEEK_CUR); /* rollback to object boundary. */

    return(rc / size);
} /* __PHYSFS_platformWrite */


int __PHYSFS_platformSeek(void *opaque, PHYSFS_uint64 pos)
{
    int fd = *((int *) opaque);

    #ifdef PHYSFS_HAVE_LLSEEK
      unsigned long offset_high = ((pos >> 32) & 0xFFFFFFFF);
      unsigned long offset_low = (pos & 0xFFFFFFFF);
      loff_t retoffset;
      int rc = llseek(fd, offset_high, offset_low, &retoffset, SEEK_SET);
      BAIL_IF_MACRO(rc == -1, strerror(errno), 0);
    #else
      #if !defined(TARGET_PS2) && !defined (TARGET_PSP)
      BAIL_IF_MACRO(lseek(fd, (int) pos, SEEK_SET) == -1, strerror(errno), 0);
      #else
      /* simply use the cache mechanism for filepos */
      ps2FileBuf* file = (ps2FileBuf*) opaque;
      file->curpos = pos;
      fd = 0;
      #endif
    #endif

    return(1);
} /* __PHYSFS_platformSeek */


PHYSFS_sint64 __PHYSFS_platformTell(void *opaque)
{
    int fd = *((int *) opaque);
    PHYSFS_sint64 retval;

    #ifdef PHYSFS_HAVE_LLSEEK
      loff_t retoffset;
      int rc = llseek(fd, 0, &retoffset, SEEK_CUR);
      BAIL_IF_MACRO(rc == -1, strerror(errno), -1);
      retval = (PHYSFS_sint64) retoffset;
    #else
#if !defined(TARGET_PS2) && !defined (TARGET_PSP)
      retval = (PHYSFS_sint64) lseek(fd, 0, SEEK_CUR);
      BAIL_IF_MACRO(retval == -1, strerror(errno), -1);
#else
      /* simply use the cache mechanism for filepos */
      ps2FileBuf* file = (ps2FileBuf*) opaque;
      retval = file->curpos;
      BAIL_IF_MACRO(retval < 0, strerror(errno), -1);
      fd = 0;
#endif
    VERYDEBUGPRINT("__PHYSFS_platformTell on %d returned %d\n", fd, (int)retval);
    #endif

    return(retval);
} /* __PHYSFS_platformTell */


PHYSFS_sint64 __PHYSFS_platformFileLength(void *opaque)
{
    int fd = *((int *) opaque);
#ifdef TARGET_PS2
    /* this version exists only because this API receives only the fd as input, 
     * not the file name, as sceGetstat requires. However. we could embed the 
     * file name in the file cache and be able to use a more strightforward API.
     * FileLength has not been seen as a major performance issue so far */
    int currentOffset;
    currentOffset = sceLseek(fd, 0, SEEK_CUR);     /* save current position       */
    sceLseek(fd, 0, SEEK_END);                     /* seek to the end of the file */
    int fileSize = sceLseek(fd, 0, SEEK_CUR);      /* get the file size           */
    sceLseek(fd, currentOffset, SEEK_SET);         /* seek back to our old offset */
    DEBUGPRINT("__PHYSFS_platformFileLength(): file with handle %d has size %d\n", fd, fileSize);
    return ((PHYSFS_sint64)fileSize);              /* success!                    */          
#elif defined (TARGET_PSP)
   /* this version exists only because this API receives only the fd as input, 
     * not the file name, as sceGetstat requires. However. we could embed the 
     * file name in the file cache and be able to use a more strightforward API.
     * FileLength has not been seen as a major performance issue so far */
    int currentOffset;
    currentOffset = sceIoLseek(fd, 0, SEEK_CUR);     /* save current position       */
    sceIoLseek(fd, 0, SEEK_END);                     /* seek to the end of the file */
    int fileSize = sceIoLseek(fd, 0, SEEK_CUR);      /* get the file size           */
    sceIoLseek(fd, currentOffset, SEEK_SET);         /* seek back to our old offset */
    DEBUGPRINT("__PHYSFS_platformFileLength(): file with handle %d has size %d\n", fd, fileSize);
    return ((PHYSFS_sint64)fileSize);              /* success!                    */          

#else
   struct stat statbuf;
    BAIL_IF_MACRO(fstat(fd, &statbuf) == -1, strerror(errno), -1);
    return((PHYSFS_sint64) statbuf.st_size);
#endif
} /* __PHYSFS_platformFileLength */


int __PHYSFS_platformEOF(void *opaque)
{
    PHYSFS_sint64 pos = __PHYSFS_platformTell(opaque);
    PHYSFS_sint64 len = __PHYSFS_platformFileLength(opaque);
    DEBUGPRINT("__PHYSFS_platformEOF(): file with handle %d %s at the end\n", *((int *) opaque), (pos >= len) ? "is" : "isn't");
    return(pos >= len);
} /* __PHYSFS_platformEOF */


int __PHYSFS_platformFlush(void *opaque)
{
#if !defined(TARGET_PS2) && !defined (TARGET_PSP)
    int fd = *((int *) opaque);
    BAIL_IF_MACRO(fsync(fd) == -1, strerror(errno), 0);
#else
    (void)opaque;
#endif
    return(1);
} /* __PHYSFS_platformFlush */


int __PHYSFS_platformClose(void *opaque)
{
    int fd = *((int *) opaque);
#ifdef TARGET_PS2

    DEBUGPRINT("__PHYSFS_platformClose(): closing file with handle %d\n", fd);
    BAIL_IF_MACRO(sceClose(fd) < 0, strerror(errno), 0);
    ps2FileBuf* file = (ps2FileBuf*) opaque;
    free(file->buf); 
#elif defined (TARGET_PSP)
    DEBUGPRINT("__PHYSFS_platformClose(): closing file with handle %d\n", fd);
    BAIL_IF_MACRO(sceIoClose(fd) < 0, strerror(errno), 0);
    ps2FileBuf* file = (ps2FileBuf*) opaque;
    free(file->buf); 
#else
    BAIL_IF_MACRO(close(fd) == -1, strerror(errno), 0);
#endif	
    free(opaque);
    return(1);
} /* __PHYSFS_platformClose */


int __PHYSFS_platformDelete(const char *path)
{
#if defined(TARGET_PS2) || defined (TARGET_PSP)
    printf("ERROR IN PLATFORM __PHYSFS_platformDelete not implemented\n");
#endif
    BAIL_IF_MACRO(remove(path) == -1, strerror(errno), 0);
    return(1);
} /* __PHYSFS_platformDelete */


PHYSFS_sint64 __PHYSFS_platformGetLastModTime(const char *fname)
{
    struct stat statbuf;
#if defined(TARGET_PS2) || defined (TARGET_PSP)
    printf("ERROR IN PLATFORM __PHYSFS_platformGetLastModTime not implemented\n");
#endif
    BAIL_IF_MACRO(stat(fname, &statbuf) < 0, strerror(errno), -1);
    return statbuf.st_mtime;
} /* __PHYSFS_platformGetLastModTime */

#endif /* !defined WIN32 */

/* end of posix.c ... */

