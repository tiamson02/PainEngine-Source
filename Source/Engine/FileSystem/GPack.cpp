#ifndef TRANSGAMING_USE_PHYSFS

#if defined( TRANSGAMING ) && !( defined TRANSGAMING_WINDOWS )
# include <msvcrt/direct.h>
# include <msvcrt/io.h>
#endif

#include "Engine.h"
#include "GFile.h"
#include "GMemFile.h"
#include "GPack.h"
#include "zlib.h"
#include "Core/nommgr.h"
#include "WildCard.h"
#include "Core/mmgr.h"
#include "GFileManager.h"

#ifndef TRANSGAMING
extern BYTE mem[6000000];
extern BYTE mem1[6000000];
#endif

#ifndef TRANSGAMING
LPSTR ConvertSlash(LPCSTR path);
void ConvertSlashOnSelf(LPSTR path);
LPCSTR GetLwr(char *buf);
#endif

#if defined( TRANSGAMING_PSP )
#include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
#include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
#include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

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
#ifdef TRANSGAMING
  hMapping = 0;
#else
  hMapping = NULL;
#endif
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

  pfile = NULL;
#else
      fclose(pfile);
#endif


  if (pMapMem)
  {
      BOOL ok = UnmapViewOfFile(pMapMem);
      if (ok == 0)
      {
          //int err = GetLastError();
#ifdef TRANSGAMING
          TRACE("[]::GPack::Release(): ERROR-> could not unmap the file!\n");
#else
          MessageBox(NULL,"UnmapViewOfFile","ERROR!",MB_TOPMOST);			
#endif
      }
      pMapMem = NULL;
  }

  if (hMapping) CloseHandle (hMapping);
#ifdef TRANSGAMING
  hMapping = 0;
#else
  hMapping = NULL;
