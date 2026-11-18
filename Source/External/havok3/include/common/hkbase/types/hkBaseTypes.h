/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKBASETYPES_H
#define HKBASE_HKBASETYPES_H

//
// compiler
// 

#if defined(_MSC_VER)
#	define HK_COMPILER_MSVC
#elif defined(__SNC__)
#	define HK_COMPILER_SNC
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
#	define HK_ENDIAN_LITTLE 1
#	define HK_ENDIAN_BIG	0
#	define HK_POINTER_SIZE 4
#elif defined(_M_AMD64)
#	define HK_ARCH_AMD64
#	define HK_ENDIAN_LITTLE 1
#	define HK_ENDIAN_BIG	0
#	define HK_POINTER_SIZE 8
#elif defined( R3000 ) || defined( __R4000__ )
#	define HK_ARCH_PSP
#	define HK_ENDIAN_LITTLE 1
#	define HK_ENDIAN_BIG	0
#	define HK_POINTER_SIZE 4
#elif (!defined( R3000 )) && (!defined( __R4000__ )) && (defined( __mips__ ) || defined( __MIPS__ ))
#	define HK_ARCH_PS2
#	define HK_ENDIAN_LITTLE 1
#	define HK_ENDIAN_BIG	0
#	define HK_POINTER_SIZE 4
#elif defined(_PPC_) || defined(__POWERPC__) || defined(_M_PPC) || defined(_M_PPCBE) || defined(GEKKO)
#	define HK_ARCH_PPC
#	define HK_ENDIAN_LITTLE 0
#	define HK_ENDIAN_BIG	1
#	define HK_POINTER_SIZE 4
#elif defined(__PPU__) && defined(__CELLOS_LV2__)
#	define HK_ARCH_PS3
#	define HK_ENDIAN_LITTLE 0
#	define HK_ENDIAN_BIG	1
#	define HK_POINTER_SIZE 8
#elif defined(__SPU__) && defined(__CELLOS_LV2__)
#	define HK_ARCH_PS3SPU
#	define HK_ENDIAN_LITTLE 0
#	define HK_ENDIAN_BIG	1
#	define HK_POINTER_SIZE 8 // Caution: Actually the pointer size is 4, but usually where this is used pointers will be "shadows" from the PPU
#else
#	error Could not autodetect target architecture
#endif

//
// platform
// 

#if defined(_XBOX)
#	define HK_PLATFORM_IS_CONSOLE 1
#   if defined(HK_ARCH_PPC)
#	  define HK_PLATFORM_XBOX360
#	else
#	  define HK_PLATFORM_XBOX
#   endif
#elif defined(__APPLE_CC__)
#	define HK_PLATFORM_DARWIN
#	define HK_PLATFORM_IS_CONSOLE 0
#elif defined(_WIN32)
#	define HK_PLATFORM_WIN32
#	define HK_PLATFORM_IS_CONSOLE 0
#elif (!defined( R3000 )) && (!defined( __R4000__ )) && (defined( __mips__ ) || defined( __MIPS__ ))
#	define HK_PLATFORM_PS2
#	define HK_PS2
#	define HK_PLATFORM_IS_CONSOLE 1
#elif defined(__unix__)
#	define HK_PLATFORM_UNIX
#	define HK_PLATFORM_IS_CONSOLE 0
#elif defined(GEKKO)
#	define HK_PLATFORM_GC
#	define HK_PLATFORM_IS_CONSOLE 1
#elif defined(__POWERPC__)
#	define HK_PLATFORM_MAC
#elif defined( R3000 ) || defined( __R4000__ )
#	define HK_PLATFORM_PSP
#	define HK_PLATFORM_IS_CONSOLE 1
#elif defined(__PPU__) && defined(__CELLOS_LV2__)
#	define HK_PLATFORM_PS3
#	define HK_PLATFORM_IS_CONSOLE 1
#elif defined(__SPU__) && defined(__CELLOS_LV2__)
#	define HK_PLATFORM_PS3SPU
#	define HK_PLATFORM_IS_CONSOLE 1
#else
#	error Could not autodetect target platform.
#endif

//
// types
//

/// hkReal is the default floating point type.
typedef float  hkReal;
/// hkFloat is provided if floats are explicitly required.
typedef float  hkFloat32;
/// hkDouble is provided if doubles are explicit required.
typedef double hkDouble64;


/// Signed 8 bit integer
typedef signed char		hkChar;
/// Signed 8 bit integer
typedef signed char		hkInt8;
/// Signed 16 bit integer
typedef signed short	hkInt16;
/// Signed 32 bit integer
typedef signed int		hkInt32;

