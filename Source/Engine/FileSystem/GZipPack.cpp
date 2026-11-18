#ifndef TRANSGAMING_USE_PHYSFS

#include "Engine.h"
#include <windows.h>
#include "unzip.h"
#include "GFile.h"
#include "GMemFile.h"
#include "GPack.h"
#include "GZipPack.h"
#include "iowin32.h"
#include "GFileManager.h"

#ifndef TRANSGAMING
LPSTR ConvertSlash(LPCSTR path);
void ConvertSlashOnSelf(LPSTR path);
LPCSTR GetLwr(char *buf);
#endif
//=========================================================================  

GZipPack::GZipPack()
{
    UsingSharedMemory = true;
    pZipFile = NULL;
}

GZipPack::~GZipPack()
{
  if (pZipFile)
  {
    unzCloseCurrentFile(pZipFile);
    unzClose(pZipFile);
    pZipFile = NULL;
  }
}



bool GZipPack::OpenPack(LPCSTR fname,LPCSTR path)
{
#ifdef TRANSGAMING
  strncpy(strFName,fname, MAX_PATH);  
  strncpy(strPath,path, MAX_PATH);  
#else
  strcpy(strFName,fname);  
  strcpy(strPath,path);  
#endif
  if (pZipFile)  return true;

  zlib_filefunc_def ffunc;
  fill_win32_filefunc(&ffunc);

  pZipFile = unzOpen2(strFName,&ffunc);
  
  if (!pZipFile) return false;

  unz_global_info gi;
  unzGetGlobalInfo (pZipFile,&gi);

  

  int cntFIdx = gi.number_entry;
  if (cntFIdx<1) return false;
      

  int i=0;
  int err = unzGoToFirstFile(pZipFile);
  while (err == UNZ_OK)
  { 
    #define UNZ_MAXFILENAMEINZIP (256)
    char szCurrentFileName[UNZ_MAXFILENAMEINZIP+1];

    m_setTag("Pack");
    FIdx *pFIdx = new FIdx();

    unz_file_info fi;
    unzGetCurrentFileInfo(pZipFile,&fi,szCurrentFileName,
      sizeof(szCurrentFileName)-1,NULL,0,NULL,0);

    unz_file_pos_s pos;
    unzGetFilePos(pZipFile,&pos);
    //pFIdx[i].offset = unzGetCurrentOffset(pZipFile);
    pFIdx->offset = pos.pos_in_zip_directory;
    pFIdx->size = fi.uncompressed_size;
    pFIdx->idx = i;

#ifdef TRANSGAMING
    strncpy(pFIdx->name, szCurrentFileName, FIdx::NAMELENGTH);
#else
    strcpy(pFIdx->name,szCurrentFileName);
#endif
    strlwr(szCurrentFileName);
    
    pMyManager->RegisterFile(szCurrentFileName,pFIdx);        

    i++;
    err = unzGoToNextFile(pZipFile);
    //StackTracer::Log("%s\n",szCurrentFileName);
  }
  unzGoToFirstFile(pZipFile);

  return true;
}


#ifndef TRANSGAMING
extern BYTE mem[6000000];
#endif

GFile *GZipPack::GetFile(FIdx *pf)
{  
  if (!pZipFile) return NULL;
  unzCloseCurrentFile(pZipFile);
  
  //long32 i = FindFile(strFName);
  //if (i<0) 
  //  return NULL;

  //FIdx *pf = &pFIdx[i];

  unz_file_pos_s pos;
  pos.num_of_file = pf->idx;
  pos.pos_in_zip_directory = pf->offset;
  
  unzGoToFilePos(pZipFile,&pos);  
  unzOpenCurrentFile(pZipFile);  

  m_setTag("Pack");
  GMemFile *pF = new GMemFile();
  long32 size = pf->size;
#ifndef TRANSGAMING
  if (size<6000000 && UsingSharedMemory)
  {
    pF->Open(mem,size,false);
    unzReadCurrentFile(pZipFile,mem,size);
    mem[size] = NULL;
  }
  else
#endif
  {
    m_setTag("Pack");
    BYTE *mptr = (BYTE*) malloc(size);
    pF->Open(mptr,size,true);
    unzReadCurrentFile(pZipFile,mptr,size);
  }
  
  return pF;
}


#endif //!TRANSGAMING_USE_PHYSFS
