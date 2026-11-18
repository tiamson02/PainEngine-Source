
#pragma once

#include "../HashMap.h"

class GPack;
class GFileManager;

struct FIdx
{
    GPack *pPack;
    char name[256];
    long idx,size,csize,offset;
	long searchCnt;
	FIdx *next,*prev;
	FIdx()
	{
		name[0] = NULL;
		idx=size=csize=offset=searchCnt=0;
		pPack = NULL;
		next = NULL;
		prev = NULL;
	}
};

class GPack
{
  protected:
    GFileManager *pMyManager;
    FILE * pfile;
    bool compressed;
    char strPath[MAX_PATH];  
    char strFName[MAX_PATH];  
    void *pMapMem;

    
    HANDLE hMapping;
    long FindFile(LPCSTR strFName);
  
  public:
    bool UsingSharedMemory;
    GPack();
    virtual ~GPack();
    
    void Release();
    virtual bool OpenPack(LPCSTR fname,LPCSTR path);
    static long Make(LPCSTR fname,LPCSTR path,bool compr = false);
	static long Extract(LPCSTR fname,LPCSTR path);
    virtual GFile *GetFile(FIdx *pFidx);
    LPCSTR GetPath() {return strPath;};
    LPCSTR GetFName() {return strFName;};

    void AddFile(LPCSTR fname,void *data, int size, bool compressed = true);
    void Save(LPCSTR fname);
    friend class GFileManager;
};
