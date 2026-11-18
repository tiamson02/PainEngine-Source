// #pragma once
#ifndef _LOGBUFFER_
#define _LOGBUFFER_

#include "Engine.h"

#include <stdio.h>

class Vector;
class BoundingBox;
class Matrix;

#ifndef FINAL_BUILD
    //#define LOG_ON
#endif

typedef void LogCallbackFunc(const char *txt);

#ifdef LOG_ON 

#include "Core/HashMap.h"
#include "Core/LeString.h"

//#define FAST_LOGGING

#ifndef FAST_LOGGING

class ENGINE_API LogBuffer
{
    static const int MaxBufSize = 64*1024;
    LogCallbackFunc * lcFunc;

public:
#ifdef TRANSGAMING
    static int logDisabled;
#endif

    LogBuffer(const char* fileName=0);
    ~LogBuffer();

    void Open(const char* fileName);
	bool IsOpen( void ) { return ( File != 0 ); };
    void Close();
    void Print(const char* fmt, ...);
    void Print(const Matrix& m);
    void Print(const BoundingBox& b);
    void Print(const Vector& v);
    void Flush();
    void SetCallbackFunc(LogCallbackFunc *lcfunc) {lcFunc = lcfunc;};

private:
    FILE* 	File;
    int	    Size;
    int	    FlushMark;
    char	Buf[MaxBufSize];
};

#else

class ENGINE_API LogBuffer
{
	static const int BufSize = 64*1024;

	struct LoggingBuffer
	{
		char Buffer[BufSize];
		LoggingBuffer* Next;
	};

public:
#ifdef TRANSGAMING
    static int logDisabled;
#endif

    LogBuffer( const char* fileName = 0 );
	~LogBuffer();

	void Open( const char* fileName );
	void Close();
	void Print(const char* fmt, ...);
	void Print(const Matrix& m);
	void Print(const BoundingBox& b);
	void Print(const Vector& v);
	void Flush();
	void SetCallbackFunc(LogCallbackFunc *lcfunc) {lcFunc = lcfunc;};

private:
	FILE*				File;
	int					Size;
	LoggingBuffer*		Buffer;
	LoggingBuffer*		Last;
	LogCallbackFunc*	lcFunc;
};

#endif

#else

class ENGINE_API LogBuffer
{
public:
#ifdef TRANSGAMING
    static int logDisabled;
#endif

    LogBuffer(const char* fileName=0) {};
	~LogBuffer() {};

	void Open(const char* fileName) {};
	void Close() {};
	void Print(const char* fmt, ...) {};
	void Print(const Matrix& m) {};
	void Print(const BoundingBox& b) {};
	void Print(const Vector& v) {};
	void Flush() {};
    void SetCallbackFunc(LogCallbackFunc *lcfunc) {};
};

#endif

#endif	//_LOGBUFFER_
