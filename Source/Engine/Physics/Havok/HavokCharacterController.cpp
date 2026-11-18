
#include "Engine.h"
#include "Engine/BaseGame.h"
#include "Physics/Havok/HavokCharacterController.h"
#include "Physics/PhysicsEngine.h"
#include "Engine/InputSystem.h"
#include "Engine/World.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Core/FileSystem/FileManagerStandard.h"
extern ENGINE_API FileManagerStd GFileMan;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#ifdef PLAYER_CONTROLLER_TEST
#include "Physics/Havok/POHavokPart.h"
#endif

#ifdef TRANSGAMING_PS2
#include "tg_pc2ps2.h"
#endif

#ifndef HAVOK3
#include <hkcollide2/shape/hkAabbCastShape.h>
#include <hkdynamics2/phantom/hkCharacterPhantom.h>
#endif

#define OVERBOUNCE 1.01f
#define COS_OF_36_DEG 0.809016994374f
#define COS_OF_40_DEG 0.766044443119f
#define MAX_BACKWARD_DISTANCE 0.7f
#define MAX_POSSIBLE_TIME_MS_FOR_STEP 25
//#define FULL_DEBUG_PRINT

// *********************************************************************************************************************

#ifdef HAVOK3
class hkMyCharPhantom : public hkSimpleShapePhantom
{
public:
	hkMyCharPhantom( const hkShape *shape, const hkTransform &transform, hkUint32 m_collisionFilterInfo=0 ) : hkSimpleShapePhantom(shape, transform, m_collisionFilterInfo), m_ourBody(0), m_collidingMeshes(0) {};
#else
class hkMyCharPhantom : public hkCharacterPhantom
{
public:
	hkMyCharPhantom( const hkCharacterPhantomConstructionInfo& info ) : hkCharacterPhantom(info), m_ourBody(0), m_collidingMeshes(0) {};
#endif

// BLAIR - Cut this out for now!
#ifndef HAVOK3
	inline const hkArray<hkSimpleClosestPointsDetails*>& getCollisionDetails() const { return m_myCollisionDetails; }

	int updateCollisions( hkCollisionEnvironment& env )
	{
		hkReal minDist = hkMath::HK_REAL_MAX;

		int i = 0;
		const int a = m_myCollisionDetails.getSize();
		int	index = -1;

		for( i = a - 1; i >= 0; --i )
		{
			hkSimpleClosestPointsDetails* pointDetails = m_myCollisionDetails[i];
			pointDetails->updateClosestPoints( env );
			if( pointDetails->getClosestIndex() >= 0 )
			{
				if( index >= 0 )
				{
					if( pointDetails->getClosestDistance() < minDist )
					{
						minDist = pointDetails->getClosestDistance();
						index = i;
					}
				}
				else
				{
					minDist = pointDetails->getClosestDistance();
					index = i;
				}
			}
		}

		return index;
	}

	virtual void addOverlappingCollidable( hkCollidable& collidable )
	{
		if( collidable.getShape() == HK_NULL )
			return;

		int collGroup = collidable.getCollisionFilterInfo();
		switch( collGroup )
		{
		case HCGFixed:
		case HCGBarrier:
			// Collision with fixed geometry - yes
			{
                m_setTag("Physics");
				hkSimpleClosestPointsDetails* destElem = new hkSimpleClosestPointsDetails( collidable, m_collidable, *m_owner->getCollisionEnvironment() );
				m_myCollisionDetails.pushBack( destElem );
			}
			return;
		case HCGNormal:
			// Collision with pinned meshes
			if( collidable.getType() == HK_BROAD_PHASE_ENTITY )
			{
				hkRigidBody* rb = static_cast<hkRigidBody*>(collidable.getOwner());
				if( rb->isKeyframed() )
				{
                    m_setTag("Physics");
					hkSimpleClosestPointsDetails* destElem = new hkSimpleClosestPointsDetails( collidable, m_collidable, *m_owner->getCollisionEnvironment() );
					m_myCollisionDetails.pushBack( destElem );
					++m_collidingMeshes;
				}
				return;
			}
		case HCGPlayerBody:
			// Collision between players
			if( m_ourBody && ( collidable.getOwner() != m_ourBody ) && ( collidable.getType() == HK_BROAD_PHASE_ENTITY ) )
			{
				hkRigidBody* rb = static_cast<hkRigidBody*>(collidable.getOwner());
				for( int i = m_collidingPlayers.getSize() - 1; i >= 0; --i )
					if( m_collidingPlayers[i] == rb )
						return;
				m_collidingPlayers.pushBack( rb );
			}
			return;
/*
		case HCGParticles:
			// Collision between player, take 2
			if( ( collidable.getOwner() != m_ourBody ) && ( collidable.getType() == HK_BROAD_PHASE_PHANTOM ) )
			{
                m_setTag("Physics");
				hkSimpleClosestPointsDetails* destElem = new hkSimpleClosestPointsDetails( collidable, m_collidable, *m_owner->getCollisionEnvironment() );
				m_myCollisionDetails.pushBack( destElem );
			}
			return;
*/
		default:
			return;
		}
	}

	virtual void removeOverlappingCollidable( hkCollidable& collidable )
	{
		if( collidable.getShape() == HK_NULL )
			return;

		int collGroup = collidable.getCollisionFilterInfo();
		switch( collGroup )
		{
		case HCGFixed:
		case HCGBarrier:
			// Collision with fixed geometry - yes
			for( int i = m_myCollisionDetails.getSize() - 1; i >= 0; --i )
			{
				if( &m_myCollisionDetails[i]->getCollidableA() == &collidable )
				{
					m_myCollisionDetails[i]->destroy( *m_owner->getCollisionEnvironment() );
					m_myCollisionDetails[i] = m_myCollisionDetails[m_myCollisionDetails.getSize()-1];
					m_myCollisionDetails.popBack();
					return;
				}
			}
			return;
		case HCGNormal:
			// Collision with pinned meshes
			if( collidable.getType() == HK_BROAD_PHASE_ENTITY )
			{
				hkRigidBody* rb = static_cast<hkRigidBody*>(collidable.getOwner());
				if( rb->isKeyframed() )
				{
					for( int i = m_myCollisionDetails.getSize() - 1; i >= 0; --i )
					{
						if( &m_myCollisionDetails[i]->getCollidableA() == &collidable )
						{
							m_myCollisionDetails[i]->destroy( *m_owner->getCollisionEnvironment() );
							m_myCollisionDetails[i] = m_myCollisionDetails[m_myCollisionDetails.getSize()-1];
							m_myCollisionDetails.popBack();
							--m_collidingMeshes;
							return;
						}
					}
				}
			}
			return;
		case HCGPlayerBody:
			// Collision between players
			if( m_ourBody && ( collidable.getOwner() != m_ourBody ) && ( collidable.getType() == HK_BROAD_PHASE_ENTITY ) )
			{
				hkRigidBody* rb = static_cast<hkRigidBody*>(collidable.getOwner());
				for( int i = m_collidingPlayers.getSize() - 1; i >= 0; --i )
					if( m_collidingPlayers[i] == rb )
					{
						m_collidingPlayers[i] = m_collidingPlayers[m_collidingPlayers.getSize()-1];
						m_collidingPlayers.popBack();
						return;
					}
			}
			return;
/*
		case HCGParticles:
			// Collision between player, take 2
			if( ( collidable.getOwner() != m_ourBody ) && ( collidable.getType() == HK_BROAD_PHASE_PHANTOM ) )
			{
				for( int i = m_myCollisionDetails.getSize() - 1; i >= 0; --i )
				{
					if( &m_myCollisionDetails[i]->getCollidableA() == &collidable )
					{
						m_myCollisionDetails[i]->destroy( *m_owner->getCollisionEnvironment() );
						m_myCollisionDetails[i] = m_myCollisionDetails[m_myCollisionDetails.getSize()-1];
						m_myCollisionDetails.popBack();
						return;
					}
				}
			}
			return;
*/
		default:
			return;
		}
	}
#else
// BLAIR - For now, we'll make this a null class...most of it will need to be redone I think
int updateCollisions( hkCollisionEnvironment& env )
{
}
virtual void addOverlappingCollidable( hkCollidable& collidable )
{
}
virtual void removeOverlappingCollidable( hkCollidable& collidable )
{
}
#endif
	void setPlayerBody( hkRigidBody* body )
	{
		m_ourBody = body;
		if( !m_ourBody )
			m_collidingPlayers.clear();
	}

	bool collidingWithMeshes( void ) { return ( m_collidingMeshes > 0 ); };


	hkArray<hkRigidBody*> m_collidingPlayers;

private:
#ifndef HAVOK3
	hkArray<hkSimpleClosestPointsDetails*> m_myCollisionDetails;
#endif
	hkRigidBody* m_ourBody;
	int m_collidingMeshes;
};

// *********************************************************************************************************************

HavokCharacterConstructionInfo::HavokCharacterConstructionInfo()
{
	m_position.setZero4();						// Character starts at the origin
	m_halfExtents.set(0.5f, 0.5f, 1.0f);		// 1m wide and 2m high
}

HavokCharacterController::HavokCharacterController( const HavokCharacterConstructionInfo& info, hkWorld* world, hkRigidBody* body ) :
	m_onGround( false ),
	m_world( world ),
	m_characterPhantom( 0 ),
	m_aabbCastShape( 0 ),

