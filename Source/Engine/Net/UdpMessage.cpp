#include "Engine.h"

#include "Core/FileSystem/FileManagerStandard.h"
extern ENGINE_API FileManagerStd GFileMan;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Net/UdpMessage.h"
#include "Engine/Mesh.h"

#define HAVOK_COLLISION_TOLERANCE 0.01f
#define RESOLUTION_OF_POS_DIFF 0.00025f
#include "Net/NetworkDevice2.h"

#include "Engine/World.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#ifndef FINAL_BUILD
LogBuffer UdpLog( UDP_FAULT_LOG_FILE_NAME );
#endif

#ifdef UDP_END_WRITING_CRASH_LOG
int EndWritingCrashNumber = 0;
#endif

//#define CHECK_TOKENS_ENABLED

#ifndef UDP_MESSAGE_PROFILER

void UdpMessage::WritePositionOptimized( Vector& pos )
{
	const char xBits = 22;
	const char yBits = 20;
	const char zBits = 22;

	const ulong32 maxX = (1<<(xBits-1));
	const ulong32 maxY = (1<<(yBits-1));
	const ulong32 maxZ = (1<<(zBits-1));

	ulong32 dword1 = 0, dword2 = 0;
	bool sign;
	ulong32 integ;

	// Pack X
	sign = ( pos.X < 0 );
	integ = (ulong32)((fabs(pos.X))/(HAVOK_COLLISION_TOLERANCE/2.0f));
//	Assert( integ < maxX );
	if( integ >= maxX )
		integ = maxX-1;
	integ &= ((1<<(xBits-1))-1);
	dword1 |= integ;
	if( sign ) dword1 |= (1<<(xBits-1));

	// Pack Y
	sign = ( pos.Y < 0 );
	integ = (ulong32)((fabs(pos.Y))/(HAVOK_COLLISION_TOLERANCE/2.0f));
//	Assert( integ < maxY );
	if( integ >= maxZ )
		integ = maxY-1;
	integ &= ((1<<(yBits-1))-1);
	dword1 |= ( integ << xBits );
	dword2 |= ( integ >> (32 - xBits ) );
	if( sign ) dword2 |= ( 1 << (xBits + yBits - 1 - 32 ) );

	// Pack Z
	sign = ( pos.Z < 0 );
	integ = (ulong32)((fabs(pos.Z))/(HAVOK_COLLISION_TOLERANCE/2.0f));
//	Assert( integ < maxZ );
	if( integ >= maxZ )
		integ = maxZ-1;
	integ &= ((1<<(zBits-1))-1);
	dword2 |= ( integ << ( xBits + yBits - 32 ) );
	if( sign ) dword2 |= ( 1 << (xBits + yBits + zBits - 1 - 32 ) );

	WriteULong( dword1 );
	WriteULong( dword2 );
}

Vector UdpMessage::ReadPositionOptimized( void )
{
	const char xBits = 22;
	const char yBits = 20;
	const char zBits = 22;

	bool sign;
	ulong32 integ;

	Vector output;

	ulong32 dword1 = ReadULong();
	ulong32 dword2 = ReadULong();

	// Unpack X
	integ = dword1;
	integ &= ((1<<(xBits-1))-1);
	sign = ((dword1 & (1<<(xBits-1))) != 0 );
#ifdef TRANSGAMING
	output.X = (float)(((float)integ)*(HAVOK_COLLISION_TOLERANCE/2.0f));
#else
	output.X = (float)(((double)integ)*(HAVOK_COLLISION_TOLERANCE/2.0));
#endif
	if( sign ) output.X *= -1.0;

	// Unpack Y
	integ = dword1 >> xBits;
	integ |= dword2 << ( 32 - xBits );
	integ &= ((1<<(yBits-1))-1);
	sign = ((dword2 & (1<<(xBits + yBits - 1 - 32 ))) != 0 );
#ifdef TRANSGAMING
	output.Y = (float)(((float)integ)*(HAVOK_COLLISION_TOLERANCE/2.0f));
#else
	output.Y = (float)(((double)integ)*(HAVOK_COLLISION_TOLERANCE/2.0));
#endif
	if( sign ) output.Y *= -1.0;

	// Unpack Z
	integ = dword2 >> ( xBits + yBits - 32 );
	integ &= ((1<<(zBits-1))-1);
	sign = ((dword2 & (1<<(xBits + yBits + zBits - 1 - 32 ))) != 0 );
#ifdef TRANSGAMING
	output.Z = (float)(((float)integ)*(HAVOK_COLLISION_TOLERANCE/2.0f));
#else
	output.Z = (float)(((double)integ)*(HAVOK_COLLISION_TOLERANCE/2.0));
#endif
	if( sign ) output.Z *= -1.0;

	return output;
}

