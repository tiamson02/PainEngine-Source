
// #pragma once
#ifndef _PHYSICSENGINE_
#define _PHYSICSENGINE_


class UdpMessage;
class World;

#include "Core/DynArray.h"

#define DEFAULT_PHYSICS_FPS 45

enum CollisionGroups
{
	HCGFixed =					1,
	HCGPhantom =				2,
	HCGNormal =					3,
	HCGBody =					4,
	HCGMissile =				5,
	HCGRagdoll =				6,
	HCGNoncolliding =			7,
	HCGParticles =				8,
	HCGInsideItems =			9,

	HCGActiveRagdoll =			10,
	HCGActiveRagdoll2 =			11,
	HCGActiveRagdoll3 =			12,
	HCGActiveRagdoll4 =			13,
	HCGActiveRagdoll5 =			14,
	HCGActiveRagdoll6 =			15,
	HCGActiveRagdoll7 =			16,
	HCGActiveRagdoll8 =			17,
	HCGActiveRagdoll9 =			18,
	HCGActiveRagdoll0 =			19,

	HCGActiveRagdollColliding =	20,
	HCGActiveRagdollSpecial =	21,

	HCGBarrier =				22,
	HCGPlayerBody =				23,
	HCGFixedSpecial =			24,
	HCGOnlyWithFixedSpecial =	25,

	HCGNormalNCWithSelf =		26,
	HCGMonsterBarrier	=		27,
	HCGFixedWithNoncollidable = 28,	// like fixed, but colliding with "noncolliding"
	HCGNormalBodyNCWithSelf   = 29,

	HCGClientGrenade =			30,
	HCGPlayerBodyNoRegions =	31,
};

class ENGINE_API PhysicsEngine
{
	friend class PhysicsObject;
	friend class PhysicsWorld;
	friend class HavokWorld;
public:
	// Construction/destruction
    PhysicsEngine();
    ~PhysicsEngine();

	// Start module
    void Init();
	// Tick module
	bool Tick( float timeDelta, bool dontUpdateEntities = false );
	void UpdatePredictionTick( float timeDelta );

	// Physics load/save - for savegames
	bool	LoadPhysics( FileObject* f );
	bool	SavePhysics( FileObject* f );

	// World methods
	PhysicsWorld* MakePhysicsWorld( World* world, float deactDelay, float deactMaxPosDiff );
	PhysicsWorld* MakePhysicsWorldFromHKE( World* world, const char* fileName );

	PhysicsWorld* GetWorldFromRigidBody( void* rb );
	int RigidBodyInfo( void* rb, void** result, int& joint );
	int BarrierInfo( void* rb );

	// Tweaks
	void GetTweaksFromScript( void );
	void PackTweaksIntoMessage( UdpMessage* message );
	void GetTweaksFromMessage( UdpMessage* message );

	// Variables to tweak for Adrian's PlayerAction

	// Our player
	float	TWAbsoluteVerticalVelocityBelowWhichDoubleJumpHappens;
	float	TWSecondsWhenYouCanBunnyHopAfterLanding;
	float	TWSecondsWhenYouCanBunnyHopBeforeLanding;
	float	TWPlayerSpeed;
	float	TWBunnyHopAcceleration;
	float	TWPlayerJumpStrength;
	float	TWPlayerStairsUpSpeed;
	float	TWPlayerStairsDownSpeed;
	float	TWMaximalItemPushMass;
	float	TWMaximalBunnyHopSpeed;
	float	TWGravity;
	float	TWPlayerMinimalFallSpeedToDisableJump;
	float	TWPlayerSlowdownDuringJump;
	float	TWStrongAirControl;
	float	TWWeakAirControl;
	float	TWIceSlideModifier;
	float	TWIceSlideAngleModifier;
	float	TWPlayerSpeedIce;
	float	TWSlopeAngleToSlide;
	float	TWPlayerShockedTime;
	float	TWPlayerExplosionShockedTime;

