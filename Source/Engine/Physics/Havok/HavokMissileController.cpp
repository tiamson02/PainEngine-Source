
#include "Engine.h"
#include "Physics/Havok/HavokMissileController.h"

#include "Engine/Entity.h"
#include "Engine/World.h"
#include "Physics/PhysicsObject.h"
#include "Physics/PhysicsEngine.h"
#include "Net/NetworkDevice2.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;


#ifndef HAVOK3
#include <hkdynamics/phantom/hkCharacterPhantom.h>
#include <hkcollide2/shape/hkAabbCastShape.h> // BLAIR - this doesn't exist in 3.3
#include <hkdynamics2/phantom/hkCharacterPhantom.h>
#endif
#define OVERBOUNCE 1.01f
#define PHANTOM_TOLERANCE 0.1f
#define HAVOK_COLLISION_TOLERANCE 0.01f
#define MAX_POSSIBLE_TIME_MS_FOR_STEP 25
#define TIME_AFTER_WHICH_COLLIDING_WITH_PLAYERS 50
#define TIME_AFTER_WHICH_STAKES_FALL 200
#define MAX_CORRECT_POS_SEARCH_DIST 0.1f

#ifdef MISSILE_LOGGING
unsigned int missileLogNumber = 0;
#endif

// *********************************************************************************************************************

#ifdef HAVOK3
// BLAIR - By the looks of it, hkCharacterPhantoms have been replaced by hkShapePhantoms ( phantoms with a shape ).
// ... since our memory right now is more important than CPU ( FOR THE MOMENT! ) we will try the hkSimpleShapePhantom, as
// it doesn't cache the collision results, althouth it DOES CALCULATE THEM EVERY TIME!!  This means, if we have enough
// memory, and our game is running too slow, we should switch it up to hkCachingShapePhantom!!
class hkMyMissilePhantom : public hkSimpleShapePhantom
#else
class hkMyMissilePhantom : public hkCharacterPhantom
#endif
{
public:
#ifdef HAVOK3
	hkMyMissilePhantom( const hkShape *shape, const hkTransform &transform, hkUint32 m_collisionFilterInfo=0 ) : hkSimpleShapePhantom( shape, transform, m_collisionFilterInfo ), collideWithPlayers(false) {};
#else
	hkMyMissilePhantom( const hkCharacterPhantomConstructionInfo& info ) : hkCharacterPhantom(info), collideWithPlayers(false) {};
#endif

// BLAIR - Make it a NULL class for now...
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
		if( ( collidable.getShape() == HK_NULL ) || ( collidable.getType() != HK_BROAD_PHASE_ENTITY ) )
			return;

#ifdef HAVOK3
		if( !hkAabbPhantom::addOverlappingCollidable( collidable ) )
#else
		if( !hkAabbPhantom::addFilteredOverlappingCollidable( collidable ) )
#endif
			return;

		if( !collideWithPlayers )
		{
			int collGroup = collidable.getCollisionFilterInfo();
			if( ( collGroup == HCGPlayerBody ) || ( collGroup == HCGRagdoll ) )
				return;
//			GLog.Print( "Collided with body %u, group %u\n", (ulong32)collidable.getOwner(), collGroup );
		}

        m_setTag("Physics");
		hkSimpleClosestPointsDetails* destElem = new hkSimpleClosestPointsDetails( collidable, m_collidable, *m_owner->getCollisionEnvironment() );
		m_myCollisionDetails.pushBack( destElem );
	}

	virtual void removeOverlappingCollidable( hkCollidable& collidable )
	{
		if( ( collidable.getShape() == HK_NULL ) || ( collidable.getType() != HK_BROAD_PHASE_ENTITY ) )
			return;

//		int collGroup = collidable.getCollisionFilterInfo();
//		GLog.Print( "Stopped colliding with body %u, group %u\n", (ulong32)collidable.getOwner(), collGroup );

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

	bool collideWithPlayers;

private:
	hkArray<hkSimpleClosestPointsDetails*> m_myCollisionDetails;
#else
	int updateCollisions( hkCollisionEnvironment& env )
	{
	}
	virtual void addOverlappingCollidable( hkCollidable& collidable )
	{
	}
	virtual void removeOverlappingCollidable( hkCollidable& collidable )
	{
	}
	bool collideWithPlayers;
#endif
};

