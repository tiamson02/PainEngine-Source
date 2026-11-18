#error This file is obselete

//#include "Engine.h"

#ifdef OIUYTREWLKJHGFDS

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Physics/Havok/POHavokPart.h"
#include "Net/UdpMessage.h"
#include "Engine/Mesh.h"
#include "Physics/Ragdoll.h"
#include "Physics/PhysicsEngine.h"
#include "Net/NetworkClient.h"
#include "Net/ObjectSynchronizer.h"
#include "Net/NetworkDevice.h"
#include "Engine/ModelAnimation.h"
#include "Physics/PhysicsObject.h"
#include "Render/Video.h"
#include "Engine/Model.h"
#include "Engine/World.h"

#ifndef FINAL_BUILD
#define REPORT_CREATION_DELETION
#endif

UniqueNumberReuser::UniqueNumberReuser( void ) : mNextNumber(0), mStart(0), mEnd(0)
{
	::memset( (void*)mQuarantined, 0, UNIQUE_NUMBER_QUARANTINE_SIZE*sizeof(unsigned long) );
}

unsigned long UniqueNumberReuser::GetNext( void )
{
	if( !GEngine->Net->IsServer() )
	{
		GLog.Print( "!!!!!!!!!!!!!UniqueNumberReuser::GetNext() called on Client!!!!!!!!!!!!!\n" );
		return mNextNumber++;
	}

	if( mFree.Size() )
	{
		unsigned long next = mFree[mFree.Size()-1];
		mFree.RemoveLast();
		return next;
	}
	else
		return mNextNumber++;
}

void UniqueNumberReuser::Free( unsigned long uniqueNumber )
{
	if( GEngine->Net->IsServer() )
	{
		mQuarantined[mEnd] = uniqueNumber;
		if( mStart == ((mEnd+1)&(UNIQUE_NUMBER_QUARANTINE_SIZE-1)) )	// mQuarantined is full
		{
			mFree.Add( mQuarantined[mStart] );
			mEnd++;
			mEnd &= (UNIQUE_NUMBER_QUARANTINE_SIZE-1);
			mStart++;
			mStart &= (UNIQUE_NUMBER_QUARANTINE_SIZE-1);
		}
		else	// it is not full
		{
			mEnd++;
			mEnd &= (UNIQUE_NUMBER_QUARANTINE_SIZE-1);
		}
	}
	else	// we don't use quarantined on clients - what's the use?
		mFree.Add( uniqueNumber );
}

void UniqueNumberReuser::Clear()
{
    mFree.Clear();
    mNextNumber = 0;
    mStart = 0;
    mEnd = 0;
    ::memset( (void*)mQuarantined, 0, UNIQUE_NUMBER_QUARANTINE_SIZE*sizeof(unsigned long) );
}

bool UniqueNumberReuser::Empty()
{
	return (mNextNumber == 0);
}

ObjectSynchronizer::ObjectSynchronizer( unsigned long uniqueNumber, NetworkClient* client ) : IsZombie( false )
,	UpdateClass(0), LastUpdateTimeLow(0), LastUpdateTimeHigh(0), mClient(client), mLastFrameSent(0)
{
	Assert( client );

	Assert( uniqueNumber != (unsigned long)-1 );
	UniqueNumber = uniqueNumber;

	client->mObjectSynchronizers.Add( this );

#ifdef _DEBUG
	ObjectSynchronizer** t = client->mObjectSynchronizersHash.Find( UniqueNumber );
	Assert( !t );
#endif

	client->mObjectSynchronizersHash.Insert( UniqueNumber, this );

//	GLog.Print( "CREATE OBJ SYCHRO %d\n", UniqueNumber );
}

ObjectSynchronizer::~ObjectSynchronizer()
{
//	GLog.Print( "DELETE OBJ SYCHRO %d\n", UniqueNumber );
	Assert( mClient );
	mClient->mObjectSynchronizersHash.Remove( UniqueNumber );
	for( int i = 0; i < mClient->mObjectSynchronizers.Size(); ++i )
		if( mClient->mObjectSynchronizers[i] == this )
		{
			mClient->mObjectSynchronizers.FastRemove( i );
			break;
		}
}

void ObjectSynchronizer::PostCreateMessage( void )
{
	if( !GEngine->Net->IsServer() ) return;
	mClient->mObjectSynchronizersToCreate.AddUnique( this );
}

void ObjectSynchronizer::PostDeleteMessage( void )
{
	mClient->mObjectSynchronizersToDelete.AddUnique( this );
}

void ObjectSynchronizer::SetZombie( void )
{
	if( !IsZombie )
		mClient->mObjectSynchronizersToDelete.AddUnique( this );
	IsZombie = true;
}

inline void ObjectSynchronizer::WriteSynchronizer( UdpMessage* message, unsigned long uniqueNumber, unsigned char type )
{
	Assert( ( uniqueNumber == -1 ) || ( uniqueNumber < 0x3fffffff ) );
	char c = message->WriteULongOptimized( (((unsigned long)type)&0x3)|((uniqueNumber + 1)<<2) ); NPROF2(NPSynchronizerNumber,c);
}

void ObjectSynchronizer::RemoveConnectionWithEntity( Entity* entity )
{
	GEngine->Net->mObjectSynchronizersHashByEntity.Remove( entity, entity->GIndex );
}

void ObjectSynchronizer::MakeConnectionWithEntity( Entity* entity )
{
	GEngine->Net->mObjectSynchronizersHashByEntity.Insert( entity, entity->GIndex,  UniqueNumber );
}
// ***************************************************************************************

EntitySynchronizer::EntitySynchronizer( unsigned long uniqueNumber, NetworkClient* client ) :
	ObjectSynchronizer( uniqueNumber, client ), mObject( 0 ), mPO( 0 ) {};
Entity* EntitySynchronizer::GetEntity( void ) { return mObject; };

// ***************************************************************************************

MeshSynchronizer::MeshSynchronizer( unsigned long uniqueNumber, NetworkClient* client, WorldMesh* meshToSynchronize ) 
:	EntitySynchronizer( uniqueNumber, client ), PositionSynchroIndex( 0 )
,	OrientationSynchroIndex( 0 ), Position( 0, 0, 0 ), Orientation()
{
	ActiveMeshIndex = -1;
	mObject = (Entity*)meshToSynchronize;
	PostCreateMessage();
}

void MeshSynchronizer::Maintain( void )
{
	if( !IsZombie && mObject && mObject->PO && !mPO )	// that means PO was added to model in last tick
	{
		unsigned long ui = GEngine->Net->SureGetPtrIndex( mObject->PO, mObject->GIndex );
        m_setTag("Net");
		mPO = new POSynchronizer( ui, mClient, mObject->PO, this );
		mPO->UpdateClass = UpdateClass;
	}
}

void MeshSynchronizer::AddUpdateObjMessage( UdpMessage* message )
{
	if( IsZombie || !mObject || mObject->PO )
		return;

	if( GEngine->World->Physics->GetPhysicsFrame() == mLastFrameSent ) return;
	mLastFrameSent = GEngine->World->Physics->GetPhysicsFrame();

	DWORD curTimeLow = 0;
	DWORD curTimeHigh = 0;
	GEngine->SysDriver.GetPureTickCount( curTimeLow, curTimeHigh );
	UpdateClassParams* updateParams = mClient->ConnectionParams.Find(UpdateClass);
	if( updateParams && updateParams->MinSynchroDelay )
	{
		DWORD diffLow, diffHigh;
		GetTickCountDifference( LastUpdateTimeLow, LastUpdateTimeHigh, curTimeLow, curTimeHigh, diffLow, diffHigh );
		if( !TickCount_SecondHigherThanFirst( updateParams->MinSynchroDelayLow, updateParams->MinSynchroDelayHigh, diffLow, diffHigh ) )
			return;
	}

	int startMesgPos = message->GetPos();
	unsigned long cnt = GEngine->Net->Profiler.Remember( NPSynchronizerNumber );

	Assert( UniqueNumber != -1 );
	WriteSynchronizer( message, UniqueNumber, STypeMesh );

	int mesgPos = message->GetPos();
	message->WriteUChar( 0 );

	unsigned char sendState = 0;

	// Try to send new position
	{
		Vector pos = mObject->GetWorldPosition();

		if( pos == Position )
			PositionSynchroIndex++;
		else
		{
			PositionSynchroIndex = 0;
			Position = pos;
		}

		if( PositionSynchroIndex < mClient->mHowManyUnreliableAssumedToPass )
		{
			sendState |= (1<<0);
			message->WritePositionOptimized( pos ); NPROF( NPEntityPosition );
		}

		if( PositionSynchroIndex == 255 ) PositionSynchroIndex = 254;	// no overflow needed
	}

	// Try to send new orientation
	{
		Quaternion q = mObject->GetWorldRotation();

		if( q == Orientation )
			OrientationSynchroIndex++;
		else
		{
			OrientationSynchroIndex = 0;
			Orientation = q;
		}

		if( OrientationSynchroIndex < mClient->mHowManyUnreliableAssumedToPass )
		{
			sendState |= (1<<1);
			message->WriteQuaternionOptimized( q ); NPROF( NPEntityRotation );
		}

		if( OrientationSynchroIndex == 255 ) OrientationSynchroIndex = 254;	// no overflow needed
	}

	if( sendState == 0 )	// There was nothing to synchronize - message will not be sent this way
	{
		GEngine->Net->Profiler.Remind( NPSynchronizerNumber, cnt );
		message->Seek(startMesgPos);
		return;
	}

	message->WriteCheckToken( MESH_UPDATE );

	int mesgPos2 = message->GetPos();
	message->Seek(mesgPos);
	message->WriteUChar(sendState); NPROF(NPPOUpdateSynchroMarker);
	message->Seek(mesgPos2);

	LastUpdateTimeLow = curTimeLow;
	LastUpdateTimeHigh = curTimeHigh;
}

