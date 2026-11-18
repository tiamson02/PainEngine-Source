
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Physics/Havok/HavokPhysics.h"
#include "Physics/Ragdoll.h"
#include "Physics/PhysicsEngine.h"
#include "Physics/PhysicsWorld.h"
#include "Engine/Region.h"
#include "Physics/PhysicsObject.h"
#include "Engine/Model.h"
#include "Engine/Mesh.h"
#include "Engine/GlassSystem.h"
#include "Engine/World.h"
#include "HavokRagdoll.h"
#include "Physics/Havok/HavokWorld.h"

// Convenience Macro for keycodes

#ifdef HAVOK3
#define HAVOK_KEYCODE(code, value)					\
    extern const char		  HK_KEYCODE[]  = code;	\
    extern const unsigned int HK_KEYVALUE = value;	
	HAVOK_KEYCODE("CLIENT.Ph.TransGaming_Painkiller_PSP-PS2_HavokPhysics3",0x7efc4264);
#else
#define HAVOK_KEYCODE(code, value)					\
    extern const char		  HK_KEYCODE  = code;	\
    extern const unsigned int HK_KEYVALUE = value;	
	HAVOK_KEYCODE("PeopleCanFly",0x79918257);
#endif


havokAction::havokAction( hkRigidBody* body ) : hkUnaryAction( body ) {};
#ifdef HAVOK3
hkAction* havokAction::clone( const hkArray<hkEntity*>& newEntities, const hkArray<hkPhantom*>& newPhantoms ) const { return HK_NULL; }
#endif

havokAntiGravityAction::havokAntiGravityAction( hkRigidBody* body, const hkVector4& gravity ) : havokAction( body ), m_gravity( gravity ) {}
hkVector4& havokAntiGravityAction::getGravity( void ) { return m_gravity; }
void havokAntiGravityAction::setGravity( hkVector4& newGrav ) { m_gravity = newGrav; }
int havokAntiGravityAction::getType( void ) { return 0; };

void havokAntiGravityAction::applyAction( const hkStepInfo& stepInfo )
{
	GLog.Print("HavokPhysics::havokAntiGravityAction::applyAction()\n");

#ifdef HAVOK3
	hkRigidBody* body = static_cast<hkRigidBody*>(m_entity);
	if( body->getMotionType() == hkMotion::MOTION_KEYFRAMED )
#else
	if( m_body->isKeyframed() )
#endif
		return;

	hkVector4 force;
	force.setMul4( -1, m_gravity );
#ifdef HAVOK3
	float mass = body->getMass();
#else
	float mass = m_body->getMass();
#endif

#ifdef HAVOK_POSITION_BUG_TRACKING
	if( mass < 0.000001f )
	{
		Assert( mass >= 0.0f );
		Entity* enti = 0;
#ifdef HAVOK3
		if( body->hasProperty( HPEntityPointer ) )
		{
			hkPropertyValue p = body->getProperty( HPEntityPointer );
			enti = (Entity*)p.getPtr();
		}
#else
		if( m_body->hasProperty( HPEntityPointer ) )
		{
			hkPropertyValue p = m_body->getProperty( HPEntityPointer );
			enti = (Entity*)p.asPtr;
		}
#endif
		GLog.Print( "Zero mass in havokAntiGravityAction ( entity '%s, filename '%s' )\n", enti?enti->Name.AsChar():"(null)", enti?enti->FileName.AsChar():"(null)" );
	}
#endif
	force.mul4( mass );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
	if( !force.isOk4() )
#else
	if( !force.isOk() )
#endif // HAVOK3
		GLog.Print( "Corrupted force in havokAntiGravityAction\n" );
#endif

#ifdef HAVOK3
	body->applyForce( stepInfo.m_deltaTime, force );
#else
	m_body->applyForce( force );
#endif
}

havokSpeedDampingAction::havokSpeedDampingAction( hkRigidBody* body, float maxSpeed, float dampingSpeed, float dampingStrength )
:	havokAction( body ), m_maxSpeed(maxSpeed), m_dampingSpeed(dampingSpeed), m_dampingStrength(dampingStrength) {}
int havokSpeedDampingAction::getType( void ) { return 1; };

void havokSpeedDampingAction::applyAction( const hkStepInfo& stepInfo )
{
	Assert( ( m_dampingStrength > 0.0f ) && ( m_dampingStrength < 1.0f ) );
	Assert( m_maxSpeed >= m_dampingSpeed );
#ifdef HAVOK3
	hkRigidBody* body = static_cast<hkRigidBody*>(m_entity);
	hkVector4 velocity = body->getLinearVelocity();
#else
	hkVector4 velocity = m_body->getLinearVelocity();
#endif
	float speed = velocity.length3();

	if( speed >= m_maxSpeed )
		velocity.mul4( m_maxSpeed/speed );
	else if( speed >= m_dampingSpeed )
		velocity.mul4( 1.0f - m_dampingStrength );
	else
		return;
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
	if( !velocity.isOk4() || ( fabs(velocity(0)) > 1000.0f ) || ( fabs(velocity(1)) > 1000.0f ) || ( fabs(velocity(2)) > 1000.0f ) )
#else
	if( !velocity.isOk() || ( fabs(velocity(0)) > 1000.0f ) || ( fabs(velocity(1)) > 1000.0f ) || ( fabs(velocity(2)) > 1000.0f ) )
#endif
		GLog.Print( "Velocity > 1000 in havokSpeedDampingAction\n" );
#endif
#ifdef HAVOK3
	body->setLinearVelocity( velocity );
#else
	m_body->setLinearVelocity( velocity );
#endif
}

void havokSpeedDampingAction::getInfo( float& maxSpeed, float& dampingSpeed, float& dampingStrength )
{
	maxSpeed = m_maxSpeed; dampingSpeed = m_dampingSpeed; dampingStrength = m_dampingStrength;
}

void havokSpeedDampingAction::setInfo( float maxSpeed, float dampingSpeed, float dampingStrength )
{
	m_maxSpeed = maxSpeed; m_dampingSpeed = dampingSpeed; m_dampingStrength = dampingStrength;
}

havokMaintainLinearMovementAction::havokMaintainLinearMovementAction( hkRigidBody* body, hkVector4& axis, bool soft )
:	havokAction( body ), m_axis( axis ), m_soft( soft )
{
	m_position = body->getPosition();
	m_axis.normalize3();
}

void havokMaintainLinearMovementAction::setAxis( hkVector4& axis )
{
	m_axis = axis;
	m_axis.normalize3();
#ifndef HAVOK3
	m_position = m_body->getPosition();
#endif
}

void havokMaintainLinearMovementAction::setInfo( hkVector4& axis, hkVector4& position, bool soft )
{
	m_axis = axis; m_position = position; m_soft = soft;
}

void havokMaintainLinearMovementAction::getInfo( hkVector4& axis, hkVector4& position, bool& soft )
{
	axis = m_axis; position = m_position; soft = m_soft;
}

void havokMaintainLinearMovementAction::getAxis( hkVector4& axis ) { axis = m_axis; }
int havokMaintainLinearMovementAction::getType( void ) { return 2; };

void havokMaintainLinearMovementAction::applyAction( const hkStepInfo& stepInfo )
{
#ifdef HAVOK3
	hkRigidBody* body = static_cast<hkRigidBody*>(m_entity);
	hkVector4 position = body->getPosition();
#else
	hkVector4 position = m_body->getPosition();
#endif
	hkVector4 temp( position );
	temp.sub4( m_position );
	hkVector4 movement( m_axis );
	movement.mul4( temp.dot3( m_axis ) );
	movement.add4( m_position );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
	float myMass = body->getMass();
#else
	float myMass = m_body->getMass();
#endif
	if( myMass < 0.000001f )
	{
		Assert( myMass >= 0.0f );
		Entity* enti = 0;

#ifdef HAVOK3
		if( body->hasProperty( HPEntityPointer ) )
		{
			hkPropertyValue p = body->getProperty( HPEntityPointer );
			enti = (Entity*)p.getPtr();
		}
#else
		if( m_body->hasProperty( HPEntityPointer ) )
		{
			hkPropertyValue p = m_body->getProperty( HPEntityPointer );
			enti = (Entity*)p.asPtr;
		}
#endif
		GLog.Print( "Zero mass in havokMaintainLinearMovementAction ( entity '%s, filename '%s' )\n", enti?enti->Name.AsChar():"(null)", enti?enti->FileName.AsChar():"(null)" );
	}
#endif

	if( !movement.equals3( position, 0.005f ) )
	{
		if( m_soft )
		{
			hkVector4 diff( movement );
			diff.sub4( position );
#ifdef HAVOK3
			diff.mul4( body->getMass()*15.0f );
#else
			diff.mul4( m_body->getMass()*15.0f );
#endif

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !diff.isOk4() )
#else
			if( !diff.isOk() )
#endif
				GLog.Print( "Corrupted force in havokMaintainLinearMovementAction\n" );
			else if( diff.length3() > 50000.0f )
				GLog.Print( "Force > 50000 in havokMaintainLinearMovementAction\n" );
#endif

#ifdef HAVOK3
			body->applyLinearImpulse( diff );
#else
			m_body->applyLinearImpulse( diff );
#endif
		}
		else
		{
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !movement.isOk4() || ( fabs(movement(0)) > 10000.0f ) || ( fabs(movement(1)) > 10000.0f ) || ( fabs(movement(2)) > 10000.0f ) )
#else
			if( !movement.isOk() || ( fabs(movement(0)) > 10000.0f ) || ( fabs(movement(1)) > 10000.0f ) || ( fabs(movement(2)) > 10000.0f ) )
#endif
				GLog.Print( "Position > 10000 in havokMaintainLinearMovementAction\n" );
#endif
#ifdef HAVOK3
			body->setPosition( movement );
#else
			m_body->setPosition( movement );
#endif
		}
	}
}

havokMaintainPositionAction::havokMaintainPositionAction( hkRigidBody* body, hkVector4& desiredPosition, float strength )
:	havokAction( body ), m_position( desiredPosition ), m_strength( strength ) {};
void havokMaintainPositionAction::setPosition( hkVector4& position ) { m_position = position; };
void havokMaintainPositionAction::getPosition( hkVector4& position ) { position = m_position; };
void havokMaintainPositionAction::setInfo( hkVector4& position, float& strength ) { m_position = position; m_strength = strength; };
void havokMaintainPositionAction::getInfo( hkVector4& position, float& strength ) { position = m_position; strength = m_strength; };
int havokMaintainPositionAction::getType( void ) { return 3; };

void havokMaintainPositionAction::applyAction( const hkStepInfo& stepInfo )
{
#ifdef HAVOK3
	hkRigidBody* body = static_cast<hkRigidBody*>(m_entity);
	hkVector4 position = body->getPosition();
#else
	hkVector4 position = m_body->getPosition();
#endif
	hkVector4 force( m_position );
	force.sub4( position );
#ifdef HAVOK3
	force.mul4( m_strength*body->getMass() );
#else
	force.mul4( m_strength*m_body->getMass() );
#endif

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
	float myMass = body->getMass();
#else
	float myMass = m_body->getMass();
#endif
	if( myMass < 0.000001f )
	{
		Assert( myMass >= 0.0f );
		Entity* enti = 0;
#ifdef HAVOK3
		if( body->hasProperty( HPEntityPointer ) )
		{
			hkPropertyValue p = body->getProperty( HPEntityPointer );
			enti = (Entity*)p.getPtr();
		}
		GLog.Print( "Zero mass in havokMaintainPositionAction ( entity '%s, filename '%s' )\n", enti?enti->Name.AsChar():"(null)", enti?enti->FileName.AsChar():"(null)" );
#else
		if( m_body->hasProperty( HPEntityPointer ) )
		{
			hkPropertyValue p = m_body->getProperty( HPEntityPointer );
			enti = (Entity*)p.asPtr;
		}
#endif
		GLog.Print( "Zero mass in havokMaintainPositionAction ( entity '%s, filename '%s' )\n", enti?enti->Name.AsChar():"(null)", enti?enti->FileName.AsChar():"(null)" );
	}
#endif

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
	if( !force.isOk4() )
#else
	if( !force.isOk() )
#endif
		GLog.Print( "Corrupted force in havokMaintainPositionAction\n" );
	else if( force.length3() > 50000.0f )
		GLog.Print( "Force > 50000 in havokMaintainPositionAction\n" );
#endif

#ifdef HAVOK3
	body->applyLinearImpulse( force );
#else
	m_body->applyLinearImpulse( force );
#endif
}

havokMaintainVelocityAction::havokMaintainVelocityAction( hkRigidBody* body, hkVector4& desiredVelocity, float strength )
:	havokAction( body ), m_velocity( desiredVelocity ), m_strength( strength ) {};
void havokMaintainVelocityAction::setVelocity( hkVector4& velocity ) { m_velocity = velocity; };
void havokMaintainVelocityAction::getVelocity( hkVector4& velocity ) { velocity = m_velocity; };
void havokMaintainVelocityAction::setInfo( hkVector4& velocity, float& strength ) { m_velocity = velocity; m_strength = strength; };
void havokMaintainVelocityAction::getInfo( hkVector4& velocity, float& strength ) { velocity = m_velocity; strength = m_strength; };
int havokMaintainVelocityAction::getType( void ) { return 4; };

void havokMaintainVelocityAction::applyAction( const hkStepInfo& stepInfo )
{
#ifdef HAVOK3
	hkRigidBody* body = static_cast<hkRigidBody*>(m_entity);
	body->activate();
	hkVector4 velocity = body->getLinearVelocity();
	hkVector4 force( m_velocity );
	force.sub4( velocity );
	force.mul4( m_strength*body->getMass() );
#else
	m_body->forceActivate();
	hkVector4 velocity = m_body->getLinearVelocity();
	hkVector4 force( m_velocity );
	force.sub4( velocity );
	force.mul4( m_strength*m_body->getMass() );
#endif

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
	float myMass = body->getMass();
#else
	float myMass = m_body->getMass();
#endif
	if( myMass < 0.000001f )
	{
		Assert( myMass >= 0.0f );
		Entity* enti = 0;
#ifdef HAVOK3
		if( body->hasProperty( HPEntityPointer ) )
		{
			hkPropertyValue p = body->getProperty( HPEntityPointer );
			enti = (Entity*)p.getPtr();
		}
#else
		if( m_body->hasProperty( HPEntityPointer ) )
		{
			hkPropertyValue p = m_body->getProperty( HPEntityPointer );
			enti = (Entity*)p.asPtr;
		}
#endif
		GLog.Print( "Zero mass in havokMaintainVelocityAction ( entity '%s, filename '%s' )\n", enti?enti->Name.AsChar():"(null)", enti?enti->FileName.AsChar():"(null)" );
	}
#endif

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
	if( !force.isOk4() )
#else
	if( !force.isOk() )
#endif
		GLog.Print( "Corrupted force in havokMaintainVelocityAction\n" );
	else if( force.length3() > 50000.0f )
		GLog.Print( "Force > 50000 in havokMaintainVelocityAction\n" );
#endif

#ifdef HAVOK3
	body->applyLinearImpulse( force );
#else
	m_body->applyLinearImpulse( force );
#endif
}

