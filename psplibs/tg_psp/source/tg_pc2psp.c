//#include <eekernel.h>
//#include <libhig.h>
//#include "../crosslibs/include/mmsystem.h"
#ifndef TRANSGAMING_PSP
#define TRANSGAMING_PSP
#endif
#include <kernel.h>
#include <sysmem.h>
#include <stdio.h>

#include <windows.h>
#include <winnt.h>
#include <reent.h>

#include "include/tg_pc2psp.h"


//for the system time
//the oringinal code is 
//	{
//		unsigned long* pointer = (unsigned long *)&start;
//		__asm
//		{
//			mov		ecx, pointer
//			rdtsc
//			mov		[ecx],     eax
//			mov		[ecx+4],   edx
//		}
//	}
//

//Engine/Core/SystemDriver.cpp line 233
unsigned long PSP_GetTimerSystemTime(void)
{
	//return GetTimerSystemTime();
    return (unsigned long)(sceKernelGetSystemTimeWide());
}

float PSP_GetCurrentTimeInSeconds(){
    return ((unsigned long)sceKernelGetSystemTimeWide()) * 0.000001f;
}

float PSP_GetTimeInSeconds(unsigned long t1, unsigned long t2){
    return ((t2 - t1) * 0.000001f);
}

//DWORD timeGetTime(void)
//{
//	//DWORD WINAPI	timeGetTime(void);
//	return PSP_GetTimerSystemTime();
//}


//for aligned memory
//Engine/core/mmgr.cpp
void*	PSP_aligned_malloc(size_t size, size_t alignment )
{
	//return sceHiMemAlign( alignment, size);
	return NULL;
}

void*	PSP_aligned_realloc(void *memblock, size_t size, size_t alignment )
{
//no alignment right now	
	if(((int) memblock) % alignment != 0)
		return NULL;
//	Assert (((int) memblock) % alignment == 0);
	//return sceHiMemRealloc(memblock, size);
	return NULL;
}

void	PSP_aligned_free(void *memblock)
{
	//sceHiMemFree(memblock);
}

#ifdef TRANSGAMING_SHOW_STUB_WARNINGS
# define TRACE printf
#else
# define TRACE(args...)
#endif

//#define TRACE (void)0

// ------------------------- WINDOWS STUBS ---------------------------------

INT WINAPI MessageBoxA(HWND hWnd, LPCSTR text, LPCSTR title, UINT type)
{
	// TODO - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning -1 (text = '%s', title = '%s')\n", text, title);
    
	return -1;
}

// drive.c
UINT WINAPI GetLogicalDriveStringsA( UINT len, LPSTR buffer )
{
	// TODO - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning -1 (len = %d)\n", len);

	return -1;
}

UINT WINAPI GetDriveTypeA(LPCSTR root)
{
	// TODO - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning -1 (root = '%s')\n", root);

	return -1;
}

BOOL WINAPI GetVolumeInformationA( LPCSTR root, LPSTR label,
                                       DWORD label_len, DWORD *serial,
                                       DWORD *filename_len, DWORD *flags,
                                       LPSTR fsname, DWORD fsname_len )

