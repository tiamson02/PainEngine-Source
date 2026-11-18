// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <limits.h>
#include <float.h>

typedef __int64             SQWORD;
typedef unsigned __int64    QWORD;

#define DLL_IMPORT  __declspec(dllimport)
#define DLL_EXPORT  __declspec(dllexport)

#define FORCEINLINE __forceinline
//#define inline __forceinline

#pragma warning(disable: 4201) // non-standard extension used: nameless union/struct
#pragma warning(disable: 4238) // nonstandard extension used : class rvalue used as lvalue
#pragma warning(disable: 4251) // needs to have dll-interface to be used by clients of class
#pragma warning(disable: 4702) // unreachable code
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4244) // conversion from 'X' to 'Y', possible loss of data

FORCEINLINE void GMemCopy(void* Dst, const void* Src, DWORD Count)
{
    __asm
    {
        mov     ecx, Count
        mov     edi, Dst
        mov     esi, Src
        mov     ebx, ecx
        shr     ecx, 2
        and     ebx, 3
        rep     movsd
        mov     ecx, ebx
        rep     movsb
    }
}

FORCEINLINE void GMemZero(void* Dst, DWORD Count)
{
    __asm
    {
        mov     ecx, Count
        mov     edi, Dst
        xor     eax, eax
        mov     ebx, ecx
        shr     ecx, 2
        and     ebx, 3
        rep     stosd
        mov     ecx, ebx
        rep     stosb
    }
}

#pragma warning (push)
#pragma warning (disable: 4100) // formal parameter unused

inline float __fastcall GCosFloat(float X)
{
	return cosf(X);
}

inline float __fastcall GSinFloat(float X)
{
	return sinf(X);
}

#pragma warning (pop)

inline int GFloatRound(float F)
{
	int I;
	__asm fld   [F]
	__asm fistp [I]
	return I;
}

#pragma warning (push)
#pragma warning (disable: 4035) // no return value
inline DWORD GPerfCounter() 
{
    __asm 
    {
        xor eax,eax     // eax will be tampered with
        _emit 0x0f      // rdtsc - eax,edx
        _emit 0x31      // use only eax
        xor edx,edx     // edx will be tampered with
    }
}
#pragma warning (pop)

#define ARRAY_COUNT(array) (sizeof(array)/sizeof((array)[0]))

inline char* GStrDup(const char* str)
{
    if(str) {
        const size_t len=strlen(str);
        if(len>0) {
            char* ptr=(char*)malloc(len+1);
            strcpy(ptr,str);
            return ptr;
        }
    }
    return 0;
}

inline char* GStrDup2(const char* str, const size_t size=0, const int pos=0)
{
    if(str) {
        size_t len=strlen(str);        
        len=(size>0 && size<len) ? size : len+1;
        if(len>0) {
            char* ptr=(char*)malloc(len);
            strncpy(ptr,str+pos,len);
            return ptr;
        }
    }
    return 0;
}

inline char* GStrCat(char* s1, const char* s2)
{
    if(s1) {
        const size_t l1=strlen(s1);
        if(s2) {
            const size_t l2=strlen(s2);
            if(l2>0) {
                char* ptr=(char*)realloc(s1,l1+l2+1);
                strcpy(ptr+l1,s2);
                return ptr;
            }
        }
        return s1;
    }
    return GStrDup(s2);
}

inline char* GStrFree(char* str)
{
    free(str);
    return 0;
}   

inline static void GSetPrecision(int HiLow) 
{ 
	DWORD ControlWord;
    __asm 
    { 
		mov ebx,HiLow
        shl ebx,8 // 3=hi =0=low 
        fnclex 
        fstcw WORD PTR ControlWord; 
        mov eax,ControlWord 
        and eax,0x0000fcff 
        or eax,ebx 
        mov ControlWord,eax 
        fldcw WORD PTR ControlWord; 
	} 
}

#define EInvalidIndex -1
