
#include "Engine.h"
#include "Physics/Havok/HavokPhysics.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#include "Net/UdpMessage.h"
#include "Physics/Ragdoll.h"
#include "Physics/PhysicsEngine.h"
#include "Physics/PhysicsWorld.h"
#include "Physics/PhysicsObject.h"
#include "Engine/Model.h"


#ifdef HAVOK3

static void HK_CALL errorFunc(const char* msg, void*)
{
	printf("Error : %s\n", msg);
}
#endif

PhysicsEngine::PhysicsEngine()
{
	// Default tweak values
	TWAbsoluteVerticalVelocityBelowWhichDoubleJumpHappens = 1.0f;
	TWSecondsWhenYouCanBunnyHopAfterLanding = 0.2f;
	TWSecondsWhenYouCanBunnyHopBeforeLanding = 0.2f;
	TWPlayerSpeed = 9.0f;
	TWBunnyHopAcceleration = 0.1f;
	TWPlayerJumpStrength = 1.1f;
	TWPlayerStairsUpSpeed = 1.0f;
	TWPlayerStairsDownSpeed = 1.0f;
	TWMaximalItemPushMass = 2500.0f;
	TWMaximalBunnyHopSpeed = 40.0f;
	TWGravity = 2*9.81f;
	TWPlayerMinimalFallSpeedToDisableJump = -10.0f;
	TWPlayerSlowdownDuringJump = 20.0f;
	TWStrongAirControl = 0.2f;
	TWWeakAirControl = 0.002f;
	TWIceSlideModifier = 0.1f;
	TWIceSlideAngleModifier = 0.5f;
	TWPlayerSpeedIce = 12.0f;
	TWSlopeAngleToSlide = 45.0f;
	TWPlayerShockedTime = 0.5f;
	TWPlayerExplosionShockedTime = 0.1f;

	TWMPAbsoluteVerticalVelocityBelowWhichDoubleJumpHappens = 1.0f;
	TWMPSecondsWhenYouCanBunnyHopAfterLanding = 0.2f;
	TWMPSecondsWhenYouCanBunnyHopBeforeLanding = 0.2f;
	TWMPPlayerSpeed = 9.0f;
	TWMPAccelerationWhenWalking = 1.0f;
	TWMPDecelerationWhenWalking = 1.0f;
	TWMPBunnyHopAcceleration = 0.1f;
	TWMPPlayerJumpStrength = 1.1f;
	TWMPPlayerStairsUpSpeed = 1.0f;
	TWMPPlayerStairsDownSpeed = 1.0f;
	TWMPMaximalItemPushMass = 2500.0f;
	TWMPMaximalBunnyHopSpeed = 40.0f;
	TWMPPlayerMinimalFallSpeedToDisableJump = -10.0f;
	TWMPPlayerSlowdownDuringJump = 20.0f;
	TWMPStrongAirControl = 0.2f;
	TWMPWeakAirControl = 0.002f;
	TWMPSlopeAngleToSlide = 45.0f;
	TWMPMinimalTimeBetweenBunnyHops = 0.3f;
	TWMPBunnyHopDifficulty = 0.0f;
	TWMPPlayerShockedTime = 0.1f;
	TWMPPlayerExplosionShockedTime = 0.1f;
	TWMPMinimalExplosionImpulse = 0.9f;
	TWMPMaxStepHeightWhileInAir = 0.7f;
	TWMPMaxStepHeight = 0.7f;
	TWMPPhantomTolerance = 0.1f;
	TWMPOverbounce = 1.01f;
	TWMPMaxTimeToZeroCamera = 300;
	TWMPMinCameraCatchup = 0.02f;
	TWMPMaxCameraDiff = 0.5f;
	TWMPMaxCorrectPosSearchDist = 0.1f;
	TWMPAlternateRJ = false;
	TWMPAlternateRJOnlyMyRockets = false;
	TWMPAlternateRJWorkAboveThisStrength = 0.0f;

	MPPredictionInterpolation = false;
	MPPredictionInterpolationFactor = 0.66f;
	MPKnockbackMultiplier = 0.005f;

	TWPromodePlayerSpeed = 8.0f;
	TWPromodePlayerStrafeSpeedModifier = 1.75f;
	TWPromodePlayerJumpStrength = 1.35f;
	TWPromodePlayerMinimalFallSpeedToDisableJump = -10.0f;
	TWPromodePlayerGroundFriction = 4.0f;
	TWPromodePlayerStopSpeedFactor = 0.5f;
	TWPromodePlayerGroundFrictionModifierNearFalls = 2.0f;
	TWPromodePlayerAirAcceleration = 10.0f;
	TWPromodePlayerAcceleration = 10.0f;
	TWPromodePlayerAirMaxAccelerationSpeedModifier = 0.15f;
	TWPromodeGravity = 2*9.81f;
	TWPromodeDoubleJump = false;
	TWPromodeSecondsYouHaveSameSpeedAfterLanding = 0.1f;
	TWPromodeHorizontalSpeedAfterLandingMultiplier = 0.2f;

	TWUnderwaterGravity = 0.1f;
	TWUnderwaterDampingRaiseLinear = 2.0f;
	TWUnderwaterDampingRaiseAngular = 1.5f;
	TWUnderwaterPlayerSpeed = 10.0f;
	TWUnderwaterSwimDecelerationSpeed = 0.2f;
	TWUnderwaterNormalizeMovementVector = false;

	TWHighFreqMinLinVel = 0.5f;
	TWHighFreqMinAngVel = 0.5f;
	TWLowFreqMinLinVel = 1.0f;
	TWLowFreqMinAngVel = 1.0f;

	TWMinimalGlassBreakMass = 20.0f;
	TWMinimalGlassBreakSpeed = 10.0f;

	oldPlayerSpeed = TWPlayerSpeed;
	canForgetOPS = false;

	PhysicsFPS = DEFAULT_PHYSICS_FPS;

    m_setTag("Physics");
	mConvexCache = (void*)new HavokWorldConvexCache;
}

