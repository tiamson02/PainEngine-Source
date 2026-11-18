
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "Engine/InputSystem.h"

#include "Physics/Havok/HavokPhysics.h"
#include "Physics/Havok/POHavokPart.h"
#include "Physics/Havok/HavokWorld.h"
#include "Physics/Havok/HavokCharacterController.h"
#include "Physics/Havok/HavokMissileController.h"
#include "Physics/PhysicsEngine.h"

#include "Net/NetworkDevice2.h"
#include "Physics/PhysicsWorld.h"
#define NPROF(x)
#define NPROF2(x,y)

#include "Engine/Model.h"
#include "Engine/World.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#define MAX_PO_ANGULAR_VELOCITY 30

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#include "PlayerMove.inl"

#define PLAYER_HEAD_POS_Y 0.0f
#define ROTATION_STRENGTH_MODIFIER 10.0f

#define NEW_MISSILES

// -------------------------------------------------------------------------------------------------------

PhysicsObjectPlayerVariables::PhysicsObjectPlayerVariables( void ) : AccumulatedDelta(0.0f), TimeSinceJumped(0.0f), TimeSinceLanded(0.0f),
		JumpVector(0,0,0), JumpAction(0), FakeBulletTimeGravity(0),
		TimeSinceExplosion(10.0f), LastExecutionSpeed(0,0,0), JumpPressed( false ),
		TimeSinceLastOnGround(0), SpeedWhenLanded(0), AtSteps(false), Falling(true), CanBunnyHop(false),
		DoubleJumped(false), Speed(0.0f), RealTimeSinceJumped(1.0f), TimeSinceBunnyHop(0.0f), OnSlopeTimes(0),
		HighestSpeedOfGoingOut(-100.0), FramesInRocksAndNotGoingOut(0),	TimeModifierLastTime(1.0f), Exploded(false),
		JumpInvalidation(false), LastNonzeroAction(1<<EInputMoveForward), TimeToBeShocked(0.0f), LowestYSpeed(0.0f),
		StickToUnderBody(false), StickToUnderBodyPending(false), UnderBody(0), StickPosDiff(0,0,0),
		RevertedToLastGoodPositionLastFrame(false), BulletTimeShock(false), Controller(0)
{
#ifdef	TRANSGAMING
	LastPlayerMovementFixFrame = (GEngine && GEngine->pcfWorld)?GEngine->pcfWorld->Frame:0;
#else
	LastPlayerMovementFixFrame = (GEngine && GEngine->World)?GEngine->World->Frame:0;
#endif
}

PhysicsObjectPlayerVariables::~PhysicsObjectPlayerVariables( void )
{
	if( Controller )
	{
		delete Controller;
		Controller = 0;
	}
}
// -------------------------------------------------------------------------------------------------------

PhysicsObject::PhysicsObject( void ) :
		State(0), Action(EInputNone), mHowFarICanSee( 40.0f ), BodyType( 0 ), BodyScale( 1.0f ),
		mModelPivotOffset( 0.0f, 0.0f, 0.0f ), mFreedomOfRotation( ERotAllAxes ), mMoveVector(0,0,0),
		mHowFarICanSee360( 1.0f ), mViewAngleYaw( F_PI/2 ), mViewAnglePitch( F_PI ), mVolumeMass( -1.0f ),
		mPOTableIndex(-1), ForcesToApply(0), mAngularInTick(0.0f), mPlayerVariables(0),
		havokInfluenceInMonsterMovement( 0.95f ), doNotCheckFloors( false ), prevVel(0,0,0),
		floorNormal(0,1,0), mInertiaTensorMultiplier(1.0f), mMissileController(0)
{
    m_setTag("Physics");
	mHavokPart = new POHavokPart;
}

PhysicsObject::~PhysicsObject( void )
{
	if( mMissileController )
		delete mMissileController;

	RemoveForces();

	// Proper removal of 'phys' meshes
	int* ind = mHavokPart->World->PhysIndices.Find( this );
	if( ind )
	{
		mHavokPart->World->PhysIndices.Remove( this );
		mHavokPart->World->PhysArray[*ind] = 0;
	}

	State &= ~EPOCollisionCB;
	if( mHavokPart->CollisionListener )
	{
		mHavokPart->Body->removeCollisionListener( mHavokPart->CollisionListener );
		delete mHavokPart->CollisionListener;
		mHavokPart->CollisionListener = 0;
	}

	mHavokPart->RemoveAllActionsForBodyDelete();

	if( mHavokPart->Body )
	{
		if( State & EPOHasOurDeactivator )
			mHavokPart->World->mDeactivator->RemoveRigidBody( mHavokPart->Body );

		// To disable Havok memory leak
#ifdef HAVOK3
		if( mHavokPart->Body->getMotionType() == hkMotion::MOTION_KEYFRAMED )
			mHavokPart->Body->setMotionType( hkMotion::MOTION_FIXED );
#else
		if( mHavokPart->Body->isKeyframed() )
			mHavokPart->Body->setKeyframed( false );
#endif

		if( mHavokPart->Body->getSimulationIsland() )
			mHavokPart->World->World->removeEntity( mHavokPart->Body );
		mHavokPart->Body->removeReference();
		mHavokPart->Body = 0;
	}

	if( mHavokPart->StepCheckPhantom )
	{
		mHavokPart->StepCheckPhantom->removeReference();
		mHavokPart->StepCheckPhantom = 0;
	}

	if( mPOTableIndex != -1 )
	{
		mHavokPart->World->PhysicsObjects.FastRemove( mPOTableIndex );
		if( mPOTableIndex < mHavokPart->World->PhysicsObjects.Size() )
			mHavokPart->World->PhysicsObjects[mPOTableIndex]->mPOTableIndex = mPOTableIndex;
		mPOTableIndex = -1;
	}

	if( mPlayerVariables && mPlayerVariables->FakeBulletTimeGravity )
	{
		havokAntiGravityAction* grav = (havokAntiGravityAction*)mPlayerVariables->FakeBulletTimeGravity;
		if( grav->getSimulationIsland() )
			mHavokPart->World->World->removeAction( grav );
		grav->removeReference();
		mPlayerVariables->FakeBulletTimeGravity = 0;
	}

	delete mHavokPart;
	mHavokPart = 0;

	if( mPlayerVariables )
	{
		if( mPlayerVariables->UnderBody )
			((hkRigidBody*)mPlayerVariables->UnderBody)->addReference();
		delete mPlayerVariables;
	}
	mPlayerVariables = 0;
}

void PhysicsObject::SetMPMissileController( int type )
{
#ifdef NEW_MISSILES
	if( mMissileController )
		delete mMissileController;
	if( !mPlayerVariables ){
        m_setTag("Physics");
		mMissileController = new HavokMissileController( mHavokPart->World->World, mHavokPart->Body, BodyScale, type );
    }
	else
		mMissileController = 0;
#endif
}

void PhysicsObject::EffectForce( Vector force, const Vector* point )
{
#ifdef HAVOK3
	if( !mHavokPart->Body || mHavokPart->Body->getMotionType() == hkMotion::MOTION_KEYFRAMED ) return;
#else
	if( !mHavokPart->Body || mHavokPart->Body->isKeyframed() ) return;
#endif

	float mass = GetMass();
#ifdef HAVOK_POSITION_BUG_TRACKING
	if( mass < 0.000001f )
	{
		Entity* entity = GetEntity();
		GLog.Print( "Zero mass in EffectForce ( entity '%s, filename '%s' )\n", entity?entity->Name.AsChar():"(null)", entity?entity->FileName.AsChar():"(null)" );
	}
#endif
	float maxForcePerTick = (mass>80.0f)?MAX_FORCE_PER_TICK*GetMass()/80.0f:MAX_FORCE_PER_TICK;
	if( mPlayerVariables ) maxForcePerTick /= GEngine->TimeMultiplier;

	if( force.MagnitudeSquared() <= maxForcePerTick*maxForcePerTick )
	{
		hkVector4 fc( force.X, force.Y, force.Z );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !fc.isOk4() )
#else
		if( !fc.isOk() )
#endif
			GLog.Print( "Corrupted force in EffectForce\n" );
		else if( fc.length3() > 50000.0f )
			GLog.Print( "Force > 50000 in EffectForce\n" );
#endif

		if( point )
		{
			hkVector4 pt( point->X, point->Y, point->Z );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !pt.isOk4() )
#else
			if( !pt.isOk() )
#endif
				GLog.Print( "Corrupted point in EffectForce\n" );
			else if( pt.length3() > 10000.0f )
				GLog.Print( "Point pos > 10000 in EffectForce\n" );
#endif
			mHavokPart->Body->applyPointImpulse( fc, pt );
		}
		else
			mHavokPart->Body->applyLinearImpulse( fc );
	}
	else
	{
		float forceMagnitude = force.Magnitude();
		float ourForceMagnitude = ( forceMagnitude > maxForcePerTick*MAX_FORCE_TICKS )?(maxForcePerTick*MAX_FORCE_TICKS):forceMagnitude;
		//GLog.Print( "PO Warning: force = %f ( really %f ) > %f, dividing into multiple ticks\n", ourForceMagnitude, forceMagnitude, maxForcePerTick );
        m_setTag("Physics");
		POForce* pof = new POForce;

		pof->ForceDir = force;
		pof->ForceDir.Normalize();
		pof->HavePoint = ( point != 0 );
		if( point ) pof->ForcePoint = *point;
		pof->ForceLeft = ourForceMagnitude - maxForcePerTick;
		Assert( pof->ForceLeft > 0.0f );

		hkVector4 fc( pof->ForceDir.X, pof->ForceDir.Y, pof->ForceDir.Z );
		fc.mul4( maxForcePerTick );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !fc.isOk4() )
#else
			if( !fc.isOk() )
#endif
				GLog.Print( "Corrupted force in EffectForce\n" );
			else if( fc.length3() > 50000.0f )
				GLog.Print( "Force > 50000 in EffectForce\n" );
#endif

		if( point )
		{
			hkVector4 pt( point->X, point->Y, point->Z );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !pt.isOk4() )
#else
			if( !pt.isOk() )
#endif
				GLog.Print( "Corrupted point in EffectForce\n" );
			else if( pt.length3() > 10000.0f )
				GLog.Print( "Point pos > 10000 in EffectForce\n" );
#endif
			mHavokPart->Body->applyPointImpulse( fc, pt );
		}
		else
			mHavokPart->Body->applyLinearImpulse( fc );

		pof->NextForce = ForcesToApply;
		ForcesToApply = pof;
	}
}

void PhysicsObject::RemoveForces( void )
{
	// Dispose of forces data
	while( ForcesToApply )
	{
		POForce* ftoa = ForcesToApply;
		ForcesToApply = ForcesToApply->NextForce;
		delete ftoa;
	}
}

void PhysicsObject::EffectForces( void )
{
	if( !ForcesToApply ) return;
#ifdef HAVOK3
	if( !mHavokPart->Body || mHavokPart->Body->getMotionType() == hkMotion::MOTION_KEYFRAMED )
#else
	if( !mHavokPart->Body || mHavokPart->Body->isKeyframed() )
#endif
	{
		RemoveForces();
		return;
	}

	float mass = GetMass();
#ifdef HAVOK_POSITION_BUG_TRACKING
	if( mass < 0.000001f )
	{
		Entity* entity = GetEntity();
		GLog.Print( "Zero mass in EffectForces ( entity '%s, filename '%s' )\n", entity?entity->Name.AsChar():"(null)", entity?entity->FileName.AsChar():"(null)" );
	}
#endif
	float maxForcePerTick = (mass>80.0f)?MAX_FORCE_PER_TICK*GetMass()/80.0f:MAX_FORCE_PER_TICK;
	if( mPlayerVariables ) maxForcePerTick /= GEngine->TimeMultiplier;

	for( POForce* pof = ForcesToApply, *pof2 = NULL; pof;  )
	{
		if( pof->ForceLeft <= maxForcePerTick )
		{
			hkVector4 fc( pof->ForceDir.X, pof->ForceDir.Y, pof->ForceDir.Z );
			fc.mul4( pof->ForceLeft );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !fc.isOk4() )
#else
			if( !fc.isOk() )
#endif
				GLog.Print( "Corrupted force in EffectForces\n" );
			else if( fc.length3() > 50000.0f )
				GLog.Print( "Force > 50000 in EffectForces\n" );
#endif

			if( pof->HavePoint )
			{
				hkVector4 pt( pof->ForcePoint.X, pof->ForcePoint.Y, pof->ForcePoint.Z );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
				if( !pt.isOk4() )
#else
				if( !pt.isOk() )
#endif
					GLog.Print( "Corrupted point in EffectForces\n" );
				else if( pt.length3() > 10000.0f )
					GLog.Print( "Point pos > 10000 in EffectForces\n" );
#endif
				mHavokPart->Body->applyPointImpulse( fc, pt );
			}
			else
				mHavokPart->Body->applyLinearImpulse( fc );

			if( pof2 ) pof2->NextForce = pof->NextForce; else ForcesToApply = pof->NextForce;
			delete pof;
			pof = pof2?pof2->NextForce:ForcesToApply;
		}
		else
		{
			hkVector4 fc( pof->ForceDir.X, pof->ForceDir.Y, pof->ForceDir.Z );
			fc.mul4( maxForcePerTick );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !fc.isOk4() )
#else
			if( !fc.isOk() )
#endif
				GLog.Print( "Corrupted force in EffectForces\n" );
			else if( fc.length3() > 50000.0f )
				GLog.Print( "Force > 50000 in EffectForces\n" );
#endif

			if( pof->HavePoint )
			{
				hkVector4 pt( pof->ForcePoint.X, pof->ForcePoint.Y, pof->ForcePoint.Z );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
				if( !pt.isOk4() )
#else
				if( !pt.isOk() )
#endif
					GLog.Print( "Corrupted point in EffectForces\n" );
				else if( pt.length3() > 10000.0f )
					GLog.Print( "Point pos > 10000 in EffectForces\n" );
#endif
				mHavokPart->Body->applyPointImpulse( fc, pt );
			}
			else
				mHavokPart->Body->applyLinearImpulse( fc );

			pof->ForceLeft -= maxForcePerTick;
			pof2 = pof;
			pof = pof->NextForce;
		}
	}
}

Vector PhysicsObject::GetPivotOffset( void ) const
{
	hkQuaternion q = mHavokPart->Body->getRotation();
	hkVector4 q1 = q.getImag();
	Quaternion quat( -q.getReal(), q1(0), q1(1), q1(2) );

	return quat * mModelPivotOffset; // rotate pivot offset (should be optimized?)
}

Vector PhysicsObject::GetForwardVec( void ) const
{
	if( mHavokPart->Body )
	{
		const hkQuaternion& rotation = mHavokPart->Body->getRotation();
		hkVector4 endRot1( 0,0,1 );
		hkVector4 endRot2;
		endRot2.setRotatedDir( rotation, endRot1 );
		Vector endRot( endRot2(0), endRot2(1), endRot2(2) );	// can't guess why '-' :((
		endRot.Normalize();
		return endRot;
    }

    return Vector(0,0,0);
}

