
#include "Engine.h"
#include "PKTVManager.h"
#include "PKTVEntityProxies.h"
#include "UdpMessage.h"
#ifdef PKTV_DISPLAY
#include "Engine/World.h"
#include "Physics/PhysicsObject.h"
#include "Physics/PhysicsEngine.h"
#include "Engine/Mesh.h"
#include "Engine/ModelAnimation.h"
#include "Render/Billboard.h"
#include "Engine/Model.h"
#include "Render/Light.h"
#include "Render/ParticleEffect.h"
#include "Render/Trail.h"
#include "Render/Decal.h"
#include "Render/Video.h"
#endif

#ifdef TRANSGAMING
//#include <windef.h>
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#define HAVOK_COLLISION_TOLERANCE 0.01f

// =================================================================================================================
// =================================================================================================================
// =================================================================================================================

EntityProxy::EntityProxy( void ) : mConstantData( NULL ), mType( ETypeNone )
{
}

EntityProxy::EntityProxy( UdpMessage* creationData ) : mConstantData( NULL ), mType( ETypeNone )
{
	ReadFullState( creationData );
}


#ifdef	TRANSGAMING
EntityProxy::~EntityProxy( void )
{
	Destroy();
}

void	EntityProxy::Destroy( void )	
{
	if( mConstantData )
	{
		mConstantData->RemoveReference();
		mConstantData = 0;
	}
}

#else

EntityProxy::~EntityProxy( void )
{
	if( mConstantData )
	{
		mConstantData->RemoveReference();
		mConstantData = 0;
	}
}

#endif	


#ifdef PKTV_DISPLAY

void EntityProxy::ReadFromEntity( Entity *e )
{
	mType = e->Type;

//	if( e->Type == 1 )
//		GLog.Print( "reading from entity type 1, index %d\n", e->GIndex );

    m_setTag("Net");
	mConstantData = new EntityConstantData();
	mConstantData->mIndex = e->GIndex;
	mConstantData->FileName = e->FileName;
	mConstantData->Name = e->Name;
	mConstantData->AddReference();

	Flags = e->Flags;
	Translation = e->GetPosition();
	Rotation = e->GetRotation();
	Scale = e->GetScale();
	Demonic = e->Demonic;
	TimeToDelete = e->TimeToDelete;
	FadeTime = e->FadeTime;
	Sort = e->Sort;
	if( e->Parent ) {
		ParentGIndex = e->Parent->GIndex;
//		if( mType == 3 || mType == 10 )
//			GLog.Print( "Entity type %d, index %d, parent type %d, index %d\n", e->Type, e->GIndex, e->Parent->Type, ParentGIndex );
	}
	else
		ParentGIndex = 0;
	ParentJoint = e->ParentJoint;
	TransformByParent = e->TransformByParent;
}

#endif

void EntityProxy::ReadFullState( UdpMessage *mesg )
{
	int lastCategory = mesg->GetProfilingCategory();
	mesg->SetProfilingCategory( EntityProxyFull );

	ulong32 contents = mesg->ReadStateOptimized();

	mType =				( contents & (1<<0) )	?mesg->ReadUChar()					:3;

    m_setTag("Net");
	mConstantData = new EntityConstantData();
	mConstantData->mIndex = mesg->ReadULong();
	mConstantData->FileName = GEngine->PKTV->Dict.ReadString( mesg );
	mConstantData->Name = GEngine->PKTV->Dict.ReadString( mesg );
	mConstantData->AddReference();

	Flags =				( contents & (1<<1) )	?mesg->ReadULongOptimized()			:512;
	Translation =		( contents & (1<<2) )	?mesg->ReadVector()					:Vector(0,0,0);
	Rotation =			( contents & (1<<3) )	?mesg->ReadQuaternionOptimized()	:IdentityQuaternion();
	Scale =				( contents & (1<<4) )	?mesg->ReadFloat()					:1.0f;
	Demonic =			( ( contents & (1<<5) ) != 0 );
	TimeToDelete =		-1;
	FadeTime =			( contents & (1<<6) )	?FadeTime = mesg->ReadFloat()		:0.5f;
	Sort =				( contents & (1<<7) )	?mesg->ReadLong()					:4;
	ParentGIndex =		( contents & (1<<8) )	?mesg->ReadULong()					:0;
	ParentJoint =		( contents & (1<<9) )	?mesg->ReadChar()					:-1;
	TransformByParent =	( ( contents & (1<<10) ) != 0 );

	mesg->SetProfilingCategory( lastCategory );
}

void EntityProxy::StoreFullState( UdpMessage* mesg )
{
	// Calc part
	ulong32 contents = 0;

	if( mType != 3 )							contents |= (1<<0);
	if( Flags != 512 )							contents |= (1<<1);
	if( Translation != Vector(0,0,0) )			contents |= (1<<2);
	if( !( Rotation == IdentityQuaternion() ) )	contents |= (1<<3);
	if( Scale != 1.0f )							contents |= (1<<4);
	if( Demonic )								contents |= (1<<5);
	if( FadeTime != 0.5f )						contents |= (1<<6);
	if( Sort != 4 )								contents |= (1<<7);
	if( ParentGIndex != 0 )						contents |= (1<<8);
	if( ParentJoint != -1 )						contents |= (1<<9);
	if( TransformByParent )						contents |= (1<<10);

	// Write part
	mesg->WriteStateOptimized( contents );

	if( contents & (1<<0) )	mesg->WriteUChar( mType );

	mesg->WriteULong( mConstantData->mIndex );
	GEngine->PKTV->Dict.WriteString( mConstantData->FileName, mesg );
	GEngine->PKTV->Dict.WriteString( mConstantData->Name, mesg );

	if( contents & (1<<1) )	mesg->WriteULongOptimized( Flags );
	if( contents & (1<<2) )	mesg->WriteVector( Translation );
	if( contents & (1<<3) )	mesg->WriteQuaternionOptimized( Rotation );
	if( contents & (1<<4) )	mesg->WriteFloat( Scale );
	if( contents & (1<<6) )	mesg->WriteFloat( FadeTime );
	if( contents & (1<<7) )	mesg->WriteLong( Sort );
	if( contents & (1<<8) )	mesg->WriteULong( ParentGIndex );
	if( contents & (1<<9) )	mesg->WriteChar( ParentJoint );
}

unsigned int EntityProxy::StoreDiff( UdpMessage *mesg, EntityProxy *diffFrom )
{
	unsigned int contents = CalcDiff( diffFrom );

	if( contents )
	{
		mesg->WriteULongOptimized( contents );

		if( contents & EProxyWriteFlags )
			mesg->WriteULongOptimized( Flags ^ diffFrom->Flags );
		if( contents & EProxyWriteTranslation )
			mesg->WritePositionOptimized( Translation );
		else if( contents & EProxyWriteTranslationDiff )
		{
			Vector diff = Translation-diffFrom->Translation;
			mesg->WritePositionDiffOptimized( diff );
		}
		if( contents & EProxyWriteRotation )
			mesg->WriteQuaternionOptimized2( Rotation );
		if( contents & EProxyWriteScale )
			mesg->WriteFloat( Scale );
		if( contents & EProxyWriteTimeToDelete )
			mesg->WriteFloat( TimeToDelete );
		if( contents & EProxyWriteFadeTime )
			mesg->WriteFloat( FadeTime );
		if( contents & EProxyWriteSort )
			mesg->WriteLong( Sort );
		if( contents & EProxyWriteParentGIndex )
			mesg->WriteULong( ParentGIndex );
		if( contents & EProxyWriteParentJoint )
			mesg->WriteChar( ParentJoint );
		if( contents & EProxyWriteBoolFlags )
		{
			unsigned char boolFlags = 0;
			if( Demonic ) boolFlags |= 1;
			if( TransformByParent ) boolFlags |= 2;
			mesg->WriteUChar( boolFlags );
		}
	}

	return contents;
}

EntityProxy *EntityProxy::CreateFromDiffAndMyself( UdpMessage *mesg, DWORD frameTimeDelta )
{
	EntityProxy *proxy = CloneMyself();
	ReadDiff( proxy, mesg, frameTimeDelta );
	return proxy;
}

EntityProxy *EntityProxy::CloneMyself( void )
{
    m_setTag("Net");
	EntityProxy *proxy = new EntityProxy();
	CopyData( proxy );
	return proxy;
}

unsigned int EntityProxy::CalcDiff( EntityProxy *diffFrom )
{
	unsigned int contents = 0;

	if( Flags != diffFrom->Flags )
		contents |= EProxyWriteFlags;
	float posDiff = ( Translation - diffFrom->Translation ).Magnitude();
	if( posDiff > 0.000001f )
	{
		if( UdpMessage::CanUsePositionDiff( Translation - diffFrom->Translation ) )
			contents |= EProxyWriteTranslationDiff;
		else
			contents |= EProxyWriteTranslation;
	}
	if( !( Rotation == diffFrom->Rotation ) )
		contents |= EProxyWriteRotation;
	if( Scale != diffFrom->Scale )
		contents |= EProxyWriteScale;
	if( ( Demonic != diffFrom->Demonic ) || ( TransformByParent != diffFrom->TransformByParent ) )
		contents |= EProxyWriteBoolFlags;
	if( TimeToDelete != diffFrom->TimeToDelete )
		contents |= EProxyWriteTimeToDelete;
	if( FadeTime != diffFrom->FadeTime )
		contents |= EProxyWriteFadeTime;
	if( Sort != diffFrom->Sort )
		contents |= EProxyWriteSort;
	if( ParentGIndex != diffFrom->ParentGIndex )
		contents |= EProxyWriteParentGIndex;
	if( ParentJoint != diffFrom->ParentJoint )
		contents |= EProxyWriteParentJoint;

	return contents;
}

unsigned int EntityProxy::ReadDiff( EntityProxy *proxy, UdpMessage *mesg, DWORD frameTimeDelta )
{
	int lastCategory = mesg->GetProfilingCategory();
	mesg->SetProfilingCategory( EntityProxyDiff );

	unsigned int contents = mesg->ReadULongOptimized();

#ifdef ENTITY_CONTENTS_FLAG_MONITORING
	unsigned char hm = 0;
	unsigned char pos = 0;
	for( int i = 0; i < 32; ++i )
		if( contents & (1<<i) )
		{
			++GEngine->PKTV->BitTesting[i];
			pos = i;
			++hm;
		}
	if( ( hm == 1 ) && ( pos != 2 ) )
		GLog.Print( "Only non-zero bit: %d, type: %d\n", pos, GetType() );
#endif

	if( contents & EProxyWriteFlags )
		proxy->Flags = Flags ^ mesg->ReadULongOptimized();
	if( contents & EProxyWriteTranslation )
		proxy->Translation = mesg->ReadPositionOptimized();
	else if( contents & EProxyWriteTranslationDiff )
		proxy->Translation = mesg->ReadPositionDiffOptimized() + Translation;
	if( contents & EProxyWriteRotation )
		proxy->Rotation = mesg->ReadQuaternionOptimized2();
	if( contents & EProxyWriteScale )
		proxy->Scale = mesg->ReadFloat();
	if( contents & EProxyWriteTimeToDelete )
		proxy->TimeToDelete = mesg->ReadFloat();
	if( contents & EProxyWriteFadeTime )
		proxy->FadeTime = mesg->ReadFloat();
	if( contents & EProxyWriteSort )
		proxy->Sort = mesg->ReadLong();
	if( contents & EProxyWriteParentGIndex )
		proxy->ParentGIndex = mesg->ReadULong();
	if( contents & EProxyWriteParentJoint )
		proxy->ParentJoint = mesg->ReadChar();
	if( contents & EProxyWriteBoolFlags )
	{
		unsigned char boolFlags = mesg->ReadUChar();
		proxy->Demonic = ((boolFlags & 1)!=0);
		proxy->TransformByParent = ((boolFlags & 2)!=0);
	}

	mesg->SetProfilingCategory( lastCategory );

	return contents;
}

void EntityProxy::CopyData( EntityProxy *target )
{
	target->mType = mType;

	target->mConstantData = mConstantData;
	mConstantData->AddReference();

	target->Flags = Flags;
	target->Translation = Translation;
	target->Rotation = Rotation;
	target->Scale = Scale;
	target->Demonic = Demonic;
	target->TimeToDelete = TimeToDelete;
	target->FadeTime = FadeTime;
	target->Sort = Sort;
	target->ParentGIndex = ParentGIndex;
	target->ParentJoint = ParentJoint;
	target->TransformByParent = TransformByParent;
}

#ifdef PKTV_DISPLAY

#ifdef	TRANSGAMING

bool EntityProxy::EntityExists( void )
{
	int index = GetIndex();
	return( GEngine->pcfWorld->EntityGIndexes.Size() > index && GEngine->pcfWorld->EntityGIndexes[index] != NULL );
}

