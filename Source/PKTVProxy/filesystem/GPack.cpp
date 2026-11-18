#ifdef TRANSGAMING
# include <msvcrt/direct.h>
# include <msvcrt/io.h>
#endif
#include "../Engine.h"
#include "gfile.h"
#include "gmemfile.h"
#include "gpack.h"
#include "zlib.h"
#include "../nommgr.h"
#include "WildCard.h"
#include "../mmgr.h"
#include "GFileManager.h"

#if defined( TRANSGAMING_PSP )
# include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
# include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
# include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

extern BYTE mem[6000000];
extern BYTE mem1[6000000];
LPSTR ConvertSlash(LPCSTR path);
void ConvertSlashOnSelf(LPSTR path);
LPCSTR GetLwr(char *buf);


char *CFN(char *buf,int l, int a) // crypt file name
{
    
    for (int i=0 ; i<l ; i++)
        buf[i] ^= l*2+i*2+l%5+a;

    return buf;
}


//===========================================================================
GPack::GPack()
{
  compressed = false;
  hMapping = NULL;
  pMapMem = NULL;
  UsingSharedMemory = true;
  pfile = NULL;
  pMyManager = NULL;
}
//===========================================================================
GPack::~GPack()
{
  Release();
}
//===========================================================================
void GPack::Release()
{
  
  if (pfile)
#ifdef TRANSGAMING
      TNG_fclose(pfile);
#else
      fclose(pfile);
#endif

  if (pMapMem)
  {
      BOOL ok = UnmapViewOfFile(pMapMem);
      if (ok == 0)
      {
          //int err = GetLastError();
          MessageBox(NULL,"UnmapViewOfFile","ERROR!",MB_TOPMOST);			
      }
      pMapMem = NULL;
  }

  if (hMapping) CloseHandle (hMapping);
  hMapping = NULL;
}
//===========================================================================
bool GPack::OpenPack(LPCSTR _strFName,LPCSTR path)
{
  Release();

  strcpy(strFName,_strFName);
  strcpy(strPath,path);  
  
#ifdef TRANSGAMING
  pfile = TNG_fopen(strFName,"rb");
#else
  pfile = fopen(strFName,"rb");
#endif
  if (!pfile) return false;

  BYTE c = 0;
#ifdef TRANSGAMING
  TNG_fread(&c,1,sizeof(c),pfile);
#else
  fread(&c,1,sizeof(c),pfile);
#endif
  compressed = c?true:false;

  long ofs_idx  = 0;
#ifdef TRANSGAMING
  TNG_fread(&ofs_idx,1,sizeof(long),pfile);
  TNG_fseek(pfile,ofs_idx,SEEK_SET);
#else
  fread(&ofs_idx,1,sizeof(long),pfile);
  fseek(pfile,ofs_idx,SEEK_SET);
#endif

  long cntFIdx;
#ifdef TRANSGAMING
  TNG_fread(&cntFIdx,1,sizeof(cntFIdx),pfile);
#else
  fread(&cntFIdx,1,sizeof(cntFIdx),pfile);
#endif
  if (cntFIdx==0) return true;
    
  for (int i=0;i<cntFIdx;i++)
  {
    FIdx *pFIdx = new FIdx();
    pFIdx->pPack = this;
	pFIdx->searchCnt = 0;
    int l;
#ifdef TRANSGAMING
	TNG_fread(&l,1,sizeof(l),pfile);
    TNG_fread(pFIdx->name,1,l,pfile);
#else
    fread(&l,1,sizeof(l),pfile);
    fread(pFIdx->name,1,l,pfile);
#endif
    pFIdx->name[l] = NULL;
    CFN(pFIdx->name,l,i);
#ifdef TRANSGAMING
	TNG_fread(&pFIdx->offset,1,sizeof(pFIdx->offset),pfile);
    TNG_fread(&pFIdx->size,1,sizeof(pFIdx->size),pfile);    
    TNG_fread(&pFIdx->csize,1,sizeof(pFIdx->csize),pfile);    
#else
    fread(&pFIdx->offset,1,sizeof(pFIdx->offset),pfile);
    fread(&pFIdx->size,1,sizeof(pFIdx->size),pfile);    
    fread(&pFIdx->csize,1,sizeof(pFIdx->csize),pfile);    
#endif
    pFIdx->idx = i;
    char buf[MAX_PATH];
    strcpy(buf,pFIdx->name);
    strlwr(buf);    
	if (pMyManager)
		pMyManager->RegisterFile(buf,pFIdx);
  }
  
  return true;
}

