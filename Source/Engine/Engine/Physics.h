
// HAVOK_COLLISION_TOLERANCE defined in IPDriver.h due to linking order

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
};

enum POStates
{
	// PhysicsObject type
	EPOIsPlayer =				1,
	EPOIsMonster =				1<<1,
	EPOIsActiveMesh =			1<<2,

	// Are collision callbacks on?
	EPOCollisionCB =			1<<3,

	// Player movement states
	EPOFalling =				1<<4,
	EPOAtStepSmall =			1<<5,
	EPOAtStepMedium =			1<<6,
	EPOAtStepBig =				1<<7,
	EPOAtSteps =				(1<<5)+(1<<6)+(1<<7),
	EPOBunnyHopTime =			1<<8,
	EPODoubleJumped =			1<<9,

	// addEntity assert
	EPOAddEntityAssert =		1<<10,

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
};

class POHavokPart;
class PhysicsEngine;

class PhysicsObjectPlayerVariables
{
	friend class PhysicsObject;
	friend class PhysicsWorld;
private:
	float				mAccumulatedDelta;		// Time delta that went by frm the last PlayerAction; used in PlayerAction
	void*				FakeBulletTimeGravity;

	// PlayerAction support variables
	float				TimeSinceJumped;
	float				TimeSinceLanded;
	int					BunnyHops;
	Vector				JumpVector;
	bool				NearLadder;

	PhysicsObjectPlayerVariables( void ) : mAccumulatedDelta(0.0f), TimeSinceJumped(0.0f), TimeSinceLanded(0.0f),
		BunnyHops(0), JumpVector(0,0,0), NearLadder(false), FakeBulletTimeGravity(0) {};
	~PhysicsObjectPlayerVariables() {};
};

class ENGINE_API PhysicsObject
{
	friend class PhysicsWorld;
private:
	Vector				mModelPivotOffset;		// The offset of model, relative to physics object offset
	char				mFreedomOfRotation;		// Remembers the current freedom of rotation

	int					StepCheck( Vector moveVector );

	PhysicsObjectPlayerVariables*	mPlayerVariables;

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

	float				mVolumeMass;
	Vector				prevPos;
	Vector				prevVel;
public:
	Vector				floorNormal;
	float				havokInfluenceInMonsterMovement;
	bool				doNotCheckFloors;
	bool				onFloor;

	// -------------- Variables --------------------
	DWORD				State;
	DWORD				Action;

	// Havok variables
	POHavokPart*		mHavokPart;

	// ----------------------- Methods -----------------------------------

	// Construction/destruction
	PhysicsObject( void );
	~PhysicsObject( void );

	// Maintenance
	void StepTimeDelta( float timeDelta ) { if( mPlayerVariables ) mPlayerVariables->mAccumulatedDelta += timeDelta; };	
	void Tick( float timeDiff );
	inline bool IsPlayer( void ) { return ( mPlayerVariables != 0 ); };
	void SetGlass( Entity* entity );

	// Action handlers
	void PlayerAction( Vector& forwardVector, Vector& rightVector, char* debug );
	void PlayerLeap( Vector& rightVector, Vector& forwardVector, float leapStrength = 1.0f );

	void SetMoveVector( Vector& vec ) { mMoveVector = vec; };
	void SetMonsterVisionProperties( float viewDistance, float viewDistance360degrees, float maxYaw, float maxPitch = F_PI/2 )
	{
		mHowFarICanSee = viewDistance;
		mHowFarICanSee360 = viewDistance360degrees;
		mViewAngleYaw = maxYaw;
		mViewAnglePitch = maxPitch;
	};

	// Physics Object properties
	// ---------------------------
	//		Freedom of rotation
	char GetFreedomOfRotation( void ) { return mFreedomOfRotation; };
	void SetFreedomOfRotation( int type, float softness = 1.0f );

	//		Friction
	float GetFriction( void );
	void SetFriction( float friction );

	//		Mass
	float GetMass( void );
	void SetMass( float mass );
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
	void SetCollisionCallbacks( bool enable, float minimalDelay = 0.0f, float minimalStrength = 0.0f, float minimalNonFixedDelay = 0.0f, float minimalOtherBodyMass = 0.0f );
	BOOL GetCollisionCallbacks( void ) { return ((State & EPOCollisionCB) != 0); };

