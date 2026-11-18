
// #pragma once
#ifndef _HAVOKMISSILECONTROLLER_
#define _HAVOKMISSILECONTROLLER_


#include "Physics/Havok/HavokPhysics.h"

//#define MISSILE_LOGGING

#ifdef MISSILE_LOGGING
extern unsigned int missileLogNumber;
#include "Core/FileSystem/LogBuffer.h"
#endif

class hkMyMissilePhantom;
class hkAabbCastShape;

#define MAX_MISSILE_SPEED 190.f
#define SPEED_TRANSFER_FACTOR (SHRT_MIN/-MAX_MISSILE_SPEED)

class HavokMissileController
{
	friend struct MissileState;

	struct MoveState
	{
		unsigned char deltaMs;
		hkVector4 position;
		short velocityX, velocityY, velocityZ;
		unsigned char properties;
		unsigned short timeSinceCreation;
		hkVector4 lastKnownGoodPosition;

		void getVel( hkVector4& velocity ) const
		{
			velocity.set( velocityX/SPEED_TRANSFER_FACTOR, velocityY/SPEED_TRANSFER_FACTOR, velocityZ/SPEED_TRANSFER_FACTOR );
		}

		void setVel( const hkVector4& velocity )
		{
			int tempV = (int)(velocity(0)*SPEED_TRANSFER_FACTOR);
			velocityX = ((tempV>SHRT_MAX)?SHRT_MAX:((tempV<SHRT_MIN)?SHRT_MIN:tempV));
			tempV = (int)(velocity(1)*SPEED_TRANSFER_FACTOR);
			velocityY = ((tempV>SHRT_MAX)?SHRT_MAX:((tempV<SHRT_MIN)?SHRT_MIN:tempV));
			tempV = (int)(velocity(2)*SPEED_TRANSFER_FACTOR);
			velocityZ = ((tempV>SHRT_MAX)?SHRT_MAX:((tempV<SHRT_MIN)?SHRT_MIN:tempV));
		}
	};

public:

	enum ControllerTypes
	{
		CT_Default		= 0,
		CT_Stake		= 1,
		CT_Grenade		= 2,
		CT_PainHead		= 3,
		CT_HeaterBomb	= 4,
		CT_Soul			= 5,
		CT_Spectator	= 6,
	};

	HavokMissileController( hkWorld* world, hkRigidBody* body, float size, int type = CT_Default );
	~HavokMissileController();

	// Work methods
	// ---------------------
	void missileMove( DWORD timeDelta );
	void predict( void );

	// Smaller methods
	// ---------------------
	const hkVector4& getPosition( void ) const;											// Get current position
	void setPosition( const hkVector4& targetPosition );								// Set current position
	hkVector4 getVelocity( void ) const;												// Get current velocity
	void setVelocity( const hkVector4& vel );											// Set curent velocity
	void addVelocity( const hkVector4& impulse );										// Apply an impulse
//	void setGravity( bool grav );														// Enable/disable gravity
	bool getGravity( void ) { return ( ( m_state.properties & MIPR_Gravity ) != 0 ); };	// Get gravity setting
	void setDamping( float damping ) { m_damping = damping; };							// Set linear damping
	float getDamping( void ) { return m_damping; };										// Get linear damping
	float getMissileFriction( void ) { return m_friction; };							// Get friction
	void setMissileFriction( float friction ) { m_friction = friction; };				// Set friction
	void setAngularVelocity( Vector& velocity );
	Vector getAngularVelocity( void );

	enum MIPR
	{
		MIPR_Gravity		= (1<<0),
		MIPR_Stuck			= (1<<1),
		MIPR_StakeGravity	= (1<<2),
		MIPR_SurfaceStick	= (1<<3),
		MIPR_PainHead		= (1<<4),
		MIPR_HeaterBomb		= (1<<5),
		MIPR_Soul			= (1<<6),

		MIPR_HasAngVel		= (1<<7),		// for netcode only
	};

private:
	Entity*	getEntity( void );

	void	internalMove( MoveState& state, DWORD diffTime );
	void	slideMove( MoveState& state );

	bool	positionValid( MoveState& state );
	void	correctPosition( MoveState& state );

	void	packPosition( void );
	void	unpackPosition( void );

	// DATA
	// ---------------------

	MoveState		m_state;
	hkVector4		m_halfExtents;
	hkReal			m_damping;
	hkReal			m_friction;
	hkReal			m_restitution;

	hkReal			m_timeDeltaFraction;	// only used to add-up time delta ( input from tick ). Not used for movement & network.
	unsigned char	m_internalTimeDelta;

	class hkWorld*		m_world;
	hkMyMissilePhantom*	m_missilePhantom;
	hkAabbCastShape*	m_aabbCastShape;
	hkRigidBody*		m_body;

	DWORD				m_lastStateReceived;
	DWORD				m_lastFrameWhenMoved;

	DWORD				m_timeExists;

	bool				m_collisionsReport;

#ifdef MISSILE_LOGGING
	LogBuffer			mLog;
#endif
};


#endif //_HAVOKMISSILECONTROLLER_