	m_msSinceExplosion(10000),
	m_msSinceJumped(0),
	m_msSinceLanded(0),
	m_speed(0),
	m_timeDelta(0.f),
	m_internalTimeDelta(0),
	m_action(0),
	m_lastNonzeroAction(1<<EInputMoveForward),
	m_jumpAction(0),
	m_fallSpeed(0.f),
	m_jumpInvalidation( false ),
	m_falling( true ),
	m_canBunnyHop( false ),
	m_hasJumpVector( false ),
#ifdef PLAYER_CONTROLLER_TEST
	m_file(0),
#endif
	m_velocityX(0),
	m_velocityY(0),
	m_velocityZ(0),
	m_externalInfluences(0),
	m_lastStateReceived(0),
	m_body(body),
	MPByte(0),
	m_cameraVerticalCatchUp(0.f),
	m_receivedRotation(0),
	m_lastRotation(0)
{
	m_state.deltaMs					= 0;
	m_state.position				= info.m_position;
	m_state.velocity.setZero4();
	m_state.lastHeightOfStep		= 0;
	m_state.cameraVerticalCatchUp	= 0;

	mLastPos						= info.m_position;

	m_stepNormal					= cos( GEngine->Physics->TWSlopeAngleToSlide*F_DEG2RAD );
	m_halfExtents					= info.m_halfExtents;

#ifdef HAVOK3
	hkTransform	transform;
	transform.setIdentity();

    m_setTag("Physics");
	hkShape* capsuleShape = new hkCapsuleShape(m_state.position, m_state.position, m_halfExtents(0));
    m_setTag("Physics");
	m_characterPhantom = new hkMyCharPhantom( capsuleShape, transform, hkGroupFilter::calcFilterInfo( HCGParticles ) );
#else
	hkCharacterPhantomConstructionInfo pci;	
	pci.m_min.setSub4(m_state.position, m_halfExtents );
	pci.m_max.setAdd4(m_state.position, m_halfExtents );

    m_setTag("Physics");
	m_aabbCastShape = new hkAabbCastShape( m_state.position, m_state.position, m_halfExtents );
	pci.m_shape = m_aabbCastShape;
	pci.m_transform.setIdentity();
	pci.m_collisionFilter = world->getCollisionFilter();
	pci.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( HCGParticles );

    m_setTag("Physics");
	m_characterPhantom = new hkMyCharPhantom( pci );
#endif
	m_characterPhantom->setPlayerBody( m_body );
	m_world->addPhantom( m_characterPhantom );
	m_lastKnownGoodPosition = m_state.position;

//	if( getEntity()->PO )
//		getEntity()->PO->mPlayerVariables->LastGoodPosition = Vector( m_state.position(0), m_state.position(1), m_state.position(2) );
//	the above is not needed

}

HavokCharacterController::~HavokCharacterController()
{
#ifndef HAVOK3 // =========================================
	if( m_aabbCastShape )
		m_aabbCastShape->removeReference();

	if( m_characterPhantom )
		m_characterPhantom->removeReference();

#ifdef PLAYER_CONTROLLER_TEST
	if( m_file )
	{
		const unsigned char marker = 0x00;
		m_file->Write( &marker, sizeof( unsigned char ) );
		delete m_file;
		m_file = 0;
	}
#endif

#else
	GLog.Print("HavokCharacterController::~HavokCharacterController() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 ========================================
}

void HavokCharacterController::setPlayerBody( hkRigidBody* body )
{
	m_body = body;
	if( m_characterPhantom )
		m_characterPhantom->setPlayerBody( body );
}

void HavokCharacterController::setPosition( const hkVector4& targetPositionWS )
{
	GLog.Print("HavokCharacterController::setPosition = %f, %f %f\n", targetPositionWS(0), targetPositionWS(1), targetPositionWS(2));
	m_state.position = targetPositionWS;
	getEntity()->PO->mPlayerVariables->LastGoodPosition = Vector( m_state.position(0), m_state.position(1), m_state.position(2) );
	m_state.lastHeightOfStep = 0;
	correctPosition();
	++m_externalInfluences;
	m_state.cameraVerticalCatchUp = 0;
}

void HavokCharacterController::applyCharacter( MoveState& state )
{
#ifndef HAVOK3 // =========================================
	hkVector4 deltaPos;
	deltaPos.setMul4( state.deltaMs/1000.f, state.velocity );

	m_maxStepHeight = m_falling?GEngine->Physics->TWMPMaxStepHeightWhileInAir:GEngine->Physics->TWMPMaxStepHeight;

	// Compute new bb
	hkVector4 minExt;
	hkVector4 maxExt;
	{
		// Expand by the displacement vector when necessary
		hkVector4 minToWorld = state.position;
		hkVector4 maxToWorld = state.position;

		for( int i = 0; i < 3; ++i )
			if( deltaPos(i) < 0 )
				minToWorld(i) += deltaPos(i) - GEngine->Physics->TWMPPhantomTolerance;
			else
				maxToWorld(i) += deltaPos(i) + GEngine->Physics->TWMPPhantomTolerance;

		// Expand by the step height
		maxToWorld(1) += m_maxStepHeight;

		minExt.setSub4( minToWorld, m_halfExtents );
		maxExt.setAdd4( maxToWorld, m_halfExtents );
	}
	const hkAabb aabb( minExt, maxExt );
	m_characterPhantom->updateAabb( aabb );
	m_aabbCastShape->setAabb( aabb );

//	GLog.Print( "\n" );

//	GLog.Print( "Position: (%f,%f,%f)\n", state.position(0), state.position(1), state.position(2) );

	MoveStateInternal first;
	first.deltaMs = state.deltaMs;
	first.lastHeightOfStep = state.position(1)-state.lastHeightOfStep*0.01f;
	first.position = state.position;
	first.velocity = state.velocity;
	first.cameraVerticalCatchUp = state.cameraVerticalCatchUp;

//	hkVector4 veldelta( state.velocity );
//	veldelta(1) = 0;
//	veldelta.mul4( state.deltaMs/1000.f );
//	GLog.Print( "deltaMS: %u, len: %f\n", state.deltaMs, veldelta.length3() );

	if( slideMove( first, true ) )
	{
		MoveStateInternal second;
		second.deltaMs = state.deltaMs;
		second.lastHeightOfStep = state.position(1)-state.lastHeightOfStep*0.01f;
		second.position = state.position;
		second.velocity = state.velocity;
		second.cameraVerticalCatchUp = state.cameraVerticalCatchUp;

		slideMove( second, false );

		hkVector4 diffPos( state.position );
		diffPos.sub4( first.position );
		diffPos(1) = 0;
		hkReal firstDist = diffPos.lengthSquared3();
		diffPos = state.position;
		diffPos.sub4( second.position );
		diffPos(1) = 0;
		hkReal secondDist = diffPos.lengthSquared3();
//		GLog.Print( "Dist1: %f. dist2: %f\n", firstDist, secondDist );
//		GLog.Print( "Stairs: %f, no stairs: %f, choosing", firstDist, secondDist );
		if( secondDist < firstDist )
		{
//			GLog.Print( " stairs" );
//			GLog.Print( "choosing first, first ms: %u, second ms: %u\n", first.deltaMs, second.deltaMs );
			state.deltaMs = first.deltaMs;
			state.lastHeightOfStep = ( first.position(1) > first.lastHeightOfStep ) ? floor((first.position(1)-first.lastHeightOfStep)*100.f) : 0;
			state.position = first.position;
			state.velocity = first.velocity;
			state.cameraVerticalCatchUp = first.cameraVerticalCatchUp;
		}
		else
		{
//			GLog.Print( " no stairs" );
//			GLog.Print( "choosing second, first ms: %u, second ms: %u\n", first.deltaMs, second.deltaMs );
			state.deltaMs = second.deltaMs;
			state.lastHeightOfStep = ( second.position(1) > second.lastHeightOfStep ) ? floor((second.position(1)-second.lastHeightOfStep)*100.f) : 0;
			state.position = second.position;
			state.velocity = second.velocity;
			state.cameraVerticalCatchUp = second.cameraVerticalCatchUp;
		}
	}
	else
	{
//		GLog.Print( "No stairs" );
		state.deltaMs = first.deltaMs;
		state.lastHeightOfStep = ( first.position(1) > first.lastHeightOfStep ) ? floor((first.position(1)-first.lastHeightOfStep)*100.f) : 0;
		state.position = first.position;
		state.velocity = first.velocity;
		state.cameraVerticalCatchUp = first.cameraVerticalCatchUp;
	}
//	GLog.Print( ", deltaMs %u, left %u\n", lastDeltaMs, state.deltaMs );

	updateCameraInterpolation( state );
#else
	GLog.Print("HavokCharacterController::applyCharacter() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 ======================
}

void HavokCharacterController::updateCameraInterpolation( MoveState& state )
{
	if( state.cameraVerticalCatchUp <= 0 )
		return;
	
	if( state.deltaMs >= GEngine->Physics->TWMPMaxTimeToZeroCamera )
		state.cameraVerticalCatchUp = 0;
	else
	{
		hkReal minFix = state.cameraVerticalCatchUp*(((float)state.deltaMs)/GEngine->Physics->TWMPMaxTimeToZeroCamera);
		if( minFix < GEngine->Physics->TWMPMinCameraCatchup )
			minFix = GEngine->Physics->TWMPMinCameraCatchup;
		if( state.cameraVerticalCatchUp <= minFix )
			state.cameraVerticalCatchUp = 0;
		else
			state.cameraVerticalCatchUp -= minFix;
		if( state.cameraVerticalCatchUp > GEngine->Physics->TWMPMaxCameraDiff )
			state.cameraVerticalCatchUp = GEngine->Physics->TWMPMaxCameraDiff;
	}
}

void HavokCharacterController::applyCharacterLadder( MoveState& state )
{
#ifndef HAVOK3 // =========================================
	hkVector4 deltaPos;
	deltaPos.setMul4( state.deltaMs/1000.f, state.velocity );

	// Compute new bb
	hkVector4 minExt;
	hkVector4 maxExt;
	{
		// Expand by the displacement vector when necessary
		hkVector4 minToWorld = state.position;
		hkVector4 maxToWorld = state.position;

		for( int i = 0; i < 3; ++i )
			if( deltaPos(i) < 0 )
				minToWorld(i) += deltaPos(i) - GEngine->Physics->TWMPPhantomTolerance - MAX_BACKWARD_DISTANCE;
			else
				maxToWorld(i) += deltaPos(i) + GEngine->Physics->TWMPPhantomTolerance + MAX_BACKWARD_DISTANCE;

		minExt.setSub4( minToWorld, m_halfExtents );
		maxExt.setAdd4( maxToWorld, m_halfExtents );
	}
	const hkAabb aabb( minExt, maxExt );
	m_characterPhantom->updateAabb( aabb );
	m_aabbCastShape->setAabb( aabb );

	unsigned char lastDeltaMs;

	do
	{
		lastDeltaMs = state.deltaMs;

		MoveStateInternal first;
		first.deltaMs = state.deltaMs;
		first.position = state.position;
		first.velocity = state.velocity;
		if( ladderMove( first, true ) )
		{
			MoveStateInternal second;
			second.deltaMs = state.deltaMs;
			second.position = state.position;
			second.velocity = state.velocity;

			ladderMove( second, false );

			hkVector4 diffPos( state.position );
			diffPos.sub4( first.position );
			hkReal firstDist = diffPos.lengthSquared3();
			diffPos = state.position;
			diffPos.sub4( second.position );
			if( diffPos.lengthSquared3() < firstDist + 0.005f )
			{
				state.deltaMs = first.deltaMs;
				state.lastHeightOfStep = first.position(1);
				state.position = first.position;
				state.velocity = first.velocity;
			}
			else
			{
				state.deltaMs = second.deltaMs;
				state.lastHeightOfStep = second.position(1);
				state.position = second.position;
				state.velocity = second.velocity;
			}
		}
		else
		{
			state.deltaMs = first.deltaMs;
			state.lastHeightOfStep = first.position(1);
			state.position = first.position;
			state.velocity = first.velocity;
		}

		if( !state.deltaMs )
		{
			updateCameraInterpolation( state );
			return;
		}
	}
	while( lastDeltaMs > state.deltaMs );

	if( state.deltaMs )
		state.velocity.setZero4();	// since we couldn't go further, the velocity for the rest of delta time was 0
	updateCameraInterpolation( state );
#else
	GLog.Print("HavokCharacterController::applyCharacterLadder() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 ===============================

}

// Returns distance that we went up; might be 0.f

hkReal HavokCharacterController::upAStep( MoveStateInternal& state )
{
#ifndef HAVOK3 // BLAIR ### -----------------------------------------
	if( !m_onGround )
		state.lastHeightOfStep = state.position(1);	// this allows one step stair walk from jump
	else if( m_maxStepHeight + state.lastHeightOfStep <= state.position(1) )
		return 0.f;

	//	Attempt to move up by the step height
	hkVector4 upWorld( state.position );
	upWorld(1) = m_maxStepHeight+state.lastHeightOfStep;

	m_aabbCastShape->setFromWorld( state.position );
	m_aabbCastShape->setToWorld( upWorld );

	// Do AABB cast
	int minindex = m_characterPhantom->updateCollisions( *m_world->getCollisionEnvironment() );
	hkReal mindist = 1.f;

	// If we hit something
	if( minindex >= 0 )
	{
		mindist = m_characterPhantom->getCollisionDetails()[minindex]->getClosestDistance();
		if( mindist <= 0.f )			//this means penetration
		{
			mindist = 0.f;
			return 0.f;
		}
	}

	// Take the real vertical velocity away temporarily
	m_tempVelocity = state.velocity(1);
	m_tempDeltaMs = state.deltaMs;
	if( state.velocity(1) < 0.f )
		state.velocity(1) = 0.f;

	// Interpolate our new position
	hkReal distanceUp = state.position(1);
	state.position.setInterpolate4( state.position, upWorld, mindist );

	return state.position(1)-distanceUp;
#else
	GLog.Print("HavokCharacterController::upAStep() - FIXME!!!!!!!!!!!\n");
	return 0;
#endif //ifndef HAVOK3 ## BLAIR -----------------------------------
}

void HavokCharacterController::downAStep( MoveStateInternal& state, hkReal distanceDown )
{
	// BLAIR - Time To Take Evasive manuevers!  WILL LOOK AT THIS LATER!
#ifndef HAVOK3 // ========================================================
	// Attempt to move back down again
	hkVector4 downWorld( state.position );
	downWorld(1) -= distanceDown;

	m_aabbCastShape->setFromWorld( state.position );
	m_aabbCastShape->setToWorld( downWorld );

	// Do AABB cast
	int minindex = m_characterPhantom->updateCollisions( *m_world->getCollisionEnvironment() );
	hkReal mindist = 1.f;

	// If we hit something
	if( minindex >= 0 )
	{
		mindist = m_characterPhantom->getCollisionDetails()[minindex]->getClosestDistance();
		if( mindist < 0.f )			//this means penetration
			mindist = 0.f;

		// Interpolate our new position
		state.position.setInterpolate4( state.position, downWorld , mindist );

		// Update last step height
		if( state.lastHeightOfStep > state.position(1) )
			state.lastHeightOfStep = state.position(1);
		else
		{
			int cdminindex = m_characterPhantom->getCollisionDetails()[minindex]->getClosestIndex();
			hkVector4 normal( m_characterPhantom->getCollisionDetails()[minindex]->getContactPoints()[cdminindex].getNormal() );
			normal.normalize3();
			if( hkMath::fabs(normal(1)) > m_stepNormal )
				state.lastHeightOfStep = state.position(1);
		}

		hkReal steppedUp = state.position(1)-downWorld(1);
		if( steppedUp > 0.f )
			state.cameraVerticalCatchUp += steppedUp;
	}
	else
		state.position = downWorld;

	// Give the vertical velocity back
	state.velocity(1) = m_tempVelocity;
	state.deltaMs = m_tempDeltaMs;
#else
	GLog.Print("HavokCharacterController::downAStep() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 ===================================
}

// Returns distance that we went back; might be 0.f

bool HavokCharacterController::backAStep( hkVector4& position, hkVector4& velocity, hkVector4& moveVector )
{
#ifndef HAVOK3 // =====================================================
	// Determine the backwards direction
	hkVector4 dirVector( velocity );
	dirVector(1) = 0.f;
	dirVector.normalize3();
	Assert( dirVector.lengthSquared3() > 0.f );
#ifdef	TRANSGAMING_PS2
	dirVector.mul4( -MAX_BACKWARD_DISTANCE/hkReal(dirVector.length3()) );
#else
	dirVector.mul4( -MAX_BACKWARD_DISTANCE/dirVector.length3() );
#endif

	//	Attempt to move backwards
	hkVector4 backWorld( position );
	backWorld.add4( dirVector );

	m_aabbCastShape->setFromWorld( position );
	m_aabbCastShape->setToWorld( backWorld );

	// Do AABB cast
	int minindex = m_characterPhantom->updateCollisions( *m_world->getCollisionEnvironment() );
	hkReal mindist = 1.f;

	// If we hit something
	if( minindex >= 0 )
	{
		mindist = m_characterPhantom->getCollisionDetails()[minindex]->getClosestDistance();
		if( mindist < 0.f )			//this means penetration
			mindist = 0.f;
	}

	// Interpolate our new position
	position.setInterpolate4( position, backWorld, mindist );
	moveVector.setMul4( mindist, dirVector );

	return ( mindist > 0.f );
#else
	GLog.Print("HavokCharacterController::backAStep() - FIXME!!!!!!!!!!!\n");
	return true;
#endif // ifndef HAVOK3 =============================================

}

void HavokCharacterController::forwardAStep( hkVector4& position, hkVector4& movedBackwards )
{
#ifndef HAVOK3 // ===============================================
	// Attempt to move back forward again
	hkVector4 forwardWorld( position );
	forwardWorld.sub4( movedBackwards );

	m_aabbCastShape->setFromWorld( position );
	m_aabbCastShape->setToWorld( forwardWorld );

	// Do AABB cast
	int minindex = m_characterPhantom->updateCollisions( *m_world->getCollisionEnvironment() );
	hkReal mindist = 1.f;

	// If we hit something
	if( minindex >= 0 )
	{
		mindist = m_characterPhantom->getCollisionDetails()[minindex]->getClosestDistance();
		if( mindist < 0.f )			//this means penetration
			mindist = 0.f;

		// Interpolate our new position
		position.setInterpolate4( position, forwardWorld , mindist );
	}
	else
		position = forwardWorld;
#else
	GLog.Print("HavokCharacterController::forwardAStep() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =====================================
}

// The return of this says if it stepped up

bool HavokCharacterController::slideMove( MoveStateInternal& state, bool stepUp )
{
	// Early exit for dummy requests to move
	if( !state.deltaMs )
		return false;

	hkReal steppedUp = 0.f;
#ifndef HAVOK3 //=============================================
	hkVector4 initialVelocity( state.velocity );
	hkVector4 prevNormal(0,0,0);

	// First, find out colliding players
	if( m_characterPhantom->m_collidingPlayers.getSize() )
	{
		float minCross = -10000.f;
		float maxCross = 10000.f;
		int minIndex = -1;
		int maxIndex = -1;
		hkVector4 normal;

		bool playerAbove = false;
		bool playerBelow = false;

		hkVector4 horVel = state.velocity;
		horVel(1) = 0;
		horVel.normalize3();

		for( int i = 0; i < m_characterPhantom->m_collidingPlayers.getSize(); ++i )
		{
			hkRigidBody* rb = m_characterPhantom->m_collidingPlayers[i];
			hkVector4 rbPos = rb->getPosition();
			hkVector4 posDiff( rbPos );
			posDiff.sub4( state.position );
			hkReal posDiffY = posDiff(1);
			posDiff(1) = 0;

			if( posDiff.length3() > 2*m_halfExtents(0) )
				continue;
			if( horVel.dot3( posDiff ) <= 0 )
				continue;

			if( posDiffY >= 0 )
				playerAbove = true;
			else
				playerBelow = true;

			posDiff.normalize3();
			rbPos.setCross( horVel, posDiff );	// its cross from now on
			if( rbPos(1) < 0 )
			{
				if( rbPos(1) > minCross )
				{
					minCross = rbPos(1);
					minIndex = i;
					normal = posDiff;
					if( maxIndex != -1 )	// both sides blocked
					{
						++m_externalInfluences;
						return false;
					}
				}
			}
			else
			{
				if( rbPos(1) <= maxCross )
				{
					maxCross = rbPos(1);
					maxIndex = i;
					normal = posDiff;
					if( minIndex != -1 )	// both sides blocked
					{
						++m_externalInfluences;
						return false;
					}
				}
			}
		}

		if( ( minIndex != -1 ) || ( maxIndex != -1 ) )
		{
#ifdef	TRANSGAMING_PS2
			normal.mul4( hkReal(state.velocity.dot3( normal ))*OVERBOUNCE );
#else
			normal.mul4( state.velocity.dot3( normal )*OVERBOUNCE );
#endif
			state.velocity.sub4( normal );
			++m_externalInfluences;

			if( playerAbove && state.velocity(1) > 0.f )
				state.velocity(1) = 0.f;
			if( playerBelow && state.velocity(1) < 0.f )
				state.velocity(1) = 0.f;
		}
	}

	// Try to slide along up to 4 planes
	for( int i = 0; i < 4; ++i )
	{
		hkVector4 deltaPos;
		deltaPos.setMul4( state.deltaMs/1000.f, state.velocity );
		hkVector4 destPos( state.position );
		destPos.add4( deltaPos );

		// Perform an AABB cast
		m_aabbCastShape->setFromWorld( state.position );
		m_aabbCastShape->setToWorld( destPos );

		int minindex = m_characterPhantom->updateCollisions( *m_world->getCollisionEnvironment() );

		hkReal mindist = 1.f;

		// If we hit an object
		if( minindex >= 0 )
		{
			mindist = m_characterPhantom->getCollisionDetails()[minindex]->getClosestDistance();

			if( mindist < 0.f )				// This means penetration
				mindist = 0.f;

			hkVector4	hitNormalWorld;

			int cdminindex = m_characterPhantom->getCollisionDetails()[minindex]->getClosestIndex();
			hitNormalWorld = m_characterPhantom->getCollisionDetails()[minindex]->getContactPoints()[cdminindex].getNormal();
			hitNormalWorld.normalize3();

			// Move the AABB to the point of contact
			float timeOfMove = state.deltaMs * mindist;
			state.deltaMs -= floor(timeOfMove);
			state.position.setInterpolate4( state.position, destPos, mindist );

			// Add in gravity during that distance
			hkVector4 grav(0, -9.81f * 3, 0);
			grav.mul4( timeOfMove/1000.f );
			state.velocity.add4( grav );

			bool verticalObject = ( hkMath::fabs(hitNormalWorld(1)) < m_stepNormal );

			if( stepUp && ( steppedUp == 0.f ) && verticalObject )
			{
				if( ( steppedUp = upAStep( state ) ) > 0.f )
					continue;
				else
					stepUp = false;	// so we won't try to step up again if we failed once
			}

			// Kill all velocity in the direction of the surface normal
			hkVector4 velNorm( hitNormalWorld );
#ifdef	TRANSGAMING_PS2
			velNorm.mul4( hkReal(state.velocity.dot3( hitNormalWorld ))*OVERBOUNCE );
#else
			velNorm.mul4( state.velocity.dot3( hitNormalWorld )*OVERBOUNCE );
#endif
			state.velocity.sub4( velNorm );

			// Check that our clipping and sliding has not caused us to retreat
			if( initialVelocity.dot3( state.velocity ) < 0.f )
				break;
		}
		else	// If we didn't hit anything we are unobstructed so we move the full remaining distance
		{
			state.position = destPos;
			break;
		}	
	}

	if( steppedUp != 0.f )
		downAStep( state, steppedUp );

	// Add in gravity during that distance
	hkVector4 grav( 0, -9.81f * 3, 0 );
	grav.mul4( state.deltaMs/1000.f );
	state.velocity.add4( grav );
	state.deltaMs = 0;
#else
	GLog.Print("HavokCharacterController::slideMove() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 ====================================
	return ( steppedUp != 0.f );
}

// The return of this says if it stepped backwards

bool HavokCharacterController::ladderMove( MoveStateInternal& state, bool stepBackwards )
{
#ifndef HAVOK3 // ==========================================

	// Early exit for dummy requests to move
	if( !state.deltaMs )
		return false;

	bool steppedBackwards = false;
	hkVector4 backVector;

	hkVector4 initialVelocity( state.velocity );
	hkVector4 prevNormal(0,0,0);

	// First, find out colliding players
	if( m_characterPhantom->m_collidingPlayers.getSize() )
	{
		float minCross = -10000.f;
		float maxCross = 10000.f;
		int minIndex = -1;
		int maxIndex = -1;
		hkVector4 normal;

		bool playerAbove = false;
		bool playerBelow = false;

		hkVector4 horVel = state.velocity;
		horVel(1) = 0;
		horVel.normalize3();

		for( int i = 0; i < m_characterPhantom->m_collidingPlayers.getSize(); ++i )
		{
			hkRigidBody* rb = m_characterPhantom->m_collidingPlayers[i];
			hkVector4 rbPos = rb->getPosition();
			hkVector4 posDiff( rbPos );
			posDiff.sub4( state.position );

			hkReal posDiffY = posDiff(1);
			posDiff(1) = 0;

			if( posDiff.length3() > 2*m_halfExtents(0) )
				continue;
			if( horVel.dot3( posDiff ) <= 0 )
				continue;

			if( posDiffY >= 0 )
				playerAbove = true;
			else
				playerBelow = true;

			posDiff.normalize3();
			rbPos.setCross( horVel, posDiff );	// its cross from now on
			if( rbPos(1) < 0 )
			{
				if( rbPos(1) > minCross )
				{
					minCross = rbPos(1);
					minIndex = i;
					normal = posDiff;
					if( maxIndex != -1 )	// both sides blocked
					{
						++m_externalInfluences;
						return false;
					}
				}
			}
			else
			{
				if( rbPos(1) <= maxCross )
				{
					maxCross = rbPos(1);
					maxIndex = i;
					normal = posDiff;
					if( minIndex != -1 )	// both sides blocked
					{
						++m_externalInfluences;
						return false;
					}
				}
			}
		}

		if( ( minIndex != -1 ) || ( maxIndex != -1 ) )
		{
#ifdef	TRANSGAMING_PS2
			normal.mul4( hkReal(state.velocity.dot3( normal ))*OVERBOUNCE );
#else
			normal.mul4( state.velocity.dot3( normal )*OVERBOUNCE );
#endif
			state.velocity.sub4( normal );
			++m_externalInfluences;

			if( playerAbove && state.velocity(1) > 0.f )
				state.velocity(1) = 0.f;
			if( playerBelow && state.velocity(1) < 0.f )
				state.velocity(1) = 0.f;
		}
	}

	// Try to slide along up to 4 planes
	for( int i = 0; i < 4; ++i )
	{
		hkVector4 deltaPos;
		deltaPos.setMul4( state.deltaMs/1000.f, state.velocity );
		hkVector4 destPos( state.position );
		destPos.add4( deltaPos );

		// Perform an AABB cast
		m_aabbCastShape->setFromWorld( state.position );
		m_aabbCastShape->setToWorld( destPos );

		int minindex = m_characterPhantom->updateCollisions( *m_world->getCollisionEnvironment() );

		hkReal mindist = 1.f;

		// If we hit an object
		if( minindex >= 0 )
		{
			mindist = m_characterPhantom->getCollisionDetails()[minindex]->getClosestDistance();

			if( mindist < 0.f )				// This means penetration
				mindist = 0.f;

			hkVector4	hitNormalWorld;

			int cdminindex = m_characterPhantom->getCollisionDetails()[minindex]->getClosestIndex();
			hitNormalWorld = m_characterPhantom->getCollisionDetails()[minindex]->getContactPoints()[cdminindex].getNormal();
			hitNormalWorld.normalize3();

			// Move the AABB to the point of contact
			float timeOfMove = state.deltaMs * mindist;
			state.deltaMs -= floor(timeOfMove);
			state.position.setInterpolate4( state.position, destPos, mindist );

			if( stepBackwards && ( steppedBackwards == 0.f ) )
			{
				if( hkMath::fabs(hitNormalWorld(1)) < m_stepNormal )
				{
					if( ( steppedBackwards = backAStep( state.position, state.velocity, backVector ) ) == true )
						continue;
					else
						stepBackwards = false;	// so we won't try to step backwards again if we failed once
				}
			}

			// Kill all velocity in the direction of the surface normal
			hkVector4 velNorm( hitNormalWorld );
#ifdef	TRANSGAMING_PS2
			velNorm.mul4( hkReal(state.velocity.dot3( hitNormalWorld ))*OVERBOUNCE );
#else
			velNorm.mul4( state.velocity.dot3( hitNormalWorld )*OVERBOUNCE );
#endif
			state.velocity.sub4( velNorm );

			// Check that our clipping and sliding has not caused us to retreat
			if( initialVelocity.dot3( state.velocity ) < 0.f )
			{
				if( steppedBackwards )
				{
					forwardAStep( state.position, backVector );	// we stop here
					return true;
				}
				else
					return false;
			}

			// Interact with objects we have hit
//			hkSimpleClosestPointsDetails* cpd = m_characterPhantom->getCollisionDetails()[minindex];
//			applyImpact( cpd );
		}
		else	// If we didn't hit anything we are unobstructed so we move the full remaining distance
		{
			state.position = destPos;

			if( steppedBackwards )
				forwardAStep( state.position, backVector );
			state.deltaMs = 0;

			return steppedBackwards;
		}	
	}

	if( steppedBackwards )
	{
		forwardAStep( state.position, backVector );
		return true;
	}
	else
		return false;
#else
	GLog.Print("HavokCharacterController::ladderMove() - FIXME!!!!!!!!!!!\n");
	// BLAIR
	return false;
#endif // ifndef HAVOK ===========================
}

void HavokCharacterController::playerMove( bool fromExternalData )
{
#ifndef HAVOK3 // ========================================================
	Assert( m_body );
	if( !m_body->getSimulationIsland() )
		return;

	int lastState = GProfiler.GetState();
	GProfiler.SetState(PHYSICS_TICK_PLAYER_CONTROLLER);

	m_predicting = false;

	if( m_characterPhantom->collidingWithMeshes() )
		++m_externalInfluences;
//	if( m_characterPhantom && m_characterPhantom->m_collidingPlayers.getSize() )
//		GLog.Print( "Me: %d, colliding players: %d\n", (int)m_body, m_characterPhantom->m_collidingPlayers.getSize() );
			
	if( fromExternalData )	// Move on external data
	{
		m_action = LastMove.Action;
		m_yaw = LastMove.Yaw;
		m_pitch = LastMove.Pitch;
		m_internalTimeDelta = LastMove.TimeDelta;
	}
	else	// Save last move
	{
		LastMove.Action = m_action;
		LastMove.Yaw = m_yaw;
		LastMove.TimeDelta = m_internalTimeDelta;
		LastMove.Pitch = m_pitch;
	}

	getEntity()->PO->mPlayerVariables->LastGoodPosition = Vector( m_state.position(0), m_state.position(1), m_state.position(2) );
/*
	GLog.Print( "######################### Action ############################\n" );
	PlayerInput input;
	input.GetFromPlayer( getEntity()->PO );
	input.Print( GLog );

	GLog.Print( "######################### Input\n" );
	PlayerState state;
	state.GetFromPlayer( getEntity()->PO );
	state.Print( GLog );
*/
#ifdef PLAYER_CONTROLLER_TEST
	SaveInput();
#endif

	ulong32 fpucontrol = _controlfp( 0, 0 );
#ifdef TRANSGAMING_PS2
	_controlfp( _PS2_64, _MCW_PS2 );
#else
	_controlfp( _PC_64, _MCW_PC );
#endif
	internalPlayerMove();
//	internalPlayerMovePromode();

#ifdef TRANSGAMING_PS2
	_controlfp( fpucontrol, _MCW_PS2 );
#else
	_controlfp( fpucontrol, _MCW_PC );
#endif

	// Transfer results to player body
	m_cameraVerticalCatchUp = m_state.cameraVerticalCatchUp;
	m_body->setPosition( m_state.position );

	m_body->setKeyframed(true);
	m_action = 0;

#ifdef PLAYER_CONTROLLER_TEST
	SaveFull();
#endif
/*
	GLog.Print( "######################### Output\n" );
	state.GetFromPlayer( getEntity()->PO );
	state.Print( GLog );
*/

	GProfiler.SetState(lastState);
#else
	GLog.Print("HavokCharacterController::playerMove() - FIXME!!!!!!!!!!!\n");
#endif //  ifndef HAVOK3 =====================================
}

void HavokCharacterController::internalPlayerMove( void )
{
#ifndef HAVOK3 // ========================================================
	if( m_internalTimeDelta > MAX_POSSIBLE_TIME_MS_FOR_STEP )
	{
		PlayerInput input;
		input.Action = m_action;
		input.Yaw = m_yaw;
		input.Pitch = m_pitch;
		input.TimeDelta = m_internalTimeDelta;
		m_internalTimeDelta = input.TimeDelta/2;
		internalPlayerMove();
		m_action = input.Action;
		m_yaw = input.Yaw;
		m_pitch = input.Pitch;
		m_internalTimeDelta = input.TimeDelta - input.TimeDelta/2;
		internalPlayerMove();
		return;
	}

	// Get time delta
	unsigned char actionDelta = m_internalTimeDelta;
	m_internalTimeDelta = 0;

	if( !m_speed )
		m_speed = GEngine->Physics->TWMPPlayerSpeed*1000.f;

	// Only those actions interest us
	m_action &= ((1<<EInputJump)|(1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));

#ifdef FULL_DEBUG_PRINT
	String debugString;
	debugString += String::Sprintf( "Velocity: (%f,%f,%f)\n", m_state.velocity(0),m_state.velocity(1),m_state.velocity(2) );

	debugString += "Action: ";
	if( m_action & (1<<EInputJump) )
		debugString += "J";
	if( m_action & (1<<EInputMoveForward) )
		debugString += "F";
	if( m_action & (1<<EInputMoveBackward) )
		debugString += "B";
	if( m_action & (1<<EInputMoveLeft) )
		debugString += "L";
	if( m_action & (1<<EInputMoveRight) )
		debugString += "R";
#endif

	// Prepare velocity for this move
	velIn();

	// Ladder test
	bool nearLadder = false;
	{
		Vector ladderTestPos;
		ladderTestPos.Set( m_state.position(0), m_state.position(1)-m_halfExtents(1), m_state.position(2) );
#ifdef	TRANSGAMING
		nearLadder = GEngine->pcfWorld->NearLadder( ladderTestPos );	// test feet
		if( !nearLadder )
		{
			ladderTestPos.Y += 10*m_halfExtents(1)/11;
			nearLadder = GEngine->pcfWorld->NearLadder( ladderTestPos );	// test head
		}
#else
		nearLadder = GEngine->World->NearLadder( ladderTestPos );	// test feet
		if( !nearLadder )
		{
			ladderTestPos.Y += 10*m_halfExtents(1)/11;
			nearLadder = GEngine->World->NearLadder( ladderTestPos );	// test head
		}
#endif
	}

	if( nearLadder && !( m_action&(1<<EInputJump) ) )
	{
		// determine desired move
		hkVector4 move(0,0,0);
		hkVector4 upVec(0,1,0);

		float yaw = m_yaw*F_PI/SHRT_MAX;
		hkVector4 forward( sin(yaw), 0, cos(yaw) );
		hkVector4 right;
		right.setCross( forward, upVec );
		forward(1) = tan(m_pitch*F_PI/2/SHRT_MAX);
		forward.normalize3();

		if( m_action & (1<<EInputMoveForward) )		move.add4( forward );
		if( m_action & (1<<EInputMoveBackward) )	move.sub4( forward );
		if( m_action & (1<<EInputMoveRight) )		move.add4( right );
		if( m_action & (1<<EInputMoveLeft) )		move.sub4( right );

		if( move(1) > 0.3f )
			move(1) = GEngine->Physics->TWPlayerStairsUpSpeed;
		else if( move(1) < -0.3f )
			move(1) = -GEngine->Physics->TWPlayerStairsDownSpeed;
		else
			move(1) = 0.0f;

		hkVector4 vel;

		if( m_action & (1<<EInputJump) )
		{
			vel = upVec;
			vel.mul4( 0.7f*GEngine->Physics->TWMPPlayerJumpStrength );
			vel.mul4( GEngine->Physics->TWMPPlayerSpeed );
			vel.sub4( m_state.velocity );
		}
		else if( move.lengthSquared3() > 0 )
		{
			vel = move;
			vel.mul4( GEngine->Physics->TWMPPlayerSpeed );
			vel.sub4( m_state.velocity );
			vel.mul4( 0.2f );
		}
		else
		{
			m_state.velocity.mul4( 0.7f );
			m_action = 0;
			m_fallSpeed = m_state.velocity(1);

			m_state.deltaMs = actionDelta;
			applyCharacterLadder( m_state );

			// Transfer velocity to shorts
			velOut();
			return;
		}

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !vel.isOk4() )
#else
		if( !vel.isOk() )
#endif
			GLog.Print( "Corrupted force in HavokTickIndependentPlayerActionLadder\n" );
		else if( vel.length3() > 50000.0f )
			GLog.Print( "Force > 50000 in HavokTickIndependentPlayerActionLadder\n" );
#endif

		m_fallSpeed = m_state.velocity(1);
		m_state.velocity.add4(vel);
		m_state.deltaMs = actionDelta;
		applyCharacterLadder( m_state );

		// Transfer velocity to shorts
		velOut();
		return;
	}

	// Disable jump ability when falling too fast
	if( ( m_state.velocity(1) < GEngine->Physics->TWMPPlayerMinimalFallSpeedToDisableJump ) && !m_jumpInvalidation )
		m_action &= ~(1<<EInputJump);

	// Some easy variables
	const bool wasFalling = m_falling;

#ifdef FULL_DEBUG_PRINT
	debugString += wasFalling?"    wasFalling, ":"not wasFalling, ";
#endif

	hkVector4 floorNormal(0,1,0);

	correctPosition();
	m_onGround = floorCheck( floorNormal );
	bool falling = m_falling = !m_onGround && !nearLadder;

	const hkVector4 upVector(0,1,0);

#ifdef FULL_DEBUG_PRINT
	debugString += falling?"    falling, ":"not falling, ";
	debugString += m_onGround?"    onGround, ":"not onGround, ";
#endif

	hkVector4 fwdVel( m_state.velocity );
	fwdVel(1) = 0;

	// Check if we are sliding
	bool sliding = false;
	if( m_onGround )
	{
		float dot = floorNormal.dot3( upVector );

		if( dot <= 0.f )
		{
			correctNormal( floorNormal );
			dot = floorNormal.dot3( upVector );
		}

		sliding = ( dot < cos( GEngine->Physics->TWSlopeAngleToSlide*F_DEG2RAD ) );

		if( sliding )
		{
			if( !fwdVel.lengthSquared3() )
				sliding = false;
			else
				falling = true;
		}
	}

#ifdef FULL_DEBUG_PRINT
	debugString += sliding?"    sliding, ":"not sliding, ";
#endif

	// Desired move
	hkVector4 move(0,0,0);

	float yaw = m_yaw*F_PI/SHRT_MAX;
	hkVector4 forward( sin(yaw), 0, cos(yaw) );	// no need to normalize
	hkVector4 right;
	right.setCross( forward, upVector );

	if( m_action & (1<<EInputMoveForward) ) move.add4( forward );
	if( m_action & (1<<EInputMoveBackward) ) move.sub4( forward );
	if( m_action & (1<<EInputMoveRight) ) move.add4( right );
	if( m_action & (1<<EInputMoveLeft) ) move.sub4( right );
	move.normalize3();

	bool zeroFwdVel = ( fwdVel.lengthSquared3() <= 0.0001f );
	float airControl = ( zeroFwdVel || ( fwdVel.dot3( move ) > 0 ) )?GEngine->Physics->TWMPStrongAirControl:GEngine->Physics->TWMPWeakAirControl;
	bool stillShocked = ( m_msSinceExplosion < GEngine->Physics->TWMPPlayerShockedTime*1000.f );

#ifdef FULL_DEBUG_PRINT
	debugString += ( zeroFwdVel || ( fwdVel.dot3( move ) > 0 ) )?"   airControl, ":"no airControl, ";
#endif

	if( stillShocked )
		m_onGround = false;

	const float jumpStrength = GEngine->Physics->TWMPPlayerJumpStrength;
	const bool pressedJump = ( (m_action & (1<<EInputJump)) != 0 );
	if( !pressedJump )
		m_jumpInvalidation = false;

#ifdef FULL_DEBUG_PRINT
	debugString += pressedJump?"    pressedJump, ":"not pressedJump, ";
#endif

	// Adjust movement variables
	{
		unsigned int t = m_msSinceLanded + actionDelta;
		m_msSinceLanded = (t>255)?255:t;
	}

	// Adjust time since we pressed or released Jump button ( > 0 = time since presses, < 0 = time since released )
	if( pressedJump )
	{
		int t = m_msSinceJumped + actionDelta;
		m_msSinceJumped = (t>SHRT_MAX)?SHRT_MAX:t;
	}
	else
		m_msSinceJumped = 0;

	// Detect any jump
	bool jumped = pressedJump && !wasFalling && !m_jumpInvalidation && !sliding;

#ifdef FULL_DEBUG_PRINT
	debugString += jumped?"    jumped, ":"not jumped, ";
	debugString += m_jumpInvalidation?"    jump invalidation, ":"not jump invalidation, ";
#endif

	if( ( m_msSinceJumped > GEngine->Physics->TWMPSecondsWhenYouCanBunnyHopBeforeLanding*1000.f ) && falling )
		m_jumpInvalidation = true;

	if( m_action & ((1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight)) )
		m_lastNonzeroAction = m_action;

	// Detect bunny hop
	bool bunnyHopped = ( m_canBunnyHop && jumped );
	unsigned char bunnyHopDiff = m_msSinceLanded;

#ifdef FULL_DEBUG_PRINT
	debugString += bunnyHopped?"bunnyHopped, ":"not bunnyHopped, ";
#endif

	// Prepare data for air control
	if( stillShocked )		// Started falling from explosion
	{
		m_msSinceExplosion += actionDelta;
		m_jumpAction = m_lastNonzeroAction;
#ifdef	TRANSGAMING_PS2
		m_speed = hkReal(fwdVel.length3())*1000.f;
#else
		m_speed = fwdVel.length3()*1000.f;
#endif
		m_hasJumpVector = ( m_speed != 0 );
	}
	else if( jumped )						// Just jumped
	{
		m_canBunnyHop = false;
		m_hasJumpVector = (move.lengthSquared3() > 0.f );
		m_jumpAction = m_lastNonzeroAction;
	}
	else if( sliding )						// Sliding on something
	{
		m_canBunnyHop = false;
		m_hasJumpVector = false;
		m_jumpAction = 0;
	}
	else if( falling && !wasFalling )		// Started falling normally... walked off something
	{
		m_hasJumpVector = ( fwdVel.lengthSquared3() > 0.f );
		m_jumpAction = m_lastNonzeroAction;
	}

	// Report fall damage
	if( ( m_fallSpeed < m_state.velocity(1) ) && ( m_fallSpeed < -20.0f ) && ( m_fallSpeed-m_state.velocity(1) < -20.0f ) )
	{
		Entity* entity = 0;
		if( m_body->hasProperty( HPEntityPointer ) )
		{
			hkPropertyValue p = m_body->getProperty( HPEntityPointer );
#ifdef HAVOK3
			entity = (Entity*)p.getPtr();
#else
			entity = (Entity*)p.asPtr;
#endif
		}
		GScript.QueueMessage("PLAYER_HIT_GROUND", "%e,%f", entity, m_fallSpeed-m_state.velocity(1) );
	}

	// Just landed
    if( wasFalling && m_onGround )
	{
		m_msSinceLanded = 0;

		// Jumped just before landing? - bunny hop enable
		if( pressedJump && ( m_msSinceJumped < GEngine->Physics->TWMPSecondsWhenYouCanBunnyHopBeforeLanding*1000.f ) )
		{
			bunnyHopDiff = m_msSinceJumped;
			bunnyHopped = true;
		}
		else
			m_canBunnyHop = true;

		// Conserve speed for bunny hops
#ifdef	TRANSGAMING_PS2
		m_speed = hkReal(fwdVel.length3())*1000.f;
#else
		m_speed = fwdVel.length3()*1000.f;
#endif
	}

	// Keep speed in proper boundaries
	if( m_speed > GEngine->Physics->TWMPMaximalBunnyHopSpeed*1000.f )
		m_speed = GEngine->Physics->TWMPMaximalBunnyHopSpeed*1000.f;

	// Bunny Hop Acceleration and Stopping
	if( bunnyHopped )
	{
		m_canBunnyHop = false;

		float factor = 1.0f;
		if( GEngine->Physics->TWMPBunnyHopDifficulty > 0.0f )
			factor = 1.0f-bunnyHopDiff/1000.f*GEngine->Physics->TWMPBunnyHopDifficulty;
		if( factor < 0.0f )
			factor = 0.0f;

		if( m_speed < GEngine->Physics->TWMPMaximalBunnyHopSpeed*1000.f )
			m_speed += pow(GEngine->Physics->TWMPMaximalBunnyHopSpeed - m_speed/1000.f,1.4f)*GEngine->Physics->TWMPBunnyHopAcceleration*factor*1000.f;
		else
			m_speed = GEngine->Physics->TWMPMaximalBunnyHopSpeed*1000.f;
	}
	else if( ( m_msSinceLanded > GEngine->Physics->TWMPSecondsWhenYouCanBunnyHopAfterLanding*1000.f ) )
	{
		if( m_canBunnyHop )
		{
			m_canBunnyHop = false;
			m_speed = GEngine->Physics->TWMPPlayerSpeed*1000.f;
		}
		else if( !wasFalling && !falling )
			m_speed = GEngine->Physics->TWMPPlayerSpeed*1000.f;
	}

	if( sliding )
	{
		move = fwdVel;
		move.normalize3();
	}

	// Air control
	if( falling )
	{
		// Gather myJumpVector from JumpAction
		hkVector4 myPressVector(0,0,0);
		if( m_jumpAction & (1<<EInputMoveForward) ) myPressVector.add4( forward );
		if( m_jumpAction & (1<<EInputMoveBackward) ) myPressVector.sub4( forward );
		if( m_jumpAction & (1<<EInputMoveRight) ) myPressVector.add4( right );
		if( m_jumpAction & (1<<EInputMoveLeft) ) myPressVector.sub4( right );
		myPressVector.normalize3();

		float braker = -myPressVector.dot3(move);
		if( m_hasJumpVector && ( braker > 0.0f ) )
		{
			float subtract = GEngine->Physics->TWMPPlayerSlowdownDuringJump*actionDelta*braker;
			if( m_speed <= subtract )
				m_speed = 1000;
			else
				m_speed -= subtract;
			airControl = 1.0f;
		}
		move = myPressVector;
	}
	else if( m_speed < GEngine->Physics->TWMPPlayerSpeed*1000.f )		// Keep speed in proper boundaries
		m_speed = GEngine->Physics->TWMPPlayerSpeed*1000.f;

	float speed = m_speed/1000.f;

	hkVector4 vel;

	bool preventJumpyScreen = false;
	bool constSpeedWhenWalking = false;

	if( jumped )
	{
		float velUp = 8.0f*jumpStrength;
		if( velUp > m_state.velocity(1) )
			m_state.velocity(1) = velUp;
		vel.set(0,0,0);
	}
	else
	{
		if( falling )
		{
			vel = move;
			vel.mul4( speed );
			vel.sub4( fwdVel );
#ifdef	TRANSGAMING_PS2
			float minStrength = 0.05f*((hkReal(fwdVel.length3())<1.0f)?1.0f:hkReal(fwdVel.length3()));
#else
			float minStrength = 0.05f*((fwdVel.length3()<1.0f)?1.0f:fwdVel.length3());
#endif
			vel.mul4( minStrength * airControl );
//			vel.mul4( 0.05f*fwdVel.length3()*airControl );
		}
		else	// acceleration and deceleration when walking
		{
			vel = move;
			vel.mul4( speed );

			float lenFwdVel = fwdVel.length3();
			if( lenFwdVel > 0.0001f )
			{
#ifdef	TRANSGAMING_PS2
				float lenFwdAccel = hkReal(vel.dot3( fwdVel ))/lenFwdVel;
#else
				float lenFwdAccel = vel.dot3( fwdVel )/lenFwdVel;
#endif

				hkVector4 velAccel = fwdVel;
				velAccel.mul4( lenFwdAccel/lenFwdVel );
				hkVector4 sideAccel = vel;
				sideAccel.sub4( velAccel );

				float factor = (lenFwdAccel-lenFwdVel);
				if( factor > 1.0f ) factor = 1.0f;
				else if( factor < -1.0f ) factor = -1.0f;

				bool moving = ( move.length3() > 0.0f );
				factor *= moving?GEngine->Physics->TWMPAccelerationWhenWalking:GEngine->Physics->TWMPDecelerationWhenWalking;

				vel = fwdVel;
				vel.mul4( factor/lenFwdVel );
				sideAccel.mul4( 0.2f );
				vel.add4( sideAccel );

				if( !moving )
					preventJumpyScreen = true;
				else
					constSpeedWhenWalking = true;
			}
			else
			{
				if( vel.length3() > 1.0f )
					vel.normalize3();
				vel.mul4( move.length3()?GEngine->Physics->TWMPAccelerationWhenWalking:GEngine->Physics->TWMPDecelerationWhenWalking );
			}
		}
	}

	if( stillShocked )
	{
		double factor = (GEngine->Physics->TWMPPlayerShockedTime*1000.f - m_msSinceExplosion)/(GEngine->Physics->TWMPPlayerShockedTime*1000.f);
		if( factor < 0.0 ) factor = 0.0;
		vel.mul4( 1.0 - factor );
	}

	vel.mul4( 60*actionDelta/1000.f );

	m_fallSpeed = m_state.velocity(1);

	if( constSpeedWhenWalking )
	{
		hkVector4 velocityPrev = m_state.velocity;
		m_state.velocity.add4( vel );
		hkVector4 newVel = m_state.velocity;
		float newVelLen = newVel.length3();
		if( newVelLen > 0.1f )
		{
			if( velocityPrev.length3() < speed )
			{
				if( ( newVelLen > speed ) && ( newVelLen - speed < 3 ) )
				{
					newVel.mul4(speed/newVelLen);
					m_state.velocity = newVel;
				}
			}
			else if( ( newVelLen < speed ) && ( speed - newVelLen < 3 ) )
			{
				newVel.mul4(speed/newVelLen);
				m_state.velocity = newVel;
			}
		}
	}
	else if( preventJumpyScreen )	// Fix for jumpy screen while doing nothing
	{
		hkVector4 velocityPrev = m_state.velocity;
		m_state.velocity.add4( vel );
		if( m_state.velocity.dot3( velocityPrev ) < 0.0f )
		{
			m_state.velocity = velocityPrev;
			m_state.velocity.mul4(0.5);
		}
	}
	else
		m_state.velocity.add4( vel );

#ifdef FULL_DEBUG_PRINT
	debugString += "\n";
	debugString += String::Sprintf( "Velocity: (%f,%f,%f)\n", m_state.velocity(0),m_state.velocity(1),m_state.velocity(2) );
	GLog.Print( debugString.AsChar() );
#endif

	m_state.deltaMs = actionDelta;
	applyCharacter( m_state );

	// Transfer velocity to shorts
	velOut();
#else
	GLog.Print("HavokCharacterController::internalPlayerMove() - FIXME!!!!!!!!!!!\n");
#endif // #ifndef HAVOK3 // ========================================================
}

void HavokCharacterController::correctNormal( hkVector4& floorNormal )
{
#ifndef HAVOK3 // ========================================================

	Assert( m_body );
	bool canCollide = !m_body->hasProperty( HPRayCastDontCollide );
	if( canCollide )
	{
		hkPropertyValue pv;
		pv.asInt = 1;
		m_body->addProperty( HPRayCastDontCollide, pv );
	}

	hkVector4 start( m_state.position );
	hkVector4 end( start );

	hkVector4 chwilowy( 0,4.5f*m_halfExtents(1)/5.5f,0 );
	start.add4( chwilowy );
	chwilowy.set( 0,-7.5f*m_halfExtents(1)/5.5f,0 );
	end.add4( chwilowy );

	hkBroadPhase* broadPhase = m_world->getBroadPhase();
	hkShape::hkRayResults initResults;
	havokFloorCheckRaycastCallback rayCallBack( start, end, initResults );
	rayCallBack.fixedGeometryOnly = false;
	broadPhase->castPrimitive( start, end, rayCallBack );

	if( rayCallBack.getHitObject() != HK_NULL )
	{
		initResults = rayCallBack.getRayResults();
		floorNormal.setRotatedDir( ((hkRigidBody*)rayCallBack.getHitObject()->getOwner())->getRotation(), initResults.m_unnormalizedHitNormal );
		floorNormal.normalize3();
	}

	if( canCollide )
		m_body->removeProperty( HPRayCastDontCollide );
#else
	GLog.Print("HavokCharacterController::correctNormal() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 ============================================

}

bool HavokCharacterController::positionValid( hkReal* distance )
{
#ifndef HAVOK3 // ========================================================
	hkVector4 minExt;
	hkVector4 maxExt;
	{
		hkVector4 minToWorld = m_state.position;
		hkVector4 maxToWorld = m_state.position;
		minExt.setSub4( minToWorld, m_halfExtents );
		maxExt.setAdd4( maxToWorld, m_halfExtents );
	}
	const hkAabb aabb( minExt, maxExt );
	m_characterPhantom->updateAabb( aabb );
	m_aabbCastShape->setAabb( aabb );
	m_aabbCastShape->setFromWorld( m_state.position );
	m_aabbCastShape->setToWorld( m_state.position );
	int minindex = m_characterPhantom->updateCollisions( *m_world->getCollisionEnvironment() );

	if( distance )
	{
		if( minindex >= 0 )
		{
			hkReal mindist = m_characterPhantom->getCollisionDetails()[minindex]->getClosestDistance();
			if( mindist >= 0.f )
				return true;
			if( mindist > -0.0001f )
				mindist = -0.0001f;		// this is so the real little penetrations don't take ages to resolve due to limited float precision
			*distance = -mindist;
			return false;
		}
		else
			return true;
	}
	else
		return ( minindex < 0 );

#else
	GLog.Print("HavokCharacterController::positionValid() - FIXME!!!!!!!!!!!\n");
	return true;
#endif // ifndef HAVOK3 =================================
}

void HavokCharacterController::correctPosition( void )
{
	if( positionValid() )
	{
		// Position good, move on

		if( m_state.position(1) > -397.0f )	// this condition to go around Sokol's editor procedure
			m_lastKnownGoodPosition = m_state.position;
		return;
	}
	else
	{
		++m_externalInfluences;
		if( !m_state.position.equals3( m_lastKnownGoodPosition ) )
		{
			// Try to return to last known good position
			m_state.position = m_lastKnownGoodPosition;
			if( positionValid() )
			{
				GLog.Print( "Moved from position (%f,%f,%f) to last known good: (%f,%f,%f)\n",
					m_state.position(0), m_state.position(1), m_state.position(2),
					m_lastKnownGoodPosition(0), m_lastKnownGoodPosition(1), m_lastKnownGoodPosition(2) );
				return;
			}
		}

		// Try to find a good position somewhere nearby
		for( int i = 0; i < 3; ++i )
		{
			float mcpsd = GEngine->Physics->TWMPMaxCorrectPosSearchDist;
			m_state.position = m_lastKnownGoodPosition;
			m_state.position(0) += (rand()/(float)RAND_MAX)*2*mcpsd-mcpsd;
			m_state.position(1) += (rand()/(float)RAND_MAX)*2*mcpsd-mcpsd;
			m_state.position(2) += (rand()/(float)RAND_MAX)*2*mcpsd-mcpsd;
			if( positionValid() )
			{
				GLog.Print( "Moved randomly from position (%f,%f,%f) to last known good: (%f,%f,%f)\n",
					m_state.position(0), m_state.position(1), m_state.position(2),
					m_lastKnownGoodPosition(0), m_lastKnownGoodPosition(1), m_lastKnownGoodPosition(2) );
				m_lastKnownGoodPosition = m_state.position;
				return;
			}
		}
	}

	m_state.position = m_lastKnownGoodPosition;
	GLog.Print( "Position validation problem!!!\n" );
}

bool HavokCharacterController::floorCheck( hkVector4& floorNormal )
{
#ifndef HAVOK3 // ========================================================
	hkReal floorCheckDistance = 1.5f*m_halfExtents(1)/5.5f;
	hkVector4 floorPoint( m_state.position );
	floorPoint(1) -= floorCheckDistance;

	hkVector4 minExt;
	hkVector4 maxExt;
	{
		hkVector4 minToWorld = m_state.position;
		hkVector4 maxToWorld = m_state.position;
		minToWorld(1) -= floorCheckDistance;
		minExt.setSub4( minToWorld, m_halfExtents );
		maxExt.setAdd4( maxToWorld, m_halfExtents );
	}
	const hkAabb aabb( minExt, maxExt );
	m_characterPhantom->updateAabb( aabb );
	m_aabbCastShape->setAabb( aabb );
	m_aabbCastShape->setFromWorld( m_state.position );
	m_aabbCastShape->setToWorld( floorPoint );
	int minindex = m_characterPhantom->updateCollisions( *m_world->getCollisionEnvironment() );

	if( minindex >= 0 )
	{
		int cdminindex = m_characterPhantom->getCollisionDetails()[minindex]->getClosestIndex();
		floorNormal = m_characterPhantom->getCollisionDetails()[minindex]->getContactPoints()[cdminindex].getNormal();
		floorNormal.normalize3();
		floorNormal.setNeg4( floorNormal );

		if( floorNormal(1) > m_stepNormal )
			m_state.lastHeightOfStep = 0;
		return true;
	}
	else
		return false;
#else
	GLog.Print("HavokCharacterController::floorCheck() - FIXME!!!!!!!!!!!\n");
	return true;
#endif // ifndef HAVOK3 ===================================
}

bool HavokCharacterController::floorCheckShort( float down )
{
#ifndef HAVOK3 // ========================================================
	hkReal floorCheckDistance = down*m_halfExtents(1)/5.5f;
	hkVector4 floorPoint( m_state.position );
	floorPoint(1) -= floorCheckDistance;

	hkVector4 minExt;
	hkVector4 maxExt;
	{
		hkVector4 minToWorld = m_state.position;
		hkVector4 maxToWorld = m_state.position;
		minToWorld(1) -= floorCheckDistance;
		minExt.setSub4( minToWorld, m_halfExtents );
		maxExt.setAdd4( maxToWorld, m_halfExtents );
	}
	const hkAabb aabb( minExt, maxExt );
	m_characterPhantom->updateAabb( aabb );
	m_aabbCastShape->setAabb( aabb );
	m_aabbCastShape->setFromWorld( m_state.position );
	m_aabbCastShape->setToWorld( floorPoint );
	int minindex = m_characterPhantom->updateCollisions( *m_world->getCollisionEnvironment() );

	return ( minindex >= 0 );
#else
	GLog.Print("HavokCharacterController::floorCheckShort() - FIXME!!!!!!!!!!!\n");
	return true;
#endif // ifndef HAVOK3 =============================
}

void HavokCharacterController::addVelocity( const hkVector4& impulse )
{
	// Transfer velocity from shorts
	velIn();
	m_state.velocity.add4(impulse);
	velOut();
	++m_externalInfluences;
}

void HavokCharacterController::predict( bool interpolate )
{
//#ifndef HAVOK3 // ========================================================
	if( !m_lastStateReceived || !m_body || !m_body->getSimulationIsland() )
		return;		// no prediction if no state received from external, or no body or body not in world

	DWORD curTime= GEngine->SysDriver.GetCurrentTimeMS();
	DWORD diffTime = curTime-m_lastStateReceived;
	if( curTime <= m_lastStateReceived )
		diffTime = 0;
	else if( diffTime > 500 )	// no updates for more than 0.5 seconds? no need to predict more; it gets out of hand anyway
//	{
		diffTime = 1;
//		if( curTime < m_lastStateReceived )
//			GLog.Print( "Player problem! Cur time: %u, last: %u\n", curTime, m_lastStateReceived );
//	}
	if( !diffTime )
		return;

	int lastState = GProfiler.GetState();
	GProfiler.SetState(PHYSICS_TICK_PLAYER_CONTROLLER);

	MoveState state( m_state );
	m_predicting = true;
	internalPredict( state, diffTime );

	hkVector4 posNow( m_state.position );
	if( interpolate )
	{
		hkVector4 diff( mLastPos );
		diff.sub4( posNow );
		hkReal diffLen = diff.length3();
		if( ( diffLen > 0.003f ) && ( diffLen < 3.0f ) )
		{
			hkVector4 posLast( mLastPos );
			posLast.mul4( GEngine->Physics->MPPredictionInterpolationFactor );
			posNow.mul4( 1.0f-GEngine->Physics->MPPredictionInterpolationFactor );
			posNow.add4( posLast );
		}
	}
	mLastPos = posNow;

	hkVector4 temp( 0,1,0 );
	int rotationDelta = m_receivedRotation-m_lastRotation;
	if( rotationDelta )
	{
		if( rotationDelta > SHRT_MAX )
			rotationDelta -= USHRT_MAX;
		else if( rotationDelta < SHRT_MIN )
			rotationDelta += USHRT_MAX;
		int rotation = (int)(m_lastRotation + rotationDelta*0.33f);
		if( rotation > SHRT_MAX )
			rotation -= USHRT_MAX;
		else if( rotation < SHRT_MIN )
			rotation += USHRT_MAX;
		m_lastRotation = (short)rotation;
	}
//	GLog.Print( "m_yaw: %d, LastYaw: %d, Received rot: %d, rotation: %d\n", m_yaw, LastMove.Yaw, m_receivedRotation, m_lastRotation );
	hkQuaternion qu( temp, ((float)m_lastRotation)*F_PI/32768.f );
	m_body->setRotation( qu );

	m_cameraVerticalCatchUp = state.cameraVerticalCatchUp;
	m_body->setPosition( posNow );

	GProfiler.SetState(lastState);

//#endif // ifndef HAVOK3 ==================================================
}

void HavokCharacterController::internalPredict( MoveState& state, DWORD diffTime )
{
//#ifndef HAVOK3 // ========================================================
	if( diffTime > MAX_POSSIBLE_TIME_MS_FOR_STEP )
	{
		internalPredict( state, diffTime/2 );
		internalPredict( state, diffTime-diffTime/2 );
	}

	state.deltaMs = diffTime;

	if( m_action&(1<<EInputJump) )
		applyCharacter( state );
	else
	{
		// Ladder test
		bool nearLadder = false;
		{
			Vector ladderTestPos;
			ladderTestPos.Set( state.position(0), state.position(1)-m_halfExtents(1), state.position(2) );
#ifdef	TRANSGAMING
			nearLadder = GEngine->pcfWorld->NearLadder( ladderTestPos );	// test feet
			if( !nearLadder )
			{
				ladderTestPos.Y += 10*m_halfExtents(1)/11;
				nearLadder = GEngine->pcfWorld->NearLadder( ladderTestPos );	// test head
			}
#else
			nearLadder = GEngine->World->NearLadder( ladderTestPos );	// test feet
			if( !nearLadder )
			{
				ladderTestPos.Y += 10*m_halfExtents(1)/11;
				nearLadder = GEngine->World->NearLadder( ladderTestPos );	// test head
			}
#endif
		}

		if( nearLadder )
			applyCharacterLadder( state );
		else
			applyCharacter( state );
	}

//#endif // ifndef HAVOK3 =====================================

}

Entity*	HavokCharacterController::getEntity( void )
{
	Assert( m_body->hasProperty( HPEntityPointer ) );
	hkPropertyValue p = m_body->getProperty( HPEntityPointer );
#ifdef HAVOK3
	return (Entity*)p.getPtr();
#else
	return (Entity*)p.asPtr;
#endif
}

#ifdef PLAYER_CONTROLLER_TEST
void HavokCharacterController::StartRecording( const char* filename )
{
	m_file = GFileMan.CreateFileWriter( filename );
	if( !m_file )
		return;
}

void HavokCharacterController::SaveFull( void )
{
	if( !m_file )
		return;

//	if( ((float)rand())/RAND_MAX > 0.2f )
//		return;

	const unsigned char marker = 0xab;
	m_file->Write( &marker, sizeof( unsigned char ) );

	m_file->Write( &m_velocityX, sizeof( short ) );
	m_file->Write( &m_velocityY, sizeof( short ) );
	m_file->Write( &m_velocityZ, sizeof( short ) );
	m_file->Write( &m_state.lastHeightOfStep, sizeof( unsigned char ) );
	m_file->Write( &m_state.position, sizeof( hkVector4 ) );

	m_file->Write( &m_speed, sizeof( unsigned short ) );
	m_file->Write( &m_msSinceLanded, sizeof( unsigned char ) );
	m_file->Write( &m_msSinceJumped, sizeof( unsigned char ) );
	m_file->Write( &m_msSinceExplosion, sizeof( unsigned short ) );

	m_file->Write( &m_onGround, sizeof( bool ) );
	m_file->Write( &m_hasJumpVector, sizeof( bool ) );
	m_file->Write( &m_canBunnyHop, sizeof( bool ) );
	m_file->Write( &m_falling, sizeof( bool ) );
	m_file->Write( &m_jumpInvalidation, sizeof( bool ) );

	m_file->Write( &m_jumpAction, sizeof( DWORD ) );
	m_file->Write( &m_lastNonzeroAction, sizeof( DWORD ) );

	m_file->Write( &m_externalInfluences, sizeof( DWORD ) );
}

void HavokCharacterController::SaveInput( void )
{
	if( !m_file )
		return;
	const unsigned char marker = 0xba;
	m_file->Write( &marker, sizeof( unsigned char ) );
	m_file->Write( &m_internalTimeDelta, sizeof( unsigned char ) );
	m_file->Write( &m_action, sizeof( DWORD ) );
	m_file->Write( &m_yaw, sizeof( short ) );
	m_file->Write( &m_pitch, sizeof( short ) );
}

void HavokCharacterController::LoadInput( FileObject* file )
{
	file->Read( &m_internalTimeDelta, sizeof( unsigned char ) );
	file->Read( &m_action, sizeof( DWORD ) );
	file->Read( &m_yaw, sizeof( short ) );
	file->Read( &m_pitch, sizeof( short ) );
}

void HavokCharacterController::LoadFullAndCompare( LogBuffer& log, FileObject* file, ulong32& stateNumber )
{
//#ifndef HAVOK3 // ========================================================
	log.Print( "============= %u ===================\n", stateNumber++ );

	short			velocityX, velocityY, velocityZ;
	unsigned char	lastHeightOfStep;
	hkVector4		position;
	unsigned short	speed;
	unsigned char	msSinceLanded;
	unsigned char	msSinceJumped;
	unsigned short	msSinceExplosion;
	bool			onGround;
	bool			hasJumpVector;
	bool			canBunnyHop;
	bool			falling;
	bool			jumpInvalidation;
	DWORD			jumpAction;
	DWORD			lastNonzeroAction;

	file->Read( &velocityX, sizeof( short ) );
	file->Read( &velocityY, sizeof( short ) );
	file->Read( &velocityZ, sizeof( short ) );
	file->Read( &lastHeightOfStep, sizeof( unsigned char ) );
	file->Read( &position, sizeof( hkVector4 ) );

	file->Read( &speed, sizeof( unsigned short ) );
	file->Read( &msSinceLanded, sizeof( unsigned char ) );
	file->Read( &msSinceJumped, sizeof( unsigned char ) );
	file->Read( &msSinceExplosion, sizeof( unsigned short ) );

	file->Read( &onGround, sizeof( bool ) );
	file->Read( &hasJumpVector, sizeof( bool ) );
	file->Read( &canBunnyHop, sizeof( bool ) );
	file->Read( &falling, sizeof( bool ) );
	file->Read( &jumpInvalidation, sizeof( bool ) );

	file->Read( &jumpAction, sizeof( DWORD ) );
	file->Read( &lastNonzeroAction, sizeof( DWORD ) );

	DWORD influences;
	file->Read( &influences, sizeof( DWORD ) );
	bool influenced = ( influences > m_externalInfluences );

	if( influenced )
		log.Print( "Externally influenced %d times\n", influences - m_externalInfluences );
	m_externalInfluences = influences;

	if( velocityX != m_velocityX )
	{
		if( !influenced )
			log.Print( "Velocity X local: %d, from file: %d\n", m_velocityX, velocityX );
		m_velocityX = velocityX;
	}

	if( velocityY != m_velocityY )
	{
		if( !influenced )
			log.Print( "Velocity Y local: %d, from file: %d\n", m_velocityY, velocityY );
		m_velocityY = velocityY;
	}

	if( velocityZ != m_velocityZ )
	{
		if( !influenced )
			log.Print( "Velocity Z local: %d, from file: %d\n", m_velocityZ, velocityZ );
		m_velocityZ = velocityZ;
	}

	if( m_state.lastHeightOfStep != lastHeightOfStep )
	{
		if( !influenced )
			log.Print( "Last height of step local: %u, from file: %u\n", m_state.lastHeightOfStep, lastHeightOfStep );
		m_state.lastHeightOfStep = lastHeightOfStep;
	}

	if( !m_state.position.equals3( position ) )
	{
		if( !influenced )
			log.Print( "Position local: (%f,%f,%f), from file: (%f,%f,%f)\n",
				m_state.position(0), m_state.position(1), m_state.position(2),
				position(0), position(1), position(2) );
		m_state.position = position;
	}

	if( m_speed != speed )
	{
		if( !influenced )
			log.Print( "Speed local: %d, from file: %d\n", m_speed, speed );
		m_speed = speed;
	}

	if( m_msSinceLanded != msSinceLanded )
	{
		if( !influenced )
			log.Print( "Since landed local: %d, from file: %d\n", m_msSinceLanded, msSinceLanded );
		m_msSinceLanded = msSinceLanded;
	}

	if( m_msSinceJumped != msSinceJumped )
	{
		if( !influenced )
			log.Print( "Since jumped local: %d, from file: %d\n", m_msSinceJumped, msSinceJumped );
		m_msSinceJumped = msSinceJumped;
	}

	if( m_msSinceExplosion != msSinceExplosion )
	{
		if( !influenced )
			log.Print( "Since explosion local: %d, from file: %d\n", m_msSinceExplosion, msSinceExplosion );
		m_msSinceExplosion = msSinceExplosion;
	}

	if( m_onGround != onGround )
	{
		if( !influenced )
			log.Print( "On ground local: %s, from file: %s\n", m_onGround?"true":"false", onGround?"true":"false" );
		m_onGround = onGround;
	}

	if( m_hasJumpVector != hasJumpVector )
	{
		if( !influenced )
			log.Print( "Has jump vector local: %s, from file: %s\n", m_hasJumpVector?"true":"false", hasJumpVector?"true":"false" );
		m_hasJumpVector = hasJumpVector;
	}

	if( m_canBunnyHop != canBunnyHop )
	{
		if( !influenced )
			log.Print( "Can bunny hop local: %s, from file: %s\n", m_canBunnyHop?"true":"false", canBunnyHop?"true":"false" );
		m_canBunnyHop = canBunnyHop;
	}

	if( m_falling != falling )
	{
		if( !influenced )
			log.Print( "Falling local: %s, from file: %s\n", m_falling?"true":"false", falling?"true":"false" );
		m_falling = falling;
	}

	if( m_jumpInvalidation != jumpInvalidation )
	{
		if( !influenced )
			log.Print( "Jump invalidation local: %s, from file: %s\n", m_jumpInvalidation?"true":"false", jumpInvalidation?"true":"false" );
		m_jumpInvalidation = jumpInvalidation;
	}

	if( m_jumpAction != jumpAction )
	{
		if( !influenced )
			log.Print( "Jump action local: %d, from file: %d\n", m_jumpAction, jumpAction );
		m_jumpAction = jumpAction;
	}

	if( m_lastNonzeroAction != lastNonzeroAction )
	{
		if( !influenced )
			log.Print( "Last nonzero action local: %d, from file: %d\n", m_lastNonzeroAction, lastNonzeroAction );
		m_lastNonzeroAction = lastNonzeroAction;
	}

//#endif // ifndef HAVOK3 ====================================

}

void HavokCharacterController::TestSavedData( const char* filename, PhysicsObject* po )
{
	// Close the file first
	if( m_file )
	{
		const unsigned char marker = 0x00;
		m_file->Write( &marker, sizeof( unsigned char ) );
		delete m_file;
		m_file = 0;
	}

	FileObject* f = GFileMan.CreateFileReader( filename );
	if( !f )
	{
		GLog.Print( "Couldn't open file: %s !\n", filename );
		return;
	}

	LogBuffer log( "movementTestLog.txt" );

	unsigned char marker;
	f->Read( &marker, sizeof( unsigned char ) );

	ulong32 stateNumber = 0;

	while( marker )
	{
		// Read new state and compare it to actual; put input variables into their positions
		if( marker == 0xab )
		{
			LoadFullAndCompare( log, f, stateNumber );
			f->Read( &marker, sizeof( unsigned char ) );
		}
		else
		{
			Assert( marker == 0xba );
			LoadInput( f );
			f->Read( &marker, sizeof( unsigned char ) );
			playerMove();
		}
	}

	delete f;
}
#endif

void HavokCharacterController::internalPlayerMovePromode( void )
{
	if( m_internalTimeDelta > MAX_POSSIBLE_TIME_MS_FOR_STEP )
	{
		PlayerInput input;
		input.Action = m_action;
		input.Yaw = m_yaw;
		input.Pitch = m_pitch;
		input.TimeDelta = m_internalTimeDelta;
		m_internalTimeDelta = input.TimeDelta/2;
		internalPlayerMovePromode();
		m_action = input.Action;
		m_yaw = input.Yaw;
		m_pitch = input.Pitch;
		m_internalTimeDelta = input.TimeDelta - input.TimeDelta/2;
		internalPlayerMovePromode();
		return;
	}

	// Get time delta
	unsigned char actionDelta = m_internalTimeDelta;
	m_internalTimeDelta = 0;

	if( !m_speed )
		m_speed = (unsigned short)(GEngine->Physics->TWPromodePlayerSpeed*1000.f);

	float speed = GEngine->Physics->TWPromodePlayerSpeed;

	// Only those actions interest us
	m_action &= ((1<<EInputJump)|(1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));

//	if( m_action )
//		GLog.Print( "" );

	// Prepare velocity for this move
	velIn();

	// Ladder test
	bool nearLadder = false;
	{
		Vector ladderTestPos;
		ladderTestPos.Set( m_state.position(0), m_state.position(1)-m_halfExtents(1), m_state.position(2) );
#ifdef	TRANSGAMING
		nearLadder = GEngine->pcfWorld->NearLadder( ladderTestPos );	// test feet
		if( !nearLadder )
		{
			ladderTestPos.Y += 10*m_halfExtents(1)/11;
			nearLadder = GEngine->pcfWorld->NearLadder( ladderTestPos );	// test head
		}
#else
		nearLadder = GEngine->World->NearLadder( ladderTestPos );	// test feet
		if( !nearLadder )
		{
			ladderTestPos.Y += 10*m_halfExtents(1)/11;
			nearLadder = GEngine->World->NearLadder( ladderTestPos );	// test head
		}
#endif
	}

	if( nearLadder && !( m_action&(1<<EInputJump) ) )
	{
		// determine desired move
		hkVector4 move(0,0,0);
		hkVector4 upVec(0,1,0);

		float yaw = m_yaw*F_PI/SHRT_MAX;
		hkVector4 forward( sin(yaw), 0, cos(yaw) );
		hkVector4 right;
		right.setCross( forward, upVec );
		forward(1) = tan(m_pitch*F_PI/2/SHRT_MAX);
		forward.normalize3();

		if( m_action & (1<<EInputMoveForward) )		move.add4( forward );
		if( m_action & (1<<EInputMoveBackward) )	move.sub4( forward );
		if( m_action & (1<<EInputMoveRight) )		move.add4( right );
		if( m_action & (1<<EInputMoveLeft) )		move.sub4( right );

		if( move(1) > 0.3f )
			move(1) = GEngine->Physics->TWPlayerStairsUpSpeed;
		else if( move(1) < -0.3f )
			move(1) = -GEngine->Physics->TWPlayerStairsDownSpeed;
		else
			move(1) = 0.0f;

		hkVector4 vel;

		if( m_action & (1<<EInputJump) )
		{
			vel = upVec;
			vel.mul4( 0.7f*GEngine->Physics->TWPromodePlayerJumpStrength );
			vel.mul4( GEngine->Physics->TWPromodePlayerSpeed );
			vel.sub4( m_state.velocity );
		}
		else if( move.lengthSquared3() > 0 )
		{
			vel = move;
			vel.mul4( GEngine->Physics->TWPromodePlayerSpeed );
			vel.sub4( m_state.velocity );
			vel.mul4( 0.2f );
		}
		else
		{
			m_state.velocity.mul4( 0.7f );
			m_action = 0;
			m_fallSpeed = m_state.velocity(1);

			m_state.deltaMs = actionDelta;
			applyCharacterLadder( m_state );

			// Transfer velocity to shorts
			velOut();
			return;
		}

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !vel.isOk4() )
#else
		if( !vel.isOk() )
#endif
			GLog.Print( "Corrupted force in HavokTickIndependentPlayerActionLadder\n" );
		else if( vel.length3() > 50000.0f )
			GLog.Print( "Force > 50000 in HavokTickIndependentPlayerActionLadder\n" );
#endif

		m_fallSpeed = m_state.velocity(1);
		m_state.velocity.add4(vel);
		m_state.deltaMs = actionDelta;
		applyCharacterLadder( m_state );

		// Transfer velocity to shorts
		velOut();
		return;
	}

//	String debugString;

	// Disable jump ability when falling too fast
	if( m_state.velocity(1) < GEngine->Physics->TWMPPlayerMinimalFallSpeedToDisableJump )
		m_action &= ~(1<<EInputJump);

	// Some easy variables
	const bool wasFalling = m_falling;

//	debugString += wasFalling?"    wasFalling, ":"not wasFalling, ";

	hkVector4 floorNormal(0,1,0);

	correctPosition();
	m_onGround = floorCheck( floorNormal );
	bool m_falling = !m_onGround && !nearLadder;

	const hkVector4 upVector(0,1,0);

//	debugString += falling?"    falling, ":"not falling, ";
//	debugString += m_onGround?"    onGround, ":"not onGround, ";

	hkVector4 velocity( m_state.velocity );
	hkVector4 fwdVel( m_state.velocity );
	fwdVel(1) = 0;

/*
	// Check if we are sliding
	bool sliding = false;
	if( m_onGround )
	{
		float dot = floorNormal.dot3( upVector );

		if( dot <= 0.f )
		{
			correctNormal( floorNormal );
			dot = floorNormal.dot3( upVector );
		}

		sliding = ( dot < cos( GEngine->Physics->TWSlopeAngleToSlide*F_DEG2RAD ) );

		if( sliding )
		{
			if( !fwdVel.lengthSquared3() )
				sliding = false;
			else
				falling = true;
		}
	}
//	debugString += sliding?"    sliding, ":"not sliding, ";
*/

	// Adjust movement variables
	{
		unsigned int t = m_msSinceLanded + actionDelta;
		m_msSinceLanded = (t>255)?255:t;
	}

	// Report fall damage
	if( ( m_fallSpeed < m_state.velocity(1) ) && ( m_fallSpeed < -20.0f ) && ( m_fallSpeed-m_state.velocity(1) < -20.0f ) )
	{
		Entity* entity = 0;
		if( m_body->hasProperty( HPEntityPointer ) )
		{
			hkPropertyValue p = m_body->getProperty( HPEntityPointer );
#ifdef HAVOK3
			entity = (Entity*)p.getPtr();
#else
			entity = (Entity*)p.asPtr;
#endif
		}
		GScript.QueueMessage("PLAYER_HIT_GROUND", "%e,%f", entity, m_fallSpeed-m_state.velocity(1) );
	}

	// Check landing
	if( !m_falling && wasFalling )
	{
		m_msSinceLanded = 0;
		if( GEngine->Physics->TWPromodeHorizontalSpeedAfterLandingMultiplier )
		{
			hkVector4 addition( velocity );
			addition(1) = 0;
			float len = addition.length3();
			if( len )
				addition.mul4(GEngine->Physics->TWPromodeHorizontalSpeedAfterLandingMultiplier*GEngine->Physics->TWPromodePlayerSpeed/len);
			velocity.add4( addition );
		}
	}

	const bool pressedJump = ( (m_action & (1<<EInputJump)) != 0 );
	if( !pressedJump )
		m_jumpInvalidation = false;

	// Jump - you can do it in the same tick - see?
	if( ( m_action & (1<<EInputJump) ) && !m_jumpInvalidation )
	{
		if( !m_falling && ( velocity(1) < 1.0f ) )
		{
			velocity(1) = GEngine->Physics->TWPromodePlayerJumpStrength*speed;
			m_falling = true;
		}
	}

	// Friction
	if( !m_falling &&
		( m_msSinceLanded >= GEngine->Physics->TWPromodeSecondsYouHaveSameSpeedAfterLanding*1000.f ) )
	{
		// kill off velocity
		float curSpeed = velocity.length3();
		if( curSpeed < (speed/20.0f) )
		{
			velocity(0) = 0;
			velocity(2) = 0;
		}
		else
		{
			float groundFriction = GEngine->Physics->TWPromodePlayerGroundFriction;
			const float stopSpeed = speed*GEngine->Physics->TWPromodePlayerStopSpeedFactor;

			float control = (curSpeed<stopSpeed)?stopSpeed:curSpeed;
			float speedDrop = actionDelta*groundFriction*control/1000.f;

			float newSpeed = curSpeed - speedDrop;
			if( newSpeed < 0.0f )
				newSpeed = 0.0f;
			newSpeed /= curSpeed;
			velocity.mul4(newSpeed);
		}
	}

	// determine desired move
	hkVector4 wishVel(0,0,0);
	{
		float yaw = m_yaw*F_PI/SHRT_MAX;
		hkVector4 forward( sin(yaw), 0, cos(yaw) );	// no need to normalize
		hkVector4 right;
		right.setCross( forward, upVector );

		const float forwardSpeed = speed;
		forward.mul4( forwardSpeed );

		const float sideSpeed = speed*GEngine->Physics->TWPromodePlayerStrafeSpeedModifier;
		right.mul4( sideSpeed );

		if( m_action & (1<<EInputMoveForward) ) wishVel.add4( forward );
		if( m_action & (1<<EInputMoveBackward) ) wishVel.sub4( forward );
		if( m_action & (1<<EInputMoveRight) ) wishVel.add4( right );
		if( m_action & (1<<EInputMoveLeft) ) wishVel.sub4( right );
		wishVel(1) = 0;
	}

	float wishSpeed = wishVel.length3();
	hkVector4 wishDir = wishVel;
	wishDir.normalize3();

	if( wishSpeed > speed )
	{
		wishVel.mul4( speed/wishSpeed );
		wishSpeed = speed;
	}

	if( m_falling )	// Air control
	{
		// Accelerate in air
		float accel = GEngine->Physics->TWPromodePlayerAirAcceleration;
		float wishSpd = wishSpeed;
		if( wishSpd > ( speed*GEngine->Physics->TWPromodePlayerAirMaxAccelerationSpeedModifier ) )
			wishSpd = ( speed*GEngine->Physics->TWPromodePlayerAirMaxAccelerationSpeedModifier );

		float dotSpeed = velocity.dot3( wishDir );
		float addSpeed = wishSpd - dotSpeed;

		if( addSpeed > 0 )
		{
			float accelSpeed = accel * wishSpeed * actionDelta / 1000.f;
			if( accelSpeed > addSpeed )
				accelSpeed = addSpeed;
	
			hkVector4 wd( wishDir );
			wd.mul4( accelSpeed );
			velocity.add4( wd );
		}
	}
	else						// Ground control
	{
		velocity(1) = 0;

		// Accelerate on ground
		float accel = GEngine->Physics->TWPromodePlayerAcceleration;
		float dotSpeed = velocity.dot3( wishDir );
		float addSpeed = wishSpeed - dotSpeed;

		if( addSpeed > 0 )
		{
			float accelSpeed = accel * wishSpeed * actionDelta / 1000.f;
			if( accelSpeed > addSpeed )
				accelSpeed = addSpeed;
	
			hkVector4 wd( wishDir );
			wd.mul4( accelSpeed );
			velocity.add4( wd );
		}
	}

	m_state.velocity = velocity;
	m_state.deltaMs = actionDelta;

	applyCharacter( m_state );

//	debugString += "\n";
//	GLog.Print( debugString.AsChar() );

	// Transfer velocity to shorts
	velOut();
}
