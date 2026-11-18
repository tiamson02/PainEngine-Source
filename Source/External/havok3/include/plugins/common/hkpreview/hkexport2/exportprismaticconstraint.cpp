/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>
#include <hkexport2/exportprismaticconstraint.h>
#include <hkexport2/exportfastconstraintsolver.h>
#include <hkexport2/exportsubspace.h>
#include <hkexport2/exportworld.h>
#include <hkmath/linear/hkVector4Util.h>

using namespace NS_ExportStream;

// FACTORY ----------------------------------------------------------------------------------------------
EXPORT_SINGLETON_IMPLEMENTATION(ExportPrismaticConstraintFactory);

ExportConstraint* ExportPrismaticConstraintFactory::CreateExportConstraint(Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log)
{
	if (token=="Prismatic") return new ExportPrismaticConstraint(constraint_solver);
	return HK_NULL;
}

// ------------------------------------------------------------------------------------------------------

int ExportPrismaticConstraint::priv_CLASS_SerialNumber=1;

// Constructor
ExportPrismaticConstraint::ExportPrismaticConstraint (ExportFastConstraintSolver *constraint_solver) 
: ExportConstraint (constraint_solver)
{
	PROT_AutoName();
	m_constraint=HK_NULL;
	priv_rigid_body_ref = HK_NULL;
	priv_rigid_body_att = HK_NULL;
}

// Destructor
ExportPrismaticConstraint::~ExportPrismaticConstraint() 
{
	if (m_constraint)
	{
		m_constraint->removeReference();
		m_constraint = HK_NULL;
	}
}

