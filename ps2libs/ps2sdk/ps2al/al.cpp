#include "al/al.h"
#include "al/al_internal.h"
#include "al_listener.h"

#include <stdlib.h>
#include <sifdev.h>
#include <eekernel.h>
#include <string.h>
#include <malloc.h>

#include "sound.h"
#define USE_SYSTEM_STDIO
#include "stdio_multistream.h"

//---------------------------------------------------------------------------------------------------------------------

/*void printf2(int channel, const char* fmt,...)
{
   va_list argptr;
   va_start(argptr, fmt);
   
	//output stream IOP TTY 1, outputing to other EE TTY is more difficult.
	//streams are tty0: to tty9: (I'm not aware of any others)
	 
	char text[256];
	sprintf(text, "tty%d:", channel);
	int handle	= sceOpen(text, SCE_WRONLY);
	int textlen	= vsnprintf(text, sizeof(text), fmt, argptr);
	sceWrite(handle, text, textlen); 
	sceWrite(handle, "\r\n", 2); 
	sceClose(handle);

	va_end(argptr);
}*/

//---------------------------------------------------------------------------------------------------------------------

ChannelStructure Channels[48];

ALushort LastSpotFXID=0x1000;

ALenum g_al_error = AL_NO_ERROR;

void PS2AL_SetError( ALenum new_error )
{
	if (g_al_error != AL_NO_ERROR)
		
	g_al_error = new_error;
}

ALenum alGetError( void )
{
	ALenum retvalue = g_al_error;
	g_al_error = AL_NO_ERROR;
	PS2AL_DEBUGPRINT("alGetError returned (%x)\n",retvalue);
	return retvalue;
}

ALboolean alIsBuffer( ALuint buffer )
{
	PS2AL_DEBUGPRINT("alIsBuffer checking buffer (%x) (%s)\n",buffer, buffer!=0?((SampleType*)buffer)->mName:"");
	if (buffer != 0)
	{
		return AL_TRUE;
	}
	else
	{
		return AL_FALSE;
	}
}

ALvoid alGenBuffers(ALsizei n,ALuint *puiBuffers)
{
	PS2AL_DEBUGPRINT("alGenBuffers generating (%d) buffers\n",n);

	int buffer;

	ms_init(NULL);

	for (buffer=0;buffer<n;++buffer)
	{
		puiBuffers[buffer]=(ALuint)malloc(sizeof(SampleType));
	}
}

ALvoid alDeleteBuffers(ALsizei n,ALuint *puiBuffers)
{

	PS2AL_DEBUGPRINT("alDeleteBuffers deleting (%d) buffers\n",n);
	int buffer;

	ms_init(NULL);

	for (buffer=0;buffer<n;++buffer)
	{
		SOUND_MemFreeUsingSFXID(((SampleType*)puiBuffers[buffer])->mID);
		SOUND_FreeSpotFXID(((SampleType*)puiBuffers[buffer])->mID);
		SOUND_FlushIOPCommand( 0 );
		free((SampleType*)puiBuffers[buffer]);
	}
}

#define SPU_XFER_BUFFER		0x4000
#define SPU_XFER_ALIGN(x)	((x+(SPU_XFER_BUFFER-1))&~SPU_XFER_BUFFER)

