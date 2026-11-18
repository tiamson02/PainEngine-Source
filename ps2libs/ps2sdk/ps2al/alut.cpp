#include <stdlib.h>
#include <eekernel.h>
#include <string.h>

#include "al/alut.h"
#include "sound.h"

#ifdef USE_STDIO_ZIP
#include "stdio-zip.h"
#else
#include "stdio_multistream.h"
#endif

#define MAX_SOUNDS 30


void alutInit(int *argc, char **argv)
{
	char *module_path="";
	int i;
	if (argc)
	{
		for (i = 0; i < *argc; i++ ) 
		{
			if ( strstr(argv[i], "iop_module_path=") == argv[i] )
				module_path = &argv[i][strlen("iop_module_path=")];
		}
	}
	ms_init(module_path);
	SOUND_AllocateSpotFXMemory(MAX_SOUNDS); 
	SOUND_MemAllocInit(MAX_SOUNDS,SOUND_ALLOCATE_ALL_RAM); // 2 effect buffers too
}

ALvoid alutExit(ALvoid) 
{
	ms_init(NULL);
	SOUND_MemAllocClose();
}

ALvoid alutLoadWAVFile(ALbyte *file,ALenum *format,ALvoid **data,ALsizei *size,ALsizei *freq, ALboolean *loop)
{
	FILE *fp;
	char *pos;

	ms_init(NULL);

	if ((pos=strstr((char*)file,".wav")))
		strcpy(pos,".vag");
	if ((pos=strstr((char*)file,".WAV")))
		strcpy(pos,".vag");
	fp=fopen((char*)file,"rb");
	if (fp!=NULL)
	{
		int Filesize=fseek(fp,0,SEEK_END);
		char *TempBuffer=(char*)malloc(Filesize);
		fseek(fp,0,SEEK_SET);
		fread(TempBuffer,1,Filesize,fp);
		fclose(fp);
		alutLoadWAVMemory((ALbyte*)TempBuffer,format,data,size,freq,loop);
		free(TempBuffer);
	}
}

typedef struct 
{
	char Tag[4];
	unsigned int Version;
	unsigned int Reserved1;
	unsigned char DataSize[4];
	unsigned char Pitch[4];
	unsigned int Reserved2[3];
	unsigned char filename[16];
	unsigned int Reserved3[2];
}VAGHeader;



ALvoid alutLoadWAVMemory(ALbyte *memory,ALenum *format,ALvoid **data,ALsizei *size,ALsizei *freq,ALboolean *loop)
{
	VAGHeader *header;

	ms_init(NULL);

	header=(VAGHeader *)memory;
	if (strcmp(header->Tag,"VAGp")==0)
	{
		*freq=header->Pitch[3]+header->Pitch[2]*256+header->Pitch[1]*256*256+header->Pitch[0]*256*256*256;
		*size=header->DataSize[3]+header->DataSize[2]*256+header->DataSize[1]*256*256+header->DataSize[0]*256*256*256+48;
		*data=(void*)malloc(*size);
		memcpy(*data,memory,*size);
	}
}


ALvoid alutUnloadWAV(ALenum format,ALvoid *data,ALsizei size,ALsizei freq)
{
	ms_init(NULL);

	if (data)
		free(data);
}