Vector PhysicsObject::GetRightVec( void ) const
{
	if( mHavokPart->Body )
	{
		const hkQuaternion& rotation = mHavokPart->Body->getRotation();
		hkVector4 endRot1( 1,0,0 );
		hkVector4 endRot2;
		endRot2.setRotatedDir( rotation, endRot1 );
		Vector endRot( endRot2(0), endRot2(1), endRot2(2) );	// can't guess why '-' :((
		endRot.Normalize();
        return endRot;
    }

    return Vector(0,0,0);
}

void PhysicsObject::SetRotQuaternion( const Quaternion& quat )
{
	if( mHavokPart->Body )
	{
		hkQuaternion q( quat.X, quat.Y, quat.Z, -quat.W);
#ifdef HAVOK_POSITION_BUG_TRACKING
		if( !q.isOk() )
			GLog.Print( "Bad quaternion given to PO in SetRotQuaternion\n" );
#endif
		mHavokPart->Body->setRotation( q );
    }
}

Quaternion PhysicsObject::GetRotQuaternion( void )
{
	if( mHavokPart->Body )
	{
		hkReal r = mHavokPart->Body->getRotation().getReal();
		hkVector4 q = mHavokPart->Body->getRotation().getImag();
		return Quaternion( -r, q(0), q(1), q(2) );
    }

	return Quaternion(1,0,0,0);
}

void PhysicsObject::SetPawnHeadPos( const Vector& pos )
{
	if( mHavokPart->Body )
	{
		hkVector4 temp( pos.X, pos.Y-(4.5f+delta+PLAYER_HEAD_POS_Y)*BodyScale*OLD_PAWN_SCALE, pos.Z );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !temp.isOk4() || ( fabs(temp(0)) > 10000.0f ) || ( fabs(temp(1)) > 10000.0f ) || ( fabs(temp(2)) > 10000.0f ) )
#else
		if( !temp.isOk() || ( fabs(temp(0)) > 10000.0f ) || ( fabs(temp(1)) > 10000.0f ) || ( fabs(temp(2)) > 10000.0f ) )
#endif
			GLog.Print( "Position > 10000 in SetPawnHeadPos\n" );
#endif
		mHavokPart->Body->setPosition( temp );

		if( mPlayerVariables )
		{
			mPlayerVariables->LastGoodPosition.X = temp(0);
			mPlayerVariables->LastGoodPosition.Y = temp(1);
			mPlayerVariables->LastGoodPosition.Z = temp(2);
			mPlayerVariables->LastGoodPosition += GetPivotOffset();
			if( mPlayerVariables->Controller )
				mPlayerVariables->Controller->setPosition( temp );
		}
	}
}

void PhysicsObject::SetPosition( const Vector& pos, bool updateSave )
{

	if( mHavokPart->Body )
	{
		Vector PivotOffset = GetPivotOffset();
		hkVector4 temp( pos.X - PivotOffset.X, pos.Y - PivotOffset.Y, pos.Z - PivotOffset.Z );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !temp.isOk4() )
#else
		if( !temp.isOk() )
#endif
			GLog.Print( "Bad position in SetPosition\n" );
		else if( ( fabs(temp(0)) > 10000.0f ) || ( fabs(temp(1)) > 10000.0f ) || ( fabs(temp(2)) > 10000.0f ) )
			GLog.Print( "Position > 10000 in SetPosition\n" );
#endif
		if( mMissileController )
			mMissileController->setPosition( temp );
		mHavokPart->Body->setPosition( temp );

		if( mPlayerVariables )
		{
			mPlayerVariables->LastGoodPosition = pos;
			if( mPlayerVariables->Controller )
				mPlayerVariables->Controller->setPosition( temp );
		}
	}
}

void PhysicsObject::SetPositionAndRotation( const Vector& pos, const Quaternion& quat )
{
	if( mHavokPart->Body )
	{
		hkQuaternion q( quat.X, quat.Y, quat.Z, -quat.W);
#ifdef HAVOK_POSITION_BUG_TRACKING
		if( !q.isOk() )
			GLog.Print( "Bad quaternion given to PO in SetPositionAndRotation\n" );
#endif
		mHavokPart->Body->setRotation( q );
		Vector PivotOffset = GetPivotOffset();
		hkVector4 temp( pos.X - PivotOffset.X, pos.Y - PivotOffset.Y, pos.Z - PivotOffset.Z );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !temp.isOk4() || ( fabs(temp(0)) > 10000.0f ) || ( fabs(temp(1)) > 10000.0f ) || ( fabs(temp(2)) > 10000.0f ) )
#else
		if( !temp.isOk() || ( fabs(temp(0)) > 10000.0f ) || ( fabs(temp(1)) > 10000.0f ) || ( fabs(temp(2)) > 10000.0f ) )
#endif
			GLog.Print( "Position > 10000 in SetPositionAndRotation\n" );
#endif
		mHavokPart->Body->setPosition( temp );
		if( mPlayerVariables )
			mPlayerVariables->LastGoodPosition = pos;
	}
}

void PhysicsObject::ZeroVelocity( void )
{
	if( mHavokPart->Body )
	{
		hkVector4 temp( 0,0,0 );
		mHavokPart->Body->setLinearVelocity( temp );
	}
}

void PhysicsObject::ZeroAngularVelocity( void )
{
	if( mHavokPart->Body )
	{
		hkVector4 temp( 0,0,0 );
		mHavokPart->Body->setAngularVelocity( temp );
	}
}

bool PhysicsObject::FloorCheckMP( float down )
{
	return ( mPlayerVariables && mPlayerVariables->Controller )?mPlayerVariables->Controller->floorCheckShort( down ):false;
}

bool PhysicsObject::FloorCheck(float distance1, float distance2, float* hitAt, bool fixedGeomOnly, bool takeFromController )
{
	if( takeFromController && mPlayerVariables && mPlayerVariables->Controller )
		return mPlayerVariables->Controller->isOnGround();

	bool canCollide = CanLineTraceCollision();
	if( canCollide )
		EnableLineTraceCollision( false );

	hkVector4 start;
	start = mHavokPart->Body->getPosition();
	hkVector4 end( start );
	hkVector4 chwilowy( 0,distance2*BodyScale*OLD_PAWN_SCALE,0 );
	start.add4( chwilowy );
	chwilowy.set( 0,distance1*BodyScale*OLD_PAWN_SCALE,0 );
	end.add4( chwilowy );
#ifdef HAVOK3
	// Set the values for the ray that we will be casting...
	hkWorldRayCastInput ray;
	{
		ray.m_from = start;
		ray.m_to = end;
	}

	hkWorldRayCastOutput output;
	{
		havokFloorCheckRaycastCallback rayCallback( ray, &output );
		// Get the broadphase...
		hkBroadPhase* broadPhase = mHavokPart->World->World->getBroadPhase();
		hkBroadPhase::hkCastRayInput inputRay;
		inputRay.m_from = ray.m_from;
		inputRay.m_toBase = &ray.m_to;
		//inputRay.m_aabbCacheInfo = HK_NULL;

		// Now cast the ray!
		broadPhase->castRay( inputRay, &rayCallback, hkSizeOf(havokFloorCheckRaycastCallback) );
	}

	if ( output.hasHit() )
	{
		hkVector4 intersectionPointWorld;
		//intersectionPointWorld.setInterpolate4( ray.m_from, ray.m_to, output.m_hitFraction );
		if( hitAt ) *hitAt = output.m_hitFraction;
		hkVector4 hitNormal;
		hitNormal.setRotatedDir( ((hkRigidBody*)output.m_rootCollidable->getOwner())->getRotation(), output.m_normal );
		floorNormal = Vector( hitNormal(0), hitNormal(1), hitNormal(2) );
		//floorNormal.Normalize();
		if( canCollide )
			EnableLineTraceCollision( true );
		return true;
	}
#else
	hkBroadPhase* broadPhase = mHavokPart->World->World->getBroadPhase();
	hkShape::hkRayResults initResults;
	havokFloorCheckRaycastCallback rayCallBack( start, end, initResults );
	rayCallBack.fixedGeometryOnly = fixedGeomOnly;
	broadPhase->castPrimitive( start, end, rayCallBack );
	if( rayCallBack.getHitObject() != HK_NULL )
	{
		initResults = rayCallBack.getRayResults();

		if( hitAt ) *hitAt = initResults.m_mindist;
		hkVector4 hitNormal;
		hitNormal.setRotatedDir( ((hkRigidBody*)rayCallBack.getHitObject()->getOwner())->getRotation(), initResults.m_unnormalizedHitNormal );
		floorNormal = Vector( hitNormal(0), hitNormal(1), hitNormal(2) );
		floorNormal.Normalize();
		if( canCollide )
			EnableLineTraceCollision( true );
		return true;
	}
#endif
	if( canCollide )
		EnableLineTraceCollision( true );
	return false;
}

bool PhysicsObject::MonsterFloorCheck(float distance1, float distance2, float* hitAt, bool fixedGeomOnly )
{
//#ifndef HAVOK3 // ============================================

	bool canCollide = CanLineTraceCollision();
	if( canCollide )
		EnableLineTraceCollision( false );

	hkVector4 start;
	start = mHavokPart->Body->getPosition();
	hkVector4 end( start );
	hkVector4 chwilowy( 0,distance2*BodyScale*OLD_PAWN_SCALE,0 );
	start.add4( chwilowy );
	chwilowy.set( 0,distance1*BodyScale*OLD_PAWN_SCALE,0 );
	end.add4( chwilowy );

#ifdef HAVOK3
	hkWorldRayCastInput ray;
	{
		ray.m_from = start;
		ray.m_to = end;
	}

	hkWorldRayCastOutput output;
	{
		havokMonsterFloorCheckRaycastCallback rayCallback( ray, &output );
		rayCallback.fixedGeometryOnly = fixedGeomOnly;
		// Get the broadphase...
		hkBroadPhase* broadPhase = mHavokPart->World->World->getBroadPhase();
		hkBroadPhase::hkCastRayInput inputRay;
		inputRay.m_from = ray.m_from;
		inputRay.m_toBase = &ray.m_to;
		//inputRay.m_aabbCacheInfo = HK_NULL;
		// Now cast the ray!
		broadPhase->castRay( inputRay, &rayCallback, hkSizeOf(havokFloorCheckRaycastCallback) );
	}

	if( output.hasHit() )
	{
		if( hitAt ) 
			*hitAt = output.m_hitFraction;
		hkVector4 hitNormal;
		hitNormal.setRotatedDir( ((hkRigidBody*)output.m_rootCollidable->getOwner())->getRotation(), output.m_normal );
		floorNormal = Vector( hitNormal(0), hitNormal(1), hitNormal(2) );
		//floorNormal.Normalize();
		if( canCollide )
			EnableLineTraceCollision( true );
		return true;
	}
#else
	hkBroadPhase* broadPhase = mHavokPart->World->World->getBroadPhase();
	hkShape::hkRayResults initResults;

	/*
	GScript.DoString( "A1 = %f A2 = %f A3 = %f", start(0),start(1),start(2) );
	GScript.DoString( "A4 = %f A5 = %f A6 = %f", end(0),end(1),end(2) );
	*/
	havokMonsterFloorCheckRaycastCallback rayCallBack( start, end, initResults );
	rayCallBack.fixedGeometryOnly = fixedGeomOnly;
	broadPhase->castPrimitive( start, end, rayCallBack );
	if( rayCallBack.getHitObject() != HK_NULL )
	{
		initResults = rayCallBack.getRayResults();

		if( hitAt ) *hitAt = initResults.m_mindist;
		hkVector4 hitNormal;
		hitNormal.setRotatedDir( ((hkRigidBody*)rayCallBack.getHitObject()->getOwner())->getRotation(), initResults.m_unnormalizedHitNormal );
		floorNormal = Vector( hitNormal(0), hitNormal(1), hitNormal(2) );
		floorNormal.Normalize();
		if( canCollide )
			EnableLineTraceCollision( true );
		return true;
	}
#endif
	if( canCollide )
		EnableLineTraceCollision( true );
	return false;

//#else
//	GLog.Print("PhysicsObject::MonsterFloorCheck() - FIXME!!!!!!!!!!!\n");
//	return false;
//#endif // ifndef HAVOK3 =========================================
}

void* PhysicsObject::MeshUnder( float distance )
{
	bool canCollide = CanLineTraceCollision();
	if( canCollide )
		EnableLineTraceCollision( false );

	hkVector4 start;
	start = mHavokPart->Body->getPosition();
	hkVector4 end( start );
	hkVector4 chwilowy( 0,-4.5f*BodyScale*OLD_PAWN_SCALE,0 );
	start.add4( chwilowy );
	chwilowy.set( 0,-7.0f*BodyScale*OLD_PAWN_SCALE - distance,0 );
	end.add4( chwilowy );

#ifdef HAVOK3
	// Input Ray
	hkWorldRayCastInput ray;
	{
		ray.m_from = start;
		ray.m_to = end;
	}

	hkWorldRayCastOutput output;
	{
		havokFloorCheckRaycastCallback rayCallback( ray, &output );
		// Get the broadphase...
		hkBroadPhase* broadPhase = mHavokPart->World->World->getBroadPhase();
		hkBroadPhase::hkCastRayInput inputRay;
		inputRay.m_from = ray.m_from;
		inputRay.m_toBase = &ray.m_to;
		//inputRay.m_aabbCacheInfo = HK_NULL;

		// Now cast the ray!
		broadPhase->castRay( inputRay, &rayCallback, hkSizeOf(havokFloorCheckRaycastCallback) );
	}
	const hkCollidable* coll = output.m_rootCollidable;
#else
	hkBroadPhase* broadPhase = mHavokPart->World->World->getBroadPhase();
	hkShape::hkRayResults initResults;
	havokFloorCheckRaycastCallback rayCallBack( start, end, initResults );
	broadPhase->castPrimitive( start, end, rayCallBack );
	hkCollidable* coll = rayCallBack.getHitObject();
#endif
	void* hitBody = 0;
	if( coll )
		hitBody = coll->getOwner();
	if( canCollide )
		EnableLineTraceCollision( true );
	return hitBody;
}

