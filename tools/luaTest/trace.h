/*
    File:   trace.h
    Author: eric van beurden
    Date:   8/7/2006

    Namespaces:           <none>
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#ifndef  __trace_H_
# define __trace_H_

# include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif
    void toLog(const char *fmt, ...);
    void toMem(const char *fmt, ...);
#ifdef __cplusplus
}
#endif

#define vsnprintf   _vsnprintf
#define snprintf    _snprintf
#define access      _access

#define F_OK        0
#define W_OK        2
#define R_OK        4
#define X_OK        6

#ifdef _DEBUG
# define TRACE      toLog
# define MEM        toMem
# define Assert     assert
#else
# define TRACE      __noop
# define MEM        __noop
# define Assert(x)
#endif


#endif
