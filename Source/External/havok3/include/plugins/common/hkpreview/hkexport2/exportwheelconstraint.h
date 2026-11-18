/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTWHEELCONSTRAINT
#define INC_EXPORTWHEELCONSTRAINT

#include <hkexport2/exportconstraint.h>
#include <hkexport2/exportconstraintfactory.h>
#include <hkexport2/exportrigidbody.h>
#include <hkdynamics/constraint/bilateral/wheel/hkWheelConstraintData.h>

class ExportWheelConstraint: public ExportConstraint
{
public:
	// Constructor
	ExportWheelConstraint (ExportFastConstraintSolver *constraint_solver);
	// Destructor
	~ExportWheelConstraint();

	// Overrides from ExportClass
	const char* ClassName() {return "Wheel";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	// Import
	void Import(NS_ExportStream::InExportStream &xtream, CheckLog& log);

	// Access to RigidBodyA
	ExportRigidBody *GetRigidBodyA() const {return priv_rigid_body_chassis;}
	void SetRigidBodyA (ExportRigidBody *rigid_body) {priv_rigid_body_chassis=rigid_body;}
	// Access to RigidBodyB
	ExportRigidBody *GetRigidBodyB() const {return priv_rigid_body_wheel;}
	void SetRigidBodyB (ExportRigidBody *rigid_body) {priv_rigid_body_wheel=rigid_body;}

	// Access to hkWheel
	hkConstraintInstance *GethkWheel() const {return m_constraint;}

	// Override from ExportConstraint
	hkConstraintInstance *GetHKConstraint() const {return GethkWheel();}

private:
	ExportRigidBody *priv_rigid_body_chassis;			// Referenced
	ExportRigidBody *priv_rigid_body_wheel;
	hkConstraintInstance *m_constraint;
	// Class (static) member
	static int priv_CLASS_SerialNumber;
};

// FACTORY ----------------------------------------------------------------------------------------------
class ExportWheelConstraintFactory: public ExportConstraintFactory, public ExportSingleton<ExportWheelConstraintFactory>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(HK_MEMORY_CLASS_EXPORT);
	//OverRide
	ExportConstraint *CreateExportConstraint(NS_ExportStream::Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log);
	ExportWheelConstraintFactory() { } //should really only be called once
};

typedef hkArray<ExportWheelConstraint *> ExportWheelConstraint_Vector;
typedef ExportWheelConstraint_Vector::iterator ExportWheelConstraint_VectorIterator;

#endif //INC_EXPORTWHEELCONSTRAINT


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
