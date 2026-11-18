/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>
#include <hkexport2/exportptopconstraint.h>
#include <hkexport2/exportfastconstraintsolver.h>
#include <hkexport2/exportsubspace.h>
#include <hkexport2/exportworld.h>

using namespace NS_ExportStream;

// FACTORY ----------------------------------------------------------------------------------------------

EXPORT_SINGLETON_IMPLEMENTATION(ExportPointToPointConstraintFactory);

ExportConstraint * ExportPointToPointConstraintFactory::CreateExportConstraint(NS_ExportStream::Token &token,ExportFastConstraintSolver *constraint_solver,CheckLog& log)
{
	if (token=="PointToPoint") return new ExportPointToPointConstraint(constraint_solver);
	if (token=="FatPointToPoint") return new ExportPointToPointConstraint(constraint_solver);
	return HK_NULL;
}

// ------------------------------------------------------------------------------------------------------

int ExportPointToPointConstraint::priv_CLASS_SerialNumber=1;

// Constructor
ExportPointToPointConstraint::ExportPointToPointConstraint (ExportFastConstraintSolver *constraint_solver) 
: ExportConstraint (constraint_solver)
{
	PROT_AutoName();
	priv_hk_point_to_point=HK_NULL;
	priv_rigid_body_a = HK_NULL;
	priv_rigid_body_b = HK_NULL;
}

// Destructor
ExportPointToPointConstraint::~ExportPointToPointConstraint() 
{
	if (priv_hk_point_to_point)
	{
		priv_hk_point_to_point->removeReference();
		priv_hk_point_to_point = HK_NULL;
	}
}

// Import
void ExportPointToPointConstraint::Import(InExportStream &xtream, CheckLog& log)
{
	float strength = 0.0f;
	float tau = 0.0f;
	float old_tau = 0.0f;
	hkVector4 local_point_a,local_point_b;
	hkRigidBody* body_a = HK_NULL;
	hkRigidBody* body_b = HK_NULL;
	bool is_breakable = false;
	float linear_strength = 0.0f;
	float angular_strength = 0.0f;

	xtream>>prot_buffer; SetName(prot_buffer);
	log.BeginLevel(LOC("POINT TO POINT CONSTRAINT"),prot_buffer);

	if(xtream.GetVersion()<1320)
	{
		xtream.TokenMustBe("TAU"); xtream>>old_tau;
	}

	bool has_two_bodies = true;
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
		hkString::sprintf(buffer, LOC("Unknown Rigidbody in Point to Point: %s"), GetName());
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
			hkString::sprintf(buffer, LOC("Unknown Rigidbody in Point to Point: %s"), GetName());
			PROT_WarningAndRecover( buffer, xtream, "END_CONSTRAINT", log, false );
			return;
		}
	}
	else
	{
		body_b = GetConstraintSolver()->GetSubspace()->GetWorld()->GetHKWorld()->getFixedRigidBody();
	}
	xtream.TokenMustBe("LOCAL_POINT_B"); xtream>>local_point_b;
	
	hkBallAndSocketConstructionInfo constraint_info;

	//bring to havok2 rigid body space: see exportrigidbody.cpp
	local_point_a.setTransformedPos( priv_rigid_body_a->GetHavok2FromHavok1BodyTransform(), local_point_a);
	if (priv_rigid_body_b)
	{
		local_point_b.setTransformedPos( priv_rigid_body_b->GetHavok2FromHavok1BodyTransform(), local_point_b);
	}

	constraint_info.setInBodySpace( body_a, body_b, local_point_a, local_point_b);
	priv_hk_point_to_point = new hkBallAndSocketConstraint( constraint_info );

	if (xtream.GetVersion()>=1370)
	{
		xtream.TokenMustBe("IS_BREAKABLE"); xtream>>is_breakable;
		if (is_breakable)
		{
			xtream.TokenMustBe("LINEAR_STRENGTH"); xtream>>linear_strength;
			xtream.TokenMustBe("ANGULAR_STRENGTH"); xtream>>angular_strength;
			char buffer[200];
			hkString::sprintf( buffer, LOC("Havok 2 breakable constraint has only one tolerance value, this will be taken from the linear strength parameter: %f."), linear_strength);
			log.AddError( buffer );
			hkWorld* world = GetConstraintSolver()->GetSubspace()->GetWorld()->GetHKWorld();
			SetHKBreakableConstraint( new hkBreakableConstraint( priv_hk_point_to_point, world ) );
			GetHKBreakableConstraint()->setThreshold( linear_strength );
		}
		xtream.TokenMustBe("STRENGTH"); xtream>>strength;
		xtream.TokenMustBe("TAU"); xtream>>tau;
		//priv_hk_point_to_point->setStrength(strength);
		//priv_hk_point_to_point->setTau(tau);
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
