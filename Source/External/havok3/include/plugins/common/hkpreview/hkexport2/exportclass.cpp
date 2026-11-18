/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>
#include <hkexport2/exportclass.h>

using namespace NS_ExportStream;

ExportClass::ExportClass()
{
}

ExportClass::~ExportClass()
{
	if (prot_name)
	{
		hkDeallocate(prot_name);
	}
}

void ExportClass::PROT_AutoName()
{
	// This sets a default serialized name
	char def_name[200];

	hkString::sprintf(def_name,"%s_%d",ClassName(),SerialNumber());

	prot_name=hkString::strDup(def_name);
}

void ExportClass::SetName(const char* name)
{
	if (prot_name) hkDeallocate(prot_name);
	prot_name = hkString::strDup(name);
}

void throw_exception(char *message)
{
	//hkprintf("%s\n",message);
	HK_ASSERT2(0x1dad8c82, 0,message);
}

// Sends a warning and tries to recover (when importing)
void ExportClass::PROT_WarningAndRecover( const char* warning, InExportStream &xtream, const char *str_token, CheckLog& log, hkBool is_checkpoint )
{
	log.AddError( warning );
	char buffer[ EXPORT_BUFFER_SIZE ];
	int jump;
	if ( is_checkpoint )
	{
		jump = xtream.JumpToCheckpoint( str_token );
	}
	else
	{
		jump = xtream.JumpToToken( str_token );
	}
	if ( jump == -1 )
	{
		hkString::sprintf( buffer, "Couldn't recover from error: %s was not found", str_token );
		log.AddError( buffer );
		throw_exception(hkString::strDup( buffer ) );
	}
	else
	{
		hkString::sprintf( buffer, "%d lines/bytes jumped", jump );
		log.AddError( buffer );
	}
}

// Retrieves the name of this object as it was in the modeller, with modifications to make it compatible with 
// xml serialization (no spaces)
hkString ExportClass::GetSerializableName()
{
	const hkString original(GetName());
	const hkString result = original.replace(' ','_');
	
	return result;
}

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
