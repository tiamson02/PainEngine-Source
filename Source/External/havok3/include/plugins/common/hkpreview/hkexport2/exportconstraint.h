/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef INC_EXPORTCONSTRAINT
#define INC_EXPORTCONSTRAINT

#include <hkexport2/exportclass.h>
#include <hkdynamics/constraint/breakable/hkBreakableConstraintData.h>

class ExportFastConstraintSolver;

//*** Abstract Class ***//
class ExportConstraint: public ExportClass
{
public:
	// Constructor
	ExportConstraint(ExportFastConstraintSolver *constraint_solver);
	// Destructor
	~ExportConstraint();

	virtual void Import(NS_ExportStream::InExportStream &xtream, CheckLog &log)=0;

	virtual hkConstraintInstance *GetHKConstraint() const = 0;

	// Owner Retrieving
	ExportFastConstraintSolver *GetConstraintSolver() const {return prot_constraint_solver;}

protected:
	ExportFastConstraintSolver *prot_constraint_solver;		// Referenced;
};

typedef hkArray<ExportConstraint *> ExportConstraint_Vector;
typedef ExportConstraint_Vector::const_iterator ExportConstraint_VectorIterator;

#endif // INC_EXPORTCONSTRAINT

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
