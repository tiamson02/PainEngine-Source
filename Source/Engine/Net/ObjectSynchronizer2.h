
// #pragma once
#ifndef _OBJECTSYNCHRONIZER2_
#define _OBJECTSYNCHRONIZER2_



#include "Core/Math/LeMath.h"
#include "Net/UdpMessage.h"

#ifndef FINAL_BUILD
#define PLAYER_COMPARE
#endif

class UdpMessage;
class NetworkDevice2;
class Entity;
class WorldMesh;
class Model;
class PhysicsObject;
class LogBuffer;

#ifndef TRANSGAMING_NO_HAVOK
// Blair - No Havok support for the PSP yet.
class HavokMissileController;
#endif

#define NO_MISSILE_UPDATES
#define LOCAL_PLAYER_INPUT
#define NO_LOCAL_PLAYER_UPDATES

// =========================================================================================================

class PlayerState
{
private:

	enum Flags
	{
		// JumpAction is first byte first 5 bits
		OnGround			= (1<<5),
		HasJumpVector		= (1<<6),
		CanBunnyHop			= (1<<7),
		// LastNonzeroAction is second byte first 5 bits
		Falling				= (1<<5),
		JumpInvalidation	= (1<<6),
		MPByteChanged		= (1<<7)
	};

	short			VelocityX;
	short			VelocityY;
	short			VelocityZ;
	unsigned char	LastHeightOfStep;
	Vector			Position;
	unsigned short	Speed;
	unsigned char	MsSinceLanded;
	unsigned char	MsSinceJumped;
	unsigned short	MsSinceExplosion;
	unsigned char	Flags1;
	unsigned char	Flags2;

	unsigned char	MPByte;

	enum
	{
		PF_NOINFLUENCE	= (1<<0),
		PF_VELOCITY_X	= (1<<0),
		PF_VELOCITY_Y	= (1<<1),
		PF_VELOCITY_XZ	= (1<<2),
		PF_VELOCITY_Z	= (1<<2),
		PF_LAST_HOS		= (1<<3),
		PF_POSITION_X	= (1<<4),
		PF_POSITION_Y	= (1<<5),
		PF_POSITION_Z	= (1<<6),
		PF_SPEED		= (1<<7),
		PF_MPBYTE		= (1<<7),
		// MsSinceJumped, MsSinceLanded, and MsSinceExplosion are always transferred
		// Flags too
	};

public:
	DWORD			ExternalInfluences;

	void Write( UdpMessage* mesg ) const
	{
		mesg->WriteShort( VelocityX );
		mesg->WriteShort( VelocityY );
		mesg->WriteShort( VelocityZ );
		mesg->WriteUChar( LastHeightOfStep );
		mesg->WriteFloat( Position.X );
		mesg->WriteFloat( Position.Y );
		mesg->WriteFloat( Position.Z );
		mesg->WriteUShort( Speed );
		mesg->WriteUChar( MsSinceLanded );
		mesg->WriteUChar( MsSinceJumped );
		mesg->WriteUShort( MsSinceExplosion );
		mesg->WriteUChar( Flags1 );
		mesg->WriteUChar( Flags2 );
		mesg->WriteUChar( MPByte );
	}

	void WriteAbridged( UdpMessage* mesg, PlayerState& previous ) const;