bool PhysicsObject::FloorCheckRandom( float distance1, float distance2 )
{
	Entity* entity = GetEntity();
	bool hasRagdoll = entity && entity->AsModel() && ((Model*)entity)->RagdollPtr;
	bool canCollideRagdoll = false;
	if( hasRagdoll )
	{
		canCollideRagdoll = ((Model*)entity)->RagdollPtr->CanLineTraceCollision();
		if( canCollideRagdoll )
			((Model*)entity)->RagdollPtr->EnableLineTraceCollision(false);
	}

	bool canCollide = CanLineTraceCollision();
	if( canCollide )
		EnableLineTraceCollision( false );

	float xDiff = (((double)rand())/RAND_MAX-0.5)*4;
	float zDiff = (((double)rand())/RAND_MAX-0.5)*4;

	hkVector4 start;
	start = mHavokPart->Body->getPosition();
	hkVector4 end( start );
	hkVector4 chwilowy( xDiff, distance2, zDiff );
	chwilowy.mul4( BodyScale*OLD_PAWN_SCALE );
	start.add4( chwilowy );
	chwilowy.set( xDiff, distance1, zDiff );
	chwilowy.mul4( BodyScale*OLD_PAWN_SCALE );
	end.add4( chwilowy );

#ifdef HAVOK3
	// Set the values for the ray that we will be casting...
	hkWorldRayCastInput ray;
	{
		ray.m_from = start;
		ray.m_to = end;
	}

	hkWorldRayCastOutput output;
	{
		havokFloorCheckRaycastCallback rayCallback( ray, &output );
		// Get the broadphase...
		hkBroadPhase* broadPhase = mHavokPart->World->World->getBroadPhase();
		hkBroadPhase::hkCastRayInput inputRay;
		inputRay.m_from = ray.m_from;
		inputRay.m_toBase = &ray.m_to;
		//inputRay.m_aabbCacheInfo = HK_NULL;

		// Now cast the ray!
		broadPhase->castRay( inputRay, &rayCallback, hkSizeOf(havokFloorCheckRaycastCallback) );
	}
	bool hit = output.hasHit();
	if ( hit )
	{
		//hkVector4 intersectionPointWorld;
		//intersectionPointWorld.setInterpolate4( ray.m_from, ray.m_to, output.m_hitFraction );
		hkVector4 hitNormal;
		hitNormal.setRotatedDir( ((hkRigidBody*)output.m_rootCollidable->getOwner())->getRotation(), output.m_normal );
		floorNormal = Vector( hitNormal(0), hitNormal(1), hitNormal(2) );
		//floorNormal.Normalize();
	}
#else
	hkBroadPhase* broadPhase = mHavokPart->World->World->getBroadPhase();
	hkShape::hkRayResults initResults;
	havokFloorCheckRaycastCallback rayCallBack( start, end, initResults );
	broadPhase->castPrimitive( start, end, rayCallBack );

	bool hit = ( rayCallBack.getHitObject() != HK_NULL );
	if( hit )
	{
		initResults = rayCallBack.getRayResults();
		hkVector4 hitNormal;
		hitNormal.setRotatedDir( ((hkRigidBody*)rayCallBack.getHitObject()->getOwner())->getRotation(), initResults.m_unnormalizedHitNormal );
		floorNormal = Vector( hitNormal(0), hitNormal(1), hitNormal(2) );
		floorNormal.Normalize();
	}
#endif
	if( canCollide ) EnableLineTraceCollision( true );
	if( hasRagdoll && canCollideRagdoll ) ((Model*)entity)->RagdollPtr->EnableLineTraceCollision(true);
	return hit;
}

Vector PhysicsObject::GetPawnHeadPos( void ) const
{
	if( mHavokPart->Body )
	{
		const hkVector4& pos = mHavokPart->Body->getPosition();
		return Vector( pos(0), pos(1)+(4.5f+delta+PLAYER_HEAD_POS_Y)*BodyScale*OLD_PAWN_SCALE, pos(2) );
    }

    return Vector(0,0,0);
}

Vector PhysicsObject::GetPawnFloorPos( void ) const
{
	if( mHavokPart->Body )
	{
		const hkVector4& pos = mHavokPart->Body->getPosition();
		return Vector( pos(0), pos(1)-(5.5f-delta)*BodyScale*OLD_PAWN_SCALE, pos(2) );
    }

    return Vector(0,0,0);
}

void PhysicsObject::AddRotateActor( const Vector& hitPoint, const Vector& hitVector )
{
	if( mHavokPart->Body )
	{
		hkVector4 ray( hitPoint.X, 0, hitPoint.Z );
		ray.sub4( mHavokPart->Body->getCenterOfMassInWorld() );
		ray(1) = 0;
		hkVector4 cross;
		hkVector4 hv( hitVector.X, 0, hitVector.Z );
		cross.setCross( ray, hv );
		cross.mul4( 0.03f );
		mAngularInTick += cross(1);
	}
}

void PhysicsObject::EffectRotateActor( void )
{
	if( mHavokPart->Body )
	{
//		GLog.Print( "Real ang vel effected: %f\n", mAngularInTick );
		if( ( fabs(mAngularInTick) < 10 ) && !(rand()%8) )
			mAngularInTick *= 10;
		if( mAngularInTick >  50 ) mAngularInTick =  50;
		if( mAngularInTick < -50 ) mAngularInTick = -50;
		hkVector4 av( 0, mAngularInTick, 0 );
		mHavokPart->Body->setAngularVelocity( av );
	}
}

void PhysicsObject::Hit( const Vector& hitPoint, const Vector& hitVector )
{
	if( mHavokPart->Body )
	{
		// It's a tweak - I can't understand why pawn rotates in opposite direction
		hkVector4 iv1 = mHavokPart->Body->getAngularVelocity();
		if( mPlayerVariables && mPlayerVariables->Controller )
		{
			hkVector4 v( hitVector.X, hitVector.Y, hitVector.Z );
			v.mul4( GEngine->Physics->MPKnockbackMultiplier );

//			Vector before = mPlayerVariables->Controller->getLinearVelocity3();
			mPlayerVariables->Controller->addVelocity( v );
//			Vector after = mPlayerVariables->Controller->getLinearVelocity3();
//			GLog.Print( "Before: (%f,%f,%f)", before.X, before.Y, before.Z );
//			GLog.Print( ", after: (%f,%f,%f)", after.X, after.Y, after.Z );
//			after -= before;
//			GLog.Print( ", influence diff: (%f,%f,%f)\n", after.X, after.Y, after.Z );
		}
		else
			EffectForce( hitVector, &hitPoint );
		hkVector4 iv2 = mHavokPart->Body->getAngularVelocity();
		iv2.sub4( iv1 );
		iv1.sub4( iv2 );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !iv1.isOk4() )
#else
		if( !iv1.isOk() )
#endif
			GLog.Print( "Bad Angular Velocity in PO::Hit\n" );
#endif
		float iv1len = iv1.length3();
		if( iv1len > MAX_PO_ANGULAR_VELOCITY )
			iv1.mul4(MAX_PO_ANGULAR_VELOCITY/iv1len);

		mHavokPart->Body->setAngularVelocity( iv1 );
	}
}

void PhysicsObject::Impulse( const Vector& hitPoint, const Vector& hitVector )
{
	if( mHavokPart->Body )
		EffectForce( hitVector, &hitPoint );
}

void PhysicsObject::Rotate( Vector& rotateVector )
{
	if( mHavokPart->Body )
	{
		hkVector4 temp( rotateVector.X, rotateVector.Y, rotateVector.Z );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !temp.isOk4() )
#else
		if( !temp.isOk() )
#endif
			GLog.Print( "Bad angular impulse in PO::Rotate\n" );
		else if( temp.length3() > 1000.0f )
			GLog.Print( "Angular impulse > 1000 in PO::Rotate\n" );
#endif
		mHavokPart->Body->applyAngularImpulse( temp );
	}
}

bool PhysicsObject::PositionBugFixed( void )
{
	hkVector4 start;
	start = mHavokPart->Body->getPosition();
	float x = start(0);
	unsigned int a = *(unsigned int *)(&x);
	a &= 0x7fffffff;
	if( a > 0x7f000000 )
	{
		Entity* entity = GetEntity();
		String entName = entity?entity->Name:"(null entity)";
		String fileName = entity?entity->FileName:"(null entity)";
		GLog.Print( "Position bug detected - object '%s' (filename '%s'). Trying to fix it by brute force!!!...", entName.AsChar(), fileName.AsChar() );
		FixHavokPositionBug();
		GLog.Print( "Tried. Is it fixed?\n" );
		return true;
	}
	return false;
}

#define HAVOK_POSFIX_VARIATION_VALUE 0.05f

void PhysicsObject::FixHavokPositionBug( void )
{
#ifndef HAVOK3 // ============================================

	Assert( mHavokPart->Body );

	// Move as much info as we can outside old body
#ifdef HAVOK3
	hkRigidBodyCinfo info;
	mHavokPart->Body->getCinfo( info );
#else
	hkRigidBodyConstructionInfo info;
	mHavokPart->Body->getConstructionInfo( info );
#endif

#ifdef HAVOK_POSITION_BUG_TRACKING
	Assert( info.m_mass >= 0.0f );
#endif

	// Set right position and rotation here - we know they're fucked up in old body
	{
		Entity* entity = GetEntity();
		Vector pos = entity->GetPosition();

		Quaternion quat = entity->GetRotation();
		Vector PivotOffset = quat * mModelPivotOffset;

		pos.X += ( 2*((float)rand()/(float)RAND_MAX) - 1.0f )*HAVOK_POSFIX_VARIATION_VALUE;
		pos.Y += ( 2*((float)rand()/(float)RAND_MAX) - 1.0f )*HAVOK_POSFIX_VARIATION_VALUE;
		pos.Z += ( 2*((float)rand()/(float)RAND_MAX) - 1.0f )*HAVOK_POSFIX_VARIATION_VALUE;

		info.m_position.set( pos.X - PivotOffset.X, pos.Y - PivotOffset.Y, pos.Z - PivotOffset.Z );

		hkQuaternion temp( quat.X, quat.Y, quat.Z, -quat.W );
		info.m_rotation = temp;

//		GLog.Print( "old position: (%f,%f,%f), old rotation: ((%f,%f,%f),%f)...", pos.X, pos.Y, pos.Z, quat.X, quat.Y, quat.Z, quat.W );
	}

	// Make a new body
    m_setTag("Physics");
	hkRigidBody* newBody = new hkRigidBody( info );

	// Fix for convex sweep shapes
#ifdef HAVOK3
	const hkShape* outShape = mHavokPart->Body->getCollidable()->getShape();
#else
	const hkShape* outShape = mHavokPart->Body->getCollidable().getShape();
#endif
	if( outShape->getPrimaryType() == HK_SHAPE_USER7 )
	{
		GLog.Print( "...fix for convex sweep shapes..." );
		const hkConvexShape* intShape = ((const hkConvexSweepShape*)outShape)->getChildShape();
        m_setTag("Physics");
#ifdef HAVOK212
		hkConvexSweepShape* css = new hkConvexSweepShape( intShape, newBody, 10 );	// emergency sweep velocity
#else
		hkConvexSweepShape* css = new hkConvexSweepShape( intShape );
#endif
#ifdef HAVOK3
		newBody->getCollidable()->setShape( css );
#else
		newBody->getCollidable().setShape( css );
#endif
		outShape->removeReference();
	}

	// Move properties to new body
	hkPropertyValue pv;

#ifdef HAVOK3
	if( mHavokPart->Body->hasProperty( HPRayCastDontCollide ) ) { pv.setInt(1); newBody->addProperty( HPRayCastDontCollide, pv ); }
#else
	if( mHavokPart->Body->hasProperty( HPRayCastDontCollide ) ) { pv.asInt = 1; newBody->addProperty( HPRayCastDontCollide, pv ); }
#endif

	if( mHavokPart->Body->hasProperty( HPEntityPointer ) )
	{
		hkPropertyValue entity = mHavokPart->Body->getProperty( HPEntityPointer );
		newBody->addProperty( HPEntityPointer, entity );
	}

	if( mHavokPart->Body->hasProperty( HPGlass ) )
	{
		hkPropertyValue glass = mHavokPart->Body->getProperty( HPGlass );
		newBody->addProperty( HPGlass, glass );
	}

	// Transfer bullet time gravity
	if( mPlayerVariables && mPlayerVariables->FakeBulletTimeGravity )
	{
		havokAntiGravityAction* antigrav = (havokAntiGravityAction*)mPlayerVariables->FakeBulletTimeGravity;
		hkVector4 gravity = mHavokPart->World->World->getGravity();
        m_setTag("Physics");
		mPlayerVariables->FakeBulletTimeGravity = new havokAntiGravityAction( newBody, gravity );
		if( antigrav->getSimulationIsland() )
		{
			mHavokPart->World->World->removeAction( antigrav );
			mHavokPart->World->World->addAction( (havokAntiGravityAction*)mPlayerVariables->FakeBulletTimeGravity );
		}
		antigrav->removeReference();
	}

	// Transfer keyframed state
#ifdef HAVOK3
	if( mHavokPart->Body->getMotionType() == hkMotion::MOTION_KEYFRAMED )
#else
	if( mHavokPart->Body->isKeyframed() )
#endif
	{
		newBody->setKeyframed( true );
		//mHavokPart->Body->setKeyframed( false );
	}

	// Transfer enabled state
	if( mHavokPart->Body->getSimulationIsland() )
	{
		mHavokPart->World->World->removeEntity( mHavokPart->Body );
		mHavokPart->World->World->addEntity( newBody );
	}

	// Transfer hard deactivator
	if( State & EPOHasOurDeactivator )
	{
		mHavokPart->World->mDeactivator->RemoveRigidBody( mHavokPart->Body );
		mHavokPart->World->mDeactivator->AddRigidBody( newBody );
	}

	// Activation state is not transferred - for what?

	// Transfer collision callbacks
	if( ( State & EPOCollisionCB ) && mHavokPart->CollisionListener )
	{
		newBody->setProcessContactCallbackDelay(0);

		float minDelay, minStrength, minMass;
		mHavokPart->CollisionListener->getInfo( minDelay, minStrength, minMass );
		mHavokPart->Body->removeCollisionListener( mHavokPart->CollisionListener );
		delete mHavokPart->CollisionListener;
        TRACE("PhysicsObject::FixHavokPositionBug(): creating collision listener with a delay of %g\n", minDelay);
        m_setTag("Physics");
		mHavokPart->CollisionListener = new havokCollisionListener( newBody, minDelay, minStrength, minMass );
	}

	// Transfer actions
	for( int i = 0; i < POHavokPart::EActionLast; ++i )
	{
		havokAction* action = mHavokPart->GetAction( i );
		if( action )
		{
			havokAction* newAction = 0;

			// get info from old action and make new action for a new body
			switch( i )
			{
			case POHavokPart::EActionAntiGravity:
				{
					havokAntiGravityAction* hkaction = (havokAntiGravityAction*)action;
					hkVector4& grav = hkaction->getGravity();
                    m_setTag("Physics");
					newAction = new havokAntiGravityAction( newBody, grav );
				}
				break;
			case POHavokPart::EActionSpeedDamping:
				{
					havokSpeedDampingAction* hkaction = (havokSpeedDampingAction*)action;
					float f0, f1, f2;
					hkaction->getInfo( f0, f1, f2 );
                    m_setTag("Physics");
					newAction = new havokSpeedDampingAction( newBody, f0, f1, f2 );
				}
				break;
			case POHavokPart::EActionMaintainLinearMovement:
				{
					havokMaintainLinearMovementAction* hkaction = (havokMaintainLinearMovementAction*)action;
					hkVector4 axis;
					hkVector4 position;
					bool soft;
					hkaction->getInfo( axis, position, soft );
                    m_setTag("Physics");
					newAction = new havokMaintainLinearMovementAction( newBody, axis, soft );
					((havokMaintainLinearMovementAction*)newAction)->setInfo( axis, position, soft );
				}
				break;
			case POHavokPart::EActionMaintainPosition:
				{
					havokMaintainPositionAction* hkaction = (havokMaintainPositionAction*)action;
					hkVector4 position;
					float strength;
					hkaction->getInfo( position, strength );
                    m_setTag("Physics");
					newAction = new havokMaintainPositionAction( newBody, position, strength );
				}
				break;
			case POHavokPart::EActionMaintainVelocity:
				{
					havokMaintainVelocityAction* hkaction = (havokMaintainVelocityAction*)action;
					hkVector4 velocity;
					float strength;
					hkaction->getInfo( velocity, strength );
                    m_setTag("Physics");
					newAction = new havokMaintainVelocityAction( newBody, velocity, strength );
				}
				break;
			case POHavokPart::EActionMaintainAngularVelocity:
				{
					havokMaintainAngularVelocityAction* hkaction = (havokMaintainAngularVelocityAction*)action;
					hkVector4 angVelocity;
					float strength;
					hkaction->getInfo( angVelocity, strength );
                    m_setTag("Physics");
					newAction = new havokMaintainAngularVelocityAction( newBody, angVelocity, strength );
				}
				break;
			case POHavokPart::EActionTransporter:
				{
					havokTransporterAction* hkaction = (havokTransporterAction*)action;
					hkVector4 velocity;
					float strength;
					hkaction->getInfo( velocity, strength ); NPROF2(NPPOActionData,4);
                    m_setTag("Physics");
					newAction = new havokTransporterAction( newBody, velocity, strength );
				}
				break;
			default:
				break;
			}

			// replace old action with new
			mHavokPart->RemoveAction( i );
			if( newAction )
				mHavokPart->PutAction( newAction );
		}
	}

	mHavokPart->Body->removeReference();
	mHavokPart->Body = newBody;
#else
	GLog.Print("PhysicsObject::FixHavokPositionBug() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3 =========================================
}

void PhysicsObject::UpdateEntity( float tickPart )
{
	if( !mHavokPart->Body )
		return;

	Entity* entity = GetEntity();
	if( !entity )
		return;

	if( mMissileController )
		mMissileController->predict();
	else if( State & EPOGrenadeFlightFix )
		FixGrenadeFlight( entity );
	else if( State & EPOIsPlayer )
	{
		if( mPlayerVariables->Controller )
			mPlayerVariables->Controller->predict( GEngine->Physics->MPPredictionInterpolation );
		else if( ( !GEngine->Net || GEngine->Net->IsServer() ) && ( GetCollisionGroup() != HCGNoncolliding ) )
			FixPlayerMovement( entity );//*/
	}

	if( State & EPOEntitySteered )
	{
		if( mHavokPart->Body->getSimulationIsland() )
		{
			Vector pos = entity->GetWorldPosition();
			Quaternion quat = entity->GetWorldRotation();
			Vector PivotOffset = GetPivotOffset();
			hkVector4 temp( pos.X - PivotOffset.X, pos.Y - PivotOffset.Y, pos.Z - PivotOffset.Z );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !temp.isOk4() || ( fabs(temp(0)) > 10000.0f ) || ( fabs(temp(1)) > 10000.0f ) || ( fabs(temp(2)) > 10000.0f ) )
#else
			if( !temp.isOk() || ( fabs(temp(0)) > 10000.0f ) || ( fabs(temp(1)) > 10000.0f ) || ( fabs(temp(2)) > 10000.0f ) )
#endif
				GLog.Print( "Position > 10000 in UpdateEntity\n" );
#endif
			mHavokPart->Body->setPosition( temp );
			hkQuaternion q( quat.X, quat.Y, quat.Z, -quat.W);
#ifdef HAVOK_POSITION_BUG_TRACKING
			if( !q.isOk() )
				GLog.Print( "Bad quaternion given to PO in UpdateEntity\n" );
#endif
			mHavokPart->Body->setRotation( q );
			if( mPlayerVariables )
				mPlayerVariables->LastGoodPosition = pos;
		}
	}
	else
	{
		// Move visible parts
		if( mHavokPart->Body->getSimulationIsland() )
		{
			if( entity->ParentJoint == -1 )
			{
				hkVector4 v = mHavokPart->Body->getPosition();
				if( v.length3() > 10000.0f )
				{
#ifdef _DEBUG
						GLog.Print( "Entity '%s' (filename '%s') more distant than 10000.0 from (0,0,0) (pos:(%f,%f,%f)). Fallen off level?\n", entity->Name.AsChar(), entity->FileName.AsChar(), v(0), v(1), v(2) );
#endif
						Vector realPos = Vector( v(0), v(1), v(2) ) + GetPivotOffset();
						GScript.QueueMessage( "IN_DEATH_ZONE","%e,%f,%f,%f,%s", entity, realPos.X, realPos.Y, realPos.Z, "havok");
						entity->TestDeathZone = false;
						mHavokPart->World->RigidBodiesPosExceeded.AddUnique( mHavokPart->Body );
					}
				hkQuaternion q = mHavokPart->Body->getRotation();
				hkVector4 q1 = q.getImag();
				Quaternion quat( -q.getReal(), q1(0), q1(1), q1(2) );
				Vector PivotOffset = GetPivotOffset();
				entity->SetPositionAndRotation( Vector( v(0), v(1), v(2) ) + PivotOffset,  quat);
//				if( entity->FileName == "../Data/Items/kolek.dat" )
//					GLog.Print( "Setting entity pos at (%f,%f,%f) + pivot offset\n", v(0), v(1), v(2) );
//					GLog.Print( "Rotation: (%f,%f,%f,%f)\n", -q.getReal(), q1(0), q1(1), q1(2) );
			}
        }
	}
}

