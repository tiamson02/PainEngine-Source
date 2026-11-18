/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*/
//**************************************************************************
//	FILE			:	tgPSMF.cpp
//
//	Project  		:	
//
//	By		 		:	Mike Kavallierou
//
//	Function 		:	Implementation file for PSMF class.
//						
//	Comments		:	See header file for details on usage
//									
//	Date	 		:	04 / 03 / 05
//
//**************************************************************************
//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------
#ifdef __MWERKS__
	#pragma mark INCLUDES
#endif

#include <malloc.h>
#include <string.h>

#include "tgAssert.h"
#include "tgPSMF.h"

#include "strfile.h"

//-----------------------------------------------------------------------------
// M A C R O S 
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------
#ifdef __MWERKS__
	#pragma mark -
	#pragma mark TYPEDEFS
#endif

//-----------------------------------------------------------------------------
// F O R W A R D   D E C L A R A T I O N S
//-----------------------------------------------------------------------------
#ifdef __MWERKS__
	#pragma mark -
	#pragma mark FORWARD DECLARATIONS
#endif

//-----------------------------------------------------------------------------
// C O N S T A N T S
//-----------------------------------------------------------------------------
#ifdef __MWERKS__
	#pragma mark -
	#pragma mark CONSTANTS
#endif

//-----------------------------------------------------------------------------
// P U B L I C   F U N C T I O N S
//-----------------------------------------------------------------------------
#ifdef __MWERKS__
	#pragma mark -
	#pragma mark PUBLIC FUNCTIONS:
#endif

// Description
// Constructor, currently does nothing
//
// Parameters
//
// Returns
//
CPSMF::CPSMF()
{
	// TG_PRINT( "PSMF() Constructor" );
}

// Description
// Destructor, frees any memory allocated during usage
//
// Parameters
//
// Returns
//
CPSMF::~CPSMF()
{
	// TG_PRINT( "PSMF() Destructor" );
	TG_SAFE_FREE( m_pHeaderMemoryBuffer );
	return;
}

// Description
// Clears all variables and memory
//
// Parameters
//
// Returns
//
void CPSMF::Init( void )
{
	
	memset( &m_PsmfManager, 0, sizeof( ScePsmf ) );

	TG_SAFE_FREE( m_pHeaderMemoryBuffer );
	m_iStreamOffset				= 0;
	m_iStreamSize				= 0;
	m_uiStartTime				= 0;
	m_uiEndTime					= 0;
	m_iNumberOfStreams			= 0;
	m_iNumberOfChannels			= 0;
	m_iNumberOfVideoChannels	= 0;
	m_iNumberOfPCMChannels		= 0;
	m_iNumberOfAtracChannels	= 0;

	return;
}

// Description
// Initialises data for holding .PMF file details, setup the PSMF header and copies header
// data into header buffer ready for querying
//
// Parameters
// cMemoryAddress - pointer to memory address where PSMF file is loaded into
//
// Returns
//
int CPSMF::LoadPSMFHeaderFromMemory( const char *cMemoryAddress )
{
	// clear all values
	CPSMF::Init();

	// Parse header for header size
	if( SetPSMFHeaderBuffer( cMemoryAddress ) == TG_FAIL )
		return TG_FAIL;

	// copy header data to internal header buffer
	memcpy( m_pHeaderMemoryBuffer, cMemoryAddress, m_iStreamOffset );

	// extract all relevent details
	if( SetPSMFDetails() == TG_FAIL )
		return TG_FAIL;

	return TG_SUCCESS;
}