void UdpMessage::WritePositionDiffOptimized( Vector& pos )
{
	const char xBits = 11;
	const char yBits = 10;
	const char zBits = 11;

	const ulong32 maxX = (1<<(xBits-1));
	const ulong32 maxY = (1<<(yBits-1));
	const ulong32 maxZ = (1<<(zBits-1));

	ulong32 dword = 0;
	bool sign;
	ulong32 integ;

	// Pack X
	sign = ( pos.X < 0 );
	integ = (ulong32)((fabs(pos.X))/RESOLUTION_OF_POS_DIFF);
	Assert( integ < maxX );
	if( integ >= maxX )
		integ = maxX-1;
	integ &= ((1<<(xBits-1))-1);
	dword |= integ;
	if( sign ) dword |= (1<<(xBits-1));

	// Pack Y
	sign = ( pos.Y < 0 );
	integ = (ulong32)((fabs(pos.Y))/RESOLUTION_OF_POS_DIFF);
	Assert( integ < maxY );
	if( integ >= maxZ )
		integ = maxY-1;
	integ &= ((1<<(yBits-1))-1);
	dword |= ( integ << xBits );
	if( sign ) dword |= ( 1 << (xBits + yBits - 1 ) );

	// Pack Z
	sign = ( pos.Z < 0 );
	integ = (ulong32)((fabs(pos.Z))/RESOLUTION_OF_POS_DIFF);
	Assert( integ < maxZ );
	if( integ >= maxZ )
		integ = maxZ-1;
	integ &= ((1<<(zBits-1))-1);
	dword |= ( integ << ( xBits + yBits ) );
	if( sign ) dword |= ( 1 << (xBits + yBits + zBits - 1 ) );

	WriteULong( dword );
}

Vector UdpMessage::ReadPositionDiffOptimized( void )
{
	const char xBits = 11;
	const char yBits = 10;
	const char zBits = 11;

	bool sign;
	ulong32 integ;

	Vector output;

	ulong32 dword = ReadULong();

	// Unpack X
	integ = dword;
	integ &= ((1<<(xBits-1))-1);
	sign = ((dword & (1<<(xBits-1))) != 0 );
	output.X = (float)(((double)integ)*RESOLUTION_OF_POS_DIFF);
	if( sign ) output.X *= -1.0;

	// Unpack Y
	integ = dword >> xBits;
	integ &= ((1<<(yBits-1))-1);
	sign = ((dword & (1<<(xBits + yBits - 1 ))) != 0 );
	output.Y = (float)(((double)integ)*RESOLUTION_OF_POS_DIFF);
	if( sign ) output.Y *= -1.0;

	// Unpack Z
	integ = dword >> ( xBits + yBits );
	integ &= ((1<<(zBits-1))-1);
	sign = ((dword & (1<<(xBits + yBits + zBits - 1 ))) != 0 );
	output.Z = (float)(((double)integ)*RESOLUTION_OF_POS_DIFF);
	if( sign ) output.Z *= -1.0;

	return output;
}

bool UdpMessage::CanUsePositionDiff( const Vector& pos )
{
	return ( ( ceil(fabs(pos.X)/RESOLUTION_OF_POS_DIFF) < (1<<10) ) &&
			( ceil(fabs(pos.Y)/RESOLUTION_OF_POS_DIFF) < (1<<9) ) &&
			( ceil(fabs(pos.Z)/RESOLUTION_OF_POS_DIFF) < (1<<10) ) );
}

