/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_RELATIVE_ORIENTATION_CONSTRAINT_H
#define HK_DYNAMICS2_RELATIVE_ORIENTATION_CONSTRAINT_H

#include <hkdynamics/constraint/hkConstraintData.h>

extern const hkClass hkRelativeOrientationConstraintDataClass;

	/// The ball-and-socket or point-to-point constraint.
class hkRelativeOrientationConstraintData : public hkConstraintData
{
	public:

		HK_DECLARE_REFLECTION();

		hkRelativeOrientationConstraintData();

			/// Sets relative orientation that is to be enforced.
		void setInBodySpace(const hkQuaternion& orientationOfBodyBInBodyASpace);

			/// Will preserve the initial relative orientation of the bodies.
		void setInWorldSpace(const hkTransform& bodyATransform, const hkTransform& bodyBTransform);
			

		/// Check consistency of constraint members.
		virtual hkBool isValid() const;

			/// Get type from this constraint.
		virtual int getType() const;

	public:
			//
			//	Solver interface
			//
		enum 
		{
			SOLVER_RESULT_ANG_0 = 0,		// linear constraint 
			SOLVER_RESULT_ANG_1 = 1,		// linear constraint 
			SOLVER_RESULT_ANG_2 = 2,		// linear constraint 
			SOLVER_RESULT_MAX = 3
		};

		struct Runtime
		{
			class hkSolverResults m_solverResults[3/*VC6 doesn't like the scoping for SOLVER_RESULT_MAX*/];
		};

		inline const Runtime* getRuntime( hkConstraintRuntime* runtime ){ return reinterpret_cast<Runtime*>(runtime); }

	public:

			/// Desired orientation of the bodyB in the bodyA's space
		hkQuaternion m_aTb;

	public:
			//
			// Internal functions.
			//
		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;

		virtual void getRuntimeInfo( hkBool wantRuntime, hkConstraintData::RuntimeInfo& infoOut ) const;

	public:

		hkRelativeOrientationConstraintData(hkFinishLoadedObjectFlag f) : hkConstraintData(f) {}
};

#endif // HK_DYNAMICS2_RELATIVE_ORIENTATION_CONSTRAINT_H

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