	void WriteDiff( UdpMessage* mesg, PlayerState& previous, bool optimized )
	{
		Flags2 &= ~MPByteChanged;	// the only reason why this is not const

		unsigned char flags = 0;

		if( ExternalInfluences == previous.ExternalInfluences )
		{
			flags |= PF_NOINFLUENCE;
			if( optimized )
			{
				mesg->WriteUChar( flags );
				return;
			}
		}

		int pos = mesg->GetPos();
		mesg->WriteUChar(0);	// placeholder

		if( VelocityY != previous.VelocityY )
		{
			flags |= PF_VELOCITY_Y;
			mesg->WriteShort( VelocityY );
		}

		if( ( VelocityX != previous.VelocityX ) || ( VelocityZ != previous.VelocityZ ) )
		{
			flags |= PF_VELOCITY_XZ;
			mesg->WriteShort( VelocityX );
			mesg->WriteShort( VelocityZ );
		}

		if( LastHeightOfStep != previous.LastHeightOfStep )
		{
			flags |= PF_LAST_HOS;
			mesg->WriteUChar( LastHeightOfStep );
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

		if( Speed != previous.Speed )
		{
			flags |= PF_SPEED;
			mesg->WriteUShort( Speed );
		}

		mesg->WriteUChar( MsSinceLanded );
		mesg->WriteUChar( MsSinceJumped );
		mesg->WriteUShort( MsSinceExplosion );
		mesg->WriteUChar( Flags1 );

		if( MPByte != previous.MPByte )
		{
			mesg->WriteUChar( Flags2 | MPByteChanged );
			mesg->WriteUChar( MPByte );
		}
		else
			mesg->WriteUChar( Flags2 );

		int pos2 = mesg->GetPos();
		mesg->Seek( pos );
		mesg->WriteUChar( flags );
		mesg->Seek( pos2 );
	}

	void ReadDiff( UdpMessage* mesg, PlayerState& previous, bool optimized )
	{
		unsigned char flags = mesg->ReadUChar();

		if( optimized && ( flags & PF_NOINFLUENCE ) )
		{
			ExternalInfluences = 0;
			return;
		}

		VelocityY = ( flags & PF_VELOCITY_Y )?mesg->ReadShort():previous.VelocityY;
		VelocityX = ( flags & PF_VELOCITY_XZ )?mesg->ReadShort():previous.VelocityX;
		VelocityZ = ( flags & PF_VELOCITY_XZ )?mesg->ReadShort():previous.VelocityZ;
		LastHeightOfStep = ( flags & PF_LAST_HOS )?mesg->ReadUChar():previous.LastHeightOfStep;
		Position.X = ( flags & PF_POSITION_X )?mesg->ReadFloat():previous.Position.X;
		Position.Y = ( flags & PF_POSITION_Y )?mesg->ReadFloat():previous.Position.Y;
		Position.Z = ( flags & PF_POSITION_Z )?mesg->ReadFloat():previous.Position.Z;
		Speed = ( flags & PF_SPEED )?mesg->ReadUShort():previous.Speed;
		MsSinceLanded = mesg->ReadUChar();
		MsSinceJumped = mesg->ReadUChar();
		MsSinceExplosion = mesg->ReadUShort();
		Flags1 = mesg->ReadUChar();
		Flags2 = mesg->ReadUChar();
		if( Flags2 & MPByteChanged )
		{
			MPByte = mesg->ReadUChar();
			Flags2 &= ~MPByteChanged;
		}
		else
			MPByte = previous.MPByte;
		ExternalInfluences = ( flags & PF_NOINFLUENCE )?0:1;
	}

	void Read( UdpMessage* mesg )
	{
		VelocityX			= mesg->ReadShort();
		VelocityY			= mesg->ReadShort();
		VelocityZ			= mesg->ReadShort();
		LastHeightOfStep	= mesg->ReadUChar();
		Position.X			= mesg->ReadFloat();
		Position.Y			= mesg->ReadFloat();
		Position.Z			= mesg->ReadFloat();
		Speed				= mesg->ReadUShort();
		MsSinceLanded		= mesg->ReadUChar();
		MsSinceJumped		= mesg->ReadUChar();
		MsSinceExplosion	= mesg->ReadUShort();
		Flags1				= mesg->ReadUChar();
		Flags2				= mesg->ReadUChar();
		MPByte				= mesg->ReadUChar();
	}

	void ReadAbridged( UdpMessage* mesg, PlayerState& previous )
	{
		unsigned char flags = mesg->ReadUChar();
		VelocityX = ( flags & PF_VELOCITY_X )?mesg->ReadShort():previous.VelocityX;
		VelocityY = ( flags & PF_VELOCITY_Y )?mesg->ReadShort():previous.VelocityY;
		VelocityZ = ( flags & PF_VELOCITY_Z )?mesg->ReadShort():previous.VelocityZ;
		Position.X = ( flags & PF_POSITION_X )?mesg->ReadFloat():previous.Position.X;
		Position.Y = ( flags & PF_POSITION_Y )?mesg->ReadFloat():previous.Position.Y;
		Position.Z = ( flags & PF_POSITION_Z )?mesg->ReadFloat():previous.Position.Z;
		MPByte = ( flags & PF_MPBYTE )?mesg->ReadUChar():previous.MPByte;
	}

	void GetFromPlayer( PhysicsObject* po );
	void SetIntoPlayer( PhysicsObject* po );
	void SetIntoPlayerAbridged( PhysicsObject* po );
	bool CompareWithPlayer( PhysicsObject* po );
	bool operator==( const PlayerState& other ) const;
	void Print( LogBuffer& logBuffer );
	void SetMPByte( PhysicsObject* po );
};

// =========================================================================================================

class PlayerInput
{
#ifndef TRANSGAMING_NO_HAVOK
// We currently don't support Havok with the PSP.
	friend class HavokCharacterController;
#endif
	friend class NetworkPeer2;