void UdpMessage::Test( void )
{
/*
	for( int i = 0; i < 10000; i++ )
	{
		Quaternion v1;
		v1.X = FloatRand( -1.0,1.0 );
		v1.Y = FloatRand( -1.0,1.0 );
		v1.Z = FloatRand( -1.0,1.0 );
		v1.W = FloatRand( -1.0,1.0 );
		v1 = v1*(1.0/sqrt(v1.Norm()));

		Assert( fabs(v1.Norm() - 1.0) < 0.01 );

		SeekToBegin();
		WriteQuaternionOptimized( v1 );
		EndWriting();
		SeekToBegin();
		Quaternion v2 = ReadQuaternionOptimized();

		Assert( fabs(v2.W - v1.W) < 0.01 );
		Assert( fabs(v2.X - v1.X) < 0.01 );
		Assert( fabs(v2.Y - v1.Y) < 0.01 );
		Assert( fabs(v2.Z - v1.Z) < 0.01 );
	}
*/
/*
	for( int i = 0; i < 1000000; i++ )
	{
		unsigned short u1 = rand() & 0x7FFF;
		SeekToBegin();
		WriteUpTo32767Optimized( u1 );
		EndWriting();
		SeekToBegin();
		unsigned short u2 = ReadUpTo32767Optimized();
		Assert( u2 == u1 );
	}
*/
/*
	for( int i = 0; i < 1000000; i++ )
	{
		ulong32 u1 = rand();
		SeekToBegin();
		WriteULongOptimized( u1 );
		EndWriting();
		SeekToBegin();
		ulong32 u2 = ReadULongOptimized();
		Assert( u2 == u1 );
	}
*/
/*
	LogBuffer TestFloatOptimizationLog( "testFloat.log" );

	for( int i = 0; i < 1000; i++ )
	{
		float f1 = (((float)rand())*100)/RAND_MAX;
		ulong32 written = WriteFloatOptimized( f1, 0.0001f, 5, 4 );
		float f2 = ReadFloatOptimized( written, 0.0001f, 5, 4 );
		TestFloatOptimizationLog.Print( "Source: %20f, Dest: %20f\n", f1, f2 );
	}
*/

/*
	LogBuffer TestLinearVelocityOptimizationLog( "testFloat.log" );

	float maxDiff = 0.0f;
	Vector maxv1(0,0,0), maxv2(0,0,0);

	float maxDiffRelative = 0.0f;
	Vector maxvr1(0,0,0), maxvr2(0,0,0);

	ulong32 minus = 0;

	for( int i = 0; i < 10000000; i++ )
	{
		float f1 = (((float)rand())/RAND_MAX)*200 - 100;
		float f2 = (((float)rand())/RAND_MAX)*200 - 100;
		float f3 = (((float)rand())/RAND_MAX)*200 - 100;
		if( f1 < 0 ) minus++;
		if( f2 < 0 ) minus++;
		if( f3 < 0 ) minus++;
		Vector linVel1( f1, f2, f3 );
		SeekToBegin();
		WriteLinearVelocityOptimized( linVel1 );
		EndWriting();
		SeekToBegin();
		Vector linVel2 = ReadLinearVelocityOptimized();
		Vector diff = linVel1 - linVel2;

		if( diff.Magnitude() > maxDiff )
		{
			maxDiff = diff.Magnitude();
			maxv1 = linVel1;
			maxv2 = linVel2;
		}

		if( diff.Magnitude()/linVel1.Magnitude() > maxDiffRelative )
		{
			maxDiffRelative = diff.Magnitude()/linVel1.Magnitude();
			maxvr1 = linVel1;
			maxvr2 = linVel2;
		}
//			TestLinearVelocityOptimizationLog.Print( "Source: (%15f,%15f,%15f) Dest: (%15f,%15f,%15f) Diff: %f\n",
//				linVel1.X, linVel1.Y, linVel1.Z, linVel2.X, linVel2.Y, linVel2.Z, diff.Magnitude() );
	}
	TestLinearVelocityOptimizationLog.Print( "Source: (%15f,%15f,%15f) Dest: (%15f,%15f,%15f) MaxDiff: %f\n",
		maxv1.X, maxv1.Y, maxv1.Z, maxv2.X, maxv2.Y, maxv2.Z, maxDiff );
	TestLinearVelocityOptimizationLog.Print( "Source: (%15f,%15f,%15f) Dest: (%15f,%15f,%15f) MaxDiffRelative: %f\n",
		maxvr1.X, maxvr1.Y, maxvr1.Z, maxvr2.X, maxvr2.Y, maxvr2.Z, maxDiffRelative );
	TestLinearVelocityOptimizationLog.Print( "Minus: %d\n", minus );

//	MaxDiffRelative: 0.030225 
*/
/*
	for( int i = 0; i < 1000000; i++ )
	{
		float f1 = (((float)rand())/RAND_MAX)*2 - 1;
		float f2 = (((float)rand())/RAND_MAX)*2 - 1;
		float f3 = (((float)rand())/RAND_MAX)*2 - 1;
		Vector normal( f1, f2, f3 );
		normal.Normalize();
		SeekToBegin();
		WriteNormalOptimized( normal );
		EndWriting();
		SeekToBegin();
		Vector normal2 = ReadNormalOptimized();
		float diff = (normal2-normal).Magnitude();
		Assert( diff < 0.05f );
	}
*/
/*
	ulong32 uniqueNumber;
	unsigned char type;
	ulong32 read;
	ulong32 uniqueNumber2;
	unsigned char type2;

	for( int i = 0; i < 1000000; ++i )
	{
		do { uniqueNumber = (((ulong32)rand())<<16)+rand(); } while( ( uniqueNumber != -1 ) && ( uniqueNumber > 0x3fffffff ) );

		type = rand()%3;
		SeekToBegin();

		WriteULongOptimized( (((ulong32)type)&0x3)|((uniqueNumber + 1)<<2) );

		EndWriting();
		SeekToBegin();

		read = ReadULongOptimized();
		uniqueNumber2 = (read>>2)-1;
		type2 = unsigned char(read&0x3);

		Assert( uniqueNumber == uniqueNumber2 );
		Assert( type == type2 );
	}

	uniqueNumber = (ulong32)-1;
	type = rand()%3;
	SeekToBegin();

	WriteULongOptimized( (((ulong32)type)&0x3)|((uniqueNumber + 1)<<2) );

	EndWriting();
	SeekToBegin();

	read = ReadULongOptimized();
	uniqueNumber2 = (read>>2)-1;
	type2 = unsigned char(read&0x3);

	Assert( uniqueNumber == uniqueNumber2 );
	Assert( type == type2 );
*/
}

