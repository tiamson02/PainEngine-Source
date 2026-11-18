
float PhysicsObjectPlayerVariables::ReportFallDamage( float speedNow )
{
	if( LastExecutionSpeed.Y < speedNow )
	{
		if( LowestYSpeed == 0.0f )
			LowestYSpeed = LastExecutionSpeed.Y;
		return 0;
	}
	else
	{
		if( LowestYSpeed == 0.0f )
			return 0;
		else
		{
			float diff = -LowestYSpeed;
			if( diff < 0.0f ) diff = 0.0f;
			LowestYSpeed = 0.0f;
			return diff;
		}
	}
}

void PhysicsObject::SetPlayerShocked( void )
{
	if( mPlayerVariables )
	{
		if( mPlayerVariables->Controller )
			mPlayerVariables->Controller->m_msSinceExplosion = 0;
		else
			mPlayerVariables->TimeSinceExplosion = 0.0f;
	}
}

bool PhysicsObject::JumpedInLastAction( void )
{
	if( mPlayerVariables )
	{
		if( mPlayerVariables->Controller )
			return !mPlayerVariables->Controller->m_msSinceJumped;
		else
			return ( mPlayerVariables->RealTimeSinceJumped == 0.0f );
	}
	return false;
}

void PhysicsObject::DetachPlayerFromUnderbody( void )
{
	if( mPlayerVariables )
	{
		mPlayerVariables->StickToUnderBodyPending = false;
		if( mPlayerVariables->StickToUnderBody )
		{
			mPlayerVariables->StickToUnderBody = false;
			((hkRigidBody*)mPlayerVariables->UnderBody)->removeReference();
			mPlayerVariables->UnderBody = 0;
		}
	}
}

void PhysicsObject::MovePlayerOutOfWall( void )
{
	PhysicsObjectPlayerVariables* pv = mPlayerVariables;
	float hit;

	if( !FloorCheck( -5.4f, 5.4f, &hit, true ) )
	{
		pv->LastGoodPosition = GetPosition();
		return;
	}

	bool reverted = pv->RevertedToLastGoodPositionLastFrame;
	pv->RevertedToLastGoodPositionLastFrame = false;
	hkVector4 here = mHavokPart->Body->getPosition();
	hkVector4 velocity = mHavokPart->Body->getLinearVelocity();
	float diffDown = (1.0f-hit)*10.8f;
	float diffUp = diffDown-11;

	// Try to move up
	if( ( velocity(1) < 3.0f ) && ( hit > 0.4f ) && !FloorCheck( -5.3f+diffDown, 5.4f+diffDown, 0, true ) )
	{
		here(1) += diffDown*BodyScale*OLD_PAWN_SCALE;
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !here.isOk4() || ( fabs(here(0)) > 10000.0f ) || ( fabs(here(1)) > 10000.0f ) || ( fabs(here(2)) > 10000.0f ) )
#else
		if( !here.isOk() || ( fabs(here(0)) > 10000.0f ) || ( fabs(here(1)) > 10000.0f ) || ( fabs(here(2)) > 10000.0f ) )
#endif
			GLog.Print( "Position > 10000 in MovePlayerOutOfWall\n" );
#endif
		mHavokPart->Body->setPosition( here );
		pv->LastGoodPosition = GetPosition();
		return;
	}

	// Try to move down
	if( ( velocity(1) > -3.0f ) && ( hit < 0.6f ) && !FloorCheck( -5.4f+diffUp, 5.3f+diffUp, 0, true ) )
	{
		here(1) += diffUp*BodyScale*OLD_PAWN_SCALE;
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !here.isOk4() || ( fabs(here(0)) > 10000.0f ) || ( fabs(here(1)) > 10000.0f ) || ( fabs(here(2)) > 10000.0f ) )
#else
		if( !here.isOk() || ( fabs(here(0)) > 10000.0f ) || ( fabs(here(1)) > 10000.0f ) || ( fabs(here(2)) > 10000.0f ) )
#endif
			GLog.Print( "Position > 10000 in MovePlayerOutOfWall\n" );
#endif
		mHavokPart->Body->setPosition( here );
		pv->LastGoodPosition = GetPosition();
		return;
	}

	// Revert to last known good position and zero speed
	if( !reverted )
	{
		if( velocity.length3() > GEngine->Physics->TWPlayerSpeed*1.5f )
		{
#ifdef HAVOK3
# ifdef TRANSGAMING
            TRACE("MovePlayerOutOfWall : %f < %f\n", velocity(1), GEngine->Physics->TWPlayerSpeed );
# else
            const int BUFSIZE = 256;
		    char velocityCheck[BUFSIZE];
		    snprintf( velocityCheck, BUFSIZE, "MovePlayerOutOfWall : %f < %f\n", velocity(1), GEngine->Physics->TWPlayerSpeed );
		    GLog.Print(velocityCheck);
# endif
#endif
			if( velocity(1) < GEngine->Physics->TWPlayerSpeed )	// else with all probability we just jumped on a jumppad
			{
				hkVector4 zero(0,0,0);
				mHavokPart->Body->setLinearVelocity( zero );
			}
			SetPosition( pv->LastGoodPosition );
			pv->RevertedToLastGoodPositionLastFrame = true;
		}
	}

	pv->LastGoodPosition = GetPosition();
	return;
}

void PhysicsObject::PlayerAction( Vector& forwardVector, Vector& rightVector )
{
    onLadder = false;

	if( !mPlayerVariables || !mHavokPart->Body || !mHavokPart->Body->getSimulationIsland() )
	{
		Action = 0;
		return;
	}

	PhysicsObjectPlayerVariables* pv = mPlayerVariables;
	if( GEngine->Physics->oldPlayerSpeed != GEngine->Physics->TWPlayerSpeed )
		if( pv->Speed == GEngine->Physics->oldPlayerSpeed )
			pv->Speed = GEngine->Physics->TWPlayerSpeed;
	GEngine->Physics->canForgetOPS = true;

	hkRigidBody* body = mHavokPart->Body;
#ifdef HAVOK3
	body->activate();
#else
	body->forceActivate();
#endif
	if( pv->Speed == 0.0f ) pv->Speed = GEngine->Physics->TWPlayerSpeed;

	// Only those actions interest us
	Action &= ((1<<EInputJump)|(1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));

	if( pv->TimeToBeShocked != 0.0f )
		pv->BulletTimeShock = true;

	// Gravity Modifier - This is to ensure that player moves better in bullet time
	if( ( GEngine->TimeMultiplier != 1.0f ) && !pv->BulletTimeShock )
	{
		havokAntiGravityAction* antigrav = (havokAntiGravityAction*)pv->FakeBulletTimeGravity;
		hkVector4 gravity = mHavokPart->World->World->getGravity();
		if( !antigrav )
		{
            m_setTag("Physics");
			pv->FakeBulletTimeGravity = new havokAntiGravityAction( mHavokPart->Body, gravity );
			antigrav = (havokAntiGravityAction*)pv->FakeBulletTimeGravity;
			mHavokPart->World->World->addAction( antigrav );
		}
		hkVector4 mulGravity(0,0,0);
		mulGravity.setMul4( -1.0/GEngine->TimeMultiplier/GEngine->TimeMultiplier + 1.0, gravity );
		hkVector4& gravityNow = antigrav->getGravity();
		gravityNow = mulGravity;
	}
	else
	{
		if( pv->BulletTimeShock )
			Action &= ~(1<<EInputJump);

		if( pv->FakeBulletTimeGravity )
		{
			havokAntiGravityAction* antigrav = (havokAntiGravityAction*)pv->FakeBulletTimeGravity;
			if( antigrav->getSimulationIsland() )
				mHavokPart->World->World->removeAction( antigrav );
			antigrav->removeReference();
			pv->FakeBulletTimeGravity = 0;
		}
	}

	// Calculate velocity
	hkVector4 velocity = body->getLinearVelocity();

	// If we're on ladder, use this
#ifdef	TRANSGAMING
	const bool nearLadder = GEngine->pcfWorld->NearLadder( GetPawnFloorPos() ) || GEngine->pcfWorld->NearLadder( GetPawnHeadPos() );
#else
	const bool nearLadder = GEngine->World->NearLadder( GetPawnFloorPos() ) || GEngine->World->NearLadder( GetPawnHeadPos() );
#endif
	if( nearLadder && !(Action&(1<<EInputJump)) )
	{
        onLadder = true;
		PlayerActionLadder( forwardVector, rightVector );
		Action = 0;
		pv->LastExecutionSpeed = Vector( velocity(0), velocity(1), velocity(2) );
		return;
	}

	// If we're shocked, do this
	if( pv->TimeToBeShocked != 0.0f )
	{
		// Our time delta
		float actionDelta = pv->AccumulatedDelta;
		if( actionDelta > 1.0f ) actionDelta = 0.05f;
		pv->AccumulatedDelta = 0;
		pv->TimeToBeShocked -= actionDelta;
		if( pv->TimeToBeShocked < 0.0f )
			pv->TimeToBeShocked = 0.0f;

		// Give landing data to script
		float fallDamage = pv->ReportFallDamage( velocity(1) );
		if( fallDamage*GEngine->TimeMultiplier > 20.0f  )
			GScript.QueueMessage("PLAYER_HIT_GROUND", "%e,%f", GetEntity(), -fallDamage*GEngine->TimeMultiplier );

		Action = 0;
		pv->LastExecutionSpeed = Vector( velocity(0), velocity(1), velocity(2) );
		return;
	}

	// If we're in the ground, jump out of it
	MovePlayerOutOfWall();

	// If we're on ice, notice this
#ifdef	TRANSGAMING
	const bool onIce = GEngine->pcfWorld->OnIce( GetPawnFloorPos() );
#else
	const bool onIce = GEngine->World->OnIce( GetPawnFloorPos() );
#endif
//	GLog.Print( onIce?"Ice: yes\n":"Ice: no\n" );

	// ************************************************************
	// FROM HERE WE START MODIFYING
	// ************************************************************

	// Calculate linear speed relative to the floor
	hkRigidBody* underBody = (hkRigidBody*)MeshUnder(0.5f*BodyScale*OLD_PAWN_SCALE);
	hkVector4 underBodySpeed(0,0,0);
	if( underBody )
	{
		bool velCh = false;
		if( underBody->hasProperty( HPEntityPointer ) )	// Transporter?
		{
			hkPropertyValue p = underBody->getProperty( HPEntityPointer );
#ifdef HAVOK3
			Entity* entity = (Entity*)p.getPtr();
#else
			Entity* entity = (Entity*)p.asPtr;
#endif
			if( entity && entity->PO )
			{
				havokTransporterAction* transpAction = (havokTransporterAction*)entity->PO->mHavokPart->GetAction( POHavokPart::EActionTransporter );
				if( transpAction )
				{
					hkVector4 trVelocity;
					float strength;
					transpAction->getInfo( trVelocity, strength );
					velocity.sub4( trVelocity );
					velCh = true;
				}
			}
		}

		if( !velCh && !underBody->isFixed() )
		{
			underBodySpeed = underBody->getLinearVelocity();
			velocity.sub4( underBodySpeed );
			if( pv->StickToUnderBodyPending )
			{
				pv->StickToUnderBody = true;
				pv->StickToUnderBodyPending = false;
				pv->UnderBody = (void*)underBody;
				hkVector4 ubPos = underBody->getPosition();
				hkVector4 plPos = mHavokPart->Body->getPosition();
				plPos.sub4( ubPos );
				pv->StickPosDiff.X = plPos(0); pv->StickPosDiff.Y = plPos(1); pv->StickPosDiff.Z = plPos(2);
				underBody->addReference();
			}
		}
	}

	// Disable jump ability when falling too fast
	if( ( velocity(1) < GEngine->Physics->TWPlayerMinimalFallSpeedToDisableJump ) && !pv->JumpInvalidation )
		Action &= ~(1<<EInputJump);

	// Make sure gravity works
	EnableGravity(true);

	// ***********************************************************
	// GET SOME VARIABLES FIRST, AND FIND OUT WHERE WE ARE
	// ************************************************************

//	String debugPrint = String::Sprintf( "Vel Y: %f ", velocity(1) );

	// Some easy variables
	const bool wasFalling = pv->Falling;
	bool hitFloor = FloorCheck() || FloorCheckRandom();
	bool strongAirControl = ( (Action & (1<<EInputMoveForward)) != 0 );
	bool falling = pv->Falling = !hitFloor && !pv->AtSteps && !nearLadder;

	if( !falling && ( pv->TimeToBeShocked == 0.0f ) )
		pv->BulletTimeShock = false;

	// Check if we are on the slope
	bool hitSlope = true;
	if( !hitFloor ) hitSlope = FloorCheck( -10.0f );	//to update floor normal
	if( hitSlope )
	{
		const Vector upVector(0,1,0);
		float dot = floorNormal.Dot( upVector );
//		GLog.Print( "Slope: %f degrees\n", acos( dot )/F_DEG2RAD );
		if( dot < cos( GEngine->Physics->TWSlopeAngleToSlide*F_DEG2RAD ) )
		{
			if( pv->OnSlopeTimes < 10 )
				pv->OnSlopeTimes++;
		}
		else
			pv->OnSlopeTimes = 0;
	}
	else if( pv->OnSlopeTimes > 0 )
		pv->OnSlopeTimes--;

	bool sliding = false;
	if( pv->OnSlopeTimes > 5 )
	{
		sliding = true;
		strongAirControl = false;
		falling = true;
	}

//	debugPrint += (falling?"falling, ":"notFalling, ");
//	debugPrint += (hitFloor?"hitfloor, ":"noHitFloor, ");
	if( pv->TimeSinceExplosion < GEngine->Physics->TWPlayerShockedTime ) hitFloor = false;
	hkVector4 fwdVel( velocity );
	fwdVel(1) = 0;
	bool wasNotJumping = ( pv->TimeSinceJumped < 0.0f );
//	debugPrint += (wasNotJumping?"wasNotJumping, ":"wasJumping, ");
//	debugPrint += (wasFalling?"wasFalling, ":"wasNotFalling, ");
	const float jumpStrength = GEngine->Physics->TWPlayerJumpStrength;
	const bool pressedJump = ( (Action & (1<<EInputJump)) != 0 );
	if( !pressedJump ) pv->JumpInvalidation = false;
//	debugPrint += (pressedJump?"pressedJump, ":"noJump, ");

	if( pv->StickToUnderBody )
	{
		if( pressedJump )
		{
			((hkRigidBody*)pv->UnderBody)->removeReference();
			pv->UnderBody = 0;
			pv->StickToUnderBody = false;
		}
		else
		{
			hkVector4 pos = ((hkRigidBody*)pv->UnderBody)->getPosition();
			hkVector4 posDiff( pv->StickPosDiff.X, pv->StickPosDiff.Y+0.1, pv->StickPosDiff.Z );
			pos.add4( posDiff );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !pos.isOk4() || ( fabs(pos(0)) > 10000.0f ) || ( fabs(pos(1)) > 10000.0f ) || ( fabs(pos(2)) > 10000.0f ) )
#else
			if( !pos.isOk() || ( fabs(pos(0)) > 10000.0f ) || ( fabs(pos(1)) > 10000.0f ) || ( fabs(pos(2)) > 10000.0f ) )
#endif // HAVOK3
				GLog.Print( "Position > 10000 in PlayerAction\n" );
#endif
			mHavokPart->Body->setPosition( pos );
			hkVector4 ubSpeed = ((hkRigidBody*)pv->UnderBody)->getLinearVelocity();
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !ubSpeed.isOk4() || ( fabs(ubSpeed(0)) > 1000.0f ) || ( fabs(ubSpeed(1)) > 1000.0f ) || ( fabs(ubSpeed(2)) > 1000.0f ) )
#else
			if( !ubSpeed.isOk() || ( fabs(ubSpeed(0)) > 1000.0f ) || ( fabs(ubSpeed(1)) > 1000.0f ) || ( fabs(ubSpeed(2)) > 1000.0f ) )
#endif // HAVOK3
				GLog.Print( "Underbody Velocity > 1000 in PlayerMove\n" );
#endif
			mHavokPart->Body->setLinearVelocity( ubSpeed );
			return;
		}
	}

    // Desired move
	hkVector4 move(0,0,0);

	const hkVector4 up( 0,1,0 );
	hkVector4 right( rightVector.X, 0, rightVector.Z );
	hkVector4 forward;
	forward.setCross( up, right );

    if(Action & (1<<EInputMoveForward)) move.add4( forward );
	if(Action & (1<<EInputMoveBackward)) move.sub4( forward );
	if(Action & (1<<EInputMoveRight)) move.add4( right );
	if(Action & (1<<EInputMoveLeft)) move.sub4( right );
