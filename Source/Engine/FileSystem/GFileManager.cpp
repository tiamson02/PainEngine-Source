#ifdef TRANSGAMING_PS2
# include <sifdev.h>
#endif
#ifdef TRANSGAMING_PSP
# include <iofilemgr_stat.h>
#endif
#include "Engine.h"
#include "zip.h"
#include "GFile.h"
#include "GMemFile.h"
#include "GPack.h"
#include "GFileManager.h"
#include "iowin32.h" 
#include "Core/nommgr.h"
#include "WildCard.h"
#include "Core/mmgr.h"
#include "Core/FileSystem/LogBuffer.h"

#if defined( TRANSGAMING_PSP )
#include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
#include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
#include "../TNG_WIN/TNG_stdio_WIN.h"
#endif


#ifdef TRANSGAMING_BOOGALOOGA
struct ff
{
  String            fmask;
  long32              h;
  _finddata_t       fd;
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



extern ENGINE_API LogBuffer GLog;

#ifdef TRANSGAMING
LPSTR ConvertSlash(LPCSTR path, char *buf, size_t len);
void SimplifyPathOnSelf(LPSTR fn, size_t len);
#else
LPSTR ConvertSlash(LPCSTR path);
LPCSTR GetLwr(char *buf);
void SimplifyPathOnSelf(LPSTR fn);
#endif
void ConvertSlashOnSelf(LPSTR path);

#ifndef TRANSGAMING
BYTE mem[6000000]; // static buffer for decompressing data
BYTE mem1[6000000];
#endif

#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
int GFileManager::mMaxpRegisteredFilesUsage = 0;
int GFileManager::mMaxpFoldersUsage = 0;
#endif

//===========================================================================
GFileManager::GFileManager()
{
#ifndef TRANSGAMING_USE_PHYSFS
    pRegisteredFiles = NULL;
	pFolders = NULL;
	searchCnt = 0;
#endif
#ifndef TRANSGAMING
    ZipFile = NULL;
#endif
    PakFile = NULL;
}
//===========================================================================
GFileManager::~GFileManager()
{
#ifndef TRANSGAMING_USE_PHYSFS
	int i=0;
  for (i=0;i<vFile.Size();i++)
  {
    vFile[i]->pMyManager = NULL;
    delete vFile[i];  
  }  
  vFile.ClearArray();

  for (i=0;i<vPack.Size();i++)
    delete vPack[i];  
  
  vPack.ClearArray();

  if (pRegisteredFiles)
  {
    for(int i=0; i<pRegisteredFiles->Size(); ++i)
    {
        FIdx * pFIdx =  pRegisteredFiles->GetPair(i).Value;
        if (pFIdx)
            delete pFIdx;  
    }
    pRegisteredFiles->Clear();
	delete pRegisteredFiles;
  }

  if (pFolders)
  {
	  pFolders->Clear();
	  delete pFolders;
  }
#endif
}
//===========================================================================
GFile* GFileManager::Add(GFile* pGF)
{
#ifndef TRANSGAMING_USE_PHYSFS
  if (!pGF) return NULL;
  
  pGF->pMyManager = this;
  vFile.Add(pGF);
#endif
 
  return pGF;
}
//===========================================================================
GFile* GFileManager::CreateFileWriter(LPCSTR strFName,LPCSTR mode)
{
#ifdef TRANSGAMING_USE_PHYSFS
    //!!// implement part of the save game system here (memory stick writer)
    if (!strFName || strFName[0] == '\0')
        return NULL;


    m_setTag("File");
    GFile *f = new GFile();


    if (!f->Open(strFName, mode)){
        delete f;

        return NULL;
    }

    return Add(f);
#else
    if (!strFName || strFName[0] == '\0')
        return NULL;
    
#ifdef TRANSGAMING
    char wmode[4];

    
    strncpy(wmode, mode, sizeof(wmode));

    if (strstr(mode,"wz"))
        strncpy(wmode, "wb", sizeof(wmode));

    if (strstr(mode,"wp") && !GetPAKFile())
        strncpy(wmode, "wb", sizeof(wmode));

    char fn[MAX_PATH];
    strncpy(fn, strFName, MAX_PATH);
#else
    char wmode[3];
    strcpy(wmode,mode);

    if (strstr(mode,"wz") && !GetZIPFile())
        strcpy(wmode,"wb");

    if (strstr(mode,"wp") && !GetPAKFile())
        strcpy(wmode,"wb");

    char fn[MAX_PATH];
    strcpy(fn,strFName);
#endif
    ConvertSlashOnSelf(fn);

    if (strstr(wmode,"wz")) // write to zip
    {
        m_setTag("File");
        GMemFile *pF = new GMemFile();

#ifdef TRANSGAMING
        strncpy(pF->strFName, fn, MAX_PATH);
        strncpy(pF->wmode,"wz", 3);

        void *tmpBuf = malloc(1048576);
        pF->Open(tmpBuf, 0, true);
#else
        strcpy(pF->strFName,fn);
        strcpy(pF->wmode,"wz");

        pF->Open(mem,0,false);
#endif
        return Add(pF);
    }
    else if (strstr(wmode,"wp")) // write to pak
    {
        m_setTag("File");
        GMemFile *pF = new GMemFile();
#ifdef TRANSGAMING
        strncpy(pF->strFName, fn, MAX_PATH);
        strncpy(pF->wmode, "wp", 3);

        void *tmpBuf = malloc(1048576);
        pF->Open(tmpBuf, 0, true);
#else
        strcpy(pF->strFName,fn);
        strcpy(pF->wmode,"wp");

        pF->Open(mem,0,false);
#endif
        return Add(pF);
    }
    else
    {
        m_setTag("File");
        GFile *pF = new GFile();
        if (pF->Open(fn,wmode))
            return Add(pF);
        else
            delete (pF);
    }

    return NULL;  
#endif
}
//===========================================================================
GFile* GFileManager::CreateFileReader(LPCSTR strFName,LPCSTR mode)
{
    if (!strFName || strFName[0] == '\0')
        return NULL;

#ifdef TRANSGAMING_USE_PHYSFS
    //file doesn't exist => fail
    if (!TNG_exists(strFName))
        return NULL;


    m_setTag("File");
    GFile *f = new GFile();


    if (!f->Open(strFName, mode)){
        delete f;

        return NULL;
    }

    return Add(f);
#else
    char fn[MAX_PATH];

    strncpy(fn, strFName, MAX_PATH);

#ifndef TRANSGAMING_PS2_RUN_FROM_LINUX
    ConvertSlashOnSelf(fn);
    strlwr(fn);
#endif

	//GLog.Print("GFileManager::CreateFileReader(%s)\n",fn);

    m_setTag("File");
	GFile *pF = new GFile();
	if (pF->Open(fn,mode))
		return Add(pF);
	else
	{
		delete (pF);
		pF = NULL;
	}

    if (pRegisteredFiles)
    {
#ifdef TRANSGAMING
        SimplifyPathOnSelf(fn, MAX_PATH);
        FIdx **ppf = pRegisteredFiles->Find(fn);

        TRACE("GFileManager::CreateFileReader(): Trying to find '%s' in .pak file : ", fn);
        if (ppf)
        {
            TRACE("FOUND in '%s'!\n", (*ppf)->pPack->GetFName());
            return Add((*ppf)->pPack->GetFile(*ppf));
        }
        else
        	TRACE("MISSING!\n");
#else
        SimplifyPathOnSelf(fn);
        FIdx **ppf = pRegisteredFiles->Find(fn);

        if (ppf)
            return Add((*ppf)->pPack->GetFile(*ppf));
#endif
    }
#endif

  return NULL;  
}
//===========================================================================
#ifndef TRANSGAMING
bool GFileManager::Copy(LPCSTR strSrcFName,LPCSTR strDstFName)
{
    GFile* pf = CreateFileReader(strSrcFName,"rb");
    if (!pf) 
	{
		return false;
	}
    long32 s = pf->GetSize();

    m_setTag("File");
    BYTE *buf = (BYTE *)malloc(s);
    pf->Read(buf,s);

#ifdef TRANSGAMING
    FILE * f = TNG_fopen(strDstFName,"wb");
    TNG_fwrite(buf,1,s,f);
    TNG_fclose(f);
#else
    FILE * f = fopen(strDstFName,"wb");
    fwrite(buf,1,s,f);
    fclose(f);
#endif      
	Close(pf);

    free(buf);
    return true;
}
#endif
//===========================================================================
void GFileManager::DetachFile(GFile* pGF)
{
#ifndef TRANSGAMING_USE_PHYSFS
    if (!pGF) return;

    int i = vFile.Find(pGF);
    if (i>-1)
    {
        pGF->pMyManager = NULL;
        vFile.FastRemove(i);
    }
#endif
}

//===========================================================================
void GFileManager::Close(GFile* pGF)
{
  if (!pGF) return;
  DetachFile(pGF);
  delete pGF;
}
//=========================================================================  
#ifndef TRANSGAMING_USE_PHYSFS
GPack *GFileManager::RegisterPack(GPack* pPack,LPCSTR fname,LPCSTR path,bool must_exist)
{  
    if (!pPack)
        return NULL;


    char pt[MAX_PATH];
    
    strncpy(pt, path, MAX_PATH);
    ConvertSlashOnSelf(pt);
    strlwr(pt);

    pPack->pMyManager = this;
    if (!pPack->OpenPack(fname, pt))
    {
        delete(pPack);

        char            tmp[MAX_PATH];
# if !defined( TRANSGAMING ) || defined( TRANSGAMING_WINDOWS )
        WIN32_FIND_DATA finfo;
        HANDLE          ffh;
# endif

        strncpy(tmp, path, MAX_PATH);
        
        tmp[strlen(tmp)-1] = 0;

# if defined(TRANSGAMING_PS2)
        struct sce_stat  st;

        if (sceGetstat(tmp, &st) != 0)
# elif defined(TRANSGAMING_PSP)
        // eric-> WARNING!  This check won't work because host0/disc0 isn't prepended to the filename!
        struct SceIoStat  st;

        if (sceIoGetstat(tmp, &st) != 0)
# else 
        ffh = FindFirstFile(tmp, &finfo);

        if (ffh == INVALID_HANDLE_VALUE)
# endif


        {
            if (must_exist){
# ifdef TRANSGAMING
                TRACE("[]::GFileManager::RegisterPack(): ERROR-> pack or directory not found: '%s'\n", fname);
# else
                MessageBox(NULL, "Pack or directory not found:", fname, MB_TOPMOST);
# endif
            }
            else
                return NULL;
        }
# ifndef TRANSGAMING
        else
            FindClose(ffh);
# endif

        return NULL;
    }

    vPack.Add(pPack);
 
    return pPack;  
}
#endif

//=========================================================================  
#ifndef TRANSGAMING_USE_PHYSFS
void GFileManager::UnregisterPack(GPack* pPack)
{  
    if (!pPack)
        return;

    if (pRegisteredFiles)
    {
        for(int i=0; i<pRegisteredFiles->Size(); ++i)
        {
            FIdx * pFIdx = pRegisteredFiles->GetPair(i).Value;
            if (pFIdx && pFIdx->pPack == pPack)
            {
				if (pFIdx->prev)
					pFIdx->prev->next = pFIdx->next;
				if (pFIdx->next)
					pFIdx->next->prev = pFIdx->prev;

				if (!pFIdx->prev) // first child
				{
					String fullpath = String(pFIdx->pPack->GetPath()) + String(pFIdx->name);	
					fullpath.ToLower();
					String path = String::GetPath(fullpath);
					if (fullpath.AsChar()[fullpath.Len()-1] == '/')
						path = String::GetPath(path);															
					if (pFIdx->next)
						pFolders->ChangeValue(path,pFIdx->next);
					else
					{
						pFolders->ChangeValue(path,NULL);
						pFolders->Remove(path);
					}
				}

                delete pFIdx;                
                pRegisteredFiles->GetPair(i).Value = NULL;
                pRegisteredFiles->Remove(pRegisteredFiles->GetPair(i).Key);                
            }            
        }
    }

    int p = vPack.Find(pPack);
    if (p>-1)
        vPack.Remove(p);
    delete pPack;  
}
#endif
//=========================================================================  
#ifndef TRANSGAMING
GPack *GFileManager::FindPackByPath(LPCSTR path)
{
  char fn[MAX_PATH];

#ifdef TRANSGAMING
  strncpy(fn, path, MAX_PATH);  
#else
  strcpy(fn,path);  
#endif
  ConvertSlashOnSelf(fn);
  strlwr(fn);
  
  for (int i=0;i<vPack.Size();i++)
  {
    char * s = strstr(fn,vPack[i]->GetPath());
    if (s) return vPack[i];
  }       
  return NULL;
}
#endif
//=========================================================================  
#ifndef TRANSGAMING_USE_PHYSFS
void GFileManager::FF_MarkPackedFileAsFound(long32 h, LPCSTR file)
{
	ff *pFF = (ff *)h;
	if (pFF == NULL || !pRegisteredFiles) return;

	String path = pFF->fmask;
	char *es = strrchr(pFF->fmask.AsChar(),'/');
	if (es)
	{
		path = path.SubStr(es-pFF->fmask.AsChar()+1);
		if (FF_IsDirectory(h)) 
			path = path + String(file) + "/";
		else
			path = path + String(file);

		path.ToLower();
		FIdx **ppf = pRegisteredFiles->Find(path.AsChar());
		if (ppf)
			(*ppf)->searchCnt = searchCnt;
	}
}
//=========================================================================  
long32 GFileManager::FF_Open(LPCSTR path)
{  
    searchCnt++;

    m_setTag("File");
    ff *pFF = new ff();

    // na dysku
    //allows searching for wildcards

#ifdef TRANSGAMING
    char tmpBuf[MAX_PATH];

    pFF->fmask = strlwr(ConvertSlash(path, tmpBuf, MAX_PATH));
#else
    pFF->fmask = strlwr(ConvertSlash(path));
#endif

#ifdef TRANSGAMING_BOOGALOOGA
    pFF->h = _findfirst(path, &pFF->fd);

    if (pFF->h != -1)
    {
        FF_MarkPackedFileAsFound((long32)pFF, pFF->fd.name);
        return (long32) pFF;
    }
#else
    pFF->h = FindFirstFile(path, &pFF->fd);

    if (pFF->h != INVALID_HANDLE_VALUE) 
    {
        FF_MarkPackedFileAsFound((long32)pFF, pFF->fd.cFileName);
        return (long32) pFF;
    }
#endif

  
    // w plikach pakowanych
    pFF->packed = true;

    if (FF_Next((long32)pFF) > 0)
        return (long32) pFF;
    else
        delete (pFF);
    return -1;
}
//=========================================================================  
#ifndef TRANSGAMING
int HowManySlashes(const char *buf)
{
    int sn = 0;  
    char *ch = strchr(buf,'/');
    while(ch)
    {
        sn++;
        ch = strchr(ch+1,'/');
    }	  
    return sn;
}
#endif
//=========================================================================  
long32 GFileManager::FF_Next(long32 h)
{
    ff *pFF = (ff *)h;
    if (pFF == NULL) return 0;
    
    // zwykle pliki
    if (!pFF->packed)
    {
#ifdef TRANSGAMING_BOOGALOOGA
        if (_findnext(pFF->h, &pFF->fd) == 0)
        {
            FF_MarkPackedFileAsFound((long32)pFF, pFF->fd.name);
    	    return 1;
    	}
#else
        if (FindNextFile(pFF->h,&pFF->fd))
        {
            FF_MarkPackedFileAsFound((long32)pFF, pFF->fd.cFileName);
    	    return 1;
    	}
#endif
        else
    	{
    		// nastepnie w plikach pakowanych
    		pFF->found = NULL;
    		pFF->packed = true;

    		return FF_Next((long32)pFF);
    	}
    }
    else if (pRegisteredFiles)
    {
        const char * key = pFF->fmask.AsChar();
        String path = String::GetPath(key);

        FIdx * pFIdx = NULL;
        if (!pFF->found)
        {
            FIdx ** pFolder =  pFolders->Find(path);
            if (pFolder)
                pFIdx = *pFolder;
        }
        else
            pFIdx = pFF->found->next;

        
        pFF->found = NULL;

        while (pFIdx)
        {
            if (pFIdx->searchCnt != searchCnt)
            {
                String fullkey = key;
                String fullfile = String(pFIdx->pPack->GetPath()) + pFIdx->name;
                fullfile.ToLower();				
                      
                bool directory = false;
                int l = strlen(pFIdx->name);
                if ( pFIdx->name[l-1] == '/') 
                    directory =  true;          
                      
                // dla katalogow zamieniamy na *
                if (directory && strstr(fullfile.AsChar(),"*.*")) 
                    fullfile = fullfile.SubStr(fullfile.Len()-2);

                if (wildcard::wildcmp(fullkey.AsChar(),fullfile.AsChar()))
                { 
                    pFF->found = pFIdx;
                    pFIdx->searchCnt = searchCnt;
                    return (long32) pFF->found;
                }
            }
            pFIdx = pFIdx->next;
        }
    }

    return -1;
}
//=========================================================================  
LPCSTR GFileManager::FF_GetName(long32 h)
{
    ff *pFF = (ff *)h;
    if (pFF == NULL) 
        return 0;
  
    // zwykle pliki
    if (!pFF->packed){
#ifdef TRANSGAMING_BOOGALOOGA
        return pFF->fd.name;
#else
        return pFF->fd.cFileName;
#endif
    }

    else if (pRegisteredFiles)
    {
        FIdx * pFIdx = pFF->found;
        if (!pFIdx)
            return NULL;

        static char name[MAX_PATH];
        bool directory = false;

        int len = strlen(pFIdx->name)-1;
        if (FF_IsDirectory(h)) 
        {
            len--;
            directory = true;
        }
    
        int b=0;
        for (b=len; b>0;b--){
            if (pFIdx->name[b] == '/' || pFIdx->name[b] == '\\')
            {
                b++;
                break;
            }
        }

#ifdef TRANSGAMING
        strncpy(name, &pFIdx->name[b], FIdx::NAMELENGTH);

        if (name[0] == '\0')
             strncpy(name, ".", FIdx::NAMELENGTH);
#else
         strcpy(name,&pFIdx->name[b]);  

         if (name[0] == '\0')
             strcpy(name,".");
#endif
         
         if (directory)
#ifdef TRANSGAMING
             name[strlen(name)-1] = 0;
#else
             name[strlen(name)-1] = NULL;
#endif
         
         return name;
    }  

    return NULL;
}
//=========================================================================  
bool GFileManager::FF_IsDirectory(long32 h)
{
  ff *pFF = (ff *)h;
  if (pFF == NULL) return 0;
  
  // zwykle pliki
  if (!pFF->packed){
#ifdef TRANSGAMING_BOOGALOOGA
      return (pFF->fd.attrib & _A_SUBDIR) ? true : false;
#else
      return pFF->fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY?true:false;
#endif
  }

  else if (pRegisteredFiles)
  {      
      FIdx * pFIdx = pFF->found;
      if (pFIdx)
      {
        int l = strlen(pFIdx->name);
        if ( pFIdx->name[l-1] == '/') 
            return  true;          
      }
  }  

  return false;
}
//=========================================================================  
void GFileManager::FF_Close(long32 h)
{
    ff *pFF = (ff *)h;
    if (pFF == NULL) return;
  
    if (pFF->h != INVALID_HANDLE_VALUE){
#ifdef TRANSGAMING_BOOGALOOGA
        _findclose(pFF->h);
#else
        FindClose(pFF->h);
#endif
    }

    delete (pFF);  
}
#endif //!TRANSGAMING_USE_PHYSFS
//=========================================================================  
bool GFileManager::FileExist(LPCSTR strFName)
{
#ifdef TRANSGAMING_USE_PHYSFS
    return TNG_exists(strFName);
#else
	bool ret = false;
	char fn[MAX_PATH];

	strncpy(fn, strFName, MAX_PATH);
//	ConvertSlashOnSelf(fn);
	strlwr(fn);

# ifdef TRANSGAMING
	FILE *f = TNG_fopen(fn,"rb");
# else
	FILE *f = fopen(fn,"rb");
# endif
	if (f)
	{
		ret = true;
# ifdef TRANSGAMING
		TNG_fclose(f);
# else
		fclose(f);
# endif
	}
    else if (pRegisteredFiles)
	{
# ifdef TRANSGAMING
		SimplifyPathOnSelf(fn, MAX_PATH);
# else
		SimplifyPathOnSelf(fn);
# endif
        FIdx **ppf = pRegisteredFiles->Find(fn);
        if (ppf)
            ret = true;
	}

	return ret;
#endif
}
//=========================================================================  
ulong32 GFileManager::FileSize(LPCSTR strFName)
{
#ifdef TRANSGAMING_USE_PHYSFS
    return TNG_fileSize(strFName);
#else
    char fn[MAX_PATH];

    strncpy(fn, strFName, MAX_PATH);
    ConvertSlashOnSelf(fn);
    strlwr(fn);

    ulong32 l = 0;

# ifdef TRANSGAMING
    FILE *f = TNG_fopen(fn,"rb");
# else
    FILE *f = fopen(fn,"rb");
# endif

    if (f)
    {
# ifdef TRANSGAMING
        TNG_fseek(f,0,SEEK_END);
        l = TNG_ftell(f);
        TNG_fclose(f);
# else
        fseek(f,0,SEEK_END);
        l = ftell(f);
        fclose(f);
# endif
    }
    else if (pRegisteredFiles)
    {
        FIdx **ppf = pRegisteredFiles->Find(fn);
        if (ppf)
            l = (*ppf)->size;
    }

    return l;
#endif
}
//=========================================================================  
void GFileManager::FindFiles(const char* dirName, DynamicArray<String>& list, DWORD mode)
{
#ifdef TRANSGAMING_USE_PHYSFS
    //nothing to do! => fail
    if ((mode & (EFindDirs | EFindFiles)) == 0)
        return;

    
    char        dir[MAX_PATH];
    char **     files = TNG_findOpen(dirName, dir, MAX_PATH);
    size_t      len = strlen(dir);
    

    for (int i = 0; files[i]; i++){
        if (strcmp(files[i], ".") && strcmp(files[i], "..")){
            snprintf(&dir[len], MAX_PATH - len, "/%s", files[i]);

            if (TNG_isDirectory(dir)){
                if (!(mode & EFindDirs))
                    continue;
            }

            else{
                if (!(mode & EFindFiles))
                    continue;
            }

            list.Add(files[i]);
        }
    }


    TNG_findClose(files);
#else
    long32 h = FF_Open(dirName);
	if(h!=-1) 
    {
		do 
        {				
            LPCSTR name = FF_GetName(h);
			if(strcmp(name,".") && strcmp(name,"..")) 
            {
				if(FF_IsDirectory(h)) 
				{
					if (!(mode&EFindDirs)) 
                        continue;
				}
				else
				{
					if (!(mode&EFindFiles)) 
                        continue;
				}					
                list.Add(name);
			}
		}
		while(FF_Next(h)>0);

        FF_Close(h);
	}
#endif
}

#ifdef TRANSGAMING
bool GFileManager::FindAFile(const char* dirName, char *buf, int len){
# ifdef TRANSGAMING_USE_PHYSFS
    return TNG_oneExists(dirName, buf, len);
# else
    long32 h = FF_Open(dirName);
    bool result = false;


    if (h != -1){
        const char *name = FF_GetName(h);


        if(strcmp(name, ".") && strcmp(name, "..")){
            strncpy(buf, name, len);

            result = true;
        }
        

        FF_Close(h);
    }


    return result;
# endif
}
#endif

String GetCorrectFileName(LPCSTR fname)
{
	char tmp[MAX_PATH];

#ifdef TRANSGAMING
	strncpy(tmp, fname, MAX_PATH);

    strlwr(tmp);
	ConvertSlashOnSelf(tmp);	
	SimplifyPathOnSelf(tmp, MAX_PATH);
#else
	strcpy(tmp,fname);
	strlwr(tmp);
	ConvertSlashOnSelf(tmp);	
	SimplifyPathOnSelf(tmp);
#endif

	return tmp;
}


//=========================================================================  
// Common tools
//=========================================================================  
#ifdef TRANSGAMING
LPSTR ConvertSlash(LPCSTR path, char *pth, size_t len){
	int j = 0;


    for (int i = 0; path[i]; i++){
        switch (path[i]){
            case '\\':
                pth[j++] = '/';

                //skip next path segment if '\/' appears together
                if (path[i + 1] == '/')
                    i++;

                break;

            case '/':
                if (path[i + 1] == '/'){
                    i++;
                    pth[j++] = '/';
                }

            default:
                pth[j++] = path[i];
                break;
        }
	}  


    //make sure the buffer is terminated
    pth[j] = 0;
	return pth;
}
#else
LPSTR ConvertSlash(LPCSTR path)
{
	static char pth[MAX_PATH];
	int i1=0;
    int i2=0;
	while(true)
	{
		pth[i1] = path[i2];
		if (pth[i1] == '\0')  break;
		if (pth[i1] == '\\') pth[i1] = '/';
        if (pth[i1] == '/' && path[i2+1] == '/')
        {
            i2++;
            pth[i1] = path[i2];            
        }
		i1++;
        i2++;
	}  

	return pth;
}
#endif
//==================================================================
void ConvertSlashOnSelf(LPSTR path)
{
	int i1=0;
    int i2=0;
	while(true)
	{
        path[i1] = path[i2];
		if (path[i1] == '\0')  break;
		if (path[i1] == '\\') path[i1] = '/';
        if (path[i1] == '/' && path[i2+1] == '/')
        {
            i2++;
            path[i1] = path[i2];            
        }
		i1++;
        i2++;
	}  
}
//==================================================================
#ifdef TRANSGAMING
void SimplifyPathOnSelf(LPSTR fn, size_t len)
#else
void SimplifyPathOnSelf(LPSTR fn)
#endif
{
	// example 
	// ../Data/Textures/../Textures2/Hud ->  ../Data/Textures2/Hud
	char *s = strstr(fn,"/../");
	while(s)
	{
		int b = s-fn-1;
		char * start = s;
		s = NULL;
		if (b > 0)
		{
			for (int i=b; i>0 ; i--)
			{
				if (fn[i] == '/')
				{
#ifdef TRANSGAMING
					strncpy(&fn[i], start + 3, len - i);
#else
					strcpy(&fn[i],start+3);
#endif
					s = strstr(fn,"/../");
					break;
				}
			}
		}
	}
}
//=========================================================================  
#ifndef TRANSGAMING
LPCSTR GetLwr(char *buf)
{
	static char ret[MAX_PATH];
	strcpy(ret,buf);
	strlwr(ret);
	return ret;
}
#endif
//=========================================================================  
#ifndef TRANSGAMING
bool GFileManager::CreateDirectory( const char* dirName )
{
    TRACE("GFileManager::CreateDirectory(): creating the folder '%s'\n", dirName);

    return( ::CreateDirectory( dirName, NULL ) == TRUE );
}
#endif
//=========================================================================  
#ifndef TRANSGAMING
bool GFileManager::TestDirectory( const char* dirName )
{
    TRACE("GFileManager::TestDirectory(): testing for the existence of the folder '%s'\n", dirName);
	TCHAR buf[1024];
	if( !::GetCurrentDirectory( 1024, buf ) ) return false;
	BOOL haveDir = ::SetCurrentDirectory( dirName );
	::SetCurrentDirectory( buf );
	return ( haveDir != 0 );
}
#endif
//=========================================================================  
#ifndef TRANSGAMING
String GFileManager::FileTime( const char* fileName )
{
    static const int    BUFSIZE = 16;
    static char         strTime[BUFSIZE];
    FILETIME ftSrc;
    SYSTEMTIME timeSrc;
    OFSTRUCT of;

#ifdef TRANSGAMING
    strTime[0] = 0;
#else
    strTime[0] = NULL;
#endif

    HFILE hFile = OpenFile( fileName, &of, OF_READ|OF_SHARE_DENY_NONE );
    if( hFile != HFILE_ERROR )
    {
        GetFileTime( (HANDLE)hFile, NULL, NULL, &ftSrc );
        CloseHandle( (HANDLE)hFile );

        FILETIME fTime;
        FileTimeToLocalFileTime(&ftSrc,&fTime);
        FileTimeToSystemTime(&fTime,&timeSrc);

        snprintf(strTime, BUFSIZE,
            "%04d%02d%02d%02d%02d%02d",
            timeSrc.wYear,
            timeSrc.wMonth,
            timeSrc.wDay,
            timeSrc.wHour,
            timeSrc.wMinute,
            timeSrc.wSecond
            );
    }

    return String(strTime);
}
#endif
//=========================================================================  
#ifndef TRANSGAMING
void GFileManager::RemoveFilesInDirectory( const char* dirName )
{
    String filePath;
    DynamicArray<String> fileNames;

#ifdef TRANSGAMING
    const int   BUFSIZE = 1024;
    char        buffer[BUFSIZE];
    int         dirNameLen = strlen(dirName);


    strncpy(buffer, dirName, BUFSIZE);


    if (dirNameLen && ((buffer[dirNameLen - 1] == '\\') || (buffer[dirNameLen - 1] == '/')))
        strncpy(&buffer[dirNameLen], "*", BUFSIZE - dirNameLen);

    else
        strncpy(&buffer[dirNameLen], "/*", BUFSIZE - dirNameLen);


    FindFiles(buffer, fileNames);
#else
    char buffer[1024];
    int dirNameLen = strlen(dirName);
    strcpy( buffer, dirName );


    if( dirNameLen && ( ( buffer[dirNameLen-1] == '\\' ) || ( buffer[dirNameLen-1] == '/' ) ) )
        buffer[dirNameLen-1] = 0;

    String realDirName = buffer;
    realDirName += "\\*";

    FindFiles( realDirName.AsChar(), fileNames );
#endif


    for( int i = 0; i < fileNames.Size(); ++i )
    {
        filePath = dirName;
        filePath += '\\';
        filePath += fileNames[i];
        if( !DeleteFile( filePath.AsChar() ) )
            GLog.Print( "Warning: File '%s' could not be deleted!\n", filePath.AsChar() );
    }
}
#endif
//=========================================================================  
#ifndef TRANSGAMING
void GFileManager::CreateZIP(LPCSTR strFName)
{
    char fn[MAX_PATH];


    strncpy(fn, strFName, MAX_PATH);
    ConvertSlashOnSelf(fn);

    zlib_filefunc_def ffunc;
    fill_win32_filefunc(&ffunc);
    
    ZipFile = zipOpen2(fn,false,NULL,&ffunc);    
}
//=========================================================================  
void GFileManager::CloseZIP()
{
    if (ZipFile)
        zipClose(ZipFile,NULL);
    
    ZipFile = NULL;
}
#endif
//=========================================================================  
#ifndef TRANSGAMING_USE_PHYSFS
extern long32 g_offset;
extern DynamicArray <FIdx*> g_FIdx;
void GFileManager::CreatePAK(LPCSTR strFName)
{
    char fn[MAX_PATH];

    strncpy(fn, strFName, MAX_PATH);
    ConvertSlashOnSelf(fn);
    
    if (PakFile)
        ClosePAK();

#ifdef TRANSGAMING
    PakFile = TNG_fopen(fn,"wb");
#else
    PakFile = fopen(fn,"wb");
#endif

    g_FIdx.ClearArray();
    g_offset = 0;

    if (PakFile)
    {
        FILE *f = PakFile;
        BYTE c = 1;
#ifdef TRANSGAMING
        TNG_fwrite(&c,1,sizeof(c),f);
        g_offset+=1;  

        long32 idx_start = 0;
        TNG_fwrite(&idx_start,1,sizeof(long32),f);
#else
        fwrite(&c,1,sizeof(c),f);
        g_offset+=1;  

        long32 idx_start = 0;
        fwrite(&idx_start,1,sizeof(long32),f);
#endif
        g_offset+=sizeof(long32);  
        
    }
}
extern char *CFN(char *buf,int l, int a); // crypt file name
//=========================================================================  
void GFileManager::ClosePAK()
{
    if (PakFile)
    {
        FILE *f = PakFile;

        long32 idx_start = g_offset;
        
        long32 cntF = g_FIdx.Size();
#ifdef TRANSGAMING
        TNG_fwrite(&cntF,1,sizeof(cntF),f);  
#else
        fwrite(&cntF,1,sizeof(cntF),f);  
#endif
        // zapis idx
        for (int i=0;i<cntF;i++)
        {
            char name[MAX_PATH];            
#ifdef TRANSGAMING
            strncpy(name, String::GetFilename(g_FIdx[i]->name), MAX_PATH);
            int l = strlen(name);
            CFN(name,l,i);

            TNG_fwrite(&l,1,sizeof(l),f);
            TNG_fwrite(name,1,l,f);
            TNG_fwrite(&g_FIdx[i]->offset,1,sizeof(g_FIdx[i]->offset),f);
            TNG_fwrite(&g_FIdx[i]->size,1,sizeof(g_FIdx[i]->size),f);
            TNG_fwrite(&g_FIdx[i]->csize,1,sizeof(g_FIdx[i]->csize),f);
#else
            strcpy(name,String::BaseName(g_FIdx[i]->name).AsChar());
            int l = strlen(name);
            CFN(name,l,i);

            fwrite(&l,1,sizeof(l),f);
            fwrite(name,1,l,f);
            fwrite(&g_FIdx[i]->offset,1,sizeof(g_FIdx[i]->offset),f);
            fwrite(&g_FIdx[i]->size,1,sizeof(g_FIdx[i]->size),f);
            fwrite(&g_FIdx[i]->csize,1,sizeof(g_FIdx[i]->csize),f);
#endif

            // przy okazji wyrzucamy z pamieci
            delete g_FIdx[i];
        }
        g_FIdx.ClearArray();

        // zapis startowego offsetu opisu
#ifdef TRANSGAMING
        TNG_fseek(f,1,SEEK_SET);        
        TNG_fwrite(&idx_start,1,sizeof(long32),f);
        TNG_fclose(f);

        TNG_fclose(PakFile);
#else
        fseek(f,1,SEEK_SET);        
        fwrite(&idx_start,1,sizeof(long32),f);
        fclose(f);

        fclose(PakFile);
#endif
    }
    PakFile = NULL;
}
#endif  //!TRANSGAMING_USE_PHYSFS
//=========================================================================  
#ifndef TRANSGAMING_USE_PHYSFS
void GFileManager::RegisterFile(LPCSTR fname,FIdx *pFIdx)
{
    if (!pRegisteredFiles)
	{
#ifdef TRANSGAMING
        m_setTag("File");
        pRegisteredFiles  = new HashMap <String,FIdx*,16384 * 4>;
        m_setTag("File");
		pFolders = new HashMap <String,FIdx*, 8192>;
#else
        pRegisteredFiles  = new HashMap <String,FIdx*,16384*4>;
		pFolders = new HashMap <String,FIdx*,8192>;
#endif
	}

	String fullpath = String(pFIdx->pPack->GetPath()) + String(fname);	
	fullpath.ToLower();
	String path = String::GetPath(fullpath);
	
	if (fullpath.AsChar()[fullpath.Len()-1] == '/')
		path = String::GetPath(path);

	FIdx **ppFolder =  pFolders->Find(path);    
	FIdx **ppFile    =  pRegisteredFiles->Find(fullpath);
	bool skipFolder = false;	
    
	if (ppFile)
    {	
				
		FIdx * p = *ppFile;
		if (p->prev)
		{
			p->prev->next = pFIdx;		
			pFIdx->prev = p->prev;			
			skipFolder = true;
		}
		if (p->next)
		{
			p->next->prev = pFIdx;
			pFIdx->next = p->next;
			skipFolder = true;
		}
		
		if (ppFolder && p->prev == NULL) // first in folder
		{
			pFolders->ChangeValue(path,pFIdx);
			skipFolder = true;
		}
			
        delete (*ppFile);
		pRegisteredFiles->ChangeValue(fullpath,pFIdx);
    }
    else{
		pRegisteredFiles->Insert(fullpath,pFIdx);

#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
        if (pRegisteredFiles->Size() > mMaxpRegisteredFilesUsage){
            mMaxpRegisteredFilesUsage = pRegisteredFiles->Size();
            TRACE("@@@ GFileManager::RegisterFile(): peak pRegisteredFiles hash usage = %d\n", mMaxpRegisteredFilesUsage);
        }
#endif
    }

	
	if (!skipFolder)
	{
		if (ppFolder)
		{
			FIdx * p = *ppFolder;
			while (p->next)
				p = p->next;

			p->next = pFIdx;
			pFIdx->prev = p;		
		}
        else{
			pFolders->Insert(path,pFIdx);

#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
            if (pFolders->Size() > mMaxpFoldersUsage){
                mMaxpFoldersUsage = pFolders->Size();
                TRACE("@@@ GFileManager::RegisterFile(): peak pFolders hash usage = %d\n", mMaxpFoldersUsage);
            }
#endif

        }
	}
}
#endif
//=========================================================================  
