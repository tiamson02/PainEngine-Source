/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTHINGECONSTRAINT
#define INC_EXPORTHINGECONSTRAINT

#include <hkexport2/exportconstraint.h>
#include <hkexport2/exportconstraintfactory.h>
#include <hkexport2/exportrigidbody.h>
#include <hkdynamics2/constraint/bilateral/hinge/hkHingeConstraint.h>
#include <hkdynamics2/constraint/bilateral/limitedhinge/hkLimitedHingeConstraint.h>

class ExportHingeConstraint: public ExportConstraint
{
public:
	// Constructor
	ExportHingeConstraint (ExportFastConstraintSolver *constraint_solver);
	// Destructor
	~ExportHingeConstraint();

	// Overrides from ExportClass
	char *ClassName() {return "Hinge";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	// Import
	void Import(NS_ExportStream::InExportStream &xtream, CheckLog &log);

	// Access to RigidBodyA
	ExportRigidBody *GetRigidBodyA() const {return priv_rigid_body_a;}
	void SetRigidBodyA (ExportRigidBody *rigid_body) {priv_rigid_body_a=rigid_body;}
	// Access to RigidBodyB
	ExportRigidBody *GetRigidBodyB() const {return priv_rigid_body_b;}
	void SetRigidBodyB (ExportRigidBody *rigid_body) {priv_rigid_body_b=rigid_body;}

	bool GetIsLimited() const {return priv_is_limited;}

	// Override from ExportConstraint
	hkConstraint *GetHKConstraint() const;
	// Access to HK::Hinge
	hkHingeConstraint *GetHKHinge() const {return priv_hk_hinge;}
	hkLimitedHingeConstraint *GetHKLimitedHinge() const {return priv_hk_limited_hinge;}



private:
	ExportRigidBody *priv_rigid_body_a;			// Referenced
	ExportRigidBody *priv_rigid_body_b;
	hkHingeConstraint* priv_hk_hinge;
	hkLimitedHingeConstraint* priv_hk_limited_hinge;
	bool priv_is_limited;
	// Class (static) member
	static int priv_CLASS_SerialNumber;
};

// FACTORY ----------------------------------------------------------------------------------------------
class ExportHingeConstraintFactory: public ExportConstraintFactory, public ExportSingleton<ExportHingeConstraintFactory>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	//OverRide
	ExportConstraint *CreateExportConstraint(NS_ExportStream::Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log);
	ExportHingeConstraintFactory() { } //should really only be called once
}; 

typedef hkArray<ExportHingeConstraint *> ExportHingeConstraint_Vector;
typedef ExportHingeConstraint_Vector::iterator ExportHingeConstraint_VectorIterator;

#endif //INC_EXPORTHINGECONSTRAINT
 

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
