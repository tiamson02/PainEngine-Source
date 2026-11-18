
#include "Engine.h"
#include "Physics/Havok/HavokRagdoll.h"
#include "Physics/Havok/HavokWorld.h"
#include "Physics/Ragdoll.h"
#include "Physics/PhysicsWorld.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Engine/World.h"
#include "Engine/Model.h"

Ragdoll::Ragdoll() : havokRagdoll(0)
{
}

Ragdoll::~Ragdoll()
{
	if( havokRagdoll )
		delete havokRagdoll;
}

#ifdef TRANSGAMING
bool Ragdoll::Init( Model* model, RagdollSkeleton* ragdollSkeleton, const char* fileName, float scale, int collisionGroup )
#else
inline bool Ragdoll::Init( Model* model, RagdollSkeleton* ragdollSkeleton, const char* fileName, float scale, int collisionGroup )
#endif
{
	if( havokRagdoll )
		delete havokRagdoll;
#ifdef	TRANSGAMING
    //TRACE("Ragdoll::Init(): building a ragdoll through physics engine %p\n", GEngine->pcfWorld->Physics);
	if (GEngine->pcfWorld->Physics)
		havokRagdoll = GEngine->pcfWorld->Physics->mHavokWorld->CreateRagdoll( model, ragdollSkeleton, fileName, scale, collisionGroup );
#else
	if (GEngine->World->Physics)
		havokRagdoll = GEngine->World->Physics->mHavokWorld->CreateRagdoll( model, ragdollSkeleton, fileName, scale, collisionGroup );
#endif
	if( havokRagdoll )
	{
		havokRagdoll->OuterRagdoll = this;
		havokRagdoll->PrecalculateFastJointToLimbTable( model->Skeleton.Joints.Size() );
	}

	return ( havokRagdoll != 0 );
}

#ifndef TRANSGAMING // =======================

inline void Ragdoll::Activate( const DynamicArray<Matrix>& frameMatrix, int collisionGroup )
{
	Assert( havokRagdoll );
	havokRagdoll->Activate( frameMatrix, collisionGroup );
}

inline void Ragdoll::SetCollisionGroup( int newGroup )
{
	Assert( havokRagdoll );
	havokRagdoll->SetCollisionGroup( newGroup );
}

inline void Ragdoll::Animate( const DynamicArray<Matrix>& frameMatrix )
{
	Assert( havokRagdoll );
	havokRagdoll->Animate( frameMatrix );
}

inline void Ragdoll::Deactivate( void )
{
	Assert( havokRagdoll );
	havokRagdoll->Deactivate();
}

inline bool Ragdoll::IsActive( void )
{
	Assert( havokRagdoll );
	return havokRagdoll->IsActive();
}

inline void Ragdoll::SetMass( float mass )
{
	Assert( havokRagdoll );
	havokRagdoll->SetMass( mass );
}

inline void Ragdoll::ScaleInertiaTensor( float scale )
{
	Assert( havokRagdoll );
	havokRagdoll->ScaleInertiaTensor( scale );
}

inline void Ragdoll::Joint_BreakConstraints( int joint )
{
	Assert( havokRagdoll );
	havokRagdoll->Joint_BreakConstraints( joint );
}

inline void Ragdoll::SetVelocities( const Vector& v, const Vector& av )
{
	Assert( havokRagdoll );
	havokRagdoll->SetVelocities( v, av );
}

inline void Ragdoll::Move( const Vector& v )
{
	Assert( havokRagdoll );
	havokRagdoll->Move( v );
}

inline void Ragdoll::SetPinned( bool enable )
{
	Assert( havokRagdoll );
	havokRagdoll->SetPinned( enable );
}

inline bool Ragdoll::IsPinned( void )
{
	Assert( havokRagdoll );
	return havokRagdoll->IsPinned();
}

inline void Ragdoll::Joint_SetVelocities( int joint, const Vector& v, const Vector& av )
{
	Assert( havokRagdoll );
	havokRagdoll->Joint_SetVelocities( joint, v, av );
}

