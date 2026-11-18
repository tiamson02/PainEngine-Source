
#pragma once

enum
{
    EFindFiles    = (1<<1),
    EFindDirs     = (1<<2)
};

#include <msvcrt/io.h>
#include "../DynArray.h"
#include "GFile.h"
#include "GPack.h"
#include "GZipPack.h"


#ifndef TRANSGAMING_BOOGALOOGA
struct ff
{
  String            fmask;
  HANDLE            h;
  WIN32_FIND_DATA   fd;
  FIdx *            found;
  bool              packed;
  ff()
  {
	  h =       NULL;
	  found =   NULL;
      fmask =   "";
      packed =  false;
  }
};
#endif


class GFileManager
{
protected:
  DynamicArray<GPack*> vPack;  
  DynamicArray<GFile*> vFile;
  
  void * ZipFile;    
  FILE * PakFile;    
  long searchCnt;

public:
  HashMap <String,FIdx*,16384*4> * pRegisteredFiles;
  HashMap <String,FIdx*,8192> * pFolders;
  GFileManager();
  ~GFileManager();
  
  GPack *RegisterPack(GPack* pPack,LPCSTR fname,LPCSTR path,bool must_exist=true);
  void RegisterFile(LPCSTR fname,FIdx *pFIdx);

  void UnregisterPack(GPack* pPack);
  GFile* CreateFileReader(LPCSTR strFName,LPCSTR mode = "rb");
  GFile* CreateFileWriter(LPCSTR strFName,LPCSTR mode = "wb");
  void Close(GFile* pGF);
  void DetachFile(GFile* pGF);
  GFile* Add(GFile* pGF);
  GPack *FindPackByPath(LPCSTR path);
  bool CreateDirectory( LPCSTR dir );
  bool TestDirectory( LPCSTR dir );

  bool Copy(LPCSTR strSrcFName,LPCSTR strDstFName);  
  bool FileExist(LPCSTR strFName);  
  unsigned long FileSize(LPCSTR strFName);  
  void FindFiles(const char* dirName, DynamicArray<String>& list, DWORD mode=EFindFiles);  
  String FileTime( const char* fileName );
  void RemoveFilesInDirectory( const char* dirName );


  long FF_Open(LPCSTR path);
  long FF_Next(long h);  
  LPCSTR FF_GetName(long h);  
  bool FF_IsDirectory(long h);  
  void FF_Close(long h);  
  void FF_MarkPackedFileAsFound(long h, LPCSTR file);

  void CreateZIP(LPCSTR strFName);
  void CloseZIP();
  void* GetZIPFile() {return ZipFile;};

  void CreatePAK(LPCSTR strFName);
  void ClosePAK();
  FILE* GetPAKFile() {return PakFile;};
};

String GetCorrectFileName(LPCSTR fname);