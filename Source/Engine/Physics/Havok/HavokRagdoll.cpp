
#include "Engine.h"
#include "Physics/Havok/HavokPhysics.h"
#include "Physics/Havok/HavokRagdoll.h"
#include "Physics/Havok/HavokWorld.h"
#include "Physics/Havok/POHavokPart.h"
#include "Physics/Ragdoll.h"
#include "Physics/PhysicsEngine.h"
#include "Physics/PhysicsWorld.h"
#include "Physics/PhysicsObject.h"
#include "Engine/Entity.h"
#include "Engine/Model.h"
#include "Engine/World.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

Limb::Limb( hkRigidBody* rb, int joint, const Matrix &offset, bool isFixed )
	: Body(rb), Joint(joint), Offset(offset), Fixed(isFixed), AntiGravityAction( 0 ), CollisionGroup( HCGRagdoll ),
	CollisionListener( 0 ) 
{
	InvOffset = offset.Inverse();
	hkVector4 p = rb->getPosition();
	LastPosForInterpolation = Vector(p(0),p(1),p(2));
}

Limb::~Limb()
{
	if( CollisionListener )
	{
		Body->removeCollisionListener( CollisionListener );
		delete CollisionListener;
		CollisionListener = 0;
	}
}

HavokRagdoll::HavokRagdoll( Model* model, RagdollSkeleton* skeleton, HavokWorld* world ) :
		mModel(model), Skeleton(skeleton), LinearDamping(0.05f), AngularDamping(0.003f),
		mWorld(world), OuterRagdoll( 0 ), FastJointToLimbTable(0), Flags(IsMovedByExplosions),
		mInertiaTensorMultiplier(1.0f)
{
}

HavokRagdoll::~HavokRagdoll()
{
	EnableGravity( true );
	if( Flags & HardDeactivator )
		for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
			mWorld->mDeactivator->RemoveRigidBody( Limbs[iLimb].Body );

	mWorld->RemoveRagdoll( this );
	mModel->RagdollPtr = 0;

	if( FastJointToLimbTable )
	{
		delete [] FastJointToLimbTable;
		FastJointToLimbTable = 0;
	}
}

void HavokRagdoll::PrecalculateFastJointToLimbTable( int jointNumber )
{
	if( FastJointToLimbTable ) delete [] FastJointToLimbTable;
	if( !jointNumber ) { FastJointToLimbTable = 0; return; };

    m_setTag("Physics");
	char* buffer = new char[jointNumber];
	::memset( buffer, -1, jointNumber );

	for( int i = 0; i < Limbs.Size(); ++i )
	{
		Assert( Limbs[i].Joint >= 0 );
		Assert( buffer[ Limbs[i].Joint ] == -1 );
		buffer[ Limbs[i].Joint ] = i;
	}
	FastJointToLimbTable = buffer;
}

void HavokRagdoll::SetCollisionGroup( int newGroup )
{
	static int ragdollBodyIndex = 0;

	if( !Limbs.Size() ) return;

	bool noncollidingCounter = false;
	if( ( newGroup >= HCGActiveRagdoll ) && ( newGroup <= HCGActiveRagdoll0 ) )
		noncollidingCounter = true;

	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		hkRigidBody *rb = Limbs[iLimb].Body;
		if( noncollidingCounter )
		{
#ifdef HAVOK3
			rb->getCollidable()->setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( HCGActiveRagdoll+ragdollBodyIndex ) );
#else
			rb->getCollidable().setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( HCGActiveRagdoll+ragdollBodyIndex ) );
#endif
			Limbs[iLimb].CollisionGroup = HCGActiveRagdoll+ragdollBodyIndex;
		}
		else
		{
#ifdef HAVOK3
			rb->getCollidable()->setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( newGroup ) );
#else
			rb->getCollidable().setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( newGroup ) );
#endif
			Limbs[iLimb].CollisionGroup = newGroup;
		}

		if( rb->getSimulationIsland() )
#ifdef HAVOK3
			// BLAIR - ****** TAKE ANOTHER LOOK AT THIS!!! The values MAY BE WRONG and/or EXPENSIVE!! *************
			mWorld->World->updateCollisionFilterOnEntity( rb, HK_UPDATE_FILTER_ON_ENTITY_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY, HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS );
#else
			mWorld->World->updateCollisionFilter( rb );
#endif
	}

	// Turn counter
	if( noncollidingCounter ) { ragdollBodyIndex++; if( ragdollBodyIndex > 9 ) ragdollBodyIndex = 0; };
}

int HavokRagdoll::GetCollisionGroup( void )
{
	if( !Limbs.Size() ) return HCGRagdoll;
#ifdef HAVOK3
	return Limbs[0].Body->getCollidable()->getCollisionFilterInfo();
#else
	return Limbs[0].Body->getCollidable().getCollisionFilterInfo();
#endif
}

void HavokRagdoll::Activate( const DynamicArray<Matrix>& frameMatrix, int collisionGroup )
{
	Flags |= Active;

	if( !Limbs.Size() ) return;

	hkVector4 zero(0,0,0);
	hkWorld* world = mWorld->World;

	float realDampingAngular = AngularDamping;
	if( mWorld->IsUnderwater() )
		realDampingAngular = 1.0f - ((1.0f-realDampingAngular)/GEngine->Physics->TWUnderwaterDampingRaiseAngular);

	hkPropertyValue p = Limbs[0].Body->getProperty( HPEntityPointer );
#ifdef HAVOK3
	Entity* entity = (Entity*)p.getPtr();
#else
	Entity* entity = (Entity*)p.asPtr;
#endif
	if( entity && entity->PO )
		entity->PO->EffectRotateActor();

	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		Limb &limb = Limbs[iLimb];
		hkRigidBody *rb = limb.Body;

		if( !rb->getSimulationIsland() )
			world->addEntity( limb.Body );

		if( !limb.Fixed )
#ifdef HAVOK3
// BLAIR - !!!!!!!!! This might be the wrong way to do this...
			rb->setMotionType( hkMotion::MOTION_FIXED );
#else
			rb->setKeyframed( false );
#endif

		hkVector4 bodyVelocity(0,0,0);
		if( entity && entity->PO )
		{
			hkVector4 limbPos = rb->getCenterOfMassInWorld();
			entity->PO->mHavokPart->Body->getPointVelocity( limbPos, bodyVelocity );
		}

		Matrix transform = limb.InvOffset * frameMatrix[limb.Joint];

		hkTransform T;
		T.set4x4ColumnMajor( &transform.mArray[0] );
		rb->setTransform(T);

#ifdef HAVOK_POSITION_BUG_TRACKING
		if( !T.isOk() )
			GLog.Print( "Bad transform matrix given to ragdoll limb in activate\n" );
		else
		{
			const hkVector4& transl = T.getTranslation();
			if( transl.lengthSquared3() > 10000.0f*10000.0f )
			{
				GLog.Print( "Ragdoll limb pos > 10000 in activate: (%f,%f,%f), entity name: %s, file: %s\n",
					transl(0), transl(1), transl(2), entity->Name.AsChar(), entity->FileName.AsChar() );
			}
		}
#endif

		rb->setAngularVelocity( zero );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !bodyVelocity.isOk4() )
#else
		if( !bodyVelocity.isOk() )
#endif
			GLog.Print( "Bad velocity in Ragdoll::Activate\n" );
		else if( bodyVelocity.length3() > 1000.0f )
			GLog.Print( "Velocity > 1000 in Ragdoll::Activate\n" );
#endif
		if( limb.Fixed )
			rb->setLinearVelocity( zero );
		else
			rb->setLinearVelocity( bodyVelocity );
		rb->setLinearDamping( LinearDamping );
		rb->setAngularDamping( realDampingAngular );

		if( limb.Fixed )
		{
			// Find out what masses the bodies that are connected to you have, and give this body the average of them
			DynamicArray<float> masses;
			for( int iConstraint = 0; iConstraint < Constraints.Size(); ++iConstraint )
			{
#ifdef HAVOK3
				hkConstraintInstance* c = Constraints[iConstraint].Constraint;
#else
				hkConstraint* c = Constraints[iConstraint].Constraint;
#endif
				hkRigidBody* bodyA = (hkRigidBody*)c->getEntityA();
				hkRigidBody* bodyB = (hkRigidBody*)c->getEntityB();

				if( ( rb == bodyA ) && bodyB )
				{
					float m = bodyB->getMass();
					if( m ) masses.Add( m );
				}
				else if( ( rb == bodyB ) && bodyA )
				{
					float m = bodyA->getMass();
					if( m ) masses.Add( m );
				}
				else
					continue;
				break;
			}

			if( masses.Size() )
			{
				float mass = 0;
				for( int i = 0; i < masses.Size(); ++i ) mass += masses[i];
				mass /= masses.Size();
#ifdef HAVOK_POSITION_BUG_TRACKING
				Assert( mass >= 0.0f );
#endif
				rb->setMass( mass );
//				GLog.Print( "Mass set to fixed: %f\n", mass );
			}
		}
	}

	// Add constraints to world
	for( int iConstraint = 0; iConstraint < Constraints.Size(); ++iConstraint )
	{
		FixConstraintPivots( Constraints[iConstraint] );
		world->addConstraint( Constraints[iConstraint].Constraint );
	}

	// Add actions to world
	for( int iAction = 0; iAction < Actions.Size(); ++iAction )
	{
		FixActionPivots( Actions[iAction] );
		world->addAction( Actions[iAction].Action );
	}

	SetCollisionGroup( collisionGroup );

	Tick();
}

void HavokRagdoll::Animate( const DynamicArray<Matrix>& frameMatrix )
{
	if( Flags & (Active|EternallyFrozen) ) return;

	if( Flags & Frozen ) Thaw();

	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		Limb &limb = Limbs[iLimb];
		hkRigidBody *rb = limb.Body;

		Matrix transform = limb.InvOffset * frameMatrix[limb.Joint];

		Quaternion quat( transform );
		quat.Normalize();
		hkQuaternion q( quat.X, quat.Y, quat.Z, -quat.W);

		Vector pos = transform.GetTranslation();
		hkVector4 p( pos.X, pos.Y, pos.Z );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !p.isOk4() )
#else
		if( !p.isOk() )
#endif
		{
			hkPropertyValue p = Limbs[0].Body->getProperty( HPEntityPointer );
#ifdef HAVOK3
			Entity* entity = (Entity*)p.getPtr();
#else
			Entity* entity = (Entity*)p.asPtr;
#endif
			GLog.Print( "Bad ragdoll limb pos in animate, entity name: %s, file: %s\n",
				entity->Name.AsChar(), entity->FileName.AsChar() );
		}
		else if( p.lengthSquared3() > 10000.0f*10000.0f )
		{
			hkPropertyValue p2 = Limbs[0].Body->getProperty( HPEntityPointer );
#ifdef HAVOK3
			Entity* entity = (Entity*)p2.getPtr();
#else
			Entity* entity = (Entity*)p2.asPtr;
#endif
			GLog.Print( "Ragdoll limb pos > 10000 in animate: (%f,%f,%f), entity name: %s, file: %s\n", p(0), p(1), p(2),
				entity->Name.AsChar(), entity->FileName.AsChar() );

			GLog.Print( "frameMatrix lookz like that: (%f,%f,%f,%f) ",frameMatrix[limb.Joint].A00,frameMatrix[limb.Joint].A01,frameMatrix[limb.Joint].A02,frameMatrix[limb.Joint].A03);
		}

		if( !q.isOk() )
		{
			hkPropertyValue p = Limbs[0].Body->getProperty( HPEntityPointer );
#ifdef HAVOK3
			Entity* entity = (Entity*)p.getPtr();
#else
			Entity* entity = (Entity*)p.asPtr;
#endif
			GLog.Print( "Bad quaternion given to ragdoll limb in animate, entity name: %s, file: %s\n",
				entity->Name.AsChar(), entity->FileName.AsChar() );
		}
#endif

		rb->setPositionAndRotation( p, q );

		if( !rb->isActive() && rb->getSimulationIsland() )
#ifdef HAVOK3
			rb->activate();
#else
			rb->forceActivate();
#endif
	}
}

void HavokRagdoll::Deactivate( void )
{
	Flags &= ~Active;

	hkWorld* world = mWorld->World;

	for( int iAction = 0; iAction < Actions.Size(); ++iAction )
		world->removeAction( Actions[iAction].Action );

	for( int iConstraint = 0; iConstraint < Constraints.Size(); ++iConstraint )
		world->removeConstraint( Constraints[iConstraint].Constraint );

	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		hkRigidBody *rb = Limbs[iLimb].Body;
#ifdef HAVOK3
		if( Limbs[iLimb].Fixed ) rb->setMotionType( hkMotion::MOTION_FIXED );
#else
		if( Limbs[iLimb].Fixed ) rb->setKeyframed( false );
#endif
		// Remove (primitives) to detector
#ifdef HAVOK3
		rb->getCollidable()->setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( Limbs[iLimb].CollisionGroup ) );
#else
		rb->getCollidable().setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( Limbs[iLimb].CollisionGroup ) );
#endif
		if( rb->getSimulationIsland() )
#ifdef HAVOK3
			// BLAIR - Not 100% sure this is what we want, but it makes the most sense since the rb is not
			// a phantom or the world.
			world->updateCollisionFilterOnEntity( rb, HK_UPDATE_FILTER_ON_ENTITY_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY, HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS );
#else
			world->updateCollisionFilter( rb );
#endif
		// Warning: this may cause deactivation of things in the same simulation island as ragdoll
#ifdef HAVOK3
		rb->setMotionType( hkMotion::MOTION_FIXED );
#else
		rb->setKeyframed(true);
#endif
	}
}

bool HavokRagdoll::IsActive( void )
{
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
		if( Limbs[iLimb].Body->isActive() )
			return true;
	return false;
}

void HavokRagdoll::ForceActivate( void )
{
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
#ifdef HAVOK3
		Limbs[iLimb].Body->activate();
#else
		Limbs[iLimb].Body->forceActivate();
#endif
}