void PhysicsObject::Tick( float timeDiff )
{
	if( mPlayerVariables )
		mPlayerVariables->AccumulatedDelta += timeDiff;

	if( !mHavokPart->Body || !mHavokPart->Body->getSimulationIsland() )
		return;

	// Reset angular in tick; it works only if effected in the same frame it was differentiated from 0 in
	mAngularInTick = 0;

	if( ForcesToApply ) EffectForces();

//	if( PositionBugFixed() ) return;

	// Determine if we need to activate
	if( !mHavokPart->Body->isActive() )
	{
#ifdef HAVOK3
		int collGroup = mHavokPart->Body->getCollidable()->getCollisionFilterInfo();
#else
		int collGroup = mHavokPart->Body->getCollidable().getCollisionFilterInfo();
#endif
		switch( collGroup )
		{
		case HCGPlayerBody:
#ifdef HAVOK3
		case HCGBody:			mHavokPart->Body->activate();	break;
#else
		case HCGBody:			mHavokPart->Body->forceActivate();	break;
#endif
		case HCGParticles:		return;
		default:				break;
		}
	}

	// Move monsters
	if( ( State & EPOIsMonster ) && !( State & EPOEntitySteered ) )
	{
		Model* monster = (Model*)GetEntity();
		if( !monster ) return;

		bool noRagdoll = !monster->IsRagdoll();

		onFloor = false;
		if (noRagdoll) {
			hkVector4 actVel = mHavokPart->Body->getLinearVelocity();
			if( doNotCheckFloors || MonsterFloorCheck(-5.5f-1.5f/BodyScale/OLD_PAWN_SCALE) )
				onFloor = true;
			else
				if (actVel(1) < -0.01)				// nie wykryl podlogi, ale tez nie spada
					mMoveVector = Vector(0,0,0);

			if (!(State & EPOIsFlying)) {
				hkVector4 desiredVel( mMoveVector.X, mMoveVector.Y, mMoveVector.Z );
				hkVector4 havokVel = actVel;
				hkVector4 previousVel(prevVel.X, prevVel.Y, prevVel.Z);
				havokVel.sub4(previousVel);
					
				Vector grav = GetGravity();			// zeby tlumil ruch w dol, gdy cialo nie ma gracitacji

				havokVel(0) *=  havokInfluenceInMonsterMovement;
				if (havokVel(1) > 0 || grav.Y == 0) {
					havokVel(1) *=  havokInfluenceInMonsterMovement;
				}
				havokVel(2) *=  havokInfluenceInMonsterMovement;
				
				prevVel = Vector(desiredVel(0), desiredVel(1), desiredVel(2));
				desiredVel.add4(havokVel);
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
				if( !desiredVel.isOk4() || ( fabs(desiredVel(0)) > 1000.0f ) || ( fabs(desiredVel(1)) > 1000.0f ) || ( fabs(desiredVel(2)) > 1000.0f ) )
#else
				if( !desiredVel.isOk() || ( fabs(desiredVel(0)) > 1000.0f ) || ( fabs(desiredVel(1)) > 1000.0f ) || ( fabs(desiredVel(2)) > 1000.0f ) )
#endif
					GLog.Print( "Velocity > 1000 in PO::Tick\n" );
#endif
				mHavokPart->Body->setLinearVelocity(desiredVel);
			}
		}
		//mMoveVector = Vector(0,0,0);
	}
	Assert( !mMissileController );
}

void PhysicsObject::SetMass( float mass )
{
	GLog.Print("PhysicsObject::SetMass()\n");

	if( !mHavokPart->Body || ( mass <= 0.0f ) ) return;
	if( ( mFreedomOfRotation != ERotAllAxes ) && ( mFreedomOfRotation != ERotFullFree ) )
	{
		if( mVolumeMass == -1.0f )
		{
			mVolumeMass = mHavokPart->Body->getMass();
#ifdef HAVOK_POSITION_BUG_TRACKING
			Assert( mVolumeMass > 0.0f );
#endif
		}
		mHavokPart->Body->setMass( mass );
		return;
	}

	float currentMass = mHavokPart->Body->getMass();
#ifdef HAVOK_POSITION_BUG_TRACKING
	Assert( currentMass >= 0.0f );
#endif

	if( mVolumeMass == -1.0f )
		mVolumeMass = currentMass;

	if( currentMass < 0.0001f )
	{
		currentMass = 0.0001f;
		mHavokPart->Body->setMass( currentMass );
	}

	float scaleMultiplier = mass/currentMass;

	if( scaleMultiplier == 1.0f ) return;

	hkMassProperties massProperties;
	mHavokPart->Body->getInertiaLocal( massProperties.m_inertiaTensor );
	massProperties.m_centerOfMass = mHavokPart->Body->getCenterOfMassLocal();
	massProperties.m_mass = currentMass;

#ifdef HAVOK_POSITION_BUG_TRACKING
	Assert( massProperties.m_mass >= 0.0f );
#endif

#ifdef HAVOK3
	HavokWorld::scaleMassProperties( scaleMultiplier, massProperties );
#else
	hkInertiaTensorComputer::scaleMassProperties( scaleMultiplier, massProperties );
#endif
	mInertiaTensorMultiplier *= scaleMultiplier;

#ifdef HAVOK_POSITION_BUG_TRACKING
	Assert( massProperties.m_mass >= 0.0f );
#endif

	mHavokPart->Body->setMass( massProperties.m_mass );
	Assert( massProperties.m_centerOfMass.equals3( mHavokPart->Body->getCenterOfMassLocal() ) );
	mHavokPart->Body->setInertiaLocal( massProperties.m_inertiaTensor );
}

void PhysicsObject::ScaleMass( float scaleMultiplier )
{
	GLog.Print("Physics::ScaleMass()\n");

	if( !mHavokPart->Body || ( scaleMultiplier == 1.0f ) ) return;

	if( ( mFreedomOfRotation != ERotAllAxes ) && ( mFreedomOfRotation != ERotFullFree ) )
	{
		float currentMass = mHavokPart->Body->getMass();
#ifdef HAVOK_POSITION_BUG_TRACKING
		Assert( currentMass >= 0.0f );
#endif
		if( mVolumeMass == -1.0f )
			mVolumeMass = currentMass;

#ifdef HAVOK_POSITION_BUG_TRACKING
	Assert( currentMass*scaleMultiplier >= 0.0f );
#endif

		mHavokPart->Body->setMass( currentMass*scaleMultiplier );
		return;
	}

	if( scaleMultiplier < 0.0001f )
		scaleMultiplier = 0.0001f;

	if( mVolumeMass == -1.0f )
	{
		mVolumeMass = mHavokPart->Body->getMass();
#ifdef HAVOK_POSITION_BUG_TRACKING
		Assert( mVolumeMass > 0.0f );
#endif
	}

	hkMassProperties massProperties;
	mHavokPart->Body->getInertiaLocal( massProperties.m_inertiaTensor );
	massProperties.m_centerOfMass = mHavokPart->Body->getCenterOfMassLocal();
	massProperties.m_mass = mHavokPart->Body->getMass();

#ifdef HAVOK_POSITION_BUG_TRACKING
	Assert( massProperties.m_mass >= 0.0f );
#endif

#ifdef HAVOK3
	HavokWorld::scaleMassProperties( scaleMultiplier, massProperties );
#else
	hkInertiaTensorComputer::scaleMassProperties( scaleMultiplier, massProperties );
#endif
	mInertiaTensorMultiplier *= scaleMultiplier;

#ifdef HAVOK_POSITION_BUG_TRACKING
	Assert( massProperties.m_mass >= 0.0f );
#endif

	mHavokPart->Body->setMass( massProperties.m_mass );
	Assert( massProperties.m_centerOfMass.equals3( mHavokPart->Body->getCenterOfMassLocal() ) );
	mHavokPart->Body->setInertiaLocal( massProperties.m_inertiaTensor );
}

void PhysicsObject::ScaleInertiaTensor( float scaleMultiplier )
{
	GLog.Print("PhysicsObject::ScaleInertiaTensor()\n");

	if( !mHavokPart->Body || ( scaleMultiplier == 1.0f ) ) return;
	if( ( mFreedomOfRotation != ERotAllAxes ) && ( mFreedomOfRotation != ERotFullFree ) ) return;

	if( scaleMultiplier < 0.00000001f )
		scaleMultiplier = 0.00000001f;

	hkMassProperties massProperties;
	mHavokPart->Body->getInertiaLocal( massProperties.m_inertiaTensor );
	massProperties.m_centerOfMass = mHavokPart->Body->getCenterOfMassLocal();
	massProperties.m_mass = mHavokPart->Body->getMass();

#ifdef HAVOK_POSITION_BUG_TRACKING
	Assert( massProperties.m_mass >= 0.0f );
#endif

#ifdef HAVOK3
	HavokWorld::scaleMassProperties( scaleMultiplier, massProperties );
#else
	hkInertiaTensorComputer::scaleMassProperties( scaleMultiplier, massProperties );
#endif
	mInertiaTensorMultiplier *= scaleMultiplier;

#ifdef HAVOK_POSITION_BUG_TRACKING
	Assert( massProperties.m_mass >= 0.0f );
#endif

	mHavokPart->Body->setInertiaLocal( massProperties.m_inertiaTensor );
}

float PhysicsObject::GetVolumeMass( void )
{
	if( mVolumeMass == -1.0f )
	{
		mVolumeMass = mHavokPart->Body->getMass();
		if( mVolumeMass == 0.f )
			mVolumeMass = 1.f;
	}
	return mVolumeMass;
}

