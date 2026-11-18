/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>

#include <hkexport2/exportwheelconstraint.h>
#include <hkexport2/exportfastconstraintsolver.h>
#include <hkexport2/exportsubspace.h>
#include <hkexport2/exportworld.h>
#include <hkdynamics/constraint/bilateral/wheel/hkWheelConstraintData.h>
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
	m_constraint=HK_NULL;
	priv_rigid_body_chassis = HK_NULL;
	priv_rigid_body_wheel = HK_NULL;
}

// Destructor
ExportWheelConstraint::~ExportWheelConstraint() 
{
	if (m_constraint)
	{
		m_constraint->removeReference();
		m_constraint = HK_NULL;
	}
}

// Import
void ExportWheelConstraint::Import(InExportStream &xtream, CheckLog& log)
{
	float strength = 0.0f;
	float tau = 0.0f;
	float suspension_min,suspension_max,suspension_friction;
	float spin_velocity, spin_gain, spin_friction;
	float linear_strength = 0;
	float angular_strength = 0;
	hkBool has_two_bodies = true;
	hkBool is_breakable = false;

	hkRigidBody* hk_wheel_body = HK_NULL;
	hkRigidBody* hk_chassis_body = HK_NULL;
	hkVector4 world_constraint_pos, world_suspension_axis, world_spin_axis;
	hkVector4 local_pos_chassis, local_pos_wheel;
	hkVector4 local_hinge_chassis, local_hinge_wheel;
	hkVector4 local_suspension_chassis;
	hkVector4 local_steering_chassis;

	const hkConvertCS& converter = GetConstraintSolver()->GetSubspace()->GetWorld()->getCSConverter();

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
			char buffer[EXPORT_BUFFER_SIZE];
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
		char buffer[EXPORT_BUFFER_SIZE];
		hkString::sprintf(buffer, LOC("Unknown Wheel Rigidbody in Wheel Constraint: %s"), GetName());
		PROT_WarningAndRecover( buffer, xtream, "END_CONSTRAINT", log, false );
		return;
	}

	if (xtream.GetVersion()>=1370)
	{
		xtream.TokenMustBe("POINT_IN_CHASSIS"); xtream>>local_pos_chassis;
		converter.convertVector(local_pos_chassis);

		xtream.TokenMustBe("POINT_IN_WHEEL"); xtream>>local_pos_wheel;
		converter.convertVector(local_pos_wheel);

		xtream.TokenMustBe("HINGE_IN_CHASSIS"); xtream>>local_hinge_chassis;
		converter.convertVector(local_hinge_chassis);

		xtream.TokenMustBe("HINGE_IN_WHEEL"); xtream>>local_hinge_wheel;
		converter.convertVector(local_hinge_wheel);

		xtream.TokenMustBe("STEERING_IN_CHASSIS"); xtream>>local_steering_chassis;
		converter.convertVector(local_steering_chassis);

		xtream.TokenMustBe("SUSPENSION_IN_CHASSIS"); xtream>>local_suspension_chassis;
		converter.convertVector(local_suspension_chassis);

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
		xtream.TokenMustBe("WORLD_CONSTRAINT_POS"); xtream>>world_constraint_pos;
		xtream.TokenMustBe("WORLD_SPIN_AXIS"); xtream>>world_spin_axis;
		xtream.TokenMustBe("WORLD_SUSPENSION_AXIS"); xtream>>world_suspension_axis;
	}

	xtream.TokenMustBe("SUSPENSION_MIN"); xtream>>suspension_min;
	xtream.TokenMustBe("SUSPENSION_MAX"); xtream>>suspension_max;
	xtream.TokenMustBe("SUSPENSION_FRICTION"); xtream>>suspension_friction;

	// Spin parameters are ignored
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

	hkWheelConstraintData* constraintData = new hkWheelConstraintData();
	constraintData->setInBodySpace( local_pos_wheel, local_pos_chassis, local_hinge_wheel,
		local_hinge_chassis, local_suspension_chassis, local_steering_chassis);
	constraintData->m_suspensionMinLimit = suspension_min;
	constraintData->m_suspensionMaxLimit = suspension_max;

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

		m_constraint = new hkConstraintInstance( hk_wheel_body, hk_chassis_body, breakConstraint );
		breakConstraint->removeReference();
	}
	else
	{
		m_constraint = new hkConstraintInstance( hk_wheel_body, hk_chassis_body, constraintData );
	}
	constraintData->removeReference(); // should be held by the constraint now.

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
