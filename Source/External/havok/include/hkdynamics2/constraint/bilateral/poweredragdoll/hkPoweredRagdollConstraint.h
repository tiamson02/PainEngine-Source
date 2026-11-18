/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_POWERED_RAGDOLL_CONSTRAINT_H
#define HK_DYNAMICS2_POWERED_RAGDOLL_CONSTRAINT_H

#include <hkdynamics2/constraint/bilateral/ragdoll/hkRagdollConstraint.h>
#include <hkdynamics2/constraint/bilateral/poweredragdoll/hkPoweredRagdollConstraint.h>

class hkMotorController;

/// Ragdoll constraint with motors to drive the 3 angles.  If you are using a motor with a
/// target position to set, set it to 0 and the rest will be handled by the setTarget() method
class hkPoweredRagdollConstraint : public hkRagdollConstraint
{
	public:

			///
		hkPoweredRagdollConstraint(const hkRagdollConstructionInfo& info);

			///
		~hkPoweredRagdollConstraint();

			/// Gets the construction info for the constraint.
		void getConstructionInfo(hkRagdollConstructionInfo& info);

			/// Twist should line up with col0 and plane with col1, col2 = col0 cross col1.
		void setTarget( const hkMatrix3& relativeFrameB );

			/// Returns target frame
		void getTarget(hkMatrix3& targetFrameB );

			/// Return the motor used to drive the twist axis angle.
		hkMotorController* getTwistMotor();

			/// Set the motor used to drive the twist axis angle.
			/// Increments reference of new motor, decrements counter of replaced motor ( if any )
		void setTwistMotor( hkMotorController* motor );

			/// Return the motor used to drive the cone axis angle.
		hkMotorController* getConeMotor();

			/// Set the motor used to drive the cone axis angle.
			/// Increments reference of new motor, decrements counter of replaced motor ( if any ).
		void setConeMotor( hkMotorController* motor );

			/// Return the motor used to drive the plane axis angle
		hkMotorController* getPlaneMotor();

			/// Set the motor used to drive the plane axis angle.
			/// Increments reference of new motor, decrements counter of replaced motor ( if any ).
		void setPlaneMotor( hkMotorController* motor );

			/// Get type from this constraint.
		virtual int getType() const;

	public:

		hkMotorController* m_twistMotor;
		hkMotorController* m_coneMotor;
		hkMotorController* m_planeMotor;

		/// The frame the constraint will try to match
		hkMatrix3 m_targetFrameB; 

	public:

		// Internal functions

		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;
};

#endif // HK_DYNAMICS2_POWERED_RAGDOLL_CONSTRAINT_H

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
