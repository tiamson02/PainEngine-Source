/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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
	m_constraint = HK_NULL;
	priv_rigid_body_ref = HK_NULL;
	priv_rigid_body_att = HK_NULL;
}

// Destructor
ExportRagdollConstraint::~ExportRagdollConstraint() 
{
	if (m_constraint)
	{
		m_constraint->removeReference();
		m_constraint = HK_NULL;
	}
}

// Import
void ExportRagdollConstraint::Import(InExportStream &xtream, CheckLog &log)
{
	float strength = 0.0f;
	float tau = 0.0f;
	float twist_min, twist_max, cone_min, cone_max, plane_min, plane_max;
	float friction = 0.0f;
	hkVector4 world_pivot_point, world_twist_axis, world_plane_axis;
	hkVector4 twist_ref, plane_ref, t_cross_p_ref, pos_ref;
	hkVector4 twist_att, plane_att, t_cross_p_att, pos_att;

	hkVector4 cs_to_ref_cols[4];
	hkVector4 cs_to_att_cols[4];
	hkBool has_two_bodies = true;
	hkBool is_breakable = false;
	float linear_strength = 0;
	float angular_strength = 0;
	hkRigidBody* hk_body_ref = HK_NULL;
	hkRigidBody* hk_body_att = HK_NULL;

	const hkConvertCS& converter = GetConstraintSolver()->GetSubspace()->GetWorld()->getCSConverter();


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
			char buffer[EXPORT_BUFFER_SIZE];
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
		char buffer[EXPORT_BUFFER_SIZE];
		hkString::sprintf(buffer, LOC("Unknown Attached Rigidbody in Ragdoll: %s"), GetName());
		PROT_WarningAndRecover( buffer, xtream, "END_CONSTRAINT", log, false );
		return;
	}

	if (xtream.GetVersion()>=1370)
	{
		//here the transforms are read in in the format expected by the constraint .ie, twist,twist^plane,plane,translation
		xtream.TokenMustBe("CS_TO_REF_TM_COL0"); xtream>>twist_ref;
		converter.convertVector(twist_ref);

		xtream.TokenMustBe("CS_TO_REF_TM_COL1"); xtream>>t_cross_p_ref;
		// t_cross_p_ref is ignored

		xtream.TokenMustBe("CS_TO_REF_TM_COL2"); xtream>>plane_ref;
		converter.convertVector(plane_ref);

		xtream.TokenMustBe("CS_TO_REF_TM_COL3"); xtream>>pos_ref;
		converter.convertVector(pos_ref);


		xtream.TokenMustBe("CS_TO_ATT_TM_COL0"); xtream>>twist_att;
		converter.convertVector(twist_att);

		xtream.TokenMustBe("CS_TO_ATT_TM_COL1"); xtream>>t_cross_p_att;
		// t_cross_p_att is ignored

		xtream.TokenMustBe("CS_TO_ATT_TM_COL2"); xtream>>plane_att;
		converter.convertVector(plane_att);

		xtream.TokenMustBe("CS_TO_ATT_TM_COL3"); xtream>>pos_att;
		converter.convertVector(pos_att);

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
		converter.convertVector(world_pivot_point);

		xtream.TokenMustBe("TWIST_AXIS"); xtream>>world_twist_axis;
		converter.convertVector(world_twist_axis);

		xtream.TokenMustBe("PLANE_AXIS"); xtream>>world_plane_axis;
		converter.convertVector(world_plane_axis);

		pos_ref.setTransformedInversePos( hk_body_ref->getTransform(), world_pivot_point);
		pos_att.setTransformedInversePos( hk_body_att->getTransform(), world_pivot_point);
		twist_ref.setRotatedInverseDir( hk_body_ref->getRotation(), world_twist_axis);
		twist_att.setRotatedInverseDir( hk_body_att->getRotation(), world_twist_axis);
		plane_ref.setRotatedInverseDir( hk_body_ref->getRotation(), world_plane_axis);
		plane_att.setRotatedInverseDir( hk_body_att->getRotation(), world_plane_axis);
	}

	xtream.TokenMustBe("TWIST_MIN"); xtream>>twist_min;
	xtream.TokenMustBe("TWIST_MAX"); xtream>>twist_max;
	converter.convertMinMaxAngles(twist_min, twist_max);

	xtream.TokenMustBe("CONE_MIN"); xtream>>cone_min;
	xtream.TokenMustBe("CONE_MAX"); xtream>>cone_max;
	converter.convertMinMaxAngles(cone_min, cone_max);

	xtream.TokenMustBe("PLANE_MIN"); xtream>>plane_min;
	xtream.TokenMustBe("PLANE_MAX"); xtream>>plane_max;
	// The plane min and max are specified not around and axis, but with respect to the plane normal
	// thus they don't need to be converted when using left-handed systems

	if (xtream.GetVersion()>=2000)
	{
		
		xtream.TokenMustBe("FRICTION"); xtream>>friction;
	}


	//this rotation was carried out transparently in HK 1.6 to account for non-symmetrical cone limits so we do it explicitly here.
	//twist axis is rotated so that it is in the middle of the given cone limits
	hkReal center_cone = (cone_max + cone_min) * 0.5f;
	hkReal diff_cone   = (cone_max - cone_min) * 0.5f;

	if( hkMath::fabs( center_cone ) > 0.001f )
	{

#if defined( HK_PLATFORM_PSP )
	
		// the R3000 compiler complains when processing the line below ( internal compiler error )
		// so most of the functions called by setRotatedDir(...) have been manually expanded
		hkQuaternion q( plane_ref, -center_cone );

		hkVector4 imagCrossV;
		hkVector4 qImag = q.getImag();

		// setCross(...)
		const hkReal nx = qImag(1)*twist_ref(2) - qImag(2)*twist_ref(1);
		const hkReal ny = qImag(2)*twist_ref(0) - qImag(0)*twist_ref(2);
		const hkReal nz = qImag(0)*twist_ref(1) - qImag(1)*twist_ref(0);
		imagCrossV.set( nx, ny, nz , 0.0f );

		hkReal scale[3];
		scale[0] = (2.0f * q.m_vec(3) * q.m_vec(3)) - 1.0f;
		scale[1] = 2.0f * ( q.m_vec(0)*twist_ref(0) + q.m_vec(1)*twist_ref(1) + q.m_vec(2)*twist_ref(2) );
		scale[2] = 2.0f * q.m_vec(3);

		twist_ref.setMul4( scale[0], twist_ref );
		twist_ref.addMul4( scale[1], q.getImag() );
		twist_ref.addMul4( scale[2], imagCrossV  );

#else

		twist_ref.setRotatedDir( hkQuaternion( plane_ref, -center_cone ), twist_ref );

#endif

	}

	hkRagdollConstraintData* constraintData = new hkRagdollConstraintData();
	constraintData->setConeAngularLimit( diff_cone );
	constraintData->setPlaneMinAngularLimit( plane_min );
	constraintData->setPlaneMaxAngularLimit( plane_max );
	constraintData->setTwistMinAngularLimit( twist_min );
	constraintData->setTwistMaxAngularLimit( twist_max );
	constraintData->m_maxFrictionTorque = friction;

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

	hkRigidBody* bodyA;
	hkRigidBody* bodyB;
	if( xtream.GetVersion() < 2000 )
	{
		bodyA = hk_body_ref;
		bodyB = hk_body_att;
		constraintData->setInBodySpace( pos_ref, pos_att, plane_ref, plane_att, twist_ref, twist_att ); 
	}
	else
	{
		bodyA = hk_body_att;
		bodyB = hk_body_ref;
		constraintData->setInBodySpace( pos_att, pos_ref, plane_att, plane_ref, twist_att, twist_ref ); 
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

		m_constraint = new hkConstraintInstance( bodyA, bodyB, breakConstraint );
		breakConstraint->removeReference();
	}
	else
	{
		m_constraint = new hkConstraintInstance( bodyA, bodyB, constraintData );
	}
	constraintData->removeReference();

	if (xtream.GetVersion() >= 1370)
	{
		//priv_hk_ragdoll->setStrength(strength);
		//priv_hk_ragdoll->setTau(tau);
	}

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