void MeshSynchronizer::AddCreateObjMessage( UdpMessage* message, NetworkClient* client )
{
	Assert( mObject && !IsZombie );

	if( mObject->PO )
		ActiveMeshIndex = mObject->PO->GetActiveMeshIndex();
	else
		ActiveMeshIndex = -1;

	GLog.Print( "Mesh::CREATEOBJ %d sent\n", UniqueNumber );
	Assert( UniqueNumber != -1 );
	WriteSynchronizer( message, UniqueNumber, STypeMesh );
	message->WriteUChar( UpdateClass ); NPROF( NPUpdateClass );
	message->WriteShort( ActiveMeshIndex ); NPROF(NPActiveMeshIndex);

	client->StringTransport.WriteString( message, mObject->FileName );
	client->StringTransport.WriteString( message, mObject->Name );
	message->WriteFloat( mObject->GetScale() ); NPROF(NPEntityScale);
	message->WriteULong( (unsigned long)mObject->Flags ); NPROF(NPEntityFlags);

	Position = mObject->GetWorldPosition();
	message->WritePositionOptimized( Position ); NPROF( NPEntityPosition );
	PositionSynchroIndex = 0;
	Orientation = mObject->GetWorldRotation();
	message->WriteQuaternionOptimized( Orientation ); NPROF( NPEntityRotation );
	OrientationSynchroIndex = 0;

	char* synStr = mObject->GetSynchroString();
	if( synStr )
	{
		String s( synStr );
		message->WriteBool( true );
		client->StringTransport.WriteString( message, s );
	}
	else
		message->WriteBool( false );

	message->WriteCheckToken( MESH_CREATE );
}

void MeshSynchronizer::AddDeleteObjMessage( UdpMessage* message, NetworkClient* client )
{
//	GLog.Print( "Mesh::DELETEOBJ %d sent\n", UniqueNumber );
	Assert( UniqueNumber != -1 );
	WriteSynchronizer( message, UniqueNumber, STypeMesh );
	message->WriteBool( IsZombie ); NPROF(NPIsZombie);
	message->WriteCheckToken( MESH_DELETE );
}

bool MeshSynchronizer::MeshEqual( String& filename, String& name, float scale )
{
	String meshFileName = mObject->FileName.Len()?mObject->FileName:"";
	String meshName = mObject->Name.Len()?mObject->Name:"";
	return( (meshFileName == filename) && (meshName == name) && (mObject->GetScale() == scale) );
}

bool MeshSynchronizer::InterpretMessage( UdpMessage* message, unsigned char command, NetworkClient* client )
{
	switch( command )
	{
	case NetworkClient::CREATEOBJ:
		{
			UpdateClass = message->ReadUChar();
			short activeMeshIndex = message->ReadShort();
			String filename = client->StringTransport.ReadString( message );
			String entityName = client->StringTransport.ReadString( message );
			float scale = message->ReadFloat();
			DWORD flags = (DWORD)message->ReadULong();
			Vector t = message->ReadPositionOptimized();
			Quaternion q = message->ReadQuaternionOptimized();

            String synStr = "";
			if( message->ReadBool() )
				synStr = client->StringTransport.ReadString( message );

			message->ReadCheckToken( MESH_CREATE );

#ifdef REPORT_CREATION_DELETION
			GLog.Print( "Mesh::CREATEOBJ %d: file = %s, name = %s, scale = %f\n",
				UniqueNumber, filename.AsChar(), entityName.AsChar(), scale );
#endif

			if( mObject )
			{
				GLog.Print( "Tried to create again exisitng object!!!\n" );
				GLog.Flush();
				return false;
			}
/*
			// Check if we have to remove old model
			if( mObject && !MeshEqual( filename, entityName, scale ) )
			{
				Assert( !mPO );
				RemoveConnectionWithEntity( mObject );
				GEngine->World->DeleteEntityDelayed( mObject );
				mObject = 0;
			}
*/
			if( !mObject && ( activeMeshIndex != -1 ) )
				mObject = GEngine->World->Physics->GetEntityByActiveMeshIndex( activeMeshIndex );

			if( mObject ) MakeConnectionWithEntity( mObject );

			// Check if we have to make a new one
			if( !mObject )
			{
				// Create new entity with those contents
                mObject = (WorldMesh*)GEngine->World->CreateEntity( Entity::ETypeMesh, filename.AsChar(), entityName.AsChar(), scale, (flags & Entity::EEntCenteredGeometry)!=0 );
				if( !mObject )
				{
					GLog.Print( "Was unable to create mesh! filename = %s, entityName = %s, scale = %f, unique = %d\n",
						filename.AsChar(), entityName.AsChar(), scale, UniqueNumber );
					message->DumpToLogfile( "Unable to create mesh" );
					return true;
				}

				GEngine->World->AddEntity( mObject );
			}

			mObject->Flags = flags;
			mObject->SetPositionAndRotation( t, q );

			if( synStr.Len() )
			{
				mObject->SetSynchroString(synStr.AsChar());
				GScript.QueueMessage("ENTITY_CREATE","%e,%s",mObject,synStr.AsChar());
			}
		}
		return true;
	case NetworkClient::DELETEOBJ:
#ifdef REPORT_CREATION_DELETION
		GLog.Print( "Mesh::DELETEOBJ %d\n", UniqueNumber );
#endif
		if( mObject )
		{
			char* str = mObject->GetSynchroString();
			if( str && ::strlen(str) )
   				GScript.QueueMessage( "ENTITY_DELETE","%e,%s", mObject, str );

			RemoveConnectionWithEntity( mObject );
			GEngine->World->DeleteEntityDelayed( mObject );
			mObject = 0;
		}
		IsZombie = message->ReadBool();
		message->ReadCheckToken( MESH_DELETE );
		return true;
	case NetworkClient::UPDATE:
		{
			unsigned char sendState = message->ReadUChar();

			if( sendState & (1<<0) )
				Position = message->ReadPositionOptimized();

			if( sendState & (1<<1) )
				Orientation = message->ReadQuaternionOptimized();

			message->ReadCheckToken( MESH_UPDATE );

			if( mObject )
			{
				switch( sendState )
				{
				case 0:	// none
				default:
					break;
				case 1:	// only position
					mObject->SetPosition( Position );
					break;
				case 2:	// only orientation
					mObject->SetRotation( Orientation );
					break;
				case 3:	// both
					mObject->SetPositionAndRotation( Position, Orientation );
					break;
				}
			}
		}
		return true;
	case NetworkClient::OTHEROBJ:
	default:
		GLog.Print( "Warning! Unexpected message type OTHEROBJ to be recognized by Mesh! Network stream bugs may follow!\n" );
		message->DumpToLogfile( "Mssage type OTHEROBJ given to Mesh" );
		return false;
	}
}

void MeshSynchronizer::SetZombie( void )
{
	if( mPO )
		mPO->SetZombie();
	ObjectSynchronizer::SetZombie();
}

MeshSynchronizer::~MeshSynchronizer()
{
	if( mPO )
	{
		mPO->SetZombie();
		mPO->mEntity = 0;
	}
}

// ***************************************************************************************

ModelSynchronizer::ModelSynchronizer( unsigned long uniqueNumber, NetworkClient* client, Model* modelToSynchronize ) 
:	EntitySynchronizer( uniqueNumber, client )//, mRagdoll( 0 )
,	PositionSynchroIndex( 0 ), OrientationSynchroIndex( 0 ), Position( 0, 0, 0 ), Orientation()
{
	mObject = (Entity*)modelToSynchronize;
	if( mObject )
	{
		mSynchroAnimation = (( mObject->SynchroState() & Entity::ESynchroAnimation ) != 0);
		PostCreateMessage();
	}
}

void ModelSynchronizer::Maintain( void )
{
	if( IsZombie || !mObject )
		return;

	if( mObject->PO && !mPO )
	{
		unsigned long ui = GEngine->Net->SureGetPtrIndex( mObject->PO, mObject->GIndex );
        m_setTag("Net");
		mPO = new POSynchronizer( ui, mClient, mObject->PO, this );
		mPO->UpdateClass = UpdateClass;
	}
/*
	if( ((Model*)mObject)->RagDoll )
	{
		if( !mRagdoll )
		{
			if( mObject->SynchroState() & Entity::ESynchroRagdollCreation )
			{
				unsigned long ui = GEngine->Net->SureGetPtrIndex( ((Model*)mObject)->RagdollPtr, mObject->GIndex );
                m_setTag("Net");
				mRagdoll = new RagdollSynchronizer( ui, mClient, ((Model*)mObject)->RagdollPtr, this );
				mRagdoll->UpdateClass = UpdateClass;
			}
		}
		else if( ( GEngine->GameState == PCFSystem::PCFSystemStates::DedicatedServer ) &&
			!( mObject->SynchroState() & Entity::ESynchroRagdoll ) )
		{
			((Model*)mObject)->RagdollPtr->EternalFreeze();
		}
	}
*/
	if( mObject->SynchroState() & Entity::ESynchroAnimation )
	{
		if( !mSynchroAnimation )
		{
			// Add animation synchro other message
			SendStruct ss;
			ss.OtherMessageType = OMT_AnimSynchro;
			ss.Data = 0;
			mToSend.AddUnique(ss);
			mSynchroAnimation = true;
		}
	}
	else
		mSynchroAnimation = false;
}

