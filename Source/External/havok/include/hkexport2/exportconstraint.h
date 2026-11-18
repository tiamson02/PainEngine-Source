/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef INC_EXPORTCONSTRAINT
#define INC_EXPORTCONSTRAINT

#include <hkexport2/exportclass.h>
#include <hkdynamics2/constraint/breakable/hkBreakableConstraint.h>

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

	virtual hkConstraint *GetHKConstraint() const =0;
	virtual hkBreakableConstraint* GetHKBreakableConstraint() const { return prot_breakable_constraint;}
	//adds a reference to the constraint passed in, which is removed on deletion
	virtual void SetHKBreakableConstraint( hkBreakableConstraint* new_breakableConstraint);
	virtual bool GetIsBreakable() const;

	// Owner Retrieving
	ExportFastConstraintSolver *GetConstraintSolver() const {return prot_constraint_solver;}

	bool IsLimitedPointToPoint;

protected:
	ExportFastConstraintSolver *prot_constraint_solver;		// Referenced;
	bool prot_is_breakable;
	hkBreakableConstraint* prot_breakable_constraint;
};

typedef hkArray<ExportConstraint *> ExportConstraint_Vector;
typedef ExportConstraint_Vector::const_iterator ExportConstraint_VectorIterator;

#endif // INC_EXPORTCONSTRAINT

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