	//		Can be caught by line trace?
	void EnableLineTraceCollision( bool enable );
	bool CanLineTraceCollision( void );

	//		Collision groups - which type of the body we have ( Fixed, Missile, Ragdoll, Normal, etc. )
	int GetCollisionGroup( void );
	void SetCollisionGroup( int newGroup );

	//		Activating/deactivating objects
	void Activate( bool activate );
	inline bool IsActive( void );

	//		Enabling/disabling objects
	void Enable( bool enable );
	inline bool IsEnabled( void );

	// Movement and position
	// ----------------------

	//		Orientation
	void SetOrientation( float angle );
	float GetOrientation() const;

	Vector GetForwardVec( void ) const;		// This retrieves the vector pointing to the PO front
	Vector GetRightVec( void ) const;		// This retrieves the vector pointing to the right of PO front

	//		Position
	void SetPosition( const Vector& pos );
	Vector GetPosition( void ) const;
	Vector GetCenterOfMass( void ) const;
	void MoveCenterOfMass( Vector& relative ) const;
	
	//      Access to havok body
    void *GetHavokBody() const;

	//		Set position and orientation in timeDelta by means of keyframed velocity setting
	void SetPositionAndRotationWithSoftKeyframe( const Vector& pos, const Quaternion& quat, float deltaTime );

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
	void Rotate( Vector& rotateVector );				// This applies angular impulse to PhysicsObject
	void Hit( Vector& hitPoint, Vector& hitVector );	// This applies linear and angular impulse.
														// Like PO's been hit at point with vector strength.
	void Impulse( Vector& hitPoint, Vector& hitVector ); // pure impulse

	//		Is the monster/player in the air?
	bool FloorCheck(float dist = 0);

	// Other methods
	// ---------------------------------

	float GetMaxSphereRay( void );

	//		Gravity force
	void EnableGravity( bool enable );

	//		Speed damping
	void EnableSpeedDamping( bool enable, float maxSpeed, float dampingSpeed, float dampingStrength );

	//		Monster jump tweaks
	void SetFlying( bool enable ) { if( enable ) State |= EPOIsFlying; else State &=~EPOIsFlying; };

	//		Explosion effects of PO
	void IsMovedByExplosions( bool enable ) { if( enable ) State &= ~EPOExplosionsDontWorkOnIt; else State |= EPOExplosionsDontWorkOnIt; };
	bool IsMovedByExplosions( void ) { return !( State & EPOExplosionsDontWorkOnIt ); };

	//		Pinning/unpinning of PO
	inline void SetPinned( bool enable );
	inline bool IsPinned( void );

	void SetHardDeactivator( void );
};

class HavokEngine;
class Model;
class ModelSkeleton;
class RagdollSkeleton;
class HavokRagdoll;

class ENGINE_API Ragdoll
{
	friend class PhysicsWorld;
public:
	Ragdoll( void );
	~Ragdoll( void );

	inline bool Init( Model* model, RagdollSkeleton* ragdollSkeleton, const char* fileName, float scale );
	inline void Activate( const DynamicArray<Matrix>& frameMatrix, bool colliding, bool special );
	inline void MakeSpecial( bool special, bool colliding );
	inline void Animate( const DynamicArray<Matrix>& frameMatrix );
	inline void Deactivate( void );
	inline bool IsActive( void );
	inline void SetMass( float mass );
	inline void BreakConstraintsForJoint( int joint );
	inline void ApplyVelocitiesToAllJoints( const Vector& v, const Vector& av );
	inline void MoveAllJoints( const Vector& v );
	inline void ApplyVelocitiesToJoint( int joint, const Vector& v, const Vector& av );
	inline void ApplyVelocitiesToJointLinked( int joint, const Vector& v, const Vector& av );
	inline void ApplyPositionToJoint( int joint, const Vector& v );
	inline void ApplyRotationToJoint( int joint, const Quaternion& q );
	inline void ApplyPointImpulse( Vector point, Vector impulse );
	inline void SetPinnedJoint( int joint, bool enable );
	inline bool IsPinnedJoint( int joint );
	inline void EnableLineTraceCollision( bool enable );
	inline void SetLinearDamping( float linearDamping );
	inline void SetAngularDamping( float angularDamping );
	inline void SetRagdollBoneCollisionCallbacks( int joint, float minimalDelay = 0.0f, float minimalStrength = 0.0f, float minimalNonFixedDelay = 0.0f, float minimalOtherBodyMass = 0.0f );
	inline int  GetJointFromHavokBody( void* body );
	inline bool JointsLinked( int joint1, int joint2 );

private:
	HavokRagdoll* havokRagdoll;
};

