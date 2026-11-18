#include "Engine.h"

#include "PKTVUnpackedFrame.h"
#include "ProxySystem.h"
#include "PKTVManager.h"
#include "LogBuffer.h"

#define PKTVDelete 0
#define PKTVSkip 1
#define PKTVCreate 2
#define PKTVUpdate 3

ulong32 EntityProxy::OverallMemoryTaken;
ulong32 BillboardEntityProxy::OverallMemoryTaken;
ulong32 DecalEntityProxy::OverallMemoryTaken;
ulong32 LightEntityProxy::OverallMemoryTaken;
ulong32 ModelEntityProxy::OverallMemoryTaken;
ulong32 ParticleFXEntityProxy::OverallMemoryTaken;
ulong32 TrailEntityProxy::OverallMemoryTaken;

PKTVUnpackedFrame::PKTVUnpackedFrame( UdpMessage* packedFrame, PKTVUnpackedFrame* diffFrame )
:	NextFrame(0)
,	mMemoryTaken(0)
,	mCurrentPlayer(0)
{
	PackedFrame = packedFrame;
	PackedFrame->SeekToBegin();
	unsigned short timeDiff = PackedFrame->ReadUShort();
	FrameNumber = PackedFrame->ReadULongOptimized();

	mMapName = "";

	if( timeDiff )
	{
		Assert( diffFrame );
		ReadEngineState( packedFrame, diffFrame );
	}
	else
		ReadEngineState( packedFrame, 0 );

	unsigned short entitiesNum = PackedFrame->ReadUShort();

	Time = timeDiff?diffFrame->Time + timeDiff:0;

	if( !timeDiff )	// full frame
	{
		for( int i = 0; i < entitiesNum; ++i )
		{
			EntityProxy* e = 0;
			unsigned char type = PackedFrame->ReadUChar();
			switch( type )
			{
			case 0:
			case EntityProxy::ETypeMesh:
				e = new EntityProxy( PackedFrame );
				break;

			case EntityProxy::ETypeDecal:
				e = new DecalEntityProxy( PackedFrame );
				break;

			case EntityProxy::ETypeModel:
				e = new ModelEntityProxy( PackedFrame );
				break;

			case EntityProxy::ETypeParticleFx:
				e = new ParticleFXEntityProxy( PackedFrame );
				break;

			case EntityProxy::ETypeTrail:
				e = new TrailEntityProxy( PackedFrame );
				break;

			case EntityProxy::ETypeBillboard:
				e = new BillboardEntityProxy( PackedFrame );
				break;

			case EntityProxy::ETypeLight:
				e = new LightEntityProxy( PackedFrame );
				break;

			default:
				Assert( false );
				break;
			}

			Assert( e );
			Proxies.Add( e );
		}
	}
	else
	{
		Assert( diffFrame );
		Assert( diffFrame->FrameNumber == FrameNumber-1 );

		int from = 0;

		UdpMessage bits;
		int numBytes = (int)ceil(entitiesNum/4.0);
		for( int i = 0; i < numBytes; ++i )
			bits.WriteByte( PackedFrame->ReadByte() );
		bits.EndWriting( true );
		bits.SeekToBegin();

		for( int i = 0; i < entitiesNum; ++i )
		{
			unsigned char entityActionType = bits.ReadBits( 2 );
			switch( entityActionType )
			{
			case PKTVDelete:	// entity got deleted - don't copy Proxies[from] anywhere here
				++from;
				break;
			case PKTVSkip:		// entity gets copied fully from diffFrame
				Proxies.Add( diffFrame->Proxies[from]->CloneMyself() );
				++from;
				break;
			case PKTVCreate:	// entity gets created from the info in the stream
				{
					EntityProxy* e = 0;
					unsigned char type = PackedFrame->ReadUChar();
					switch( type )
					{
					case EntityProxy::ETypeMesh:
						e = new EntityProxy( PackedFrame );
						break;

					case EntityProxy::ETypeDecal:
						e = new DecalEntityProxy( PackedFrame );
						break;

					case EntityProxy::ETypeModel:
						e = new ModelEntityProxy( PackedFrame );
						break;

					case EntityProxy::ETypeParticleFx:
						e = new ParticleFXEntityProxy( PackedFrame );
						break;

					case EntityProxy::ETypeTrail:
						e = new TrailEntityProxy( PackedFrame );
						break;

					case EntityProxy::ETypeBillboard:
						e = new BillboardEntityProxy( PackedFrame );
						break;

					case EntityProxy::ETypeLight:
						e = new LightEntityProxy( PackedFrame );
						break;

					default:
						Assert( false );
						break;
					}

					Assert( e );
					Proxies.Add( e );
				}
				break;
			case PKTVUpdate:	// entity gets updated - proxy gets created from diff entity and info from stream
				Proxies.Add( diffFrame->Proxies[from]->CreateFromDiffAndMyself( PackedFrame, timeDiff ) );
				++from;
				break;
			}
		}
	}

	PackedFrame->SeekToBegin();
	SortEntitiesByGIndex();	// TO DO - do we need this?
}