void HavokRagdoll::SetHardDeactivator( void )
{
	if( !(Flags&HardDeactivator) )
		for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
			mWorld->mDeactivator->AddRigidBody( Limbs[iLimb].Body );
	Flags |= HardDeactivator;
}

float HavokRagdoll::GetMass( void )
{
	hkReal complex_mass = 0.0f;
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		hkRigidBody *rb = Limbs[iLimb].Body;
		if( rb )
		{
			complex_mass += rb->getMass();
#ifdef HAVOK_POSITION_BUG_TRACKING
			Assert( rb->getMass() >= 0.0f );
#endif
		}
	}
	return complex_mass;
}

void HavokRagdoll::SetMass( float mass )
{
	hkReal complex_mass = GetMass();
	if( ( mass == 0.0f ) || (complex_mass == 0.0f ) || ( complex_mass == mass ) ) return;

	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		hkRigidBody *rb = Limbs[iLimb].Body;
		if( rb )
		{
			float m = rb->getMass()/complex_mass*mass;
#ifdef HAVOK_POSITION_BUG_TRACKING
			Assert( m >= 0.0f );
#endif
			rb->setMass( m );
		}
	}
}

void HavokRagdoll::ScaleInertiaTensor( float scale )
{
	// BLAIR ****!!!!!!!!!!******** scaleMassProperties doesn't exist in Havok3 *****
	Assert( scale > 0.0f );
	if( scale == 0.0f ) return;
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		hkRigidBody *rb = Limbs[iLimb].Body;
		if( rb )
		{
			hkMassProperties massProperties;
			rb->getInertiaLocal( massProperties.m_inertiaTensor );
			massProperties.m_centerOfMass = rb->getCenterOfMassLocal();
			massProperties.m_mass = rb->getMass();
#ifdef HAVOK_POSITION_BUG_TRACKING
			Assert( massProperties.m_mass >= 0.0f );
#endif

#ifdef HAVOK3
			//HavokWorld::scaleMassProperties( scale, massProperties );
#else
			hkInertiaTensorComputer::scaleMassProperties( scale, massProperties );
#endif
#ifdef HAVOK_POSITION_BUG_TRACKING
			Assert( massProperties.m_mass >= 0.0f );
#endif
			rb->setInertiaLocal( massProperties.m_inertiaTensor );
		}
	}
	mInertiaTensorMultiplier *= scale;
}

bool HavokRagdoll::ConstraintInWorld( int index )
{
#ifndef HAVOK3

#ifdef HAVOK3
	hkConstraintInstance* co = Constraints[index].Constraint;
#else
	hkConstraint* co = Constraints[index].Constraint;
#endif

	hkRigidBody* rb = (hkRigidBody*)co->getEntityA();
	hkSimulationIsland* island = rb->getSimulationIsland();
	if( !island ) return false;
	int size = island->getConstraints().getSize();
	for( int iConst = 0; iConst < size; ++iConst )
		if( island->getConstraints()[iConst] == co )
			return true;

	rb = (hkRigidBody*)co->getEntityB();
	if( rb )
	{
		island = rb->getSimulationIsland();
		if( !island ) return false;
		size = island->getConstraints().getSize();
		for( int iConst = 0; iConst < size; ++iConst )
			if( island->getConstraints()[iConst] == co )
				return true;
	}

	return false;
#else
	TRACE("######## FIXME:HavokRagdoll::ConstraintInWorld -> This function has been deactivated by BLAIR for now ########");
	return true;
#endif
}

bool HavokRagdoll::ActionInWorld( int index )
{
	hkAction* ac = Actions[index].Action;
	hkArray<hkEntity*> entities;
	ac->getEntities( entities );
	hkSimulationIsland* oldIsland = NULL;
	for( int i = 0; i < entities.getSize(); ++i )
	{
		hkRigidBody* rb = (hkRigidBody*)entities[i];
		hkSimulationIsland* island = rb->getSimulationIsland();
		if( !island ) return false;
		if( oldIsland == island ) continue;
		oldIsland = island;
		int size = island->getActions().getSize();
		for( int iAct = 0; iAct < size; ++iAct )
		if( island->getActions()[iAct] == ac )
			return true;
	}

	return false;
}

void HavokRagdoll::Joint_BreakConstraints( int joint )
{
	// BLAIR - ###### DISABLE THIS FOR NOW UNTIL RAGDOLLS FIXED!!! ###########
#ifndef HAVOK3
	Assert( joint >= 0 );
	int iLimb = FastJointToLimbTable[joint];
	if( iLimb == -1 ) return;
	hkRigidBody *rb = Limbs[iLimb].Body;
	hkSimulationIsland* island = rb->getSimulationIsland();
	Assert( island );

#ifdef HAVOK3
	// BLAIR - The new ragdolls, which are hkPhysicsSystems, will have the new getConstraints...
	int size = ragdoll->getConstraints().getSize();
#else
	int size = island->getConstraints().getSize();
#endif
	for( int iConst = 0; iConst < size; ++iConst )
	{
		hkConstraint* c = island->getConstraints()[iConst];
		if( c->getEntityA() == rb || c->getEntityB() == rb )
		{
			mWorld->World->removeConstraint( c );
/*
			c->removeReference();

			for( int iConstraint = 0; iConstraint < Constraints.Size(); ++iConstraint )
			{
				if( Constraints[iConstraint].Constraint == c )
				{
					Constraints.Remove( iConstraint );
					break;
				}
			}
*/
		}
	}

	size = island->getActions().getSize();
	for( int iAct = 0; iAct < size; ++iAct )
	{
		hkAction* a = island->getActions()[iAct];
		hkArray<hkEntity*> entities;
		a->getEntities( entities );
		if( entities[0] == rb || ( ( entities.getSize() > 1 ) && ( entities[1] == rb ) ) )
		{
			mWorld->World->removeAction( a );
/*
			a->removeReference();

			for( int iAction = 0; iAction < Actions.Size(); ++iAction )
			{
				if( Actions[iAction].Action == a )
				{
					Actions.Remove( iAction );
					break;
				}
			}
*/
		}
	}
#else
	TRACE("######## FIXME:HavokRagdoll::Joint_BreakConstraints -> need to fix ragdoll->getConstraints #########\n");
#endif
}

void HavokRagdoll::SetVelocities( const Vector& v, const Vector& av)
{
	hkVector4 linVel(v.X,v.Y,v.Z);
	hkVector4 angVel(av.X,av.Y,av.Z);

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
	if( !linVel.isOk4() || ( fabs(linVel(0)) > 1000.0f ) || ( fabs(linVel(1)) > 1000.0f ) || ( fabs(linVel(2)) > 1000.0f ) )
#else
	if( !linVel.isOk() || ( fabs(linVel(0)) > 1000.0f ) || ( fabs(linVel(1)) > 1000.0f ) || ( fabs(linVel(2)) > 1000.0f ) )
#endif
		GLog.Print( "Velocity > 1000 in Ragdoll::SetVelocities\n" );
#ifdef HAVOK3
	if( !angVel.isOk4() || ( fabs(angVel(0)) > 100.0f ) || ( fabs(angVel(1)) > 100.0f ) || ( fabs(angVel(2)) > 100.0f ) )
#else
	if( !angVel.isOk() || ( fabs(angVel(0)) > 100.0f ) || ( fabs(angVel(1)) > 100.0f ) || ( fabs(angVel(2)) > 100.0f ) )
#endif
		GLog.Print( "Angular Velocity > 100 in Ragdoll::SetVelocities\n" );
#endif

	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		hkRigidBody *rb = Limbs[iLimb].Body;
		rb->setLinearVelocity( linVel );
		rb->setAngularVelocity( angVel );
	}
}

void HavokRagdoll::Move( const Vector& v )
{
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		hkRigidBody *rb = Limbs[iLimb].Body;
		hkVector4 p;
		p = rb->getPosition();
		p(0) = p(0) + v.X;
		p(1) = p(1) + v.Y;
		p(2) = p(2) + v.Z;
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !p.isOk4() || ( fabs(p(0)) > 10000.0f ) || ( fabs(p(1)) > 10000.0f ) || ( fabs(p(2)) > 10000.0f ) )
#else
		if( !p.isOk() || ( fabs(p(0)) > 10000.0f ) || ( fabs(p(1)) > 10000.0f ) || ( fabs(p(2)) > 10000.0f ) )
#endif
			GLog.Print( "Position > 10000 in Ragdoll::Move\n" );
#endif
		rb->setPosition(p);
	}
}

void HavokRagdoll::Joint_SetVelocities( int joint, const Vector& v, const Vector& av )
{
	Assert( joint >= 0 );
	unsigned char iLimb = FastJointToLimbTable[joint];
	if( iLimb == (unsigned char)(-1) )
		return;
	if (iLimb >= Limbs.Size())
		return;
	hkRigidBody *rb = Limbs[iLimb].Body;
	hkVector4 linVel(v.X,v.Y,v.Z);
	hkVector4 angVel(av.X,av.Y,av.Z);

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
	if( !linVel.isOk4() || ( fabs(linVel(0)) > 1000.0f ) || ( fabs(linVel(1)) > 1000.0f ) || ( fabs(linVel(2)) > 1000.0f ) )
#else
	if( !linVel.isOk() || ( fabs(linVel(0)) > 1000.0f ) || ( fabs(linVel(1)) > 1000.0f ) || ( fabs(linVel(2)) > 1000.0f ) )
#endif
		GLog.Print( "Velocity > 1000 in Ragdoll::Joint_SetVelocities\n" );
#ifdef HAVOK3
	if( !angVel.isOk4() || ( fabs(angVel(0)) > 100.0f ) || ( fabs(angVel(1)) > 100.0f ) || ( fabs(angVel(2)) > 100.0f ) )
#else
	if( !angVel.isOk() || ( fabs(angVel(0)) > 100.0f ) || ( fabs(angVel(1)) > 100.0f ) || ( fabs(angVel(2)) > 100.0f ) )
#endif
		GLog.Print( "Angular Velocity > 100 in Ragdoll::Joint_SetVelocities\n" );
#endif

	rb->setLinearVelocity( linVel );
	rb->setAngularVelocity( angVel );
}

Vector HavokRagdoll::Joint_GetLinearVelocity( int joint )
{
	Vector lv(0,0,0);
	if ( joint >= 0 ) {
		int iLimb = FastJointToLimbTable[joint];
		if( iLimb == -1 ) return lv;
		hkVector4 linVel = Limbs[iLimb].Body->getLinearVelocity();
		lv.X = linVel(0); lv.Y = linVel(1); lv.Z = linVel(2);
	}
	return lv;
}

Vector HavokRagdoll::Joint_GetAngularVelocity( int joint )
{
	Vector av(0,0,0);
	Assert( joint >= 0 );
	int iLimb = FastJointToLimbTable[joint];
	if( iLimb == -1 ) return av;
	hkVector4 angVel = Limbs[iLimb].Body->getAngularVelocity();
	av.X = angVel(0); av.Y = angVel(1); av.Z = angVel(2);
	return av;
}

void HavokRagdoll::Joint_SetVelocitiesForLinked( int joint, const Vector& v, const Vector& av )
{
	// Find out what limbs to move
	DynamicArray<hkRigidBody*> limbs;
		
	Assert( joint >= 0 );
	int iLimb = FastJointToLimbTable[joint];
	if( iLimb == -1 ) return;
	limbs.Add( Limbs[iLimb].Body );

	int limbsNum = 1;
	do
	{
		limbsNum = limbs.Size();
		for(int i = 0; i < Constraints.Size(); ++i )
		{
#ifdef HAVOK3
			hkConstraintInstance* c = Constraints[i].Constraint;
#else
			hkConstraint* c = Constraints[i].Constraint;
#endif
			hkRigidBody* bodyA = (hkRigidBody*)c->getEntityA();
			hkRigidBody* bodyB = (hkRigidBody*)c->getEntityB();
			for(int j = 0; j < limbs.Size(); ++j )
			{
				if( limbs[j] == bodyA )
					limbs.AddUnique( bodyB );
				else if( limbs[j] == bodyB )
					limbs.AddUnique( bodyA );
				else
					continue;
				break;
			}
		}

		for(int i = 0; i < Actions.Size(); ++i )
		{
			hkAction* a = Actions[i].Action;
			hkArray<hkEntity*> entities;
			a->getEntities( entities );
			hkRigidBody* bodyA = (hkRigidBody*)entities[0];
			hkRigidBody* bodyB = (entities.getSize() > 1)?(hkRigidBody*)entities[1]:0;
			for(int j = 0; j < limbs.Size(); ++j )
			{
				if( limbs[j] == bodyA )
					limbs.AddUnique( bodyB );
				else if( limbs[j] == bodyB )
					limbs.AddUnique( bodyA );
				else
					continue;
				break;
			}
		}
	}
	while( limbsNum < limbs.Size() );

	hkVector4 linVel(v.X,v.Y,v.Z);
	hkVector4 angVel(av.X,av.Y,av.Z);

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
	if( !linVel.isOk4() || ( fabs(linVel(0)) > 1000.0f ) || ( fabs(linVel(1)) > 1000.0f ) || ( fabs(linVel(2)) > 1000.0f ) )
#else
	if( !linVel.isOk() || ( fabs(linVel(0)) > 1000.0f ) || ( fabs(linVel(1)) > 1000.0f ) || ( fabs(linVel(2)) > 1000.0f ) )
#endif
		GLog.Print( "Velocity > 1000 in Ragdoll::SetVelocities\n" );
#ifdef HAVOK3
	if( !angVel.isOk4() || ( fabs(angVel(0)) > 100.0f ) || ( fabs(angVel(1)) > 100.0f ) || ( fabs(angVel(2)) > 100.0f ) )
#else
	if( !angVel.isOk() || ( fabs(angVel(0)) > 100.0f ) || ( fabs(angVel(1)) > 100.0f ) || ( fabs(angVel(2)) > 100.0f ) )
#endif
		GLog.Print( "Angular Velocity > 100 in Ragdoll::SetVelocities\n" );
#endif

	for( int i = 0; i < limbs.Size(); ++i )
	{
		hkRigidBody *rb = limbs[i];
		rb->setLinearVelocity( linVel );
		rb->setAngularVelocity( angVel );
	}
}

