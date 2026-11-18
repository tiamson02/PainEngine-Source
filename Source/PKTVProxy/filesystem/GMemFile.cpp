#include "../Engine.h"
#include <io.h>
#include "gfile.h"
#include "gmemfile.h"
#include "zip.h"

#if defined( TRANSGAMING_PSP )
# include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
# include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
# include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

#include "GFileManager.h"
extern GFileManager*  	GFileMan;


extern BYTE mem[4000000];
extern BYTE mem1[4000000];

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
  strcpy(strFName,fname);

#ifdef TRANSGAMING
  FILE *ft = TNG_fopen(fname,mode);
#else
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

bool GMemFile::Open(void *ptr,long _size,bool mf)
{
  Close();
  
  if (!ptr) return false;
  
  mptr = (BYTE*) ptr;
  size = _size;
  mpos = 0;

  mfree = mf;
  
  return true;
}

extern long g_offset;
extern DynamicArray <FIdx*> g_FIdx;

void GMemFile::Close()
{
    if (!mptr)
        return;

    if (strstr(wmode,"wz"))
    {

         zipOpenNewFileInZip3(GFileMan->GetZIPFile(),String::BaseName(strFName).AsChar(),NULL,
            NULL,0,NULL,0,NULL /* comment*/,
            Z_DEFLATED,
            Z_BEST_SPEED,0,
            -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
            NULL,false);

        zipWriteInFileInZip(GFileMan->GetZIPFile(),mptr,size);
        zipCloseFileInZip(GFileMan->GetZIPFile());        
    }

    if (strstr(wmode,"wp"))
    {
        if (GFileMan->GetPAKFile())
        {
            uLong zl = 4000000;
            compress2(mem1,&zl,mptr,size,Z_BEST_SPEED);            
#ifdef TRANSGAMING
			TNG_fwrite(mem1,zl,1,GFileMan->GetPAKFile());
#else
            fwrite(mem1,zl,1,GFileMan->GetPAKFile());
#endif
            //fwrite(mptr,size,1,GFileMan->GetPAKFile());
            FIdx *idx = new FIdx();        
            strcpy(idx->name,strFName);
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
            MessageBox(NULL,"UnmapViewOfFile","ERROR!",MB_TOPMOST);			
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