// *********************************************************************************************************************

HavokMissileController::HavokMissileController( hkWorld* world, hkRigidBody* body, float size, int type ) :
	m_world( world ),
	m_body( body ),
	m_timeDeltaFraction( 0.f ),
	m_damping( 0.5f ),
	m_restitution( 1.0f )
{
	m_state.deltaMs				= 0;
	m_state.position			= m_body->getPosition();
	hkVector4 vel				= m_body->getLinearVelocity();
	m_state.properties			= 0;
	m_state.timeSinceCreation	= 0;
	m_state.setVel( vel );
	m_halfExtents.set( size, size, size );
	m_friction			= m_body->getMaterial().getFriction()*0.2f;
	m_damping			= m_body->getLinearDamping();

#ifndef HAVOK3
#ifdef HAVOK3
	// This should be in effect...
	//if( GEngine->Net && GEngine->Net->IsClient() && ( pci.m_collisionFilterInfo == HCGNoncolliding ) )
	//	pci.m_collisionFilterInfo = HCGParticles;

    // BLAIR - *** THIS MIGHT NOT BE THE SHAPE WE WANT TO USE!!!
    m_setTag("Physics");
	m_aabbCastShape = new hkPhantomCallbackShape;
    m_setTag("Physics");
	m_missilePhantom = new hkMyMissilePhantom( m_aabbCastShape, hkTransform::getIdentity(), hkGroupFilter::calcFilterInfo( m_body->getCollidable().getCollisionFilterInfo() ) );
#else
	hkCharacterPhantomConstructionInfo pci;
	pci.m_min.setSub4(m_state.position, m_halfExtents );
	pci.m_max.setAdd4(m_state.position, m_halfExtents );

    m_setTag("Physics");
	m_aabbCastShape = new hkAabbCastShape( m_state.position, m_state.position, m_halfExtents );
	pci.m_shape = m_aabbCastShape;
	pci.m_transform.setIdentity();
	pci.m_collisionFilter = world->getCollisionFilter();
	pci.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( m_body->getCollidable().getCollisionFilterInfo() );

	if( GEngine->Net && GEngine->Net->IsClient() && ( pci.m_collisionFilterInfo == HCGNoncolliding ) )
		pci.m_collisionFilterInfo = HCGParticles;

    m_setTag("Physics");
	m_missilePhantom = new hkMyMissilePhantom( pci );
#endif
	m_world->addPhantom( m_missilePhantom );
	m_body->setKeyframed( true );

	switch( type )
	{
	case CT_Default:
		m_state.properties |= MIPR_SurfaceStick;
		break;
	case CT_Stake:
		m_state.properties |= (MIPR_StakeGravity|MIPR_SurfaceStick);
		break;
	case CT_HeaterBomb:
		m_state.properties |= (MIPR_Gravity|MIPR_HeaterBomb);
		m_restitution = 1.5f;
		break;
	case CT_Grenade:
		m_state.properties |= MIPR_Gravity;
		break;
	case CT_PainHead:
		m_state.properties |= (MIPR_SurfaceStick|MIPR_PainHead);
		break;
	case CT_Soul:
		m_state.properties |= (MIPR_Gravity|MIPR_Soul);
		m_restitution = 0;
	case CT_Spectator:
		m_restitution = 0;
		break;
	}
	m_lastStateReceived = GEngine->SysDriver.GetCurrentTimeMS();
#ifdef	TRANSGAMING
	m_lastFrameWhenMoved = GEngine->pcfWorld->Frame;
#else
	m_lastFrameWhenMoved = GEngine->World->Frame;
#endif
	m_state.lastKnownGoodPosition = m_state.position;

#ifdef MISSILE_LOGGING
	String logName = String::Sprintf( "missile_%03d.log", missileLogNumber++ );
	mLog.Open( logName.AsChar() );
	mLog.Print( "Friction: %f\n", m_friction );
	mLog.Print( "Damping: %f\n", m_damping );
	mLog.Print( "Colisiong filter: %u\n", pci.m_collisionFilterInfo );
	mLog.Print( "Gravity: %s\n", (m_state.properties & MIPR_Gravity)?"true":"false" );
#endif

#endif // ifndef HAVOK3
}

