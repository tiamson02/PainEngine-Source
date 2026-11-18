#ifndef TRANSGAMING_USE_PHYSFS

#include "Engine.h"
#if !( defined( TRANSGAMING_PS2 ) || defined(TRANSGAMING_PSP) )
#include <io.h>
#endif
#include "GFile.h"
#include "GMemFile.h"
#include "zip.h"


#if defined( TRANSGAMING_PSP )
#include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
#include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
#include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

#include "Core/FileSystem/FileManagerStandard.h"
extern ENGINE_API GFileManager  	GFileMan;


//extern BYTE mem[4000000];
#ifndef TRANSGAMING
extern BYTE mem1[4000000];
#endif

GMemFile::GMemFile() 
: GFile()
,mptr(NULL)
,mpos(0)
,mfree(false)
,size(0)
,pMapMem(NULL)
,mapOffset(0)
{
}

GMemFile::~GMemFile()
{

    if (pMyManager)
        ((GFileManager*)pMyManager)->DetachFile(this);
    Close();    
};


bool GMemFile::Open(LPCSTR fname,LPCSTR mode)
{
  Close();

#ifdef TRANSGAMING
  strncpy(strFName, fname, MAX_PATH);

  FILE *ft = TNG_fopen(fname,mode);
#else
  strcpy(strFName,fname);
  FILE *ft = fopen(fname,mode);
#endif
  if (!ft) return false;

#ifdef TRANSGAMING
  TNG_fseek(ft,0,SEEK_END);
  size = TNG_ftell(ft);
  TNG_fseek(ft,0,SEEK_SET);
#else
  fseek(ft,0,SEEK_END);
  size =ftell(ft);
  fseek(ft,0,SEEK_SET);
#endif

  m_setTag("File");
  mptr = (BYTE*) malloc(size);
#ifdef TRANSGAMING
  TNG_fread(mptr,size,1,ft);
  TNG_fclose(ft);
#else
  fread(mptr,size,1,ft);
  fclose(ft);
#endif

  mfree = true;
  
  return true;
}

bool GMemFile::Open(void *ptr,long32 _size,bool mf)
{
  Close();
  
  if (!ptr) return false;
  
  mptr = (BYTE*) ptr;
  size = _size;
  mpos = 0;

  mfree = mf;
  
  return true;
}

extern long32 g_offset;
extern DynamicArray <FIdx*> g_FIdx;

void GMemFile::Close()
{
    if (!mptr)
        return;

#ifndef TRANSGAMING
    if (strstr(wmode,"wz"))
    {

         zipOpenNewFileInZip3(GFileMan.GetZIPFile(),String::BaseName(strFName).AsChar(),NULL,
            NULL,0,NULL,0,NULL /* comment*/,
            Z_DEFLATED,
            Z_BEST_SPEED,0,
            -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
            NULL,false);

        zipWriteInFileInZip(GFileMan.GetZIPFile(),mptr,size);
        zipCloseFileInZip(GFileMan.GetZIPFile());        
    }
#endif

    if (strstr(wmode,"wp"))
    {
        if (GFileMan.GetPAKFile())
        {

#ifdef TRANSGAMING
            //grab the upper bound on the compressed file size and allocate a buffer to fit it
            uLong zl = compressBound(size);
            Bytef *tmpBuf = new Bytef[zl];

            if (tmpBuf){
                compress2(tmpBuf, &zl, mptr, size, 3);
                TNG_fwrite(tmpBuf, zl, 1, GFileMan.GetPAKFile());
                delete []tmpBuf;
            }
#else
            uLong zl = 4000000;

            compress2(mem1,&zl,mptr,size,Z_BEST_SPEED);            
            fwrite(mem1,zl,1,GFileMan.GetPAKFile());
#endif
            //fwrite(mptr,size,1,GFileMan.GetPAKFile());
            m_setTag("Pack");
            FIdx *idx = new FIdx();        

#ifdef TRANSGAMING
            strncpy(idx->name, strFName, FIdx::NAMELENGTH);
#else
            strcpy(idx->name,strFName);
#endif
            idx->offset = g_offset;        
            idx->size = size;        
            idx->csize = zl;        
            g_FIdx.Add(idx);
            g_offset +=  idx->csize;
        }
    }

    if (mfree && mptr)
        free(mptr);


	if (pMapMem)
	{
		void * un = ((BYTE*)pMapMem)-mapOffset;
		BOOL ok = UnmapViewOfFile(un);
		if (ok == 0)
		{
            //int err = GetLastError();
#ifdef TRANSGAMING
            TRACE("[]::GMemFile::Close(): ERROR-> could not unmap the file\n");
#else
            MessageBox(NULL,"UnmapViewOfFile","ERROR!",MB_TOPMOST);			
#endif
		}
	}

	pMapMem = NULL;
    mptr = NULL; 
    mpos=0; 
    mfree=false;  
}

int GMemFile::Read(void *buf,int length)
{
  if (!mptr || mpos>=size) return 0;

  if (mpos+length > size) length = size-mpos;

  memcpy(buf,&mptr[mpos],length);
  mpos+=length;
  return length;
}

fpos_t GMemFile::Tell() 
{
  if (!mptr) return 0;
  
  return mpos;
}

int GMemFile::Seek(fpos_t pos,int mode)
{
  if (!mptr) return -1;

  if (mode == SEEK_SET) mpos = pos;
  if (mode == SEEK_CUR) mpos += pos;
  if (mode == SEEK_END) mpos = size + pos; // gdy chce przeczytac koncowke 
  
  return 0; // OK
}

int GMemFile::GetC()
{
  if (!mptr) return EOF;
  int c = mptr[mpos];
  mpos++;
  return c;  
}

int GMemFile::Write(const void* buf, int length)
{
    memcpy(&mptr[mpos],buf,length);
    mpos = mpos + length;
    if (mpos > size)
        size = mpos;
    return length;
}

#endif //!TRANSGAMING_USE_PHYSFS