ALvoid alBufferData(ALuint buffer,ALenum format,ALvoid *data,ALsizei size,ALsizei freq)
{
	ALuint addr_IOP;
	int				DataToLoad = size;	//size passes in is already -48 as its read from the VAG
	unsigned int  SPUAddress;
	unsigned int  LoadSize;
	unsigned int  EEAddress=(unsigned int)data+48;

	PS2AL_DEBUGPRINT("alBufferData setting up buffer (%x) (%s) format (%x) data (%x) size (%x) freq (%d)\n",buffer,((SampleType*)buffer)->mName,format,data,size,freq);

	ms_init(NULL);

	((SampleType*)buffer)->mID=LastSpotFXID++;
	((SampleType*)buffer)->mPitch=freq;

	SPUAddress = SOUND_MemAllocate(size);
	if (!SPUAddress)
		return;

	addr_IOP	=	(ALuint)sceSifAllocIopHeap (SPU_XFER_BUFFER);

	SOUND_PatchSFX(((SampleType*)buffer)->mID, SPUAddress);

	while(DataToLoad > 0)
	{
		LoadSize = DataToLoad>SPU_XFER_BUFFER?SPU_XFER_BUFFER:DataToLoad;

		SOUND_TransferDataToIOP( EEAddress, addr_IOP, SPU_XFER_BUFFER, SOUND_DMA_WAIT );	//transfair full amount as this doesn't round up sizes to 64 bytes
		SOUND_TransIOPToSPU( addr_IOP, SPUAddress, LoadSize);								//this does round up size to 64 bytes, so LoadSize is ok
		SOUND_FlushIOPCommand( 0 );

    		DataToLoad -= LoadSize;
    		SPUAddress += LoadSize;
		EEAddress += LoadSize;
	}
	sceSifFreeIopHeap( (void*)addr_IOP );
}

ALvoid alGetSourcei(ALuint source,ALenum pname,ALint *value)
{
	
	ms_init(NULL);
	int status;

	if (pname==AL_SOURCE_STATE)
	{	
		status = SOUND_GetKeyStatus(source);
		if (status == 0)
			*value=AL_STOPPED;
		else
			*value=AL_PLAYING ;
	}

	PS2AL_DEBUGPRINT("alGetSourcei (%x) (%s) name (%x) value (%x)\n",source,Channels[source].mSample->mName,pname,*value);
}


ALvoid alGenSources(ALsizei n,ALuint *sources)
{
	int source,channel;
	PS2AL_DEBUGPRINT("alGenSources  number(%x) \n",n);

	ms_init(NULL);

	for (source=0;source<n;++source)
		for (channel=0;channel<48;channel++)
			if (!Channels[channel].mUsed)
			{
				Channels[channel].mUsed=AL_TRUE;
				sources[source]=channel;
				break;
			}
}

ALvoid alDeleteSources(ALsizei n,ALuint *sources)
{
	PS2AL_DEBUGPRINT("alDeleteSources  number(%x) \n",n);

	int source;

	ms_init(NULL);

	for (source=0;source<n;++source)
		Channels[sources[source]].mUsed=AL_FALSE;
}

ALvoid alSourcei(ALuint source,ALenum pname,ALint value)
{

	ms_init(NULL);

	if (pname==AL_BUFFER)
		Channels[source].mSample=(SampleType*)value;
	if (pname==AL_LOOPING)
		Channels[source].mLooped=value;

	PS2AL_UpdateSource(source);
	
	PS2AL_DEBUGPRINT("alSourcei source (%x) name(%s) pname (%x) value(%x) \n",source,Channels[source].mSample->mName,pname,value);
	
}

void alSourcefv( ALuint source, ALenum param, const ALfloat* values )
{
	alSource3f( source, param, values[0], values[1], values[2]);
}

ALvoid alSourcef(ALuint source,ALenum pname,ALfloat value)
{
	alSource3f( source, pname, value, 0.0f, 0.0f);
}

void alSource3f( ALuint source, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 )
{
	PS2AL_DEBUGPRINT("alSource3f source (%x)(%s) pname (%x) value1(%f) value2(%f) value3(%f) \n",source,Channels[source].mSample->mName,param,value1,value2, value3);
	int update_source_flag = 0;
	ms_init(NULL);

	switch (param)
	{
		case AL_GAIN:
		{
			if (value1 == Channels[source].mVolume)
				break;
			
			Channels[source].mVolume=value1;
			update_source_flag = 1;
			
			break;
		}
		default:
			break;
	}
	
	if (update_source_flag != 0)
	{
		PS2AL_UpdateSource(source);
		update_source_flag = 0;
	}

}