// Description
// Reads in the first sector of a PSMF file and allocates enough space for the PSMF header, also gets 
// the starting offset for the data stream itself, (past the header information inthe .PMF file) and
// gets the streams data size
//
// TODO
// Currently using simple file handling, this needs to be moved to tgFile() or equivalent
//
// Parameters
// szFilename - pointer to filename to load PSMF data from
//
// Returns
// TG_SUCCESS or TG_FAIL
//
int CPSMF::LoadPSMFHeaderFromFile( const char *szFilename )
{

	SceChar8 cFileBuffer[2048];	// temporary buffer to load PSMF header into
	SceInt32 iReadSize;			// read size from strFileRead function

	StrFile streamFile;			// file reference to pass to strfile() functions

	// initialise and clear all class member variables
	CPSMF::Init();

	// open file
	if ( strFileAssign( &streamFile, szFilename ) < 0 )
	{
		printf ( "strFileAssign() failed (%s)\n", szFilename );
		return TG_FAIL;
	}

	if ( strFileOpen( &streamFile, szFilename, SCE_O_RDONLY ) < 0 )
	{
		printf ( "strFileOpen() failed (%s)\n", szFilename );
		return TG_FAIL;
	}

	if( strFileLseek( &streamFile, 0L, SEEK_SET ) < 0 )
	{
		printf ( "strFileLseek() failed (%s)\n", szFilename );
		return TG_FAIL;
	}

	// read in first sectors worth of file, this holds enough information to detail how
	// big the PSMF header is for the .PMF file, then allocate a buffer to hold the header,
	// and read the full header into the allocated buffer
	if( ( iReadSize = strFileRead( &streamFile, cFileBuffer, 2048) ) < 2048 )
	{
		printf ( "strFileRead() failed (%s), bytes read: %d\n", szFilename, iReadSize );
		return TG_FAIL;
	}
	
	// pass in first sector and allocate memory to hold header
	if( SetPSMFHeaderBuffer( cFileBuffer ) == TG_FAIL )
		return TG_FAIL;

	// seek back to start of file, and this time load whole of header into buffer
	if( strFileLseek( &streamFile, 0L, SEEK_SET ) < 0 )
	{
		printf ( "strFileLseek() failed (%s)\n", szFilename );
		return TG_FAIL;
	}

	// read in PSMF header, IE all data before the stream begins in the file
	if( ( iReadSize = strFileRead( &streamFile, m_pHeaderMemoryBuffer, (SceInt32)m_iStreamOffset ) ) < (SceInt32)m_iStreamOffset )
	{
		printf ( "strFileRead() failed (%s)\n", szFilename );
		return TG_FAIL;
	}
	
	if( ( m_iResult = strFileClose( &streamFile ) ) == -1 )
	{
		printf ( "strFileClose() failed (%s)\n", szFilename );
		return TG_FAIL;
	}

	// extract details from the PSMF header into class variables
	if( SetPSMFDetails() == TG_FAIL )
		return TG_FAIL;

	return TG_SUCCESS;
}

// Description
// Reads stream offset and size from PSMF header and allocates enough memory to store this
// header
//
// Parameters
// cMemoryAddress - pointer to memory buffer to read PSMF header information from
//
// Returns
// TG_SUCCESS or TG_FAIL
//
int CPSMF::SetPSMFHeaderBuffer( const char *cMemoryAddress )
{

	// This function is the same as sceMpegQueryStreamOffset()
	if( ( m_iResult = scePsmfQueryStreamOffset( cMemoryAddress, &m_iStreamOffset ) ) != SCE_OK )
	{
		TG_PRINT( "scePsmfQueryStreamOffset() Failed to get data offset size, error code 0x%x", m_iResult );
		return TG_FAIL;
	}

	// Query what size the data stream is, this is currently the file size minus m_iStreamOffset (the data header)
	if( ( m_iResult = scePsmfQueryStreamSize( cMemoryAddress, &m_iStreamSize ) ) != SCE_OK )
	{
		printf( "scePsmfQueryStreamSize() Failed to get data stream size, error code 0x%x\n", m_iResult );
		return TG_FAIL;
	}

	m_pHeaderMemoryBuffer = (SceChar8 *)malloc( m_iStreamOffset );
	if( m_pHeaderMemoryBuffer == NULL )
	{
		printf( "SetPSMFHeaderBuffer() Failed to allocate header buffer\n" );
		return TG_FAIL;
	}

	return TG_SUCCESS;
}

