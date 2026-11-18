
// #pragma once
#ifndef _REGISTRYMANAGER_
#define _REGISTRYMANAGER_


#include "Core/LeString.h"

class ENGINE_API RegistryManager
{
public:
	RegistryManager();
	~RegistryManager();

	void Update( void );

	// *********** DATA ********************

	String RegistryPath;

	// *********** Variables ***************

#ifndef TRANSGAMING
	String			ExePathname;
#endif

	// == MULTIPLAYER ==

	// General variables
	String			CDKey;
	String			PlayerName;

	// Client variables
	String			LastHostName;
	String			LastPassword;
	unsigned int	LastPort;

	// Server variables
	bool			IsPublic;
	bool			ServerMode;
	unsigned int	FragsLimit;
	String			Map;

	// Bonus stuff
	bool			BonusPlayerModel;
	bool			BonusCardsUse;
	bool			BonusPlayerModel2;

protected:
	void ReadFromRegistry( void );
	void OutputErrorString( void );

	bool ReadString( HKEY dirKey, const char *variable, String& value );
	bool ReadUInt( HKEY dirKey, const char *variable, unsigned int& value );
	bool ReadBool( HKEY dirKey, const char *variable, bool& value );
	bool ReadCDKey( HKEY dirKey, String& cdkey );

	bool WriteString( HKEY dirKey, const char *variable, const char *value );
	bool WriteUInt( HKEY dirKey, const char *variable, unsigned int value );
	bool WriteBool( HKEY dirKey, const char *variable, bool value );
	bool WriteCDKey( HKEY dirKey, const char *cdkey );
};


#endif //_REGISTRYMANAGER_


