/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_BREAKABLE_CONSTRAINT_H
#define HK_BREAKABLE_CONSTRAINT_H

#include <hkdynamics2/constraint/hkRigidConstraint.h>

class hkBreakableListener;
class hkWorld;

/// Defintion of the breakable constraint class which is essentially a wrapper around a hkConstraint.
/// The constraint will "break" ie. cease to apply forces/impulse to maintain itself
/// whenever the "threshold" is exceeded. 
/// N.B. Currently, "threshold" is an empirical value
/// based on the magnitude of the force(s) required to maintain the constraint and 
/// must be hand-tweaked, but larger values produce "harder-to-break" constraints.
class hkBreakableConstraint : public hkConstraint
{
	public:

		hkBreakableConstraint(hkConstraint* constraint,hkWorld* world);	
		~hkBreakableConstraint();

			/// Sets whether the constraint is broken or not
		inline void setIsBroken(bool tf);

			/// Set the current threshold.
		inline void setThreshold(hkReal thresh);

			/// Remove Constraint from world when broken
		inline void setRemoveWhenBroken(bool tf);

			/// Call user defined function when constraint breaks
        inline void addBreakableListener(hkBreakableListener* bListener); 
		
			/// Returns whether constraint will be removed when it breaks
		inline bool getRemoveWhenBroken();
		
			/// Returns whether the constraint is in broken state
		inline bool getIsBroken();

			/// Returns the breaking threshold
		inline hkReal getThreshold();

			/// Provides access to solver results.
	    virtual hkSolverResults* getSolverResults();
		
			/// Provides number of solver results
		virtual int getNumSolverResults() const;

			/// Check consistency of constraint members
		virtual bool isValid() const;
		
		virtual int getType() const;

	public:

			/// The wrapped constraint
		hkConstraint* m_constraint;

			/// Pointer to the world to which constraint belongs
		hkWorld* m_world;

			/// The (squared) threshold of breakage
		hkReal m_solverResultLimitSqrd;

			/// Is the constraint in a "broken" state
		bool m_isBroken;
			 
			/// Remove breakable constraint from world when broken? 
		bool  m_removeWhenBroken;

		
		/// Pointer to user specified function that is called 
		/// when the constraint breaks
		hkBreakableListener* m_listener;

		/// Needed to fool solver
		void buildBogusJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

	public:

		/// Internal functions

		/// This is where the constraint is either solved for or broken
		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;
};

#include <hkdynamics2/constraint/breakable/hkBreakableConstraint.inl>

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
