/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>
#include <hkexport2/exportragdollconstraint.h>
#include <hkexport2/exportfastconstraintsolver.h>
#include <hkexport2/exportsubspace.h>
#include <hkexport2/exportworld.h>
using namespace NS_ExportStream;

// FACTORY ----------------------------------------------------------------------------------------------

EXPORT_SINGLETON_IMPLEMENTATION(ExportRagdollConstraintFactory);

ExportConstraint* ExportRagdollConstraintFactory::CreateExportConstraint(Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log)
{
	if (token=="Ragdoll") return new ExportRagdollConstraint(constraint_solver);
	return HK_NULL;
}
// ------------------------------------------------------------------------------------------------------

int ExportRagdollConstraint::priv_CLASS_SerialNumber=1;

// Constructor
ExportRagdollConstraint::ExportRagdollConstraint (ExportFastConstraintSolver *constraint_solver) 
: ExportConstraint (constraint_solver)
{
	PROT_AutoName();
	priv_hk_ragdoll = HK_NULL;
	priv_rigid_body_ref = HK_NULL;
	priv_rigid_body_att = HK_NULL;
}

// Destructor
ExportRagdollConstraint::~ExportRagdollConstraint() 
{
	if (priv_hk_ragdoll)
	{
		priv_hk_ragdoll->removeReference();
		priv_hk_ragdoll = HK_NULL;
	}
}

