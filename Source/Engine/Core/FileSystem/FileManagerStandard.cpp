#error This file is obselete
#include "Engine.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Core/LeString.h"
#include "Core/FileSystem/FileManagerStandard.h"

#include <msvcrt/direct.h>
#include <msvcrt/io.h>

#define chdir	_chdir
#define getcwd	_getcwd

#if defined( TRANSGAMING_PSP )
# include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
# include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
# include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

#endif

class FileObjectStd : public FileObject
{
public:
	FileObjectStd(FILE* file)
	:	FileObject()
	,	mFile(file)
	{}

	~FileObjectStd()
	{
		Close();
	}

	void Close()
	{
#ifdef TRANSGAMING
		TNG_fclose(mFile);
#else
		fclose(mFile);
#endif
	}

	bool Seek( int pos,int origin = SEEK_SET )
	{
#ifdef TRANSGAMING
		return TNG_fseek(mFile,pos,origin)==0;
#else
		return fseek(mFile,pos,origin)==0;
#endif
	}

	long32 Tell()
	{
#ifdef TRANSGAMING
		return (long32) TNG_ftell(mFile);
#else
		return (long32) ftell(mFile);
#endif
	}

protected:
	FILE*	mFile;
};

class FileReaderStd : public FileObjectStd
{
public:
	FileReaderStd(FILE* file, int size) 
	:	FileObjectStd(file) 
	,	mSize(size)
	{
		Seek(0);
	}

//joe
//	bool Write(const void* /*buf*/, int /*size*/) { return false; }
	int Write(const void* /*buf*/, int /*size*/) { return false; }
//	bool Read (void* buf, int size)
	int Read (void* buf, int size)
	{
#ifdef TRANSGAMING
		return TNG_fread(buf,size,1,mFile)==1;
#else
		return fread(buf,size,1,mFile)==1;
#endif
	}

	int GetSize() const { return mSize; }

protected:
	int	mSize;
};

class FileWriterStd : public FileObjectStd
{
public:
	FileWriterStd(FILE* file) 
	:	FileObjectStd(file) 
	{}

//joe
//	bool Read (void* /*buf*/, int /*size*/) { return false; }
	int Read (void* /*buf*/, int /*size*/) { return false; }
//	bool Write(const void* buf, int size)
	int Write(const void* buf, int size)
	{
#ifdef TRANSGAMING
		return TNG_fwrite(buf,size,1,mFile)==1;
#else
		return fwrite(buf,size,1,mFile)==1;
#endif
	}

	int GetSize() const { return -1; }
};

FileObject* FileManagerStd::CreateFileReader(const char* fileName)
{
	if( strlen( fileName ) == 0 )
		return NULL;
#ifdef TRANSGAMING
	FILE* f=TNG_fopen(fileName,"rb");
#else
	FILE* f=fopen(fileName,"rb");
#endif
	if(f) 
    {
#ifdef TRANSGAMING
		TNG_fseek(f,0,SEEK_END);
        m_setTag("File");
		return new FileReaderStd(f,TNG_ftell(f));
#else
		fseek(f,0,SEEK_END);
		return new FileReaderStd(f,ftell(f));
#endif
	}
	return 0;
}

FileObject* FileManagerStd::CreateFileWriter(const char* fileName, bool append )
{
	FILE* f;
#ifdef TRANSGAMING
	if( append )
		f = TNG_fopen(fileName,"ab");
	else
		f = TNG_fopen(fileName,"wb");
#else
	if( append )
		f = fopen(fileName,"ab");
	else
		f = fopen(fileName,"wb");
#endif
    m_setTag("File");
	return f ? new FileWriterStd(f) : 0;
}

bool FileManagerStd::SetWorkingDirectory(const char* dirName)
{
	return chdir(dirName)==0;
}

String FileManagerStd::GetWorkingDirectory()
{
	char buf[1024]="";
	getcwd(buf,sizeof(buf));
	return String(buf);
}

