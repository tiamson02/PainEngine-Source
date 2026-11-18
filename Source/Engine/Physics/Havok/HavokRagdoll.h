
// #pragma once
#ifndef _HAVOKRAGDOLL_
#define _HAVOKRAGDOLL_


class hkRigidBody;
#ifdef HAVOK3
class hkConstraintInstance;
#else
class hkConstraint;
#endif
class hkAction;
class hkVector4;
class havokAntiGravityAction;
class havokCollisionListener;
class RagdollData;
class RagdollSkeleton;
class Model;
struct HitData;

#include "Core/Math/LeMath.h"
#include "Core/LeString.h"

#include "Physics/PhysicsEngine.h"

class Limb
{
	friend class HavokRagdoll;
	friend class HavokWorld;

	hkRigidBody*				Body;
	Matrix						Offset;
	Matrix						InvOffset;
	int							Joint;
	bool						Fixed:1;
	unsigned char				CollisionGroup:7;
	havokAntiGravityAction*		AntiGravityAction;
	havokCollisionListener*		CollisionListener;
	Vector						LastPosForInterpolation;

public:
	Limb( hkRigidBody* rb, int joint, const Matrix &offset, bool isFixed );
	~Limb();
};

struct ConstraintData
{
#ifdef HAVOK3
	hkConstraintInstance* Constraint;
#else
	hkConstraint* Constraint;
#endif
	unsigned char Type;
};

struct ActionData
{
	hkAction* Action;
	unsigned char Type;
};

struct HavokRagdollRigidBodyInfo
{
	String Name;

	bool Fixed:1;
	bool MassFromHKE:1;
	bool MassFromDef:1;

	float Mass;
	float AngularDamping;
	float LinearDamping;
	float Friction;
	float Restitution;

	Vector Position;
	Matrix Transform;
	DynamicArray<Vector> Verts;
};

struct HavokRagdollGenericConstraintData
{
	Vector PivotA;
	Vector PivotB;
	Vector AngBasisA1, AngBasisA2, AngBasisA3, AngBasisB1, AngBasisB2, AngBasisB3;
	float TempX0, TempX1, TempY0, TempY1, TempZ0, TempZ1;
};

struct HavokRagdollConstraintData
{
	void* ConstraintInfo;
	unsigned char Type;
	bool Breakable;
	float Threshold;
	int BodyA;
	int BodyB;
	String Name;
	HavokRagdollGenericConstraintData* Gen;
};

struct HavokRagdollActionData
{
	float Strength;
	float Damping;
	Vector PivotA;
	Vector PivotB;
	Quaternion Rotation;
	float RestLength;
	bool OnCompression;
	bool OnExtension;
	//-------------
	unsigned char Type;
	int BodyA;
	int BodyB;
	String Name;
};

class HavokRagdollTemplate
{
public:
	HavokRagdollTemplate( String model, String ragdoll );
	~HavokRagdollTemplate( void );

	String ModelFile;
	String RagdollFile;

	DynamicArray<HavokRagdollRigidBodyInfo> RigidBodies;
	DynamicArray<HavokRagdollConstraintData> Constraints;
	DynamicArray<HavokRagdollActionData> Actions;
};

class HavokRagdoll
{
	friend class HavokWorld;
	friend class Ragdoll;

	enum
	{
		Active					= (1<<0),
		Frozen					= (1<<1),
		EternallyFrozen			= (1<<2),
		HardDeactivator			= (1<<3),
		IsMovedByExplosions		= (1<<4),
	};

	// --------- MEMBERS -----------
	// Params
	unsigned char					Flags;
	DynamicArray<Limb>				Limbs;
	char*							FastJointToLimbTable;
	DynamicArray<ConstraintData>	Constraints;
	DynamicArray<ActionData>		Actions;

	float							LinearDamping;
	float							AngularDamping;
	float							mInertiaTensorMultiplier;

	// Links to surrounding objects
	Model*				mModel;			// Model we're ragdoll of
	RagdollSkeleton*	Skeleton;		// Skeleton that moves us
	HavokWorld*			mWorld;			// Havok world we're in
	Ragdoll*			OuterRagdoll;	// Outer ragdoll to be deleted when we die

public:

	// --------- METHODS -----------

	// Construction/destruction (trivial)
	HavokRagdoll( Model* model, RagdollSkeleton* skeleton, HavokWorld* world );
	~HavokRagdoll();

	// Load/save
	bool LoadRagdoll( FileObject* f );
	bool SaveRagdoll( FileObject* f );
	bool LoadRagdollCollisionGroups( FileObject* f );
	bool SaveRagdollCollisionGroups( FileObject* f );