PKTVUnpackedFrame::~PKTVUnpackedFrame( void )
{
	for( int i = 0; i < Proxies.Size(); ++i )
		delete Proxies[i];
	Assert( PackedFrame );
	delete PackedFrame;
	PackedFrame = 0;
}

int __cdecl SortProxies( const void *proxy1, const void *proxy2 )
{
#pragma warning(disable:4312)
	return ((EntityProxy*)(*(DWORD*)proxy1))->GetIndex()-((EntityProxy*)(*(DWORD*)proxy2))->GetIndex();
#pragma warning(default:4312)
}

void PKTVUnpackedFrame::SortEntitiesByGIndex( void )
{
	if( Proxies.Size() > 1 )
		qsort( &Proxies[0], Proxies.Size(), sizeof( EntityProxy *), SortProxies );
}

UdpMessage* PKTVUnpackedFrame::MakePackedFrame( PKTVUnpackedFrame* diffFrame )
{
	UdpMessage* tempFrame = new UdpMessage( 65536 );

	UdpMessage tempStore( 65536 );

	if( diffFrame )
	{
		Assert( FrameNumber == diffFrame->FrameNumber+1 );

		// Make real diff frame
		unsigned short timeDiff = Time - diffFrame->Time;
		Assert( timeDiff > 0 );
		tempFrame->WriteUShort( timeDiff );
		tempFrame->WriteULongOptimized( FrameNumber );

		WriteEngineState( tempFrame, diffFrame );

		int from = 0;
		int to = 0;

		unsigned short entityNum = 0;
		int entityNumIndex = tempFrame->GetPos();
		tempFrame->WriteUShort(0);

		while( ( to < Proxies.Size() ) || ( from < diffFrame->Proxies.Size() ) )
		{
			++entityNum;
			if( to == Proxies.Size() )								// entity deleted
			{
				tempFrame->WriteBits( PKTVDelete, 2 );
				++from;
				continue;
			}
			else if( from == diffFrame->Proxies.Size() )			// entity created
			{
				tempFrame->WriteBits( PKTVCreate, 2 );
				BYTE toType = Proxies[to]->GetType();
#ifdef _DEBUG
				if( toType > 11 ) GLog->Print( "aaa" );
#endif
				tempStore.WriteUChar( toType );
				Proxies[to]->StoreFullState( &tempStore );
				++to;
				continue;
			}

			int thisIndex = Proxies[to]->GetIndex();
			int lastIndex = diffFrame->Proxies[from]->GetIndex();

			if( thisIndex < lastIndex )								// entity created
			{
				tempFrame->WriteBits( PKTVCreate, 2 );
				BYTE toType = Proxies[to]->GetType();
#ifdef _DEBUG
				if( toType > 11 ) GLog->Print( "bbb" );
#endif
				tempStore.WriteUChar( toType );
				Proxies[to]->StoreFullState( &tempStore );
				++to;
			}
			else if( thisIndex > lastIndex )						// entity deleted
			{
				tempFrame->WriteBits( PKTVDelete, 2 );
				++from;
			}
			else													// entity diff
			{
				if( Proxies[to]->StoreDiff( &tempStore, diffFrame->Proxies[from] ) )
					tempFrame->WriteBits( PKTVUpdate, 2 );
				else
					tempFrame->WriteBits( PKTVSkip, 2 );
				++to;
				++from;
			}
		}

		tempFrame->ResetBitPos();
		int pos2 = tempFrame->GetPos();
		tempFrame->Seek( entityNumIndex );
		tempFrame->WriteUShort( entityNum );
		tempFrame->Seek( pos2 );

		tempStore.EndWriting( true );
		tempFrame->WriteData( &tempStore );
	}
	else
	{
		// Make keyframe (first)

		tempFrame->WriteUShort( 0 );	// timeDiff == 0 means its a keyframe
		tempFrame->WriteULongOptimized( FrameNumber );
		WriteEngineState( tempFrame, 0 );
		tempFrame->WriteUShort( Proxies.Size() );
		for( int i = 0; i < Proxies.Size(); ++i )
		{
			tempFrame->WriteUChar( Proxies[i]->GetType() );
			Proxies[i]->StoreFullState( tempFrame );
		}
	}
	tempFrame->EndWriting(true);
	Assert( tempFrame->GetSize() < 65536 );
	tempFrame->Trim();
	tempFrame->SeekToBegin();
	return tempFrame;
}

