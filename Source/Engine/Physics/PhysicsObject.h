
// #pragma once
#ifndef _PHYSICSOBJECT_
#define _PHYSICSOBJECT_


#define MAX_FORCE_PER_TICK 500.0f
#define MAX_FORCE_TICKS 10

class Entity;
class Glass;
class PlayerInput;
class HavokMissileController;
struct HitData;

#include "Core/Math/LeMath.h"
#include "Core/Median.h"

enum POStates
{
	// PhysicsObject type
	EPOIsPlayer =				1,
	EPOIsMonster =				1<<1,
//	EPOIsActiveMesh =			1<<2,

	// Are collision callbacks on?
	EPOCollisionCB =			1<<3,
	EPOCreatedWithNoScale =		1<<4,
	EPOGrenadeFlightFix =		1<<5,
	EPOEntitySteered =			1<<10,

	// additional tweak for monster jumps
	EPOIsFlying =				1<<11,

	EPOExplosionsDontWorkOnIt =	1<<12,
	EPOHasOurDeactivator =		1<<13,
};

enum FreedomOfRotation
{
	ERotDisabled = 0,
	ERotYAxis = 1,
	ERotAllAxes = 2,
	ERotFullFree = 3,
	ERotHardTurn = 4,
	ERotXAxis = 5,
	ERotZAxis = 6,
};

enum EPawnBody
{
	EPawnBodyDefault = 0,
	EPawnBodySphere = 1,
	EPawnBodyFatter = 2,
	EPawnBodyDefaultSweep = 3,

	EPawnFromMeshConvex = 4,
	EPawnFromMeshNotConvex = 5,
	EPawnFromMeshGlass = 6,
	EPawnFromMeshNonCentered = 7,
	EPawnFromMeshNonCenteredNotConvex = 8,

	// Sweep types
	EPawnBodySphereSweep = 9,
	EPawnBodyFatterSweep = 10,
	EPawnFromMeshConvexSweep = 11,
	EPawnFromMeshNonCenteredSweep = 12,
	EPawnFromMeshGlassSweep = 13,

	// Ragdoll type
	EPawnRagdoll = 15,

	EPawnBodyPlayer = 100,
};

class POHavokPart;
class PhysicsEngine;

#define Y_INTERPOLATION_SET_SIZE 4

#ifndef TRANSGAMING_NO_HAVOK
// Blair - We currently don't support Havok on the PSP.
class HavokCharacterController;
#endif

class PhysicsObjectPlayerVariables
{
	friend class PhysicsObject;
	friend class PhysicsWorld;
	friend class HavokWorld;
	friend class NetworkDevice;
	friend class NetworkClient;
#ifndef TRANSGAMING_NO_HAVOK
// Blair - We currently don't support Havok on the PSP.
	friend class HavokCharacterController;
#endif
protected:
	float	AccumulatedDelta;		// Time delta that went by frm the last PlayerAction; used in PlayerAction
	void*	FakeBulletTimeGravity;

	// PlayerAction support variables

	bool	AtSteps:1;
	bool	Falling:1;
	bool	CanBunnyHop:1;
	bool	DoubleJumped:1;
	bool	Exploded:1;

	bool	StickToUnderBodyPending:1;
	bool	StickToUnderBody:1;
	void*	UnderBody;
	Vector	StickPosDiff;

	float	TimeSinceJumped;
	float	RealTimeSinceJumped;
	float	TimeSinceLanded;
	Vector	JumpVector;
	DWORD	JumpAction;
	DWORD	LastNonzeroAction;
	float	TimeToBeShocked;
	float	TimeSinceExplosion;
	float	TimeSinceBunnyHop;
	Vector	LastExecutionSpeed;
	Vector	LastGoodPosition;
	DWORD	LastPlayerMovementFixFrame;
	bool	RevertedToLastGoodPositionLastFrame;
	float	LowestYSpeed;
	float	Speed;
	unsigned char OnSlopeTimes;
	float	TimeModifierLastTime;
	bool	JumpInvalidation;
	bool	BulletTimeShock;

	// Drawing myself out of rocks helper
	float	HighestSpeedOfGoingOut;
	int		FramesInRocksAndNotGoingOut;

	// MultiPlayerAction support variables
	bool	JumpPressed;
	float	TimeSinceLastOnGround;
	float	SpeedWhenLanded;

//	Median<float,Y_INTERPOLATION_SET_SIZE> YCorrection;

