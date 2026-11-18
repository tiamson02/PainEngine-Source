
// #pragma once
#ifndef _RAGDOLL_
#define _RAGDOLL_


class RagdollSkeleton;
class HavokRagdoll;
class RagdollData;
struct HitData;
class Model;

#include "Core/DynArray.h"
#include "Core/Math/LeMath.h"

#ifdef TRANSGAMING
#include "Physics/Havok/HavokRagdoll.h"
#endif


class ENGINE_API Ragdoll
{
//	friend class RagdollSynchronizer;
	friend class havokBreakableListener;
public:
	Ragdoll( void );
	~Ragdoll( void );
/*
#ifdef TRANSGAMING
// Some errors were showing when linking due to Model.o not being able to find these functions...removing the inlining made them show.
	bool Init( Model* model, RagdollSkeleton* ragdollSkeleton, const char* fileName, float scale, int collisionGroup = -1 );

	// Maintenance
	void Animate( const DynamicArray<Matrix>& frameMatrix );

	// Load/save
	bool LoadRagdoll( FileObject* f );
	bool SaveRagdoll( FileObject* f );
	bool LoadRagdollCollisionGroups( FileObject* f );
	bool SaveRagdollCollisionGroups( FileObject* f );

	// Activation state
	void Activate( const DynamicArray<Matrix>& frameMatrix, int collisionGroup );
	void Deactivate( void );
#else
*/
#ifndef TRANSGAMING
	inline bool Init( Model* model, RagdollSkeleton* ragdollSkeleton, const char* fileName, float scale, int collisionGroup = -1 );

	// Maintenance
	inline void Animate( const DynamicArray<Matrix>& frameMatrix );

	// Load/save
	inline bool LoadRagdoll( FileObject* f );
	inline bool SaveRagdoll( FileObject* f );
	inline bool LoadRagdollCollisionGroups( FileObject* f );
	inline bool SaveRagdollCollisionGroups( FileObject* f );

	// Activation state
	inline void Activate( const DynamicArray<Matrix>& frameMatrix, int collisionGroup );
	inline void Deactivate( void );
//#endif
	inline bool IsActive( void );

	// Properties
	inline void SetMass( float mass );
	inline void ScaleInertiaTensor( float scale );
	inline void SetFriction( float friction );
	inline void SetRestitution( float restitution );
	inline void SetBreakablesThreshold( float threshold );
	inline void SetVelocities( const Vector& v, const Vector& av );
	inline void SetLinearDamping( float linearDamping );
	inline void SetAngularDamping( float angularDamping );
	inline void Move( const Vector& v );
	inline bool CanLineTraceCollision( void );
	inline void EnableLineTraceCollision( bool enable );
	inline void SetCollisionGroup( int newGroup );
	inline int  GetCollisionGroup( void );
	inline void SetDeactivationHardness( float hardness );
    inline void IsMovedByExplosions( bool state);
	inline void SetHardDeactivator( void );

	// Gravity
	void EnableGravity( bool enable );
	void SetGravity( Vector& gravity );
	Vector GetGravity( void );

	// Pinning
	inline void SetPinned( bool enable );
	inline bool IsPinned( void );
	inline void EternalFreeze( void );

	inline void ForceActivate( void );

	// Other
	inline void ApplyPointImpulse( Vector point, Vector impulse );
	inline bool LineTrace( Vector& start, Vector& end, HitData& hitData );
#ifdef TRANSGAMING
	void SelfExplosion( Vector center, float strength, float range );
#else
	inline void SelfExplosion( Vector center, float strength, float range );
#endif
	// Joint properties
	inline int  GetJointFromHavokBody( void* body );