havokMaintainAngularVelocityAction::havokMaintainAngularVelocityAction( hkRigidBody* body, hkVector4& desiredAngularVelocity, float strength )
:	havokAction( body ), m_angularVelocity( desiredAngularVelocity ), m_strength( strength ) {};
void havokMaintainAngularVelocityAction::setAngularVelocity( hkVector4& angularVelocity ) { m_angularVelocity = angularVelocity; };
void havokMaintainAngularVelocityAction::getAngularVelocity( hkVector4& angularVelocity ) { angularVelocity = m_angularVelocity; };
void havokMaintainAngularVelocityAction::setInfo( hkVector4& angVelocity, float& strength ) { m_angularVelocity = angVelocity; m_strength = strength; };
void havokMaintainAngularVelocityAction::getInfo( hkVector4& angVelocity, float& strength ) { angVelocity = m_angularVelocity; strength = m_strength; };
int havokMaintainAngularVelocityAction::getType( void ) { return 5; };

void havokMaintainAngularVelocityAction::applyAction( const hkStepInfo& stepInfo )
{
#ifndef HAVOK3 // ============================================

	hkVector4 angularVelocity = m_body->getAngularVelocity();
	hkVector4 torque( m_angularVelocity );
	torque.sub4( angularVelocity );
	torque.mul4( m_strength*m_body->getMass() );
#ifdef HAVOK_POSITION_BUG_TRACKING
	float myMass = m_body->getMass();
	if( myMass < 0.000001f )
	{
		Assert( myMass >= 0.0f );
		Entity* enti = 0;
		if( m_body->hasProperty( HPEntityPointer ) )
		{
			hkPropertyValue p = m_body->getProperty( HPEntityPointer );
#ifdef HAVOK3
			enti = (Entity*)p.getPtr();
#else
			enti = (Entity*)p.asPtr;
#endif
		}
		GLog.Print( "Zero mass in havokMaintainAngularVelocityAction ( entity '%s, filename '%s' )\n", enti?enti->Name.AsChar():"(null)", enti?enti->FileName.AsChar():"(null)" );
	}
#ifdef HAVOK3
	if( !torque.isOk4() )
#else
	if( !torque.isOk() )
#endif
		GLog.Print( "Bad angular impulse in havokMaintainAngularVelocityAction\n" );

	else if( torque.length3() > 1000.0f )
		GLog.Print( "Angular impulse > 1000 in havokMaintainAngularVelocityAction\n" );
#endif
	m_body->applyAngularImpulse( torque );

#else
	GLog.Print("HavokPhysics::havokMaintainAngularVelocityAction::applyAction() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}

havokTransporterAction::havokTransporterAction( hkRigidBody* body, hkVector4& appliedVelocity, float strength )
:	havokAction( body ), m_velocity( appliedVelocity ), m_strength( strength ) {}
void havokTransporterAction::setInfo( hkVector4& velocity, float& strength ) { m_velocity = velocity; m_strength = strength; };
void havokTransporterAction::getInfo( hkVector4& velocity, float& strength ) { velocity = m_velocity; strength = m_strength; };
int havokTransporterAction::getType( void ) { return 7; };
#ifdef HAVOK3
//hkAction* havokTransporterAction::clone( const hkArray<hkEntity*>& newEntities, const hkArray<hkPhantom*>& newPhantoms ) {}
#endif
void havokTransporterAction::applyAction( const hkStepInfo& stepInfo )
{
#ifndef HAVOK3 // ============================================
	Assert( m_body );

	// Get world from body
	hkWorld* world = m_body->getOwner();
	if( !world ) return;

	// Get other variables
	hkCollidable& coll = m_body->getCollidable();
	hkCollisionEnvironment* env = world->getCollisionEnvironment();
	const hkTransform* collidableTrans = coll.getTransform();
	const hkReal halfTolerance =  env->m_tolerance * 0.5f;		 // Get half the world tolerance

	// Get the AABB associated with the hkCollidable
	hkAabb aabb; 
	coll.getShape()->getAabb( *collidableTrans,  halfTolerance, aabb );

	// Query the AABB against the others in the hkWorld using hkBroadPhase
	hkArray<hkBroadPhaseHandlePair> pairsOut;
	world->getBroadPhase()->querySingleAabb( aabb, pairsOut );

	// For all the pairs of hkCollidables get ay hkCollisionAgent and then get the collision results
	for ( int count = 0; count < pairsOut.getSize(); ++count)
	{
		// NOTE: hkBroadPhaseHandlePair.m_a will always be NULL
		hkCollidable *otherCollidable = static_cast<hkCollidable*>(pairsOut[count].m_b); 

		// Ignore our own AABB
        if( otherCollidable && otherCollidable != &coll )
		{
			// First, get a hkRigidBody and an entity
            hkBaseObject* otherObject = (hkBaseObject*)otherCollidable->getOwner();

            if( otherObject->m_object_memsize != 132 )
                continue;

            hkRigidBody* otherBody = (hkRigidBody*)otherCollidable->getOwner();

#ifdef HAVOK3
			if( !otherBody || otherBody->isFixed() || otherBody->getMotionType() == hkMotion::MOTION_FIXED )
#else
            if( !otherBody || otherBody->isFixed() || otherBody->isKeyframed() )
#endif
				continue;

			if( otherBody->hasProperty( HPEntityPointer ) )
			{
				hkPropertyValue p = otherBody->getProperty( HPEntityPointer );
#ifdef HAVOK3
				Entity* entity = (Entity*)p.getPtr();
#else
				Entity* entity = (Entity*)p.asPtr;
#endif

				// Ok, now let's find out how to apply force to this thing
				if( entity->PO )
				{
                    m_setTag("Physics");
					hkSimpleClosestPointsDetails* details = new hkSimpleClosestPointsDetails( coll, *otherCollidable, *env );
					details->updateClosestPoints(*env);
					if( details->getClosestDistance() < env->m_tolerance )
					{
						if( entity->PO->GetType() == EPawnBodyPlayer )
						{
							// It's a player
							// Don't do anything - let the player movement code handle this
						}
						else if( entity->PO->State & EPOIsMonster )
						{
							// It's monster
							if( entity->AsModel() && entity->AsModel()->IsRagdoll() )
							{	// PO not in the world, so ragdoll collided. Probably dead monster.
								hkVector4 velocity = otherBody->getLinearVelocity();
								hkVector4 force( m_velocity );
								force.sub4( velocity );
								force.mul4( m_strength * otherBody->getMass() );
#ifdef HAVOK_POSITION_BUG_TRACKING
								float myMass = otherBody->getMass();
								if( myMass < 0.000001f )
								{
									Assert( myMass >= 0.0f );
									Entity* enti = 0;
									if( otherBody->hasProperty( HPEntityPointer ) )
									{
										hkPropertyValue p = otherBody->getProperty( HPEntityPointer );
#ifdef HAVOK3
										enti = (Entity*)p.getPtr();
#else
										enti = (Entity*)p.asPtr;
#endif
									}
									GLog.Print( "Zero mass in havokTransporterAction ( entity '%s, filename '%s' )\n", enti?enti->Name.AsChar():"(null)", enti?enti->FileName.AsChar():"(null)" );
								}
#ifdef HAVOK3
								if( !force.isOk4() )
#else
								if( !force.isOk() )
#endif
									GLog.Print( "Corrupted force in havokTransporterAction\n" );
								else if( force.length3() > 50000.0f )
									GLog.Print( "Force > 50000 in havokTransporterAction\n" );
#endif
								otherBody->applyLinearImpulse( force );
							}
							else
							{	// PO in world, living monster
								hkVector4 velocity = otherBody->getLinearVelocity();
								hkVector4 sum( m_velocity );
								sum.mul4( 0.5f );	// modifier
								sum.add4( velocity );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
								if( !sum.isOk4() || ( fabs(sum(0)) > 1000.0f ) || ( fabs(sum(1)) > 1000.0f ) || ( fabs(sum(2)) > 1000.0f ) )
#else
								if( !sum.isOk() || ( fabs(sum(0)) > 1000.0f ) || ( fabs(sum(1)) > 1000.0f ) || ( fabs(sum(2)) > 1000.0f ) )
#endif
									GLog.Print( "Velocity > 1000 in havokTransporterAction\n" );
#endif
								otherBody->setLinearVelocity( sum );
							}
						}
						else
						{
							// It's some mesh or something - get present velocity, move it to applied with some strength
							hkVector4 velocity = otherBody->getLinearVelocity();
							hkVector4 factor1( velocity );
							factor1.mul4( 1.0f - m_strength );
							hkVector4 factor2( m_velocity );
							factor2.mul4( m_strength );
							factor1.add4( factor2 );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
							if( !factor1.isOk4() || ( fabs(factor1(0)) > 1000.0f ) || ( fabs(factor1(1)) > 1000.0f ) || ( fabs(factor1(2)) > 1000.0f ) )
#else
							if( !factor1.isOk() || ( fabs(factor1(0)) > 1000.0f ) || ( fabs(factor1(1)) > 1000.0f ) || ( fabs(factor1(2)) > 1000.0f ) )
#endif
								GLog.Print( "Velocity > 1000 (2) in havokTransporterAction\n" );
#endif
							otherBody->setLinearVelocity( factor1 );
						}
					}
					details->destroy(*env);
				}
				else if( entity->AsModel() && entity->AsModel()->IsRagdoll() )
				{	// No PO, so ragdoll collided. Probably active ragdoll.

                    m_setTag("Physics");
					hkSimpleClosestPointsDetails* details = new hkSimpleClosestPointsDetails( coll, *otherCollidable, *env );
					details->updateClosestPoints(*env);
					if( details->getClosestDistance() < env->m_tolerance )
					{
						Ragdoll* ragdoll = entity->AsModel()->RagdollPtr;
						Vector vel( m_velocity(0), m_velocity(1), m_velocity(2) );
						ragdoll->SetVelocities( vel*0.5, Vector(0,0,0) );
					}
					details->destroy(*env);
				}
			}
		}
	}

#else
	GLog.Print("HavokPhysics::havokTransporterAction::applyAction() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}

void havokDisappearTrigger::entityDeactivatedCallback( hkEntity* entity ) {};
void havokDisappearTrigger::entityActivatedCallback( hkEntity* entity )
{
	// Get mesh from havok entity
	if( !entity->hasProperty( HPEntityPointer ) ) return;
	hkPropertyValue p = entity->getProperty( HPEntityPointer );
#ifdef HAVOK3
	Entity* ent = (Entity*)p.getPtr();
#else
	Entity* ent = (Entity*)p.asPtr;
#endif

	// Make mesh disappear after some time, if not already activated
	if( ent->TimeToDelete == -1 )
		ent->TimeToDelete = 10 + rand()%10;
}

havokCollisionListener::havokCollisionListener( hkRigidBody* rigidBody, float minimalDelay, float minimalStrength, float minimalMass )
{
	mTooManyContactPoints = false;
	mRigidBody = rigidBody;
	mMinimalDelay = minimalDelay;
	mMinimalStrength = minimalStrength;
	mMinimalMass = minimalMass;
#ifdef TRANSGAMING
	float  currTime = GEngine->SysDriver.GetCurrentTime();
#else
	double currTime = GEngine->SysDriver.GetCurrentTime();
#endif
	mLastTimeCalled = currTime - mMinimalDelay;
	rigidBody->addCollisionListener( this );

	mWereGlass = false;
	if( mRigidBody->hasProperty( HPGlass ) )
	{
		hkPropertyValue p = mRigidBody->getProperty( HPEntityPointer );
#ifdef HAVOK3
		if( p.getInt() )
#else
		if( p.asInt )
#endif
			mWereGlass = true;
	}
}

havokCollisionListener::~havokCollisionListener() {};
void havokCollisionListener::contactPointAddedCallback(	hkContactPointAddedEvent& event )
{
	if( mTooManyContactPoints )
#ifdef HAVOK3
		event.m_status = HK_CONTACT_POINT_REJECT;
#else
		event.m_status = HK_CONTACT_POINT_INVALID;
#endif
}
#ifdef HAVOK3
void havokCollisionListener::contactPointConfirmedCallback(hkContactPointConfirmedEvent &event) {}
#endif

void havokCollisionListener::contactPointRemovedCallback( hkContactPointRemovedEvent& event ) {};

void havokCollisionListener::contactProcessCallback( hkContactProcessEvent& event )
{
#ifdef HAVOK3
	hkProcessCollisionData& result = event.m_collisionData;
	if( !result.getNumContactPoints() ) return;

	// Great number of contact points bugfix
	if( result.getNumContactPoints() > 255 )
#else
	hkCollisionResult& result = event.m_collisionResult;
	if( !result.m_contactPoints.getSize() ) return;

	// Great number of contact points bugfix
	if( result.m_contactPoints.getSize() > 255 )
#endif
	{
		GLog.Print( "MY HAVOK ERROR: more than 255 contact points in collision! Havok probably overwrites the game memory right now :)\n" );
		GLog.Flush();
		mTooManyContactPoints = true;
		return;
	}

#ifdef HAVOK3
	if( result.getNumContactPoints() > 210 )
#else
	if( result.m_contactPoints.getSize() > 210 )
#endif
	{
		hkEntity* ent1 = (hkEntity*)event.m_collidableA.getOwner();
		hkEntity* ent2 = (hkEntity*)event.m_collidableB.getOwner();
		if( ent1 != mRigidBody ) { ent2 = ent1; ent1 = mRigidBody; };

		hkPropertyValue p = ent1->getProperty( HPEntityPointer );
#ifdef HAVOK3
		Entity* entity1 = (Entity*)p.getPtr();
		p = ent2->getProperty( HPEntityPointer );
		Entity* entity2 = (Entity*)p.getPtr();
#else
		Entity* entity1 = (Entity*)p.asPtr;
		p = ent2->getProperty( HPEntityPointer );
		Entity* entity2 = (Entity*)p.asPtr;
#endif

		GLog.Print( "MY HAVOK WARNING: more than 210 contact points in collision of '%s'(filename '%s') with '%s' (filename '%s')!\n",
			entity1->Name.AsChar(), entity1->FileName.AsChar(), entity2->Name.AsChar(), entity2->Name.AsChar() );
		GLog.Flush();

		mTooManyContactPoints = true;

		return;
	}

    mTooManyContactPoints = false;

#ifdef HAVOK3
	hkVector4 start = result.getContactPoint(0).m_contact.getPosition();
	hkVector4 hknormal = result.getContactPoint(0).m_contact.getNormal();
#else
	hkVector4 start = result.m_contactPoints[0].m_position;
	hkVector4 hknormal = result.m_contactPoints[0].m_normal;
#endif

	// Test for bad normals
	float x = hknormal(0);
	unsigned int a = *(unsigned int *)(&x);
	a &= 0x7fffffff;
	if (a > 0x7f000000) return;					// don't return collisions with #NAN normal
	if( ( x > 1.0f ) || ( x < -1.0f ) ) return;	// of with otherwise bad normal

	Vector normal = Vector(-hknormal(0),-hknormal(1),-hknormal(2));
	hkEntity* ent1 = (hkEntity*)event.m_collidableA.getOwner();
	hkEntity* ent2 = (hkEntity*)event.m_collidableB.getOwner();
//	hkWorld* world = ent1->getSimulationIsland()->getWorld();

	hkEntity* other = ( ent1 == mRigidBody )?ent2:ent1;

	hkPropertyValue p = mRigidBody->getProperty( HPEntityPointer );
#ifdef HAVOK3
	Entity* ourEntity = (Entity*)p.getPtr();
	p = other->getProperty( HPEntityPointer );
	Entity* otherEntity = (Entity*)p.getPtr();
#else
	Entity* ourEntity = (Entity*)p.asPtr;
	p = other->getProperty( HPEntityPointer );
	Entity* otherEntity = (Entity*)p.asPtr;
#endif
/*
	if( ourEntity && otherEntity )
	{
		if( ourEntity->PO && ( ourEntity->PO->GetCollisionGroup() == HCGMissile ) )
			GLog.Print( "Collision our %s %s\n", ourEntity->Name.AsChar(), otherEntity->Name.AsChar() );
		if( otherEntity->PO && ( otherEntity->PO->GetCollisionGroup() == HCGMissile ) )
			GLog.Print( "Collision other %s %s\n", ourEntity->Name.AsChar(), otherEntity->Name.AsChar() );
	}
*/
	if( other->isFixed() )
	{
		hkVector4 relVel;
		relVel = mRigidBody->getLinearVelocity();
		if( relVel.length3() < mMinimalStrength )
			return;
	}
	else
	{
		hkRigidBody* otherRB = static_cast<hkRigidBody*>(other);
		hkVector4 relVel;
		relVel = mRigidBody->getLinearVelocity();
		relVel.sub4( otherRB->getLinearVelocity() );
		if( relVel.length3() < mMinimalStrength )
			return;
	}

	if( mWereGlass )
	{
		hkRigidBody* otherRB = static_cast<hkRigidBody*>(other);
		float mass = otherRB->getMass();
		if( mass > mMinimalMass )
		{
			mMinimalMass = 10000;	// so it won't happen again
			hkPropertyValue p = mRigidBody->getProperty( HPGlass );
#ifdef HAVOK3
			Glass* ourGlass = (Glass*)p.getPtr();
#else
			Glass* ourGlass = (Glass*)p.asPtr;
#endif
			Assert( ourGlass );
			hkVector4 othVel = otherRB->getLinearVelocity();

			ourGlass->StartGlassDelayed( Vector(start(0),start(1),start(2)), 3.0f, Vector( othVel(0), othVel(1), othVel(2) ) );

			if( mRigidBody->isFixed() )
			{
				PhysicsWorld* world = GEngine->Physics->GetWorldFromRigidBody( mRigidBody );
				world->RemoveStaticMesh( mRigidBody );
			}

			return;
		}
	}

#ifdef TRANSGAMING
	float  currTime = GEngine->SysDriver.GetCurrentTime();
#else
	double currTime = GEngine->SysDriver.GetCurrentTime();
#endif 

	if( !ourEntity )
		GLog.Print( "WARNING: !ourEntity in havokCollisionListener!! Ignoring.\n" );
	else if( !otherEntity )
		GLog.Print( "WARNING: !otherEntity in havokCollisionListener!! Ignoring.\n" );
	else if( ourEntity && (!ourEntity->PO || !ourEntity->PO->IsEnabled() ) )
	{
		if( currTime - mLastTimeCalled < mMinimalDelay )
			return;

		mLastTimeCalled = currTime;

		if( ourEntity->Type == Entity::ETypeModel )
		{
			/*
			int iRagdoll = FindRagdollByModel((Model*)ourEntity);
			if( iRagdoll >= 0 )
			{
				HavokRagdoll& ragdoll = mHavokWorld->Ragdolls[iRagdoll];
				for( int iLimb = 0; iLimb < ragdoll.Limbs.Size(); iLimb++ )
					if( ragdoll.Limbs[iLimb].Body == mRigidBody )
					{
						GScript.QueueMessage("COLLISION_WITH_OTHER_ENTITY","%e,%f,%f,%f,%f,%f,%f,%e,%p,%p",ourEntity,start(0), start(1), start(2),
							normal(0), normal(1), normal(2),otherEntity,mRigidBody, other);
						return;
					}
			}
			*/
			GScript.QueueMessage("COLLISION_WITH_OTHER_ENTITY","%e,%f,%f,%f,%f,%f,%f,%e,%p,%p", ourEntity,
				start(0), start(1), start(2), normal.X, normal.Y, normal.Z, otherEntity, mRigidBody, other );
		}
	}
	else if( currTime - mLastTimeCalled >= mMinimalDelay )
	{
		mLastTimeCalled = currTime;

		if( !( ourEntity->PO && !ourEntity->PO->GetCollisionCallbacks() ) )
			GScript.QueueMessage("COLLISION_WITH_OTHER_ENTITY","%e,%f,%f,%f,%f,%f,%f,%e,%p,%p", ourEntity,
				start(0), start(1), start(2), normal.X, normal.Y, normal.Z, otherEntity, mRigidBody, other );

/*
		if( !( otherEntity->PO && !otherEntity->PO->GetCollisionCallbacks() ) )
			GScript.QueueMessage("COLLISION_WITH_OTHER_ENTITY","%e,%f,%f,%f,%f,%f,%f,%e,%p,%p", otherEntity,
				start(0), start(1), start(2), normal.X, normal.Y, normal.Z, ourEntity, other, mRigidBody);
*/
		// WARNING: last two parameters are switched here!!! It shouldn't be like this but I'm too afraid that much
		// of already written code is adapted to it to change it now.
	}
}

void havokCollisionListener::entityDeletedCallback( hkEntity* entity )
{
	entity->removeCollisionListener( this ); 
	delete this;
}

void havokCollisionListener::entityRemovedCallback( hkEntity* entity ) {}
void havokCollisionListener::entityAddedCallback( hkEntity* entity ) {}
void havokCollisionListener::getInfo( float& minimalDelay, float& minimalStrength, float& minimalMass )
{
	minimalDelay = mMinimalDelay;
	minimalStrength = mMinimalStrength;
	minimalMass = mMinimalMass;
}

#ifdef HAVOK3
havokRegion::havokRegion( Entity* ourRegion ) 
{
	// THIS IS BOGUS.....rewrite havokRegion...
	//hkSphereShape shape;
	//hkTransform transform;
	//hkShapePhantom( shape, transform );
	mRegion = ourRegion; 
	GLog.Print("HavokPhysics::havokRegion::havokRegion() - FIXME!!!!!!!!!!!\n");
}
#else
havokRegion::havokRegion( Entity* ourRegion ) { mRegion = ourRegion; };
#endif

void havokRegion::phantomEnterEvent( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env )
{
#ifdef HAVOK3
	if( collidableB.getType() != hkWorldObject::BROAD_PHASE_ENTITY ) return;
#else
	if( collidableB.getType() != HK_BROAD_PHASE_ENTITY ) return;
#endif
	hkEntity* rigid = (hkEntity*)collidableB.getOwner();
	if( rigid->isFixed() ) return;	// we don't need collision with fixed geometry
	WorldRegion* region = (WorldRegion*)mRegion;
	hkPropertyValue p = rigid->getProperty( HPEntityPointer );
#ifdef HAVOK3
	Entity* rigidEntity = (Entity*)p.getPtr();
#else
	Entity* rigidEntity = (Entity*)p.asPtr;
#endif
	region->EventTrigger( true, rigidEntity );
}

void havokRegion::phantomLeaveEvent( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env )
{
#ifdef HAVOK3
	if( collidableB.getType() != hkWorldObject::BROAD_PHASE_ENTITY ) return;
#else
	if( collidableB.getType() != HK_BROAD_PHASE_ENTITY ) return;
#endif
	hkEntity* rigid = (hkEntity*)collidableB.getOwner();
	if( rigid->isFixed() ) return;	// we don't need collision with fixed geometry
	WorldRegion* region = (WorldRegion*)mRegion;
	hkPropertyValue p = rigid->getProperty( HPEntityPointer );
#ifdef HAVOK3
	Entity* rigidEntity = (Entity*)p.getPtr();
#else
	Entity* rigidEntity = (Entity*)p.asPtr;
#endif
	region->EventTrigger( false, rigidEntity );
}

#ifdef HAVOK3
havokFilteredRaycastCallback::havokFilteredRaycastCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output )
{
	m_input = input;
	m_output = output;
}
hkReal havokFilteredRaycastCallback::addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex )
{
	const hkCollidable* col = static_cast<hkCollidable*>( static_cast<const hkTypedBroadPhaseHandle*>(broadPhaseHandle)->getOwner() );
	Assert( col );
	//if( col->getType() != hkWorldObject::BROAD_PHASE_ENTITY )
	if( col->getType() != hkWorldObject::BROAD_PHASE_ENTITY )
		return m_output->m_hitFraction;
	const hkShape* shape = col->getShape();
	if( !shape )
		return m_output->m_hitFraction;
	Assert( col );
	hkEntity* entity = (hkEntity*)col->getOwner();
	Assert( entity );

	if( entity->hasProperty( HPRayCastDontCollide ) )
		return m_output->m_hitFraction;

	// collide with raycast
	//const hkTransform* trans = col->getTransform();
	const hkTransform& trans = col->getTransform();
	hkShapeRayCastInput shapeInput;
	shapeInput.m_from.setTransformedInversePos( trans, m_input.m_from);
	shapeInput.m_to.setTransformedInversePos( trans, m_input.m_to);		
	
	if( shape->castRay(shapeInput, *m_output) )
	{
		// transform the normal back into worldspace
		m_output->m_rootCollidable = col;
		m_output->m_normal.setRotatedDir( trans.getRotation(), m_output->m_normal );
	}

	return m_output->m_hitFraction;
}
#else
havokFilteredRaycastCallback::havokFilteredRaycastCallback( const hkVector4& fromWorld, const hkVector4& toWorld, const hkShape::hkRayResults& initResults )
		: hkRayCastCallback( fromWorld, toWorld, initResults ) {}
#endif

hkReal havokFilteredRaycastCallback::castPrimitive(const hkBroadPhaseHandle* broadphaseHandle )
{
#ifndef HAVOK3 // ============================================

	hkCollidable* col = (hkCollidable*) broadphaseHandle;
#ifdef HAVOK3
	if( col->getType() != hkWorldObject::BROAD_PHASE_ENTITY )
#else
	if( col->getType() != HK_BROAD_PHASE_ENTITY )
#endif
		return m_rayResults.m_mindist;
	const hkShape* shape = col->getShape();
	if( !shape )
		return m_rayResults.m_mindist;
	Assert( col );
	hkEntity* entity = (hkEntity*)col->getOwner();
	Assert( entity );

	if( entity->hasProperty( HPRayCastDontCollide ) )
		return m_rayResults.m_mindist;

	// collide with raycast
	const hkTransform* trans = col->getTransform();
	hkVector4 shapefrom,shapeto;
	shapefrom.setTransformedInversePos( *trans, m_fromWorld );
	shapeto.setTransformedInversePos( *trans, m_toWorld );
			
	if( shape->castRay( shapefrom, shapeto, m_rayResults ) )
		m_hitObject = col;

	return m_rayResults.m_mindist;
#else
	GLog.Print("HavokPhysics::havokFilteredRaycastCallback::castPrimitive() - FIXME!!!!!!!!!!!\n");
	return 0.0f;
#endif // ifndef HAVOK3 =========================================
}

#ifdef HAVOK3
havokGrenadeFixCallback::havokGrenadeFixCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output )
{
	m_input = input;
	m_output = output;
}
hkReal havokGrenadeFixCallback::addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex )
{
	const hkCollidable* col = static_cast<hkCollidable*>( static_cast<const hkTypedBroadPhaseHandle*>(broadPhaseHandle)->getOwner() );
	Assert( col );
	if( col->getType() != hkWorldObject::BROAD_PHASE_ENTITY )
		return m_output->m_hitFraction;
	const hkShape* shape = col->getShape();
	if( !shape )
		return m_output->m_hitFraction;
	Assert( col );
	hkEntity* entity = (hkEntity*)col->getOwner();
	Assert( entity );

	if( entity->hasProperty( HPRayCastDontCollide ) )
		return m_output->m_hitFraction;

	hkPropertyValue p = entity->getProperty( HPEntityPointer );
#ifdef HAVOK3
	Entity* engineEntity = (Entity*)p.getPtr();
#else
	Entity* engineEntity = (Entity*)p.asPtr;
#endif
	if( engineEntity )
	{
		WorldMesh* mesh = engineEntity->AsMesh();
		if( mesh && ( mesh->Flags & WorldMesh::ENoClip ) && ( mesh->Flags & WorldMesh::EWater ) )
			return m_output->m_hitFraction;
	}

	// Does it have a right collision group?
	int cg = entity->getCollidable()->getCollisionFilterInfo();

	if( ( cg == HCGPhantom ) ||
		( cg == HCGBody ) ||
		( cg == HCGMissile ) ||
		( cg == HCGNoncolliding ) ||
		( cg == HCGParticles ) ||
		( cg == HCGInsideItems ) ||
		( cg == HCGBarrier ) ||
		( cg == HCGPlayerBody ) ||
		( cg == HCGOnlyWithFixedSpecial ) ||
		( cg == HCGMonsterBarrier ) ||
		( cg == HCGClientGrenade ) )
		return m_output->m_hitFraction;

	if( mClient && ( cg == HCGRagdoll ) )
		return m_output->m_hitFraction;

	// collide with raycast
	const hkTransform& trans = col->getTransform();
	hkShapeRayCastInput shapeInput;
	shapeInput.m_from.setTransformedInversePos( trans, m_input.m_from);
	shapeInput.m_to.setTransformedInversePos( trans, m_input.m_to);		
	
	if( shape->castRay(shapeInput, *m_output) )
	{
		// transform the normal back into worldspace
		m_output->m_rootCollidable = col;
		m_output->m_normal.setRotatedDir( trans.getRotation(), m_output->m_normal );
	}

	return m_output->m_hitFraction;
}
#else
havokGrenadeFixCallback::havokGrenadeFixCallback( const hkVector4& fromWorld, const hkVector4& toWorld, const hkShape::hkRayResults& initResults, bool client )
		: hkRayCastCallback( fromWorld, toWorld, initResults ), mClient( client ) {}
