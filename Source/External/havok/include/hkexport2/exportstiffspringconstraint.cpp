/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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
	priv_hk_stiff_spring = HK_NULL;
	priv_rigid_body_a = HK_NULL;
	priv_rigid_body_b = HK_NULL;
}

// Destructor
ExportStiffSpringConstraint::~ExportStiffSpringConstraint() 
{
	if (priv_hk_stiff_spring)
	{
		priv_hk_stiff_spring->removeReference();
		priv_hk_stiff_spring = HK_NULL;
	}
}

// Import
void ExportStiffSpringConstraint::Import(InExportStream &xtream, CheckLog& log)
{
	float strength = 0.0f;
	float tau = 0.0f;
	float length = 0.0f;
	hkVector4 local_point_a,local_point_b;
	bool has_two_bodies = true;
	hkRigidBody* body_a = HK_NULL;
	hkRigidBody* body_b = HK_NULL;
	bool is_breakable = false;
	float linear_strength = 0.0f;
	float angular_strength = 0.0f;

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
		char buffer[200];
		hkString::sprintf(buffer, LOC("Unknown Rigidbody in Stiff Spring: %s"), GetName());
		PROT_WarningAndRecover( buffer, xtream, "END_CONSTRAINT", log, false );
		return;
	}
	xtream.TokenMustBe("LOCAL_POINT_A"); xtream>>local_point_a;
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
			char buffer[200];
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

	hkStiffSpringConstructionInfo stiff_spring_info;
	stiff_spring_info.setInBodySpace(body_a, body_b, local_point_a, local_point_b);
	stiff_spring_info.setSpringLength(length);

	priv_hk_stiff_spring = new hkStiffSpringConstraint( stiff_spring_info);

	if (is_breakable)
	{
		char buffer[200];
		hkString::sprintf( buffer, LOC("Havok 2 breakable constraint has only one tolerance value, this will be taken from the linear strength parameter: %f."), linear_strength);
		log.AddError( buffer );
		hkWorld* world = GetConstraintSolver()->GetSubspace()->GetWorld()->GetHKWorld();
		SetHKBreakableConstraint( new hkBreakableConstraint( priv_hk_stiff_spring, world ) );
		GetHKBreakableConstraint()->setThreshold( linear_strength );
	}

	xtream.TokenMustBe("END_CONSTRAINT");
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