{
	// TODO - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

SHORT WINAPI GetAsyncKeyState(INT nKey)
{
	// TODO - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

HKL WINAPI GetKeyboardLayout(DWORD dwLayout)
{
	// TODO - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

// registry.c
DWORD WINAPI RegCreateKeyA( HKEY hkey, LPCSTR name, LPHKEY retkey )
{
	// TODO - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning -1\n");

	return -1;
}


// registry.c
DWORD WINAPI RegCloseKey( HKEY hkey )
{
	// TODO - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning -1\n");

	return -1;
}

// input.c
UINT WINAPI MapVirtualKeyExA(UINT code, UINT maptype, HKL hkl)
{
	// TODO - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning -1\n");

	return -1;
}

// input.c
INT WINAPI ToAsciiEx( UINT virtKey, UINT scanCode, LPBYTE lpKeyState,
                      LPWORD lpChar, UINT flags, HKL dwhkl )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning -1\n");

	return -1;
}

// directory.c
BOOL WINAPI CreateDirectoryA( LPCSTR path,
                                  LPSECURITY_ATTRIBUTES lpsecattribs )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE (path = '%s')\n", path);

	return FALSE;
}

// drive.c
UINT WINAPI GetCurrentDirectoryA( UINT buflen, LPSTR buf )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

// drive.c
BOOL WINAPI SetCurrentDirectoryA( LPCSTR dir )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

// file.c
HFILE WINAPI OpenFile( LPCSTR name, OFSTRUCT *ofs, UINT mode )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL (filename = '%s')\n", name);

	return NULL;	
}

// file.c
BOOL WINAPI DeleteFileA( LPCSTR path )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE (path = '%s')\n", path);

	return FALSE;
}

// file.c
BOOL WINAPI GetFileTime( HANDLE hFile, FILETIME *lpCreationTime,
                           FILETIME *lpLastAccessTime,
                           FILETIME *lpLastWriteTime )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;	
}

// file.c
BOOL WINAPI RemoveDirectoryA( LPCSTR path )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE (path = '%s')\n", path);

	return FALSE;	
}

// handle.c
BOOL WINAPI CloseHandle( HANDLE handle )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}


//*********************************************** \/\/ FindFirstFile() API \/\/ *************************************//

// files/dos_fs.c
HANDLE WINAPI FindFirstFileA(
        LPCSTR lpFileName,
        WIN32_FIND_DATAA *lpFindData )
{
	// FIXME - Implement this!!!!!!!!!!
	TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning INVALID_HANDLE_VALUE (filename = '%s')\n", lpFileName);
    

	return INVALID_HANDLE_VALUE;
}

BOOL WINAPI FindNextFileA( HANDLE handle, WIN32_FIND_DATAA *data )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;		
}

// files/dos_fs.c
BOOL WINAPI FindClose( HANDLE handle )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

//*********************************************** /\/\ FindFirstFile() API /\/\ *************************************//

// files/dos_fs.c
BOOL WINAPI FileTimeToLocalFileTime( const FILETIME *utcft,
                                       LPFILETIME localft )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

// files/dos_fs.c
BOOL WINAPI FileTimeToSystemTime( const FILETIME *ft, LPSYSTEMTIME syst )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;	
}

// dlls/ole32/compobj.c
HRESULT WINAPI CoInitializeEx(
        LPVOID lpReserved,      /* [in] pointer to win32 malloc interface
                                   (obsolete, should be NULL) */
        DWORD dwCoInit          /* [in] A value from COINIT specifies the threading model */
)
{
	// FIXME - Implement this properly!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning S_OK\n");

	HRESULT hr = S_OK;
	return hr;
}

// dlls/ole32/compobj.c
void WINAPI CoUninitialize(void)
{
	// FIXME - Implement this properly!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!\n");

}

// message.c
BOOL WINAPI PeekMessageA( MSG *msg, HWND hwnd, UINT first, UINT last, UINT flags )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;	
}

// message.c
BOOL WINAPI TranslateMessage( const MSG *msg )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

// message.c
LONG WINAPI DispatchMessageA( const MSG* msg )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

/*
// windows/defwnd.c
LRESULT WINAPI DefWindowProcA( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return 0;
}

// windows/class.c
BOOL WINAPI UnregisterClassA( LPCSTR className, HINSTANCE hInstance )
{
	// FIXME - Implement this!!!!!!!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return 0;
}
*/

unsigned int _controlfp(unsigned int newval, unsigned int mask)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

// winsock.h
int gethostname( char* name, int namelen)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;

}

struct hostent* FAR gethostbyname( const char* name)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL (name = '%s')\n", name);

	return NULL;
}

