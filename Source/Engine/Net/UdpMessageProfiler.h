
class ENGINE_API UdpMessageProfiler
{
public:

	UdpMessageProfiler( void ) {}
	~UdpMessageProfiler( void ) {}

	void AddBytesRead( int category, int bytesRead )
	{
		Assert( ( category >= 0 ) && ( category < Last ) );
		while( category >= CategoriesData.Size() )
			CategoriesData.Add(0);
		CategoriesData[category] += bytesRead;
	}

	void PrintFullStats( void );
	void Reset( void ) { CategoriesData.ClearArray(); }

private:
	DynamicArray<ulong32>		CategoriesData;
};

class ENGINE_API UdpMessage
{
	friend class NetworkDevice2;
	friend class NetworkPeer2;
	friend class PKTVProxyConnection;
public:
	UdpMessage( int maxSize = MAX_UDP_PACKET_SIZE ) : mSize(0), mDelete(true), mWasSent(false), mBitsPos(0), mCategory(0) {
        m_setTag("Net");
        mPos = mMessage = new char[maxSize]; 
    }
	UdpMessage( char* mesg, int size, bool del = false ) : mMessage(mesg), mPos(mesg), mSize(size), mDelete(del), mWasSent(false), mBitsPos(0), mCategory(0) {}
	~UdpMessage( void ) { if( mDelete ) delete[] mMessage; }

	int  GetProfilingCategory( void ) { return mCategory; }
	void SetProfilingCategory( int category ) { mCategory = category; }

	void Trim( void );	// Trim buffer size to message size

	void SeekToBegin( void ) { mPos = mMessage; }
	void Seek( int pos ) { mPos = mMessage + pos; }
	int GetPos( void ) const { return mPos - mMessage; }
	int GetSize( void ) const { return mSize; }			// when writing valid only after EndWriting() call.

	// Message reading inlines - fast - no htons,htonl at all
	bool			ReadBool( void );
	unsigned char	ReadByte( void );
	char			ReadChar( void );
	unsigned char	ReadUChar( void );
	short			ReadShort( void );
	unsigned short	ReadUShort( void );
	long32			ReadLong( void );
	ulong32	ReadULong( void );
	float			ReadFloat( void );
	double			ReadDouble( void );
	String			ReadString( void );
	String			ReadStringNL( void );

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

	bool ReadFromFile( const char *filename );

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

	bool WriteToFile( const char *filename );

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
	int mCategory;

public:
	DynamicArray<int> DeliveryReceipts;
};
