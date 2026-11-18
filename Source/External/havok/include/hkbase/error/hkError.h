/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKERROR_H
#define HKBASE_HKERROR_H

/// Class with static methods to provide hooks to error reporting functions
/// You can redirect all asserts, errors and warnings by setting the handlers
/// at run time. Asserts and Warnings are only compiled into debug builds.
/// Errors are compiled in all builds. See hkDefaultError.h for a sample implementation
/// of the handlers.
class hkError : public hkSingleton<hkError>
{
	public:

			/// displayMessage is used for asserts
		virtual void assertMessage(const char* what, const char* description, const char* file, int line) = 0;

			/// displayMessage is used for errors.
		virtual void errorMessage(const char* what, const char* description, const char* file, int line) = 0;

			/// displayMessage is used for warnings (usually performance related).
		virtual void warningMessage(const char* what, const char* description, const char* file, int line) = 0;

			/// Disables the assert on the specified line of the specified file.
		virtual void disableAssert( const char *file, int line ) = 0;
};

namespace hkCompileError
{
	template <bool b>
    struct COMPILE_ASSERTION_FAILURE;
	template <>
    struct COMPILE_ASSERTION_FAILURE<true>{ };
}

// compile time asserts and errors are always enabled
#define HK_COMPILE_TIME_ASSERT(a) enum { HK_PREPROCESSOR_JOIN_TOKEN(compile_time_assert_, __LINE__) \
   									= sizeof(hkCompileError::COMPILE_ASSERTION_FAILURE<bool(a)>) }

#define HK_ERROR(a)		HK_MULTILINE_MACRO_BEGIN													\
							hkInplaceStringStreambuf<> str;													\
							hkOstream ostr(&str);													\
							ostr << a;																\
							hkError::getInstance().errorMessage("Error", str.getString(),__FILE__,__LINE__);	\
						HK_MULTILINE_MACRO_END

// asserts and warnings may be compiled out
#ifdef HK_DEBUG
#	define HK_ASSERT(a)		HK_MULTILINE_MACRO_BEGIN 												\
								if(!(a)) {															\
									hkError::getInstance().assertMessage("Assert failed",#a,__FILE__,__LINE__);		\
								}																	\
							HK_MULTILINE_MACRO_END

#	define HK_ASSERT2(a, TEXT)		HK_MULTILINE_MACRO_BEGIN 													\
								if(!(a)) {																		\
									hkInplaceStringStreambuf<> str;														\
									hkOstream ostr(&str);														\
									ostr << #a << "\n";															\
									ostr << TEXT;																\
									hkError::getInstance().assertMessage("Assert failed", str.getString(),__FILE__,__LINE__);	\
								}																				\
							HK_MULTILINE_MACRO_END

#	define HK_WARN(TEXT)	HK_MULTILINE_MACRO_BEGIN															\
									hkInplaceStringStreambuf<> str;														\
									hkOstream ostr(&str);														\
									ostr << TEXT;																\
									hkError::getInstance().warningMessage("Warning", str.getString(),__FILE__,__LINE__);	\
							HK_MULTILINE_MACRO_END


#   define HK_WARN_ONCE(TEXT)	HK_MULTILINE_MACRO_BEGIN 							    \
							static bool shown = false;									\
							if( !shown )												\
							{															\
								shown = true;                                            \
								hkInplaceStringStreambuf<> str;									\
								hkOstream ostr(&str);									\
								ostr << TEXT;											\
								hkError::getInstance().getInstance().warningMessage("Warning", str.getString(),__FILE__,__LINE__); \
							}														\
							HK_MULTILINE_MACRO_END
#else
#	define HK_WARN(a)				//nothing
#	define HK_WARN_ONCE(a)			//nothing
#	define HK_ASSERT(a)				//nothing 
#	define HK_ASSERT2(a, TEXT)		//nothing 
#endif

#endif // HKBASE_HKERROR_H

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