/// Unsigned 8 bit integer
typedef unsigned char	hkUchar;
/// Unsigned  8 bit integer
typedef unsigned char	hkUint8;
/// Unsigned  16 bit integer
typedef unsigned short	hkUint16;
/// Unsigned  32 bit integer
typedef unsigned int	hkUint32;

/// An integer type guaranteed to be the same size as a pointer.
#if defined(HK_ARCH_PS2)
	typedef unsigned int hkUlong;
#elif defined(HK_ARCH_PSP)
	typedef unsigned int hkUlong;
#elif defined(HK_ARCH_AMD64)
	typedef unsigned __int64 hkUlong;
#elif defined(HK_COMPILER_MSVC) && (_MSC_VER >= 1300)
	typedef unsigned long __w64 hkUlong; // VC7.0 or higher, 64bit warnings
#else
	typedef unsigned long hkUlong;
#endif

#if defined(HK_PLATFORM_PS3SPU)
#	define HK_CPU_PTR( A ) hkUint64
#	define HK_INDEX_CPU_PTR( A, INDEX, TYPE ) ( (A) + (INDEX) * sizeof(TYPE) )
#else
#	define HK_CPU_PTR( A ) A
#	define HK_INDEX_CPU_PTR( A, INDEX, TYPE ) ( (A) + INDEX )
#endif

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

#if  defined(DEBUG) || defined(_DEBUG)
#	undef HK_DEBUG
#	define HK_DEBUG
#	define hkDebug 1
#else 
#	define hkDebug 0
#endif

#if defined(HK_ARCH_IA32)
#	if defined(HK_COMPILER_MSVC)
#		define HK_BREAKPOINT() __asm { int 3 }
#	elif defined(HK_COMPILER_GCC)
#		define HK_BREAKPOINT() asm("int $3");
#	else
#		error unknown asm syntax
#	endif
#elif defined(HK_PLATFORM_PS3)
#	define HK_BREAKPOINT() 	__asm__ volatile ( "tw 31,1,1" )
#else
#	define HK_BREAKPOINT() ((*((int*)0)) = 0)
#endif

#define HK_NULL 0

// use the compiler friendly but programmer ugly version for release only
#ifdef HK_DEBUG
#	define HK_MULTILINE_MACRO_BEGIN	do {
#	define HK_MULTILINE_MACRO_END		} while(0)
#else
#	define HK_MULTILINE_MACRO_BEGIN	if(1) {	
#	define HK_MULTILINE_MACRO_END		} else
#endif

/// Note that ALIGNMENT must be a power of two for this to work.
/// Note: to use this macro you must cast your pointer to a byte pointer or to an integer value.
#define HK_NEXT_MULTIPLE_OF(ALIGNMENT, VALUE)  ( ((VALUE) + ((ALIGNMENT)-1)) & (~((ALIGNMENT)-1)) )

/// The offset of a member within a structure
#define HK_OFFSET_OF(CLASS,MEMBER) int(reinterpret_cast<hkUlong>(&(reinterpret_cast<CLASS*>(16)->MEMBER))-16)


/// Join two preprocessor tokens, even when a token is itself a macro.
#define HK_PREPROCESSOR_JOIN_TOKEN(A,B) HK_PREPROCESSOR_JOIN_TOKEN2(A,B)
#define HK_PREPROCESSOR_JOIN_TOKEN2(A,B) HK_PREPROCESSOR_JOIN_TOKEN3(A,B)
#define HK_PREPROCESSOR_JOIN_TOKEN3(A,B) A##B

//
// compiler specific settings
//


	// *************************************
	//			GCC and SN
	// *************************************
#if defined(HK_COMPILER_GCC) || defined(HK_COMPILER_SNC)
#   if defined(__GNUC_PATCHLEVEL__)
#   	define HK_COMPILER_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 +__GNUC_PATCHLEVEL__)
#	else
#		define HK_COMPILER_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
#   endif

#	if( HK_COMPILER_GCC_VERSION >= 30400 ) && (HK_COMPILER_GCC_VERSION < 40000) && !defined( HK_COMPILER_SNC )
#		undef HK_OFFSET_OF
#		define HK_OFFSET_OF(CLASS,MEMBER) (__offsetof__(reinterpret_cast<long>(&reinterpret_cast<CLASS*>(16)->MEMBER)-16))
#	endif

