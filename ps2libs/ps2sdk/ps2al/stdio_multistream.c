
#include <stdlib.h>
#include <sifdev.h>
#include <eekernel.h>
#include <string.h>
#include <stdarg.h>
#define STDIO_MS_IMPLEMENTATION
#include "stdio_multistream.h"
#include "sound.h"
#include "shellCore.h"
#include "shellFile.h"

static unsigned char TempReadBuffer[64] __attribute__((aligned (64)));

static unsigned char StackBuffer[32*1024] __attribute__((aligned (64)));

static int MultiStreamedInitialized=0;

#define STREAM_DATA_BUFFER_SIZE (32*1024)
#define STREAM_MAX 10

char DevicePrefix[256]="";

char *ms_getdeviceprefix()
{
	return(DevicePrefix);
}

void ms_init(char *modulepath)
{
	if (!MultiStreamedInitialized)
	{
		char *pos=0;
		char ModuleName[256];
		void *largememory;

		MultiStreamedInitialized=1;
#if 0
		strcpy(DevicePrefix,modulepath);
		if ((pos=strchr(DevicePrefix,','))) // atwinmon
			*(pos+1)=0;
		else if ((pos=strchr(DevicePrefix,':'))) // host or disk
			*(pos+1)=0;
/*
		largememory=malloc(40*1024*1024);
		if (largememory) // devkit
		{
			free(largememory);
			strcpy(ModuleName,modulepath);
			strcat(ModuleName,"/SulphaSound.irx");
			if (sceSifLoadModule (ModuleName, 0, NULL)<0)
				exit(0);

			strcpy(ModuleName,modulepath);
			strcat(ModuleName,"/SulphaComm.irx");
		}
*/
		strcpy(ModuleName,modulepath);
		strcat(ModuleName,"/libsd.irx");
		if (sceSifLoadModule (ModuleName, 0, NULL)<0)
			exit(0);
		strcpy(ModuleName,modulepath);
		strcat(ModuleName,"/stream.irx");
		if (sceSifLoadModule (ModuleName, 0, NULL)<0)
			exit(0);
#else
		shellFileLoadStartModule("libsd", 0, "", NULL);
		shellFileLoadStartModule("stream", 0, "", NULL);
#endif


		if( SOUND_InitIOP () < 0 )
			return;

		if(sceSifInitIopHeap() < 0 )
			return;

		SOUND_InitSPU();
		SOUND_InitCD( SOUND_SET_CALLBACK );		// Only required if using MultiStream cd loading routines, this doesn't reinit the whole CD subsystem with SOUND_SET_CALLBACK as a param
		
		// Use MultiStream CD/Host loading routines. Allocate 8 file slots and 0 SFX slots (we don't know how many we need yet)
		SOUND_InitStreamData( SND_LOAD_INTERNAL, FOPEN_MAX, 0 );
		SOUND_SetMaxStreamLimit( STREAM_MAX );			// Maximum of number of streams playing (nb one for parent, one for child)

		ChangeThreadPriority(GetThreadId(),10);
		SOUND_InitFastLoad(9,(int)StackBuffer,32*1024); // thread priority=9 (less than 10)
		SOUND_SetFastLoadMode( SOUND_FASTLOAD_INVALID_DCACHE_ON );
		SOUND_SetFastLoadMode( SOUND_FASTLOAD_ON );
		
		SOUND_AllocateStreamBuffer(0,SOUND_DATA_STREAM,STREAM_DATA_BUFFER_SIZE);	// Stream Number, SPU Address, Size

		FlushIOPCommand(0);		// Wait for the above SOUND_ commands to be processed before continuing.
	}
}

ms_FILE * ms_fopen(const char *FileName, const char *Mode)
{
	unsigned int FileID;
	SOUND_FILE_INFO FileInfo;
	ms_FILE *ret;
	char FullFilename[256];

	ms_init(NULL);
	strcpy(FullFilename,DevicePrefix);
	strcat(FullFilename,FileName);
	// should probably check for the 'r' mode
	FileID=SOUND_GetNextFileIDCounter();
	SOUND_CreateFileInfo(FileID,FullFilename,0);		// Create file info for filename					
	SOUND_FlushIOPCommand(0);						
	SOUND_ReturnFileInfo(&FileInfo);		
	if ((FileInfo.Source==0)&&(FileInfo.LSN==0)&&(FileInfo.Size==0))
	{
		return NULL;
	}

	if (strnicmp(ms_getdeviceprefix(),"atfile:",7)==0) // atwinmon
		SOUND_ATWinMonOpenFile(FileID);

	ret=(ms_FILE *)malloc(sizeof(ms_FILE));
	ret->mID = FileID;
	ret->mPosition=0;
	ret->mSize=FileInfo.Size;
	return ret;
}

