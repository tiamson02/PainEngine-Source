/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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
	hkBool two_bodies;
	hkRigidBody* body_a = HK_NULL;
	hkRigidBody* body_b = HK_NULL;

	const hkConvertCS& converter = GetSubspace()->GetWorld()->getCSConverter();

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
		char buffer[EXPORT_BUFFER_SIZE];
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
			char buffer[EXPORT_BUFFER_SIZE];
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
	converter.convertQuaternion(rotation);

	//rotation is currently from havok_1_body_a->havok1_body_b 
	//See export rigid body for details on how the rigid body's transform is changed on import to havok 2
	//we need havok_2_body_b_from_havok_2_body_b --> havok_2_b_from_havok_1_b * havok_1_b_from_havok_1_a * havok_1_a_from_havok_2_a
	hkRotation havok_1_b_from_havok_1_a; havok_1_b_from_havok_1_a.set( rotation );
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
	
	priv_hk_angular_dashpot = new hkAngularDashpotAction(body_a, body_b);

	priv_hk_angular_dashpot->m_rotation = hkQuaternion( havok_2_b_from_havok_2_a );
	priv_hk_angular_dashpot->m_damping = damping;
	priv_hk_angular_dashpot->m_strength = strength;


	priv_hk_angular_dashpot->setName(GetName());


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
