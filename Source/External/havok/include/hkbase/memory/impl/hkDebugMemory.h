/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// To use memory debugging, comment in hkdebugmemory in hkmemory.cpp
// and run your program in the debugger (fulldebug configuration).
// If there are memory leaks, their location and call stack 
// are printed to the debug window.

#ifndef HK_DEBUG_MEMORY_H
#define HK_DEBUG_MEMORY_H

#include <vector>
#include <map>

typedef void* hkAddress;
struct hkPointerInfo
{
	hkPointerInfo() : numbytes(unsigned(-1)), ischunk(false) { }
	std::vector<hkAddress> stacktrace;
	int numbytes;
	bool ischunk;
};

class hkDebugMemoryBase : public hkMemory
{
	public:

		hkDebugMemoryBase();

		virtual void* allocate(int nbytes, HK_MEMORY_CLASS cl);

		virtual void deallocate(void* ptr);

		virtual void* alignedAllocate(int alignment, int nbytes, HK_MEMORY_CLASS cl);

		virtual void alignedDeallocate(void* ptr);

		virtual void* allocateChunk(int nbytes, HK_MEMORY_CLASS cl);

		virtual void deallocateChunk(void*, int nbytes, HK_MEMORY_CLASS );

		virtual void printStatistics(hkOstream* c);

		virtual void getStatSynopsis(hkMemoryStatistics* in){}

	protected:

		virtual void dumpStackTrace( const std::vector<hkAddress>& trace ) = 0;

		virtual void outputString(const char* s) const = 0;

		void printMemoryReport();

		virtual void* internalAllocate(int nbytes, HK_MEMORY_CLASS, bool ischunk, int alignment=16) = 0;

		virtual void internalFree(void* p, int bytesToFree);

	protected:

		std::map<void*, hkPointerInfo> m_activePointers;

		enum { STACK_BUFFER_SIZE= 0x4000 };
		char HK_ALIGNED_VARIABLE(m_stackBuffer[STACK_BUFFER_SIZE+8], 16);

};

hkDebugMemoryBase::hkDebugMemoryBase()
{
	// set up a small (16k) stack by default. This class has been allocated
	// by global operator new which typically aligns only to 8 bytes.
	// Round up the current pointer to a multiple of 16.
	unsigned alignbuf = HK_NEXT_MULTIPLE_OF(16, reinterpret_cast<unsigned>(m_stackBuffer));
	setStackArea(reinterpret_cast<char*>( alignbuf ), STACK_BUFFER_SIZE);

}

void hkDebugMemoryBase::printMemoryReport()
{
	if( m_activePointers.size() != 0)
	{
		std::map<void*,hkPointerInfo>::iterator i = m_activePointers.begin();
		std::map<void*,hkPointerInfo>::iterator e = m_activePointers.end();

		outputString("**************************************************************\n");
		outputString("* BEGIN MEMORY LEAK REPORT                                   *\n");
		outputString("**************************************************************\n");
		
		for( ; i!=e; ++i)
		{
			hkPointerInfo& pinfo = (*i).second;
			char buf[256];
			hkString::snprintf(buf, 256, "%i bytes leaked. (chunk==%i)\n", pinfo.numbytes, pinfo.ischunk);
			outputString(buf);
			dumpStackTrace( pinfo.stacktrace );
		}

		outputString("**************************************************************\n");
		outputString("* END MEMORY LEAK REPORT                                     *\n");
		outputString("**************************************************************\n");
	}
	else
	{
		outputString("**************************************************************\n");
		outputString("* NO HAVOK MEMORY LEAKS FOUND                                *\n");
		outputString("**************************************************************\n");
	}
}

void hkDebugMemoryBase::printStatistics(hkOstream* c)
{
	if(c)
	{
		c->printf("No statistics available\n");
	}
}

void hkDebugMemoryBase::internalFree(void* p, int bytesToFree)
{
	std::map<void*,hkPointerInfo>::iterator i = m_activePointers.find(p);
	HK_ASSERT( i != m_activePointers.end() && "Freeing junk pointer");
	if(bytesToFree != -1)
	{
		const int bytesAllocated = (*i).second.numbytes;
		HK_ASSERT( bytesAllocated == bytesToFree && "Freeing chunk of wrong size");
	}
	else
	{
		HK_ASSERT( !(*i).second.ischunk && "Calling deallocate on a chunk");
	}
	m_activePointers.erase(i);
	hkSystemFree(p);
}

void* hkDebugMemoryBase::allocate(int nbytes, HK_MEMORY_CLASS c)
{
	return internalAllocate(nbytes, c, false);
}

void hkDebugMemoryBase::deallocate(void* p)
{
	if(p)
	{
		internalFree(p, -1);
	}
}

void* hkDebugMemoryBase::allocateChunk(int nbytes, HK_MEMORY_CLASS c)
{
	return internalAllocate(nbytes, c, true);
}

void hkDebugMemoryBase::deallocateChunk(void* p, int nbytes, HK_MEMORY_CLASS cl)
{
	if(p)
	{
		internalFree(p, nbytes);
	}
}