	inline void   Joint_BreakConstraints( int joint );
	inline void   Joint_SetVelocities( int joint, const Vector& v, const Vector& av );
	inline void   Joint_SetVelocitiesForLinked( int joint, const Vector& v, const Vector& av );
	inline void   Joint_SetPositionLL( int joint, const Vector& v );
	inline void   Joint_SetPosition( int joint, const Vector& v );
	inline void   Joint_GetPosition( int joint, Vector& v );
	inline void   Joint_SetRotation( int joint, const Quaternion& q );
	inline void   Joint_GetRotation( int joint, Quaternion& q );
	inline void   Joint_SetMatrix( int joint, const Matrix& m );
	inline Vector Joint_GetLinearVelocity( int joint );
	inline Vector Joint_GetAngularVelocity( int joint );
	inline void   Joint_SetPinned( int joint, bool enable );
	inline bool   Joint_IsPinned( int joint );
	inline void   Joint_Enable( int joint, bool enable );
	inline void   Joint_SetCollisionCallbacks( int joint, float minimalDelay = 0.0f, float minimalStrength = 0.0f, float minimalOtherBodyMass = 0.0f );
	inline bool   Joint_AreLinked( int joint1, int joint2 );
	inline bool   Joint_InRagdoll( int joint );
#else
private:
	HavokRagdoll* havokRagdoll;
public:
	bool Init( Model* model, RagdollSkeleton* ragdollSkeleton, const char* fileName, float scale, int collisionGroup = -1 );

	// Maintenance
	inline void Animate( const DynamicArray<Matrix>& frameMatrix )
	{
		Assert( havokRagdoll );
		havokRagdoll->Animate( frameMatrix );
	}

	// Load/save
	inline bool LoadRagdoll( FileObject* f )
	{
		Assert( havokRagdoll );
		return havokRagdoll->LoadRagdoll( f );
	}
	inline bool SaveRagdoll( FileObject* f )
	{
		Assert( havokRagdoll );
		return havokRagdoll->SaveRagdoll( f );
	}
	inline bool LoadRagdollCollisionGroups( FileObject* f )
	{
		Assert( havokRagdoll );
		return havokRagdoll->LoadRagdollCollisionGroups( f );
	}
	inline bool SaveRagdollCollisionGroups( FileObject* f )
	{
		Assert( havokRagdoll );
		return havokRagdoll->SaveRagdollCollisionGroups( f );
	}

	// Activation state
	inline void Activate( const DynamicArray<Matrix>& frameMatrix, int collisionGroup )
	{
		Assert( havokRagdoll );
		havokRagdoll->Activate( frameMatrix, collisionGroup );
	}
	inline void Deactivate( void )
	{
		Assert( havokRagdoll );
		havokRagdoll->Deactivate();
	}
//#endif
	inline bool IsActive( void )
	{
		Assert( havokRagdoll );
		return havokRagdoll->IsActive();
	}

	// Properties
	inline void SetMass( float mass )
	{
		Assert( havokRagdoll );
		havokRagdoll->SetMass( mass );
	}
	inline void ScaleInertiaTensor( float scale )
	{
		Assert( havokRagdoll );
		havokRagdoll->ScaleInertiaTensor( scale );
	}
	inline void SetFriction( float friction )
	{
		Assert( havokRagdoll );
		havokRagdoll->SetFriction( friction );
	}
	inline void SetRestitution( float restitution )
	{
		Assert( havokRagdoll );
		havokRagdoll->SetRestitution( restitution );
	}
	inline void SetBreakablesThreshold( float threshold )
	{
		Assert( havokRagdoll );
		havokRagdoll->SetBreakablesThreshold( threshold );
	}
	inline void SetVelocities( const Vector& v, const Vector& av )
	{
		Assert( havokRagdoll );
		havokRagdoll->SetVelocities( v, av );
	}
	inline void SetLinearDamping( float linearDamping )
	{
		Assert( havokRagdoll );
		havokRagdoll->SetLinearDamping( linearDamping );
	}
	inline void SetAngularDamping( float angularDamping )
	{
		Assert( havokRagdoll );
		havokRagdoll->SetAngularDamping( angularDamping );
	}
	inline void Move( const Vector& v )
	{
		Assert( havokRagdoll );
		havokRagdoll->Move( v );
	}
	inline bool CanLineTraceCollision( void )
	{
		Assert( havokRagdoll );
		return havokRagdoll->LineTraceCollisionEnabled();
	}
	inline void EnableLineTraceCollision( bool enable )
	{
		Assert( havokRagdoll );
		havokRagdoll->EnableLineTraceCollision( enable );
	}
	inline void SetCollisionGroup( int newGroup )
	{
		Assert( havokRagdoll );
		havokRagdoll->SetCollisionGroup( newGroup );
	}
	inline int  GetCollisionGroup( void )
	{
		Assert( havokRagdoll );
		return havokRagdoll->GetCollisionGroup();
	}
	inline void SetDeactivationHardness( float hardness )
	{
		Assert( havokRagdoll );
		havokRagdoll->SetDeactivationHardness( hardness );
	}
    inline void IsMovedByExplosions( bool state)
	{
		Assert( havokRagdoll );
		if( state )
			havokRagdoll->Flags |= HavokRagdoll::IsMovedByExplosions;
		else
			havokRagdoll->Flags &= ~HavokRagdoll::IsMovedByExplosions;
	}
	inline void SetHardDeactivator( void )
	{
		Assert( havokRagdoll );
		havokRagdoll->SetHardDeactivator();
	}