Entity *EntityProxy::MakeEntity( void )
{
	Entity *e = GEngine->pcfWorld->CreateEntity( mType, mConstantData->FileName.AsChar(), mConstantData->Name.AsChar(), Scale, ( Flags & Entity::EEntCenteredGeometry ) != 0 );
	Assert( e );

//	GLog.Print( "Entity make: '%s','%s',%f\n", mConstantData->FileName.AsChar(), mConstantData->Name.AsChar(), Scale );

	GEngine->pcfWorld->AddEntity( e );

	DWORD index = GetIndex();
	if( index != e->GIndex )
	{
		for( DWORD i = GEngine->pcfWorld->EntityGIndexes.Size(); i <= index; i++ )
		{
			GEngine->pcfWorld->EntityGIndexes.Add( 0 );
		}

		DWORD oldIndex = e->GIndex;
		e->GIndex = index;
		if( GEngine->pcfWorld->EntityGIndexes[index] )
			GLog.Print( "WARNING there is already an entity with index %d, type %d, new type %d\n", index, GEngine->pcfWorld->EntityGIndexes[index]->Type, e->Type );
		GEngine->pcfWorld->EntityGIndexes[index] = e;
		GEngine->pcfWorld->EntityGIndexes[oldIndex] = NULL;
	}

	return e;
}

void EntityProxy::DeleteEntity( void )
{
	int index = GetIndex();
	if( GEngine->pcfWorld->EntityGIndexes.Size() > index )
	{
		Entity *e = GEngine->pcfWorld->EntityGIndexes[index];
		if( e )
		{
//			if( e->Type == 1 )
//				GLog.Print( "Deleting type 1 entity %d\n", index );
			GEngine->pcfWorld->DeleteEntity( e );
		}
	}
	else
	{
		GLog.Print( "PKTV WARNING: Trying to delete out of range entity (num %d, index %d)\n",
					GEngine->pcfWorld->EntityGIndexes.Size(), index );
	}
}

void EntityProxy::UpdateEntity( EntityProxy *nextData, float fraction, DWORD timeDiffMs )
{
	int index = GetIndex();
	if( GEngine->pcfWorld->EntityGIndexes.Size() > index )
	{
		Entity *e = GEngine->pcfWorld->EntityGIndexes[index];
		if( e )
		{
			e->Flags = Flags;
			e->TimeToDelete = TimeToDelete;
			e->FadeTime = FadeTime;
			e->Sort = Sort;

			e->SetPositionAndRotation( Translation + ( nextData->Translation - Translation ) * fraction,
									   Rotation + ( nextData->Rotation - Rotation ) * fraction );

			e->SetScale( Scale );
			e->Demonic = Demonic;

			if( ParentGIndex )
			{
				Entity *pe = GEngine->pcfWorld->EntityGIndexes[ParentGIndex];
				if( pe && !e->Parent )
					pe->RegisterChild( e, TransformByParent, ParentJoint );
//				else if( !pe && !e->Parent )
//					GLog.Print( "No parent for entity, parent GIndex = %d, entity GIndex = %d, type = %d\n",
//								ParentGIndex, e->GIndex, e->Type );
			}

			e->UpdateTransform( true );
		}
	}
}

#else

bool EntityProxy::EntityExists( void )
{
	int index = GetIndex();
	return( GEngine->World->EntityGIndexes.Size() > index && GEngine->World->EntityGIndexes[index] != NULL );
}

Entity *EntityProxy::MakeEntity( void )
{
	Entity *e = GEngine->World->CreateEntity( mType, mConstantData->FileName.AsChar(), mConstantData->Name.AsChar(), Scale, ( Flags & Entity::EEntCenteredGeometry ) != 0 );
	Assert( e );

//	GLog.Print( "Entity make: '%s','%s',%f\n", mConstantData->FileName.AsChar(), mConstantData->Name.AsChar(), Scale );

	GEngine->World->AddEntity( e );

	DWORD index = GetIndex();
	if( index != e->GIndex )
	{
		for( DWORD i = GEngine->World->EntityGIndexes.Size(); i <= index; i++ )
		{
			GEngine->World->EntityGIndexes.Add( 0 );
		}

		DWORD oldIndex = e->GIndex;
		e->GIndex = index;
		if( GEngine->World->EntityGIndexes[index] )
			GLog.Print( "WARNING there is already an entity with index %d, type %d, new type %d\n", index, GEngine->World->EntityGIndexes[index]->Type, e->Type );
		GEngine->World->EntityGIndexes[index] = e;
		GEngine->World->EntityGIndexes[oldIndex] = NULL;
	}

	return e;
}

void EntityProxy::DeleteEntity( void )
{
	int index = GetIndex();
	if( GEngine->World->EntityGIndexes.Size() > index )
	{
		Entity *e = GEngine->World->EntityGIndexes[index];
		if( e )
		{
//			if( e->Type == 1 )
//				GLog.Print( "Deleting type 1 entity %d\n", index );
			GEngine->World->DeleteEntity( e );
		}
	}
	else
	{
		GLog.Print( "PKTV WARNING: Trying to delete out of range entity (num %d, index %d)\n",
					GEngine->World->EntityGIndexes.Size(), index );
	}
}

void EntityProxy::UpdateEntity( EntityProxy *nextData, float fraction, DWORD timeDiffMs )
{
	int index = GetIndex();
	if( GEngine->World->EntityGIndexes.Size() > index )
	{
		Entity *e = GEngine->World->EntityGIndexes[index];
		if( e )
		{
			e->Flags = Flags;
			e->TimeToDelete = TimeToDelete;
			e->FadeTime = FadeTime;
			e->Sort = Sort;

			e->SetPositionAndRotation( Translation + ( nextData->Translation - Translation ) * fraction,
									   Rotation + ( nextData->Rotation - Rotation ) * fraction );

			e->SetScale( Scale );
			e->Demonic = Demonic;

			if( ParentGIndex )
			{
				Entity *pe = GEngine->World->EntityGIndexes[ParentGIndex];
				if( pe && !e->Parent )
					pe->RegisterChild( e, TransformByParent, ParentJoint );
//				else if( !pe && !e->Parent )
//					GLog.Print( "No parent for entity, parent GIndex = %d, entity GIndex = %d, type = %d\n",
//								ParentGIndex, e->GIndex, e->Type );
			}

			e->UpdateTransform( true );
		}
	}
}

#endif	//TRANSGAMING


#endif // PKTV_DISPLAY

// =================================================================================================================

BillboardEntityProxy::BillboardEntityProxy( void )
: EntityProxy()
{
}

BillboardEntityProxy::BillboardEntityProxy( UdpMessage *creationData )
{
	ReadFullState( creationData );
}

BillboardEntityProxy::~BillboardEntityProxy( void )
{
#ifdef	TRANSGAMING
	Destroy();
#else
	EntityProxy::~EntityProxy();
#endif	
}

void BillboardEntityProxy::StoreFullState( UdpMessage* mesg )
{
	EntityProxy::StoreFullState( mesg );

	mesg->WriteLong( BillboardType );
	mesg->WriteLong( BlendMode );
	GEngine->PKTV->Dict.WriteString( TexName, mesg );
	mesg->WriteULong( Color );
	mesg->WriteFloat( Timer );
	mesg->WriteFloat( FadeInTime );
	mesg->WriteFloat( FadeOutTime );
	mesg->WriteFloat( Alpha );
	mesg->WriteFloat( AlphaMax );
	mesg->WriteFloat( Distance );
	mesg->WriteFloat( MaxDistance );
	mesg->WriteFloat( MinDistance );
	mesg->WriteFloat( MaxRadius );
	mesg->WriteFloat( MinRadius );
	mesg->WriteFloat( OffDistance );
	mesg->WriteFloat( TraceMargin );
	mesg->WriteBool( IsVisible );
	mesg->WriteBool( HitStatus );
	mesg->WriteFloat( TimeDelta );
	mesg->WriteFloat( TraceInterval );
}

unsigned int BillboardEntityProxy::StoreDiff( UdpMessage *mesg, EntityProxy *diffFrom )
{
	unsigned int contents = EntityProxy::StoreDiff( mesg, diffFrom );

	if( contents )
	{
		if( contents & EProxyWriteBBBillboardType )
			mesg->WriteLong( BillboardType );
		if( contents & EProxyWriteBBBlendMode )
			mesg->WriteLong( BlendMode );
		if( contents & EProxyWriteBBTexName )
			GEngine->PKTV->Dict.WriteString( TexName, mesg );
		if( contents & EProxyWriteBBColor )
			mesg->WriteULong( Color );
		if( contents & EProxyWriteBBTimer )
			mesg->WriteFloat( Timer );
		if( contents & EProxyWriteBBFadeInTime )
			mesg->WriteFloat( FadeInTime );
		if( contents & EProxyWriteBBFadeOutTime )
			mesg->WriteFloat( FadeOutTime );
		if( contents & EProxyWriteBBAlpha )
			mesg->WriteFloat( Alpha );
		if( contents & EProxyWriteBBAlphaMax )
			mesg->WriteFloat( AlphaMax );
		if( contents & EProxyWriteBBDistance )
			mesg->WriteFloat( Distance );
		if( contents & EProxyWriteBBMaxDistance )
			mesg->WriteFloat( MaxDistance );
		if( contents & EProxyWriteBBMinDistance )
			mesg->WriteFloat( MinDistance );
		if( contents & EProxyWriteBBMaxRadius )
			mesg->WriteFloat( MaxRadius );
		if( contents & EProxyWriteBBMinRadius )
			mesg->WriteFloat( MinRadius );
		if( contents & EProxyWriteBBOffDistance )
			mesg->WriteFloat( OffDistance );
		if( contents & EProxyWriteBBTraceMargin )
			mesg->WriteFloat( TraceMargin );
		if( contents & EProxyWriteBBIsVisible )
			mesg->WriteBool( IsVisible );
		if( contents & EProxyWriteBBHitStatus )
			mesg->WriteBool( HitStatus );
		if( contents & EProxyWriteBBTraceInterval )
			mesg->WriteFloat( TraceInterval );
	}

	return contents;
}

EntityProxy *BillboardEntityProxy::CloneMyself( void )
{
	BillboardEntityProxy *proxy = new BillboardEntityProxy();
	CopyData( proxy );
	return proxy;
}

#ifdef PKTV_DISPLAY

void BillboardEntityProxy::ReadFromEntity( Entity *e )
{
	EntityProxy::ReadFromEntity( e );

	Billboard *b = ( Billboard *)e;

	BillboardType = b->BillboardType;
	BlendMode = b->BlendMode;
	TexName = b->TexName;
	Color = b->Color;
	Timer = b->Timer;
	FadeInTime = b->FadeInTime;
	FadeOutTime = b->FadeOutTime;
	Alpha = b->Alpha;
	AlphaMax = b->AlphaMax;
	Distance = b->Distance;
	MaxDistance = b->MaxDistance;
	MinDistance = b->MinDistance;
	MaxRadius = b->MaxRadius;
	MinRadius = b->MinRadius;
	OffDistance = b->OffDistance;
	TraceMargin = b->TraceMargin;
	IsVisible = ( b->IsVisible == TRUE );
	HitStatus = ( b->HitStatus == TRUE );
	TimeDelta = b->TimeDelta;
	TraceInterval = b->TraceInterval;
}

#endif

void BillboardEntityProxy::ReadFullState( UdpMessage *mesg )
{
	EntityProxy::ReadFullState( mesg );

	int lastCategory = mesg->GetProfilingCategory();
	mesg->SetProfilingCategory( BillboardEntityProxyFull );

	BillboardType = mesg->ReadLong();
	BlendMode = mesg->ReadLong();
	TexName = GEngine->PKTV->Dict.ReadString( mesg );
	Color = mesg->ReadULong();
	Timer = mesg->ReadFloat();
	FadeInTime = mesg->ReadFloat();
	FadeOutTime = mesg->ReadFloat();
	Alpha = mesg->ReadFloat();
	AlphaMax = mesg->ReadFloat();
	Distance = mesg->ReadFloat();
	MaxDistance = mesg->ReadFloat();
	MinDistance = mesg->ReadFloat();
	MaxRadius = mesg->ReadFloat();
	MinRadius = mesg->ReadFloat();
	OffDistance = mesg->ReadFloat();
	TraceMargin = mesg->ReadFloat();
	IsVisible = mesg->ReadBool();
	HitStatus = mesg->ReadBool();
	TimeDelta = mesg->ReadFloat();
	TraceInterval = mesg->ReadFloat();

	mesg->SetProfilingCategory( lastCategory );
}

void BillboardEntityProxy::CopyData( EntityProxy *target )
{
	EntityProxy::CopyData( target );

	BillboardEntityProxy *t = ( BillboardEntityProxy *)target;
	
	t->BillboardType = BillboardType;
	t->BlendMode = BlendMode;
	t->TexName = TexName;
	t->Color = Color;
	t->Timer = Timer;
	t->FadeInTime = FadeInTime;
	t->FadeOutTime = FadeOutTime;
	t->Alpha = Alpha;
	t->AlphaMax = AlphaMax;
	t->Distance = Distance;
	t->MaxDistance = MaxDistance;
	t->MinDistance = MinDistance;
	t->MaxRadius = MaxRadius;
	t->MinRadius = MinRadius;
	t->OffDistance = OffDistance;
	t->TraceMargin = TraceMargin;
	t->IsVisible = IsVisible;
	t->HitStatus = HitStatus;
	t->TimeDelta = TimeDelta;
	t->TraceInterval = TraceInterval;
}