// Description
// Reads all PSMF file details into class variables
//
// Parameters
//
// Returns
// TG_SUCCESS or TG_FAIL
//
int CPSMF::SetPSMFDetails( void )
{

	SceMpegStrType	StreamType;
	SceInt32		iStreamChannel;

	if( ( m_iResult = scePsmfSetPsmf( &m_PsmfManager, m_pHeaderMemoryBuffer ) ) != SCE_OK )
	{
		printf( "scePsmfSetPsmf() Failed to set PSMF, error code 0x%x\n", m_iResult );
		return TG_FAIL;
	}

	// PSMF file will contain only one start/end presentation time, plus the number of channels within
	// the .PMF file
	if( ( m_iResult = scePsmfGetPresentationStartTime( &m_PsmfManager, &m_uiStartTime ) ) != SCE_OK )
	{
		printf( "scePsmfGetPresentationStartTime() Failed to get start time, error code 0x%x\n", m_iResult );
		return TG_FAIL;
	}

	if( ( m_iResult = scePsmfGetPresentationEndTime( &m_PsmfManager, &m_uiEndTime ) ) != SCE_OK )
	{
		printf( "scePsmfGetPresentationEndTime() Failed to get end time, error code 0x%x\n", m_iResult );
	        return TG_FAIL;
	}

	// number of channels, this is the total number of different channels within the same file, made up of 
	// MPG4, PCM and Atrac types
	if( ( m_iNumberOfStreams = scePsmfGetNumberOfStreams( &m_PsmfManager ) ) < 0 )
	{
		printf( "scePsmfGetGetNumberOfStreams() Failed to get number of streams, error code 0x%x\n", m_iNumberOfStreams );
		return TG_FAIL;
	}

	// get number of the various types of channel in stream
	for( int i = 0; i < m_iNumberOfStreams; i++ )
	{
		if( ( m_iResult = scePsmfSpecifyStream( &m_PsmfManager, i ) ) != SCE_OK )
		{

			printf( "SetPSMFDetails() Failed to specify stream within PSMF file, error code 0x%x\n", m_iResult );
			return TG_FAIL;
		}

		if( ( m_iResult = scePsmfGetCurrentStreamType( &m_PsmfManager, &StreamType, &iStreamChannel ) ) != SCE_OK )
		{

			printf( "SetPSMFDetails() Failed to specify stream within PSMF file, error code 0x%x\n", m_iResult );
			return TG_FAIL;
		}

		switch( StreamType )
		{
		case SCE_MPEG_STREAM_M4AVC:
			{
				m_iNumberOfVideoChannels++;
				break;
			}
		case SCE_MPEG_STREAM_ATRAC:
			{
				m_iNumberOfAtracChannels++;
				break;
			}
		case SCE_MPEG_STREAM_PCM:
			{
				m_iNumberOfPCMChannels++;
				break;
			}
		default:
			{
				printf( "SetPSMFDetails() Invalid channel type within PSMF file (%d)\n", StreamType );
				return TG_FAIL;
			}
		}

		m_iNumberOfChannels++;
	}

	// TODO We could add further variables to setup other things to ensure we can output what is needed
	// for use, possibilities include video bit-mask etc

	return TG_SUCCESS;
}

// Description
// Returns the resolution of the specified video channel within the .PMF file, note that this function
// loops through all the streams in the file searching for the video channel passed in. IE if the file has been
// encoded with 2 audio channels first, then a video channel, the video channel will be stream number 2, but video 
// channel zero, from an API point of view the programmer is not bothered what actual stream the video channel is
// within the .PMF file, but just wants to play the video channel passed in
//
// Parameters
// iVideoChannel	- video stream channel to get horizontal resolution from
// iHResolution		- address of variable to hold video stream resolution
//
// Returns
// TG_SUCCESS or TG_FAIL
//
int CPSMF::GetResolution( int iChannel, int *iResolution, E_RESOLUTION_TYPE eResolutionType )
{

	SceMpegStrType		StreamType;
	SceInt32			iStreamChannel;
	ScePsmfVideoInfo	VideoSequence;

	// Loop through all streams searching for video channels, once found return resolution required
	for( int i = 0; i < m_iNumberOfStreams; i++ )
	{
		if( ( m_iResult = scePsmfSpecifyStream( &m_PsmfManager, i ) ) != SCE_OK )
		{

			printf( "GetResolution() Failed to specify stream within PSMF file, error code 0x%x\n", m_iResult );
			return TG_FAIL;
		}

		if( ( m_iResult = scePsmfGetCurrentStreamType( &m_PsmfManager, &StreamType, &iStreamChannel ) ) != SCE_OK )
		{

			printf( "GetResolution() Failed to specify stream within PSMF file, error code 0x%x\n", m_iResult );
			return TG_FAIL;
		}

		// if we have a video stream, and it matches the channel number requested then write the video
		// resolution to the structure passed in
		if( ( StreamType == SCE_MPEG_STREAM_M4AVC ) && ( iStreamChannel == iChannel ) ) 
		{
			if( ( m_iResult = scePsmfGetVideoInfo( &m_PsmfManager, &VideoSequence ) ) != SCE_OK )
			{

				printf( "GetResolution() Failed to get video resolution for channel %d, error code 0x%x\n", iStreamChannel, m_iResult );
				return TG_FAIL;
			}
			else
			{
				if( eResolutionType == HORIZONTAL_RESOLUTION )
					*iResolution = VideoSequence.horizontalSize;
				else
					*iResolution = VideoSequence.verticalSize;

				return TG_SUCCESS;
			}

		}

	}

	// if the specified video channel is not found then set resolution to an invalid value and
	// return failure
	*iResolution = -1;
	return TG_FAIL;
}

