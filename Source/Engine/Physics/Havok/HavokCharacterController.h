
// #pragma once
#ifndef _HAVOKCHARACTERCONTROLLER_
#define _HAVOKCHARACTERCONTROLLER_


// For Havok-independent player movement

#include "Physics/Havok/HavokPhysics.h"
#include "Physics/PhysicsObject.h"
#include "Net/ObjectSynchronizer2.h"

class hkMyCharPhantom;
class hkAabbCastShape;
class FileObject;
class UdpMessage;
class LogBuffer;

#define PLAYER_CONTROLLER_TEST

#define MAX_PLAYER_SPEED 190.f
#define PLAYER_SPEED_TRANSFER_FACTOR (SHRT_MIN/-MAX_PLAYER_SPEED)

struct HavokCharacterConstructionInfo
{
	HavokCharacterConstructionInfo();			// Default values are set in the constructor

	hkVector4	m_position;						// The initial position for the character
	hkVector4	m_halfExtents;					// The size of the AABB for the character 
};

class HavokCharacterController
{
	friend class PhysicsObject;
	friend class PlayerState;

	struct MoveState
	{
		unsigned char deltaMs;
		hkVector4 position;
		hkVector4 velocity;
		unsigned char lastHeightOfStep;
		hkReal cameraVerticalCatchUp;
	};

	struct MoveStateInternal
	{
		unsigned char deltaMs;
		hkVector4 position;
		hkVector4 velocity;
		hkReal lastHeightOfStep;
		hkReal cameraVerticalCatchUp;
	};

public:


	HavokCharacterController( const HavokCharacterConstructionInfo& info, hkWorld* world, hkRigidBody* body );
	~HavokCharacterController();

	// Main methods
	// ---------------------
	void	playerMove( bool fromExternalData = false );
	void	addTime( float timeDelta )
	{
		m_timeDelta += timeDelta;
		unsigned int ad = (unsigned int)ceil(1000.f*m_timeDelta);
		m_timeDelta -= (ad/1000.f);

		ad += m_internalTimeDelta;
		if( ad > 255 )
			ad = 30;
		m_internalTimeDelta = ad;
	}

	void	setAction( DWORD action ) { m_action = action; };
	void	setPlayerBody( hkRigidBody* body );
	void	setLookDirection( Vector forwardVector )
	{
		float yaw;
		float pitch;
		Assert( fabs(forwardVector.Magnitude() - 1.0f) < 0.001f );
		yaw = atan2(forwardVector.X,forwardVector.Z);
		pitch = asin(forwardVector.Y);	// magnitude == 1.0f
		m_pitch = (short)(pitch*65535/F_PI);
		m_yaw = (short)(yaw*32767/F_PI);
	}

	void	setLookDirection( float yaw, float pitch )
	{
		m_pitch = (short)(pitch*65535/F_PI);
		m_yaw = (short)(yaw*32767/F_PI);
	}

	// Prediction
	// ---------------------
	void	predict( bool interpolate );
	void	setStateReceivedTime( DWORD time ) { m_lastStateReceived = time; };

	// Inlines
	// ---------------------

	// Get current position
	const hkVector4&	getPosition( void ) const
	{
		return m_state.position;
	}

	// Warp the character (through walls etc.)
	void				setPosition( const hkVector4& targetPositionWS );

	// Get linear velocity
	const hkVector4&	getLinearVelocity( void ) const { return m_state.velocity; };

	// Get linear velocity 2
	const hkVector4&	getLinearVelocity2( void ) { velIn(); return m_state.velocity; };
	Vector				getLinearVelocity3( void ) { return Vector( m_velocityX, m_velocityY, m_velocityZ ); };

	// Set linear velocity
	void				setLinearVelocity( const hkVector4& vel )
	{
		m_state.velocity = vel;
		velOut();
		++m_externalInfluences;
	};

	void				addExternalInfluence( void ) { ++m_externalInfluences; };

	// returns true if character is support in the 'up' direction
	bool				isOnGround( void ) const { return m_onGround; };

	// returns true if character is falling
	bool				isFalling( void ) const { return m_falling; };

	// Apply an impulse
	void				addVelocity( const hkVector4& impulse );

	// Set time for the player to be shocked by explosion
	void				setPlayerShocked( unsigned short timeSinceExplosion )
	{
		m_msSinceExplosion = timeSinceExplosion;
		++m_externalInfluences;
	}

	void setPlayerBodyRotation( bool fromLastMove ) { m_receivedRotation = fromLastMove?LastMove.Yaw:m_yaw; };

	// Data for last move made; used only for sending over the network
	PlayerInput			LastMove;

	unsigned char		MPByte;

private:

	// Enemy player interpolation settings
	bool		mInterpolate;
	hkVector4	mLastPos;