#endif

hkReal havokGrenadeFixCallback::castPrimitive(const hkBroadPhaseHandle* broadphaseHandle )
{
#ifndef HAVOK3 // ============================================

	hkCollidable* col = (hkCollidable*) broadphaseHandle;
#ifdef HAVOK3
	if( col->getType() != hkWorldObject::BROAD_PHASE_ENTITY )
#else
	if( col->getType() != HK_BROAD_PHASE_ENTITY )
#endif
		return m_rayResults.m_mindist;
	const hkShape* shape = col->getShape();
	if( !shape )
		return m_rayResults.m_mindist;
	Assert( col );
	hkEntity* entity = (hkEntity*)col->getOwner();
	Assert( entity );

	if( entity->hasProperty( HPRayCastDontCollide ) )
		return m_rayResults.m_mindist;

	hkPropertyValue p = entity->getProperty( HPEntityPointer );
#ifdef HAVOK3
	Entity* engineEntity = (Entity*)p.getPtr();
#else
	Entity* engineEntity = (Entity*)p.asPtr;
#endif
	if( engineEntity )
	{
		WorldMesh* mesh = engineEntity->AsMesh();
		if( mesh && ( mesh->Flags & WorldMesh::ENoClip ) && ( mesh->Flags & WorldMesh::EWater ) )
			return m_rayResults.m_mindist;
	}

	// Does it have a right collision group?
	int cg = entity->getCollidable().getCollisionFilterInfo();

	if( ( cg == HCGPhantom ) ||
		( cg == HCGBody ) ||
		( cg == HCGMissile ) ||
		( cg == HCGNoncolliding ) ||
		( cg == HCGParticles ) ||
		( cg == HCGInsideItems ) ||
		( cg == HCGBarrier ) ||
		( cg == HCGPlayerBody ) ||
		( cg == HCGOnlyWithFixedSpecial ) ||
		( cg == HCGMonsterBarrier ) ||
		( cg == HCGClientGrenade ) )
		return m_rayResults.m_mindist;

	if( mClient && ( cg == HCGRagdoll ) )
		return m_rayResults.m_mindist;

	// collide with raycast
	const hkTransform* trans = col->getTransform();
	hkVector4 shapefrom,shapeto;
	shapefrom.setTransformedInversePos( *trans, m_fromWorld );
	shapeto.setTransformedInversePos( *trans, m_toWorld );
			
	if( shape->castRay( shapefrom, shapeto, m_rayResults ) )
		m_hitObject = col;

	return m_rayResults.m_mindist;

#else
	GLog.Print("HavokPhysics::havokGrenadeFixCallback::castPrimitive() - FIXME!!!!!!!!!!!\n");
	return 0.0f;
#endif // ifndef HAVOK3 =========================================
}

