#include "Engine.h"
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "Core/RegistryManager.h"

//joe 
//no use
//#include <lmerr.h>

//#define REGISTRY_PATH "Software\\PeopleCanFly\\PainkillerDemo1"
#ifdef STAND_ALONE_SERVER
#define REGISTRY_PATH "Software\\PeopleCanFly\\PainkillerSS"
#else
#ifdef MPDEMO
#define REGISTRY_PATH "Software\\PeopleCanFly\\Painkiller Multiplayer demo"
#else
#define REGISTRY_PATH "Software\\PeopleCanFly\\Painkiller"
#endif
#endif

RegistryManager::RegistryManager()
{
	// Default variable values
	RegistryPath = REGISTRY_PATH;

	// General variables
	CDKey = "1234-1234-1234-1234";
	PlayerName = "DefaultPlayer";

	// Client variables
	LastHostName = "";
	LastPassword = "";
	LastPort = 0;

	// Server variables
	IsPublic = true;
	ServerMode = false;
	FragsLimit = 20;
	Map = "";

	// Bonus stuff
	BonusPlayerModel = false;
	BonusCardsUse = false;
	BonusPlayerModel2 = false;

	ReadFromRegistry();
}

RegistryManager::~RegistryManager()
{
	Update();
}

void RegistryManager::Update( void )
{
	HKEY dirKey;

	// Make sure registry directory exists
	if( RegCreateKey( HKEY_LOCAL_MACHINE, RegistryPath.AsChar(), &dirKey ) )
	{
		GLog.Print( "Registry update failed!\n" );
		return;
	}

	// General variables
#ifndef TRANSGAMING
# ifdef MPDEMO
	WriteString( dirKey, "EXEPath", ExePathname );
# else
	WriteString( dirKey, "EXEpath", ExePathname );
# endif
#endif
	WriteCDKey( dirKey, CDKey.AsChar() );
	WriteString( dirKey, "PlayerName", PlayerName.AsChar() );

	// Client variables
	WriteString( dirKey, "LastHostName", LastHostName.AsChar() );
	WriteString( dirKey, "LastPassword", LastPassword.AsChar() );
	WriteUInt( dirKey, "LastPort", LastPort );

	// Server variables
	WriteBool( dirKey, "IsPublic", IsPublic );
	WriteBool( dirKey, "ServerMode", ServerMode );
	WriteUInt( dirKey, "FragsLimit", FragsLimit );
	WriteString( dirKey, "LastMap", Map.AsChar() );

	// Bonus stuff
	WriteBool( dirKey, "SingleB", BonusCardsUse );
	WriteBool( dirKey, "MultiB", BonusPlayerModel );
	WriteBool( dirKey, "MultiB2", BonusPlayerModel2 );

	if( RegCloseKey( dirKey ) ) GLog.Print( "Error closing registry key!!\n" );
}

void RegistryManager::ReadFromRegistry( void )
{
	HKEY dirKey;

	// Make sure registry directory exists
	if( RegCreateKey( HKEY_LOCAL_MACHINE, RegistryPath.AsChar(), &dirKey ) )
	{
		GLog.Print( "Registry read failed!\n" );
		return;
	}

	// General variables
#ifndef TRANSGAMING
# ifdef MPDEMO
	ReadString( dirKey, "EXEPath", ExePathname );
# else
	ReadString( dirKey, "EXEpath", ExePathname );
# endif
#endif
	ReadCDKey( dirKey, CDKey );
	ReadString( dirKey, "PlayerName", PlayerName );

	// Client variables
	ReadString( dirKey, "LastHostName", LastHostName );
	ReadString( dirKey, "LastPassword", LastPassword );
	ReadUInt( dirKey, "LastPort", LastPort );

	// Server variables
	ReadBool( dirKey, "IsPublic", IsPublic );
	ReadBool( dirKey, "ServerMode", ServerMode );
	ReadUInt( dirKey, "FragsLimit", FragsLimit );
	ReadString( dirKey, "LastMap", Map );

	// Bonus stuff
	ReadBool( dirKey, "SingleB", BonusCardsUse );
	ReadBool( dirKey, "MultiB", BonusPlayerModel );
	ReadBool( dirKey, "MultiB2", BonusPlayerModel2 );

	if( RegCloseKey( dirKey ) ) GLog.Print( "Error closing registry key!!\n" );
}

void RegistryManager::OutputErrorString( void )
{
	HMODULE hModule = NULL;
	LPSTR MessageBuffer;
	DWORD error = GetLastError();
	DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM;
	String result;

//joe 
//no use
//	if( error >= NERR_BASE && error <= MAX_NERR)
	if( error >= 2100 && error <= 2100+899)
	{
		hModule = LoadLibraryEx( TEXT("netmsg.dll"), NULL, LOAD_LIBRARY_AS_DATAFILE );
		if( hModule != NULL ) dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
	}

	if( FormatMessageA( dwFormatFlags,
		(void*)hModule, // module to get message from (NULL == system)
		error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
		(LPSTR) &MessageBuffer,
		0, NULL ) )
	{
		result = MessageBuffer;
#if defined( TRANSGAMING ) && !defined( TRANSGAMING_WINDOWS )
		LocalFree( (unsigned int)MessageBuffer );
#else
		LocalFree( MessageBuffer );
#endif
	}
	else
		result = "Unknown";

	if( hModule != NULL ) FreeLibrary( hModule );

	GLog.Print( "%s\n", result.AsChar() );
}

