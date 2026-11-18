#ifdef TRANSGAMING
# include <iofilemgr_stat.h>
#endif
#include "..\Engine.h"
#include "zip.h"
#include "GFile.h"
#include "GMemFile.h"
#include "GPack.h"
#include "GFileManager.h"
#include "iowin32.h" 
#include "../nommgr.h"
#include "WildCard.h"
#include "../mmgr.h"
#include "../LogBuffer.h"

#if defined( TRANSGAMING_PSP )
# include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
# include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
# include "../TNG_WIN/TNG_stdio_WIN.h"
#endif


#ifdef TRANSGAMING_BOOGALOOGA
struct ff
{
  String            fmask;
  long              h;
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



extern LogBuffer* GLog;

LPSTR ConvertSlash(LPCSTR path);
void ConvertSlashOnSelf(LPSTR path);
LPCSTR GetLwr(char *buf);
void SimplifyPathOnSelf(LPSTR fn);

#ifndef TRANSGAMING
BYTE mem[6000000]; // static buffer for decompressing data
BYTE mem1[6000000];
#endif
//===========================================================================
GFileManager::GFileManager()
{
    pRegisteredFiles = NULL;
	pFolders = NULL;
    PakFile = NULL;
    ZipFile = NULL;
	searchCnt = 0;
}
//===========================================================================
GFileManager::~GFileManager()
{
  for (int i=0;i<vFile.Size();i++)
  {
    vFile[i]->pMyManager = NULL;
    delete vFile[i];  
  }  
  vFile.Clear();

  for (i=0;i<vPack.Size();i++)
    delete vPack[i];  
  
  vPack.Clear();

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
  
}
//===========================================================================
GFile* GFileManager::Add(GFile* pGF)
{
  if (!pGF) return NULL;
  
  pGF->pMyManager = this;
  vFile.Add(pGF);
  return pGF;
}
//===========================================================================
GFile* GFileManager::CreateFileWriter(LPCSTR strFName,LPCSTR mode)
{
    if (!strFName || strFName[0] == '\0')
        return NULL;
    
    char wmode[3];
    strcpy(wmode,mode);

    if (strstr(mode,"wz") && !GetZIPFile())
        strcpy(wmode,"wb");

    if (strstr(mode,"wp") && !GetPAKFile())
        strcpy(wmode,"wb");

    char fn[MAX_PATH];
    strcpy(fn,strFName);
    ConvertSlashOnSelf(fn);

    if (strstr(wmode,"wz")) // write to zip
    {
        GMemFile *pF = new GMemFile();
        strcpy(pF->strFName,fn);
        strcpy(pF->wmode,"wz");
        pF->Open(mem,0,false);
        return Add(pF);
    }
    else if (strstr(wmode,"wp")) // write to pak
    {
        GMemFile *pF = new GMemFile();
        strcpy(pF->strFName,fn);
        strcpy(pF->wmode,"wp");
        pF->Open(mem,0,false);
        return Add(pF);
    }
    else
    {
        GFile *pF = new GFile();
        if (pF->Open(fn,wmode))
            return Add(pF);
        else
            delete (pF);
    }

    return NULL;  
}
//===========================================================================
GFile* GFileManager::CreateFileReader(LPCSTR strFName,LPCSTR mode)
{
    if (!strFName || strFName[0] == '\0')
        return NULL;

    char fn[MAX_PATH];
    strcpy(fn,strFName);
    ConvertSlashOnSelf(fn);
    strlwr(fn);

	//GLog->Print("GFileManager::CreateFileReader(%s)\n",fn);

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
	   SimplifyPathOnSelf(fn);
       FIdx **ppf = pRegisteredFiles->Find(fn);
        if (ppf)
            return Add((*ppf)->pPack->GetFile(*ppf));
    }