#ifdef HAVOK3
havokFilteredRaycastSpecialCallback::havokFilteredRaycastSpecialCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output )
{
	m_input = input;
	m_output = output;
}

hkReal havokFilteredRaycastSpecialCallback::addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex )
{
	const hkCollidable* col = static_cast<hkCollidable*>( static_cast<const hkTypedBroadPhaseHandle*>(broadPhaseHandle)->getOwner() );
	Assert( col );
	if( col->getType() != hkWorldObject::BROAD_PHASE_ENTITY )
		return m_output->m_hitFraction;
	const hkShape* shape = col->getShape();
	if( !shape )
		return m_output->m_hitFraction;
	Assert( col );
	hkEntity* entity = (hkEntity*)col->getOwner();
	Assert( entity );

	if( entity->hasProperty( HPRayCastDontCollide ) )
	{
		hkPropertyValue p = entity->getProperty( HPEntityPointer );
#ifdef HAVOK3
		Entity* engineEntity = (Entity*)p.getPtr();
#else
		Entity* engineEntity = (Entity*)p.asPtr;
#endif
		if( !engineEntity )
			return m_output->m_hitFraction;
		PhysicsObject* po = engineEntity->PO;
		if( !po )
			return m_output->m_hitFraction;
		if( !( po->State & EPOIsPlayer ) )
			return m_output->m_hitFraction;
	}

	// collide with raycast
	const hkTransform& trans = col->getTransform();
	hkShapeRayCastInput shapeInput;
	shapeInput.m_from.setTransformedInversePos( trans, m_input.m_from);
	shapeInput.m_to.setTransformedInversePos( trans, m_input.m_to);		
	
	if( shape->castRay(shapeInput, *m_output) )
	{
		// transform the normal back into worldspace
		m_output->m_rootCollidable = col;
		m_output->m_normal.setRotatedDir( trans.getRotation(), m_output->m_normal );
	}

	return m_output->m_hitFraction;
}
#else
havokFilteredRaycastSpecialCallback::havokFilteredRaycastSpecialCallback( const hkVector4& fromWorld, const hkVector4& toWorld,
		const hkShape::hkRayResults& initResults )
		: hkRayCastCallback( fromWorld, toWorld, initResults ) {};
#endif

hkReal havokFilteredRaycastSpecialCallback::castPrimitive(const hkBroadPhaseHandle* broadphaseHandle )
{
#ifndef HAVOK3 // ============================================

	hkCollidable* col = (hkCollidable*) broadphaseHandle;
#ifdef HAVOK3
	if( col->getType() != hkWorldObject::BROAD_PHASE_ENTITY )
#else
	if( col->getType() != HK_BROAD_PHASE_ENTITY )
#endif
		return m_rayResults.m_mindist;
	const hkShape* shape = col->getShape();
	if( !shape )
		return m_rayResults.m_mindist;
	Assert( col );
	hkEntity* entity = (hkEntity*)col->getOwner();
	Assert( entity );

	if( entity->hasProperty( HPRayCastDontCollide ) )
	{
		hkPropertyValue p = entity->getProperty( HPEntityPointer );
#ifdef HAVOK3
		Entity* engineEntity = (Entity*)p.getPtr();
#else
		Entity* engineEntity = (Entity*)p.asPtr;
#endif
		if( !engineEntity )
			return m_rayResults.m_mindist;
		PhysicsObject* po = engineEntity->PO;
		if( !po )
			return m_rayResults.m_mindist;
		if( !( po->State & EPOIsPlayer ) )
			return m_rayResults.m_mindist;
	}

	// collide with raycast
	const hkTransform* trans = col->getTransform();
	hkVector4 shapefrom,shapeto;
	shapefrom.setTransformedInversePos( *trans, m_fromWorld );
	shapeto.setTransformedInversePos( *trans, m_toWorld );
			
	if( shape->castRay( shapefrom, shapeto, m_rayResults ) )
		m_hitObject = col;

	return m_rayResults.m_mindist;
#else
	GLog.Print("havokFilteredRaycastSpecialCallback::castPrimitive() - FIXME!!!!!!!!!!!\n");
	return 0.0f;
#endif // ifndef HAVOK3 =========================================
}

#ifdef HAVOK3
havokFirstHitRaycastCallback::havokFirstHitRaycastCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output )
{
	m_input = input;
	m_output = output;
}
hkReal havokFirstHitRaycastCallback::addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex )
{
	const hkCollidable* col = static_cast<hkCollidable*>( static_cast<const hkTypedBroadPhaseHandle*>(broadPhaseHandle)->getOwner() );
	Assert( col );
	if( col->getType() != hkWorldObject::BROAD_PHASE_ENTITY )
		return m_output->m_hitFraction;

	const hkShape* shape = col->getShape();
	if( !shape )
		return m_output->m_hitFraction;
	Assert( col );
	hkEntity* entity = (hkEntity*)col->getOwner();
	Assert( entity );

	if( entity->hasProperty( HPRayCastDontCollide ) )
		return m_output->m_hitFraction;

	if( entity->hasProperty( HPGlass ) )
	{
		hkPropertyValue p = entity->getProperty( HPGlass );
#ifdef HAVOK3
		if( p.getInt() )
#else
		if( p.asInt )
#endif
			return m_output->m_hitFraction;
	};

	const hkTransform& trans = col->getTransform();
	hkShapeRayCastInput shapeInput;
	shapeInput.m_from.setTransformedInversePos( trans, m_input.m_from);
	shapeInput.m_to.setTransformedInversePos( trans, m_input.m_to);		
	
	if( shape->castRay(shapeInput, *m_output) )
	{
		// transform the normal back into worldspace
		m_output->m_rootCollidable = col;
		m_output->m_normal.setRotatedDir( trans.getRotation(), m_output->m_normal );
	}

	// First hit speed-up addition
	if( m_output->m_hitFraction < 1.0f )
		m_output->m_hitFraction = 0.0f;

	return m_output->m_hitFraction;
}
#else
havokFirstHitRaycastCallback::havokFirstHitRaycastCallback( const hkVector4& fromWorld, const hkVector4& toWorld,
		const hkShape::hkRayResults& initResults )
		: hkRayCastCallback( fromWorld, toWorld, initResults ) {}
#endif

hkReal havokFirstHitRaycastCallback::castPrimitive(const hkBroadPhaseHandle* broadphaseHandle )
{
#ifndef HAVOK3 // ============================================

	hkCollidable* col = (hkCollidable*) broadphaseHandle;
#ifdef HAVOK
	if( col->getType() != hkWorldObject::BROAD_PHASE_ENTITY  )
#else
	if( col->getType() != HK_BROAD_PHASE_ENTITY )
#endif
		return m_rayResults.m_mindist;
	const hkShape* shape = col->getShape();
	if( !shape )
		return m_rayResults.m_mindist;
	Assert( col );
	hkEntity* entity = (hkEntity*)col->getOwner();
	Assert( entity );

	if( entity->hasProperty( HPRayCastDontCollide ) )
		return m_rayResults.m_mindist;

	if( entity->hasProperty( HPGlass ) )
	{
		hkPropertyValue p = entity->getProperty( HPGlass );
#ifdef HAVOK3
		if( p.getInt() )
#else
		if( p.asInt )
#endif
			return m_rayResults.m_mindist;
	};

	// collide with raycast
	const hkTransform* trans = col->getTransform();
	hkVector4 shapefrom,shapeto;
	shapefrom.setTransformedInversePos( *trans, m_fromWorld );
	shapeto.setTransformedInversePos( *trans, m_toWorld );
			
	if( shape->castRay( shapefrom, shapeto, m_rayResults ) )
		m_hitObject = col;

	// First hit speed-up addition
	if( m_rayResults.m_mindist < 1.0f )
		m_rayResults.m_mindist = 0.0f;

	return m_rayResults.m_mindist;

#else
	GLog.Print("HavokPhysics::havokFirstHitRaycastCallback::castPrimitive() - FIXME!!!!!!!!!!!\n");
	return 0.0f;
#endif // ifndef HAVOK3 =========================================
}

#ifdef HAVOK3
havokFixedGeometryRaycastCallback::havokFixedGeometryRaycastCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output )
{
	m_input = input;
	m_output = output;
}
hkReal havokFixedGeometryRaycastCallback::addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex )
{
	const hkCollidable* col = static_cast<hkCollidable*>( static_cast<const hkTypedBroadPhaseHandle*>(broadPhaseHandle)->getOwner() );
	Assert( col );
	if( col->getType() != hkWorldObject::BROAD_PHASE_ENTITY )
		return m_output->m_hitFraction;
	const hkShape* shape = col->getShape();
	if( !shape )
		return m_output->m_hitFraction;
	Assert( col );
	hkEntity* entity = (hkEntity*)col->getOwner();
	Assert( entity );

	if( !entity->isFixed() )
		return m_output->m_hitFraction;

	// collide with raycast
	const hkTransform& trans = col->getTransform();
	hkShapeRayCastInput shapeInput;
	shapeInput.m_from.setTransformedInversePos( trans, m_input.m_from);
	shapeInput.m_to.setTransformedInversePos( trans, m_input.m_to);		
	
	if( shape->castRay(shapeInput, *m_output) )
	{
		// transform the normal back into worldspace
		m_output->m_rootCollidable = col;
		m_output->m_normal.setRotatedDir( trans.getRotation(), m_output->m_normal );
	}

	return m_output->m_hitFraction;
}
#else
havokFixedGeometryRaycastCallback::havokFixedGeometryRaycastCallback( const hkVector4& fromWorld, const hkVector4& toWorld,
		const hkShape::hkRayResults& initResults )
		: hkRayCastCallback( fromWorld, toWorld, initResults ) {};
#endif

hkReal havokFixedGeometryRaycastCallback::castPrimitive(const hkBroadPhaseHandle* broadphaseHandle )
{
#ifndef HAVOK3 // ============================================

	hkCollidable* col = (hkCollidable*) broadphaseHandle;
#ifdef HAVOK3
	if( col->getType() != hkWorldObject::BROAD_PHASE_ENTITY )
#else
	if( col->getType() != HK_BROAD_PHASE_ENTITY )
#endif
		return m_rayResults.m_mindist;
	const hkShape* shape = col->getShape();
	if( !shape )
		return m_rayResults.m_mindist;
	Assert( col );
	hkEntity* entity = (hkEntity*)col->getOwner();
	Assert( entity );

	if( !entity->isFixed() )
		return m_rayResults.m_mindist;

	// collide with raycast
	const hkTransform* trans = col->getTransform();
	hkVector4 shapefrom,shapeto;
	shapefrom.setTransformedInversePos( *trans, m_fromWorld );
	shapeto.setTransformedInversePos( *trans, m_toWorld );
			
	if( shape->castRay( shapefrom, shapeto, m_rayResults ) )
		m_hitObject = col;

	return m_rayResults.m_mindist;

#else
	GLog.Print("HavokPhysics::havokFixedGeometryRaycastCallback::castPrimitive() - FIXME!!!!!!!!!!!\n");
	return 0.0f;
#endif // ifndef HAVOK3 =========================================
}

#ifdef HAVOK3
havokMovementFixRaycastCallback::havokMovementFixRaycastCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output )
{
	m_input = input;
	m_output = output;
}
hkReal havokMovementFixRaycastCallback::addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex )
{
	const hkCollidable* col = static_cast<hkCollidable*>( static_cast<const hkTypedBroadPhaseHandle*>(broadPhaseHandle)->getOwner() );
	Assert( col );
	if( col->getType() != hkWorldObject::BROAD_PHASE_ENTITY )
		return m_output->m_hitFraction;
	const hkShape* shape = col->getShape();
	if( !shape )
		return m_output->m_hitFraction;
	Assert( col );
	hkEntity* entity = (hkEntity*)col->getOwner();
	Assert( entity );

	if( !entity->isFixed() )
		return m_output->m_hitFraction;

	int cg = entity->getCollidable()->getCollisionFilterInfo();
	if( ( cg == HCGNoncolliding ) || ( cg == HCGMonsterBarrier ) ) // water or monster barrier
		return m_output->m_hitFraction;

	// collide with raycast
	const hkTransform& trans = col->getTransform();
	hkShapeRayCastInput shapeInput;
	shapeInput.m_from.setTransformedInversePos( trans, m_input.m_from);
	shapeInput.m_to.setTransformedInversePos( trans, m_input.m_to);		
	
	if( shape->castRay(shapeInput, *m_output) )
	{
		// transform the normal back into worldspace
		m_output->m_rootCollidable = col;
		m_output->m_normal.setRotatedDir( trans.getRotation(), m_output->m_normal );
	}

	return m_output->m_hitFraction;
}
#else
havokMovementFixRaycastCallback::havokMovementFixRaycastCallback( const hkVector4& fromWorld, const hkVector4& toWorld,
		const hkShape::hkRayResults& initResults )
		: hkRayCastCallback( fromWorld, toWorld, initResults ) {};
#endif

hkReal havokMovementFixRaycastCallback::castPrimitive(const hkBroadPhaseHandle* broadphaseHandle )
{
#ifndef HAVOK3 // ============================================
	hkCollidable* col = (hkCollidable*) broadphaseHandle;
	if( col->getType() != HK_BROAD_PHASE_ENTITY )
		return m_rayResults.m_mindist;
	const hkShape* shape = col->getShape();
	if( !shape )
		return m_rayResults.m_mindist;
	Assert( col );
	hkEntity* entity = (hkEntity*)col->getOwner();
	Assert( entity );

	if( !entity->isFixed() )
		return m_rayResults.m_mindist;

	int cg = entity->getCollidable().getCollisionFilterInfo();
	if( ( cg == HCGNoncolliding ) || ( cg == HCGMonsterBarrier ) ) // water or monster barrier
		return m_rayResults.m_mindist;

	// collide with raycast
	const hkTransform* trans = col->getTransform();
	hkVector4 shapefrom,shapeto;
	shapefrom.setTransformedInversePos( *trans, m_fromWorld );
	shapeto.setTransformedInversePos( *trans, m_toWorld );
			
	if( shape->castRay( shapefrom, shapeto, m_rayResults ) )
		m_hitObject = col;

	return m_rayResults.m_mindist;
#else
	GLog.Print("havokMovementFixRaycastCallback::castPrimitive() - FIXME!!!!!!!!!!!\n");
	return 0.0f;
#endif // ifndef HAVOK3 =========================================

}

