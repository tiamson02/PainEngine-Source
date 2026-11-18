#ifndef  __STDTYPES_H__
# define __STDTYPES_H__

# include <stddef.h>


//some function names and constants that windows libc doesn't define properly
# ifdef USE_WINDOWS
#  define vsnprintf   _vsnprintf
#  define snprintf    _snprintf
#  define access      _access
#  define strcasecmp  _stricmp
#  define strncasecmp _strnicmp
#  define unlink      _unlink

//status names for the access() function
#  define F_OK        0
#  define W_OK        2
#  define R_OK        4
#  define X_OK        6
# endif


//signed integral types
# ifdef USE_WINDOWS
typedef __int64                 int64;
# else
typedef long long               int64;
# endif
typedef int                     int32;
typedef short                   int16;
typedef char                    int8;


//unsigned integral types
# ifdef USE_WINDOWS
typedef unsigned __int64        uint64;
# else
typedef unsigned long long      uint64;
# endif
typedef unsigned int            uint32;
typedef unsigned short          uint16;
typedef unsigned char           uint8;


//signed floating point types
typedef float                   float32;
typedef double                  float64;
typedef long double             float80;


typedef ptrdiff_t               ptr_t;


inline ptr_t ptr2int(void *ptr){
    union{
        ptr_t i;
        void *p;
    } p;


    p.p = ptr;

    return p.i;
}

template <typename T>
inline T *int2ptr(ptr_t ptr){
    union{
        ptr_t i;
        T *   p;
    } p;


    p.i = ptr;

    return p.p;
}

#endif
