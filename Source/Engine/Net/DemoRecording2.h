#ifndef TRANSGAMING

#ifndef __DEMOREC2_H__
#define __DEMOREC2_H__

#include "Core/LeString.h"
#include "Core/Math/LeMath.h"


class AudioRecorder;

class ENGINE_API DemoRecording2
{
	enum RecordingState
	{
		IDLE = 0,
		RECORDING = 1,
		PLAYING = 2,
	};

public:

	DemoRecording2();
	~DemoRecording2();

	void StartRecording( const char* filename );
	void Stop( void );
	bool StartPlaying( const char* filename, String* errorString, bool timeDemo = false, bool takeScreenshots = false );

	void Update( void );
	void RecordPlayer( int playerIndex, unsigned short yaw, int health, int armor );

	bool IsPlaying( void ) const	{
#ifndef TRANSGAMING
        return ( mState == PLAYING ); 
#else
        return false;
#endif
    }
	bool IsIdle( void ) const		{
#ifndef TRANSGAMING
        return ( mState == IDLE ); 
#else
        return true;
#endif
    }
	bool IsRecording( void ) const	{
#ifndef TRANSGAMING
        return ( mState == RECORDING ); 
#else
        return false;
#endif
    }

	void AddToDelete( int i ) { 
#ifndef TRANSGAMING
        mToDelete.Add( i ); 
#endif //TRANSGAMING
    }

	void SetSpeed( float speed ) { 
#ifndef TRANSGAMING
        if( speed <= 4.0f && speed >= 0.5f ) mSpeed = speed; 
#endif
    }
	float GetSpeed() const { 
#ifndef TRANSGAMING
        return mSpeed; 
#else
		return 0.0f;
#endif
    }

	float GetFPS() const {
#ifndef TRANSGAMING
        return mLastFPS; 
#else
		return 0.0f;
#endif
    }

private:

	bool ReadHeader( String& fileVer, String& fileNetVer, long32& timeDiff );
	bool WriteHeader( String fileVer, String fileNetVer, long32 timeDiff );

	void UpdateRecording( void );
	void UpdatePlaying( void );

	void UpdateView( Vector &pos, float yaw, float dir );

#ifndef TRANSGAMING
	RecordingState	mState;
	GFile* mRecordingFile;
	DWORD mStartTime;
	String mLastMap;
	DynamicArray<int> mToDelete;

	HashMap<int, int, 1024>	mEntities;

	float mSpeed;
	bool mReady;
	bool mIsTimeDemo;
	int mNumFrames;
	float mLastFPS;
	bool mTakeScreenshots;
	String mFileName;

	AudioRecorder *mAudioRec;
#endif
};


extern ENGINE_API DemoRecording2 *gDemoRec;


#endif // __DEMOREC2_H__

#endif