void PhysicsObject::SetFreedomOfRotation( int type, float softness )
{
	if( !mHavokPart->Body ) return;
	hkMassProperties massProperties;

	hkMatrix3 mat;
	mat.setZero();
	switch( type )
	{
	case ERotDisabled:
		mat(0,0) = FLT_MAX;
		mat(0,1) = FLT_MAX;
		mat(0,2) = FLT_MAX;
		mat(1,0) = FLT_MAX;
		mat(1,1) = FLT_MAX;
		mat(1,2) = FLT_MAX;
		mat(2,0) = FLT_MAX;
		mat(2,1) = FLT_MAX;
		mat(2,2) = FLT_MAX;
		break;
	case ERotYAxis:
		mat(0,0) = FLT_MAX;
		mat(0,1) = FLT_MAX;
		mat(0,2) = FLT_MAX;
		mat(1,0) = FLT_MAX;
		mat(1,1) = 1.0f*ROTATION_STRENGTH_MODIFIER;	// don't restrict Y axis rotation
		mat(1,2) = FLT_MAX;
		mat(2,0) = FLT_MAX;
		mat(2,1) = FLT_MAX;
		mat(2,2) = FLT_MAX;
		break;
	case ERotAllAxes:
/*
		mat(0,0) = FLT_MAX;
		mat(1,1) = FLT_MAX;
		mat(2,2) = FLT_MAX;
		break;
*/
	case ERotFullFree:
		hkInertiaTensorComputer::computeSphereVolumeMassProperties( 1.0f, mHavokPart->Body->getMass(), massProperties );
#ifdef HAVOK_POSITION_BUG_TRACKING
		Assert( mHavokPart->Body->getMass() >= 0.0f );
#endif
		mHavokPart->Body->setInertiaLocal( massProperties.m_inertiaTensor );
		mHavokPart->Body->setCenterOfMassLocal( massProperties.m_centerOfMass );
		mFreedomOfRotation = ERotFullFree;
		return;
	case ERotHardTurn:
		mat(0,0) = softness*ROTATION_STRENGTH_MODIFIER;
		mat(1,1) = softness*ROTATION_STRENGTH_MODIFIER;
		mat(2,2) = softness*ROTATION_STRENGTH_MODIFIER;
		break;
	case ERotXAxis:
		mat(0,0) = 1.0f*ROTATION_STRENGTH_MODIFIER;	// don't restrict X axis rotation
		mat(0,1) = FLT_MAX;
		mat(0,2) = FLT_MAX;
		mat(1,0) = FLT_MAX;
		mat(1,1) = FLT_MAX;
		mat(1,2) = FLT_MAX;
		mat(2,0) = FLT_MAX;
		mat(2,1) = FLT_MAX;
		mat(2,2) = FLT_MAX;
		break;
	case ERotZAxis:
		mat(0,0) = FLT_MAX;
		mat(0,1) = FLT_MAX;
		mat(0,2) = FLT_MAX;
		mat(1,0) = FLT_MAX;
		mat(1,1) = FLT_MAX;
		mat(1,2) = FLT_MAX;
		mat(2,0) = FLT_MAX;
		mat(2,1) = FLT_MAX;
		mat(2,2) = 1.0f*ROTATION_STRENGTH_MODIFIER;	// don't restrict Z axis rotation
		break;
	};
	mHavokPart->Body->setInertiaLocal( mat );
	mFreedomOfRotation = (char)type;
	mInertiaTensorMultiplier = 1.0f;
}

void PhysicsObject::SetFriction( float friction )
{
	if( mHavokPart->Body )
		mHavokPart->Body->getMaterial().setFriction( friction );
}

void PhysicsObject::SetRestitution( float restitution )
{
	if( mHavokPart->Body )
		mHavokPart->Body->getMaterial().setRestitution( restitution );
}

float PhysicsObject::GetLinearDamping( void )
{
	if( mHavokPart->Body )
	{
		if( mHavokPart->World->IsUnderwater() )
		{
			hkReal linDamping = mHavokPart->Body->getLinearDamping();
			hkReal diff = 1.0 - linDamping;
			diff *= GEngine->Physics->TWUnderwaterDampingRaiseLinear;
			return ( 1.0 - diff );
		}
		return mHavokPart->Body->getLinearDamping();
	}
	return 0.0f;
}

void PhysicsObject::SetLinearDamping( float damping )
{
	if( mHavokPart->Body )
	{
		if( mHavokPart->World->IsUnderwater() )
		{
			hkReal diff = 1.0 - damping;
			diff /= GEngine->Physics->TWUnderwaterDampingRaiseLinear;
			mHavokPart->Body->setLinearDamping( 1.0 - diff );
		}
		else
		{
			mHavokPart->Body->setLinearDamping( damping );
			if( mMissileController )
				mMissileController->setDamping( damping );
		}
	}
}

float PhysicsObject::GetAngularDamping( void )
{
	if( mHavokPart->Body )
	{
		if( mHavokPart->World->IsUnderwater() )
		{
			hkReal angDamping = mHavokPart->Body->getAngularDamping();
			hkReal diff = 1.0 - angDamping;
			diff *= GEngine->Physics->TWUnderwaterDampingRaiseAngular;
			return ( 1.0 - diff );
		}
		return mHavokPart->Body->getAngularDamping();
	}
	return 0.0f;
}

void PhysicsObject::SetAngularDamping( float damping )
{
	if( mHavokPart->Body )
	{
		if( mHavokPart->World->IsUnderwater() )
		{
			hkReal diff = 1.0 - damping;
			diff /= GEngine->Physics->TWUnderwaterDampingRaiseAngular;
			mHavokPart->Body->setAngularDamping( 1.0 - diff );
		}
		else
			mHavokPart->Body->setAngularDamping( damping );
	}
}

float PhysicsObject::GetMass( void )
{
#ifdef HAVOK_POSITION_BUG_TRACKING
	float mass = mHavokPart->Body?mHavokPart->Body->getMass():0.0f;
	Assert( mass >= 0.0f );
	return mass;
#else
	return mHavokPart->Body?mHavokPart->Body->getMass():0.0f;
#endif
}

float PhysicsObject::GetFriction( void )
{
	return mHavokPart->Body?mHavokPart->Body->getMaterial().getFriction():0.0f;
}

float PhysicsObject::GetRestitution( void )
{
	return mHavokPart->Body?mHavokPart->Body->getMaterial().getRestitution():0.0f;
}

void *PhysicsObject::GetHavokBody() const
{
    return mHavokPart?mHavokPart->Body:NULL;
};

void PhysicsObject::SetCollisionCallbacks( bool enable, float minimalDelay, float minimalStrength, float minimalOtherBodyMass )
{
	if( !mHavokPart->Body ) return;
	if( enable )
	{
		if( !mHavokPart->CollisionListener )
		{
			mHavokPart->Body->setProcessContactCallbackDelay(0);
            TRACE("PhysicsObject::SetCollisionCallbacks(): creating collision listener with a delay of %g\n", minimalDelay);
            m_setTag("Physics");
			mHavokPart->CollisionListener = new havokCollisionListener( mHavokPart->Body, minimalDelay, minimalStrength, minimalOtherBodyMass );
		}
		State |= EPOCollisionCB;
	}
	else if( !enable && mHavokPart->CollisionListener )
	{
		State &= ~EPOCollisionCB;
		if( !( State & EPOIsMonster ) && !IsPlayer() )
		{
			mHavokPart->Body->removeCollisionListener( mHavokPart->CollisionListener );
			delete mHavokPart->CollisionListener;
			mHavokPart->CollisionListener = 0;
		}
	};
}

void PhysicsObject::SetOrientation( float angle )
{
	hkVector4 temp( 0,-1,0 );
	hkQuaternion qu( temp, -angle );
#ifdef HAVOK_POSITION_BUG_TRACKING
	if( !qu.isOk() )
		GLog.Print( "Bad quaternion given to PO in SetOrientation\n" );
#endif
	if( mHavokPart->Body )
		mHavokPart->Body->setRotation( qu );
}

float PhysicsObject::GetOrientation() const
{
	hkVector4 v;
	hkVector4 temp(0,0,-1);
	v.setRotatedDir( mHavokPart->Body->getRotation(), temp );
	return -(float)atan2(v(0),-v(2));
}

void PhysicsObject::Enable( bool enable )
{
	if( !mHavokPart->Body ) return;

	if( enable )
	{
		if( !mHavokPart->Body->getSimulationIsland() )
		{
			m_setTag("Physics");
			mHavokPart->World->World->addEntity( mHavokPart->Body );
			mHavokPart->ReconnectActionsToWorld();

			Assert( mPOTableIndex == -1 );
			mPOTableIndex = mHavokPart->World->PhysicsObjects.Add( this );
		}
	}
	else
	{
		if( mHavokPart->Body->getSimulationIsland() )
		{
			mHavokPart->World->World->removeEntity( mHavokPart->Body );

			Assert( mPOTableIndex != -1 );

			mHavokPart->World->PhysicsObjects.FastRemove( mPOTableIndex );
			if( mPOTableIndex < mHavokPart->World->PhysicsObjects.Size() )
				mHavokPart->World->PhysicsObjects[mPOTableIndex]->mPOTableIndex = mPOTableIndex;
			mPOTableIndex = -1;
		}
	}
}

#ifdef TRANSGAMING
bool PhysicsObject::IsEnabled( void )
#else
inline bool PhysicsObject::IsEnabled( void )
#endif
{
	if( mHavokPart->Body )
		return ( mHavokPart->Body->getSimulationIsland() != 0);
	return false;
}

#ifdef TRANSGAMING
void PhysicsObject::SetPinned( bool enable )
#else
inline void PhysicsObject::SetPinned( bool enable )
#endif
{
	Assert( mHavokPart->Body );
#ifdef HAVOK3
	if ( enable )
		mHavokPart->Body->setMotionType( hkMotion::MOTION_KEYFRAMED );
	else
		mHavokPart->Body->setMotionType( hkMotion::MOTION_FIXED );
#else
	mHavokPart->Body->setKeyframed( enable );
#endif
}

#ifdef TRANSGAMING
bool PhysicsObject::IsPinned( void )
#else
inline bool PhysicsObject::IsPinned( void )
#endif
{
	Assert( mHavokPart->Body );
#ifdef HAVOK3
	if ( mHavokPart->Body->getMotionType() == hkMotion::MOTION_KEYFRAMED )
		return true;
	else
		return false;
#else
	return mHavokPart->Body->isKeyframed();
#endif
}

#ifdef TRANSGAMING
bool PhysicsObject::IsFixed( void )
#else
inline bool PhysicsObject::IsFixed( void )
#endif
{
	Assert( mHavokPart->Body );
	return mHavokPart->Body->isFixed();
}

inline bool PhysicsObject::IsPhys( void )
{
	Assert( mHavokPart->Body );
#ifdef HAVOK3
	return ( (mHavokPart->Body->getMotionType() != hkMotion::MOTION_FIXED) && (mHavokPart->Body->getMotionType() != hkMotion::MOTION_KEYFRAMED) && ( mHavokPart->Body->getSimulationIsland() != 0 ) );
#else
	return ( !mHavokPart->Body->isFixed() && !mHavokPart->Body->isKeyframed() && ( mHavokPart->Body->getSimulationIsland() != 0 ) );
#endif
}

void PhysicsObject::Activate( bool activate )
{
	if( !mHavokPart->Body ) return;
	if( activate )
#ifdef HAVOK3
		mHavokPart->Body->activate();
	else
		mHavokPart->Body->deactivate();
#else
		mHavokPart->Body->forceActivate();
	else
		mHavokPart->Body->forceDeactivate();
#endif
}

#ifdef TRANSGAMING
bool PhysicsObject::IsActive( void )
#else
inline bool PhysicsObject::IsActive( void )
#endif
{
	if( !mHavokPart->Body ) return false;
	return mHavokPart->Body->isActive();
}

Vector PhysicsObject::GetVel( void ) const
{
	if( mHavokPart->Body )
	{
		if( mMissileController )
		{
			hkVector4 vel = mMissileController->getVelocity();
			return Vector( vel(0), vel(1), vel(2) );
		}
		if( mPlayerVariables && mPlayerVariables->Controller )
		{
			const hkVector4& vel = mPlayerVariables->Controller->getLinearVelocity();
			return Vector( vel(0), vel(1), vel(2) );
		}
		else
		{
			const hkVector4& vel = mHavokPart->Body->getLinearVelocity();
			return Vector( vel(0), vel(1), vel(2) );
		}
    }

    return Vector(0,0,0);
}

Vector PhysicsObject::GetPosition( void ) const
{
	if( mHavokPart->Body )
	{
		const hkVector4& pos = mHavokPart->Body->getPosition();
		Vector PivotOffset = GetPivotOffset();

		return Vector( pos(0), pos(1), pos(2) ) + PivotOffset;
    }

    return Vector(0,0,0);
}

Vector PhysicsObject::GetCenterOfMass( void ) const
{
	if( mHavokPart->Body )
	{
		const hkVector4& pos = mHavokPart->Body->getCenterOfMassInWorld();
		Vector PivotOffset = GetPivotOffset();
		return Vector( pos(0), pos(1), pos(2) ) + PivotOffset;
    }

    return Vector(0,0,0);
}

void PhysicsObject::MoveCenterOfMass( Vector& relative ) const
{
	if( mHavokPart->Body )
	{
		hkVector4 massCenter = mHavokPart->Body->getCenterOfMassLocal();
		hkMatrix3 inertiaTensor;
		mHavokPart->Body->getInertiaLocal( inertiaTensor );
		hkReal mass = mHavokPart->Body->getMass();

#ifdef HAVOK_POSITION_BUG_TRACKING
		Assert( mass >= 0.0f );
#endif

		hkVector4 rel;
		rel.set( relative.X, relative.Y, relative.Z );

		hkInertiaTensorComputer::shiftInertiaFromCom( massCenter, mass, inertiaTensor );

		massCenter.add4( rel );

		hkInertiaTensorComputer::shiftInertiaToCom( massCenter, mass, inertiaTensor );
		mHavokPart->Body->setCenterOfMassLocal( massCenter );
		mHavokPart->Body->setInertiaLocal( inertiaTensor );
    }
}

void PhysicsObject::SetVel( const Vector& vel )
{
	if( mHavokPart->Body )
	{
#ifdef HAVOK3
		if( mHavokPart->Body->getMotionType() == hkMotion::MOTION_KEYFRAMED )
#else
		if( mHavokPart->Body->isKeyframed() )
#endif
		{
			hkVector4 temp( vel.X, vel.Y, vel.Z );
			if( mMissileController )
				mMissileController->setVelocity( temp );
			else if( mPlayerVariables && mPlayerVariables->Controller )
				mPlayerVariables->Controller->setLinearVelocity( temp );
			return;
		}

		hkVector4 temp( vel.X, vel.Y, vel.Z );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !temp.isOk4() || ( fabs(temp(0)) > 1000.0f ) || ( fabs(temp(1)) > 1000.0f ) || ( fabs(temp(2)) > 1000.0f ) )
#else
		if( !temp.isOk() || ( fabs(temp(0)) > 1000.0f ) || ( fabs(temp(1)) > 1000.0f ) || ( fabs(temp(2)) > 1000.0f ) )
#endif
			GLog.Print( "Velocity > 1000 in PO::SetVel\n" );
#endif
		mHavokPart->Body->setLinearVelocity( temp );
		if( mHavokPart->Body->getSimulationIsland() )
#ifdef HAVOK3
			mHavokPart->Body->activate();
#else
			mHavokPart->Body->forceActivate();
#endif
    }
}