#ifdef HAVOK3
	if (!( move(0) == 0.0f && move(1) == 0.0f && move(2) == 0.0f ))
		move.normalize3();
#else
	move.normalize3();
#endif
	bool moved = ( Action != 0 );
	mHavokPart->Body->getMaterial().setFriction( (onIce|moved)?0.0f:1.0f );

	// Our time delta
	float actionDelta = pv->AccumulatedDelta;
	if( actionDelta > 1.0f ) actionDelta = 0.05f;
	pv->AccumulatedDelta = 0;

	// Adjust movement variables
	pv->TimeSinceLanded += actionDelta;
	pv->RealTimeSinceJumped += actionDelta;
	// Adjust time since we pressed or released Jump button ( > 0 = time since presses, < 0 = time since released )
	if( pressedJump && ( pv->TimeSinceJumped >= 0 ) )		pv->TimeSinceJumped += actionDelta;
	else if( !pressedJump && ( pv->TimeSinceJumped <= 0 ) )	pv->TimeSinceJumped -= actionDelta;
	else													pv->TimeSinceJumped = 0.0f;

	// Detect double jump
	bool doubleJumped = ( wasNotJumping && pressedJump && wasFalling && !pv->DoubleJumped &&
		( fabs(velocity(1)) < GEngine->Physics->TWAbsoluteVerticalVelocityBelowWhichDoubleJumpHappens ) );
//	debugPrint += (doubleJumped?"doubleJumped, ":"notDoubleJumped, ");

	if( doubleJumped ) pv->DoubleJumped = true;

	// Detect any jump
	bool jumped = ( pressedJump && !wasFalling && !pv->JumpInvalidation ) || doubleJumped;
	if( jumped ) pv->RealTimeSinceJumped = 0.0f;
//	debugPrint += (jumped?"jumped, ":"notJumped, ");

	// Detect bunny hop
	bool bunnyHopped = ( pv->CanBunnyHop && jumped && !doubleJumped );
//	debugPrint += (bunnyHopped?"bunnyHopped, ":"notBunnyHopped, ");

	if( ( pv->TimeSinceJumped > GEngine->Physics->TWSecondsWhenYouCanBunnyHopBeforeLanding ) && falling )
		pv->JumpInvalidation = true;

	// ***********************************************************
	// NOW USING THIS DATA, DECIDE WHAT TO DO
	// ************************************************************

	// If BulletTime has just started or finished, adjust our real speed ( and then you can work some forces, like always )
	if( pv->TimeModifierLastTime != GEngine->TimeMultiplier )
	{
		hkVector4 realVel = body->getLinearVelocity();
		realVel.mul4( pv->TimeModifierLastTime/GEngine->TimeMultiplier );
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !realVel.isOk4() || ( fabs(realVel(0)) > 1000.0f ) || ( fabs(realVel(1)) > 1000.0f ) || ( fabs(realVel(2)) > 1000.0f ) )
#else
		if( !realVel.isOk() || ( fabs(realVel(0)) > 1000.0f ) || ( fabs(realVel(1)) > 1000.0f ) || ( fabs(realVel(2)) > 1000.0f ) )
#endif // HAVOK3
			GLog.Print( "Velocity > 1000 (2) in PlayerMove\n" );
#endif
		body->setLinearVelocity( realVel );
	}
	pv->TimeModifierLastTime = GEngine->TimeMultiplier;

	bool stillShocked = pv->TimeSinceExplosion < GEngine->Physics->TWPlayerShockedTime;

	// Prepare data for air control
	if( stillShocked )		// Started falling from explosion
	{
		pv->TimeSinceExplosion += actionDelta;
		pv->JumpVector = Vector( fwdVel(0), 0, fwdVel(2) );
		pv->JumpVector.Normalize();
		pv->JumpAction = 0;
		pv->Speed = fwdVel.length3();
		pv->Exploded = true;
	}
	else if( jumped )						// Just jumped
	{
		pv->CanBunnyHop = false;
		pv->JumpVector.X = move(0);
		pv->JumpVector.Y = move(1);
		pv->JumpVector.Z = move(2);
		pv->JumpAction = Action & ((1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));
	}
	else if( sliding )						// Sliding on something
	{
		pv->CanBunnyHop = false;
		pv->JumpVector.X = 0;
		pv->JumpVector.Y = 0;
		pv->JumpVector.Z = 0;
		pv->JumpAction = 0;
	}
	else if( falling && !wasFalling )		// Started falling normally... walked off something
	{
		pv->JumpVector = Vector( fwdVel(0), 0, fwdVel(2) );
		pv->JumpVector.Normalize();
		pv->JumpAction = Action & ((1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));
	}

	// Give landing data to script
	float fallDamage = pv->ReportFallDamage( velocity(1) );
#ifndef HAVOK3 // BLAIR - For the May 5th E3 demo, we won't need falling damage
	if( fallDamage*GEngine->TimeMultiplier > 20.0f  )
		GScript.QueueMessage("PLAYER_HIT_GROUND", "%e,%f", GetEntity(), -fallDamage*GEngine->TimeMultiplier );