ALvoid alSourcePlay(ALuint source)
{

	ms_init(NULL);

	if (Channels[source].mSample == NULL)
		return;		//+ddp - crash fix

	if (Channels[source].mLooped == AL_TRUE)
	{
	PS2AL_DEBUGPRINT("alSourcePlay source looped (%x) (%s) \n",source,Channels[source].mSample->mName);
		SOUND_PlaySFXLoop(Channels[source].mSample->mID, source, 
						(int)(Channels[source].mVolume*(float)0x3fff), 
						(int)(Channels[source].mVolume*(float)0x3fff),
						SOUND_HzToSPU(Channels[source].mSample->mPitch),0,0,
						0);

						
	}
	else
	{
	
	PS2AL_DEBUGPRINT("alSourcePlay source non-looped (%x) (%s) \n",source,Channels[source].mSample->mName);
		SOUND_PlaySFX(Channels[source].mSample->mID, source, 
						(int)(Channels[source].mVolume*(float)0x3fff), 
						(int)(Channels[source].mVolume*(float)0x3fff),
						SOUND_HzToSPU(Channels[source].mSample->mPitch),0,0);
	}

	SOUND_FlushIOPCommand(0);		

}

ALvoid alSourceStop(ALuint source)
{
	PS2AL_DEBUGPRINT("alSourceStop source (%x)(%s)\n",source,Channels[source].mSample->mName);
	ms_init(NULL);
	SOUND_StopSound(source);
    	SOUND_FlushIOPCommand(0);
}



ALubyte* alGetString( ALenum param )
{
	PS2AL_DEBUGPRINT("alGetString param(%x)\n",param);
	return (ALubyte*)"";
}




void alListenerfv( ALenum param, const ALfloat* values )
{
	PS2AL_DEBUGPRINT("alListenerfv param(%x) value1(%f) value2 (%f) value3 (%f)\n",param,values[0], values[1], values[2]);
	CListener::GetListener().SetValue(param,values[0], values[1], values[2]);
	
}

void alListenerf( ALenum param, ALfloat value )
{
	PS2AL_DEBUGPRINT("alListenerf param(%x) value1(%f) n",param,value);
	CListener::GetListener().SetValue(param, value, 0.0f, 0.0f);
}

void alListener3f( ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 )
{
	PS2AL_DEBUGPRINT("alListener3f param(%x) value1(%f) value2 (%f) value3 (%f)\n",param,value1, value2, value3);
	CListener::GetListener().SetValue(param,value1, value2, value3);
}

void alGetListener3f( ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3 )
{
	CListener::GetListener().GetValue(param,value1, value2, value3);
	PS2AL_DEBUGPRINT("alGetListener3f param(%x) value1(%f) value2 (%f) value3 (%f)\n",param,*value1, *value2, *value3);
}
void alGetListenerf( ALenum param, ALfloat* value )
{
	PS2AL_DEBUGPRINT("alGetListenerf param(%x) value1(%f) \n",param,*value);
	ALfloat temp1, temp2;
	CListener::GetListener().GetValue(param, value, &temp1, &temp2);
}
void alGetListeneri( ALenum param, ALint* value )
{
	CListener::GetListener().GetValue(param, value, 0, 0);
	PS2AL_DEBUGPRINT("alGetListeneri param(%x) value1(%x) \n",param,*value);
}

//-----------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------

#define DATACHAN	1
#define MXAUDCHAN1	2 
#define MXAUDCHAN2	3
#define SPAUDCHAN1	4
#define SPAUDCHAN2	5

//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	char	*Filename;
	u_int	Data;
	u_int	FileID;
} STRMHEADER;

int			MXStream_FileID		= -1;
int			SPStream_FileID		= -1;
u_short		MXStream_SPUChan		= 46;
u_short		SPStream_SPUChan		= 44;
u_int		MXStream_SPUBuffer		=  0;
u_int		SPStream_SPUBuffer		=  0;
int			Stream_HeadersIndex		= 0;
STRMHEADER	Stream_Headers[256];

extern void BuildPS2MIBName(const char *filename, char *MIB);

