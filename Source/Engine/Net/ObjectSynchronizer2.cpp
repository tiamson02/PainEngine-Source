
#include "Engine.h"

#include "Net/ObjectSynchronizer2.h"
#include "Net/UdpMessage.h"
#include "Net/NetworkDevice2.h"
#include "Net/NetworkPeer2.h"
#include "Engine/Entity.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Engine/World.h"
#include "Physics/PhysicsEngine.h"
#include "Physics/PhysicsWorld.h"
#include "Physics/PhysicsObject.h"
#include "Engine/Mesh.h"
#include "Engine/ModelAnimation.h"
#include "Engine/Model.h"

#ifndef TRANSGAMING_NO_HAVOK
// Blair - We don't currently support Havok on the PSP
#include "Physics/Havok/HavokCharacterController.h"
#include "Physics/Havok/HavokMissileController.h"
#endif

#include "Engine/InputSystem.h"

#if defined( TRANSGAMING_PS2 )
#include "tg_pc2ps2.h"
#elif defined( TRANSGAMING_PSP )
#include "tg_pc2psp.h"
#endif

//#define TEST_CERTAIN_OBJECT

#ifdef TEST_CERTAIN_OBJECT
#define TCO_SETOBJ(str) bool write = ( mObject->FileName == "str" );
#define TCO_WRITE(str) if( write ) GLog.Print( "str\n" );
#else
#define TCO_SETOBJ(str)
#define TCO_WRITE(str)
#endif

// =========================================================================================================

EntitySynchronizer2::EntitySynchronizer2( ulong32 uniqueNumber, NetworkDevice2* device )
:	mDevice( device )
,	UniqueNumber( uniqueNumber )
,	mObject( 0 )
{
	device->mObjectSynchronizers.Add( this );
	device->mObjectSynchronizersHash.Insert( UniqueNumber, this );
#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
    if (device->mObjectSynchronizersHash.Size() > device->mMaxObjectSync){
        device->mMaxObjectSync = device->mObjectSynchronizersHash.Size();
        TRACE("@@@ EntitySynchronizer2::EntitySynchronizer2(): peak mObjectSynchronizersHash hash usage = %d\n", device->mMaxObjectSync);
    }
#endif
}

EntitySynchronizer2::~EntitySynchronizer2( void )
{
	mDevice->mObjectSynchronizersHash.Remove( UniqueNumber );
	for( int i = 0; i < mDevice->mObjectSynchronizers.Size(); ++i )
		if( mDevice->mObjectSynchronizers[i] == this )
		{
			mDevice->mObjectSynchronizers.FastRemove( i );
			break;
		}

}

inline void EntitySynchronizer2::WriteSynchronizer( UdpMessage* message, ulong32 uniqueNumber, unsigned char type )
{
	Assert( ( uniqueNumber != (DWORD)-1 ) && ( uniqueNumber < 0x3fffffff ) );
	message->WriteULongOptimized( (((ulong32)type)&0x3)|((uniqueNumber + 1)<<2) );
}

void EntitySynchronizer2::RemoveConnectionWithEntity( Entity* entity )
{
	mDevice->mObjectSynchronizersHashByEntity.Remove( entity, entity->GIndex );
}

void EntitySynchronizer2::MakeConnectionWithEntity( Entity* entity )
{
	mDevice->mObjectSynchronizersHashByEntity.Insert( entity, entity->GIndex,  UniqueNumber );
#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
    if (mDevice->mObjectSynchronizersHashByEntity.Size() > mDevice->mMaxObjectSyncByEntity){
        mDevice->mMaxObjectSyncByEntity = mDevice->mObjectSynchronizersHashByEntity.Size();
        TRACE("@@@ EntitySynchronizer2::MakeConnectionWithEntity(): peak mObjectSynchronizersHashByEntity hash usage = %d\n", mDevice->mMaxObjectSyncByEntity);
    }
#endif
}

void EntitySynchronizer2::SendCreateNormalPO( UdpMessage* mesg, NetworkPeer2* peer, NetworkObjectState* newState, bool sendPosOrient )
{
//	GLog.Print( "PO::CREATEOBJ %d sent\n", UniqueNumber );
	Assert( mObject && ( UniqueNumber != -1 ) );

	PONetCreationEntry2 entry;

#ifndef TRANSGAMING_NO_HAVOK
	entry.ObjectState = mObject->PO->State;
	entry.Mass = mObject->PO->GetMass();
	entry.Friction = mObject->PO->GetFriction();
	entry.Restitution = mObject->PO->GetRestitution();
	entry.Damping = mObject->PO->GetLinearDamping();
	entry.BodyScale = mObject->PO->BodyScale;
	entry.BodyType = mObject->PO->BodyType;
	entry.FreedomOfRotation = mObject->PO->GetFreedomOfRotation();
	entry.Pinned = mObject->PO->IsPinned();
	entry.CollisionGroup = mObject->PO->GetCollisionGroup();
	Assert( entry.CollisionGroup <= 31 );
	entry.HasGravity = ( mObject->PO->GetGravity().MagnitudeSquared() > 0.f );
#endif

	peer->POCreationCache.SendPOCreationData( mesg, entry );

	newState->CollisionGroup = entry.CollisionGroup;
	newState->HasGravity = entry.HasGravity;

	newState->Position = mObject->GetPosition();
	if( sendPosOrient )
		mesg->WritePositionOptimized( newState->Position );

	newState->Orientation = mObject->GetRotation();
	if( sendPosOrient )
		mesg->WriteQuaternionOptimized( newState->Orientation );

#ifndef TRANSGAMING_NO_HAVOK
	newState->Velocity = mObject->PO->GetVel();
	mesg->WriteLinearVelocityOptimized( newState->Velocity );

	newState->AngularVelocity = mObject->PO->GetAngularVel();
	mesg->WriteAngularVelocityOptimized( newState->AngularVelocity );
#endif

	mesg->WriteCheckToken( PHO_CREATE );
}

void EntitySynchronizer2::InterpretCreateNormalPO( UdpMessage* mesg, NetworkPeer2* peer, NetworkObjectState* newState, bool dump, bool recvPosOrient )
{
	PONetCreationEntry2 entry;
	peer->POCreationCache.ReceivePOCreationData( mesg, entry );

	newState->Position = recvPosOrient?mesg->ReadPositionOptimized():mObject->GetPosition();
	newState->Orientation = recvPosOrient?mesg->ReadQuaternionOptimized():mObject->GetRotation();
	newState->Velocity = mesg->ReadLinearVelocityOptimized();
	newState->AngularVelocity = mesg->ReadAngularVelocityOptimized();

	mesg->ReadCheckToken( PHO_CREATE );

	if( !mObject || dump )
		return;

	unsigned short objectState = entry.ObjectState;
	float mass = entry.Mass;
	float friction = entry.Friction;
	float restitution = entry.Restitution;
	float bodyScale = entry.BodyScale;
	unsigned char bodyType = entry.BodyType;
	char freedomOfRotation = entry.FreedomOfRotation;
	bool pinned = entry.Pinned;
	newState->CollisionGroup = entry.CollisionGroup;
	Assert( newState->CollisionGroup <= 31 );
	newState->HasGravity = entry.HasGravity;

	mObject->SetPositionAndRotation( newState->Position, newState->Orientation );
#ifdef	TRANSGAMING
#ifndef TRANSGAMING_NO_HAVOK
	if( !mObject->PO )
		mObject->PO = GEngine->pcfWorld->Physics->CreatePhysicsObject( newState->Position, newState->Orientation,
			bodyType, mObject, bodyScale, newState->CollisionGroup );
#endif // TRANSGAMING_NO_HAVOK
#else
	if( !mObject->PO )
		mObject->PO = GEngine->World->Physics->CreatePhysicsObject( newState->Position, newState->Orientation,
			bodyType, mObject, bodyScale, newState->CollisionGroup );
#endif	

	mObject->PO->State = objectState;
#ifndef TRANSGAMING_NO_HAVOK
	mObject->PO->SetFreedomOfRotation( freedomOfRotation );
	mObject->PO->SetVel( newState->Velocity );
	mObject->PO->SetAngularVel( newState->AngularVelocity );

	if( mass != mObject->PO->GetMass() )
		mObject->PO->SetMass( mass );

	if( friction != mObject->PO->GetFriction() )
		mObject->PO->SetFriction( friction );

	if( restitution != mObject->PO->GetRestitution() )
		mObject->PO->SetRestitution( restitution );

	mObject->PO->EnableGravity( newState->HasGravity );
	mObject->PO->SetPinned( pinned );
#endif
	char* synStr = mObject->GetSynchroString();
	if( synStr )
		GScript.QueueMessage("PO_CREATE","%e,%p,%s",mObject,mObject->PO,synStr);
}

void EntitySynchronizer2::SendCreateMissilePO( UdpMessage* mesg, NetworkPeer2* peer, NetworkObjectState* newState, bool sendPosOrient )
{
//	GLog.Print( "PO::CREATEMISSILE %d sent\n", UniqueNumber );
	Assert( mObject && ( UniqueNumber != -1 ) && mObject->PO && mObject->PO->mMissileController );

	PONetCreationEntry2 entry;

#ifndef TRANSGAMING_NO_HAVOK
	entry.ObjectState = mObject->PO->State;
	entry.Mass = mObject->PO->GetMass();
	entry.Friction = mObject->PO->mMissileController->getMissileFriction();
	entry.Restitution = mObject->PO->GetRestitution();
	entry.Damping = mObject->PO->mMissileController->getDamping();
	entry.BodyScale = mObject->PO->BodyScale;
	entry.BodyType = mObject->PO->BodyType;
	entry.FreedomOfRotation = mObject->PO->GetFreedomOfRotation();
	entry.Pinned = mObject->PO->IsPinned();
	entry.CollisionGroup = mObject->PO->GetCollisionGroup();
#endif
	Assert( entry.CollisionGroup <= 31 );
#ifndef TRANSGAMING_NO_HAVOK
	entry.HasGravity = mObject->PO->mMissileController->getGravity();
#else
// Blair - We don't support Havok or the MissileController on the PSP.
// Since we have no physics, there should be no gravity.
	entry.HasGravity = 0;
#endif
	peer->POCreationCache.SendPOCreationData( mesg, entry );

	newState->CollisionGroup = entry.CollisionGroup;
	newState->HasGravity = entry.HasGravity;

	newState->Orientation = mObject->GetRotation();
	if( sendPosOrient )
		mesg->WriteQuaternionOptimized( newState->Orientation );

#ifndef TRANSGAMING_NO_HAVOK
// Blair - No Missile or Havok for the PSP yet...
#ifndef NO_MISSILE_UPDATES
	m_setTag("Net");
    newState->Missile = new MissileState;
	newState->Missile->GetFrom( mObject->PO->mMissileController );
	if( sendPosOrient )
		newState->Missile->Write( mesg );
	else
		newState->Missile->WriteNoPos( mesg );
#else
	MissileState missile;
	missile.GetFrom( mObject->PO->mMissileController );
	if( sendPosOrient )
		missile.Write( mesg );
	else
		missile.WriteNoPos( mesg );
#endif
#endif // TRANSGAMING_NO_HAVOK

	mesg->WriteCheckToken( PHO_CREATE );
}

