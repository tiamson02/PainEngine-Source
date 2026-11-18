#error This file is obselete

// #pragma once
#ifndef _DEMORECORDING_
#define _DEMORECORDING_


class NetworkClient;
class NetworkDevice;
class GFile;

#include "Net/UdpMessage.h"
#include "Net/StringHashSystem.h"
#include "Net/ObjectSynchronizer.h"

struct PlayerDataEntry
{
	DWORD Time;
	unsigned short Yaw;
	float Dir;
};

class PlayerDataRecord
{
public:
	PlayerDataRecord() : CurrentEntryMarker(0) {};
	~PlayerDataRecord() {};
	DynamicArray<PlayerDataEntry> Entries;

	int CurrentEntryMarker;		// this one is 0 when before first entry, 1 when after first, 2,3,... and when after last, its (DWORD)-1
};

class DemoRecording
{
	friend class IPChannel;
	friend class IPDriver;

	enum RecordingState
	{
		IDLE = 0,
		RECORDING = 1,
		PLAYING = 2,
	};

public:
	DemoRecording( void );
	~DemoRecording( void ) { Stop(); };

	void StartRecording( const char* filename, NetworkClient* client = 0 );
	void Stop( void );
	bool StartPlaying( const char* filename, NetworkClient* client, String* errorString );

	void SetNet( NetworkDevice* device ) { mNet = device; };
	bool FileExists( const char* filename );
	bool CanPlayFile( const char* filename, unsigned char& clientIndex, String* errorText, bool& predictionOn );
	void WritePlayerData( unsigned char playerNumber, unsigned short yaw );
	void UpdateOurPlayerData( void );
	void Pause( bool pause );
	void AdjustTimeToNextMessage( void );
	void AdjustTimeToSlowdown( float timeMultiplier );
	void CheckRestoreCaches( void );

	bool IsPlaying( void )		{ return ( State == PLAYING ); };
	bool IsIdle( void )			{ return ( State == IDLE ); };
	bool IsRecording( void )	{ return ( State == RECORDING ); };

private:
	NetworkDevice* mNet;
	NetworkClient* mClient;
	GFile* mRecordingFile;
	RecordingState State;
	bool mPause;
	DWORD mPauseStart;

	NetworkClient* ChooseBestClientToRecord( void );

	// These are called by IPChannel
	void RecordUdpMessage( UdpMessage* message, bool fromPos = false );
	bool PlayUdpMessage( UdpMessage* message );
	bool ReadUdpMessage( DWORD curTime );

	// Shortcuts
	bool ReadHeader( GFile* f, String& fileVer, String& fileNetVer, unsigned char& clientIndex, bool& restoreCaches, bool dumpCaches, long& timeDiff, bool& predictionOn );
	bool WriteHeader( GFile* f, String fileVer, String fileNetVer, unsigned char clientIndex, bool restoreCaches, long timeDiff, bool predictionOn );
	void WriteFakeConnection( void );

	// Player data
	bool PassPlayerData( void );
	bool PrereadPlayerData( const char* filename, String* errorString );
	DynamicArray<PlayerDataRecord*> PlayersInfo;
	void ClearPlayersInfo( void );

	// Message received and waiting to propagate
	char mMessageBuffer[MAX_UDP_PACKET_SIZE];
	DWORD mMessageTime;
	int mMessageSize;

	// Time difference between recorded time and played time
	// It says how much we have to add or subtract from each received time to get our time
	DWORD mTimeDiff;
	bool mTimeDiffPlus;
	bool mTimeDiffSet;

	// Caches for faking connection
	bool RestoreCaches;
	PONetCreationCache POCache;
	StringHashSystem StringTransport;

	// Last time time adjusted ( for adjust time slowdown )
	DWORD mLastTimeTimeAdjusted;

	// For spectator state checks when recording - recording stops when player switches to spectator or spectator to player
	bool mWasSpectator;
};



#endif //_DEMORECORDING_

