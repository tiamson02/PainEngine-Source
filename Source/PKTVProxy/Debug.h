
#pragma once

#ifdef _DEBUG

void  GAssertFailed(const char* Expr, const char* FileName, int LineNo);
#define Assert(x)   if(!(x)) { GAssertFailed(#x,__FILE__,__LINE__); { __asm int 3 } }

#else

#define Assert(x)

#endif // _DEBUG

#ifdef TRANSGAMING_WINDOWS
struct  ENGINE_API StackTracer
#else
struct  StackTracer
#endif
{
	static void Push(const char* name);
	static void ErrorMessageBox();
private:
#define StackTracer_MAX_FUNC_COUNT	(128)
    static const char* NameStack[StackTracer_MAX_FUNC_COUNT];
	static int Count;
};


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