void EntitySynchronizer2::InterpretCreateMissilePO( UdpMessage* mesg, NetworkPeer2* peer, NetworkObjectState* newState, bool dump, bool recvPosOrient )
{
	PONetCreationEntry2 entry;
	peer->POCreationCache.ReceivePOCreationData( mesg, entry );

	newState->Orientation = recvPosOrient?mesg->ReadQuaternionOptimized():mObject->GetRotation();
	newState->AngularVelocity = Vector(0,0,0);
#ifndef NO_MISSILE_UPDATES
	m_setTag("Net");
    newState->Missile = new MissileState;

	if( recvPosOrient )
	{
		newState->Missile->Read( mesg );
		newState->Position = newState->Missile->Position;
	}
	else
	{
		newState->Missile->ReadNoPos( mesg );
		newState->Missile->Position = newState->Position;
	}
#else
	MissileState missile;

	if( recvPosOrient )
	{
		missile.Read( mesg );
		newState->Position = missile.Position;
	}
	else
	{
		missile.ReadNoPos( mesg );
		missile.Position = newState->Position;
	}
#endif

	mesg->ReadCheckToken( PHO_CREATE );

	if( !mObject || dump )
		return;

	unsigned short objectState = entry.ObjectState;
	float mass = entry.Mass;
	float friction = entry.Friction;
	float damping = entry.Damping;
	float bodyScale = entry.BodyScale;
	unsigned char bodyType = entry.BodyType;
	char freedomOfRotation = entry.FreedomOfRotation;
	newState->CollisionGroup = entry.CollisionGroup;
	Assert( newState->CollisionGroup <= 31 );
	newState->HasGravity = entry.HasGravity;

	mObject->SetPositionAndRotation( newState->Position, newState->Orientation );
	Assert( !mObject->PO );
#ifdef	TRANSGAMING
#ifndef TRANSGAMING_NO_HAVOK
	mObject->PO = GEngine->pcfWorld->Physics->CreatePhysicsObject( newState->Position, newState->Orientation,
		bodyType, mObject, bodyScale, newState->CollisionGroup );
#endif //TRANSGAMING_NO_HAVOK
#else
	mObject->PO = GEngine->World->Physics->CreatePhysicsObject( newState->Position, newState->Orientation,
		bodyType, mObject, bodyScale, newState->CollisionGroup );
#endif	

	mObject->PO->State = objectState;
#ifndef TRANSGAMING_NO_HAVOK
	mObject->PO->SetFreedomOfRotation( freedomOfRotation );
	mObject->PO->SetAngularVel( newState->AngularVelocity );

	if( mass != mObject->PO->GetMass() )
		mObject->PO->SetMass( mass );

	char* synStr = mObject->GetSynchroString();
	if( synStr )
		GScript.QueueMessage("PO_CREATE","%e,%p,%s",mObject,mObject->PO,synStr);

	mObject->PO->SetMPMissileController();
	mObject->PO->mMissileController->setMissileFriction( friction );
	mObject->PO->mMissileController->setDamping( damping );

#ifndef NO_MISSILE_UPDATES
	newState->Missile->SetInto( mObject->PO->mMissileController );
#else
	missile.SetInto( mObject->PO->mMissileController );
#endif

#endif // TRANSGAMING_NO_HAVOK

//it is the float limitation function
//I can not find the same function in gcc
	ulong32 fpucontrol = _controlfp( 0, 0 );
#if defined( TRANSGAMING_PS2 )
	_controlfp( _PS2_64, _MCW_PS2 );
#elif defined( TRANSGAMING_PSP )
	_controlfp( _PSP_64, _MCW_PSP );
#else
	_controlfp( _PC_64, _MCW_PC );
#endif

#ifndef TRANSGAMING_NO_HAVOK
	for( ulong32 j = 0; j < peer->TempPhysFrames; ++j )
		mObject->PO->mMissileController->missileMove( (DWORD)(1000/GEngine->Physics->PhysicsFPS) );
#endif

#ifdef TRANSGAMING_PS2
	_controlfp( fpucontrol, _MCW_PS2 );
#elif TRANSGAMING_PSP
	_controlfp( fpucontrol, _MCW_PSP );
#else
	_controlfp( fpucontrol, _MCW_PC );
#endif
}

//#define TEMP_MESG

bool EntitySynchronizer2::SendUpdateNormalPO( UdpMessage* mesg, NetworkObjectState* newState, NetworkObjectState* old )
{
	Assert( mObject && mObject->PO && ( UniqueNumber != -1 ) );

#ifndef TRANSGAMING_NO_HAVOK
	newState->Position = mObject->PO->GetPosition();
	newState->Orientation = mObject->PO->GetRotQuaternion();
	newState->Velocity = mObject->PO->GetVel();
	newState->AngularVelocity = mObject->PO->GetAngularVel();
	newState->CollisionGroup = mObject->PO->GetCollisionGroup();
	Assert( newState->CollisionGroup <= 31 );
	newState->HasGravity = ( mObject->PO->GetGravity().MagnitudeSquared() > 0.f );
#endif
	unsigned char flags = 0;
	int pos = mesg->GetPos();
	mesg->WriteUChar(0);	// placeholder

#ifdef TEMP_MESG
	String a = String::Sprintf( "%s,%s: ", mObject->Name.AsChar(), mObject->FileName.AsChar() );
#endif

	if( newState->Position != old->Position )
	{
		flags |= (1<<0);
		mesg->WritePositionOptimized( newState->Position );
#ifdef TEMP_MESG
		a += String::Sprintf( ", pos: ( (%f,%f,%f) != (%f,%f,%f) )",
			old->Position.X, old->Position.Y, old->Position.Z,
			newState->Position.X, newState->Position.Y, newState->Position.Z );
#endif
	}

	if( !( newState->Orientation == old->Orientation ) )
	{
		flags |= (1<<1);
		mesg->WriteQuaternionOptimized2( newState->Orientation );
#ifdef TEMP_MESG
		a += String::Sprintf( ", quat: ( (%f,%f,%f,%f) != (%f,%f,%f,%f) )",
			old->Orientation.W, old->Orientation.X, old->Orientation.Y, old->Orientation.Z, 
			newState->Orientation.W, newState->Orientation.X, newState->Orientation.Y, newState->Orientation.Z );
#endif
	}

	if( newState->Velocity != old->Velocity )
	{
		flags |= (1<<2);
		mesg->WriteLinearVelocityOptimized( newState->Velocity );
#ifdef TEMP_MESG
		a += String::Sprintf( ", vel: ( (%f,%f,%f) != (%f,%f,%f) )",
			old->Velocity.X, old->Velocity.Y, old->Velocity.Z,
			newState->Velocity.X, newState->Velocity.Y, newState->Velocity.Z );
#endif
	}

	if( newState->AngularVelocity != old->AngularVelocity )
	{
		flags |= (1<<3);
		mesg->WriteAngularVelocityOptimized( newState->AngularVelocity );
#ifdef TEMP_MESG
		a += String::Sprintf( ", ang vel: ( (%f,%f,%f) != (%f,%f,%f) )",
			old->AngularVelocity.X, old->AngularVelocity.Y, old->AngularVelocity.Z,
			newState->AngularVelocity.X, newState->AngularVelocity.Y, newState->AngularVelocity.Z );
#endif
	}

	if( newState->CollisionGroup != old->CollisionGroup )
	{
		flags |= (1<<4);
		mesg->WriteUChar( newState->CollisionGroup );
#ifdef TEMP_MESG
		a += String::Sprintf( ", col grp: ( %u != %u )", old->CollisionGroup, newState->CollisionGroup );
#endif
	}

	if( newState->HasGravity != old->HasGravity )
	{
		flags |= (1<<5);
#ifdef TEMP_MESG
		a += String::Sprintf( ", gravity: ( %s != %s )", old->HasGravity?"true":"false", newState->HasGravity?"true":"false" );
#endif
	}

	if( flags )
	{
		int pos2 = mesg->GetPos();
		mesg->Seek(pos);
		mesg->WriteUChar(flags);
		mesg->Seek(pos2);
		mesg->WriteCheckToken( PHO_UPDATE );
#ifdef TEMP_MESG
		a += "\n";
		GLog.Print( a.AsChar() );
#endif
		return true;
	}
	else
	{
		mesg->Seek( pos );
		return false;
	}
}

#ifdef TEMP_MESG
#undef TEMP_MESG
#endif

void EntitySynchronizer2::InterpretUpdateNormalPO( UdpMessage* mesg, NetworkObjectState* newState, NetworkObjectState* old )
{
	Assert( mObject && mObject->PO && ( UniqueNumber != -1 ) );

	unsigned char flags = mesg->ReadUChar();

	newState->Position = (flags&(1<<0))?mesg->ReadPositionOptimized():old->Position;
	newState->Orientation = (flags&(1<<1))?mesg->ReadQuaternionOptimized2():old->Orientation;
	newState->Velocity = (flags&(1<<2))?mesg->ReadLinearVelocityOptimized():old->Velocity;
	newState->AngularVelocity = (flags&(1<<3))?mesg->ReadAngularVelocityOptimized():old->AngularVelocity;
	newState->CollisionGroup = (flags&(1<<4))?mesg->ReadUChar():old->CollisionGroup;
	Assert( newState->CollisionGroup <= 31 );
	if( flags & (1<<5) )
		newState->HasGravity = !newState->HasGravity;
	mesg->ReadCheckToken( PHO_UPDATE );
#ifndef TRANSGAMING_NO_HAVOK
	mObject->PO->SetRotQuaternion( newState->Orientation );
	mObject->PO->SetPosition( newState->Position );
	mObject->PO->SetVel( newState->Velocity );
	mObject->PO->SetAngularVel( newState->AngularVelocity );
	mObject->PO->SetCollisionGroup( newState->CollisionGroup );
	mObject->PO->EnableGravity( newState->HasGravity );
#endif

}

void EntitySynchronizer2::CopyUpdateNormalPO( NetworkObjectState* newState, NetworkObjectState* old )
{
	Assert( mObject && mObject->PO && ( UniqueNumber != -1 ) );

	newState->Position = old->Position;
	newState->Orientation = old->Orientation;
	newState->Velocity = old->Velocity;
	newState->AngularVelocity = old->AngularVelocity;
	newState->CollisionGroup = old->CollisionGroup;
	Assert( newState->CollisionGroup <= 31 );
	newState->HasGravity = old->HasGravity;
#ifndef TRANSGAMING_NO_HAVOK
	mObject->PO->SetRotQuaternion( newState->Orientation );
	mObject->PO->SetPosition( newState->Position );
	mObject->PO->SetVel( newState->Velocity );
	mObject->PO->SetAngularVel( newState->AngularVelocity );
	mObject->PO->SetCollisionGroup( newState->CollisionGroup );
	mObject->PO->EnableGravity( newState->HasGravity );
#endif
}

void EntitySynchronizer2::DeleteSubject( void )
{
	if( mObject )
	{
		char* str = mObject->GetSynchroString();
		if( str && ::strlen(str) )
			GScript.QueueMessage( "ENTITY_DELETE","%e,%s", mObject, str );

		RemoveConnectionWithEntity( mObject );
#ifdef	TRANSGAMING
		GEngine->pcfWorld->DeleteEntityDelayed( mObject );
#else
		GEngine->World->DeleteEntityDelayed( mObject );
#endif	
		mObject = 0;
	}
}