void HavokRagdoll::Joint_SetPositionLowLevel( int joint, const Vector& v ) {
	Assert( joint >= 0 );
	int iLimb = FastJointToLimbTable[joint];
	if( iLimb == -1 ) return;

	hkRigidBody *rb = Limbs[iLimb].Body;

	hkVector4 p( v.X, v.Y, v.Z );
	rb->setPosition(p);
}

void HavokRagdoll::Joint_SetPosition( int joint, const Vector& v )
{
	Assert( joint >= 0 );
	int iLimb = FastJointToLimbTable[joint];
	if( iLimb == -1 ) return;

	Matrix transform = Limbs[iLimb].InvOffset * TranslationMatrix(v);

	hkTransform T;
	T.set4x4ColumnMajor( &transform.mArray[0] );

#ifdef HAVOK_POSITION_BUG_TRACKING
	const hkVector4& transl = T.getTranslation();
	if( transl.lengthSquared3() > 10000.0f*10000.0f )
	{
		hkPropertyValue p = Limbs[iLimb].Body->getProperty( HPEntityPointer );
#ifdef HAVOK3
		Entity* entity = (Entity*)p.getPtr();
#else
		Entity* entity = (Entity*)p.asPtr;
#endif
		GLog.Print( "Ragdoll limb pos > 10000 in joint_set_pos: (%f,%f,%f), entity name: %s, file: %s\n",
			transl(0), transl(1), transl(2), entity->Name.AsChar(), entity->FileName.AsChar() );
	}

	if( !T.isOk() )
	{
		GLog.Print( "Bad transform matrix given to ragdoll limb in joint_set_pos\n" );
	}
#endif

	Limbs[iLimb].Body->setTransform( T );
}

void HavokRagdoll::Joint_SetMatrix( int joint, const Matrix& m )
{
	Assert( joint >= 0 );
	int iLimb = FastJointToLimbTable[joint];
	if( iLimb == -1 ) return;

	Matrix transform = Limbs[iLimb].InvOffset * m;

	hkTransform T;
	T.set4x4ColumnMajor( &transform.mArray[0] );

#ifdef HAVOK_POSITION_BUG_TRACKING
	const hkVector4& transl = T.getTranslation();
	if( transl.lengthSquared3() > 10000.0f*10000.0f )
		GLog.Print( "Ragdoll limb pos > 10000 in joint_set_matrix: (%f,%f,%f)\n", transl(0), transl(1), transl(2) );

	if( !T.isOk() )
		GLog.Print( "Bad transform matrix given to ragdoll limb in joint_set_matrix\n" );
#endif

	Limbs[iLimb].Body->setTransform( T );
}

void HavokRagdoll::Joint_GetPosition( int joint,  Vector& v ) {
	Assert( joint >= 0 );
	int iLimb = FastJointToLimbTable[joint];
	if( iLimb == -1 ) return;
	hkVector4 v2 = Limbs[iLimb].Body->getPosition();
	v.X = v2(0);
	v.Y = v2(1);
	v.Z = v2(2);
}

void HavokRagdoll::Joint_GetRotation( int joint,  Quaternion& q ) {
	Assert( joint >= 0 );
	int iLimb = FastJointToLimbTable[joint];
	if( iLimb == -1 ) return;
	hkQuaternion qq2 = Limbs[iLimb].Body->getRotation();
	q.W = qq2(3);	// -?
	q.X = qq2(0);
	q.Y = qq2(1);
	q.Z = qq2(2);
}


void HavokRagdoll::Joint_SetRotation( int joint, const Quaternion& q )
{
	Assert( joint >= 0 );
	int iLimb = FastJointToLimbTable[joint];
	if( iLimb == -1 ) return;

	hkQuaternion qq(q.X, q.Y, q.Z, q.W);

#ifdef HAVOK_POSITION_BUG_TRACKING

# ifdef TRANSGAMING
	if( !qq.isOk() )
		GLog.Print( "Bad quaternion given to ragdoll limb in joint_setrotation. Model: '%s', filename '%s', joint %d, ('%s')\n",
			mModel?mModel->Name.AsChar():"(null)", mModel?mModel->FileName.AsChar():"(null)", joint, mModel?mModel->GetJointName(joint):"(null)" );
# else
	if( !qq.isOk() )
		GLog.Print( "Bad quaternion given to ragdoll limb in joint_setrotation. Model: '%s', filename '%s', joint %d, ('%s')\n",
			mModel?mModel->Name.AsChar():"(null)", mModel?mModel->FileName.AsChar():"(null)", joint, mModel?mModel->GetJointName(joint).AsChar():"(null)" );
# endif
#endif

	Limbs[iLimb].Body->setRotation(qq);
}

void HavokRagdoll::GetLimbsPositionsAndRotations( DynamicArray<Vector>& positions, DynamicArray<Quaternion>& rotations )
{
	positions.ClearArray();
	rotations.ClearArray();
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		hkVector4 v;
		v = Limbs[iLimb].Body->getPosition();
		positions.Add( Vector( v(0), v(1), v(2) ) );
		hkQuaternion q;
		q = Limbs[iLimb].Body->getRotation();
		rotations.Add( Quaternion( -q(3), q(0), q(1), q(2) ) );
	}
}

void HavokRagdoll::SetLimbsPositionsAndRotations( DynamicArray<Vector>& positions, DynamicArray<Quaternion>& rotations )
{
	Assert( ( positions.Size() == rotations.Size() ) && ( rotations.Size() == Limbs.Size() ) );

	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		Vector& vec = positions[iLimb];
		hkVector4 v( vec.X, vec.Y, vec.Z );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !v.isOk4() || ( fabs(v(0)) > 10000.0f ) || ( fabs(v(1)) > 10000.0f ) || ( fabs(v(2)) > 10000.0f ) )
#else
		if( !v.isOk() || ( fabs(v(0)) > 10000.0f ) || ( fabs(v(1)) > 10000.0f ) || ( fabs(v(2)) > 10000.0f ) )
#endif
			GLog.Print( "Position > 10000 in SetLimbsPositionsAndRotations\n" );
#endif
		Limbs[iLimb].Body->setPosition(v);

		Quaternion& quat = rotations[iLimb];
		hkQuaternion q( quat.X, quat.Y, quat.Z, -quat.W);

#ifdef HAVOK_POSITION_BUG_TRACKING
	if( !q.isOk() )
		GLog.Print( "Bad quaternion given to ragdoll limb in setlimbspositionsandrotations\n" );
#endif
		Limbs[iLimb].Body->setRotation(q);
	}
}

void HavokRagdoll::ApplyPointImpulse( Vector point, Vector impulse )
{
	float limbs = Limbs.Size();
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		hkRigidBody *rb = Limbs[iLimb].Body;
#ifdef HAVOK3
		if( rb->getMotionType() == hkMotion::MOTION_KEYFRAMED )
#else
		if( rb->isKeyframed() )
#endif
			continue;
		hkVector4 pt = rb->getCenterOfMassInWorld();
		hkVector4 imp(impulse.X/limbs,impulse.Y/limbs,impulse.Z/limbs);
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !imp.isOk4() )
#else
		if( !imp.isOk() )
#endif
			GLog.Print( "Corrupted force in ApplyPointImpulse\n" );
		else if( imp.length3() > 50000.0f )
			GLog.Print( "Force > 50000 in ApplyPointImpulse\n" );
#ifdef HAVOK3
		if( !pt.isOk4() )
#else
		if( !pt.isOk() )
#endif
			GLog.Print( "Corrupted point in ApplyPointImpulse\n" );
		else if( pt.length3() > 10000.0f )
			GLog.Print( "Point pos > 10000 in ApplyPointImpulse\n" );
		float myMass = rb->getMass();
		if( myMass < 0.000001f )
		{
			Assert( myMass >= 0.0f );
			Entity* enti = (Entity*)mModel;
			GLog.Print( "Zero mass in ApplyPointImpulse ( entity '%s, filename '%s' )\n", enti?enti->Name.AsChar():"(null)", enti?enti->FileName.AsChar():"(null)" );
		}
#endif
		rb->applyPointImpulse( imp, pt);
	}
}

void HavokRagdoll::SetPinned( bool enable )
{
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
#ifdef HAVOK3
		// #### BLAIR - I'm going to assume setKeyframed(false) to be hkMOTION::MOTION_FIXED.
		// HOWEVER, if this seems wonky, it probably means the intention of unkeyframing
		// the body was to make it hkMOTION::MOTION_DYNAMIC ####
		if ( enable )
			Limbs[iLimb].Body->setMotionType( hkMotion::MOTION_KEYFRAMED );
		else
			Limbs[iLimb].Body->setMotionType( hkMotion::MOTION_FIXED );
#else
		Limbs[iLimb].Body->setKeyframed( enable );
#endif
}

bool HavokRagdoll::IsPinned( void )
{
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
#ifdef HAVOK3
		if( Limbs[iLimb].Body->getMotionType() == hkMotion::MOTION_KEYFRAMED )
#else
		if( Limbs[iLimb].Body->isKeyframed() )
#endif
			return true;
	return false;
}

void HavokRagdoll::Joint_Enable( int joint, bool enable )
{
	Assert( joint >= 0 );
	int iLimb = FastJointToLimbTable[joint];
	if( iLimb == -1 ) return;
	if( enable )
	{
		if( !Limbs[iLimb].Body->getSimulationIsland() )
			mWorld->World->addEntity( Limbs[iLimb].Body );
	}
	else
	{
		if( Limbs[iLimb].Body->getSimulationIsland() )
			mWorld->World->removeEntity( Limbs[iLimb].Body );
	}
}

void HavokRagdoll::Joint_SetPinned( int joint, bool enable )
{
	Assert( joint >= 0 );
	int iLimb = FastJointToLimbTable[joint];
	if( iLimb != -1 )
#ifdef HAVOK3
		// #### BLAIR - I'm going to assume setKeyframed(false) to be hkMOTION::MOTION_FIXED.
		// HOWEVER, if this seems wonky, it probably means the intention of unkeyframing
		// the body was to make it hkMOTION::MOTION_DYNAMIC ####
		if ( enable )
			Limbs[iLimb].Body->setMotionType( hkMotion::MOTION_KEYFRAMED );
		else
			Limbs[iLimb].Body->setMotionType( hkMotion::MOTION_FIXED );
#else
		Limbs[iLimb].Body->setKeyframed( enable );
#endif
}

bool HavokRagdoll::Joint_IsPinned( int joint )
{
	Assert( joint >= 0 );
	int iLimb = FastJointToLimbTable[joint];
	if( iLimb != -1 ){
#ifdef HAVOK3
		if( Limbs[iLimb].Body->getMotionType() == hkMotion::MOTION_KEYFRAMED )
			return true;
		else
			return false;
#else
		return Limbs[iLimb].Body->isKeyframed();
#endif
    }
	return false;
}

void HavokRagdoll::EnableLineTraceCollision( bool enable )
{
	if( enable )
		for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
			Limbs[iLimb].Body->removeProperty( HPRayCastDontCollide );
	else
		for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
		{
			hkPropertyValue pv;
#ifdef HAVOK3
			pv.setInt(1);
#else
			pv.asInt = 1;
#endif
			Limbs[iLimb].Body->addProperty( HPRayCastDontCollide, pv );
		}
}

bool HavokRagdoll::LineTraceCollisionEnabled( void )
{
	Assert( Limbs.Size() );
	return !Limbs[0].Body->hasProperty( HPRayCastDontCollide );
}

void HavokRagdoll::EnableUnderwaterWorld( bool enable, float raiseLinear, float raiseAngular )
{
	float localRL = enable?raiseLinear:1.0f/raiseLinear;
	float localRA = enable?raiseAngular:1.0f/raiseAngular;

	for( int j = 0; j < Limbs.Size(); ++j )
	{
		hkRigidBody* rb = Limbs[j].Body;
		if( rb )
		{
			hkReal linDamping = rb->getLinearDamping();
			hkReal diff = 1.0 - linDamping;
			diff /= localRL;
			rb->setLinearDamping( 1.0 - diff );

			hkReal angDamping = rb->getAngularDamping();
			diff = 1.0 - angDamping;
			diff /= localRA;
			rb->setAngularDamping( 1.0 - diff );
		}
	}
}

void HavokRagdoll::SetLinearDamping( float linearDamping )
{
	if( linearDamping < 0.0f ) linearDamping = 0.0f;
	if( linearDamping > 1.0f ) linearDamping = 1.0f;
	LinearDamping = linearDamping;

	if (Flags & Active) {
		for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
		{
			Limb &limb = Limbs[iLimb];
			hkRigidBody *rb = limb.Body;
			rb->setLinearDamping( linearDamping );
		}
	}
}

void HavokRagdoll::SetAngularDamping( float angularDamping )
{
	if( angularDamping < 0.0f ) angularDamping = 0.0f;
	if( angularDamping > 1.0f ) angularDamping = 1.0f;
	AngularDamping = angularDamping;
	if (Flags & Active) {
		for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
		{
			Limb &limb = Limbs[iLimb];
			hkRigidBody *rb = limb.Body;
			rb->setAngularDamping( angularDamping );
		}
	}
}

void HavokRagdoll::Joint_SetCollisionCallbacks( int joint, float minimalDelay, float minimalStrength, float minimalOtherBodyMass )
{
	Assert( joint >= 0 );
	int iLimb = FastJointToLimbTable[joint];
	if( iLimb == -1 ) return;

	hkRigidBody *rb = Limbs[iLimb].Body;
	rb->setProcessContactCallbackDelay(0);
	if( Limbs[iLimb].CollisionListener )
	{
		Limbs[iLimb].Body->removeCollisionListener( Limbs[iLimb].CollisionListener );
		delete Limbs[iLimb].CollisionListener;
	}
    TRACE("HavokRagdoll::Joint_SetCollisionCallbacks(): creating collision listener with a delay of %g\n", minimalDelay);
    m_setTag("Physics");
	Limbs[iLimb].CollisionListener = new havokCollisionListener( rb, minimalDelay, minimalStrength, minimalOtherBodyMass );
}

