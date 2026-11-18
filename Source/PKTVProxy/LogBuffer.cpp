#include "Engine.h"
#include "LeMath.h"
#include "LogBuffer.h"
#include "Console.h"
#include <stdarg.h>

#if defined( TRANSGAMING_PSP )
# include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
# include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
# include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

#ifdef LOG_ON 

#ifndef FAST_LOGGING

LogBuffer::LogBuffer(const char* fileName)
:	File(0), Size(0), FlushMark(0), lcFunc(0)
{
	Open(fileName);
}

LogBuffer::~LogBuffer()
{
	Close();
}

void LogBuffer::Open(const char* fileName)
{
	if(fileName)
#ifdef TRANSGAMING
		File=TNG_fopen(fileName,"wb");
#else
		File=fopen(fileName,"wb");
#endif
}

void LogBuffer::Close()
{
	if(File)
	{
		if(Size > FlushMark)
#ifdef TRANSGAMING
			TNG_fwrite(Buf+FlushMark,Size-FlushMark,1,File);
		TNG_fclose(File);
#else
			fwrite(Buf+FlushMark,Size-FlushMark,1,File);
		fclose(File);
#endif
		File=0;
	}
	Size=FlushMark=0;
}

void LogBuffer::Flush()
{
	if(File && Size > FlushMark)
	{
#ifdef TRANSGAMING
		TNG_fwrite(Buf+FlushMark,Size-FlushMark,1,File);
		FlushMark=Size;
		TNG_fflush(File);
#else
		fwrite(Buf+FlushMark,Size-FlushMark,1,File);
		FlushMark=Size;
		fflush(File);
#endif
	}
}

void LogBuffer::Print(const char* fmt, ...)
{
	char buf[4096];

	va_list argPtr;
	va_start(argPtr, fmt);
	int charsWritten=vsprintf(buf,fmt,argPtr);
	va_end(argPtr);

	if(charsWritten>0)
	{
		if(Size+charsWritten > MaxBufSize)
		{
			Flush();
			Size=FlushMark=0;
		}

		memcpy(Buf+Size,buf,charsWritten);
		Size+=charsWritten;
		
		if (File)
		{
			#ifndef LOGS_FLUSH
			if (Size-FlushMark>512)
			#endif
			{
#ifdef TRANSGAMING
				TNG_fwrite(Buf+FlushMark,Size-FlushMark,1,File);
#else
				fwrite(Buf+FlushMark,Size-FlushMark,1,File);
#endif
				FlushMark=Size;
			}

			#ifdef LOGS_FLUSH
#ifdef TRANSGAMING
			TNG_fflush(File);
#else
			fflush(File);
#endif
			Size=FlushMark=0;
			#endif
		}
		


#ifdef _DEBUG
		OutputDebugString(buf);
#endif // _DEBUG


        if (lcFunc)
            lcFunc(buf);

		if( mConsole )
			mConsole->AddToBuffer( buf );
	}
}

#else

LogBuffer::LogBuffer( const char* fileName ) : File(0), Size(0), lcFunc(0)
{
	Buffer = Last = new LoggingBuffer;
	Buffer->Next = 0;
	Open( fileName );
}

LogBuffer::~LogBuffer()
{
	Close();
}

void LogBuffer::Open( const char* fileName )
{
	if( fileName )
#ifdef TRANSGAMING
		File = TNG_fopen( fileName, "wb" );
#else
		File = fopen( fileName, "wb" );
#endif
}

void LogBuffer::Close()
{
	if( File )
	{
		while( Buffer )
		{
			if( Buffer == Last )
			{
				if( Size )
#ifdef TRANSGAMING
					TNG_fwrite( Buffer->Buffer, Size, 1, File );
#else
					fwrite( Buffer->Buffer, Size, 1, File );
#endif
			}
			else
#ifdef TRANSGAMING
				TNG_fwrite( Buffer->Buffer, BufSize, 1, File );
#else
				fwrite( Buffer->Buffer, BufSize, 1, File );
#endif

			LoggingBuffer* next = Buffer->Next;
			delete Buffer;
			Buffer = next;
		}

#ifdef TRANSGAMING
		TNG_fclose( File );
#else
		fclose( File );
#endif
		File = 0;
	}
	else
	{
		while( Buffer )
		{
			LoggingBuffer* next = Buffer->Next;
			delete Buffer;
			Buffer = next;
		}
	}
	Size = 0;
	Buffer = 0;
	Last = 0;
}

void LogBuffer::Flush()
{
/*
	if( File )
	{
		while( Buffer )
		{
			if( Buffer == Last )
			{
				if( Size )
#ifdef TRANSGAMING
                    TNG_fwrite( Buffer->Buffer, Size, 1, File );
#else
					fwrite( Buffer->Buffer, Size, 1, File );
#endif
			}
			else
#ifdef TRANSGAMING
				TNG_fwrite( Buffer->Buffer, BufSize, 1, File );
#else
				fwrite( Buffer->Buffer, BufSize, 1, File );
#endif

			LoggingBuffer* next = Buffer->Next;
			delete Buffer;
			Buffer = next;
		}

		Last = Buffer = new LoggingBuffer;
		Buffer->Next = 0;
		Size = 0;
#ifdef TRANSGAMING
		TNG_fflush( File );
#else
		fflush( File );
#endif
	}
*/
}

void LogBuffer::Print(const char* fmt, ...)
{
	char buf[4096];

	va_list argPtr;
	va_start( argPtr, fmt );
	int charsWritten = vsprintf( buf, fmt, argPtr );
	va_end( argPtr );

	if( charsWritten > 0 )
	{
		if( Size + charsWritten >= BufSize )
		{
			int firstPart = BufSize-Size;
			::memcpy( Last->Buffer+Size, buf, firstPart );
			Last->Next = new LoggingBuffer;
			Last = Last->Next;
			Last->Next = 0;
			if( charsWritten > firstPart )
				::memcpy( Last->Buffer, buf+firstPart, charsWritten-firstPart );
			Size = charsWritten-firstPart;
		}
		else
		{
			::memcpy( Last->Buffer+Size, buf, charsWritten );
			Size += charsWritten;
		}
		
//#ifdef _DEBUG
//		OutputDebugString(buf);
//#endif

		if( lcFunc )
			lcFunc( buf );

		if( mConsole )
			mConsole->AddToBuffer( buf );
	}
}

#endif

void LogBuffer::Print(const Matrix& m)
{
	Print("%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n",
			m.mArray[ 0],m.mArray[ 1],m.mArray[ 2],m.mArray[ 3],
			m.mArray[ 4],m.mArray[ 5],m.mArray[ 6],m.mArray[ 7],
			m.mArray[ 8],m.mArray[ 9],m.mArray[10],m.mArray[11],
			m.mArray[12],m.mArray[13],m.mArray[14],m.mArray[15]);
}

void LogBuffer::Print(const BoundingBox& box)
{
	if(box.Valid())
		Print("(%.2f,%.2f,%.2f) (%.2f,%.2f,%.2f)",
		   		box.Min.X,box.Min.Y,box.Min.Z,
				box.Max.X,box.Max.Y,box.Max.Z);
    else
		Print("(Box Invalid)");
}

void LogBuffer::Print(const Vector& v)
{
	Print("(%.2f,%.2f,%.2f)",v.X,v.Y,v.Z);
}

#endif