/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>
#include <hkexport2/exportangulardashaction.h>
#include <hkexport2/exportsubspace.h>
#include <hkexport2/exportrigidbody.h>
#include <hkexport2/exportworld.h>

using namespace NS_ExportStream;

// FACTORY ----------------------------------------------------------------------------------------------
EXPORT_SINGLETON_IMPLEMENTATION(ExportAngularDashpotActionFactory);

ExportAction* ExportAngularDashpotActionFactory::CreateExportAction(Token &token, ExportSubspace *subspace, CheckLog& log)
{
	if (token=="AngularDashpot") 
	{
		return new ExportAngularDashpotAction(subspace);
	}
	return HK_NULL;
}

// ------------------------------------------------------------------------------------------------------

int ExportAngularDashpotAction::priv_CLASS_SerialNumber=1;

// Constructor
ExportAngularDashpotAction::ExportAngularDashpotAction(ExportSubspace *subspace) : ExportAction (subspace)
{
	PROT_AutoName();
	priv_hk_angular_dashpot = HK_NULL;
	priv_rigid_body_A = HK_NULL;
	priv_rigid_body_B = HK_NULL;
}

// Destructor
ExportAngularDashpotAction::~ExportAngularDashpotAction()
{
	if (priv_hk_angular_dashpot)
	{
		priv_hk_angular_dashpot->removeReference();
		priv_hk_angular_dashpot = HK_NULL;
	}
}

void ExportAngularDashpotAction::Import(InExportStream &xtream, CheckLog& log)
{
	float strength,damping;
	hkQuaternion rotation;
	bool two_bodies;
	hkRigidBody* body_a = HK_NULL;
	hkRigidBody* body_b = HK_NULL;

	xtream>>prot_buffer; SetName(prot_buffer);
	log.BeginLevel(LOC("ANGULAR DASHPOT"),prot_buffer);

	//collections are elements from havok 1 which we can safely ignore on import
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
	if ( ( !priv_rigid_body_A ) || ( !body_a ) )
	{
		char buffer[200];
		hkString::sprintf(buffer, LOC("Unknown Rigidbody in AngularDashpot: %s"), GetName());
		PROT_WarningAndRecover(buffer,xtream,"END_ACTION", log);
		return;
	}

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
			char buffer[200];
			hkString::sprintf(buffer, LOC("Unknown Rigidbody in AngularDashpot: %s"), GetName());
			PROT_WarningAndRecover( buffer, xtream, "END_ACTION", log );
			return;
		}
	}
	else
	{
		body_b = GetSubspace()->GetWorld()->GetHKWorld()->getFixedRigidBody();
	}
	xtream.TokenMustBe("ROTATION"); xtream>>rotation;

	//rotation is currently from havok_1_body_a->havok1_body_b 
	//See export rigid body for details on how the rigid body's transform is changed on import to havok 2
	//we need havok_2_body_b_from_havok_2_body_b --> havok_2_b_from_havok_1_b * havok_1_b_from_havok_1_a * havok_1_a_from_havok_2_a
	hkRotation havok_1_b_from_havok_1_a( rotation );
	hkRotation havok_2_b_from_havok_1_a;
	hkRotation havok_2_b_from_havok_2_a;
	if (priv_rigid_body_B)
	{
		//havok_2_b_from_havok_1_b * havok_1_b_from_havok_1_a
		havok_2_b_from_havok_1_a.setMul( priv_rigid_body_B->GetHavok2FromHavok1BodyTransform().getRotation(), havok_1_b_from_havok_1_a);
		//(havok_2_b_from_havok_1_b * havok_1_b_from_havok_1_a) * havok_1_a_from_havok_2_a --> 
		//(havok_2_b_from_havok_1_b * havok_1_b_from_havok_1_a) * havok_2_a_from_havok_1_a^-1
		havok_2_b_from_havok_2_a.setMulInverse( havok_2_b_from_havok_1_a, priv_rigid_body_A->GetHavok2FromHavok1BodyTransform().getRotation() );
	}
	else
	{
		//(havok_2_b_from_havok_1_b * havok_1_b_from_havok_1_a) * havok_1_a_from_havok_2_a --> 
		//(havok_2_b_from_havok_1_b * havok_1_b_from_havok_1_a) * havok_2_a_from_havok_1_a^-1
		//if there's no second body then havok_2_b <-> havok_1_b = identity
		havok_2_b_from_havok_2_a.setMulInverse( havok_1_b_from_havok_1_a, priv_rigid_body_A->GetHavok2FromHavok1BodyTransform().getRotation() );
	}
	
	hkAngularDashpotActionConstructionInfo construct_info;
	construct_info.set( body_a, body_b, hkQuaternion( havok_2_b_from_havok_2_a ));
	construct_info.setDamping( damping );
	construct_info.setStrength( strength );

	priv_hk_angular_dashpot = new hkAngularDashpotAction( construct_info );

	xtream.TokenMustBe("END_ACTION");
	log.EndLevel();
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