PhysicsEngine::~PhysicsEngine()
{
	delete (HavokWorldConvexCache*)mConvexCache;
	mConvexCache = 0;

	while( mPhysicsWorlds.Size() )
		delete mPhysicsWorlds[0];
#ifdef _DEBUG
	hkMemory& mem = hkMemory::getInstance();
#ifndef HAVOK3
	mem.printStatistics( &hkcout );
#endif
#endif
	hkError::replaceInstance( HK_NULL );
	hkBaseSystem::quit();
}

void PhysicsEngine::Init()
{
	// Init main Havok engine
#ifdef HAVOK3
    m_setTag("Physics");
#ifdef TRANSGAMING_WINDOWS
	hkMallocMemory* memoryManager = new hkMallocMemory();
#else
	hkPoolMemory* memoryManager = new hkPoolMemory();
#endif
    m_setTag("Physics");
	hkThreadMemory* threadMemory = new hkThreadMemory( memoryManager, 4 );
    m_setTag("Physics");
	hkBaseSystem::init( memoryManager, threadMemory, errorFunc );
	memoryManager->removeReference();

	// Initialize the stack...
	char* stackBuffer;
	{
		int	stack_size = 0x20000;
		stackBuffer = hkAllocate<char>( stack_size, HK_MEMORY_CLASS_BASE );
		threadMemory->setStackArea(stackBuffer, stack_size);
	}

	hkStreambufFactory::getInstance().replaceInstance( new TNGPhysFSStreambufFactory() );
#else
	hkBaseSystem::init();
    m_setTag("Physics");
	havokErrorHandler* eh = new havokErrorHandler();
	eh->disableAssert( "hkKeyframedRigidMotion.cpp", 91 );
	eh->disableAssert( "hkCollidable.cpp", 21 );
//	eh->disableAssert( "hkCollidable.cpp", 21 );
	hkError::replaceInstance( eh );
#endif
}

