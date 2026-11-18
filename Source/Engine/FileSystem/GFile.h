
// #pragma once
#ifndef _GFILE_
#define _GFILE_

#include "Core/TypeTraits.h"
#include "Core/Debug.h"

class GFileManager;

#if !defined( TRANSGAMING ) || defined( TRANSGAMING_WINDOWS )
class ENGINE_API GFile
#else
class GFile
#endif
{
protected:
  void *pMyManager;
  FILE *f;
  char strFName[MAX_PATH];
  
public:
  GFile();
  virtual ~GFile();
  
  const char *GetName() {return strFName;};  virtual bool Open(LPCSTR fname,LPCSTR mode = "rb");
  virtual void Close();

  virtual int Read(void *buf,int length);
  virtual int ReadA(void *buf,int length);
  virtual int Write(const void* buf, int length);
  virtual void Flush();

  virtual fpos_t Tell();
  virtual int Seek(fpos_t pos,int mode=SEEK_SET);  
  virtual int GetC();
  virtual int GetS(LPSTR buf,int len);
  virtual long32 GetSize();

  bool PutToken( unsigned short token ) { return Write(&token,sizeof(unsigned short))>0?true:false; }
  bool TestToken( unsigned short token )
  {
	  unsigned short t;
	  bool ok = Read(&t,sizeof(unsigned short))?(t==token):false;
	  Assert(ok);
	  return ok;
  }

  DWORD GetCRC();

  virtual bool IsFile() {return true;};
  virtual bool IsMemFile() {return false;};
  virtual FILE *GetFILE() {return f;};

  friend class GFileManager;
};

#define FileObject     GFile

inline GFile& operator<<(GFile& file, const bool  value) { file.Write(&value,sizeof(bool));  return file; }
inline GFile& operator<<(GFile& file, const char  value) { file.Write(&value,sizeof(char));  return file; }
inline GFile& operator<<(GFile& file, const BYTE  value) { file.Write(&value,sizeof(BYTE));  return file; }
inline GFile& operator<<(GFile& file, const WORD  value) { file.Write(&value,sizeof(WORD));  return file; }
inline GFile& operator<<(GFile& file, const int   value) { file.Write(&value,sizeof(int));   return file; }
inline GFile& operator<<(GFile& file, const unsigned int   value) { file.Write(&value,sizeof(unsigned int));   return file; }
#if !defined(TRANSGAMING) || !defined(_WIN64)
inline GFile& operator<<(GFile& file, const DWORD value) { file.Write(&value,sizeof(DWORD)); return file; }
#endif
inline GFile& operator<<(GFile& file, const FLOAT value) { file.Write(&value,sizeof(FLOAT)); return file; }


inline GFile& operator>>(GFile& file, bool&  value) { file.Read(&value,sizeof(bool));  return file; }
inline GFile& operator>>(GFile& file, char&  value) { file.Read(&value,sizeof(char));  return file; }
inline GFile& operator>>(GFile& file, BYTE&  value) { file.Read(&value,sizeof(BYTE));  return file; }
inline GFile& operator>>(GFile& file, WORD&  value) { file.Read(&value,sizeof(WORD));  return file; }
inline GFile& operator>>(GFile& file, int&   value) { file.Read(&value,sizeof(int));   return file; }
inline GFile& operator>>(GFile& file, unsigned int&   value) { file.Read(&value,sizeof(unsigned int));   return file; }
#if !defined(TRANSGAMING) || !defined(_WIN64)
inline GFile& operator>>(GFile& file, DWORD& value) { file.Read(&value,sizeof(DWORD)); return file; }
#endif
inline GFile& operator>>(GFile& file, FLOAT& value) { file.Read(&value,sizeof(FLOAT)); return file; }



#endif //_GFILE_

