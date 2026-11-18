/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */



#include <hkexport2/hkexport.h>
#include <hkexport2/exportspringaction.h>
#include <hkexport2/exportsubspace.h>
#include <hkexport2/exportrigidbody.h>
#include <hkexport2/exportworld.h>

using namespace NS_ExportStream;

// FACTORY ----------------------------------------------------------------------------------------------
EXPORT_SINGLETON_IMPLEMENTATION(ExportSpringActionFactory);

ExportAction* ExportSpringActionFactory::CreateExportAction(Token &token, ExportSubspace *subspace, CheckLog& log)
{
	if (token=="Spring") return new ExportSpringAction(subspace);
	return HK_NULL;
}

// ------------------------------------------------------------------------------------------------------

int ExportSpringAction::priv_CLASS_SerialNumber=1;

// Constructor
ExportSpringAction::ExportSpringAction(ExportSubspace *subspace) : ExportAction (subspace)
{
	PROT_AutoName();
	priv_hk_spring = HK_NULL;
	priv_rigid_body_A = HK_NULL;
	priv_rigid_body_B = HK_NULL;
}

// Destructor
ExportSpringAction::~ExportSpringAction()
{
	if (priv_hk_spring)
	{
		priv_hk_spring->removeReference();
		priv_hk_spring = HK_NULL;
	}
}

void ExportSpringAction::Import(InExportStream &xtream, CheckLog& log)
{
	float restitution,rest_length,damping;
	hkVector4 point_A,point_B;
	bool has_two_bodies = true;
	hkRigidBody* body_a = HK_NULL;
	hkRigidBody* body_b = HK_NULL;

	xtream>>prot_buffer; SetName(prot_buffer);
	log.BeginLevel(LOC("SPRING"),prot_buffer);

	xtream.TokenMustBe("COLLECTION"); xtream>>prot_buffer; //no longer of import

	xtream.TokenMustBe("BODY_A"); xtream>>prot_buffer;
	priv_rigid_body_A=prot_subspace->GetRigidBody(prot_buffer);
	body_a = priv_rigid_body_A->GetHKRigidBody();

	if ((!priv_rigid_body_A)||(!body_a))
	{
		char buffer[200];
		hkString::sprintf(buffer, LOC("Unknown Rigidbody in Spring: %s"), GetName());
		PROT_WarningAndRecover(buffer,xtream,"END_ACTION", log);
		return;
	}
	xtream.TokenMustBe("POINT_A"); xtream>>point_A;

	if (xtream.GetVersion()>=1350)
	{
		xtream.TokenMustBe("HAS_TWO_BODIES"); xtream>>has_two_bodies;
	}
	if (has_two_bodies)
	{
		xtream.TokenMustBe("BODY_B"); xtream>>prot_buffer;
		priv_rigid_body_B=prot_subspace->GetRigidBody(prot_buffer);
		body_b = priv_rigid_body_B->GetHKRigidBody();

		if ((!priv_rigid_body_B)||(!body_b))
		{
			char buffer[200];
			hkString::sprintf(buffer, LOC("Unknown Rigidbody in Spring: %s"), GetName());
			PROT_WarningAndRecover(buffer,xtream,"END_ACTION", log);
			return;
		}
	}
	else
	{
		body_b = GetSubspace()->GetWorld()->GetHKWorld()->getFixedRigidBody();
	}
	xtream.TokenMustBe("POINT_B"); xtream>>point_B;

	xtream.TokenMustBe("RESTITUTION"); xtream>>restitution;
	xtream.TokenMustBe("REST_LENGTH"); xtream>>rest_length;
	xtream.TokenMustBe("DAMPING"); xtream>>damping;

	bool on_compression=true, on_extension=true;
	if (xtream.GetVersion()>=140)
	{
		xtream.TokenMustBe("ON_COMPRESSION"); xtream>>on_compression;
		xtream.TokenMustBe("ON_EXTENSION"); xtream>>on_extension;
	}
	
	hkSpringActionConstructionInfo spring_info;

	point_A.setTransformedPos( priv_rigid_body_A->GetHavok2FromHavok1BodyTransform(), point_A);
	if (priv_rigid_body_B)
	{
		point_B.setTransformedPos( priv_rigid_body_B->GetHavok2FromHavok1BodyTransform(), point_B);
	}

	spring_info.setInBodySpace( body_a, body_b, point_A, point_B );
	spring_info.setDamping( damping );
	spring_info.setStrength ( restitution );
	spring_info.setRestLength ( rest_length );

	priv_hk_spring = new hkSpringAction( spring_info);

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
