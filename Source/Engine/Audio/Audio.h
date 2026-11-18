
// #pragma once
#ifndef _AUDIO_
#define _AUDIO_


#include "Core/Math/LeMath.h"

#define AUDIO_WARNINGS_ON

//#define ALL_AUDIO_DEBUG

#ifdef ALL_AUDIO_DEBUG
#define MILES_STREAM_DEBUG
#define MILES_2DSOUND_DEBUG
#define MILES_3DSOUND_DEBUG
#endif

class MilesEngine;

enum SpeakerTypes
{
	Speakers2 = 0,
	Headphones = 1,
	Surround = 2,
	Speakers4 = 3,
	Speakers51 = 4,
	Speakers71 = 5,
};

enum RoomTypes
{
	Generic,
	PaddedCell,
	Room,
	Bathroom,
	LivingRoom,
	StoneRoom,
	Auditorium,
	ConcertHall,
	Cave,
	Arena,
	Hangar,
	CarpetedHallway,
	Hallway,
	StoneCorridor,
	Alley,
	Forest,
	City,
	Mountains,
	Quarry,
	Plain,
	ParkingLot,
	SewerPipe,
	UnderWater,
	Drugged,
	Dizzy,
	Psychotic,
};

class MilesLoadedFile;
class SoundList;
class MilesEngine;

#endif //AUDIO