void PhysicsEngine::GetTweaksFromScript( void )
{
	ScriptObject PlayerMove = GScript.Globals()["Tweak"]["PlayerMove"];
	
	TWAbsoluteVerticalVelocityBelowWhichDoubleJumpHappens = PlayerMove["AbsoluteVerticalVelocityBelowWhichDoubleJumpHappens"].GetFloat();
	TWSecondsWhenYouCanBunnyHopAfterLanding = PlayerMove["SecondsWhenYouCanBunnyHopAfterLanding"].GetFloat();
	TWSecondsWhenYouCanBunnyHopBeforeLanding = PlayerMove["SecondsWhenYouCanBunnyHopBeforeLanding"].GetFloat();
	TWPlayerSpeed = PlayerMove["PlayerSpeed"].GetFloat();
	TWBunnyHopAcceleration = PlayerMove["BunnyHopAcceleration"].GetFloat();
	TWPlayerJumpStrength = PlayerMove["JumpStrength"].GetFloat();
	TWPlayerStairsUpSpeed = PlayerMove["StairsUpSpeed"].GetFloat();
	TWPlayerStairsDownSpeed = PlayerMove["StairsDownSpeed"].GetFloat();
	TWMaximalItemPushMass = PlayerMove["MaximalItemPushMass"].GetFloat();
	TWMaximalBunnyHopSpeed = PlayerMove["MaximalBunnyHopSpeed"].GetFloat();
	TWPlayerSlowdownDuringJump = PlayerMove["SlowdownDuringJump"].GetFloat();
	TWStrongAirControl = PlayerMove["StrongAirControl"].GetFloat();
	TWWeakAirControl = PlayerMove["WeakAirControl"].GetFloat();

	TWIceSlideModifier = PlayerMove["IceSlideModifier"].GetFloat();;
	TWIceSlideAngleModifier = PlayerMove["IceSlideAngleModifier"].GetFloat();;
	TWPlayerSpeedIce = PlayerMove["PlayerSpeedIce"].GetFloat();
	TWSlopeAngleToSlide = PlayerMove["SlopeAngleToSlide"].GetFloat();
	TWPlayerShockedTime = PlayerMove["ShockedTime"].GetFloat();
	TWPlayerExplosionShockedTime = PlayerMove["ExplosionShockedTime"].GetFloat();
	if( GEngine->Physics->TWPlayerShockedTime <= 0.0f ) GEngine->Physics->TWPlayerShockedTime = 0.00001f;

	ScriptObject MultiPlayerMove = GScript.Globals()["Tweak"]["MultiPlayerMove"];
	
	TWMPAbsoluteVerticalVelocityBelowWhichDoubleJumpHappens = MultiPlayerMove["AbsoluteVerticalVelocityBelowWhichDoubleJumpHappens"].GetFloat();
	TWMPSecondsWhenYouCanBunnyHopAfterLanding = MultiPlayerMove["SecondsWhenYouCanBunnyHopAfterLanding"].GetFloat();
	TWMPSecondsWhenYouCanBunnyHopBeforeLanding = MultiPlayerMove["SecondsWhenYouCanBunnyHopBeforeLanding"].GetFloat();
	TWMPPlayerSpeed = MultiPlayerMove["PlayerSpeed"].GetFloat();
	TWMPAccelerationWhenWalking = MultiPlayerMove["AccelerationWhenWalking"].GetFloat();
	TWMPDecelerationWhenWalking = MultiPlayerMove["DecelerationWhenWalking"].GetFloat();
	if( TWMPDecelerationWhenWalking > 1.0f ) TWMPDecelerationWhenWalking = 1.0f;
	if( TWMPDecelerationWhenWalking < 0.0f ) TWMPDecelerationWhenWalking = 0.0f;
	TWMPBunnyHopAcceleration = MultiPlayerMove["BunnyHopAcceleration"].GetFloat();
	TWMPPlayerJumpStrength = MultiPlayerMove["JumpStrength"].GetFloat();
	TWMPPlayerStairsUpSpeed = MultiPlayerMove["StairsUpSpeed"].GetFloat();
	TWMPPlayerStairsDownSpeed = MultiPlayerMove["StairsDownSpeed"].GetFloat();
	TWMPMaximalItemPushMass = MultiPlayerMove["MaximalItemPushMass"].GetFloat();
	TWMPMaximalBunnyHopSpeed = MultiPlayerMove["MaximalBunnyHopSpeed"].GetFloat();
	TWMPPlayerSlowdownDuringJump = MultiPlayerMove["SlowdownDuringJump"].GetFloat();
	TWMPStrongAirControl = MultiPlayerMove["StrongAirControl"].GetFloat();
	TWMPWeakAirControl = MultiPlayerMove["WeakAirControl"].GetFloat();
	TWMPSlopeAngleToSlide = MultiPlayerMove["SlopeAngleToSlide"].GetFloat();
	TWMPMinimalTimeBetweenBunnyHops = MultiPlayerMove["MinimalTimeBetweenBunnyHops"].GetFloat();
	TWMPBunnyHopDifficulty = MultiPlayerMove["BunnyHopDifficulty"].GetFloat();
	TWMPPlayerShockedTime = MultiPlayerMove["ShockedTime"].GetFloat();
	TWMPPlayerExplosionShockedTime = MultiPlayerMove["ExplosionShockedTime"].GetFloat();
	if( GEngine->Physics->TWMPPlayerShockedTime <= 0.0f ) GEngine->Physics->TWMPPlayerShockedTime = 0.00001f;
	TWMPMinimalExplosionImpulse = MultiPlayerMove["MinimalExplosionImpulse"].GetFloat();
	MPKnockbackMultiplier = MultiPlayerMove["KnockbackMultiplier"].GetFloat();

	TWMPMaxStepHeightWhileInAir = MultiPlayerMove["MaxStepHeightWhileInAir"].GetFloat();
	TWMPMaxStepHeight = MultiPlayerMove["MaxStepHeight"].GetFloat();
	TWMPPhantomTolerance = MultiPlayerMove["PhantomTolerance"].GetFloat();
	TWMPOverbounce = MultiPlayerMove["Overbounce"].GetFloat();
	TWMPMaxTimeToZeroCamera = MultiPlayerMove["MaxTimeToZeroCamera"].GetInt();
	TWMPMinCameraCatchup = MultiPlayerMove["MinCameraCatchup"].GetFloat();
	TWMPMaxCameraDiff = MultiPlayerMove["MaxCameraDiff"].GetFloat();
	TWMPMaxCorrectPosSearchDist = MultiPlayerMove["MaxCorrectPosSearchDist"].GetFloat();
	TWMPAlternateRJ = MultiPlayerMove["AlternateRocketJump"].GetBool();
	TWMPAlternateRJOnlyMyRockets = MultiPlayerMove["AlternateRocketJumpOnlyMyRockets"].GetBool();
	TWMPAlternateRJWorkAboveThisStrength = MultiPlayerMove["AlternateRocketJumpWorkAboveThisStrength"].GetFloat();

	ScriptObject Cfg = GScript.Globals()["Cfg"];
	MPPredictionInterpolation = Cfg["NetcodeEnemyPredictionInterpolation"].GetBool();
	MPPredictionInterpolationFactor = Cfg["NetcodeEnemyPredictionInterpolationFactor"].GetFloat();
	if( MPPredictionInterpolationFactor > 1.0f ) MPPredictionInterpolationFactor = 1.0f;
	else if( MPPredictionInterpolationFactor < 0.0f ) MPPredictionInterpolationFactor = 0.0f;

	ScriptObject GlobalData = GScript.Globals()["Tweak"]["GlobalData"];
	TWGravity = GlobalData["Gravity"].GetFloat();
	TWPlayerMinimalFallSpeedToDisableJump = GlobalData["MinimalFallSpeedToDisableJump"].GetFloat();;

	ScriptObject QWPlayerMove = GScript.Globals()["Tweak"]["PromodePlayerMove"];

	TWPromodePlayerSpeed = QWPlayerMove["PlayerSpeed"].GetFloat();
	TWPromodePlayerStrafeSpeedModifier = QWPlayerMove["StrafeSpeedModifier"].GetFloat();
	TWPromodePlayerJumpStrength = QWPlayerMove["JumpStrength"].GetFloat();
	TWPromodePlayerMinimalFallSpeedToDisableJump = QWPlayerMove["MinimalFallSpeedToDisableJump"].GetFloat();
	TWPromodePlayerGroundFriction = QWPlayerMove["GroundFriction"].GetFloat();
	TWPromodePlayerStopSpeedFactor = QWPlayerMove["StopSpeedFactor"].GetFloat();
	TWPromodePlayerGroundFrictionModifierNearFalls = QWPlayerMove["GroundFrictionModifierNearFalls"].GetFloat();
	TWPromodePlayerAirAcceleration = QWPlayerMove["AirAcceleration"].GetFloat();
	TWPromodePlayerAcceleration = QWPlayerMove["GroundAcceleration"].GetFloat();
	TWPromodePlayerAirMaxAccelerationSpeedModifier = QWPlayerMove["AirMaxAccelerationSpeedModifier"].GetFloat();
	TWPromodeGravity = QWPlayerMove["Gravity"].GetFloat();
	TWPromodeDoubleJump = QWPlayerMove["DoubleJump"].GetBool();
	TWPromodeSecondsYouHaveSameSpeedAfterLanding = QWPlayerMove["SecondsYouHaveSameSpeedAfterLanding"].GetFloat();
	TWPromodeHorizontalSpeedAfterLandingMultiplier = QWPlayerMove["HorizontalSpeedAfterLandingMultiplier"].GetFloat();
	
	ScriptObject UnderWater = GScript.Globals()["Tweak"]["Underwater"];
	
	TWUnderwaterGravity = UnderWater["Gravity"].GetFloat();
	TWUnderwaterDampingRaiseLinear = UnderWater["RaiseLinear"].GetFloat();
	if( TWUnderwaterDampingRaiseLinear < 1.0f ) TWUnderwaterDampingRaiseLinear = 1.0f;
	TWUnderwaterDampingRaiseAngular = UnderWater["RaiseAngular"].GetFloat();
	if( TWUnderwaterDampingRaiseAngular < 1.0f ) TWUnderwaterDampingRaiseAngular = 1.0f;
	
	TWUnderwaterPlayerSpeed = PlayerMove["UnderwaterSpeed"].GetFloat();
	TWUnderwaterSwimDecelerationSpeed = PlayerMove["SpeedOfDecelerationUnderwater"].GetFloat();
	TWUnderwaterNormalizeMovementVector = PlayerMove["NormalizeMovementVectorUnderwater"].GetBool();

	ScriptObject DeactivatorParameters = GScript.Globals()["Tweak"]["ActiveMeshesDeactivator"];

	TWHighFreqMinLinVel = DeactivatorParameters["HighFrequencyMinimalLinearVelocity"].GetFloat();
	TWHighFreqMinAngVel = DeactivatorParameters["HighFrequencyMinimalAngularVelocity"].GetFloat();
	TWLowFreqMinLinVel = DeactivatorParameters["LowFrequencyMinimalLinearVelocity"].GetFloat();
	TWLowFreqMinAngVel = DeactivatorParameters["LowFrequencyMinimalAngularVelocity"].GetFloat();

	ScriptObject GlassParameters = GScript.Globals()["Tweak"]["Glass"];

	TWMinimalGlassBreakMass = GlassParameters["MinimalGlassBreakMass"].GetFloat();
	TWMinimalGlassBreakSpeed = GlassParameters["MinimalGlassBreakSpeed"].GetFloat();

	oldPlayerSpeed = TWPlayerSpeed;
}

