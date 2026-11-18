

#include "al/al.h"
#include "al/alut.h"
#define USE_SYSTEM_STDIO

#include "stdio_multistream.h"
#include "al/al_internal.h"


/* Source Functions */

void alGetSourcefv( ALuint source, ALenum param, ALfloat* values )
{
	PS2AL_DEBUGPRINT("alGetSourcefv STUB source (%x)(%s) param (%x) value1 (%f) value2(%f) value3(%f)\n",source,Channels[source].mSample->mName,param,values[0], values[1], values[2]);

}

void alGetSourcef( ALuint source, ALenum param, ALfloat* value )
{
	PS2AL_DEBUGPRINT("alGetSourcef STUB source (%x) (%s) param (%x) value(%f)\n",source,Channels[source].mSample->mName,param,*value);
}


ALvoid alSourceQueueBuffers( ALuint source, ALsizei n, ALuint* buffers )
{
	PS2AL_DEBUGPRINT("alSourceQueueBuffers STUB source (%x)(%s) number(%x) \n",source,Channels[source].mSample->mName,n);
}

ALvoid alSourceUnqueueBuffers( ALuint source, ALsizei n, ALuint* buffers )
{
	PS2AL_DEBUGPRINT("alSourceUnqueueBuffers STUB source (%x) (%s) number(%x) \n",source,Channels[source].mSample->mName,n);
}




void alEnable( ALenum capability )
{
	PS2AL_DEBUGPRINT("alEnable STUB cap (%x)\n",capability);
}
void alDistanceModel( ALenum distanceModel )
{
	PS2AL_DEBUGPRINT("alDistanceModel STUB cap (%x)\n",distanceModel);
}
void alGetBufferi( ALuint bid, ALenum param, ALint* value )
{
	PS2AL_DEBUGPRINT("alGetSourcei STUB bid (%x) param (%x) value(%x)\n",bid,param,*value);
}

