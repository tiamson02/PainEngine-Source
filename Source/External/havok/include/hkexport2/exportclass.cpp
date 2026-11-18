/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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

void ExportClass::SetName(char *name)
{
	if (prot_name) hkDeallocate(prot_name);
	prot_name = hkString::strDup(name);
}

void throw_exception(char *message)
{
	hkprintf("%s\n",message);
	HK_ASSERT2(0,message);
}

// Sends a warning and tries to recover (when importing)
void ExportClass::PROT_WarningAndRecover( const char* warning, InExportStream &xtream, const char *str_token, CheckLog& log, bool is_checkpoint )
{
	log.AddError( warning );
	char buffer[ 200 ];
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

/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
