/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>

#include <hklocalize/hklocalize.h>
#include <hkexport2/exportwheelconstraint.h>
#include <hkexport2/exportfastconstraintsolver.h>
#include <hkexport2/exportsubspace.h>
#include <hkexport2/exportworld.h>

using namespace NS_ExportStream;

// FACTORY ----------------------------------------------------------------------------------------------
EXPORT_SINGLETON_IMPLEMENTATION(ExportWheelConstraintFactory);

ExportConstraint* ExportWheelConstraintFactory::CreateExportConstraint(Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log)
{
	if (token=="Carwheel") return new ExportWheelConstraint(constraint_solver);
	return HK_NULL;
}
// ------------------------------------------------------------------------------------------------------

int ExportWheelConstraint::priv_CLASS_SerialNumber=1;

// Constructor
ExportWheelConstraint::ExportWheelConstraint (ExportFastConstraintSolver *constraint_solver) 
: ExportConstraint (constraint_solver)
{
	PROT_AutoName();
	priv_hk_Wheel=HK_NULL;
	priv_rigid_body_chassis = HK_NULL;
	priv_rigid_body_wheel = HK_NULL;
}

// Destructor
ExportWheelConstraint::~ExportWheelConstraint() 
{
	if (priv_hk_Wheel)
	{
		priv_hk_Wheel->removeReference();
		priv_hk_Wheel = HK_NULL;
	}
}

