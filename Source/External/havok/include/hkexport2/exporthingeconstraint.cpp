/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>

#include <hkexport2/exporthingeconstraint.h>
#include <hkexport2/exportfastconstraintsolver.h>
#include <hkexport2/exportsubspace.h>
#include <hkexport2/exportworld.h>
//#include <hkmath2/interval.h>
//#include <hkdynamics/action/constraintlimit.h>
using namespace NS_ExportStream;

// FACTORY ----------------------------------------------------------------------------------------------

EXPORT_SINGLETON_IMPLEMENTATION(ExportHingeConstraintFactory);

ExportConstraint* ExportHingeConstraintFactory::CreateExportConstraint(Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log)
{
	if (token=="Hinge") return new ExportHingeConstraint(constraint_solver);
	return HK_NULL;
}
// ------------------------------------------------------------------------------------------------------

int ExportHingeConstraint::priv_CLASS_SerialNumber=1;

// Constructor
ExportHingeConstraint::ExportHingeConstraint (ExportFastConstraintSolver *constraint_solver) 
: ExportConstraint (constraint_solver)
{
	PROT_AutoName();

	priv_hk_hinge = HK_NULL;
	priv_hk_limited_hinge = HK_NULL;
	priv_is_limited = false;
	priv_rigid_body_a = HK_NULL;
	priv_rigid_body_b = HK_NULL;
}

// Destructor
ExportHingeConstraint::~ExportHingeConstraint() 
{
	if (priv_hk_hinge)
	{
		priv_hk_hinge->removeReference();
		priv_hk_hinge = HK_NULL;
	}
	if (priv_hk_limited_hinge)
	{
		priv_hk_limited_hinge->removeReference();
		priv_hk_limited_hinge = HK_NULL;
	}
}

hkConstraint* ExportHingeConstraint::GetHKConstraint() const 
{
	if (GetIsLimited())
	{
		return priv_hk_limited_hinge;
	}
	return priv_hk_hinge;
}