// =========================================================================================================

MeshSynchronizer2::MeshSynchronizer2( ulong32 uniqueNumber, NetworkDevice2* device, WorldMesh* meshToSynchronize )
:	EntitySynchronizer2( uniqueNumber, device )
,	ActiveMeshIndex( -1 )
{
	mObject = (Entity*)meshToSynchronize;
}

MeshSynchronizer2::~MeshSynchronizer2()
{
}

void MeshSynchronizer2::AddCreateObjMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame )
{
	Assert( mObject &&( UniqueNumber != -1 ) );
	WriteSynchronizer( message, UniqueNumber, STypeMesh );

    m_setTag("Net");
	NetworkObjectState* state = new NetworkObjectState( UniqueNumber );

#ifndef TRANSGAMING_NO_HAVOK
	if( mObject->PO )
		ActiveMeshIndex = mObject->PO->GetActiveMeshIndex();
	else
#endif
		ActiveMeshIndex = -1;

//	GLog.Print( "Mesh::CREATEOBJ %d sent\n", UniqueNumber );

	message->WriteShort( ActiveMeshIndex );
	peer->StringTransport.WriteString( message, mObject->FileName );
	peer->StringTransport.WriteString( message, mObject->Name );
	message->WriteFloat( mObject->GetScale() );
	message->WriteULong( (ulong32)mObject->Flags );

	state->POType = NetworkObjectState::PO_None;
	if( mObject->PO )
	{
		if( mObject->PO->mMissileController )
			state->POType = NetworkObjectState::PO_Missile;
		else if( mObject->PO->mPlayerVariables )
			state->POType = NetworkObjectState::PO_Player;
		else
			state->POType = NetworkObjectState::PO_Normal;
	}
	message->WriteUChar( state->POType );

	state->Position = mObject->GetWorldPosition();
	if( state->POType != NetworkObjectState::PO_Missile )
		message->WritePositionOptimized( state->Position );
	else
		message->WriteVector( state->Position );
	state->Orientation = mObject->GetWorldRotation();
	message->WriteQuaternionOptimized( state->Orientation );

	char* synStr = mObject->GetSynchroString();
	if( synStr )
	{
		String s( synStr );
		message->WriteBool( true );
		peer->StringTransport.WriteString( message, s );
	}
	else
		message->WriteBool( false );

	message->WriteCheckToken( MESH_CREATE );

	switch( state->POType )
	{
	case NetworkObjectState::PO_Normal:
		SendCreateNormalPO( message, peer, state, false );
		break;
	case NetworkObjectState::PO_Player:
		m_setTag("Net");
        state->Player = new PlayerState;
		state->Player->GetFromPlayer( mObject->PO );
		state->Player->Write( message );
#ifndef LOCAL_PLAYER_INPUT
        m_setTag("Net");
		state->Input = new PlayerInput;
		state->Input->GetFromPlayer( mObject->PO );
		state->Input->WriteNoDelta( message );
#else
		{
			PlayerInput input;
			input.GetFromPlayer( mObject->PO );
			input.WriteNoDelta( message );
		}
#endif
		break;
	case NetworkObjectState::PO_Missile:
		SendCreateMissilePO( message, peer, state, false );
		break;
	default:
		break;
	}

	// Add state to frame
	state->Next = targetFrame->Answer->EntitiesStateList;
	targetFrame->Answer->EntitiesStateList = state;
}

bool MeshSynchronizer2::AddUpdateObjMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame, NetworkObjectState* oldState )
{
	int startPos = message->GetPos();
	bool doNotSend = false;

	Assert( mObject &&( UniqueNumber != -1 ) );
	WriteSynchronizer( message, UniqueNumber, STypeMesh );

	TCO_SETOBJ(../Data/Items/kolek.dat);

    m_setTag("Net");
	NetworkObjectState* state = new NetworkObjectState( UniqueNumber );

	TCO_WRITE(01);

	// Placeholder for flags
	unsigned char flags = 0;
	int mesgPos = message->GetPos();
	message->WriteUChar( 0 );

	if( mObject->PO && ( oldState->POType == NetworkObjectState::PO_None ) )	// create PO
	{
		flags |= NetworkObjectState::NET_CREATEPO;

		TCO_WRITE(02);

		if( mObject->PO->mMissileController )
			state->POType = NetworkObjectState::PO_Missile;
		else if( mObject->PO->mPlayerVariables )
			state->POType = NetworkObjectState::PO_Player;
		else
			state->POType = NetworkObjectState::PO_Normal;

		message->WriteUChar( state->POType );

		switch( state->POType )
		{
		case NetworkObjectState::PO_Missile:
			SendCreateMissilePO( message, peer, state, true );
			break;
		case NetworkObjectState::PO_Player:
			TCO_WRITE(03);
            m_setTag("Net");
			state->Player = new PlayerState;
			state->Player->GetFromPlayer( mObject->PO );
			state->Player->Write( message );
#ifndef LOCAL_PLAYER_INPUT
            m_setTag("Net");
			state->Input = new PlayerInput;
			state->Input->GetFromPlayer( mObject->PO );
			state->Input->WriteNoDelta( message );
#else
			{
				PlayerInput input;
				input.GetFromPlayer( mObject->PO );
				input.WriteNoDelta( message );
			}
#endif
			break;
		case NetworkObjectState::PO_Normal:
			SendCreateNormalPO( message, peer, state, true );
			break;
		default:
			Assert( false );
		}
	}
	else if( mObject->PO )	// update PO
	{
		TCO_WRITE(05);
		if( mObject->PO->mMissileController )
		{
			state->POType = NetworkObjectState::PO_Missile;
#ifndef NO_MISSILE_UPDATES
            m_setTag("Net");
			state->Missile = new MissileState;
			state->Missile->GetFrom( mObject->PO->mMissileController );
			Assert( oldState->Missile );
			if( !(*state->Missile == *oldState->Missile) )
			{
				flags |= NetworkObjectState::NET_PODATA;
				state->Missile->Write( message );
			}
#endif
		}
		else if( mObject->PO->mPlayerVariables )		// Add player PO update part
		{
			TCO_WRITE(06);
			state->POType = NetworkObjectState::PO_Player;
            m_setTag("Net");
			state->Player = new PlayerState;

#ifndef TRANSGAMING_NO_HAVOK
			if( peer->mLocalPlayerOptimizedEveryNFrames && ( targetFrame->Number % peer->mLocalPlayerOptimizedEveryNFrames == 0 ) )
				mObject->PO->AddMPExternalInfluence();
#endif

			state->Player->GetFromPlayer( mObject->PO );
			if( targetFrame->Answer->LocalPlayerEntity != mObject )
			{
				Assert( oldState->Player );
				state->Player->WriteAbridged( message, *oldState->Player );
#ifndef LOCAL_PLAYER_INPUT
                m_setTag("Net");
				state->Input = new PlayerInput;
				state->Input->GetFromPlayer( mObject->PO );
				state->Input->WriteYawPitch( message );
#else
				PlayerInput input;
				input.GetFromPlayer( mObject->PO );
				input.WriteYawPitch( message );
#endif
				flags |= NetworkObjectState::NET_POINPUT;
			}
			else
			{
				Assert( oldState->Player );
				state->Player->WriteDiff( message, *oldState->Player, ( peer->mLocalPlayerOptimizedEveryNFrames != 0 ) );
				flags |= NetworkObjectState::NET_PODATA;
			}
		}
		else									// Add normal PO update part
		{
			TCO_WRITE(07);
			state->POType = NetworkObjectState::PO_Normal;
			if( SendUpdateNormalPO( message, state, oldState ) )
				flags |= NetworkObjectState::NET_PODATA;
		}
	}
	else					// update mesh ( and delete PO, if there was one )
	{
		TCO_WRITE(08);

		bool deletePO = ( oldState->POType != NetworkObjectState::PO_None );

		if( deletePO )
			flags |= NetworkObjectState::NET_DELETEPO;

		state->POType = NetworkObjectState::PO_None;

		state->Position = mObject->GetWorldPosition();
		if( deletePO || ( state->Position != oldState->Position ) )
		{
			flags |= NetworkObjectState::NET_POSITION;
			message->WritePositionOptimized( state->Position );
		}

		state->Orientation = mObject->GetWorldRotation();

		if( deletePO || !( state->Orientation == oldState->Orientation ) )
		{
			flags |= NetworkObjectState::NET_ORIENTATION;
			message->WriteQuaternionOptimized( state->Orientation );
		}

		message->WriteCheckToken( MESH_UPDATE );
	}

	TCO_WRITE(09);

	// Write flags
	int mesgPos2 = message->GetPos();
	if( mesgPos2 != mesgPos )
	{
		message->Seek(mesgPos);
		message->WriteUChar(flags);
		message->Seek(mesgPos2);
	}
	else
		message->WriteUChar(flags);

	if( flags == 0 )
	{
		doNotSend = true;
		TCO_WRITE(10);
	}

	// Add state to frame
	state->Next = targetFrame->Answer->EntitiesStateList;
	targetFrame->Answer->EntitiesStateList = state;

	if( doNotSend )
		message->Seek(startPos);
//	else
//		GLog.Print( "Mesh '%s': updated %d bytes, flags: %u\n", mObject->FileName.AsChar(), message->GetPos()-startPos, flags );
	return !doNotSend;
}