void UdpMessage::WriteQuaternion( Quaternion& w )
{
	Assert( fabs(w.Norm() - 1.0) < 0.01 );
	WriteFloat( w.W );
	WriteFloat( w.X );
	WriteFloat( w.Y );
	WriteFloat( w.Z );
}

void UdpMessage::WriteQuaternionOptimized( Quaternion w )
{
	if( fabs(w.Norm() - 1.0) >= 0.01 )
		w.Normalize();

	// Convert so W is higher than 0
	if ( w.W < 0 )
	{
		w.W = -w.W;
		w.X = -w.X;
		w.Y = -w.Y;
		w.Z = -w.Z;
	}

	unsigned short y = (unsigned short)(fabs(w.Y)*16383.0);
	if( w.Y < 0 )
		y |= 0x4000;
	y |= 0x8000;

	unsigned char c1 = (unsigned char)((y & 0xff00 ) >> 8);
	unsigned char c2 = (unsigned char)(y & 0x00ff);
	WriteUChar( c1 );
	WriteUChar( c2 );
	WriteShort( (short)(w.X*32767.0) );
	WriteShort( (short)(w.Z*32767.0) );
}

Quaternion UdpMessage::ReadQuaternionOptimized( void )
{
	Quaternion w;
	unsigned char c1 = ReadUChar();
	unsigned char c2 = ReadUChar();
	unsigned short y = ( c1 << 8 ) + c2;
	bool minus = ((y & 0x4000) != 0);
	y &= 0x3fff;
	short y2;
	if( minus ) 
		y2 = -y;
	else
		y2 = y;
	w.Y = (float)(((double)y2)/16383.0);
	w.X = (float)(((double)ReadShort())/32767.0);
	w.Z = (float)(((double)ReadShort())/32767.0);
	w.W = (float)sqrt(fabs(1.0 - w.X*w.X - w.Y*w.Y - w.Z*w.Z));
	return w;
}

float UdpMessage::ReadFloatSignedOptimized( float unit )
{
	return ReadShort()*unit;
}

void UdpMessage::WriteFloatSignedOptimized( float value, float unit )
{
	Assert( fabs(value) < unit*32768 );
	WriteShort( (short)floor( value/unit ) );
}

float UdpMessage::ReadFloatUnsignedOptimized( float unit )
{
	return ReadUShort()*unit;
}

void UdpMessage::WriteFloatUnsignedOptimized( float value, float unit )
{
	Assert( ( value >= 0 ) && ( value < unit*65536.0 ) );
	WriteUShort( (unsigned short)floor( value/unit ) );
}

unsigned short UdpMessage::ReadUpTo32767Optimized( void )
{
	unsigned char c = ReadUChar();
	unsigned short out = 0;

	if( c > 127 )
	{
		out |= (c&0x7F);
		unsigned short c2 = (unsigned short)ReadUChar();
		out |= (c2<<7);
	}
	else
		out = c;

	return out;
}

void UdpMessage::WriteUpTo32767Optimized( unsigned short sh )
{
	unsigned char c = (unsigned char)(sh & 0x7F);
	if( sh > 127 )
	{
		c |= 0x80;
		unsigned char c2 = (unsigned char)(( sh & 0x7F80 ) >> 7);
		WriteUChar( c );
		WriteUChar( c2 );
	}
	else
		WriteUChar( c );
}

bool UdpMessage::WriteToFile( const char *filename )
{
	if( filename[0] == 0 )
		return false;

	// Open file
	FileObject* f = GFileMan.CreateFileWriter( filename );
	if( !f )
		return false;

	EndWriting();	// just in case

	// Write message
	if( !f->Write( mMessage, mSize ) )
	{
		delete f;
		return false;
	};
	return true;
}

