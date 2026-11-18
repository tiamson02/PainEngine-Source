#ifndef __AL_H_
#define __AL_H_

typedef char ALboolean;
typedef signed char ALbyte;
typedef unsigned char ALubyte;
typedef short ALshort;
typedef unsigned short ALushort;
typedef unsigned int ALuint;
typedef int ALint;
typedef float ALfloat;
typedef signed int ALsizei;
typedef void ALvoid;
typedef int ALenum;

#define AL_TRUE                                   1
#define AL_FALSE                                  0
#define AL_LOOPING                                0x1007
#define AL_BUFFER                                 0x1009
#define AL_GAIN                                   0x100A
#define AL_SOURCE_STATE                           0x1010
#define AL_INITIAL                                0x1011
#define AL_PLAYING                                0x1012
#define AL_PAUSED                                 0x1013
#define AL_STOPPED                                0x1014



/* Start additions for Torque */
#define AL_CONE_INNER_ANGLE                       0x1001
#define AL_CONE_OUTER_ANGLE                       0x1002
#define AL_PITCH                                  0x1003
#define AL_POSITION                               0x1004
#define AL_DIRECTION                              0x1005
#define AL_VELOCITY                               0x1006
#define AL_ORIENTATION                            0x100F
#define AL_BUFFERS_QUEUED                        0x1015
#define AL_BUFFERS_PROCESSED                     0x1016
#define AL_REFERENCE_DISTANCE                     0x1020
#define AL_CONE_OUTER_GAIN                        0x1022
#define AL_MAX_DISTANCE                           0x1023

#define AL_SOURCE_RELATIVE                        0x202

#define AL_INVALID                                (-1)
#define AL_NONE                                  0
#define AL_FALSE                                 0
#define AL_TRUE                                  1
#define AL_NO_ERROR                              AL_FALSE

#define AL_FREQUENCY                              0x2001
#define AL_BITS                                   0x2002
#define AL_CHANNELS                               0x2003
#define AL_SIZE                                   0x2004
#define AL_DISTANCE_MODEL                         0xD000
#define AL_INVERSE_DISTANCE                       0xD001

#define AL_FORMAT_MONO8                          0x1100
#define AL_FORMAT_MONO16                         0x1101
#define AL_FORMAT_STEREO8                        0x1102
#define AL_FORMAT_STEREO16                       0x1103

#define AL_VENDOR                                0xB001
#define AL_VERSION                               0xB002
#define AL_RENDERER                              0xB003
#define AL_EXTENSIONS                            0xB004



/* End additons for Torque */



#ifdef __cplusplus
extern "C" 
{
#endif

void  alGenSources( ALsizei n, ALuint* sources ); 
void  alDeleteSources( ALsizei n, ALuint* sources );
void  alSourcei( ALuint sid, ALenum param, ALint value ); 
void  alSourcef( ALuint sid, ALenum param, ALfloat value ); 
void  alSourcePlay( ALuint sid );
void  alSourceStop( ALuint sid );
void  alGenBuffers( ALsizei n, ALuint* buffers );
void  alDeleteBuffers( ALsizei n, ALuint* buffers );
void  alBufferData( ALuint   buffer,ALenum format,ALvoid* data,ALsizei  size, ALsizei freq );
void  alGetSourcei( ALuint sid, ALenum pname, ALint* value );

/* Start additions for Torque */
void alListenerf( ALenum param, ALfloat value );
void alListener3f( ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 );
ALenum alGetError( void );
void alSource3f( ALuint sid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 );
void alGetListener3f( ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3 );
void alGetListenerf( ALenum param, ALfloat* value );
void alEnable( ALenum capability );
void alDistanceModel( ALenum distanceModel );
void alGetSourcefv( ALuint sid, ALenum param, ALfloat* values );
void alGetSourcef( ALuint sid, ALenum param, ALfloat* value );
void alSourcefv( ALuint sid, ALenum param, const ALfloat* values );
void alListenerfv( ALenum param, const ALfloat* values );
void alGetBufferi( ALuint bid, ALenum param, ALint* value );
ALboolean alIsBuffer( ALuint buffer );
ALubyte* alGetString( ALenum param );
void alGetListeneri( ALenum param, ALint* value );
ALvoid alSourceQueueBuffers( ALuint source, ALsizei n, ALuint* buffers );
ALvoid alSourceUnqueueBuffers( ALuint source, ALsizei n, ALuint* buffers );

//ps2 multistream functions
void PS2AL_Update(void);
ALuint PS2AL_GetHeader(char *Filename);
ALuint PS2AL_GetFileID(char *Filename);
void PS2AL_PreloadHeader(ALuint Data, char *Filename);
void PS2AL_PlayMusic(ALuint HeaderInfo, ALuint FileID, ALuint Volume=0x3fff);
void PS2AL_StopMusic(void);
void PS2AL_PlaySpeech(ALuint HeaderInfo, ALuint FileID, ALuint Volume=0x3fff);
void PS2AL_StopSpeech(void);
ALuint PS2AL_GetNumHeaders(void);
char *PS2AL_GetHeaderName(ALuint index);

/* End additons for Torque */


#ifdef __cplusplus
}  /* extern "C" */
#endif



#endif /* __AL_H_ */