HavokMissileController::~HavokMissileController()
{
#ifndef HAVOK3
	if( m_aabbCastShape )
		m_aabbCastShape->removeReference();

	if( m_missilePhantom )
		m_missilePhantom->removeReference();
#endif // ifndef HAVOK3
}

void HavokMissileController::slideMove( MoveState& state )
{
#ifndef HAVOK3
	// Early exit for dummy requests to move
	if( !state.deltaMs )
		return;

	hkVector4 velocity;
	state.getVel( velocity );

	if( ( state.properties & MIPR_SurfaceStick ) && ( state.properties & MIPR_Stuck ) )
		return;

	m_missilePhantom->collideWithPlayers = ( state.timeSinceCreation > TIME_AFTER_WHICH_COLLIDING_WITH_PLAYERS );

	hkVector4 deltaPos;
	if( m_damping > 0 )
		velocity.mul4( 1.0f - m_damping*state.deltaMs/1000.f );
	deltaPos.setMul4( state.deltaMs/1000.f, velocity );

	// Compute new bb
	hkVector4 minExt;
	hkVector4 maxExt;
	{
		// Expand by the displacement vector when necessary
		hkVector4 minToWorld = state.position;
		hkVector4 maxToWorld = state.position;

		for( int i = 0; i < 3; ++i )
			if( deltaPos(i) < 0 )
				minToWorld(i) += deltaPos(i) - PHANTOM_TOLERANCE;
			else
				maxToWorld(i) += deltaPos(i) + PHANTOM_TOLERANCE;

		minExt.setSub4( minToWorld, m_halfExtents );
		maxExt.setAdd4( maxToWorld, m_halfExtents );
	}
	const hkAabb aabb( minExt, maxExt );
	m_missilePhantom->updateAabb( aabb );
	m_aabbCastShape->setAabb( aabb );

	hkVector4 prevNormal(0,0,0);
	
	// Try to bounce from up to 5 planes
	for( int i = 0; i < 5; ++i )
	{
		if( ( state.properties & MIPR_SurfaceStick ) && ( state.properties & MIPR_Stuck ) )
			return;

		hkVector4 deltaPos;
		deltaPos.setMul4( state.deltaMs/1000.f, velocity );
		hkVector4 destPos( state.position );
		destPos.add4( deltaPos );

		// Perform an AABB cast
		m_aabbCastShape->setFromWorld( state.position );
		m_aabbCastShape->setToWorld( destPos );

		int minindex = m_missilePhantom->updateCollisions( *m_world->getCollisionEnvironment() );

		hkReal mindist = 1.f;

		// If we hit an object
		if( minindex >= 0 )
		{
			mindist = m_missilePhantom->getCollisionDetails()[minindex]->getClosestDistance();

			if( mindist < 0.f )				// This means penetration
				mindist = 0.f;

			hkVector4	hitNormalWorld;

			int cdminindex = m_missilePhantom->getCollisionDetails()[minindex]->getClosestIndex();
			hitNormalWorld = m_missilePhantom->getCollisionDetails()[minindex]->getContactPoints()[cdminindex].getNormal();
			hitNormalWorld.normalize3();

			// Move the AABB to the point of contact
			float timeOfMove = state.deltaMs * mindist;
			state.deltaMs -= floor(timeOfMove);
			state.position.setInterpolate4( state.position, destPos, mindist );

			if( state.properties & MIPR_SurfaceStick )
				state.properties |= MIPR_Stuck;
			else
			{
				// Add in gravity during that distance
				if( state.properties & MIPR_Gravity )
				{
					hkVector4 grav(0, -9.81f * 3, 0);
					grav.mul4( timeOfMove/1000.f );
					velocity.add4( grav );
				}

				hkVector4 velNorm( hitNormalWorld );
#ifdef	TRANSGAMING_PS2
				velNorm.mul4( hkReal(velocity.dot3( hitNormalWorld ))*(m_restitution+OVERBOUNCE) );
#else
				velNorm.mul4( hkReal(velocity.dot3( hitNormalWorld ))*(m_restitution+OVERBOUNCE) );
				//velNorm.mul4( velocity.dot3( hitNormalWorld )*(m_restitution+OVERBOUNCE) );
#endif
				velocity.sub4( velNorm );
				velocity.mul4( 1.f-m_friction );
			}

#ifdef MISSILE_LOGGING
			mLog.Print( "Bounce\n" );
			mLog.Print( "Friction: %f\n", m_friction );
			mLog.Print( "Damping: %f\n", m_damping );
			mLog.Print( "Collision filter: %u\n", m_body->getCollidable().getCollisionFilterInfo() );
			mLog.Print( "Gravity: %s\n", (state.properties & MIPR_Gravity)?"true":"false" );
#endif

			// Give collision to script
			if( m_collisionsReport )
			{
				Entity* entity = getEntity();
				if( entity->PO && entity->PO->GetCollisionCallbacks() )
				{
					const hkCollidable& coll = m_missilePhantom->getCollisionDetails()[minindex]->getCollidableA();
					hkEntity* ent = (hkEntity*)coll.getOwner();
					hkPropertyValue p = ent->getProperty( HPEntityPointer );
#ifdef HAVOK3
					GScript.QueueMessage("COLLISION_WITH_OTHER_ENTITY","%e,%f,%f,%f,%f,%f,%f,%e,%p,%p", entity,
						state.position(0), state.position(1), state.position(2), hitNormalWorld(0), hitNormalWorld(1), hitNormalWorld(2),
						(Entity*)p.getPtr(), m_body, ent );
#else
					GScript.QueueMessage("COLLISION_WITH_OTHER_ENTITY","%e,%f,%f,%f,%f,%f,%f,%e,%p,%p", entity,
						state.position(0), state.position(1), state.position(2), hitNormalWorld(0), hitNormalWorld(1), hitNormalWorld(2),
						(Entity*)p.asPtr, m_body, ent );
#endif
				}
			}
		}
		else	// If we didn't hit anything we are unobstructed so we move the full remaining distance
		{
			state.position = destPos;

			// Add in gravity during that distance
			if( state.properties & MIPR_Gravity )
			{
				hkVector4 grav( 0, -9.81f * 3, 0 );
				grav.mul4( state.deltaMs/1000.f );
				velocity.add4( grav );
				state.deltaMs = 0;
			}
			state.setVel( velocity );
			return;
		}
	}
#endif // ifndef HAVOK3
}