void ModelSynchronizer::AddUpdateObjMessage( UdpMessage* message )
{
	if( IsZombie || !mObject || mObject->PO  || ((Model*)mObject)->RagDoll )
		return;

	if( GEngine->World->Physics->GetPhysicsFrame() == mLastFrameSent ) return;
	mLastFrameSent = GEngine->World->Physics->GetPhysicsFrame();

	DWORD curTimeLow = 0;
	DWORD curTimeHigh = 0;
	GEngine->SysDriver.GetPureTickCount( curTimeLow, curTimeHigh );
	UpdateClassParams* updateParams = mClient->ConnectionParams.Find(UpdateClass);
	if( updateParams && updateParams->MinSynchroDelay )
	{
		DWORD diffLow, diffHigh;
		GetTickCountDifference( LastUpdateTimeLow, LastUpdateTimeHigh, curTimeLow, curTimeHigh, diffLow, diffHigh );
		if( !TickCount_SecondHigherThanFirst( updateParams->MinSynchroDelayLow, updateParams->MinSynchroDelayHigh, diffLow, diffHigh ) )
			return;
	}

	int startMesgPos = message->GetPos();
	unsigned long cnt = GEngine->Net->Profiler.Remember( NPSynchronizerNumber );
	Assert( UniqueNumber != -1 );
	WriteSynchronizer( message, UniqueNumber, STypeModel );

	int mesgPos = message->GetPos();
	message->WriteUChar( 0 );

	unsigned char sendState = 0;

	// Try to send new position
	{
		Vector pos = mObject->GetPosition();

		if( pos == Position )
			PositionSynchroIndex++;
		else
		{
			PositionSynchroIndex = 0;
			Position = pos;
		}

		if( PositionSynchroIndex < mClient->mHowManyUnreliableAssumedToPass )
		{
			sendState |= (1<<0);
			message->WritePositionOptimized( pos ); NPROF( NPEntityPosition );
		}

		if( PositionSynchroIndex == 255 ) PositionSynchroIndex = 254;	// no overflow needed
	}

	// Try to send new orientation
	{
		Quaternion q = mObject->GetRotation();

		if( q == Orientation )
			OrientationSynchroIndex++;
		else
		{
			OrientationSynchroIndex = 0;
			Orientation = q;
		}

		if( OrientationSynchroIndex < mClient->mHowManyUnreliableAssumedToPass )
		{
			sendState |= (1<<1);
			message->WriteQuaternionOptimized( q ); NPROF( NPEntityRotation );
		}

		if( OrientationSynchroIndex == 255 ) OrientationSynchroIndex = 254;	// no overflow needed
	}

	if( sendState == 0 )	// There was nothing to synchronize - message will not be sent this way
	{
		GEngine->Net->Profiler.Remind( NPSynchronizerNumber, cnt );
		message->Seek(startMesgPos);
		return;
	}

	message->WriteCheckToken( MODEL_UPDATE );

	int mesgPos2 = message->GetPos();
	message->Seek(mesgPos);
	message->WriteUChar(sendState); NPROF(NPPOUpdateSynchroMarker);
	message->Seek(mesgPos2);

	LastUpdateTimeLow = curTimeLow;
	LastUpdateTimeHigh = curTimeHigh;
}

void ModelSynchronizer::AddCreateObjMessage( UdpMessage* message, NetworkClient* client )
{
	GLog.Print( "Model::CREATEOBJ %d sent\n", UniqueNumber );
	Assert( mObject );
	Assert( UniqueNumber != -1 );
	WriteSynchronizer( message, UniqueNumber, STypeModel );
	message->WriteUChar( UpdateClass ); NPROF( NPUpdateClass );
	client->StringTransport.WriteString( message, mObject->FileName );
	client->StringTransport.WriteString( message, mObject->Name );
	message->WriteFloat( ((Model*)mObject)->Mesh->Scale ); NPROF(NPEntityScale);
	message->WriteULong( (unsigned long)mObject->Flags ); NPROF(NPEntityFlags);
	Vector vec = mObject->GetPosition();
	message->WritePositionOptimized( vec ); NPROF( NPEntityPosition );
	Quaternion q = mObject->GetRotation();
	message->WriteQuaternionOptimized( q ); NPROF( NPEntityRotation );

	if( mObject->SynchroState() & Entity::ESynchroAnimation )
	{
		message->WriteBool( true ); NPROF( NPModelHasAnim );
		SendAnimationInfo( message, client );
	}
	else
		message->WriteBool( false ); NPROF( NPModelHasAnim );

	char* synStr = mObject->GetSynchroString();
	if( synStr )
	{
		String s( synStr );
		message->WriteBool( true );
		client->StringTransport.WriteString( message, s );
	}
	else
		message->WriteBool( false );

	message->WriteCheckToken( MODEL_CREATE );
}

void ModelSynchronizer::AddDeleteObjMessage( UdpMessage* message, NetworkClient* client )
{
//	GLog.Print( "Model::DELETEOBJ %d sent\n", UniqueNumber );
	Assert( UniqueNumber != -1 );
	WriteSynchronizer( message, UniqueNumber, STypeModel );
	message->WriteBool( IsZombie ); NPROF(NPIsZombie);
	message->WriteCheckToken( MODEL_DELETE );
}

int ModelSynchronizer::HowManyOtherMessages( void )
{
    if( !mObject || IsZombie ) { ClearOtherMessages(); return 0; }
	return mToSend.Size();
}

void ModelSynchronizer::AddOtherObjMessage( UdpMessage* message, NetworkClient* client, int index )
{
	Assert( UniqueNumber != -1 );
	WriteSynchronizer( message, UniqueNumber, STypeModel );
	message->WriteUChar( mToSend[index].OtherMessageType ); NPROF(NPOtherTypeMarker);

	switch( mToSend[index].OtherMessageType )
	{
	case OMT_Activation:
		{
			AnimActivation* aa = (AnimActivation*)mToSend[index].Data;

			message->WriteFloat( aa->BlendTime ); NPROF( NPAnimBlendTime );
			message->WriteFloat( aa->TimeScale ); NPROF( NPAnimTimeScale );
			message->WriteBool( aa->Looped ); NPROF(NPAnimLooped);
			message->WriteChar( aa->AnimNumber ); NPROF(NPAnimNumber);
			message->WriteBool( aa->HasMovementCurve ); NPROF(NPAnimHasMoveCurve);
		}
		break;
	case OMT_Load:
		{
			AnimLoad* al = (AnimLoad*)mToSend[index].Data;

			client->StringTransport.WriteString( message, al->Name ); NPROF2(NPAnimName,al->Name.Size());
			message->WriteBool( al->Looped ); NPROF(NPAnimLooped);
			message->WriteChar( al->AnimNumber ); NPROF(NPAnimNumber);
		}
		break;
	case OMT_AnimSynchro:	// ANIMATION SYNCHRONIZATION - VERY UNOPTIMIZED
		SendAnimationInfo( message, client );
		break;
	default:
		break;
	}
	message->WriteCheckToken( MODEL_OTHER );
}

void ModelSynchronizer::SendAnimationInfo( UdpMessage* message, NetworkClient* client )
{
	Model* mModel = (Model*)mObject;
	Assert( mModel );

	// Write animations state for entity
	message->WriteUChar( (unsigned char)mModel->Mesh->Animations.Size() ); NPROF(NPAnimsCount);
	for( int i = 1; i < mModel->Mesh->Animations.Size(); ++i )
	{
		if( mModel->Mesh->Animations[i] )
		{
			if( mModel->Mesh->Animations[i]->Name.IsNull() )
				GLog.Print( "Netcode bug 1! Check it.\n" );
			client->StringTransport.WriteString( message, mModel->Mesh->Animations[i]->Name );
			message->WriteBool( mModel->Mesh->Animations[i]->Looped ); NPROF(NPAnimLooped);
			message->WriteBool( ((SkeletalAnimation*)(mModel->Mesh->Animations[i]))->HasMovementCurve()!=0 ); NPROF(NPAnimHasMoveCurve);
		}
		else
		{
			String empty = "";
			client->StringTransport.WriteString( message, empty );
		}
	}

	// Synchronize animation controller
	message->WriteUChar( (unsigned char)mModel->AnimController.Animations.Size() ); NPROF(NPActiveAnimsCount);
	for( int i = 0; i < mModel->AnimController.Animations.Size(); ++i )
	{
		int index = -1;
		for( int j = 0; j < mModel->Mesh->Animations.Size(); ++j )
		{
			if( mModel->Mesh->Animations[j] == mModel->AnimController.Animations[i].Animation )
			{
				index = j;
				break;
			}
		}
		Assert( index != -1 );
		message->WriteUChar( (unsigned char)index ); NPROF(NPActiveAnimIndex);
		message->WriteFloat( mModel->AnimController.Animations[i].Weight ); NPROF(NPActiveAnimWeight);
		message->WriteFloat( mModel->AnimController.Animations[i].TimeScale ); NPROF(NPActiveAnimTimeScale);
		message->WriteBool( mModel->AnimController.Animations[i].Looped ); NPROF(NPAnimLooped);
		message->WriteFloat( mModel->AnimController.Animations[i].FrameBlendTime ); NPROF(NPActiveAnimFrameBlendTime);
		message->WriteFloat( mModel->AnimController.Animations[i].BlendTime ); NPROF(NPActiveAnimBlendTime);
	}
}