Vector PhysicsObject::GetAngularVel( void  )
{
	if( mHavokPart->Body )
	{
		hkVector4 v =  mHavokPart->Body->getAngularVelocity();
		return Vector( v(0), v(1), v(2) );
	}

	return Vector(0,0,0);
}

void PhysicsObject::SetAngularVel( const Vector& angVel )
{
	if( mHavokPart->Body )
	{
		hkVector4 v( angVel.X, angVel.Y, angVel.Z );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !v.isOk4() )
#else
		if( !v.isOk() )
#endif
			GLog.Print( "Bad Angular Velocity in PO::SetAngularVel\n" );
		else if( v.length3() > 150.0f )
		{
#ifndef TRANSGAMING
			GLog.Print( "Angular Velocity > 150 in PO::SetAngularVel ( = %f )\n", v.length3() );
#else
			GLog.Print( "Angular Velocity > 150 in PO::SetAngularVel\n");
#endif
		}
#endif
		mHavokPart->Body->setAngularVelocity( v );
		if( mHavokPart->Body->getSimulationIsland() )
#ifdef HAVOK3
			mHavokPart->Body->activate();
#else
			mHavokPart->Body->forceActivate();
#endif
	}
}

void PhysicsObject::EnableGravity( bool enable )
{
	Vector newGrav(0,0,0);
	if( enable )
	{
		hkVector4 worldGravity = mHavokPart->World->World->getGravity();
		newGrav.X = worldGravity(0);
		newGrav.Y = worldGravity(1);
		newGrav.Z = worldGravity(2);
	}
	SetGravity( newGrav );
}

void PhysicsObject::SetGravity( Vector gravity )
{
	if( !mHavokPart->Body ) return;

	hkVector4 worldGravity = mHavokPart->World->World->getGravity();
	hkVector4 currentGravity;

	havokAntiGravityAction* action = (havokAntiGravityAction*)mHavokPart->GetAction( POHavokPart::EActionAntiGravity );
	if( action )
	{
		currentGravity = worldGravity;
		currentGravity.sub4( action->getGravity() );
	}
	else
		currentGravity = worldGravity;

	hkVector4 newGravity( gravity.X, gravity.Y, gravity.Z );

	if( currentGravity.equals3( newGravity ) ) return;

	if( newGravity.equals3( worldGravity ) )
	{
		if( action )
			mHavokPart->RemoveAction( POHavokPart::EActionAntiGravity );
		return;
	}

	hkVector4 gravityToSet( worldGravity );
	gravityToSet.sub4( newGravity );

	if( !action )
	{
        m_setTag("Physics");
		action = new havokAntiGravityAction( mHavokPart->Body, gravityToSet );
		mHavokPart->PutAction( action );
	}
	else
		action->setGravity( gravityToSet );
}

Vector PhysicsObject::GetGravity( void )
{
	hkVector4 worldGravity = mHavokPart->World->World->getGravity();
	hkVector4 currentGravity;
	havokAntiGravityAction* action = (havokAntiGravityAction*)mHavokPart->GetAction( POHavokPart::EActionAntiGravity );
	if( action )
	{
		currentGravity = worldGravity;
		currentGravity.sub4( action->getGravity() );
	}
	else
		currentGravity = worldGravity;
	
	return Vector( currentGravity(0), currentGravity(1), currentGravity(2) );
}

void PhysicsObject::EnableSpeedDamping( bool enable, float maxSpeed, float dampingSpeed, float dampingStrength )
{
	if( !mHavokPart->Body ) return;

	havokSpeedDampingAction* action = (havokSpeedDampingAction*)mHavokPart->GetAction( POHavokPart::EActionSpeedDamping );

	if( !enable && action )		// disable speed damping
		mHavokPart->RemoveAction( POHavokPart::EActionSpeedDamping );
	else if( enable )			// enable speed damping
	{
		if( action )
			mHavokPart->RemoveAction( POHavokPart::EActionSpeedDamping );

		hkVector4 gravity = mHavokPart->World->World->getGravity();
        m_setTag("Physics");
		action = new havokSpeedDampingAction( mHavokPart->Body, maxSpeed, dampingSpeed, dampingStrength );
		mHavokPart->PutAction( action );
	}
}

void PhysicsObject::MaintainLinearMovement( bool enable, Vector axis, bool soft )
{
	if( !mHavokPart->Body ) return;

	havokMaintainLinearMovementAction* action = (havokMaintainLinearMovementAction*)mHavokPart->GetAction( POHavokPart::EActionMaintainLinearMovement );

	if( !enable && action )		// disable linear movement maintainer
		mHavokPart->RemoveAction( POHavokPart::EActionMaintainLinearMovement );
	else if( enable )			// enable linear movement maintainer
	{
		if( action )
			mHavokPart->RemoveAction( POHavokPart::EActionMaintainLinearMovement );

		hkVector4 havokAxis( axis.X, axis.Y, axis.Z );
        m_setTag("Physics");
		action = new havokMaintainLinearMovementAction( mHavokPart->Body, havokAxis, soft );
		mHavokPart->PutAction( action );
	}
}

void PhysicsObject::MaintainPosition( bool enable, Vector desiredPosition, float strength )
{
	if( !mHavokPart->Body ) return;

	havokMaintainPositionAction* action = (havokMaintainPositionAction*)mHavokPart->GetAction( POHavokPart::EActionMaintainPosition );

	if( !enable && action )		// disable position maintainer
		mHavokPart->RemoveAction( POHavokPart::EActionMaintainPosition );
	else if( enable )			// enable position maintainer
	{
		if( action )
			mHavokPart->RemoveAction( POHavokPart::EActionMaintainPosition );

		Vector PivotOffset = GetPivotOffset();
		hkVector4 havokDesPos( desiredPosition.X - PivotOffset.X, desiredPosition.Y - PivotOffset.Y, desiredPosition.Z - PivotOffset.Z );
        m_setTag("Physics");
		action = new havokMaintainPositionAction( mHavokPart->Body, havokDesPos, strength );
		mHavokPart->PutAction( action );
	}
}

void PhysicsObject::MaintainVelocity( bool enable, Vector desiredVelocity, float strength )
{
	if( !mHavokPart->Body ) return;

	havokMaintainVelocityAction* action = (havokMaintainVelocityAction*)mHavokPart->GetAction( POHavokPart::EActionMaintainVelocity );

	if( !enable && action )		// disable velocity maintainer
		mHavokPart->RemoveAction( POHavokPart::EActionMaintainVelocity );
	else if( enable )			// enable velocity maintainer
	{
		if( action )
			mHavokPart->RemoveAction( POHavokPart::EActionMaintainVelocity );

		hkVector4 havokDesVel( desiredVelocity.X, desiredVelocity.Y, desiredVelocity.Z );
        m_setTag("Physics");
		action = new havokMaintainVelocityAction( mHavokPart->Body, havokDesVel, strength );
		mHavokPart->PutAction( action );
	}
}

void PhysicsObject::MaintainAngularVelocity( bool enable, Vector desiredAngularVelocity, float strength )
{
	if( !mHavokPart->Body ) return;

	havokMaintainAngularVelocityAction* action = (havokMaintainAngularVelocityAction*)mHavokPart->GetAction( POHavokPart::EActionMaintainAngularVelocity );

	if( !enable && action )		// disable angular velocity maintainer
		mHavokPart->RemoveAction( POHavokPart::EActionMaintainAngularVelocity );
	else if( enable )			// enable angular velocity maintainer
	{
		if( action )
			mHavokPart->RemoveAction( POHavokPart::EActionMaintainAngularVelocity );

		hkVector4 havokDesAngVel( desiredAngularVelocity.X, desiredAngularVelocity.Y, desiredAngularVelocity.Z );
        m_setTag("Physics");
		action = new havokMaintainAngularVelocityAction( mHavokPart->Body, havokDesAngVel, strength );
		mHavokPart->PutAction( action );
	}
}

void PhysicsObject::SetAsTransporter( bool enable, Vector appliedVelocity, float strength )
{
	if( !mHavokPart->Body ) return;

	havokTransporterAction* action = (havokTransporterAction*)mHavokPart->GetAction( POHavokPart::EActionTransporter );

	if( !enable && action )
		mHavokPart->RemoveAction( POHavokPart::EActionTransporter );
	else if( enable )
	{
		if( action )
			mHavokPart->RemoveAction( POHavokPart::EActionTransporter );

		hkVector4 av( appliedVelocity.X, appliedVelocity.Y, appliedVelocity.Z );
        m_setTag("Physics");
		action = new havokTransporterAction( mHavokPart->Body, av, strength );
		mHavokPart->PutAction( action );
	}
}

void PhysicsObject::EnableLineTraceCollision( bool enable )
{
	if( mHavokPart->Body )
	{
		bool collides = CanLineTraceCollision();
		if( enable && !collides )
			mHavokPart->Body->removeProperty( HPRayCastDontCollide );
		else if( !enable && collides )
		{
			hkPropertyValue pv;
#ifdef HAVOK3
			pv.setInt(1);
#else
			pv.asInt = 1;
#endif
			mHavokPart->Body->addProperty( HPRayCastDontCollide, pv );
		}
	}
}

bool PhysicsObject::CanLineTraceCollision( void )
{
	Assert( mHavokPart->Body );
	return !mHavokPart->Body->hasProperty( HPRayCastDontCollide );
}

int PhysicsObject::GetCollisionGroup( void )
{
	if( mHavokPart->Body )
#ifdef HAVOK3
		return mHavokPart->Body->getCollidable()->getCollisionFilterInfo();
#else
		return mHavokPart->Body->getCollidable().getCollisionFilterInfo();
#endif
	else
		return 1;
}

void PhysicsObject::SetCollisionGroup( int newGroup )
{
	Assert( (newGroup > 0) && (newGroup < 32) );
	if( mHavokPart->Body )
	{
#ifdef HAVOK3
		mHavokPart->Body->getCollidable()->setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( newGroup ) );
		if( mHavokPart->Body->getSimulationIsland() )
			mHavokPart->World->World->updateCollisionFilterOnEntity( mHavokPart->Body, HK_UPDATE_FILTER_ON_ENTITY_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY, HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS );
#else
		mHavokPart->Body->getCollidable().setCollisionFilterInfo( hkGroupFilter::calcFilterInfo( newGroup ) );
		if( mHavokPart->Body->getSimulationIsland() )
			mHavokPart->World->World->updateCollisionFilter( mHavokPart->Body );
#endif
	}
}

float PhysicsObject::GetMaxSphereRay( void )
{
	float scale = BodyScale*OLD_PAWN_SCALE;
	switch( BodyType )
	{
	case EPawnBodyPlayer:	// default player body, 4 spheres
	case EPawnBodyDefault:
	case EPawnBodyDefaultSweep:
		return 2.0f*scale;

	case EPawnBodySphere:		// simple sphere, like, for instance, it would be for raven
	case EPawnBodySphereSweep:	// simple sphere, like, for instance, it would be for raven
		return 5.5f*scale;

	case EPawnBodyFatter:		// three spheres, two big, one smaller, more fat than with default spheres
	case EPawnBodyFatterSweep:	// three spheres, two big, one smaller, more fat than with default spheres
		return 3.0f*scale;

	default:
		return 11.0f*scale;
	}
}

void PhysicsObject::SetHardDeactivator( void )
{
	if( (State & EPOHasOurDeactivator) || !mHavokPart->Body ) return;
	mHavokPart->World->mDeactivator->AddRigidBody( mHavokPart->Body );
	State |= EPOHasOurDeactivator;
}

void PhysicsObject::SetAsGlass( Glass* glass )
{
	hkPropertyValue pv;
#ifdef HAVOK3
	//pv.getPtr() = glass;
	pv.setPtr(glass);
#else
	pv.asPtr = glass;
#endif
	mHavokPart->Body->addProperty( HPGlass, pv );
}

Glass* PhysicsObject::GetGlass( void )
{
	Glass* glass = 0;
	if( mHavokPart->Body->hasProperty( HPGlass ) )
	{
		hkPropertyValue p = mHavokPart->Body->getProperty( HPGlass );
#ifdef HAVOK3
		glass = (Glass*)p.getPtr();
#else
		glass = (Glass*)p.asPtr;
#endif
	}
	return glass;
}

short PhysicsObject::GetActiveMeshIndex( void )
{
	HavokWorld* world = mHavokPart->World;
	int* index = world->PhysIndices.Find( this );
	return index?*index:-1;
}

Entity* PhysicsObject::GetEntity( void )
{
	if( mHavokPart->Body )
	{
		Entity* entity = 0;
		if( mHavokPart->Body->hasProperty( HPEntityPointer ) )
		{
			hkPropertyValue p = mHavokPart->Body->getProperty( HPEntityPointer );
#ifdef HAVOK3
			entity = (Entity*)p.getPtr();
#else
			entity = (Entity*)p.asPtr;
#endif
		}
		return entity;
	}
	return 0;
}

int PhysicsObject::GetType( void )
{
	return BodyType;
}