// Import
void ExportRagdollConstraint::Import(InExportStream &xtream, CheckLog &log)
{
	float strength = 0.0f;
	float tau = 0.0f;
	float twist_min, twist_max, cone_min, cone_max, plane_min, plane_max;
	hkVector4 world_pivot_point, world_twist_axis, world_plane_axis;
	hkVector4 twist_ref, plane_ref, t_cross_p_ref, pos_ref;
	hkVector4 twist_att, plane_att, t_cross_p_att, pos_att;

	hkVector4 cs_to_ref_cols[4];
	hkVector4 cs_to_att_cols[4];
	bool has_two_bodies = true;
	bool is_breakable = false;
	float linear_strength, angular_strength;
	hkRigidBody* hk_body_ref = HK_NULL;
	hkRigidBody* hk_body_att = HK_NULL;


	xtream>>prot_buffer; SetName(prot_buffer);
	log.BeginLevel(LOC("RAGDOLL CONSTRAINT"), GetName());

	if (xtream.GetVersion()>=1330)
	{
		xtream.TokenMustBe("TWO_BODIED"); xtream>>has_two_bodies;
	}
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
			char buffer[200];
			hkString::sprintf(buffer, LOC("Unknown Reference Rigidbody in Ragdoll: %s"), GetName());
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
		char buffer[200];
		hkString::sprintf(buffer, LOC("Unknown Attached Rigidbody in Ragdoll: %s"), GetName());
		PROT_WarningAndRecover( buffer, xtream, "END_CONSTRAINT", log, false );
		return;
	}

	if (xtream.GetVersion()>=1370)
	{
		//here the transforms are read in in the format expected by the constraint .ie, twist,twist^plane,plane,translation
		xtream.TokenMustBe("CS_TO_REF_TM_COL0"); xtream>>twist_ref;
		xtream.TokenMustBe("CS_TO_REF_TM_COL1"); xtream>>t_cross_p_ref;
		xtream.TokenMustBe("CS_TO_REF_TM_COL2"); xtream>>plane_ref;
		xtream.TokenMustBe("CS_TO_REF_TM_COL3"); xtream>>pos_ref;
		xtream.TokenMustBe("CS_TO_ATT_TM_COL0"); xtream>>twist_att;
		xtream.TokenMustBe("CS_TO_ATT_TM_COL1"); xtream>>t_cross_p_att;
		xtream.TokenMustBe("CS_TO_ATT_TM_COL2"); xtream>>plane_att;
		xtream.TokenMustBe("CS_TO_ATT_TM_COL3"); xtream>>pos_att;
		xtream.TokenMustBe("IS_BREAKABLE"); xtream>>is_breakable;
		if (is_breakable)
		{
			xtream.TokenMustBe("LINEAR_STRENGTH"); xtream>>linear_strength;
			xtream.TokenMustBe("ANGULAR_STRENGTH"); xtream>>angular_strength;
		}
		xtream.TokenMustBe("STRENGTH"); xtream>>strength;
		xtream.TokenMustBe("TAU"); xtream>>tau;
	}
	else
	{
		xtream.TokenMustBe("WORLD_PIVOT_POINT"); xtream>>world_pivot_point;
		xtream.TokenMustBe("TWIST_AXIS"); xtream>>world_twist_axis;
		xtream.TokenMustBe("PLANE_AXIS"); xtream>>world_plane_axis;

		pos_ref.setTransformedInversePos( hk_body_ref->getTransform(), world_pivot_point);
		pos_att.setTransformedInversePos( hk_body_att->getTransform(), world_pivot_point);
		twist_ref.setRotatedInverseDir( hk_body_ref->getRotation(), world_twist_axis);
		twist_att.setRotatedInverseDir( hk_body_att->getRotation(), world_twist_axis);
		plane_ref.setRotatedInverseDir( hk_body_ref->getRotation(), world_plane_axis);
		plane_att.setRotatedInverseDir( hk_body_att->getRotation(), world_plane_axis);
	}
	xtream.TokenMustBe("TWIST_MIN"); xtream>>twist_min;
	xtream.TokenMustBe("TWIST_MAX"); xtream>>twist_max;
	xtream.TokenMustBe("CONE_MIN"); xtream>>cone_min;
	xtream.TokenMustBe("CONE_MAX"); xtream>>cone_max;
	xtream.TokenMustBe("PLANE_MIN"); xtream>>plane_min;
	xtream.TokenMustBe("PLANE_MAX"); xtream>>plane_max;


	//this rotation was carried out transparently in HK 1.6 to account for non-symmetrical cone limits so we do it explicitly here.
	//twist axis is rotated so that it is in the middle of the given cone limits
	hkReal center_cone = (cone_max + cone_min) * 0.5f;
	hkReal diff_cone   = (cone_max - cone_min) * 0.5f;

	if( hkMath::fabs( center_cone ) > 0.001f )
	{
		char buffer[200];
		hkString::sprintf(buffer, 
			LOC("Cone limits are non symmetrical, the reference twist axis will be rotated by (%f rads) to center the limits."), center_cone);
		log.AddError(buffer);
		log.AddError(LOC("This should not adversely affect the behavior of the constraint but explains the conversion to a single cone limit."));
		twist_ref.setRotatedDir( hkQuaternion( plane_ref, -center_cone ), twist_ref );
	}

	hkRagdollConstructionInfo doll_construct_info;
	doll_construct_info.setConeMinAngularLimit( diff_cone );
	doll_construct_info.setPlaneMinAngularLimit( plane_min );
	doll_construct_info.setPlaneMaxAngularLimit( plane_max );
	doll_construct_info.setTwistMinAngularLimit( twist_min );
	doll_construct_info.setTwistMaxAngularLimit( twist_max );

	//the other values get filled in by the constructor
	//change the elements which are in body space so that they are correct if the rigidbodies have had their
	//transforms shifted: see exportrigidbody.cpp so we want to put the points etc
	//into havok2body space -> multiply by stored havok2->havok1 transform
	hkTransform hk2ref_from_hk1ref;
	hkTransform hk2att_from_hk1att( priv_rigid_body_att->GetHavok2FromHavok1BodyTransform() );
	if (priv_rigid_body_ref)
	{
		hk2ref_from_hk1ref = priv_rigid_body_ref->GetHavok2FromHavok1BodyTransform();
	}
	else
	{
		hk2ref_from_hk1ref.setIdentity();
	}

	pos_ref.setTransformedPos( hk2ref_from_hk1ref, pos_ref );
	pos_att.setTransformedPos( hk2att_from_hk1att, pos_att );
	plane_ref.setRotatedDir( hk2ref_from_hk1ref.getRotation(), plane_ref );
	plane_att.setRotatedDir( hk2att_from_hk1att.getRotation(), plane_att );
	twist_ref.setRotatedDir( hk2ref_from_hk1ref.getRotation(), twist_ref );
	twist_att.setRotatedDir( hk2att_from_hk1att.getRotation(), twist_att );

	doll_construct_info.setInBodySpace( hk_body_ref, hk_body_att, pos_ref, pos_att, plane_ref, plane_att, twist_ref, twist_att); 
	priv_hk_ragdoll = new hkRagdollConstraint(doll_construct_info);

	if (is_breakable)
	{
		char buffer[200];
		hkString::sprintf( buffer, LOC("Havok 2 breakable constraint has only one tolerance value, this will be taken from the linear strength parameter: %f."), linear_strength);
		log.AddError( buffer );
		hkWorld* world = GetConstraintSolver()->GetSubspace()->GetWorld()->GetHKWorld();
		SetHKBreakableConstraint(new hkBreakableConstraint(priv_hk_ragdoll,world));
		GetHKBreakableConstraint()->setThreshold( linear_strength );
	}
	if (xtream.GetVersion() >= 1370)
	{
		//priv_hk_ragdoll->setStrength(strength);
		//priv_hk_ragdoll->setTau(tau);
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