void ModelSynchronizer::ApplyAnimationInfo( UdpMessage* message, NetworkClient* client )
{
	// Read animations state for entity and load animations
	Model* mModel = (Model*)mObject;
	Assert( mModel );
	unsigned char animSize = message->ReadUChar();
	for( int i = 1; i < animSize; ++i )
	{
		String animName = client->StringTransport.ReadString( message );
		if( !(animName.IsNull() ) )
		{
			bool looped = message->ReadBool();
			bool setMovementCurve = message->ReadBool();
			int index = mModel->LoadAnimation( animName.AsChar(), looped , i );
			if( setMovementCurve )
				mModel->SetAnimationMovementCurve( index, "ROOOT", ModelAnimation::ETransZ );
		}
	}

	// Synchronize animation controller
	mModel->AnimController.Animations.Clear();
	int activeAnimSize = message->ReadUChar();
	for( int i = 0; i < activeAnimSize; ++i )
	{
		mModel->AnimController.Animations.GrowArray(1);
		mModel->AnimController.Animations[i].Animation = mModel->Mesh->Animations[ message->ReadUChar() ];
		mModel->AnimController.Animations[i].Weight = message->ReadFloat();
		mModel->AnimController.Animations[i].TimeScale = message->ReadFloat();
		mModel->AnimController.Animations[i].Looped = message->ReadBool();
		mModel->AnimController.Animations[i].FrameBlendTime = message->ReadFloat();
		mModel->AnimController.Animations[i].BlendTime = message->ReadFloat();
	}
}

void ModelSynchronizer::DumpAnimationInfo( UdpMessage* message, NetworkClient* client )
{
	// Read animations state for entity and load animations
	unsigned char animSize = message->ReadUChar();
	for( int i = 1; i < animSize; ++i )
	{
		String animName = client->StringTransport.ReadString( message );
		if( !(animName.IsNull() ) ) { message->ReadBool(); message->ReadBool(); }
	}

	int activeAnimSize = message->ReadUChar();
	for( int i = 0; i < activeAnimSize; ++i ) { message->ReadUChar(); message->ReadFloat(); message->ReadFloat(); message->ReadBool(); message->ReadFloat(); message->ReadFloat(); }
}

void ModelSynchronizer::ClearOtherMessages( void )
{
	for( int i = 0; i < mToSend.Size(); ++i )
		switch( mToSend[i].OtherMessageType )
		{
		case OMT_Activation:
			delete (AnimActivation*)mToSend[i].Data;
			break;
		case OMT_Load:
			delete (AnimLoad*)mToSend[i].Data;
			break;
		default:
			break;
		}
	mToSend.Clear();
}

bool ModelSynchronizer::ModelEqual( String& filename, String& name, float scale )
{
	String modelFileName = mObject->FileName.Len()?mObject->FileName:"";
	String modelName = mObject->Name.Len()?mObject->Name:"";
	return( (modelFileName == filename) && (modelName == name) && (((Model*)mObject)->Mesh->Scale == scale) );
}

bool ModelSynchronizer::InterpretMessage( UdpMessage* message, unsigned char command, NetworkClient* client )
{
	switch( command )
	{
	case NetworkClient::CREATEOBJ:
		{
			UpdateClass = message->ReadUChar();
			String filename = client->StringTransport.ReadString( message );
			String entityName = client->StringTransport.ReadString( message );
			float scale = message->ReadFloat();
			DWORD flags = (DWORD)message->ReadULong();
			Vector t = message->ReadPositionOptimized();
			Quaternion q = message->ReadQuaternionOptimized();
			bool animationsFollow = message->ReadBool();

#ifdef REPORT_CREATION_DELETION
			GLog.Print( "Model::CREATEOBJ %d: file = %s, name = %s, scale = %f\n",
				UniqueNumber, filename.AsChar(), entityName.AsChar(), scale );
#endif

			if( mObject )
			{
				GLog.Print( "Tried to create again exisitng object!!!\n" );
				GLog.Flush();
				return false;
			}
/*
			// Check if we have to remove old model
			if( mObject && !ModelEqual( filename, entityName, scale ) )
			{
				Assert( !mRagdoll );
				Assert( !mPO );
				RemoveConnectionWithEntity( mObject );
				GEngine->World->DeleteEntityDelayed( mObject );
				mObject = 0;
			}
*/
			// Check if we have to make a new one
			if( !mObject )
			{
				// Create new entity with those contents
				mObject = (Model*)GEngine->World->CreateEntity( Entity::ETypeModel, filename.AsChar(), entityName.AsChar(), scale );
				if( !mObject )
				{
					GLog.Print( "Was unable to create model! filename = %s, entityName = %s, scale = %f, unique = %d\n",
						filename.AsChar(), entityName.AsChar(), scale, UniqueNumber );
					message->DumpToLogfile( "Unable to create model" );

					// Dump rest of model data
					if( animationsFollow )
						DumpAnimationInfo( message, client );
		            String synStr = "";
					if( message->ReadBool() )
						synStr = client->StringTransport.ReadString( message );

					message->ReadCheckToken( MODEL_CREATE );

					return true;
				}
				GEngine->World->AddEntity( mObject );
			}

			MakeConnectionWithEntity( mObject );

			mObject->Flags = flags;
			mObject->SetPositionAndRotation( t, q );

			if( animationsFollow )
				ApplyAnimationInfo( message, client );

			String synStr = "";
			if( message->ReadBool() )
				synStr = client->StringTransport.ReadString( message );

			message->ReadCheckToken( MODEL_CREATE );

			if( synStr.Len() )
			{
				mObject->SetSynchroString(synStr.AsChar());
				GScript.QueueMessage("ENTITY_CREATE","%e,%s",mObject,synStr.AsChar());
			}
		}
		return true;
	case NetworkClient::DELETEOBJ:
#ifdef REPORT_CREATION_DELETION
		GLog.Print( "Model::DELETEOBJ %d\n", UniqueNumber );
#endif
		if( mObject )
		{
			char* str = mObject->GetSynchroString();
			if( str && ::strlen(str) )
   				GScript.QueueMessage( "ENTITY_DELETE","%e,%s", mObject, str );

			RemoveConnectionWithEntity( mObject );
			GEngine->World->DeleteEntityDelayed( mObject );
			mObject = 0;
		}
		IsZombie = message->ReadBool();
		message->ReadCheckToken( MODEL_DELETE );
		GLog.Print( "Model::DELETEOBJ FINISHED %d\n", UniqueNumber );
		return true;
	case NetworkClient::UPDATE:
		{
			unsigned char sendState = message->ReadUChar();

			if( sendState & (1<<0) )
				Position = message->ReadPositionOptimized();

			if( sendState & (1<<1) )
				Orientation = message->ReadQuaternionOptimized();

			message->ReadCheckToken( MODEL_UPDATE );

			if( mObject )
			{
				switch( sendState )
				{
				case 0:	// none
				default:
					break;
				case 1:	// only position
					mObject->SetPosition( Position );
					break;
				case 2:	// only orientation
					mObject->SetRotation( Orientation );
					break;
				case 3:	// both
					mObject->SetPositionAndRotation( Position, Orientation );
					break;
				}
			}
		}
		return true;
	case NetworkClient::OTHEROBJ:
		{
			int type = message->ReadUChar();
			switch( type )
			{
			case OMT_Activation:
				{
					float blendTime = message->ReadFloat();
					float timeScale = message->ReadFloat();
					bool looped = message->ReadBool();
					int animation = (int)message->ReadChar();
					bool setMovementCurve = message->ReadBool();
					Assert( animation != -1 );

					if( setMovementCurve && !((SkeletalAnimation*)(((Model*)mObject)->Mesh->Animations[animation]))->HasMovementCurve() )
						((Model*)mObject)->SetAnimationMovementCurve( animation, "ROOOT", ModelAnimation::ETransZ );
					if( animation == 0 )
						((Model*)mObject)->SetRagdoll( true, HCGInsideItems );
					else
						((Model*)mObject)->SetRagdoll( false, HCGInsideItems );
					((Model*)mObject)->ActivateAnimation( animation, looped, timeScale, blendTime );
				}
				message->ReadCheckToken( MODEL_OTHER );
				return true;
			case OMT_Load:
				{
					String name = client->StringTransport.ReadString( message );
					bool looped = message->ReadBool();
					int animation = (int)message->ReadChar();

					((Model*)mObject)->LoadAnimation( name.AsChar(), looped, animation );
				}
				message->ReadCheckToken( MODEL_OTHER );
				return true;
			case OMT_AnimSynchro:
				ApplyAnimationInfo( message, client );
				message->ReadCheckToken( MODEL_OTHER );
				return true;
			default:
				GLog.Print( "Warning! Unexpected message type in OTHEROBJ to be recognized by Model! Network stream bugs may follow!\n" );
				message->DumpToLogfile( "Mssage type OTHEROBJ:default given to Model" );
				return false;
			}
		}
		return true;
	default:
		return false;
	}
}

void ModelSynchronizer::SetZombie( void )
{
	if( mPO ) mPO->SetZombie();
//	if( mRagdoll ) mRagdoll->SetZombie();
	ObjectSynchronizer::SetZombie();
}

void ModelSynchronizer::AddAnimActivation( int animNumber, bool looped, float blendTime, float timeScale, bool hasMovementCurve )
{
	if( !mObject || !( mObject->SynchroState() & Entity::ESynchroAnimation ) ) return;

	Assert( animNumber != -1 );
    m_setTag("Net");
	AnimActivation* aa = new AnimActivation;
	aa->AnimNumber = (unsigned char)animNumber;
	aa->Looped = looped;
	aa->BlendTime = blendTime;
	aa->TimeScale = timeScale;
	aa->HasMovementCurve = hasMovementCurve;

	SendStruct ss;
	ss.OtherMessageType = OMT_Activation;
	ss.Data = (void*)aa;

	mToSend.Add(ss);
}

