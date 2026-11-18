/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>

#include <hklocalize/hklocalize.h>
#include <hkexport2/exportcollisiongroups.h>
#include <hkexport2/exportworld.h>

using namespace NS_ExportStream;

int	ExportCollisionGroups::priv_CLASS_SerialNumber=1;

ExportCollisionGroups::ExportCollisionGroups(ExportSubspace *subspace) 
: ExportClass(), priv_subspace( subspace )
{
	PROT_AutoName();
	priv_CLASS_SerialNumber++;
}

ExportCollisionGroups::~ExportCollisionGroups()
{
}

void ExportCollisionGroups::Import( InExportStream &xtream, CheckLog & log )
{
	xtream>>prot_buffer; SetName(prot_buffer);
	unsigned int group_num_A, group_num_B;

	while (xtream>>prot_token, prot_token==("COLLISIONS_ENABLED"))
	{
		xtream.TokenMustBe("GROUP");
		xtream>>group_num_A;
		xtream.TokenMustBe("GROUP");
		xtream>>group_num_B;
		if ( (group_num_A==32) || (group_num_B == 32) )
		{
			log.AddError( LOC("You are enabling collisions for group 32 on import. This has been ignored as 32 is reserved for havok 2 import of disable all collisions."));
			log.AddError( LOC("This could cause some unexpected behavior.") );
		}
		priv_subspace->GetWorld()->GetHKGroupCollisionFilter()->enableCollisionGroups( group_num_A, group_num_B );
	}
	if (prot_token!=("END_ENABLED_COLLISION_GROUPS")) throw_exception("Unexpected token. END_ENABLED_COLLISION_GROUPS was expected");
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