//===========================================================================
GFile *GPack::GetFile(FIdx *pFidx)
{
  if (!pfile) return NULL;

  GMemFile *gf = new GMemFile();  

  unsigned long size = pFidx->size;
  if (compressed)
      size = pFidx->csize;
#ifdef TRANSGAMING
  TNG_fseek(pfile,pFidx->offset,SEEK_SET);  
#else  
  fseek(pfile,pFidx->offset,SEEK_SET);  
#endif
  if (!compressed)
  {            
      // to przyspieszyc dla milesa
      BYTE *ptr = (BYTE*) malloc(size);
      if (!ptr)
      {
          MessageBox(NULL,"Not Enough Memory","Error!!!",0);
          exit(0);
      }
#ifdef TRANSGAMING
	  TNG_fread(ptr,size,1,pfile);
#else
      fread(ptr,size,1,pfile);
#endif
      gf->Open(ptr,size,true);
  }
  else
  {
      bool mfree = false;

      BYTE *ptr;
      if (size < 6000000)
          ptr = mem1;
      else
      {
          mfree = true;
          ptr = (BYTE*) malloc(size);
          if (!ptr)
          {
              MessageBox(NULL,"Not Enough Memory","Error!!!",0);
              exit(0);
          }
      }
#ifdef TRANSGAMING
	  TNG_fread(ptr,size,1,pfile);
#else
      fread(ptr,size,1,pfile);
#endif
      
      long size = pFidx->size;
      if (size<6000000 && UsingSharedMemory)
      {
          uLong zl = 6000000;
          uncompress(mem,&zl,ptr,pFidx->csize);
          mem[size] = NULL;
          gf->Open(mem,size);
      }
      else
      {
          uLong zl = size;
          BYTE *mptr = (BYTE*) malloc(size);
          uncompress(mptr,&zl,ptr,pFidx->csize);
          gf->Open(mptr,size,true);
      }   

      if (mfree)
          free(ptr);
  }
  
  return gf;
}
//===========================================================================
void PackDirectory(FILE *f,LPCSTR strPath,bool compr);
long g_offset=0;
DynamicArray <FIdx*> g_FIdx;
long GPack::Make(LPCSTR strFName,LPCSTR strPath,bool compr)
{  
  g_offset = 0;
  g_FIdx.Clear();
#ifdef TRANSGAMING
  FILE *f = TNG_fopen(strFName,"wb");
#else
  FILE *f = fopen(strFName,"wb");
#endif
  if (!f) return 0;
  
  BYTE c = compr;
#ifdef TRANSGAMING
  TNG_fwrite(&c,1,sizeof(c),f);
#else
  fwrite(&c,1,sizeof(c),f);
#endif
  g_offset+=1;  

  long idx_start=0;
#ifdef TRANSGAMING
  TNG_fwrite(&idx_start,1,sizeof(long),f);
#else
  fwrite(&idx_start,1,sizeof(long),f);
#endif
  g_offset+=sizeof(long);  

  PackDirectory(f,strPath,compr);

  idx_start = g_offset;

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
    strcpy(name,&g_FIdx[i]->name[strlen(strPath)+1]);
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
#else
  fseek(f,1,SEEK_SET);        
  fwrite(&idx_start,1,sizeof(long),f);
  fclose(f);
#endif
  
  return cntF;
}