	unsigned char	TimeDelta;
	ulong32	Action;
	short			Yaw;
	short			Pitch;

public:

	PlayerInput( void ) : TimeDelta(0), Action(0), Yaw(0), Pitch(0) {};

	short GetPitch( void ) { return Pitch; };

	void Write( UdpMessage* mesg )
	{
		mesg->WriteUChar( TimeDelta );
		mesg->WriteULong( Action );
		mesg->WriteShort( Yaw );
		mesg->WriteShort( Pitch );
	}

	void Read( UdpMessage* mesg )
	{
		TimeDelta	= mesg->ReadUChar();
		Action		= mesg->ReadULong();
		Yaw			= mesg->ReadShort();
		Pitch		= mesg->ReadShort();
	}

	void WriteNoDelta( UdpMessage* mesg )
	{
		mesg->WriteULong( Action );
		mesg->WriteShort( Yaw );
		mesg->WriteShort( Pitch );
	}

	void ReadNoDelta( UdpMessage* mesg )
	{
		Action		= mesg->ReadULong();
		Yaw			= mesg->ReadShort();
		Pitch		= mesg->ReadShort();
	}

	void WriteYawPitch( UdpMessage* mesg )
	{
		mesg->WriteShort( Yaw );
		mesg->WriteShort( Pitch );
	}

	void ReadYawPitch( UdpMessage* mesg )
	{
		Yaw			= mesg->ReadShort();
		Pitch		= mesg->ReadShort();
	}

	// Flags for ReadDelta and WriteDelta
	enum
	{
		EPITimeDelta	= (1<<0),
		EPIAction		= (1<<1),
		EPIYaw			= (1<<2),
		EPIPitch		= (1<<3),
		EPIDeltaYaw		= (1<<4),
		EPIDeltaPitch	= (1<<5),
		EPIActionByte	= (1<<6),
	};

	void TestDelta( PlayerInput* previous );

	void WriteDelta( UdpMessage* mesg, PlayerInput* previous )
	{
		unsigned char flags = 0;
		int pos = mesg->GetPos();
		mesg->WriteUChar(0);	// placeholder

		if( TimeDelta != previous->TimeDelta )
		{
			flags |= EPITimeDelta;
			mesg->WriteUChar( TimeDelta );
		}

		if( Action != previous->Action )
		{
			if( ( Action ^ previous->Action ) <= 255 )
			{
				flags |= EPIActionByte;
				mesg->WriteUChar( (unsigned char)( Action ^ previous->Action ) );
			}
			else
			{
				flags |= EPIAction;
				mesg->WriteULong( Action );
			}
		}

		if( Yaw != previous->Yaw )
		{
			short yawDelta = Yaw-previous->Yaw;
			if( abs(yawDelta) <= SCHAR_MAX )
			{
				flags |= EPIDeltaYaw;
				mesg->WriteChar( yawDelta );
			}
			else
			{
				flags |= EPIYaw;
				mesg->WriteShort( Yaw );
			}
		}

		if( Pitch != previous->Pitch )
		{
			short pitchDelta = Pitch-previous->Pitch;
			if( abs(pitchDelta) <= SCHAR_MAX )
			{
				flags |= EPIDeltaPitch;
				mesg->WriteChar( pitchDelta );
			}
			else
			{
				flags |= EPIPitch;
				mesg->WriteShort( Pitch );
			}
		}

		if( flags )
		{
			int pos2 = mesg->GetPos();
			mesg->Seek( pos );
			mesg->WriteUChar( flags );
			mesg->Seek( pos2 );
		}
	}