unsigned int BillboardEntityProxy::ReadDiff( EntityProxy *proxy, UdpMessage *mesg, DWORD frameTimeDelta )
{
	unsigned int contents = EntityProxy::ReadDiff( proxy, mesg, frameTimeDelta );

	int lastCategory = mesg->GetProfilingCategory();
	mesg->SetProfilingCategory( BillboardEntityProxyDiff );

	BillboardEntityProxy *p = ( BillboardEntityProxy *)proxy;

	if( contents & EProxyWriteBBBillboardType )
		p->BillboardType = mesg->ReadLong();
	if( contents & EProxyWriteBBBlendMode )
		p->BlendMode = mesg->ReadLong();
	if( contents & EProxyWriteBBTexName )
		p->TexName = GEngine->PKTV->Dict.ReadString( mesg );
	if( contents & EProxyWriteBBColor )
		p->Color = mesg->ReadULong();
	if( contents & EProxyWriteBBTimer )
		p->Timer = mesg->ReadFloat();
	if( contents & EProxyWriteBBFadeInTime )
		p->FadeInTime = mesg->ReadFloat();
	if( contents & EProxyWriteBBFadeOutTime )
		p->FadeOutTime = mesg->ReadFloat();
	if( contents & EProxyWriteBBAlpha )
		p->Alpha = mesg->ReadFloat();
	if( contents & EProxyWriteBBAlphaMax )
		p->AlphaMax = mesg->ReadFloat();
	if( contents & EProxyWriteBBDistance )
		p->Distance = mesg->ReadFloat();
	if( contents & EProxyWriteBBMaxDistance )
		p->MaxDistance = mesg->ReadFloat();
	if( contents & EProxyWriteBBMinDistance )
		p->MinDistance = mesg->ReadFloat();
	if( contents & EProxyWriteBBMaxRadius )
		p->MaxRadius = mesg->ReadFloat();
	if( contents & EProxyWriteBBMinRadius )
		p->MinRadius = mesg->ReadFloat();
	if( contents & EProxyWriteBBOffDistance )
		p->OffDistance = mesg->ReadFloat();
	if( contents & EProxyWriteBBTraceMargin )
		p->TraceMargin = mesg->ReadFloat();
	if( contents & EProxyWriteBBIsVisible )
		p->IsVisible = mesg->ReadBool();
	if( contents & EProxyWriteBBHitStatus )
		p->HitStatus = mesg->ReadBool();
	p->TimeDelta = ((float)frameTimeDelta)/1000.f;
	if( contents & EProxyWriteBBTraceInterval )
		p->TraceInterval = mesg->ReadFloat();

	mesg->SetProfilingCategory( lastCategory );

	return contents;
}

unsigned int BillboardEntityProxy::CalcDiff( EntityProxy *diffFrom )
{
	unsigned int contents = EntityProxy::CalcDiff( diffFrom );

	BillboardEntityProxy *diff = ( BillboardEntityProxy *)diffFrom;

	if( BillboardType != diff->BillboardType )
		contents |= EProxyWriteBBBillboardType;
	if( BlendMode != diff->BlendMode )
		contents |= EProxyWriteBBBlendMode;
	if( TexName != diff->TexName )
		contents |= EProxyWriteBBTexName;
	if( Color != diff->Color )
		contents |= EProxyWriteBBColor;
	if( Timer != diff->Timer )
		contents |= EProxyWriteBBTimer;
	if( FadeInTime != diff->FadeInTime )
		contents |= EProxyWriteBBFadeInTime;
	if( FadeOutTime != diff->FadeOutTime )
		contents |= EProxyWriteBBFadeOutTime;
	if( Alpha != diff->Alpha )
		contents |= EProxyWriteBBAlpha;
	if( AlphaMax != diff->AlphaMax )
		contents |= EProxyWriteBBAlphaMax;
	if( Distance != diff->Distance )
		contents |= EProxyWriteBBDistance;
	if( MaxDistance != diff->MaxDistance )
		contents |= EProxyWriteBBMaxDistance;
	if( MinDistance != diff->MinDistance )
		contents |= EProxyWriteBBMinDistance;
	if( MaxRadius != diff->MaxRadius )
		contents |= EProxyWriteBBMaxRadius;
	if( MinRadius != diff->MinRadius )
		contents |= EProxyWriteBBMinRadius;
	if( OffDistance != diff->OffDistance )
		contents |= EProxyWriteBBOffDistance;
	if( TraceMargin != diff->TraceMargin )
		contents |= EProxyWriteBBTraceMargin;
	if( IsVisible != diff->IsVisible )
		contents |= EProxyWriteBBIsVisible;
	if( HitStatus != diff->HitStatus )
		contents |= EProxyWriteBBHitStatus;
	if( TraceInterval != diff->TraceInterval )
		contents |= EProxyWriteBBTraceInterval;

	return contents;
}

#ifdef PKTV_DISPLAY


Entity *BillboardEntityProxy::MakeEntity( void )
{
	Billboard *e = ( Billboard *)EntityProxy::MakeEntity();
	if( e )
	{
		e->BillboardType = BillboardType;
		e->BlendMode = BlendMode;

		e->TexName = TexName;
		e->Color = Color;

		e->Timer = Timer;
		e->FadeInTime = FadeInTime;
		e->FadeOutTime = FadeOutTime;

		e->Alpha = Alpha;
		e->AlphaMax = AlphaMax;

		e->Distance = Distance;
		e->MaxDistance = MaxDistance;
		e->MinDistance = MinDistance;
		e->MaxRadius = MaxRadius;
		e->MinRadius = MinRadius;
		e->OffDistance = OffDistance;
		e->TraceMargin = TraceMargin;
		e->IsVisible = IsVisible;
		e->HitStatus = HitStatus;
		e->TimeDelta = TimeDelta;
		e->TraceInterval = TraceInterval;

		e->Sprite.Size = e->MaxRadius;
		e->UpdateTransform();
		e->SetTexture(e->TexName.AsChar());
		e->UpdateBBox();
	}

	return e;
}


void BillboardEntityProxy::DeleteEntity( void )
{
	return EntityProxy::DeleteEntity();
}


void BillboardEntityProxy::UpdateEntity( EntityProxy *nextData, float fraction, DWORD timeDiffMs )
{
	EntityProxy::UpdateEntity( nextData, fraction, timeDiffMs );
}


#endif // PKTV_DISPLAY

// =================================================================================================================

DecalEntityProxy::DecalEntityProxy( void )
:	EntityProxy()
,	Verts(0)
,	NumElems(0)
{
}

DecalEntityProxy::DecalEntityProxy( UdpMessage *creationData )
:	Verts(0)
,	NumElems(0)
{
	ReadFullState( creationData );
}

DecalEntityProxy::~DecalEntityProxy( void )
{
	if( Verts ) delete[] Verts;
	NumElems = 0;

#ifdef	TRANSGAMING
	Destroy();
#else
	EntityProxy::~EntityProxy();
#endif	

}

void DecalEntityProxy::StoreFullState( UdpMessage* mesg )
{
	EntityProxy::StoreFullState( mesg );

	// Calc part
	ulong32 contents = 0;

	if( !TextureName.IsNull() )			contents |= (1<<0);
	if( ScaleFactor != 1.0f )		contents |= (1<<1);
	if( ConstColor != (DWORD)-1 )	contents |= (1<<2);
	if( NeverDie )					contents |= (1<<3);

	// Write part
	mesg->WriteUChar( contents );
	if( contents & (1<<0) )		GEngine->PKTV->Dict.WriteString( TextureName, mesg );
	if( contents & (1<<1) )		mesg->WriteFloat( ScaleFactor );
	if( contents & (1<<2) )		mesg->WriteULong( ConstColor );
	mesg->WriteFloat( TimeToDie );

	mesg->WriteLong( NumElems );
	for( int i = 0; i < NumElems; ++i )
	{
		// Calc part - one elem
		unsigned char cont2 = 0;

		if( Verts[i].Color != (DWORD)-1 )	cont2 |= (1<<0);
		if( Verts[i].U == 1.0f )			cont2 |= (1<<1);
		if( Verts[i].U == 0.0f )			cont2 |= (1<<2);
		if( Verts[i].V == 1.0f )			cont2 |= (1<<3);
		if( Verts[i].V == 0.0f )			cont2 |= (1<<4);

		// Write part - one elem
		mesg->WriteUChar( cont2 );

		mesg->WriteVector( Verts[i].Pos );
		if( cont2 & (1<<0) )				mesg->WriteULong( Verts[i].Color );
		if( !( cont2 & ((1<<1)|(1<<2)) ) )	mesg->WriteFloat( Verts[i].U );
		if( !( cont2 & ((1<<3)|(1<<4)) ) )	mesg->WriteFloat( Verts[i].V );
	}
}

unsigned int DecalEntityProxy::StoreDiff( UdpMessage *mesg, EntityProxy *diffFrom )
{
	return EntityProxy::StoreDiff( mesg, diffFrom );
}

EntityProxy *DecalEntityProxy::CloneMyself( void )
{
	DecalEntityProxy *proxy = new DecalEntityProxy();
	CopyData( proxy );
	return proxy;
}

#ifdef PKTV_DISPLAY

void DecalEntityProxy::ReadFromEntity( Entity *e )
{
	EntityProxy::ReadFromEntity( e );

	Decal *d = ( Decal *)e;

	mConstantData->FileName = d->GetEffectName();
	if( d->GetTextureName() )
		TextureName = d->GetTextureName();
	else
		TextureName = "";
	ScaleFactor = d->GetScale();
	ConstColor = d->GetConstColor();
	TimeToDie = d->GetTimeToDie();
	NeverDie = d->GetNeverDie();

	int newNumElems = d->GetNumElems();
	if( NumElems != newNumElems )
	{
		if( Verts ) delete[] Verts;
		NumElems = newNumElems;
		Verts = NumElems?new DecalTable[NumElems]:0;
	}

	for( int i = 0; i < NumElems; ++i )
	{
		Verts[i].Pos = d->GetVertPos( i );
		Verts[i].Color = d->GetVertColor( i );
		d->GetVertUV( i, Verts[i].U, Verts[i].V );
	}
}

#endif

void DecalEntityProxy::ReadFullState( UdpMessage *mesg )
{
	EntityProxy::ReadFullState( mesg );

	int lastCategory = mesg->GetProfilingCategory();
	mesg->SetProfilingCategory( DecalEntityProxyFull );

	unsigned char contents = mesg->ReadUChar();

	TextureName =	( contents & (1<<0) )	?GEngine->PKTV->Dict.ReadString( mesg )		:"";
	ScaleFactor =	( contents & (1<<1) )	?mesg->ReadFloat()							:1.0f;
	ConstColor =	( contents & (1<<2) )	?mesg->ReadULong()							:(DWORD)-1;
	TimeToDie =		mesg->ReadFloat();
	NeverDie =		( contents & (1<<3) ) != 0;

	int newNumElems = mesg->ReadLong();
	if( newNumElems != NumElems )
	{
		if( Verts ) delete[] Verts;
		NumElems = newNumElems;
		Verts = NumElems?new DecalTable[NumElems]:0;
	}

	for( int i = 0; i < NumElems; ++i )
	{
		unsigned char cont2 = mesg->ReadUChar();

		Verts[i].Pos =		mesg->ReadVector();
		Verts[i].Color =	( cont2 & (1<<0) )	?mesg->ReadULong()	:(DWORD)-1;
		Verts[i].U =		( cont2 & (1<<1) )	?1.0f: ( ( cont2 & (1<<2) ) ? 0.0f : mesg->ReadFloat() );
		Verts[i].V =		( cont2 & (1<<3) )	?1.0f: ( ( cont2 & (1<<4) ) ? 0.0f : mesg->ReadFloat() );
	}

	mesg->SetProfilingCategory( lastCategory );
}

void DecalEntityProxy::CopyData( EntityProxy *target )
{
	EntityProxy::CopyData( target );

	DecalEntityProxy *t = ( DecalEntityProxy *)target;

	t->TextureName = TextureName;
	t->ScaleFactor = ScaleFactor;
	t->ConstColor = ConstColor;
	t->TimeToDie = TimeToDie;
	t->NeverDie = NeverDie;

	if( NumElems != t->NumElems )
	{
		if( t->Verts ) delete[] t->Verts;
		t->NumElems = NumElems;
		t->Verts = NumElems?new DecalTable[NumElems]:0;
	}

	if( NumElems )
		::memcpy( t->Verts, Verts, NumElems*sizeof(DecalTable) );
}

unsigned int DecalEntityProxy::ReadDiff( EntityProxy *proxy, UdpMessage *mesg, DWORD frameTimeDelta )
{
	return EntityProxy::ReadDiff( proxy, mesg, frameTimeDelta );
}

unsigned int DecalEntityProxy::CalcDiff( EntityProxy *diffFrom )
{
	return EntityProxy::CalcDiff( diffFrom );
}

#ifdef PKTV_DISPLAY


Entity *DecalEntityProxy::MakeEntity( void )
{
	Decal *e = ( Decal *)EntityProxy::MakeEntity();
	if( e )
	{
		if( TextureName.Len() )
			e->SetTexture( TextureName );
		e->SetScale( ScaleFactor );
		e->SetConstColor( ConstColor );
		e->SetTimeToDie( TimeToDie );
		e->SetNeverDie( NeverDie );
		e->SetNumElems( NumElems );
		e->LocalBox.Reset();
		for( int i = 0; i < NumElems; i++ )
		{
			e->SetVertex( i, Verts[i].Pos, Verts[i].Color, Verts[i].U, Verts[i].V );
			e->LocalBox += Verts[i].Pos;
		}
		if( NumElems == 0 )
			e->LocalBox += Vector(0,0,0);

		e->UpdateTransform();
	}

	return e;
}