int HavokRagdoll::GetJointFromHavokBody( void* body )
{
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
		if( Limbs[iLimb].Body == (hkRigidBody*)body )
			return Limbs[iLimb].Joint;
	return -1;
}

bool HavokRagdoll::Joint_AreLinked( int joint1, int joint2 )
{
	if( joint1 == joint2 ) return true;

	//Find out what limbs to move
	DynamicArray<hkRigidBody*> limbs;

	hkRigidBody* targetBody = 0;

	Assert( joint2 >= 0 );
	int iLimb2 = FastJointToLimbTable[joint2];
	if( iLimb2 == -1 ) return false;
	targetBody = Limbs[iLimb2].Body;
	if( !targetBody ) return false;		// joint2 not found in model

	Assert( joint1 >= 0 );
	int iLimb1 = FastJointToLimbTable[joint1];
	if( iLimb1 == -1 ) return false;
	limbs.Add(Limbs[iLimb1].Body);
	if( !limbs.Size() ) return false;	// joint1 not found in model

	int limbsNum = 1;
	do
	{
		limbsNum = limbs.Size();
		for(int i = 0; i < Constraints.Size(); ++i )
		{
#ifdef HAVOK3
			hkConstraintInstance* c = Constraints[i].Constraint;
#else
			hkConstraint* c = Constraints[i].Constraint;
#endif
			hkRigidBody* bodyA = (hkRigidBody*)c->getEntityA();
			hkRigidBody* bodyB = (hkRigidBody*)c->getEntityB();
			for(int j = 0; j < limbs.Size(); ++j )
			{
				if( limbs[j] == bodyA )
				{
					if( bodyB == targetBody )
						return true;
					else
						limbs.AddUnique( bodyB );
				}
				else if( limbs[j] == bodyB )
				{
					if( bodyA == targetBody )
						return true;
					else
						limbs.AddUnique( bodyA );
				}
				else
					continue;
				break;
			}
		}

		for(int i = 0; i < Actions.Size(); ++i )
		{
			hkAction* a = Actions[i].Action;
			hkArray<hkEntity*> entities;
			a->getEntities( entities );
			hkRigidBody* bodyA = (hkRigidBody*)entities[0];
			hkRigidBody* bodyB = (entities.getSize() > 1)?(hkRigidBody*)entities[1]:0;
			for(int j = 0; j < limbs.Size(); ++j )
			{
				if( limbs[j] == bodyA )
				{
					if( bodyB == targetBody )
						return true;
					else
						limbs.AddUnique( bodyB );
				}
				else if( limbs[j] == bodyB )
				{
					if( bodyA == targetBody )
						return true;
					else
					limbs.AddUnique( bodyA );
				}
				else
					continue;
				break;
			}
		}
	}
	while( limbsNum < limbs.Size() );
	return false;
}

void HavokRagdoll::Tick( void )
{
	DynamicArray<RagdollJoint>& jointsSkel = Skeleton->Joints;

	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		Limb &limb = Limbs[iLimb];

		const hkTransform& T = limb.Body->getTransform();

#ifdef HAVOK_POSITION_BUG_TRACKING
		const hkVector4& transl = T.getTranslation();
		if( transl.lengthSquared3() > 10000.0f*10000.0f )
		{
			hkPropertyValue p = limb.Body->getProperty( HPEntityPointer );
#ifdef HAVOK3
			Entity* entity = (Entity*)p.getPtr();
#else
			Entity* entity = (Entity*)p.asPtr;
#endif
			GLog.Print( "Ragdoll limb pos > 10000 in tick: (%f,%f,%f), entity name: %s, file: %s\n",
				transl(0), transl(1), transl(2), entity->Name.AsChar(), entity->FileName.AsChar() );
		}
#endif

		Matrix transform;
		T.get4x4ColumnMajor( &transform.mArray[0] );
		jointsSkel[limb.Joint].Transform = limb.Offset * transform;
	}
}

void HavokRagdoll::TickInterpolate( float tickPart )
{
	DynamicArray<RagdollJoint>& jointsSkel = Skeleton->Joints;

	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		Limb &limb = Limbs[iLimb];

		hkTransform T = limb.Body->getTransform();

		hkVector4 end = T.getTranslation();

#ifdef HAVOK_POSITION_BUG_TRACKING
		if( end.lengthSquared3() > 10000.0f*10000.0f )
		{
			hkPropertyValue p = limb.Body->getProperty( HPEntityPointer );
#ifdef HAVOK3
			Entity* entity = (Entity*)p.getPtr();
#else
			Entity* entity = (Entity*)p.asPtr;
#endif
			GLog.Print( "Ragdoll limb pos > 10000 in tickInterpolate: (%f,%f,%f), entity name: %s, file: %s\n",
				end(0), end(1), end(2), entity->Name.AsChar(), entity->FileName.AsChar() );
		}
#endif

		hkVector4 start;
		start.set( limb.LastPosForInterpolation.X, limb.LastPosForInterpolation.Y, limb.LastPosForInterpolation.Z );
		end.sub4( start );
		end.mul4( tickPart );
		start.add4( end );
		T.setTranslation( start );

		Matrix transform;
		T.get4x4ColumnMajor( &transform.mArray[0] );
		jointsSkel[limb.Joint].Transform = limb.Offset * transform;
	}
}

void HavokRagdoll::AdvanceInterpolationFrame( void )
{
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		Limb &limb = Limbs[iLimb];
		hkTransform T = limb.Body->getTransform();
		const hkVector4& pos = T.getTranslation();
		limb.LastPosForInterpolation = Vector(pos(0),pos(1),pos(2));
	}
}

void HavokRagdoll::Freeze( void )
{
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		hkRigidBody *rb = Limbs[iLimb].Body;
#ifdef HAVOK3
		rb->getCollidable()->setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( HCGNoncolliding ) );
#else
		rb->getCollidable().setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( HCGNoncolliding ) );
#endif
		if( rb->getSimulationIsland() )
#ifdef HAVOK3
			mWorld->World->updateCollisionFilterOnEntity( rb, HK_UPDATE_FILTER_ON_ENTITY_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY, HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS );
#else
			mWorld->World->updateCollisionFilter( rb );
#endif
	}
	Flags |= Frozen;
}

void HavokRagdoll::Thaw( void )
{
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		hkRigidBody *rb = Limbs[iLimb].Body;
		if( !rb->isActive() && rb->getSimulationIsland() )
#ifdef HAVOK3
			rb->activate();
#else
			rb->forceActivate();
#endif

#ifdef HAVOK3
		rb->getCollidable()->setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( Limbs[iLimb].CollisionGroup ) );
#else
		rb->getCollidable().setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( Limbs[iLimb].CollisionGroup ) );
#endif
		if( rb->getSimulationIsland() )
#ifdef HAVOK3
			// BLAIR - not sure if this is the right one yet...I'm sure with the OnEntity part, just not the parameters...
			mWorld->World->updateCollisionFilterOnEntity( rb, HK_UPDATE_FILTER_ON_ENTITY_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY, HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS );
#else
			mWorld->World->updateCollisionFilter( rb );
#endif
	}
	Flags &= ~Frozen;
}

void HavokRagdoll::FixConstraintPivots( ConstraintData& data )
{
#ifdef HAVOK3
	hkConstraintInstance* constraint;
	//hkVector4 worldPivotA, worldPivotB;
#else
	hkConstraint* constraint;
#endif
	unsigned char type = data.Type;

	if( data.Type >= 10 )
	{
#ifdef HAVOK3
		// BLAIR #### - I Might be losing some info here....NOT FINISHED!
		constraint = data.Constraint;
#else
		hkBreakableConstraint* breakable = static_cast<hkBreakableConstraint*>(data.Constraint);
		constraint = breakable->m_constraint;
#endif
		type = data.Type-10;
	}
	else
		constraint = data.Constraint;

	switch( type )
	{
	case 0:
		{
#ifdef HAVOK3
			// ### NOT FINISHED!!! ####
			// BLAIR - See demo/demos/Utilities/GameUtils.cpp
			//hkConstraintInstance constraintInstance;
			hkRagdollConstraintData* info = (hkRagdollConstraintData*)constraint->getData();
#else
			hkRagdollConstraint* ragdollCons = static_cast<hkRagdollConstraint*>(constraint);
			hkRagdollConstructionInfo info;
			ragdollCons->getConstructionInfo(info);
#endif
			hkVector4 worldPivotA, worldPivotB;

#ifdef HAVOK3
			Assert( constraint->getRigidBodyA()->getSimulationIsland() );
			Assert( constraint->getRigidBodyB()->getSimulationIsland() );
			
			const hkTransform& a = constraint->getRigidBodyA()->getTransform();
			const hkTransform& b = constraint->getRigidBodyB()->getTransform();

			worldPivotA._setTransformedPos( a, info->m_basisA.m_pivot );
			worldPivotB._setTransformedPos( b, info->m_basisB.m_pivot );
#else
			Assert( info.m_bodyA->getSimulationIsland() );
			Assert( info.m_bodyB->getSimulationIsland() );

			const hkTransform& a = info.m_bodyA->getTransform();
			const hkTransform& b = info.m_bodyB->getTransform();

			worldPivotA._setTransformedPos( a, info.m_pivotInA );
			worldPivotB._setTransformedPos( b, info.m_pivotInB );
#endif

			hkVector4 halfWay = worldPivotA;
			halfWay.add4( worldPivotB );
			halfWay.mul4( 0.5 );

#ifdef HAVOK3
			info->m_basisA.m_pivot._setTransformedInversePos( a, halfWay );
			info->m_basisB.m_pivot._setTransformedInversePos( b, halfWay );
#else
			info.m_pivotInA._setTransformedInversePos( a, halfWay );
			info.m_pivotInB._setTransformedInversePos( b, halfWay );
			ragdollCons->setData( info );
#endif
		}
		break;
	case 1:
		{
#ifdef HAVOK3
			hkLimitedHingeConstraintData* info = (hkLimitedHingeConstraintData*)constraint->getData();
#else
			hkLimitedHingeConstraint* hingeCons = static_cast<hkLimitedHingeConstraint*>(constraint);
			hkLimitedHingeConstructionInfo info;
			hingeCons->getConstructionInfo(info);
#endif
			hkVector4 worldPivotA, worldPivotB;
#ifdef HAVOK3
			worldPivotA._setTransformedPos( constraint->getRigidBodyA()->getTransform(), info->m_basisA.m_pivot );
			worldPivotB._setTransformedPos( constraint->getRigidBodyB()->getTransform(), info->m_basisB.m_pivot );
#else
			worldPivotA._setTransformedPos( info.m_bodyA->getTransform(), info.m_pivotInA );
			worldPivotB._setTransformedPos( info.m_bodyB->getTransform(), info.m_pivotInB );
#endif
			hkVector4 halfWay = worldPivotA;
			halfWay.add4( worldPivotB );
			halfWay.mul4( 0.5 );
#ifdef HAVOK3
			info->m_basisA.m_pivot._setTransformedInversePos( constraint->getRigidBodyA()->getTransform(), halfWay );
			info->m_basisB.m_pivot._setTransformedInversePos( constraint->getRigidBodyB()->getTransform(), halfWay );
#else
			info.m_pivotInA._setTransformedInversePos( info.m_bodyA->getTransform(), halfWay );
			info.m_pivotInB._setTransformedInversePos( info.m_bodyB->getTransform(), halfWay );
			hingeCons->setData( info );
#endif
		}
		break;
	case 2:
		{
#ifdef HAVOK3
			hkHingeConstraintData* info = (hkHingeConstraintData*)constraint->getData();
#else
			hkHingeConstraint* hingeCons = static_cast<hkHingeConstraint*>(constraint);
			hkHingeConstructionInfo info;
			hingeCons->getConstructionInfo(info);
#endif
			hkVector4 worldPivotA, worldPivotB;
#ifdef HAVOK3
			worldPivotA._setTransformedPos( constraint->getRigidBodyA()->getTransform(), info->m_basisA.m_pivot );
			worldPivotB._setTransformedPos( constraint->getRigidBodyB()->getTransform(), info->m_basisB.m_pivot );
#else
			worldPivotA._setTransformedPos( info.m_bodyA->getTransform(), info.m_pivotInA );
			worldPivotB._setTransformedPos( info.m_bodyB->getTransform(), info.m_pivotInB );
#endif
			hkVector4 halfWay = worldPivotA;
			halfWay.add4( worldPivotB );
			halfWay.mul4( 0.5 );
#ifdef HAVOK3
			info->m_basisA.m_pivot._setTransformedInversePos( constraint->getRigidBodyA()->getTransform(), halfWay );
			info->m_basisB.m_pivot._setTransformedInversePos( constraint->getRigidBodyB()->getTransform(), halfWay );
#else
			info.m_pivotInA._setTransformedInversePos( info.m_bodyA->getTransform(), halfWay );
			info.m_pivotInB._setTransformedInversePos( info.m_bodyB->getTransform(), halfWay );
			hingeCons->setData( info );
#endif
		}
	case 3:
		break;
	case 4:
		{
#ifdef HAVOK3
			hkBallAndSocketConstraintData* info = (hkBallAndSocketConstraintData*)constraint->getData();
#else
			hkBallAndSocketConstraint* bsCons = static_cast<hkBallAndSocketConstraint*>(constraint);
			hkBallAndSocketConstructionInfo info;
			bsCons->getConstructionInfo(info);
#endif
			hkVector4 worldPivotA, worldPivotB;
#ifdef HAVOK3
			worldPivotA.setTransformedPos( constraint->getRigidBodyA()->getTransform(), info->m_pivotInA );
			worldPivotB.setTransformedPos( constraint->getRigidBodyB()->getTransform(), info->m_pivotInB );
#else
			worldPivotA.setTransformedPos( info.m_bodyA->getTransform(), info.m_pivotInA );
			worldPivotB.setTransformedPos( info.m_bodyB->getTransform(), info.m_pivotInB );
#endif
			hkVector4 halfWay = worldPivotA;
			halfWay.add4( worldPivotB );
			halfWay.mul4( 0.5 );
#ifdef HAVOK3
			info->m_pivotInA._setTransformedInversePos( constraint->getRigidBodyA()->getTransform(), halfWay );
			info->m_pivotInB._setTransformedInversePos( constraint->getRigidBodyB()->getTransform(), halfWay );
#else
			info.m_pivotInA.setTransformedInversePos( info.m_bodyA->getTransform(), halfWay );
			info.m_pivotInB.setTransformedInversePos( info.m_bodyB->getTransform(), halfWay );

			bsCons->setData( info );
#endif
		}
		break;
/*
	case 5:
		{
			hkStiffSpringConstraint* ssCons = static_cast<hkStiffSpringConstraint*>(constraint);
			hkStiffSpringConstructionInfo info;
			ssCons->getConstructionInfo(info);
			hkVector4 worldPivotA, worldPivotB;
			worldPivotA.setTransformedPos( info.m_bodyA->getTransform(), info.m_pivotInA );
			worldPivotB.setTransformedPos( info.m_bodyB->getTransform(), info.m_pivotInB );
			hkVector4 halfWay = worldPivotA;
			halfWay.add4( worldPivotB );
			halfWay.mul4( 0.5 );
			info.m_pivotInA.setTransformedInversePos( info.m_bodyA->getTransform(), halfWay );
			info.m_pivotInB.setTransformedInversePos( info.m_bodyB->getTransform(), halfWay );
			ssCons->setData( info );
		}
		break;
*/
/*
	case 6:
		{
			hkPrismaticConstraint* prismCons = static_cast<hkPrismaticConstraint*>(constraint);
			hkPrismaticConstructionInfo info;
			prismCons->getConstructionInfo(info);
			hkVector4 worldPivotA, worldPivotB;
			worldPivotA.setTransformedPos( info.m_bodyA->getTransform(), info.m_pivotInA );
			worldPivotB.setTransformedPos( info.m_bodyB->getTransform(), info.m_pivotInB );
			hkVector4 halfWay = worldPivotA;
			halfWay.add4( worldPivotB );
			halfWay.mul4( 0.5 );
			info.m_pivotInA.setTransformedInversePos( info.m_bodyA->getTransform(), halfWay );
			info.m_pivotInB.setTransformedInversePos( info.m_bodyB->getTransform(), halfWay );
			prismCons->setData( info );
		}
		break;
*/
/*
	case 7:
		{
			hkWheelConstraint* wheelCons = static_cast<hkWheelConstraint*>(constraint);
			hkWheelConstructionInfo info;
			wheelCons->getConstructionInfo(info);
			hkVector4 worldPivotA, worldPivotB;
			worldPivotA.setTransformedPos( info.m_bodyA->getTransform(), info.m_pivotInA );
			worldPivotB.setTransformedPos( info.m_bodyB->getTransform(), info.m_pivotInB );
			hkVector4 halfWay = worldPivotA;
			halfWay.add4( worldPivotB );
			halfWay.mul4( 0.5 );
			info.m_pivotInA.setTransformedInversePos( info.m_bodyA->getTransform(), halfWay );
			info.m_pivotInB.setTransformedInversePos( info.m_bodyB->getTransform(), halfWay );
			wheelCons->setData( info );
		}
		break;
*/
	default:
		break;
	}
}