void* hkDebugMemoryBase::alignedAllocate(int alignment, int nbytes, HK_MEMORY_CLASS cl)
{
	return internalAllocate(nbytes, cl, false, alignment);
}

void hkDebugMemoryBase::alignedDeallocate(void* p)
{
	if(p)
	{
		internalFree(p, -1);
	}
}


#if defined(HK_PLATFORM_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <imagehlp.h>

// ignore the first two frames since they are always in this file
static const unsigned IGNORE_FIRST_N_FRAMES = 2;

typedef BOOL  (__stdcall *tSymInitialize)( IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess );
typedef DWORD (__stdcall *tSymGetOptions)( VOID );
typedef DWORD (__stdcall *tSymSetOptions)( IN DWORD SymOptions );
typedef BOOL  (__stdcall *tSymGetSymFromAddr)( IN HANDLE hProcess, IN DWORD dwAddr, OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_SYMBOL Symbol );
typedef BOOL  (__stdcall *tStackWalk)( DWORD MachineType, HANDLE hProcess, HANDLE hThread, LPSTACKFRAME StackFrame, PVOID ContextRecord, PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine, PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine, PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine, PTRANSLATE_ADDRESS_ROUTINE TranslateAddress );
typedef PVOID (__stdcall *tSymFunctionTableAccess)( HANDLE hProcess, DWORD AddrBase );
typedef DWORD (__stdcall *tSymGetModuleBase)( IN HANDLE hProcess, IN DWORD dwAddr );
typedef BOOL  (__stdcall *tSymGetLineFromAddr)( IN HANDLE hProcess, IN DWORD dwAddr, OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE Line );

class hkDebugMemoryWin32 : public hkDebugMemoryBase
{
	public:

		hkDebugMemoryWin32();
		~hkDebugMemoryWin32();

	protected:
		
		void dumpStackTrace( const std::vector<hkAddress>& trace );

		void* internalAllocate(int nbytes, HK_MEMORY_CLASS, bool ischunk, int alignment);

		void outputString(const char* s) const;

	protected:

		HINSTANCE hImagehlpDll;

		tSymInitialize pSymInitialize;
		tSymGetOptions pSymGetOptions;
		tSymSetOptions pSymSetOptions;
		tSymGetSymFromAddr pSymGetSymFromAddr;
		tStackWalk pStackWalk;
		tSymFunctionTableAccess pSymFunctionTableAccess;
		tSymGetModuleBase pSymGetModuleBase;
		tSymGetLineFromAddr pSymGetLineFromAddr;
};
typedef hkDebugMemoryWin32 hkDebugMemory;

#define LOAD_FUNCTION(A) if(1) { p##A = (t##A) GetProcAddress(hImagehlpDll, #A); HK_ASSERT(p##A); } else

hkDebugMemoryWin32::hkDebugMemoryWin32()
{
	hImagehlpDll = LoadLibrary("imagehlp.dll");
	if ( hImagehlpDll )
	{
		LOAD_FUNCTION(SymInitialize);
		LOAD_FUNCTION(SymGetOptions);
		LOAD_FUNCTION(SymSetOptions);
		LOAD_FUNCTION(SymGetSymFromAddr);
		LOAD_FUNCTION(StackWalk);
		LOAD_FUNCTION(SymFunctionTableAccess);
		LOAD_FUNCTION(SymGetModuleBase);
		LOAD_FUNCTION(SymGetLineFromAddr);

		BOOL initsymbols = pSymInitialize( GetCurrentProcess(), HK_NULL, TRUE);
		HK_ASSERT(initsymbols);

		DWORD symOptions = pSymGetOptions();
		symOptions |= SYMOPT_LOAD_LINES;
		pSymSetOptions( symOptions );
	}
}

hkDebugMemoryWin32::~hkDebugMemoryWin32()
{
	printMemoryReport();
	pSymInitialize = 0;
	pSymGetOptions = 0;
	pSymSetOptions = 0;
	pSymGetSymFromAddr = 0;
	pStackWalk = 0;
	pSymFunctionTableAccess = 0;
	pSymGetModuleBase = 0;
	pSymGetLineFromAddr = 0;
	FreeLibrary( hImagehlpDll );
	hImagehlpDll = 0;
}

