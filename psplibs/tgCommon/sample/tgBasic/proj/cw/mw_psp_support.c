/*
	mw_psp_support.c
*/

#include <thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/* heap size setting */
//int sce_newlib_heap_kb_size = 100;  /* 100KB */

/* stack size setting */
//int sce_newlib_stack_kb_size = 256;	/* 256KB */

/* thread priority setting */
int sce_newlib_priority = SCE_KERNEL_MODULE_INIT_PRIORITY;

/* thread attribute setting */
unsigned int sce_newlib_attribute = SCE_KERNEL_TH_UMODE;

/* dummy */
void _init(void);
void _fini(void);
void _init(void){}
void _fini(void){}

#ifdef __cplusplus
}
#endif