// Description
// Returns audio parameters of specified audio channel within the .PMF file, note that this function
// loops through all the streams in the file searching for the audio channel passed in. IE if the file has been
// encoded with 2 audio channels first, then a video channel, the video channel will be stream number 2, but video 
// channel zero, from an API point of view the programmer is not bothered what actual stream number the audio
// channel is, but just wants to play the audio channel passed in
//
// Parameters
// iAudioChannel	- audio stream channel to get horizontal resolution from
// AudioInfo		- address of variable to hold audio stream information
//
// Returns
// TG_SUCCESS or TG_FAIL
//
int CPSMF::GetAudioInfo( int iChannel, int iAudioStreamType, ScePsmfAudioInfo *AudioInfo )
{

	SceMpegStrType		StreamType;
	SceInt32			iStreamChannel;

	// Loop through all streams searching for audio channels, once found return audio information
	for( int i = 0; i < m_iNumberOfStreams; i++ )
	{
		if( ( m_iResult = scePsmfSpecifyStream( &m_PsmfManager, i ) ) != SCE_OK )
		{
			printf( "GetAudioInfo() Failed to specify stream within PSMF file, error code 0x%x\n", m_iResult );
			return TG_FAIL;
		}

		if( ( m_iResult = scePsmfGetCurrentStreamType( &m_PsmfManager, &StreamType, &iStreamChannel ) ) != SCE_OK )
		{
			printf( "GetAudioInfo() Failed to specify stream within PSMF file, error code 0x%x\n", m_iResult );
			return TG_FAIL;
		}

//		if( ( ( StreamType == SCE_MPEG_STREAM_ATRAC ) || ( StreamType == SCE_MPEG_STREAM_PCM ) ) && ( iStreamChannel == iChannel ) ) 
		if( ( StreamType == iAudioStreamType ) && ( iStreamChannel == iChannel ) ) 
		{
			if( ( m_iResult = scePsmfGetAudioInfo( &m_PsmfManager, AudioInfo ) ) != SCE_OK )
			{
				printf( "GetAudioInfo() Failed to get audio info for channel %d, error code 0x%x\n", iStreamChannel, m_iResult );
				return TG_FAIL;
			}
			else
			{
				//printf( "StreamType : %d, iChannel %d,iStreamChannel %d\n", StreamType, iChannel, iStreamChannel );
				return TG_SUCCESS;
			}

		}

	}

	// on failure to find specified audio channel, set values to zero and return failure
	AudioInfo->channelConfiguration	= 0;
	AudioInfo->samplingFrequency	= 0;

	return TG_FAIL;
}

// Description
// Outputs all relevent information regarding the PSMF class to the debug window
//
// Parameters
//
// Returns
//
void CPSMF::OutputDebugInfo()
{
	printf( "Member Variables\n" );
	printf( "================\n" );
//        ScePsmf         m_PsmfManager;                  // internal handle to PSMF header

	printf( "m_pHeaderMemoryBuffer    : 0x%x\n", (unsigned int)m_pHeaderMemoryBuffer );
	printf( "m_iStreamOffset          : %d\n", m_iStreamOffset );
	printf( "m_iStreamSize            : %d\n", m_iStreamSize );
	printf( "m_uiStartTime            : %d\n", m_uiStartTime );
	printf( "m_uiEndTime              : %d\n", m_uiEndTime );
	printf( "m_iNumberOfStreams       : %d\n", m_iNumberOfStreams);

	printf( "m_iNumberOfChannels      : %d\n", m_iNumberOfChannels);
	printf( "m_iNumberOfVideoChannels : %d\n", m_iNumberOfVideoChannels);
	printf( "m_iNumberOfPCMChannels   : %d\n", m_iNumberOfPCMChannels);
	printf( "m_iNumberOfAtracChannels : %d\n", m_iNumberOfAtracChannels);

	printf( "m_iResult                : %d\n", m_iResult );
	printf( "================\n" );

	return;
}

//-----------------------------------------------------------------------------
// P R I V A T E   F U N C T I O N S
//-----------------------------------------------------------------------------
#ifdef __MWERKS__
	#pragma mark -
	#pragma mark PRIVATE FUNCTIONS:
#endif

