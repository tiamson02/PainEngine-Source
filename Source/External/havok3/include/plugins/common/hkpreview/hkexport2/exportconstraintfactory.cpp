/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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