//-----------------------------------------------------------------------------------------------------------------------

void PS2AL_PreloadHeader(ALuint Data, char *Filename)
{
	ALuint	loaded = PS2AL_GetHeader(Filename);
	if (!loaded)
	{
		char	MIB[256];

		BuildPS2MIBName(Filename, MIB);

		Stream_Headers[Stream_HeadersIndex].Filename	= Filename;
		Stream_Headers[Stream_HeadersIndex].Data		= (u_int)Data;
		Stream_Headers[Stream_HeadersIndex].FileID		= (u_int)SOUND_CreateFileInfo(SOUND_CREATE_FILE,MIB,0);
		SOUND_FlushIOPCommand(0);												// WAIT FOR TRANSFER REQUEST TO FINISH

		Stream_HeadersIndex++;
	}
}

//-----------------------------------------------------------------------------------------------------------------------

ALuint PS2AL_GetHeader(char *Filename)
{
	for (int i=0; i<Stream_HeadersIndex; i++)
	{
		if (stricmp(Stream_Headers[i].Filename, Filename)==0)
		{
			return(Stream_Headers[i].Data);
		}
	}
	return(0);
}

//-----------------------------------------------------------------------------------------------------------------------

ALuint PS2AL_GetFileID(char *Filename)
{
	for (int i=0; i<Stream_HeadersIndex; i++)
	{
		if (stricmp(Stream_Headers[i].Filename, Filename)==0)
		{
			return(Stream_Headers[i].FileID);
		}
	}
	return(0);
}

//-----------------------------------------------------------------------------------------------------------------------

ALuint PS2AL_GetNumHeaders(void)
{
	return(Stream_HeadersIndex);
}

//-----------------------------------------------------------------------------------------------------------------------

char *PS2AL_GetHeaderName(ALuint index)
{
	return(Stream_Headers[index].Filename);
}

//-----------------------------------------------------------------------------------------------------------------------

void PS2AL_StopMusic(void)
{
	//already streaming... closedown before starting next
	if (MXStream_FileID != -1)
	{
		SOUND_CloseStreamBuffer(MXAUDCHAN1);					// Close stream
		SOUND_MemFree(MXStream_SPUBuffer);
		SOUND_FlushIOPCommand(0);

		MXStream_FileID = -1;
	}
}

//-----------------------------------------------------------------------------------------------------------------------

