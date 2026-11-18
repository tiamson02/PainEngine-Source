/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTRAGDOLLCONSTRAINT
#define INC_EXPORTRAGDOLLCONSTRAINT

#include <hkexport2/exportconstraint.h>
#include <hkexport2/exportconstraintfactory.h>
#include <hkexport2/exportrigidbody.h>
#include <hkinternal/dynamics/constraint/bilateral/ragdoll/hkRagdollConstraintData.h>

class ExportRagdollConstraint: public ExportConstraint
{
public:
	// Constructor
	ExportRagdollConstraint (ExportFastConstraintSolver *constraint_solver);
	// Destructor
	~ExportRagdollConstraint();

	// Overrides from ExportClass
	const char* ClassName() {return "Ragdoll";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	// Import
	void Import(NS_ExportStream::InExportStream &xtream, CheckLog &log);

	// Access to RigidBodyA
	ExportRigidBody *GetRigidBodyReference() const {return priv_rigid_body_ref;}
	void SetRigidBodyReference (ExportRigidBody *rigid_body) {priv_rigid_body_ref=rigid_body;}
	// Access to RigidBodyB
	ExportRigidBody *GetRigidBodyAttached() const {return priv_rigid_body_att;}
	void SetRigidBodyAttached (ExportRigidBody *rigid_body) {priv_rigid_body_att=rigid_body;}

	// Access to hkRagdoll
	hkConstraintInstance *GetHKRagdoll() const {return m_constraint;}

	// Override from ExportConstraint
	hkConstraintInstance *GetHKConstraint() const {return GetHKRagdoll();}

private:
	ExportRigidBody *priv_rigid_body_ref;			// Referenced
	ExportRigidBody *priv_rigid_body_att;
	hkConstraintInstance *m_constraint;
	// Class (static) member
	static int priv_CLASS_SerialNumber;
};

// FACTORY ----------------------------------------------------------------------------------------------
class ExportRagdollConstraintFactory: public ExportConstraintFactory, public ExportSingleton<ExportRagdollConstraintFactory>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(HK_MEMORY_CLASS_EXPORT);
	//OverRide
	ExportConstraint *CreateExportConstraint(NS_ExportStream::Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log);
	ExportRagdollConstraintFactory() { } //should really only be called once
};

typedef hkArray<ExportRagdollConstraint *> ExportRagdollConstraint_Vector;
typedef ExportRagdollConstraint_Vector::iterator ExportRagdollConstraint_VectorIterator;

#endif //INC_EXPORTRAGDOLLCONSTRAINT


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
