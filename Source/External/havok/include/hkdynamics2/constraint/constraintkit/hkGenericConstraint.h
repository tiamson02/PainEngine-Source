/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_GENERIC_CONSTRAINT_H
#define HK_DYNAMICS2_GENERIC_CONSTRAINT_H

#include <hkdynamics2/constraint/hkRigidConstraint.h>
#include <hkdynamics2/constraint/constraintkit/hkGenericConstraintScheme.h>

class hkGenericConstraintParameters;

/// A generic constraint for use with the hkConstraintConstructionKit. A generic constraint
/// initially doesn't restrict any movement for its bodies - it must be configured using the kit.
class hkGenericConstraint : public hkRigidConstraint
{

	public:
			///A parameter index. The constraint construction kit returns an index each time you specify a pivot
			/// point, basis, or axis for the constraint, allowing you to access these later.
		typedef int hkParameterIndex;

			///Creates a new generic constraint.
		hkGenericConstraint( hkRigidBody *bodyA, hkRigidBody *bodyB );

		virtual ~hkGenericConstraint();

			/// Provides access to solver results.
		virtual hkSolverResults* getSolverResults();
			
			/// Provides number of solver results
		virtual int getNumSolverResults() const;

			/// Gets the parameter at the index returned during kit construction
		hkVector4* getParameters( hkParameterIndex parameterIndex );

			/// Sets the parameters starting at the index returned during kit construction
		void setParameters( hkParameterIndex parameterIndex, int numParameters, const hkVector4* newValues ); 

			/// Checks consistency of constraint members.
		virtual bool isValid() const;

		hkGenericConstraintScheme* getScheme();


	protected:

		// commands 

		// linear constraints

		void constrainAllLinearW( hkArray<int>::iterator& currentCommand, hkArray<hkVector4>::iterator& currentData, const hkGenericConstraintScheme& scheme, hkGenericConstraintParameters& vars, const hkConstraintQueryIn &in, hkConstraintQueryOut &out ) const;

		inline void constrainLinearW( hkArray<int>::iterator& currentCommand, hkArray<hkVector4>::iterator& currentData, const hkGenericConstraintScheme& scheme, hkGenericConstraintParameters& vars, const hkConstraintQueryIn &in, hkConstraintQueryOut &out ) const;

		inline void constrainToAngularW( hkArray<int>::iterator& currentCommand, hkArray<hkVector4>::iterator& currentData, const hkGenericConstraintScheme& scheme, hkGenericConstraintParameters& vars, const hkConstraintQueryIn &in, hkConstraintQueryOut &out ) const;

		inline void constrainAllAngularW( hkArray<int>::iterator& currentCommand, hkArray<hkVector4>::iterator& currentData, const hkGenericConstraintScheme& scheme, hkGenericConstraintParameters& vars, const hkConstraintQueryIn &in, hkConstraintQueryOut &out ) const;

		// limits, friction, motors

		inline void setLinearLimitW( hkArray<int>::iterator& currentCommand, hkArray<hkVector4>::iterator& currentData, const hkGenericConstraintScheme& scheme, hkGenericConstraintParameters& vars, const hkConstraintQueryIn &in, hkConstraintQueryOut &out ) const;

		inline void setAngularLimitW( hkArray<int>::iterator& currentCommand, hkArray<hkVector4>::iterator& currentData, const hkGenericConstraintScheme& scheme, hkGenericConstraintParameters& vars, const hkConstraintQueryIn &in, hkConstraintQueryOut &out ) const;

		inline void setConeLimitW( hkArray<int>::iterator& currentCommand, hkArray<hkVector4>::iterator& currentData, const hkGenericConstraintScheme& scheme, hkGenericConstraintParameters& vars, const hkConstraintQueryIn &in, hkConstraintQueryOut &out ) const;

		inline void setTwistLimitW( hkArray<int>::iterator& currentCommand, hkArray<hkVector4>::iterator& currentData, const hkGenericConstraintScheme& scheme, hkGenericConstraintParameters& vars, const hkConstraintQueryIn &in, hkConstraintQueryOut &out ) const;

		inline void setAngularMotorW( hkArray<int>::iterator& currentCommand, hkArray<hkVector4>::iterator& currentData, hkGenericConstraintScheme& scheme, hkGenericConstraintParameters& vars, const hkConstraintQueryIn &in, hkConstraintQueryOut &out ) const;

		inline void setLinearMotorW( hkArray<int>::iterator& currentCommand, hkArray<hkVector4>::iterator& currentData, hkGenericConstraintScheme& scheme, hkGenericConstraintParameters& vars, const hkConstraintQueryIn &in, hkConstraintQueryOut &out ) const;

		inline void setAngularFrictionW( hkArray<int>::iterator& currentCommand, hkArray<hkVector4>::iterator& currentData, hkGenericConstraintScheme& scheme, hkGenericConstraintParameters& vars, const hkConstraintQueryIn &in, hkConstraintQueryOut &out ) const;
			
		void setLinearFrictionW( hkArray<int>::iterator& currentCommand, hkArray<hkVector4>::iterator& currentData, hkGenericConstraintScheme& scheme, hkGenericConstraintParameters& vars, const hkConstraintQueryIn &in, hkConstraintQueryOut &out ) const;
		
		/// end commands

		void hatchScheme( hkGenericConstraintScheme* scheme, const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		hkGenericConstraintScheme m_scheme;

		// Internal functions

		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;

		virtual int getType() const;

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
