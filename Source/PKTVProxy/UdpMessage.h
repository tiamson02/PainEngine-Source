
#pragma once

#define MAX_UDP_PACKET_SIZE 1400
#define SAFE_UDP_PACKET_SIZE 1350
#define UDP_FAULT_LOG_FILE_NAME "UdpFaults.log"

#define UDP_END_WRITING_CRASH_LOG

#ifdef UDP_END_WRITING_CRASH_LOG
extern int EndWritingCrashNumber;
#endif

class Entity;

#include "LeString.h"
#include "LeMath.h"

class  UdpMessage
{
	friend class NetworkDevice2;
	friend class NetworkPeer2;
	friend class PKTVProxyConnection;
public:
	UdpMessage( int maxSize = MAX_UDP_PACKET_SIZE ) : mSize(0), mDelete(true), mWasSent(false), mBitsPos(0) { mPos = mMessage = new char[maxSize]; }
	UdpMessage( char* mesg, int size, bool del = false ) : mMessage(mesg), mPos(mesg), mSize(size), mDelete(del), mWasSent(false), mBitsPos(0) {}

	~UdpMessage( void ) { if( mDelete ) delete[] mMessage; };

	void Trim( void );	// Trim buffer size to message size

	void SeekToBegin( void ) { mPos = mMessage; };
	void Seek( int pos ) { mPos = mMessage + pos; };
	int GetPos( void ) const { return mPos - mMessage; };
	int GetSize( void ) const { return mSize; };			// when writing valid only after EndWriting() call.

	// Message reading inlines - fast - no htons,htonl at all
	bool			ReadBool( void )		{ mPos += sizeof(unsigned char); return (*(mPos-sizeof(unsigned char)) != 0 ); };
	unsigned char	ReadByte( void )		{ mPos += sizeof(unsigned char); return *(unsigned char*)(mPos-sizeof(unsigned char)); };
	char			ReadChar( void )		{ mPos += sizeof(char); return *(char*)(mPos-sizeof(char)); };
	unsigned char	ReadUChar( void )		{ mPos += sizeof(unsigned char); return *(unsigned char*)(mPos-sizeof(unsigned char)); };
	short			ReadShort( void )		{ mPos += sizeof(short); return *(short*)(mPos-sizeof(short)); };
	unsigned short	ReadUShort( void )		{ mPos += sizeof(unsigned short); return *(unsigned short*)(mPos-sizeof(unsigned short)); };
	long32			ReadLong( void )		{ mPos += sizeof(long32); return *(long32*)(mPos-sizeof(long32)); };
	ulong32	ReadULong( void )		{ mPos += sizeof(ulong32); return *(ulong32*)(mPos-sizeof(ulong32)); };
	float			ReadFloat( void )		{ mPos += sizeof(float); return *(float*)(mPos-sizeof(float)); };
	double			ReadDouble( void )		{ mPos += sizeof(double); return *(double*)(mPos-sizeof(double)); };
	String			ReadString( void )		{ String str( mPos ); mPos += (str.Len()+1); return str; };
	String			ReadStringNL( void )	{ char* w = mPos; while( *w != 10) w++; *w = 0; String str( mPos ); mPos += (str.Len()+1); *w = 10; return str; }
	Vector			ReadVector( void )		{ Vector t; t.X = ReadFloat(); t.Y = ReadFloat(); t.Z = ReadFloat(); return t; };
	Quaternion		ReadQuaternion( void )	{ Quaternion q; q.W = ReadFloat(); q.X = ReadFloat(); q.Y = ReadFloat(); q.Z = ReadFloat(); return q; };
	Quaternion		ReadQuaternionOptimized( void );
	Quaternion		ReadQuaternionOptimized2( void );
	float			ReadFloatSignedOptimized( float unit );
	float			ReadFloatUnsignedOptimized( float unit );
	Entity*			ReadEntityPtr( void );
	ulong32	ReadULongOptimized( void );

	void			WriteCheckToken( unsigned char token );
	bool			ReadCheckToken( unsigned char token );

	unsigned short	ReadUpTo32767Optimized( void );

	// Up to 29 bytes, higher are ignored
	ulong32	ReadStateOptimized( void )
	{
		ulong32 state;
		unsigned char t;

		// First byte
		t = ReadByte();
		state = t & 0x7F;

		// Second byte if neeeded
		if( t & 0x80 )
		{
			t = ReadByte();
			state |= ((t&0x7F)<<7);

			// Third byte if needed
			if( t & 0x80 )
			{
				t = ReadByte();
				state |= ((t&0x7F)<<14);

				// Fourth byte if needed
				if( t & 0x80 )
				{
					t = ReadByte();
					state |= (t<<21);
				}
			}
		}
		return state;
	}

	static bool CanUsePositionDiff( const Vector& pos );

	Vector ReadPositionOptimized( void );
	Vector ReadPositionDiffOptimized( void );
	Vector ReadLinearVelocityOptimized( void );
	Vector ReadAngularVelocityOptimized( void );
	Vector ReadNormalOptimized( void );

	unsigned char	PeekByte( void )		{ return *(unsigned char*)mPos; };
	char			PeekChar( void )		{ return *(char*)mPos; };
	unsigned char	PeekUChar( void )		{ return *(unsigned char*)mPos; };
	short			PeekShort( void )		{ return *(short*)mPos; };
	unsigned short	PeekUShort( void )		{ return *(unsigned short*)mPos; };
	long32			PeekLong( void )		{ return *(long32*)mPos; };
	ulong32	PeekULong( void )		{ return *(ulong32*)mPos; };
	float			PeekFloat( void )		{ return *(float*)mPos; };
	double			PeekDouble( void )		{ return *(double*)mPos; };