void HavokMissileController::missileMove( DWORD timeDelta )
{
#ifndef HAVOK3
	Assert( m_body );
	if( !m_body->getSimulationIsland() )
		return;

	int lastState = GProfiler.GetState();
	GProfiler.SetState(PHYSICS_TICK_MISSILE_CONTROLLER);

#ifdef MISSILE_LOGGING
	mLog.Print( "DT: %d, P: (%f,%f,%f), V: (%d,%d,%d)\n", timeDelta,
		m_state.position(0), m_state.position(1), m_state.position(2),
		m_state.velocityX, m_state.velocityY, m_state.velocityZ );
#endif

	correctPosition( m_state );
	m_collisionsReport = true;
	internalMove( m_state, timeDelta );

	if( m_state.properties & MIPR_Stuck )
	{
		hkVector4 zero(0,0,0);
		m_body->setAngularVelocity( zero );
	}

	// Transfer results to body
	m_body->setPosition( m_state.position );

	if( m_state.properties & MIPR_StakeGravity )
	{
		hkVector4 zero(0,0,0);
		m_body->setAngularVelocity( zero );
		hkVector4 vecTarget;
		m_state.getVel( vecTarget );
		if( vecTarget.lengthSquared3() )
		{
			vecTarget.normalize3();
			Vector vecT( vecTarget(0), vecTarget(1), vecTarget(2) );
			Matrix m = NormalZRotationMatrix(vecT);
			Quaternion q(m);
			hkQuaternion q2(q.X, q.Y, q.Z, -q.W );
			m_body->setRotation( q2 );
		}
	}

	m_body->setKeyframed(true);
	m_lastStateReceived = GEngine->SysDriver.GetCurrentTimeMS();
#ifdef	TRANSGAMING
	m_lastFrameWhenMoved = GEngine->pcfWorld->Frame;
#else
	m_lastFrameWhenMoved = GEngine->World->Frame;
#endif

	GProfiler.SetState(lastState);
#endif // ifndef HAVOK3
}