bool HavokRagdoll::SaveConstraintPivots( FileObject* f, ConstraintData& data )
{
#ifdef HAVOK3
	hkConstraintInstance* constraint;
#else
	hkConstraint* constraint;
#endif
	unsigned char type = data.Type;

	*f << type;

	if( data.Type >= 10 )
	{
#ifdef HAVOK3
		constraint = data.Constraint;
		hkBreakableConstraintData* breakable = (hkBreakableConstraintData*)constraint->getData();
#else
		hkBreakableConstraint* breakable = static_cast<hkBreakableConstraint*>(data.Constraint);
		constraint = breakable->m_constraint;
#endif
		type = data.Type-10;

#ifdef HAVOK3
		char broken = breakable->getIsBroken(constraint)?1:0;
#else
		char broken = breakable->getIsBroken()?1:0;
#endif
		float threshold = breakable->getThreshold();
		*f << broken << threshold;
	}
	else
		constraint = data.Constraint;

	switch( type )
	{
	case 0:
		{
#ifdef HAVOK3
			hkRagdollConstraintData* info = (hkRagdollConstraintData*)constraint->getData();

			float v0,v1,v2,v3,v4,v5,v6,v7;
			v0 = info->m_basisA.m_pivot(0); v1 = info->m_basisA.m_pivot(1); v2 = info->m_basisA.m_pivot(2); v3 = info->m_basisA.m_pivot(3);
			v4 = info->m_basisB.m_pivot(0); v5 = info->m_basisB.m_pivot(1); v6 = info->m_basisB.m_pivot(2); v7 = info->m_basisB.m_pivot(3);
#else
			hkRagdollConstraint* ragdollCons = static_cast<hkRagdollConstraint*>(constraint);
			hkRagdollConstructionInfo info;
			ragdollCons->getConstructionInfo(info);

			float v0,v1,v2,v3,v4,v5,v6,v7;
			v0 = info.m_pivotInA(0); v1 = info.m_pivotInA(1); v2 = info.m_pivotInA(2); v3 = info.m_pivotInA(3);
			v4 = info.m_pivotInB(0); v5 = info.m_pivotInB(1); v6 = info.m_pivotInB(2); v7 = info.m_pivotInB(3);
#endif
			*f << v0 << v1 << v2 << v3 << v4 << v5 << v6 << v7;
		}
		break;
	case 1:
		{
#ifdef HAVOK3
			hkLimitedHingeConstraintData* info = (hkLimitedHingeConstraintData*)constraint->getData();

			float v0,v1,v2,v3,v4,v5,v6,v7;
			v0 = info->m_basisA.m_pivot(0); v1 = info->m_basisA.m_pivot(1); v2 = info->m_basisA.m_pivot(2); v3 = info->m_basisA.m_pivot(3);
			v4 = info->m_basisB.m_pivot(0); v5 = info->m_basisB.m_pivot(1); v6 = info->m_basisB.m_pivot(2); v7 = info->m_basisB.m_pivot(3);
#else
			hkLimitedHingeConstraint* hingeCons = static_cast<hkLimitedHingeConstraint*>(constraint);
			hkLimitedHingeConstructionInfo info;
			hingeCons->getConstructionInfo(info);

			float v0,v1,v2,v3,v4,v5,v6,v7;
			v0 = info.m_pivotInA(0); v1 = info.m_pivotInA(1); v2 = info.m_pivotInA(2); v3 = info.m_pivotInA(3);
			v4 = info.m_pivotInB(0); v5 = info.m_pivotInB(1); v6 = info.m_pivotInB(2); v7 = info.m_pivotInB(3);
#endif
			*f << v0 << v1 << v2 << v3 << v4 << v5 << v6 << v7;
		}
		break;
	case 2:
		{
#ifdef HAVOK3
			hkHingeConstraintData* info = (hkHingeConstraintData*)constraint->getData();

			float v0,v1,v2,v3,v4,v5,v6,v7;
			v0 = info->m_basisA.m_pivot(0); v1 = info->m_basisA.m_pivot(1); v2 = info->m_basisA.m_pivot(2); v3 = info->m_basisA.m_pivot(3);
			v4 = info->m_basisB.m_pivot(0); v5 = info->m_basisB.m_pivot(1); v6 = info->m_basisB.m_pivot(2); v7 = info->m_basisB.m_pivot(3);
#else
			hkHingeConstraint* hingeCons = static_cast<hkHingeConstraint*>(constraint);
			hkHingeConstructionInfo info;
			hingeCons->getConstructionInfo(info);

			float v0,v1,v2,v3,v4,v5,v6,v7;
			v0 = info.m_pivotInA(0); v1 = info.m_pivotInA(1); v2 = info.m_pivotInA(2); v3 = info.m_pivotInA(3);
			v4 = info.m_pivotInB(0); v5 = info.m_pivotInB(1); v6 = info.m_pivotInB(2); v7 = info.m_pivotInB(3);
#endif
			*f << v0 << v1 << v2 << v3 << v4 << v5 << v6 << v7;
		}
		break;
	case 4:
		{
#ifdef HAVOK3
			hkBallAndSocketConstraintData* info = (hkBallAndSocketConstraintData*)constraint->getData();

			float v0,v1,v2,v3,v4,v5,v6,v7;
			v0 = info->m_pivotInA(0); v1 = info->m_pivotInA(1); v2 = info->m_pivotInA(2); v3 = info->m_pivotInA(3);
			v4 = info->m_pivotInB(0); v5 = info->m_pivotInB(1); v6 = info->m_pivotInB(2); v7 = info->m_pivotInB(3);
#else
			hkBallAndSocketConstraint* bsCons = static_cast<hkBallAndSocketConstraint*>(constraint);
			hkBallAndSocketConstructionInfo info;
			bsCons->getConstructionInfo(info);

			float v0,v1,v2,v3,v4,v5,v6,v7;
			v0 = info.m_pivotInA(0); v1 = info.m_pivotInA(1); v2 = info.m_pivotInA(2); v3 = info.m_pivotInA(3);
			v4 = info.m_pivotInB(0); v5 = info.m_pivotInB(1); v6 = info.m_pivotInB(2); v7 = info.m_pivotInB(3);
#endif
			*f << v0 << v1 << v2 << v3 << v4 << v5 << v6 << v7;
		}
		break;
	default:
		break;
	}
	return true;
}

bool HavokRagdoll::LoadConstraintPivots( FileObject* f, ConstraintData& data )
{
#ifdef HAVOK3
	hkConstraintInstance* constraint;
#else
	hkConstraint* constraint;
#endif
	unsigned char type = data.Type;

	unsigned char inType;
	*f >> inType;

	if( inType != type ) return false;

	if( data.Type >= 10 )
	{
#ifdef HAVOK3
		constraint = data.Constraint;
		hkBreakableConstraintData* breakable = (hkBreakableConstraintData*)constraint->getData();
#else
		hkBreakableConstraint* breakable = static_cast<hkBreakableConstraint*>(data.Constraint);
		constraint = breakable->m_constraint;
#endif
		type = data.Type-10;
		float threshold;
		char broken;
		*f >> broken >> threshold;
#ifndef HAVOK3
		// BLAIR - This function has been removed in HAVOK3...TAKE ANOTHER LOOK AT THIS!!
		breakable->setIsBroken( (broken!=0) );
#endif
		breakable->setThreshold( threshold );
	}
	else
		constraint = data.Constraint;

	switch( type )
	{
	case 0:
		{
#ifdef HAVOK3
			hkRagdollConstraintData* info = (hkRagdollConstraintData*)constraint->getData();

			float v0,v1,v2,v3,v4,v5,v6,v7;
			*f >> v0 >> v1 >> v2 >> v3 >> v4 >> v5 >> v6 >> v7;
			info->m_basisA.m_pivot(0) = v0; info->m_basisA.m_pivot(1) = v1; info->m_basisA.m_pivot(2) = v2; info->m_basisA.m_pivot(3) = v3;
			info->m_basisB.m_pivot(0) = v4; info->m_basisB.m_pivot(1) = v5; info->m_basisB.m_pivot(2) = v6; info->m_basisB.m_pivot(3) = v7;
#else
			hkRagdollConstraint* ragdollCons = static_cast<hkRagdollConstraint*>(constraint);
			hkRagdollConstructionInfo info;
			ragdollCons->getConstructionInfo(info);
			float v0,v1,v2,v3,v4,v5,v6,v7;
			*f >> v0 >> v1 >> v2 >> v3 >> v4 >> v5 >> v6 >> v7;
			info.m_pivotInA(0) = v0; info.m_pivotInA(1) = v1; info.m_pivotInA(2) = v2; info.m_pivotInA(3) = v3;
			info.m_pivotInB(0) = v4; info.m_pivotInB(1) = v5; info.m_pivotInB(2) = v6; info.m_pivotInB(3) = v7;
			ragdollCons->setData( info );
#endif
		}
		break;
	case 1:
		{
#ifdef HAVOK3
			hkLimitedHingeConstraintData* info = (hkLimitedHingeConstraintData*)constraint->getData();

			float v0,v1,v2,v3,v4,v5,v6,v7;
			*f >> v0 >> v1 >> v2 >> v3 >> v4 >> v5 >> v6 >> v7;
			info->m_basisA.m_pivot(0) = v0; info->m_basisA.m_pivot(1) = v1; info->m_basisA.m_pivot(2) = v2; info->m_basisA.m_pivot(3) = v3;
			info->m_basisB.m_pivot(0) = v4; info->m_basisB.m_pivot(1) = v5; info->m_basisB.m_pivot(2) = v6; info->m_basisB.m_pivot(3) = v7;
#else
			hkLimitedHingeConstraint* hingeCons = static_cast<hkLimitedHingeConstraint*>(constraint);
			hkLimitedHingeConstructionInfo info;
			hingeCons->getConstructionInfo(info);
			float v0,v1,v2,v3,v4,v5,v6,v7;
			*f >> v0 >> v1 >> v2 >> v3 >> v4 >> v5 >> v6 >> v7;
			info.m_pivotInA(0) = v0; info.m_pivotInA(1) = v1; info.m_pivotInA(2) = v2; info.m_pivotInA(3) = v3;
			info.m_pivotInB(0) = v4; info.m_pivotInB(1) = v5; info.m_pivotInB(2) = v6; info.m_pivotInB(3) = v7;
			hingeCons->setData( info );
#endif
		}
		break;
	case 2:
		{
#ifdef HAVOK3
			hkHingeConstraintData* info = (hkHingeConstraintData*)constraint->getData();

			float v0,v1,v2,v3,v4,v5,v6,v7;
			*f >> v0 >> v1 >> v2 >> v3 >> v4 >> v5 >> v6 >> v7;
			info->m_basisA.m_pivot(0) = v0; info->m_basisA.m_pivot(1) = v1; info->m_basisA.m_pivot(2) = v2; info->m_basisA.m_pivot(3) = v3;
			info->m_basisB.m_pivot(0) = v4; info->m_basisB.m_pivot(1) = v5; info->m_basisB.m_pivot(2) = v6; info->m_basisB.m_pivot(3) = v7;
#else
			hkHingeConstraint* hingeCons = static_cast<hkHingeConstraint*>(constraint);
			hkHingeConstructionInfo info;
			hingeCons->getConstructionInfo(info);
			float v0,v1,v2,v3,v4,v5,v6,v7;
			*f >> v0 >> v1 >> v2 >> v3 >> v4 >> v5 >> v6 >> v7;
			info.m_pivotInA(0) = v0; info.m_pivotInA(1) = v1; info.m_pivotInA(2) = v2; info.m_pivotInA(3) = v3;
			info.m_pivotInB(0) = v4; info.m_pivotInB(1) = v5; info.m_pivotInB(2) = v6; info.m_pivotInB(3) = v7;
			hingeCons->setData( info );
#endif
		}
		break;
	case 4:
		{
#ifdef HAVOK3
			hkBallAndSocketConstraintData* info = (hkBallAndSocketConstraintData*)constraint->getData();

			float v0,v1,v2,v3,v4,v5,v6,v7;
			*f >> v0 >> v1 >> v2 >> v3 >> v4 >> v5 >> v6 >> v7;
			info->m_pivotInA(0) = v0; info->m_pivotInA(1) = v1; info->m_pivotInA(2) = v2; info->m_pivotInA(3) = v3;
			info->m_pivotInB(0) = v4; info->m_pivotInB(1) = v5; info->m_pivotInB(2) = v6; info->m_pivotInB(3) = v7;
#else
			hkBallAndSocketConstraint* bsCons = static_cast<hkBallAndSocketConstraint*>(constraint);
			hkBallAndSocketConstructionInfo info;
			bsCons->getConstructionInfo(info);
			float v0,v1,v2,v3,v4,v5,v6,v7;
			*f >> v0 >> v1 >> v2 >> v3 >> v4 >> v5 >> v6 >> v7;
			info.m_pivotInA(0) = v0; info.m_pivotInA(1) = v1; info.m_pivotInA(2) = v2; info.m_pivotInA(3) = v3;
			info.m_pivotInB(0) = v4; info.m_pivotInB(1) = v5; info.m_pivotInB(2) = v6; info.m_pivotInB(3) = v7;
			bsCons->setData( info );
#endif
		}
		break;
	default:
		break;
	}
	return true;
}

