
#ifndef _CORE_DEBUG_
#define _CORE_DEBUG_


#if !defined( TRANSGAMING )
#pragma once
#endif


#ifdef TRANSGAMING_WINDOWS
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#endif

#if defined( _DEBUG ) && !defined( TRANSGAMING_WINDOWS )

void ENGINE_API GAssertFailed(const char* Expr, const char* FileName, int LineNo);
#define Assert(x)   if(!(x)) { GAssertFailed(#x,__FILE__,__LINE__); { __asm int 3 } }

#else


# if defined(DEBUG) && defined(TRANSGAMING)
#  include <stdarg.h>
#  include <stdio.h>

#  ifdef TRANSGAMING_WINDOWS
void ENGINE_API HALT();
#  else
void HALT();
#  endif

#  ifdef TRANSGAMING_SILENT_DEBUG
#   ifdef TRANSGAMING_WINDOWS
#    define Assert(x)  do {} while(0)
#    define TRACE      __noop
#   else
#    define Assert(x)  do {} while(0)
#    define TRACE(fmt, args...)  do {} while(0)
#   endif
#  else     // TRANSGAMING_SILENT_DEBUG
#   define Assert(x)  if (!(x)){\
                         TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");\
                         TRACE(">>> Assert(): FAILED %s (%d)-> '%s'\n", __FILE__, __LINE__, #x);\
                         TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");\
                         HALT();\
                     }

#   ifdef TRANSGAMING_WINDOWS
#    undef TRACE
#    define TRACE TNGWindowsTrace
void ENGINE_API TNGWindowsTrace( const char* fmt,...);
#   else
void TNGToFile(const char *fmt, ...);
#    define TRACE    TNGToFile
#   endif   // TRANSGAMING_WINDOWS
#  endif    // TRANSGAMING_SILENT_DEBUG

//safe strings functions and macros
/*#  define ENABLE_STRING_CHECK
#  define ENABLE_CHECK_ALL
#  include "noStringCheck.h"//*/

int snprintf_safe(char *buf, size_t len, const char *file, int line, const char *fmt, ...);
int vsnprintf_safe(char *buf, size_t len, const char *file, int line, const char *fmt, va_list args);
char *strncpy_safe(char *dst, const char *src, size_t len, const char *flie, int line);
char *strncat_safe(char *dst, const char *src, size_t len, const char *file, int line);

# else      // DEBUG && TRANSGAMING
#  ifdef TRANSGAMING_LOGGING
void HALT();

#   define Assert(x)  if (!(x)){\
                         TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");\
                         TRACE(">>> Assert(): FAILED %s (%d)-> '%s'\n", __FILE__, __LINE__, #x);\
                         TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");\
                         HALT();\
                     }
#   ifdef TRANSGAMING_WINDOWS
#    define TRACE TNGWindowsTrace
void TNGWindowsTrace( const char* fmt,...);
#   else
void TNGToFile(const char *fmt, ...);
#    define TRACE   TNGToFile
#   endif   // TRANSGAMING_WINDOWS
#  else     // TRANSGAMING_LOGGING
void HALT();
#   define Assert(x)                    {}
#   ifdef TRANSGAMING_WINDOWS
#    define TRACE __noop
#   else
#    define TRACE(fmt,args...)           {}
#   endif   // TRANSGAMING_WINDOWS
#  endif    // TRANSGAMING_LOGGING
# endif     // DEBUG && TRANSGAMING

#endif // _DEBUG

//#ifndef TRANSGAMING
#if !( defined( TRANSGAMING_PSP ) || defined( TRANSGAMING_PS2 ))
struct ENGINE_API StackTracer
{
	static void Push(const char* name);
	static void ErrorMessageBox();
private:
#define StackTracer_MAX_FUNC_COUNT	(128)
    static const char* NameStack[StackTracer_MAX_FUNC_COUNT];
	static int Count;
};
#endif


//#ifdef _DEBUG

	#define guard(func)
	#define unguard

/*
#else

	#define guard(func) \
		static const char* __FUNC_NAME__ = #func; \
		try \
		{   


	#define unguard \
		} \
		catch(...) \
		{ \
			StackTracer::Push(__FUNC_NAME__); \
			throw; \
		} 

#endif // _DEBUG
*/
#endif //_CORE_DEBUG_