	float	ReportFallDamage( float speedNow );
	void	Load( FileObject* f, PhysicsObject* ourObject );
	void	Save( FileObject* f );
public:
	PhysicsObjectPlayerVariables( void );
	~PhysicsObjectPlayerVariables( void );

#ifndef TRANSGAMING_NO_HAVOK
	HavokCharacterController*	Controller;
#else
// Blair - We don't currently support Havok on the PSP but will
// eventually. We'll put this in as a placeholder.
	void*				Controller;
#endif
	PhysicsObjectPlayerVariables& operator=( const PhysicsObjectPlayerVariables& other );
};

struct POForce
{
	float ForceLeft;
	Vector ForceDir;
	Vector ForcePoint;
	bool HavePoint;
	POForce* NextForce;
};

class ENGINE_API PhysicsObject
{
	friend class PhysicsWorld;
	friend class HavokWorld;

	// For old netcode
	friend class POSynchronizer;
	friend class NetworkDevice;		// for predictions
	friend class NetworkClient;		// for predictions

	// For new netcode
	friend class EntitySynchronizer2;
	friend class MeshSynchronizer2;
	friend class ModelSynchronizer2;
	friend class NetworkPeer2;
protected:

	Vector				mModelPivotOffset;		// The offset of model, relative to physics object offset
	char				mFreedomOfRotation;		// Remembers the current freedom of rotation

	int					StepCheck( Vector moveVector, float distanceModifier = 1.0f );
	void				PlayerActionUnderwater( Vector& forwardVector, Vector& rightVector );
	void				PlayerActionLadder( Vector& forwardVector, Vector& rightVector );
	void*				MeshUnder( float distance );
//	float				PlayerTrace( Vector& localStart, Vector& localEnd );
	Vector				GetPivotOffset() const; // gets transformed model pivot offset 

	inline void			MPPredRemoveFromBatch( bool addToWorld );

	int					mPOTableIndex;	// maintenance
	float				mInertiaTensorMultiplier;		// for load/save

	// Physics object's body type and scale info
	DWORD				BodyType;
	float				BodyScale;

	// Monster vision variables
	float				mHowFarICanSee;
	float				mHowFarICanSee360;
	float				mViewAngleYaw;
	float				mViewAnglePitch;

	// Monster movement speed&direction vector
	Vector				mMoveVector;
	//float				boost;

	// Monster rotation accumulation vector
	float				mAngularInTick;

	POForce*			ForcesToApply;

	float				mVolumeMass;
	Vector				prevVel;

	bool				PositionBugFixed( void );
	void				FixHavokPositionBug( void );
	void				MovePlayerOutOfWall( void );

	HavokMissileController* mMissileController;

public:
	PhysicsObjectPlayerVariables*	mPlayerVariables;

	Vector				floorNormal;
	float				havokInfluenceInMonsterMovement;
	bool				doNotCheckFloors;
	bool				onFloor;
    bool                onLadder;

	// -------------- Variables --------------------
	DWORD				State;

/*#ifdef TRANSGAMING
    DWORD               getAction() const{ return Action; }
    void                setAllAction(DWORD a){ Action = a; }
    void                setAction(DWORD flag){ Action |= flag; }
    void                clearAction(DWORD flag){ Action &= ~flag; }

private:
#endif//*/
	DWORD				Action;

public:
	// Havok variables
	POHavokPart*		mHavokPart;

	// ----------------------- Methods -----------------------------------

	// Construction/destruction
	PhysicsObject( void );
	virtual ~PhysicsObject( void );

	// Maintenance
	void Tick( float timeDiff );
	void UpdateEntity( float tickPart = 0.0f );
	inline bool IsPlayer( void );
	void AttachPlayerToUnderbody( void ) { if( mPlayerVariables ) mPlayerVariables->StickToUnderBodyPending = true; };
	void DetachPlayerFromUnderbody( void );
	void SetAsGlass( Glass* glass );
	Glass* GetGlass( void );
	short GetActiveMeshIndex( void );
	Entity* GetEntity( void );
	int GetType( void );
	bool JumpedInLastAction( void );

	float GetDistanceFromPoint( Vector point );

	// Divided forces
	void EffectForce( Vector force, const Vector* point = 0 );
	void EffectForces( void );
	void RemoveForces( void );

