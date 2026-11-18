/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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
	hkBool has_two_bodies = true;
	hkRigidBody* body_a = HK_NULL;
	hkRigidBody* body_b = HK_NULL;

	const hkConvertCS& converter = GetSubspace()->GetWorld()->getCSConverter();

	xtream>>prot_buffer; SetName(prot_buffer);
	log.BeginLevel(LOC("SPRING"),prot_buffer);

	xtream.TokenMustBe("COLLECTION"); xtream>>prot_buffer; //no longer of import

	xtream.TokenMustBe("BODY_A"); xtream>>prot_buffer;
	priv_rigid_body_A=prot_subspace->GetRigidBody(prot_buffer);
	body_a = priv_rigid_body_A->GetHKRigidBody();

	if ((!priv_rigid_body_A)||(!body_a))
	{
		char buffer[EXPORT_BUFFER_SIZE];
		hkString::sprintf(buffer, LOC("Unknown Rigidbody in Spring: %s"), GetName());
		PROT_WarningAndRecover(buffer,xtream,"END_ACTION", log);
		return;
	}
	xtream.TokenMustBe("POINT_A"); xtream>>point_A;
	converter.convertVector(point_A);

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
			char buffer[EXPORT_BUFFER_SIZE];
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
	converter.convertVector(point_B);

	xtream.TokenMustBe("RESTITUTION"); xtream>>restitution;
	xtream.TokenMustBe("REST_LENGTH"); xtream>>rest_length;
	xtream.TokenMustBe("DAMPING"); xtream>>damping;

	hkBool on_compression=true, on_extension=true;
	if (xtream.GetVersion()>=140)
	{
		xtream.TokenMustBe("ON_COMPRESSION"); xtream>>on_compression;
		xtream.TokenMustBe("ON_EXTENSION"); xtream>>on_extension;
	}

	priv_hk_spring = new hkSpringAction( body_a, body_b );
	point_A.setTransformedPos( priv_rigid_body_A->GetHavok2FromHavok1BodyTransform(), point_A);
	if (priv_rigid_body_B)
	{
		point_B.setTransformedPos( priv_rigid_body_B->GetHavok2FromHavok1BodyTransform(), point_B);
	}

	priv_hk_spring->setPositionsInBodySpace( point_A, point_B );
	priv_hk_spring->m_damping = damping ;
	priv_hk_spring->m_strength = restitution;
	priv_hk_spring->m_restLength = rest_length;

	priv_hk_spring->setName(GetName());

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