void ModelSynchronizer::AddAnimLoad( String name, bool looped, int animIndex )
{
	if( !mObject || !( mObject->SynchroState() & Entity::ESynchroAnimation ) ) return;

    m_setTag("Net");
	AnimLoad* al = new AnimLoad;
	al->Name = name;
	al->Looped = looped;
	al->AnimNumber = (unsigned char)animIndex;

	SendStruct ss;
	ss.OtherMessageType = OMT_Load;
	ss.Data = (void*)al;

	mToSend.Add(ss);
}

ModelSynchronizer::~ModelSynchronizer()
{
	ClearOtherMessages();

	if( mPO )
	{
		mPO->SetZombie();
		mPO->mEntity = 0;
	}
/*
	if( mRagdoll )
	{
		mRagdoll->SetZombie();
		mRagdoll->mModel = 0;
	}
*/
}
// ***************************************************************************************
#define PO_SAME_POSITION_TOLERANCE (0.05f*0.05f)
#define PO_SAME_ROTATION_TOLERANCE 0.999f
#define PO_PLAYER_SYNCHRO_NO_LESS_THAN 1500

POSynchronizer::POSynchronizer( unsigned long uniqueNumber, NetworkClient* client, PhysicsObject* poToSynchronize, EntitySynchronizer* entitySynchro )
:	ObjectSynchronizer( uniqueNumber, client ), mObject( poToSynchronize ), mEntity( entitySynchro )
,	CollisionGroup( 127 ), CollisionGroupSynchroIndex( 0 ), PositionSynchroIndex( 0 )
,	OrientationSynchroIndex( 0 ), Position( 0, 0, 0 ), Orientation()
,	EntityGIndex(0), Saved(false)
{
	if( entitySynchro ) EntityGIndex = entitySynchro->mObject->GIndex;
	if( poToSynchronize )
	{
		mObject->SetSynchronizedProp();
		PostCreateMessage();
		for( int i = 0; i < POHavokPart::EActionLast; ++i )
			if( poToSynchronize->mHavokPart->GetAction( i ) ) AddActionUpdate( i );
	}
}

void POSynchronizer::Maintain( void )
{
	if( IsZombie ) return;

	Assert( mEntity );
	Assert( mEntity->mObject );

	// Check if we have to create or delete PO on the other side
	if( mEntity->mObject->PO && !mObject )
	{
		mObject = mEntity->mObject->PO;
		mObject->SetSynchronizedProp();
		PostCreateMessage();
	}
	else if( mObject && !mEntity->mObject->PO )
	{
//		mObject = 0;		// Can't do this - we need this pointer for RemoveZombieObjectSynchronizers()
		ClearOtherMessages();
		PostDeleteMessage();
		SetZombie();
	}
}

void POSynchronizer::ClientMaintain( void )
{
	if( IsZombie || !mObject || !mEntity || !mEntity->mObject ) return;
	if( mObject->GetType() == EPawnBodyPlayer ) return;

	DWORD curTimeLow = 0;
	DWORD curTimeHigh = 0;
	GEngine->SysDriver.GetPureTickCount( curTimeLow, curTimeHigh );
	DWORD diffLow, diffHigh;
	GetTickCountDifference( LastUpdateTimeLow, LastUpdateTimeHigh, curTimeLow, curTimeHigh, diffLow, diffHigh );

	DWORD fourSecondsLow, fourSecondsHigh, fourSeconds = 3*1000/GEngine->Physics->PhysicsFPS;
	GEngine->SysDriver.MilisecondsToTickCount( fourSeconds, fourSecondsLow, fourSecondsHigh );

	/*
	static long lastTime1 = GEngine->SysDriver.GetCurrentTime();
	long currTime = GEngine->SysDriver.GetCurrentTime();
	GLog.Print("time delta1: %f\n",(currTime-lastTime1)/1000.0f);
	lastTime1 = currTime;
	*/

	if( TickCount_SecondHigherThanFirst( diffLow, diffHigh, fourSecondsLow, fourSecondsHigh ) )
		return;

	/*
	static long lastTime2 = GEngine->SysDriver.GetCurrentTime();
	GLog.Print("time delta2: %f\n",(currTime-lastTime2)/1000.0f);
	lastTime2 = currTime;
	*/

/*
	Vector vel = mObject->GetVel();
	double speed = vel.Magnitude();
	if( speed > 20.0f )
		return;
*/
	Vector pos = mObject->GetPosition();
	double diffPos = ( Position - pos ).Magnitude();

	if( diffPos > 10.0*PO_SAME_POSITION_TOLERANCE )
	{
		mObject->SetRotQuaternion( Orientation );
		mObject->SetPosition( Position, true );
	}
	LastUpdateTimeLow = curTimeLow;
	LastUpdateTimeHigh = curTimeHigh;
}

void POSynchronizer::AddUpdateObjMessage( UdpMessage* message )
{
	if( IsZombie || !mObject ) return;

	if( GEngine->World->Physics->GetPhysicsFrame() == mLastFrameSent ) return;
	mLastFrameSent = GEngine->World->Physics->GetPhysicsFrame();

	Assert( mEntity );
	Assert( mEntity->mObject );

	bool isPlayer = ( mObject->GetType() == EPawnBodyPlayer );

	bool canSend = isPlayer;

	UpdateClassParams* updateParams = mClient->ConnectionParams.Find(UpdateClass);
	DWORD curTimeLow = 0;
	DWORD curTimeHigh = 0;

	if( !isPlayer )
	{
		GEngine->SysDriver.GetPureTickCount( curTimeLow, curTimeHigh );
		if( updateParams && updateParams->MinSynchroDelay )
		{
			DWORD diffLow, diffHigh;
			GetTickCountDifference( LastUpdateTimeLow, LastUpdateTimeHigh, curTimeLow, curTimeHigh, diffLow, diffHigh );
			if( TickCount_SecondHigherThanFirst( updateParams->MinSynchroDelayLow, updateParams->MinSynchroDelayHigh, diffLow, diffHigh ) )
				canSend = true;
		}
	}

	if( !canSend ) return;

	int startMesgPos = message->GetPos();

	unsigned long cnt = GEngine->Net->Profiler.Remember( NPSynchronizerNumber );

	Assert( UniqueNumber != -1 );
	Assert( mEntity->UniqueNumber != -1 );
	WriteSynchronizer( message, UniqueNumber, STypePO );
	WriteSynchronizer( message, mEntity->UniqueNumber, mEntity->GetType() );

	int mesgPos = message->GetPos();
	message->WriteUChar( 0 );

	unsigned char sendState = 0;

	Vector pos = mObject->GetPosition();
	Quaternion q = mObject->GetRotQuaternion();
	Vector vel = mObject->GetVel();
	Vector velAng = mObject->GetAngularVel();

	if( !isPlayer )
	{
		// Try to send new position
		{
			if( (pos-Position).MagnitudeSquared() < (updateParams?(updateParams->MinPositionDiff*updateParams->MinPositionDiff):PO_SAME_POSITION_TOLERANCE) )
				PositionSynchroIndex++;
			else
			{
				PositionSynchroIndex = 0;
				Position = pos;
			}

			if( PositionSynchroIndex < mClient->mHowManyUnreliableAssumedToPass )
				sendState |= (1<<0);
			if( PositionSynchroIndex == 255 ) PositionSynchroIndex = 254;	// no overflow needed
		}

		// Try to send new rotation
		{
			float dot = q.Dot(Orientation);
			if( dot > PO_SAME_ROTATION_TOLERANCE )
				OrientationSynchroIndex++;
			else
			{
				OrientationSynchroIndex = 0;
				Orientation = q;
			}

			if( OrientationSynchroIndex < mClient->mHowManyUnreliableAssumedToPass )
				sendState |= (1<<1);

			if( OrientationSynchroIndex == 255 ) OrientationSynchroIndex = 254;	// no overflow needed
		}

		if( vel.Magnitude() > (updateParams?updateParams->MinLinearVelocityDiff:0.001f) )
			sendState |= (1<<2);

		if( velAng.Magnitude() > (updateParams?updateParams->MinAngularVelocityDiff:0.001f) )
			sendState |= (1<<3);
	}
	else		// Is a player
		sendState |= (1<<5);	// special for player

	int newCG = mObject->GetCollisionGroup();
	if( CollisionGroup != newCG )
	{
		CollisionGroup = newCG;
		CollisionGroupSynchroIndex = 5;
	}

	if( CollisionGroupSynchroIndex > 0 )
	{
		CollisionGroupSynchroIndex--;
		sendState |= (1<<4);
	}

	// Write them
	if( !isPlayer )
	{
		if( sendState ) { message->WritePositionOptimized( pos ); NPROF( NPPOPosition ); sendState |= (1<<0); }
		if( sendState & (1<<1) ) { message->WriteQuaternionOptimized2( q ); }
		if( sendState & (1<<2) ) { message->WriteLinearVelocityOptimized( vel ); NPROF( NPPOVelocity ); }
		if( sendState & (1<<3) ) { message->WriteAngularVelocityOptimized( velAng ); NPROF( NPPOAngularVelocity ); }
		if( sendState & (1<<4) ) { message->WriteUChar( CollisionGroup ); NPROF(NPCollGroupIndex); }
	}
	else
	{
		message->WritePositionOptimized( pos ); NPROF( NPPOPosition );
		message->WriteQuaternionOptimized2( q );
		message->WriteLinearVelocityOptimized( vel ); NPROF( NPPOVelocity );
		if( sendState & (1<<4) ) { message->WriteUChar( CollisionGroup ); NPROF(NPCollGroupIndex); }
	}

	if( !sendState )	// There was nothing to synchronize - message will not be sent this way
	{
		GEngine->Net->Profiler.Remind( NPSynchronizerNumber, cnt );
		message->Seek(startMesgPos);
		return;
	}

	message->WriteCheckToken( PHO_UPDATE );

	int mesgPos2 = message->GetPos();
	message->Seek(mesgPos);
	message->WriteUChar(sendState); NPROF(NPPOUpdateSynchroMarker);
	message->Seek(mesgPos2);

	if( !isPlayer )
	{
		LastUpdateTimeLow = curTimeLow;
		LastUpdateTimeHigh = curTimeHigh;
	}
}