// Import
void ExportWheelConstraint::Import(InExportStream &xtream, CheckLog& log)
{
	float strength = 0.0f;
	float tau = 0.0f;
	float suspension_min,suspension_max,suspension_friction;
	float spin_velocity, spin_gain, spin_friction;
	float linear_strength, angular_strength;
	bool has_two_bodies = true;
	bool is_breakable = false;

	hkRigidBody* hk_wheel_body = HK_NULL;
	hkRigidBody* hk_chassis_body = HK_NULL;
	hkVector4 world_constraint_pos, world_suspension_axis, world_spin_axis;
	hkVector4 local_pos_chassis, local_pos_wheel;
	hkVector4 local_hinge_chassis, local_hinge_wheel;
	hkVector4 local_suspension_chassis;
	hkVector4 local_steering_chassis;

	xtream>>prot_buffer; SetName(prot_buffer);
	log.BeginLevel(LOC("Wheel CONSTRAINT"),prot_buffer);

	if (xtream.GetVersion()>=1330)
	{
		xtream.TokenMustBe("TWO_BODIED"); xtream>>has_two_bodies;
	}
	
	if (has_two_bodies)
	{
		xtream.TokenMustBe("RIGID_BODY_CHASSIS"); xtream>>prot_buffer;
		priv_rigid_body_chassis=prot_constraint_solver->GetSubspace()->GetRigidBody(prot_buffer);
		if (priv_rigid_body_chassis)
		{
			hk_chassis_body = priv_rigid_body_chassis->GetHKRigidBody();
		}
		if ((!priv_rigid_body_chassis)||(!hk_chassis_body))
		{
			char buffer[200];
			hkString::sprintf(buffer, LOC("Unknown Chassis Rigidbody in Wheel Constraint: %s"), GetName());
			PROT_WarningAndRecover( buffer, xtream, "END_CONSTRAINT", log, false );
			return;
		}
	}
	else
	{
		hk_chassis_body = GetConstraintSolver()->GetSubspace()->GetWorld()->GetHKWorld()->getFixedRigidBody();
	}

	xtream.TokenMustBe("RIGID_BODY_WHEEL"); xtream>>prot_buffer;
	priv_rigid_body_wheel=prot_constraint_solver->GetSubspace()->GetRigidBody(prot_buffer);
	if (priv_rigid_body_wheel)
	{
		hk_wheel_body = priv_rigid_body_wheel->GetHKRigidBody();
	}
	if ((!priv_rigid_body_wheel)||(!hk_wheel_body))
	{
		char buffer[200];
		hkString::sprintf(buffer, LOC("Unknown Wheel Rigidbody in Wheel Constraint: %s"), GetName());
		PROT_WarningAndRecover( buffer, xtream, "END_CONSTRAINT", log, false );
		return;
	}

	if (xtream.GetVersion()>=1370)
	{
		xtream.TokenMustBe("POINT_IN_CHASSIS"); xtream>>local_pos_chassis;
		xtream.TokenMustBe("POINT_IN_WHEEL"); xtream>>local_pos_wheel;
		xtream.TokenMustBe("HINGE_IN_CHASSIS"); xtream>>local_hinge_chassis;
		xtream.TokenMustBe("HINGE_IN_WHEEL"); xtream>>local_hinge_wheel;
		xtream.TokenMustBe("STEERING_IN_CHASSIS"); xtream>>local_steering_chassis;
		xtream.TokenMustBe("SUSPENSION_IN_CHASSIS"); xtream>>local_suspension_chassis;
		xtream.TokenMustBe("IS_BREAKABLE"); xtream>>is_breakable;
		if (is_breakable)
		{
			xtream.TokenMustBe("LINEAR_STRENGTH"); xtream>>linear_strength;
			xtream.TokenMustBe("ANGULAR_STRENGTH"); xtream>>angular_strength;
		}
		xtream.TokenMustBe("STRENGTH"); xtream>>strength;
		xtream.TokenMustBe("TAU"); xtream>>tau;
//		priv_hk_Wheel->setStrength(strength);
//		priv_hk_Wheel->setTau(tau);
	}
	else
	{
		xtream.TokenMustBe("WORLD_CONSTRAINT_POS"); xtream>>world_constraint_pos;
		xtream.TokenMustBe("WORLD_SPIN_AXIS"); xtream>>world_spin_axis;
		xtream.TokenMustBe("WORLD_SUSPENSION_AXIS"); xtream>>world_suspension_axis;
	}

	xtream.TokenMustBe("SUSPENSION_MIN"); xtream>>suspension_min;
	xtream.TokenMustBe("SUSPENSION_MAX"); xtream>>suspension_max;
	xtream.TokenMustBe("SUSPENSION_FRICTION"); xtream>>suspension_friction;

	xtream.TokenMustBe("SPIN_VELOCITY"); xtream>>spin_velocity;
	xtream.TokenMustBe("SPIN_GAIN"); xtream>>spin_gain;
	xtream.TokenMustBe("SPIN_FRICTION"); xtream>>spin_friction;
	
	//bring to havok2 space if necessary: see exportrigidbody.cpp
	local_pos_wheel.setTransformedPos( priv_rigid_body_wheel->GetHavok2FromHavok1BodyTransform(), local_pos_wheel);
	local_hinge_wheel.setRotatedDir( priv_rigid_body_wheel->GetHavok2FromHavok1BodyTransform().getRotation(), local_hinge_wheel);
	if ( priv_rigid_body_chassis )
	{
		local_pos_chassis.setTransformedPos( priv_rigid_body_chassis->GetHavok2FromHavok1BodyTransform(), local_pos_chassis);
		local_hinge_chassis.setRotatedDir( priv_rigid_body_chassis->GetHavok2FromHavok1BodyTransform().getRotation(), local_hinge_chassis);
		local_suspension_chassis.setRotatedDir( priv_rigid_body_chassis->GetHavok2FromHavok1BodyTransform().getRotation(), local_suspension_chassis);
		local_steering_chassis.setRotatedDir( priv_rigid_body_chassis->GetHavok2FromHavok1BodyTransform().getRotation(), local_steering_chassis);
	}

	hkWheelConstructionInfo wheel_construct_info;
	wheel_construct_info.setInBodySpace( hk_wheel_body, hk_chassis_body, local_pos_wheel, local_pos_chassis, local_hinge_wheel,
		local_hinge_chassis, local_suspension_chassis, local_steering_chassis);

	wheel_construct_info.setMinSuspensionLimit( suspension_min );
	wheel_construct_info.setMaxSuspensionLimit( suspension_max );

	priv_hk_Wheel = new hkWheelConstraint( wheel_construct_info );

	if (is_breakable)
	{
		char buffer[200];
		hkString::sprintf( buffer, LOC("Havok 2 breakable constraint has only one tolerance value, this will be taken from the linear strength parameter: %f."), linear_strength);
		log.AddError( buffer );
		hkWorld* world = GetConstraintSolver()->GetSubspace()->GetWorld()->GetHKWorld();
		SetHKBreakableConstraint( new hkBreakableConstraint( priv_hk_Wheel, world ) );
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