bool UdpMessage::ReadFromFile( const char *filename )
{
	if( filename[0] == 0 )
		return false;

	// Open file
	FileObject* f = GFileMan.CreateFileReader(filename);
	if( !f )
		return false;

	// Get file size
	int filesize = f->GetSize();

	delete[] mMessage;
    m_setTag("Net");
	mMessage = new char[ filesize ];
	mPos = mMessage+filesize;
	mSize = filesize;
	mDelete = true;

	// Read message
	if( !f->Read( mMessage, filesize ) )
	{
		delete f;
		return false;
	}

	delete f;
	return true;
}

void UdpMessage::WriteAngularVelocityOptimized( Vector& angularVelocity )
{
	if( ( fabs( angularVelocity.X ) < 0.0001f ) &&
		( fabs( angularVelocity.Z ) < 0.0001f ) )
	{
		if( fabs( angularVelocity.Y ) < 0.0001f )
		{
			WriteUChar(0);
		}
		else
		{
			WriteUChar(1);
			WriteFloat( angularVelocity.Y );
		}
	}
	else
	{
		WriteUChar(2);
		WriteFloat( angularVelocity.X );
		WriteFloat( angularVelocity.Y );
		WriteFloat( angularVelocity.Z );
	}
}

Vector UdpMessage::ReadAngularVelocityOptimized( void )
{
	unsigned char type = ReadUChar();
	switch( type )
	{
	case 0: return Vector( 0,0,0 );
	case 1: return Vector( 0, ReadFloat(), 0 );
	case 2:
		{
			Vector w;
			w.X = ReadFloat();
			w.Y = ReadFloat();
			w.Z = ReadFloat();
			return w;
		}
	default:
		Assert( type == 0 );	// this will fire
		return Vector(0,0,0);
	}
}

void UdpMessage::WriteLinearVelocityOptimized( Vector& linearVelocity )
{
	ulong32 x = WriteFloatOptimized( linearVelocity.X, 0.0001f, 6, 4 );
	ulong32 y = WriteFloatOptimized( linearVelocity.Y, 0.0001f, 5, 4 );
	ulong32 z = WriteFloatOptimized( linearVelocity.Z, 0.0001f, 6, 4 );
	ulong32 result = x & 0x7FF;
	result |= (( y & 0x3FF )<<11);
	result |= (( z & 0x7FF )<<21);
	WriteULong( result );
}

Vector UdpMessage::ReadLinearVelocityOptimized( void )
{
	ulong32 value = ReadULong();
	float x = ReadFloatOptimized( (value & 0x7FF), 0.0001f, 6, 4 );
	float y = ReadFloatOptimized( ((value>>11) & 0x3FF), 0.0001f, 5, 4 );
	float z = ReadFloatOptimized( ((value>>21) & 0x7FF), 0.0001f, 6, 4 );
	return Vector(x,y,z);
}

Quaternion UdpMessage::ReadQuaternionOptimized2( void )
{
	Quaternion q;
	unsigned char c = PeekUChar();
	if( c & 0x80 )	// normally optimized quaternion
		q = ReadQuaternionOptimized();
	else			// ERotYAxis - full optimization
	{
		c = ReadUChar();
		float angle = (float)((((double)c)/127.0*2.0*F_PI));
		Vector up(0,1,0);
		q = Quaternion( angle, up );
	}
	return q;
}

void UdpMessage::WriteQuaternionOptimized2( Quaternion& w )
{
	if( (fabs(w.X) > 0.001f) || (fabs(w.Z) > 0.001f) )
		WriteQuaternionOptimized( w );
	else
	{
		float angle;
		Vector axis;
		w.ToAngleAxis( angle, axis );

		// Make sure Y axis will be > 0
		if( axis.Y < 0 )	angle *= -1.0f;

		// Clamp to <0,2PI>
		if( angle > F_PI )	angle -= (float)(2.0*F_PI*floor(angle/2.0/F_PI));
		if( angle < -F_PI )	angle += (float)(2.0*F_PI*ceil(angle/-2.0/F_PI));
		if( angle < 0 )		angle += 2.0*F_PI;

		WriteUChar( (unsigned char)floor(angle*127.0/(2.0*F_PI)) );
	}
}

