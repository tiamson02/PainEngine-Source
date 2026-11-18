/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>
#include <hkexport2/exportmotoraction.h>
#include <hkexport2/exportsubspace.h>
#include <hkexport2/exportrigidbody.h>
#include <hkexport2/exportworld.h>

using namespace NS_ExportStream;


// FACTORY ----------------------------------------------------------------------------------------------

EXPORT_SINGLETON_IMPLEMENTATION(ExportMotorActionFactory);

ExportAction* ExportMotorActionFactory::CreateExportAction(NS_ExportStream::Token &token,ExportSubspace *subspace,CheckLog& log)
{
	if (token=="Motor") return new ExportMotorAction(subspace);
	return HK_NULL;
}

// ------------------------------------------------------------------------------------------------------

int ExportMotorAction::priv_CLASS_SerialNumber=1;

// Constructor
ExportMotorAction::ExportMotorAction(ExportSubspace *subspace) : ExportAction (subspace)
{
	PROT_AutoName();
	priv_rigid_body = HK_NULL;
	priv_hk_motor_action=HK_NULL;
}

// Destructor
ExportMotorAction::~ExportMotorAction()
{
	if (priv_hk_motor_action)
	{
		priv_hk_motor_action->removeReference();
		priv_hk_motor_action = HK_NULL;
	}
}

void ExportMotorAction::Import(InExportStream &xtream, CheckLog& log)
{
	float ang_speed,gain;
	hkVector4 axis;
	hkRigidBody* r_body = HK_NULL;

	const hkConvertCS& converter = GetSubspace()->GetWorld()->getCSConverter();

	xtream>>prot_buffer; SetName(prot_buffer);
	log.BeginLevel(LOC("MOTOR ACTION"),prot_buffer);

	xtream.TokenMustBe("RIGID_BODY"); xtream>>prot_buffer;
	priv_rigid_body=prot_subspace->GetRigidBody(prot_buffer);
	if (priv_rigid_body)
	{
		r_body = priv_rigid_body->GetHKRigidBody();
	}

	if ((!priv_rigid_body) || (!r_body))
	{
		char buffer[EXPORT_BUFFER_SIZE];
		hkString::sprintf(buffer, LOC("Unknown rigid body in motor: %s"),GetName());
		log.AddError(buffer);
		PROT_WarningAndRecover(buffer,xtream,"END_ACTION", log);
		return;
	}

	xtream.TokenMustBe("AXIS"); xtream>>axis;
	converter.convertVector(axis);

	xtream.TokenMustBe("ANGULAR_SPEED"); xtream>>ang_speed;
	converter.convertRotationAngle(ang_speed);

	xtream.TokenMustBe("GAIN"); xtream>>gain;

	//bring to primitive space if necessary
	axis.setRotatedDir( priv_rigid_body->GetHavok2FromHavok1BodyTransform().getRotation(), axis );

	priv_hk_motor_action = new hkMotorAction(r_body,axis,ang_speed,gain);

	// We set the name for this object for further serialization
	priv_hk_motor_action->setName(GetName());

	xtream.TokenMustBe("END_ACTION");
	log.EndLevel();

	priv_hk_motor_action->setName(GetName());
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