	// Action handlers
	void PlayerAction( Vector& forwardVector, Vector& rightVector );
	void MultiPlayerAction( Vector& forwardVector, Vector& rightVector, bool prediction = false );
//	void MultiPlayerActionQW( Vector& forwardVector, Vector& rightVector );
	void MultiPlayerActionNonPhysic( float yaw, float pitch, float timeDelta );
	void MultiPlayerActionNonPhysic( PlayerInput* input, bool setRotation, bool enablePredictionFromThisState );
	void MultiPlayerActionSetupPredictionTime( DWORD time );
	void MultiPlayerGetOrientFromController( void );
	short GetPlayerPitch( void );
	void SetMPByte( unsigned char mpbyte );
	unsigned char GetMPByte( void );
	void AddMPExternalInfluence( void );
	float GetCameraFix( void );

	void FixGrenadeFlight( Entity* entity );	// So grenades won't push through walls
	void FixPlayerMovement( Entity* entity );	// So player won't push through walls
	void SetGrenade( bool grenade ) { if( grenade ) State |= EPOGrenadeFlightFix; else State &= ~EPOGrenadeFlightFix; };
	void SetMPMissileController( int type = 0 );

	bool LoadPO( FileObject* f );
	bool SavePO( FileObject* f );

	void SetMoveVector( Vector& vec );
	inline void SetMonsterVisionProperties( float viewDistance, float viewDistance360degrees, float maxYaw, float maxPitch = F_PI/2 );

	void SetNotPredictionAffected( void );
	void SetSynchronizedProp( void );

	// Physics Object properties
	// ---------------------------
	//		Freedom of rotation
	inline char GetFreedomOfRotation( void );
	void SetFreedomOfRotation( int type, float softness = 1.0f );

	//		Friction
	float GetFriction( void );
	void SetFriction( float friction );

	//		Mass
	float GetMass( void );
	void SetMass( float mass );
	void ScaleMass( float scaleMultiplier );			// this changes the inertia tensor too!
	void ScaleInertiaTensor( float scaleMultiplier );	// this changes the inertia tensor
	float GetVolumeMass( void );

	//		Restitution
	float GetRestitution( void );
	void SetRestitution( float restitution );

	//		Linear damping
	float GetLinearDamping( void );
	void SetLinearDamping( float damping );

	//		Angular damping
	float GetAngularDamping( void );
	void SetAngularDamping( float damping );

	// Collisions, enable/disable, activation
	// ---------------------
	//		Reporting
	void SetCollisionCallbacks( bool enable, float minimalDelay = 0.0f, float minimalStrength = 0.0f, float minimalOtherBodyMass = 0.0f );
	inline bool GetCollisionCallbacks( void );

	//		Can be caught by line trace?
	void EnableLineTraceCollision( bool enable );
	bool CanLineTraceCollision( void );

	//		Collision groups - which type of the body we have ( Fixed, Missile, Ragdoll, Normal, etc. )
	int GetCollisionGroup( void );
	void SetCollisionGroup( int newGroup );

	//		Activating/deactivating objects
	void Activate( bool activate );
#ifdef TRANSGAMING
	bool IsActive( void );
#else
	inline bool IsActive( void );
#endif
	//		Enabling/disabling objects
	virtual void Enable( bool enable );
#ifdef TRANSGAMING
	bool IsEnabled( void );
#else
	inline bool IsEnabled( void );
#endif
	// Movement and position
	// ----------------------

	//		Orientation
	void SetOrientation( float angle );
	float GetOrientation() const;

	Vector GetForwardVec( void ) const;		// This retrieves the vector pointing to the PO front
	Vector GetRightVec( void ) const;		// This retrieves the vector pointing to the right of PO front

	//		Position
	void SetPosition( const Vector& pos, bool updateSave = false );
	Vector GetPosition( void ) const;
	Vector GetCenterOfMass( void ) const;
	void MoveCenterOfMass( Vector& relative ) const;
	
	//      Access to havok body
    void *GetHavokBody() const;

	//		Set position and orientation in timeDelta by means of keyframed velocity setting
	void SetPositionAndRotation( const Vector& pos, const Quaternion& quat );

	//		Specialized position
	void SetPawnHeadPos( const Vector& pos );
	Vector GetPawnHeadPos( void ) const;
	Vector GetPawnFloorPos( void ) const;

	//		Rotation quaternion
	void SetRotQuaternion( const Quaternion& quat );
	Quaternion GetRotQuaternion( void );

	//		Velocity
    Vector GetVel( void ) const;
	void SetVel( const Vector& vel );

	void ZeroVelocity( void );
	void ZeroAngularVelocity( void );

	//		Angular velocity
	Vector GetAngularVel( void  );
	void SetAngularVel( const Vector& angVel );

