/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_STEERINGCOMPONENT_H
#define HK_STEERINGCOMPONENT_H

#include <hkvehicle2/vehicle/hkVehicleComponent.h>



/// An abstract component for vehicle steering for both the main (chassis) and individual wheel steering angles.
class SteeringComponent : public VehicleComponent
{
	public:

			/// Constructor.
		SteeringComponent(const hkSteeringComponentBP &bp);

			/// Destructor.
		~SteeringComponent();

			/// Gets the main steering angle.
		inline const hkReal &getMainSteeringAngle() const;

			/// Gets the steering angle at an specific wheel.
		inline const hkReal &getWheelSteeringAngle(int wheel) const;

	protected:

			/// This component must be updated by the method updateComponent().
		hkReal  m_mainSteeringAngle;
			/// This component must be updated by the method updateComponent().
		hkReal* m_wheelsSteeringAngle;

	protected:

		const int m_numWheels;

	protected:

		    /// Reset all values except pointer values.
		void reinitializeInternal( const hkSteeringComponentBP &bp );

};

#include <hkvehicle2/steering/hkSteeringComponent.inl>



#endif //HK_STEERINGCOMPONENT_H

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