bool PhysicsObject::LineTrace( Vector& start, Vector& end, HitData& hitData )
{
	GLog.Print("PhysicsObject::LineTrace()...this one");
	hkVector4 startWorld( start.X, start.Y, start.Z );
	hkVector4 endWorld( end.X, end.Y, end.Z );
	Assert( mHavokPart->Body );
#ifdef HAVOK3
	hkShapeRayCastInput rayCastInput;
	hkShapeRayCastOutput rayCastOutput;
#else
	hkShape::hkRayResults initResults;
#endif

	const hkTransform& trans = mHavokPart->Body->getTransform();
	hkVector4 shapefrom,shapeto;
	shapefrom.setTransformedInversePos( trans, startWorld );
	shapeto.setTransformedInversePos( trans, endWorld );

#ifdef HAVOK3
	rayCastInput.m_from = shapefrom; 
	rayCastInput.m_to = shapeto;
	if( mHavokPart->Body->getCollidable()->getShape()->castRay( rayCastInput, rayCastOutput ) )
	{
		hkVector4 intersectionPointWorld;
		intersectionPointWorld.setInterpolate4( startWorld, endWorld, rayCastOutput.m_hitFraction );
#else
	if( mHavokPart->Body->getCollidable().getShape()->castRay( shapefrom, shapeto, initResults ) )
	{
		hkVector4 intersectionPointWorld;
		intersectionPointWorld.setInterpolate4( startWorld, endWorld, initResults.m_mindist );
#endif
		hitData.HitPoint = Vector( intersectionPointWorld(0), intersectionPointWorld(1), intersectionPointWorld(2) );

		hkVector4 temp;
		temp = startWorld;
		temp.sub4( intersectionPointWorld );
		hitData.Dist = temp.length3();

		hkVector4 hitNormal;
#ifdef HAVOK3
		// BLAIR - the normal is now NORMALIZED instead of unnormalized...check this out!
		hitNormal.setRotatedDir( mHavokPart->Body->getRotation(), rayCastOutput.m_normal );
#else
		hitNormal.setRotatedDir( mHavokPart->Body->getRotation(), initResults.m_unnormalizedHitNormal );
#endif
		hitNormal.normalize3();
		hitData.Normal = Vector( hitNormal(0), hitNormal(1), hitNormal(2) );
		hitData.HavokEntity = mHavokPart->Body;
		hitData.UserData = GetEntity();

		return true;
	}
	return false;
}

void PhysicsObject::FixGrenadeFlight( Entity* entity )
{
#ifndef HAVOK3 // ================================================

	if( !entity || !mHavokPart->Body || !mHavokPart->Body->getSimulationIsland() ) return;

	Vector lastPosition = entity->GetPosition();
	Vector np = GetPosition();
	if( lastPosition == np ) return;			// more than one update in a tick?

	hkVector4 oldPos( lastPosition.X, lastPosition.Y, lastPosition.Z );
	hkVector4 newPos( np.X, np.Y, np.Z );
	hkVector4 vel = mHavokPart->Body->getLinearVelocity();

	bool collides = CanLineTraceCollision();
	if( collides ) EnableLineTraceCollision( false );

	hkVector4 hitPoint;
	hkVector4 hitNormal;

	bool client = ( GEngine->Net && !GEngine->Net->IsServer() );

	for( int count = 0; count < 10; ++count )	// safe-exit measure
	{
		hkBroadPhase* broadPhase = mHavokPart->World->World->getBroadPhase();
		hkShape::hkRayResults initResults;

		havokGrenadeFixCallback rayCallBack( oldPos, newPos, initResults, client );
		broadPhase->castPrimitive( oldPos, newPos, rayCallBack );
		initResults = rayCallBack.getRayResults();
		hkCollidable* coll = rayCallBack.getHitObject();

		if( coll == HK_NULL )
		{
			if( collides ) EnableLineTraceCollision( true );
			Vector nnp = Vector( newPos(0), newPos(1), newPos(2) );
			SetPosition( nnp );
			if( count )
			{
				float friction = GetFriction();
				vel.mul4( 1.6f-friction );
#ifdef	TRANSGAMING
				GLog.Print( "Frame %d: grenade trace collided %d times\n", GEngine->pcfWorld->Frame, count );
#else
				GLog.Print( "Frame %d: grenade trace collided %d times\n", GEngine->World->Frame, count );
#endif
			}
			mHavokPart->Body->setLinearVelocity( vel );
			return;
		}

		hitPoint.setInterpolate4( oldPos, newPos, initResults.m_mindist );
		hitNormal.setRotatedDir( ((hkRigidBody*)coll->getOwner())->getRotation(), initResults.m_unnormalizedHitNormal );
		hitNormal.normalize3();

		// Give collision to script
		if( GetCollisionCallbacks() )
		{
			hkEntity* ent = (hkEntity*)coll->getOwner();
			hkPropertyValue p = ent->getProperty( HPEntityPointer );
#ifdef HAVOK3
			GScript.QueueMessage("COLLISION_WITH_OTHER_ENTITY","%e,%f,%f,%f,%f,%f,%f,%e,%p,%p", entity,
				hitPoint(0), hitPoint(1), hitPoint(2), hitNormal(0), hitNormal(1), hitNormal(2),
				(Entity*)p.getPtr(), mHavokPart->Body, ent );
#else
			GScript.QueueMessage("COLLISION_WITH_OTHER_ENTITY","%e,%f,%f,%f,%f,%f,%f,%e,%p,%p", entity,
				hitPoint(0), hitPoint(1), hitPoint(2), hitNormal(0), hitNormal(1), hitNormal(2),
				(Entity*)p.asPtr, mHavokPart->Body, ent );
#endif
		}

		// Calculate new velocity
		hkVector4 parallelTimes2 = hitNormal;
#ifdef	TRANSGAMING_PS2
		parallelTimes2.mul4( 2 * hkReal(vel.dot3(hitNormal)) );
#else
		parallelTimes2.mul4( 2 * vel.dot3(hitNormal)) );
#endif
		vel.sub4( parallelTimes2 );

		// Calculate new position
		hkVector4 hptoend( newPos );
		hptoend.sub4( hitPoint );
		parallelTimes2 = hitNormal;
#ifdef	TRANSGAMING_PS2
		parallelTimes2.mul4( 2*hkReal(hptoend.dot3(hitNormal)) );
#else
		parallelTimes2.mul4( 2*hptoend.dot3(hitNormal) );
#endif
		newPos.sub4( parallelTimes2 );
		hptoend.sub4( parallelTimes2 );

		// Update oldPos and adjust it a bit to newPos
		oldPos = hitPoint;
		float lenhptoend = hptoend.length3();
		if( lenhptoend > 0.002f )
			hptoend.mul4( 0.002f/lenhptoend );
		oldPos.add4( hptoend );

		// Next iteration
	}

	// Either 10 hits in a frame or algorithm lock-up
	GLog.Print( "Physics warning: either 10 hits in a frame or grenade flight fix algorithm lock-up\n" );
	if( collides ) EnableLineTraceCollision( true );
	Vector nnp = Vector( newPos(0), newPos(1), newPos(2) );
	SetPosition( nnp );
	vel.set(0,0,0);
	mHavokPart->Body->setLinearVelocity( vel );

#else
	GLog.Print("PhysicsObject::FixGrenadeFlight() - FIXME!!!!!!!!!!!\n");
#endif // ifndef HAVOK3

}

#ifdef TRANSGAMING_PSP
#include <tg_pc2psp.h>
#include <Engine/simpleTiming.h>
#endif

void PhysicsObject::FixPlayerMovement( Entity* entity )
{
//#ifndef TRANSGAMING
//	GLog.Print("PhysicsObject::FixPlayerMovement()\n");
//#endif
    //TRACE("PhysicsObject::FixPlayerMovement(): executing FixPlayerMovement() for player '%s' (filename = '%s', time = %gs)\n", entity->Name.AsChar(), entity->FileName.AsChar(), PSP_GetCurrentTimeInSeconds());
	if( !entity || !mPlayerVariables || !mHavokPart->Body || !mHavokPart->Body->getSimulationIsland() ) return;
#ifdef	TRANSGAMING
	if( mPlayerVariables->LastPlayerMovementFixFrame != GEngine->pcfWorld->Frame-1 )
	{
		mPlayerVariables->LastPlayerMovementFixFrame = GEngine->pcfWorld->Frame;
        //TRACE("PhysicsObject::FixPlayerMovement(): done fixing player movement [already fixed this frame] (time = %gs)\n", PSP_GetCurrentTimeInSeconds());

		return;
	}
	mPlayerVariables->LastPlayerMovementFixFrame = GEngine->pcfWorld->Frame;
#else
	if( mPlayerVariables->LastPlayerMovementFixFrame != GEngine->World->Frame-1 )
	{
		mPlayerVariables->LastPlayerMovementFixFrame = GEngine->World->Frame;
		return;
	}
	mPlayerVariables->LastPlayerMovementFixFrame = GEngine->World->Frame;
#endif

	hkVector4 newPos = mHavokPart->Body->getPosition();
#ifdef HAVOK3
    if( !newPos.isOk4() ){
        //TRACE("PhysicsObject::FixPlayerMovement(): done fixing player movement [bad newPos] (time = %gs)\n", PSP_GetCurrentTimeInSeconds());

        return;
    }
#else
	if( !newPos.isOk() ) return;
#endif
	Vector lastPosition = mPlayerVariables->LastGoodPosition;
	hkVector4 oldPos( lastPosition.X, lastPosition.Y, lastPosition.Z );
	Vector pivotOffset = GetPivotOffset();
	hkVector4 pivOff( pivotOffset.X, pivotOffset.Y, pivotOffset.Z );
	oldPos.sub4( pivOff );
#ifdef HAVOK3
    if( !oldPos.isOk4() ){
        //TRACE("PhysicsObject::FixPlayerMovement(): done fixing player movement [bad oldPos] (time = %gs)\n", PSP_GetCurrentTimeInSeconds());

        return;
    }
#else
	if( !oldPos.isOk() ) return;
#endif
    if( oldPos.equals3( newPos ) ){
        //TRACE("PhysicsObject::FixPlayerMovement(): done fixing player movement [multiple updates] (time = %gs)\n", PSP_GetCurrentTimeInSeconds());

        return;			// more than one update in a tick?
    }

    //TRACE("PhysicsObject::FixPlayerMovement(): doing ray casts (time = %gs)\n", PSP_GetCurrentTimeInSeconds());
	hkVector4 vel = mHavokPart->Body->getLinearVelocity();

	// Disable PO collision if neccessary
	bool collides = CanLineTraceCollision();
	if( collides ) EnableLineTraceCollision( false );

	// Disable ragdoll collision if neccessary
	bool ragdollCollides = false;
	Model* ragM = entity->AsModel();
	Ragdoll* rag = 0;
	if( ragM )
	{
        //TRACE("PhysicsObject::FixPlayerMovement(): ragdolls???? (time = %gs)\n", PSP_GetCurrentTimeInSeconds());
		rag = ragM->RagdollPtr;
		if( rag && rag->CanLineTraceCollision() )
		{
			rag->EnableLineTraceCollision( false );
			ragdollCollides = true;
		}
        //TRACE("PhysicsObject::FixPlayerMovement(): done ragdolls???? (time = %gs)\n", PSP_GetCurrentTimeInSeconds());
	}

	hkVector4 hitPoint;
#ifdef HAVOK3
	hkWorldRayCastInput ray;
	{
		ray.m_from = oldPos;
		ray.m_to = newPos;
	}
	hkWorldRayCastOutput output;
	{
        //TRACE("PhysicsObject::FixPlayerMovement(): casting ray 1 (time = %gs)\n", PSP_GetCurrentTimeInSeconds());

		havokMovementFixRaycastCallback rayCallback( ray, &output );
		hkBroadPhase* broadPhase = mHavokPart->World->World->getBroadPhase();
		hkBroadPhase::hkCastRayInput inputRay;
		inputRay.m_from = ray.m_from;
		inputRay.m_toBase = &ray.m_to;
		//inputRay.m_aabbCacheInfo = HK_NULL;

		// Now cast the ray!
		broadPhase->castRay( inputRay, &rayCallback, hkSizeOf(havokMovementFixRaycastCallback) );
        //TRACE("PhysicsObject::FixPlayerMovement(): done casting ray 1 (time = %gs)\n", PSP_GetCurrentTimeInSeconds());
	}
	const hkCollidable* coll = output.m_rootCollidable;
#else
	hkBroadPhase* broadPhase = mHavokPart->World->World->getBroadPhase();
	hkShape::hkRayResults initResults;

	havokMovementFixRaycastCallback rayCallBack( oldPos, newPos, initResults );
	broadPhase->castPrimitive( oldPos, newPos, rayCallBack );
	initResults = rayCallBack.getRayResults();
	hkCollidable* coll = rayCallBack.getHitObject();
#endif
	if( coll == HK_NULL )
	{
		if( collides ) EnableLineTraceCollision( true );
		if( ragdollCollides ) rag->EnableLineTraceCollision( true );
        //TRACE("PhysicsObject::FixPlayerMovement(): done fixing player movement [no collision] (time = %gs)\n", PSP_GetCurrentTimeInSeconds());
		return;
	}

#ifdef HAVOK3
	hitPoint.setInterpolate4( oldPos, newPos, output.m_hitFraction );
#else
	hitPoint.setInterpolate4( oldPos, newPos, initResults.m_mindist );
#endif

#ifndef TRANSGAMING
	GLog.Print( "Player anti-wall-through fix applied. Old pos (%f,%f,%f), new pos (%f,%f,%f), hitpoint (%f,%f,%f)\n",
		oldPos(0),oldPos(1), oldPos(2), newPos(0), newPos(1), newPos(2), hitPoint(0), hitPoint(1), hitPoint(2) );
#endif

	if( ragdollCollides ) rag->EnableLineTraceCollision( true );
	if( collides ) EnableLineTraceCollision( true );

	Vector nnp = Vector( oldPos(0), oldPos(1), oldPos(2) );
	SetPosition( nnp );
	vel.set(0,0,0);

#ifndef TRANSGAMING
	GLog.Print( "FixPlayerMovement : Pos = ( %f %f %f ) | vel = ( %f %f %f ) \n", oldPos(0), oldPos(1), oldPos(2), vel(0), vel(1), vel(2) );
#endif
	mHavokPart->Body->setLinearVelocity( vel );
    //TRACE("PhysicsObject::FixPlayerMovement(): done fixing player movement (time = %gs)\n", PSP_GetCurrentTimeInSeconds());
}

#define PO_LOAD_SAVE_TOKEN 0x5e46

