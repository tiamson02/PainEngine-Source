#include "Engine.h"
//#include <stdarg.h>
#include "Core/Debug.h"




#if defined( TRANSGAMING_PSP )
# include "TNG_stdio_PSP.h"
# include <assert.h>
#elif defined( TRANSGAMING_PS2 )
# include "TNG_stdio_PS2.h"
# include <assert.h>
#elif defined( TRANSGAMING_WINDOWS )
# include "../TNG_WIN/TNG_stdio_WIN.h"
# include <assert.h>
#endif

#if defined( _DEBUG ) && !defined( TRANSGAMING_WINDOWS )


void ENGINE_API GAssertFailed(const char* Expr, const char* FileName, int LineNo)
{
    char tmp[1024];
    sprintf(tmp,"ASSERT L:%d %s", LineNo, Expr);
    MessageBox(NULL,tmp,FileName,MB_OK);
}

#endif // _DEBUG

#if defined(TRANSGAMING) && defined(DEBUG) || defined(TRANSGAMING_LOGGING)
#ifdef TRANSGAMING_WINDOWS
void ENGINE_API HALT()
#else
void HALT()
#endif
{
    //m_dumpMemoryReport("memoryReport (HALT).txt", true, true);
   // while(1){}
    int *x = NULL;
    x[15] = 3;
}
#ifdef TRANSGAMING_WINDOWS
void ENGINE_API TNGWindowsTrace( const char* fmt,...)
{
	char buffer[4096];
	va_list args;
	va_start( args, fmt );
	vsnprintf( buffer, 4096, fmt, args );
	va_end( args );

	OutputDebugString( buffer );
}
#endif

#if defined(DEBUG) || defined(TRANSGAMING_LOGGING)
#include <time.h>

//#define TRANSGAMING_LOG_TO_FILE


# ifdef TRANSGAMING_LOG_TO_FILE
class DummyLogHandler{
private:
    static const int BUFSIZE = 4096;

    FILE *  f;
    char    buf[BUFSIZE];

public:
    DummyLogHandler(){
        f = TNG_fopen("result.txt", "wt");

        if (f == NULL){
            printf("DummyLogHandler(): ERROR-> could not open the log stream!\n");

            return;
        }

        time_t t = time(0);

        strftime(buf, BUFSIZE, "********************************* Log Opened %A %B %d, %Y (%I:%M:%S%p) *********************************\n", localtime(&t));
        TNG_fwrite(buf, 1, strlen(buf), f);
        TNG_fflush(f);
    }
    ~DummyLogHandler(){
        if (f)
            TNG_fclose(f);
    }

    void toLog(const char *fmt, va_list args){
        vsnprintf(buf, BUFSIZE, fmt, args);

        if (f){
            TNG_fwrite(buf, 1, strlen(buf), f);
            TNG_fflush(f);
        }

        printf("%s", buf);
    }
};

DummyLogHandler g_log;


void TNGToFile(const char *fmt, ...){
	va_list args;
	va_start( args, fmt );

    g_log.toLog(fmt, args);
	va_end( args );
}
# else
void TNGToFile(const char *fmt, ...){
    char buf[4096];
	va_list args;


	va_start( args, fmt );
    vsnprintf(buf, 4096, fmt, args);
	va_end( args );

    printf("%s", buf);
    fflush(stdout);
}
# endif

#endif

//******************************* Safe String Implementation ************************************//
#ifndef TRANSGAMING_WINDOWS
#define DISABLE_STRING_CHECK
#include "noStringCheck.h"
#endif

int snprintf_safe(char *buf, size_t len, const char *file, int line, const char *fmt, ...){
    va_list args;


    va_start(args, fmt);

    int error = vsnprintf(buf, len, fmt, args);

    va_end(args);


    //buffer overflowed => break!
    if (error < 0){
        buf[len - 1] = 0;
        TRACE("\n#### ***************************************************** ####\n");
        TRACE("#### snprintf(): '%s' (%d): ERROR-> buffer overflow!  String '%s' exceeded %d characters and will be truncated\n", file, line, buf, len);
        TRACE("#### ***************************************************** ####\n\n");

        return error;
    }


    return error;
}

int vsnprintf_safe(char *buf, size_t len, const char *file, int line, const char *fmt, va_list args){
    int error = vsnprintf(buf, len, fmt, args);


    //buffer overflowed => break!
    if (error < 0){
        buf[len - 1] = 0;
        TRACE("\n#### ***************************************************** ####\n");
        TRACE("#### vsnprintf(): '%s' (%d): ERROR-> buffer overflow!  String '%s' exceeded %d characters and will be truncated\n", file, line, buf, len);
        TRACE("#### ***************************************************** ####\n\n");

        return error;
    }


    return error;
}

char * strncpy_safe(char *dst, const char *src, size_t len, const char *file, int line){

    //buffer will overflow => break!
    if (strlen(src) >= len){
        TRACE("\n#### ***************************************************** ####\n");
        TRACE("#### strncpy(): '%s' (%d): ERROR-> buffer overflow!  String '%s' exceeded %d characters and will be truncated\n", file, line, src, len);
        TRACE("#### ***************************************************** ####\n\n");
    }

    
    char *result = strncpy(dst, src, len);


    //make sure the buffer is always terminated
    dst[len - 1] = 0;

    return result;
}

char *strncat_safe(char *dst, const char *src, size_t len, const char *file, int line){
    
    //buffer will overflow => break!
    if (strlen(src) + strlen(dst) >= len){
        TRACE("\n#### ***************************************************** ####\n");
        TRACE("#### strncat(): '%s' (%d): ERROR-> buffer overflow!  String '%s' + '%s' exceeded %d characters and will be truncated\n", file, line, dst, src, len);
        TRACE("#### ***************************************************** ####\n\n");
    }

    
    char *result = strncat(dst, src, len);


    //make sure the buffer is always terminated
    dst[len - 1] = 0;

    return result;
}
//***********************************************************************************************//

#endif


#if !defined( TRANSGAMING ) || defined( TRANSGAMING_WINDOWS )
const char* StackTracer::NameStack[StackTracer_MAX_FUNC_COUNT];
int StackTracer::Count=0;

void StackTracer::Push(const char* name)
{
	if(Count<StackTracer_MAX_FUNC_COUNT) {
		NameStack[Count++]=name;
	}
}

void StackTracer::ErrorMessageBox()
{
    char Msg[4096]; Msg[0]=0;
	for(int i=Count-1; i>=0; --i) {
        strcat(Msg,NameStack[i]);
        strcat(Msg,": ");
	}
#ifdef TRANSGAMING
    FILE* f=TNG_fopen("crash.log","w");
# define fprintf TNG_fprintf
#else
    FILE* f=fopen("crash.log","w");
#endif
    if(f) {
        fprintf(f,"SHIT HIT THE FAN\n\n");
        fprintf(f,Msg);
        fprintf(f,"\n");
#ifdef TRANSGAMING
# undef fprintf
		TNG_fclose(f);
#else
        fclose(f);
#endif
    }

    MessageBox(NULL,Msg,"SHIT HIT THE FAN",MB_OK);
}
#endif