void POSynchronizer::AddCreateObjMessage( UdpMessage* message, NetworkClient* client )
{
	GLog.Print( "PO::CREATEOBJ %d sent\n", UniqueNumber );
	Assert( mEntity && mEntity->mObject && !IsZombie && ( UniqueNumber != -1 ) && ( mEntity->UniqueNumber != -1 ) );

	WriteSynchronizer( message, UniqueNumber, STypePO );
	WriteSynchronizer( message, mEntity->UniqueNumber, mEntity->GetType() );

	PONetCreationEntry entry;
	entry.UpdateClass = UpdateClass;
	entry.ObjectState = mObject->State;
	entry.BodyType = mObject->BodyType;
	entry.BodyScale = mObject->BodyScale;
	entry.FreedomOfRotation = mObject->GetFreedomOfRotation();
	entry.Mass = mObject->GetMass();
	entry.Friction = mObject->GetFriction();
	entry.Restitution = mObject->GetRestitution();

	CollisionGroup = mObject->GetCollisionGroup();
	entry.CollisionGroup = CollisionGroup;
	CollisionGroupSynchroIndex = 0;

#ifdef _DEBUG
	int sent = client->POCreationCache.SendPOCreationData( message, entry ); NPROF2( NPPOCreationData, sent );
#else
	client->POCreationCache.SendPOCreationData( message, entry );
#endif

	Position = mEntity->mObject->GetPosition();
	message->WritePositionOptimized( Position ); NPROF( NPPOPosition );
	PositionSynchroIndex = 0;

	Orientation = mEntity->mObject->GetRotation();
	message->WriteQuaternionOptimized( Orientation ); NPROF2(NPPORotation,6);
	OrientationSynchroIndex = 0;

	Vector velocity = mObject->GetVel();
	message->WriteLinearVelocityOptimized( velocity ); NPROF( NPPOVelocity );

	Vector angularVelocity = mObject->GetAngularVel();
	message->WriteAngularVelocityOptimized( angularVelocity ); NPROF( NPPOAngularVelocity );

	message->WriteBool( mObject->IsPinned() );

	message->WriteCheckToken( PHO_CREATE );
}

void POSynchronizer::AddDeleteObjMessage( UdpMessage* message, NetworkClient* client )
{
//	GLog.Print( "PO::DELETEOBJ %d sent. Entity %d, type %d \n", UniqueNumber, mEntity->UniqueNumber, mEntity->GetType() );
	Assert( ( UniqueNumber != -1 ) && ( mEntity->UniqueNumber != -1 ) );

	WriteSynchronizer( message, UniqueNumber, STypePO );
	WriteSynchronizer( message, mEntity->UniqueNumber, mEntity->GetType() );
	message->WriteBool( IsZombie ); NPROF(NPIsZombie);
	message->WriteCheckToken( PHO_DELETE );
}

bool POSynchronizer::InterpretMessage( UdpMessage* message, unsigned char command, NetworkClient* client )
{
	unsigned long entityNumber;
	unsigned char entityType;
	ObjectSynchronizer::ReadSynchronizer( message, entityNumber, entityType );

	Assert( !mEntity || ( mEntity->UniqueNumber == entityNumber ) );

	if( mEntity && ( mEntity->UniqueNumber != entityNumber ) )
	{
		GLog.Print( "Different entity came for existing PO!!!\n" );
		GLog.Print( "Existing entity %d, new %d\n", mEntity->UniqueNumber, entityNumber );
		return false;
	}

	if( !mEntity )
	{
		mEntity = (EntitySynchronizer*)client->SureGetEntitySynchronizer( entityNumber, entityType );
		mEntity->mPO = this;
	}

	// New prediction code!!!
	if( !mClient->mSpectator )
	{
		if( !GEngine->Net->LoadedInPreTick )
		{
			GEngine->Net->LoadedInPreTick = true;
			GEngine->Net->UpdatesSinceLoaded = 0;
			GEngine->World->Physics->MPPredSaveRagdolls();
			GEngine->World->Physics->MPPredRemoveAllFromWorld();
		}

		// From now on we'll be simulating this object too - if it's not a player
		if( mObject )
			mObject->MPPredRemoveFromBatch( ( command != NetworkClient::DELETEOBJ ) );
	}

	switch( command )
	{
	case NetworkClient::CREATEOBJ:
		{
			PONetCreationEntry entry;
			client->POCreationCache.ReceivePOCreationData( message, entry );

			UpdateClass = entry.UpdateClass;

			Vector pos = message->ReadPositionOptimized();
			Quaternion rot = message->ReadQuaternionOptimized();
			Vector vel = message->ReadLinearVelocityOptimized();
			Vector velAng = message->ReadAngularVelocityOptimized();

			bool pinned = message->ReadBool();

			message->ReadCheckToken( PHO_CREATE );

#ifdef REPORT_CREATION_DELETION
			GLog.Print( "PO::CREATEOBJ %d: entity %d, file = %s, name = %s\n",
				UniqueNumber, mEntity->UniqueNumber, mEntity->mObject->FileName.AsChar(), mEntity->mObject->Name.AsChar() );
#endif

			Assert( mEntity->mObject );
			mEntity->mObject->SetPositionAndRotation( pos, rot );
			if( !mEntity->mObject->PO )
				mEntity->mObject->PO = GEngine->World->Physics->CreatePhysicsObject( pos, rot, entry.BodyType, mEntity->mObject, entry.BodyScale, entry.CollisionGroup );

			mObject = mEntity->mObject->PO;
			mObject->SetSynchronizedProp();

			Position = mObject->GetPosition();
			Orientation = mObject->GetRotQuaternion();

			mObject->State = entry.ObjectState;

			mObject->SetFreedomOfRotation( entry.FreedomOfRotation );
			mObject->SetVel( vel );
			mObject->SetAngularVel( velAng );
			CollisionGroup = entry.CollisionGroup;

			if( entry.Mass != mObject->GetMass() ) mObject->SetMass( entry.Mass );
			if( entry.Friction != mObject->GetFriction() ) mObject->SetFriction( entry.Friction );
			if( entry.Restitution != mObject->GetRestitution() ) mObject->SetRestitution( entry.Restitution );

			mObject->SetPinned( pinned );

			char* synStr = mEntity->mObject->GetSynchroString();
			if( synStr )
				GScript.QueueMessage("PO_CREATE","%e,%p,%s",mEntity->mObject,mObject,synStr);
		}
		return true;
	case NetworkClient::DELETEOBJ:
#ifdef REPORT_CREATION_DELETION
		GLog.Print( "PO::DELETEOBJ %d\n", UniqueNumber );
#endif
		if( mEntity && mEntity->mObject )
			mEntity->mObject->RemovePhysicsObject();
		// else assume it's deleted
		mObject = 0;
		IsZombie = message->ReadBool();
		message->ReadCheckToken( PHO_DELETE );
		return true;
	case NetworkClient::UPDATE:
		{
			// New prediction code!!!
			if( !mClient->mSpectator )
				client->SaveTime = client->LastSaveSent;
			GEngine->Net->UpdatesSinceLoaded++;

			unsigned char sendState = message->ReadUChar();

			UpdateClassParams* updateParams = 0;
			if( sendState & 0x3F )
				updateParams = mClient->ConnectionParams.Find(UpdateClass);

			Vector newPos;
			Quaternion newOrient;
			Vector vel;
			Vector velAng;
			unsigned char newCG = 0;

			if( sendState & (1<<5) )
			{
				sendState |= 0x0F;
				newPos = message->ReadPositionOptimized();
				newOrient = message->ReadQuaternionOptimized2();
				vel = message->ReadLinearVelocityOptimized();
				velAng = Vector(0,0,0);
/*
				GLog.Print( "%d: (%f,%f,%f), (%f,%f,%f) from net, frame: %d\n",
					mEntity->mObject->GIndex,
					newPos.X, newPos.Y, newPos.Z,
					vel.X, vel.Y, vel.Z,
					client->LastSaveSent );
*/
			}
			else
			{
				if( sendState & (1<<0) ) newPos = message->ReadPositionOptimized();
				if( sendState & (1<<1) ) newOrient = message->ReadQuaternionOptimized2();
				if( sendState & (1<<2) ) vel = message->ReadLinearVelocityOptimized();
				if( sendState & (1<<3) ) velAng = message->ReadAngularVelocityOptimized();
			}
			if( sendState & (1<<4) ) newCG = message->ReadUChar();

			message->ReadCheckToken( PHO_UPDATE );

			if( sendState & (1<<1) )
				if( mObject )
				{
					Orientation = newOrient;
					mObject->SetRotQuaternion( Orientation );
					GEngine->SysDriver.GetPureTickCount( LastUpdateTimeLow, LastUpdateTimeHigh );
				}

			if( sendState & (1<<0) )
			{
				if( mObject )
				{
    				Position = newPos;
	    			mObject->SetPosition( Position );
//					GLog.Print( "Player server pos: (%f,%f,%f), set at %d\n", Position.X, Position.Y, Position.Z, client->SaveTime );
					GEngine->SysDriver.GetPureTickCount( LastUpdateTimeLow, LastUpdateTimeHigh );
                }
			}

			if( sendState & (1<<2) )
				if( mObject )
					mObject->SetVel( vel );

			if( sendState & (1<<3) )
				if( mObject )
					mObject->SetAngularVel( velAng );

			if( sendState & (1<<4) )
			{
				if( mObject && ( CollisionGroup != newCG ) )
				{
					mObject->SetCollisionGroup( newCG );
					CollisionGroup = newCG;
				}
			}
		}
		return true;
	case NetworkClient::OTHEROBJ:
		{
			unsigned char howMany = message->ReadUChar();
			for( int i = 0; i < howMany; ++i )
				mObject->mHavokPart->ReadAction( message );
			message->ReadCheckToken( PHO_OTHER );
		}
		return true;
	default:
		GLog.Print( "Warning! Unexpected message type to be recognized by PO! Network stream bugs may follow!\n" );
		message->DumpToLogfile( "Message type default given to PO" );
		return false;
	}
}