	void ReadDelta( UdpMessage* mesg, PlayerInput* previous )
	{
		unsigned char flags = mesg->ReadUChar();
		TimeDelta = ( flags & EPITimeDelta )?mesg->ReadUChar():previous->TimeDelta;

		if( flags & EPIAction )
			Action = mesg->ReadULong();
		else
		{
			Action = previous->Action;
			if( flags & EPIActionByte )
			{
				DWORD Cxor = mesg->ReadUChar();
				Action ^= Cxor;
			}
		}

		if( flags & EPIYaw )
			Yaw = mesg->ReadShort();
		else if( flags & EPIDeltaYaw )
		{
			char deltaYaw = mesg->ReadChar();
			Yaw = previous->Yaw + deltaYaw;
		}
		else
			Yaw = previous->Yaw;

		if( flags & EPIPitch )
			Pitch = mesg->ReadShort();
		else if( flags & EPIDeltaPitch )
		{
			char deltaPitch = mesg->ReadChar();
			Pitch = previous->Pitch + deltaPitch;
		}
		else
			Pitch = previous->Pitch;
	}

	void SetIntoPlayer( PhysicsObject* po );
	void GetFromPlayer( PhysicsObject* po );

	void Print( LogBuffer& logBuffer );
};

// =========================================================================================================

struct MissileState
{
	// Members
	Vector Position;
	short VelocityX, VelocityY, VelocityZ;
	unsigned char Properties;
	unsigned short TimeSinceCreation;
	Vector AngularVelocity;

	// Methods

	MissileState() : Position(0,0,0), VelocityX(0), VelocityY(0), VelocityZ(0),
		Properties(0), TimeSinceCreation(0), AngularVelocity(0,0,0) {};

	void Write( UdpMessage* mesg );
	void Read( UdpMessage* mesg );
	void WriteNoPos( UdpMessage* mesg );
	void ReadNoPos( UdpMessage* mesg );

#ifndef TRANSGAMING_NO_HAVOK
	void SetInto( HavokMissileController* mc );
	void GetFrom( HavokMissileController* mc );
#endif
	void Print( LogBuffer& logBuffer );
	bool operator==( const MissileState& other ) const
	{
		return (
			( VelocityX == other.VelocityX ) &&
			( VelocityY == other.VelocityY ) &&
			( VelocityZ == other.VelocityZ ) &&
			( Position == other.Position ) &&
			( Properties == other.Properties ) &&
			( TimeSinceCreation == other.TimeSinceCreation ) &&
			( AngularVelocity == other.AngularVelocity )
		);
	}
};

// =========================================================================================================

class NetworkObjectState
{
public:
	NetworkObjectState( ulong32 uniqueNumber ) :
		POType( PO_None ), HasGravity( true ), Position(0,0,0), Orientation(0,0,0,1),
		Velocity(0,0,0), AngularVelocity(0,0,0), CollisionGroup(7), UniqueNumber( uniqueNumber ),
#ifndef LOCAL_PLAYER_INPUT
		Input(0),
#endif
#ifndef NO_MISSILE_UPDATES
		Missile(0),
#endif
		Player(0), Next(0) {};
	~NetworkObjectState( void )
	{
		if( Player )
		{
			delete Player;
			Player = 0;
		}
#ifndef NO_MISSILE_UPDATES
		if( Missile )
		{
			delete Missile;
			Missile = 0;
		}
#endif
#ifndef LOCAL_PLAYER_INPUT
		if( Input )
		{
			delete Input;
			Input = 0;
		}
#endif
	}

	enum ObjectNetworkFlags
	{
		NET_CREATEPO	= (1<<0),
		NET_DELETEPO	= (1<<1),
		NET_POINPUT		= (1<<2),
		NET_PODATA		= (1<<3),
		NET_POSITION	= (1<<4),
		NET_ORIENTATION	= (1<<5),
	};

	enum TypeOfPO
	{
		PO_None		= 0,
		PO_Normal	= 1,
		PO_Player	= 2,
		PO_Missile	= 3
	};

	NetworkObjectState* MakeExactCopy( void )
	{
        m_setTag("Net");
		NetworkObjectState* newState = new NetworkObjectState( UniqueNumber );

		// Normal PO data
		newState->POType = POType;
		newState->Position = Position;
		newState->Orientation = Orientation;
		newState->Velocity = Velocity;
		newState->AngularVelocity = AngularVelocity;
		newState->CollisionGroup = CollisionGroup;
		newState->HasGravity = HasGravity;

		if( Player )
		{
            m_setTag("Net");
			newState->Player = new PlayerState;
			*newState->Player = *Player;
		}

#ifndef NO_MISSILE_UPDATES
		if( Missile )
		{
            m_setTag("Net");
			newState->Missile = new MissileState;
			*newState->Missile = *Missile;
		}
#endif

#ifndef LOCAL_PLAYER_INPUT
		if( Input )
		{
            m_setTag("Net");
			newState->Input = new PlayerInput;
			*newState->Input = *Input;
		}
#endif

		return newState;
	};