void PKTVUnpackedFrame::WriteEngineState( UdpMessage *msg, PKTVUnpackedFrame* diffFrame )
{
	if( !diffFrame )
	{
		GSystem->PKTV->Dict.WriteString( mMapName, msg );

		msg->WriteVector( mCamRot );
		msg->WriteVector( mCamPos );
		msg->WriteFloat( mCamFOV );

		msg->WriteBool( mGhostsFX );
		msg->WriteBool( mDemonFXEnabled );
		msg->WriteBool( mDemonFXSuper );
		msg->WriteFloat( mDemonFXWarp );

		msg->WriteByte( ( BYTE )mPlayerData.Size() );

		for( int i = 0; i < mPlayerData.Size(); i++ )
		{
			msg->WriteShort( mPlayerData[i].health );
			msg->WriteShort( mPlayerData[i].armor );
			msg->WriteByte( mPlayerData[i].armorType );

			msg->WriteByte( mPlayerData[i].weapon );
			msg->WriteBool( mPlayerData[i].switchFire );
			msg->WriteShort( mPlayerData[i].ammoPrimary );
			msg->WriteShort( mPlayerData[i].ammoSecondary );
		}
	}
	else
	{
		unsigned short state = 0;
		int pos = msg->GetPos();
		msg->WriteUShort( 0 );

		if( mMapName != diffFrame->mMapName )			{ state |= 1<<0; GSystem->PKTV->Dict.WriteString( mMapName, msg ); }
		if( mCamRot != diffFrame->mCamRot )				{ state |= 1<<1; msg->WriteVector( mCamRot ); }
		if( mCamPos != diffFrame->mCamPos )				{ state |= 1<<2; msg->WriteVector( mCamPos ); }
		if( mCamFOV != diffFrame->mCamFOV )				{ state |= 1<<3; msg->WriteFloat( mCamFOV ); }
		if( mGhostsFX )									{ state |= 1<<4; }
		if( mDemonFXEnabled )							{ state |= 1<<5; }
		if( mDemonFXSuper )								{ state |= 1<<6; }
		if( mDemonFXWarp != diffFrame->mDemonFXWarp )	{ state |= 1<<7; msg->WriteFloat( mDemonFXWarp ); }

		int pos2 = msg->GetPos();
		msg->Seek( pos );
		msg->WriteUShort( state );
		msg->Seek( pos2 );

		msg->WriteByte( ( BYTE )mPlayerData.Size() );

		for( int i = 0; i < mPlayerData.Size(); i++ )
		{
			pos = msg->GetPos();
			msg->WriteUShort( 0 );
			state = 0;

			if( mPlayerData[i].health != diffFrame->mPlayerData[i].health )			{ state |= 1<<1; msg->WriteShort( mPlayerData[i].health ); }
			if( mPlayerData[i].armor != diffFrame->mPlayerData[i].armor )			{ state |= 1<<2; msg->WriteShort( mPlayerData[i].armor ); }
			if( mPlayerData[i].armorType != diffFrame->mPlayerData[i].armorType )	{ state |= 1<<3; msg->WriteByte( mPlayerData[i].armorType ); }

			if( mPlayerData[i].weapon != diffFrame->mPlayerData[i].weapon )				{ state |= 1<<4; msg->WriteByte( mPlayerData[i].weapon ); }
			if( mPlayerData[i].switchFire != diffFrame->mPlayerData[i].switchFire )		{ state |= 1<<5; msg->WriteBool( mPlayerData[i].switchFire ); }
			if( mPlayerData[i].ammoPrimary != diffFrame->mPlayerData[i].ammoPrimary )	{ state |= 1<<6; msg->WriteShort( mPlayerData[i].ammoPrimary ); }
			if( mPlayerData[i].ammoSecondary != diffFrame->mPlayerData[i].ammoSecondary){ state |= 1<<7; msg->WriteShort( mPlayerData[i].ammoSecondary ); }

			pos2 = msg->GetPos();
			msg->Seek( pos );
			msg->WriteUShort( state );
			msg->Seek( pos2 );
		}
	}
}