void FileManagerStd::FindFiles(const char* dirName, DynamicArray<String>& list, DWORD mode)
{
	_finddata_t finfo;
	long32 h = _findfirst(dirName,&finfo);
	if(h != -1) 
    {
		do 
        {				
			if(strcmp(finfo.name,".") && strcmp(finfo.name,"..")) 
            {
				if(finfo.attrib & _A_SUBDIR) 
				{
					if (!(mode&EFindDirs)) continue;
				}
				else
				{
					if (!(mode&EFindFiles)) continue;
				}					
                list.Add(String(finfo.name));
			}
		}
		while(_findnext(h,&finfo) != -1);
		_findclose(h);
	}
/*
	WIN32_FIND_DATA findData;

	String realDirName = dirName;

	HANDLE fileHandle = FindFirstFile( realDirName.AsChar(), &findData );
	if( fileHandle == INVALID_HANDLE_VALUE ) return;	// no files in this directory

	do
	{
		if( strcmp( findData.cFileName, ".") == 0 )
			continue;
		if( strcmp( findData.cFileName, "..") == 0 )
			continue;
		if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if(!(mode&EFindDirs)) continue;
		}
		else
			if(!(mode&EFindFiles)) continue;
		list.Add(String(findData.cFileName));
	}
	while( FindNextFile( fileHandle, &findData ) );

	FindClose( fileHandle );
*/
}

bool FileManagerStd::CreateDirectory( const char* dirName )
{
/*
	String workDir = GetWorkingDirectory();

	DynamicArray<String> workDirTokens;
	DynamicArray<String> dirTokens;

	String delims = "/\\";
	String::Tokenize( workDir.AsChar(), delims.AsChar(), workDirTokens );
	String::Tokenize( dirName, delims.AsChar(), dirTokens );

	int revert = 0;
	if( dirTokens[0] == "./" )
		dirTokens.Remove(0);

	for( int i = 0; i < dirTokens.Size()-1; ++i )
	{
		if( dirTokens[0] == ".." )
		{
			dirTokens.Remove(0);
			revert++;
		}
		else if( dirTokens[i+1] == ".." )
		{
			dirTokens.Remove(i);
			dirTokens.Remove(i);
			i--;
		}
	}

	if( dirTokens.Size() == 0 )
		return false;

	for( int i = 0; i < revert; ++i )
		workDirTokens.RemoveLast();

	if( workDirTokens.Size() == 0 )
		return false;

	String resultName = "";

	for( int i = 0; i < workDirTokens.Size(); ++i )
	{
		resultName += workDirTokens[i];
		resultName += "\\";
	}

	for( int i = 0; i < dirTokens.Size()-1; ++i )
	{
		resultName += dirTokens[i];
		resultName += "\\";
	}
	resultName += dirTokens[i];

	return( ::CreateDirectory( resultName.AsChar() ) );
*/
	return( ::CreateDirectory( dirName, NULL ) == TRUE );
}

String FileManagerStd::FileTime( const char* fileName )
{
	static char strTime[15];
	FILETIME ftSrc;
	SYSTEMTIME timeSrc;
	OFSTRUCT of;

	strTime[0] = NULL;

	HFILE hFile = OpenFile( fileName, &of, OF_READ|OF_SHARE_DENY_NONE );
	if( hFile != HFILE_ERROR )
	{
		GetFileTime( (HANDLE)hFile, NULL, NULL, &ftSrc );
		CloseHandle( (HANDLE)hFile );

		FILETIME fTime;
		FileTimeToLocalFileTime(&ftSrc,&fTime);
		FileTimeToSystemTime(&fTime,&timeSrc);

		sprintf(strTime,
			"%04d%02d%02d%02d%02d%02d",
			timeSrc.wYear,
			timeSrc.wMonth,
			timeSrc.wDay,
			timeSrc.wHour,
			timeSrc.wMinute,
			timeSrc.wSecond
			);
	}

	return String(strTime);
}

void FileManagerStd::RemoveFilesInDirectory( const char* dirName )
{
	String filePath;
	DynamicArray<String> fileNames;

	char buffer[1024];
	int dirNameLen = strlen(dirName);
	strcpy( buffer, dirName );
	if( dirNameLen && ( ( buffer[dirNameLen-1] == '\\' ) || ( buffer[dirNameLen-1] == '/' ) ) )
		buffer[dirNameLen-1] = 0;

	String realDirName = buffer;
	realDirName += "\\*";

	FindFiles( realDirName.AsChar(), fileNames );

	for( int i = 0; i < fileNames.Size(); ++i )
	{
		filePath = dirName;
		filePath += '\\';
		filePath += fileNames[i];
		if( !DeleteFile( filePath.AsChar() ) )
			GLog.Print( "Warning: File '%s' could not be deleted!\n", filePath );
	}
}
