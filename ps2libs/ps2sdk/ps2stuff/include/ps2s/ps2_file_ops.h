#ifndef ps2_file_ops_h
#define ps2_file_ops_h


#ifdef __cplusplus
extern "C" {
#endif

extern char* GetFilename( char *buffer, const char *file );

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus

#include <string>

namespace MediaType {
   typedef enum {
      kHost0 = 0, kAtmon = 1, kCdRom = 2, kInvalid = 3
   } tMediaType;
}
typedef MediaType::tMediaType tMediaType;

namespace HostType {
   typedef enum {
      kLinux, kWindows, kInvalid
   } tHostType;
}
typedef HostType::tHostType tHostType;

class FileIo {
      static tMediaType		Media;
      static tHostType		HostType;
      static std::string	Prefix;
      
      // don't instanciate this class
      FileIo();

   public:
      /// you must set the media type before calling
      /// ConvertPath. hostIp is only required for atmon
      static void		SetMediaType(tMediaType type, const char *hostIp = 0);
      static void		SetHostType(tHostType type);
      static std::string	ConvertPath(const std::string &path);
      static char*		ConvertPath(char *buffer, const char *file);
};


#endif // _cplusplus

#endif // ps2_file_ops_h