	// Gravity
	void EnableGravity( bool enable )
	{
		Assert( havokRagdoll );
		havokRagdoll->EnableGravity( enable );
	}
	void SetGravity( Vector& gravity )
	{
		Assert( havokRagdoll );
		havokRagdoll->SetGravity( gravity );
	}
	Vector GetGravity( void )
	{
		Assert( havokRagdoll );
		return havokRagdoll->GetGravity();
	}

	// Pinning
	inline void SetPinned( bool enable )
	{
		Assert( havokRagdoll );
		havokRagdoll->SetPinned( enable );
	}
	inline bool IsPinned( void )
	{
		Assert( havokRagdoll );
		return havokRagdoll->IsPinned();
	}
	inline void EternalFreeze( void )
	{
		Assert( havokRagdoll );
		havokRagdoll->EternallyFreeze();
	}

	inline void ForceActivate( void )
	{
		Assert( havokRagdoll );
		havokRagdoll->ForceActivate();
	}

	// Other
	inline void ApplyPointImpulse( Vector point, Vector impulse )
	{
		Assert( havokRagdoll );
		havokRagdoll->ApplyPointImpulse( point, impulse );
	}
	inline bool LineTrace( Vector& start, Vector& end, HitData& hitData )
	{
		Assert( havokRagdoll );
		return havokRagdoll->LineTrace( start, end, hitData );
	}
	inline void SelfExplosion( Vector center, float strength, float range )
	{
		Assert( havokRagdoll );
		havokRagdoll->SelfExplosion( center, strength, range, 0, 0, 0, 0, 0 );
	}
	// Joint properties
	inline int  GetJointFromHavokBody( void* body )
	{
		Assert( havokRagdoll );
		return havokRagdoll->GetJointFromHavokBody( body );
	}
	inline void   Joint_BreakConstraints( int joint )
	{
		Assert( havokRagdoll );
		havokRagdoll->Joint_BreakConstraints( joint );
	}
	inline void   Joint_SetVelocities( int joint, const Vector& v, const Vector& av )
	{
		Assert( havokRagdoll );
		havokRagdoll->Joint_SetVelocities( joint, v, av );
	}
	inline void   Joint_SetVelocitiesForLinked( int joint, const Vector& v, const Vector& av )
	{
		Assert( havokRagdoll );
		havokRagdoll->Joint_SetVelocitiesForLinked( joint, v, av );
	}
	inline void   Joint_SetPositionLL( int joint, const Vector& v )
	{
		Assert( havokRagdoll );
		havokRagdoll->Joint_SetPositionLowLevel( joint, v );
	}
	inline void   Joint_SetPosition( int joint, const Vector& v )
	{
		Assert( havokRagdoll );
		havokRagdoll->Joint_SetPosition( joint, v );
	}
	inline void   Joint_GetPosition( int joint, Vector& v )
	{
		Assert( havokRagdoll );
		havokRagdoll->Joint_GetPosition( joint, v );
	}
	inline void   Joint_SetRotation( int joint, const Quaternion& q )
	{
		Assert( havokRagdoll );
		havokRagdoll->Joint_SetRotation( joint, q );
	}
	inline void   Joint_GetRotation( int joint, Quaternion& q )
	{
		Assert( havokRagdoll );
		havokRagdoll->Joint_GetRotation( joint, q );
	}
	inline void   Joint_SetMatrix( int joint, const Matrix& m )
	{
		Assert( havokRagdoll );
		havokRagdoll->Joint_SetMatrix( joint, m );
	}
	inline Vector Joint_GetLinearVelocity( int joint )
	{
		Assert( havokRagdoll );
		return havokRagdoll->Joint_GetLinearVelocity( joint );
	}
	inline Vector Joint_GetAngularVelocity( int joint )
	{
		Assert( havokRagdoll );
		return havokRagdoll->Joint_GetAngularVelocity( joint );
	}
	inline void   Joint_SetPinned( int joint, bool enable )
	{
		Assert( havokRagdoll );
		havokRagdoll->Joint_SetPinned( joint, enable );
	}
	inline bool   Joint_IsPinned( int joint )
	{
		Assert( havokRagdoll );
		return havokRagdoll->Joint_IsPinned( joint );
	}
	inline void   Joint_Enable( int joint, bool enable )
	{
		Assert( havokRagdoll );
		havokRagdoll->Joint_Enable( joint, enable );
	}
	inline void   Joint_SetCollisionCallbacks( int joint, float minimalDelay = 0.0f, float minimalStrength = 0.0f, float minimalOtherBodyMass = 0.0f )
	{
		Assert( havokRagdoll );
		havokRagdoll->Joint_SetCollisionCallbacks( joint, minimalDelay, minimalStrength, minimalOtherBodyMass );
	}
	inline bool   Joint_AreLinked( int joint1, int joint2 )
	{
		Assert( havokRagdoll );
		return havokRagdoll->Joint_AreLinked( joint1, joint2 );
	}
	inline bool   Joint_InRagdoll( int joint )
	{
		Assert( havokRagdoll );
		return havokRagdoll->Joint_InRagdoll( joint );
	}
#endif

#ifndef TRANSGAMING
private:
	inline void GetLimbsPositionsAndRotations( DynamicArray<Vector>& positions, DynamicArray<Quaternion>& rotations );
	inline void SetLimbsPositionsAndRotations( DynamicArray<Vector>& positions, DynamicArray<Quaternion>& rotations );
	inline void FillRagdollData( RagdollData& data );
	inline void SetFromRagdollData( RagdollData& data );

