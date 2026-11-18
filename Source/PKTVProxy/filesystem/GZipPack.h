
#pragma once

class GZipPack: public GPack
{

  void* pZipFile;  
public:
  
  GZipPack();
  ~GZipPack();

  void Release();
  virtual bool OpenPack(LPCSTR fname,LPCSTR path);
  virtual GFile *GetFile(FIdx *pFidx);

};
