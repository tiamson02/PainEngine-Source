
#pragma once

class GMemFile: public GFile
{
protected:
  char wmode[3];
  bool mfree;
  BYTE *mptr;
  long mpos;
  long size;
  long mapOffset;
  void *pMapMem;

public:
  GMemFile();
  virtual ~GMemFile();

  virtual bool Open(void *ptr,long size,bool mf = false);
  virtual bool Open(LPCSTR fname,LPCSTR mode = "rb");
  virtual void Close();    
  virtual void *GetPtr() {return mptr;};
  virtual void *GetCurPtr() 
  {
    if (!mptr) return NULL;
    return &mptr[mpos];
  };
  
  virtual long GetSize() {return size;};
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
