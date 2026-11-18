/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTLIMITEDPOINTTOPOINTCONSTRAINT
#define INC_EXPORTLIMITEDPOINTTOPOINTCONSTRAINT

#include <hkexport2/exportconstraint.h>
#include <hkexport2/exportconstraintfactory.h>
#include <hkexport2/exportrigidbody.h>
#include <hkdynamics2/constraint/bilateral/ballandsocket/hkBallAndSocketConstraint.h>
#include <hkdynamics2/constraint/constraintkit/hkConstraintConstructionKit.h>

class ExportLimitedPointToPointConstraint: public ExportConstraint
{
public:
	// Constructor
	ExportLimitedPointToPointConstraint (ExportFastConstraintSolver *constraint_solver);
	// Destructor
	~ExportLimitedPointToPointConstraint();

	// Overrides from ExportClass
	char *ClassName() {return "PointToPoint";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	// Import
	void Import(NS_ExportStream::InExportStream &xtream, CheckLog& log);

	// Access to RigidBodyA
	ExportRigidBody *GetRigidBodyA() const {return priv_rigid_body_a;}
	void SetRigidBodyA (ExportRigidBody *rigid_body) {priv_rigid_body_a=rigid_body;}
	// Access to RigidBodyB
	ExportRigidBody *GetRigidBodyB() const {return priv_rigid_body_b;}
	void SetRigidBodyB (ExportRigidBody *rigid_body) {priv_rigid_body_b=rigid_body;}

	// Override from ExportConstraint
	hkConstraint *GetHKConstraint() const {return GetHKPointToPoint();}
	// Access to hkGenericConstraint
	hkGenericConstraint *GetHKPointToPoint()  const {return priv_constraint;}
	// Access to index of generic constraint parameter for each value, these are set to -1 on construction
	hkGenericConstraint::hkParameterIndex GetPivotAParameterIndex() const { return priv_pivot_a_index;}
	hkGenericConstraint::hkParameterIndex GetPivotBParameterIndex() const { return priv_pivot_b_index;}
	hkGenericConstraint::hkParameterIndex GetRotationAParameterIndex() const { return priv_rotation_a_index;}
	hkGenericConstraint::hkParameterIndex GetRotationBParameterIndex() const { return priv_rotation_b_index;}
	hkGenericConstraint::hkParameterIndex GetXRotationLimitParameterIndex() const { return priv_x_rotation_limit_index;}
	hkGenericConstraint::hkParameterIndex GetYRotationLimitParameterIndex() const { return priv_y_rotation_limit_index;}
	hkGenericConstraint::hkParameterIndex GetZRotationLimitParameterIndex() const { return priv_z_rotation_limit_index;}

private:
	ExportRigidBody *priv_rigid_body_a;			// Referenced
	ExportRigidBody *priv_rigid_body_b;
	hkBallAndSocketConstraint *priv_hk_point_to_point;
	hkGenericConstraint* priv_constraint; 
	hkGenericConstraint::hkParameterIndex priv_pivot_a_index;
	hkGenericConstraint::hkParameterIndex priv_pivot_b_index;
	hkGenericConstraint::hkParameterIndex priv_rotation_a_index;
	hkGenericConstraint::hkParameterIndex priv_rotation_b_index;
	hkGenericConstraint::hkParameterIndex priv_x_rotation_limit_index;
	hkGenericConstraint::hkParameterIndex priv_y_rotation_limit_index;
	hkGenericConstraint::hkParameterIndex priv_z_rotation_limit_index;


	// Class (static) member
	static int priv_CLASS_SerialNumber;
};

// FACTORY ----------------------------------------------------------------------------------------------
class ExportLimitedPointToPointConstraintFactory: public ExportConstraintFactory, public ExportSingleton<ExportLimitedPointToPointConstraintFactory>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	//OverRide
	ExportConstraint *CreateExportConstraint(NS_ExportStream::Token &token, ExportFastConstraintSolver *constraint_solver, CheckLog& log);
	ExportLimitedPointToPointConstraintFactory() { } //should really only be called once
};

typedef hkArray<ExportLimitedPointToPointConstraint *> ExportLimitedPointToPointConstraint_Vector;
typedef ExportLimitedPointToPointConstraint_Vector::iterator ExportLimitedPointToPointConstraint_VectorIterator;

#endif //INC_EXPORTLIMITEDPOINTTOPOINTCONSTRAINT
 

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
