/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef INC_EXPORTCONSTRAINTFACTORY
#define INC_EXPORTCONSTRAINTFACTORY

#include <hkexport2/exportstream.h>

class ExportConstraint;
class ExportFastConstraintSolver;

class ExportConstraintFactory : public hkBaseObject
{
public:
	
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)

	virtual ExportConstraint *CreateExportConstraint(NS_ExportStream::Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log)=0;
};

typedef hkArray<ExportConstraintFactory *> ExportConstraintFactory_Vector;
typedef ExportConstraintFactory_Vector::iterator ExportConstraintFactory_VectorIterator;

class ExportConstraintRegistry : public ExportSingleton<ExportConstraintRegistry>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)

	ExportConstraint *CreateExportConstraint(NS_ExportStream::Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log);
	void AddFactory(ExportConstraintFactory *new_factory) {priv_factories.pushBack(new_factory);}

	ExportConstraintRegistry();	// private constructor as indicated in ExportSingleton.h
	ExportConstraintFactory_Vector priv_factories; // NOT OWNED, just referenced (they are not destroyed)
};

#endif //INC_EXPORTCONSTRAINTFACTORY

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
