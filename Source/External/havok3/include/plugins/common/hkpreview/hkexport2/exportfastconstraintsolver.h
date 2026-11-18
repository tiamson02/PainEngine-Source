/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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
	const char* ClassName() {return "FastConstraintSolver";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	// Import
	void Import(NS_ExportStream::InExportStream &xtream, CheckLog &log);

	void AddConstraint(ExportConstraint *constraint) {priv_constraints_vector.pushBack(constraint);}
	int GetNumConstraints() const {return priv_constraints_vector.getSize();}
	ExportConstraint *GetConstraint(const char* name) const;
	ExportConstraint *GetConstraint(int index) const {return priv_constraints_vector[index];}

private:
	// class (static) member
	hkArray<ExportConstraint*> priv_constraints_vector;		//owned
	static int priv_CLASS_SerialNumber;
};

class ExportFastConstraintSolverActionFactory: public ExportSingleton<ExportFastConstraintSolverActionFactory>, public ExportActionFactory
{
public: 
	HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(HK_MEMORY_CLASS_EXPORT);
	// OVERRIDE 
	ExportAction *CreateExportAction(NS_ExportStream::Token &token, ExportSubspace *subspace, CheckLog& log);

	ExportFastConstraintSolverActionFactory() { }
};

typedef hkArray<ExportFastConstraintSolver *> ExportFastConstraintSolver_Vector;
typedef ExportFastConstraintSolver_Vector::const_iterator ExportFastConstraintSolver_VectorIterator;

#endif //INC_EXPORTFASTCONSTRAINTSOLVER
 

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