	float	oldPlayerSpeed;
	bool	canForgetOPS;

	// Our multiplayer
	float	TWMPAbsoluteVerticalVelocityBelowWhichDoubleJumpHappens;
	float	TWMPSecondsWhenYouCanBunnyHopAfterLanding;
	float	TWMPSecondsWhenYouCanBunnyHopBeforeLanding;
	float	TWMPPlayerSpeed;
	float	TWMPAccelerationWhenWalking;
	float	TWMPDecelerationWhenWalking;
	float	TWMPBunnyHopAcceleration;
	float	TWMPPlayerJumpStrength;
	float	TWMPPlayerStairsUpSpeed;
	float	TWMPPlayerStairsDownSpeed;
	float	TWMPMaximalItemPushMass;
	float	TWMPMaximalBunnyHopSpeed;
	float	TWMPPlayerMinimalFallSpeedToDisableJump;
	float	TWMPPlayerSlowdownDuringJump;
	float	TWMPStrongAirControl;
	float	TWMPWeakAirControl;
	float	TWMPSlopeAngleToSlide;
	float	TWMPMinimalTimeBetweenBunnyHops;
	float	TWMPBunnyHopDifficulty;
	float	TWMPPlayerShockedTime;
	float	TWMPPlayerExplosionShockedTime;
	float	TWMPMinimalExplosionImpulse;

	float	TWMPMaxStepHeightWhileInAir;
	float	TWMPMaxStepHeight;
	float	TWMPPhantomTolerance;
	float	TWMPOverbounce;
	int		TWMPMaxTimeToZeroCamera;
	float	TWMPMinCameraCatchup;
	float	TWMPMaxCameraDiff;
	float	TWMPMaxCorrectPosSearchDist;
	bool	TWMPAlternateRJ;
	bool	TWMPAlternateRJOnlyMyRockets;
	float	TWMPAlternateRJWorkAboveThisStrength;

	bool	MPPredictionInterpolation;
	float	MPPredictionInterpolationFactor;
	float	MPKnockbackMultiplier;

	// Quake-World style player
	float	TWPromodePlayerSpeed;
	float	TWPromodePlayerStrafeSpeedModifier;
	float	TWPromodePlayerJumpStrength;
	float	TWPromodePlayerMinimalFallSpeedToDisableJump;
	float	TWPromodePlayerGroundFriction;
	float	TWPromodePlayerStopSpeedFactor;
	float	TWPromodePlayerGroundFrictionModifierNearFalls;
	float	TWPromodePlayerAirAcceleration;
	float	TWPromodePlayerAcceleration;
	float	TWPromodePlayerAirMaxAccelerationSpeedModifier;
	float	TWPromodeGravity;
	bool	TWPromodeDoubleJump;
	float	TWPromodeSecondsYouHaveSameSpeedAfterLanding;
	float	TWPromodeHorizontalSpeedAfterLandingMultiplier;

	// Underwater tweaks
	float	TWUnderwaterGravity;
	float	TWUnderwaterDampingRaiseLinear;
	float	TWUnderwaterDampingRaiseAngular;
	float	TWUnderwaterPlayerSpeed;
	float	TWUnderwaterSwimDecelerationSpeed;
	bool	TWUnderwaterNormalizeMovementVector;

	// Active meshes deactivator parameter tweaks
	float	TWHighFreqMinLinVel;
	float	TWHighFreqMinAngVel;
	float	TWLowFreqMinLinVel;
	float	TWLowFreqMinAngVel;

	float	TWMinimalGlassBreakMass;
	float	TWMinimalGlassBreakSpeed;

	// Physics FPS
	int PhysicsFPS;

	// data
private:
	DynamicArray<PhysicsWorld*>	mPhysicsWorlds;
	void* mConvexCache;
};

bool CreateConvexGeometry( DynamicArray<Vector>& inputVertices, DynamicArray<Vector>& outputVertices, DynamicArray<WORD>& outputTriangles );



#endif //_PHYSICSENGINE_

