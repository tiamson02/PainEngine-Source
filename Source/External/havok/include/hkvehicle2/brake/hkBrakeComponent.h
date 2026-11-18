/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_BRAKECOMPONENT
#define INC_BRAKECOMPONENT



/// Determines torque applied by brakes for each wheel and the locking of
/// wheels (due to handbraking or strong braking being applied, for example).
class BrakeComponent : public VehicleComponent
{
	public:

			/// Constructor.
		BrakeComponent(const hkBrakeComponentBP &bp);

			/// Destructor.
		~BrakeComponent();

			/// Gets the braking torque applied at a specific wheel.
		inline const hkReal &getWheelBrakingTorque(int wheel) const;

			/// Gets if the wheel has been fixed (by handbrake for example).
		inline const bool &getWheelIsFixed(int wheel) const;

	protected:

			/// This member must be updated by the method updateComponent().
		hkReal* m_wheelsBrakingTorque;

			/// This member must be updated by the method updateComponent().
		bool* m_wheelsIsFixed;

	protected:

		const int m_numWheels;

	protected:

		    /// Reset all values except pointer values.
		void reinitializeInternal( const hkBrakeComponentBP &bp );

};

#include <hkvehicle2/brake/hkBrakeComponent.inl>




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
