/* Internal PS2AL implementation header */



#ifndef __AL_INTERNAL_H__
#define __AL_INTERNAL_H__

#include <stdlib.h>
#include <sifdev.h>
#include <eekernel.h>
#include <stdio.h>

//#include "sound.h"

typedef struct
{
	ALuint mID;
	ALuint mPitch;
	char mName[16];
} SampleType;

struct ChannelStructure
{
	ALboolean	mUsed;
	SampleType *mSample;
	ALfloat		mVolume;
	ALboolean	mLooped;
};

extern ChannelStructure Channels[48];



#ifdef __cplusplus
extern "C" {
#endif
// Utility function to set the Error for OpenAL
void PS2AL_SetError( ALenum new_error );
ALuint PS2AL_UpdateSource( ALuint );

#ifdef __cplusplus
}
#endif

// Define ENABLE_PS2AL_DEBUG_MESSAGES here in order to turn on 
// debug logging for ps2al
//#define ENABLE_PS2AL_DEBUG_MESSAGES


#ifdef ENABLE_PS2AL_DEBUG_MESSAGES
#define PS2AL_DEBUGPRINT(format, args...) \
	printf(format, ## args)
#else
#define PS2AL_DEBUGPRINT(format, args...)
#endif

#endif //__AL_INTERNAL_H__