#ifdef HAVOK3
havokFloorCheckRaycastCallback::havokFloorCheckRaycastCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output ) 
{
	m_input = input;
	m_output = output;
}
hkReal havokFloorCheckRaycastCallback::addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex )
{
	const hkCollidable* col = static_cast<hkCollidable*>( static_cast<const hkTypedBroadPhaseHandle*>(broadPhaseHandle)->getOwner() );
	Assert( col );
	if( col->getType() != hkWorldObject::BROAD_PHASE_ENTITY )
		return m_output->m_hitFraction;
	const hkShape* shape = col->getShape();
	if( !shape )
		return m_output->m_hitFraction;

	hkRigidBody* rb = (hkRigidBody*)col->getOwner();
	Assert( rb );

	if( fixedGeometryOnly && !rb->isFixed() )
		return m_output->m_hitFraction;

	int cg = rb->getCollidable()->getCollisionFilterInfo();

	if( rb->hasProperty( HPRayCastDontCollide ) )
	{
		if( cg != HCGBarrier )
			return m_output->m_hitFraction;
	}
	else
	{
		if( rb->hasProperty( HPGlass ) )
		{
			hkPropertyValue p = rb->getProperty( HPGlass );
			if( p.getInt() )
				return m_output->m_hitFraction;
		}

		int cg = rb->getCollidable()->getCollisionFilterInfo();

		if( ( cg == HCGMissile ) ||
			( cg == HCGRagdoll ) ||
			( cg == HCGNoncolliding ) ||
			( cg == HCGParticles ) ||
			( cg == HCGInsideItems ) ||
			( cg == HCGActiveRagdollSpecial ) ||
			( cg == HCGOnlyWithFixedSpecial ) )
			return m_output->m_hitFraction;
	}

	// collide with raycast
	const hkTransform& trans = col->getTransform();
	hkShapeRayCastInput shapeInput;
	//sinput.m_from.setTransformedInversePos( *trans, m_fromWorld);
	//sinput.m_to.setTransformedInversePos( *trans, m_toWorld);
	shapeInput.m_rayShapeCollectionFilter = HK_NULL;
	shapeInput.m_from.setTransformedInversePos( trans, m_input.m_from);
	shapeInput.m_to.setTransformedInversePos( trans, m_input.m_to);		
	if( shape->castRay(shapeInput, *m_output) )
	{
		// transform the normal back into worldspace
		m_output->m_rootCollidable = col;
		m_output->m_normal.setRotatedDir( trans.getRotation(), m_output->m_normal );
	}
	return m_output->m_hitFraction;
}

#else
havokFloorCheckRaycastCallback::havokFloorCheckRaycastCallback( const hkVector4& fromWorld, const hkVector4& toWorld,
		const hkShape::hkRayResults& initResults )
		: hkRayCastCallback( fromWorld, toWorld, initResults ), fixedGeometryOnly( false ) {};
#endif

hkReal havokFloorCheckRaycastCallback::castPrimitive(const hkBroadPhaseHandle* broadphaseHandle )
{
#ifndef HAVOK3 // ============================================

	hkCollidable* col = (hkCollidable*) broadphaseHandle;
	Assert( col );
	if( col->getType() != HK_BROAD_PHASE_ENTITY )
		return m_rayResults.m_mindist;
	const hkShape* shape = col->getShape();
	if( !shape )
		return m_rayResults.m_mindist;

	hkRigidBody* rb = (hkRigidBody*)col->getOwner();
	Assert( rb );

	if( fixedGeometryOnly && !rb->isFixed() )
		return m_rayResults.m_mindist;

	int cg = rb->getCollidable().getCollisionFilterInfo();

	if( rb->hasProperty( HPRayCastDontCollide ) )
	{
		if( cg != HCGBarrier )
			return m_rayResults.m_mindist;
	}
	else
	{
		if( rb->hasProperty( HPGlass ) )
		{
			hkPropertyValue p = rb->getProperty( HPGlass );
			if( p.asInt )
				return m_rayResults.m_mindist;
		}

		int cg = rb->getCollidable().getCollisionFilterInfo();

		if( ( cg == HCGMissile ) ||
			( cg == HCGRagdoll ) ||
			( cg == HCGNoncolliding ) ||
			( cg == HCGParticles ) ||
			( cg == HCGInsideItems ) ||
			( cg == HCGActiveRagdollSpecial ) ||
			( cg == HCGOnlyWithFixedSpecial ) )
			return m_rayResults.m_mindist;
	}

	// collide with raycast
	const hkTransform* trans = col->getTransform();
	hkVector4 shapefrom,shapeto;
	shapefrom.setTransformedInversePos( *trans, m_fromWorld );
	shapeto.setTransformedInversePos( *trans, m_toWorld );
			
	if( shape->castRay( shapefrom, shapeto, m_rayResults ) )
		m_hitObject = col;

	// First hit speed-up addition
//	if( m_rayResults.m_mindist < 1.0f )
//		m_rayResults.m_mindist = 0.0f;

	return m_rayResults.m_mindist;
#else
	GLog.Print("havokFloorCheckRaycastCallback::castPrimitive() - FIXME!!!!!!!!!!!\n");
	return 0.0f;
#endif // ifndef HAVOK3 =========================================
}

#ifdef HAVOK3
havokMonsterFloorCheckRaycastCallback::havokMonsterFloorCheckRaycastCallback( const hkWorldRayCastInput& input, hkWorldRayCastOutput* output )
{
	m_input = input;
	m_output = output;
}
hkReal havokMonsterFloorCheckRaycastCallback::addBroadPhaseHandle( const hkBroadPhaseHandle* broadPhaseHandle, int castIndex )
{
	const hkCollidable* col = static_cast<hkCollidable*>( static_cast<const hkTypedBroadPhaseHandle*>(broadPhaseHandle)->getOwner() );
	Assert( col );
	if( col->getType() != hkWorldObject::BROAD_PHASE_ENTITY )
		return m_output->m_hitFraction;
	const hkShape* shape = col->getShape();
	if( !shape )
		return m_output->m_hitFraction;

	hkRigidBody* rb = (hkRigidBody*)col->getOwner();
	Assert( rb );

	if( fixedGeometryOnly && !rb->isFixed() )
		return m_output->m_hitFraction;

	int cg = rb->getCollidable()->getCollisionFilterInfo();

	if( rb->hasProperty( HPRayCastDontCollide ) )
	{
		if( cg != HCGMonsterBarrier )
			return m_output->m_hitFraction;
	}
	else
	{
		if( rb->hasProperty( HPGlass ) )
		{
			hkPropertyValue p = rb->getProperty( HPGlass );
			if( p.getInt() )
				return m_output->m_hitFraction;
		}

		int cg = rb->getCollidable()->getCollisionFilterInfo();

		if( ( cg == HCGMissile ) ||
			( cg == HCGRagdoll ) ||
			( cg == HCGNoncolliding ) ||
			( cg == HCGParticles ) ||
			( cg == HCGInsideItems ) ||
			( cg == HCGActiveRagdollSpecial ) ||
			( cg == HCGOnlyWithFixedSpecial ) )
			return m_output->m_hitFraction;
	}
	// collide with raycast
	const hkTransform& trans = col->getTransform();
	//hkVector4 shapefrom,shapeto;
	hkShapeRayCastInput sinput;
	//sinput.m_from.setTransformedInversePos( *trans, m_fromWorld);
	//sinput.m_to.setTransformedInversePos( *trans, m_toWorld);
	sinput.m_rayShapeCollectionFilter = HK_NULL;
	sinput.m_from.setTransformedInversePos( trans, m_input.m_from);
	sinput.m_to.setTransformedInversePos( trans, m_input.m_to);
	//shapefrom.setTransformedInversePos( *trans, m_fromWorld );
	//shapeto.setTransformedInversePos( *trans, m_toWorld );
			
	if( shape->castRay(sinput, *m_output) )
	{
		// transform the normal back into worldspace
		m_output->m_rootCollidable = col;
		m_output->m_normal.setRotatedDir( trans.getRotation(), m_output->m_normal );
	}

	// First hit speed-up addition
//	if( m_rayResults.m_mindist < 1.0f )
//		m_rayResults.m_mindist = 0.0f;

	return m_output->m_hitFraction;

//#else
//	GLog.Print("havokMonsterFloorCheckRaycastCallback::castPrimitive() - FIXME!!!!!!!!!!!\n");
//	return 0.0f;
//#endif // ifndef HAVOK3 =========================================
}
#else
havokMonsterFloorCheckRaycastCallback::havokMonsterFloorCheckRaycastCallback( const hkVector4& fromWorld, const hkVector4& toWorld,
		const hkShape::hkRayResults& initResults )
		: hkRayCastCallback( fromWorld, toWorld, initResults ), fixedGeometryOnly( false ) {};
#endif

hkReal havokMonsterFloorCheckRaycastCallback::castPrimitive(const hkBroadPhaseHandle* broadphaseHandle )
{
#ifndef HAVOK3 // ============================================
	hkCollidable* col = (hkCollidable*) broadphaseHandle;
	Assert( col );
	if( col->getType() != HK_BROAD_PHASE_ENTITY )
		return m_rayResults.m_mindist;
	const hkShape* shape = col->getShape();
	if( !shape )
		return m_rayResults.m_mindist;

	hkRigidBody* rb = (hkRigidBody*)col->getOwner();
	Assert( rb );

	if( fixedGeometryOnly && !rb->isFixed() )
		return m_rayResults.m_mindist;

	int cg = rb->getCollidable().getCollisionFilterInfo();

	if( rb->hasProperty( HPRayCastDontCollide ) )
	{
		if( cg != HCGMonsterBarrier )
			return m_rayResults.m_mindist;
	}
	else
	{
		if( rb->hasProperty( HPGlass ) )
		{
			hkPropertyValue p = rb->getProperty( HPGlass );
			if( p.asInt )
				return m_rayResults.m_mindist;
		}

		int cg = rb->getCollidable().getCollisionFilterInfo();

		if( ( cg == HCGMissile ) ||
			( cg == HCGRagdoll ) ||
			( cg == HCGNoncolliding ) ||
			( cg == HCGParticles ) ||
			( cg == HCGInsideItems ) ||
			( cg == HCGActiveRagdollSpecial ) ||
			( cg == HCGOnlyWithFixedSpecial ) )
			return m_rayResults.m_mindist;
	}

	// collide with raycast
	const hkTransform* trans = col->getTransform();
	hkVector4 shapefrom,shapeto;
	shapefrom.setTransformedInversePos( *trans, m_fromWorld );
	shapeto.setTransformedInversePos( *trans, m_toWorld );
			
	if( shape->castRay( shapefrom, shapeto, m_rayResults ) )
		m_hitObject = col;

	// First hit speed-up addition
//	if( m_rayResults.m_mindist < 1.0f )
//		m_rayResults.m_mindist = 0.0f;

	return m_rayResults.m_mindist;

#else
	GLog.Print("havokMonsterFloorCheckRaycastCallback::castPrimitive() - FIXME!!!!!!!!!!!\n");
	return 0.0f;
#endif // ifndef HAVOK3 =========================================

}

//#pragma warning(disable:4511)
#ifdef HAVOK3
havokStepCheckPhantom::havokStepCheckPhantom( hkAabb& info, float minimalMass ) : hkAabbPhantom( info ), mMinimalMass( minimalMass ) {}
#else
havokStepCheckPhantom::havokStepCheckPhantom( hkPhantomConstructionInfo& info, float minimalMass ) : hkAabbPhantom( info ), mMinimalMass( minimalMass ) {}
#endif

#ifdef HAVOK3
/*
hkPhantomType havokStepCheckPhantom::getType() const
{
	GLog.Print("havokStepCheckPhantom::getType() - FIXME!!!\n");
	return HK_PHANTOM_AABB;
}
void havokStepCheckPhantom::calcAabb( hkAabb& aabb )
{
	GLog.Print("havokStepCheckPhantom::calcAabb() - FIXME!!!\n");
}
hkBool havokStepCheckPhantom::isOverlappingCollidableAdded( hkCollidable* collidable )
{
	GLog.Print("havokStepCheckPhantom::isOverlappingCollidableAdded() - FIXME!!!\n");
	return 0;
}
hkPhantom* havokStepCheckPhantom::clone() const
{
	GLog.Print("havokStepCheckPhantom::clone() - FIXME!!!\n");
	return HK_NULL;
}
void havokStepCheckPhantom::deallocateInternalArrays()
{
	// I think this is an internal function only...
	GLog.Print("havokStepCheckPhantom::deallocateInternalArrays() - FIXME!!!\n");
}
*/
#endif

void havokStepCheckPhantom::addOverlappingCollidable( hkCollidable& c )
{
#ifdef HAVOK3
	if( c.getType() == hkWorldObject::BROAD_PHASE_ENTITY )
#else
	if( c.getType() == HK_BROAD_PHASE_ENTITY )
#endif
	{
		hkRigidBody* rb = static_cast<hkRigidBody*>(c.getOwner());
#ifdef HAVOK3
		hkUint32 cf = rb->getCollidable()->getCollisionFilterInfo();
		if( ( rb->isFixed() && !( cf == HCGMonsterBarrier ) ) ||
			( ( ( cf == HCGNormal ) || ( cf == HCGFixed ) || ( ( cf >= HCGActiveRagdoll ) && ( cf <= HCGActiveRagdollSpecial ) ) ) &&
			( rb->getMotionType() == hkMotion::MOTION_KEYFRAMED || ( rb->getMass() > mMinimalMass ) ) ) )
			hkAabbPhantom::addOverlappingCollidable( &c );
#else
		hkUint32 cf = rb->getCollidable().getCollisionFilterInfo();
		if( ( rb->isFixed() && !( cf == HCGMonsterBarrier ) ) ||
			( ( ( cf == HCGNormal ) || ( cf == HCGFixed ) || ( ( cf >= HCGActiveRagdoll ) && ( cf <= HCGActiveRagdollSpecial ) ) ) &&
			( rb->isKeyframed() || ( rb->getMass() > mMinimalMass ) ) ) )
			hkAabbPhantom::addOverlappingCollidable( c );
#endif
	}
}

void havokStepCheckPhantom::removeOverlappingCollidable( hkCollidable& c )
{
#ifdef HAVOK3
	if( c.getType() == hkWorldObject::BROAD_PHASE_ENTITY )
		hkAabbPhantom::removeOverlappingCollidable( &c );
#else
	if( c.getType() == HK_BROAD_PHASE_ENTITY )
		hkAabbPhantom::removeOverlappingCollidable( c );
#endif
}
//#pragma warning(default:4511)