	unsigned char	POType;

	PlayerState*	Player;
#ifndef LOCAL_PLAYER_INPUT
	PlayerInput*	Input;
#endif
#ifndef NO_MISSILE_UPDATES
	MissileState*	Missile;
#endif

	Vector			Position;		// position, of both PO and entity
	Quaternion		Orientation;	// orientation, of both PO and entity

	// Normal PO data
	Vector			Velocity;
	Vector			AngularVelocity;
	unsigned char	CollisionGroup;
	bool			HasGravity;

	ulong32	UniqueNumber;

	NetworkObjectState*		Next;
};

// =========================================================================================================

class NetworkAnswerFrame
{
public:
	NetworkAnswerFrame() : EntitiesStateList(0), Latency(0), LocalPlayerEntity(0), PredictAgainLocalPlayer(true) {};
	~NetworkAnswerFrame()
	{
		while( EntitiesStateList )
		{
			NetworkObjectState* temp = EntitiesStateList;
			EntitiesStateList = EntitiesStateList->Next;
			delete temp;
		}
	}

	Entity* LocalPlayerEntity;		// for speed-up
	bool PredictAgainLocalPlayer;	// for speed-up

	DWORD Latency;
	unsigned short LastSynchroStringNumber;
	unsigned short LastSynchroPONumber;
	ulong32 LastReliableMessageSentInFrame;
	NetworkObjectState* EntitiesStateList;
};

// =========================================================================================================

class NetworkFrame
{
public:
	NetworkFrame() : Answer(0), Next(0) {};
	~NetworkFrame()
	{
		if( Answer )
			delete Answer;
	}

	ulong32 PhysicsFrame;
	DWORD SentTime;			// this is really received time on server
	DWORD Number;
	PlayerInput Input;
	PlayerState PlayerData;
	unsigned short DeltaFrom;
	NetworkAnswerFrame* Answer;
	NetworkFrame* Next;
};

// =========================================================================================================
// ************************ PO Creation Cache - Start ****************************

class PONetCreationEntry2
{
public:
	unsigned short ObjectState;
	unsigned char BodyType;
	float BodyScale;
	char FreedomOfRotation;
	float Mass;
	float Friction;
	float Restitution;
	float Damping;
	unsigned char CollisionGroup;
	bool HasGravity;
	bool Pinned;
	// ----------------
	unsigned short index;
	int timesSent;

	bool operator==(const PONetCreationEntry2& other) const
	{
		return (
			( ObjectState == other.ObjectState ) &&
			( Mass == other.Mass ) &&
			( Friction == other.Friction ) &&
			( Restitution == other.Restitution ) &&
			( Damping == other.Damping ) &&
			( BodyScale == other.BodyScale ) &&
			( BodyType == other.BodyType ) &&
			( FreedomOfRotation == other.FreedomOfRotation ) &&
			( CollisionGroup == other.CollisionGroup ) &&
			( HasGravity == other.HasGravity ) &&
			( Pinned == other.Pinned )
			);
	}

	void SendEntry( UdpMessage* message );
	void ReceiveEntry( UdpMessage* message );
};

class PONetCreationCache2
{
	DynamicArray<PONetCreationEntry2> Entries;
	unsigned short mFirstUnsureNumberToSend;
	unsigned short mFirstUnsureNumberToReceive;
public:
	PONetCreationCache2() : mFirstUnsureNumberToSend(0), mFirstUnsureNumberToReceive(0) {};

	void SendPOCreationData( UdpMessage* message, PONetCreationEntry2& entry );
	void ReceivePOCreationData( UdpMessage* message, PONetCreationEntry2& entry );

	void SetFirstUnsureNumberToSend( unsigned short number ) { mFirstUnsureNumberToSend = number; };
	void SetFirstUnsureNumberToReceive( unsigned short number ) { mFirstUnsureNumberToReceive = number; };
	unsigned short GetFirstUnusedNumber( void ) { return Entries.Size(); };

	void Clear( void ) { Entries.ClearArray(); mFirstUnsureNumberToSend = 0; mFirstUnsureNumberToReceive = 0; };
};
// ************************ PO Creation Cache - End ******************************

// =========================================================================================================

class EntitySynchronizer2
{
public:
	enum
	{
		STypeMesh		= 0,
		STypeModel		= 1
	};