bool MeshSynchronizer2::InterpretCreateMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame )
{
    m_setTag("Net");
	NetworkObjectState* state = new NetworkObjectState( UniqueNumber );

	short activeMeshIndex = message->ReadShort();
	String filename = peer->StringTransport.ReadString( message );
	String entityName = peer->StringTransport.ReadString( message );
	float scale = message->ReadFloat();
	DWORD flags = (DWORD)message->ReadULong();

//	GLog.Print( "Flags read: %u\n", flags );

	state->POType = (NetworkObjectState::TypeOfPO)message->ReadUChar();
	state->Position = (state->POType == NetworkObjectState::PO_Missile)?message->ReadVector():message->ReadPositionOptimized();
	state->Orientation = message->ReadQuaternionOptimized();

	String synStr = "";
	if( message->ReadBool() )
		synStr = peer->StringTransport.ReadString( message );

	message->ReadCheckToken( MESH_CREATE );

	bool alreadyExisted = ( mObject != 0 );	// It already existed; that's really an update

#ifdef	TRANSGAMING
#ifndef TRANSGAMING_NO_HAVOK
	if( !mObject && ( activeMeshIndex != -1 ) )
		mObject = GEngine->pcfWorld->Physics->GetEntityByActiveMeshIndex( activeMeshIndex );
#endif // TRANSGAMING_NO_HAVOK
#else
	if( !mObject && ( activeMeshIndex != -1 ) )
		mObject = GEngine->World->Physics->GetEntityByActiveMeshIndex( activeMeshIndex );
#endif	

	if( mObject && !alreadyExisted )
		MakeConnectionWithEntity( mObject );

	// Check if we have to make a new one
	if( !mObject )
	{
		// Create new entity with those contents
#ifdef	TRANSGAMING
		mObject = (WorldMesh*)GEngine->pcfWorld->CreateEntity( Entity::ETypeMesh, filename.AsChar(), entityName.AsChar(), scale, (flags & Entity::EEntCenteredGeometry)!=0 );
#else
		mObject = (WorldMesh*)GEngine->World->CreateEntity( Entity::ETypeMesh, filename.AsChar(), entityName.AsChar(), scale, (flags & Entity::EEntCenteredGeometry)!=0 );
#endif	
		if( !mObject )
		{
			GLog.Print( "Was unable to create mesh! filename = %s, entityName = %s, scale = %f, unique = %d\n",
				filename.AsChar(), entityName.AsChar(), scale, UniqueNumber );
			message->DumpToLogfile( "Unable to create mesh" );
			delete state;
			return false;
		}

#ifdef	TRANSGAMING
		GEngine->pcfWorld->AddEntity( mObject );
#else
		GEngine->World->AddEntity( mObject );
#endif	
		mObject->Flags = flags;
	}

	if( alreadyExisted && mObject->PO )
	{
		PhysicsObject* poTemp = mObject->PO;
		mObject->PO = 0;
		mObject->SetPositionAndRotation( state->Position, state->Orientation );
		mObject->PO = poTemp;
	}
	else
		mObject->SetPositionAndRotation( state->Position, state->Orientation );

	if( synStr.Len() && !alreadyExisted )
	{
		mObject->SetSynchroString(synStr.AsChar());
		GScript.QueueMessage("ENTITY_CREATE","%e,%s",mObject,synStr.AsChar());
	}

	// PO Create
	switch( state->POType )
	{
	case NetworkObjectState::PO_None:
		if( alreadyExisted && mObject->PO )
			mObject->RemovePhysicsObject();
		break;
	case NetworkObjectState::PO_Missile:
		InterpretCreateMissilePO( message, peer, state, alreadyExisted, false );
		if( alreadyExisted && mObject->PO )
		{
			Assert( mObject->PO->mMissileController );
		}
		break;
	case NetworkObjectState::PO_Player:
		if( alreadyExisted && mObject->PO )
		{
			Assert( mObject->PO->mPlayerVariables->Controller );
		}
		else
		{
			mObject->CreatePhysicsObject( EPawnBodyPlayer, 1.0f, -1, false );
			char* synStr = mObject->GetSynchroString();
			if( synStr )
				GScript.QueueMessage("PO_CREATE","%e,%p,%s",mObject,mObject->PO,synStr);
		}

        m_setTag("Net");
		state->Player = new PlayerState;
		state->Player->Read( message );
		state->Player->SetIntoPlayer( mObject->PO );
#ifndef LOCAL_PLAYER_INPUT
        m_setTag("Net");
		state->Input = new PlayerInput;
		state->Input->ReadNoDelta( message );
		state->Input->SetIntoPlayer( mObject->PO );
#else
		{
			PlayerInput input;
			input.ReadNoDelta( message );
			input.SetIntoPlayer( mObject->PO );
		}
#endif
		break;
	case NetworkObjectState::PO_Normal:
		InterpretCreateNormalPO( message, peer, state, alreadyExisted, false );
		break;
	default:
		Assert( state->POType == NetworkObjectState::PO_None );	// to crash
	}

	// Add state to frame
	state->Next = targetFrame->Answer->EntitiesStateList;
	targetFrame->Answer->EntitiesStateList = state;

	return true;
}

bool MeshSynchronizer2::InterpretUpdateMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame, NetworkFrame* deltaFrame )
{
	Assert( mObject );
	Assert( UniqueNumber != -1 );

	TCO_SETOBJ(../Data/Items/kolek.dat);

    m_setTag("Net");
	NetworkObjectState* state = new NetworkObjectState( UniqueNumber );

	// Find the correct state
	NetworkObjectState* oldState = 0;
	for( oldState = deltaFrame->Answer->EntitiesStateList; oldState; oldState = oldState->Next )
		if( oldState->UniqueNumber == UniqueNumber )
			break;

	Assert( oldState );

	TCO_WRITE(01);

	unsigned char flags = message->ReadUChar();

	if( flags & NetworkObjectState::NET_CREATEPO )
	{
		TCO_WRITE(02);
		int poType = message->ReadUChar();
		switch( poType )
		{
		case NetworkObjectState::PO_Missile:
			InterpretCreateMissilePO( message, peer, state, false, true );
			break;
		case NetworkObjectState::PO_Player:
			{
				TCO_WRITE(03);
				mObject->CreatePhysicsObject( EPawnBodyPlayer, 1.0f, -1, false );
				char* synStr = mObject->GetSynchroString();
				if( synStr )
					GScript.QueueMessage("PO_CREATE","%e,%p,%s",mObject,mObject->PO,synStr);

                m_setTag("Net");
				state->Player = new PlayerState;
				state->Player->Read( message );
				state->Player->SetIntoPlayer( mObject->PO );
#ifndef LOCAL_PLAYER_INPUT
                m_setTag("Net");
				state->Input = new PlayerInput;
				state->Input->ReadNoDelta( message );
				state->Input->SetIntoPlayer( mObject->PO );
#else
				PlayerInput input;
				input.ReadNoDelta( message );
				input.SetIntoPlayer( mObject->PO );
#endif
			}
			break;
		case NetworkObjectState::PO_Normal:
			TCO_WRITE(04);
			InterpretCreateNormalPO( message, peer, state, false, true );
			break;
		default:
			TCO_WRITE(05);
			Assert( false );
			break;
		}
	}
	else
	{
		TCO_WRITE(06);

		if( ( flags & NetworkObjectState::NET_DELETEPO ) && mObject->PO )
		{
			TCO_WRITE(07);
			mObject->RemovePhysicsObject();
		}

		if( mObject->PO )
		{
			TCO_WRITE(08);
			if( mObject->PO->mMissileController )
			{
#ifndef NO_MISSILE_UPDATES
                m_setTag("Net");
				state->Missile = new MissileState;
				if( flags & NetworkObjectState::NET_PODATA )
					state->Missile->Read( message );
				else
				{
					Assert( oldState->Missile );
					*state->Missile = *oldState->Missile;
				}
				state->Missile->SetInto( mObject->PO->mMissileController );
#endif
			}
			else if( mObject->PO->mPlayerVariables )
			{
				TCO_WRITE(09);
                m_setTag("Net");
				state->Player = new PlayerState;
				if( flags & NetworkObjectState::NET_POINPUT )
				{
					Assert( oldState->Player );
					state->Player->ReadAbridged( message, *oldState->Player );
					state->Player->SetIntoPlayerAbridged( mObject->PO );
#ifndef LOCAL_PLAYER_INPUT
                    m_setTag("Net");
					state->Input = new PlayerInput;
					state->Input->ReadYawPitch( message );
					state->Input->SetIntoPlayer( mObject->PO );
#else
					PlayerInput input;
					input.ReadYawPitch( message );
					input.SetIntoPlayer( mObject->PO );
#endif
				}
				else
				{
					Assert( oldState->Player );
					state->Player->ReadDiff( message, *oldState->Player, ( peer->mLocalPlayerOptimizedEveryNFrames != 0 ) );
					Assert( targetFrame->Answer->LocalPlayerEntity == mObject );

/*
#ifdef PLAYER_COMPARE
					if( state->Player->ExternalInfluences )
					{
						if( targetFrame->PlayerData == *state->Player )
							GLog.Print( "###2### Diff: player data identical, and shouldn't be for frame: %d\n", targetFrame->Number );
					}
#endif
*/

					if( peer->mLocalPlayerOptimizedEveryNFrames && !state->Player->ExternalInfluences )
					{
						*state->Player = targetFrame->PlayerData;
						targetFrame->Answer->PredictAgainLocalPlayer = false;
					}
					else if( targetFrame->PlayerData == *state->Player )
					{
						targetFrame->Answer->PredictAgainLocalPlayer = false;	// Optimalization; no prediction recalculation if the answer for local player same as the local state
						state->Player->SetMPByte( mObject->PO );
					}
					else
					{
						Assert( targetFrame->Answer->PredictAgainLocalPlayer == true );
						state->Player->SetIntoPlayer( mObject->PO );
#ifdef PLAYER_COMPARE
						if( !state->Player->ExternalInfluences )
						{
							if( targetFrame->PlayerData.CompareWithPlayer( mObject->PO ) )
								GLog.Print( "###3### Diff: player data non-identical, and should be for frame: %d\n", targetFrame->Number );
						}
#endif
					}
				}
			}
			else
			{
				TCO_WRITE(10);
				if( flags & NetworkObjectState::NET_PODATA )
					InterpretUpdateNormalPO( message, state, oldState );
				else
					CopyUpdateNormalPO( state, oldState );
			}
		}
		else
		{
			TCO_WRITE(11);
			flags &= NetworkObjectState::NET_POSITION|NetworkObjectState::NET_ORIENTATION;

			if( flags & NetworkObjectState::NET_POSITION )
			{
				if( flags & NetworkObjectState::NET_ORIENTATION )
				{
					Vector position = message->ReadPositionOptimized();
					Quaternion orientation = message->ReadQuaternionOptimized();
					mObject->SetPositionAndRotation( position, orientation );
				}
				else
					mObject->SetPosition( message->ReadPositionOptimized() );
			}
			else if( flags & NetworkObjectState::NET_ORIENTATION )
				mObject->SetRotation( message->ReadQuaternionOptimized() );
			else
				TCO_WRITE(12);

			message->ReadCheckToken( MESH_UPDATE );
		}
	}

	TCO_WRITE(13);

	// Add state to frame
	state->Next = targetFrame->Answer->EntitiesStateList;
	targetFrame->Answer->EntitiesStateList = state;

	return true;
}

// =========================================================================================================

ModelSynchronizer2::ModelSynchronizer2( ulong32 uniqueNumber, NetworkDevice2* device, Model* modelToSynchronize ) 
:	EntitySynchronizer2( uniqueNumber, device ), mSynchroAnimation( false )
{
	mObject = (Entity*)modelToSynchronize;
	if( mObject )
		mSynchroAnimation = (( mObject->SynchroState() & Entity::ESynchroAnimation ) != 0);
}

ModelSynchronizer2::~ModelSynchronizer2()
{
}