class hkCollisionDispatcher;
void hkUserSphereTriangleAgent::registerAgent(hkCollisionDispatcher* dispatcher)
{
#ifndef HAVOK3 // ============================================
    dispatcher->registerCollisionAgent(&createSphereTriangleAgent, HK_SHAPE_SPHERE, HK_SHAPE_TRIANGLE);	
    dispatcher->registerCollisionAgent(&createTriangleSphereAgent, HK_SHAPE_TRIANGLE, HK_SHAPE_SPHERE);	
#else
	GLog.Print("HavokPhysics::hkUserSphereTriangleAgent::registerAgent() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
};

#ifdef HAVOK3
void hkUserSphereTriangleAgent::getPenetrations( const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input, hkCdBodyPairCollector& collector )
{
}
void hkUserSphereTriangleAgent::linearCast( const hkCdBody& bodyA, const hkCdBody& bodyB, const hkLinearCastCollisionInput& input, hkCdPointCollector& castCollector, hkCdPointCollector* startCollector )
{
}

#endif

#ifdef HAVOK3
inline void hkUserSphereTriangleAgent::processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkProcessCollisionInput& env, hkProcessCollisionOutput& result)
#else
inline void hkUserSphereTriangleAgent::processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result)
#endif
{
#ifndef HAVOK3 // ============================================
	hkReal	dist;
	hkVector4 pointInWorld;
	hkVector4 normalInWorld;

	if( getClosestPoint( collA, collB, dist, pointInWorld, normalInWorld, env ) )
	{
		hkCollisionContactPoint& point = *result.m_contactPoints.expandBy(1);
		point.m_distance = dist;
		point.m_normal = normalInWorld;
		point.m_position = pointInWorld;
		
		if( m_contactPointId == HK_INVALID_CONTACT_POINT )
			m_contactPointId = env.m_contactMgr->addContactPoint(collA, collB, env, point );
		point.m_contactPointId = m_contactPointId;
	}
	else
	{
		if( m_contactPointId != HK_INVALID_CONTACT_POINT )
		{
			env.m_contactMgr->removeContactPoint(m_contactPointId, env );
			m_contactPointId = HK_INVALID_CONTACT_POINT;
		}
	}
#else
	GLog.Print("HavokPhysics::hkUserSphereTriangleAgent::processCollision() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}

#ifdef HAVOK3
void hkUserSphereTriangleAgent::getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionInput& env, hkCdBodyPairCollector& pointDetails)
#else
void hkUserSphereTriangleAgent::getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, hkClosestPointsDetails& pointDetails)
#endif
{
#ifndef HAVOK3 // ============================================
	hkReal	dist;
	hkVector4 pointInWorld;
	hkVector4 normalInWorld;
	
	if( getClosestPoint( collidableA, collidableB, dist, pointInWorld, normalInWorld, env ) )
	{
		hkClosestPointsDetails::ClosestPointEvent event;
		event.m_collidableA = &collidableA;
		event.m_collidableB = &collidableB;
		event.m_pointInWorld = pointInWorld;
		event.m_normalInWorld = normalInWorld;
		event.m_distance = dist;
		pointDetails.addClosestPointCallback( event );
	}
#else
	GLog.Print("HavokPhysics::hkUserSphereTriangleAgent::getClosestPoints() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}
			
bool hkUserSphereTriangleAgent::arePenetrating(const  hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env )
{
#ifndef HAVOK3 // ============================================
	const hkSphereShape* sphereA = static_cast<const hkSphereShape*>(collA.m_shape);
	const hkTriangleShape* triB = static_cast<const hkTriangleShape*>(collB.m_shape);

	const hkVector4& posA = collA.getTransform()->getTranslation();

	hkVector4 triVertices[3];
	for( int i = 0; i < 3; i ++ )
		triVertices[i]._setTransformedPos( *collB.getTransform(), triB->getVertex(i) );

	hkCollideTriangleUtil::ClosestPointTriangleResult cptr;
	hkCollideTriangleUtil::closestPointTriangle( posA, triVertices, m_closestPointTriangleCache, cptr );
    
	const hkVector4& posB = cptr.closestPoint;

	hkVector4 vec;    vec.setSub4( posB, posA );

	const hkReal distSquared = vec.dot3(vec);
	const hkReal radiusSum = sphereA->getRadius() + triB->getRadius();

	return (distSquared < radiusSum * radiusSum);

#else
	GLog.Print("HavokPhysics::hkUserSphereTriangleAgent::arePenetrating() - FIXME!!!!!!!!!!!\n");
	return 0.0f;
#endif // ifndef HAVOK3 =========================================
}

#ifdef HAVOK3
void hkUserSphereTriangleAgent::cleanup( const hkCollisionInput& env )
#else
void hkUserSphereTriangleAgent::cleanup( const hkCollisionEnvironment& env )
#endif
{
#ifndef HAVOK3 // ============================================
	if( m_contactPointId != HK_INVALID_CONTACT_POINT )
	{
		env.m_contactMgr->removeContactPoint(m_contactPointId, env );
		//m_contactPointId = HK_INVALID_CONTACT_POINT;
	}

	delete this;
#else
	GLog.Print("HavokPhysics::hkUserSphereTriangleAgent::cleanup() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}

#ifdef HAVOK3
hkUserSphereTriangleAgent::hkUserSphereTriangleAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionInput& env ) : hkCollisionAgent( HK_NULL )
#else
hkUserSphereTriangleAgent::hkUserSphereTriangleAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env )
#endif
{
#ifndef HAVOK3 // ============================================
	m_contactPointId = HK_INVALID_CONTACT_POINT;
	const hkTriangleShape* triB = static_cast<const hkTriangleShape*>(collB.m_shape);

	const hkVector4* vertices = triB->getVertices();
//	hkCollideTriangleUtil::setupClosestPointTriangleCache( &vertices[0], m_closestPointTriangleCache );
	{
		const hkVector4* tri = &vertices[0];

		hkVector4 Q; Q.setSub4(tri[0], tri[1]);      
		hkVector4 R; R.setSub4(tri[2], tri[1]);

		m_closestPointTriangleCache.m_QQ = Q.lengthSquared3();
		m_closestPointTriangleCache.m_RR = R.lengthSquared3();

		m_closestPointTriangleCache.m_QR = R.dot3(Q);

		hkReal QQRR = m_closestPointTriangleCache.m_QQ * m_closestPointTriangleCache.m_RR;
		hkReal QRQR = m_closestPointTriangleCache.m_QR * m_closestPointTriangleCache.m_QR;
		hkReal Det = (QQRR - QRQR);

//		HK_ASSERT2( Det != 0, "possible degenerate triangle encountered");

		if( Det != 0 )
			m_closestPointTriangleCache.m_invDet = 1.0f / Det;
	}

	hkVector4 edge0; edge0.setSub4(vertices[1], vertices[0]);
	hkVector4 edge1; edge1.setSub4(vertices[1], vertices[2]);
	hkVector4 trinormal; trinormal.setCross( edge0, edge1 );
	hkReal len = trinormal.lengthSquared3();

	m_invTriangleNormalLength = 1.0f / len;
#else
	GLog.Print("HavokPhysics::hkUserSphereTriangleAgent::hkUserSphereTriangleAgent() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}

hkCollisionAgent* hkUserSphereTriangleAgent::createSphereTriangleAgent(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env)
{
#ifndef HAVOK3 // ============================================
    m_setTag("Physics");
    return new hkUserSphereTriangleAgent( collA, collB, env );
#else
	GLog.Print("HavokPhysics::hkUserSphereTriangleAgent::createSphereTriangleAgent() - FIXME!!!!!!!!!!!\n");
	return HK_NULL;
#endif // ifndef HAVOK3 =========================================
}
        
hkCollisionAgent* hkUserSphereTriangleAgent::createTriangleSphereAgent(	const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env)
{
#ifndef HAVOK3 // ============================================
    m_setTag("Physics");
	hkUserSphereTriangleAgent* agent = new hkSymmetricAgent<hkUserSphereTriangleAgent>(collA, collB, env);
	return agent;
#else
	GLog.Print("HavokPhysics::hkUserSphereTriangleAgent::createTriangleSphereAgent() - FIXME!!!!!!!!!!!\n");
	return HK_NULL;
#endif // ifndef HAVOK3 =========================================
}

HK_FORCE_INLINE bool hkUserSphereTriangleAgent::getClosestPoint( const hkCollidable& collidableA, const hkCollidable& collidableB, hkReal&	dist, hkVector4& pointInWorld, hkVector4& normalInWorld, const hkCollisionEnvironment& environment )
{
#ifndef HAVOK3 // ============================================
	const hkSphereShape* sphereA = static_cast<const hkSphereShape*>(collidableA.m_shape);
	const hkTriangleShape* triB = static_cast<const hkTriangleShape*>(collidableB.m_shape);

	const hkVector4& posA = collidableA.getTransform()->getTranslation();

	hkVector4 triVertices[3];
	hkVector4Util::transformPoints( *collidableB.getTransform(), triB->getVertices(), 3, &triVertices[0]);

	hkVector4 edge0; edge0.setSub4(triVertices[1], triVertices[0]);
	hkVector4 edge1; edge1.setSub4(triVertices[1], triVertices[2]);
	hkVector4 trinormal; trinormal.setCross( edge0, edge1 );
	trinormal.mul4( m_invTriangleNormalLength );

	hkCollideTriangleUtil::ClosestPointTriangleResult cptr;
	hkCollideTriangleUtil::closestPointTriangle( posA, &triVertices[0], m_closestPointTriangleCache, cptr );
    
	const hkVector4& posB = cptr.closestPoint;

    hkVector4 vec;    vec.setSub4( posB, posA );

    const hkReal distSquared = vec.dot3(vec);
    const hkReal radiusSum = sphereA->getRadius() + triB->getRadius() + environment.m_tolerance;
 
	{
		hkVector4 vec2; vec2.setSub4( triVertices[0], posA );
		hkReal dist2 = trinormal.dot3( vec2 );

		if ( dist2 > 0 )
			return false;

		if ( dist2 < -radiusSum )
			return false;
	}

    if ( distSquared < radiusSum * radiusSum )
    {
        hkReal invDist = hkMath::sqrtInverse(distSquared);

		normalInWorld.setMul4( -invDist, vec );
		dist = distSquared * invDist - (sphereA->getRadius() + triB->getRadius());

		hkVector4 dir; dir.setMul4( dist * 0.5f + triB->getRadius(), normalInWorld );
		pointInWorld.setAdd4(  posB, dir );
		
		return true;
    }
	return false;
#else
	GLog.Print("HavokPhysics::hkUserSphereTriangleAgent::getClosestPoint() - FIXME!!!!!!!!!!!\n");
	return false;
#endif // ifndef HAVOK3 =========================================
}

void hkUserMultiSphereTriangleAgent::registerAgent(hkCollisionDispatcher* dispatcher)
{
#ifndef HAVOK3 // ============================================
	dispatcher->registerCollisionAgent(&createMultiSphereTriangleAgent, HK_SHAPE_MULTI_SPHERE, HK_SHAPE_TRIANGLE);	
	dispatcher->registerCollisionAgent(&createTriangleMultiSphereAgent, HK_SHAPE_TRIANGLE, HK_SHAPE_MULTI_SPHERE);	
#else
	GLog.Print("HavokPhysics::hkUserMultiSphereTriangleAgent::registerAgent() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}

#ifdef HAVOK3
void hkUserMultiSphereTriangleAgent::getPenetrations( const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input, hkCdBodyPairCollector& collector )
{
}
void hkUserMultiSphereTriangleAgent::linearCast( const hkCdBody& bodyA, const hkCdBody& bodyB, const hkLinearCastCollisionInput& input, hkCdPointCollector& castCollector, hkCdPointCollector* startCollector )
{
}
#endif

// hkCollisionAgent interface implementation.
#ifdef HAVOK3
inline void hkUserMultiSphereTriangleAgent::processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkProcessCollisionInput& env, hkProcessCollisionOutput& result)
#else
inline void hkUserMultiSphereTriangleAgent::processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result)
#endif
{
#ifndef HAVOK3 // ============================================
	const hkMultiSphereShape* msA = static_cast<const hkMultiSphereShape*>(collA.m_shape);
	const hkTriangleShape* triB = static_cast<const hkTriangleShape*>(collB.m_shape);

	const hkVector4* vertices = triB->getVertices();

	hkVector4 triVertices[3];
	hkVector4Util::transformPoints(*collB.getTransform(), vertices, 3, triVertices );

	const hkVector4* localSpheres = msA->getSpheres();
	hkVector4 worldSpheres[hkMultiSphereShape::MAX_SPHERES];

	const int nsphere = msA->getNumSpheres();

	hkVector4Util::transformPoints( *collA.getTransform(), &localSpheres[0], nsphere, &worldSpheres[0]  );

	hkVector4 edge0; edge0.setSub4(triVertices[1], triVertices[0]);
	hkVector4 edge1; edge1.setSub4(triVertices[1], triVertices[2]);
	hkVector4 trinormal; trinormal.setCross( edge0, edge1 );
	trinormal.mul4( m_invTriangleNormalLength );

	const hkVector4* curSphere = &worldSpheres[0];
	const hkVector4* localSphere = &localSpheres[0];

	result.m_contactPoints.reserve( nsphere + result.m_contactPoints.getSize() );

	for( int j = nsphere-1; j>=0 ; curSphere++,localSphere++,j-- )
	{
		hkReal sphereRadius = (*localSphere)(3);
		const hkReal radiusSum = sphereRadius + triB->getRadius() + env.m_tolerance;

		{
			hkVector4 vec; vec.setSub4( triVertices[0], *curSphere );
			hkReal dist = trinormal.dot3( vec );
			if( dist > 0 )
				goto removeContactPoint;
			if ( dist < -radiusSum )
				goto removeContactPoint;
		}

		{
			hkCollideTriangleUtil::ClosestPointTriangleResult cptr;
			hkCollideTriangleUtil::closestPointTriangle( *curSphere, &triVertices[0], m_closestPointTriangleCache, cptr );
			const hkVector4& posB = cptr.closestPoint;
			hkVector4 vec; vec.setSub4( posB, *curSphere );
			const hkReal distSquared = vec.lengthSquared3();

			if ( distSquared < radiusSum * radiusSum )
			{
				hkReal invDist = hkMath::sqrtInverse(distSquared);

				hkCollisionContactPoint& point = *result.m_contactPoints.expandByUnchecked(1);
				point.m_normal.setMul4( -invDist, vec );
				point.m_distance = distSquared * invDist - (sphereRadius + triB->getRadius());

				hkVector4 dir; dir.setMul4( point.m_distance * 0.5f + triB->getRadius(), point.m_normal );
				point.m_position.setAdd4(  posB, dir );

				if(m_contactPointId[j] == HK_INVALID_CONTACT_POINT)
					m_contactPointId[j] = env.m_contactMgr->addContactPoint(collA, collB, env, point );
				point.m_contactPointId = m_contactPointId[j];
				continue;
			}
		}

		{
			removeContactPoint:
			if( m_contactPointId[j] != HK_INVALID_CONTACT_POINT )
			{
				env.m_contactMgr->removeContactPoint(m_contactPointId[j], env );
				m_contactPointId[j] = HK_INVALID_CONTACT_POINT;
			}
		}
	}
#else
	GLog.Print("HavokPhysics::hkUserMultiSphereTriangleAgent::processCollision() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}

bool hkUserMultiSphereTriangleAgent::arePenetrating( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env )
{
#ifndef HAVOK3 // ============================================
	const hkMultiSphereShape* msA = static_cast<const hkMultiSphereShape*>(collA.m_shape);
	const hkVector4* localSpheres = msA->getSpheres();
	hkVector4 worldSpheres[hkMultiSphereShape::MAX_SPHERES];
	const int nsphere = msA->getNumSpheres();
	hkVector4Util::transformPoints( *collA.getTransform(), &localSpheres[0], nsphere, &worldSpheres[0]  );

    const hkTriangleShape* triB = static_cast<const hkTriangleShape*>(collB.m_shape);

	hkVector4 triVertices[3];
	for (int i = 0; i < 3; i ++ )
		triVertices[i]._setTransformedPos( *collB.getTransform(), triB->getVertex(i) );

	for( int i = 0; i < nsphere; ++i )
	{
		hkCollideTriangleUtil::ClosestPointTriangleResult cptr;
		hkCollideTriangleUtil::closestPointTriangle( worldSpheres[i], triVertices, m_closestPointTriangleCache, cptr );
		const hkVector4& posB = cptr.closestPoint;

		hkVector4 vec;    vec.setSub4( posB, worldSpheres[i] );

		const hkReal distSquared = vec.dot3(vec);
		const hkReal radiusSum = localSpheres[i](3) + triB->getRadius();

		if( distSquared < radiusSum * radiusSum )
			return true;
	}

	return false;
#else
	GLog.Print("HavokPhysics::hkUserMultiSphereTriangleAgent::arePenetrating() - FIXME!!!!!!!!!!!\n");
	return false;
#endif // ifndef HAVOK3 =========================================
}

// hkCollisionAgent interface implementation.
#ifdef HAVOK3
void hkUserMultiSphereTriangleAgent::getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionInput& env, hkCdBodyPairCollector& pointDetails)
#else
void hkUserMultiSphereTriangleAgent::getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, hkClosestPointsDetails& pointDetails)
#endif
{
#ifndef HAVOK3 // ============================================
	const hkMultiSphereShape* msA = static_cast<const hkMultiSphereShape*>(collidableA.m_shape);
	const hkTriangleShape* triB = static_cast<const hkTriangleShape*>(collidableB.m_shape);

	const hkVector4* vertices = triB->getVertices();

	hkVector4 triVertices[3];
	hkVector4Util::transformPoints(*collidableB.getTransform(), vertices, 3, triVertices );

	const hkVector4* localSpheres = msA->getSpheres();
	hkVector4 worldSpheres[hkMultiSphereShape::MAX_SPHERES];
	const int nsphere = msA->getNumSpheres();

	hkVector4Util::transformPoints( *collidableA.getTransform(), &localSpheres[0], nsphere, &worldSpheres[0]  );

	for( int i = 0; i < nsphere; ++i )
	{
		const hkVector4& curSphere = worldSpheres[i];

		hkReal sphereRadius = localSpheres[i](3);
		const hkReal radiusSum = sphereRadius + triB->getRadius() + env.m_tolerance;

		{
			hkCollideTriangleUtil::ClosestPointTriangleResult cptr;
			hkCollideTriangleUtil::closestPointTriangle( curSphere, &triVertices[0], m_closestPointTriangleCache, cptr );
			const hkVector4& posB = cptr.closestPoint;
			hkVector4 vec; vec.setSub4( posB, curSphere );
			const hkReal distSquared = vec.dot3(vec);

			if( distSquared < radiusSum * radiusSum )
			{
				hkReal invDist = hkMath::sqrtInverse(distSquared);

				hkVector4 normalInWorld;
				hkVector4 pointInWorld;
				hkReal	distance;

				normalInWorld.setMul4( -invDist, vec );
				distance = distSquared * invDist - (sphereRadius + triB->getRadius());
				hkVector4 dir; dir.setMul4( distance * 0.5f + triB->getRadius(), normalInWorld );
				pointInWorld.setAdd4( posB, dir );

				hkClosestPointsDetails::ClosestPointEvent event;
				event.m_pointInWorld = pointInWorld;
				
				event.m_normalInWorld = normalInWorld;
				event.m_collidableA = &collidableA;
				event.m_collidableB = &collidableB;
				event.m_distance = distance;

				pointDetails.addClosestPointCallback( event );
			}
		}
	}
#else
	GLog.Print("HavokPhysics::hkUserMultiSphereTriangleAgent::getClosestPoints() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}
	
// hkCollisionAgent interface implementation.
#ifdef HAVOK3
void hkUserMultiSphereTriangleAgent::cleanup( const hkCollisionInput& env)
#else
void hkUserMultiSphereTriangleAgent::cleanup( const hkCollisionEnvironment& env)
#endif
{
#ifndef HAVOK3 // ============================================
	for( int i = 0; i < hkMultiSphereShape::MAX_SPHERES; ++i )
		if( m_contactPointId[i] != HK_INVALID_CONTACT_POINT )
			env.m_contactMgr->removeContactPoint(m_contactPointId[i], env );
	delete this;
#else
	GLog.Print("HavokPhysics::hkUserMultiSphereTriangleAgent::cleanup() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}

#ifdef HAVOK3
hkUserMultiSphereTriangleAgent::hkUserMultiSphereTriangleAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionInput& env ) : hkCollisionAgent( HK_NULL )
#else
hkUserMultiSphereTriangleAgent::hkUserMultiSphereTriangleAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env )
#endif
{
#ifndef HAVOK3 // ============================================

//	const hkMultiSphereShape* msA = static_cast<const hkMultiSphereShape*>(collA.m_shape);
	for( int i = 0; i < hkMultiSphereShape::MAX_SPHERES; i++ )
		m_contactPointId[i] = HK_INVALID_CONTACT_POINT;

	const hkTriangleShape* triB = static_cast<const hkTriangleShape*>(collB.m_shape);
	const hkVector4* vertices = triB->getVertices();
//	hkCollideTriangleUtil::setupClosestPointTriangleCache( &vertices[0], m_closestPointTriangleCache );

	{
		const hkVector4* tri = &vertices[0];

		hkVector4 Q; Q.setSub4(tri[0], tri[1]);      
		hkVector4 R; R.setSub4(tri[2], tri[1]);

		m_closestPointTriangleCache.m_QQ = Q.lengthSquared3();
		m_closestPointTriangleCache.m_RR = R.lengthSquared3();

		m_closestPointTriangleCache.m_QR = R.dot3(Q);

		hkReal QQRR = m_closestPointTriangleCache.m_QQ * m_closestPointTriangleCache.m_RR;
		hkReal QRQR = m_closestPointTriangleCache.m_QR * m_closestPointTriangleCache.m_QR;
		hkReal Det = (QQRR - QRQR);

//		HK_ASSERT2( Det != 0, "possible degenerate triangle encountered");

		if( Det != 0 )
			m_closestPointTriangleCache.m_invDet = 1.0f / Det;
	}

	hkVector4 edge0; edge0.setSub4(vertices[1], vertices[0]);
	hkVector4 edge1; edge1.setSub4(vertices[1], vertices[2]);
	hkVector4 trinormal; trinormal.setCross( edge0, edge1 );
	hkReal len = trinormal.lengthSquared3();

	m_invTriangleNormalLength = 1.0f / len;
#else
	GLog.Print("HavokPhysics::hkUserMultiSphereTriangleAgent::hkUserMultiSphereTriangleAgent() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}

hkCollisionAgent* hkUserMultiSphereTriangleAgent::createMultiSphereTriangleAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env)
{
#ifdef HAVOK3
	GLog.Print("HavokPhysics::hkUserMultiSphereTriangleAgent::createTriangleMultiSphereAgent() - FIXME!!!!!!!!!!!\n");
	return HK_NULL;
#else
    m_setTag("Physics");
    return new hkUserMultiSphereTriangleAgent( collA, collB, env );
#endif
}
        
hkCollisionAgent* hkUserMultiSphereTriangleAgent::createTriangleMultiSphereAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env)
{
#ifdef HAVOK3
	GLog.Print("HavokPhysics::hkUserMultiSphereTriangleAgent::createTriangleMultiSphereAgent() - FIXME!!!!!!!!!!!\n");
	return HK_NULL;
#else
    m_setTag("Physics");
	return new hkSymmetricAgent<hkUserMultiSphereTriangleAgent>(collA, collB, env);
#endif
}

#ifdef HAVOK3
havokErrorHandler::~havokErrorHandler(){
    enableAll();
}

void havokErrorHandler::message(Message m, int id, const char* description, const char* file, int line){
    const char *msg;

    if (isEnabled(id)){
        switch (m){
            case MESSAGE_REPORT:
                msg = "havokErrorHandler::message(): HAVOKREPORT (%s (%d))-> '%s' (id = %d)\n";
                break;

            case MESSAGE_WARNING:
                msg = "havokErrorHandler::message(): HAVOKWARNING (%s (%d))-> '%s' (id = %d)\n";
                break;

            case MESSAGE_ASSERT:
                msg = "havokErrorHandler::message(): HAVOKASSERT (%s (%d))-> '%s' (id = %d)\n";
                break;

            case MESSAGE_ERROR:
                msg = "havokErrorHandler::message(): HAVOKERROR (%s (%d))-> '%s' (id = %d)\n";
                break;

            default:
                GLog.Print("havokErrorHandler::message(): ERROR-> unknown havok error type! (file = '%s', line = %d, description = '%s', id = %d)\n", file, line, description, id);
                TRACE("havokErrorHandler::message(): ERROR-> unknown havok error type! (file = '%s', line = %d, description = '%s', id = %d)\n", file, line, description, id);
                return;
        }

        GLog.Print(msg, file, line, description, id);
        TRACE(msg, file, line, description, id);
    }
}

void havokErrorHandler::setEnabled( int id, hkBool enabled ){
    if (enabled){
        for (int i = 0; i < m_disabledAsserts.getSize(); i++){
            if (m_disabledAsserts[i] == id){
                m_disabledAsserts.removeAt(i);

                return;
            }
        }
    }

    else{
        for (int i = 0; i < m_disabledAsserts.getSize(); i++){
            if (m_disabledAsserts[i] == id)
                return;
        }

        m_disabledAsserts.pushBack(id);
    }
}

hkBool havokErrorHandler::isEnabled( int id ){
    for (int i = 0; i < m_disabledAsserts.getSize(); i++){
        if (m_disabledAsserts[i] == id)
            return false;
    }

    return true;
}

void havokErrorHandler::enableAll(){
    m_disabledAsserts.clearAndDeallocate();
}

#else
havokErrorHandler::~havokErrorHandler( void )
{
	for( int i = 0; i < m_disabledAsserts.getSize(); ++i )
		hkDeallocate(m_disabledAsserts[i].m_file);
}

void havokErrorHandler::assertMessage(const char* what, const char* description, const char* file, int line)
{
	if( isDisabled(file, line) == false )
	{
#ifdef DEBUG
		GLog.Print( "HAVOK ASSERT - Problem: %s\n", what );
		GLog.Print( "HAVOK ASSERT - Description: %s\n", description );
		GLog.Print( "HAVOK ASSERT - File: %s, Line: %d\n", file, line );
		GLog.Flush();
#endif
	}
}

void havokErrorHandler::errorMessage(const char* what, const char* description, const char* file, int line)
{
	if( isDisabled(file, line) == false )
	{
#ifdef DEBUG
		GLog.Print( "HAVOK ERROR - Problem: %s\n", what );
		GLog.Print( "HAVOK ERROR - Description: %s\n", description );
		GLog.Print( "HAVOK ERROR - File: %s, Line: %d\n", file, line );
		GLog.Flush();
#endif
	}
}

void havokErrorHandler::warningMessage(const char* what, const char* description, const char* file, int line)
{
	if( isDisabled(file, line) == false )
	{
#ifdef DEBUG
	GLog.Print( "HAVOK WARNING - Problem: %s\n", what );
	GLog.Print( "HAVOK WARNING - Description: %s\n", description );
	GLog.Print( "HAVOK WARNING - File: %s, Line: %d\n", file, line );
	GLog.Flush();
#endif
	}
}

void havokErrorHandler::disableAssert( const char *file, int line ) 
{
	const char* filename = hkGetFileBaseName( file );

	hkAssertPoint ap;
	ap.m_file = hkString::strDup( filename );
	ap.m_line = line;
	m_disabledAsserts.pushBack(ap);
}

const char* havokErrorHandler::hkGetFileBaseName( const char* path )
{
	const char* slash = "/\\";
	for(int i = 0; i < 2; ++i)
	{
		const char *filename = hkString::strRchr( path, slash[i] );
		if( filename != HK_NULL )
			path = filename + 1;
	}
	return path;
}

bool havokErrorHandler::isDisabled(const char* file, int line)
{
	if ( m_disabledAsserts.getSize() )
	{
		const char* filename = hkGetFileBaseName( file );
				
		for (int i=0; i < m_disabledAsserts.getSize(); i++)
		{
			hkAssertPoint ap = m_disabledAsserts[i];
			if( ( hkString::strCasecmp( filename, ap.m_file ) == 0 ) && (line == ap.m_line) )
				return true;
		}
	}
	return false;
}
#endif

hkReferencedObject* havokErrorHandler::create() { 
    m_setTag("Physics");
    return new havokErrorHandler(); 
}

havokPhysicsObjectDeactivator::havokPhysicsObjectDeactivator( void )
{
	mMinHighFrequencyLinearVelocitySquared = 1.0f;
	mMinHighFrequencyAngularVelocitySquared = 1.0f;
	mMinLowFrequencyLinearVelocitySquared = 1.0f;
	mMinLowFrequencyAngularVelocitySquared = 1.0f;
}

#ifdef HAVOK3
hkBool havokPhysicsObjectDeactivator::shouldDeactivateHighFrequency( const hkEntity* entity )
#else
bool havokPhysicsObjectDeactivator::shouldDeactivateHighFrequency( const hkEntity* entity )
#endif
{
	hkVector4 linVel = ((hkRigidBody*)entity)->getLinearVelocity();
	hkVector4 angVel = ((hkRigidBody*)entity)->getAngularVelocity();

	if( ( angVel.lengthSquared3() < mMinHighFrequencyAngularVelocitySquared ) &&
		( linVel.lengthSquared3() < mMinHighFrequencyLinearVelocitySquared ) )
		return true;
	return false;
}

#ifdef HAVOK3
hkBool havokPhysicsObjectDeactivator::shouldDeactivateLowFrequency( const hkEntity* entity )
#else
bool havokPhysicsObjectDeactivator::shouldDeactivateLowFrequency( const hkEntity* entity )
#endif
{
	hkVector4 linVel = ((hkRigidBody*)entity)->getLinearVelocity();
	hkVector4 angVel = ((hkRigidBody*)entity)->getAngularVelocity();

	if( ( angVel.lengthSquared3() < mMinLowFrequencyAngularVelocitySquared ) &&
		( linVel.lengthSquared3() < mMinLowFrequencyLinearVelocitySquared ) )
		return true;
	return false;
}

#define DEACTIV_TIMES_TO_FAIL 3

myPhysicsObjectDeactivator::myPhysicsObjectDeactivator( float deactDelay, float deactMaxPosDiff )
{
	mMinHighFrequencyLinearVelocitySquared = 0.05f*0.05f;
	mMinHighFrequencyAngularVelocitySquared = 0.001f*0.001f;
	mTimeFromLastTest = 0.0f;
	mTimerTime = 0.3f;
	mTimeFromLastPosChange = 0.0f;
	mTimerPosChangeTime = deactDelay;
	mMinMovementSquared = deactMaxPosDiff;
}

void myPhysicsObjectDeactivator::AddRigidBody( hkRigidBody* body )
{
	Assert( mBodiesToTest.Find( body ) == -1 );
	mBodiesToTest.Add( body );
	mBodyFailedLastTime.Add( false );
	hkVector4 pos = body->getCenterOfMassInWorld();
	Vector ourPos( pos(0), pos(1), pos(2) );
	mBodyPosLastTime.Add( ourPos );
}

void myPhysicsObjectDeactivator::RemoveRigidBody( hkRigidBody* body )
{
	int index = mBodiesToTest.Find( body );
	if( index != -1 )
	{
		mBodiesToTest.FastRemove( index );
		mBodyFailedLastTime.FastRemove( index );
		mBodyPosLastTime.FastRemove( index );
	}
}

//#define LOG_MY_DEACTIVATOR

void myPhysicsObjectDeactivator::DeactivateBodies( float deltaTime )
{
	DynamicArray<hkRigidBody*> bodiesToStop;
	DynamicArray<hkSimulationIsland*> islands;

	mTimeFromLastPosChange += deltaTime;
	if( mTimeFromLastPosChange >= mTimerPosChangeTime )
	{
		mTimeFromLastPosChange -= mTimerPosChangeTime*floor(mTimeFromLastPosChange/mTimerPosChangeTime);

		for( int i = 0; i < mBodiesToTest.Size(); ++i )
		{
			if( !mBodiesToTest[i]->getSimulationIsland() )
				continue;

			hkVector4 pos = mBodiesToTest[i]->getCenterOfMassInWorld();
			if( mBodiesToTest[i]->isActive() )
			{
				hkVector4 deltaPos = pos;
				hkVector4 lastTime( mBodyPosLastTime[i].X, mBodyPosLastTime[i].Y, mBodyPosLastTime[i].Z );
				deltaPos.sub4( lastTime );

				if( deltaPos.lengthSquared3() < mMinMovementSquared )
				{
					bodiesToStop.Add( mBodiesToTest[i] );
					islands.AddUnique( mBodiesToTest[i]->getSimulationIsland() );
				}
			}

			mBodyPosLastTime[i].X = pos(0);
			mBodyPosLastTime[i].Y = pos(1);
			mBodyPosLastTime[i].Z = pos(2);
		}
	}

#ifdef LOG_MY_DEACTIVATOR
	if( bodiesToStop.Size() )
		GLog.Print( "Krystian: custom deactivator - displacement method - bodies to stop: %d - ", bodiesToStop.Size() );
#endif

	int count = 0;
	for( int i = 0; i < islands.Size(); ++i )
	{
		const hkArray<hkEntity*>& ents = islands[i]->getEntities();
		bool ok = true;
		for( int j = 0; j < ents.getSize(); ++j )
#ifdef HAVOK3
			if( (((hkRigidBody*)ents[j])->getMotionType() != hkMotion::MOTION_KEYFRAMED ) && ( bodiesToStop.Find( (hkRigidBody*)ents[j] ) == -1 ) )
#else
			if( !((hkRigidBody*)ents[j])->isKeyframed() && ( bodiesToStop.Find( (hkRigidBody*)ents[j] ) == -1 ) )
#endif
			{
				ok = false;
				break;
			}

		if( ok && ents.getSize() )
		{
#ifdef HAVOK3
			((hkRigidBody*)ents[0])->deactivate();
#else
			((hkRigidBody*)ents[0])->forceDeactivate();
#endif
			count += ents.getSize();
		}
	}


#ifdef LOG_MY_DEACTIVATOR
	if( bodiesToStop.Size() )
		GLog.Print( "stopped: %d, not stopped: %d\n", count, bodiesToStop.Size()-count );
#endif

	bodiesToStop.ClearArray();
	islands.ClearArray();

	mTimeFromLastTest += deltaTime;
	if( mTimeFromLastTest >= mTimerTime )
	{
		mTimeFromLastTest -= mTimerTime*floor(mTimeFromLastTest/mTimerTime);

		for( int i = 0; i < mBodiesToTest.Size(); ++i )
		{
			if( !mBodiesToTest[i]->getSimulationIsland() )
				continue;

			if( mBodiesToTest[i]->isActive() )
			{
				hkVector4 linVel = mBodiesToTest[i]->getLinearVelocity();
				hkVector4 angVel = mBodiesToTest[i]->getAngularVelocity();

				if( ( angVel.lengthSquared3() < mMinHighFrequencyAngularVelocitySquared ) &&
					( linVel.lengthSquared3() < mMinHighFrequencyLinearVelocitySquared ) )
				{
					if( mBodyFailedLastTime[i] == DEACTIV_TIMES_TO_FAIL )
					{
						bodiesToStop.Add( mBodiesToTest[i] );
						islands.AddUnique( mBodiesToTest[i]->getSimulationIsland() );
						mBodyFailedLastTime[i]--;
					}
					mBodyFailedLastTime[i]++;
				}
				else
					mBodyFailedLastTime[i] = 0;

			}
			else
				mBodyFailedLastTime[i] = 0;
		}
	}

#ifdef LOG_MY_DEACTIVATOR
	if( bodiesToStop.Size() )
		GLog.Print( "Krystian: custom deactivator - speed method - bodies to stop: %d - ", bodiesToStop.Size() );
#endif

	count = 0;
	for( int i = 0; i < islands.Size(); ++i )
	{
		const hkArray<hkEntity*>& ents = islands[i]->getEntities();

		bool ok = true;
		for( int j = 0; j < ents.getSize(); ++j )
		{
			float vel = ((hkRigidBody*)ents[j])->getLinearVelocity().lengthSquared3();
			float mass = ((hkRigidBody*)ents[j])->getMass();

			// Ruins columns fix
			if (mass <  0.000001f)
			{
				TRACE("@@@@@ Doing Mass fix to 1.0f original values mass (%f) vel (%f)",mass, vel);
				mass = 1.0f;
			}
			vel = vel*30.0f/mass;

			if( vel > 10.0f*mMinHighFrequencyLinearVelocitySquared )
			{
				ok = false;
				break;
			}
		}

		if( ok && ents.getSize() )
		{
#ifdef HAVOK3
			((hkRigidBody*)ents[0])->deactivate();
#else
			((hkRigidBody*)ents[0])->forceDeactivate();
#endif
			count += ents.getSize();
		}
	}

#ifdef LOG_MY_DEACTIVATOR
	if( bodiesToStop.Size() )
	{
		int count = 0;
		for( int i = 0; i < bodiesToStop.Size(); ++i )
			if( !bodiesToStop[i]->isActive() )
				count++;
		GLog.Print( "stopped: %d, not stopped: %d\n", count, bodiesToStop.Size()-count );
	}
#endif
}

#undef DEACTIV_TIMES_TO_FAIL

#ifdef HAVOK3
// BLAIR - This needs to be looked at....
havokWorldBorder::havokWorldBorder( hkWorld* world, hkReal broadPhaseSize ) : hkBroadPhaseBorder( world, hkWorldCinfo::BROADPHASE_BORDER_DO_NOTHING  ) {}
#else
havokWorldBorder::havokWorldBorder( hkWorld* world, hkReal broadPhaseSize ) : hkBroadPhaseBorder( world, broadPhaseSize ) {}
#endif

void havokWorldBorder::maxPositionExceededCallback( hkEntity* entity )
{
	hkRigidBody* rb = reinterpret_cast<hkRigidBody*>( entity );
	hkPropertyValue p = rb->getProperty( HPEntityPointer );
#ifdef HAVOK3
	Entity* ourEntity = (Entity*)p.getPtr();
#else
	Entity* ourEntity = (Entity*)p.asPtr;
#endif

#ifdef HAVOK_POSITION_BUG_TRACKING
	hkVector4 pos = rb->getPosition();
#ifdef HAVOK3
	if( !pos.isOk4() )
#else
	if( !pos.isOk() )
#endif
	{
#ifdef	TRANSGAMING
		GLog.Print( "Frame: %d - bad position in Havok callback!!! ", GEngine->pcfWorld->Frame );
#else
		GLog.Print( "Frame: %d - bad position in Havok callback!!! ", GEngine->World->Frame );
#endif
		if( ourEntity )
			GLog.Print( "Entity: '%s', filename: '%s', %s\n", ourEntity->Name.AsChar(), ourEntity->FileName.AsChar(),
				ourEntity->TestDeathZone?"test death zone":"no test death zone" );
		else
			GLog.Print( "No entity\n" );
	}
/*
	else
	{
		GLog.Print( "Frame: %d - Object out of broadband!!!\n", GEngine->World->Frame );
		GLog.Print( "Position: (%f,%f,%f)\n", pos(0), pos(1), pos(2) );
		if( ourEntity )
			GLog.Print( "Entity: '%s', filename: '%s', %s\n", ourEntity->Name.AsChar(), ourEntity->FileName.AsChar(),
				ourEntity->TestDeathZone?"test death zone":"no test death zone" );
		else
			GLog.Print( "No entity\n" );
	}
*/
#endif

	if( ourEntity )
	{
		hkVector4 pos = rb->getPosition();
		GScript.QueueMessage("IN_DEATH_ZONE","%e,%f,%f,%f,%s", ourEntity, pos(0), pos(1), pos(2), "havok2" );
		ourEntity->TestDeathZone = false;
	}
#ifdef	TRANSGAMING
    GEngine->pcfWorld->Physics->mHavokWorld->RigidBodiesPosExceeded.AddUnique( rb );
#else
    GEngine->World->Physics->mHavokWorld->RigidBodiesPosExceeded.AddUnique( rb );
#endif

//	hkBroadPhaseBorder::maxPositionExceededCallback( entity );
}

/*
void hkDispatchFailureAgent::processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result) {};
bool hkDispatchFailureAgent::arePenetrating( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env ){ return false; };
void hkDispatchFailureAgent::getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, hkClosestPointsDetails& pointDetails) {};
void hkDispatchFailureAgent::cleanup(const hkCollisionEnvironment& env) {};
*/
hkCollisionAgent* HK_CALL hkDispatchFailureAgent::createDispatchFailureAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env )
{
#ifndef HAVOK3 // ============================================
	GLog.Print( "Failed to dispatch type %s against %s\n", getNameFromShapeType(collA.getShape()->getPrimaryType()), getNameFromShapeType(collB.getShape()->getPrimaryType()) );
	GLog.Print( "with secondary types %s against %s\n", getNameFromShapeType(collA.getShape()->getSecondaryType()), getNameFromShapeType(collB.getShape()->getSecondaryType()) );
	return hkNullAgent::createNullAgent( collA, collB, env );
#else
	GLog.Print("HavokPhysics::hkDispatchFailureAgent::createDispatchFailureAgent() - FIXME!!!!!!!!!!!\n");
	return HK_NULL;
#endif // ifndef HAVOK3 =========================================
}

char* hkDispatchFailureAgent::getNameFromShapeType(hkShapeType type)
{
	switch(type)
	{
	case HK_SHAPE_CONVEX:	return "HK_SHAPE_CONVEX";
	case HK_SHAPE_BV_TREE:	return "HK_SHAPE_BV_TREE";
	case HK_SHAPE_BV:	return "HK_SHAPE_BV";
	case HK_SHAPE_HEIGHT_FIELD:	return "HK_SHAPE_HEIGHT_FIELD";
	case HK_SHAPE_SPHERE:	return "HK_SHAPE_SPHERE";
	case HK_SHAPE_PLANE:	return "HK_SHAPE_PLANE";
	case HK_SHAPE_TRIANGLE:	return "HK_SHAPE_TRIANGLE";
	case HK_SHAPE_BOX:	return "HK_SHAPE_BOX";
	case HK_SHAPE_CYLINDER:	return "HK_SHAPE_CYLINDER";
#ifndef HAVOK3
	case HK_SHAPE_LINE_SEGMENT:	return "HK_SHAPE_LINE_SEGMENT";
#endif
	case HK_SHAPE_CONVEX_VERTICES:	return "HK_SHAPE_CONVEX_VERTICES";
	case HK_SHAPE_MULTI_SPHERE:	return "HK_SHAPE_MULTI_SPHERE";
	case HK_SHAPE_MULTI_RAY:	return "HK_SHAPE_MULTI_RAY";
#ifndef HAVOK3
	case HK_SHAPE_CONNECTED_TRIANGLE:	return "HK_SHAPE_CONNECTED_TRIANGLE";
	case HK_SHAPE_AABBCAST:	return "HK_SHAPE_AABBCAST";
#endif
	case HK_SHAPE_LIST:	return "HK_SHAPE_LIST";
	case HK_SHAPE_COLLECTION:	return "HK_SHAPE_COLLECTION";
	case HK_SHAPE_USER0:	return "HK_SHAPE_USER0";
	case HK_SHAPE_USER1:	return "HK_SHAPE_USER1";
	case HK_SHAPE_USER2:	return "HK_SHAPE_USER2";
#ifndef HAVOK3
	case HK_SHAPE_USER3:	return "HK_SHAPE_USER3";
	case HK_SHAPE_USER4:	return "HK_SHAPE_USER4";
	case HK_SHAPE_USER5:	return "HK_SHAPE_USER5";
	case HK_SHAPE_USER6:	return "HK_SHAPE_USER6";
	case HK_SHAPE_USER7:	return "HK_SHAPE_USER7";
#endif
	case HK_SHAPE_TRANSFORM:	return "HK_SHAPE_TRANSFORM";
#ifndef HAVOK3
	case HK_SHAPE_PHANTOM:	return "HK_SHAPE_PHANTOM";
	case HK_SHAPE_PHANTOM_WITH_NO_AGENT:	return "HK_SHAPE_PHANTOM_WITH_NO_AGENT";
	case HK_SHAPE_CONVEX_WELDER:	return "HK_SHAPE_CONVEX_WELDER";

	default: HK_ASSERT2(0,"Unknown shape type in ShapeVsShapeTest::getNameFromShapeType.\n");	return HK_NULL;
#else
	default: return HK_NULL;
#endif
	}
};
/*
havokVehicleAction::havokVehicleAction( hkRigidBody* chassis, HavokVehicle* vehicle, int turnNo, int numTurns ) :
	hkUnaryAction( chassis ), mHavokVehicle( vehicle ), mTurnNo( turnNo ), mNumTurns( numTurns ) {};
havokVehicleAction::~havokVehicleAction() {};
void havokVehicleAction::applyAction( const hkStepInfo& stepInfo )
{
	if( ++mTurnNo >= mNumTurns )
	{
		mTurnNo = 0;
		mHavokVehicle->applyVehicle( stepInfo );
	}
}
*/

#ifdef HAVOK3
void havokBreakableListener::constraintBrokenCallback( hkBreakableConstraintEvent& event )
#else
void havokBreakableListener::constraintBrokenCallback( hkBreakableData& event )
#endif
{
#ifndef HAVOK3 // ============================================

#ifdef HAVOK3
	hkConstraintInstance* constr = event.m_constraint;
#else
	hkConstraint* constr = event.m_constraint;
#endif

#ifdef	TRANSGAMING
	if( !GEngine || !GEngine->pcfWorld || !GEngine->pcfWorld->Physics ) return;
#else
	if( !GEngine || !GEngine->World || !GEngine->World->Physics ) return;
#endif

	// Give model to script
	Model* model = (Model*)constr->getUserData();
	if( model )
	{
		int joint1 = -1, joint2 = -1;
		if( model->RagdollPtr )
			model->RagdollPtr->havokRagdoll->GetConstraintJoints( constr, joint1, joint2 );

		GScript.QueueMessage( "BREAKABLE_BROKEN","%e,%d,%d", model, joint1, joint2 );
	}

#else
	GLog.Print("HavokPhysics::havokBreakableListener::constraintBrokenCallback() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}