#	define HK_ALIGN(DECL, ALIGNMENT) __attribute__((aligned(ALIGNMENT))) DECL
#	define HK_ALIGN16(DECL) __attribute__((aligned(16))) DECL
#   if defined(HK_PLATFORM_PS2)
	    typedef unsigned long hkUint64;	// long is 64 bits on PS2
	    typedef long hkInt64;
		typedef int hkInt128 __attribute__ ((mode (TI)));
		typedef unsigned int hkUint128 __attribute__ ((mode (TI)));
#   elif defined(HK_PLATFORM_PS3)
		typedef unsigned long hkUint64;
		typedef long hkInt64;
#   elif defined(HK_PLATFORM_PS3SPU)
		typedef unsigned long long hkUint64;
		typedef long long hkInt64;
#   else
	    typedef unsigned long long hkUint64;
	    typedef long long hkInt64;
#   endif
#	if defined(HK_PLATFORM_GC)
		typedef hkUint64 hkSystemTime; // GameCube Time ticks are 64 bit
#	else
		typedef long hkSystemTime;
#	endif
#	if defined(HK_PLATFORM_DARWIN) || defined(HK_PLATFORM_PS3) || defined (HK_PLATFORM_PS3SPU)
		typedef unsigned long hk_size_t;
#	else
		typedef unsigned hk_size_t;
#	endif
#	if defined(HK_ARCH_IA32) && HK_COMPILER_GCC_VERSION >= 30100
#		define HK_COMPILER_HAS_INTRINSICS_IA32
#	endif
#	if defined(HK_ARCH_PS2) && HK_COMPILER_GCC_VERSION >= 30003 && !defined(HK_COMPILER_SNC)
#		define HK_COMPILER_HAS_INTRINSICS_PS2
#	endif
#	define HK_FORCE_INLINE inline
// calling convention
#	define HK_CALL
#	define HK_FAST_CALL


	// *************************************
	//			MICROSOFT and INTEL
	// *************************************
#elif defined(HK_COMPILER_MSVC) || defined(HK_COMPILER_INTEL)
#	define HK_COMPILER_MSVC_VERSION _MSC_VER
#	define HK_COMPILER_INTEL_VERSION _MSC_VER
#	pragma warning( disable : 4786 ) // Debug tuncated to 255:
#	pragma warning( disable : 4530 ) // C++ Exception handler used but not enabled:(used in <xstring>)
#	if defined(HK_PLATFORM_XBOX) || defined(HK_PLATFORM_XBOX360) || defined(HK_PLATFORM_WIN32)
#		define HK_ALIGN(DECL, ALIGNMENT) __declspec(align(ALIGNMENT)) DECL
#		define HK_ALIGN16(DECL) __declspec(align(16)) DECL
#		define HK_FORCE_INLINE __forceinline
#	elif defined(HK_PLATFORM_UNIX)
#		define HK_ALIGN(DECL, ALIGNMENT) __attribute__((aligned(ALIGNMENT))) DECL
#		define HK_ALIGN16(DECL) __attribute__((aligned(16))) DECL
#		define HK_FORCE_INLINE inline
#	else
#		error "fix the alignment on this platform"
#	endif
#	if defined(HK_ARCH_IA32)
		typedef unsigned __int64 hkUint64;
		typedef __int64 hkInt64;
		typedef long hkSystemTime;
#		if defined(HK_COMPILER_MSVC) && (_MSC_VER >= 1300)
			typedef unsigned __w64 hk_size_t; // VC7.0 or higher, 64bit warnings
#		else
			typedef unsigned hk_size_t;
#		endif
#		define HK_COMPILER_HAS_INTRINSICS_IA32
#	elif defined( HK_ARCH_PPC )
		typedef unsigned __int64 hkUint64;
		typedef __int64 hkInt64;
		typedef unsigned hk_size_t;
		typedef __int64 hkSystemTime; // 64bit time 
#		define HK_COMPILER_HAS_INTRINSICS_PPC
#	elif defined(HK_ARCH_AMD64)
		typedef unsigned __int64 hkUint64;
		typedef __int64 hkInt64;
		typedef __int64 hkSystemTime;
		typedef unsigned __int64 hk_size_t;
#	else
#		error No defs for this architecture
#	endif
// calling convention
#	define HK_CALL __cdecl
#	define HK_FAST_CALL __fastcall

	// *************************************
	//			METROWERKS
	// *************************************
