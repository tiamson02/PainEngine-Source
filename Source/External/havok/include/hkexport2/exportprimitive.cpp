/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkexport2/hkexport.h>

#include <hkexport2/exportprimitive.h>
#include <hkexport2/exportrigidbody.h>

using namespace NS_ExportStream;

// Constructor
ExportPrimitive::ExportPrimitive(ExportRigidBody *rigid_body) : ExportClass()
{
	prot_rigid_body=rigid_body;
	prot_collision_mask = 0;
	source_shape = 0;
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


	xtream.TokenMustBe(("MASS")); xtream>>prot_mass;
	xtream.TokenMustBe(("ROTATION")); xtream>>rotation;
	xtream.TokenMustBe(("TRANSLATION")); xtream>>translation;
	if (xtream.GetVersion() >= 1350)
	{
		xtream.TokenMustBe(("COLLISION_MASK")); xtream>>prot_collision_mask;
	}
	prot_transform=hkTransform(rotation,translation);
}

void ExportPrimitive::CommonPostImport(InExportStream &xtream, CheckLog &log)
{
	xtream.TokenMustBe(("COLLISIONS_DISABLED")); xtream>>prot_collisions_disabled;	
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
