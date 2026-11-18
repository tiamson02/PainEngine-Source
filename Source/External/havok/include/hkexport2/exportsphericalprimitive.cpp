/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkexport2/hkexport.h>
#include <hkexport2/exportsphericalprimitive.h>
#include <hkexport2/exportrigidbody.h>
#include <hkcollide2/shape/hkSphereShape.h>

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
	
	CommonPostImport(xtream, log);

	xtream.TokenMustBe("END_PRIMITIVE");
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