#endif
	// Just landed
    if( wasFalling && hitFloor )
	{
		pv->Exploded = false;
		pv->DoubleJumped = false;	// you can double jump hop again
		pv->TimeSinceLanded = 0.0f;

		// Jumped just before landing? - bunny hop enable
		if( ( pv->TimeSinceJumped > 0 ) &&
			( pv->TimeSinceJumped < GEngine->Physics->TWSecondsWhenYouCanBunnyHopBeforeLanding ) && pressedJump )
			bunnyHopped = true;
		else
			pv->CanBunnyHop = true;

		// Conserve speed for bunny hops
#ifdef	TRANSGAMING_PS2
		pv->Speed = hkReal(fwdVel.length3())*GEngine->TimeMultiplier;
#else
		pv->Speed = fwdVel.length3()*GEngine->TimeMultiplier;
#endif
	}

	// Keep speed in proper boundaries
	if( pv->Speed > GEngine->Physics->TWMaximalBunnyHopSpeed ) pv->Speed = GEngine->Physics->TWMaximalBunnyHopSpeed;

	// Bunny Hop Acceleration and Stopping
	if( bunnyHopped )
	{
		pv->Exploded = false;
		pv->CanBunnyHop = false;
		if( pv->Speed < GEngine->Physics->TWMaximalBunnyHopSpeed )
			pv->Speed += (GEngine->Physics->TWMaximalBunnyHopSpeed-pv->Speed)*GEngine->Physics->TWBunnyHopAcceleration;
		else
			pv->Speed = GEngine->Physics->TWMaximalBunnyHopSpeed;
	}
	else if( ( pv->TimeSinceLanded > GEngine->Physics->TWSecondsWhenYouCanBunnyHopAfterLanding )
		&& hitFloor )
	{
		pv->Exploded = false;
		pv->CanBunnyHop = false;
		pv->Speed = GEngine->Physics->TWPlayerSpeed;
	}

	// Air control
	if( sliding )
	{
		move = fwdVel;
#ifdef HAVOK3
	if (!( move(0) == 0.0f && move(1) == 0.0f && move(2) == 0.0f ))
#endif
		move.normalize3();
	}
	else if( falling )
	{
		// Gather myJumpVector from JumpAction
		hkVector4 myPressVector(0,0,0);
		if(pv->JumpAction & (1<<EInputMoveForward)) myPressVector.add4( forward );
		if(pv->JumpAction & (1<<EInputMoveBackward)) myPressVector.sub4( forward );
		if(pv->JumpAction & (1<<EInputMoveRight)) myPressVector.add4( right );
		if(pv->JumpAction & (1<<EInputMoveLeft)) myPressVector.sub4( right );

		float jwl = pv->JumpVector.Magnitude();
		if( myPressVector.length3() )
			myPressVector.normalize3();
		else if( pv->Exploded )	// that means we're in the air because of explosion, i guess
			myPressVector = forward;

		float braker = -myPressVector.dot3(move);
		if( jwl && ( braker > 0.0f ) )
		{
			float subtract = GEngine->Physics->TWPlayerSlowdownDuringJump*actionDelta*braker*pv->Speed;
			while( pv->Speed <= subtract ) subtract /= 2;
			pv->Speed -= subtract;
			strongAirControl = true;
		}
		move = myPressVector;
	}
	else
	{
		// Keep speed in proper boundaries
		if( pv->Speed < GEngine->Physics->TWPlayerSpeed ) pv->Speed = GEngine->Physics->TWPlayerSpeed;
	}

	float speed = pv->Speed/GEngine->TimeMultiplier;

	int stepMeasure = StepCheck( Vector( move(0), move(1), move(2) ) );	// returns 0 to 4, 4 - highest = wall

//	String sm = String::Sprintf( "Step: %d", stepMeasure );
//	debugPrint += sm;
//	GLog.Print( "%s\n", debugPrint.AsChar() );

	hkVector4 vel;

#ifdef HAVOK_POSITION_BUG_TRACKING
	float myMass = body->getMass();
	if( myMass < 0.000001f )
	{
		Assert( myMass >= 0.0f );
		Entity* enti = GetEntity();
		GLog.Print( "Zero mass in PlayerAction ( entity '%s, filename '%s' )\n", enti?enti->Name.AsChar():"(null)", enti?enti->FileName.AsChar():"(null)" );
	}
#endif

	// When on a fast moving object, give the body additional force downward so it doesn't slide from it
	if( ( underBodySpeed.length3() > 5.0f ) && (pv->TimeSinceJumped <= 0) && ( velocity(1) > 0.0f ) )
	{
		hkVector4 imp( 0, -1.0f*body->getMass()*velocity(1), 0 );

#ifdef HAVOK_POSITION_BUG_TRACKING
		Assert( body->getMass() >= 0.0f );
#ifdef HAVOK3
		if( !imp.isOk4() )
#else
		if( !imp.isOk() )
#endif
			GLog.Print( "Corrupted force in PlayerAction (underbody)\n" );
		else if( imp.length3() > 50000.0f )
			GLog.Print( "Force > 50000 in PlayerAction (underbody)\n" );
#endif

		body->applyLinearImpulse( imp );
	}

	// Clamp jump strength when using speed+haste cards
	float jumpPower = 0.0f;
	if( jumped )
	{
		jumpPower = 0.7f*jumpStrength*GEngine->Physics->TWPlayerSpeed/GEngine->TimeMultiplier;
//		if( jumpPower > 50.0f ) jumpPower = 50.0f;
	}

	// Movement ( with steps )
	switch( stepMeasure )
	{
	case 4:
		// Full wall - jump upward only
		// Full wall - don't press forward, if you press forward we won't be able to fall down
		pv->AtSteps = false;
		if( jumped )
		{
			vel = up;
			vel.mul4( jumpPower );
			vel.sub4( velocity );
		}
		else
		{
			vel.set( 0,0,0 );
			vel.sub4( fwdVel );
			vel.mul4( 0.5f );
		}

		pv->JumpVector = Vector(0,0,0);
		pv->Speed = GEngine->Physics->TWPlayerSpeed;
		break;
	case 3:
		// High step - press strongly upward
		pv->AtSteps = true;
/*
		if( jumped )
		{
			vel = up;
			vel.mul4( jumpPower );
			move.mul4( 0.3f*speed );
			vel.add4( move );
			vel.sub4( velocity );
		}
		else
*/
		{
			vel = up;
			vel.mul4( 0.8f );
			move.mul4( 0.3f );
			vel.add4( move );
			vel.mul4( speed );
			vel.sub4( velocity );
			vel.mul4( 0.5f );
		}
		break;
	case 2:
		// Medium step - press less strongly upward
		pv->AtSteps = true;
		if( jumped )
		{
			vel = up;
			vel.mul4( jumpPower );
			move.mul4( 0.3f*speed );
			vel.add4( move );
			vel.sub4( velocity );
		}
		else
		{
			vel = up;
			vel.mul4( 0.5f );
			move.mul4( 0.3f );
			vel.add4( move );
			vel.mul4( speed );
			vel.sub4( velocity );
			vel.mul4( 0.5f );
		}
		break;
	case 1:
		// Small step - press a bit upward, but also strongly forward
		pv->AtSteps = true;
		if( jumped )
		{
			vel = up;
			vel.mul4( jumpPower );
			move.mul4( speed );
			vel.add4( move );
			vel.sub4( velocity );
		}
		else
		{
			vel = up;
			vel.mul4( 0.4f );
			vel.add4( move );
			vel.mul4( speed );
			vel.sub4( velocity );
			vel.mul4( 0.5f );
		}
		break;
	case 0:			// Press forward
	default:
		pv->AtSteps = false;
		if( jumped )
		{
			if( onIce )
			{
#ifdef	TRANSGAMING_PS2
				pv->Speed = hkReal(fwdVel.length3())*GEngine->TimeMultiplier;
#else
				pv->Speed = fwdVel.length3()*GEngine->TimeMultiplier;
#endif
				speed = fwdVel.length3();
			}
			vel = up;
			vel.mul4( jumpPower );
			move.mul4( speed );
			vel.add4( move );
			vel.sub4( velocity );
		}
		else if( falling )
		{
			vel = move;
			vel.mul4( speed );
			vel.sub4( fwdVel );
#ifdef	TRANSGAMING_PS2
			vel.mul4( 0.05f*hkReal(fwdVel.length3())*strongAirControl?GEngine->Physics->TWStrongAirControl:GEngine->Physics->TWWeakAirControl );
#else
			vel.mul4( 0.05f*fwdVel.length3()*strongAirControl?GEngine->Physics->TWStrongAirControl:GEngine->Physics->TWWeakAirControl );
#endif
		}
		else
		{
			vel = move;

			if( onIce )
			{
				if( wasFalling )
				{
					pv->Speed *= GEngine->Physics->TWPlayerSpeed/GEngine->Physics->TWPlayerSpeedIce;
					speed = pv->Speed/GEngine->TimeMultiplier;
				}
				vel.mul4( speed*GEngine->Physics->TWPlayerSpeedIce/GEngine->Physics->TWPlayerSpeed );
				vel.sub4( fwdVel );

				float angleFactor = GEngine->Physics->TWIceSlideAngleModifier;

				float logHigh = log10( 0.2f );
				float fwdVelLen = fwdVel.length3();
				float iceFactor = angleFactor;

				float logLow = log10( 0.2f*GEngine->Physics->TWIceSlideModifier );
				// Fix for starting
//				float logLow = log10( ( fwdVelLen < 0.05f*GEngine->Physics->TWPlayerSpeed )?3.0f*GEngine->Physics->TWIceSlideModifier:0.2f*GEngine->Physics->TWIceSlideModifier );
				if( fwdVelLen > 0.001f )
				{
#ifdef	TRANSGAMING_PS2
					iceFactor = hkReal( move.dot3( fwdVel ) )/fwdVelLen;
#else
					iceFactor = move.dot3( fwdVel )/fwdVelLen;
#endif
					if( iceFactor < angleFactor ) iceFactor = angleFactor;
				}

				iceFactor -= angleFactor;
				iceFactor /= (1.0f-angleFactor);
				double ourLog = iceFactor*logHigh + (1.0 - iceFactor)*logLow;
				vel.mul4( pow(10.0,ourLog) );
			}
			else
			{
				vel.mul4( speed );
				vel.sub4( fwdVel );
				vel.mul4( 0.2f );
			}
		}
		break;
	}

	if( stillShocked )
	{
		double factor = (GEngine->Physics->TWPlayerShockedTime - pv->TimeSinceExplosion)/GEngine->Physics->TWPlayerShockedTime;
		if( factor < 0.0 ) factor = 0.0;
		vel.mul4( 1.0 - factor );
	}

	vel.mul4( body->getMass() );

#ifdef HAVOK_POSITION_BUG_TRACKING
	Assert( body->getMass() >= 0.0f );
#ifdef HAVOK3
	if( !vel.isOk4() )
#else
	if( !vel.isOk() )
#endif
		GLog.Print( "Corrupted force in PlayerAction (vel)\n" );
	else if( vel.length3() > 50000.0f )
		GLog.Print( "Force > 50000 in PlayerAction\n" );
#endif

	body->applyLinearImpulse( vel );

	Action = 0;
	pv->LastExecutionSpeed = Vector( velocity(0), velocity(1), velocity(2) );
}