#endif
}
//===========================================================================
bool GPack::OpenPack(LPCSTR _strFName,LPCSTR path)
{
  Release();

  
#ifdef TRANSGAMING
  strncpy(strFName, _strFName, MAX_PATH);
  strncpy(strPath, path, MAX_PATH);  

  pfile = TNG_fopen(strFName,"rb");
#else
  strcpy(strFName,_strFName);
  strcpy(strPath,path);  

  pfile = fopen(strFName,"rb");
#endif

  if (!pfile) return false;

  //read compressed flag
  BYTE c = 0;
#ifdef TRANSGAMING
  TNG_fread(&c,1,sizeof(c),pfile);
#else
  fread(&c,1,sizeof(c),pfile);
#endif
  compressed = c?true:false;

  //read the offset of the file table
  DWORD ofs_idx  = 0;
#ifdef TRANSGAMING
  TNG_fread(&ofs_idx,1,sizeof(ofs_idx),pfile);
  TNG_fseek(pfile,ofs_idx,SEEK_SET);
#else
  fread(&ofs_idx,1,sizeof(ofs_idx),pfile);
  fseek(pfile,ofs_idx,SEEK_SET);
#endif
  

  //read file count
  DWORD cntFIdx;
#ifdef TRANSGAMING
  TNG_fread(&cntFIdx,1,sizeof(cntFIdx),pfile);
#else
  fread(&cntFIdx,1,sizeof(cntFIdx),pfile);
#endif
  if (cntFIdx==0) return true;
    

  //buffer to store the filename in (to make it lower case)
  char buf[MAX_PATH];


  //read the entire file table
  for (int i=0;i<cntFIdx;i++)
  {
    m_setTag("Pack");
    FIdx *pFIdx = new FIdx();
    pFIdx->pPack = this;
	pFIdx->searchCnt = 0;
    int l;

    //read in the filename length, then the filename.  TNG-NOTE: this read should be limited to 
    //  256 characters (size of the pFIdx->name buffer)
#ifdef TRANSGAMING
    TNG_fread(&l,1,sizeof(l),pfile);
    TNG_fread(pFIdx->name,1,l,pfile);

    if (l >= 256){
        pFIdx->name[255] = 0;
        TRACE("********************************************************************************\n");
        TRACE("*** GPack::OpenPack(): ERROR-> buffer overrun in filename! (name = '%s', l = %d)\n", pFIdx->name, l);
        TRACE("********************************************************************************\n");
        Assert(0);
    }

    pFIdx->name[l] = 0;
#else
    fread(&l,1,sizeof(l),pfile);
    fread(pFIdx->name,1,l,pfile);
    pFIdx->name[l] = NULL;
#endif

    //encrypt the filename
    CFN(pFIdx->name,l,i);



    //read the file's offset, actual size, and compressed size
#ifdef TRANSGAMING
    TNG_fread(&pFIdx->offset,1,sizeof(pFIdx->offset),pfile);
    TNG_fread(&pFIdx->size,1,sizeof(pFIdx->size),pfile);    
    TNG_fread(&pFIdx->csize,1,sizeof(pFIdx->csize),pfile);    
#else
    fread(&pFIdx->offset,1,sizeof(pFIdx->offset),pfile);
    fread(&pFIdx->size,1,sizeof(pFIdx->size),pfile);    
    fread(&pFIdx->csize,1,sizeof(pFIdx->csize),pfile);    
#endif

    //save the file's index so its name can be decrypted later
    pFIdx->idx = i;


    //make the filename all lower case and register the file
#ifdef TRANSGAMING
    strncpy(buf, pFIdx->name, MAX_PATH);
#else
    strcpy(buf,pFIdx->name);
#endif
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

  m_setTag("Pack");
  GMemFile *gf = new GMemFile();  

  ulong32 size = pFidx->size;
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
      m_setTag("Pack");
      BYTE *ptr = (BYTE*) malloc(size);
      if (!ptr)
      {
#ifdef TRANSGAMING
          TRACE("[]::GPack::GetFile(): FATAL-> not enough memory!\n");
#else
          MessageBox(NULL,"Not Enough Memory","Error!!!",0);
#endif
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
#ifndef TRANSGAMING
      if (size < 6000000)
          ptr = mem1;
      else
#endif
      {
          mfree = true;
          m_setTag("Pack");
          ptr = (BYTE*) malloc(size);
          if (!ptr)
          {
#ifdef TRANSGAMING
              TRACE("[]::GPack::GetFile(): FATAL-> not enough memory!\n");
#else
              MessageBox(NULL,"Not Enough Memory","Error!!!",0);
#endif
              exit(0);
          }
      }
      
#ifdef TRANSGAMING
      TNG_fread(ptr,size,1,pfile);
#else
      fread(ptr,size,1,pfile);
#endif
      
      long32 size = pFidx->size;
#ifndef TRANSGAMING
      if (size<6000000 && UsingSharedMemory)
      {
          uLong zl = 6000000;
          uncompress(mem,&zl,ptr,pFidx->csize);
          mem[size] = NULL;
          gf->Open(mem,size);
      }
      else
#endif
      {
          uLong zl = size;
          m_setTag("Pack");
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
long32 g_offset=0;
DynamicArray <FIdx*> g_FIdx;
long32 GPack::Make(LPCSTR strFName,LPCSTR strPath,bool compr)
{  
  g_offset = 0;
  g_FIdx.ClearArray();

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

  DWORD idx_start=0;
#ifdef TRANSGAMING
  TNG_fwrite(&idx_start,1,sizeof(idx_start),f);
#else
  fwrite(&idx_start,1,sizeof(idx_start),f);
#endif
  g_offset+=sizeof(long32);  

  PackDirectory(f,strPath,compr);

  idx_start = g_offset;

  DWORD cntF = g_FIdx.Size();
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
    strncpy(name, &g_FIdx[i]->name[strlen(strPath)+1], MAX_PATH);
    int l = strlen(name);
    CFN(name,l,i);

    TNG_fwrite(&l,1,sizeof(l),f);
    TNG_fwrite(name,1,l,f);
    TNG_fwrite(&g_FIdx[i]->offset,1,sizeof(g_FIdx[i]->offset),f);
    TNG_fwrite(&g_FIdx[i]->size,1,sizeof(g_FIdx[i]->size),f);
    TNG_fwrite(&g_FIdx[i]->csize,1,sizeof(g_FIdx[i]->csize),f);
#else
    strcpy(name,&g_FIdx[i]->name[strlen(strPath)+1]);
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
  TNG_fwrite(&idx_start,1,sizeof(idx_start),f);
  TNG_fclose(f);
#else
  fseek(f,1,SEEK_SET);        
  fwrite(&idx_start,1,sizeof(idx_start),f);
  fclose(f);
#endif

  
  return cntF;
}


//===========================================================================
void PackDirectory(FILE *f,LPCSTR strPath,bool compr)
{
#ifdef TRANSGAMING_BOOGALOOGA
    long32        hFind;
    _finddata_t     FindFileData;
#else
    WIN32_FIND_DATA   FindFileData;
    HANDLE            hFind;
#endif
    char              tmp[MAX_PATH],path[MAX_PATH];


#ifdef TRANSGAMING
    strncpy(path, strPath, MAX_PATH);
    size_t len = strlen(path);

    if (path[len - 1] != '/'){
        path[len++] = '/';
        path[len] = 0;
    }

    strncpy(tmp, path, MAX_PATH);
    strncat(tmp, "*.*", MAX_PATH);
#else
    strcpy(path,strPath);
    if (path[strlen(path)-2] != '/') strcat(path,"/");
    strcpy(tmp,path);
    strcat(tmp,"*.*");
#endif
      
#ifdef TRANSGAMING_BOOGALOOGA
    hFind = _findfirst(tmp, &FindFileData);

    while(hFind != INVALID_HANDLE_VALUE)
    {
        if (FindFileData.name[0] != '.')
        {      
            strcpy(tmp,path);
            strcat(tmp,FindFileData.name);
            if (FindFileData.attrib & _A_SUBDIR)
            {
#else
    TRACE("PackDirectory(): packing the directory '%s'\n", strPath);
    hFind = FindFirstFile(tmp, &FindFileData);

    while(hFind != INVALID_HANDLE_VALUE)
    {
        if (FindFileData.cFileName[0] != '.')
        {      
# ifdef TRANSGAMING
            strncpy(tmp, path, MAX_PATH);
            strncat(tmp,FindFileData.cFileName, MAX_PATH);
# else
            strcpy(tmp,path);
            strcat(tmp,FindFileData.cFileName);
# endif

            if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
#endif

                m_setTag("Pack");
                FIdx *idx = new FIdx();        
#ifdef TRANSGAMING
                strncpy(idx->name, tmp, FIdx::NAMELENGTH);
                strncat(idx->name, "/", FIdx::NAMELENGTH);
#else
                strcpy(idx->name,tmp);
                strcat(idx->name,"/");
#endif
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
                long32 len = TNG_ftell(ft);
                TNG_fseek(ft,0,SEEK_SET);  
                m_setTag("Pack");
                BYTE *mptr = (BYTE*) malloc(len);

                uLong zl;
                TNG_fread(mptr,len,1,ft);
                if (!compr)
                {
                    TNG_fwrite(mptr,len,1,f);            
                    zl = len;
                }
                else{
                    zl = compressBound(len);

                    m_setTag("Pack");
                    BYTE *ptr = (BYTE*) malloc(zl);
                    compress2(ptr,&zl,mptr,len,Z_BEST_SPEED);
                    TNG_fwrite(ptr,zl,1,f);
                    free (ptr);
                }
                
                free (mptr);
                TNG_fclose(ft);
#else  
                FILE *ft = fopen(tmp,"rb");        
                fseek(ft,0,SEEK_END);
                long32 len =ftell(ft);
                fseek(ft,0,SEEK_SET);      
                m_setTag("Pack");
                BYTE *mptr = (BYTE*) malloc(len);

                uLong zl = 6000000;
                fread(mptr,len,1,ft);
                if (!compr)
                {
                    fwrite(mptr,len,1,f);            
                    zl = len;
                }
                else
                {
                    if (len < 5500000)
                    {
                        compress2(mem,&zl,mptr,len,Z_BEST_SPEED);            
                        fwrite(mem,zl,1,f);
                    }
                    else
                    {
                        zl = len+2000;
                        m_setTag("Pack");
                        BYTE *ptr = (BYTE*) malloc(zl);
                        compress2(ptr,&zl,mptr,len,Z_BEST_SPEED);            
                        fwrite(ptr,zl,1,f);
                        free (ptr);
                    }                
                }                        
                
                free (mptr);
                fclose(ft);
#endif

                m_setTag("Pack");
                FIdx *idx = new FIdx();        
#ifdef TRANSGAMING
                strncpy(idx->name, tmp, FIdx::NAMELENGTH);
#else
                strcpy(idx->name,tmp);
#endif
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

#ifndef TRANSGAMING
long32 GPack::Extract(LPCSTR strFName,LPCSTR strPath)
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
            m_setTag("Pack");
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
#endif

#endif //!TRANSGAMING_USE_PHYSFS