	Entity*	getEntity( void );

	void	internalPlayerMove( void );
	void	internalPlayerMovePromode( void );
	void	internalPredict( MoveState& state, DWORD diffTime );

	void	applyCharacter( MoveState& state );						// update and move the character
	void	applyCharacterLadder( MoveState& state );				// update and move the character

	hkReal	upAStep( MoveStateInternal& state );
	void	downAStep( MoveStateInternal& state, hkReal distanceDown );
	bool	backAStep( hkVector4& position, hkVector4& velocity, hkVector4& moveVector );
	void	forwardAStep( hkVector4& position, hkVector4& movedBackwards );

	// try to move in the direction of the charactor. If it hits something try to slide along
	bool	slideMove( MoveStateInternal& state, bool stepUp );
	bool	ladderMove( MoveStateInternal& state, bool stepBackwards );

	bool	floorCheck( hkVector4& floorNormal );
	bool	floorCheckShort( float down );
	void	correctNormal( hkVector4& floorNormal );
	bool	positionValid( hkReal* distance = 0 );
	void	correctPosition( void );
	void	updateCameraInterpolation( MoveState& state );

	void	 velIn( void )
	{
		m_state.velocity.set( m_velocityX/PLAYER_SPEED_TRANSFER_FACTOR, m_velocityY/PLAYER_SPEED_TRANSFER_FACTOR, m_velocityZ/PLAYER_SPEED_TRANSFER_FACTOR );
	}

	void	velOut( void )
	{
			int tempV = (int)(m_state.velocity(0)*PLAYER_SPEED_TRANSFER_FACTOR);
			m_velocityX = ((tempV>SHRT_MAX)?SHRT_MAX:((tempV<SHRT_MIN)?SHRT_MIN:tempV));
			tempV = (short)(m_state.velocity(1)*PLAYER_SPEED_TRANSFER_FACTOR);
			m_velocityY = ((tempV>SHRT_MAX)?SHRT_MAX:((tempV<SHRT_MIN)?SHRT_MIN:tempV));
			tempV = (short)(m_state.velocity(2)*PLAYER_SPEED_TRANSFER_FACTOR);
			m_velocityZ = ((tempV>SHRT_MAX)?SHRT_MAX:((tempV<SHRT_MIN)?SHRT_MIN:tempV));
	}

#ifdef PLAYER_CONTROLLER_TEST
	// For testing
	FileObject* m_file;

public:
	void StartRecording( const char* filename );
	void TestSavedData( const char* filename, PhysicsObject* po );
private:
	void SaveFull( void );
	void SaveInput( void );
	void LoadInput( FileObject* file );
	void LoadFullAndCompare( LogBuffer& log, FileObject* file, ulong32& stateNumber );
#endif
	// DATA
	// ---------------------

	MoveState m_state;

	// Set once, at construction
	hkReal		m_stepNormal;
	hkVector4	m_halfExtents;

	hkReal				m_timeDelta;	// only used to add-up time delta ( input from tick ). Not used for movement & network.

	unsigned char		m_internalTimeDelta;
	DWORD				m_action;
	short				m_yaw;
	short				m_pitch;

	DWORD				m_lastNonzeroAction;
	DWORD				m_jumpAction;
	hkReal				m_fallSpeed;

	bool				m_jumpInvalidation;
	bool				m_falling;
	bool				m_canBunnyHop;
	bool				m_hasJumpVector;
	bool				m_onGround;

	DWORD				m_externalInfluences;	// number of external influences since the object creation

	unsigned short		m_msSinceExplosion;		// this measures time in ms since explosion to render player temporarily immovable

	unsigned char		m_msSinceJumped;		// this measures the difference in ms between pressing jump key and touching floor - to calculate bunny hop
	unsigned char		m_msSinceLanded;		// this measures the difference in ms between touching floor and pressing jump key - to calculate bunny hop

	unsigned short		m_speed;				// this reminds us how much speed we have

	short				m_velocityX, m_velocityY, m_velocityZ;

	float				m_tempVelocity;
	unsigned char		m_tempDeltaMs;

	class hkWorld*		m_world;
	hkMyCharPhantom*	m_characterPhantom;
	hkAabbCastShape*	m_aabbCastShape;
	hkRigidBody*		m_body;

	hkVector4			m_lastKnownGoodPosition;
	hkReal				m_maxStepHeight;

	// Rotation interpolation
	short				m_receivedRotation;
	short				m_lastRotation;

	// Steps ascension interploation
	hkReal				m_cameraVerticalCatchUp;

	DWORD				m_lastStateReceived;

	// Are we predicting or actually moving now?
	bool				m_predicting;
};



#endif //_HAVOKCHARACTERCONTROLLER_