SOCKET socket( int af, int type, int protocol)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL\n");

	return NULL;
}

unsigned long inet_addr( const char* cp)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

int ioctlsocket( SOCKET s, long cmd, u_long* argp )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

int closesocket( SOCKET s)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

int getsockname( SOCKET s, struct sockaddr* name, int* namelen )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

u_short htons( u_short hostshort )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

u_short ntohs( u_short netshort )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

int bind( SOCKET s, const struct sockaddr* name, int namelen)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

int sendto( SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

int getpeername( SOCKET s, struct sockaddr* name, int* namelen)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

int send( SOCKET s, const char* buf, int len, int flags)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

int recv( SOCKET s, char* buf, int len, int flags)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

int recvfrom( SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

int connect( SOCKET s, const struct sockaddr* name, int namelen)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

int shutdown( SOCKET s, int how)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

int WSAStartup( WORD wVersionRequested, LPWSADATA lpWSAData)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

int WSACleanup(void)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

int WSAGetLastError(void)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}


// winbase.h
DWORD GetTickCount(void)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

DWORD GetLastError(void)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

FARPROC WINAPI GetProcAddress( HMODULE hModule, LPCSTR function )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0 (function = '%s')\n", function);

	return 0;
}

HMODULE LoadLibraryEx( LPCTSTR lpFileName, HANDLE hFile, DWORD dwFlags )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0 (filename = '%s')\n", lpFileName);

	return 0;
}

HMODULE WINAPI LoadLibraryA(LPCSTR libname)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0 (libname = '%s')\n", libname);

	return 0;
}

DWORD GetModuleFileNameA( HMODULE hModule, LPTSTR lpFilename, DWORD nSize )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

DWORD FormatMessage( DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPTSTR lpBuffer, DWORD nSize, va_list* Arguments )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

HLOCAL LocalFree( HLOCAL hMem )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

BOOL GetVersionEx( LPOSVERSIONINFO lpVersionInfo )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

BOOL GetComputerNameA( LPTSTR lpBuffer, LPDWORD lpnSize )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

INT WINAPI lstrcmpiA( LPCSTR str1, LPCSTR str2 )
{
    return strcasecmp(str1, str2);
}

BOOL QueryPerformanceCounter( LARGE_INTEGER *lpPerformanceCount )
{
	// Get the current system time from the PSP.
	lpPerformanceCount->QuadPart = sceKernelGetSystemTimeWide();
	return TRUE;
}

BOOL QueryPerformanceFrequency( LARGE_INTEGER *lpFrequency )
{
	// This is a dummy number for now. In the future, it should be more dynamic.
    // The timer functions always return the value in microseconds
	lpFrequency->QuadPart = 1000000; 
	return TRUE;
}


//*********************************** VirtualAlloc() Functions *************************************//
static const int                    g_maxVirtualAllocInfoBlocks = 20;
static int                          g_virtualAllocInfoCount = 0;
static MEMORY_BASIC_INFORMATION     g_virtualAllocInfo[20] = {0};
static char                         g_virtualAllocInfoInitialized = 0;

void initVirtualAllocInfo(){
    int i;


    for (i = 0; i < g_maxVirtualAllocInfoBlocks; i++){
        g_virtualAllocInfo[i].BaseAddress =       0;
        g_virtualAllocInfo[i].AllocationBase =    0;
        g_virtualAllocInfo[i].AllocationProtect = 0;
        g_virtualAllocInfo[i].RegionSize =        0;
        g_virtualAllocInfo[i].State =             0;
        g_virtualAllocInfo[i].Protect =           0;
        g_virtualAllocInfo[i].Type =              0;


    }


    g_virtualAllocInfoInitialized = 1;
}


