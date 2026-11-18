/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef INC_EXPORTCONSTRAINTFACTORY
#define INC_EXPORTCONSTRAINTFACTORY

#include <hkexport2/exportstream.h>

class ExportConstraint;
class ExportFastConstraintSolver;

class ExportConstraintFactory : public hkReferencedObject
{
public:
	
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT);

	virtual ExportConstraint *CreateExportConstraint(NS_ExportStream::Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log)=0;
};

typedef hkArray<ExportConstraintFactory *> ExportConstraintFactory_Vector;
typedef ExportConstraintFactory_Vector::iterator ExportConstraintFactory_VectorIterator;

class ExportConstraintRegistry : public ExportSingleton<ExportConstraintRegistry>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT);

	ExportConstraint *CreateExportConstraint(NS_ExportStream::Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log);
	void AddFactory(ExportConstraintFactory *new_factory) {priv_factories.pushBack(new_factory);}

	ExportConstraintRegistry();	// private constructor as indicated in ExportSingleton.h
	ExportConstraintFactory_Vector priv_factories; // NOT OWNED, just referenced (they are not destroyed)
};

#endif //INC_EXPORTCONSTRAINTFACTORY

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
