#error This file is obselete

// #pragma once
#ifndef _OBJECTSYNCHRONIZER_
#define _OBJECTSYNCHRONIZER_


class NetworkClient;
class UdpMessage;

class Entity;
class HavokWorld;

#include "Core/Math/LeMath.h"
#include "Physics/PhysicsWorld.h"
#include "Core/LeString.h"
#include "Net/UdpMessage.h"

enum NetcodeTokens
{
	MESH_CREATE		= 0,
	MESH_UPDATE		= 1,
	MESH_DELETE		= 2,
	MODEL_CREATE	= 3,
	MODEL_UPDATE	= 4,
	MODEL_DELETE	= 5,
	MODEL_OTHER		= 6,
	PHO_CREATE		= 7,
	PHO_UPDATE		= 8,
	PHO_DELETE		= 9,
	PHO_OTHER		= 10,

	NCL_LOADMAP		= 11,
	NCL_MAPLOADING	= 12,
	NCL_SYNCHRO		= 13,
	NCL_SYNCHROACK	= 14,
	NCL_UPDATECL	= 15,
	NCL_UPDATEMPS	= 16,
	NCL_LATENCY		= 17,
	NCL_PLACTION	= 18,
	NCL_CLOPEN		= 19,

	IPD_GETCH		= 20,
	IPD_CHALLENGE	= 21,
	IPD_CONNECT		= 22,
	IPD_CLCONNECT	= 23,

	NCL_ENTITYMSG	= 24,
	NCL_SCRIPTMSG	= 25,
};

// Has to be the power of 2
#define UNIQUE_NUMBER_QUARANTINE_SIZE 1024

class UniqueNumberReuser
{
public:
	UniqueNumberReuser( void );
	~UniqueNumberReuser( void ) {};
	unsigned long GetNext( void );
	void Free( unsigned long uniqueNumber );
    void Clear();
    bool Empty();

private:
	unsigned long mNextNumber;
	unsigned long mQuarantined[UNIQUE_NUMBER_QUARANTINE_SIZE];
	int mStart, mEnd;
	DynamicArray<unsigned long> mFree;
};

class ObjectSynchronizer
{
public:

	enum
	{
		STypeMesh		= 0,
		STypeModel		= 1,
		STypePO			= 2,
//		STypeRagdoll	= 3,
		// Warning: if you add something here, change WriteSynchronizer() and ReadSynchronizer() methods!
	};

	ObjectSynchronizer( unsigned long uniqueNumber, NetworkClient* client );
	virtual ~ObjectSynchronizer();
	virtual void Maintain( void ) = 0;
	virtual void ClientMaintain( void ) = 0;
	virtual void AddUpdateObjMessage( UdpMessage* message ) = 0;
	virtual void AddCreateObjMessage( UdpMessage* message, NetworkClient* client ) = 0;
	virtual void AddDeleteObjMessage( UdpMessage* message, NetworkClient* client ) = 0;

	virtual int HowManyOtherMessages( void ) = 0;
	virtual void AddOtherObjMessage( UdpMessage* message, NetworkClient* client, int index ) = 0;
	virtual void ClearOtherMessages( void ) = 0;

	virtual bool InterpretMessage( UdpMessage* message, unsigned char command, NetworkClient* client ) = 0;
	virtual void SetZombie( void );
	virtual unsigned char GetType( void ) = 0;
	bool GetZombie( void ) { return IsZombie; };
	inline void SetLastUpdateTime( DWORD timeLow, DWORD timeHigh ) { LastUpdateTimeLow = timeLow; LastUpdateTimeHigh = timeHigh; };

	// Net optimization shortcuts
	inline static void WriteSynchronizer( UdpMessage* message, unsigned long uniqueNumber, unsigned char type );
	inline static void ReadSynchronizer( UdpMessage* message, unsigned long& uniqueNumber, unsigned char& type );

	virtual void SaveState( void ) = 0;
	virtual void RestoreState( void ) = 0;

	void MakeConnectionWithEntity( Entity* entity );
	void RemoveConnectionWithEntity( Entity* entity );

	// Members
	unsigned long UniqueNumber;
	unsigned char UpdateClass;
protected:
	NetworkClient* mClient;
	DWORD LastUpdateTimeLow;
	DWORD LastUpdateTimeHigh;
	bool IsZombie;
	inline void PostCreateMessage( void );
	inline void PostDeleteMessage( void );
	unsigned long mLastFrameSent;
};

class EntitySynchronizer : public ObjectSynchronizer
{
	friend class POSynchronizer;
public:
	EntitySynchronizer( unsigned long uniqueNumber, NetworkClient* client );

	virtual void Maintain( void ) = 0;
	virtual void ClientMaintain( void ) = 0;
	virtual void AddUpdateObjMessage( UdpMessage* message ) = 0;
	virtual void AddCreateObjMessage( UdpMessage* message, NetworkClient* client ) = 0;
	virtual void AddDeleteObjMessage( UdpMessage* message, NetworkClient* client ) = 0;