void ModelSynchronizer2::AddCreateObjMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame )
{
	Assert( mObject );
	Assert( UniqueNumber != -1 );
	WriteSynchronizer( message, UniqueNumber, STypeModel );

    m_setTag("Net");
	NetworkObjectState* state = new NetworkObjectState( UniqueNumber );

//	GLog.Print( "Model::CREATEOBJ %d sent\n", UniqueNumber );
	peer->StringTransport.WriteString( message, mObject->FileName );
	peer->StringTransport.WriteString( message, mObject->Name );
	message->WriteFloat( ((Model*)mObject)->Mesh->Scale );
	message->WriteULong( (ulong32)mObject->Flags );

	state->POType = NetworkObjectState::PO_None;
	if( mObject->PO )
	{
		if( mObject->PO->mMissileController )
			state->POType = NetworkObjectState::PO_Missile;
		else if( mObject->PO->mPlayerVariables )
			state->POType = NetworkObjectState::PO_Player;
		else
			state->POType = NetworkObjectState::PO_Normal;
	}
	message->WriteUChar( state->POType );

	state->Position = mObject->GetPosition();
	if( state->POType != NetworkObjectState::PO_Missile )
		message->WritePositionOptimized( state->Position );
	else
		message->WriteVector( state->Position );

	state->Orientation = mObject->GetRotation();
	message->WriteQuaternionOptimized( state->Orientation );

	bool animationsFollow = ( ( mObject->SynchroState() & Entity::ESynchroAnimation ) != 0 );
	message->WriteBool( animationsFollow );

	char* synStr = mObject->GetSynchroString();
	if( synStr )
	{
		String s( synStr );
		message->WriteBool( true );
		peer->StringTransport.WriteString( message, s );
	}
	else
		message->WriteBool( false );

	if( mObject->SynchroState() & Entity::ESynchroAnimation )
		SendAnimationInfo( message, peer );

	switch( state->POType )
	{
	case NetworkObjectState::PO_Normal:
		SendCreateNormalPO( message, peer, state, false );
		break;
	case NetworkObjectState::PO_Player:
        m_setTag("Net");
		state->Player = new PlayerState;
		state->Player->GetFromPlayer( mObject->PO );
		state->Player->Write( message );
#ifndef LOCAL_PLAYER_INPUT
        m_setTag("Net");
		state->Input = new PlayerInput;
		state->Input->GetFromPlayer( mObject->PO );
		state->Input->WriteNoDelta( message );
#else
		{
			PlayerInput input;
			input.GetFromPlayer( mObject->PO );
			input.WriteNoDelta( message );
		}
#endif
		break;
	case NetworkObjectState::PO_Missile:
		SendCreateMissilePO( message, peer, state, false );
		break;
	default:
		break;
	}
	message->WriteCheckToken( MODEL_CREATE );

	// Add state to frame
	state->Next = targetFrame->Answer->EntitiesStateList;
	targetFrame->Answer->EntitiesStateList = state;
}

bool ModelSynchronizer2::AddUpdateObjMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame, NetworkObjectState* oldState )
{
	int startPos = message->GetPos();
	bool doNotSend = false;

	Assert( mObject );
	Assert( UniqueNumber != -1 );
	WriteSynchronizer( message, UniqueNumber, STypeModel );

    m_setTag("Net");
	NetworkObjectState* state = new NetworkObjectState( UniqueNumber );

	// Placeholder for flags
	unsigned char flags = 0;
	int mesgPos = message->GetPos();
	message->WriteUChar( 0 );

	if( mObject->PO && ( oldState->POType == NetworkObjectState::PO_None ) )	// create PO
	{
		flags |= NetworkObjectState::NET_CREATEPO;

		if( mObject->PO->mMissileController )
			state->POType = NetworkObjectState::PO_Missile;
		else if( mObject->PO->mPlayerVariables )
			state->POType = NetworkObjectState::PO_Player;
		else
			state->POType = NetworkObjectState::PO_Normal;

		message->WriteUChar( state->POType );

		switch( state->POType )
		{
		case NetworkObjectState::PO_Missile:
			SendCreateMissilePO( message, peer, state, true );
			break;
		case NetworkObjectState::PO_Player:
            m_setTag("Net");
			state->Player = new PlayerState;
			state->Player->GetFromPlayer( mObject->PO );
			state->Player->Write( message );
#ifndef LOCAL_PLAYER_INPUT
            m_setTag("Net");
			state->Input = new PlayerInput;
			state->Input->GetFromPlayer( mObject->PO );
			state->Input->WriteNoDelta( message );
#else
			{
				PlayerInput input;
				input.GetFromPlayer( mObject->PO );
				input.WriteNoDelta( message );
			}
#endif
			break;
		case NetworkObjectState::PO_Normal:
			SendCreateNormalPO( message, peer, state, true );
			break;
		default:
			Assert( false );
			break;
		}
	}
	else if( mObject->PO )	// update PO
	{
		if( mObject->PO->mMissileController )
		{
			state->POType = NetworkObjectState::PO_Missile;
#ifndef NO_MISSILE_UPDATES
            m_setTag("Net");
			state->Missile = new MissileState;
			state->Missile->GetFrom( mObject->PO->mMissileController );
			Assert( oldState->Missile );
			if( !(*state->Missile == *oldState->Missile) )
			{
				flags |= NetworkObjectState::NET_PODATA;
				state->Missile->Write( message );
			}
#endif
		}
		else if( mObject->PO->mPlayerVariables )		// Add player PO update part
		{
			state->POType = NetworkObjectState::PO_Player;
            m_setTag("Net");
			state->Player = new PlayerState;

#ifndef TRANSGAMING_NO_HAVOK
			if( peer->mLocalPlayerOptimizedEveryNFrames && ( targetFrame->Number % peer->mLocalPlayerOptimizedEveryNFrames == 0 ) )
				mObject->PO->AddMPExternalInfluence();
#endif
			state->Player->GetFromPlayer( mObject->PO );
			if( targetFrame->Answer->LocalPlayerEntity != mObject )
			{
				Assert( oldState->Player );
				state->Player->WriteAbridged( message, *oldState->Player );
#ifndef LOCAL_PLAYER_INPUT
                m_setTag("Net");
				state->Input = new PlayerInput;
				state->Input->GetFromPlayer( mObject->PO );
				state->Input->WriteYawPitch( message );
#else
				PlayerInput input;
				input.GetFromPlayer( mObject->PO );
				input.WriteYawPitch( message );
#endif
				flags |= NetworkObjectState::NET_POINPUT;
			}
			else
			{
				Assert( oldState->Player );
				state->Player->WriteDiff( message, *oldState->Player, ( peer->mLocalPlayerOptimizedEveryNFrames != 0 ) );
				flags |= NetworkObjectState::NET_PODATA;
			}
		}
		else									// Add normal PO update part
		{
			state->POType = NetworkObjectState::PO_Normal;
			if( SendUpdateNormalPO( message, state, oldState ) )
				flags |= NetworkObjectState::NET_PODATA;
		}
	}
	else					// update model ( and delete PO, if there was one )
	{
		bool deletePO = ( oldState->POType != NetworkObjectState::PO_None );	// there was a PO

		if( deletePO )
			flags |= NetworkObjectState::NET_DELETEPO;

		state->POType = NetworkObjectState::PO_None;
		state->Position = mObject->GetPosition();
		if( deletePO || ( state->Position != oldState->Position ) )
		{
			flags |= NetworkObjectState::NET_POSITION;
			message->WritePositionOptimized( state->Position );
		}

		state->Orientation = mObject->GetRotation();
		if( deletePO || !( state->Orientation == oldState->Orientation ) )
		{
			flags |= NetworkObjectState::NET_ORIENTATION;
			message->WriteQuaternionOptimized( state->Orientation );
		}
	}
	message->WriteCheckToken( MODEL_UPDATE );

	// Write flags
	int mesgPos2 = message->GetPos();
	if( mesgPos2 != mesgPos )
	{
		message->Seek(mesgPos);
		message->WriteUChar(flags);
		message->Seek(mesgPos2);
	}
	else
		message->WriteUChar(flags);

	// Add state to frame
	state->Next = targetFrame->Answer->EntitiesStateList;
	targetFrame->Answer->EntitiesStateList = state;

	if( flags == 0 )
		doNotSend = true;

	if( doNotSend )
		message->Seek(startPos);
//	else
//		GLog.Print( "Model '%s': updated %d bytes, flags: %u\n", mObject->FileName.AsChar(), message->GetPos()-startPos, flags );
	return !doNotSend;
}

bool ModelSynchronizer2::InterpretCreateMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame )
{
	bool alreadyExisted = ( mObject != 0 );

    m_setTag("Net");
	NetworkObjectState* state = new NetworkObjectState( UniqueNumber );

	String filename = peer->StringTransport.ReadString( message );
	String entityName = peer->StringTransport.ReadString( message );
	float scale = message->ReadFloat();
	DWORD flags = (DWORD)message->ReadULong();
	unsigned char poType = message->ReadUChar();
	state->Position = (poType == NetworkObjectState::PO_Missile)?message->ReadVector():message->ReadPositionOptimized();
	state->Orientation = message->ReadQuaternionOptimized();

	bool animationsFollow = message->ReadBool();

	String synStr = "";
	if( message->ReadBool() )
		synStr = peer->StringTransport.ReadString( message );

	// Create new entity with those contents
	if( !mObject )
	{
#ifdef	TRANSGAMING
		mObject = (Model*)GEngine->pcfWorld->CreateEntity( Entity::ETypeModel, filename.AsChar(), entityName.AsChar(), scale );
#else
		mObject = (Model*)GEngine->World->CreateEntity( Entity::ETypeModel, filename.AsChar(), entityName.AsChar(), scale );
#endif	
		if( !mObject )
		{
			GLog.Print( "Was unable to create model! filename = %s, entityName = %s, scale = %f, unique = %d\n",
				filename.AsChar(), entityName.AsChar(), scale, UniqueNumber );
			message->DumpToLogfile( "Unable to create model" );

			// Dump rest of model data
			if( animationsFollow )
				DumpAnimationInfo( message, peer );

			// PO Create
			switch( poType )
			{
			default:
				break;
			case NetworkObjectState::PO_Missile:
				InterpretCreateMissilePO( message, peer, state, true, false );
				break;
			case NetworkObjectState::PO_Player:
                m_setTag("Net");
				state->Player = new PlayerState;
				state->Player->Read( message );
				message->ReadShort();
				message->ReadShort();
				message->ReadUChar();
				break;
			case NetworkObjectState::PO_Normal:
				InterpretCreateNormalPO( message, peer, state, true, false );
				break;
			}

			message->ReadCheckToken( MODEL_CREATE );
			delete state;
			return true;
		}

#ifdef	TRANSGAMING
		GEngine->pcfWorld->AddEntity( mObject );
#else
		GEngine->World->AddEntity( mObject );
#endif	
		MakeConnectionWithEntity( mObject );
		mObject->Flags = flags;
	}

	if( alreadyExisted && mObject->PO )
	{
		PhysicsObject* poTemp = mObject->PO;
		mObject->PO = 0;
		mObject->SetPositionAndRotation( state->Position, state->Orientation );
		mObject->PO = poTemp;
	}
	else
		mObject->SetPositionAndRotation( state->Position, state->Orientation );

	if( animationsFollow )
	{
		if( !alreadyExisted )
			ApplyAnimationInfo( message, peer );
		else
			DumpAnimationInfo( message, peer );
	}

	if( synStr.Len() && !alreadyExisted )
	{
		mObject->SetSynchroString(synStr.AsChar());
		GScript.QueueMessage("ENTITY_CREATE","%e,%s",mObject,synStr.AsChar());
	}

	// PO Create
	switch( poType )
	{
	case NetworkObjectState::PO_None:
		if( alreadyExisted && mObject->PO )
			mObject->RemovePhysicsObject();
		break;
	case NetworkObjectState::PO_Missile:
		InterpretCreateMissilePO( message, peer, state, alreadyExisted, false );
		if( alreadyExisted && mObject->PO )
		{
			Assert( mObject->PO->mMissileController );
		}
		break;
	case NetworkObjectState::PO_Player:
		if( alreadyExisted && mObject->PO )
		{
			Assert( mObject->PO->mPlayerVariables->Controller );
		}
		else
		{
			mObject->CreatePhysicsObject( EPawnBodyPlayer, 1.0f, -1, false );
			char* synStr = mObject->GetSynchroString();
			if( synStr )
				GScript.QueueMessage("PO_CREATE","%e,%p,%s",mObject,mObject->PO,synStr);
		}
        m_setTag("Net");
		state->Player = new PlayerState;
		state->Player->Read( message );
		state->Player->SetIntoPlayer( mObject->PO );
#ifndef LOCAL_PLAYER_INPUT
        m_setTag("Net");
		state->Input = new PlayerInput;
		state->Input->ReadNoDelta( message );
		state->Input->SetIntoPlayer( mObject->PO );
#else
		{
			PlayerInput input;
			input.ReadNoDelta( message );
			input.SetIntoPlayer( mObject->PO );
		}
#endif
		break;
	case NetworkObjectState::PO_Normal:
		InterpretCreateNormalPO( message, peer, state, alreadyExisted, false );
		break;
	default:
		Assert( poType == NetworkObjectState::PO_None );	// to crash
	}

	message->ReadCheckToken( MODEL_CREATE );

	// Add state to frame
	state->Next = targetFrame->Answer->EntitiesStateList;
	targetFrame->Answer->EntitiesStateList = state;

	return true;
}