  return NULL;  
}
//===========================================================================
bool GFileManager::Copy(LPCSTR strSrcFName,LPCSTR strDstFName)
{
    GFile* pf = CreateFileReader(strSrcFName,"rb");
    if (!pf) 
	{
		return false;
	}
    long s = pf->GetSize();

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
//===========================================================================
void GFileManager::DetachFile(GFile* pGF)
{
    if (!pGF) return;

    int i = vFile.Find(pGF);
    if (i>-1)
    {
        pGF->pMyManager = NULL;
        vFile.FastRemove(i);
    }
}

//===========================================================================
void GFileManager::Close(GFile* pGF)
{
  if (!pGF) return;
  DetachFile(pGF);
  delete pGF;
}
//=========================================================================  
GPack *GFileManager::RegisterPack(GPack* pPack,LPCSTR fname,LPCSTR path,bool must_exist)
{  
    if (!pPack) return NULL;

    char pt[MAX_PATH];
    strcpy(pt,path);
    ConvertSlashOnSelf(pt);
    strlwr(pt);

    pPack->pMyManager = this;
    if (!pPack->OpenPack(fname,pt))
    {
        delete(pPack);

        char tmp[MAX_PATH];
#ifndef TRANSGAMING
        WIN32_FIND_DATA FindFileData;
        HANDLE hFind;
#endif

        strcpy(tmp,path);
        tmp[strlen(tmp)-1] = NULL;


#ifdef TRANSGAMING
        struct SceIoStat  st;


        if (sceIoGetstat(tmp, &st) != 0)
#else
        hFind = FindFirstFile(tmp, &FindFileData);
        if (hFind == INVALID_HANDLE_VALUE)
#endif
        {
          if (must_exist)
            MessageBox(NULL,"Pack or directory not found:",fname,MB_TOPMOST);
          else
            return NULL;
        }
#ifndef TRANSGAMING
        else
            FindClose(hFind);
#endif

        return NULL;
    }

    vPack.Add(pPack);
    
    return pPack;  
}
//=========================================================================  
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
//=========================================================================  
GPack *GFileManager::FindPackByPath(LPCSTR path)
{
  char fn[MAX_PATH];
  strcpy(fn,path);  
  ConvertSlashOnSelf(fn);
  strlwr(fn);
  
  for (int i=0;i<vPack.Size();i++)
  {
    char * s = strstr(fn,vPack[i]->GetPath());
    if (s) return vPack[i];
  }       
  return NULL;
}
//=========================================================================  
void GFileManager::FF_MarkPackedFileAsFound(long h, LPCSTR file)
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
long GFileManager::FF_Open(LPCSTR path)
{  
  searchCnt++;
  ff *pFF = new ff();

 // na dysku

  pFF->fmask = strlwr(ConvertSlash(path));
#ifdef TRANSGAMING_BOOGALOOGA
  pFF->h = _findfirst(path, &pFF->fd);

  if (pFF->h != -1) 
#else
  pFF->h = FindFirstFile(path,&pFF->fd);

  if (pFF->h != INVALID_HANDLE_VALUE) 
#endif
  {
	  FF_MarkPackedFileAsFound((long)pFF,pFF->fd.cFileName);
      return (long) pFF;
  }
  
  // w plikach pakowanych
  pFF->packed = true;
  if (FF_Next((long)pFF) > 0)
	  return (long) pFF;
  else
	  delete (pFF);


  return -1;
}
//=========================================================================  
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
//=========================================================================  
long GFileManager::FF_Next(long h)
{
  ff *pFF = (ff *)h;
  if (pFF == NULL) return 0;
  
  // zwykle pliki
  if (!pFF->packed)
  {

#ifdef TRANSGAMING_BOOGALOOGA
    if (_findnext(pFF->h, &pFF->fd) == 0)
	{
		FF_MarkPackedFileAsFound((long)pFF,pFF->fd.name);
		return 1;
	}
#else
    if (FindNextFile(pFF->h,&pFF->fd))
	{
		FF_MarkPackedFileAsFound((long)pFF,pFF->fd.cFileName);
		return 1;
	}
#endif
    else
	{
		// nastepnie w plikach pakowanych
		pFF->found = NULL;
		pFF->packed = true;
		return FF_Next((long)pFF);
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
				  return (long) pFF->found;
			  }
		  }
		  pFIdx = pFIdx->next;
	  }
  }  
  return -1;
}
//=========================================================================  
LPCSTR GFileManager::FF_GetName(long h)
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

     for (int b=len; b>0;b--){
         if (pFIdx->name[b] == '/' || pFIdx->name[b] == '\\')
         {
             b++;
             break;
         }
     }

      strcpy(name,&pFIdx->name[b]);  
      if (name[0] == '\0')
          strcpy(name,".");
      
      if (directory)
          name[strlen(name)-1] = NULL;
      
      return name;
  }  

  return NULL;
}
//=========================================================================  
bool GFileManager::FF_IsDirectory(long h)
{
    ff *pFF = (ff *)h;
    if (pFF == NULL) return 0;
    
    // zwykle pliki
    if (!pFF->packed){
#ifdef TRANSGAMING_BOOGALOOGA
        return (pFF->fd.attrib & _A_SUBDIR) ? true : false;
#else
        return (pFF->fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
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
void GFileManager::FF_Close(long h)
{
    ff *pFF = (ff *)h;
    if (pFF == NULL) return;
    
    if (pFF->h){
#ifdef TRANSGAMING_BOOGALOOGA
        _findclose(pFF->h);
#else
        FindClose(pFF->h);
#endif
    }

    delete (pFF);  
}
//=========================================================================  
bool GFileManager::FileExist(LPCSTR strFName)
{
	bool ret = false;
	char fn[MAX_PATH];

	strcpy(fn,strFName);
	ConvertSlashOnSelf(fn);
	strlwr(fn);

#ifdef TRANSGAMING
	FILE *f = TNG_fopen(fn,"rb");
#else
	FILE *f = fopen(fn,"rb");
#endif
	if (f)
	{
		ret = true;
#ifdef TRANSGAMING
		TNG_fclose(f);
#else
		fclose(f);
#endif
	}
    else if (pRegisteredFiles)
	{
		SimplifyPathOnSelf(fn);
        FIdx **ppf = pRegisteredFiles->Find(fn);
        if (ppf)
            ret = true;
	}

	return ret;
}
//=========================================================================  
unsigned long GFileManager::FileSize(LPCSTR strFName)
{
    char fn[MAX_PATH];

    strcpy(fn,strFName);
    ConvertSlashOnSelf(fn);
    strlwr(fn);

    unsigned long l = 0;

#ifdef TRANSGAMING
	FILE *f = TNG_fopen(fn,"rb");
#else
    FILE *f = fopen(fn,"rb");
#endif
    if (f)
    {
#ifdef TRANSGAMING
		TNG_fseek(f,0,SEEK_END);
        l = TNG_ftell(f);
        TNG_fclose(f);
#else
        fseek(f,0,SEEK_END);
        l = ftell(f);
        fclose(f);
#endif
    }
    else if (pRegisteredFiles)
    {
        FIdx **ppf = pRegisteredFiles->Find(fn);
        if (ppf)
            l = (*ppf)->size;
    }

    return l;
}
//=========================================================================  
void GFileManager::FindFiles(const char* dirName, DynamicArray<String>& list, DWORD mode)
{
    long h = FF_Open(dirName);
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
                list.Add(String(name));
			}
		}
		while(FF_Next(h)>0);

        FF_Close(h);
	}
}

