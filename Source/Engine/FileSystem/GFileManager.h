
// #pragma once
#ifndef _GFILEMANAGER_
#define _GFILEMANAGER_


enum
{
    EFindFiles    = (1<<1),
    EFindDirs     = (1<<2)
};

#include "Core/DynArray.h"
#include "Core/LeString.h"
#include "GFile.h"
#ifndef TRANSGAMING_USE_PHYSFS
#include "GPack.h"
#include "GZipPack.h"
#endif


#ifndef TRANSGAMING_USE_PHYSFS
struct ff
{
    String              fmask;
    HANDLE              h;
    WIN32_FIND_DATA     fd;
    FIdx *              found;
    bool                packed;
    ff()
    {
#ifdef TRANSGAMING
        h =         0;
#else
        h =         NULL;
#endif
        found =     NULL;
        fmask =     "";
        packed =    false;
    }
};
#endif


class ENGINE_API GFileManager
{
protected:
#ifndef TRANSGAMING_USE_PHYSFS
  DynamicArray<GPack*> vPack;  
  DynamicArray<GFile*> vFile;
#endif
  
#ifndef TRANSGAMING
  void * ZipFile;  
#endif
  FILE * PakFile;    
#ifndef TRANSGAMING_USE_PHYSFS
  long32 searchCnt;
#endif

public:
#ifndef TRANSGAMING_USE_PHYSFS
# ifdef TRANSGAMING
  HashMap <String,FIdx*,16384 * 4> *    pRegisteredFiles;
  HashMap <String,FIdx*,8192> *     pFolders;
#  ifdef FIND_HASH_PEAK
  static int                        mMaxpRegisteredFilesUsage;
  static int                        mMaxpFoldersUsage;
#  endif
# else
  HashMap <String,FIdx*,16384*4> * pRegisteredFiles;
  HashMap <String,FIdx*,8192> * pFolders;
# endif
#endif
  GFileManager();
  ~GFileManager();
  
#ifndef TRANSGAMING_USE_PHYSFS
  GPack *RegisterPack(GPack* pPack,LPCSTR fname,LPCSTR path,bool must_exist=true);
  void RegisterFile(LPCSTR fname,FIdx *pFIdx);
  void UnregisterPack(GPack* pPack);
#endif

  GFile* CreateFileReader(LPCSTR strFName,LPCSTR mode = "rb");
  GFile* CreateFileWriter(LPCSTR strFName,LPCSTR mode = "wb");
  void Close(GFile* pGF);
  void DetachFile(GFile* pGF);
  GFile* Add(GFile* pGF);

#ifndef TRANSGAMING
  bool CreateDirectory( LPCSTR dir );
  GPack *FindPackByPath(LPCSTR path);
  bool TestDirectory( LPCSTR dir );

  bool Copy(LPCSTR strSrcFName,LPCSTR strDstFName);  
#endif

  bool FileExist(LPCSTR strFName);  
  ulong32 FileSize(LPCSTR strFName);  
  void FindFiles(const char* dirName, DynamicArray<String>& list, DWORD mode=EFindFiles);  
#ifdef TRANSGAMING
  bool FindAFile(const char *dirName, char *buf, int len);
#else
  String FileTime( const char* fileName );
  void RemoveFilesInDirectory( const char* dirName );
#endif


#ifndef TRANSGAMING_USE_PHYSFS
  long32 FF_Open(LPCSTR path);
  long32 FF_Next(long32 h);  
  LPCSTR FF_GetName(long32 h);  
  bool FF_IsDirectory(long32 h);  
  void FF_Close(long32 h);  
  void FF_MarkPackedFileAsFound(long32 h, LPCSTR file);
#endif

#ifndef TRANSGAMING
  void CreateZIP(LPCSTR strFName);
  void CloseZIP();
  void* GetZIPFile() {return ZipFile;};
#endif

#ifndef TRANSGAMING_USE_PHYSFS
  void CreatePAK(LPCSTR strFName);
  void ClosePAK();
  FILE* GetPAKFile() {return PakFile;};
#endif
};

String GetCorrectFileName(LPCSTR fname);



#endif //_GFILEMANAGER_