Entity* UdpMessage::ReadEntityPtr( void )
{
	ulong32 number = ReadULong();
	bool isStaticMesh = ((number & 0x80000000) != 0 );
	if( isStaticMesh )
	{
		unsigned short meshNumber = (unsigned short)(number & 0x7FFFFFFF);
		if( meshNumber == 65535 ) return 0;
#ifdef	TRANSGAMING
		for( int i = 0; i < GEngine->pcfWorld->Glasses.Size(); ++i )
			if( GEngine->pcfWorld->Glasses[i]->GetMainMesh()->FixedMeshIndex == meshNumber )
				return GEngine->pcfWorld->Glasses[i]->GetMainMesh();
		for( int i = 0; i < GEngine->pcfWorld->Meshes.Size(); ++i )
			if( GEngine->pcfWorld->Meshes[i]->FixedMeshIndex == meshNumber )
				return GEngine->pcfWorld->Meshes[i];
#else
		for( int i = 0; i < GEngine->World->Glasses.Size(); ++i )
			if( GEngine->World->Glasses[i]->GetMainMesh()->FixedMeshIndex == meshNumber )
				return GEngine->World->Glasses[i]->GetMainMesh();
		for( int i = 0; i < GEngine->World->Meshes.Size(); ++i )
			if( GEngine->World->Meshes[i]->FixedMeshIndex == meshNumber )
				return GEngine->World->Meshes[i];
#endif	
		return 0;
	}
	else
		return GEngine->Net->GetEntityFromNumber( number );
}

void UdpMessage::WriteEntityPtr( Entity* entity )
{
	ulong32 number = GEngine->Net->GetEntityNumber( entity );
    if( (number == -1) && entity && entity->AsMesh() )
	{
		number = entity->AsMesh()->FixedMeshIndex;
		number |= 0x80000000;
	}
	WriteULong( number );
}

char UdpMessage::WriteULongOptimized( ulong32 w )
{
	if( w & 0xF0000000 )
	{
		WriteByte( ((unsigned char)w & 0x7F)     | 0x80 );
		WriteByte( ((unsigned char)(w>>7)&0x7F)  | 0x80 );
		WriteByte( ((unsigned char)(w>>14)&0x7F) | 0x80 );
		WriteByte( ((unsigned char)(w>>21)&0x7F) | 0x80 );
		WriteByte( ((unsigned char)(w>>28)&0x0F) );
		return 5;
	}
	else if( w & 0xFFE00000 )
	{
		WriteByte( ((unsigned char)w & 0x7F)     | 0x80 );
		WriteByte( ((unsigned char)(w>>7)&0x7F)  | 0x80 );
		WriteByte( ((unsigned char)(w>>14)&0x7F) | 0x80 );
		WriteByte( ((unsigned char)(w>>21)&0x7F) );
		return 4;
	}
	else if( w & 0xFFC000 )
	{
		WriteByte( ((unsigned char)w & 0x7F) | 0x80 );
		WriteByte( ((unsigned char)(w>>7)&0x7F) | 0x80 );
		WriteByte( ((unsigned char)(w>>14)&0x7F) );
		return 3;
	}
	else if( w & 0xFF80 )
	{
		WriteByte( ((unsigned char)w & 0x7F) | 0x80 );
		WriteByte( ((unsigned char)(w>>7)&0x7F) );
		return 2;
	}

	WriteByte( (unsigned char)w & 0x7F );
	return 1;
}

ulong32 UdpMessage::ReadULongOptimized( void )
{
	ulong32 result;
	unsigned char t;

	// First byte
	t = ReadByte();
	result = t & 0x7F;

	// Second byte if neeeded
	if( t & 0x80 )
	{
		t = ReadByte();
		result |= ((t&0x7F)<<7);

		// Third byte if needed
		if( t & 0x80 )
		{
			t = ReadByte();
			result |= ((t&0x7F)<<14);

			// Fourth byte if needed
			if( t & 0x80 )
			{
				t = ReadByte();
				result |= ((t&0x7F)<<21);

				// Fifth byte if needed
				if( t & 0x80 )
				{
					t = ReadByte();
					result |= ((t&0x0F)<<28);
				}
			}
		}
	}
	return result;
}

ulong32 UdpMessage::WriteFloatOptimized( float value, float base, unsigned char baseBits, unsigned char powerBits )
{
	Assert( ( powerBits > 0 ) && ( powerBits <= 4 ) && ( baseBits > 0 ) && ( baseBits <= 23 ) );

	bool sign = (value<0);
	if( sign )
		value = fabs(value);

	ulong32 maxBase = (1<<baseBits);
	ulong32 powerMask = (1<<powerBits)-1;
	ulong32 multiplier = 1;
	ulong32 mantiss = 0;

	float value2 = value / base;
	ulong32 ulongVal = (ulong32)floor(value2);

	while( ulongVal >= maxBase*(multiplier-1) )
	{
		multiplier <<= 1;

		if( mantiss & (~powerMask) )	// Number out of range
		{
			ulong32 result = 0xFFFFFFFE & ((1<<(1+powerBits+baseBits))-1);
			if( sign ) result |= 0x1;
			return result;
		}

		mantiss++;
	}
	multiplier >>= 1;
	mantiss--;

	ulongVal -= maxBase*(multiplier-1);
	ulongVal /= multiplier;

	ulong32 fraction = ulongVal;
	Assert( !( fraction & ~((1<<baseBits)-1) ) );

	ulong32 result = 0;
	if( sign ) result |= 0x1;
	result |= (mantiss<<1);
	result |= (fraction<<(1+powerBits));

	return result;	// result will have baseBits+powerBits+1 of significant bits
}