void PhysicsObject::MultiPlayerAction( Vector& forwardVector, Vector& rightVector, bool prediction )
{
	if( !mPlayerVariables || !mHavokPart->Body || !mHavokPart->Body->getSimulationIsland() )
	{
		Action = 0;
		return;
	}

	mHavokPart->Body->getMaterial().setFriction( 0.0f );

	hkRigidBody* body = mHavokPart->Body;
#ifdef HAVOK3
	body->activate();
#else
	body->forceActivate();
#endif
	PhysicsObjectPlayerVariables* pv = mPlayerVariables;
	if( pv->Speed == 0.0f ) pv->Speed = GEngine->Physics->TWMPPlayerSpeed;

	// Only those actions interest us
//	if( GEngine->Net && GEngine->Net->PlayerPredictionMode() && GEngine->Net->IsClient() )
//		Action &= ((1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));
//	else
		Action &= ((1<<EInputJump)|(1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));

	// Gravity Modifier - This is to ensure that player moves better in bullet time
	if( GEngine->TimeMultiplier != 1.0f )
	{
		havokAntiGravityAction* antigrav = (havokAntiGravityAction*)pv->FakeBulletTimeGravity;
		hkVector4 gravity = mHavokPart->World->World->getGravity();
		if( !antigrav )
		{
            m_setTag("Physics");
			pv->FakeBulletTimeGravity = new havokAntiGravityAction( mHavokPart->Body, gravity );
			antigrav = (havokAntiGravityAction*)pv->FakeBulletTimeGravity;
			mHavokPart->World->World->addAction( antigrav );
		}
		hkVector4 mulGravity(0,0,0);
		mulGravity.setMul4( -1.0/GEngine->TimeMultiplier/GEngine->TimeMultiplier + 1.0, gravity );
		hkVector4& gravityNow = antigrav->getGravity();
		gravityNow = mulGravity;
	}
	else
	{
		if( pv->FakeBulletTimeGravity )
		{
			havokAntiGravityAction* antigrav = (havokAntiGravityAction*)pv->FakeBulletTimeGravity;
			if( antigrav->getSimulationIsland() )
				mHavokPart->World->World->removeAction( antigrav );
			antigrav->removeReference();
			pv->FakeBulletTimeGravity = 0;
		}
	}

	// Calculate velocity
	hkVector4 velocity = body->getLinearVelocity();

	// If we're on ladder, use this
#ifdef	TRANSGAMING
	const bool nearLadder = GEngine->pcfWorld->NearLadder( GetPawnFloorPos() ) || GEngine->pcfWorld->NearLadder( GetPawnHeadPos() );
#else
	const bool nearLadder = GEngine->World->NearLadder( GetPawnFloorPos() ) || GEngine->World->NearLadder( GetPawnHeadPos() );
#endif
	if( nearLadder && !(Action&(1<<EInputJump)) )
	{
		PlayerActionLadder( forwardVector, rightVector );
		Action = 0;
		pv->LastExecutionSpeed = Vector( velocity(0), velocity(1), velocity(2) );
		return;
	}

	// If we're shocked, do this
	if( pv->TimeToBeShocked != 0.0f )
	{
		// Our time delta
		float actionDelta = pv->AccumulatedDelta;
		if( actionDelta > 1.0f ) actionDelta = 0.05f;
		pv->AccumulatedDelta = 0;
		pv->TimeToBeShocked -= actionDelta;
		if( pv->TimeToBeShocked < 0.0f )
			pv->TimeToBeShocked = 0.0f;

		// Give landing data to script
		float fallDamage = pv->ReportFallDamage( velocity(1) );
		if( !prediction )
			if( fallDamage*GEngine->TimeMultiplier > 20.0f )
				GScript.QueueMessage("PLAYER_HIT_GROUND", "%e,%f", GetEntity(), -fallDamage*GEngine->TimeMultiplier );

		Action = 0;
		pv->LastExecutionSpeed = Vector( velocity(0), velocity(1), velocity(2) );
		return;
	}

	// If we're in the ground, jump out of it
	MovePlayerOutOfWall();

	// ************************************************************
	// FROM HERE WE START MODIFYING
	// ************************************************************

	// Calculate linear speed relative to the floor
	hkRigidBody* underBody = (hkRigidBody*)MeshUnder(0.05f*BodyScale*OLD_PAWN_SCALE);
	hkVector4 underBodySpeed(0,0,0);
	if( underBody && !underBody->isFixed() )
	{
		underBodySpeed = underBody->getLinearVelocity();
		velocity.sub4( underBodySpeed );
	}

	// Disable jump ability when falling too fast
	if( ( velocity(1) < GEngine->Physics->TWMPPlayerMinimalFallSpeedToDisableJump ) && !pv->JumpInvalidation )
		Action &= ~(1<<EInputJump);

	// Make sure gravity works
	EnableGravity(true);

	// ***********************************************************
	// GET SOME VARIABLES FIRST, AND FIND OUT WHERE WE ARE
	// ************************************************************

//	String debugPrint = String::Sprintf( "Vel Y: %f ", velocity(1) );

	// Some easy variables
	const bool wasFalling = pv->Falling;
	bool hitFloorSimple = FloorCheck();
	bool hitFloor = hitFloorSimple || FloorCheckRandom();
	bool falling = pv->Falling = !hitFloor && !pv->AtSteps && !nearLadder;

	// Check if we are on the slope
	bool hitSlope = true;
	if( !hitFloor ) hitSlope = FloorCheck( -10.0f );	//to update floor normal
	if( hitSlope )
	{
		const Vector upVector(0,1,0);
		float dot = floorNormal.Dot( upVector );
//		GLog.Print( "Slope: %f degrees\n", acos( dot )/F_DEG2RAD );
		if( dot < cos( GEngine->Physics->TWMPSlopeAngleToSlide*F_DEG2RAD ) )
		{
			if( pv->OnSlopeTimes < 10 )
				pv->OnSlopeTimes++;
		}
		else if( pv->OnSlopeTimes > 0 )
			pv->OnSlopeTimes--;
	}
	else if( pv->OnSlopeTimes > 0 )
		pv->OnSlopeTimes--;

    // Desired move
	hkVector4 move(0,0,0);
	const hkVector4 up( 0,1,0 );
	hkVector4 right( rightVector.X, 0, rightVector.Z );
	hkVector4 forward;
	forward.setCross( up, right );
    if(Action & (1<<EInputMoveForward)) move.add4( forward );
	if(Action & (1<<EInputMoveBackward)) move.sub4( forward );
	if(Action & (1<<EInputMoveRight)) move.add4( right );
	if(Action & (1<<EInputMoveLeft)) move.sub4( right );
	move.normalize3();

	hkVector4 fwdVel( velocity );
	fwdVel(1) = 0;

	float airControl = ( fwdVel.dot3( move ) > 0 )?GEngine->Physics->TWMPStrongAirControl:GEngine->Physics->TWMPWeakAirControl;

	bool sliding = false;
	if( pv->OnSlopeTimes > 5 )
	{
		sliding = true;
		airControl = GEngine->Physics->TWMPWeakAirControl;
		falling = true;
	}

	if( pv->TimeSinceExplosion < GEngine->Physics->TWMPPlayerShockedTime ) hitFloor = false;
//	debugPrint += (falling?"falling, ":"notFalling, ");
//	debugPrint += (hitFloor?"hitfloor, ":"noHitFloor, ");
//	debugPrint += (hitFloorSimple?"hfSimple, ":"noHfSimple, ");
	bool wasNotJumping = ( pv->TimeSinceJumped < 0.0f );
//	debugPrint += (wasNotJumping?"wasNotJumping, ":"wasJumping, ");
//	debugPrint += (wasFalling?"wasFalling, ":"wasNotFalling, ");
	const float jumpStrength = GEngine->Physics->TWMPPlayerJumpStrength;
	const bool pressedJump = ( (Action & (1<<EInputJump)) != 0 );
	if( !pressedJump ) pv->JumpInvalidation = false;
//	debugPrint += (pressedJump?"pressedJump, ":"noJump, ");
//	debugPrint += (pv->JumpInvalidation?"jumpInvalidation, ":"noJumpInvalidation, ");

	// Our time delta
	float actionDelta = pv->AccumulatedDelta;
	if( actionDelta > 1.0f ) actionDelta = 0.05f;
	pv->AccumulatedDelta = 0;

	// Adjust movement variables
	pv->TimeSinceLanded += actionDelta;
	pv->RealTimeSinceJumped += actionDelta;
	pv->TimeSinceBunnyHop += actionDelta;
	// Adjust time since we pressed or released Jump button ( > 0 = time since presses, < 0 = time since released )
	if( pressedJump && ( pv->TimeSinceJumped >= 0 ) )		pv->TimeSinceJumped += actionDelta;
	else if( !pressedJump && ( pv->TimeSinceJumped <= 0 ) )	pv->TimeSinceJumped -= actionDelta;
	else													pv->TimeSinceJumped = 0.0f;

	// Detect double jump
	bool doubleJumped = ( wasNotJumping && pressedJump && wasFalling && !pv->DoubleJumped &&
		( fabs(velocity(1)) < GEngine->Physics->TWMPAbsoluteVerticalVelocityBelowWhichDoubleJumpHappens ) );
//	debugPrint += (doubleJumped?"doubleJumped, ":"notDoubleJumped, ");

	if( doubleJumped ) pv->DoubleJumped = true;

	// Detect any jump
	bool alreadyJumped = ( pv->RealTimeSinceJumped < 0.2f );
	bool jumped = ( pressedJump && !wasFalling && !pv->JumpInvalidation && !alreadyJumped ) || doubleJumped;
	if( jumped ) pv->RealTimeSinceJumped = 0.0f;
//	debugPrint += (jumped?"jumped, ":"notJumped, ");

	// Detect bunny hop
	bool bunnyHopped = ( pv->CanBunnyHop && jumped && !doubleJumped );
	float bunnyHopDiff = pv->TimeSinceLanded;
//	debugPrint += (bunnyHopped?"bunnyHopped, ":"notBunnyHopped, ");

	if( ( pv->TimeSinceJumped > GEngine->Physics->TWMPSecondsWhenYouCanBunnyHopBeforeLanding ) && falling )
		pv->JumpInvalidation = true;

	// ***********************************************************
	// NOW USING THIS DATA, DECIDE WHAT TO DO
	// ************************************************************

	bool stillShocked = pv->TimeSinceExplosion < GEngine->Physics->TWMPPlayerShockedTime;

	if( Action & ((1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight)) )
		pv->LastNonzeroAction = Action;

	// Prepare data for air control
	if( stillShocked )		// Started falling from explosion
	{
		pv->TimeSinceExplosion += actionDelta;
		pv->JumpVector = Vector( fwdVel(0), 0, fwdVel(2) );
		pv->JumpVector.Normalize();
		pv->JumpAction = pv->LastNonzeroAction;
		pv->Speed = fwdVel.length3();
		pv->Exploded = true;
	}
	else if( jumped )						// Just jumped
	{
		pv->CanBunnyHop = false;
		pv->JumpVector = Vector( move(0), move(1), move(2) );
		pv->JumpAction = pv->LastNonzeroAction;
	}
	else if( sliding )						// Sliding on something
	{
		pv->CanBunnyHop = false;
		pv->JumpVector = Vector( 0, 0, 0 );
		pv->JumpAction = 0;
	}
	else if( falling && !wasFalling )		// Started falling normally... walked off something
	{
		pv->JumpVector = Vector( fwdVel(0), 0, fwdVel(2) );
		pv->JumpVector.Normalize();
		pv->JumpAction = pv->LastNonzeroAction;
	}

	// Give landing data to script
	float fallDamage = pv->ReportFallDamage( velocity(1) );
	if( !prediction )
		if( fallDamage*GEngine->TimeMultiplier > 20.0f  )
			GScript.QueueMessage("PLAYER_HIT_GROUND", "%e,%f", GetEntity(), -fallDamage*GEngine->TimeMultiplier );

	// Just landed
    if( wasFalling && hitFloor )
	{
//		pv->YCorrection.AddResult( GetPawnHeadPos().Y );
//		pv->YCorrection.Clear();

		pv->Exploded = false;
		pv->DoubleJumped = false;	// you can double jump hop again
		pv->TimeSinceLanded = 0.0f;

		// Jumped just before landing? - bunny hop enable
		if( ( pv->TimeSinceJumped > 0 ) &&
			( pv->TimeSinceJumped < GEngine->Physics->TWMPSecondsWhenYouCanBunnyHopBeforeLanding ) && pressedJump )
		{
			bunnyHopDiff = pv->TimeSinceJumped;
			bunnyHopped = true;
		}
		else
			pv->CanBunnyHop = true;

		// Conserve speed for bunny hops
#ifdef	TRANSGAMING_PS2
		pv->Speed = hkReal( fwdVel.length3() )*GEngine->TimeMultiplier;
#else
		pv->Speed = fwdVel.length3()*GEngine->TimeMultiplier;
#endif
	}

	// Keep speed in proper boundaries
	if( pv->Speed > GEngine->Physics->TWMPMaximalBunnyHopSpeed ) pv->Speed = GEngine->Physics->TWMPMaximalBunnyHopSpeed;

	// Bunny Hop Acceleration and Stopping
	if( bunnyHopped )
	{
		pv->Exploded = false;
		if( pv->TimeSinceBunnyHop > GEngine->Physics->TWMPMinimalTimeBetweenBunnyHops )
		{
			pv->TimeSinceBunnyHop = 0.0f;
			pv->CanBunnyHop = false;

			float factor = 1.0f;
			if( GEngine->Physics->TWMPBunnyHopDifficulty > 0.0f )
				factor = 1.0f-bunnyHopDiff*GEngine->Physics->TWMPBunnyHopDifficulty;
			if( factor < 0.0f )
				factor = 0.0f;

			if( pv->Speed < GEngine->Physics->TWMPMaximalBunnyHopSpeed )
				pv->Speed += pow(GEngine->Physics->TWMPMaximalBunnyHopSpeed-pv->Speed,1.4f)*GEngine->Physics->TWMPBunnyHopAcceleration*factor;
			else
				pv->Speed = GEngine->Physics->TWMPMaximalBunnyHopSpeed;
		}
	}
	else if( ( pv->TimeSinceLanded > GEngine->Physics->TWMPSecondsWhenYouCanBunnyHopAfterLanding ) )
	{
		if( pv->CanBunnyHop )
		{
			pv->CanBunnyHop = false;
			pv->Speed = GEngine->Physics->TWMPPlayerSpeed;
		}
		else if( !wasFalling && !falling )
		{
			pv->Exploded = false;
			pv->Speed = GEngine->Physics->TWMPPlayerSpeed;
		}
	}

	// Air control
	if( sliding )
	{
		move = fwdVel;
		move.normalize3();
	}
	if( falling )
	{
		// Gather myJumpVector from JumpAction
		hkVector4 myPressVector(0,0,0);
		if(pv->JumpAction & (1<<EInputMoveForward)) myPressVector.add4( forward );
		if(pv->JumpAction & (1<<EInputMoveBackward)) myPressVector.sub4( forward );
		if(pv->JumpAction & (1<<EInputMoveRight)) myPressVector.add4( right );
		if(pv->JumpAction & (1<<EInputMoveLeft)) myPressVector.sub4( right );

		float jwl = pv->JumpVector.Magnitude();
		if( myPressVector.length3() )
			myPressVector.normalize3();
//		else if( pv->Exploded )	// that means we're in the air because of explosion, i guess
//			myPressVector = forward;

		float braker = -myPressVector.dot3(move);
		if( jwl && ( braker > 0.0f ) )
		{
			float subtract = GEngine->Physics->TWMPPlayerSlowdownDuringJump*actionDelta*braker;
			if( pv->Speed <= subtract )
				pv->Speed = 1.0f;
			else
				pv->Speed -= subtract;
			airControl = 1.0f;
		}
		move = myPressVector;
	}
	else if( pv->Speed < GEngine->Physics->TWMPPlayerSpeed )		// Keep speed in proper boundaries
		pv->Speed = GEngine->Physics->TWMPPlayerSpeed;

	float speed = pv->Speed/GEngine->TimeMultiplier;
	int stepMeasure = StepCheck( Vector( move(0), move(1), move(2) ) );	// returns 0 to 4, 4 - highest = wall
	hkVector4 vel;

	bool preventJumpyScreen = false;
	bool constSpeedWhenWalking = false;

	// Movement ( with steps )
	if( jumped )
	{
//		mPlayerVariables->YCorrection.Clear();
//		GLog.Print( "Jumped\n" );

		hkVector4 jumpVel = velocity;
		float velUp = 8.0f*jumpStrength;
		if( velUp > jumpVel(1) )
		{
			jumpVel(1) = velUp;
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !jumpVel.isOk4() || ( fabs(jumpVel(0)) > 1000.0f ) || ( fabs(jumpVel(1)) > 1000.0f ) || ( fabs(jumpVel(2)) > 1000.0f ) )
#else
			if( !jumpVel.isOk() || ( fabs(jumpVel(0)) > 1000.0f ) || ( fabs(jumpVel(1)) > 1000.0f ) || ( fabs(jumpVel(2)) > 1000.0f ) )
#endif
				GLog.Print( "Velocity > 1000 in jump in MultiPlayerMove\n" );
#endif
			body->setLinearVelocity( jumpVel );
		}
		vel.set(0,0,0);
		if( stepMeasure == 4 )
		{
			pv->JumpVector = Vector(0,0,0);
			pv->Speed = GEngine->Physics->TWMPPlayerSpeed;
		}
	}
	else switch( stepMeasure )
	{
	case 4:
		// Full wall - don't press forward, if you press forward we won't be able to fall down
		pv->AtSteps = false;
		vel.set( 0,0,0 );
		vel.sub4( fwdVel );
		vel.mul4( 0.5f );
		pv->JumpVector = Vector(0,0,0);
		pv->Speed = GEngine->Physics->TWMPPlayerSpeed;
		break;
	case 3:
		// High step - press strongly upward
		pv->AtSteps = true;
		vel = up;
		vel.mul4( 0.8f );
		move.mul4( 0.3f );
		vel.add4( move );
		vel.mul4( speed );
		vel.sub4( velocity );
		vel.mul4( 0.5f );
		break;
	case 2:
		// Medium step - press less strongly upward
		pv->AtSteps = true;
		vel = up;
		vel.mul4( 0.5f );
		move.mul4( 0.3f );
		vel.add4( move );
		vel.mul4( speed );
		vel.sub4( velocity );
		vel.mul4( 0.5f );
		break;
	case 1:
		// Small step - press a bit upward, but also strongly forward
		pv->AtSteps = true;
		vel = up;
		vel.mul4( 0.4f );
		vel.add4( move );
		vel.mul4( speed );
		vel.sub4( velocity );
		vel.mul4( 0.5f );
		break;
	case 0:			// Press forward
	default:
		pv->AtSteps = false;
		if( falling && ( pv->RealTimeSinceJumped < 1.0f ) )
		{
			vel = move;
			vel.mul4( speed );
			vel.sub4( fwdVel );
#ifdef	TRANSGAMING_PS2
			vel.mul4( 0.05f*hkReal( fwdVel.length3() )*airControl );
#else
			vel.mul4( 0.05f*fwdVel.length3()*airControl );
#endif
		}
		else if( underBodySpeed.length3() > 5.0f )
		{
#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !underBodySpeed.isOk4() || ( fabs(underBodySpeed(0)) > 1000.0f ) || ( fabs(underBodySpeed(1)) > 1000.0f ) || ( fabs(underBodySpeed(2)) > 1000.0f ) )
#else
			if( !underBodySpeed.isOk() || ( fabs(underBodySpeed(0)) > 1000.0f ) || ( fabs(underBodySpeed(1)) > 1000.0f ) || ( fabs(underBodySpeed(2)) > 1000.0f ) )
#endif
				GLog.Print( "Velocity > 1000 in underbodyspeed in MultiPlayerMove\n" );
#endif
			body->setLinearVelocity( underBodySpeed );
			vel.set(0,0,0);
		}
		else if( falling )
		{
			vel = move;
			vel.mul4( speed );
			vel.sub4( fwdVel );
#ifdef	TRANSGAMING_PS2
			vel.mul4( 0.05f*hkReal(fwdVel.length3())* airControl );
#else
			vel.mul4( 0.05f*fwdVel.length3()* airControl );
#endif
		}
		else	// acceleration and deceleration when walking
		{
			vel = move;
			vel.mul4( speed );

			float lenFwdVel = fwdVel.length3();
			if( lenFwdVel > 0.0001f )
			{
#ifdef	TRANSGAMING_PS2
				float lenFwdAccel = hkReal( vel.dot3( fwdVel ) )/lenFwdVel;
#else
				float lenFwdAccel = vel.dot3( fwdVel )/lenFwdVel;
#endif

				hkVector4 velAccel = fwdVel;
				velAccel.mul4( lenFwdAccel/lenFwdVel );
				hkVector4 sideAccel = vel;
				sideAccel.sub4( velAccel );

				if( lenFwdAccel > lenFwdVel+1 ) lenFwdAccel = lenFwdVel+1;
				else if (lenFwdAccel < lenFwdVel-1 ) lenFwdAccel = lenFwdVel-1;

				vel = fwdVel;
				vel.mul4( lenFwdAccel/lenFwdVel - 1 );
				bool moving = ( move.length3() > 0.0f );
				vel.mul4( moving?GEngine->Physics->TWMPAccelerationWhenWalking:GEngine->Physics->TWMPDecelerationWhenWalking );

				sideAccel.mul4( 0.2f );
				vel.add4( sideAccel );
				if( !moving )
					preventJumpyScreen = true;
				else
					constSpeedWhenWalking = true;
			}
			else
			{
				if( vel.length3() > 1.0f ) vel.normalize3();
				vel.mul4( move.length3()?GEngine->Physics->TWMPAccelerationWhenWalking:GEngine->Physics->TWMPDecelerationWhenWalking );
			}
		}
		break;
	}

	if( stillShocked )
	{
		double factor = (GEngine->Physics->TWMPPlayerShockedTime - pv->TimeSinceExplosion)/GEngine->Physics->TWMPPlayerShockedTime;
		if( factor < 0.0 ) factor = 0.0;
		vel.mul4( 1.0 - factor );
	}