void PhysicsEngine::PackTweaksIntoMessage( UdpMessage* message )
{
	// Pack number of tweaks first
	int pos = message->GetPos();
	message->WriteUShort( 0 );	// placeholder

	// Pack tweaks

	// Single player not synchroed; for what?

	message->WriteFloat( TWMPAbsoluteVerticalVelocityBelowWhichDoubleJumpHappens );	// 01
	message->WriteFloat( TWMPSecondsWhenYouCanBunnyHopAfterLanding );				// 02
	message->WriteFloat( TWMPSecondsWhenYouCanBunnyHopBeforeLanding );				// 03
	message->WriteFloat( TWMPPlayerSpeed );											// 04
	message->WriteFloat( TWMPAccelerationWhenWalking );								// 05
	message->WriteFloat( TWMPBunnyHopAcceleration );								// 06
	message->WriteFloat( TWMPPlayerJumpStrength );									// 07
	message->WriteFloat( TWMPPlayerStairsUpSpeed );									// 08
	message->WriteFloat( TWMPPlayerStairsDownSpeed );								// 09
	message->WriteFloat( TWMPMaximalItemPushMass );									// 10
	message->WriteFloat( TWMPMaximalBunnyHopSpeed );								// 11
	message->WriteFloat( TWGravity );												// 12
	message->WriteFloat( TWMPPlayerMinimalFallSpeedToDisableJump );					// 13
	message->WriteFloat( TWMPPlayerSlowdownDuringJump );							// 14
	message->WriteFloat( TWMPStrongAirControl );									// 15
	message->WriteFloat( TWMPWeakAirControl );										// 16
	message->WriteFloat( TWIceSlideModifier );										// 17
	message->WriteFloat( TWMPSlopeAngleToSlide );									// 18
	message->WriteFloat( TWMPMinimalTimeBetweenBunnyHops );							// 19
	message->WriteFloat( TWMPBunnyHopDifficulty );									// 20
	message->WriteFloat( TWMPPlayerShockedTime );									// 21
	message->WriteFloat( TWMPPlayerExplosionShockedTime );							// 22
	message->WriteFloat( TWMPMinimalExplosionImpulse );								// 23

	// Quake-World style player
	message->WriteFloat( TWPromodePlayerSpeed );											// 24
	message->WriteFloat( TWPromodePlayerStrafeSpeedModifier );							// 25
	message->WriteFloat( TWPromodePlayerJumpStrength );									// 26
	message->WriteFloat( TWPromodePlayerMinimalFallSpeedToDisableJump );					// 27
	message->WriteFloat( TWPromodePlayerGroundFriction );								// 28
	message->WriteFloat( TWPromodePlayerStopSpeedFactor );								// 29
	message->WriteFloat( TWPromodePlayerGroundFrictionModifierNearFalls );				// 30
	message->WriteFloat( TWPromodePlayerAirAcceleration );								// 31
	message->WriteFloat( TWPromodePlayerAcceleration );									// 32
	message->WriteFloat( TWPromodePlayerAirMaxAccelerationSpeedModifier );				// 33
	message->WriteFloat( TWPromodeGravity );												// 34
	message->WriteBool( TWPromodeDoubleJump );											// 35
	message->WriteFloat( TWPromodeSecondsYouHaveSameSpeedAfterLanding );					// 36
	message->WriteFloat( TWPromodeHorizontalSpeedAfterLandingMultiplier );				// 37

	message->WriteFloat( TWMPDecelerationWhenWalking );								// 38

	// Underwater tweaks not synchroed; for now

	int pos2 = message->GetPos();
	message->Seek( pos );
	message->WriteUShort( (unsigned short)(pos2-pos) );
	message->Seek( pos2 );
}