#elif defined(HK_COMPILER_MWERKS)
#	define HK_ALIGN(DECL, ALIGNMENT) DECL __attribute__((aligned(ALIGNMENT)))
#	define HK_ALIGN16(DECL) DECL __attribute__((aligned(16)))
#   if defined(HK_PLATFORM_PS2)
		typedef unsigned long hkUint64;	// long is 64 bits on PS2
		typedef long hkInt64;
#   endif
#	if defined(HK_PLATFORM_PSP)
		typedef unsigned long long hkUint64;	// long long is 64 bits on PSP
		typedef long long hkInt64;
#	endif				
#   if defined(HK_PLATFORM_PS2) || defined(HK_PLATFORM_PSP)		
		typedef long hkSystemTime;
		typedef unsigned hk_size_t;
		typedef __int128 hkInt128;
		typedef unsigned __int128 hkUint128;
#   else
		typedef unsigned long long hkUint64;
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
#	define HK_FAST_CALL

#else
#	error Unknown compiler
#endif // compilers

typedef hkUint16 hkObjectIndex;
typedef hkReal hkTime;

#define HK_INVALID_OBJECT_INDEX 0xffff 


HK_FORCE_INLINE hkInt32 HK_CALL hkPointerToInt32( const void* ptr )
{
	return static_cast<int>( hkUlong(ptr) );
}

/// get the byte offset of B - A, as a full ulong.
HK_FORCE_INLINE hkUlong HK_CALL hkGetByteOffset( const void* base, const void* pntr)
{
	return hkUlong(pntr) - hkUlong(base);
}

/// get the byte offset of B - A, as an int (64bit issues, so here for easy code checks)
HK_FORCE_INLINE int HK_CALL hkGetByteOffsetInt( const void* base, const void* pntr)
{
	return static_cast<int>( hkGetByteOffset( base, pntr ) );
}

template <typename TYPE>
HK_FORCE_INLINE TYPE* HK_CALL hkAddByteOffset( TYPE* base, hkUlong offset )
{
	return reinterpret_cast<TYPE*>( reinterpret_cast<char*>(base) + offset );
}

template <typename TYPE>
HK_FORCE_INLINE const TYPE* HK_CALL hkAddByteOffsetConst( const TYPE* base, hkUlong offset )
{
	return reinterpret_cast<const TYPE*>( reinterpret_cast<const char*>(base) + offset );
}

	/// If you have a pair of pointers and you have one pointer, than this function allows you to quickly get the other pointer of the pair.
template <typename TYPE>
HK_FORCE_INLINE TYPE* HK_CALL hkSelectOther( TYPE* a, TYPE* pairA, TYPE* pairB )
{
	return reinterpret_cast<TYPE*>( hkUlong(a) ^ hkUlong(pairA) ^ hkUlong(pairB) );
}


#ifdef HK_PLATFORM_XBOX360
#	define HK_RESTRICT __restrict
#else
#	define HK_RESTRICT
#endif




#define hkSizeOf(A) int(sizeof(A))

#define HK_DECLARE_REFLECTION() \
	static const struct hkInternalClassMember Members[]; \
	struct DefaultStruct

class hkClass;

/// A generic object with metadata.
struct hkVariant
{
	void* m_object;
	const hkClass* m_class;
};

/// A wrapper to store a hkBool in one byte, regardless of compiler options.
class hkBool
{
	public:

		inline hkBool()
		{
		}

		inline hkBool(bool b)
		{
			m_bool = static_cast<char>(b);
		}

		inline operator bool() const
		{
			return m_bool != 0;
		}

		inline hkBool& operator=(bool e)
		{
			m_bool = static_cast<char>(e);
			return *this;
		}

		inline hkBool operator==(bool e) const
		{
			return static_cast<int>(m_bool) == static_cast<int>(e);
		}

		inline hkBool operator!=(bool e) const
		{
			return static_cast<int>(m_bool) != static_cast<int>(e);
		}

	private:
		char m_bool;
};



template<typename ENUM, typename STORAGE>
class hkEnum
{
	public:

		hkEnum()
		{
		}

		hkEnum(ENUM e)
		{
			m_storage = static_cast<STORAGE>(e);
		}

		operator ENUM() const
		{
			return static_cast<ENUM>(m_storage);
		}
		void operator=(ENUM e)
		{
			m_storage = static_cast<STORAGE>(e);
		}
		hkBool operator==(ENUM e) const
		{
			return m_storage == static_cast<STORAGE>(e);
		}
		hkBool operator!=(ENUM e) const
		{
			return m_storage != static_cast<STORAGE>(e);
		}

	private:

		STORAGE m_storage;
};

#endif // HKBASE_HKBASETYPES_H

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