bool RegistryManager::ReadString( HKEY dirKey, const char *variable, String& value )
{
	DWORD variableType;
	char receiveBuffer[1024];
	DWORD dataSize = 1024;
	LONG callResult;

	callResult = RegQueryValueEx( dirKey, variable, 0, &variableType, (LPBYTE)receiveBuffer, &dataSize );
	if( callResult )
		OutputErrorString();
	else if( variableType != REG_EXPAND_SZ )
		GLog.Print( "Registry read error: variable '%s' is not a string!\n", variable);
	else
	{
		value = receiveBuffer;
		return true;
	}
	return false;
}

bool RegistryManager::ReadUInt( HKEY dirKey, const char *variable, unsigned int& value )
{
	DWORD variableType;
	char receiveBuffer[10];
	DWORD dataSize = 10;
	LONG callResult;

	callResult = RegQueryValueEx( dirKey, variable, 0, &variableType, (LPBYTE)receiveBuffer, &dataSize );
	if( callResult )
		OutputErrorString();
	else if( variableType != REG_DWORD )
		GLog.Print( "Registry read error: variable '%s' is not a DWORD!\n", variable );
	else
	{
		value = (unsigned int)(*((DWORD*)receiveBuffer));
		return true;
	}
	return false;
}

bool RegistryManager::ReadBool( HKEY dirKey, const char *variable, bool& value )
{
	unsigned int val;
	bool ok = ReadUInt( dirKey, variable, val );
	if( ok ) value = ( val != 0 );
	return ok;
}

bool RegistryManager::WriteString( HKEY dirKey, const char *variable, const char *value )
{
	LONG callResult;
	callResult = RegSetValueEx( dirKey, variable, 0, REG_EXPAND_SZ, (LPBYTE)value, strlen(value) );
	if( callResult )
	{
		GLog.Print( "Registry write error: variable '%s' not set. Reason: ", variable );
		OutputErrorString();
		GLog.Print( "\n" );
	}
	return ( callResult == 0 );
}

bool RegistryManager::WriteUInt( HKEY dirKey, const char *variable, unsigned int value )
{
	LONG callResult;
	callResult = RegSetValueEx( dirKey, variable, 0, REG_DWORD, (LPBYTE)&value, sizeof(unsigned int) );
	if( callResult )
	{
		GLog.Print( "Registry write error: variable '%s' not set. Reason: ", variable);
		GLog.Print( "\n" );
		OutputErrorString();
	}
	return ( callResult == 0 );
}

bool RegistryManager::WriteBool( HKEY dirKey, const char *variable, bool value )
{
	unsigned int val = value?1:0;
	return WriteUInt( dirKey, variable, val );
}

bool RegistryManager::ReadCDKey( HKEY dirKey, String& cdkey )
{
	char cdk[20] = "1234-1234-1234-1234";
	cdk[19] = 0;
	bool ok = true;
	unsigned int i0 = 0,i1 = 0,i2 = 0,i3 = 0;

	if( !ReadUInt( dirKey, "RagdollData3", i0 ) ) ok = false;
	if( ok ) { if( !ReadUInt( dirKey, "RagdollData1", i1 ) ) ok = false; };
	if( ok ) { if( !ReadUInt( dirKey, "RagdollData0", i2 ) ) ok = false; };
	if( ok ) { if( !ReadUInt( dirKey, "RagdollData2", i3 ) ) ok = false; };
	if( ok )
	{
		i0 ^= 0x3ADE68AF;	// a prime
		i1 ^= 0x36614A8B;	// a prime
		i2 ^= 0x38F49EE9;	// a prime
		i3 ^= 0x399E29A5;	// a prime

		*((unsigned int*)cdk) = i0;
		*((unsigned int*)(cdk+5)) = i1;
		*((unsigned int*)(cdk+10)) = i2;
		*((unsigned int*)(cdk+15)) = i3;
		cdkey = cdk;
	}

	return ok;
}

bool RegistryManager::WriteCDKey( HKEY dirKey, const char *cdkey )
{
	if( strlen(cdkey) != 19 )
	{
		GLog.Print( "CD-Key's length is not 19 characters!\n" );
		return false;
	}

	char cdk[32];
#ifdef TRANSGAMING
	::strncpy(cdk, cdkey, 32);
#else
	::strcpy( cdk, cdkey );
#endif

	unsigned int i0 = *((unsigned int*)cdk);
	unsigned int i1 = *((unsigned int*)(cdk+5));
	unsigned int i2 = *((unsigned int*)(cdk+10));
	unsigned int i3 = *((unsigned int*)(cdk+15));

	i0 ^= 0x3ADE68AF;	// a prime
	i1 ^= 0x36614A8B;	// a prime
	i2 ^= 0x38F49EE9;	// a prime
	i3 ^= 0x399E29A5;	// a prime

	bool ok = true;
	if( !WriteUInt( dirKey, "RagdollData3", i0 ) ) ok = false;
	if( !WriteUInt( dirKey, "RagdollData1", i1 ) ) ok = false;
	if( !WriteUInt( dirKey, "RagdollData0", i2 ) ) ok = false;
	if( !WriteUInt( dirKey, "RagdollData2", i3 ) ) ok = false;
	return ok;
}
