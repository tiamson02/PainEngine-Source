/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_DYNAMICS2_POINT_TO_PATH_CONSTRAINT_H
#define HK_DYNAMICS2_POINT_TO_PATH_CONSTRAINT_H

#include <hkdynamics2/constraint/hkRigidConstraint.h>
#include <hkdynamics2/constraint/bilateral/pointtopath/hkPointToPathConstructionInfo.h>

	/// A constraint where one body is free to move along a specified path relative to the other. 
	/// Body A is the attached body, while the path is on body B.
class hkPointToPathConstraint : public hkRigidConstraint
{
	public:  

			/// Initialize constraint with info.
		hkPointToPathConstraint(const hkPointToPathConstructionInfo& info);

			/// Destructor.
		virtual ~hkPointToPathConstraint();

			/// Sets all construction data except the bodies.
		void setData(const hkPointToPathConstructionInfo& info);

			/// Gets the construction info for the constraint.
		inline void getConstructionInfo(hkPointToPathConstructionInfo& info);
		
			/// Set the friction magnitude.
		inline void setMaxFrictionForce(hkReal friction);
		
			/// Get the friction magnitude.
		inline hkReal getMaxFrictionForce();

			/// Provides access to solver results.
	    virtual hkSolverResults* getSolverResults();
		
			/// Provides number of solver results.
		virtual int getNumSolverResults() const;

			/// Check consistency of constraint members.
		virtual bool isValid() const;
		
			/// Get the path.
		inline hkParametricCurve* getPath();

			/// Get the "Constraint" space for the ith body.
			/// getConstraintToLocalTransform(0) returns the Transform of the attached body's 
			/// pivot, in the attached body's Local space.
			/// getConstraintToLocalTransform(1) returns the Transform of the reference body's
			/// path, in the reference body's Local space.	
		inline const hkTransform& getConstraintToLocalTransform(int i) const;


			/// Get type from this constraint.
		virtual int getType() const;

		enum 
		{
			SOLVER_RESULT_FRICTION = 0,	// the friction.
			SOLVER_RESULT_1 = 1,		// constraint 0, depends on the type.
			SOLVER_RESULT_2 = 2,		// constraint 0, depends on the type.
			SOLVER_RESULT_3 = 3,		// constraint 0, depends on the type.
			SOLVER_RESULT_4 = 4,		// constraint 0, depends on the type.
			SOLVER_RESULT_5 = 5,		// constraint 0, depends on the type.
			SOLVER_RESULT_6 = 6,		// constraint 0, depends on the type.
			SOLVER_RESULT_MAX = 7
		};

	public:

			/// The parametric path followed by bodyA.	
		hkParametricCurve* m_path;
		
			/// Current parametric position.
		hkReal m_parametricPosition;
		
			/// Max linear friction impulse.
		hkReal m_maxFrictionForce;
		
			/// The transforms from constraint space to object space.
		hkTransform m_transform_OS_KS[2];

			/// Pivot in world space of attached body.
		hkVector4 m_pivotWS;

			/// Heading vector in attached body space.
		hkVector4 m_heading;

			/// Up vector in attached body space.
		hkVector4 m_up;

			/// Angular constraining options.
		hkPointToPathConstructionInfo::OrientationConstraintType m_angularConstrainedDOF:8;
		
			/// Solver results.
		hkSolverResults m_results[SOLVER_RESULT_MAX];

	public:
			//
			// Internal functions
			//
		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;
};

#include <hkdynamics2/constraint/bilateral/pointtopath/hkPointToPathConstraint.inl>

#endif	// HK_DYNAMICS2_POINT_TO_PATH_CONSTRAINT_H

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
