#include "../Engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <windef.h>
#include "gfile.h"
#include "gfilemanager.h"

#if defined( TRANSGAMING_PSP )
# include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
# include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
# include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

GFile::GFile(): 
  f(NULL)
, pMyManager(NULL)
{
    strFName[0] = '\0';
}

GFile::~GFile()
{
    if (pMyManager)
        ((GFileManager*)pMyManager)->DetachFile(this);
    Close();    
};

bool GFile::Open(LPCSTR fname,LPCSTR mode)
{
  Close();
  strcpy(strFName,fname);
#ifdef TRANSGAMING
  f = TNG_fopen(fname,mode);
#else
  f = fopen(fname,mode);
#endif
  if (!f) return false;
  
  return true;
}

void GFile::Close()
{
#ifdef TRANSGAMING
  if (f) TNG_fclose(f);
#else
  if (f) fclose(f);
#endif
  f = NULL;
}

int GFile::Read(void *buf,int length)
{
    if (!f) return 0;
#ifdef TRANSGAMING
	return TNG_fread(buf, 1, length, f);
#else
    return fread(buf, 1, length, f);
#endif
}

int GFile::ReadA(void *buf,int length)
{
    if (!f) return 0;

#ifdef TRANSGAMING
	return TNG_fread(buf, 1, length, f);
#else
    return fread(buf, 1, length, f);
#endif
}

int GFile::Write(const void *buf,int length)
{
    if (!f) return 0;

#ifdef TRANSGAMING
	return TNG_fwrite(buf,length,1,f);
#else
    return fwrite(buf,length,1,f);
#endif
}

void GFile::Flush()
{
    if (!f) return;

#ifdef TRANSGAMING
	TNG_fflush(f);
#else    
    fflush(f);
#endif
}

fpos_t GFile::Tell()
{
  if (!f) return 0;

#ifdef TRANSGAMING
  return TNG_ftell(f);
#else
  return ftell(f);
#endif
}

int GFile::Seek(fpos_t pos,int mode)
{
  if (!f) return -1;

  if (mode == SEEK_SET)
#ifdef TRANSGAMING
	TNG_fsetpos(f,&pos);
#else
    fsetpos(f,&pos);
#endif

#ifdef TRANSGAMING
  return TNG_fseek(f,pos,mode);
#else
  return fseek(f,pos,mode);
#endif
}

int GFile::GetC()
{
  if (!f) EOF;
#ifdef TRANSGAMING
  return TNG_getc(f);
#else
  return getc(f);  
#endif
}

int GFile::GetS(LPSTR buf,int len)
{
  for (int i=0;i<len;i++)
  {
    if (Read(&buf[i],1) == 0)
    {
      buf[i] = NULL;
      return i;
    }

    
    if (buf[i]=='\r') 
    {
      buf[i] = '\n';
      buf[i+1] = NULL;
      char tmp;
      Read(&tmp,1); // pominiecie \n
      return i+1;
    }

    if (buf[i]=='\n') 
    {
      buf[i] = '\n';
      buf[i+1] = NULL;
      return i+1;
    }
  }
  return 0;
};

long  GFile::GetSize()
{
  long p = Tell();
  Seek(0,SEEK_END);
  long l = Tell();
  Seek(p,SEEK_SET);  
  return l;
}

// CRC

extern DWORD GCRCTable[256];

DWORD GFile::GetCRC()
{
	// Move to beginning of file
	long p = Tell();
	Seek(0,SEEK_SET);

#define COMPUTE(var, ch) (var) = ((var) << 8) ^ GCRCTable[(((var) >> 24) ^ (ch))&0xff]

	DWORD crc = 0;
	unsigned long len = 0;

	char buf[1024];
	int partL;
	while( ( partL = Read( buf, 1024 ) ) != 0 )
	{
		for( int i = 0; i < partL; ++i )
		{
			COMPUTE( crc, buf[i] );
			++len;
		}
	}

	// Include the length of the file
	for( ; len != 0; len >>= 8 )
	{
		COMPUTE (crc, len & 0xff);
	}

	// Restore file pos
	Seek(p,SEEK_SET);
	return ~crc;
#undef COMPUTE
}