/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>
#include <hkexport2/exportstiffspringconstraint.h>
#include <hkexport2/exportfastconstraintsolver.h>
#include <hkexport2/exportsubspace.h>
#include <hkexport2/exportworld.h>
using namespace NS_ExportStream;

// FACTORY ----------------------------------------------------------------------------------------------

EXPORT_SINGLETON_IMPLEMENTATION(ExportStiffSpringConstraintFactory);

ExportConstraint* ExportStiffSpringConstraintFactory::CreateExportConstraint(Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log)
{
	if (token=="StiffSpring") return new ExportStiffSpringConstraint(constraint_solver);
	return HK_NULL;
}

// ------------------------------------------------------------------------------------------------------

int ExportStiffSpringConstraint::priv_CLASS_SerialNumber=1;

// Constructor
ExportStiffSpringConstraint::ExportStiffSpringConstraint (ExportFastConstraintSolver *constraint_solver) 
: ExportConstraint (constraint_solver)
{
	PROT_AutoName();
	m_constraint = HK_NULL;
	priv_rigid_body_a = HK_NULL;
	priv_rigid_body_b = HK_NULL;
}

// Destructor
ExportStiffSpringConstraint::~ExportStiffSpringConstraint() 
{
	if (m_constraint)
	{
		m_constraint->removeReference();
		m_constraint = HK_NULL;
	}
}

// Import
void ExportStiffSpringConstraint::Import(InExportStream &xtream, CheckLog& log)
{
	float strength = 0.0f;
	float tau = 0.0f;
	float length = 0.0f;
	hkVector4 local_point_a,local_point_b;
	hkBool has_two_bodies = true;
	hkRigidBody* body_a = HK_NULL;
	hkRigidBody* body_b = HK_NULL;
	hkBool is_breakable = false;
	float linear_strength = 0.0f;
	float angular_strength = 0.0f;

	const hkConvertCS& converter = GetConstraintSolver()->GetSubspace()->GetWorld()->getCSConverter();

	xtream>>prot_buffer; SetName(prot_buffer);
	log.BeginLevel(LOC("STIFF SPRING CONSTRAINT"),prot_buffer);

	if (xtream.GetVersion()>=1330)
	{
		xtream.TokenMustBe("TWO_BODIED"); xtream>>has_two_bodies;
	}

	xtream.TokenMustBe("RIGID_BODY_A"); xtream>>prot_buffer;
	priv_rigid_body_a=prot_constraint_solver->GetSubspace()->GetRigidBody(prot_buffer);
	if (priv_rigid_body_a)
	{
		body_a = priv_rigid_body_a->GetHKRigidBody();
	}
	if ((!priv_rigid_body_a)||(!body_a))
	{
		char buffer[EXPORT_BUFFER_SIZE];
		hkString::sprintf(buffer, LOC("Unknown Rigidbody in Stiff Spring: %s"), GetName());
		PROT_WarningAndRecover( buffer, xtream, "END_CONSTRAINT", log, false );
		return;
	}
	xtream.TokenMustBe("LOCAL_POINT_A"); xtream>>local_point_a;
	converter.convertVector(local_point_a);

	if (has_two_bodies)
	{
		xtream.TokenMustBe("RIGID_BODY_B"); xtream>>prot_buffer;
		priv_rigid_body_b=prot_constraint_solver->GetSubspace()->GetRigidBody(prot_buffer);
		if (priv_rigid_body_b)
		{
			body_b = priv_rigid_body_b->GetHKRigidBody();
		}
		if ((!priv_rigid_body_b)||(!body_b))
		{
			char buffer[EXPORT_BUFFER_SIZE];
			hkString::sprintf(buffer, LOC("Unknown Rigidbody in Stiff Spring: %s"), GetName());
			PROT_WarningAndRecover( buffer, xtream, "END_CONSTRAINT", log, false );
			return;
		}
	}
	else
	{
		body_b = GetConstraintSolver()->GetSubspace()->GetWorld()->GetHKWorld()->getFixedRigidBody();
	}
	xtream.TokenMustBe("LOCAL_POINT_B"); xtream>>local_point_b;
	converter.convertVector(local_point_b);

	xtream.TokenMustBe("SPRING_LENGTH"); xtream>>length;
	
	if (xtream.GetVersion()>=1370)
	{
		xtream.TokenMustBe("IS_BREAKABLE"); xtream>>is_breakable;
		if (is_breakable)
		{
			xtream.TokenMustBe("LINEAR_STRENGTH"); xtream>>linear_strength;
			xtream.TokenMustBe("ANGULAR_STRENGTH"); xtream>>angular_strength;
		}
		xtream.TokenMustBe("STRENGTH"); xtream>>strength;
		xtream.TokenMustBe("TAU"); xtream>>tau;
		//priv_hk_stiff_spring->setStrength(strength);
		//priv_hk_stiff_spring->setTau(tau);
	}

	//bring to havok 2 space: see exportrigidbody.cpp
	local_point_a.setTransformedPos( priv_rigid_body_a->GetHavok2FromHavok1BodyTransform(), local_point_a );
	if (priv_rigid_body_b)
	{
		local_point_b.setTransformedPos( priv_rigid_body_b->GetHavok2FromHavok1BodyTransform(), local_point_b );
	}

	
	hkStiffSpringConstraintData* constraintData = new hkStiffSpringConstraintData();
	constraintData->setInBodySpace(body_a->getTransform(), body_b->getTransform(), 
									local_point_a, local_point_b);
	constraintData->setSpringLength(length);

	if (is_breakable)
	{
		if( xtream.GetVersion() < 2000 )
		{
			char buffer[200];
			hkString::sprintf( buffer, LOC("Havok 2 breakable constraint has only one tolerance value, this will be taken from the linear strength parameter: %f."), linear_strength);
			log.AddError( buffer );
		}

		hkWorld* world = GetConstraintSolver()->GetSubspace()->GetWorld()->GetHKWorld();
		hkBreakableConstraintData *breakConstraint = new hkBreakableConstraintData( constraintData, world );
		breakConstraint->setThreshold( linear_strength );

		m_constraint = new hkConstraintInstance( body_a, body_b, breakConstraint );
		breakConstraint->removeReference();
	}
	else
	{
		m_constraint = new hkConstraintInstance( body_a, body_b, constraintData );
	}
	constraintData->removeReference();


	GetHKConstraint()->setName(GetName());

	xtream.TokenMustBe("END_CONSTRAINT");
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
