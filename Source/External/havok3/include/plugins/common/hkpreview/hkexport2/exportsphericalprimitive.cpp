/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkexport2/hkexport.h>
#include <hkexport2/exportsphericalprimitive.h>
#include <hkexport2/exportrigidbody.h>
#include <hkcollide/shape/sphere/hkSphereShape.h>

using namespace NS_ExportStream;

// FACTORY ----------------------------------------------------------------------------------------------
ExportPrimitive *ExportSphericalPrimitiveFactory::CreateExportPrimitive(Token &token, ExportRigidBody *rigid_body, CheckLog &log)
{
	if (token=="SphericalPrimitive") return new ExportSphericalPrimitive(rigid_body);
	return HK_NULL;
}

EXPORT_SINGLETON_IMPLEMENTATION(ExportSphericalPrimitiveFactory);
// ------------------------------------------------------------------------------------------------------

int ExportSphericalPrimitive::priv_CLASS_SerialNumber=1;

// Constructor
ExportSphericalPrimitive::ExportSphericalPrimitive (ExportRigidBody *rigid_body): ExportPrimitive(rigid_body)
{
	PROT_AutoName();
	priv_CLASS_SerialNumber++;
}
	
// Destructor
ExportSphericalPrimitive::~ExportSphericalPrimitive()
{}

// Import & Export
void ExportSphericalPrimitive::Import (InExportStream &xtream, CheckLog &log)
{
	float radius;

	xtream>>prot_buffer; SetName(prot_buffer);
	log.BeginLevel(LOC("SPHERE PRIMITIVE"),prot_buffer);

	CommonPreImport(xtream, log);

	xtream.TokenMustBe("RADIUS"); xtream>>radius;
	prot_shape = new hkSphereShape(radius);
	
//XXNAMES	// We set the name for this object for further serialization
//XXNAMES	prot_shape->setName(GetSerializableName().cString());

	CommonPostImport(xtream, log);

	xtream.TokenMustBe("END_PRIMITIVE");
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