void HavokRagdoll::FixActionPivots( ActionData& data )
{
	switch( data.Type )
	{
	case 0:	// dashpot action
		{
			hkDashpotAction* dashpotAct = static_cast<hkDashpotAction*>(data.Action);
#ifndef HAVOK3
			hkDashpotActionConstructionInfo info;
#endif
			hkArray<hkEntity*> entities;
			dashpotAct->getEntities( entities );
			if( entities.getSize() < 2 ) return;

			hkVector4 worldPivotA;
			hkVector4 worldPivotB;
			worldPivotA.setTransformedPos( ((hkRigidBody*)entities[0])->getTransform(), dashpotAct->getPointA() );
			worldPivotB.setTransformedPos( ((hkRigidBody*)entities[1])->getTransform(), dashpotAct->getPointB() );

			hkVector4 halfWay = worldPivotA;
			halfWay.add4( worldPivotB );
			halfWay.mul4( 0.5 );

			worldPivotA.setTransformedInversePos( ((hkRigidBody*)entities[0])->getTransform(), halfWay );
			worldPivotB.setTransformedInversePos( ((hkRigidBody*)entities[1])->getTransform(), halfWay );

			dashpotAct->setPointA( worldPivotA );
			dashpotAct->setPointB( worldPivotB );
		}
		break;
	case 1:	// angular dashpot action
		break;
	case 2:	// spring action
/*
		{
			hkSpringAction* springAct = static_cast<hkSpringAction*>(data.Action);
			hkSpringActionConstructionInfo info;
			hkArray<hkEntity*> entities;
			springAct->getEntities( entities );
			if( entities.getSize() < 2 ) return;

			hkVector4 worldPivotA;
			hkVector4 worldPivotB;
			worldPivotA.setTransformedPos( ((hkRigidBody*)entities[0])->getTransform(), springAct->getPositionAinA() );
			worldPivotB.setTransformedPos( ((hkRigidBody*)entities[1])->getTransform(), springAct->getPositionBinB() );

			hkVector4 halfWay = worldPivotA;
			halfWay.add4( worldPivotB );
			halfWay.mul4( 0.5 );

			worldPivotA.setTransformedInversePos( ((hkRigidBody*)entities[0])->getTransform(), halfWay );
			worldPivotB.setTransformedInversePos( ((hkRigidBody*)entities[1])->getTransform(), halfWay );

			springAct->setBodyA( (hkRigidBody*)entities[0], worldPivotA );
			springAct->setBodyB( (hkRigidBody*)entities[1], worldPivotB );
		}
*/
		break;
	default:
		break;
	}
}

bool HavokRagdoll::SaveActionPivots( FileObject* f, ActionData& data )
{
	*f << data.Type;

	switch( data.Type )
	{
	case 0:	// dashpot action
		{
			hkDashpotAction* dashpotAct = static_cast<hkDashpotAction*>(data.Action);
#ifndef HAVOK3
			hkDashpotActionConstructionInfo info;
#endif
			hkArray<hkEntity*> entities;
			dashpotAct->getEntities( entities );
			unsigned char entNum = entities.getSize();
			*f << entNum;
			if( entNum < 2 ) return true;

			hkVector4 inA = dashpotAct->getPointA();
			hkVector4 inB = dashpotAct->getPointB();

			float v0,v1,v2,v3,v4,v5,v6,v7;
			v0 = inA(0); v1 = inA(1); v2 = inA(2); v3 = inA(3);
			v4 = inB(0); v5 = inB(1); v6 = inB(2); v7 = inB(3);
			*f << v0 << v1 << v2 << v3 << v4 << v5 << v6 << v7;
		}
		break;
	default:
		break;
	}
	return true;
}

bool HavokRagdoll::LoadActionPivots( FileObject* f, ActionData& data )
{
	unsigned char inType;
	*f >> inType;

	if( inType != data.Type ) return false;

	switch( data.Type )
	{
	case 0:	// dashpot action
		{
			hkDashpotAction* dashpotAct = static_cast<hkDashpotAction*>(data.Action);
#ifndef HAVOK3
			hkDashpotActionConstructionInfo info;
#endif
			hkArray<hkEntity*> entities;
			dashpotAct->getEntities( entities );

			unsigned char entNum = entities.getSize();
			unsigned char inEntNum;
			*f >> inEntNum;
			if( inEntNum != entNum ) return false;
			if( entNum < 2 ) return true;

			float v0,v1,v2,v3,v4,v5,v6,v7;
			*f >> v0 >> v1 >> v2 >> v3 >> v4 >> v5 >> v6 >> v7;
			hkVector4 inA(v0,v1,v2,v3);
			hkVector4 inB(v4,v5,v6,v7);
			dashpotAct->setPointA( inA );
			dashpotAct->setPointB( inB );
		}
		break;
	default:
		break;
	}
	return true;
}

void HavokRagdoll::EnableGravity( bool enable )
{
	Vector newGrav(0,0,0);
	if( enable )
	{
		hkVector4 worldGravity = mWorld->World->getGravity();
		newGrav.X = worldGravity(0);
		newGrav.Y = worldGravity(1);
		newGrav.Z = worldGravity(2);
	}
	SetGravity( newGrav );
}

void HavokRagdoll::SetGravity( Vector& gravity )
{
#ifndef HAVOK3 // ============================================

	hkVector4 worldGravity = mWorld->World->getGravity();
	hkVector4 currentGravity;
	if( !Limbs.Size() )
		currentGravity = worldGravity;
	else if( Limbs[0].AntiGravityAction )
	{
		currentGravity = worldGravity;
		currentGravity.sub4( Limbs[0].AntiGravityAction->getGravity() );
	}
	else
		currentGravity = worldGravity;

	hkVector4 newGravity( gravity.X, gravity.Y, gravity.Z );

	if( currentGravity.equals3( newGravity ) ) return;

	if( newGravity.equals3( worldGravity ) )
	{
		for( int i = 0; i <Limbs.Size(); ++i )
			if( Limbs[i].AntiGravityAction )
			{
				if( Limbs[i].AntiGravityAction->getSimulationIsland() )
					mWorld->World->removeAction( Limbs[i].AntiGravityAction );
				Limbs[i].AntiGravityAction->removeReference();
				Limbs[i].AntiGravityAction = 0;
			}
		return;
	}

	hkVector4 gravityToSet( worldGravity );
	gravityToSet.sub4( newGravity );

	for( int i = 0; i <Limbs.Size(); ++i )
		if( !Limbs[i].AntiGravityAction )
		{
            m_setTag("Physics");
			Limbs[i].AntiGravityAction = new havokAntiGravityAction( Limbs[i].Body, gravityToSet );
			mWorld->World->addAction( Limbs[i].AntiGravityAction );
		}
		else
			Limbs[i].AntiGravityAction->setGravity( gravityToSet );
#else
	GLog.Print("HavokRagdoll::SetGravity() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}

Vector HavokRagdoll::GetGravity( void )
{
	hkVector4 worldGravity = mWorld->World->getGravity();
	hkVector4 currentGravity;
	if( !Limbs.Size() )
		currentGravity = worldGravity;
	else if( Limbs[0].AntiGravityAction )
	{
		currentGravity = worldGravity;
		currentGravity.sub4( Limbs[0].AntiGravityAction->getGravity() );
	}
	else
		currentGravity = worldGravity;
	
	return Vector( currentGravity(0), currentGravity(1), currentGravity(2) );
}

void HavokRagdoll::FillRagdollData( RagdollData& data )
{
	// Get positions and rotations
	GetLimbsPositionsAndRotations( data.LimbPositions, data.LimbOrientations );

	// Get velocities and angular velocities
	data.LimbLinearVelocities.ClearArray();
	data.LimbAngularVelocities.ClearArray();
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		hkVector4 v;
		v = Limbs[iLimb].Body->getLinearVelocity();
		data.LimbLinearVelocities.Add( Vector( v(0), v(1), v(2) ) );
		v = Limbs[iLimb].Body->getAngularVelocity();
		data.LimbAngularVelocities.Add( Vector( v(0), v(1), v(2) ) );
	}

	// Get pinned data
	for( int iLimb = 0; iLimb < Limbs.Size(); ++iLimb )
#ifdef HAVOK3
		if ( Limbs[iLimb].Body->getMotionType() == hkMotion::MOTION_KEYFRAMED )
			data.LimbPinned.Add( true );
		else
			data.LimbPinned.Add( false );
#else
		data.LimbPinned.Add( Limbs[iLimb].Body->isKeyframed() );
#endif

	// Get dampings
	data.LinearDamping = LinearDamping;
	data.AngularDamping = AngularDamping;

	// Get mass
	data.Mass = GetMass();

	// Get collision group
	data.CollisionGroup = GetCollisionGroup();
}

void HavokRagdoll::SetFromRagdollData( RagdollData& data, bool loading )
{
	Flags |= Active;	// Activate by default

	if( loading )
	{
		for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
		{
			hkRigidBody *rb = Limbs[iLimb].Body;
			if( !rb->getSimulationIsland() ) mWorld->World->addEntity( rb );
		}
	}

	// Set mass
	if( !loading )
		SetMass( data.Mass );

	// Set dampings
	if( data.LinearDamping != LinearDamping )
		SetLinearDamping( data.LinearDamping );
	if( data.AngularDamping != AngularDamping )
		SetAngularDamping( data.AngularDamping );

	// Set collision group
	if( !loading )
		SetCollisionGroup( data.CollisionGroup );

	// Set pinned data
	for( int iLimb = 0; iLimb < Limbs.Size(); ++iLimb )
#ifdef HAVOK3
	{
		// BLAIR - this should be ok...
		if ( data.LimbPinned[iLimb] )
			Limbs[iLimb].Body->setMotionType( hkMotion::MOTION_KEYFRAMED );
		else
			Limbs[iLimb].Body->setMotionType( hkMotion::MOTION_FIXED );
	}
#else
		Limbs[iLimb].Body->setKeyframed( data.LimbPinned[iLimb] );
#endif
	// Set velocities and angular velocities
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		Vector& vec = data.LimbLinearVelocities[iLimb];
		hkVector4 v( vec.X, vec.Y, vec.Z );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !v.isOk4() || ( fabs(v(0)) > 1000.0f ) || ( fabs(v(1)) > 1000.0f ) || ( fabs(v(2)) > 1000.0f ) )
#else
		if( !v.isOk() || ( fabs(v(0)) > 1000.0f ) || ( fabs(v(1)) > 1000.0f ) || ( fabs(v(2)) > 1000.0f ) )
#endif
			GLog.Print( "Velocity > 1000 in Ragdoll::SetFromRagdollData\n" );
#endif

		Limbs[iLimb].Body->setLinearVelocity(v);

		vec = data.LimbAngularVelocities[iLimb];
		v.set( vec.X, vec.Y, vec.Z );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !v.isOk4() || ( fabs(v(0)) > 100.0f ) || ( fabs(v(1)) > 100.0f ) || ( fabs(v(2)) > 100.0f ) )
#else
		if( !v.isOk() || ( fabs(v(0)) > 100.0f ) || ( fabs(v(1)) > 100.0f ) || ( fabs(v(2)) > 100.0f ) )
#endif
			GLog.Print( "Angular Velocity > 100 in Ragdoll::SetFromRagdollData\n" );
#endif

		Limbs[iLimb].Body->setAngularVelocity(v);
	}

	// Set positions and rotations
	SetLimbsPositionsAndRotations( data.LimbPositions, data.LimbOrientations );
}