enum EPawnBody
{
	EPawnBodyDefault = 0,
	EPawnBodySimple = 1,
	EPawnBodySphere = 1,
	EPawnBodyFatter = 2,
	EPawnBodyMissile = 3,

	EPawnFromMesh = 4,
	EPawnFromMeshConvex = 4,
	EPawnFromMeshNotConvex = 5,
	EPawnFromMeshGlass = 6,
	EPawnFromMeshNonCentered = 7,
	EPawnFromMeshNonCenteredNotConvex = 8,

	EPawnBodyPlayer = 100,
};

enum Directions
{
	Front = 0,
	Back = 1,
	Left = 2,
	Right = 3,
};

struct HitData
{
    float   Dist;
    Vector  HitPoint;
    Vector  Normal;
	void*   HavokEntity;
	void*	UserData;
};

class HavokWorld;

class ENGINE_API PhysicsEngine
{
	friend class PhysicsObject;
	friend class PhysicsWorld;
public:
	// Construction/destruction
    PhysicsEngine();
    ~PhysicsEngine();

	// Start module
    void Init();
	// Tick module
	void Tick( float timeDelta );

	// World methods
	PhysicsWorld* MakePhysicsWorld( World* world );
	PhysicsWorld* MakePhysicsWorldFromHKE( World* world, const char* fileName );

	PhysicsWorld* GetWorldFromRigidBody( void* rb );

	// Tweaks
	void GetTweaksFromScript( void );

	// Variables to tweak for Adrian's PlayerAction
	float	TWAbsoluteVerticalVelocityBelowWhichDoubleJumpHappens;	// 1.0f
	float	TWSecondsWhenYouCanBunnyHopAfterLanding;				// 0.2f
	float	TWSecondsWhenYouCanBunnyHopBeforeLanding;				// 0.2f
	float	TWPlayerSpeed;											// 10.0f
	float	TWFloorSlopeCosineOverWhichYouCantStandOnFloor;			// 0.7
	float	TWSpeedOfDeceleratincWhenWalking;						// 0.2
	bool	TWNormalizeMovementVector;								// false
	float	TWAirControlCoefficient0To1;							// 0.5f
	float	TWBunnyHopAcceleration;									// 0.2f
	bool	TWPainKillerLeapsUpward;								// true
	float	TWPlayerJumpStrength;									// 1.0f
	float	TWPlayerStairsUpSpeed;									// 1.0f
	float	TWPlayerStairsDownSpeed;								// 1.0f

	// Underwater tweaks
	float	TWUnderwaterGravity;									// 0.1f
	float	TWUnderwaterDampingRaiseLinear;							// 2.0f
	float	TWUnderwaterDampingRaiseAngular;						// 1.5f
	float	TWUnderwaterPlayerSpeed;								// 10.0f
	float	TWUnderwaterSwimDecelerationSpeed;						// 0.2f
	bool	TWUnderwaterNormalizeMovementVector;					// false

	// Active meshes deactivator parameter tweaks
	float	TWHighFreqMinLinVel;									// 0.5f
	float	TWHighFreqMinAngVel;									// 0.5f
	float	TWLowFreqMinLinVel;										// 1.0f
	float	TWLowFreqMinAngVel;										// 1.0f

	float	TWMinimalGlassBreakMass;								// 20.0f
	float	TWMinimalGlassBreakSpeed;								// 10.0f

	// data
private:
	DynamicArray<PhysicsWorld*>	mPhysicsWorlds;
};

class ENGINE_API PhysicsWorld
{
	friend class PhysicsEngine;
	friend class PhysicsObject;
	friend class Ragdoll;
public:
	~PhysicsWorld();

	void Start();
	void Tick(float timeDelta);
	void Pause();

	PhysicsObject* CreatePhysicsObject( const Vector& pos, const Quaternion& quat, DWORD poBodyType, Entity* entity = NULL, float scale = 1.0f );
	
	bool CalculatePawnToPointVisibility( PhysicsObject* po, Vector& positionToSee );
	bool CalculatePawnToEntityVisibility( PhysicsObject* po, PhysicsObject* entity );