//===========================================================================
void PackDirectory(FILE *f,LPCSTR strPath,bool compr)
{
#ifdef TRANSGAMING_BOOGALOOGA
    long        hFind;
    _finddata_t FindFileData;
#else
  WIN32_FIND_DATA   FindFileData;
  HANDLE            hFind;
#endif

  char tmp[MAX_PATH],path[MAX_PATH];
  strcpy(path,strPath);
  if (path[strlen(path)-2] != '/') strcat(path,"/");
  strcpy(tmp,path);
  strcat(tmp,"*.*");
  

#ifdef TRANSGAMING_BOOGALOOGA
  hFind = _findfirst(tmp, &FindFileData);
  while (hFind != -1)
  {
    if (FindFileData.name[0] != '.')
    {      
      strcpy(tmp,path);
      strcat(tmp,FindFileData.name);
      if (FindFileData.attrib & _A_SUBDIR)
#else

  hFind = FindFirstFile(tmp, &FindFileData);
  while(hFind != INVALID_HANDLE_VALUE)
  {
    if (FindFileData.cFileName[0] != '.')
    {      
      strcpy(tmp,path);
      strcat(tmp,FindFileData.cFileName);
      if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
#endif
      {
        FIdx *idx = new FIdx();        
        strcpy(idx->name,tmp);
        strcat(idx->name,"/");
        idx->offset = g_offset;        
		idx->searchCnt = 0;
        idx->size = 0;        
        idx->csize = 0;        
        g_FIdx.Add(idx);
        g_offset +=  idx->csize;

        PackDirectory(f,tmp,compr); // rekurencja dla podkatalogow
      }
      else
      { 
#ifdef TRANSGAMING
		FILE *ft = TNG_fopen(tmp,"rb");        
        TNG_fseek(ft,0,SEEK_END);
        long len = TNG_ftell(ft);
        TNG_fseek(ft,0,SEEK_SET);        
#else
        FILE *ft = fopen(tmp,"rb");        
        fseek(ft,0,SEEK_END);
        long len =ftell(ft);
        fseek(ft,0,SEEK_SET);        
#endif
        BYTE *mptr = (BYTE*) malloc(len);

        uLong zl = 6000000;
#ifdef TRANSGAMING
		TNG_fread(mptr,len,1,ft);
#else
        fread(mptr,len,1,ft);
#endif
        if (!compr)
        {
#ifdef TRANSGAMING
			TNG_fwrite(mptr,len,1,f);
#else
            fwrite(mptr,len,1,f);            
#endif
            zl = len;
        }
        else
        {
            if (len < 5500000)
            {
                compress2(mem,&zl,mptr,len,Z_BEST_SPEED);            
#ifdef TRANSGAMING
				TNG_fwrite(mem,zl,1,f);
#else
                fwrite(mem,zl,1,f);
#endif
            }
            else
            {
                zl = len+2000;
                BYTE *ptr = (BYTE*) malloc(zl);
                compress2(ptr,&zl,mptr,len,Z_BEST_SPEED);
#ifdef TRANSGAMING
				TNG_fwrite(ptr,zl,1,f);
#else
                fwrite(ptr,zl,1,f);
#endif
                free (ptr);
            }                
        }                        
        
        free (mptr);
#ifdef TRANSGAMING
		TNG_fclose(ft);
#else
        fclose(ft);
#endif

        FIdx *idx = new FIdx();        
        strcpy(idx->name,tmp);
        idx->offset = g_offset;        
        idx->size = len;        
        idx->csize = zl; 
		idx->searchCnt = 0;
        
        g_FIdx.Add(idx);
        g_offset +=  idx->csize;
      }
       
    };

#ifdef TRANSGAMING_BOOGALOOGA
    if (_findnext(hFind, &FindFileData) != 0)
        break;
#else
    if (!FindNextFile(hFind,&FindFileData))
        break;
#endif
  };


#ifdef TRANSGAMING_BOOGALOOGA
  if (hFind != -1)
      _findclose(hFind);
#else
  if (hFind != INVALID_HANDLE_VALUE)
      FindClose(hFind);
#endif
}

long GPack::Extract(LPCSTR strFName,LPCSTR strPath)
{  
	GPack tmpPack;
	GFileManager tmpFM;

	tmpPack.pMyManager = &tmpFM;
	tmpPack.OpenPack(strFName,"");
	
	if (!tmpFM.pRegisteredFiles)
		return 0;

	for (int i=0;i<tmpFM.pRegisteredFiles->Size();i++)
	{
		FIdx * pFIdx =  tmpFM.pRegisteredFiles->GetPair(i).Value;
		String fname = String(strPath)+ String("\\") + String(pFIdx->name);

		GFile *f = tmpFM.CreateFileReader(pFIdx->name);	
		if (f->GetSize() > 0)
		{
			void *buf = malloc(f->GetSize());
			f->Read(buf,f->GetSize());

			GFile *fw = tmpFM.CreateFileWriter(fname.AsChar());	
			if (fw)
			{
				fw->Write(buf,f->GetSize());
				tmpFM.Close(fw);
			}
			free(buf);
		}		
		else
		{
			CreateDirectoryA(fname.AsChar(),NULL);
		}
		tmpFM.Close(f);
	}

	return 0;
}
