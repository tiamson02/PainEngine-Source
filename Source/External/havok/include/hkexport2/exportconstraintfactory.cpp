/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */



#include <hkexport2/hkexport.h>

#include <hkexport2/exportconstraintfactory.h>
#include <hkexport2/exportptopconstraint.h>
#include <hkexport2/exportragdollconstraint.h>
#include <hkexport2/exporthingeconstraint.h>
#include <hkexport2/exportwheelconstraint.h>
#include <hkexport2/exportlimitedptopconstraint.h>
#include <hkexport2/exportstiffspringconstraint.h>
#include <hkexport2/exportprismaticconstraint.h>

#include <hkexport2/exportfastconstraintsolver.h>

using namespace NS_ExportStream;

EXPORT_SINGLETON_IMPLEMENTATION(ExportConstraintRegistry);

ExportConstraintRegistry::ExportConstraintRegistry()
{
	// Default factories
	AddFactory(&ExportPointToPointConstraintFactory::getInstance());
	AddFactory(&ExportRagdollConstraintFactory::getInstance());
	AddFactory(&ExportHingeConstraintFactory::getInstance());
	AddFactory(&ExportWheelConstraintFactory::getInstance());
	AddFactory(&ExportLimitedPointToPointConstraintFactory::getInstance());
	AddFactory(&ExportStiffSpringConstraintFactory::getInstance());
	AddFactory(&ExportPrismaticConstraintFactory::getInstance());
}

ExportConstraint *ExportConstraintRegistry::CreateExportConstraint(Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log)
{
	ExportConstraintFactory_VectorIterator f_it;
	ExportConstraint *result;

	for ( f_it = priv_factories.begin(); f_it<priv_factories.end(); f_it++)
	{
		if ( ( result = ( *f_it )->CreateExportConstraint( token, constraint_solver, log ) ) != HK_NULL )
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
