
#pragma once

#include "Engine.h"

#include <stdio.h>

class Vector;
class BoundingBox;
class Matrix;

#ifndef FINAL_BUILD
    #define LOG_ON
#endif

typedef void LogCallbackFunc(const char *txt);

#ifdef LOG_ON 

#include "HashMap.h"
#include "LeString.h"

class CursConsole;

//#define FAST_LOGGING

#ifndef FAST_LOGGING

class  LogBuffer
{
    static const int MaxBufSize = 64*1024;
    LogCallbackFunc*	lcFunc;
	CursConsole*		mConsole;

public:
    LogBuffer(const char* fileName=0);
    ~LogBuffer();

    void Open(const char* fileName);
    void Close();
    void Print(const char* fmt, ...);
    void Print(const Matrix& m);
    void Print(const BoundingBox& b);
    void Print(const Vector& v);
    void Flush();
    void SetCallbackFunc(LogCallbackFunc *lcfunc) {lcFunc = lcfunc;};
	void SetConsole( CursConsole* console ) { mConsole = console; };

private:
    FILE* 	File;
    int	    Size;
    int	    FlushMark;
    char	Buf[MaxBufSize];
};

#else

class  LogBuffer
{
	static const int BufSize = 64*1024;

	struct LoggingBuffer
	{
		char Buffer[BufSize];
		LoggingBuffer* Next;
	};

public:
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
	void SetConsole( CursConsole* console ) { mConsole = console; };

private:
	FILE*				File;
	int					Size;
	LoggingBuffer*		Buffer;
	LoggingBuffer*		Last;
	LogCallbackFunc*	lcFunc;
	CursConsole*		mConsole;
};

#endif

#else

class  LogBuffer
{
public:
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
	void SetConsole( CursConsole* console ) {};
};

#endif

extern  LogBuffer* GLog;
#ifdef PKTV_CONNECTION_DEBUG
extern	LogBuffer* PKTVDLog;
#endif

