/*
    File:   trace.h
    Author: eric van beurden
    Date:   15/6/2006

    Namespaces:           <none>
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#ifndef  __trace_H_
# define __trace_H_

# include "stdTypes.h"


//!
namespace Log{

void toFile(const char *fmt, ...);

};  //namespace Log;


#ifdef _DEBUG
# define TRACE  Log::toFile
#elif defined _LOGGING
# define TRACE  Log::toFile
#else
# define TRACE  __noop
#endif

#endif