void PKTVUnpackedFrame::ReadEngineState( UdpMessage *msg, PKTVUnpackedFrame* diffFrame )
{
	if( !diffFrame )
	{
		mMapName = GSystem->PKTV->Dict.ReadString( msg );

		mCamRot = msg->ReadVector();
		mCamPos = msg->ReadVector();
		mCamFOV = msg->ReadFloat();

		mGhostsFX = msg->ReadBool();
		mDemonFXEnabled = msg->ReadBool();
		mDemonFXSuper = msg->ReadBool();
		mDemonFXWarp = msg->ReadFloat();

		mPlayerData.Clear();

		BYTE numPlayers = msg->ReadByte();

		for( int i = 0; i < numPlayers; i++ )
		{
			mPlayerData.Grow();

			mPlayerData[i].health = msg->ReadShort();
			mPlayerData[i].armor = msg->ReadShort();
			mPlayerData[i].armorType = msg->ReadByte();

			mPlayerData[i].weapon = msg->ReadByte();
			mPlayerData[i].switchFire = msg->ReadBool();
			mPlayerData[i].ammoPrimary = msg->ReadShort();
			mPlayerData[i].ammoSecondary = msg->ReadShort();
		}
	}
	else
	{
		unsigned short state = msg->ReadUShort();

		mMapName		= ( state & (1<<0) ) ? GSystem->PKTV->Dict.ReadString( msg )	: diffFrame->mMapName;
		mCamRot			= ( state & (1<<1) ) ? msg->ReadVector()						: diffFrame->mCamRot;
		mCamPos			= ( state & (1<<2) ) ? msg->ReadVector()						: diffFrame->mCamPos;
		mCamFOV			= ( state & (1<<3) ) ? msg->ReadFloat()							: diffFrame->mCamFOV;
		mGhostsFX		= ( ( state & (1<<4) ) != 0 ); 
		mDemonFXEnabled	= ( ( state & (1<<5) ) != 0 );
		mDemonFXSuper	= ( ( state & (1<<6) ) != 0 );
		mDemonFXWarp	= ( state & (1<<7) ) ? msg->ReadFloat()							: diffFrame->mDemonFXWarp;

		BYTE numPlayers = msg->ReadByte();

		for( int i = 0; i < numPlayers; i++ )
		{
			if( mPlayerData.Size() == i )
				mPlayerData.Grow();

			state = msg->ReadUShort();
			mPlayerData[i].health			= ( state & (1<<1) ) ? msg->ReadShort()			: diffFrame->mPlayerData[i].health;
			mPlayerData[i].armor			= ( state & (1<<2) ) ? msg->ReadShort()			: diffFrame->mPlayerData[i].armor;
			mPlayerData[i].armorType		= ( state & (1<<3)) ? msg->ReadByte()			: diffFrame->mPlayerData[i].armorType;

			mPlayerData[i].weapon			= ( state & (1<<4)) ? msg->ReadByte()			: diffFrame->mPlayerData[i].weapon;
			mPlayerData[i].switchFire		= ( state & (1<<5)) ? msg->ReadBool()			: diffFrame->mPlayerData[i].switchFire;
			mPlayerData[i].ammoPrimary		= ( state & (1<<6)) ? msg->ReadShort()			: diffFrame->mPlayerData[i].ammoPrimary;
			mPlayerData[i].ammoSecondary	= ( state & (1<<7)) ? msg->ReadShort()			: diffFrame->mPlayerData[i].ammoSecondary;
		}
	}
}

