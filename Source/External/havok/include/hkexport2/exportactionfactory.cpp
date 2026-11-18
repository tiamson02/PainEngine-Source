/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>

#include <hkexport2/exportactionfactory.h>
#include <hkexport2/exportspringaction.h>
#include <hkexport2/exportdashpotaction.h>
#include <hkexport2/exportangulardashaction.h>
#include <hkexport2/exportmotoraction.h>
#include <hkexport2/exportfastconstraintsolver.h>

using namespace NS_ExportStream;

EXPORT_SINGLETON_IMPLEMENTATION(ExportActionRegistry);
EXPORT_SINGLETON_IMPLEMENTATION(ExportUnsupportedActionFactory);

ExportAction* ExportUnsupportedActionFactory::CreateExportAction(NS_ExportStream::Token &token, ExportSubspace *subspace, CheckLog& log)
{
	if (token=="SuspensionDashpot")
	{
		log.AddError(LOC("ACTION: Suspension Dashpot not supported in Havok 2 and ignored on import."));
		return HK_NULL;
	}
	if (token=="SpinDashpot")
	{
		log.AddError(LOC("ACTION: Spin Dashpot not supported in Havok 2 and ignored on import."));
		return HK_NULL;
	}
	if (token=="Drag")
	{
		log.AddError(LOC("ACTION: Drag action not supported in Havok 2 and ignored on import."));
		return HK_NULL;
	}
	if (token=="Deactivator")
	{
		log.AddError(LOC("ACTION: Old deactivator not supported in Havok 2 and ignored on import."));
		return HK_NULL;
	}
	if (token=="KeyFrame")
	{
		log.AddError(LOC("ACTION: Keyframes for unyielding bodies not imported."));
		return HK_NULL;
	}
	return HK_NULL;
}


ExportActionRegistry::ExportActionRegistry()
{
	// Default factories
	AddFactory(&ExportSpringActionFactory::getInstance());
	AddFactory(&ExportDashpotActionFactory::getInstance());
	AddFactory(&ExportAngularDashpotActionFactory::getInstance());
	AddFactory(&ExportMotorActionFactory::getInstance());
	AddFactory(&ExportFastConstraintSolverActionFactory::getInstance());
	AddFactory(&ExportUnsupportedActionFactory::getInstance());
}

ExportAction *ExportActionRegistry::CreateExportAction(Token &token, ExportSubspace *subspace,  CheckLog& log)
{
	ExportActionFactory_VectorIterator f_it;
	ExportAction *result;

	for ( f_it = priv_factories.begin(); f_it < priv_factories.end(); f_it++ )
	{
		if ( ( result = (*f_it)->CreateExportAction( token, subspace, log ) ) != HK_NULL )
		{
			return result;
		}
	}
	return (ExportAction *)HK_NULL;
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
