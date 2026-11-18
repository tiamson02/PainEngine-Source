#include "Engine.h"

#include "Debug.h"

#ifdef _DEBUG

#if defined( TRANSGAMING_PSP )
# include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
# include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
# include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

void GAssertFailed(const char* Expr, const char* FileName, int LineNo)
{
    char tmp[1024];
    sprintf(tmp,"ASSERT L:%d %s", LineNo, Expr);
    MessageBox(NULL,tmp,FileName,MB_OK);
}

#endif // _DEBUG

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