	//		Actions you can perform on PO
	void Rotate( Vector& rotateVector );									// This applies angular impulse to PhysicsObject
	void Hit( const Vector& hitPoint, const Vector& hitVector );			// This applies linear and angular impulse.
																			// Like PO's been hit at point with vector strength.
	void Impulse( const Vector& hitPoint, const Vector& hitVector );		// pure impulse

	//		The subsystem to make ragdoll rotation from shotgun pellets work
	void AddRotateActor( const Vector& hitPoint, const Vector& hitVector );
	void EffectRotateActor( void );

	//		Is the monster/player in the air?
	bool FloorCheck( float distance1 = -7.0f, float distance2 = 4.5f, float* hitAt = NULL, bool fixedGeomOnly = false, bool takeFromController = true );
	bool MonsterFloorCheck( float distance1 = -7.0f, float distance2 = 4.5f, float* hitAt = NULL, bool fixedGeomOnly = false );
	bool FloorCheckRandom( float distance1 = -7.0f, float distance2 = 4.5f );
	bool FloorCheckMP( float down );

	// Manipulator methods
	// ---------------------------------

	//		Gravity force
	void EnableGravity( bool enable );
	void SetGravity( Vector gravity );
	Vector GetGravity( void );

	//		Speed damping
	void EnableSpeedDamping( bool enable, float maxSpeed, float dampingSpeed, float dampingStrength );

	//		Linear Movement Maintainer
	void MaintainLinearMovement( bool enable, Vector axis, bool soft = false );

	//		Position Maintainer
	void MaintainPosition( bool enable, Vector desiredPosition, float strength );

	//		Velocity Maintainer
	void MaintainVelocity( bool enable, Vector desiredVelocity, float strength );

	//		Angular Velocity Maintainer
	void MaintainAngularVelocity( bool enable, Vector desiredVelocity, float strength );

	//		Transporter Property
	void SetAsTransporter( bool enable, Vector appliedVelocity, float strength );

	// Other methods
	// ---------------------------------

	inline void SetEntitySteered( bool steered );
	inline bool GetEntitySteered( void );
	float GetMaxSphereRay( void );
	bool LineTrace( Vector& start, Vector& end, HitData& hitData );
	void SetPlayerShocked( void );

	//		Monster jump tweaks
	inline void SetFlying( bool enable );
#ifdef TRANSGAMING
	void SetPlayerShocked( float timeToBeShocked );
#else
	inline void SetPlayerShocked( float timeToBeShocked );
#endif
	//		Explosion effects of PO
	inline void IsMovedByExplosions( bool enable );
	inline bool IsMovedByExplosions( void );

	//		Pinning/unpinning of PO
#ifdef TRANSGAMING
	void SetPinned( bool enable );
	bool IsPinned( void );
#else
	inline void SetPinned( bool enable );
	inline bool IsPinned( void );
#endif

#ifdef TRANSGAMING
	bool IsFixed( void );
#else
	inline bool IsFixed( void );
#endif
	inline bool IsPhys( void );

	void SetHardDeactivator( void );
//	float CorrectY( float y );
};

// Inlines
inline bool PhysicsObject::IsPlayer( void ) { return ( mPlayerVariables != 0 ); }
inline void PhysicsObject::SetMoveVector( Vector& vec ) { mMoveVector = vec; }

inline void PhysicsObject::SetMonsterVisionProperties( float viewDistance, float viewDistance360degrees, float maxYaw, float maxPitch )
{
	mHowFarICanSee = viewDistance;
	mHowFarICanSee360 = viewDistance360degrees;
	mViewAngleYaw = maxYaw;
	mViewAnglePitch = maxPitch;
}

inline char PhysicsObject::GetFreedomOfRotation( void ) { return mFreedomOfRotation; }
inline bool PhysicsObject::GetCollisionCallbacks( void ) { return ((State & EPOCollisionCB) != 0); }
inline void PhysicsObject::SetEntitySteered( bool steered ) { if( steered ) State |= EPOEntitySteered; else State &= ~EPOEntitySteered; }
inline bool PhysicsObject::GetEntitySteered( void ) { return ((State & EPOEntitySteered) != 0 ); }
inline void PhysicsObject::SetFlying( bool enable ) { if( enable ) State |= EPOIsFlying; else State &=~EPOIsFlying; }
inline void PhysicsObject::IsMovedByExplosions( bool enable ) { if( enable ) State &= ~EPOExplosionsDontWorkOnIt; else State |= EPOExplosionsDontWorkOnIt; }
inline bool PhysicsObject::IsMovedByExplosions( void ) { return !( State & EPOExplosionsDontWorkOnIt ); }



#endif //_PHYSICSOBJECT_

