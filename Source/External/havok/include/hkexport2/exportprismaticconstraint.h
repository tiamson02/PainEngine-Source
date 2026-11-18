/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTPRISMATICCONSTRAINT
#define INC_EXPORTPRISMATICCONSTRAINT

#include <hkexport2/exportconstraint.h>
#include <hkexport2/exportconstraintfactory.h>
#include <hkexport2/exportrigidbody.h>
#include <hkdynamics2/constraint/bilateral/prismatic/hkPrismaticConstraint.h>

class ExportPrismaticConstraint: public ExportConstraint
{
public:
	// Constructor
	ExportPrismaticConstraint (ExportFastConstraintSolver *constraint_solver);
	// Destructor
	~ExportPrismaticConstraint();

	// Overrides from ExportClass
	char *ClassName() {return "Prismatic";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	// Import
	void Import(NS_ExportStream::InExportStream &xtream, CheckLog& log);

	// Access to RigidBodyA
	ExportRigidBody *GetRigidBodyReference() const {return priv_rigid_body_ref;}
	void SetRigidBodyReference (ExportRigidBody *rigid_body) {priv_rigid_body_ref=rigid_body;}
	// Access to RigidBodyB
	ExportRigidBody *GetRigidBodyAttached() const {return priv_rigid_body_att;}
	void SetRigidBodyAttached (ExportRigidBody *rigid_body) {priv_rigid_body_att=rigid_body;}

	// Override from ExportConstraint
	hkConstraint *GetHKConstraint() const {return GetHKPrismatic();}
	// Access to hkPrismatic
	hkPrismaticConstraint *GetHKPrismatic() const {return priv_hk_prismatic;}

private:
	ExportRigidBody *priv_rigid_body_ref;			// Referenced
	ExportRigidBody *priv_rigid_body_att;
	hkPrismaticConstraint *priv_hk_prismatic;
	// Class (static) member
	static int priv_CLASS_SerialNumber;
};

// FACTORY ----------------------------------------------------------------------------------------------
class ExportPrismaticConstraintFactory: public ExportConstraintFactory, public ExportSingleton<ExportPrismaticConstraintFactory>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	//OverRide
	ExportConstraint *CreateExportConstraint(NS_ExportStream::Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log);
	ExportPrismaticConstraintFactory() { } //should really only be called once
}; 

typedef hkArray<ExportPrismaticConstraint *> ExportPrismaticConstraint_Vector;
typedef ExportPrismaticConstraint_Vector::iterator ExportPrismaticConstraint_VectorIterator;

#endif //INC_EXPORTPRISMATICCONSTRAINT
 

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