void PS2AL_PlayMusic(ALuint HeaderInfo, ALuint FileID, ALuint Volume)
{
	u_int				NumberOfFiles;									// Number of interleaved files in Body.
	u_int				BufferSize;
	SOUND_MIH_INFO		buffer;
	u_short				Pitch;
	u_int				TrackSize;										// Size of one data track from Body file (containing 1 interleaved data packet)

	if (FileID == (ALuint)MXStream_FileID)
		return;

	PS2AL_StopMusic();
	SOUND_GetMIHInfo(HeaderInfo,&buffer,0);								// As it says - Get relevent info about the .MIH file.

	MXStream_FileID		= FileID;
	TrackSize			= buffer.TrackSize;									// Size of a single track (one half of a buffer for one stream)
	MXStream_SPUBuffer= SOUND_MemAllocate(TrackSize*2);
	NumberOfFiles	= buffer.NumberOfFiles;								// Number of interleaved streams in .MIB file
	Pitch			= (u_short)SOUND_HzToSPU(buffer.Pitch);				// Pitch stored as hz. Convert to SPU value.
	BufferSize		= NumberOfFiles*TrackSize*2;						// HalfBuffer size *2 = full IOP buffer size required.

	if (SOUND_AllocateStreamBuffer(MXAUDCHAN1,MXStream_SPUBuffer,BufferSize)==0)	// Sets IOP and SPU buffer size to BufferSize
	{
		if (SOUND_ResizeSPUBuffer(MXAUDCHAN1,TrackSize)==0)							// Resize SPU buffer to TrackSize
		{
			if (SOUND_PlayStream(FileID,MXAUDCHAN1,MXStream_SPUChan,Volume,0,Pitch,STREAM_LOOPING,0,0)!=-1)
			{
				SOUND_SetStreamParent_Int(MXAUDCHAN1,0,TrackSize);					// Parent Stream, track to play, track size
				SOUND_SetStreamChild_Int(MXAUDCHAN2,MXAUDCHAN1,MXStream_SPUChan+1,1,0,Volume,MXStream_SPUBuffer+TrackSize);
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------------------------------

void PS2AL_StopSpeech(void)
{
	//already streaming... closedown before starting next
	if (SPStream_FileID != -1)
	{
		SOUND_CloseStreamBuffer(SPAUDCHAN1);					// Close stream
		SOUND_MemFree(SPStream_SPUBuffer);
		SOUND_FlushIOPCommand(0);

		SPStream_FileID = -1;
	}
}

//-----------------------------------------------------------------------------------------------------------------------

void PS2AL_PlaySpeech(ALuint HeaderInfo, ALuint FileID, ALuint Volume)
{
	u_int				NumberOfFiles;									// Number of interleaved files in Body.
	u_int				BufferSize;
	SOUND_MIH_INFO		buffer;
	u_short				Pitch;
	u_int				TrackSize;										// Size of one data track from Body file (containing 1 interleaved data packet)

	PS2AL_StopSpeech();

	SOUND_GetMIHInfo(HeaderInfo,&buffer,0);								// As it says - Get relevent info about the .MIH file.

	SPStream_FileID		= FileID;
	TrackSize			= buffer.TrackSize;									// Size of a single track (one half of a buffer for one stream)
	SPStream_SPUBuffer= SOUND_MemAllocate(TrackSize*2);
	NumberOfFiles	= buffer.NumberOfFiles;								// Number of interleaved streams in .MIB file
	Pitch			= (u_short)SOUND_HzToSPU(buffer.Pitch);				// Pitch stored as hz. Convert to SPU value.
	BufferSize		= NumberOfFiles*TrackSize*2;						// HalfBuffer size *2 = full IOP buffer size required.

	if (SOUND_AllocateStreamBuffer(SPAUDCHAN1,SPStream_SPUBuffer,BufferSize)==0)	// Sets IOP and SPU buffer size to BufferSize
	{
		if (SOUND_ResizeSPUBuffer(SPAUDCHAN1,TrackSize)==0)							// Resize SPU buffer to TrackSize
		{
			if (SOUND_PlayStream(FileID,SPAUDCHAN1,SPStream_SPUChan,Volume,0,Pitch,STREAM_ONCE_OFF,0,0)!=-1)
			{
				SOUND_SetStreamParent_Int(SPAUDCHAN1,0,TrackSize);					// Parent Stream, track to play, track size
				SOUND_SetStreamChild_Int(SPAUDCHAN2,SPAUDCHAN1,SPStream_SPUChan+1,1,0,Volume,SPStream_SPUBuffer+TrackSize);
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------------------------------

static void CheckForExternalLoading(void)
{
	SOUND_EXTERNAL_INFO		EXT_Info;
	int						a;

	a=SOUND_ReadyToLoad();
	if (a==-1)						// Any load requested?
	{
		SOUND_HandleCDErrors();		// No..Its safe to check for CD errors
		return;						// No..
	}
	else if (a==SND_LOAD_INTERNAL)
	{
		a=SOUND_LoadComplete(&EXT_Info);
		if (a!=-1)
		{
			SOUND_GetStatus((int)EXT_Info.StreamRequest,(int)EXT_Info.RequestCount);
		}
		SOUND_HandleCDErrors();
	}
}

//-----------------------------------------------------------------------------------------------------------------------

#include <sifrpc.h>

void PS2AL_Update(void)
{
		CheckForExternalLoading();
		SOUND_FlushIOPCommand(SIF_RPCM_NOWAIT);			// Transfer EE command list to IOP buffer
}

//-----------------------------------------------------------------------------------------------------------------------



