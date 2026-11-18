#ifndef TRANSGAMING_USE_PHYSFS

// #pragma once
#ifndef _GPACK_
#define _GPACK_


#include "Core/HashMap.h"

class GPack;
class GFileManager;

struct FIdx
{
#ifdef TRANSGAMING
    static const int NAMELENGTH = 256;

    GPack * pPack;
    char    name[NAMELENGTH];
#else
    GPack *pPack;
    char name[256];
#endif
    DWORD idx,size,csize,offset;
	long32 searchCnt;
	FIdx *next,*prev;
	FIdx()
	{
//		name[0] = NULL;
		name[0] = 0;
		idx=size=csize=offset=searchCnt=0;
		pPack = NULL;
		next = NULL;
		prev = NULL;
	}
};

class ENGINE_API GPack
{
  protected:
    GFileManager *pMyManager;
    FILE * pfile;
    bool compressed;
    char strPath[MAX_PATH];  
    char strFName[MAX_PATH];  
    void *pMapMem;

    
    HANDLE hMapping;
    long32 FindFile(LPCSTR strFName);
  
  public:
    bool UsingSharedMemory;
    GPack();
    virtual ~GPack();
    
    void Release();
    virtual bool OpenPack(LPCSTR fname,LPCSTR path);
    static long32 Make(LPCSTR fname,LPCSTR path,bool compr = false);
#ifndef TRANSGAMING
	static long32 Extract(LPCSTR fname,LPCSTR path);
#endif
    virtual GFile *GetFile(FIdx *pFidx);
    LPCSTR GetPath() {return strPath;};
    LPCSTR GetFName() {return strFName;};

    void AddFile(LPCSTR fname,void *data, int size, bool compressed = true);
    void Save(LPCSTR fname);
    friend class GFileManager;
};


#endif //_GPACK_

#endif  //!TRANSGAMING_USE_PHYSFS