LPVOID VirtualAlloc( LPVOID lpAddress, DWORD dwSize, DWORD flAllocationType, DWORD flProtect )
{
	// The PSP has no virtual memory, so we'll just allocate the mem using a malloc.  This
	// is a ** REALLY ** quick implementation of this, but it seems to work for now.  Because
	// memory is limited on the PSP, we should be careful when using VirtualAlloc, since the
	// size might have been inflated to a nice-to-have number, and not a just needed number. 

    if (!(flAllocationType & MEM_RESERVE)){
        TRACE("VirtualAlloc(): reserving %d bytes at 0x%X (allocType = 0x%X, protect = %d)\n", dwSize, lpAddress, flAllocationType, flProtect);

		return lpAddress;
    }


    if (g_virtualAllocInfoCount >= g_maxVirtualAllocInfoBlocks){
        TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        TRACE(">>> VirtualAlloc(): ERROR-> virtual allocation info buffer full!!!\n");
        TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

        return NULL;
    }

	lpAddress = malloc( dwSize );

    g_virtualAllocInfo[g_virtualAllocInfoCount].BaseAddress =       lpAddress;
    g_virtualAllocInfo[g_virtualAllocInfoCount].AllocationBase =    0;
    g_virtualAllocInfo[g_virtualAllocInfoCount].AllocationProtect = 0;
    g_virtualAllocInfo[g_virtualAllocInfoCount].RegionSize =        dwSize;
    g_virtualAllocInfo[g_virtualAllocInfoCount].State =             lpAddress;
    g_virtualAllocInfo[g_virtualAllocInfoCount].Protect =           0;
    g_virtualAllocInfo[g_virtualAllocInfoCount].Type =              0;
    g_virtualAllocInfoCount++;


    TRACE("VirtualAlloc(): allocating %d bytes at 0x%X (allocType = 0x%X, protect = %d)\n", dwSize, lpAddress, flAllocationType, flProtect);

	return lpAddress;
}

BOOL VirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType )
{
    if (!(dwFreeType & MEM_RELEASE)){
        TRACE("VirtualAlloc(): releasing %d bytes at 0x%X (freeType = 0x%X)\n", dwSize, lpAddress, dwFreeType);

		return TRUE;
    }

    TRACE("VirtualAlloc(): freeing %d bytes at 0x%X (freeType = 0x%X)\n", dwSize, lpAddress, dwFreeType);

	free(lpAddress);

    return TRUE;//*/
}

DWORD VirtualQuery( LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, DWORD dwLength )
{
        // FIXME - IMPLEMENT THIS!!!!
        TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

        return 0;
}

//***********************************************************************************************************//

HANDLE GetCurrentProcess(void)
{
        // FIXME - IMPLEMENT THIS!!!!
        TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL\n");

        return NULL;
}

HANDLE GetCurrentThread(void)
{
        // FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL\n");

    return NULL;
}

//BOOL GetProcessTimes( HANDLE hProcess, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime )
//{
//       // FIXME - IMPLEMENT THIS!!!!
//        return FALSE;
//}

VOID WINAPI OutputDebugStringA(LPCSTR string)
{
	TRACE(string);
}

void GetSystemInfo( LPSYSTEM_INFO lpSystemInfo )
{
	// Blair - Right now, these are total arbitrary numbers ( 0x10000 is our Cedega value ),
	// but I needed these declared for morphine's LUA init.
	lpSystemInfo->dwPageSize = 4;
	lpSystemInfo->dwAllocationGranularity = 0x10000;
    TRACE(__FUNCTION__"(): STUBWARNING-> only gathering partial system info\n");
}

HANDLE GetProcessHeap(void)
{
        // FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL\n");

    return NULL;
}

PVOID WINAPI RtlAllocateHeap( HANDLE heap, ULONG flags, ULONG size )
{
        // FIXME - IMPLEMENT THIS!!!!
    //TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL\n");

    return malloc(size);
}

BOOLEAN WINAPI RtlFreeHeap( HANDLE heap, ULONG flags, PVOID ptr )
{
        // FIXME - IMPLEMENT THIS!!!!
    //TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");
    free(ptr);

    return TRUE;
}