POSynchronizer::~POSynchronizer()
{
	ClearOtherMessages();
	if( mEntity )
		mEntity->mPO = 0;
}

int POSynchronizer::HowManyOtherMessages( void )
{
	if( !mObject || IsZombie ) ClearOtherMessages();
	return (mActionUpdates.Size() != 0)?1:0;
}

void POSynchronizer::ClearOtherMessages( void )
{
	mActionUpdates.Clear();
}

void POSynchronizer::AddOtherObjMessage( UdpMessage* message, NetworkClient* client, int index )
{
	Assert( mObject );
	Assert( UniqueNumber != -1 );
	Assert( mEntity->UniqueNumber != -1 );
	WriteSynchronizer( message, UniqueNumber, STypePO );
	WriteSynchronizer( message, mEntity->UniqueNumber, mEntity->GetType() );
	message->WriteUChar( mActionUpdates.Size() ); NPROF(NPPOActionCount);
	for( int i = 0; i < mActionUpdates.Size(); ++i )
		mObject->mHavokPart->WriteAction( mActionUpdates[i], message );
	message->WriteCheckToken( PHO_OTHER );
}

void POSynchronizer::SaveState( void )
{
	if( mObject )
	{
		SavePos = mObject->GetPosition();
		Assert( !( ( SavePos.X < 1 ) && ( SavePos.X > 1 ) ) );
		SaveOrient = mObject->GetRotQuaternion();
		SaveVel = mObject->GetVel();
		Assert( !( ( SaveVel.X < 1 ) && ( SaveVel.X > 1 ) ) );
		SaveAngVel = mObject->GetAngularVel();
	}
	Saved = (mObject!=0);
}

void POSynchronizer::RestoreState( void )
{
	if( !Saved || !mObject ) return;
	mObject->SetRotQuaternion( SaveOrient );
	mObject->SetPosition( SavePos );
	mObject->SetVel( SaveVel );
	mObject->SetAngularVel( SaveAngVel );
}
/*
// ***************************************************************************************
#define RAGDOLL_SAME_POSITION_TOLERANCE (0.05f*0.05f)
#define RAGDOLL_SAME_ROTATION_TOLERANCE 0.999f

RagdollSynchronizer::RagdollSynchronizer( unsigned long uniqueNumber, NetworkClient* client, Ragdoll* ragdollToSynchronize, ModelSynchronizer* modelSynchro )
:	ObjectSynchronizer( uniqueNumber, client ), mObject( ragdollToSynchronize ), mModel( modelSynchro ), mDoesNotSynchroAnim(false)
,	EntityGIndex(0)
{
	if( modelSynchro )
		EntityGIndex = modelSynchro->mObject->GIndex;
	if( ragdollToSynchronize )
		PostCreateMessage();
}

void RagdollSynchronizer::Maintain( void )
{
	if( IsZombie ) return;

	Assert( mModel );
	Assert( mModel->mObject );
	Model* model = (Model*)mModel->mObject;

	// Check if we have to create or delete ragdoll on the other side
	if( model->RagdollPtr && model->RagDoll && !mObject )
	{
		mObject = model->RagdollPtr;
		Assert( mObject );
		PostCreateMessage();
	}
	else if( mObject && ( !model->RagdollPtr || !model->RagDoll ) )
	{
//		mObject = 0;	// Can't do this - we need this pointer for RemoveZombieObjectSynchronizers()
		ClearOtherMessages();
		PostDeleteMessage();
		SetZombie();
	}
}

void RagdollSynchronizer::AddUpdateObjMessage( UdpMessage* message )
{
	if( IsZombie || !mObject ) return;

	if( GEngine->World->Physics->GetPhysicsFrame() == mLastFrameSent ) return;
	mLastFrameSent = GEngine->World->Physics->GetPhysicsFrame();

	Assert( mModel );
	Assert( mModel->mObject );

	if( !(mModel->mObject->SynchroState() & (Entity::ESynchroRagdoll|Entity::ESynchroRagdollCreation)) )
		return;	// don't synchronize ragdoll

	DWORD curTimeLow = 0;
	DWORD curTimeHigh = 0;
	GEngine->SysDriver.GetPureTickCount( curTimeLow, curTimeHigh );
	UpdateClassParams* updateParams = mClient->ConnectionParams.Find(UpdateClass);
	if( updateParams && updateParams->MinSynchroDelay )
	{
		DWORD diffLow, diffHigh;
		GetTickCountDifference( LastUpdateTimeLow, LastUpdateTimeHigh, curTimeLow, curTimeHigh, diffLow, diffHigh );
		if( !TickCount_SecondHigherThanFirst( updateParams->MinSynchroDelayLow, updateParams->MinSynchroDelayHigh, diffLow, diffHigh ) )
			return;
	}

	Assert( UniqueNumber != -1 );
	WriteSynchronizer( message, UniqueNumber, STypeRagdoll );
	char c = message->WriteULongOptimized( mModel->UniqueNumber + 1 ); NPROF2(NPSynchronizerNumber,c);

	// Synchronize ragdoll update

	// Get ragdoll update state
	DynamicArray<Vector> positions;
	DynamicArray<Quaternion> rotations;
	mObject->GetLimbsPositionsAndRotations( positions, rotations );

	Assert( positions.Size() < 29 );
	Assert( mLimbPositions.Size() == positions.Size() );

	// Gather information, which positions we synchronize this time
	unsigned long limbPositionStates = 0;
	for( int i = 0; i < positions.Size(); ++i )
	{
		if( (positions[i]-mLimbPositions[i]).MagnitudeSquared() < RAGDOLL_SAME_POSITION_TOLERANCE )
			mLimbPositionsUnchanged[i]++;
		else
		{
			mLimbPositionsUnchanged[i] = 0;
			mLimbPositions[i] = positions[i];
		}

		if( mLimbPositionsUnchanged[i] < mClient->mHowManyUnreliableAssumedToPass )
			limbPositionStates |= (1<<i);

		if( mLimbPositionsUnchanged[i] == 255 )
			mLimbPositionsUnchanged[i]--;	// no overflow needed
	}

	// Send position data
	message->WriteStateOptimized( limbPositionStates ); NPROF(NPRagdollLimbState);
	for( int i = 0; i < positions.Size(); ++i )
		if( mLimbPositionsUnchanged[i] < mClient->mHowManyUnreliableAssumedToPass )
			message->WritePositionOptimized( positions[i] ); NPROF(NPRagdollLimbPosition);

	// Gather information, which rotations we synchronize this time
	unsigned long limbRotationStates = 0;
	for( int i = 0; i < rotations.Size(); ++i )
	{
		float dot = fabs( rotations[i].Dot(mLimbRotations[i]) );
		if( dot > RAGDOLL_SAME_ROTATION_TOLERANCE )
			mLimbRotationsUnchanged[i]++;
		else
		{
			mLimbRotationsUnchanged[i] = 0;
			mLimbRotations[i] = rotations[i];
		}

		if( mLimbRotationsUnchanged[i] < mClient->mHowManyUnreliableAssumedToPass )
			limbRotationStates |= (1<<i);

		if( mLimbRotationsUnchanged[i] == 255 )
			mLimbRotationsUnchanged[i]--;	// no overflow needed
	}

	// Send rotation data
	message->WriteStateOptimized( limbRotationStates ); NPROF(NPRagdollLimbState);
	for( int i = 0; i < rotations.Size(); ++i )
		if( mLimbRotationsUnchanged[i] < mClient->mHowManyUnreliableAssumedToPass )
			message->WriteQuaternionOptimized( rotations[i] ); NPROF(NPRagdollLimbOrientation);

	LastUpdateTimeLow = curTimeLow;
	LastUpdateTimeHigh = curTimeHigh;
}

void RagdollSynchronizer::AddCreateObjMessage( UdpMessage* message, NetworkClient* client )
{
//	GLog.Print( "Ragdoll::CREATEOBJ %d sent\n", UniqueNumber );

	Assert( mModel && mModel->mObject && !IsZombie );

	Assert( UniqueNumber != -1 );
	WriteSynchronizer( message, UniqueNumber, STypeRagdoll );
	char c = message->WriteULongOptimized( mModel->UniqueNumber + 1 ); NPROF2(NPSynchronizerNumber,c);
	message->WriteUChar( UpdateClass ); NPROF( NPUpdateClass );

	// Send create ragdoll data

	// Get ragdoll data
	RagdollData data;
	mObject->FillRagdollData( data );

	// Fill mLimbPositions and mLimbRotations with current data
	mLimbPositions.Clear();
	mLimbPositionsUnchanged.Clear();
	for( int i = 0; i < data.LimbPositions.Size(); ++i )
	{
		mLimbPositions.Add( data.LimbPositions[i] );
		mLimbPositionsUnchanged.Add(0);
	}

	mLimbRotations.Clear();
	mLimbRotationsUnchanged.Clear();
	for( int i = 0; i < data.LimbOrientations.Size(); ++i )
	{
		mLimbRotations.Add( data.LimbOrientations[i] );
		mLimbRotationsUnchanged.Add(0);
	}

	// Send position and rotation data
	message->WriteUChar( mLimbPositions.Size() ); NPROF(NPRagdollLimbCount);
	for( int i = 0; i < mLimbPositions.Size(); ++i )
		message->WritePositionOptimized( mLimbPositions[i] ); NPROF(NPRagdollLimbPosition);
	for( int i = 0; i < mLimbPositions.Size(); ++i )
		message->WriteQuaternionOptimized( mLimbRotations[i] ); NPROF(NPRagdollLimbOrientation);

	// Send linear and angular velocities data
	for( int i = 0; i < mLimbPositions.Size(); ++i )
		message->WriteLinearVelocityOptimized( data.LimbLinearVelocities[i] ); NPROF(NPRagdollLimbVelocity);
	for( int i = 0; i < mLimbPositions.Size(); ++i )
		message->WriteAngularVelocityOptimized( data.LimbAngularVelocities[i] ); NPROF(NPRagdollLimbAngularVelocity);

	// Send pinned/not pinned limbs info
	for( int i = 0; i < mLimbPositions.Size(); ++i )
		message->WriteBool( data.LimbPinned[i] ); NPROF(NPRagdollLimbPinned);

	// Send dampings
	message->WriteFloat( data.LinearDamping ); NPROF(NPRagdollLinearDamping);
	message->WriteFloat( data.AngularDamping ); NPROF(NPRagdollAngularDamping);

	// Send mass
	message->WriteFloat( data.Mass ); NPROF(NPRagdollMass);

	// Send collision group
	message->WriteUChar( data.CollisionGroup ); NPROF(NPRagdollCollisionGroup);
}

void RagdollSynchronizer::AddDeleteObjMessage( UdpMessage* message, NetworkClient* client )
{
//	GLog.Print( "Ragdoll::DELETEOBJ %d sent\n", UniqueNumber );
	Assert( UniqueNumber != -1 );

	WriteSynchronizer( message, UniqueNumber, STypeRagdoll );
	char c = message->WriteULongOptimized( mModel->UniqueNumber + 1 ); NPROF2(NPSynchronizerNumber,c);
	message->WriteBool( IsZombie ); NPROF(NPIsZombie);
}

bool RagdollSynchronizer::InterpretMessage( UdpMessage* message, unsigned char command, NetworkClient* client )
{
	Assert( ( command == NetworkClient::CREATEOBJ ) || ( command == NetworkClient::DELETEOBJ ) || ( command == NetworkClient::UPDATE ) );
	unsigned long entityNumber = message->ReadULongOptimized()-1;

	Assert( !mModel || ( mModel->UniqueNumber == entityNumber ) );
	if( !mModel )
	{
		mModel = (ModelSynchronizer*)client->SureGetEntitySynchronizer( entityNumber, STypeModel );
		mModel->mRagdoll = this;
	}

	switch( command )
	{
	case NetworkClient::CREATEOBJ:
		{
			// Really, object is already created, we just have to start synchronizing it again
//			GLog.Print( "Ragdoll::CREATEOBJ\n" );
			UpdateClass = message->ReadUChar();

			RagdollData data;

			// Get position and rotation data
			int limbSize = message->ReadUChar();

			for( int i = 0; i < limbSize; ++i ) data.LimbPositions.Add( message->ReadPositionOptimized() );
			for( int i = 0; i < limbSize; ++i ) data.LimbOrientations.Add( message->ReadQuaternionOptimized() );

			// Get linear and angular velocities data
			for( int i = 0; i < limbSize; ++i ) data.LimbLinearVelocities.Add( message->ReadLinearVelocityOptimized() );
			for( int i = 0; i < limbSize; ++i ) data.LimbAngularVelocities.Add( message->ReadAngularVelocityOptimized() );

			// Get pinned/not pinned limbs info
			for( int i = 0; i < limbSize; ++i ) data.LimbPinned.Add( message->ReadBool() );

			// Get dampings
			data.LinearDamping = message->ReadFloat();
			data.AngularDamping = message->ReadFloat();

			// Get mass
			data.Mass = message->ReadFloat();

			// Get collision group
			data.CollisionGroup = message->ReadUChar();

			Assert( mModel->mObject );

			if( !mObject )
			{
				((Model*)mModel->mObject)->SetRagdoll( true, HCGInsideItems );
				mObject = ((Model*)mModel->mObject)->RagdollPtr;
				mDoesNotSynchroAnim = true;
			}
   			mObject->SetFromRagdollData( data );
		}
		return true;
	case NetworkClient::DELETEOBJ:
//		GLog.Print( "Ragdoll::DELETEOBJ %d\n", UniqueNumber );
		IsZombie = message->ReadBool();
		if( mObject )
		{
			if( mModel && mModel->mObject && mDoesNotSynchroAnim )
				((Model*)mModel->mObject)->SetRagdoll( false, HCGInsideItems );
			else if( IsZombie )
				delete mObject;
			mObject = 0;
		}
		return true;
	case NetworkClient::UPDATE:
		{
			DynamicArray<Vector> positions;
			DynamicArray<Quaternion> rotations;
			mObject->GetLimbsPositionsAndRotations( positions, rotations );

			// Get ragdoll update state
			unsigned long limbPositionStates = message->ReadStateOptimized();
			for( int i = 0; i < 30; ++i )
				if( limbPositionStates & (1<<i) )
					positions[i] = message->ReadPositionOptimized();

			unsigned long limbRotationStates = message->ReadStateOptimized();
			for( int i = 0; i < 30; ++i )
				if( limbRotationStates & (1<<i) )
					rotations[i] = message->ReadQuaternionOptimized();

			mObject->SetLimbsPositionsAndRotations( positions, rotations );
		}
		return true;
	case NetworkClient::OTHEROBJ:
	default:
		GLog.Print( "Warning! Unexpected message type default to be recognized by Ragdoll! Network stream bugs may follow!\n" );
		message->DumpToLogfile( "Message type default given to Ragdoll" );
		return false;
	}
	return true;
}

RagdollSynchronizer::~RagdollSynchronizer()
{
	if( mModel )
		mModel->mRagdoll = 0;
}
*/