	virtual int HowManyOtherMessages( void ) = 0;
	virtual void AddOtherObjMessage( UdpMessage* message, NetworkClient* client, int index ) = 0;
	virtual void ClearOtherMessages( void ) = 0;
	Entity* GetEntity( void );
	POSynchronizer* GetPO( void ) { return mPO; };

	virtual bool InterpretMessage( UdpMessage* message, unsigned char command, NetworkClient* client ) = 0;
	virtual ~EntitySynchronizer() {};

	virtual void SaveState( void ) = 0;
	virtual void RestoreState( void ) = 0;

protected:
	Entity* mObject;
	POSynchronizer* mPO;
};

// ************************ PO Creation Cache - Start ****************************
class PONetCreationEntry
{
public:
	unsigned char UpdateClass;
	unsigned short ObjectState;
	unsigned char BodyType;
	float BodyScale;
	char FreedomOfRotation;
	float Mass;
	float Friction;
	float Restitution;
	unsigned char CollisionGroup;
	// ----------------
	unsigned long index;
	int timesSent;

	bool operator==(const PONetCreationEntry& other) const
	{
		return (
			( ObjectState == other.ObjectState ) &&
			( Mass == other.Mass ) &&
			( Friction == other.Friction ) &&
			( Restitution == other.Restitution ) &&
			( BodyScale == other.BodyScale ) &&
			( BodyType == other.BodyType ) &&
			( FreedomOfRotation == other.FreedomOfRotation ) &&
			( CollisionGroup == other.CollisionGroup )
			);
	}

	void SendEntry( UdpMessage* message );
	void ReceiveEntry( UdpMessage* message );

	void Save( GFile* f );
	void Restore( GFile* f );
	PONetCreationEntry& operator=( const PONetCreationEntry& other );
};

class PONetCreationCache
{
	DynamicArray<PONetCreationEntry> Entries;
public:
	PONetCreationCache() {};
	~PONetCreationCache() {};

	int SendPOCreationData( UdpMessage* message, PONetCreationEntry& entry );
	void ReceivePOCreationData( UdpMessage* message, PONetCreationEntry& entry );

	void Save( GFile* f );
	void Restore( GFile* f );
	void DumpRestore( GFile* f );
	void Clear( void ) { Entries.Clear(); };
};
// ************************ PO Creation Cache - End ******************************

class POSynchronizer : public ObjectSynchronizer
{
	friend class NetworkClient;
	friend class MeshSynchronizer;
	friend class ModelSynchronizer;
public:
	POSynchronizer( unsigned long uniqueNumber, NetworkClient* client, PhysicsObject* poToSynchronize, EntitySynchronizer* entitySynchro );
	virtual void Maintain( void );
	virtual void ClientMaintain( void );
	virtual void AddUpdateObjMessage( UdpMessage* message );
	virtual void AddCreateObjMessage( UdpMessage* message, NetworkClient* client );
	virtual void AddDeleteObjMessage( UdpMessage* message, NetworkClient* client );

	virtual int HowManyOtherMessages( void );
	virtual void AddOtherObjMessage( UdpMessage* message, NetworkClient* client, int index );
	virtual void ClearOtherMessages( void );

	virtual void SaveState( void );
	virtual void RestoreState( void );

	virtual unsigned char GetType( void ) { return STypePO; };
	bool HasPO( void ) { return (mObject != 0); };
	virtual bool InterpretMessage( UdpMessage* message, unsigned char command, NetworkClient* client );
	~POSynchronizer();

	void AddActionUpdate( int actionType ) { mActionUpdates.AddUnique(actionType); };

protected:
	PhysicsObject* mObject;
	EntitySynchronizer* mEntity;
	unsigned int EntityGIndex;

	// Synchro variables
	unsigned char CollisionGroup;
	unsigned char CollisionGroupSynchroIndex;
	Vector Position;
	unsigned char PositionSynchroIndex;
	Quaternion Orientation;
	unsigned char OrientationSynchroIndex;
	DynamicArray<int> mActionUpdates;

	// State saving
	bool Saved;
	Vector SavePos;
	Quaternion SaveOrient;
	Vector SaveVel;
	Vector SaveAngVel;
};

class MeshSynchronizer : public EntitySynchronizer
{
	friend class POSynchronizer;
public:
	MeshSynchronizer( unsigned long uniqueNumber, NetworkClient* client, WorldMesh* meshToSynchronize );
	virtual void Maintain( void );
	virtual void ClientMaintain( void ) {};
	virtual void AddUpdateObjMessage( UdpMessage* message );
	virtual void AddCreateObjMessage( UdpMessage* message, NetworkClient* client );
	virtual void AddDeleteObjMessage( UdpMessage* message, NetworkClient* client );

	virtual int HowManyOtherMessages( void ) { return 0; };
	virtual void AddOtherObjMessage( UdpMessage* message, NetworkClient* client, int index ) {};
	virtual void ClearOtherMessages( void ) {};

	virtual bool InterpretMessage( UdpMessage* message, unsigned char command, NetworkClient* client );
	virtual unsigned char GetType( void ) { return STypeMesh; };
	virtual void SetZombie( void );
	~MeshSynchronizer();

