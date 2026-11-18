#ifndef _ALUT_H_
#define _ALUT_H_

#include "al/al.h"

#ifdef __cplusplus
extern "C" {
#endif

void  alutInit(int *argc, char *argv[]);
void  alutExit(ALvoid);

ALboolean  alutLoadWAV( const char *fname,ALvoid **wave,ALsizei *format,ALsizei *size,ALsizei *bits,ALsizei *freq );
void  alutLoadWAVFile(ALbyte *file,ALenum *format,ALvoid **data,ALsizei *size,ALsizei *freq,ALboolean *loop);
void  alutLoadWAVMemory(ALbyte *memory,ALenum *format,ALvoid **data,ALsizei *size,ALsizei *freq,ALboolean *loop);
void  alutUnloadWAV(ALenum format,ALvoid *data,ALsizei size,ALsizei freq);

#ifdef __cplusplus
}
#endif

#endif
