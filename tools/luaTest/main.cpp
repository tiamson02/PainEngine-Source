#include "Scripting.h"
#include "trace.h"
#include <stdio.h>
#include <io.h>
#include <time.h>
#ifdef USE_WINDOWS
# define WIN32_LEAN_AND_MEAN
# include <Windows.h>
#endif

int testParam(void *buf, int size){
    TRACE("testParam(): doing stuff to the buffer %p (size = %d)\n", buf, size);

    return 0;
}


void timingFunction(){
    int x = 1;


    for (int i = 0; i < 10000000; i++){
        x *= 2;
        x += 0xdeadbeef;
    }
}


__int64 getTimerFreq(){
#ifdef USE_WINDOWS
    unsigned __int64 start, end;

    __asm{
        cpuid 
        rdtsc
        mov dword ptr start, eax
        mov dword ptr [start + 4], edx
    };

    Sleep(500);

    __asm{
        cpuid
        rdtsc
        mov dword ptr end, eax
        mov dword ptr [end + 4], edx
    };

    return (((end - start) * 1000I64) / 500I64);
#else
    return 0;
#endif
}

void testScript(int argc, char **argv){
    Script          script;
    const char *    filename = "scripts/loader.lua";
    bool            loaded = false;


    script.Init();

    //an argument was specified => assume it's a filename and attempt to load it as a script
    if (argc > 1){

        //attempt to load the file specified in each argument
        for (int i = 1; i < argc; i++){
            if (!access(argv[1], F_OK)){
                TRACE("testScript(): loading the script '%s'\n", argv[i]);
             
                if (script.DoFile(argv[1]))
                    loaded = true;

                else
                    TRACE("testScript(): ERROR-> could not load the script '%s'\n", argv[i]);
            }
        }

    }

    if (!loaded){

        //load the script
        TRACE("testScript(): loading the script '%s'\n", filename);
        script.DoFile(filename);
    }


#ifdef RUN_TIMING_TESTS

/*# define T_TYPE             LARGE_INTEGER
# define T_GETVAL(v)        (v.QuadPart)
# define T_GETFREQ(f)       __asm{ cpuid } QueryPerformanceFrequency(&(f))
# define T_START(v)         __asm{ cpuid } QueryPerformanceCounter(&(v))
# define T_STOP(v)          __asm{ cpuid } QueryPerformanceCounter(&(v))
# define T_TICKS(s, e)      ((e).QuadPart - (s).QuadPart)
# define T_GETTIME(s, e, f) (T_TICKS(s, e) / (double)((f).QuadPart))//*/

# define T_TYPE             __int64
# define T_GETVAL(v)        (v)
# define T_START(v)         __asm{ cpuid } __asm{ rdtsc } __asm{ mov dword ptr v, eax } __asm{ mov dword ptr [v + 4], edx } 
# define T_STOP(v)          __asm{ cpuid } __asm{ rdtsc } __asm{ mov dword ptr v, eax } __asm{ mov dword ptr [v + 4], edx } 
# define T_GETFREQ(f)       { f = getTimerFreq(); }
# define T_TICKS(s, e)      ((e) - (s))
# define T_GETTIME(s, e, f) (T_TICKS(s, e) / (double)(f))//*/
    TRACE("testScript(): starting function timing test\n");
    T_TYPE      freq;
    T_TYPE      start;
    T_TYPE      stop;
    T_TYPE      ticks;
    T_TYPE      avgTicks1, avgTicks2, avgTicks3;
    double      t;
    const int   TestCount = 1000;


    T_GETFREQ(freq);

    T_START(start);
        timingFunction();
    T_STOP(stop);
    T_GETVAL(ticks) = T_TICKS(start, stop);
    t = T_GETTIME(start, stop, freq);
    TRACE("testScript(): code time = %g (ticks = %I64d)\n", t, T_GETVAL(ticks));

    T_START(start);
        script.Call("timingFunction", 0, "");
    T_STOP(stop);
    T_GETVAL(ticks) = T_TICKS(start, stop);
    t = T_GETTIME(start, stop, freq);
    TRACE("testScript(): script time = %g (ticks = %I64d)\n", t, T_GETVAL(ticks));
    TRACE("testScript(): done function timing test\n");


    script.Call("testCloneObj", 0, "");
    T_START(start);
    for (int i = 0; i < TestCount; i++)
        script.Call("testCloneObj", 0, "");
    T_STOP(stop);    
    T_GETVAL(avgTicks1) = T_TICKS(start, stop);

    script.Call("testClone1", 0, "");
    T_START(start);
    for (int i = 0; i < TestCount; i++)
        script.Call("testClone1", 0, "");
    T_STOP(stop);
    T_GETVAL(avgTicks2) = T_TICKS(start, stop);

    script.Call("testCloneObj", 0, "");
    T_START(start);
    for (int i = 0; i < TestCount; i++)
        script.Call("testCloneObj", 0, "");
    T_STOP(stop);
    T_GETVAL(avgTicks3) = T_TICKS(start, stop);

    TRACE("\ntestScript(): Clone1() over %d tests: time = %g (ticks = %g)\n", TestCount, (T_GETVAL(avgTicks1) / (double)TestCount) / (double)T_GETVAL(freq), T_GETVAL(avgTicks1) / (double)TestCount);
    TRACE("testScript(): Clone2() over %d tests: time = %g (ticks = %g)\n", TestCount, (T_GETVAL(avgTicks2) / (double)TestCount) / (double)T_GETVAL(freq), T_GETVAL(avgTicks2) / (double)TestCount);
    TRACE("testScript(): CloneObj() over %d tests: time = %g (ticks = %g)\n", TestCount, (T_GETVAL(avgTicks3) / (double)TestCount) / (double)T_GETVAL(freq), T_GETVAL(avgTicks3) / (double)TestCount);
    TRACE("testScript(): Clone1() total time = %g (tick = %I64d)\n", T_GETVAL(avgTicks1) / (double)T_GETVAL(freq), T_GETVAL(avgTicks1));
    TRACE("testScript(): Clone2() total time = %g (tick = %I64d)\n", T_GETVAL(avgTicks2) / (double)T_GETVAL(freq), T_GETVAL(avgTicks2));
    TRACE("testScript(): CloneObj() total time = %g (tick = %I64d)\n", T_GETVAL(avgTicks3) / (double)T_GETVAL(freq), T_GETVAL(avgTicks3));


    TRACE("\ntestScript(): timing LoadObj() with Clone1()\n");
    script.DoString("Clone = Clone1");
    T_START(start);
        script.Call("testLoadObj", 0, "");
    T_STOP(stop);
    T_GETVAL(ticks) = T_TICKS(start, stop);
    t = T_GETTIME(start, stop, freq);
    TRACE("testScript(): time = %g (ticks = %I64d)\n", t, T_GETVAL(ticks));
    TRACE("testScript(): done testing LoadObj() with Clone1()\n");


    TRACE("\ntestScript(): timing LoadObj() with Clone2()\n");
    script.DoString("Clone = Clone2");
    T_START(start);
    script.Call("testLoadObj", 0, "");
    T_STOP(stop);    
    T_GETVAL(ticks) = T_TICKS(start, stop);
    t = T_GETTIME(start, stop, freq);
    TRACE("testScript(): time = %g (ticks = %I64d)\n", t, T_GETVAL(ticks));
    TRACE("testScript(): done testing LoadObj() with Clone2()\n");
# undef T_STOP
# undef T_START
# undef T_TICKS
# undef T_GETFREQ
# undef T_GETTIME
# undef T_GETVAL
# undef T_TYPE
#endif

    //start executing the script
    //script.Call("main", 0, "");
    script.Call("testCloneObjSafety", 0, "");

    script.Release();
}


void testGeneral(int argc, char **argv){
    const int   BUFSIZE = 64;
    char        buf1[BUFSIZE];
    char *      buf2 = new char[BUFSIZE];
    static char buf3[BUFSIZE];
    int         result;


    TRACE("main(): buf1 = %p, buf2 = %p\n", buf1, buf2);
    result = testParam(buf1,    BUFSIZE);
    result = testParam(&buf1,   BUFSIZE);
    result = testParam(buf2,    BUFSIZE);
    result = testParam(&buf2,   BUFSIZE);
    result = testParam(buf3,    BUFSIZE);
    result = testParam(&buf3,   BUFSIZE);


    delete []buf2;
}


int main(int argc, char **argv){
    //testGeneral(argc, argv);
    testScript(argc, argv);

    return 0;
}