	virtual void SaveState( void ) {};
	virtual void RestoreState( void ) {};

protected:
	short ActiveMeshIndex;
	bool MeshEqual( String& filename, String& name, float scale );

	Vector Position;
	unsigned char PositionSynchroIndex;

	Quaternion Orientation;
	unsigned char OrientationSynchroIndex;
};

class ModelSynchronizer : public EntitySynchronizer
{
	friend class POSynchronizer;
//	friend class RagdollSynchronizer;
public:
	ModelSynchronizer( unsigned long uniqueNumber, NetworkClient* client, Model* modelToSynchronize );
	virtual void Maintain( void );
	virtual void ClientMaintain( void ) {};
	virtual void AddUpdateObjMessage( UdpMessage* message );
	virtual void AddCreateObjMessage( UdpMessage* message, NetworkClient* client );
	virtual void AddDeleteObjMessage( UdpMessage* message, NetworkClient* client );

	virtual int HowManyOtherMessages( void );
	virtual void AddOtherObjMessage( UdpMessage* message, NetworkClient* client, int index );
	virtual void ClearOtherMessages( void );

	virtual bool InterpretMessage( UdpMessage* message, unsigned char command, NetworkClient* client );
	virtual unsigned char GetType( void ) { return STypeModel; };
	virtual void SetZombie( void );
	~ModelSynchronizer();

	virtual void SaveState( void ) {};
	virtual void RestoreState( void ) {};

	void AddAnimActivation( int animNumber, bool looped, float blendTime, float timeScale, bool hasMovementCurve );
	void AddAnimLoad( String name, bool looped, int animIndex );

protected:
	bool ModelEqual( String& filename, String& name, float scale );
	void SendAnimationInfo( UdpMessage* message, NetworkClient* client );
	void ApplyAnimationInfo( UdpMessage* message, NetworkClient* client );
	void DumpAnimationInfo( UdpMessage* message, NetworkClient* client );

	// Internal structures for animation synchronizing
	enum OtherMessageType
	{
		OMT_Activation = 0,
		OMT_Load = 1,
		OMT_AnimSynchro = 2,
	};

	struct AnimActivation
	{
		unsigned char AnimNumber;
		bool Looped;
		float BlendTime;
		float TimeScale;
		bool HasMovementCurve;
	};

	struct AnimLoad
	{
		String Name;
		bool Looped;
		unsigned char AnimNumber;
	};

	class SendStruct
	{
	public:
		inline bool operator==(const SendStruct& s) const { return( (OtherMessageType == s.OtherMessageType) && (Data == s.Data) ); };
		int OtherMessageType;
		void* Data;
	};

	DynamicArray<SendStruct> mToSend;

//	RagdollSynchronizer* mRagdoll;
	bool mSynchroAnimation;

	Vector Position;
	unsigned char PositionSynchroIndex;
	Quaternion Orientation;
	unsigned char OrientationSynchroIndex;
};

/*
class RagdollSynchronizer : public ObjectSynchronizer
{
	friend class NetworkClient;
	friend class ModelSynchronizer;
public:
	RagdollSynchronizer( unsigned long uniqueNumber, NetworkClient* client, Ragdoll* ragdollToSynchronize, ModelSynchronizer* modelSynchro );
	virtual void Maintain( void );
	virtual void ClientMaintain( void ) {};
	virtual void AddUpdateObjMessage( UdpMessage* message );
	virtual void AddCreateObjMessage( UdpMessage* message, NetworkClient* client );
	virtual void AddDeleteObjMessage( UdpMessage* message, NetworkClient* client );

	virtual int HowManyOtherMessages( void ) { return 0; };
	virtual void AddOtherObjMessage( UdpMessage* message, NetworkClient* client, int index ) {};
	virtual void ClearOtherMessages( void ) {};
	virtual unsigned char GetType( void ) { return STypeRagdoll; };
	bool HasRagdoll( void ) { return (mObject != 0); };
	virtual bool InterpretMessage( UdpMessage* message, unsigned char command, NetworkClient* client );

	virtual void SaveState( void ) {};
	virtual void RestoreState( void ) {};

	~RagdollSynchronizer();
protected:
	Ragdoll* mObject;
	ModelSynchronizer* mModel;
	unsigned int EntityGIndex;
	bool mDoesNotSynchroAnim;

	// Internal update state
	DynamicArray<Vector> mLimbPositions;
	DynamicArray<unsigned char> mLimbPositionsUnchanged;
	DynamicArray<Quaternion> mLimbRotations;
	DynamicArray<unsigned char> mLimbRotationsUnchanged;
};
*/
inline void ObjectSynchronizer::ReadSynchronizer( UdpMessage* message, unsigned long& uniqueNumber, unsigned char& type )
{
	unsigned long read = message->ReadULongOptimized();
	uniqueNumber = (read>>2)-1;
	type = unsigned char(read&0x3);
}



#endif //_OBJECTSYNCHRONIZEROBJECTSYNCHRONIZER_