ulong32 PKTVUnpackedFrame::MemoryTaken( bool report )
{
	if( !report && mMemoryTaken )
		return mMemoryTaken;

	if( report )
	{
		// Init data-gathering structs
		EntityProxy::OverallMemoryTaken = 0;
		BillboardEntityProxy::OverallMemoryTaken = 0;
		DecalEntityProxy::OverallMemoryTaken = 0;
		LightEntityProxy::OverallMemoryTaken = 0;
		ModelEntityProxy::OverallMemoryTaken = 0;
		ParticleFXEntityProxy::OverallMemoryTaken = 0;
		TrailEntityProxy::OverallMemoryTaken = 0;
	}

	ulong32 staticPart = sizeof(PKTVUnpackedFrame);
	staticPart += mMapName.MemoryTaken()-sizeof(String);

	ulong32 proxiesPart = 0;

	for( int i = 0; i < Proxies.Size(); ++i )
		proxiesPart += Proxies[i]->MemoryTaken();

	// Constant data not included here!!

	if( report )
	{
		ulong32 packedFramePart = 0;

		if( PackedFrame )
		{
			packedFramePart += sizeof(PackedFrame);
			packedFramePart += PackedFrame->GetSize();
		}

		GLog->Print( "Frame %u - memory taken %u bytes (static %u bytes, packed frame %u bytes, proxies (%u elems) %u bytes\n",
			FrameNumber, staticPart+packedFramePart+proxiesPart, staticPart, packedFramePart, Proxies.Size(), proxiesPart );

		// Report data-gathering structs
		GLog->Print( "Overall memory taken by EntityProxy: %u bytes\n", EntityProxy::OverallMemoryTaken );
		GLog->Print( "Overall memory taken by BillboardEntityProxy: %u bytes\n", BillboardEntityProxy::OverallMemoryTaken );
		GLog->Print( "Overall memory taken by DecalEntityProxy: %u bytes\n", DecalEntityProxy::OverallMemoryTaken );
		GLog->Print( "Overall memory taken by LightEntityProxy: %u bytes\n", LightEntityProxy::OverallMemoryTaken );
		GLog->Print( "Overall memory taken by ModelEntityProxy: %u bytes\n", ModelEntityProxy::OverallMemoryTaken );
		GLog->Print( "Overall memory taken by ParticleFXEntityProxy: %u bytes\n", ParticleFXEntityProxy::OverallMemoryTaken );
		GLog->Print( "Overall memory taken by TrailEntityProxy: %u bytes\n", TrailEntityProxy::OverallMemoryTaken );
	}

	mMemoryTaken = staticPart+proxiesPart;
	return mMemoryTaken;
}