inline void Ragdoll::Joint_SetVelocitiesForLinked( int joint, const Vector& v, const Vector& av )
{
	Assert( havokRagdoll );
	havokRagdoll->Joint_SetVelocitiesForLinked( joint, v, av );
}

inline void Ragdoll::Joint_SetPosition( int joint, const Vector& v )
{
	Assert( havokRagdoll );
	havokRagdoll->Joint_SetPosition( joint, v );
}

inline void Ragdoll::Joint_SetPositionLL( int joint, const Vector& v )
{
	Assert( havokRagdoll );
	havokRagdoll->Joint_SetPositionLowLevel( joint, v );
}


inline void Ragdoll::Joint_GetPosition( int joint, Vector& v )
{
	Assert( havokRagdoll );
	havokRagdoll->Joint_GetPosition( joint, v );
}

inline void Ragdoll::Joint_SetMatrix( int joint, const Matrix& m ) {
	Assert( havokRagdoll );
	havokRagdoll->Joint_SetMatrix( joint, m );
}

inline void Ragdoll::Joint_SetRotation( int joint, const Quaternion& q )
{
	Assert( havokRagdoll );
	havokRagdoll->Joint_SetRotation( joint, q );
}

inline void Ragdoll::Joint_GetRotation( int joint, Quaternion& q )
{
	Assert( havokRagdoll );
	havokRagdoll->Joint_GetRotation( joint, q );
}

inline void Ragdoll::ApplyPointImpulse( Vector point, Vector impulse )
{
	Assert( havokRagdoll );
	havokRagdoll->ApplyPointImpulse( point, impulse );
}

inline void Ragdoll::Joint_SetPinned( int joint, bool enable )
{
	Assert( havokRagdoll );
	havokRagdoll->Joint_SetPinned( joint, enable );
}

inline bool Ragdoll::Joint_IsPinned( int joint )
{
	Assert( havokRagdoll );
	return havokRagdoll->Joint_IsPinned( joint );
}

inline Vector Ragdoll::Joint_GetLinearVelocity( int joint )
{
	Assert( havokRagdoll );
	return havokRagdoll->Joint_GetLinearVelocity( joint );
}

inline Vector Ragdoll::Joint_GetAngularVelocity( int joint )
{
	Assert( havokRagdoll );
	return havokRagdoll->Joint_GetAngularVelocity( joint );
}

inline void Ragdoll::EnableLineTraceCollision( bool enable )
{
	Assert( havokRagdoll );
	havokRagdoll->EnableLineTraceCollision( enable );
}

inline void Ragdoll::SetLinearDamping( float linearDamping )
{
	Assert( havokRagdoll );
	havokRagdoll->SetLinearDamping( linearDamping );
}

inline void Ragdoll::SetAngularDamping( float angularDamping )
{
	Assert( havokRagdoll );
	havokRagdoll->SetAngularDamping( angularDamping );
}

inline void Ragdoll::Joint_SetCollisionCallbacks( int joint, float minimalDelay, float minimalStrength, float minimalOtherBodyMass )
{
	Assert( havokRagdoll );
	havokRagdoll->Joint_SetCollisionCallbacks( joint, minimalDelay, minimalStrength, minimalOtherBodyMass );
}

inline int  Ragdoll::GetJointFromHavokBody( void* body )
{
	Assert( havokRagdoll );
	return havokRagdoll->GetJointFromHavokBody( body );
}

inline bool Ragdoll::Joint_AreLinked( int joint1, int joint2 )
{
	Assert( havokRagdoll );
	return havokRagdoll->Joint_AreLinked( joint1, joint2 );
}

inline bool Ragdoll::Joint_InRagdoll( int joint )
{
	Assert( havokRagdoll );
	return havokRagdoll->Joint_InRagdoll( joint );
}

inline void Ragdoll::EnableGravity( bool enable )
{
	Assert( havokRagdoll );
	havokRagdoll->EnableGravity( enable );
}

inline void Ragdoll::SetGravity( Vector& gravity )
{
	Assert( havokRagdoll );
	havokRagdoll->SetGravity( gravity );
}