	// Activation state
	void Activate( const DynamicArray<Matrix>& frameMatrix, int collisionGroup );
	void Deactivate( void );
	void EternallyFreeze( void );

	bool IsActive( void );
	void ForceActivate( void );

	// Properties
	float GetMass( void );
	void SetMass( float mass );
	void ScaleInertiaTensor( float scale );
	void SetFriction( float friction );
	void SetRestitution( float restitution );
	void SetBreakablesThreshold( float threshold );
	void SetVelocities( const Vector& v, const Vector& av );
	void SetLinearDamping( float linearDamping );
	void SetAngularDamping( float angularDamping );
	void Move( const Vector& v );
	void EnableLineTraceCollision( bool enable );
	bool LineTraceCollisionEnabled( void );
	void EnableUnderwaterWorld( bool enable, float raiseLinear, float raiseAngular );
	void SetCollisionGroup( int newGroup );
	int  GetCollisionGroup( void );
#ifdef HAVOK3
	int ConstraintInRagdoll( hkConstraintInstance* constraint );
	bool GetConstraintJoints( hkConstraintInstance* constraint, int& joint1, int& joint2 );
#else
	int ConstraintInRagdoll( hkConstraint* constraint );
	bool GetConstraintJoints( hkConstraint* constraint, int& joint1, int& joint2 );
#endif
	void SetDeactivationHardness( float hardness );
	void SetHardDeactivator( void );

	// Pinning
	void SetPinned( bool enable );
	bool IsPinned( void );

	// Gravity
	void EnableGravity( bool enable );
	void SetGravity( Vector& gravity );
	Vector GetGravity( void );

	// Other
	void ApplyPointImpulse( Vector point, Vector impulse );
	bool LineTrace( Vector& start, Vector& end, HitData& hitData );
	void SelfExplosion( Vector center, float strength, float range, int ownerID, int weapon_type, float weapon_damage, int explosionCount, hkRigidBody* explosionBody = 0 );

	// Joint/body translation
	int    GetJointFromHavokBody( void* body );
	void*  GetHavokBodyFromJoint( int joint );

	// Joint properties
	void   Joint_BreakConstraints( int joint );
	void   Joint_SetVelocities( int joint, const Vector& v, const Vector& av );
	void   Joint_SetVelocitiesForLinked( int joint, const Vector& v, const Vector& av );
	void	Joint_SetPositionLowLevel( int joint, const Vector& v );
	void   Joint_SetPosition( int joint, const Vector& v );
	void   Joint_GetPosition( int joint, Vector& v );
	void   Joint_GetRotation( int joint, Quaternion& q );
	void   Joint_SetRotation( int joint, const Quaternion& q );
	void   Joint_SetMatrix( int joint, const Matrix& m );
	Vector Joint_GetLinearVelocity( int joint );
	Vector Joint_GetAngularVelocity( int joint );
	void   Joint_SetPinned( int joint, bool enable );
	bool   Joint_IsPinned( int joint );
	void   Joint_Enable( int joint, bool enable );
	void   Joint_SetCollisionCallbacks( int joint, float minimalDelay = 0.0f, float minimalStrength = 0.0f, float minimalOtherBodyMass = 0.0f );
	bool   Joint_AreLinked( int joint1, int joint2 );
	bool   Joint_InRagdoll( int joint ) { return ( FastJointToLimbTable[joint] != -1 ); };

	// Maintenence
	void Tick( void );

	void TickInterpolate( float tickPart );
	void AdvanceInterpolationFrame( void );

	void Freeze( void );
	void Animate( const DynamicArray<Matrix>& frameMatrix );
private:
	void GetLimbsPositionsAndRotations( DynamicArray<Vector>& positions, DynamicArray<Quaternion>& rotations );
	void SetLimbsPositionsAndRotations( DynamicArray<Vector>& positions, DynamicArray<Quaternion>& rotations );
	void FillRagdollData( RagdollData& data );
	void SetFromRagdollData( RagdollData& data, bool loading = false );
	void PrecalculateFastJointToLimbTable( int jointNumber );

	void Thaw( void );
	void FixConstraintPivots( struct ConstraintData& data );
	void FixActionPivots( struct ActionData& data );
	bool SaveConstraintPivots( FileObject* f, ConstraintData& data );
	bool LoadConstraintPivots( FileObject* f, ConstraintData& data );
	bool SaveActionPivots( FileObject* f, ActionData& data );
	bool LoadActionPivots( FileObject* f, ActionData& data );
	bool ConstraintInWorld( int index );
	bool ActionInWorld( int index );
};



#endif //_HAVOKRAGDOLL_