BOOL GetProcessTimes( HANDLE hProcess, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime )
{
        // FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

    return FALSE;
}

BOOL GetThreadTimes( HANDLE hThread, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime )
{
        // FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

    return FALSE;
}

BOOL WINAPI FreeLibrary(HINSTANCE hLibModule)
{
       // FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

    return FALSE;
}


VOID Sleep( DWORD dwMilliseconds )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!\n");

}

HANDLE CreateEventA( LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCTSTR lpName )
{
       // FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

    return 0;
}

BOOL GetWindowPlacement( HWND hWnd, WINDOWPLACEMENT *lpwndpl )
{
        // FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

    return FALSE;
}

DWORD SetFilePointer( HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod )
{
       // FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

    return 0;
}

BOOL WINAPI UnmapViewOfFile( LPVOID addr )
{
        // FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

    return FALSE;
}


HANDLE WINAPI CreateFileA( LPCSTR filename, DWORD access, DWORD sharing, LPSECURITY_ATTRIBUTES sa, DWORD creation, DWORD attributes, HANDLE template )
{
       // FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL (filename = '%s')\n", filename);

    return NULL;
}


BOOL WINAPI ReadFile( HANDLE hFile, LPVOID buffer, DWORD bytesToRead, LPDWORD bytesRead, LPOVERLAPPED overlapped )
{
        // FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

    return FALSE;
}

BOOL WINAPI WriteFile( HANDLE hFile, LPCVOID buffer, DWORD bytesToWrite, LPDWORD bytesWritten, LPOVERLAPPED overlapped )
{
        // FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

    return FALSE;
}



// winreg.h
DWORD WINAPI RegQueryValueExA( HKEY hkey, LPCSTR name, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD count )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0 (name = '%s')\n", name);

	return 0;
}

DWORD WINAPI RegSetValueExA( HKEY hkey, LPCSTR name, DWORD reserved, DWORD type, CONST BYTE *data, DWORD count )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0 (name = '%s')\n", name);

	return 0;
}

DWORD WINAPI RegOpenKeyExA( HKEY hkey, LPCSTR name, DWORD reserved, REGSAM access, LPHKEY retkey )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0 (name = '%s')\n", name);

	return 0;
}


// winuser.h
HWND CreateWindowEx( DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam )
{
	// FIXME - IMPLEMENT THIS!!!!
        HWND hWnd = NULL;
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL (className = '%s', windowName = '%s')\n", lpClassName, lpWindowName);

	return hWnd;
}

ATOM RegisterClassExA( CONST WNDCLASSEX *lpwcx )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

BOOL UnregisterClass( LPCTSTR lpClassName, HINSTANCE hInstance )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE (className = '%s')\n", lpClassName);

	return FALSE;
}

BOOL SetWindowPos( HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

HWND GetForegroundWindow(VOID)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL\n");

	HWND hWnd = NULL;
	return hWnd;
}

BOOL SetForegroundWindow( HWND hWnd )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

BOOL DestroyWindow( HWND hWnd )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

LONG GetWindowLong( HWND hWnd, int nIndex )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

HWND GetFocus(VOID)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL\n");

	HWND hWnd = NULL;
	return hWnd;
}

HWND SetFocus( HWND hWnd )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL\n");

	HWND hWnd = NULL;
	return hWnd;
}

LRESULT DefWindowProcA( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

BOOL GetKeyboardState( PBYTE lpKeyState )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

BOOL GetCursorPos( LPPOINT lpPoint )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

BOOL SetCursorPos( int X, int Y)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

int ShowCursor( BOOL bShow )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

BOOL ShowWindow( HWND hWnd, int nCmdShow )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

BOOL AdjustWindowRect( LPRECT lpRect, DWORD dwStyle, BOOL bMenu )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

void WINAPI UpdateWindow( HWND hWnd )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!\n");

}

