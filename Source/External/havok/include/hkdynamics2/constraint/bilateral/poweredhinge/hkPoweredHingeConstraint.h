/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_POWERED_HINGE_CONSTRAINT_H
#define HK_DYNAMICS2_POWERED_HINGE_CONSTRAINT_H

#include <hkdynamics2/constraint/hkRigidConstraint.h>
#include <hkdynamics2/constraint/bilateral/poweredhinge/hkPoweredHingeConstructionInfo.h>
#include <hkdynamics2/constraint/bilateral/limitedhinge/hkLimitedHingeConstraint.h>

/// Same as limited hinge, but powered
class hkPoweredHingeConstraint : public hkLimitedHingeConstraint
{
	public:
				
			/// Creates a new hkLimitedHingeConstraint using the specified construction info.
		hkPoweredHingeConstraint(const hkPoweredHingeConstructionInfo& info);

		~hkPoweredHingeConstraint();

			/// Sets all data except the bodies
	    void setData(const hkPoweredHingeConstructionInfo& info);

			/// Gets the construction info for the constraint
		inline void getConstructionInfo(hkPoweredHingeConstructionInfo& info);

		
        // Methods to set and get angle limits and friction

			/// Set whether to ignore limits
		inline void setIgnoreLimits(bool state);
        
			/// Returns current angle position
		inline hkReal getCurrentPos();

			/// Get type from this constraint	
		virtual int getType() const;

			/// get the motor. default is HK_NULL
		inline hkMotorController* getMotor();

			/// is this motor on or off?
		inline bool getMotorActive();

			/// turn the motor on or off
		inline void setMotorActive( bool onOff );

			/// set the motor.  setting this to null will disable any motor computations
			/// increments reference of new motor, decrements counter of replaced motor ( if any )
		inline void setMotor( hkMotorController* motor );

		enum 
		{
			SOLVER_RESULT_ANG_0 = 0,		// angular constraint 0
			SOLVER_RESULT_ANG_1 = 1,		// angular constraint 1
			SOLVER_RESULT_ANG_2 = 2,		// angular constraint 2
			SOLVER_RESULT_LIN_0 = 3,			// linear constraint defined by m_constrainedAxis1B
			SOLVER_RESULT_LIN_1 = 4,			// linear constraint defined by m_constrainedAxis2B

			SOLVER_RESULT_MOTOR = 5,			// the motor
			SOLVER_RESULT_LIMIT = 6,			// the limits
			SOLVER_RESULT_MAX = 7
		};

	private:
		
		bool m_motorActive;

		// record number of times 0 has been passed ( -ve if passed backwards )
		hkReal m_rotations;

        // Motor
		hkMotorController* m_motor;

		/// Ignore the limits option
		bool m_ignoreLimits;

		// For internal use
	public:
		void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		void getConstraintInfo( hkConstraintInfo& info ) const;

};

#include <hkdynamics2/constraint/bilateral/poweredhinge/hkPoweredHingeConstraint.inl>

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
