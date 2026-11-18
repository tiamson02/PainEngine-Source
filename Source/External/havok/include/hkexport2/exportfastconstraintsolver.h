/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef INC_EXPORTFASTCONSTRAINTSOLVER
#define INC_EXPORTFASTCONSTRAINTSOLVER

#include <hkexport2/exportaction.h>
#include <hkexport2/exportconstraintfactory.h>
#include <hkexport2/exportactionfactory.h>

class ExportSubspace;

class ExportFastConstraintSolver: public ExportAction
{
public:
	// Constructor
	ExportFastConstraintSolver (ExportSubspace *subspace);
	// Destructor
	~ExportFastConstraintSolver();

	hkAction* GetHKAction() const {return HK_NULL;}

	// Overrides from ExportClass
	char *ClassName() {return "FastConstraintSolver";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	// Import
	void Import(NS_ExportStream::InExportStream &xtream, CheckLog &log);

	void AddConstraint(ExportConstraint *constraint) {priv_constraints_vector.pushBack(constraint);}
	int GetNumConstraints() const {return priv_constraints_vector.getSize();}
	ExportConstraint *GetConstraint(char *name) const;
	ExportConstraint *GetConstraint(int index) const {return priv_constraints_vector[index];}

private:
	// class (static) member
	hkArray<ExportConstraint*> priv_constraints_vector;		//owned
	static int priv_CLASS_SerialNumber;
};

class ExportFastConstraintSolverActionFactory: public ExportSingleton<ExportFastConstraintSolverActionFactory>, public ExportActionFactory
{
public: 
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	// OVERRIDE 
	ExportAction *CreateExportAction(NS_ExportStream::Token &token, ExportSubspace *subspace, CheckLog& log);

	ExportFastConstraintSolverActionFactory() { }
};

typedef hkArray<ExportFastConstraintSolver *> ExportFastConstraintSolver_Vector;
typedef ExportFastConstraintSolver_Vector::const_iterator ExportFastConstraintSolver_VectorIterator;

#endif //INC_EXPORTFASTCONSTRAINTSOLVER
 

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
