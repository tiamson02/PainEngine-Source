#include "ps2s/ps2_file_ops.h"

#include <ctype.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include "ps2s/dirent.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#include <iostream>

/********************************************
 * static data
 */

tMediaType	FileIo::Media = MediaType::kInvalid;
tHostType	FileIo::HostType = HostType::kInvalid;
std::string	FileIo::Prefix;

/********************************************
 * FileIo methods
 */

void
FileIo::SetMediaType(tMediaType type, const char *hostIp)
{
   Media = type;

   if (Media == MediaType::kHost0) {
      Prefix = "host0:";
   }
   else if (Media == MediaType::kAtmon) {
      Prefix = std::string("atfile:") + std::string(hostIp) + ",";
   }	 
   else if (Media == MediaType::kCdRom) {
      Prefix = "cdrom0:";
   }
}

void
FileIo::SetHostType(tHostType type)
{
   HostType = type;
}


// paths can be either relative or absolute, linux or windows format
std::string
FileIo::ConvertPath(const std::string &path)
{
   using namespace std;
   string newPath = path;
   
   // do cdrom-specific processing first
   if (Media == MediaType::kCdRom) {
      // strip drive specification from windows path
      if (path[1] == ':')
	 newPath.erase(0,2);

      // convert relative to absolute
      if (path[0] != '/')
	 newPath.insert(0, "/");

      // convert /'s to \'s and lower-case to upper-
      for (unsigned int i = 0; i < newPath.length(); i++) {
	 if (newPath[i] == '/')
	    newPath[i] = '\\';
	 newPath[i] = toupper(newPath[i]);
      }

      // and finally terminate with ";1"
      newPath += ";1";
   }
   // we're not loading from cdrom
   else {
      // if the host machine is running linux, convert \s to /s
      if (HostType == HostType::kLinux) {
	 for (unsigned int i = 0; i < newPath.length(); i++)
	    if (newPath[i] == '\\')
	       newPath[i] = '/';
      }
      // and if it's running windows, the other way around
      if (HostType == HostType::kWindows) {
	 for (unsigned int i = 0; i < newPath.length(); i++)
	    if (newPath[i] == '/')
	       newPath[i] = '\\';
      }
   }

   std::cout << "converting path : \"" << path << "\" -> \"" << Prefix + newPath << "\"\n";

   // now just add the prefix and we're done
   return string(Prefix + newPath);
}

char*
FileIo::ConvertPath(char *buffer, const char *file)
{
   std::string result = ConvertPath(std::string(file));
   strcpy(buffer, result.c_str());

   return buffer;
}