int PONetCreationCache::SendPOCreationData( UdpMessage* message, PONetCreationEntry& entry )
{
	for( int i = 0; i < Entries.Size(); ++i )
		if( Entries[i] == entry )
		{
			message->WriteLong( Entries[i].index );

			Entries[i].timesSent++;
			while( ( i > 0 ) && ( Entries[i-1].timesSent < Entries[i].timesSent ) )
			{
				Entries.Swap( i, i-1 );
				i--;
			}

			return 4;
		}
	int ind = Entries.Add(entry);
	Entries[ind].index = ind;
	Entries[ind].timesSent = 1;
	message->WriteLong( ind );
	entry.SendEntry(message);
	return 25;
}

void PONetCreationCache::ReceivePOCreationData( UdpMessage* message, PONetCreationEntry& entry )
{
	int index = message->ReadLong();
	Assert( !(index>Entries.Size()) && (index>=0) );
	if( index == Entries.Size() )
	{
		entry.ReceiveEntry( message );
		Entries.Add(entry);
	}
	else
		entry = Entries[index];	// on client entry index field is always equal to entry index
}

void PONetCreationEntry::SendEntry( UdpMessage* message )
{
	message->WriteUShort( ObjectState );
	message->WriteFloat( Mass );
	message->WriteFloat( Friction );
	message->WriteFloat( Restitution );
	message->WriteFloat( BodyScale );
	message->WriteUChar( BodyType );
	message->WriteChar( FreedomOfRotation );
	message->WriteUChar( CollisionGroup );
}

void PONetCreationEntry::ReceiveEntry( UdpMessage* message )
{
	ObjectState = message->ReadUShort();
	Mass = message->ReadFloat();
	Friction = message->ReadFloat();
	Restitution = message->ReadFloat();
	BodyScale = message->ReadFloat();
	BodyType = message->ReadUChar();
	FreedomOfRotation = message->ReadChar();
	CollisionGroup = message->ReadUChar();
}

void PONetCreationEntry::Save( GFile* f )
{
	*f << index << ObjectState << Mass << Friction << Restitution << BodyScale << BodyType << FreedomOfRotation << CollisionGroup;
}

void PONetCreationEntry::Restore( GFile* f )
{
	*f >> index >> ObjectState >> Mass >> Friction >> Restitution >> BodyScale >> BodyType >> FreedomOfRotation >> CollisionGroup;
}

PONetCreationEntry& PONetCreationEntry::operator=( const PONetCreationEntry& other )
{
	index = other.index;
	ObjectState = other.ObjectState;
	Mass = other.Mass;
	Friction = other.Friction;
	Restitution = other.Restitution;
	BodyScale = other.BodyScale;
	BodyType = other.BodyType;
	FreedomOfRotation = other.FreedomOfRotation;
	CollisionGroup = other.CollisionGroup;
	timesSent = other.timesSent;
	return *this;
}

void PONetCreationCache::Save( GFile* f )
{
	*f << Entries.Size();
	for( int i = 0; i < Entries.Size(); ++i )
		Entries[i].Save(f);
}

void PONetCreationCache::Restore( GFile* f )
{
	PONetCreationEntry dummy;
	Entries.Clear();
	int size;
	*f >> size;
	for( int i = 0; i < size; ++i )
	{
		Entries.Add(dummy);
		Entries[i].Restore(f);
	}
}

void PONetCreationCache::DumpRestore( GFile* f )
{
	PONetCreationEntry dummy;
	int size;
	*f >> size;
	for( int i = 0; i < size; ++i )
		dummy.Restore(f);
}

#endif