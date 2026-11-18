/*
    File:   trace.cpp
    Author: eric van beurden
    Date:   15/6/2006

    Namespaces:           Log
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#include "trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/*#define WIN32_LEAN_AND_MEAN
#include <windows.h>//*/

//!
namespace Log{

void toFile(const char *fmt, ...){
    const int   BUFSIZE = 1024;
    va_list     args;
    char        buf[BUFSIZE];


    va_start(args, fmt);
    vsnprintf(buf, BUFSIZE, fmt, args);
    va_end(args);


#ifdef OutputDebugString
    OutputDebugString(buf);
#endif
    printf("%s", buf);
    fflush(stdout);
}

};  //namespace Log;