inline Vector Ragdoll::GetGravity( void )
{
	Assert( havokRagdoll );
	return havokRagdoll->GetGravity();
}

inline void Ragdoll::GetLimbsPositionsAndRotations( DynamicArray<Vector>& positions, DynamicArray<Quaternion>& rotations )
{
	Assert( havokRagdoll );
	havokRagdoll->GetLimbsPositionsAndRotations( positions, rotations );
}

inline void Ragdoll::SetLimbsPositionsAndRotations( DynamicArray<Vector>& positions, DynamicArray<Quaternion>& rotations )
{
	Assert( havokRagdoll );
	havokRagdoll->SetLimbsPositionsAndRotations( positions, rotations );
}

inline void Ragdoll::FillRagdollData( RagdollData& data )
{
	Assert( havokRagdoll );
	havokRagdoll->FillRagdollData( data );
}

inline void Ragdoll::SetFromRagdollData( RagdollData& data )
{
	Assert( havokRagdoll );
	havokRagdoll->SetFromRagdollData( data );
}

inline int Ragdoll::GetCollisionGroup( void )
{
	Assert( havokRagdoll );
	return havokRagdoll->GetCollisionGroup();
}

inline bool Ragdoll::LineTrace( Vector& start, Vector& end, HitData& hitData )
{
	Assert( havokRagdoll );
	return havokRagdoll->LineTrace( start, end, hitData );
}

inline void Ragdoll::SetFriction( float friction )
{
	Assert( havokRagdoll );
	havokRagdoll->SetFriction( friction );
}

inline void Ragdoll::SetRestitution( float restitution )
{
	Assert( havokRagdoll );
	havokRagdoll->SetRestitution( restitution );
}

inline void Ragdoll::SetBreakablesThreshold( float threshold )
{
	Assert( havokRagdoll );
	havokRagdoll->SetBreakablesThreshold( threshold );
}

inline void Ragdoll::SetDeactivationHardness( float hardness )
{
	Assert( havokRagdoll );
	havokRagdoll->SetDeactivationHardness( hardness );
}

inline void Ragdoll::IsMovedByExplosions( bool state )
{
	Assert( havokRagdoll );
	if( state )
		havokRagdoll->Flags |= HavokRagdoll::IsMovedByExplosions;
	else
		havokRagdoll->Flags &= ~HavokRagdoll::IsMovedByExplosions;
}

inline bool Ragdoll::CanLineTraceCollision( void )
{
	Assert( havokRagdoll );
	return havokRagdoll->LineTraceCollisionEnabled();
}

inline void Ragdoll::SetHardDeactivator( void )
{
	Assert( havokRagdoll );
	havokRagdoll->SetHardDeactivator();
}

inline bool Ragdoll::LoadRagdoll( FileObject* f )
{
	Assert( havokRagdoll );
	return havokRagdoll->LoadRagdoll( f );
}

inline bool Ragdoll::SaveRagdoll( FileObject* f )
{
	Assert( havokRagdoll );
	return havokRagdoll->SaveRagdoll( f );
}

inline bool Ragdoll::LoadRagdollCollisionGroups( FileObject* f )
{
	Assert( havokRagdoll );
	return havokRagdoll->LoadRagdollCollisionGroups( f );
}

inline bool Ragdoll::SaveRagdollCollisionGroups( FileObject* f )
{
	Assert( havokRagdoll );
	return havokRagdoll->SaveRagdollCollisionGroups( f );
}

inline void Ragdoll::EternalFreeze( void )
{
	Assert( havokRagdoll );
	havokRagdoll->EternallyFreeze();
}

inline void Ragdoll::SelfExplosion( Vector center, float strength, float range )
{
	Assert( havokRagdoll );
	havokRagdoll->SelfExplosion( center, strength, range, 0, 0, 0, 0, 0 );
}

inline void Ragdoll::Joint_Enable( int joint, bool enable )
{
	Assert( havokRagdoll );
	havokRagdoll->Joint_Enable( joint, enable );
}

inline void Ragdoll::ForceActivate( void )
{
	Assert( havokRagdoll );
	havokRagdoll->ForceActivate();
}

#endif