bool ModelSynchronizer2::InterpretUpdateMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame, NetworkFrame* deltaFrame )
{
	Assert( mObject );
	Assert( UniqueNumber != -1 );

    m_setTag("Net");
	NetworkObjectState* state = new NetworkObjectState( UniqueNumber );

	// Find the correct state
	NetworkObjectState* oldState = oldState = deltaFrame->Answer->EntitiesStateList;
	while( oldState )
	{
		if( oldState->UniqueNumber == UniqueNumber )
			break;
		oldState = oldState->Next;
	}

	Assert( oldState );

	unsigned char flags = message->ReadUChar();

	if( flags & NetworkObjectState::NET_CREATEPO )
	{
		int poType = message->ReadUChar();
		switch( poType )
		{
		case NetworkObjectState::PO_Missile:
			InterpretCreateMissilePO( message, peer, state, false, true );
			break;
		case NetworkObjectState::PO_Player:
			{
				mObject->CreatePhysicsObject( EPawnBodyPlayer, 1.0f, -1, false );
				char* synStr = mObject->GetSynchroString();
				if( synStr )
					GScript.QueueMessage("PO_CREATE","%e,%p,%s",mObject,mObject->PO,synStr);

                m_setTag("Net");
				state->Player = new PlayerState;
				state->Player->Read( message );
				state->Player->SetIntoPlayer( mObject->PO );
#ifndef LOCAL_PLAYER_INPUT
                m_setTag("Net");
				state->Input = new PlayerInput;
				state->Input->ReadNoDelta( message );
				state->Input->SetIntoPlayer( mObject->PO );
#else
				PlayerInput input;
				input.ReadNoDelta( message );
				input.SetIntoPlayer( mObject->PO );
#endif
			}
			break;
		case NetworkObjectState::PO_Normal:
			InterpretCreateNormalPO( message, peer, state, false, true );
			break;
		default:
			Assert( false );
			break;
		}
	}
	else
	{
		if( ( flags & NetworkObjectState::NET_DELETEPO ) && mObject->PO )
			mObject->RemovePhysicsObject();

		if( mObject->PO )
		{
			if( mObject->PO->mMissileController )
			{
#ifndef NO_MISSILE_UPDATES
                m_setTag("Net");
				state->Missile = new MissileState;
				if( flags & NetworkObjectState::NET_PODATA )
					state->Missile->Read( message );
				else
				{
					Assert( oldState->Missile );
					*state->Missile = *oldState->Missile;
				}
				state->Missile->SetInto( mObject->PO->mMissileController );
#endif
			}
			else if( mObject->PO->mPlayerVariables )
			{
                m_setTag("Net");
				state->Player = new PlayerState;
				if( flags & NetworkObjectState::NET_POINPUT )
				{
					Assert( oldState->Player );
					state->Player->ReadAbridged( message, *oldState->Player );
					state->Player->SetIntoPlayerAbridged( mObject->PO );
#ifndef LOCAL_PLAYER_INPUT
                    m_setTag("Net");
					state->Input = new PlayerInput;
					state->Input->ReadYawPitch( message );
					state->Input->SetIntoPlayer( mObject->PO );
#else
					PlayerInput input;
					input.ReadYawPitch( message );
					input.SetIntoPlayer( mObject->PO );
#endif
#ifndef TRANSGAMING_NO_HAVOK
					mObject->PO->MultiPlayerActionSetupPredictionTime( targetFrame->SentTime );
#endif
				}
				else
				{
					Assert( oldState->Player );
					state->Player->ReadDiff( message, *oldState->Player, ( peer->mLocalPlayerOptimizedEveryNFrames != 0 ) );
					Assert( targetFrame->Answer->LocalPlayerEntity == mObject );

/*
#ifdef PLAYER_COMPARE
					if( state->Player->ExternalInfluences )
					{
						if( targetFrame->PlayerData == *state->Player )
							GLog.Print( "###4### Diff: player data identical, and shouldn't be for frame: %d\n", targetFrame->Number );
					}
#endif
*/
					if( peer->mLocalPlayerOptimizedEveryNFrames && !state->Player->ExternalInfluences )
					{
						*state->Player = targetFrame->PlayerData;
						targetFrame->Answer->PredictAgainLocalPlayer = false;
					}
					else if( targetFrame->PlayerData == *state->Player )
					{
						targetFrame->Answer->PredictAgainLocalPlayer = false;		// Optimalization; no prediction recalculation if the answer for local player same as the local state
						state->Player->SetMPByte( mObject->PO );
					}
					else
					{
						Assert( targetFrame->Answer->PredictAgainLocalPlayer == true );
						state->Player->SetIntoPlayer( mObject->PO );
#ifdef PLAYER_COMPARE
						if( !state->Player->ExternalInfluences )
						{
							if( targetFrame->PlayerData.CompareWithPlayer( mObject->PO ) )
								GLog.Print( "###5### Diff: player data non-identical, and should be for frame: %d\n", targetFrame->Number );
						}
#endif
					}
				}
			}
			else
			{
				if( flags & NetworkObjectState::NET_PODATA )
					InterpretUpdateNormalPO( message, state, oldState );
				else
					CopyUpdateNormalPO( state, oldState );
			}
		}
		else
		{
			flags &= NetworkObjectState::NET_POSITION|NetworkObjectState::NET_ORIENTATION;

			if( flags & NetworkObjectState::NET_POSITION )
				state->Position = message->ReadPositionOptimized();
			if( flags & NetworkObjectState::NET_ORIENTATION )
				state->Orientation = message->ReadQuaternionOptimized();

			if( mObject )
			{
				switch( flags )
				{
				case 0:	// none
				default:
					break;
				case NetworkObjectState::NET_POSITION:
					mObject->SetPosition( state->Position );
					break;
				case NetworkObjectState::NET_ORIENTATION:
					mObject->SetRotation( state->Orientation );
					break;
				case (NetworkObjectState::NET_POSITION|NetworkObjectState::NET_ORIENTATION):
					mObject->SetPositionAndRotation( state->Position, state->Orientation );
					break;
				}
			}
		}
	}
	message->ReadCheckToken( MODEL_UPDATE );

	// Add state to frame
	state->Next = targetFrame->Answer->EntitiesStateList;
	targetFrame->Answer->EntitiesStateList = state;

	return true;
}

void ModelSynchronizer2::SendAnimationInfo( UdpMessage* message, NetworkPeer2* peer )
{
	Model* model = (Model*)mObject;
	Assert( model );

	// Write animations state for entity
	message->WriteUChar( (unsigned char)model->Mesh->Animations.Size() );

	for( int i = 1; i < model->Mesh->Animations.Size(); ++i )
	{
		if( model->Mesh->Animations[i] )
		{
			if( model->Mesh->Animations[i]->Name.IsNull() )
				GLog.Print( "Netcode bug 1! Check it.\n" );
			peer->StringTransport.WriteString( message, model->Mesh->Animations[i]->Name );
//			message->WriteString( model->Mesh->Animations[i]->Name.AsChar() );
			message->WriteBool( model->Mesh->Animations[i]->Looped );
			message->WriteBool( ((SkeletalAnimation*)(model->Mesh->Animations[i]))->HasMovementCurve()!=0 );
		}
		else
		{
			String empty = "";
			peer->StringTransport.WriteString( message, empty );
//			message->WriteString( "" );
		}
	}

	// Synchronize animation controller
	message->WriteUChar( (unsigned char)model->AnimController.Animations.Size() );
	for( int i = 0; i < model->AnimController.Animations.Size(); ++i )
	{
		int index = -1;
		for( int j = 0; j < model->Mesh->Animations.Size(); ++j )
		{
			if( model->Mesh->Animations[j] == model->AnimController.Animations[i].Animation )
			{
				index = j;
				break;
			}
		}
		Assert( index != -1 );
		message->WriteUChar( (unsigned char)index );
		message->WriteFloat( model->AnimController.Animations[i].Weight );
		message->WriteFloat( model->AnimController.Animations[i].TimeScale );
		message->WriteBool( model->AnimController.Animations[i].Looped );
		message->WriteFloat( model->AnimController.Animations[i].FrameBlendTime );
		message->WriteFloat( model->AnimController.Animations[i].BlendTime );
	}
}

void ModelSynchronizer2::ApplyAnimationInfo( UdpMessage* message, NetworkPeer2* peer )
{
	// Read animations state for entity and load animations
	Model* model = (Model*)mObject;
	Assert( model );
	unsigned char animSize = message->ReadUChar();
	for( int i = 1; i < animSize; ++i )
	{
		String animName = peer->StringTransport.ReadString( message );
		if( !(animName.IsNull() ) )
		{
			bool looped = message->ReadBool();
			bool setMovementCurve = message->ReadBool();
			int index = model->LoadAnimation( animName.AsChar(), looped , i );
			if( setMovementCurve )
				model->SetAnimationMovementCurve( index, "ROOOT", ModelAnimation::ETransZ );
		}
	}

	// Synchronize animation controller
	model->AnimController.Animations.ClearArray();
	int activeAnimSize = message->ReadUChar();
	for( int i = 0; i < activeAnimSize; ++i )
	{
		model->AnimController.Animations.GrowArray(1);
		model->AnimController.Animations[i].Animation = model->Mesh->Animations[ message->ReadUChar() ];
		model->AnimController.Animations[i].Weight = message->ReadFloat();
		model->AnimController.Animations[i].TimeScale = message->ReadFloat();
		model->AnimController.Animations[i].Looped = message->ReadBool();
		model->AnimController.Animations[i].FrameBlendTime = message->ReadFloat();
		model->AnimController.Animations[i].BlendTime = message->ReadFloat();
	}
}

