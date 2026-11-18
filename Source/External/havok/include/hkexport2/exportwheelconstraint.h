/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
 
#ifndef INC_EXPORTWHEELCONSTRAINT
#define INC_EXPORTWHEELCONSTRAINT

#include <hkexport2/exportconstraint.h>
#include <hkexport2/exportconstraintfactory.h>
#include <hkexport2/exportrigidbody.h>
#include <hkdynamics2/constraint/bilateral/wheel/hkWheelConstraint.h>

class ExportWheelConstraint: public ExportConstraint
{
public:
	// Constructor
	ExportWheelConstraint (ExportFastConstraintSolver *constraint_solver);
	// Destructor
	~ExportWheelConstraint();

	// Overrides from ExportClass
	char *ClassName() {return "Wheel";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	// Import
	void Import(NS_ExportStream::InExportStream &xtream, CheckLog& log);

	// Access to RigidBodyA
	ExportRigidBody *GetRigidBodyA() const {return priv_rigid_body_chassis;}
	void SetRigidBodyA (ExportRigidBody *rigid_body) {priv_rigid_body_chassis=rigid_body;}
	// Access to RigidBodyB
	ExportRigidBody *GetRigidBodyB() const {return priv_rigid_body_wheel;}
	void SetRigidBodyB (ExportRigidBody *rigid_body) {priv_rigid_body_wheel=rigid_body;}

	// Override from ExportConstraint
	hkConstraint *GetHKConstraint() const {return GethkWheel();}
	// Access to hkWheel
	hkWheelConstraint *GethkWheel() const {return priv_hk_Wheel;}

private:
	ExportRigidBody *priv_rigid_body_chassis;			// Referenced
	ExportRigidBody *priv_rigid_body_wheel;
	hkWheelConstraint *priv_hk_Wheel;
	// Class (static) member
	static int priv_CLASS_SerialNumber;
};

// FACTORY ----------------------------------------------------------------------------------------------
class ExportWheelConstraintFactory: public ExportConstraintFactory, public ExportSingleton<ExportWheelConstraintFactory>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	//OverRide
	ExportConstraint *CreateExportConstraint(NS_ExportStream::Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log);
	ExportWheelConstraintFactory() { } //should really only be called once
}; 

typedef hkArray<ExportWheelConstraint *> ExportWheelConstraint_Vector;
typedef ExportWheelConstraint_Vector::iterator ExportWheelConstraint_VectorIterator;

#endif //INC_EXPORTWHEELCONSTRAINT
 

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
