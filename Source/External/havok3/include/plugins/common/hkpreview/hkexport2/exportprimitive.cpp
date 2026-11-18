/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkexport2/hkexport.h>
#include <hkexport2/exportworld.h>

#include <hkexport2/exportprimitive.h>
#include <hkexport2/exportrigidbody.h>
#include <hkcollide/shape/hkShape.h>
using namespace NS_ExportStream;

// Constructor
ExportPrimitive::ExportPrimitive(ExportRigidBody *rigid_body) : ExportClass()
{
	prot_rigid_body=rigid_body;
	prot_collision_mask = 0;
}

// Destructor
ExportPrimitive::~ExportPrimitive() 
{
	if (prot_shape)
	{
		prot_shape->removeReference();
	}
}

// Common import/export
void ExportPrimitive::CommonPreImport(InExportStream &xtream, CheckLog &log)
{
	hkVector4 translation;
	hkQuaternion rotation;

	const hkConvertCS& converter = GetRigidBody()->GetRigidBodyCollection()->GetSubspace()->GetWorld()->getCSConverter();

	xtream.TokenMustBe(("MASS")); xtream>>prot_mass;

	xtream.TokenMustBe(("ROTATION")); xtream>>rotation;
	converter.convertQuaternion(rotation);

	xtream.TokenMustBe(("TRANSLATION")); xtream>>translation;
	converter.convertVector(translation);

	if (xtream.GetVersion() >= 1350)
	{
		xtream.TokenMustBe(("COLLISION_MASK")); xtream>>prot_collision_mask;
	}
	prot_transform=hkTransform(rotation,translation);
}

void ExportPrimitive::CommonPostImport(InExportStream &xtream, CheckLog &log)
{
	xtream.TokenMustBe(("COLLISIONS_DISABLED")); xtream>>prot_collisions_disabled;	

//XXNAMES	// We set the name for this object for further serialization
//XXNAMES	prot_shape->setName(GetSerializableName().cString());

}

hkString ExportPrimitive::GetSerializableName()
{
	hkString name = ExportClass::GetSerializableName();
	name += "_SHAPE";

	return name;
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