	EntitySynchronizer2( ulong32 uniqueNumber, NetworkDevice2* device );
	virtual ~EntitySynchronizer2( void );

	// Sending ( on server )
	virtual void AddCreateObjMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame ) = 0;
	virtual bool AddUpdateObjMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame, NetworkObjectState* oldState ) = 0;

	// Receiving ( on client )
	virtual bool InterpretCreateMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame ) = 0;
	virtual bool InterpretUpdateMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame, NetworkFrame* deltaFrame ) = 0;
	virtual void DeleteSubject( void );

	// From server to client
	inline static void WriteSynchronizer( UdpMessage* message, ulong32 uniqueNumber, unsigned char type );
	inline static void ReadSynchronizer( UdpMessage* message, ulong32& uniqueNumber, unsigned char& type );

	void MakeConnectionWithEntity( Entity* entity );
	void RemoveConnectionWithEntity( Entity* entity );
	virtual unsigned char GetType( void ) = 0;

	// Additional data
	Entity* GetEntity( void ) { return mObject; };

	// Members
	ulong32 UniqueNumber;
	bool NotRefreshed;	// a local variable, used to delete not-refreshed objects when receiving frame on client

protected:
	NetworkDevice2* mDevice;
	Entity* mObject;

	void SendCreateNormalPO( UdpMessage* mesg, NetworkPeer2* peer, NetworkObjectState* newState, bool sendPosOrient );
	void InterpretCreateNormalPO( UdpMessage* mesg, NetworkPeer2* peer, NetworkObjectState* newState, bool dump, bool recvPosOrient );

	void SendCreateMissilePO( UdpMessage* mesg, NetworkPeer2* peer, NetworkObjectState* newState, bool sendPosOrient );
	void InterpretCreateMissilePO( UdpMessage* mesg, NetworkPeer2* peer, NetworkObjectState* newState, bool dump, bool recvPosOrient );

	bool SendUpdateNormalPO( UdpMessage* mesg, NetworkObjectState* newState, NetworkObjectState* old );
	void CopyUpdateNormalPO( NetworkObjectState* newState, NetworkObjectState* old );
	void InterpretUpdateNormalPO( UdpMessage* mesg, NetworkObjectState* newState, NetworkObjectState* old );
};

// =========================================================================================================

class MeshSynchronizer2 : public EntitySynchronizer2
{
public:
	MeshSynchronizer2( ulong32 uniqueNumber, NetworkDevice2* device, WorldMesh* meshToSynchronize );
	~MeshSynchronizer2();

	// Sending ( on server )
	virtual void AddCreateObjMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame );
	virtual bool AddUpdateObjMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame, NetworkObjectState* oldState );

	// Receiving ( on client )
	virtual bool InterpretCreateMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame );
	virtual bool InterpretUpdateMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame, NetworkFrame* deltaFrame );

	virtual unsigned char GetType( void ) { return STypeMesh; };

protected:
	short ActiveMeshIndex;
};

// =========================================================================================================

class ModelSynchronizer2 : public EntitySynchronizer2
{
public:
	ModelSynchronizer2( ulong32 uniqueNumber, NetworkDevice2* device, Model* modelToSynchronize );
	~ModelSynchronizer2();

	// Sending ( on server )
	virtual void AddCreateObjMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame );
	virtual bool AddUpdateObjMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame, NetworkObjectState* oldState );

	// Receiving ( on client )
	virtual bool InterpretCreateMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame );
	virtual bool InterpretUpdateMessage( UdpMessage* message, NetworkPeer2* peer, NetworkFrame* targetFrame, NetworkFrame* deltaFrame );

	virtual unsigned char GetType( void ) { return STypeModel; };

protected:

	void SendAnimationInfo( UdpMessage* message, NetworkPeer2* peer );
	void ApplyAnimationInfo( UdpMessage* message, NetworkPeer2* peer );
	void DumpAnimationInfo( UdpMessage* message, NetworkPeer2* peer );

	bool mSynchroAnimation;
};

// =========================================================================================================

inline void EntitySynchronizer2::ReadSynchronizer( UdpMessage* message, ulong32& uniqueNumber, unsigned char& type )
{
	ulong32 read = message->ReadULongOptimized();
	uniqueNumber = (read>>2)-1;
	type = (unsigned char)(read&0x3);
}



#endif //_OBJECTSYNCHRONIZER2_