#ifdef HAVOK_POSITION_BUG_TRACKING
	float myMass = body->getMass();
	if( myMass < 0.000001f )
	{
		Assert( myMass >= 0.0f );
		Entity* enti = GetEntity();
		GLog.Print( "Zero mass in MultiPlayerAction ( entity '%s, filename '%s' )\n", enti?enti->Name.AsChar():"(null)", enti?enti->FileName.AsChar():"(null)" );
	}
#endif

	vel.mul4( body->getMass()*actionDelta*60 );

	if( actionDelta == 0.0f )
		GLog.Print( "Zero actionDelta!!\n" );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
	if( !vel.isOk4() )
#else
	if( !vel.isOk() )
#endif
	{
		GLog.Print( "Corrupted force in MultiPlayerAction (vel)\n" );
//		GLog.Print( "Velocity: (%f,%f,%f), forward: (%f,%f,%f), right: (%f,%f,%f)\n", velocity(0), velocity(1), velocity(2),
//			forward(0), forward(1), forward(2), right(0), right(1), right(2) );
	}
	else if( vel.length3() > 50000.0f )
		GLog.Print( "Force > 50000 in MultiPlayerAction\n" );
#endif

	body->applyLinearImpulse( vel );

	if( constSpeedWhenWalking )
	{
		hkVector4 newVel = body->getLinearVelocity();
		float newVelLen = newVel.length3();
		if( newVelLen > 0.1f )
		{
			if( velocity.length3() < speed )
			{
				if( ( newVelLen > speed ) && ( newVelLen - speed < 3 ) )
				{
					newVel.mul4(speed/newVelLen);
					body->setLinearVelocity(newVel);
//					GLog.Print( "Fixed from %f to %f\n", newVelLen, speed );
				}
			}
			else if( ( newVelLen < speed ) && ( speed - newVelLen < 3 ) )
			{
				newVel.mul4(speed/newVelLen);
				body->setLinearVelocity(newVel);
//				GLog.Print( "Fixed from %f to %f\n", newVelLen, speed );
			}
		}
	}
	else if( preventJumpyScreen )	// Fix for jumpy screen while doing nothing
	{
		hkVector4 veloc = body->getLinearVelocity();
		if( veloc.dot3( velocity ) < 0.0f )
		{
			veloc = velocity;
			veloc.mul4(0.5);
			body->setLinearVelocity( veloc );
		}
	}

