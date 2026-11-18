/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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