bool HavokRagdoll::LineTrace( Vector& start, Vector& end, HitData& hitData )
{
	hkVector4 startWorld( start.X, start.Y, start.Z );
	hkVector4 endWorld( end.X, end.Y, end.Z );

	float tempMinDist = 2.0f;
	hkVector4 tempNormal;
	hkRigidBody* tempBody = 0;

	for( int i = 0; i < Limbs.Size(); ++i )
	{
#ifdef HAVOK3
		//hkShape::hkRayResults initResults;
		hkShapeRayCastInput input;
		hkShapeRayCastOutput output;
#else
		hkShape::hkRayResults initResults;
#endif

		const hkTransform& trans = Limbs[i].Body->getTransform();
		hkVector4 shapefrom,shapeto;
		shapefrom.setTransformedInversePos( trans, startWorld );
		shapeto.setTransformedInversePos( trans, endWorld );

#ifdef HAVOK3
		input.m_from = shapefrom;
		input.m_to = shapeto;
		if( Limbs[i].Body->getCollidable()->getShape()->castRay( input, output )
			&& ( output.m_hitFraction < tempMinDist ) )
		{
			tempMinDist = output.m_hitFraction ;
			// BLAIR - NOTE...THIS IS NORMALIZED...whereas before it wasn't.  It's also in the ray's space
			// ...not sure if it's in the shape's anymore.
			tempNormal = output.m_normal;
			tempBody = Limbs[i].Body;
		}
#else
		if( Limbs[i].Body->getCollidable().getShape()->castRay( shapefrom, shapeto, initResults )
			&& ( initResults.m_mindist < tempMinDist ) )
		{
			tempMinDist = initResults.m_mindist;
			tempNormal = initResults.m_unnormalizedHitNormal;
			tempBody = Limbs[i].Body;
		}
#endif
	}
	if( tempMinDist == 2.0f )
		return false;

	hkVector4 intersectionPointWorld;
	intersectionPointWorld.setInterpolate4( startWorld, endWorld, tempMinDist );
	hitData.HitPoint = Vector( intersectionPointWorld(0), intersectionPointWorld(1), intersectionPointWorld(2) );

	hkVector4 temp;
	temp = startWorld;
	temp.sub4( intersectionPointWorld );
	hitData.Dist = temp.length3();

	hkVector4 hitNormal;
	hitNormal.setRotatedDir( tempBody->getRotation(), tempNormal );
	hitNormal.normalize3();
	hitData.Normal = Vector( hitNormal(0), hitNormal(1), hitNormal(2) );
	hitData.HavokEntity = tempBody;

	hkPropertyValue p = tempBody->getProperty( HPEntityPointer );
#ifdef HAVOK3
	hitData.UserData = p.getPtr();
#else
	hitData.UserData = p.asPtr;
#endif
	
	return true;
}

void HavokRagdoll::SetFriction( float friction )
{
	for( int iLimb = 0; iLimb < Limbs.Size(); ++iLimb )
		Limbs[iLimb].Body->getMaterial().setFriction( friction );
}

void HavokRagdoll::SetRestitution( float restitution )
{
	for( int iLimb = 0; iLimb < Limbs.Size(); ++iLimb )
		Limbs[iLimb].Body->getMaterial().setRestitution( restitution );
}

void HavokRagdoll::SetBreakablesThreshold( float threshold )
{
	// Add constraints to world
	for( int iConstraint = 0; iConstraint < Constraints.Size(); ++iConstraint )
		if( Constraints[iConstraint].Type >= 10 )
		{
#ifdef HAVOK3
            m_setTag("Physics");
			hkBreakableConstraintData* breakable = new hkBreakableConstraintData(Constraints[iConstraint].Constraint->getData(), mWorld->World);
			breakable->setThreshold( threshold );
			breakable->setRemoveWhenBroken(false);

			// BLAIR - REMOVE THE REFERENCE FOR THIS CONSTRAINT SINCE breakable now owns it!!
			Constraints[iConstraint].Constraint->removeReference();

			// Add the constraint to the world...
			// BLAIR - We should be adding the line below, but right now, it doesn't yet make sense.  I imagine that
			// once things are fully switched to Havok3, that we might not need the Constraints[] collection, and that
			// they will all be governed by hkWorld, but for now, I'll just keep an eye on it.
			// ### THIS SHOULD BE IN!!! ###
			//mWorld->World->addConstraint( breakable );  // #### DON'T GO ANYWHERE WITHOUT ADDING THIS!!! ###
			breakable->removeReference(); // the world owns this now...

#else
			hkBreakableConstraint* breakable = static_cast<hkBreakableConstraint*>(Constraints[iConstraint].Constraint);
			breakable->setThreshold( threshold );
#endif
		}
}

#ifdef HAVOK3
int HavokRagdoll::ConstraintInRagdoll( hkConstraintInstance* constraint )
#else
int HavokRagdoll::ConstraintInRagdoll( hkConstraint* constraint )
#endif
{
	for( int i = 0; i < Constraints.Size(); ++i )
		if( Constraints[i].Constraint == constraint )
			return i;
#ifndef HAVOK3
// BLAIR - In HAVOK3, the hkBreakableConstraintData doesn't contain the pointer to the constraint...the world
// contains the pointer to the constraint, which constains the pointer to the hkBreakableConstraintData which
// constrains the pointer to the original constraint data...see? Totally clear.
		else if( Constraints[i].Type >= 10 )
		{
			hkBreakableConstraint* breakable = static_cast<hkBreakableConstraint*>(Constraints[i].Constraint);
			if( breakable->m_constraint == constraint )
				return i;
		}
#endif
	return -1;
}
#ifdef HAVOK3
bool HavokRagdoll::GetConstraintJoints( hkConstraintInstance* constraint, int& joint1, int& joint2 )
#else
bool HavokRagdoll::GetConstraintJoints( hkConstraint* constraint, int& joint1, int& joint2 )
#endif
{
	joint1 = -1;
	joint2 = -1;
	for( int i = 0; i < Constraints.Size(); ++i )
		if( Constraints[i].Constraint == constraint )
		{
			hkEntity* eA = constraint->getEntityA();
			hkEntity* eB = constraint->getEntityB();

			int found = 0;
			for( int i = 0;( i < Limbs.Size() ) && ( found < 2 ); ++i )
			{
				if( eA && Limbs[i].Body == eA )
				{
					joint1 = Limbs[i].Joint;
					found++;
					eA = 0;
				}

				if( eB && Limbs[i].Body == eB )
				{
					joint2 = Limbs[i].Joint;
					found++;
					eB = 0;
				}
			}
			return true;
		}
		else if( Constraints[i].Type >= 10 )
		{
#ifdef HAVOK3
			// BLAIR - this might not be entirely correct...but then again, maybe it is.
			hkConstraintInstance* breakable = Constraints[i].Constraint;
			if( breakable == constraint )
#else
			hkBreakableConstraint* breakable = static_cast<hkBreakableConstraint*>(Constraints[i].Constraint);
			if( breakable->m_constraint == constraint )
#endif
			{
				hkEntity* eA = constraint->getEntityA();
				hkEntity* eB = constraint->getEntityB();

				int found = 2;
				if( eA ) found--;
				if( eB ) found--;

				for( int i = 0;( i < Limbs.Size() ) && ( found < 2 ); ++i )
				{
					if( eA && Limbs[i].Body == eA )
					{
						joint1 = Limbs[i].Joint;
						found++;
						eA = 0;
					}

					if( eB && Limbs[i].Body == eB )
					{
						joint2 = Limbs[i].Joint;
						found++;
						eB = 0;
					}
				}
				return true;
			}
		}
	return false;
}

void HavokRagdoll::SetDeactivationHardness( float hardness )
{
    m_setTag("Physics");
	hkSphereShape* shape = new hkSphereShape( hardness );

	for( int iLimb = 0; iLimb < Limbs.Size(); ++iLimb )
	{
		hkSpatialRigidBodyDeactivator* dea = (hkSpatialRigidBodyDeactivator*)Limbs[iLimb].Body->getDeactivator();
#ifdef HAVOK3
		dea->m_radiusSqrd = shape->getRadius() * shape->getRadius();
#else
		dea->setRadiusSqrd( shape );
#endif
	}

	shape->removeReference();
}

void HavokRagdoll::EternallyFreeze( void )
{
	if( Flags & EternallyFrozen ) return;

	if( Flags & HardDeactivator )
	{
		for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
			mWorld->mDeactivator->RemoveRigidBody( Limbs[iLimb].Body );
		Flags &= ~HardDeactivator;
	}
	Deactivate();
	SetPinned(true);
	Flags |= EternallyFrozen;
}

#define HAVOKRAGDOLL_LOAD_SAVE_TOKEN 0x5e49

bool HavokRagdoll::LoadRagdoll( FileObject* f )
{
	if( !f->TestToken( HAVOKRAGDOLL_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: HavokRagdoll::LoadRagdoll()\n" );
		return false;
	}

	RagdollData data;

	int limbSize;
	*f >> limbSize;

	Vector temp;
	Quaternion temp2;
	char temp3;
	for( int i = 0; i < limbSize; ++i ) { *f >> temp; data.LimbPositions.Add( temp ); }
	for( int i = 0; i < limbSize; ++i ) { *f >> temp2; data.LimbOrientations.Add( temp2 ); }
	for( int i = 0; i < limbSize; ++i ) { *f >> temp; data.LimbLinearVelocities.Add( temp ); }
	for( int i = 0; i < limbSize; ++i ) { *f >> temp; data.LimbAngularVelocities.Add( temp ); }
    for( int i = 0; i < limbSize; ++i ) { *f >>	temp3; data.LimbPinned.Add( temp3?true:false ); }

    
	char test;
	float minDelay, minStrength, minMass;
	for( int i = 0; i < limbSize; ++i )
	{
		if( Limbs[i].CollisionListener )
		{
			Limbs[i].Body->removeCollisionListener( Limbs[i].CollisionListener );
			delete Limbs[i].CollisionListener;
			Limbs[i].CollisionListener = 0;
		}

		*f >> test;
		if( test )	// a collision listener existed
		{
			Limbs[i].Body->setProcessContactCallbackDelay(0);
			*f >> minDelay >> minStrength >> minMass;
            TRACE("HavokRagdoll::LoadRagdoll(): creating collision listener with a delay of %g\n", minDelay);
            m_setTag("Physics");
			Limbs[i].CollisionListener = new havokCollisionListener( Limbs[i].Body, minDelay, minStrength, minMass );
		}

		// Load limb mass
		float mass;
		*f >> mass;
		float wasMass = Limbs[i].Body->getMass();
		if( ( mass != wasMass ) &&
			( mass != 0.0f ) )
			Limbs[i].Body->setMass( mass );

		// Load limb collision group
		unsigned char cg;
		*f >> cg;
		if( cg != Limbs[i].CollisionGroup )
		{
#ifdef HAVOK3
			Limbs[i].Body->getCollidable()->setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( cg ) );
#else
			Limbs[i].Body->getCollidable().setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( cg ) );
#endif
			Limbs[i].CollisionGroup = cg;
			if( Limbs[i].Body->getSimulationIsland() )
#ifdef HAVOK3
				mWorld->World->updateCollisionFilterOnEntity( Limbs[i].Body, HK_UPDATE_FILTER_ON_ENTITY_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY, HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS );
#else
				mWorld->World->updateCollisionFilter( Limbs[i].Body );
#endif
		}

		// Load friction and restitution
		float friction, restitution;
		*f >> friction >> restitution;
		Limbs[i].Body->getMaterial().setFriction( friction );
		Limbs[i].Body->getMaterial().setRestitution( restitution );

		// Load limb inertia tensor
		float x,y,z;
		*f >> x >> y >> z;
		hkVector4 row0( x, y, z );
		*f >> x >> y >> z;
		hkVector4 row1( x, y, z );
		*f >> x >> y >> z;
		hkVector4 row2( x, y, z );
		hkMatrix3 inertiaTensor;
		inertiaTensor.setRows( row0, row1, row2 );
		Limbs[i].Body->setInertiaLocal( inertiaTensor );

		// Load limb active state
		char active;
		*f >> active;
		if( active )
#ifdef HAVOK3
			Limbs[i].Body->activate();
		else
			Limbs[i].Body->deactivate();
#else
			Limbs[i].Body->forceActivate();
		else
			Limbs[i].Body->forceDeactivate();
#endif
	}
    
	// Get various other data
	*f >> data.LinearDamping >> data.AngularDamping;
	*f >> mInertiaTensorMultiplier;

	SetFromRagdollData( data, true );

	float realDampingAngular = AngularDamping;
	if( mWorld->IsUnderwater() )
		realDampingAngular = 1.0f - ((1.0f-realDampingAngular)/GEngine->Physics->TWUnderwaterDampingRaiseAngular);

	for( int i = 0; i < limbSize; ++i )
	{
		hkRigidBody* rb = Limbs[i].Body;
		rb->setLinearDamping( LinearDamping );
		rb->setAngularDamping( realDampingAngular );
	}

	// Add constraints to world
	for( int iConstraint = 0; iConstraint < Constraints.Size(); ++iConstraint )
	{
		if( !LoadConstraintPivots( f, Constraints[iConstraint] ) ) return false;
		char inWorld;
		*f >> inWorld;
		if( inWorld )
			mWorld->World->addConstraint( Constraints[iConstraint].Constraint );
		else
			mWorld->World->removeConstraint( Constraints[iConstraint].Constraint );
	}

	// Add actions to world
	for( int iAction = 0; iAction < Actions.Size(); ++iAction )
	{
		if( !LoadActionPivots( f, Actions[iAction] ) ) return false;
		char inWorld;
		*f >> inWorld;
		if( inWorld )
			mWorld->World->addAction( Actions[iAction].Action );
		else
			mWorld->World->removeAction( Actions[iAction].Action );
	}

	unsigned char flags;
	*f >> flags;
	if( flags & IsMovedByExplosions ) Flags |= IsMovedByExplosions; else Flags &= ~IsMovedByExplosions;
	if( flags & HardDeactivator ) SetHardDeactivator();
	if( flags & Frozen ) Flags |= Frozen; else Flags &= ~Frozen;
	if( flags & EternallyFrozen ) Flags |= EternallyFrozen; else Flags &= ~EternallyFrozen;

	// Antigravity
	Vector gravity;
	*f >> gravity;
	if( gravity != GetGravity() )
		SetGravity( gravity );

	Tick();

	return true;
}

