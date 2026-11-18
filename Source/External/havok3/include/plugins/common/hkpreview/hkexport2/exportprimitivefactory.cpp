/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>

#include <hkexport2/exportprimitivefactory.h>
#include <hkexport2/exportgeometricprimitive.h>
#include <hkexport2/exportsphericalprimitive.h>

using namespace NS_ExportStream;

EXPORT_SINGLETON_IMPLEMENTATION(ExportPrimitiveRegistry);
EXPORT_SINGLETON_IMPLEMENTATION(ExportUnsupportedPrimitiveFactory);

ExportPrimitive *ExportUnsupportedPrimitiveFactory::CreateExportPrimitive(Token &token, ExportRigidBody *rigid_body, CheckLog &log)
{
	if (token=="PlanarPrimitive")
	{
		log.AddError(LOC("Planar primitives not supported in Havok 2, primitive not imported."));
	}
	return HK_NULL;
}

ExportPrimitiveRegistry::ExportPrimitiveRegistry()
{
	// Default factories
	AddFactory(&ExportGeometricPrimitiveFactory::getInstance());
	AddFactory(&ExportSphericalPrimitiveFactory::getInstance());
	AddFactory(&ExportUnsupportedPrimitiveFactory::getInstance());
}

ExportPrimitive *ExportPrimitiveRegistry::CreateExportPrimitive(Token &token, ExportRigidBody *rigid_body, CheckLog &log)
{
	ExportPrimitiveFactory_VectorIterator f_it;
	ExportPrimitive *result;

	for (f_it=priv_factories.begin(); f_it<priv_factories.end(); f_it++)
	{
		if ((result=(*f_it)->CreateExportPrimitive(token,rigid_body, log))!=HK_NULL)
		{
			return result;
		}
	}
	return HK_NULL;
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