// Import
void ExportPrismaticConstraint::Import(InExportStream &xtream, CheckLog& log)
{
	float strength = 0.0f;
	float tau = 0.0f;
	hkQuaternion b_to_a_rot;
	hkVector4 b_to_a_trans, slide_dir_relative_to_a;
	hkBool has_two_bodies = true;
	hkBool is_breakable = false;
	float linear_strength = 0;
	float angular_strength = 0;
	hkBool is_limited = false;
	float limit_min = 0;
	float limit_max = 0;
	float limit_friction = 0;
	hkRigidBody* hk_body_ref = HK_NULL;
	hkRigidBody* hk_body_att = HK_NULL;
	hkVector4 pivot_ref, pivot_att, axis_ref, axis_att, axis_perp_ref, axis_perp_att;
	hkTransform refcs_from_attcs, att_from_attcs, att_from_ref, refcs_from_ref, att_from_ref_via_cs, ref_from_refcs;

	const hkConvertCS& converter = GetConstraintSolver()->GetSubspace()->GetWorld()->getCSConverter();


	xtream>>prot_buffer; SetName(prot_buffer);
	log.BeginLevel( LOC("PRISMATIC CONSTRAINT"), prot_buffer);

	xtream.TokenMustBe("TWO_BODIED"); xtream>>has_two_bodies;

	if (has_two_bodies)
	{
		xtream.TokenMustBe("RIGID_BODY_REFERENCE"); xtream>>prot_buffer;
		priv_rigid_body_ref=prot_constraint_solver->GetSubspace()->GetRigidBody(prot_buffer);
		if (priv_rigid_body_ref)
		{
			hk_body_ref = priv_rigid_body_ref->GetHKRigidBody();
		}
		if ((!priv_rigid_body_ref)||(!hk_body_ref))
		{
			char buffer[EXPORT_BUFFER_SIZE];
			hkString::sprintf(buffer, LOC("Unknown Reference Rigidbody in Prismatic: %s"), GetName());
			PROT_WarningAndRecover( buffer, xtream, "END_CONSTRAINT", log, false );
			return;
		}
	}
	else
	{
		hk_body_ref = GetConstraintSolver()->GetSubspace()->GetWorld()->GetHKWorld()->getFixedRigidBody();
	}

	xtream.TokenMustBe("RIGID_BODY_ATTACHED"); xtream>>prot_buffer;
	priv_rigid_body_att=prot_constraint_solver->GetSubspace()->GetRigidBody(prot_buffer);
	if (priv_rigid_body_att)
	{
		hk_body_att = priv_rigid_body_att->GetHKRigidBody();
	}
	if ((!priv_rigid_body_att)||(!hk_body_att))
	{
		char buffer[EXPORT_BUFFER_SIZE];
		hkString::sprintf(buffer, LOC("Unknown Attached Rigidbody in Prismatic: %s"), GetName());
		PROT_WarningAndRecover( buffer, xtream, "END_CONSTRAINT", log, false );
		return;
	}

	xtream.TokenMustBe("B_TO_A_ROTATION"); xtream>>b_to_a_rot;
	converter.convertQuaternion(b_to_a_rot);

	xtream.TokenMustBe("B_TO_A_TRANSLATION"); xtream>>b_to_a_trans;
	converter.convertVector(b_to_a_trans);

	xtream.TokenMustBe("SLIDE_DIRECTION_RELATIVE_TO_A"); xtream>>slide_dir_relative_to_a;
	converter.convertVector(slide_dir_relative_to_a);

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
		//priv_hk_prismatic->setStrength(strength);
		//priv_hk_prismatic->setTau(tau);
		xtream.TokenMustBe("IS_LIMITED"); xtream>>is_limited;
		if (is_limited)
		{
			xtream.TokenMustBe("LIMIT_MIN"); xtream>>limit_min;
			xtream.TokenMustBe("LIMIT_MAX"); xtream>>limit_max;
			xtream.TokenMustBe("LIMIT_FRICTION"); xtream>>limit_friction;
		}
	}

	//a = ref, b = att b_to_a = att_to_ref = hk2_att_to_hk2_ref = hk2_ref_from_hk2_att = hk2ref_from_hk1ref * hk1ref_from_hk1att * hk1att_from_hk2att
	//(hk2ref_from_hk1ref * hk1ref_from_hk1att) * hk2att_from_hk1att^-1
	
	hkTransform ref_from_att( b_to_a_rot, b_to_a_trans );
	if ( priv_rigid_body_ref )
	{
		ref_from_att.setMul( priv_rigid_body_ref->GetHavok2FromHavok1BodyTransform(), ref_from_att);
		slide_dir_relative_to_a.setRotatedDir( priv_rigid_body_ref->GetHavok2FromHavok1BodyTransform().getRotation(), slide_dir_relative_to_a);
	}
	hkTransform att_from_prima;
	att_from_prima.setInverse( priv_rigid_body_att->GetHavok2FromHavok1BodyTransform() );
	ref_from_att.setMulEq( att_from_prima );	

	//we have ref from attached so construct the space slideDir from that provided and a calculated perpendicular, 
	//giving the pivot as the identity in ref as it's arbitrary, then invert to give att_from_ref
	//and multiply our constructed space by it to get the axis etc in att space
	
	pivot_att.set( 0.0f, 0.0f, 0.0f);
	pivot_ref.setTransformedPos( ref_from_att, pivot_att);

	axis_ref = slide_dir_relative_to_a;
	axis_ref.normalize3();
	axis_att.setRotatedInverseDir( ref_from_att.getRotation(), slide_dir_relative_to_a);
	hkVector4Util::calculatePerpendicularVector( axis_ref, axis_perp_ref);
	axis_perp_ref.normalize3();
	axis_perp_att.setRotatedInverseDir( ref_from_att.getRotation(), axis_perp_ref);

	hkPrismaticConstraintData* constraintData = new hkPrismaticConstraintData();

	//before the order was ref then attached now it's att to ref so body a - the ref is passed for body b
	constraintData->setInBodySpace(pivot_att, pivot_ref, axis_att, axis_ref, axis_perp_att, axis_perp_ref); 
	if (is_limited)
	{
		constraintData->setMaxFrictionForce( limit_friction );
		constraintData->setMinLinearLimit( limit_min );
		constraintData->setMaxLinearLimit( limit_max );
	}
	
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

		m_constraint = new hkConstraintInstance( hk_body_att, hk_body_ref, breakConstraint );
		breakConstraint->removeReference();
	}
	else
	{
		m_constraint = new hkConstraintInstance( hk_body_att, hk_body_ref, constraintData );
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