	// Region methods
	void* CreateRegionFromPoints( DynamicArray<Vector>& points, void* userInfo, float mass );	// Returns NULL on failure
	void DestroyRegion( void* regionHandle );
	void SetRegionPos( void* regionHandle, const Vector& pos );
	Vector GetRegionPos( void* regionHandle );

	Ragdoll* GetRagdollByModel( Model* model );

	// Ragdoll havok body management
	Quaternion GetHavokBodyRotation( void* body );
	void SetHavokBodyRotation( void* body, Quaternion& quat );
	Vector GetHavokBodyVelocity( void* body );
	void SetHavokBodyVelocity( void* body, Vector vel );
	Vector GetHavokBodyPosition( void* body );
	void SetHavokBodyPosition( void* body, Vector vel );
	void PinHavokBody( void* body, bool pin );
	void RemoveHavokBodyFromIS( void* body, bool remove );
	
	// Box tracing
	void GetIntersectingEntities( BoundingBox& box, DynamicArray<Entity*>& entityList, int collisionGroup = -1 );

	// Line tracing
    BOOL LineTrace(const Vector& start, const Vector& end, HitData& hitData );
    BOOL LineTraceHitPlayer(const Vector& start, const Vector& end, HitData& hitData );
    BOOL LineTraceFixedGeom(const Vector& start, const Vector& end, HitData& hitData );
	BOOL LineTraceFirstHit(const Vector& start, const Vector& end );
	void HitEntity(void* entity, const Vector& hitPoint, const Vector& hitVector);

	// Explosion

	void Explosion2(Vector center, float pow, float range, Entity *owner, int weapon_type, float weapon_damage );
	void ExplosionUp( Vector center, float strength, float radius, float maxVelocity, float velocityVariation );
	void ExplosionParabolic( Vector center, float flightTime, float radius, Vector target );
//	void ExplosionBossArena(Vector center, float pow, float range, Entity *owner, int weapon_type, float weapon_damage);
	void SelfExplosion( Vector center, float strength, float radius, DynamicArray<PhysicsObject*>* targetObjects );

	void SetCollisionToAll(bool enable, float minimalDelay, float minimalStrength, float minimalNonFixedDelay, float minimalMass = 150, float maximalMass = 5000, float chance = 10);

	// Manual constraint management
	void* CreateRigidConstraint( Vector pivot, Model* model, int joint, PhysicsObject* po );
	void* CreateRigidConstraint( Vector pivot, PhysicsObject* po1, PhysicsObject* po2 );
	void* CreateRigidConstraint( Vector pivot, Model* model1, int joint1, Model* model2, int joint2 );
	void* CreateRigidConstraint( Vector pivot, void* hpo1, void* hpo2 );
	inline void RemoveRigidConstraint( void* constraint );

	void CreateSpringConstraint( void* entity1, Vector pivot1, void* entity2, Vector pivot2, float shorteningSpeed );

	// Closest points
	float ClosestPointsDistanceBetween( PhysicsObject* po1, PhysicsObject* po2 );
	float ClosestPointsDistanceBetween( PhysicsObject* po1, Model* model2, int joint2 );
	float ClosestPointsDistanceBetween( Model* model1, int joint1, Model* model2, int joint2 );

	// Static meshes
	void ReleaseStaticMeshes( void );
	void RemoveStaticMesh( void* mesh );

	// Underwater world methods
	inline void EnableUnderwaterWorld( bool enable );
	inline bool IsUnderwaterWorld( void );

	void ReloadWorld( void );
	void GetFromHKE( const char* fileName );

	bool CheckStartGlass( void* body, Vector hitPoint, float activationRadius = 0);

	float ActiveMeshesMassScale;

private:
	PhysicsWorld();

	PhysicsObject* CreatePhysicsObjectFromMesh( const Vector& pos, const Quaternion& quat, DWORD poBodyType, WorldMesh* mesh );
//	PhysicsObject* CreatePhysicsObjectFromHKE( WorldMesh* mesh, const char* fileName, float scale );

	void AddMesh(WorldMesh* mesh);

	bool VisibilityWalkPortals( RaySegment& ray, Zone* zoneStart, Zone* zoneEnd );
	int ResetWorldPortalsCounter( void );
	void SetWorldPortalsCounter( int counter );

	// Variables
	HavokWorld*	mHavokWorld;
	World* mEngineWorld;
	bool mPaused;
	bool mFirstTime;
};