void DecalEntityProxy::DeleteEntity( void )
{
	return EntityProxy::DeleteEntity();
}


void DecalEntityProxy::UpdateEntity( EntityProxy *nextData, float fraction, DWORD timeDiffMs )
{
	EntityProxy::UpdateEntity( nextData, fraction, timeDiffMs );
}


#endif // PKTV_DISPLAY

// =================================================================================================================

LightEntityProxy::LightEntityProxy( void )
: EntityProxy()
{
}

LightEntityProxy::LightEntityProxy( UdpMessage *creationData )
{
	ReadFullState( creationData );
}

LightEntityProxy::~LightEntityProxy( void )
{
#ifdef	TRANSGAMING
	Destroy();
#else
	EntityProxy::~EntityProxy();
#endif	

}

void LightEntityProxy::StoreFullState( UdpMessage* mesg )
{
	EntityProxy::StoreFullState( mesg );

	mesg->WriteLong( LightType );
	mesg->WriteULong( Color );
	mesg->WriteFloat( Intensity );
	mesg->WriteVector( Direction );
	mesg->WriteFloat( ConeAngleCos );
	mesg->WriteFloat( FalloffAngleCos );
	mesg->WriteFloat( Radius );
	mesg->WriteFloat( StartFalloff );
	GEngine->PKTV->Dict.WriteString( ProjectorName, mesg );
}

unsigned int LightEntityProxy::StoreDiff( UdpMessage *mesg, EntityProxy *diffFrom )
{
	unsigned int contents = EntityProxy::StoreDiff( mesg, diffFrom );

	if( contents )
	{
		if( contents & EProxyWriteLightLightType )
			mesg->WriteLong( LightType );
		if( contents & EProxyWriteLightColor )
			mesg->WriteULong( Color );
		if( contents & EProxyWriteLightIntensity )
			mesg->WriteFloat( Intensity );
		if( contents & EProxyWriteLightDirection )
			mesg->WriteVector( Direction );
		if( contents & EProxyWriteLightConeAngleCos )
			mesg->WriteFloat( ConeAngleCos );
		if( contents & EProxyWriteLightFalloffAngleCos )
			mesg->WriteFloat( FalloffAngleCos );
		if( contents & EProxyWriteLightRadius )
			mesg->WriteFloat( Radius );
		if( contents & EProxyWriteLightStartFalloff )
			mesg->WriteFloat( StartFalloff );
		if( contents & EProxyWriteLightProjectorName )
			GEngine->PKTV->Dict.WriteString( ProjectorName, mesg );
	}

	return contents;
}

EntityProxy *LightEntityProxy::CloneMyself( void )
{
	LightEntityProxy *proxy = new LightEntityProxy();
	CopyData( proxy );
	return proxy;
}

#ifdef PKTV_DISPLAY

void LightEntityProxy::ReadFromEntity( Entity *e )
{
	EntityProxy::ReadFromEntity( e );

	Light *l = ( Light *)e;

	LightType = l->GetType();
	Color = l->Color;
	Intensity = l->Intensity;
	Direction = l->GetDirection();
	ConeAngleCos = l->GetConeAngleCos();
	FalloffAngleCos = l->FalloffAngleCos;
	Radius = l->GetRadius();
	StartFalloff = l->GetStartFalloff();
	ProjectorName = l->ProjectorName;
}

#endif

void LightEntityProxy::ReadFullState( UdpMessage *mesg )
{
	EntityProxy::ReadFullState( mesg );

	int lastCategory = mesg->GetProfilingCategory();
	mesg->SetProfilingCategory( LightEntityProxyFull );

	LightType = mesg->ReadLong();
	Color = mesg->ReadULong();
	Intensity = mesg->ReadFloat();
	Direction = mesg->ReadVector();
	ConeAngleCos = mesg->ReadFloat();
	FalloffAngleCos = mesg->ReadFloat();
	Radius = mesg->ReadFloat();
	StartFalloff = mesg->ReadFloat();
	ProjectorName = GEngine->PKTV->Dict.ReadString( mesg );

	mesg->SetProfilingCategory( lastCategory );
}

void LightEntityProxy::CopyData( EntityProxy *target )
{
	EntityProxy::CopyData( target );

	LightEntityProxy *t = ( LightEntityProxy *)target;
	t->LightType = LightType;
	t->Color = Color;
	t->Intensity = Intensity;
	t->Direction = Direction;
	t->ConeAngleCos = ConeAngleCos;
	t->FalloffAngleCos = FalloffAngleCos;
	t->Radius = Radius;
	t->StartFalloff = StartFalloff;
	t->ProjectorName = ProjectorName;
}

unsigned int LightEntityProxy::ReadDiff( EntityProxy *proxy, UdpMessage *mesg, DWORD frameTimeDelta )
{
	unsigned int contents = EntityProxy::ReadDiff( proxy, mesg, frameTimeDelta );

	int lastCategory = mesg->GetProfilingCategory();
	mesg->SetProfilingCategory( LightEntityProxyDiff );

	LightEntityProxy *p = ( LightEntityProxy *)proxy;

	if( contents & EProxyWriteLightLightType )
		p->LightType = mesg->ReadLong();
	if( contents & EProxyWriteLightColor )
		p->Color = mesg->ReadULong();
	if( contents & EProxyWriteLightIntensity )
		p->Intensity = mesg->ReadFloat();
	if( contents & EProxyWriteLightDirection )
		p->Direction = mesg->ReadVector();
	if( contents & EProxyWriteLightConeAngleCos )
		p->ConeAngleCos = mesg->ReadFloat();
	if( contents & EProxyWriteLightFalloffAngleCos )
		p->FalloffAngleCos = mesg->ReadFloat();
	if( contents & EProxyWriteLightRadius )
		p->Radius = mesg->ReadFloat();
	if( contents & EProxyWriteLightStartFalloff )
		p->StartFalloff = mesg->ReadFloat();
	if( contents & EProxyWriteLightProjectorName )
		p->ProjectorName = GEngine->PKTV->Dict.ReadString( mesg );

	mesg->SetProfilingCategory( lastCategory );

	return contents;
}

unsigned int LightEntityProxy::CalcDiff( EntityProxy *diffFrom )
{
	unsigned int contents = EntityProxy::CalcDiff( diffFrom );

	LightEntityProxy *diff = ( LightEntityProxy *)diffFrom;

	if( LightType != diff->LightType )
		contents |= EProxyWriteLightLightType;
	if( Color != diff->Color )
		contents |= EProxyWriteLightColor;
	if( Intensity != diff->Intensity )
		contents |= EProxyWriteLightIntensity;
	if( Direction != diff->Direction )
		contents |= EProxyWriteLightDirection;
	if( ConeAngleCos != diff->ConeAngleCos )
		contents |= EProxyWriteLightConeAngleCos;
	if( FalloffAngleCos != diff->FalloffAngleCos )
		contents |= EProxyWriteLightFalloffAngleCos;
	if( Radius != diff->Radius )
		contents |= EProxyWriteLightRadius;
	if( StartFalloff != diff->StartFalloff )
		contents |= EProxyWriteLightStartFalloff;
	if( ProjectorName != diff->ProjectorName )
		contents |= EProxyWriteLightProjectorName;

	return contents;
}

#ifdef PKTV_DISPLAY


Entity *LightEntityProxy::MakeEntity( void )
{
	Light *e = ( Light *)EntityProxy::MakeEntity();
	if( e )
	{
		e->SetType( LightType );
		e->Color = Color;
		e->Intensity = Intensity;
		e->SetDirection( Direction );
		e->SetConeAngleCos( ConeAngleCos );
		e->FalloffAngleCos = FalloffAngleCos;
		e->SetRadius( Radius );
		e->SetFalloff( StartFalloff );

		if ((e->Flags & Light::ELightDynamic) == Light::ELightDynamic)
		{
			e->Flags &= ~Light::ELightDynamic;
			e->EnableDynamic(true);
		}

		if( ProjectorName.Len() )
			e->SetProjectorTexture( ProjectorName );
	}

	return e;
}


void LightEntityProxy::DeleteEntity( void )
{
	return EntityProxy::DeleteEntity();
}


void LightEntityProxy::UpdateEntity( EntityProxy *nextData, float fraction, DWORD timeDiffMs )
{
	EntityProxy::UpdateEntity( nextData, fraction, timeDiffMs );
}


#endif // PKTV_DISPLAY

// =================================================================================================================

ModelEntityProxy::ModelEntityProxy( void )
: EntityProxy()
{
}

ModelEntityProxy::ModelEntityProxy( UdpMessage *creationData )
{
	ReadFullState( creationData );
}

ModelEntityProxy::~ModelEntityProxy( void )
{
#ifdef	TRANSGAMING
	Destroy();
#else
	EntityProxy::~EntityProxy();
#endif	

}

void ModelEntityProxy::StoreFullState( UdpMessage* mesg )
{
	EntityProxy::StoreFullState( mesg );

	mesg->WriteFloat( BlendAlpha );
	GEngine->PKTV->Dict.WriteString( MaterialName, mesg );
	mesg->WriteBool( ShadowEnabled );

	mesg->WriteShort( ( short )MeshVisible.Size() );
	for( int i=0; i < MeshVisible.Size(); i++ )
		mesg->WriteBool( MeshVisible[i] );

	mesg->WriteShort( ( short )Animations.Size() );
	for( int i = 0; i < Animations.Size(); i++ )
	{
		if( Animations[i].name.Size() )
		{
			mesg->WriteBool( true );
			GEngine->PKTV->Dict.WriteString( Animations[i].name, mesg );
			mesg->WriteBool( Animations[i].looped );
			mesg->WriteByte( Animations[i].movementCurve );
		}
		else
			mesg->WriteBool( false );
	}

	mesg->WriteShort( ( short )Params.Size() );
	for( int i = 0; i < Params.Size(); i++ )
	{
		Assert( Params[i].index <= Animations.Size() );
		mesg->WriteChar( Params[i].index );
		mesg->WriteFloat( Params[i].frameTime );
		mesg->WriteFloat( Params[i].weight );
		mesg->WriteFloat( Params[i].timeScale );
		mesg->WriteBool( Params[i].looped );
		mesg->WriteBool( Params[i].startAfterBlend );
		mesg->WriteFloat( Params[i].blendTime );
	}

	if( IsRagdoll )
	{
		Assert( JointIndices.Size() );

		unsigned char maxIndex = 0;
		for( int i = 0; i < JointIndices.Size(); ++i )
			if( maxIndex < JointIndices[i]+1 )
				maxIndex = JointIndices[i]+1;

		mesg->WriteUChar( maxIndex );

		unsigned char bitBuf[32];
		::memset( bitBuf, 0, 32 );
		for( int i = 0; i < JointIndices.Size(); ++i )
			bitBuf[ JointIndices[i]/8 ] |= (1<<(JointIndices[i]%8));
		mesg->WriteData( (const char*)bitBuf, (maxIndex-1)/8 + 1 );

		for( int i = 0; i < JointIndices.Size(); ++i )
		{
			mesg->WritePositionOptimized( JointTranslations[i] );
			mesg->WriteQuaternionOptimized( JointRotations[i] );
		}
	}
	else
		mesg->WriteUChar( 0 );
}