//	String sm = String::Sprintf( "Step: %d", stepMeasure );
//	debugPrint += sm;
//	GLog.Print( "%s\n", debugPrint.AsChar() );

	Action = 0;
	pv->LastExecutionSpeed = Vector( velocity(0), velocity(1), velocity(2) );
}

void PhysicsObject::PlayerActionUnderwater( Vector& forwardVector, Vector& rightVector )
{
    hkRigidBody* body = mHavokPart->Body;
#ifdef HAVOK3
	body->activate();
#else
	body->forceActivate();
#endif

#ifdef HAVOK_POSITION_BUG_TRACKING
	float myMass = body->getMass();
	if( myMass < 0.000001f )
	{
		Assert( myMass >= 0.0f );
		Entity* enti = GetEntity();
		GLog.Print( "Zero mass in PlayerActionUnderwater ( entity '%s, filename '%s' )\n", enti?enti->Name.AsChar():"(null)", enti?enti->FileName.AsChar():"(null)" );
	}
#endif

	MovePlayerOutOfWall();

	const hkVector4& velocity = body->getLinearVelocity();

	// if no action, kill off velocity if walking
	if( !Action || Action == (1<<EInputNone) )
	{
		hkVector4 temp2 = velocity;
		temp2.mul4( -GEngine->Physics->TWUnderwaterSwimDecelerationSpeed*body->getMass() );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !temp2.isOk4() )
#else
		if( !temp2.isOk() )
#endif
			GLog.Print( "Corrupted force in PlayerActionUnderwater\n" );
		else if( temp2.length3() > 50000.0f )
			GLog.Print( "Force > 50000 in PlayerActionUnderwater\n" );
#endif

        body->applyLinearImpulse( temp2 );
		Action = 0;
		return;
   }

   // determine desired move
	hkVector4 move(0,0,0);
	hkVector4 right( rightVector.X, rightVector.Y, rightVector.Z );
	hkVector4 forward( forwardVector.X, forwardVector.Y, forwardVector.Z );
	hkVector4 left;
	left.setNeg4( right );
	hkVector4 backward;
	backward.setNeg4( forward );

	if(Action & (1<<EInputMoveForward))
		move.add4( forward );
	if(Action & (1<<EInputMoveBackward))
		move.add4( backward );
	if(Action & (1<<EInputMoveRight))
		move.add4( right );
	if(Action & (1<<EInputMoveLeft))
		move.add4( left );

//	GLog.Print( "move: (%f,%f,%f)\n", move(0), move(1), move(2) );

	if( GEngine->Physics->TWUnderwaterNormalizeMovementVector )
	    move.normalize3();

//	GLog.Print( "move normalized: (%f,%f,%f)\n", move(0), move(1), move(2) );

	bool moved = ( move.lengthSquared3() > 0 );

	if( moved )
	{
		hkVector4 realMove;

		realMove = move;

		float speed = GEngine->Physics->TWUnderwaterPlayerSpeed/GEngine->TimeMultiplier;

		hkVector4 vel;

		vel = realMove;
		vel.mul4( speed );
		vel.sub4( velocity );
		vel.mul4( 0.2f );

//		GLog.Print( "vel: (%f,%f,%f)\n", vel(0), vel(1), vel(2) );

		vel.mul4( body->getMass() );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
		if( !vel.isOk4() )
#else
		if( !vel.isOk() )
#endif
			GLog.Print( "Corrupted force in PlayerActionUnderwater\n" );
		else if( vel.length3() > 50000.0f )
			GLog.Print( "Force > 50000 in PlayerActionUnderwater\n" );
#endif

		body->applyLinearImpulse( vel );
	}
	Action = 0;
}

int PhysicsObject::StepCheck( Vector moveVector, float distanceModifier )
{
	const float stepCheckPhantomExtent = 10.0f*BodyScale*OLD_PAWN_SCALE*distanceModifier;
	hkVector4 start;
#ifdef HAVOK3
	hkWorldRayCastInput input;
	hkWorldRayCastOutput output;
#endif
	start = mHavokPart->Body->getPosition();

	if( !mHavokPart->StepCheckPhantom )
	{
#ifdef HAVOK3
		hkAabb info;
#else
		// Create StepCheckPhantom
		hkPhantomConstructionInfo info;
#endif
		hkVector4 t( stepCheckPhantomExtent, stepCheckPhantomExtent, stepCheckPhantomExtent );
		info.m_min = start;
		info.m_min.sub4( t );
		info.m_max = start;
		info.m_max.add4( t );
        m_setTag("Physics");
		mHavokPart->StepCheckPhantom = new havokStepCheckPhantom( info, GEngine->Physics->TWMaximalItemPushMass );
		mHavokPart->World->World->addPhantom( mHavokPart->StepCheckPhantom );
	}
	else
	{
		// Setup StepCheckPhantom around the body
		hkVector4 t( stepCheckPhantomExtent, stepCheckPhantomExtent, stepCheckPhantomExtent );
		hkVector4 m_min = start;
		m_min.sub4( t );
		hkVector4 m_max = start;
		m_max.add4( t );
		hkAabb temp( m_min, m_max );
#ifdef HAVOK3
		// Move the phantom...
		mHavokPart->StepCheckPhantom->setAabb( temp );
#else
		mHavokPart->StepCheckPhantom->updateAabb( temp );
#endif
	}

	hkVector4 move( moveVector.X, 0.0f, moveVector.Z );

	if( move.length3() == 0.0f )
		return 0;
	move.normalize3();

	hkRigidBody* body = mHavokPart->Body;
	bool canCollide = CanLineTraceCollision();
	EnableLineTraceCollision( false );

	hkVector4 end;
#ifndef HAVOK3
	hkShape::hkRayResults result;
#endif

	hkVector4 chwilowy( 0,-2.3f*BodyScale*OLD_PAWN_SCALE,0 );
	start.add4( chwilowy );
	end = move;
	end.mul4( 3.8*BodyScale*OLD_PAWN_SCALE );
	end.add4( start );

#ifdef HAVOK3
	input.m_from = start;
	input.m_to = end;
	output.reset();

	mHavokPart->StepCheckPhantom->castRay( input, output );
	if( output.hasHit() )
	{
		EnableLineTraceCollision( canCollide );
		return 4;
	}
#else
	if( mHavokPart->StepCheckPhantom->castRay( start, end, result.m_unnormalizedHitNormal, result.m_mindist ) )
	{
		EnableLineTraceCollision( canCollide );
		return 4;
	}
#endif

	start = body->getPosition();
	chwilowy.set( 0,-1.2*BodyScale*OLD_PAWN_SCALE,0 );
	start.add4( chwilowy );
	end = move;
	end.mul4( 3.8*BodyScale*OLD_PAWN_SCALE );
	end.add4( start );

#ifdef HAVOK3
	input.m_from = start;
	input.m_to = end;
	output.reset();

	mHavokPart->StepCheckPhantom->castRay( input, output );
	if( output.hasHit() )
	{
		EnableLineTraceCollision( canCollide );
		return 4;
	}
#else
	if( mHavokPart->StepCheckPhantom->castRay( start, end, result.m_unnormalizedHitNormal, result.m_mindist ) )
	{
		EnableLineTraceCollision( canCollide );
		return 4;
	}
#endif

	start = body->getPosition();
	chwilowy.set( 0,0,0 );
	start.add4( chwilowy );
	end = move;
	end.mul4( 3.8*BodyScale*OLD_PAWN_SCALE*distanceModifier );
	end.add4( start );

#ifdef HAVOK3
	input.m_from = start;
	input.m_to = end;
	output.reset();

	mHavokPart->StepCheckPhantom->castRay( input, output );
	if( output.hasHit() )
	{
		EnableLineTraceCollision( canCollide );
		return 4;
	}
#else
	if( mHavokPart->StepCheckPhantom->castRay( start, end, result.m_unnormalizedHitNormal, result.m_mindist ) )
	{
		EnableLineTraceCollision( canCollide );
		return 4;
	}
#endif

	start = body->getPosition();
	chwilowy.set( 0,1.2*BodyScale*OLD_PAWN_SCALE,0 );
	start.add4( chwilowy );
	end = move;
	end.mul4( 3.8*BodyScale*OLD_PAWN_SCALE );
	end.add4( start );

#ifdef HAVOK3
	input.m_from = start;
	input.m_to = end;
	output.reset();

	mHavokPart->StepCheckPhantom->castRay( input, output );
	if( output.hasHit() )
	{
		EnableLineTraceCollision( canCollide );
		return 4;
	}
#else
	if( mHavokPart->StepCheckPhantom->castRay( start, end, result.m_unnormalizedHitNormal, result.m_mindist ) )
	{
		EnableLineTraceCollision( canCollide );
		return 4;
	}
#endif

	start = body->getPosition();
	chwilowy.set( 0,2.4*BodyScale*OLD_PAWN_SCALE,0 );
	start.add4( chwilowy );
	end = move;
	end.mul4( 3.8*BodyScale*OLD_PAWN_SCALE );
	end.add4( start );

#ifdef HAVOK3
	input.m_from = start;
	input.m_to = end;
	output.reset();

	mHavokPart->StepCheckPhantom->castRay( input, output );
	if( output.hasHit() )
	{
		EnableLineTraceCollision( canCollide );
		return 4;
	}
#else
	if( mHavokPart->StepCheckPhantom->castRay( start, end, result.m_unnormalizedHitNormal, result.m_mindist ) )
	{
		EnableLineTraceCollision( canCollide );
		return 4;
	}
#endif

	start = body->getPosition();
	chwilowy.set( 0,3.6*BodyScale*OLD_PAWN_SCALE,0 );
	start.add4( chwilowy );
	end = move;
	end.mul4( 3.8*BodyScale*OLD_PAWN_SCALE );
	end.add4( start );

#ifdef HAVOK3
	input.m_from = start;
	input.m_to = end;
	output.reset();

	mHavokPart->StepCheckPhantom->castRay( input, output );
	if( output.hasHit() )
	{
		EnableLineTraceCollision( canCollide );
		return 4;
	}
#else
	if( mHavokPart->StepCheckPhantom->castRay( start, end, result.m_unnormalizedHitNormal, result.m_mindist ) )
	{
		EnableLineTraceCollision( canCollide );
		return 4;
	}
#endif

	start = body->getPosition();
	chwilowy.set( 0,4.8*BodyScale*OLD_PAWN_SCALE,0 );
	start.add4( chwilowy );
	end = move;
	end.mul4( 3.8*BodyScale*OLD_PAWN_SCALE );
	end.add4( start );

#ifdef HAVOK3
	input.m_from = start;
	input.m_to = end;
	output.reset();

	mHavokPart->StepCheckPhantom->castRay( input, output );
	if( output.hasHit() )
	{
		EnableLineTraceCollision( canCollide );
		return 4;
	}
#else
	if( mHavokPart->StepCheckPhantom->castRay( start, end, result.m_unnormalizedHitNormal, result.m_mindist ) )
	{
		EnableLineTraceCollision( canCollide );
		return 4;
	}
#endif

	start = body->getPosition();
	chwilowy.set( 0,-3.4*BodyScale*OLD_PAWN_SCALE,0 );
	start.add4( chwilowy );
	end = move;
	if( 3.23*distanceModifier < 3.8 )
		end.mul4( 3.23*BodyScale*OLD_PAWN_SCALE*distanceModifier );
	else
		end.mul4( 3.8*BodyScale*OLD_PAWN_SCALE );
	end.add4( start );

#ifdef HAVOK3
	input.m_from = start;
	input.m_to = end;
	output.reset();

	mHavokPart->StepCheckPhantom->castRay( input, output );
	if( output.hasHit() )
	{
		EnableLineTraceCollision( canCollide );
		return 3;
	}
#else
	if( mHavokPart->StepCheckPhantom->castRay( start, end, result.m_unnormalizedHitNormal, result.m_mindist ) )
	{
		EnableLineTraceCollision( canCollide );
		return 3;
	}
#endif

	start = body->getPosition();
	chwilowy.set( 0,-4.8*BodyScale*OLD_PAWN_SCALE,0 );
	start.add4( chwilowy );
	end = move;
	if( 2.66*distanceModifier < 3.8 )
		end.mul4( 2.66*BodyScale*OLD_PAWN_SCALE*distanceModifier );
	else
		end.mul4( 3.8*BodyScale*OLD_PAWN_SCALE );
	end.add4( start );

#ifdef HAVOK3
	input.m_from = start;
	input.m_to = end;
	output.reset();

	mHavokPart->StepCheckPhantom->castRay( input, output );
	if( output.hasHit() )
	{
		EnableLineTraceCollision( canCollide );
		return 2;
	}
#else
	if( mHavokPart->StepCheckPhantom->castRay( start, end, result.m_unnormalizedHitNormal, result.m_mindist ) )
	{
		EnableLineTraceCollision( canCollide );
		return 2;
	}
#endif

	start = body->getPosition();
	chwilowy.set( 0,-5.48*BodyScale*OLD_PAWN_SCALE,0 );
	start.add4( chwilowy );
	end = move;
	if( 2.1*distanceModifier < 3.8 )
		end.mul4( 2.1*BodyScale*OLD_PAWN_SCALE*distanceModifier );
	else
		end.mul4( 3.8*BodyScale*OLD_PAWN_SCALE );
	end.add4( start );

#ifdef HAVOK3
	input.m_from = start;
	input.m_to = end;
	output.reset();

	mHavokPart->StepCheckPhantom->castRay( input, output );
	if( output.hasHit() )
	{
		EnableLineTraceCollision( canCollide );
		return 1;
	}
#else
	if( mHavokPart->StepCheckPhantom->castRay( start, end, result.m_unnormalizedHitNormal, result.m_mindist ) )
	{
		EnableLineTraceCollision( canCollide );
		return 1;
	}
#endif
	EnableLineTraceCollision( canCollide );
	return 0;
}