	HavokRagdoll* havokRagdoll;
#else
private:
	inline void GetLimbsPositionsAndRotations( DynamicArray<Vector>& positions, DynamicArray<Quaternion>& rotations )
	{
		Assert( havokRagdoll );
		havokRagdoll->GetLimbsPositionsAndRotations( positions, rotations );
	}
	inline void SetLimbsPositionsAndRotations( DynamicArray<Vector>& positions, DynamicArray<Quaternion>& rotations )
	{
		Assert( havokRagdoll );
		havokRagdoll->SetLimbsPositionsAndRotations( positions, rotations );
	}
	inline void FillRagdollData( RagdollData& data )
	{
		Assert( havokRagdoll );
		havokRagdoll->FillRagdollData( data );
	}
	inline void SetFromRagdollData( RagdollData& data )
	{
		Assert( havokRagdoll );
		havokRagdoll->SetFromRagdollData( data );
	}
#endif
	//HavokRagdoll* havokRagdoll;
};

// The class below is used as a container for the ragdoll data, when it is created on a remote client

class ENGINE_API RagdollData
{
public:
	DynamicArray<Vector> LimbPositions;
	DynamicArray<Quaternion> LimbOrientations;
	DynamicArray<Vector> LimbLinearVelocities;
	DynamicArray<Vector> LimbAngularVelocities;
	DynamicArray<bool> LimbPinned;

	float LinearDamping;
	float AngularDamping;
	float Mass;
	DynamicArray<bool> ConstraintsInWorld;
	DynamicArray<bool> ActionsInWorld;
	unsigned char CollisionGroup;
};



#endif //_RAGDOLL_

