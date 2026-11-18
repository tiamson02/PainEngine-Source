/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_HINGE_CONSTRAINT_H
#define HK_DYNAMICS2_HINGE_CONSTRAINT_H

#include <hkdynamics/constraint/hkConstraintData.h>

extern const hkClass hkHingeConstraintDataClass;

/// Basic hinge constraint
class hkHingeConstraintData : public hkConstraintData
{
	public:

		HK_DECLARE_REFLECTION();

		hkHingeConstraintData();

			/// Set the data for a Hinge constraint with information given in body space.
			/// \param pivotA The constraint pivot point, specified in bodyA space.
			/// \param pivotB The constraint pivot point, specified in bodyB space.
			/// \param axisA The hinge axis, specified in bodyA space.
			/// \param axisB The hinge axis, specified in bodyB space.
		void setInBodySpace(const hkVector4& pivotA, const hkVector4& pivotB, const hkVector4& axisA, const hkVector4& axisB);
	
			/// Set the data for a Hinge constraint with information given in world space.
			/// \param bodyA The first rigid body transform 
			/// \param bodyB The second rigid body transform 
			/// \param pivot The pivot point, specified in world space.
			/// \param axis The hinge axis, specified in world space.
		void setInWorldSpace(const hkTransform& bodyATransform, const hkTransform& bodyBTransform, const hkVector4& pivot, const hkVector4& axis);
		

			/// Check consistency of constraint members
		virtual hkBool isValid() const;

			/// Get type from this constraint	
		virtual int getType() const;

	public:
			//
			//	Solver interface
			//
		enum 
		{
			SOLVER_RESULT_ANG_0 = 0,		// angular constraint 0
			SOLVER_RESULT_ANG_1 = 1,		// angular constraint 1
			SOLVER_RESULT_LIN_0 = 2,		// linear constraint defined by m_constrainedAxis1B
			SOLVER_RESULT_LIN_1 = 3,		// linear constraint defined by m_constrainedAxis2B
			SOLVER_RESULT_LIN_2 = 4,		// linear constraint defined by m_constrainedAxis2B
			SOLVER_RESULT_MAX = 5 // keep this in sync with the hkSolverResult array below
		};		

		struct Runtime
		{
			class hkSolverResults m_solverResults[5/*VC6 doesn't like the scoping for SOLVER_RESULT_MAX*/];
		};

		static inline Runtime* HK_CALL getRuntime( hkConstraintRuntime* runtime ) { return reinterpret_cast<Runtime*>(runtime); }


	public:	

		struct ConstraintBasisA
		{
			HK_DECLARE_REFLECTION();
			
				/// Pivot in body A space. 
			hkVector4 m_pivot;

				/// First of two mutually perpendicular vectors to the axis in A space. 
			hkVector4 m_perpToAxle1;

				/// Second of two mutually perpendicular vectors to the axis in A space.
			hkVector4 m_perpToAxle2;
		};
		
		struct ConstraintBasisB
		{
			HK_DECLARE_REFLECTION();
			
				/// Pivot in body B space. 	
			hkVector4 m_pivot;

				/// Axle in body B space.
			hkVector4 m_axle;
		};

		struct ConstraintBasisA m_basisA;
		struct ConstraintBasisB m_basisB;
	public:

		// Internal functions

			/// constrainted axis are in A's space. and a perpendicular is in B's space
			/// error term is found by checking the perpendicular's projection on one of the axis in 
			/// a mutual space.  this works because the axis are in A's frame and the perpendicular is in B's
		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;

		virtual void getRuntimeInfo( hkBool wantRuntime, hkConstraintData::RuntimeInfo& infoOut ) const;
	public:

		hkHingeConstraintData(hkFinishLoadedObjectFlag f) : hkConstraintData(f) {}
};

#endif //HK_DYNAMICS2_HINGE_CONSTRAINT_H

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
