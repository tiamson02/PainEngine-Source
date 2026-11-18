// #pragma once
#ifndef TRANSGAMING_USE_PHYSFS

#ifndef _GMEMFILE_
#define _GMEMFILE_

class ENGINE_API GMemFile: public GFile
{
protected:
  char wmode[3];
  bool mfree;
  BYTE *mptr;
  long32 mpos;
  long32 size;
  long32 mapOffset;
  void *pMapMem;

public:
  GMemFile();
  virtual ~GMemFile();

  virtual bool Open(void *ptr,long32 size,bool mf = false);
  virtual bool Open(LPCSTR fname,LPCSTR mode = "rb");
  virtual void Close();    
  virtual void *GetPtr() {return mptr;};
  virtual void *GetCurPtr() 
  {
    if (!mptr) return NULL;
    return &mptr[mpos];
  };
  
  virtual long32 GetSize() {return size;};
  virtual int Read(void *buf,int length);
  virtual int ReadA(void *buf,int length) {return Read(buf,length);};
  virtual int Write(const void* buf, int length);
  virtual fpos_t Tell();
  virtual int Seek(fpos_t pos,int mode=SEEK_SET);  
  virtual int GetC();
  virtual bool IsFile() {return true;};
  virtual bool IsMemFile() {return true;};

  friend class GFileManager;
  friend class GPack;
};

#endif //!TRANSGAMING_USE_PHYSFS

#endif //_GMEMFILE_
