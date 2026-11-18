/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_CONSTRAINT_CONSTRUCTION_KIT_H
#define HK_DYNAMICS2_CONSTRAINT_CONSTRUCTION_KIT_H

#include <hkdynamics2/constraint/constraintkit/hkGenericConstraint.h>
#include <hkdynamics2/constraint/constraintkit/hkGenericConstraintParameters.h>
#include <hkdynamics2/constraint/constraintkit/hkConstraintModifier.h>

class hkConstraintModifier;
class hkMotorController;

/// The hkConstraintConstructionKit allows you to create your own custom Havok constraints.
class hkConstraintConstructionKit
{
	public:
		
			/// Sets up the construction kit with the specified generic constraint.
		void begin( hkGenericConstraint* constraint );

		//
		// linear constraint
		//

			/// Specifies a linear degree of freedom attached to body A, and assigns it to an axis
			/// the ID is used to refer to the axis later.  e.g. constrainLinearDof( id )
		hkGenericConstraint::hkParameterIndex 
			setLinearDofA( const hkVector4& dofInA, int linearAxisId );

			/// Specifies a linear degree of freedom attached to body B, and assigns it to an axis. 
			/// the ID is used to refer to the axis later.  e.g. constrainLinearDof( id )
		hkGenericConstraint::hkParameterIndex 
			setLinearDofB( const hkVector4& dof, int linearAxisId );

			/// Specifies a linear degree of freedom attached to the world, and assigns it to an axis.
			/// the ID is used to refer to the axis later.  e.g. constrainLinearDof( id )
		hkGenericConstraint::hkParameterIndex 
			setLinearDofWorld( const hkVector4& dofInWorld, int linearAxisId );

			/// Constrains away linear motion in specified degree of freedom
		void constrainLinearDof( int linearAxisId );

			/// Constrains away all linear degrees of freedom.
		void constrainAllLinearDof();


		//
		// angular constraint
		//

			/// Specifies a basis for the angular degrees of freedom, in body A space.
		hkGenericConstraint::hkParameterIndex
			setAngularBasisA( const hkMatrix3& dofBasisInA );

			/// Specified a basis for the angular degrees of freedom, in body B space.
		hkGenericConstraint::hkParameterIndex
			setAngularBasisB( const hkMatrix3& dofBasisInB );	

			/// Sets the angular constraint basis for A to its body frame.
		void setAngularBasisABodyFrame();

			/// Sets the angular constraint basis for B to its body frame.
		void setAngularBasisBBodyFrame();

			/// Constrains objects to only rotate around the axis from the angular bases of A and B.
		void constrainToAngularDof( int angularBasisAxisIndex );

			/// Constrains an object so that it cannot rotate relative to the second object.
		void constrainAllAngularDof();

			/// Sets the pivot point attached to body A from a point in A's body space.
		hkGenericConstraint::hkParameterIndex
			setPivotA( const hkVector4& pivotInA );

			/// Sets the pivot point attached to body B from a point in B's body space.
		hkGenericConstraint::hkParameterIndex
			setPivotB( const hkVector4& pivotInB );

			/// Sets the pivots attached to body A and body B from a common world space point.
			/// N.B. This uses the current transforms of body A and B.
		hkGenericConstraint::hkParameterIndex
			setPivotsHelper( const hkVector4& pivotInWorld );

		//
		// limits, friction, motors
		//

			/// Limits linear movement on the specified axis.
		hkGenericConstraint::hkParameterIndex
			setLinearLimit( int linearAxisId, hkReal min, hkReal max );

			/// Limits angular movement around the specified axis.
		hkGenericConstraint::hkParameterIndex
			setAngularLimit( int angularBasisAxisIndex, hkReal min, hkReal max );

			/// Limits angular movement lie to within a cone about an axis.
		hkGenericConstraint::hkParameterIndex
			setConeLimit( int angularBasisAxisIndex, hkReal angle );

			/// Limits angular movement about a twist axis. for best accuracy twist axis should stay in positive half of space
			/// defined by twist axis, plane axis and twist cross plane.  Accuracy degenerates in the negative half.
		hkGenericConstraint::hkParameterIndex
			setTwistLimit( int angularBasisAxisIndexTwist, int angularBasisAxisIndexPlane, hkReal min, hkReal max );

			/// Adds a motor to drive the angle about the specified axis. returns and index into the motor
			/// array of hkGenericConstraintScheme 
		hkGenericConstraint::hkParameterIndex
			setAngularMotor( int angularBasisAxisIndex, hkMotorController* motor );

			/// Adds a motor to drive the specified axis.  returns and index into the motor
			/// array of hkGenericConstraintScheme 
		hkGenericConstraint::hkParameterIndex
			setLinearMotor( int angularBasisAxisIndex, hkMotorController* motor );

			/// Sets friction for angular movement around the specified axis.
		hkGenericConstraint::hkParameterIndex
			setAngularFriction( int angularBasisAxisIndex, hkReal maxImpulse );

			/// Sets friction for linear movement on the specified axis.
		hkGenericConstraint::hkParameterIndex
			setLinearFriction( int linearAxisId, hkReal maxImpulse );

		//
		// tau, damping
		//

			/// Sets the stiffness and damping of the subsequent constraints.  remember to restore it later
		hkGenericConstraint::hkParameterIndex
			setStiffnessAndDamping( hkReal tau, hkReal damping );
		
			/// restore the stiffness and damping back to solver defaults
		void restoreStiffnessAndDamping();

		//
		// modifiers
		//

			/// Adds a callback to hkConstraintModifier::modify to the constraint
		void addConstraintModifierCallback( hkConstraintModifier *cm, int userData );

			/// Adds a callback to hkConstraintModifier::modifyRhs to the constraint
			/// which modifies the Rhs ( error term ) for the previously constrained Degree Of Freedom 
		void addRhsModifierCallback( hkConstraintModifier *cm, int userData );

		//
		// commands
		//

			/// Finish construction.
		void end();


	private:

		hkGenericConstraint* m_constraint;
		hkGenericConstraintScheme* m_scheme;

		int m_stiffnessReference;
		int m_dampingReference;

			// Bools to check all required data is specified, given a constraint is being requested.
		bool	m_linearDofSpecifiedA[3];
		bool	m_linearDofSpecifiedB[3];
		bool	m_angularBasisSpecifiedA;
		bool	m_angularBasisSpecifiedB;
		bool	m_pivotSpecifiedA;
		bool	m_pivotSpecifiedB;
};

#endif

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