float UdpMessage::ReadFloatOptimized( ulong32 value, float base, unsigned char baseBits, unsigned char powerBits )
{
	Assert( ( powerBits > 0 ) && ( powerBits <= 4 ) && ( baseBits > 0 ) && ( baseBits <= 23 ) );

	bool sign = ((value&0x01)!=0);
	ulong32 maxBase = (1<<baseBits);
	ulong32 powerMask = (1<<powerBits)-1;
	ulong32 fraction = (value>>(1+powerBits)) & ((1<<baseBits)-1);
	ulong32 mantiss = (value>>1) & powerMask;
	ulong32 multiplier = (1<<mantiss);

	ulong32 ulongVal = maxBase*(multiplier-1) + fraction*multiplier;
	float result = base*ulongVal;
	if( sign ) result *= -1.0f;
	return result;
}

void UdpMessage::MakeCRCStamp( void )
{
	DWORD u = Crc32( mMessage, mSize );
	u ^= 0xFD473501;
	WriteULong( u );
	mSize += sizeof(ulong32);
}

bool UdpMessage::CheckRemoveCRCStamp( void )
{
	Seek( mSize - sizeof(ulong32) );
	DWORD crc = ReadULong();
	crc ^= 0xFD473501;
	SeekToBegin();
	mSize -= sizeof(ulong32);
	DWORD crc2 = Crc32( mMessage, mSize );
	if( crc == crc2 )
		return true;
	else
	{
		GLog.Print( "CRC Error! MSG Size = %d\n", mSize );
		return false;
	}
}

void UdpMessage::WriteNormalOptimized( Vector& normal )
{
	Assert( fabs( normal.MagnitudeSquared() - 1.0f ) < 0.001f );
	// 24 bits = 11 bits for x + 10 bits for z + 3 bits for signs

	ulong32 result = 0;

	// write signs
	if( normal.X < 0 ) result |= 0x800000;
	if( normal.Y < 0 ) result |= 0x400000;
	if( normal.Z < 0 ) result |= 0x200000;

#ifdef TRANSGAMING
	ulong32 xFactor = (ulong32)(floor(fabs(normal.X) * 2048.0f));  //(1 << 11)));
	ulong32 zFactor = (ulong32)(floor(fabs(normal.Z) * 1024.0f));  //(1 << 10)));
#else
	ulong32 xFactor = (ulong32)floor(abs((double)normal.X)*(1<<11));
	ulong32 zFactor = (ulong32)floor(abs((double)normal.Z)*(1<<10));
#endif
	if( xFactor > (1<<11)-1 ) xFactor = (1<<11)-1;
	if( zFactor > (1<<10)-1 ) zFactor = (1<<10)-1;
	result |= xFactor;
	result |= (zFactor<<11);
	WriteUChar( (unsigned char)(result&0xFF) );
	WriteUChar( (unsigned char)((result&0xFF00)>>8) );
	WriteUChar( (unsigned char)((result&0xFF0000)>>16) );
}

Vector UdpMessage::ReadNormalOptimized( void )
{
	// 24 bits = 11 bits for x + 10 bits for z + 3 bits for signs
	ulong32 result = 0;
	ulong32 a;
	a = ReadUChar(); result |= a;
	a = ReadUChar(); result |= (a<<8);
	a = ReadUChar(); result |= (a<<16);

	ulong32 xFactor = result&0x7FF;
	ulong32 zFactor = (result>>11)&0x3FF;
	double x = ((double)xFactor)/(1<<11);
	double z = ((double)zFactor)/(1<<10);
	double y = sqrt(1.0-x*x-z*z);

	if( result & 0x800000 ) x = -x;
	if( result & 0x400000 ) y = -y;
	if( result & 0x200000 ) z = -z;

	Assert( fabs( x*x+y*y+z*z - 1.0f ) < 0.001f );

	return Vector(x,y,z);
}

