#ifndef TRANSGAMING_USE_PHYSFS

// #pragma once
#ifndef _GZIPPACK_
#define _GZIPPACK_


class ENGINE_API GZipPack: public GPack
{

  void* pZipFile;  
public:
  
  GZipPack();
  ~GZipPack();

  void Release();
  virtual bool OpenPack(LPCSTR fname,LPCSTR path);
  virtual GFile *GetFile(FIdx *pFidx);

};


#endif //_GZIPPACK_

#endif //!TRANSGAMING_USE_PHYSFS
