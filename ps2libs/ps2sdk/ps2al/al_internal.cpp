
#include "al/al.h"
#include "al/al_internal.h"
#include "sound.h"

ALuint PS2AL_UpdateSource( ALuint source)
{
	ALuint retval = SOUND_SetChannelVolume(source, (int)(Channels[source].mVolume*0x3fff),(int)(Channels[source].mVolume*0x3fff) );
	return retval;

}
