/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_MALLEABLE_CONSTRAINT_H
#define HK_MALLEABLE_CONSTRAINT_H

#include <hkdynamics2/constraint/hkConstraint.h>
#include <hkdynamics2/world/hkWorld.h>

/// This is a wrapper class around constraints intended to allow
/// the user to make a constraint softer..or harder by modifying
/// the tau and damping values used by the constraint solver.

class hkMalleableConstraint : public hkConstraint
{
	public:

		hkMalleableConstraint(hkConstraint* constraint);	
		~hkMalleableConstraint();

			/// Set the solver tau value
		void setStrength(const hkReal tau);
		
			/// Set the solver damping value
		void setDamping(const hkReal damping);
		
			/// Get the tau value for this constraint
        hkReal getStrength() const;
		
			/// Get the damping value for this constraint
		hkReal getDamping() const; 	

			/// Provides access to solver results.
	    virtual hkSolverResults* getSolverResults();
		
			/// Provides number of solver results
		virtual int getNumSolverResults() const;

			/// Checks consistency of constraint members			
		virtual bool isValid() const;
		
		virtual int getType() const;
		
		virtual void getConstraintInfo( hkConstraintInfo& infoOut ) const;
	
	public:

		/// The wrapped constraint
		hkConstraint* m_constraint;
		hkReal m_tau;
		hkReal m_damping;

	public:

		// Internal functions
		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

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