const hkVector4& HavokMissileController::getPosition( void ) const
{
//	GLog.Print( "Missile.GetPosition()\n" );
	return m_state.position;
}

void HavokMissileController::setPosition( const hkVector4& targetPosition )
{
//	GLog.Print( "Missile.SetPosition()\n" );
	if( m_state.position.equals3( targetPosition ) )
		return;
#ifdef MISSILE_LOGGING
	mLog.Print( "Set position from (%f,%f,%f) to (%f,%f,%f)\n", m_state.position(0), m_state.position(1), m_state.position(2), targetPosition(0), targetPosition(1), targetPosition(2) );
#endif
	m_state.position = targetPosition;
	if( !positionValid( m_state ) )
		GLog.Print( "Missile SetPosition to invalid position!\n" );
}

hkVector4 HavokMissileController::getVelocity( void ) const
{
	hkVector4 vel;
	m_state.getVel( vel );
//	GLog.Print( "Missile.GetVelocity(): (%f,%f,%f)\n", vel(0), vel(1), vel(2) );
	return vel;
}

void HavokMissileController::setVelocity( const hkVector4& vel )
{
#ifdef MISSILE_LOGGING
	mLog.Print( "Set velocity\n" );
#endif
//	GLog.Print( "Missile.SetVelocity()\n" );
	m_state.setVel( vel );
}

void HavokMissileController::addVelocity( const hkVector4& impulse )
{
//	GLog.Print( "Missile.AddVelocity()\n" );
#ifdef MISSILE_LOGGING
	mLog.Print( "Add velocity\n" );
#endif
	hkVector4 vel;
	m_state.getVel( vel );
	vel.add4( impulse );
	m_state.setVel( vel );
}

/*
void HavokMissileController::setGravity( bool grav )
{
//	GLog.Print( "Missile.SetGravity()\n" );
#ifdef MISSILE_LOGGING
	if( ( grav && ( m_state.properties & MIPR_Gravity ) ) ||
		!( grav && !( m_state.properties & MIPR_Gravity ) ) )
		return;
	mLog.Print("Gravity set to %s\n", grav?"true":"false" );
#endif
	if( grav )
		m_state.properties |= MIPR_Gravity;
	else
		m_state.properties &= ~MIPR_Gravity;
}
*/

void HavokMissileController::internalMove( MoveState& state, DWORD diffTime )
{
	if( diffTime > MAX_POSSIBLE_TIME_MS_FOR_STEP )
	{
		internalMove( state, diffTime/2 );
		internalMove( state, diffTime-diffTime/2 );
	}

	if( ( state.properties & MIPR_StakeGravity ) && ( state.timeSinceCreation > TIME_AFTER_WHICH_STAKES_FALL ) )
		state.properties |= MIPR_Gravity;

	state.deltaMs = diffTime;
	slideMove( state );
	state.timeSinceCreation += diffTime;
}