void PhysicsEngine::GetTweaksFromMessage( UdpMessage* message )
{
	// Read number of tweaks first
	int pos = message->GetPos();
	unsigned short posDiff = message->ReadUShort();
	int pos2 = pos + posDiff;

	// Read tweaks
	TWMPAbsoluteVerticalVelocityBelowWhichDoubleJumpHappens = message->ReadFloat();	// 01
	if( message->GetPos() >= pos2 ) return;
	TWMPSecondsWhenYouCanBunnyHopAfterLanding = message->ReadFloat();				// 02
	if( message->GetPos() >= pos2 ) return;
	TWMPSecondsWhenYouCanBunnyHopBeforeLanding = message->ReadFloat();				// 03
	if( message->GetPos() >= pos2 ) return;
	TWMPPlayerSpeed = message->ReadFloat();											// 04
	if( message->GetPos() >= pos2 ) return;
	TWMPAccelerationWhenWalking = message->ReadFloat();								// 05
	if( message->GetPos() >= pos2 ) return;
	TWMPBunnyHopAcceleration = message->ReadFloat();								// 06
	if( message->GetPos() >= pos2 ) return;
	TWMPPlayerJumpStrength = message->ReadFloat();									// 07
	if( message->GetPos() >= pos2 ) return;
	TWMPPlayerStairsUpSpeed = message->ReadFloat();									// 08
	if( message->GetPos() >= pos2 ) return;
	TWMPPlayerStairsDownSpeed = message->ReadFloat();								// 09
	if( message->GetPos() >= pos2 ) return;
	TWMPMaximalItemPushMass = message->ReadFloat();									// 10
	if( message->GetPos() >= pos2 ) return;
	TWMPMaximalBunnyHopSpeed = message->ReadFloat();								// 11
	if( message->GetPos() >= pos2 ) return;
	TWGravity = message->ReadFloat();												// 12
	if( message->GetPos() >= pos2 ) return;
	TWMPPlayerMinimalFallSpeedToDisableJump = message->ReadFloat();					// 13
	if( message->GetPos() >= pos2 ) return;
	TWMPPlayerSlowdownDuringJump = message->ReadFloat();							// 14
	if( message->GetPos() >= pos2 ) return;
	TWMPStrongAirControl = message->ReadFloat();									// 15
	if( message->GetPos() >= pos2 ) return;
	TWMPWeakAirControl = message->ReadFloat();										// 16
	if( message->GetPos() >= pos2 ) return;
	TWIceSlideModifier = message->ReadFloat();										// 17
	if( message->GetPos() >= pos2 ) return;
	TWMPSlopeAngleToSlide = message->ReadFloat();									// 18
	if( message->GetPos() >= pos2 ) return;
	TWMPMinimalTimeBetweenBunnyHops = message->ReadFloat();							// 19
	if( message->GetPos() >= pos2 ) return;
	TWMPBunnyHopDifficulty = message->ReadFloat();									// 20
	if( message->GetPos() >= pos2 ) return;
	TWMPPlayerShockedTime = message->ReadFloat();									// 21
	if( message->GetPos() >= pos2 ) return;
	TWMPPlayerExplosionShockedTime = message->ReadFloat();							// 22
	if( message->GetPos() >= pos2 ) return;
	TWMPMinimalExplosionImpulse = message->ReadFloat();								// 23
	if( message->GetPos() >= pos2 ) return;
	
	// Quake-World style player
	TWPromodePlayerSpeed = message->ReadFloat();											// 24
	if( message->GetPos() >= pos2 ) return;
	TWPromodePlayerStrafeSpeedModifier = message->ReadFloat();							// 25
	if( message->GetPos() >= pos2 ) return;
	TWPromodePlayerJumpStrength = message->ReadFloat();									// 26
	if( message->GetPos() >= pos2 ) return;
	TWPromodePlayerMinimalFallSpeedToDisableJump = message->ReadFloat();					// 27
	if( message->GetPos() >= pos2 ) return;
	TWPromodePlayerGroundFriction = message->ReadFloat();								// 28
	if( message->GetPos() >= pos2 ) return;
	TWPromodePlayerStopSpeedFactor = message->ReadFloat();								// 29
	if( message->GetPos() >= pos2 ) return;
	TWPromodePlayerGroundFrictionModifierNearFalls = message->ReadFloat();				// 30
	if( message->GetPos() >= pos2 ) return;
	TWPromodePlayerAirAcceleration = message->ReadFloat();								// 31
	if( message->GetPos() >= pos2 ) return;
	TWPromodePlayerAcceleration = message->ReadFloat();									// 32
	if( message->GetPos() >= pos2 ) return;
	TWPromodePlayerAirMaxAccelerationSpeedModifier = message->ReadFloat();				// 33
	if( message->GetPos() >= pos2 ) return;
	TWPromodeGravity = message->ReadFloat();												// 34
	if( message->GetPos() >= pos2 ) return;
	TWPromodeDoubleJump = message->ReadBool();											// 35
	if( message->GetPos() >= pos2 ) return;
	TWPromodeSecondsYouHaveSameSpeedAfterLanding = message->ReadFloat();					// 36
	if( message->GetPos() >= pos2 ) return;
	TWPromodeHorizontalSpeedAfterLandingMultiplier = message->ReadFloat();				// 37

	TWMPDecelerationWhenWalking = message->ReadFloat();								// 38
	if( message->GetPos() >= pos2 ) return;

	// Underwater tweaks not synchroed; for now

	message->Seek( pos2 );
}

