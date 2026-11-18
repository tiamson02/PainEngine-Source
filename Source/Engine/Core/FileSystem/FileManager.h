
// #pragma once
#ifndef _FILEMANAGER_
#define _FILEMANAGER_


#include "FileSystem/GFileManager.h"
#define FileManagerStd GFileManager

/*
class String;
template <class T> class DynamicArray;


class ENGINE_API FileObject
{
public:	
    virtual ~FileObject() {}

	virtual void Close() = 0;
	virtual bool Read (void* buf, int size) = 0;
	virtual bool Write(const void* buf, int size) = 0;
	virtual bool Seek(int pos,int origin = SEEK_SET) = 0;
	virtual int  Tell() = 0;
	virtual int  GetSize() const = 0;

	bool PutToken( unsigned short token ) { return Write(&token,sizeof(unsigned short)); }
	bool TestToken( unsigned short token ) { unsigned short t; return Read(&t,sizeof(unsigned short))?(t==token):false; }
};

enum
{
    EFindFiles    = (1<<1),
    EFindDirs     = (1<<2)
};

class ENGINE_API FileSizeObject : public FileObject
{
public:
	FileSizeObject() : Size(0) {}

    // FileObject interface
    void Close() {}
	bool Read(void* , int size)
	{
		Size+=size;
		return true;
	}

	bool Write(const void* , int size)
	{
		Size+=size;
		return true;
	}
    bool Seek(int pos, int origin = SEEK_SET ) { return true; }
    int  Tell() { return 0; }
    int  GetSize() const { return Size; }

    // other
	void Reset() { Size=0; }

private:
    int Size;
};

class ENGINE_API FileManager
{
public:
	virtual FileObject* CreateFileReader(const char* fileName) = 0;
	virtual FileObject* CreateFileWriter(const char* fileName, bool append = false ) = 0;
	virtual int  FileSize(const char* fileName) = 0;
	virtual bool FileExist(const char* fileName) = 0;
	virtual void FindFiles(const char* dirName, DynamicArray<String>& list, DWORD mode=EFindFiles)=0;
	virtual bool SetWorkingDirectory(const char* dirName) = 0;
	virtual String GetWorkingDirectory()=0;
	virtual bool CreateDirectory( const char* dirName ) = 0;
};

// =================================================================
// basic serialization support
// =================================================================

inline FileObject& operator<<(FileObject& file, const bool  value) { file.Write(&value,sizeof(bool));  return file; }
inline FileObject& operator<<(FileObject& file, const char  value) { file.Write(&value,sizeof(char));  return file; }
inline FileObject& operator<<(FileObject& file, const BYTE  value) { file.Write(&value,sizeof(BYTE));  return file; }
inline FileObject& operator<<(FileObject& file, const WORD  value) { file.Write(&value,sizeof(WORD));  return file; }
inline FileObject& operator<<(FileObject& file, const int   value) { file.Write(&value,sizeof(int));   return file; }
inline FileObject& operator<<(FileObject& file, const unsigned int   value) { file.Write(&value,sizeof(unsigned int));   return file; }
inline FileObject& operator<<(FileObject& file, const DWORD value) { file.Write(&value,sizeof(DWORD)); return file; }
inline FileObject& operator<<(FileObject& file, const FLOAT value) { file.Write(&value,sizeof(FLOAT)); return file; }


inline FileObject& operator>>(FileObject& file, bool&  value) { file.Read(&value,sizeof(bool));  return file; }
inline FileObject& operator>>(FileObject& file, char&  value) { file.Read(&value,sizeof(char));  return file; }
inline FileObject& operator>>(FileObject& file, BYTE&  value) { file.Read(&value,sizeof(BYTE));  return file; }
inline FileObject& operator>>(FileObject& file, WORD&  value) { file.Read(&value,sizeof(WORD));  return file; }
inline FileObject& operator>>(FileObject& file, int&   value) { file.Read(&value,sizeof(int));   return file; }
inline FileObject& operator>>(FileObject& file, unsigned int&   value) { file.Read(&value,sizeof(unsigned int));   return file; }
inline FileObject& operator>>(FileObject& file, DWORD& value) { file.Read(&value,sizeof(DWORD)); return file; }
inline FileObject& operator>>(FileObject& file, FLOAT& value) { file.Read(&value,sizeof(FLOAT)); return file; }
*/

#endif //_FILEMANAGER_