// Import
void ExportHingeConstraint::Import(InExportStream &xtream, CheckLog &log)
{
	float strength = 0.0f;
	float tau = 0.0f;
	float min_angle, max_angle, friction;
	hkVector4 world_hinge_pos,world_hinge_dir;
	hkVector4 hinge_pos_a, hinge_pos_b, hinge_axis_a, hinge_axis_b, hinge_perp_a, hinge_perp_b;
	float linear_strength, angular_strength;
	bool is_breakable = false;
	bool has_two_bodies = true;
	hkRigidBody* hk_body_a = HK_NULL;
	hkRigidBody* hk_body_b = HK_NULL;

	xtream>>prot_buffer; SetName(prot_buffer);
	log.BeginLevel(LOC("HINGE CONSTRAINT"),prot_buffer);

	if (xtream.GetVersion()>=1330)
	{
		xtream.TokenMustBe("TWO_BODIED"); xtream>>has_two_bodies;
	}

	xtream.TokenMustBe("RIGID_BODY_A"); xtream>>prot_buffer;
	priv_rigid_body_a = GetConstraintSolver()->GetSubspace()->GetRigidBody(prot_buffer);
	if (priv_rigid_body_a)
	{
		hk_body_a = priv_rigid_body_a->GetHKRigidBody();
	}
	if ((!priv_rigid_body_a)||(!hk_body_a))
	{
		char buffer[200];
		hkString::sprintf(buffer, LOC("Unknown Rigidbody in Hinge: %s"), GetName());
		PROT_WarningAndRecover( buffer, xtream, "END_CONSTRAINT", log, false );
		return;
	}

	if (has_two_bodies)
	{
		xtream.TokenMustBe("RIGID_BODY_B"); xtream>>prot_buffer;
		priv_rigid_body_b=prot_constraint_solver->GetSubspace()->GetRigidBody(prot_buffer);
		if (priv_rigid_body_b)
		{
			hk_body_b = priv_rigid_body_b->GetHKRigidBody();
		}
		if ((!priv_rigid_body_b)||(!hk_body_b))
		{
			char buffer[200];
			hkString::sprintf(buffer, LOC("Unknown Rigidbody in Hinge: %s"), GetName());
			PROT_WarningAndRecover( buffer, xtream, "END_CONSTRAINT", log, false );
			return;
		}
	}
	else
	{
		hk_body_b = GetConstraintSolver()->GetSubspace()->GetWorld()->GetHKWorld()->getFixedRigidBody();
	}

	if (xtream.GetVersion()>=1370)
	{
		xtream.TokenMustBe("HINGE_POS_IN_A"); xtream>>hinge_pos_a;
		xtream.TokenMustBe("HINGE_POS_IN_B"); xtream>>hinge_pos_b;
		xtream.TokenMustBe("HINGE_DIR_IN_A"); xtream>>hinge_axis_a;
		xtream.TokenMustBe("HINGE_DIR_IN_B"); xtream>>hinge_axis_b;
		xtream.TokenMustBe("HINGE_DIR_PERP_IN_A"); xtream>>hinge_perp_a;
		xtream.TokenMustBe("HINGE_DIR_PERP_IN_B"); xtream>>hinge_perp_b;
		xtream.TokenMustBe("IS_BREAKABLE"); xtream>>is_breakable;
		if (is_breakable)
		{
			xtream.TokenMustBe("LINEAR_STRENGTH"); xtream>>linear_strength;
			xtream.TokenMustBe("ANGULAR_STRENGTH"); xtream>>angular_strength;
		}
		xtream.TokenMustBe("STRENGTH"); xtream>>strength;
		xtream.TokenMustBe("TAU"); xtream>>tau;
//		priv_hk_hinge->setStrength(strength);
//		priv_hk_hinge->setTau(tau);

		hinge_axis_a.normalize3();
		hinge_perp_a.normalize3();
	}	
	else
	{
		xtream.TokenMustBe("WORLD_HINGE_POS"); xtream>>world_hinge_pos;
		xtream.TokenMustBe("WORLD_HINGE_DIR"); xtream>>world_hinge_dir;
	}

	//transform from body to primitive_space
	hinge_pos_a.setTransformedPos( priv_rigid_body_a->GetHavok2FromHavok1BodyTransform(), hinge_pos_a );
	hinge_axis_a.setRotatedDir( priv_rigid_body_a->GetHavok2FromHavok1BodyTransform().getRotation(), hinge_axis_a );
	hinge_perp_a.setRotatedDir( priv_rigid_body_a->GetHavok2FromHavok1BodyTransform().getRotation(), hinge_perp_a );
	if( priv_rigid_body_b )
	{
		hinge_pos_b.setTransformedPos( priv_rigid_body_b->GetHavok2FromHavok1BodyTransform(), hinge_pos_b );
		hinge_axis_b.setRotatedDir( priv_rigid_body_b->GetHavok2FromHavok1BodyTransform().getRotation(), hinge_axis_b );
		hinge_perp_b.setRotatedDir( priv_rigid_body_b->GetHavok2FromHavok1BodyTransform().getRotation(), hinge_perp_b );
	}

	xtream.TokenMustBe("IS_LIMITED"); xtream>>priv_is_limited;
	if(priv_is_limited)
	{
		xtream.TokenMustBe("LIMIT_MIN_ANGLE"); xtream>>min_angle;
		xtream.TokenMustBe("LIMIT_MAX_ANGLE"); xtream>>max_angle;
		xtream.TokenMustBe("LIMIT_FRICTION"); xtream>>friction;
		hkLimitedHingeConstructionInfo l_h_construct_info;

		if (xtream.GetVersion()>=1370)
		{
			l_h_construct_info.setInBodySpace(hk_body_a,hk_body_b, hinge_pos_a, hinge_pos_b, hinge_axis_a, hinge_axis_b, hinge_perp_a, hinge_perp_b);
			priv_hk_limited_hinge = new hkLimitedHingeConstraint( l_h_construct_info);
		}
		else
		{
			l_h_construct_info.setInWorldSpace( hk_body_a, hk_body_b, world_hinge_pos, world_hinge_dir);
			priv_hk_limited_hinge = new hkLimitedHingeConstraint( l_h_construct_info );
		}
		priv_hk_limited_hinge->setMaxAngularLimit(max_angle);
		priv_hk_limited_hinge->setMinAngularLimit(min_angle);
	}
	else
	{
		hkHingeConstructionInfo h_construct_info;
		if (xtream.GetVersion()>=1370)
		{
			h_construct_info.setInBodySpace( hk_body_a, hk_body_b, hinge_pos_a, hinge_pos_b, hinge_axis_a, hinge_axis_b);
		}
		else
		{
			h_construct_info.setInWorldSpace( hk_body_a, hk_body_b, world_hinge_pos, world_hinge_dir);
		}
		priv_hk_hinge = new hkHingeConstraint( h_construct_info );
	}

	if (is_breakable)
	{
		char buffer[200];
		hkString::sprintf( buffer, LOC("Havok 2 breakable constraint has only one tolerance value, this will be taken from the linear strength parameter: %f."), linear_strength);
		log.AddError( buffer );
		if (priv_is_limited)
		{
			hkWorld* world = GetConstraintSolver()->GetSubspace()->GetWorld()->GetHKWorld();
			SetHKBreakableConstraint( new hkBreakableConstraint( priv_hk_limited_hinge, world ) );
		}
		else
		{
			hkWorld* world = GetConstraintSolver()->GetSubspace()->GetWorld()->GetHKWorld();
			SetHKBreakableConstraint( new hkBreakableConstraint( priv_hk_hinge, world ) );
		}
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
