#ifndef TG_PC2PSP_H_
#define TG_PC2PSP_H_

#define _PSP_64  0x00000000
#define _MCW_PSP 0x00030000

#ifdef __cplusplus
extern "C" {
#endif

unsigned long PSP_GetTimerSystemTime(void);
float PSP_GetCurrentTimeInSeconds();
float PSP_GetTimeInSeconds(unsigned long t1, unsigned long t2);

void*	PSP_aligned_malloc(size_t size, size_t alignment );
void*	PSP_aligned_realloc(void *memblock, size_t size, size_t alignment );
void	PSP_aligned_free(void *memblock);

// General functions that need implementing....
unsigned int _controlfp(unsigned int newval, unsigned int mask);

// HELPER FUNCTIONS!
int PSP_MemoryRemaining();
int PSP_MaxFreeMemorySize();

extern RECT TG_client_rect;

#ifdef __cplusplus
}
#endif

#endif /*TG_PC2PSP_H_*/