extern "C" {

/********************************************
 * data
 */

// static

static char buffer1[256];
// static char buffer2[256];

/********************************************
 * translate paths/files
 */

// char*
// GetFilename( char *buffer, const char *file )
// {
//    bool cdrom = (Media == MediaType::kCdRom);
//    bool host0 = (Media == MediaType::kHost0);
//    bool atmon = (Media == MediaType::kHost0);

//    if (Media == MediaType::kCdRom)
//       strcpy(buffer, "cdrom0:");
//    else if (Media == MediaType::kCdRom)

//    if ( file[0] != '/' ) {
//       strcat( buffer, ROOT_DIR );
//    }
//    const char *src = file;
//    char *dst = & buffer[strlen(buffer)];
//    for ( unsigned int i = 0; i < strlen(file); i++ ) {
//       if (Media == MediaType::kCdRom)
// 	 *dst = toupper(*src);
//       else
// 	 *dst = *src;

//       if ( *dst == '/' )
// 	 *dst = FileSeparator[Media];

//       src++;
//       dst++;
//    }
//    *dst = '\0';
//    if (Media == MediaType::kCdRom)
//       strcat( buffer, ";1" );

//    // printf("converting %s to %s\n", file, buffer );
//    return buffer;
// }

char*
GetFilename(char *buffer, const char *file)
{
   return FileIo::ConvertPath(buffer, file);
}

/********************************************
 * wrappers for functions that take a pathname that needs translation
 */

int
translate_result( int result )
{
   int newResult = 0; // be optimistic; 0 = success
   if ( result < 0 ) {
      newResult = -1;
      errno = -result; // sce functions return -(error_code)
   }

   if ( newResult == -1 )
      printf("*** returning error %d ***\n", -result);

   return newResult;
}

int __wrap_chdir( const char *name) {
   GetFilename(buffer1, name);
   return translate_result(sceChdir(buffer1));
};

//    int __wrap_chstat( const char *name, struct sce_stat *buf, unsigned int cbit) {
//       GetFilename(buffer1, name);
//       return sceChstat(buffer1,buf,cbit);
//    };

int __wrap_close(int fd) {
   // you would think that since there are sceClose and sceDclose functions
   // that we should check here whether fd is a file or dir... but let's
   // see if it just works first.
   return translate_result(sceClose(fd));
};

// inline int __wrap_devctl(const char *devname, int cmd, const void *arg,
// 		  unsigned int arglen, void *bufp, unsigned int buflen) {
// 	return sceDevctl(devname, cmd, arg, arglen, bufp, buflen);
// };

#define mTranslateOpenFlag( __open_flag, __sce_open_flag )	\
   if (flag & (__open_flag)) sce_flag |= (__sce_open_flag)

int __wrap_open(const char *filename, int flag, unsigned short mode) {
   GetFilename(buffer1, filename);

   // translate the flags to sceOpen format
   int sce_flag = 0;

   // first, the read/write mode requires special attention;
   // rdonly 0, wronly 1, rdwr 2
   int rw_mode = flag & 0x3;
   sce_flag |= (rw_mode + 1);

   // now the other flags
   mTranslateOpenFlag(O_NONBLOCK, SCE_NBLOCK);
   mTranslateOpenFlag(O_APPEND, SCE_APPEND);
   mTranslateOpenFlag(O_CREAT, SCE_CREAT );
   mTranslateOpenFlag(O_TRUNC, SCE_TRUNC );
   mTranslateOpenFlag(O_EXCL, SCE_EXCL  );

   int result = sceOpen(buffer1, sce_flag, mode);
   if ( result < 0 )
      result = translate_result(result);

   return result;
};

//    int __wrap_dopen( const char *dirname) {
//       GetFilename(buffer1, dirname);
//       return sceDopen (buffer1);
//    };

int __wrap_readdir(int fd, struct dirent *buf, unsigned int count)
{
   int result = sceDread(fd, (struct sce_dirent*)buf);
   // this function doesn't return 0 on success like the others
   // so we have to be careful with translate_result()

   int new_result = 0; // 0 is end of dir contents
   if ( result < 0 )
      new_result = translate_result(result);
   else if ( result > 0 )
      new_result = 1;

   return new_result;
};

time_t
convert_time( unsigned char *old_time )
{
   char *t = (char*)old_time;
   // hardcoded for 2002
   struct tm tm_time = { t[1], t[2], t[3], t[4], t[5], 102, 0, 0, 0 };
   time_t new_time = mktime( &tm_time );
   return new_time;
}

int __wrap_stat( const char *path, struct stat *sbuf )
{
   struct sce_stat stats_sce;
   GetFilename(buffer1, path);
   // WARNING: can't call sceGetstat when running under gdb...
//    int result = sceGetstat (buffer1, &stats_sce);
   int result = 0;
   int new_result = translate_result(result);
   if ( new_result == 0 ) { // success
      // fill out sbuf with info returned in stats_sce

      // st_mode
      sbuf->st_mode = 0;
      sbuf->st_mode = (0x1ff & stats_sce.st_mode);
      if ( SCE_STM_ISREG(stats_sce.st_mode) ) sbuf->st_mode |= S_IFREG;
      if ( SCE_STM_ISLNK(stats_sce.st_mode) ) sbuf->st_mode |= S_IFLNK;
      if ( SCE_STM_ISDIR(stats_sce.st_mode) ) sbuf->st_mode |= S_IFDIR;

      sbuf->st_size = ((off_t)stats_sce.st_hisize << 32) | stats_sce.st_size;

      sbuf->st_ctime = convert_time( stats_sce.st_ctime );
      sbuf->st_mtime = convert_time( stats_sce.st_mtime );
      sbuf->st_atime = convert_time( stats_sce.st_atime );

      // zero out unknown fields
      sbuf->st_dev = 0;
      sbuf->st_ino = 0;
      sbuf->st_nlink = 0;
      sbuf->st_uid = 0;
      sbuf->st_gid = 0;
      sbuf->st_rdev = 0;
      sbuf->st_blksize = 0;
      sbuf->st_blocks = 0;
   }

   return new_result;
}

int __wrap_fstat( int fd, struct stat *sbuf )
{
   printf("*** WARNING: fstat is not implemented ***\n");
   return translate_result(-ENOMEM);
}

int __wrap_lstat( const char *path, struct stat *sbuf )
{
   printf("*** WARNING: lstat is wraps stat... probably not right ***\n");
   return __wrap_stat(path, sbuf);
}

int __wrap_lseek(int fd, int offset, int where) {
   int result = sceLseek(fd, offset, where);
   if ( result < 0 )
      result = translate_result(result);
   return result;
};

int __wrap_read(int fd, void *buf, int nbyte) {
   int result = sceRead(fd, buf, nbyte);
   if ( result < 0 )
      result = translate_result(result);
   return result;
};

int __wrap_write(int fd, const void *buf, int nbyte) {
   int result = sceWrite(fd, buf, nbyte);
   if ( result < 0 )
      result = translate_result(result);
   return result;
};

//    int __wrap_remove( const char *filename) {
//       GetFilename(buffer1, filename);
//       return sceRemove (buffer1);
//    };

//    int __wrap_mkdir( const char *dirname, int flag) {
//       GetFilename(buffer1, dirname);
//       return sceMkdir (buffer1, flag);
//    };
//    int __wrap_rmdir( const char *dirname) {
//       GetFilename(buffer1, dirname);
//       return sceRmdir (buffer1);
//    };
//    int __wrap_sync( const char *path, int flag) {
//       return sceSync(path, flag);
//    };	
//    int __wrap_symlink(const char *existing, const char *newname) {
//       GetFilename(buffer1, existing);
//       GetFilename(buffer2, newname);
//       return sceSymlink(buffer1, buffer2);
//    };
//    int __wrap_readlink(const char *path, char *buf, unsigned int bufsize) {
//       GetFilename(buffer1, path);
//       return sceReadlink(buffer1, buf, bufsize);
//    };
//    int __wrap_rename( const char *oldname, const char *newname) {
//       GetFilename(buffer1, oldname);
//       GetFilename(buffer2, newname);
//       return sceRename(buffer1, buffer2);
//    };

} // extern "C"