void ModelSynchronizer2::DumpAnimationInfo( UdpMessage* message, NetworkPeer2* peer )
{
	// Read animations state for entity and load animations
	unsigned char animSize = message->ReadUChar();
	for( int i = 1; i < animSize; ++i )
	{
		String animName = peer->StringTransport.ReadString( message );
		if( !animName.IsNull() )
		{
			message->ReadBool();
			message->ReadBool();
		}
	}

	int activeAnimSize = message->ReadUChar();
	for( int i = 0; i < activeAnimSize; ++i )
	{
		message->ReadUChar();
		message->ReadFloat();
		message->ReadFloat();
		message->ReadBool();
		message->ReadFloat();
		message->ReadFloat();
	}
}

// =========================================================================================================

void PlayerState::GetFromPlayer( PhysicsObject* po )
{
#ifndef TRANSGAMING_NO_HAVOK
// Blair - We don't currently support Havok or its MissileController on the PSP.
	Assert( po && po->mPlayerVariables && po->mPlayerVariables->Controller );
	HavokCharacterController* c = po->mPlayerVariables->Controller;

	VelocityX			= c->m_velocityX;
	VelocityY			= c->m_velocityY;
	VelocityZ			= c->m_velocityZ;
	LastHeightOfStep	= c->m_state.lastHeightOfStep;
	Position.X			= c->m_state.position(0);
	Position.Y			= c->m_state.position(1);
	Position.Z			= c->m_state.position(2);
	Speed				= c->m_speed;
	MsSinceLanded		= c->m_msSinceLanded;
	MsSinceJumped		= c->m_msSinceJumped;
	MsSinceExplosion	= c->m_msSinceExplosion;

	DWORD c_jumpAction = c->m_jumpAction;
	c_jumpAction &= ((1<<EInputJump)|(1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));
	Flags1 = c_jumpAction >> 1;
	DWORD c_lastNonzeroAction = c->m_lastNonzeroAction;
	c_lastNonzeroAction &= ((1<<EInputJump)|(1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));
	Flags2 = c_lastNonzeroAction >> 1;

	if( c->m_onGround )
		Flags1 |= OnGround;
	else
		Flags1 &= ~OnGround;

	if( c->m_hasJumpVector )
		Flags1 |= HasJumpVector;
	else
		Flags1 &= ~HasJumpVector;

	if( c->m_canBunnyHop )
		Flags1 |= CanBunnyHop;
	else
		Flags1 &= ~CanBunnyHop;

	if( c->m_falling )
		Flags2 |= Falling;
	else
		Flags2 &= ~Falling;

	if( c->m_jumpInvalidation )
		Flags2 |= JumpInvalidation;
	else
		Flags2 &= ~JumpInvalidation;

	ExternalInfluences = c->m_externalInfluences;
	MPByte = c->MPByte;
#endif // TRANSGAMING_NO_HAVOK
}

void PlayerState::SetIntoPlayer( PhysicsObject* po )
{
#ifndef TRANSGAMING_NO_HAVOK
	Assert( po && po->mPlayerVariables && po->mPlayerVariables->Controller );
	HavokCharacterController* c = po->mPlayerVariables->Controller;

	c->m_velocityX				= VelocityX;
	c->m_velocityY				= VelocityY;
	c->m_velocityZ				= VelocityZ;
	c->m_state.lastHeightOfStep	= LastHeightOfStep;
	c->m_state.position(0)		= Position.X;
	c->m_state.position(1)		= Position.Y;
	c->m_state.position(2)		= Position.Z;
	c->m_speed					= Speed;
	c->m_msSinceLanded			= MsSinceLanded;
	c->m_msSinceJumped			= MsSinceJumped;
	c->m_msSinceExplosion		= MsSinceExplosion;

	c->m_jumpAction = Flags1;
	c->m_jumpAction <<= 1;
	c->m_jumpAction &= ((1<<EInputJump)|(1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));
	c->m_lastNonzeroAction = Flags2;
	c->m_lastNonzeroAction <<= 1;
	c->m_lastNonzeroAction &= ((1<<EInputJump)|(1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));

	c->m_onGround			= ( ( Flags1 & OnGround ) != 0 );
	c->m_hasJumpVector		= ( ( Flags1 & HasJumpVector ) != 0 );
	c->m_canBunnyHop		= ( ( Flags1 & CanBunnyHop ) != 0 );
	c->m_falling			= ( ( Flags2 & Falling ) != 0 );
	c->m_jumpInvalidation	= ( ( Flags2 & JumpInvalidation ) != 0 );
	c->MPByte				= MPByte;
#endif // TRANSGAMING_NO_HAVOK
}

void PlayerState::SetIntoPlayerAbridged( PhysicsObject* po )
{
#ifndef TRANSGAMING_NO_HAVOK
	Assert( po && po->mPlayerVariables && po->mPlayerVariables->Controller );
	HavokCharacterController* c = po->mPlayerVariables->Controller;

	c->m_velocityX				= VelocityX;
	c->m_velocityY				= VelocityY;
	c->m_velocityZ				= VelocityZ;
	c->m_state.position(0)		= Position.X;
	c->m_state.position(1)		= Position.Y;
	c->m_state.position(2)		= Position.Z;
	c->MPByte					= MPByte;
#endif // TRANSGAMING_NO_HAVOK
}

bool PlayerState::CompareWithPlayer( PhysicsObject* po )
{
#ifndef TRANSGAMING_NO_HAVOK
	Assert( po && po->mPlayerVariables && po->mPlayerVariables->Controller );
	HavokCharacterController* c = po->mPlayerVariables->Controller;

	bool writtenText = false;

	if( c->m_velocityX != VelocityX )
	{
		GLog.Print( "Velocity X sent: %d, received: %d\n", VelocityX, c->m_velocityX );
		writtenText = true;
	}

	if( c->m_velocityY != VelocityY )
	{
		GLog.Print( "Velocity Y sent: %d, received: %d\n", VelocityY, c->m_velocityY );
		writtenText = true;
	}

	if( c->m_velocityZ != VelocityZ )
	{
		GLog.Print( "Velocity Z sent: %d, received: %d\n", VelocityZ, c->m_velocityZ );
		writtenText = true;
	}

	if( c->m_state.lastHeightOfStep != LastHeightOfStep )
	{
		GLog.Print( "Last height of step sent: %d, received: %d\n", LastHeightOfStep, c->m_state.lastHeightOfStep );
		writtenText = true;
	}

	if( ( c->m_state.position(0) != Position.X ) ||
		( c->m_state.position(1) != Position.Y ) ||
		( c->m_state.position(2) != Position.Z ) )
	{
		GLog.Print( "Position sent: (%f,%f,%f), received: (%f,%f,%f), diff: (%f,%f,%f)\n",
			Position.X, Position.Y, Position.Z,
			c->m_state.position(0), c->m_state.position(1), c->m_state.position(2),
			Position.X - c->m_state.position(0), Position.Y - c->m_state.position(1), Position.Z - c->m_state.position(2) );
		writtenText = true;
	}

	if( c->m_speed != Speed )
	{
		GLog.Print( "Speed sent: %d, received: %d\n", Speed, c->m_speed );
		writtenText = true;
	}

	if( c->m_msSinceLanded != MsSinceLanded )
	{
		GLog.Print( "Since landed sent: %d, received: %d\n", MsSinceLanded, c->m_msSinceLanded );
		writtenText = true;
	}

	if( c->m_msSinceJumped != MsSinceJumped )
	{
		GLog.Print( "Since jumped sent: %d, received: %d\n", MsSinceJumped, c->m_msSinceJumped );
		writtenText = true;
	}

	if( c->m_msSinceExplosion != MsSinceExplosion )
	{
		GLog.Print( "Since explosion sent: %d, received: %d\n", MsSinceExplosion, c->m_msSinceExplosion );
		writtenText = true;
	}

	bool onGround = ( ( Flags1 & OnGround ) != 0 );
	if( c->m_onGround != onGround )
	{
		GLog.Print( "On ground sent: %s, received: %s\n", onGround?"true":"false", c->m_onGround?"true":"false" );
		writtenText = true;
	}

	bool hasJumpVector = ( ( Flags1 & HasJumpVector ) != 0 );
	if( c->m_hasJumpVector != hasJumpVector )
	{
		GLog.Print( "Has jump vector sent: %s, received: %s\n", hasJumpVector?"true":"false", c->m_hasJumpVector?"true":"false" );
		writtenText = true;
	}

	bool canBunnyHop = ( ( Flags1 & CanBunnyHop ) != 0 );
	if( c->m_canBunnyHop != canBunnyHop )
	{
		GLog.Print( "Can bunny hop sent: %s, received: %s\n", canBunnyHop?"true":"false", c->m_canBunnyHop?"true":"false" );
		writtenText = true;
	}

	bool falling = ( ( Flags2 & Falling ) != 0 );
	if( c->m_falling != falling )
	{
		GLog.Print( "Falling sent: %s, receiving: %s\n", falling?"true":"false", c->m_falling?"true":"false" );
		writtenText = true;
	}

	bool jumpInvalidation = ( ( Flags2 & JumpInvalidation ) != 0 );
	if( c->m_jumpInvalidation != jumpInvalidation )
	{
		GLog.Print( "Jump invalidation sent: %s, received: %s\n", jumpInvalidation?"true":"false", c->m_jumpInvalidation?"true":"false" );
		writtenText = true;
	}

	DWORD jumpAction = Flags1 << 1;
	DWORD c_jumpAction = c->m_jumpAction;
	jumpAction &= ((1<<EInputJump)|(1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));
	c_jumpAction &= ((1<<EInputJump)|(1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));
	if( c_jumpAction != jumpAction )
	{
		GLog.Print( "Jump action sent: %d, received: %d\n", jumpAction, c_jumpAction );
		writtenText = true;
	}

	DWORD lastNonzeroAction = Flags2 << 1;
	DWORD c_lastNonzeroAction = c->m_lastNonzeroAction;
	lastNonzeroAction &= ((1<<EInputJump)|(1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));
	c_lastNonzeroAction &= ((1<<EInputJump)|(1<<EInputMoveForward)|(1<<EInputMoveBackward)|(1<<EInputMoveLeft)|(1<<EInputMoveRight));
	if( c_lastNonzeroAction != lastNonzeroAction )
	{
		GLog.Print( "Last nonzero action sent: %d, received: %d\n", lastNonzeroAction, c_lastNonzeroAction );
		writtenText = true;
	}

	return writtenText;

#else // TRANSGAMING_NO_HAVOK
	 return false;
#endif // TRANSGAMING_NO_HAVOK
}

bool PlayerState::operator==( const PlayerState& other ) const
{
	return(
		( VelocityX == other.VelocityX ) &&
		( VelocityY == other.VelocityY ) &&
		( VelocityZ == other.VelocityZ ) &&
		( LastHeightOfStep == other.LastHeightOfStep ) &&
		( Position == other.Position ) &&
		( Speed == other.Speed ) &&
		( MsSinceLanded == other.MsSinceLanded ) &&
		( MsSinceJumped == other.MsSinceJumped ) &&
		( MsSinceExplosion == other.MsSinceExplosion ) &&
		( Flags1 == other.Flags1 ) &&
		( Flags2 == other.Flags2 )
	);
}