unsigned int ModelEntityProxy::StoreDiff( UdpMessage *mesg, EntityProxy *diffFrom )
{
	unsigned int contents = EntityProxy::StoreDiff( mesg, diffFrom );

	ModelEntityProxy* df = (ModelEntityProxy*)diffFrom;

	if( contents )
	{
		if( contents & EProxyWriteModelBlendAlpha )
			mesg->WriteFloat( BlendAlpha );
		if( contents & EProxyWriteModelMaterialName )
			GEngine->PKTV->Dict.WriteString( MaterialName, mesg );
		if( contents & EProxyWriteModelShadowEnabled )
			mesg->WriteBool( ShadowEnabled );

		if( contents & EProxyWriteModelMeshVisible )
		{
			mesg->WriteUChar( (unsigned char)MeshVisible.Size() );
			for( int i=0; i < MeshVisible.Size(); ++i )
				mesg->WriteBits( MeshVisible[i]?1:0, 1 );
			mesg->ResetBitPos();
		}

		if( contents & EProxyWriteModelMeshAnims )
		{
			mesg->WriteUChar( (unsigned char)Animations.Size() );
			Assert( (unsigned char)Animations.Size() != 254 );

			DynamicArray<String> namesToWrite;
			DynamicArray<BYTE> curves;

			int bothAnims = min( Animations.Size(), df->Animations.Size() );
			for( int i = 0; i < bothAnims; ++i )
			{
				if( Animations[i].name != df->Animations[i].name || Animations[i].movementCurve != df->Animations[i].movementCurve )
				{
					mesg->WriteBits( 1,1 );
					namesToWrite.Add( Animations[i].name );
					mesg->WriteBits( Animations[i].looped, 1 );
					curves.Add( Animations[i].movementCurve );
				}
				else
					mesg->WriteBits( 0,1 );
			}

			for( int i = df->Animations.Size(); i < Animations.Size(); ++i )
			{
				namesToWrite.Add( Animations[i].name );
				mesg->WriteBits( Animations[i].looped, 1 );
				curves.Add( Animations[i].movementCurve );
			}

			mesg->ResetBitPos();

			for( int i = 0; i < namesToWrite.Size(); ++i )
			{
				GEngine->PKTV->Dict.WriteString( namesToWrite[i], mesg );
				mesg->WriteByte( curves[i] );
			}
		}

		// Animation params
		if( contents & EProxyWriteModelActiveAnims )
		{
			mesg->WriteUChar( Params.Size() );
			for( int i = 0; i < Params.Size(); ++i )
			{
				unsigned char cont2 = 0;
				if( df->Params.Size() <= i )
					cont2 = (1<<0)+(1<<1)+(1<<2)+(1<<3)+(1<<6);
				else
				{
					if( Params[i].index != df->Params[i].index )			cont2 |= (1<<0);
					if( Params[i].frameTime != df->Params[i].frameTime )	cont2 |= (1<<1);
					if( Params[i].weight != df->Params[i].weight )			cont2 |= (1<<2);
					if( Params[i].timeScale != df->Params[i].timeScale )	cont2 |= (1<<3);
					if( Params[i].looped )									cont2 |= (1<<4);
					if( Params[i].startAfterBlend )							cont2 |= (1<<5);
					if( Params[i].blendTime != df->Params[i].blendTime )	cont2 |= (1<<6);
				}

				mesg->WriteUChar( cont2 );
				if( cont2 & (1<<0) )
					mesg->WriteChar( Params[i].index );
				if( cont2 & (1<<1) )
					mesg->WriteFloat( Params[i].frameTime );
				if( cont2 & (1<<2) )
					mesg->WriteFloat( Params[i].weight );
				if( cont2 & (1<<3) )
					mesg->WriteFloat( Params[i].timeScale );
				if( cont2 & (1<<6) )
					mesg->WriteFloat( Params[i].blendTime );
			}
		}

		if( contents & EProxyWriteModelRagdoll )
		{
			if( JointIndices.Size() )
			{
				unsigned char maxIndex = 0;
				for( int i = 0; i < JointIndices.Size(); ++i )
					if( maxIndex < JointIndices[i]+1 )
						maxIndex = JointIndices[i]+1;

				mesg->WriteUChar( maxIndex );

				unsigned char bitBuf[32];
				::memset( bitBuf, 0, 32 );
				for( int i = 0; i < JointIndices.Size(); ++i )
					bitBuf[ JointIndices[i]/8 ] |= (1<<(JointIndices[i]%8));
				mesg->WriteData( (const char*)bitBuf, (maxIndex-1)/8 + 1 );

				DynamicArray<bool> posDiffs;
				if( df->IsRagdoll )
				{
					for( int i = 0; i < JointIndices.Size(); ++i )
					{
						bool canWriteOptimized =  UdpMessage::CanUsePositionDiff( JointTranslations[i]-df->JointTranslations[i] );
						mesg->WriteBits( canWriteOptimized?1:0, 1 );
						posDiffs.Add( canWriteOptimized );
					}
				}
				else
					for( int i = 0; i < JointIndices.Size(); ++i )
					{
						mesg->WriteBits( 0, 1 );
						posDiffs.Add( false );
					}
				mesg->ResetBitPos();

				Vector diff;
				for( int i = 0; i < JointIndices.Size(); ++i )
				{
					if( posDiffs[i] )
					{
						diff = JointTranslations[i]-df->JointTranslations[i];
						mesg->WritePositionDiffOptimized( diff );
					}
					else
						mesg->WritePositionOptimized( JointTranslations[i] );
					mesg->WriteQuaternionOptimized( JointRotations[i] );
				}
			}
			else
				mesg->WriteUChar( 0 );
		}
	}

	return contents;
}

EntityProxy *ModelEntityProxy::CloneMyself( void )
{
	ModelEntityProxy *proxy = new ModelEntityProxy();
	CopyData( proxy );
	return proxy;
}

#ifdef PKTV_DISPLAY

void ModelEntityProxy::ReadFromEntity( Entity *e )
{
	EntityProxy::ReadFromEntity( e );

	Model *m = ( Model *)e;

	BlendAlpha = m->BlendAlpha;
	MaterialName = m->GetMaterialName();
	ShadowEnabled = m->ShadowEnabled;

	Scale = m->Mesh->Scale;

	MeshVisible.ClearArray();
	for( int i = 0; i < m->Mesh->Meshes.Size(); i++ )
		MeshVisible.Add( m->MeshPropertiesArr[i].IsVisible );

	Animations.ClearArray();
	for( int i = 1; i < m->Mesh->Animations.Size(); ++i )
	{
		Animation anim;
		anim.name.ClearArray();

		if( m->Mesh->Animations[i] )
		{
			anim.looped = m->Mesh->Animations[i]->Looped;
			anim.movementCurve = ( ( SkeletalAnimation *)( m->Mesh->Animations[i] ) )->HasMovementCurve();
			anim.name = m->Mesh->Animations[i]->Name;
		}

		Animations.Add( anim );
	}

	Params.ClearArray();
	for( int i = 0; i < m->AnimController.Animations.Size(); ++i )
	{
		AnimParams param;
		param.index = -1;
		for( int j = 0; j < m->Mesh->Animations.Size(); ++j )
		{
			if( m->Mesh->Animations[j] == m->AnimController.Animations[i].Animation )
			{
				param.index = j;
				break;
			}
		}
		Assert( param.index != -1 );
		AnimationParams& a = m->AnimController.Animations[i];
		param.frameTime = a.FrameTime;
		param.weight = a.Weight;
		param.timeScale = a.TimeScale;
		param.looped = a.Looped;
		param.startAfterBlend = a.StartAfterBlend;
		param.blendTime = a.BlendTime;

		Params.Add( param );
	}

	IsRagdoll = m->IsRagdoll();
	if( IsRagdoll )
	{
		Vector scale;
		Quaternion rotation;
		Vector translation;

		Assert( !JointIndices.Size() );
		unsigned char size = m->Skeleton.Joints.Size();
		for( int i = 0; i < size; ++i )
		{
			if( m->Skeleton.Joints[i].Global )
			{
				m->Skeleton.Joints[i].Transform.Decompose( scale, rotation, translation );
				Assert( scale.MagnitudeSquared() - 3 < 0.01f );

				JointTranslations.Add( translation );
				JointRotations.Add( rotation );
				JointIndices.Add( i );
			}
		}
	}
}

#endif

void ModelEntityProxy::ReadFullState( UdpMessage *mesg )
{
	EntityProxy::ReadFullState( mesg );

	int lastCategory = mesg->GetProfilingCategory();
	mesg->SetProfilingCategory( ModelEntityProxyFull );

	BlendAlpha = mesg->ReadFloat();
	MaterialName = GEngine->PKTV->Dict.ReadString( mesg );
	ShadowEnabled = mesg->ReadBool();

	MeshVisible.ClearArray();
	short meshVisCnt = mesg->ReadShort();
	for( short i = 0; i < meshVisCnt; i++ )
	{
		bool vis = mesg->ReadBool();
		MeshVisible.Add( vis );
	}

	Animations.ClearArray();
	short animsCnt = mesg->ReadShort();
	for( short i = 0; i < animsCnt; i++ )
	{
		Animation anim;
		anim.name.ClearArray();

		bool exists = mesg->ReadBool();
		if( exists )
		{
			anim.name = GEngine->PKTV->Dict.ReadString( mesg );
			anim.looped = mesg->ReadBool();
			anim.movementCurve = mesg->ReadByte();
		}

		Animations.Add( anim );
	}

	Params.ClearArray();
	short paramsCnt = mesg->ReadShort();
	for( short i = 0; i < paramsCnt; i++ )
	{
		AnimParams param;

		param.index = mesg->ReadChar();
		Assert( param.index <= Animations.Size() );
		param.frameTime = mesg->ReadFloat();
		param.weight = mesg->ReadFloat();
		param.timeScale = mesg->ReadFloat();
		param.looped = mesg->ReadBool();
		param.startAfterBlend = mesg->ReadBool();
		param.blendTime = mesg->ReadFloat();

		Params.Add( param );
	}

	unsigned char howManyRagdollJoints = mesg->ReadUChar();
	if( howManyRagdollJoints )
	{
		Assert( !JointIndices.Size() );
		IsRagdoll = true;

		for( int i = 0; i < howManyRagdollJoints; ++i )
			if( mesg->ReadBits(1) )
				JointIndices.Add(i);
		mesg->ResetBitPos();

		for( int i = 0; i < JointIndices.Size(); ++i )
		{
			JointTranslations.Add( mesg->ReadPositionOptimized() );
			JointRotations.Add( mesg->ReadQuaternionOptimized() );
		}
	}
	else
		IsRagdoll = false;

	mesg->SetProfilingCategory( lastCategory );
}

void ModelEntityProxy::CopyData( EntityProxy *target )
{
	EntityProxy::CopyData( target );

	ModelEntityProxy *t = ( ModelEntityProxy *)target;
	t->BlendAlpha = BlendAlpha;
	t->MaterialName = MaterialName;
	t->ShadowEnabled = ShadowEnabled;

	t->MeshVisible.ClearArray();
	for( int i = 0; i < MeshVisible.Size(); i++ )
		t->MeshVisible.Add( MeshVisible[i] );

	t->Animations.ClearArray();
	for( int i = 0; i < Animations.Size(); ++i )
	{
		Animation anim;
		anim.name = Animations[i].name;
		anim.looped = Animations[i].looped;
		anim.movementCurve = Animations[i].movementCurve;

		t->Animations.Add( anim );
	}

	t->Params.ClearArray();
	for( int i = 0; i < Params.Size(); i++ )
	{
		AnimParams param;
		param.index = Params[i].index;
		Assert( param.index <= t->Animations.Size() );
		param.frameTime = Params[i].frameTime;
		param.weight = Params[i].weight;
		param.timeScale = Params[i].timeScale;
		param.looped = Params[i].looped;
		param.startAfterBlend = Params[i].startAfterBlend;
		param.blendTime = Params[i].blendTime;
		t->Params.Add( param );
	}

	t->IsRagdoll = IsRagdoll;
	t->JointTranslations = JointTranslations;
	t->JointRotations = JointRotations;
	t->JointIndices = JointIndices;
}

unsigned int ModelEntityProxy::ReadDiff( EntityProxy *proxy, UdpMessage *mesg, DWORD frameTimeDelta )
{
	unsigned int contents = EntityProxy::ReadDiff( proxy, mesg, frameTimeDelta );

	int lastCategory = mesg->GetProfilingCategory();
	mesg->SetProfilingCategory( ModelEntityProxyDiff );

	ModelEntityProxy *p = ( ModelEntityProxy *)proxy;

	if( contents & EProxyWriteModelBlendAlpha )
		p->BlendAlpha = mesg->ReadFloat();
	if( contents & EProxyWriteModelMaterialName )
		p->MaterialName = GEngine->PKTV->Dict.ReadString( mesg );
	if( contents & EProxyWriteModelShadowEnabled )
		p->ShadowEnabled = mesg->ReadBool();

	if( contents & EProxyWriteModelMeshVisible )
	{
		p->MeshVisible.ClearArray();
		unsigned char meshVisCnt = mesg->ReadUChar();
		for( unsigned char i = 0; i < meshVisCnt; ++i )
		{
			bool vis = ( mesg->ReadBits( 1 ) != 0 );
			p->MeshVisible.Add( vis );
		}
		mesg->ResetBitPos();
	}

	if( contents & EProxyWriteModelMeshAnims )
	{
		p->Animations.ClearArray();

		unsigned char animsSize = mesg->ReadUChar();
		int bothAnims = min( animsSize, Animations.Size() );

		DynamicArray<bool> animsToRead;
		DynamicArray<bool> additionalData;

		for( int i = 0; i < bothAnims; ++i )
		{
			unsigned char bit = mesg->ReadBits(1);
			animsToRead.Add( (bit!=0) );
			if( bit )
			{
				additionalData.Add( (mesg->ReadBits(1) != 0 ) );
			}
		}
		
		for( int i = Animations.Size(); i < animsSize; ++i )
		{
			animsToRead.Add(true);
			additionalData.Add( ( mesg->ReadBits(1) != 0 ) );
		}
		mesg->ResetBitPos();

		int c = 0;
		for( int i = 0; i < animsSize; ++i )
		{
			Animation anim;
			anim.name.ClearArray();
			bool differs = animsToRead[i];
			if( differs )
			{
				anim.name = GEngine->PKTV->Dict.ReadString( mesg );
				Assert( anim.name.Size() );
				anim.looped = additionalData[c++];
				anim.movementCurve = mesg->ReadByte();
			}
			else
				anim = Animations[p->Animations.Size()];

			p->Animations.Add( anim );
		}
	}

	// Animation params
	if( contents & EProxyWriteModelActiveAnims )
	{
		p->Params.ClearArray();
		unsigned char paramsCnt = mesg->ReadUChar();
		for( int i = 0; i < paramsCnt; ++i )
		{
			AnimParams param;
			unsigned char cont2 = mesg->ReadUChar();

			param.index =			( cont2 & (1<<0) )?mesg->ReadChar()		:Params[i].index;
			param.frameTime =		( cont2 & (1<<1) )?mesg->ReadFloat()	:Params[i].frameTime;	// can't optimize this - tried for a long32 time
			param.weight =			( cont2 & (1<<2) )?mesg->ReadFloat()	:Params[i].weight;
			param.timeScale =		( cont2 & (1<<3) )?mesg->ReadFloat()	:Params[i].timeScale;
			param.looped =			( cont2 & (1<<4) ) != 0;
			param.startAfterBlend =	( cont2 & (1<<5) ) != 0;
			param.blendTime =		( cont2 & (1<<6) )?mesg->ReadFloat()	:Params[i].blendTime;

			p->Params.Add( param );
		}
	}

	if( contents & EProxyWriteModelRagdoll )
	{
		unsigned char howManyRagdollJoints = mesg->ReadUChar();
		if( howManyRagdollJoints )
		{
			p->JointIndices.ClearArray();
			p->JointTranslations.ClearArray();
			p->JointRotations.ClearArray();
			p->IsRagdoll = true;

			for( int i = 0; i < howManyRagdollJoints; ++i )
				if( mesg->ReadBits(1) )
					p->JointIndices.Add(i);
			mesg->ResetBitPos();

			DynamicArray<bool> posDiffs;
			for( int i = 0; i < p->JointIndices.Size(); ++i )
				posDiffs.Add( mesg->ReadBits(1) != 0 );
			mesg->ResetBitPos();

			for( int i = 0; i < p->JointIndices.Size(); ++i )
			{
				if( posDiffs[i] )
					p->JointTranslations.Add( JointTranslations[i]+mesg->ReadPositionDiffOptimized() );
				else
					p->JointTranslations.Add( mesg->ReadPositionOptimized() );
				p->JointRotations.Add( mesg->ReadQuaternionOptimized() );
			}
		}
		else
			p->IsRagdoll = false;
	}

	mesg->SetProfilingCategory( lastCategory );

	return contents;
}