unsigned int ms_freadaligned(ms_FILE* File,unsigned int Address,int Count)
{
	ms_init(NULL);
	SOUND_SetFileOffsetAndPlaySize(File->mID,File->mPosition,Count);
	InvalidDCache((void*)Address,(void*)(Address+Count-1));
	SOUND_LoadFile(STREAM_LOAD_EE,0,File->mID,Address);	// Load file into EE RAM
	SOUND_FlushIOPCommand(0);			// Transfer EE command list to IOP buffer
	SOUND_WaitForFileToLoad(0,SOUND_WAIT_COLD);		// Wait for file to load
	SOUND_FlushIOPCommand(0);
	File->mPosition+=Count;
	return(Count);
}

unsigned int ms_fread(void *buf, unsigned int Size1, unsigned int Size2, ms_FILE *file)
{	
	unsigned int Size=Size1*Size2;
	unsigned int Address=(unsigned int)buf;
	unsigned int BytesRead=Size;
	int BytesAtEnd;

	ms_init(NULL);
	
	if (file->mPosition+Size>file->mSize)
		Size=file->mSize-file->mPosition;
	
	// Lets split any read requests into possibly 3 aligned reads.
	if (Address&(64-1)) // not 64 byte aligned
	{
		unsigned int BytesToRead=64-(Address&(64-1));
		if (BytesToRead>Size)
			BytesToRead=Size;
		ms_freadaligned(file,(unsigned int)TempReadBuffer,BytesToRead);
		memcpy((void*)Address,TempReadBuffer,BytesToRead);
		Size-=BytesToRead;
		Address+=BytesToRead;
	}	
	BytesAtEnd=Size&(64-1);
	Size&=~(64-1);
	if (Size>0)
	{
		ms_freadaligned(file,Address,Size);
		Address+=Size;
	}
	if (BytesAtEnd) // not 64 byte aligned
	{
		ms_freadaligned(file,(unsigned int)TempReadBuffer,BytesAtEnd);
		memcpy((void*)Address,TempReadBuffer,BytesAtEnd);
	}
	return(BytesRead);
}

unsigned int	ms_fwrite(void *buffer, unsigned int size1, unsigned int size2, ms_FILE *fp)
{
	return(0);
}

int ms_fseek(ms_FILE * file, long offset, int mode)
{
	ms_init(NULL);
	switch (mode)
	{
	case SEEK_SET:
		file->mPosition=offset;
		break;
	case SEEK_CUR:
		file->mPosition+=offset;
		break;
	case SEEK_END:
		file->mPosition=file->mSize+offset;
		break;
	};

	return file->mPosition;
}


long ms_ftell(ms_FILE *file)
{
	ms_init(NULL);
	return file->mPosition;
}


int ms_fclose(ms_FILE *file)
{
	ms_init(NULL);
	// close the file

	if (strnicmp(ms_getdeviceprefix(),"atfile:",7)==0) // atwinmon
		SOUND_ATWinMonCloseFile(file->mID);

	SOUND_FreeFileID(file->mID);			// Free file ID
	SOUND_FlushIOPCommand(0);

	// free the resources
	free(file);
	return(0);
}


void ms_printf(const char *msg,...)
{   
    char        s[256];
    va_list     valist;
    va_start(valist,msg );
    vsprintf(s, msg, valist);
    va_end(valist);
	if (strnicmp(ms_getdeviceprefix(),"atfile:",7)==0) // atwinmon
	{
		static int AtwinmonTraceFileid=-1;
		if (AtwinmonTraceFileid==-1)
		{
			if (strlen(ms_getdeviceprefix())>0)
			{
				char Filename[256];
				strcpy(Filename,ms_getdeviceprefix());
				strcat(Filename,"Output.txt");
				AtwinmonTraceFileid = sceOpen(Filename,SCE_WRONLY|SCE_TRUNC|SCE_CREAT,0777);
			}
		}
		if (AtwinmonTraceFileid!=-1)
		{
			char *pos,*s2=s;
			while((pos=strchr(s2,'\n')))
			{
				*pos=0;
				sceWrite(AtwinmonTraceFileid,s2, strlen(s2));
				sceWrite(AtwinmonTraceFileid,"\r\n", 2);
				s2=pos+1;
			}
			sceWrite(AtwinmonTraceFileid,s2, strlen(s2));
		}
	}
	else
		scePrintf("%s",s); // probably a PS2 devkit
}

ms_FILE*		ms_stderr=(ms_FILE*)1;

void			ms_fprintf(ms_FILE *fp ,const char *msg,...)
{
	char        s[256];
    va_list     valist;
    va_start(valist,msg );
    vsprintf(s, msg, valist);
    va_end(valist);
	if (fp==ms_stderr)
		ms_printf("%s",s);
	// cant write in multistream
}