void PhysicsObject::PlayerActionLadder( Vector& forwardVector, Vector& rightVector )
{
	EnableGravity(false);

	hkRigidBody* body = mHavokPart->Body;
	PhysicsObjectPlayerVariables* pv = mPlayerVariables;
	hkVector4 velocity = body->getLinearVelocity();
	float speed = GEngine->Physics->TWPlayerSpeed;
	const float jumpStrength = GEngine->Physics->TWPlayerJumpStrength;

#ifdef HAVOK_POSITION_BUG_TRACKING
	float myMass = body->getMass();
	if( myMass < 0.000001f )
	{
		Assert( myMass >= 0.0f );
		Entity* enti = GetEntity();
		GLog.Print( "Zero mass in PlayerActionLadder ( entity '%s, filename '%s' )\n", enti?enti->Name.AsChar():"(null)", enti?enti->FileName.AsChar():"(null)" );
	}
#endif

	MovePlayerOutOfWall();

	// determine desired move
	hkVector4 move(0,0,0);
	hkVector4 upVec(0,1,0);
	hkVector4 right( rightVector.X, rightVector.Y, rightVector.Z );
	hkVector4 forward( forwardVector.X, forwardVector.Y, forwardVector.Z );
	hkVector4 left;
	left.setNeg4( right );
	hkVector4 backward;
	backward.setNeg4( forward );

	hkVector4 horizontalForward;
	horizontalForward.setCross( left, upVec );

	if(Action & (1<<EInputMoveForward))		move.add4( forward );
	if(Action & (1<<EInputMoveBackward))	move.add4( backward );
	if(Action & (1<<EInputMoveRight))		move.add4( right );
	if(Action & (1<<EInputMoveLeft))		move.add4( left );

	hkVector4 forwardPart( horizontalForward );

	if( move(1) > 0.3f )
		move(1) = GEngine->Physics->TWPlayerStairsUpSpeed;
	else if( move(1) < -0.3f )
		move(1) = -GEngine->Physics->TWPlayerStairsDownSpeed;
	else
		move(1) = 0.0f;

	pv->AtSteps = false;
	hkVector4 vel;

	if( Action & (1<<EInputJump) )
	{
		vel = upVec;
		vel.mul4( 0.7f*jumpStrength );
		vel.mul4( speed );
		vel.sub4( velocity );
	}
	else if( move.lengthSquared3() > 0 )
	{
		vel = move;
		vel.mul4( speed );
		vel.sub4( velocity );
		vel.mul4( 0.2f );
	}
	else
	{
		if( !(Action & (1<<EInputJump)) )	// we don't move, we don't jump... kill off velocity
		{
			hkVector4 temp2 = velocity;
			temp2.mul4( -0.3*body->getMass() );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
			if( !temp2.isOk4() )
#else
			if( !temp2.isOk() )
#endif
				GLog.Print( "Corrupted force in PlayerActionLadder\n" );
			else if( temp2.length3() > 50000.0f )
				GLog.Print( "Force > 50000 in PlayerActionLadder\n" );
#endif

			body->applyLinearImpulse( temp2 );
		}
		return;
	}

	vel.mul4( body->getMass()/GEngine->TimeMultiplier );

#ifdef HAVOK_POSITION_BUG_TRACKING
#ifdef HAVOK3
	if( !vel.isOk4() )
#else
	if( !vel.isOk() )
#endif
		GLog.Print( "Corrupted force in PlayerActionLadder\n" );
	else if( vel.length3() > 50000.0f )
		GLog.Print( "Force > 50000 in PlayerActionLadder\n" );
#endif

	body->applyLinearImpulse( vel );
}

