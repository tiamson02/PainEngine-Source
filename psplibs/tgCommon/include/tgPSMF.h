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
//	FILE			:	tgPSMF.h
//
//	Project  		:	
//
//	By		 		:	Mike Kavallierou
//
//	Function 		:	Header file for PSMF class.
//
//	Comments		:	Class is used to hide the complexities of PSMF usage,
//						and provide	simple Get functions for retrieving PSMF
//						details, these calls were developed	for use with the
//						Mpeg class, but can be used on it's own to return
//						header details
//									
//	Date	 		:	04 / 03 / 05
//
//**************************************************************************

#ifndef TGPSMF_H
#define TGPSMF_H

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------
#include <libpsmf.h>
#include <kerneltypes.h>
#include <psptypes.h>
#include <libpsmf.h>

#include "tgTypes.h"

//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------
#ifdef __MWERKS__
	#pragma mark TYPEDEFS
	#pragma mark -
#endif

//-----------------------------------------------------------------------------
// F O R W A R D   D E C L A R A T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// C O N S T A N T S
//-----------------------------------------------------------------------------

// enum for use in specifying what type of resolution to return in GetResolution function call
enum E_RESOLUTION_TYPE
{
	HORIZONTAL_RESOLUTION,
	VERTICAL_RESOLUTION
};

// enum for meaningful way to represent whether a specified audio stream is mono or stereo
enum E_AUDIO_TYPE
{
	AUDIO_MONO = 1,
	AUDIO_STEREO
};

//-----------------------------------------------------------------------------
// C L A S S   D E F I N I T I O N S
//-----------------------------------------------------------------------------
class CPSMF
{
// The PSMF class provides access and control over PMF file headers on PSP

public:
	CPSMF();
	~CPSMF();

	void	Init();	// note that the Init() function frees up any existing allocated memory from previous usage
	
	int	LoadPSMFHeaderFromFile( const char *szFilename );	// function loads in PSMF header from file to memory
	int	LoadPSMFHeaderFromMemory( const char *pMemoryBuffer );	// function loads in PSMF header from an existing buffer to memory

//Accessor fns:

	// return video resolutions
	int	GetHResolution( int iVideoChannel, int *iHResolution ) { return GetResolution( iVideoChannel, iHResolution, HORIZONTAL_RESOLUTION ); }	// function returns horizontal resolution of specified video channel
	int	GetVResolution( int iVideoChannel, int *iVResolution ) { return GetResolution( iVideoChannel, iVResolution, VERTICAL_RESOLUTION ); }	// function returns vertical resolution of specified video channel

	int GetPCMAudioInfo( int iChannel, ScePsmfAudioInfo *AudioInfo ) { return GetAudioInfo( iChannel, SCE_MPEG_STREAM_PCM, AudioInfo ); }		// return PCM audio information, mono/stereo and sample rate
	int GetAtracAudioInfo( int iChannel, ScePsmfAudioInfo *AudioInfo ) { return GetAudioInfo( iChannel, SCE_MPEG_STREAM_ATRAC, AudioInfo ); }	// return Atrac audio information, mono/stereo and sample rate

	// return stream values, size can be used to track data reading into buffers, offset is used to specify where to start
	// reading stream data from within the .PMF file itself
	int	GetStreamSize( int *iStreamSize ) { *iStreamSize = m_iStreamSize; return TG_SUCCESS; }
	int	GetStreamOffset( int *iStreamOffset ) { *iStreamOffset = m_iStreamOffset; return TG_SUCCESS; } // function returns PSMF file stream offset

	// get current video streams start and end presentation time
	int GetStartTime( unsigned int *uiStartTime ) { *uiStartTime = m_uiStartTime; return TG_SUCCESS; }	// return presentation start time
	int GetEndTime( unsigned int *uiEndTime ) { *uiEndTime = m_uiEndTime; return TG_SUCCESS; }			// return presentation end time

	// get number of various channels within the .PMF file
	int	GetNumberOfVideoChannels( int *iNumberOfVideoChannels ) { *iNumberOfVideoChannels = m_iNumberOfVideoChannels; return TG_SUCCESS; };	// return the number of video channels within the PSMF file
	int	GetNumberOfPCMChannels( int *iNumberOfPCMChannels ) { *iNumberOfPCMChannels = m_iNumberOfPCMChannels; return TG_SUCCESS; };			// return the number of PCM channels within the PSMF file
	int	GetNumberOfAtracChannels( int *iNumberOfAtracChannels ) { *iNumberOfAtracChannels = m_iNumberOfAtracChannels; return TG_SUCCESS; };	// return the number of Atrac channels within the PSMF file
	int	GetNumberOfChannels( int *iNumberOfChannels ) { *iNumberOfChannels = m_iNumberOfChannels; return TG_SUCCESS; };						// return the number of all channels within the PSMF file

//Debug functions
	void	OutputDebugInfo();

protected:

// functions:
private:

	// return audio information, mono/stereo and sample rate
	int GetAudioInfo( int iChannel, int iAudioStreamType, ScePsmfAudioInfo *AudioInfo );

	int	GetResolution( int iChannel, int *iResolution, E_RESOLUTION_TYPE eResolutionType );

	// gets header size and allocates enough memory to hold header
	int	SetPSMFHeaderBuffer( const char *cMemoryAddress );

	// fills out .PMF file header information into class variables
	int	SetPSMFDetails( void );

//Attributes:
private:

	ScePsmf		m_PsmfManager;			// internal handle to PSMF header
	SceChar8*	m_pHeaderMemoryBuffer;	// pointer to memory allocated to hold PSMF header information

	SceUInt32	m_iStreamOffset;		// offset from start of file to start of data, used to allocate buffer header
	SceUInt32	m_iStreamSize;			// data stream size from PSMF file
	
	SceUInt32	m_uiStartTime;			// video stream presentation start time
	SceUInt32	m_uiEndTime;			// video stream presentation end time
	SceInt32	m_iNumberOfStreams;		// number of streams within file

	// stream/channel information
	SceInt32	m_iNumberOfChannels;		// should be the same as number of streams, until PSMF file starts supporting data streams
	SceInt32	m_iNumberOfVideoChannels;	// number of video channels in the PSMF file
	SceInt32	m_iNumberOfPCMChannels;		// number of PCM audio channels in the PSMF file
	SceInt32	m_iNumberOfAtracChannels;	// number of Atrac audio channels in the PSMF file

	SceInt32	m_iResult;				// return value from library calls

};

#endif

