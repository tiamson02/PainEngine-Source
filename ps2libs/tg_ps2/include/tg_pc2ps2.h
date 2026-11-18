#ifndef TG_PC2PS2_H_
#define TG_PC2PS2_H_

#define _PS2_64  0x00000000
#define _MCW_PS2 0x00030000

#ifdef __cplusplus
extern "C" {
#endif

unsigned long PS2_GetTimerSystemTime(void);
float PS2_GetCurrentTimeInSeconds();
float PS2_GetTimeInSeconds(unsigned long t1, unsigned long t2);

void*	PS2_aligned_malloc(size_t size, size_t alignment );
void*	PS2_aligned_realloc(void *memblock, size_t size, size_t alignment );
void	PS2_aligned_free(void *memblock);

// General functions that need implementing....
unsigned int _controlfp(unsigned int newval, unsigned int mask);

extern RECT TG_client_rect;

#ifdef __cplusplus
}
#endif

DEFINE_GUID(GUID_SysMouse,        0x6F1D2B60,0xD5A0,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

#endif /*TG_PC2PS2_H_*/
