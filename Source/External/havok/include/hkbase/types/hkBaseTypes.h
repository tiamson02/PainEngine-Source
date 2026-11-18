/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKBASETYPES_H
#define HKBASE_HKBASETYPES_H

//
// compiler
// 

#if defined(_MSC_VER)
#	define HK_COMPILER_MSVC
#elif defined(__GNUC__)
#	define HK_COMPILER_GCC
#elif defined(__MWERKS__)
#	define HK_COMPILER_MWERKS
#elif defined(__INTEL_COMPILER)
#	define HK_COMPILER_INTEL
#else
#	error Could not detect compiler
#endif

//
// architecture
// 

#if defined(__i386__) || defined(_M_IX86)
#	define HK_ARCH_IA32
#elif defined(__MIPS__) || defined(__mips__)
#	define HK_ARCH_PS2
#elif defined(__POWERPC__) || defined(_M_PPC) || defined(GEKKO)
#	define HK_ARCH_PPC
#else
#	error Could not autodetect target architecture
#endif

//
// platform
// 

#if defined(_XBOX)
#	define HK_PLATFORM_XBOX
#elif defined(_WIN32)
#	define HK_PLATFORM_WIN32
#elif defined(__mips__) || defined(__MIPS__)
#	define HK_PLATFORM_PS2
#	define HK_PS2
#	define HK_PS2ASM
#elif defined(__unix__)
#	define HK_PLATFORM_UNIX
#elif defined(GEKKO)
#	define HK_PLATFORM_GC
#elif defined(__POWERPC__)
#	define HK_PLATFORM_MAC
#else
#	error Could not autodetect target platform.
#endif

//
// types
//

/// hkReal is the default floating point type.
typedef float  hkReal;
/// hkFloat is provided if floats are explicity required.
typedef float  hkFloat32;
/// hkDouble is provided if doubles are explicity required.
typedef double hkDouble64;

/// A pointer to an unspecified type.
typedef void*  hkClientPtr;

typedef signed char		hkChar;
typedef signed char		hkInt8;
typedef signed short	hkInt16;
typedef signed int		hkInt32;

typedef unsigned char	hkUchar;
typedef unsigned char	hkUint8;
typedef unsigned short	hkUint16;
typedef unsigned int	hkUint32;

typedef void* hk_va_list;

/// a simple success/failure enum.
enum hkResult
{
	HK_SUCCESS = 0,
	HK_FAILURE = 1
};

//
// useful macros
//

#if  (!defined(HK_DEBUG)) && (defined(DEBUG) || defined(_DEBUG))
#	define HK_DEBUG
#endif

#define HK_BREAKPOINT() ((*((int*)0)) = 0)
#define HK_NULL 0

// use the compiler friendly but programmer ugly version for release only
#ifdef HK_DEBUG
#	define HK_MULTILINE_MACRO_BEGIN	do {
#	define HK_MULTILINE_MACRO_END		} while(0)
#else
#	define HK_MULTILINE_MACRO_BEGIN	if(1) {	
#	define HK_MULTILINE_MACRO_END		} else
#endif

/// Note that M must be a power of two for this to work.
#define HK_NEXT_MULTIPLE_OF(M, P)  ( ((P) + ((M)-1)) & (~((M)-1)) )

/// The offset of a member within a structure
#define HK_OFFSET_OF(A,B) (reinterpret_cast<unsigned>(&(reinterpret_cast<A*>(16)->B))-16)


/// Join two preprocessor tokens, even when a token is itself a macro.
#define HK_PREPROCESSOR_JOIN_TOKEN(A,B) HK_PREPROCESSOR_JOIN_TOKEN2(A,B)
#define HK_PREPROCESSOR_JOIN_TOKEN2(A,B) HK_PREPROCESSOR_JOIN_TOKEN3(A,B)
#define HK_PREPROCESSOR_JOIN_TOKEN3(A,B) A##B

//
// compiler specific settings
//

#if defined(HK_COMPILER_GCC)
#   if defined(__GNUC_PATCHLEVEL__)
#   	define HK_COMPILER_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 +__GNUC_PATCHLEVEL__)
#	else
#		define HK_COMPILER_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
#   endif