bool HavokMissileController::positionValid( MoveState& state )
{
#ifndef HAVOK3 //=======================================================
	hkVector4 minExt;
	hkVector4 maxExt;
	{
		hkVector4 minToWorld = state.position;
		hkVector4 maxToWorld = state.position;
		minExt.setSub4( minToWorld, m_halfExtents );
		maxExt.setAdd4( maxToWorld, m_halfExtents );
	}
	const hkAabb aabb( minExt, maxExt );
	m_missilePhantom->updateAabb( aabb );
	m_aabbCastShape->setAabb( aabb );
	m_aabbCastShape->setFromWorld( state.position );
	m_aabbCastShape->setToWorld( state.position );
	return ( m_missilePhantom->updateCollisions( *m_world->getCollisionEnvironment() ) < 0 );
#else
	GLog.Print("HavokMissileController::positionValid() - FIXME!!!!!!!!!!!\n");
	return false;
#endif // ifndef HAVOK3 ===============================================
}
void HavokMissileController::correctPosition( MoveState& state )
{
	if( positionValid( state ) )
	{
		// Position good, move on
		state.lastKnownGoodPosition = state.position;
		return;
	}
	else
	{
		if( !state.position.equals3( state.lastKnownGoodPosition ) )
		{
			// Try to return to last known good position
			state.position = state.lastKnownGoodPosition;
			if( positionValid( state ) )
			{
//				GLog.Print( "Missile moved from position (%f,%f,%f) to last known good: (%f,%f,%f)\n",
//					state.position(0), state.position(1), state.position(2),
//					state.lastKnownGoodPosition(0), state.lastKnownGoodPosition(1), state.lastKnownGoodPosition(2) );
				return;
			}
		}

		// Try to find a good position somewhere nearby
		for( int i = 0; i < 3; ++i )
		{
			state.position = state.lastKnownGoodPosition;
			state.position(0) += (rand()/(float)RAND_MAX)*2*MAX_CORRECT_POS_SEARCH_DIST-MAX_CORRECT_POS_SEARCH_DIST;
			state.position(1) += (rand()/(float)RAND_MAX)*2*MAX_CORRECT_POS_SEARCH_DIST-MAX_CORRECT_POS_SEARCH_DIST;
			state.position(2) += (rand()/(float)RAND_MAX)*2*MAX_CORRECT_POS_SEARCH_DIST-MAX_CORRECT_POS_SEARCH_DIST;
			if( positionValid( state ) )
			{
//				GLog.Print( "Missile moved randomly from position (%f,%f,%f) to last known good: (%f,%f,%f)\n",
//					state.position(0), state.position(1), state.position(2),
//					state.lastKnownGoodPosition(0), state.lastKnownGoodPosition(1), state.lastKnownGoodPosition(2) );
//				state.lastKnownGoodPosition = state.position;
				return;
			}
		}
	}

	state.position = state.lastKnownGoodPosition;
//	GLog.Print( "Missile position validation problem!!!\n" );
}

void HavokMissileController::predict( void )
{
#ifdef	TRANSGAMING
	if( !m_body || !m_body->getSimulationIsland() || ( m_lastFrameWhenMoved == GEngine->pcfWorld->Frame ) )
		return;		// no prediction if no body or body not in world or moved in this frame already
#else
	if( !m_body || !m_body->getSimulationIsland() || ( m_lastFrameWhenMoved == GEngine->World->Frame ) )
		return;		// no prediction if no body or body not in world or moved in this frame already
#endif

	int lastState = GProfiler.GetState();
	GProfiler.SetState(PHYSICS_TICK_MISSILE_CONTROLLER);

	DWORD curTime = GEngine->SysDriver.GetCurrentTimeMS();
	DWORD diffTime = curTime-m_lastStateReceived;

	if( curTime <= m_lastStateReceived )
		diffTime = 0;
	else if( diffTime > 500 )	// no updates for more than 0.5 seconds? no need to predict more; it gets out of hand anyway
	{
		diffTime = 1;
//		GLog.Print( "Missile problem! Cur time: %u, last: %u\n", curTime, m_lastStateReceived );
#ifdef MISSILE_LOGGING
		mLog.Print( "Missile problem! Cur time: %u, last: %u\n", curTime, m_lastStateReceived );
#endif
	}
	if( !diffTime )
	{
		GProfiler.SetState(lastState);
		return;	// nothing to predict
	}

	correctPosition( m_state );
	MoveState state( m_state );
	m_collisionsReport = false;
	internalMove( state, diffTime );

#ifdef MISSILE_LOGGING
	mLog.Print( "PREDICT: DiffTime: %u, P: (%f,%f,%f), V: (%d,%d,%d)\n", diffTime,
		state.position(0), state.position(1), state.position(2),
		state.velocityX, state.velocityY, state.velocityZ );
#endif

	m_body->setPosition( state.position );
	if( state.properties & MIPR_StakeGravity )
	{
		hkVector4 zero(0,0,0);
		m_body->setAngularVelocity( zero );
		hkVector4 vecTarget;
		state.getVel( vecTarget );
		if( vecTarget.lengthSquared3() )
		{
			vecTarget.normalize3();
			Vector vecT( vecTarget(0), vecTarget(1), vecTarget(2) );
			Matrix m = NormalZRotationMatrix(vecT);
			Quaternion q(m);
			hkQuaternion q2(q.X, q.Y, q.Z, -q.W );
			m_body->setRotation( q2 );
		}
	}
	GProfiler.SetState(lastState);
}