bool PhysicsObject::LoadPO( FileObject* f )
{
	if( !f->TestToken( PO_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: PhysicsObject::LoadPO()\n" );
		return false;
	}

	// We assume object is already created by some entity ( BodyType and BodyScale loaded by it and used by it to create this body )

	char freedomOfRot;
	Vector velocity;
	Vector angularVelocity;
	unsigned char collisionGroup;
	char addToWorld;
    char traceOn;
	DWORD state;
	unsigned char active;

	*f >> state >> freedomOfRot >> velocity >> angularVelocity >> collisionGroup >> active >> addToWorld >> traceOn;

	if( ( state & EPOHasOurDeactivator ) && !( State & EPOHasOurDeactivator ) )
		SetHardDeactivator();

	State = state;

	float mass, friction, restitution, linearDamping, angularDamping;
	char keyframed;
	*f >> mass >> friction >> restitution >> linearDamping >> angularDamping >> keyframed;
	if( mass != GetMass() ) SetMass( mass );
	if( friction != GetFriction() ) SetFriction( friction );
	if( restitution != GetRestitution() ) SetRestitution( restitution );
	if( linearDamping != GetLinearDamping() ) SetLinearDamping( linearDamping );
	if( angularDamping != GetAngularDamping() ) SetAngularDamping( angularDamping );

    EnableLineTraceCollision( traceOn?true:false );
	if( collisionGroup != GetCollisionGroup() ) SetCollisionGroup( collisionGroup );
	if( addToWorld && !IsEnabled() )
		Enable( true );

	if( freedomOfRot != GetFreedomOfRotation() ) SetFreedomOfRotation( freedomOfRot );
	if( keyframed ) SetPinned( true );
	if( velocity != GetVel() ) SetVel( velocity );
	if( angularVelocity != GetAngularVel() ) SetAngularVel( angularVelocity );

	*f >> mInertiaTensorMultiplier;

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
	mHavokPart->Body->setInertiaLocal( inertiaTensor );

    // Load monster variable
    if (State&EPOIsMonster)
    {
        *f >> mHowFarICanSee;
        *f >> mHowFarICanSee360;
        *f >> mViewAngleYaw;
        *f >> mViewAnglePitch;
        *f >> mMoveVector;
        *f >> floorNormal;
        *f >> havokInfluenceInMonsterMovement;
        *f >> doNotCheckFloors;
        *f >> onFloor;
    }

	// Load player variables if we have them
	char temp;
	*f >> temp;
	if( temp )
	{
		mPlayerVariables->Load( f, this );
		mPlayerVariables->LastGoodPosition = GetPosition();
	}

	Activate( (active != 0) );

	return mHavokPart->LoadActions( f );
}

bool PhysicsObject::SavePO( FileObject* f )
{
    // Save things to be loaded by our entity during load ( entity will use them to create us )
	if( State & EPOCreatedWithNoScale )
	    *f << BodyType << (float)-1.0f;
	else
	    *f << BodyType << BodyScale;

    if( !f->PutToken( PO_LOAD_SAVE_TOKEN ) ) return false;

	char freedomOfRot = GetFreedomOfRotation();
	Vector velocity = GetVel();
	Vector angularVelocity = GetAngularVel();
	unsigned char collisionGroup = GetCollisionGroup();
	unsigned char active = mHavokPart->Body->isActive()?1:0;

	// Save things to be loaded by us during load
	*f << State << freedomOfRot << velocity << angularVelocity << collisionGroup << active;

    // Save in-world state
    char addToWorld = mHavokPart->Body->getSimulationIsland()?1:0;
    *f << addToWorld;

    // Save trace state
    char traceOn = CanLineTraceCollision();
    *f << traceOn;

	// Save various internal data
	float mass = GetMass();
	float friction = GetFriction();
	float restitution = GetRestitution();
	float linearDamping = GetLinearDamping();
	float angularDamping = GetAngularDamping();
	char keyframed = IsPinned()?1:0;
	*f << mass << friction << restitution << linearDamping << angularDamping << keyframed;
	*f << mInertiaTensorMultiplier;

	// Save limb inertia tensor
	hkMatrix3 inertiaTensor;
	mHavokPart->Body->getInertiaLocal( inertiaTensor );
	hkVector4 row0, row1, row2;
	inertiaTensor.getRows( row0, row1, row2 );
	float x,y,z;
	x = row0(0); y = row0(1); z = row0(2);
	*f << x << y << z;
	x = row1(0); y = row1(1); z = row1(2);
	*f << x << y << z;
	x = row2(0); y = row2(1); z = row2(2);
	*f << x << y << z;

    // Save monster variables
    if (State&EPOIsMonster)
    {
        *f << mHowFarICanSee;
        *f << mHowFarICanSee360;
        *f << mViewAngleYaw;
        *f << mViewAnglePitch;
        *f << mMoveVector;
        *f << floorNormal;
        *f << havokInfluenceInMonsterMovement;
        *f << doNotCheckFloors;
        *f << onFloor;
    }

	// Save player variables if we have them
	char temp = ( mPlayerVariables != 0 )?1:0;
	*f << temp;
	if( mPlayerVariables )
		mPlayerVariables->Save( f );

	// Save actions
	return mHavokPart->SaveActions( f );
}

#ifdef TRANSGAMING
void PhysicsObject::SetPlayerShocked( float timeToBeShocked )
#else
inline void PhysicsObject::SetPlayerShocked( float timeToBeShocked )
#endif
{
	if( !mPlayerVariables ) return;
	mPlayerVariables->TimeToBeShocked = timeToBeShocked;
}

void PhysicsObject::SetNotPredictionAffected( void )
{
	Assert( mHavokPart->Body );
	hkPropertyValue p;
#ifdef HAVOK3
	p.setInt(1);
#else
	p.asInt = 1;
#endif
	mHavokPart->Body->addProperty( HPDontSaveThis, p );
}

void PhysicsObject::SetSynchronizedProp( void )
{
	Assert( mHavokPart->Body );
	hkPropertyValue p;
#ifdef HAVOK3
	p.setInt(1);
#else
	p.asInt = 1;
#endif
	mHavokPart->Body->addProperty( HPSynchronized, p );
}

inline void PhysicsObject::MPPredRemoveFromBatch( bool addToWorld ) { mHavokPart->World->MPPredRemoveFromBatch( mHavokPart->Body, addToWorld ); }

void PhysicsObject::MultiPlayerActionNonPhysic( float yaw, float pitch, float timeDelta )
{
	int i = GProfiler.GetState();
	GProfiler.SetState( HAVOK_INDEPENDENT_PLAYER_ACTION );

	Assert( mPlayerVariables && mPlayerVariables->Controller );
	HavokCharacterController* c = mPlayerVariables->Controller;
	c->setAction( Action );
	c->addTime( timeDelta );
	c->setLookDirection( yaw, pitch );
	c->setPlayerBody( mHavokPart->Body );
	c->playerMove();
	Action = 0;

	GProfiler.SetState(i);
}

void PhysicsObject::MultiPlayerActionNonPhysic( PlayerInput* input, bool setRotation, bool enablePredictionFromThisState )
{
	int i = GProfiler.GetState();
	GProfiler.SetState( HAVOK_INDEPENDENT_PLAYER_ACTION );

	Assert( mPlayerVariables && mPlayerVariables->Controller );
	HavokCharacterController* c = mPlayerVariables->Controller;
	c->LastMove = *input;
	c->setPlayerBody( mHavokPart->Body );
	c->playerMove( true );
	if( setRotation )
		c->setPlayerBodyRotation(false);
	if( enablePredictionFromThisState )
		c->setStateReceivedTime( GEngine->SysDriver.GetCurrentTimeMS() );
	Action = 0;

	GProfiler.SetState(i);
}

void PhysicsObject::MultiPlayerActionSetupPredictionTime( DWORD time )
{
	Assert( mPlayerVariables && mPlayerVariables->Controller );
	mPlayerVariables->Controller->setStateReceivedTime( time );
}

void PhysicsObject::MultiPlayerGetOrientFromController( void )
{
	Assert( mPlayerVariables && mPlayerVariables->Controller );
	HavokCharacterController* c = mPlayerVariables->Controller;
	c->setPlayerBody( mHavokPart->Body );
	c->setPlayerBodyRotation(true);
	hkVector4 zero(0,0,0);
	c->addVelocity( zero );	// to transfer velocity
}

short PhysicsObject::GetPlayerPitch( void )
{
	return ( mPlayerVariables && mPlayerVariables->Controller )?mPlayerVariables->Controller->LastMove.GetPitch():0;
}

void PhysicsObject::SetMPByte( unsigned char mpbyte )
{
	if( mPlayerVariables && mPlayerVariables->Controller )
	{
		if( mPlayerVariables->Controller->MPByte != mpbyte )
			mPlayerVariables->Controller->addExternalInfluence();
		mPlayerVariables->Controller->MPByte = mpbyte;
	}
}

unsigned char PhysicsObject::GetMPByte( void )
{
	return ( mPlayerVariables && mPlayerVariables->Controller )?mPlayerVariables->Controller->MPByte:0;
}

void PhysicsObject::AddMPExternalInfluence( void )
{
	if( mPlayerVariables && mPlayerVariables->Controller )
		mPlayerVariables->Controller->addExternalInfluence();
}

float PhysicsObject::GetDistanceFromPoint( Vector point )
{
	if( mPlayerVariables )
	{
		Vector lastPos = mPlayerVariables->LastGoodPosition;
		Vector curPos = GetPosition();
		Vector deltaPos = curPos - lastPos;
		Vector toPoint = point - lastPos;
		float dot = deltaPos.Dot( toPoint );
		if( dot <= 0 )
			return (lastPos-point).Magnitude();
		float deltaPosLen = deltaPos.Magnitude();
		Vector x( deltaPos );
		x *= (dot/deltaPosLen/deltaPosLen);
		if( dot/deltaPosLen >= deltaPosLen )
			return (curPos-point).Magnitude();
		x += lastPos;
		return (x-point).Magnitude();
	}
	else
		return (GetPosition()-point).Magnitude();
}

float PhysicsObject::GetCameraFix( void )
{
	if( !mPlayerVariables || !mPlayerVariables->Controller )
		return 0.f;
	return mPlayerVariables->Controller->m_cameraVerticalCatchUp;
}

void PhysicsObjectPlayerVariables::Load( FileObject* f, PhysicsObject* ourObject )
{
	*f >> AccumulatedDelta;

	unsigned char temp;
	*f >> temp;
	AtSteps =			(( temp & 0x01 ) != 0 );
	Falling =			(( temp & 0x02 ) != 0 );
	CanBunnyHop =		(( temp & 0x04 ) != 0 );
	DoubleJumped =		(( temp & 0x08 ) != 0 );
	Exploded =			(( temp & 0x10 ) != 0 );
	JumpInvalidation =	(( temp & 0x20 ) != 0 );
	JumpPressed =		(( temp & 0x40 ) != 0 );

	if( temp & 0x80 )
	{
		Assert( !FakeBulletTimeGravity );
		// Set up bullet time gravity
		hkVector4 gravity = ourObject->mHavokPart->World->World->getGravity();
        m_setTag("Physics");
		FakeBulletTimeGravity = new havokAntiGravityAction( ourObject->mHavokPart->Body, gravity );
		ourObject->mHavokPart->World->World->addAction( (havokAntiGravityAction*)FakeBulletTimeGravity );
	}

	*f >> temp;
	BulletTimeShock =	(( temp & 0x01 ) != 0 );

	float dummyFloat;

	*f >> TimeSinceJumped;
	*f >> RealTimeSinceJumped;
	*f >> TimeSinceLanded;
	*f >> JumpVector;
	*f >> JumpAction;
	*f >> LastNonzeroAction;
	*f >> TimeToBeShocked;
	*f >> TimeSinceExplosion;
	*f >> TimeSinceBunnyHop;
	*f >> LastExecutionSpeed;
	*f >> Speed;
	*f >> OnSlopeTimes;
	*f >> TimeModifierLastTime;
	*f >> HighestSpeedOfGoingOut;
	*f >> FramesInRocksAndNotGoingOut;
	*f >> TimeSinceLastOnGround;
	*f >> SpeedWhenLanded;
	*f >> dummyFloat;
	*f >> dummyFloat;
	*f >> dummyFloat;
	*f >> LowestYSpeed;

	*f >> LastGoodPosition;
	DWORD fr;
	*f >> fr;
#ifdef	TRANSGAMING
	LastPlayerMovementFixFrame = GEngine->pcfWorld->Frame-fr;
#else
	LastPlayerMovementFixFrame = GEngine->World->Frame-fr;
#endif

	*f >> temp;
	RevertedToLastGoodPositionLastFrame =	(( temp & 0x01 ) != 0 );
	StickToUnderBodyPending =				(( temp & 0x02 ) != 0 );
	StickToUnderBody = false;
}

void PhysicsObjectPlayerVariables::Save( FileObject* f )
{
	*f << AccumulatedDelta;

	unsigned char temp = 0;
	if( AtSteps ) temp |= 0x01;
	if( Falling ) temp |= 0x02;
	if( CanBunnyHop ) temp |= 0x04;
	if( DoubleJumped ) temp |= 0x08;
	if( Exploded ) temp |= 0x10;
	if( JumpInvalidation ) temp |= 0x20;
	if( JumpPressed ) temp |= 0x40;
	if( FakeBulletTimeGravity ) temp |= 0x80;
	*f << temp;

	temp = 0;
	if( BulletTimeShock ) temp |= 0x01;
	*f << temp;

	float dummy = 0;

	*f << TimeSinceJumped;
	*f << RealTimeSinceJumped;
	*f << TimeSinceLanded;
	*f << JumpVector;
	*f << JumpAction;
	*f << LastNonzeroAction;
	*f << TimeToBeShocked;
	*f << TimeSinceExplosion;
	*f << TimeSinceBunnyHop;
	*f << LastExecutionSpeed;
	*f << Speed;
	*f << OnSlopeTimes;
	*f << TimeModifierLastTime;
	*f << HighestSpeedOfGoingOut;
	*f << FramesInRocksAndNotGoingOut;
	*f << TimeSinceLastOnGround;
	*f << SpeedWhenLanded;
	*f << dummy;	// MotorSpeed
	*f << dummy;	// MotorOrientation
	*f << dummy;	// MotorRoll
	*f << LowestYSpeed;

	*f << LastGoodPosition;
#ifdef	TRANSGAMING
	DWORD fr = GEngine->pcfWorld->Frame-LastPlayerMovementFixFrame;
#else
	DWORD fr = GEngine->World->Frame-LastPlayerMovementFixFrame;
#endif
	*f << fr;

	temp = 0;
	if( RevertedToLastGoodPositionLastFrame ) temp |= 0x01;
	if( StickToUnderBodyPending || StickToUnderBody ) temp |= 0x02;
	*f << temp;
}

PhysicsObjectPlayerVariables& PhysicsObjectPlayerVariables::operator=( const PhysicsObjectPlayerVariables& other )
{
	AccumulatedDelta = other.AccumulatedDelta;
	AtSteps = other.AtSteps;
	Falling = other.Falling;
	CanBunnyHop = other.CanBunnyHop;
	DoubleJumped = other.DoubleJumped;
	Exploded = other.Exploded;
	JumpInvalidation = other.JumpInvalidation;
	JumpPressed = other.JumpPressed;
	FakeBulletTimeGravity = 0;						// no additional gravity in MP
	BulletTimeShock = other.BulletTimeShock;
	TimeSinceJumped = other.TimeSinceJumped;
	RealTimeSinceJumped = other.RealTimeSinceJumped;
	TimeSinceLanded = other.TimeSinceLanded;
	JumpVector = other.JumpVector;
	JumpAction = other.JumpAction;
	LastNonzeroAction = other.LastNonzeroAction;
	TimeToBeShocked = other.TimeToBeShocked;
	TimeSinceExplosion = other.TimeSinceExplosion;
	TimeSinceBunnyHop = other.TimeSinceBunnyHop;
	LastExecutionSpeed = other.LastExecutionSpeed;
	Speed = other.Speed;
	OnSlopeTimes = other.OnSlopeTimes;
	TimeModifierLastTime = other.TimeModifierLastTime;
	HighestSpeedOfGoingOut = other.HighestSpeedOfGoingOut;
	FramesInRocksAndNotGoingOut = other.FramesInRocksAndNotGoingOut;
	TimeSinceLastOnGround = other.TimeSinceLastOnGround;
	SpeedWhenLanded = other.SpeedWhenLanded;
	LowestYSpeed = other.LowestYSpeed;
	LastGoodPosition = other.LastGoodPosition;
	LastPlayerMovementFixFrame = 0;					// this should not be copied
	RevertedToLastGoodPositionLastFrame = other.RevertedToLastGoodPositionLastFrame;
	StickToUnderBodyPending = false;				// no underbodies in MP
//	YCorrection = other.YCorrection;
	return *this;
}