// 0 - unknown
// 1 - PO
// 2 - ragdoll

int PhysicsEngine::RigidBodyInfo( void* rb, void** result, int& joint )
{
	if( !rb ) return 0;
	hkRigidBody* rigB = (hkRigidBody*)rb;
	if( !rigB->hasProperty( HPEntityPointer ) ) return 0;
	hkPropertyValue p = rigB->getProperty( HPEntityPointer );
#ifdef HAVOK3
	Entity* entity = (Entity*)p.getPtr();
#else
	Entity* entity = (Entity*)p.asPtr;
#endif
	if( entity->PO && ( entity->PO->GetHavokBody() == rb ) ) { *result = (void*)entity; return 1; }
	if( !(entity->Type == Entity::ETypeModel) ) return 0;
	Ragdoll* ragdoll = ((Model*)entity)->RagdollPtr;
	if( !ragdoll ) return 0;
	*result = (void*)entity;
	joint = ragdoll->GetJointFromHavokBody( rb );
	return 2;
}

int PhysicsEngine::BarrierInfo( void* rb )
{
	if( !rb ) return 0;
	hkRigidBody* rigB = (hkRigidBody*)rb;
	if( !rigB->isFixed() ) return 0;
#ifdef HAVOK3
	int cg = rigB->getCollidable()->getCollisionFilterInfo();
#else
	int cg = rigB->getCollidable().getCollisionFilterInfo();
#endif
	switch( cg )
	{
	case HCGBarrier:		return 1;
	case HCGMonsterBarrier:	return 2;
	default:				return 0;
	}
}

