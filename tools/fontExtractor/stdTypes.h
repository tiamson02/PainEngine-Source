/*
    File:   stdTypes.h
    Author: eric van beurden
    Date:   oct 2005

    Namespaces:           <none>
    System Dependencies:  <none>
    Configuration Macros: <none>

*/

#ifndef  __stdTypes_H__
# define __stdTypes_H__

# include <stddef.h>


/*! \addtogroup stdtypes
    @{

    These are the standard type names that should be used throughout the project.  
    The types are guaranteed to be their reported size and will be so on any platform.  
    One goal of this project is platform independence, and these types should help 
    achieve that.

    Include the file:
    \code #include <stdTypes.h> \endcode
*/

# ifdef WIN32
//rename some of the libc functions to their unix standard names.  Windows' libc 
//uses the export names for some reason.
#  define vsnprintf     _vsnprintf
#  define snprintf      _snprintf
#  define access        _access

//reanme some of the string functions to match those used under unix.
#  define strcasecmp    _stricmp
#  define strncasecmp   _strnicmp

//#defines for access().  These are not defined in MS's libc, but are under unix
#  define F_OK  0x00      //request if file exists
#  define R_OK  0x02      //request if file is readable
#  define W_OK  0x04      //request if file is writeable
#  define X_OK  0x08      //request if file is executable
# endif

/*! \name Integral Types
    @{
*/
#ifdef WIN32
//! guaranteed 64 bit signed integer type.
typedef __int64             int64;

#else
//! guaranteed 64 bit signed integer type.
typedef long long           int64;

#endif

//! guaranteed 32 bit signed integer type.
typedef int                 int32;

//! guaranteed 16 bit signed integer type.
typedef short               int16;

//! guaranteed 8 bit signed integer type.
typedef char                int8;


#ifdef WIN32
//! guaranteed 64 bit unsigned integer type.
typedef unsigned __int64    uint64;

#else
//! guaranteed 64 bit unsigned integer type.
typedef unsigned long long  uint64;

#endif

//! guaranteed 32 bit unsigned integer type.
typedef unsigned long       uint32;

//! guaranteed 16 bit unsigned integer type.
typedef unsigned short      uint16;

//! guaranteed 8 bit unsigned integer type.
typedef unsigned char       uint8;
//! @}


/*! \name Pointer Types
    @{
*/
# ifdef __64BIT__
/*! \brief Integral pointer storage type

    provides storage for the integer equivalent of a native pointer.  The actual size and 
    type of this type depends on the architecture that the project is compiled to (32 or 
    64 bit).  This type is to be used for times when pointers must be stored, passed, 
    or modified as their integer equivalents.
*/
typedef uint64              int_ptr;

# else
/*! \brief Integral pointer storage type

    provides storage for the integer equivalent of a native pointer.  The actual size 
    and type of this type depends on the architecture that the project is compiled to 
    (32 or 64 bit).  This type is to be used for times when pointers must be stored, 
    passed, or modified as their integer equivalents.
*/
typedef uint32              int_ptr;
# endif
/*! @} */


/*! \name Floating Point Types
    @{
*/

/*! \brief 32-bit floating point type

    32 bit floating point value.  This type is guaranteed to be 32 bits wide, but the 
    storage range (ie: mantissa, exponent proportion) is not guaranteed.
*/
typedef float               float32;

/*! \brief 64-bit floating point type

    64 bit floating point value.  This type is guaranteed to be 64 bits wide, but the 
    storage range (ie: mantissa, exponent proportion) is not guaranteed.
*/
typedef double              float64;

/*! \brief 80+ bit floating point type
    
    80 bit or larger floating point value.  This type is not guaranteed to be 80 bits 
    wide, but its size is guarantted to be larger than or equal to the size of float64.
*/
typedef long double         float80;
//! @}

/*! @} */

#endif