	void ScrapData( int size ) { mPos += size; };
	void ReadData( char* buf, int size ) { memcpy( buf, mPos, size ); mPos += size; };
	bool ReadFinished( void ) { return ( mPos - mMessage >= mSize ); };

	bool ReadFromFile( String filename );

	// Message writing inlines - fast - no htons, htonl at all
	void WriteBool( bool w )				{ *(unsigned char*)mPos = w?1:0; mPos += sizeof(unsigned char); };
	void WriteByte( unsigned char w )		{ *(unsigned char*)mPos = w; mPos += sizeof(unsigned char); };
	void WriteChar( char w )				{ *(char*)mPos = w; mPos += sizeof(char); };
	void WriteUChar( unsigned char w )		{ *(unsigned char*)mPos = w; mPos += sizeof(unsigned char); };
	void WriteShort( short w )				{ *(short*)mPos = w; mPos += sizeof(short); };
	void WriteUShort( unsigned short w )	{ *(unsigned short*)mPos = w; mPos += sizeof(unsigned short); };
	void WriteLong( long32 w )				{ *(long32*)mPos = w; mPos += sizeof(long32); };
	void WriteULong( ulong32 w )		{ *(ulong32*)mPos = w; mPos += sizeof(ulong32); };
	void WriteFloat( float w )				{ *(float*)mPos = w; mPos += sizeof(float); };
	void WriteDouble( double w )			{ *(double*)mPos = w; mPos += sizeof(double); };
	void WriteString( const char* w )		{ if(w) { while( *w ) { *(char*)mPos = *w; mPos++; w++; }; }; *(char*)mPos = 0; mPos++; };
	void WriteStringNL( const char* w )		{ if(w) { while( *w ) { *(char*)mPos = *w; mPos++; w++; }; }; *(char*)mPos = 10; mPos++; };
	void WriteVector( Vector& w )			{ WriteFloat( w.X ); WriteFloat( w.Y ); WriteFloat( w.Z ); };
	void WriteQuaternion( Quaternion& w );
	void WriteQuaternionOptimized( Quaternion w );
	void WriteQuaternionOptimized2( Quaternion& w );
	void WriteFloatSignedOptimized( float value, float unit );
	void WriteFloatUnsignedOptimized( float value, float unit );
	void WriteEntityPtr( Entity* entity );
	char WriteULongOptimized( ulong32 w );

	void WriteData( const char* buf, int size ) { memcpy( mPos, buf, size ); mPos += size; };
	void WriteData( UdpMessage* mesg ) { memcpy( mPos, mesg->mMessage, mesg->mSize ); mPos += mesg->mSize; };
	void WriteData( UdpMessage* mesg, int start, int length ) { memcpy( mPos, mesg->mMessage+start, length ); mPos += length; };
	void WriteData( UdpMessage* mesg, int length ) { memcpy( mPos, mesg->mPos, length ); mPos += length; };
	void FillWith( unsigned char character, unsigned short size ) { memset( mPos, character, size ); mPos+= size; };

	void EndWriting( bool noChecks = false );

	void WriteUpTo32767Optimized( unsigned short sh );

	// Up to 29 bytes, higher are ignored
	void WriteStateOptimized( ulong32 state )
	{
		if( state & 0xFFE00000 )
		{
			WriteByte( ((unsigned char)state & 0x7F)     | 0x80 );
			WriteByte( ((unsigned char)(state>>7)&0x7F)  | 0x80 );
			WriteByte( ((unsigned char)(state>>14)&0x7F) | 0x80 );
			WriteByte( ((unsigned char)(state>>21)&0xFF) );
		}
		else if( state & 0xFFC000 )
		{
			WriteByte( ((unsigned char)state & 0x7F) | 0x80 );
			WriteByte( ((unsigned char)(state>>7)&0x7F) | 0x80 );
			WriteByte( ((unsigned char)(state>>14)&0x7F) );
		}
		else if( state & 0xFF80 )
		{
			WriteByte( ((unsigned char)state & 0x7F) | 0x80 );
			WriteByte( ((unsigned char)(state>>7)&0x7F) );
		}
		else
			WriteByte( (unsigned char)state & 0x7F );
	}

	void WritePositionOptimized( Vector& pos );
	void WritePositionDiffOptimized( Vector& pos );
	void WriteLinearVelocityOptimized( Vector& linearVelocity );
	void WriteAngularVelocityOptimized( Vector& angularVelocity );
	void WriteNormalOptimized( Vector& normal );

	bool WriteToFile( String filename );

	UdpMessage& operator+=( const UdpMessage& mesg ) { WriteData( mesg.GetBuffer(), mesg.GetSize() ); return *this; };

	void MakeCRCStamp( void );
	bool CheckRemoveCRCStamp( void );

	void DumpToLogfile( String reason );

	void Test( void );

	// Reading/Writing bits
	void ResetBitPos( void ) { if(mBitsPos) ++mPos; mBitsPos = 0; };	// do this to finish reading/writing bits
	unsigned char ReadBits( unsigned char number );
	void WriteBits( unsigned char value, unsigned char number );

	char* GetBuffer( void ) const { return mMessage; };

private:
	ulong32 WriteFloatOptimized( float value, float base, unsigned char baseBits, unsigned char powerBits );
	float ReadFloatOptimized( ulong32 value, float base, unsigned char baseBits, unsigned char powerBits );

	char* mMessage;
	char* mPos;
	int mSize;
	bool mDelete;
	bool mWasSent;
	unsigned char mBitsPos;

public:
	DynamicArray<int> DeliveryReceipts;
	// This are used only rarely, when there's a need to call scripts with info that message has been delivered.
	// It doesn't take up much time, so don't care about this. It is not the integral part of a class, just an add-on
	// attached to it. It has no connection with class functionality, it's like a "to be acknowledged" stamp on an envelope.
};