void PlayerState::Print( LogBuffer& logBuffer )
{
	logBuffer.Print( "Velocity: (%d,%d,%d)\n", VelocityX, VelocityY, VelocityZ );
	logBuffer.Print( "LastHeightOfStep: %u\n", LastHeightOfStep );
	logBuffer.Print( "Position: (%f,%f,%f)\n", Position.X, Position.Y, Position.Z );
	logBuffer.Print( "Speed: %u\n", Speed );
	logBuffer.Print( "Ms since: landed: %u, jumped: %u, explo: %u\n", MsSinceLanded, MsSinceJumped, MsSinceExplosion );
	logBuffer.Print( "Flags 1: %u, 2: %u\n", Flags1, Flags2 );
}

void PlayerState::WriteAbridged( UdpMessage* mesg, PlayerState& previous ) const
{
	unsigned char flags = 0;
	int pos = mesg->GetPos();
	mesg->WriteUChar(0);	// placeholder

	if( VelocityX != previous.VelocityX )
	{
		flags |= PF_VELOCITY_X;
		mesg->WriteShort( VelocityX );
	}

	if( VelocityY != previous.VelocityY )
	{
		flags |= PF_VELOCITY_Y;
		mesg->WriteShort( VelocityY );
	}

	if( VelocityZ != previous.VelocityZ )
	{
		flags |= PF_VELOCITY_Z;
		mesg->WriteShort( VelocityZ );
	}

	if( Position.X != previous.Position.X )
	{
		flags |= PF_POSITION_X;
		mesg->WriteFloat( Position.X );
	}

	if( Position.Y != previous.Position.Y )
	{
		flags |= PF_POSITION_Y;
		mesg->WriteFloat( Position.Y );
	}

	if( Position.Z != previous.Position.Z )
	{
		flags |= PF_POSITION_Z;
		mesg->WriteFloat( Position.Z );
	}

	if( MPByte != previous.MPByte )
	{
		flags |= PF_MPBYTE;
		mesg->WriteUChar( MPByte );
	}

	int pos2 = mesg->GetPos();
	mesg->Seek( pos );
	mesg->WriteUChar( flags );
	mesg->Seek( pos2 );
}

void PlayerState::SetMPByte( PhysicsObject* po )
{
#ifndef TRANSGAMING_NO_HAVOK
	Assert( po && po->mPlayerVariables && po->mPlayerVariables->Controller );
	po->mPlayerVariables->Controller->MPByte = MPByte;
#endif
}

// =========================================================================================================

void PlayerInput::SetIntoPlayer( PhysicsObject* po )
{
#ifndef TRANSGAMING_NO_HAVOK
	Assert( po && po->mPlayerVariables && po->mPlayerVariables->Controller );
	po->mPlayerVariables->Controller->LastMove = *this;
	po->Action = Action;
#endif
}

void PlayerInput::GetFromPlayer( PhysicsObject* po )
{
#ifndef TRANSGAMING_NO_HAVOK
	Assert( po && po->mPlayerVariables && po->mPlayerVariables->Controller );
	*this = po->mPlayerVariables->Controller->LastMove;
#endif
}

void PlayerInput::TestDelta( PlayerInput* previous )
{
	UdpMessage mesg;
	WriteDelta( &mesg, previous );
	PlayerInput i2;
	mesg.EndWriting();
	mesg.SeekToBegin();
	i2.ReadDelta( &mesg, previous );
	if( ( TimeDelta != i2.TimeDelta ) ||
		( Action != i2.Action ) ||
		( Yaw != i2.Yaw ) ||
		( Pitch != i2.Pitch ) )
		GLog.Print( "PlayerInput failed delta send test!!!\n" );
}

void PlayerInput::Print( LogBuffer& logBuffer )
{
	logBuffer.Print( "TD: %u, A: %u, Y: %d, P: %d\n", TimeDelta, Action, Yaw, Pitch );
}

// =========================================================================================================

void MissileState::Write( UdpMessage* mesg )
{
	mesg->WriteFloat( Position.X );
	mesg->WriteFloat( Position.Y );
	mesg->WriteFloat( Position.Z );
	mesg->WriteShort( VelocityX );
	mesg->WriteShort( VelocityY );
	mesg->WriteShort( VelocityZ );
#ifndef TRANSGAMING_NO_HAVOK
	if( AngularVelocity.MagnitudeSquared() > 0 )
	{
		unsigned char prop = Properties | HavokMissileController::MIPR_HasAngVel;
		mesg->WriteUChar( prop );
		mesg->WriteAngularVelocityOptimized( AngularVelocity );
	}
	else
#endif
		mesg->WriteUChar( Properties );

#ifndef TRANSGAMING_NO_HAVOK
	if( Properties & HavokMissileController::MIPR_StakeGravity )
		mesg->WriteUShort( TimeSinceCreation );
#endif
}

void MissileState::Read( UdpMessage* mesg )
{
	Position.X = mesg->ReadFloat();
	Position.Y = mesg->ReadFloat();
	Position.Z = mesg->ReadFloat();
	VelocityX = mesg->ReadShort();
	VelocityY = mesg->ReadShort();
	VelocityZ = mesg->ReadShort();
	Properties = mesg->ReadUChar();

#ifndef TRANSGAMING_NO_HAVOK
	if( Properties & HavokMissileController::MIPR_HasAngVel )
	{
		Properties &= ~HavokMissileController::MIPR_HasAngVel;
		AngularVelocity = mesg->ReadAngularVelocityOptimized();
	}

	if( Properties & HavokMissileController::MIPR_StakeGravity )
		TimeSinceCreation = mesg->ReadUShort();
#endif
}

void MissileState::WriteNoPos( UdpMessage* mesg )
{
	mesg->WriteShort( VelocityX );
	mesg->WriteShort( VelocityY );
	mesg->WriteShort( VelocityZ );

#ifndef TRANSGAMING_NO_HAVOK
	if( AngularVelocity.MagnitudeSquared() > 0 )
	{
		unsigned char prop = Properties | HavokMissileController::MIPR_HasAngVel;
		mesg->WriteUChar( prop );
		mesg->WriteAngularVelocityOptimized( AngularVelocity );
	}
	else
#endif
		mesg->WriteUChar( Properties );

#ifndef TRANSGAMING_NO_HAVOK
	if( Properties & HavokMissileController::MIPR_StakeGravity )
		mesg->WriteUShort( TimeSinceCreation );
#endif
}

void MissileState::ReadNoPos( UdpMessage* mesg )
{
	VelocityX = mesg->ReadShort();
	VelocityY = mesg->ReadShort();
	VelocityZ = mesg->ReadShort();
	Properties = mesg->ReadUChar();

#ifndef TRANSGAMING_NO_HAVOK
	if( Properties & HavokMissileController::MIPR_HasAngVel )
	{
		Properties &= ~HavokMissileController::MIPR_HasAngVel;
		AngularVelocity = mesg->ReadAngularVelocityOptimized();
	}

	if( Properties & HavokMissileController::MIPR_StakeGravity )
		TimeSinceCreation = mesg->ReadUShort();
#endif
}

#ifndef TRANSGAMING_NO_HAVOK
void MissileState::SetInto( HavokMissileController* mc )
{
	mc->m_state.position(0) = Position.X;
	mc->m_state.position(1) = Position.Y;
	mc->m_state.position(2) = Position.Z;
	mc->m_state.velocityX = VelocityX;
	mc->m_state.velocityY = VelocityY;
	mc->m_state.velocityZ = VelocityZ;
	mc->m_state.properties = Properties;
	if( Properties & HavokMissileController::MIPR_StakeGravity )
		mc->m_state.timeSinceCreation = TimeSinceCreation;
	if( Properties & HavokMissileController::MIPR_HeaterBomb )
		mc->m_restitution = 1.5f;
	if( Properties & HavokMissileController::MIPR_Soul )
		mc->m_restitution = 0;
	if( AngularVelocity.MagnitudeSquared() > 0 )
		mc->setAngularVelocity( AngularVelocity );
}

void MissileState::GetFrom( HavokMissileController* mc )
{
	Position.X = mc->m_state.position(0);
	Position.Y = mc->m_state.position(1);
	Position.Z = mc->m_state.position(2);
	VelocityX = mc->m_state.velocityX;
	VelocityY = mc->m_state.velocityY;
	VelocityZ = mc->m_state.velocityZ;
	Properties = mc->m_state.properties;
	if( Properties & HavokMissileController::MIPR_StakeGravity )
		TimeSinceCreation = mc->m_state.timeSinceCreation;
	AngularVelocity = mc->getAngularVelocity();
}
#endif // TRANSGAMING_NO_HAVOK

void MissileState::Print( LogBuffer& logBuffer )
{
	logBuffer.Print( "Pos (%f,%f,%f), vel: (%d,%d,%d)\n", Position.X, Position.Y, Position.Z, VelocityX, VelocityY, VelocityZ );
}

// =========================================================================================================

void PONetCreationEntry2::SendEntry( UdpMessage* message )
{
	message->WriteUShort( ObjectState );
	message->WriteFloat( Mass );
	message->WriteFloat( Friction );
	message->WriteFloat( Restitution );
	message->WriteFloat( Damping );
	message->WriteFloat( BodyScale );
	message->WriteUChar( BodyType );
	message->WriteChar( FreedomOfRotation );
	unsigned char t = CollisionGroup;
	if( HasGravity ) t |= 128;
	if( Pinned ) t |= 64;
	message->WriteUChar( t );
}

void PONetCreationEntry2::ReceiveEntry( UdpMessage* message )
{
	ObjectState = message->ReadUShort();
	Mass = message->ReadFloat();
	Friction = message->ReadFloat();
	Restitution = message->ReadFloat();
	Damping = message->ReadFloat();
	BodyScale = message->ReadFloat();
	BodyType = message->ReadUChar();
	FreedomOfRotation = message->ReadChar();
	unsigned char t = message->ReadUChar();
	CollisionGroup = t & 63;
	HasGravity = ( ( t & 128 ) != 0 );
	Pinned = ( ( t & 64 ) != 0 );
}

// =========================================================================================================

void PONetCreationCache2::SendPOCreationData( UdpMessage* message, PONetCreationEntry2& entry )
{
	for( int i = 0; i < Entries.Size(); ++i )
		if( Entries[i] == entry )
		{
			message->WriteUShort( Entries[i].index );
			if( i >= mFirstUnsureNumberToSend )
				entry.SendEntry(message);
			Entries[i].timesSent++;
			return;
		}
	int ind = Entries.Add(entry);
	Entries[ind].index = ind;
	Entries[ind].timesSent = 1;
	message->WriteUShort( ind );
	entry.SendEntry(message);
}

void PONetCreationCache2::ReceivePOCreationData( UdpMessage* message, PONetCreationEntry2& entry )
{
	int index = message->ReadUShort();
	Assert( !(index>Entries.Size()) && (index>=0) );
	if( index == Entries.Size() )
	{
		entry.ReceiveEntry( message );
		Entries.Add(entry);
	}
	else
	{
		if( index >= mFirstUnsureNumberToReceive )
		{
			PONetCreationEntry2 a;
			a.ReceiveEntry( message );
			Assert( a == Entries[index] );
		}
		entry = Entries[index];
	}
}