bool HavokRagdoll::LoadRagdollCollisionGroups( FileObject* f )
{
	if( !f->TestToken( HAVOKRAGDOLL_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: HavokRagdoll::LoadRagdollCollisionGroups()\n" );
		return false;
	}

	int limbSize;
	*f >> limbSize;

	Assert( limbSize == Limbs.Size() );

	char test;
	float minDelay, minStrength, minMass;
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		if( Limbs[iLimb].CollisionListener )
		{
			Limbs[iLimb].Body->removeCollisionListener( Limbs[iLimb].CollisionListener );
			delete Limbs[iLimb].CollisionListener;
			Limbs[iLimb].CollisionListener = 0;
		}

		*f >> test;
		if( test )	// a collision listener existed
		{
			Limbs[iLimb].Body->setProcessContactCallbackDelay(0);
			*f >> minDelay >> minStrength >> minMass;
            TRACE("HavokRagdoll::LoadRagdollCollisionGroups(): creating collision listener with a delay of %g\n", minDelay);
            m_setTag("Physics");
			Limbs[iLimb].CollisionListener = new havokCollisionListener( Limbs[iLimb].Body, minDelay, minStrength, minMass );
		}

		unsigned char cg;
		*f >> cg;
		if( cg != Limbs[iLimb].CollisionGroup )
		{
#ifdef HAVOK3
			Limbs[iLimb].Body->getCollidable()->setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( cg ) );
#else
			Limbs[iLimb].Body->getCollidable().setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( cg ) );
#endif
			Limbs[iLimb].CollisionGroup = cg;
			if( Limbs[iLimb].Body->getSimulationIsland() )
#ifdef HAVOK3
				// BLAIR - are these parameters right?  Only the Creator knows...
				mWorld->World->updateCollisionFilterOnEntity( Limbs[iLimb].Body, HK_UPDATE_FILTER_ON_ENTITY_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY, HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS );
#else
				mWorld->World->updateCollisionFilter( Limbs[iLimb].Body );
#endif
		}
	}

	return true;
}

bool HavokRagdoll::SaveRagdoll( FileObject* f )
{
	if( !f->PutToken( HAVOKRAGDOLL_LOAD_SAVE_TOKEN ) ) return false;

	// Get ragdoll data
	RagdollData data;
	FillRagdollData( data );

	*f << data.LimbPositions.Size();
	for( int i = 0; i < data.LimbPositions.Size(); ++i ) *f << data.LimbPositions[i];
	for( int i = 0; i < data.LimbPositions.Size(); ++i ) *f << data.LimbOrientations[i];
	for( int i = 0; i < data.LimbPositions.Size(); ++i ) *f << data.LimbLinearVelocities[i];
	for( int i = 0; i < data.LimbPositions.Size(); ++i ) *f << data.LimbAngularVelocities[i];
	for( int i = 0; i < data.LimbPositions.Size(); ++i ) *f << (char)data.LimbPinned[i];

	char test;
	float minDelay, minStrength, minMass;
	for( int i = 0; i < data.LimbPositions.Size(); ++i )
	{
		test = ( Limbs[i].CollisionListener != 0 )?1:0;
		*f << test;
		if( test )
		{
			Limbs[i].CollisionListener->getInfo( minDelay, minStrength, minMass );
			*f << minDelay << minStrength << minMass;
		}

		// Save limb mass
		float mass = Limbs[i].Body->getMass();
		*f << mass;

		// Save limb collision group
		unsigned char cg = Limbs[i].CollisionGroup;
		*f << cg;

		// Save friction and restitution
		float friction, restitution;
		friction = Limbs[i].Body->getMaterial().getFriction();
		restitution = Limbs[i].Body->getMaterial().getRestitution();
		*f << friction << restitution;

		// Save limb inertia tensor
		hkMatrix3 inertiaTensor;
		Limbs[i].Body->getInertiaLocal( inertiaTensor );
		hkVector4 row0, row1, row2;
		inertiaTensor.getRows( row0, row1, row2 );
		float x,y,z;
		x = row0(0); y = row0(1); z = row0(2);
		*f << x << y << z;
		x = row1(0); y = row1(1); z = row1(2);
		*f << x << y << z;
		x = row2(0); y = row2(1); z = row2(2);
		*f << x << y << z;

		// Save limb active state
		char active = Limbs[i].Body->isActive()?1:0;
		*f << active;
	}

	// Save dampings
	*f << data.LinearDamping << data.AngularDamping;
	*f << mInertiaTensorMultiplier;

	// Save constraint pivots
	for( int iConstraint = 0; iConstraint < Constraints.Size(); ++iConstraint )
	{
		SaveConstraintPivots( f, Constraints[iConstraint] );
		char inWorld = ConstraintInWorld(iConstraint)?1:0;
		*f << inWorld;
	}

	// Save action pivots
	for( int iAction = 0; iAction < Actions.Size(); ++iAction )
	{
		SaveActionPivots( f, Actions[iAction] );
		char inWorld = ActionInWorld(iAction)?1:0;
		*f << inWorld;
	}

	*f << Flags;

	// Antigravity
	Vector gravity = GetGravity();
	*f << gravity;

	return true;
}

bool HavokRagdoll::SaveRagdollCollisionGroups( FileObject* f )
{
	if( !f->PutToken( HAVOKRAGDOLL_LOAD_SAVE_TOKEN ) ) return false;

	*f << Limbs.Size();

	char test;
	float minDelay, minStrength, minMass;
	for( int iLimb = 0; iLimb < Limbs.Size(); iLimb++ )
	{
		test = ( Limbs[iLimb].CollisionListener != 0 )?1:0;
		*f << test;
		if( test )
		{
			Limbs[iLimb].CollisionListener->getInfo( minDelay, minStrength, minMass );
			*f << minDelay << minStrength << minMass;
		}

		unsigned char cg = Limbs[iLimb].CollisionGroup;
		*f << cg;
	}

	return true;
}

void HavokRagdoll::SelfExplosion( Vector center, float strength, float range, int ownerID, int weapon_type, float weapon_damage, int explosionCount, hkRigidBody* explosionBody )
{
#ifndef HAVOK3 // ===========================================================
	float maxDamage = 0.f;

	bool hadBody = ( explosionBody != 0 );
	if( !hadBody )
	{
		// Make a zero-radius sphere
        m_setTag("Physics");
		hkSphereShape* sphereShape = new hkSphereShape( 0.01f );
#ifdef HAVOK3
		hkRigidBodyCinfo sphereInfo;
#else
		hkRigidBodyConstructionInfo sphereInfo;
#endif
		sphereInfo.m_shape = sphereShape;
		sphereInfo.m_position.set( center.X, center.Y, center.Z );
#ifdef HAVOK3
		sphereInfo.m_motionType = hkMotion::MOTION_FIXED;
#else
		sphereInfo.m_motionType = HK_FIXED_MOTION;
#endif
		sphereInfo.m_collisionFilterInfo = hkGroupFilter::calcFilterInfo( HCGNoncolliding );
		sphereInfo.m_mass = 10.0f;	// just in case
        m_setTag("Physics");
		explosionBody = new hkRigidBody( sphereInfo );
		sphereShape->removeReference();
		mWorld->World->addEntity( explosionBody );
	}

	hkCollidable& explosionColl = explosionBody->getCollidable();
	hkCollisionEnvironment* env = mWorld->World->getCollisionEnvironment();

	int limbsSize = Limbs.Size();
	hkVector4 imp(0,0,0);

	if( (Flags & HavokRagdoll::Active) && (Flags & HavokRagdoll::IsMovedByExplosions) )
	{
		hkVector4 havokCenter( center.X, center.Y, center.Z );
		for( int iLimb = 0; iLimb < limbsSize; ++iLimb )
		{
			Limb &limb = Limbs[iLimb];

#ifdef HAVOK3
			if( limb.Body->getMotionType() == hkMotion::MOTION_KEYFRAMED )
#else
			if( limb.Body->isKeyframed() )
#endif
				continue;

			hkVector4 meshCenter = limb.Body->getCenterOfMassInWorld();
			hkVector4 distanceV( meshCenter );
			distanceV.sub4( havokCenter );
			float length = distanceV.length3();

#ifdef HAVOK_POSITION_BUG_TRACKING
			float myMass = limb.Body->getMass();
			if( myMass < 0.000001f )
			{
				Assert( myMass >= 0.0f );
				Entity* enti = (Entity*)mModel;
				GLog.Print( "Zero mass in Ragdoll:SelfExplosion ( entity '%s, filename '%s' )\n", enti?enti->Name.AsChar():"(null)", enti?enti->FileName.AsChar():"(null)" );
			}
#endif

			if( hadBody && ( length > 50 ) ) continue;	// this will only trigger when called from Explosion()

			hkVector4 newMeshCenter( meshCenter );

			if( length < 3 * range )
			{
				float tol = env->m_tolerance;
				env->m_tolerance = hadBody?50:3*range;
				hkCollidable& limbColl = limb.Body->getCollidable();
                m_setTag("Physics");
				hkSimpleClosestPointsDetails* details = new hkSimpleClosestPointsDetails( explosionColl, limbColl, *env );
				details->updateClosestPoints(*env);
				length = details->getClosestDistance();

				int index = details->getClosestIndex();
				if( index != -1 )
				{
					const hkContactPoint pt = details->getContactPoints()[index];
					newMeshCenter = pt.getPosition();
					if( length < 0 )
					{
						distanceV = havokCenter;
						distanceV.sub4( newMeshCenter );
					}
					else
					{
						distanceV = newMeshCenter;
						distanceV.sub4( havokCenter );
					}
					length = distanceV.length3();
				}

				details->destroy( *env );
				env->m_tolerance = tol;
			}

			if( length < range )
			{
				float factor = 1-(length/range);
				float pow = strength * factor / (float)limbsSize;
				float damage = weapon_damage * sinf(factor*F_PI/2.0f);
				if(length > 0.0001)
				{
					distanceV.normalize3();
					hkVector4 imp( distanceV );
					imp.mul4(pow);

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
					if( !imp.isOk4() )
#else
					if( !imp.isOk() )
#endif
						GLog.Print( "Corrupted force in SelfExplosion\n" );
#ifdef HAVOK3
					if( !newMeshCenter.isOk4() )
#else
					if( !newMeshCenter.isOk() )
#endif
						GLog.Print( "Corrupted point in SelfExplosion\n" );
					else if( newMeshCenter.length3() > 10000.0f )
						GLog.Print( "Point pos > 10000 in SelfExplosion\n" );
#endif

					if( pow > 10000.0f )
						pow = 10000.0f;

					if( limb.Body->getSimulationIsland() )
					{
#ifdef HAVOK3
						limb.Body->activate();
#else
						limb.Body->forceActivate();
#endif
						if( newMeshCenter.equals3(meshCenter) )
							limb.Body->applyLinearImpulse( imp );
						else
							limb.Body->applyPointImpulse( imp, newMeshCenter );
					}
				}
				if( maxDamage < damage )
					maxDamage = damage;
			}
		}
	}

	if( maxDamage )
	{
		GScript.QueueMessage("EXPLOSION","%e,%f,%f,%f,%d,%d,%d,%f", mModel,
			center.X,center.Y,center.Z,
			explosionCount,
			ownerID, weapon_type, maxDamage );
	}

	if( !hadBody )
	{
		mWorld->World->removeEntity( explosionBody );
		explosionBody->removeReference();
		explosionBody = 0;
	}

#endif // ifndef =====================================================

}

// *********************************************************************************************************************

HavokRagdollTemplate::HavokRagdollTemplate( String model, String ragdoll ) : ModelFile( model ), RagdollFile( ragdoll ) {};

HavokRagdollTemplate::~HavokRagdollTemplate( void )
{
	RigidBodies.ClearArray();

	for( int i = 0; i < Constraints.Size(); ++i )
		switch( Constraints[i].Type )
		{
#ifdef HAVOK3
		case 0:	delete ((hkRagdollConstraintData*)Constraints[i].ConstraintInfo); break;
		case 1:	delete ((hkLimitedHingeConstraintData*)Constraints[i].ConstraintInfo); break;
		case 2:	delete ((hkHingeConstraintData*)Constraints[i].ConstraintInfo); break;
		case 3:	delete Constraints[i].Gen; Constraints[i].Gen = 0; break;
		case 4:	delete ((hkBallAndSocketConstraintData*)Constraints[i].ConstraintInfo); break;
		case 5:	delete ((hkStiffSpringConstraintData*)Constraints[i].ConstraintInfo); break;
		case 6:	delete ((hkPrismaticConstraintData*)Constraints[i].ConstraintInfo); break;
		case 7:	delete ((hkWheelConstraintData*)Constraints[i].ConstraintInfo); break;
#else
		case 0:	delete ((hkRagdollConstructionInfo*)Constraints[i].ConstraintInfo); break;
		case 1:	delete ((hkLimitedHingeConstructionInfo*)Constraints[i].ConstraintInfo); break;
		case 2:	delete ((hkHingeConstructionInfo*)Constraints[i].ConstraintInfo); break;
		case 3:	delete Constraints[i].Gen; Constraints[i].Gen = 0; break;
		case 4:	delete ((hkBallAndSocketConstructionInfo*)Constraints[i].ConstraintInfo); break;
		case 5:	delete ((hkStiffSpringConstructionInfo*)Constraints[i].ConstraintInfo); break;
		case 6:	delete ((hkPrismaticConstructionInfo*)Constraints[i].ConstraintInfo); break;
		case 7:	delete ((hkWheelConstructionInfo*)Constraints[i].ConstraintInfo); break;
#endif
		}
	Constraints.ClearArray();
	Actions.ClearArray();
}
