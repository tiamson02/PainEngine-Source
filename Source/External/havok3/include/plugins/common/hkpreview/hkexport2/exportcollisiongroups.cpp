/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>

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

		priv_subspace->GetWorld()->enableCollisionGroups( group_num_A, group_num_B );
	}
	if (prot_token!=("END_ENABLED_COLLISION_GROUPS")) throw_exception("Unexpected token. END_ENABLED_COLLISION_GROUPS was expected");
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