BOOL ClientToScreen( HWND hWnd, LPPOINT lpPoint )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

BOOL ScreenToClient( HWND hWnd, LPPOINT lpPoint )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

// this will be changed by the renderer;
RECT TG_client_rect;

BOOL GetClientRect( HWND hWnd, LPRECT lpRect )
{
        // if we can assume the hWnd is the cooperative window
        // provided to the renderer, we'll return the screen
        // resolution the renderer set in TG_client_rect
	*lpRect = TG_client_rect;

    //TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE and screen size\n");

	return FALSE;
}

HWND SetCapture( HWND hWnd )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL\n");

	HWND hWnd = NULL;
	return hWnd;
}

BOOL ReleaseCapture(VOID)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

BOOL OpenClipboard( HWND hWndNewOwner )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

HANDLE GetClipboardData( UINT uFormat )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL\n");

	return NULL;
}

BOOL CloseClipboard( VOID )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

LPVOID GlobalLock( HGLOBAL hMem )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL\n");

	return NULL;
}

BOOL GlobalUnlock( HGLOBAL hMem )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

WORD WINAPI RegisterWindowMessageA( LPCSTR str )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

HCURSOR WINAPI LoadCursorA(HINSTANCE hInstance, LPCSTR name)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning NULL\n");

	HCURSOR cursor = NULL;
	return cursor;
}

BOOL GetWindowRect( HWND hWnd, LPRECT lpRect )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning FALSE\n");

	return FALSE;
}

BOOL SetRect( LPRECT lprc, int xLeft, int yTop, int xRight, int yBottom )
{
	if (!lprc) return FALSE;
	lprc->left = xLeft;
	lprc->top = yTop;
	lprc->right = xRight;
	lprc->bottom = yBottom;
	return TRUE;
}


// mmsystem.h
MMRESULT timeBeginPeriod( UINT uPeriod )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

MMRESULT timeEndPeriod( UINT uPeriod )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

DWORD timeGetTime(VOID)
{
	// FIXME - IMPLEMENT THIS!!!!
    //TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");
    
    //NOTE: this function is supposed to return time in milliseconds.  At the moment, the CLOCKS_PER_SECOND 
    //  macro is set to a millisecond granularity, so returning the value of clock() will suffice.  HOWEVER,
    //  if this changes on another system, this function will have to be updated.
    clock_t t = clock();

	return (DWORD)t;
} 

MMRESULT timeGetDevCaps( LPTIMECAPS ptc,  UINT cbtc )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}



// time.h
/*
time_t time( time_t *timer )
{
	// FIXME - IMPLEMENT THIS!!!!
	return NULL;
}
*/

void _tzset( void )
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!\n");

}

// shellapi.h
//HINSTANCE ShellExecute( HWND hwnd, LPCTSTR lpOperation, LPCTSTR lpFile, LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd );
HINSTANCE WINAPI ShellExecuteA(HWND hWnd, LPCSTR lpOperation,LPCSTR lpFile, LPCSTR lpParameters,LPCSTR lpDirectory, INT iShowCmd)
{
	// FIXME - IMPLEMENT THIS!!!!
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0 (operation = '%s', file = '%s', params = '%s', directory = '%s')\n", lpOperation, lpFile, lpParameters, lpDirectory);

	return 0;
}


// reent.h
int _link_r (struct _reent * first_ptr, const char * second_ptr, const char * third_ptr)
{
	// This should be a system version of link...
    TRACE(__FUNCTION__"(): STUBWARNING-> implement me!!  Returning 0\n");

	return 0;
}

//-------------------------------------------------------------------------------------
//** Helper functions
//-------------------------------------------------------------------------------------

// These give an idea of how much mem is available after the module is loaded.
int PSP_MemoryRemaining()
{
	return sceKernelTotalFreeMemSize();
}

int PSP_MaxFreeMemorySize()
{
	return sceKernelMaxFreeMemSize();
}