unsigned int ModelEntityProxy::CalcDiff( EntityProxy *diffFrom )
{
	unsigned int contents = EntityProxy::CalcDiff( diffFrom );

	ModelEntityProxy *diff = ( ModelEntityProxy *)diffFrom;

	if( BlendAlpha != diff->BlendAlpha )
		contents |= EProxyWriteModelBlendAlpha;
	if( MaterialName != diff->MaterialName )
		contents |= EProxyWriteModelMaterialName;
	if( ShadowEnabled != diff->ShadowEnabled )
		contents |= EProxyWriteModelShadowEnabled;

	if( MeshVisible.Size() == diff->MeshVisible.Size() )
	{
		for( int i = 0; i < MeshVisible.Size(); ++i )
		{
			if( MeshVisible[i] != diff->MeshVisible[i] )
			{
				contents |= EProxyWriteModelMeshVisible;
				break;
			}
		}
	}
	else
		contents |= EProxyWriteModelMeshVisible;

	bool animsDiff = ( Animations.Size() != diff->Animations.Size() );
	if( !animsDiff )
	{
		for( int i = 0; i < Animations.Size(); ++i )
			if( Animations[i].name != diff->Animations[i].name )
			{
				animsDiff = true;
				break;
			}
	}
	if( animsDiff )
		contents |= EProxyWriteModelMeshAnims;

	if( Params.Size() != diff->Params.Size() )
		contents |= EProxyWriteModelActiveAnims;
	else
		for( int i = 0; i < Params.Size(); ++i )
			if( !( Params[i] == diff->Params[i] ) )
			{
				contents |= EProxyWriteModelActiveAnims;
				break;
			}

	if( IsRagdoll || diff->IsRagdoll )
		contents |= EProxyWriteModelRagdoll;

	return contents;
}

#ifdef PKTV_DISPLAY

Entity *ModelEntityProxy::MakeEntity( void )
{
	Model *e = ( Model *)EntityProxy::MakeEntity();
	if( e )
	{
		e->BlendAlpha = BlendAlpha;
		if( MaterialName.Size() )
			e->SetMaterial( MaterialName );

		e->ShadowEnabled = ShadowEnabled;
		if( ShadowEnabled )
			e->CreateShadowMap( true );
	}

	return e;
}


void ModelEntityProxy::DeleteEntity( void )
{
	return EntityProxy::DeleteEntity();
}

