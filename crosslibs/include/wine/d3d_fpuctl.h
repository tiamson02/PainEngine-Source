/* Helper header to abstract away FPU control-word functionality.
*/
#ifndef WINE_D3D_FPUCTL_INCLUDED
#define WINE_D3D_FPUCTL_INCLUDED 

#include "config.h"

#if defined (HAVE_FPU_CONTROL)
 #include <fpu_control.h>
#else /* HAVE_FPU_CONTROL */
 #ifdef HAVE_SYS_TYPES_H
 # include <sys/types.h>
 #endif
 #include <machine/npx.h>

 typedef u_int16_t fpu_control_t;
 #define _FPU_GETCW(cw) __asm__ ("fnstcw %0" : "=m" (*&cw))
 #define _FPU_SETCW(cw) __asm__ ("fldcw %0" : : "m" (*&cw))
 #define _FPU_DEFAULT  __INITIAL_NPXCW__
 
#endif /* HAVE_FPU_CONTROL */


#endif /* WINE_D3D_FPUCTL_INCLUDED */