#define PHYSICSENGINE_LOAD_SAVE_TOKEN 0x5e47

bool PhysicsEngine::LoadPhysics( FileObject* f )
{
	if( !f->TestToken( PHYSICSENGINE_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: PhysicsEngine::LoadPhysics()\n" );
		return false;
	}

	int size;
	*f >> size;
	Assert( size == mPhysicsWorlds.Size() );
	for( int i = 0; i < size; ++i )
		if( !mPhysicsWorlds[i]->LoadPhysicsWorld( f ) )
			return false;
	*f >> TWPlayerSpeed;
	*f >> TWPlayerJumpStrength;
	return true;
}

bool PhysicsEngine::SavePhysics( FileObject* f )
{
	if( !f->PutToken( PHYSICSENGINE_LOAD_SAVE_TOKEN ) ) return false;

	*f << mPhysicsWorlds.Size();
	for( int i = 0; i < mPhysicsWorlds.Size(); ++i )
		if( !mPhysicsWorlds[i]->SavePhysicsWorld( f ) )
			return false;
	*f << TWPlayerSpeed;
	*f << TWPlayerJumpStrength;
	return true;
}

bool PhysicsEngine::Tick( float timeDelta, bool dontUpdateEntities )
{
	bool yes = false;

	#ifdef ENGINE_LOG
	GLog.Print("^* PET1\n");
	#endif

	for( int i = 0; i < mPhysicsWorlds.Size(); ++i )
		if( mPhysicsWorlds[i]->Tick( timeDelta ) )
			yes = true;

	#ifdef ENGINE_LOG
	GLog.Print("^* PET2\n");
	#endif

	GProfiler.SetState(PHYSICS_TICK_UPDATE_ENTITIES);
	
    if( !dontUpdateEntities ){
        //TRACE("PhysicsEngine::Tick(): updating %d worlds (timeDelta = %g)\n", mPhysicsWorlds.Size(), timeDelta);

        for( int i = 0; i < mPhysicsWorlds.Size(); ++i )
			mPhysicsWorlds[i]->UpdateEntities();
    }

	#ifdef ENGINE_LOG
	GLog.Print("^* PET3\n");
	#endif
	if( canForgetOPS )
	{
		oldPlayerSpeed = TWPlayerSpeed;
		canForgetOPS = false;
	}

	return yes;
}

void PhysicsEngine::UpdatePredictionTick( float timeDelta )
{
	for( int i = 0; i < mPhysicsWorlds.Size(); ++i )
		mPhysicsWorlds[i]->UpdatePredictionTick( timeDelta );
}


bool CreateConvexGeometry( DynamicArray<Vector>& inputVertices, DynamicArray<Vector>& outputVertices, DynamicArray<WORD>& outputTriangles )
{
	hkVector4 temp;
	hkArray<hkVector4> verts;

	for( int i = 0; i < inputVertices.Size(); ++i )
	{
		temp.set( inputVertices[i].X, inputVertices[i].Y, inputVertices[i].Z );
		verts.pushBack( temp );
	}

#ifdef HAVOK3
		hkGeometry geom;
		hkStridedVertices stridedVerts;
		{
			stridedVerts.m_numVertices = inputVertices.Size();
			stridedVerts.m_striding = sizeof(verts[0]);
			stridedVerts.m_vertices = &verts[0](0);
		}
		
		hkGeometryUtility::createConvexGeometry( stridedVerts, geom, HK_GEOM_CONVEXHULL_MODE_FAST );
		//if (!geom)
		//{
		//	printf("PhysicsEngine : CreateConvexGeometry() - PROBLEM CREATING CONVEX GEOMETRY");
		//	return false
		//}
#else
	hkGeometryFiltererTolerances tolerances;
//	tolerances.m_minAllowableVertexDistance = 0.01f;
//	tolerances.m_minRejectionEdgeLength = 0.005f;
//	tolerances.m_colinearVertexThreshold = 0.005f;
//	tolerances.m_minAllowableFaceNormalDot = 0.005f;
	hkGeometry* geom = hkGeometryUtil::createFilteredConvexGeometry( (hkVector4*)(&verts[0](0)), inputVertices.Size(), tolerances );
	if( !geom )
	{
		geom = hkGeometryUtil::createConvexGeometry( (hkVector4*)(&verts[0](0)), inputVertices.Size(), false );
		if( !geom )
			return false;
	}
#endif

#ifdef HAVOK3
	//const hkVector4* vertices = geom.m_vertices;
	//const hkTriangleIndices* triangles = geom.m_triangles;

	outputVertices.ClearArray();
	for( int i = 0; i < geom.m_vertices.getSize(); ++i )
		outputVertices.Add( Vector( geom.m_vertices[i](0), geom.m_vertices[i](1), geom.m_vertices[i](2) ) );

	outputTriangles.ClearArray();
	for( int i = 0; i < geom.m_triangles.getSize(); ++i )
	{
        outputTriangles.Add( geom.m_triangles[i].m_a );
        outputTriangles.Add( geom.m_triangles[i].m_c );
        outputTriangles.Add( geom.m_triangles[i].m_b );
	}
#else
	const hkVector4* vertices = geom->getVertices();
	const hkTriangleIndices* triangles = geom->getTriangles();

	outputVertices.ClearArray();
	for( int i = 0; i < geom->getNumVertices(); ++i )
		outputVertices.Add( Vector( vertices[i](0), vertices[i](1), vertices[i](2) ) );

	outputTriangles.ClearArray();
	for( int i = 0; i < geom->getNumTriangles(); ++i )
	{
        outputTriangles.Add( triangles[i].getVertex(0) );
        outputTriangles.Add( triangles[i].getVertex(2) );
        outputTriangles.Add( triangles[i].getVertex(1) );
	}
	delete geom;
#endif
	return true;
}