void ModelEntityProxy::UpdateEntity( EntityProxy *nextData, float fraction, DWORD timeDiffMs )
{
	EntityProxy::UpdateEntity( nextData, fraction, timeDiffMs );

	int index = GetIndex();
#ifdef	TRANSGAMING
	if( GEngine->pcfWorld->EntityGIndexes.Size() > index )
	{
		Model *e = ( Model *)GEngine->pcfWorld->EntityGIndexes[index];
#else
	if( GEngine->World->EntityGIndexes.Size() > index )
	{
		Model *e = ( Model *)GEngine->World->EntityGIndexes[index];
#endif
		if( e )
		{
			for( int i = 0; i < MeshVisible.Size(); i++ )
			{
				e->MeshPropertiesArr[i].IsVisible = MeshVisible[i];
				if( e->Flags & Entity::EEntNoDraw )
					e->MeshPropertiesArr[i].IsVisible = false;
			}

			for( int i = 0; i < Animations.Size(); ++i )
			{
				if( Animations[i].name.Size() )
				{
					int ind = e->LoadAnimation( Animations[i].name.AsChar(), Animations[i].looped, i + 1 );
					Assert( ind == i + 1 );
					if( Animations[i].movementCurve )
						e->SetAnimationMovementCurve( ind, "ROOOT", Animations[i].movementCurve );
				}
			}

			e->AnimController.Animations.ClearArray();

			// Krystian - I rebuilt completely this part of code - because the old one blended the params by their
			// position in table, not by their animation indexes

			ModelEntityProxy* nd = (ModelEntityProxy*)nextData;

			for( int i = 0; i < Params.Size(); ++i )
			{
				AnimParams& p1 = Params[i];
				AnimParams* p2 = 0;
				ModelAnimation* animation = 0;

				for( int j = 0; j < nd->Params.Size(); ++j )
					if( p1.index == nd->Params[j].index )
					{
						animation = e->Mesh->Animations[p1.index];
						p2 = &nd->Params[j];
						break;
					}

				if( !p2 )
					p2 = &Params[i];

				if( p2 )
				{
					AnimationParams& a = e->AnimController.Animations[ e->AnimController.Animations.GrowArray(1) ];

					a.Weight = p1.weight;
					a.TimeScale = p1.timeScale;
					a.Looped = p1.looped;
					a.StartAfterBlend = p1.startAfterBlend;
					Assert( p1.index < e->Mesh->Animations.Size() )
					a.Animation = e->Mesh->Animations[p1.index];

					// Blend those two properly (with looped anims)
					if( p2->frameTime >= p1.frameTime )
						a.FrameTime = p1.frameTime + ( p2->frameTime - p1.frameTime ) * fraction;
					else
					{
						a.FrameTime = p1.frameTime + ( p2->frameTime + animation->Time - p1.frameTime ) * fraction;
						if( a.FrameTime > animation->Time )
							a.FrameTime -= animation->Time;
					}

					a.BlendTime = p1.blendTime + ( p2->blendTime - p1.blendTime ) * fraction;
				}
			}

			if( IsRagdoll )
			{
				if( nd->IsRagdoll )
				{
					// Interpolating between ragdoll positions and orientations
					Vector scale(1,1,1);
					Vector transInterp;
					Quaternion rotInterp;
					for( int i = 0; i < e->Skeleton.Joints.Size(); ++i )
						e->Skeleton.Joints[i].Global = false;

					for( int i = 0; i < JointIndices.Size(); ++i )
					{
						Assert( JointIndices[i] < e->Skeleton.Joints.Size() );
						transInterp = JointTranslations[i] + (nd->JointTranslations[i]-JointTranslations[i])*fraction;
						rotInterp = Quaternion::Slerp( fraction, JointRotations[i], nd->JointRotations[i] );
						e->Skeleton.Joints[JointIndices[i]].Transform = Matrix( scale, rotInterp, transInterp );
						e->Skeleton.Joints[JointIndices[i]].Global = true;
					}
				}
				else
				{
					Vector scale(1,1,1);
					for( int i = 0; i < e->Skeleton.Joints.Size(); ++i )
						e->Skeleton.Joints[i].Global = false;

					for( int i = 0; i < JointIndices.Size(); ++i )
					{
						Assert( JointIndices[i] < e->Skeleton.Joints.Size() );
						e->Skeleton.Joints[JointIndices[i]].Transform = Matrix( scale, JointRotations[i], JointTranslations[i] );
						e->Skeleton.Joints[JointIndices[i]].Global = true;
					}
				}
			}
			else if( nd->IsRagdoll )
			{
				Vector scale(1,1,1);
				for( int i = 0; i < e->Skeleton.Joints.Size(); ++i )
					e->Skeleton.Joints[i].Global = false;

				for( int i = 0; i < nd->JointIndices.Size(); ++i )
				{
					Assert( nd->JointIndices[i] < e->Skeleton.Joints.Size() );
					e->Skeleton.Joints[nd->JointIndices[i]].Transform = Matrix( scale, nd->JointRotations[i], nd->JointTranslations[i] );
					e->Skeleton.Joints[nd->JointIndices[i]].Global = true;
				}
			}

			// For now, we don't include at all any animations until they can be blended between 2 frames
/*
			for( int i = 0; i < Params.Size(); i++ )
			{
				e->AnimController.Animations.GrowArray(1);
				AnimationParams& a = e->AnimController.Animations[i];

				AnimParams &p = Params[i];
				AnimParams *p2 = &Params[i];
				if( ( ( ModelEntityProxy *)nextData )->Params.Size() > i )
					p2 = &( ( ModelEntityProxy *)nextData )->Params[i];

				a.FrameTime = p.frameTime;
				if( p2->frameTime > p.frameTime )
					a.FrameTime += ( p2->frameTime - p.frameTime ) * fraction;
				a.Weight = p.weight;
				a.TimeScale = p.timeScale;
				a.Looped = p.looped;
				a.StartAfterBlend = p.startAfterBlend;
				a.FrameBlendTime = p.frameBlendTime;
				if( p2->frameBlendTime > p.frameBlendTime )
					a.FrameBlendTime += ( p2->frameBlendTime - p.frameBlendTime ) * fraction;
				a.BlendTime = p.blendTime;
				if( p2->blendTime > p.blendTime )
					a.BlendTime += ( p2->blendTime - p.blendTime ) * fraction;

				Assert( p.index < e->Mesh->Animations.Size() )
				a.Animation = e->Mesh->Animations[p.index];
			}
*/
		}
	}
}


#endif // PKTV_DISPLAY

// =================================================================================================================

ParticleFXEntityProxy::ParticleFXEntityProxy( void )
: EntityProxy()
{
}

ParticleFXEntityProxy::ParticleFXEntityProxy( UdpMessage *creationData )
{
	ReadFullState( creationData );
}

ParticleFXEntityProxy::~ParticleFXEntityProxy( void )
{
#ifdef	TRANSGAMING
	Destroy();
#else
	EntityProxy::~EntityProxy();
#endif	

}

void ParticleFXEntityProxy::StoreFullState( UdpMessage* mesg )
{
	EntityProxy::StoreFullState( mesg );

	// Calc part - main
	unsigned char contents = 0;
	Vector zero(0,0,0);

	if( ParentOffset != zero )								contents |= (1<<0);
	if( ParentRotation )									contents |= (1<<1);
	if( !(ParentRotationOffset == IdentityQuaternion() ) )	contents |= (1<<2);
	if( CameraOffset != zero )								contents |= (1<<3);
	if( Active )											contents |= (1<<4);
	if( IsImmortal )										contents |= (1<<5);
	if( EmitterCount != 2 )									contents |= (1<<6);

	// Write part - main
	mesg->WriteUChar( contents );

	if( contents & (1<<0) )	mesg->WriteVector( ParentOffset );
	if( contents & (1<<1) )	mesg->WriteQuaternion( ParentRotationOffset );
	if( contents & (1<<3) )	mesg->WriteVector( CameraOffset );
	if( contents & (1<<6) )	mesg->WriteUShort( EmitterCount );

	for( int i = 0; i < EmitterCount; ++i )
	{
		Emitter *e = &Emitters[i];

		// Calc part - one emitter
		unsigned char cont2 = 0;

		if( !( e->RotQuat == IdentityQuaternion() ) )	cont2 |= (1<<0);
		if( e->Position != zero )						cont2 |= (1<<1);
		if( e->LastPosition != zero )					cont2 |= (1<<2);
		if( e->Pos != zero )							cont2 |= (1<<3);
		if( e->Rotation != zero )						cont2 |= (1<<4);
		if( Scale != 1.0f )								cont2 |= (1<<5);

		// Write part - one emitter
		mesg->WriteUChar( cont2 );

		GEngine->PKTV->Dict.WriteString( e->Name, mesg );
		if( cont2 & (1<<0) )	mesg->WriteQuaternion( e->RotQuat );
		if( cont2 & (1<<1) )	mesg->WriteVector( e->Position );
		if( cont2 & (1<<2) )	mesg->WriteVector( e->LastPosition );
		mesg->WriteByte( GetEmitterFlagsFromData( e ) );
		if( cont2 & (1<<3) )	mesg->WriteVector( e->Pos );
		if( cont2 & (1<<4) )	mesg->WriteVector( e->Rotation );
		if( cont2 & (1<<5) )	mesg->WriteFloat( e->Scale );
	}
}

unsigned int ParticleFXEntityProxy::StoreDiff( UdpMessage *mesg, EntityProxy *diffFrom )
{
	unsigned int contents = EntityProxy::StoreDiff( mesg, diffFrom );
/*
	ParticleFXEntityProxy *diff = ( ParticleFXEntityProxy *)diffFrom;

	if( contents )
	{
		if( contents & EProxyWritePFXParentOffset )
			mesg->WriteVector( ParentOffset );
		if( contents & EProxyWritePFXParentRotation )
			mesg->WriteBool( ParentRotation );
		if( contents & EProxyWritePFXParentRotationOffset )
			mesg->WriteQuaternion( ParentRotationOffset );
		if( contents & EProxyWritePFXCameraOffset )
			mesg->WriteVector( CameraOffset );
		if( contents & EProxyWritePFXActive )
			mesg->WriteBool( Active );
		if( contents & EProxyWritePFXIsImmortal )
			mesg->WriteBool( IsImmortal );
		if( contents & EProxyWritePFXEmitterCount )
			mesg->WriteLong( EmitterCount );

		if( contents & EProxyWritePFXEmitters )
		{
			for( int i = 0; i < EmitterCount; i++ )
			{
				Emitter *e = &Emitters[i];
				unsigned char c = CalcEmitterDiff( e, &diff->Emitters[i] );
				mesg->WriteByte( c );
				if( c & EPFXEmitterWriteName )
					GEngine->PKTV->Dict.WriteString( e->Name, mesg );
				if( c & EPFXEmitterWriteRotQuat )
					mesg->WriteQuaternion( e->RotQuat );
				if( c & EPFXEmitterWritePosition )
					mesg->WritePositionOptimized( e->Position );
				if( c & EPFXEmitterWritePositionDiffOnly )
				{
					Vector temp = e->Position - diff->Emitters[i].Position;
					mesg->WritePositionDiffOptimized( temp );
				}
				if( c & EPFXEmitterWriteFlags )
					mesg->WriteByte( GetEmitterFlagsFromData( e ) );
				if( c & EPFXEmitterWritePos )
					mesg->WriteVector( e->Pos );
				if( c & EPFXEmitterWriteRotation )
					mesg->WriteVector( e->Rotation );
				if( c & EPFXEmitterWriteScale )
					mesg->WriteFloat( e->Scale );
			}
		}
	}
*/
	return contents;
}

EntityProxy *ParticleFXEntityProxy::CloneMyself( void )
{
	ParticleFXEntityProxy *proxy = new ParticleFXEntityProxy();
	CopyData( proxy );
	return proxy;
}

#ifdef PKTV_DISPLAY

void ParticleFXEntityProxy::ReadFromEntity( Entity *e )
{
	EntityProxy::ReadFromEntity( e );

	ParticleEffect *p = ( ParticleEffect *)e;

	ParentOffset = p->ParentOffset;
	ParentRotation = p->ParentRotation;
	ParentRotationOffset = p->ParentRotationOffset;
	CameraOffset = p->CameraOffset;
	Active = p->Active;
	IsImmortal = p->IsImmortal;
	EmitterCount = p->EmitterCount;

	for( int i = 0; i < EmitterCount; i++ )
	{
		Emitter *e1 = &Emitters[i];
		ParticleEmitter *e2 = p->Emitters[i].Emitter;

		e1->Active = e2->Active;
		e1->Dying = e2->Dying;
		e1->Evolve = e2->Evolve;
		e1->Immortal = ( e2->Immortal == TRUE );
		e1->LastPosition = e2->LastPosition;
		e1->Name = e2->Name;
		e1->Pos = p->Emitters[i].Pos;
		e1->Position = e2->Position;

#ifndef FINAL_BUILD
		if( e2->Position == Vector(0,0,0) )
			GLog.Print( "Warning: Particle Effect Emitter in (0,0,0) when recording PKTV: %u (%s)\n", (DWORD)p, p->FileName.AsChar() );
#endif

		e1->Respawn = e2->Respawn;
		e1->Rotation = p->Emitters[i].Rotation;
		e1->RotQuat = e2->RotQuat;
		e1->Scale = p->Emitters[i].Scale;
	}
}

#endif

void ParticleFXEntityProxy::ReadFullState( UdpMessage *mesg )
{
	EntityProxy::ReadFullState( mesg );

	int lastCategory = mesg->GetProfilingCategory();
	mesg->SetProfilingCategory( ParticleEntityProxyFull );

	unsigned char contents = mesg->ReadUChar();
	Vector zero(0,0,0);

	ParentOffset =			( contents & (1<<0) )		?mesg->ReadVector()		:zero;
	ParentRotation =		( contents & (1<<1) ) != 0;
	ParentRotationOffset =	( contents & (1<<2) )		?mesg->ReadQuaternion()	:IdentityQuaternion();
	CameraOffset =			( contents & (1<<3) )		?mesg->ReadVector()		:zero;
	Active =				( contents & (1<<4) ) != 0;		
	IsImmortal =			( contents & (1<<5) ) != 0;
	EmitterCount =			( contents & (1<<6) )		?mesg->ReadUShort()		:2;

	for( int i = 0; i < EmitterCount; ++i )
	{
		Emitter *e = &Emitters[i];

		unsigned char cont2 = mesg->ReadUChar();
		e->Name = GEngine->PKTV->Dict.ReadString( mesg );

		e->RotQuat =		( cont2 & (1<<0) )		?mesg->ReadQuaternion()	:IdentityQuaternion();
		e->Position =		( cont2 & (1<<1) )		?mesg->ReadVector()		:zero;
		e->LastPosition =	( cont2 & (1<<2) )		?mesg->ReadVector()		:zero;
		GetEmitterDataFromFlags( e, mesg->ReadByte() );
		e->Pos =			( cont2 & (1<<3) )		?mesg->ReadVector()		:zero;
		e->Rotation =		( cont2 & (1<<4) )		?mesg->ReadVector()		:zero;
		e->Scale =			( cont2 & (1<<5) )		?mesg->ReadFloat()		:1.0f;
	}

	mesg->SetProfilingCategory( lastCategory );
}

void ParticleFXEntityProxy::CopyData( EntityProxy *target )
{
	EntityProxy::CopyData( target );

	ParticleFXEntityProxy *t = ( ParticleFXEntityProxy *)target;
	t->ParentOffset = ParentOffset;
	t->ParentRotation = ParentRotation;
	t->ParentRotationOffset = ParentRotationOffset;
	t->CameraOffset = CameraOffset;
	t->Active = Active;
	t->IsImmortal = IsImmortal;
	t->EmitterCount = EmitterCount;

	for( int i = 0; i < EmitterCount; i++ )
	{
		Emitter *e = &Emitters[i];
		Emitter *te = &t->Emitters[i];

		te->Name = e->Name;
		te->RotQuat = e->RotQuat;
		te->Position = e->Position;
		te->LastPosition = e->LastPosition;
		te->Active = e->Active;
		te->Respawn = e->Respawn;
		te->Evolve = e->Evolve;
		te->Immortal = e->Immortal;
		te->Dying = e->Dying;
	}
}

unsigned int ParticleFXEntityProxy::ReadDiff( EntityProxy *proxy, UdpMessage *mesg, DWORD frameTimeDelta )
{
	unsigned int contents = EntityProxy::ReadDiff( proxy, mesg, frameTimeDelta );
/*
	int lastCategory = mesg->GetProfilingCategory();
	mesg->SetProfilingCategory( ParticleEntityProxyDiff );

	ParticleFXEntityProxy *p = ( ParticleFXEntityProxy *)proxy;

	if( contents & EProxyWritePFXParentOffset )
		p->ParentOffset = mesg->ReadVector();
	if( contents & EProxyWritePFXParentRotation )
		p->ParentRotation = mesg->ReadBool();
	if( contents & EProxyWritePFXParentRotationOffset )
		p->ParentRotationOffset = mesg->ReadQuaternion();
	if( contents & EProxyWritePFXCameraOffset )
		p->CameraOffset = mesg->ReadVector();
	if( contents & EProxyWritePFXActive )
		p->Active = mesg->ReadBool();
	if( contents & EProxyWritePFXIsImmortal )
		p->IsImmortal = mesg->ReadBool();
	if( contents & EProxyWritePFXEmitterCount )
		p->EmitterCount = mesg->ReadLong();

	if( contents & EProxyWritePFXEmitters )
	{
		for( int i = 0; i < p->EmitterCount; ++i )
		{
			Emitter *e = &p->Emitters[i];
			unsigned char diff = mesg->ReadByte();
			if( diff & EPFXEmitterWriteName )
				e->Name = GEngine->PKTV->Dict.ReadString( mesg );
			if( diff & EPFXEmitterWriteRotQuat )
				e->RotQuat = mesg->ReadQuaternion();
			if( diff & EPFXEmitterWritePosition )
				e->Position = mesg->ReadPositionOptimized();
			if( diff & EPFXEmitterWritePositionDiffOnly )
				e->Position = mesg->ReadPositionDiffOptimized() + Emitters[i].Position;
			e->LastPosition = Emitters[i].Position;
			if( diff & EPFXEmitterWriteFlags )
				GetEmitterDataFromFlags( e, mesg->ReadByte() );
			if( diff & EPFXEmitterWritePos )
				e->Pos = mesg->ReadVector();
			if( diff & EPFXEmitterWriteRotation )
				e->Rotation = mesg->ReadVector();
			if( diff & EPFXEmitterWriteScale )
				e->Scale = mesg->ReadFloat();
		}
	}

	mesg->SetProfilingCategory( lastCategory );
*/
	return contents;
}

unsigned int ParticleFXEntityProxy::CalcDiff( EntityProxy *diffFrom )
{
	unsigned int contents = EntityProxy::CalcDiff( diffFrom );
/*
	ParticleFXEntityProxy *diff = ( ParticleFXEntityProxy *)diffFrom;

	if( ParentOffset != diff->ParentOffset )
		contents |= EProxyWritePFXParentOffset;
	if( ParentRotation != diff->ParentRotation )
		contents |= EProxyWritePFXParentRotation;
	if( !( ParentRotationOffset == diff->ParentRotationOffset ) )
		contents |= EProxyWritePFXParentRotationOffset;
	if( CameraOffset != diff->CameraOffset )
		contents |= EProxyWritePFXCameraOffset;
	if( Active != diff->Active )
		contents |= EProxyWritePFXActive;
	if( IsImmortal != diff->IsImmortal )
		contents |= EProxyWritePFXIsImmortal;
	if( EmitterCount != diff->EmitterCount )
	{
		contents |= EProxyWritePFXEmitterCount;
		contents |= EProxyWritePFXEmitters;
	}
	else
		for( int i = 0; i < EmitterCount; ++i )
			if( CalcEmitterDiff( &Emitters[i], &diff->Emitters[i] ) > 0 )
			{
				contents |= EProxyWritePFXEmitters;
				break;
			}
*/
	return contents;
}

void ParticleFXEntityProxy::GetEmitterDataFromFlags( Emitter *e, unsigned char flags )
{
	e->Active = ( flags & ( 1 << 0 ) ) != 0;
	e->Respawn = ( flags & ( 1 << 1 ) ) != 0;
	e->Evolve = ( flags & ( 1 << 2 ) ) != 0;
	e->Immortal = ( flags & ( 1 << 3 ) ) != 0;
	e->Dying = ( flags & ( 1 << 4 ) ) != 0;
}

unsigned char ParticleFXEntityProxy::GetEmitterFlagsFromData( Emitter *e )
{
	unsigned char flags = 0;
	
	if( e->Active ) flags |= ( 1 << 0 );
	if( e->Respawn ) flags |= ( 1 << 1 );
	if( e->Evolve ) flags |= ( 1 << 2 );
	if( e->Immortal ) flags |= ( 1 << 3 );
	if( e->Dying ) flags |= ( 1 << 4 );

	return flags;
}

unsigned char ParticleFXEntityProxy::CalcEmitterDiff( Emitter *e, Emitter *diffFrom )
{
	unsigned char diff = 0;

	if( e->Name != diffFrom->Name )
		diff |= EPFXEmitterWriteName;
	if( !( e->RotQuat == diffFrom->RotQuat ) )
		diff |= EPFXEmitterWriteRotQuat;
	if( e->Position != diffFrom->Position )
	{
		if( !UdpMessage::CanUsePositionDiff( e->Position - diffFrom->Position ) || ( rand() < RAND_MAX/50 ) )
			diff |= EPFXEmitterWritePosition;
		else
			diff |= EPFXEmitterWritePositionDiffOnly;
	}
	if( GetEmitterFlagsFromData( e ) != GetEmitterFlagsFromData( diffFrom ) )
		diff |= EPFXEmitterWriteFlags;
	if( e->Pos != diffFrom->Pos )
		diff |= EPFXEmitterWritePos;
	if( e->Rotation != diffFrom->Rotation )
		diff |= EPFXEmitterWriteRotation;
	if( e->Scale != diffFrom->Scale )
		diff |= EPFXEmitterWriteScale;

	return diff;
}

#ifdef PKTV_DISPLAY


Entity *ParticleFXEntityProxy::MakeEntity( void )
{
	ParticleEffect *e = ( ParticleEffect *)EntityProxy::MakeEntity();
	if( e )
	{
		e->ParentOffset = ParentOffset;
		e->ParentRotation = ParentRotation;
		e->ParentRotationOffset = ParentRotationOffset;
		e->CameraOffset = CameraOffset;
		e->Active = Active;
		e->IsImmortal = IsImmortal;

		for( int i = 0; i < EmitterCount; i++ )
		{
			ParticleEmitter *emitter = GEngine->Video->ParticleSys.CreateEmitter(Emitters[i].Name.AsChar());
			Assert( emitter );
			e->AddEmitter(emitter);

			e->Emitters[i].Emitter->RotQuat = Emitters[i].RotQuat;
			e->Emitters[i].Emitter->Position = Emitters[i].Position;
			e->Emitters[i].Emitter->LastPosition = Emitters[i].LastPosition;

			e->Emitters[i].Emitter->Active = Emitters[i].Active;
			e->Emitters[i].Emitter->Respawn = Emitters[i].Respawn;
			e->Emitters[i].Emitter->Evolve = Emitters[i].Evolve;
			e->Emitters[i].Emitter->Immortal = Emitters[i].Immortal;
			e->Emitters[i].Emitter->Dying = Emitters[i].Dying;

			e->Emitters[i].Pos = Emitters[i].Pos;
			e->Emitters[i].Rotation = Emitters[i].Rotation;
			e->Emitters[i].Scale = Emitters[i].Scale;
		}

		e->SetupEmitters();
	}

	return e;
}


void ParticleFXEntityProxy::DeleteEntity( void )
{
	return EntityProxy::DeleteEntity();
}


void ParticleFXEntityProxy::UpdateEntity( EntityProxy *nextData, float fraction, DWORD timeDiffMs )
{
	EntityProxy::UpdateEntity( nextData, fraction, timeDiffMs );
}


#endif // PKTV_DISPLAY

// =================================================================================================================

TrailEntityProxy::TrailEntityProxy( void )
:	EntityProxy()
,	BonesData(0)
,	BonesNum(0)
,	Times(0)
,	Positions(0)
{}

TrailEntityProxy::TrailEntityProxy( UdpMessage *creationData )
:	BonesData(0)
,	BonesNum(0)
,	Times(0)
,	Positions(0)
{
	ReadFullState( creationData );
}

TrailEntityProxy::~TrailEntityProxy( void )
{
	if( Times )		delete[] Times,		Times = 0;
	if( Positions )	delete[] Positions,	Positions = 0;

	if( BonesData )
	{
		delete[] BonesData;
		BonesData = 0;
		BonesNum = 0;
	}

#ifdef	TRANSGAMING
	Destroy();
#else
	EntityProxy::~EntityProxy();
#endif	

}

void TrailEntityProxy::StoreFullState( UdpMessage* mesg )
{
	EntityProxy::StoreFullState( mesg );

	GEngine->PKTV->Dict.WriteString( TextureName, mesg );
	mesg->WriteFloat( ScaleFactor );
	mesg->WriteULong( ConstColor );
	mesg->WriteFloat( Time );
	mesg->WriteBool( PositionChanged );
	mesg->WriteLong( MaxElems );
	mesg->WriteLong( BonesNum );
	mesg->WriteLong( HeadNode );
	mesg->WriteLong( TailNode );
	mesg->WriteLong( Vpn );

	int tracks = BonesNum < 2 ? 2 : BonesNum;

	for( int i = 0; i < tracks; ++i )
		mesg->WriteLong( BonesData[i].Bone );

	for( int i = 0; i < tracks; ++i )
		mesg->WriteVector( BonesData[i].LastPosition );

	for( int i = 0; i < tracks; ++i )
		mesg->WriteVector( BonesData[i].LastNodePosition );

	int counter = ( HeadNode + MaxElems - TailNode ) % MaxElems;
	for( int i = 0; i < counter; ++i )
	{
		mesg->WriteFloat( Times[i] );
		for( int j = 0; j < tracks; ++j )
			mesg->WriteVector( Positions[j + i * tracks] );
	}
}

unsigned int TrailEntityProxy::StoreDiff( UdpMessage *mesg, EntityProxy *diffFrom )
{
	unsigned int contents = EntityProxy::StoreDiff( mesg, diffFrom );

	if( contents & EProxyWriteTrailBonesNum )
		mesg->WriteLong( BonesNum );

	return contents;
}

EntityProxy *TrailEntityProxy::CloneMyself( void )
{
	TrailEntityProxy *proxy = new TrailEntityProxy();
	CopyData( proxy );
	return proxy;
}

#ifdef PKTV_DISPLAY

void TrailEntityProxy::ReadFromEntity( Entity *e )
{
	EntityProxy::ReadFromEntity( e );

	Trail *t = ( Trail *)e;

	mConstantData->FileName = t->GetEffectName();

	if( t->GetTextureName() )
		TextureName = t->GetTextureName();
	ScaleFactor = t->GetScaleFactor();
	ConstColor = t->GetConstColor();
	Time = t->GetTime();
	PositionChanged = t->GetPositionChanged();
	MaxElems = t->GetMaxElems();
	int oldTracks = BonesNum < 2 ? 2 : BonesNum;
	BonesNum = t->GetBonesNum();
	HeadNode = t->GetHeadNode();
	TailNode = t->GetTailNode();
	Vpn = t->GetVpn();
	
	int tracks = BonesNum < 2 ? 2 : BonesNum;

	if( tracks != oldTracks )
		delete[] BonesData,	BonesData = 0;
	if( !BonesData )
		BonesData = new TrailTable[tracks];

	for( int i = 0; i < tracks; ++i )
		BonesData[i].Bone = t->GetTrailBones()[i];

	for( int i = 0; i < tracks; ++i )
		BonesData[i].LastPosition = t->GetLastPosition()[i];

	for( int i = 0; i < tracks; ++i )
		BonesData[i].LastNodePosition = t->GetLastNodePosition()[i];

	int counter = ( HeadNode + MaxElems - TailNode ) % MaxElems;

	if( Positions ) delete[] Positions;
	Positions = new Vector[counter*Vpn];
	if( Times ) delete[] Times;
	Times = new float[counter];

	for( int i = TailNode, k = 0; i != HeadNode; i = ( i + 1 ) % MaxElems, ++k )
	{
		Times[k] = t->GetTimes()[i];
		for( int j = 0; j < tracks; ++j )
			Positions[j + k * tracks] = t->GetPositions()[j + i * tracks];
		Assert( k < counter );
	}
}

#endif

void TrailEntityProxy::ReadFullState( UdpMessage *mesg )
{
	EntityProxy::ReadFullState( mesg );

	int lastCategory = mesg->GetProfilingCategory();
	mesg->SetProfilingCategory( TrailEntityProxyFull );

	TextureName = GEngine->PKTV->Dict.ReadString( mesg );
	ScaleFactor = mesg->ReadFloat();
	ConstColor = mesg->ReadULong();
	Time = mesg->ReadFloat();
	PositionChanged = mesg->ReadBool();
	MaxElems = mesg->ReadLong();
	int oldTracks = BonesNum < 2 ? 2 : BonesNum;
	BonesNum = mesg->ReadLong();
	HeadNode = mesg->ReadLong();
	TailNode = mesg->ReadLong();
	Vpn = mesg->ReadLong();

	int tracks = BonesNum < 2 ? 2 : BonesNum;
	if( tracks != oldTracks )
		delete[] BonesData,	BonesData = 0;
	if( !BonesData )
		BonesData = new TrailTable[tracks];

	for( int i = 0; i < tracks; ++i )
		BonesData[i].Bone = mesg->ReadLong();

	for( int i = 0; i < tracks; ++i )
		BonesData[i].LastPosition = mesg->ReadVector();

	for( int i = 0; i < tracks; ++i )
		BonesData[i].LastNodePosition = mesg->ReadVector();

	int counter = ( HeadNode + MaxElems - TailNode ) % MaxElems;

	if( Positions ) delete[] Positions;
	Positions = new Vector[counter*Vpn];
	if( Times ) delete[] Times;
	Times = new float[counter];

	for( int i = 0; i < counter; ++i )
	{
		Times[i] = mesg->ReadFloat();
		for( int j = 0; j < tracks; ++j )
			Positions[j + i * tracks] = mesg->ReadVector();
	}

	mesg->SetProfilingCategory( lastCategory );
}

void TrailEntityProxy::CopyData( EntityProxy *target )
{
	EntityProxy::CopyData( target );

	TrailEntityProxy *t = ( TrailEntityProxy *)target;
	t->TextureName = TextureName;
	t->ScaleFactor = ScaleFactor;
	t->ConstColor = ConstColor;
	t->Time = Time;
	t->PositionChanged = PositionChanged;
	t->MaxElems = MaxElems;
	int oldTracks = t->BonesNum < 2 ? 2 : t->BonesNum;
	t->BonesNum = BonesNum;
	t->HeadNode = HeadNode;
	t->TailNode = TailNode;
	t->Vpn = Vpn;

	int tracks = BonesNum < 2 ? 2 : BonesNum;
	if( tracks != oldTracks )
		delete[] t->BonesData,	t->BonesData = 0;
	if( !t->BonesData )
		t->BonesData = new TrailTable[tracks];

	::memcpy( BonesData, t->BonesData, tracks*sizeof(TrailTable) );

	int counter = ( HeadNode + MaxElems - TailNode ) % MaxElems;

	if( t->Positions ) delete[] t->Positions;
	t->Positions = new Vector[counter*Vpn];
	if( t->Times ) delete[] t->Times;
	t->Times = new float[counter];

	::memcpy( Times, t->Times, counter*sizeof(float) );
	::memcpy( Positions, t->Positions, counter*sizeof(Vector)*Vpn );
}

unsigned int TrailEntityProxy::ReadDiff( EntityProxy *proxy, UdpMessage *mesg, DWORD frameTimeDelta )
{
	unsigned int contents = EntityProxy::ReadDiff( proxy, mesg, frameTimeDelta );

	int lastCategory = mesg->GetProfilingCategory();
	mesg->SetProfilingCategory( TrailEntityProxyDiff );

	TrailEntityProxy *p = ( TrailEntityProxy *)proxy;

	if( contents & EProxyWriteTrailBonesNum )
		p->BonesNum = mesg->ReadLong();

	mesg->SetProfilingCategory( lastCategory );

	return contents;
}

unsigned int TrailEntityProxy::CalcDiff( EntityProxy *diffFrom )
{
	unsigned int contents = EntityProxy::CalcDiff( diffFrom );

	TrailEntityProxy *diff = ( TrailEntityProxy *)diffFrom;

	if( BonesNum != diff->BonesNum )
		contents |= EProxyWriteTrailBonesNum;

	return contents;
}

#ifdef PKTV_DISPLAY


Entity *TrailEntityProxy::MakeEntity( void )
{
	Trail *e = ( Trail *)EntityProxy::MakeEntity();

	if( TextureName.Len() )
		e->SetTexture( TextureName );

	e->SetPosition( Translation );
	e->SetScaleFactor( ScaleFactor );
	e->SetConstColor( ConstColor );
	e->SetTime( Time );
	e->SetPositionChanged( true/*PositionChanged*/ );
	e->SetMaxElems( MaxElems );
	e->SetBonesNum( BonesNum );
	e->SetNodes( HeadNode, TailNode );
	e->SetVpn( Vpn );

	int tracks = BonesNum < 2 ? 2 : BonesNum;

	e->Init( MaxElems, tracks );

	int *bones = ( int *)e->GetTrailBones();
	for( int i = 0; i < tracks; ++i )
		bones[i] = BonesData[i].Bone;

	Vector *lastPos = ( Vector *)e->GetLastPosition();
	for( int i = 0; i < tracks; ++i )
		lastPos[i] = BonesData[i].LastPosition;

	Vector *lastNodePos = ( Vector *)e->GetLastNodePosition();
	for( int i = 0; i < tracks; ++i )
		lastNodePos[i] = BonesData[i].LastNodePosition;

	for( int i = TailNode, k = 0; i != HeadNode; i = ( i + 1 ) % MaxElems, ++k )
	{
		e->GetTimes()[i] = Times[k];
		for( int j = 0; j < tracks; ++j )
			e->GetPositions()[j + i * tracks] = Positions[j + k * tracks];
	}

	e->UpdateTransform();

	return e;
}


void TrailEntityProxy::DeleteEntity( void )
{
	return EntityProxy::DeleteEntity();
}


void TrailEntityProxy::UpdateEntity( EntityProxy *nextData, float fraction, DWORD timeDiffMs )
{
	EntityProxy::UpdateEntity( nextData, fraction, timeDiffMs );

	int index = GetIndex();
#ifdef	TRANSGAMING
	if( GEngine->pcfWorld->EntityGIndexes.Size() > index )
	{
		Trail *t = ( Trail *)GEngine->pcfWorld->EntityGIndexes[index];
		if( t ) t->SetBonesNum( BonesNum );
	}
#else
	if( GEngine->World->EntityGIndexes.Size() > index )
	{
		Trail *t = ( Trail *)GEngine->World->EntityGIndexes[index];
		if( t ) t->SetBonesNum( BonesNum );
	}
#endif
}


#endif // PKTV_DISPLAY
