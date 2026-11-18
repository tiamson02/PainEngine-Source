#ifndef __PKTVUNPACKEDFRAME_H__
#define __PKTVUNPACKEDFRAME_H__

#include "PKTVEntityProxies.h"
#include "UdpMessage.h"

struct PKTVPlayerData
{
	WORD	health;
	WORD	armor;
	BYTE	armorType;
	BYTE	weapon;
	WORD	ammoPrimary;
	WORD	ammoSecondary;
	bool	switchFire;
};

// The unpacked frame stores the data which can server as the state for all entities.
class PKTVUnpackedFrame
{
	void SortEntitiesByGIndex( void );
public:
	DWORD Time;
	DWORD FrameNumber;
	PKTVUnpackedFrame* NextFrame;			// pointer to the frame after this one ( may be 0 )
	UdpMessage* PackedFrame;				// cached packed frame, made from this frame, or this and the frame before
	DynamicArray<EntityProxy*> Proxies;		// you can assume they're sorted by GIndexes

	// Create a new frame from current game state
	PKTVUnpackedFrame( void );

	// Make frame from packed frame and diff
	PKTVUnpackedFrame( UdpMessage* packedFrame, PKTVUnpackedFrame* diffFrame );

	// Destroy frame
	~PKTVUnpackedFrame( void );

	// Make diff frame from two frames
	UdpMessage* MakePackedFrame( PKTVUnpackedFrame* diffFrame );
	void WriteEngineState( UdpMessage *msg, PKTVUnpackedFrame* diffFrame = 0 );
	void ReadEngineState( UdpMessage *msg, PKTVUnpackedFrame* diffFrame = 0 );

	ulong32 MemoryTaken( bool report );

#ifdef PKTV_DISPLAY
	// Display ( transfer to engine ) current frame, interpolated with the next frame at given fraction
	void DisplayInterpolatedWith( float fraction, DWORD timeDiffMs );
	void UpdateMap( void );
	void DeleteEntitiesNonexistentInNextFrame( void );
	void DeleteEntitiesNonexistentInThisFrame( void );
	void CreateEntitiesExistingInNextFrame( void );
#endif

private:

	String	mMapName;
	Vector	mCamRot;
	Vector	mCamPos;
	float	mCamFOV;
	bool	mGhostsFX;
	bool	mDemonFXEnabled;
	bool	mDemonFXSuper;
	float	mDemonFXWarp;
	DWORD	mMemoryTaken;

	DynamicArray<PKTVPlayerData> mPlayerData;

	int		mCurrentPlayer;
};

#endif	// __PKTVUNPACKEDFRAME_H__