#	define HK_ALIGNED_VARIABLE(NAME,ALIGNMENT) NAME __attribute__((aligned(ALIGNMENT)))
#   if defined(HK_PLATFORM_PS2)
	    typedef unsigned long hkUint64;	// long is 64 bits on PS2
	    typedef long hkInt64;
		typedef int hkInt128 __attribute__ ((mode (TI)));
		typedef unsigned int hkUint128 __attribute__ ((mode (TI)));
#   else
	    typedef unsigned long long hkUint64;
	    typedef long long hkInt64;
#   endif
#	if defined(HK_PLATFORM_GC)
		typedef hkUint64 hkSystemTime; // GameCube Time ticks are 64 bit
#	else
		typedef long hkSystemTime;
#	endif
	typedef unsigned hk_size_t;
#	if defined(HK_ARCH_IA32) && HK_COMPILER_GCC_VERSION >= 30100
#		define HK_COMPILER_HAS_INTRINSICS_IA32
#	endif
#	if defined(HK_ARCH_PS2) && HK_COMPILER_GCC_VERSION >= 30003
#		define HK_COMPILER_HAS_INTRINSICS_PS2
#	endif
#	define HK_FORCE_INLINE inline
// calling convention
#	define HK_CALL

#elif defined(HK_COMPILER_MSVC) || defined(HK_COMPILER_INTEL)
#	define HK_COMPILER_MSVC_VERSION _MSC_VER
#	define HK_COMPILER_INTEL_VERSION _MSC_VER
#	pragma warning( disable : 4786 ) // Debug tuncated to 255:
#	pragma warning( disable : 4530 ) // C++ Exception handler used but not enabled:(used in <xstring>)
#	if defined(HK_PLATFORM_XBOX)
#		define HK_ALIGNED_VARIABLE(NAME,ALIGNMENT) __declspec(align(ALIGNMENT)) NAME
#		define HK_FORCE_INLINE __forceinline
#	elif defined(HK_PLATFORM_WIN32)
#		define HK_ALIGNED_VARIABLE(NAME,ALIGNMENT) __declspec(align(ALIGNMENT)) NAME
#		define HK_FORCE_INLINE __forceinline
#	elif defined(HK_PLATFORM_UNIX)
#		define HK_ALIGNED_VARIABLE(NAME,ALIGNMENT) NAME __attribute__((aligned(ALIGNMENT)))
#		define HK_FORCE_INLINE inline
#	else
#		error "fix the alignment on this platform"
#	endif
#	if defined(HK_ARCH_IA32)
		typedef unsigned __int64 hkUint64;
		typedef __int64 hkInt64;
		typedef long hkSystemTime;
		typedef unsigned hk_size_t;
#		define HK_COMPILER_HAS_INTRINSICS_IA32
#	else
#		error No defs for this architecture
#	endif
// calling convention
#	define HK_CALL __cdecl

#elif defined(HK_COMPILER_MWERKS)
#	define HK_ALIGNED_VARIABLE(NAME,ALIGNMENT) NAME __attribute__((aligned(ALIGNMENT)))
#   if defined(HK_PLATFORM_PS2)
		typedef unsigned long hkUint64;	// long is 64 bits on PS2
		typedef long hkInt64;
		typedef long hkSystemTime;
		typedef unsigned hk_size_t;
		typedef __int128 hkInt128;
		typedef unsigned __int128 hkUint128;
#   else
		typedef unsigned long hkUint64;
		typedef long long hkInt64;
#		if defined(HK_PLATFORM_GC)
			typedef hkUint64 hkSystemTime; // GameCube Time ticks are 64 bit
#		else
			typedef unsigned long hkSystemTime;
#		endif
		typedef unsigned long hk_size_t;
#   endif
#	define HK_FORCE_INLINE inline
// calling convention
#	define HK_CALL

#else
#	error Unknown compiler
#endif // compilers

/// get the byte offset of B - A
inline int HK_CALL hkGetByteOffset( const void* base, const void* pntr)
{
	return static_cast<const char*>(pntr) - static_cast<const char*>(base);
}

template <typename TYPE>
inline TYPE* HK_CALL hkAddByteOffset( TYPE* base, int offset )
{
	return reinterpret_cast<TYPE*>( reinterpret_cast<char*>(base) + offset );
}

#define hkSizeOf(A) int(sizeof(A))

#endif // HKBASE_HKBASETYPES_H

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
