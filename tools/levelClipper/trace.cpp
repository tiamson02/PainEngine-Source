/*
    File:   trace.cpp
    Author: eric van beurden
    Date:   8/7/2006

    Namespaces:           <none>
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#include "stdTypes.h"
#include <stdio.h>
#include <stdarg.h>
#ifdef USE_WINDOWS
# define WIN32_LEAN_AND_MEAN
# include <Windows.h>
#endif
#include "trace.h"


FILE *  g_log = NULL;
FILE *  g_mem = NULL;
bool    g_closeRegistered = false;

void closeLog(){
    if (g_log)
        fclose(g_log);

    if (g_mem)
        fclose(g_mem);

    g_log = NULL;
    g_mem = NULL;
}


void toLog(const char *fmt, ...){
    const int32 BUFSIZE = 4096;
    char        buf[BUFSIZE];
    va_list     args;
    

    if (g_log == NULL){

        g_log = fopen("result.txt", "wt");

        if (!g_closeRegistered && g_log){
            g_closeRegistered = true;

            atexit(closeLog);
        }
    }


    va_start(args, fmt);
    vsnprintf(buf, BUFSIZE, fmt, args);
    va_end(args);


#ifdef OutputDebugString
    OutputDebugString(buf);
#endif
    fputs(buf, stderr);
    fflush(stderr);
    if (g_log){
        fputs(buf, g_log);
        fflush(g_log);
    }
}

void toMem(const char *fmt, ...){
    const int32 BUFSIZE = 4096;
    char        buf[BUFSIZE];
    va_list     args;


    if (g_mem == NULL){

        g_mem = fopen("memory.txt", "wt");

        if (!g_closeRegistered && g_mem){
            g_closeRegistered = true;

            atexit(closeLog);
        }
    }


    va_start(args, fmt);
    vsnprintf(buf, BUFSIZE, fmt, args);
    va_end(args);


    if (g_mem){
        fputs(buf, g_mem);
        fflush(g_mem);
    }
}