/*
void PhysicsObject::MultiPlayerActionQW( Vector& forwardVector, Vector& rightVector )
{
	if( !mPlayerVariables || !mHavokPart->Body || !mHavokPart->Body->getSimulationIsland() )
	{
		Action = 0;
		return;
	}

	// Get velocity
	hkRigidBody* body = mHavokPart->Body;
	body->forceActivate();
	PhysicsObjectPlayerVariables* pv = mPlayerVariables;
	hkVector4 velocity = body->getLinearVelocity();
	hkVector4 velFwdDir( velocity );
	velFwdDir(1) = 0;
	velFwdDir.normalize3();
	hkVector4 upVec(0,1,0);

	if( !( Action & (1<<EInputJump) ) ) pv->JumpPressed = false;

	// Clamp actions to usable here
	Action &= ((1<<EInputJump)|(1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));

//	if( mHavokPart->World->IsUnderwater() )
//	{
//		PlayerActionUnderwater( forwardVector, rightVector );
//		pv->LastExecutionSpeed = Vector( velocity(0), velocity(1), velocity(2) );
//		return;
//	}

	const bool feetNearLadder = GEngine->World->NearLadder( GetPawnFloorPos() );
	const bool headNearLadder = GEngine->World->NearLadder( GetPawnHeadPos() );

	if( feetNearLadder || headNearLadder )
	{
		PlayerActionLadder( forwardVector, rightVector );
		Action = 0;
		pv->LastExecutionSpeed = Vector( velocity(0), velocity(1), velocity(2) );
		return;
	}

	// Normal movement
	EnableGravity(false);

	bool hitFloor = FloorCheck();

	// Accumulated delta problem - taken from Entity Tick
	float actionDelta = pv->AccumulatedDelta;
	if( actionDelta > 1.0f ) actionDelta = 0.05f;
	pv->AccumulatedDelta = 0;

	// Preserve speed after landing
	if( pv->TimeSinceLanded < GEngine->Physics->TWPromodeSecondsYouHaveSameSpeedAfterLanding )
	{
        if( velocity.length3() > 0.0001f )
		    velocity.mul4( pv->SpeedWhenLanded/velocity.length3() );
		pv->TimeSinceLanded += actionDelta;
	}

	// disable jump when falling fast
	if( velocity(1) < GEngine->Physics->TWPromodePlayerMinimalFallSpeedToDisableJump ) Action &= ~(1<<EInputJump);

	// Code to jump out of the ground if we're in it

#ifdef HAVOK_POSITION_BUG_TRACKING
	float myMass = body->getMass();
	if( myMass < 0.000001f )
	{
		Assert( myMass >= 0.0f );
		Entity* enti = GetEntity();
		GLog.Print( "Zero mass in MultiPlayerActionQW ( entity '%s, filename '%s' )\n", enti?enti->Name.AsChar():"(null)", enti?enti->FileName.AsChar():"(null)" );
	}
#endif

	if( FloorCheck( -3.5f ) )
	{
		hkVector4 vel(0,1,0);
		vel.mul4( 7.0f );
		vel.sub4( velocity );
		vel.mul4( body->getMass() );

#ifdef HAVOK_POSITION_BUG_TRACKING
	if( !vel.isOk() )
		GLog.Print( "Corrupted force in MultiPlayerActionQW\n" );
	else if( vel.length3() > 50000.0f )
		GLog.Print( "Force > 50000 in MultiPlayerActionQW\n" );
#endif

		body->applyLinearImpulse( vel );
		Action = 0;
		return;
	}

	// This bit makes the player movement code believe its in the air for a 0.3 second after a nearby explosion
	if( pv->TimeSinceExplosion < GEngine->Physics->TWMPPlayerShockedTime )
	{
		pv->TimeSinceExplosion += actionDelta;
		hitFloor = false;
	}

	float speed = GEngine->Physics->TWPromodePlayerSpeed/GEngine->TimeMultiplier;

    // If we didnt hit anything, we're falling
	if( ( !hitFloor && !pv->AtSteps ) ||
		( velocity(1) > GEngine->Physics->TWPromodePlayerJumpStrength*speed*0.8f ) ||
		( pv->TimeSinceLastOnGround > 0.5f ) )
		pv->Falling = true;

	// Check landing
	if( pv->Falling && ( velocity(1) < 0.0f ) && hitFloor )
	{
		pv->Falling = false;		// not falling anymore
		pv->DoubleJumped = false;	// can double jump again
		GScript.QueueMessage("PLAYER_HIT_GROUND", "%e,%f", GetEntity(), pv->LastExecutionSpeed.Y );
		pv->SpeedWhenLanded = velocity.length3();
		pv->TimeSinceLanded = 0.0f;

		if( GEngine->Physics->TWPromodeHorizontalSpeedAfterLandingMultiplier )
		{
			hkVector4 addition( velocity );
			addition(1) = 0;
			float len = addition.length3();
			if( len )
				addition.mul4(GEngine->Physics->TWPromodeHorizontalSpeedAfterLandingMultiplier*GEngine->Physics->TWPromodePlayerSpeed/len);
			velocity.add4( addition );
//			velocity(0) += 1.0f+GEngine->Physics->TWPromodeHorizontalSpeedAfterLandingMultiplier;
//			velocity(2) += 1.0f+GEngine->Physics->TWPromodeHorizontalSpeedAfterLandingMultiplier;
		}
	}

	pv->LastExecutionSpeed = Vector( velocity(0), velocity(1), velocity(2) );

	// Jump - you can do it in the same tick - see?
	if( ( Action & (1<<EInputJump) )&& !pv->JumpPressed )
	{
		if( ( !pv->Falling && ( velocity(1) < 1.0f ) ) || 	// first jump
			( GEngine->Physics->TWPromodeDoubleJump && pv->Falling && !pv->DoubleJumped
				&& ( fabs( velocity(1) ) < GEngine->Physics->TWAbsoluteVerticalVelocityBelowWhichDoubleJumpHappens ) ) )	// double jump
		{
			if( pv->Falling ) pv->DoubleJumped = true;
			velocity(1) = GEngine->Physics->TWPromodePlayerJumpStrength*speed;
			pv->Falling = true;
			pv->AtSteps = false;
			pv->JumpPressed = true;
		}
	}

	// Traces
	float traceDistance1 = 1.0f;
	float traceDistance2 = 1.0f;
	float traceDistance3 = 1.0f;

	// Friction
	if( !pv->Falling )
	{
		// Test for slope ahead
		Vector traceStart( velFwdDir(0)*0.7f, -1.5f*BodyScale*OLD_PAWN_SCALE,velFwdDir(2)*0.7f);
		Vector traceEnd( traceStart.X, -9.5f*BodyScale*OLD_PAWN_SCALE, traceStart.Z );
		traceDistance2 = PlayerTrace( traceStart, traceEnd );

		traceStart.Set( velFwdDir(0)*0.5f, -1.5f*BodyScale*OLD_PAWN_SCALE,velFwdDir(2)*0.5f);
		traceEnd.Set( traceStart.X, -9.5f*BodyScale*OLD_PAWN_SCALE, traceStart.Z );
		traceDistance1 = PlayerTrace( traceStart, traceEnd );

		traceStart.Set( velFwdDir(0)*1.0f, -1.5f*BodyScale*OLD_PAWN_SCALE,velFwdDir(2)*1.0f);
		traceEnd.Set( traceStart.X, -9.5f*BodyScale*OLD_PAWN_SCALE, traceStart.Z );
		traceDistance3 = PlayerTrace( traceStart, traceEnd );

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

			if( traceDistance2 >= 1.0f )	// no ground - increase friction
				groundFriction *= GEngine->Physics->TWPromodePlayerGroundFrictionModifierNearFalls;

			float control = (curSpeed<stopSpeed)?stopSpeed:curSpeed;
			float speedDrop = actionDelta*groundFriction*control;

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
		rightVector.Normalize();
		hkVector4 right( rightVector.X, rightVector.Y, rightVector.Z );
		hkVector4 moveForward;
		moveForward.setCross( upVec, right );
		moveForward(1) = 0;
		moveForward.normalize3();

		right(1) = 0;
		right.normalize3();

		const float forwardSpeed = speed;
		moveForward.mul4( forwardSpeed );

		const float sideSpeed = speed*GEngine->Physics->TWPromodePlayerStrafeSpeedModifier;
		right.mul4( sideSpeed );

		if( Action & (1<<EInputMoveForward) ) wishVel.add4( moveForward );
		if( Action & (1<<EInputMoveBackward) ) wishVel.sub4( moveForward );
		if( Action & (1<<EInputMoveRight) ) wishVel.add4( right );
		if( Action & (1<<EInputMoveLeft) ) wishVel.sub4( right );
		wishVel(1) = 0;
	}

	float wishSpeed = wishVel.length3();
	hkVector4 wishDir = wishVel;
	wishDir.normalize3();

	float toAdd = 0.0f;

	if( wishSpeed > speed )
	{
		wishVel.mul4( speed/wishSpeed );
		wishSpeed = speed;
	}

	if( pv->Falling )	// Air control
	{
		pv->TimeSinceLastOnGround = 0.0f;

		// Accelerate in air
		float accel = GEngine->Physics->TWPromodePlayerAirAcceleration;
		float wishSpd = wishSpeed;
		if( wishSpd > ( speed*GEngine->Physics->TWPromodePlayerAirMaxAccelerationSpeedModifier ) )
			wishSpd = ( speed*GEngine->Physics->TWPromodePlayerAirMaxAccelerationSpeedModifier );

		float dotSpeed = velocity.dot3( wishDir );
		float addSpeed = wishSpd - dotSpeed;

		if( addSpeed > 0 )
		{
			float accelSpeed = accel * wishSpeed * actionDelta;
			if( accelSpeed > addSpeed )
				accelSpeed = addSpeed;
	
			hkVector4 wd( wishDir );
			wd.mul4( accelSpeed );
			velocity.add4( wd );
		}

		// add gravity to velocity
		velocity(1) -= actionDelta*GEngine->Physics->TWPromodeGravity;

		// In our engine, Havok does the clipping to planes for now.
	}
	else						// Ground control
	{
		if( !hitFloor )
			pv->TimeSinceLastOnGround += actionDelta;

		velocity(1) = 0;

		// Accelerate on ground
		float accel = GEngine->Physics->TWPromodePlayerAcceleration;
		float dotSpeed = velocity.dot3( wishDir );
		float addSpeed = wishSpeed - dotSpeed;

		if( addSpeed > 0 )
		{
			float accelSpeed = accel * wishSpeed * actionDelta;
			if( accelSpeed > addSpeed )
				accelSpeed = addSpeed;
	
			hkVector4 wd( wishDir );
			wd.mul4( accelSpeed );
			velocity.add4( wd );
		}

		// add gravity to velocity
		velocity(1) -= actionDelta*GEngine->Physics->TWPromodeGravity;

		// Try to clip to stairs
		hkVector4 velocityHoriz( velocity );
		velocityHoriz(1) = 0;

		float traceDistance = 0.0f;

		float toAdd1 = (velocityHoriz.length3()*-3.0f*BodyScale*OLD_PAWN_SCALE)/0.5f*(traceDistance1-0.5f);
		float toAdd2 = (velocityHoriz.length3()*-3.0f*BodyScale*OLD_PAWN_SCALE)/0.7f*(traceDistance2-0.5f);
		float toAdd3 = (velocityHoriz.length3()*-3.0f*BodyScale*OLD_PAWN_SCALE)/1.0f*(traceDistance3-0.5f);

		float absToAdd1 = fabs(toAdd1);
		float absToAdd2 = fabs(toAdd2);
		float absToAdd3 = fabs(toAdd3);

		if( absToAdd1 > absToAdd2 )
		{
			if( absToAdd1 > absToAdd3 )
			{
				toAdd = toAdd1;
				traceDistance = traceDistance1;
			}
			else
			{
				toAdd = toAdd3;
				traceDistance = traceDistance3;
			}
		}
		else
		{
			if( absToAdd2 > absToAdd3 )
			{
				toAdd = toAdd2;
				traceDistance = traceDistance2;
			}
			else
			{
				toAdd = toAdd3;
				traceDistance = traceDistance3;
			}
		}

		if( ( traceDistance > 0.0f ) && ( traceDistance < 1.0f ) )
		{
			pv->AtSteps = ( fabs(toAdd) > 0.2f );
			velocity(1) += toAdd*3.0f;	// just an internal tweak so it goes better up the stairs
		}
		else
			pv->AtSteps = false;
	}

#ifdef HAVOK_POSITION_BUG_TRACKING
	if( !velocity.isOk() || ( fabs(velocity(0)) > 1000.0f ) || ( fabs(velocity(1)) > 1000.0f ) || ( fabs(velocity(2)) > 1000.0f ) )
		GLog.Print( "Velocity > 1000 in PlayerMoveQW\n" );
#endif
	body->setLinearVelocity( velocity );
	Action = 0;
}

float PhysicsObject::PlayerTrace( Vector& localStart, Vector& localEnd )
{
	bool canCollide = CanLineTraceCollision();
	if( canCollide )
		EnableLineTraceCollision( false );

	hkVector4 start;
	start = mHavokPart->Body->getPosition();
	hkShape::hkRayResults result;
	hkVector4 end( start );
	hkVector4 chwilowy( localStart.X,localStart.Y, localStart.Z );
	start.add4( chwilowy );
	chwilowy.set( localEnd.X, localEnd.Y, localEnd.Z );
	end.add4( chwilowy );

	hkBroadPhase* broadPhase = mHavokPart->World->World->getBroadPhase();
	hkShape::hkRayResults initResults;
	havokFixedGeometryRaycastCallback rayCallBack( start, end, initResults );
	broadPhase->castPrimitive( start, end, rayCallBack );
	initResults = rayCallBack.getRayResults();

	if( canCollide )
		EnableLineTraceCollision( true );

	return initResults.m_mindist;
}
*/