void UdpMessage::DumpToLogfile( String reason )
{
#ifndef FINAL_BUILD
	UdpLog.Print( "Faulty message listing\n" );
	UdpLog.Print( "----------------------\n" );
	UdpLog.Print( "GameTime: %d", GetCurrentTime() );
	UdpLog.Print( "mSize = %d, pos = %d\n", mSize, mPos-mMessage );
	UdpLog.Print( "Reason: %s\n", reason.AsChar() );
	UdpLog.Print( "----------------------\n" );

	#define HOW_MANY_CHARS_IN_A_ROW 40

	int j = 0;
	while( j < mSize )
	{
		int min = (j+HOW_MANY_CHARS_IN_A_ROW>mSize)?mSize:(j+HOW_MANY_CHARS_IN_A_ROW);

		UdpLog.Print( "Hex: " );
		for( int i = j; i < min; ++i ) UdpLog.Print( "%.2x  ", *(((unsigned char*)mMessage)+i) );
		UdpLog.Print( "\n" );

		UdpLog.Print( "Dec: " );
		for( int i = j; i < min; ++i ) UdpLog.Print( "%.3d ", *(((unsigned char*)mMessage)+i) );
		UdpLog.Print( "\n" );

		UdpLog.Print( "Chr: " );
		for( int i = j; i < min; ++i ) UdpLog.Print( "%c   ", isprint((unsigned char)(mMessage[i]))?(unsigned char)(mMessage[i]):' ' );
		UdpLog.Print( "\n" );

		UdpLog.Print( "AsString: " );
		for( int i = j; i < min; ++i ) UdpLog.Print( "%c", isprint((unsigned char)(mMessage[i]))?(unsigned char)(mMessage[i]):'_' );
		UdpLog.Print( "\n" );
		UdpLog.Print( "\n" );

		j += min;
	}
	UdpLog.Print( "----------------------\n" );
	UdpLog.Flush();
#endif
}

void UdpMessage::EndWriting( bool noChecks )
{
	mSize = mPos - mMessage;
	if( noChecks )
		return;

	Assert( mSize <= MAX_UDP_PACKET_SIZE );
	if( mSize > MAX_UDP_PACKET_SIZE )
	{
#ifdef UDP_END_WRITING_CRASH_LOG
		String s = String::Sprintf( "Message bigger than MAX_UDP_PACKET_SIZE!!! Size = %d, nr = %d\n", mSize, EndWritingCrashNumber );
#else
		String s = String::Sprintf( "Message bigger than MAX_UDP_PACKET_SIZE!!! Size = %d\n", mSize );
#endif
		GLog.Print( s.AsChar() );
		GLog.Flush();
#ifdef TRANSGAMING
        TRACE("[]::UdpMessage::EndWriting(): ERROR-> message bigger than MAX_UDP_PACKET_SIZE! (size = %d)\n", mSize);
        Assert(0);
#else
		MessageBox( NULL, s.AsChar(), "Bug!!!", MB_OK );
#endif
		DumpToLogfile( "Message bigger than MAX_UDP_PACKET_SIZE!!!\n" );
	}
#ifdef UDP_END_WRITING_CRASH_LOG
	EndWritingCrashNumber = 0;
#endif
}

#ifdef CHECK_TOKENS_ENABLED
void UdpMessage::WriteCheckToken( unsigned char token )
{
	WriteUChar( token );
//	if( mPos - mMessage > MAX_UDP_PACKET_SIZE )
//		GLog.Print( "Write attempt after end of message!! Token: %d\n", token );
}

bool UdpMessage::ReadCheckToken( unsigned char token )
{
	unsigned char readToken = ReadUChar();
	if( readToken == token )
		return true;
	else
	{
		Assert( 0 );
		GLog.Print( "Udp Message token test failed!!! Expected %d, read %d\n", token, readToken );
		if( mPos - mMessage > mSize )
			GLog.Print( "Read attempt after end of message!!\n" );
		return false;
	}
}
#else
void UdpMessage::WriteCheckToken( unsigned char token ) {};
bool UdpMessage::ReadCheckToken( unsigned char token ) { return true; };
#endif

void UdpMessage::Trim( void )
{
	Assert( mSize );
	int index = GetPos();
    m_setTag("Net");
	char* newMesg = new char[mSize];
	::memcpy( newMesg, mMessage, mSize );
	delete[] mMessage;
	mMessage = newMesg;
	Seek( index );
}

unsigned char UdpMessage::ReadBits( unsigned char number )
{
	unsigned char result = 0;
	while( number > 0 )
	{
		result <<= 1;
		if( (*mPos) & (1<<mBitsPos) )
			++result;

		++mBitsPos;
		if( mBitsPos == 8 )
		{
			++mPos;
			mBitsPos=0;
		}

		--number;
	}
	return result;
}

void UdpMessage::WriteBits( unsigned char value, unsigned char number )
{
	while( number )
	{
		bool bit = ( value >> (number-1) ) & 1;

		if( bit )
			(*mPos) |= (1<<mBitsPos);
		else
			(*mPos) &= ~(1<<mBitsPos);

		++mBitsPos;
		if( mBitsPos == 8 )
		{
			++mPos;
			mBitsPos=0;
		}

		--number;
	}
}

#else

#include "UdpMessageProfiler.cpp"

#endif
