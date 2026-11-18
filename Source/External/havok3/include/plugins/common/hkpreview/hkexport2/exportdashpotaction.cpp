/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */



#include <hkexport2/hkexport.h>
#include <hkexport2/exportdashpotaction.h>
#include <hkexport2/exportactionfactory.h>
#include <hkexport2/exportrigidbody.h>
#include <hkexport2/exportworld.h>

using namespace NS_ExportStream;

// FACTORY ----------------------------------------------------------------------------------------------
EXPORT_SINGLETON_IMPLEMENTATION(ExportDashpotActionFactory);
ExportAction* ExportDashpotActionFactory::CreateExportAction(Token &token, ExportSubspace *subspace, CheckLog& log)
{
	if (token=="Dashpot") 
	{
		return new ExportDashpotAction(subspace);
	}
	return HK_NULL;
}

// ------------------------------------------------------------------------------------------------------

int ExportDashpotAction::priv_CLASS_SerialNumber=1;

// Constructor
ExportDashpotAction::ExportDashpotAction(ExportSubspace *subspace) : ExportAction (subspace)
{
	PROT_AutoName();
	priv_hk_dashpot = HK_NULL;
	priv_rigid_body_A = HK_NULL;
	priv_rigid_body_B = HK_NULL;
}

// Destructor
ExportDashpotAction::~ExportDashpotAction()
{
	if (priv_hk_dashpot)
	{
		priv_hk_dashpot->removeReference();
		priv_hk_dashpot = HK_NULL;
	}
}

void ExportDashpotAction::Import(InExportStream &xtream, CheckLog& log)
{
	float strength,damping;
	hkVector4 local_point_A,local_point_B;
	hkBool two_bodies;
	hkRigidBody* body_a = HK_NULL;
	hkRigidBody* body_b = HK_NULL;

	const hkConvertCS& converter = GetSubspace()->GetWorld()->getCSConverter();

	xtream>>prot_buffer; SetName(prot_buffer);
	log.BeginLevel(LOC("LINEAR DASHPOT"),prot_buffer);

	//we don't care about the collection
	xtream.TokenMustBe("COLLECTION"); xtream>>prot_buffer;

	xtream.TokenMustBe("STRENGTH"); xtream>>strength;
	xtream.TokenMustBe("DAMPING"); xtream>>damping;

	xtream.TokenMustBe("TWO_BODIES"); xtream>>two_bodies;

	xtream.TokenMustBe("BODY_A"); xtream>>prot_buffer;
	priv_rigid_body_A=prot_subspace->GetRigidBody(prot_buffer);
	if (priv_rigid_body_A)
	{
		body_a = priv_rigid_body_A->GetHKRigidBody();
	}
	if ((!priv_rigid_body_A)||(!body_a))
	{
		char buffer[EXPORT_BUFFER_SIZE];
		hkString::sprintf(buffer, LOC("Unknown Rigidbody in Dashpot: %s"), GetName());
		log.AddError(buffer);
		PROT_WarningAndRecover(buffer,xtream,"END_ACTION", log);
		return;
	}

	xtream.TokenMustBe("LOCAL_POINT_A"); xtream>>local_point_A;
	converter.convertVector(local_point_A);

	if (two_bodies)
	{
		xtream.TokenMustBe("BODY_B"); xtream>>prot_buffer;
		priv_rigid_body_B=prot_subspace->GetRigidBody(prot_buffer);
		if (priv_rigid_body_B)
		{
			body_b = priv_rigid_body_B->GetHKRigidBody();
		}
		if ((!priv_rigid_body_B)||(!body_b))
		{
			char buffer[EXPORT_BUFFER_SIZE];
			hkString::sprintf(buffer, LOC("Unknown Rigidbody in Dashpot: %s"), GetName());
			log.AddError(buffer);
			PROT_WarningAndRecover(buffer,xtream,"END_ACTION", log);
			return;
		}
		xtream.TokenMustBe("LOCAL_POINT_B"); xtream>>local_point_B;
		converter.convertVector(local_point_B);
	}
	else
	{
		xtream.TokenMustBe("WORLD_POINT_A"); xtream>>local_point_B;//this is actually the world point
		converter.convertVector(local_point_B);
		body_b = GetSubspace()->GetWorld()->GetHKWorld()->getFixedRigidBody();
	}

	if (priv_rigid_body_B)
	{
		local_point_B.setTransformedPos( priv_rigid_body_B->GetHavok2FromHavok1BodyTransform(), local_point_B );
	}
	local_point_A.setTransformedPos( priv_rigid_body_A->GetHavok2FromHavok1BodyTransform(), local_point_A );

	priv_hk_dashpot = new hkDashpotAction( );
	priv_hk_dashpot->setInBodySpace( local_point_A, local_point_B );
	priv_hk_dashpot->setStrength( strength );
	priv_hk_dashpot->setDamping( damping );

	priv_hk_dashpot->setName(GetName());

	xtream.TokenMustBe("END_ACTION");
	log.EndLevel();
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
