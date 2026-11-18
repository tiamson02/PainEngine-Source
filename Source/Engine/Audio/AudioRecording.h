
// #pragma once
#ifndef _AUDIORECORDING_
#define _AUDIORECORDING_


#ifndef TRANSGAMING_NO_MILES
# include "Audio/MilesEngine.h"
#endif

#include "Net/UdpMessage.h"

#define AUDIO_RECORDER_BUFFER_SIZE (MAX_UDP_PACKET_SIZE-100)

class AudioRecorder
{
	friend class MilesEngine;	// for SetAction

	struct AudioActionBuffer
	{
		UdpMessage Buffer;
		AudioActionBuffer* Next;
	};

	// Getting info from audio system
	void SetAction( unsigned char action, ... );	// add action to recorder

	MilesEngine* mMiles;
	AudioActionBuffer* mBuffers;
	bool mPlaying;

	// Action types
	enum AudioRecordingActions
	{
		PreloadFile,
		SetSpeed,
		SetLowPass,
		SetMasterVolumeLevel,
		SetFalloffSpeed,
		SetStreamingVolume,
		Set3DDigitalEffectsVolume,
		SetSpeakerType,
		SetSpeakersFlip,
		SetSoundSpeedRandomizer,
		SetAllSoundsToLowPass,
		SetDefaultObstruction,
		SetObstructionProperties,
		SetRoom,
		SetRoomIntensity2D,
		SetRoomIntensity3D,
		PauseCurrentlyPlayingSounds,
		ResumeSounds,
		StopAllSounds,
		LoadFilesOffMemory,
		SetSoundProperties,
		Sound2D_Create,
		Sound2D_CreateEx,
		Sound2D_Play,
		Sound2D_PlayAndForget,
		Sound2D_Resume,
		Sound2D_Stop,
		Sound2D_Forget,
		Sound2D_Delete,
		Sound2D_SetDontSave,
		Sound2D_SetLoopCount,
		Sound2D_SetVolume,
		Sound2D_NoSpeedRandomize,
		Sound2D_SetSoundSpeed,
		Sound2D_SetAlwaysSameSpeed,
		Sound2D_SetLowPass,
		Sound3D_Create,
		Sound3D_CreateEx,
		Sound3D_Play,
		Sound3D_PlayAndForget,
		Sound3D_Resume,
		Sound3D_Stop,
		Sound3D_Forget,
		Sound3D_Delete,
		Sound3D_SetDontSave,
		Sound3D_SetPosition,
		Sound3D_SetLoopCount,
		Sound3D_SetVolume,
		Sound3D_SetIntensity,
		Sound3D_SetHearingDistance,
		Sound3D_SetObstructed,
		Sound3D_SetNoSpeedRandomize,
		Sound3D_SetSoundSpeed,
		Sound3D_SetVelocity,
		Sound3D_SetLowPass,
		Sound3D_SetOrientation,
		StreamingSound_Create,
		StreamingSound_Play,
		StreamingSound_Resume,
		StreamingSound_Stop,
		StreamingSound_Delete,
		StreamingSound_SetLoopCount,
		StreamingSound_SetVolume,
		StreamingSound_SetLowPass,
		SetPlayerPosition,
		SetPlayerVelocity,
		SetPlayerOrientation,
	};

public:

	// Maintenance
	AudioRecorder( MilesEngine* engineToRecord );
	~AudioRecorder( void );

	// SAVING/LOADING
	// ===============

	// Save/load diffs
	void FlushActionsTo( FileObject* file );
	void ExecuteActionsFrom( FileObject* file, bool skipOnly );

	// Save/load full state
	void StartRecordingTo( FileObject* file );
	void StartPlayingFrom( FileObject* file, bool skipOnly );
};

#endif //_AUDIORECORDING_