String GetCorrectFileName(LPCSTR fname)
{
	char tmp[MAX_PATH];
	strcpy(tmp,fname);

	strlwr(tmp);
	ConvertSlashOnSelf(tmp);	
	SimplifyPathOnSelf(tmp);
	return tmp;
}


//=========================================================================  
// Common tools
//=========================================================================  
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
void SimplifyPathOnSelf(LPSTR fn)
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
					strcpy(&fn[i],start+3);
					s = strstr(fn,"/../");
					break;
				}
			}
		}
	}
}
//=========================================================================  
LPCSTR GetLwr(char *buf)
{
	static char ret[MAX_PATH];
	strcpy(ret,buf);
	strlwr(ret);
	return ret;
}
//=========================================================================  
bool GFileManager::CreateDirectory( const char* dirName )
{
    return( ::CreateDirectory( dirName, NULL ) == TRUE );
}
//=========================================================================  
bool GFileManager::TestDirectory( const char* dirName )
{
	TCHAR buf[1024];
	if( !::GetCurrentDirectory( 1024, buf ) ) return false;
	BOOL haveDir = ::SetCurrentDirectory( dirName );
	::SetCurrentDirectory( buf );
	return ( haveDir != 0 );
}
//=========================================================================  
String GFileManager::FileTime( const char* fileName )
{
    static char strTime[15];
    FILETIME ftSrc;
    SYSTEMTIME timeSrc;
    OFSTRUCT of;

    strTime[0] = NULL;

    HFILE hFile = OpenFile( fileName, &of, OF_READ|OF_SHARE_DENY_NONE );
    if( hFile != HFILE_ERROR )
    {
        GetFileTime( (HANDLE)hFile, NULL, NULL, &ftSrc );
        CloseHandle( (HANDLE)hFile );

        FILETIME fTime;
        FileTimeToLocalFileTime(&ftSrc,&fTime);
        FileTimeToSystemTime(&fTime,&timeSrc);

        sprintf(strTime,
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
//=========================================================================  
void GFileManager::RemoveFilesInDirectory( const char* dirName )
{
    String filePath;
    DynamicArray<String> fileNames;

    char buffer[1024];
    int dirNameLen = strlen(dirName);
    strcpy( buffer, dirName );
    if( dirNameLen && ( ( buffer[dirNameLen-1] == '\\' ) || ( buffer[dirNameLen-1] == '/' ) ) )
        buffer[dirNameLen-1] = 0;

    String realDirName = buffer;
    realDirName += "\\*";

    FindFiles( realDirName.AsChar(), fileNames );

    for( int i = 0; i < fileNames.Size(); ++i )
    {
        filePath = dirName;
        filePath += '\\';
        filePath += fileNames[i];
        if( !DeleteFile( filePath.AsChar() ) )
            GLog->Print( "Warning: File '%s' could not be deleted!\n", filePath );
    }
}
//=========================================================================  
void GFileManager::CreateZIP(LPCSTR strFName)
{
    char fn[MAX_PATH];
    strcpy(fn,strFName);
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
//=========================================================================  
extern long g_offset;
extern DynamicArray <FIdx*> g_FIdx;
void GFileManager::CreatePAK(LPCSTR strFName)
{
    char fn[MAX_PATH];
    strcpy(fn,strFName);
    ConvertSlashOnSelf(fn);
    
    if (PakFile)
        ClosePAK();
#ifdef TRANSGAMING
	PakFile = TNG_fopen(fn,"wb");
#else
    PakFile = fopen(fn,"wb");
#endif

    g_FIdx.Clear();
    g_offset = 0;

    if (PakFile)
    {
        FILE *f = PakFile;
        BYTE c = 1;
#ifdef TRANSGAMING
		TNG_fwrite(&c,1,sizeof(c),f);
#else
        fwrite(&c,1,sizeof(c),f);
#endif
        g_offset+=1;  

        long idx_start = 0;
#ifdef TRANSGAMING
		TNG_fwrite(&idx_start,1,sizeof(long),f);
#else
        fwrite(&idx_start,1,sizeof(long),f);
#endif
        g_offset+=sizeof(long);  
        
    }
}
extern char *CFN(char *buf,int l, int a); // crypt file name
//=========================================================================  
void GFileManager::ClosePAK()
{
    if (PakFile)
    {
        FILE *f = PakFile;

        long idx_start = g_offset;
        
        long cntF = g_FIdx.Size();
#ifdef TRANSGAMING
		TNG_fwrite(&cntF,1,sizeof(cntF),f);  
#else
        fwrite(&cntF,1,sizeof(cntF),f);  
#endif
        // zapis idx
        for (int i=0;i<cntF;i++)
        {
            char name[MAX_PATH];            
            strcpy(name,String::BaseName(g_FIdx[i]->name).AsChar());
            int l = strlen(name);
            CFN(name,l,i);
#ifdef TRANSGAMING
			TNG_fwrite(&l,1,sizeof(l),f);
            TNG_fwrite(name,1,l,f);
            TNG_fwrite(&g_FIdx[i]->offset,1,sizeof(g_FIdx[i]->offset),f);
            TNG_fwrite(&g_FIdx[i]->size,1,sizeof(g_FIdx[i]->size),f);
            TNG_fwrite(&g_FIdx[i]->csize,1,sizeof(g_FIdx[i]->csize),f);
#else
            fwrite(&l,1,sizeof(l),f);
            fwrite(name,1,l,f);
            fwrite(&g_FIdx[i]->offset,1,sizeof(g_FIdx[i]->offset),f);
            fwrite(&g_FIdx[i]->size,1,sizeof(g_FIdx[i]->size),f);
            fwrite(&g_FIdx[i]->csize,1,sizeof(g_FIdx[i]->csize),f);
#endif

            // przy okazji wyrzucamy z pamieci
            delete g_FIdx[i];
        }
        g_FIdx.Clear();

        // zapis startowego offsetu opisu
#ifdef TRANSGAMING
		TNG_fseek(f,1,SEEK_SET);        
        TNG_fwrite(&idx_start,1,sizeof(long),f);
        TNG_fclose(f);

        TNG_fclose(PakFile);
#else
        fseek(f,1,SEEK_SET);        
        fwrite(&idx_start,1,sizeof(long),f);
        fclose(f);

        fclose(PakFile);
#endif
    }
    PakFile = NULL;
}
//=========================================================================  
void GFileManager::RegisterFile(LPCSTR fname,FIdx *pFIdx)
{
    if (!pRegisteredFiles)
	{
        pRegisteredFiles  = new HashMap <String,FIdx*,16384*4>;
		pFolders = new HashMap <String,FIdx*,8192>;
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
	else
		pRegisteredFiles->Insert(fullpath,pFIdx);

	
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
		else
			pFolders->Insert(path,pFIdx);
	}
}
//=========================================================================  