void hkDebugMemoryWin32::dumpStackTrace( const std::vector<hkAddress>& trace )
{
	typedef std::vector<hkAddress>::const_iterator iter;

	char buf[2048];
	HANDLE curproc   = GetCurrentProcess();

	for( iter i=trace.begin(); i!=trace.end(); ++i)
	{
		hkAddress symaddress = *i;

		IMAGEHLP_SYMBOL& sym = *(IMAGEHLP_SYMBOL*)buf;
		memset(&sym, 0, sizeof(IMAGEHLP_SYMBOL));
		sym.SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
		sym.MaxNameLength = 2048 - sym.SizeOfStruct;

		DWORD displacement = 0;
		BOOL ok_symbol = pSymGetSymFromAddr( 
			curproc, 
			(unsigned long)symaddress,
			&displacement,
			&sym);

		if(! ok_symbol)
		{
			::strcpy(sym.Name, "(unknown)");
		}
		else if (::strcmp(sym.Name, "WinMain") == 0 )
		{
			break;
		}

		IMAGEHLP_LINE line;
		::memset(&line, 0, sizeof(line));
		line.SizeOfStruct = sizeof(line);

		BOOL ok_getline = pSymGetLineFromAddr(
			curproc,
			(unsigned long)symaddress,
			&displacement,
			&line);

		char trace[2048];
		_snprintf(trace, 2048, "%s:%i:'%s'\n", line.FileName, line.LineNumber, sym.Name);

		outputString( trace );
	}
	outputString("-------------------------------------------------------------------\n\n");
}

void* hkDebugMemoryWin32::internalAllocate(int nbytes, HK_MEMORY_CLASS, bool ischunk, int alignment)
{
	if(nbytes!=0)
	{
		HANDLE curproc   = GetCurrentProcess();
		HANDLE curthread = GetCurrentThread();

		CONTEXT context;
		::memset(&context, 0, sizeof(context));
		context.ContextFlags = CONTEXT_FULL;
		BOOL ok_context = GetThreadContext(curthread, &context);
		HK_ASSERT(ok_context);

		STACKFRAME sf;
		::memset(&sf, 0, sizeof(sf));
		sf.AddrPC.Offset       = context.Eip;
		sf.AddrPC.Mode         = AddrModeFlat;
		sf.AddrFrame.Offset    = context.Ebp;
		sf.AddrFrame.Mode      = AddrModeFlat;

		std::vector<hkAddress> stacktrace;
		unsigned num_frames = 0;

		while(	pStackWalk(
					IMAGE_FILE_MACHINE_I386,
					curproc, 
					curthread,
					&sf,
					&context,
					HK_NULL,
					pSymFunctionTableAccess,
					pSymGetModuleBase,
					HK_NULL) )
		{
			if(num_frames++ >= IGNORE_FIRST_N_FRAMES)
			{
				stacktrace.push_back( (hkAddress)sf.AddrPC.Offset );
			}
		}

		hkPointerInfo pointerinfo;
		pointerinfo.numbytes = nbytes;
		pointerinfo.ischunk = ischunk;
		pointerinfo.stacktrace.swap( stacktrace );
		
		void* p = hkSystemMalloc(nbytes, alignment);
		m_activePointers[p] = pointerinfo;
		return p;
	}
	else
	{
		return HK_NULL;
	}
}

void hkDebugMemoryWin32::outputString(const char* s) const
{
	OutputDebugString(s);
}

#elif defined(HK_PLATFORM_UNIX) || defined(HK_PLATFORM_PS2)

#	if defined(HK_PLATFORM_UNIX)
#		include <execinfo.h>
#	else // defined(HK_PLATFORM_PS2)
		extern "C" int backtrace(void** addr, int maxaddr);
		extern "C" char** backtrace_symbols(void* const* addr, int numaddr);
#	endif

class hkDebugMemoryUnixish : public hkDebugMemoryBase
{
	public:

		~hkDebugMemoryUnixish()
		{
			printMemoryReport();
		}

	protected:

		virtual void outputString(const char* s) const
		{
			fputs(s, stderr);
		}

		void dumpStackTrace( const std::vector<hkAddress>& trace )
		{
			const unsigned IGNORE_FRAMES = 2;

			char** strings = backtrace_symbols(&trace[0], trace.size());
			if(strings)
			{
				for( int i = IGNORE_FRAMES; i < (int)trace.size(); ++i )
				{
					const char* loc = hkString::strStr(strings[i], "(");
					outputString(loc ? loc : strings[i]);
					outputString("\n");
				}
				free(strings);
			}
			else
			{
				char buf[100];
				for( int i = IGNORE_FRAMES; i < (int)trace.size(); ++i )
				{
					hkString::snprintf(buf, 100, "%.8x (no symbols)\n", trace[i]);
					outputString(buf);
				}
			}
		}

		virtual void* internalAllocate(int nbytes, HK_MEMORY_CLASS, bool ischunk, int alignment=16)
		{
			if(nbytes!=0)
			{
				const int MAXSTACK = 100;
				std::vector<hkAddress> stacktrace;
				stacktrace.resize(100);

				int nvals = backtrace(&stacktrace[0], MAXSTACK);
				stacktrace.resize(nvals);

				hkPointerInfo pointerinfo;
				pointerinfo.numbytes = nbytes;
				pointerinfo.ischunk = ischunk;
				pointerinfo.stacktrace.swap( stacktrace );

				void* p = hkSystemMalloc(nbytes, alignment);
				m_activePointers[p] = pointerinfo;
				return p;
			}
			else
			{
				return HK_NULL;
			}
		}
};

typedef hkDebugMemoryUnixish hkDebugMemory;

#else
#error No hkDebugMemory implementation for this platform.
#endif

#endif // HK_DEBUG_MEMORY_H


/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
