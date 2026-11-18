
// #pragma once
#ifndef _FILEMANAGERSTANDARD_
#define _FILEMANAGERSTANDARD_


#include "Core/FileSystem/FileManager.h"

/*
class ENGINE_API FileManagerStd : public FileManager
{
public:
	FileObject* CreateFileReader(const char* fileName);
	FileObject* CreateFileWriter(const char* fileName, bool append = false );

	int FileSize(const char* fileName)
	{
		int size=-1;
		FileObject* f=CreateFileReader(fileName);
		if(f) 
        {
			size=f->GetSize();
			delete f;
		}
		
		return size;
	}

	String FileTime( const char* fileName );
	void RemoveFilesInDirectory( const char* dirName );

	bool FileExist(const char* fileName)
	{
		FileObject* f=CreateFileReader(fileName);
		if(f) 
		{
			delete f;
			return true;
		}
		return false;
	}

	bool    SetWorkingDirectory(const char* dirName);
	String  GetWorkingDirectory();
	void FindFiles(const char* dirName, DynamicArray<String>& list, DWORD mode=EFindFiles);
	bool CreateDirectory( const char* dirName );
};
*/

#endif //_FILEMANAGERSTANDARD_