Entity*	HavokMissileController::getEntity( void )
{
	Assert( m_body->hasProperty( HPEntityPointer ) );
	hkPropertyValue p = m_body->getProperty( HPEntityPointer );
#ifdef HAVOK3
	return (Entity*)p.getPtr();
#else
	return (Entity*)p.asPtr;
#endif
}

void HavokMissileController::setAngularVelocity( Vector& velocity )
{
	if( m_body )
	{
		hkVector4 vel( velocity.X, velocity.Y, velocity.Z );
		m_body->setAngularVelocity( vel );
	}
}

Vector HavokMissileController::getAngularVelocity( void )
{
	if( !m_body )
		return Vector(0,0,0);
	hkVector4 vel = m_body->getAngularVelocity();
	return Vector( vel(0), vel(1), vel(2) );
}

/*
void HavokMissileController::packPosition( void )
{
	const char xBits = 22;
	const char yBits = 20;
	const char zBits = 22;

	const ulong32 maxX = (1<<(xBits-1));
	const ulong32 maxY = (1<<(yBits-1));
	const ulong32 maxZ = (1<<(zBits-1));

	ulong32 dword1 = 0, dword2 = 0;
	bool sign;
	ulong32eg;

	// Pack X
	sign = ( m_state.position(0) < 0 );
	integ = (ulong32)((fabs(m_state.position(0)))/(HAVOK_COLLISION_TOLERANCE/2.0f));
	if( integ >= maxX )
		integ = maxX-1;
	integ &= ((1<<(xBits-1))-1);
	dword1 |= integ;
	if( sign ) dword1 |= (1<<(xBits-1));

	// Pack Y
	sign = ( m_state.position(1) < 0 );
	integ = (ulong32)((fabs(m_state.position(1)))/(HAVOK_COLLISION_TOLERANCE/2.0f));
	if( integ >= maxZ )
		integ = maxY-1;
	integ &= ((1<<(yBits-1))-1);
	dword1 |= ( integ << xBits );
	dword2 |= ( integ >> (32 - xBits ) );
	if( sign ) dword2 |= ( 1 << (xBits + yBits - 1 - 32 ) );

	// Pack Z
	sign = ( m_state.position(2) < 0 );
	integ = (ulong32)((fabs(m_state.position(2)))/(HAVOK_COLLISION_TOLERANCE/2.0f));
	if( integ >= maxZ )
		integ = maxZ-1;
	integ &= ((1<<(zBits-1))-1);
	dword2 |= ( integ << ( xBits + yBits - 32 ) );
	if( sign ) dword2 |= ( 1 << (xBits + yBits + zBits - 1 - 32 ) );

	m_packedPos1 = dword1;
	m_packedPos2 = dword2;
}

void HavokMissileController::unpackPosition( void )
{
	const char xBits = 22;
	const char yBits = 20;
	const char zBits = 22;

	bool sign;
	ulong32eg;

	ulong32 dword1 = m_packedPos1;
	ulong32 dword2 = m_packedPos2;

	// Unpack X
	integ = dword1;
	integ &= ((1<<(xBits-1))-1);
	sign = ((dword1 & (1<<(xBits-1))) != 0 );
	m_state.position(0) = (float)(((double)integ)*(HAVOK_COLLISION_TOLERANCE/2.0));
	if( sign ) m_state.position(0) *= -1.0;

	// Unpack Y
	integ = dword1 >> xBits;
	integ |= dword2 << ( 32 - xBits );
	integ &= ((1<<(yBits-1))-1);
	sign = ((dword2 & (1<<(xBits + yBits - 1 - 32 ))) != 0 );
	m_state.position(1) = (float)(((double)integ)*(HAVOK_COLLISION_TOLERANCE/2.0));
	if( sign ) m_state.position(1) *= -1.0;

	// Unpack Z
	integ = dword2 >> ( xBits + yBits - 32 );
	integ &= ((1<<(zBits-1))-1);
	sign = ((dword2 & (1<<(xBits + yBits + zBits - 1 - 32 ))) != 0 );
	m_state.position(2) = (float)(((double)integ)*(HAVOK_COLLISION_TOLERANCE/2.0));
	if( sign ) m_state.position(2) *= -1.0;
}
*/
