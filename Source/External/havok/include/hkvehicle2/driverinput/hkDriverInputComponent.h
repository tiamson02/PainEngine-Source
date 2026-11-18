/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DRIVERINPUTCOMPONENT_H
#define HK_DRIVERINPUTCOMPONENT_H


#include <hkvehicle2/vehicle/hkVehicleComponent.h>

/// An abstraction for the driver input to the controls of the car, like a steering wheel,
/// pedals or handbrake.
class DriverInputComponent : public VehicleComponent
{
	public:

			/// Constructor.
		inline DriverInputComponent(const hkDriverInputComponentBP &bp);

			/// Gets the position of the accelerator pedal, in a range from 0.0f to 1.0f.
		inline const hkReal &getAcceleratorPedalInput() const;

			/// Gets the position of the brake pedal, in a range from 0.0f to 1.0f.
		inline const hkReal &getBrakePedalInput() const;

			/// Gets the position of the steering wheel, in a range from -1.0f to 1.0f.
		inline const hkReal &getSteeringWheelInput() const;

			/// Gets the on/off state of the handbrake lever.
		inline const bool &getHandbrakeOn() const;

			/// Gets if the driver is trying to reverse.
		inline const bool &getTryingToReverse() const;


	protected:

		// These members must be updated by the method updateComponent()
		hkReal m_acceleratorPedalInput;
		hkReal m_brakePedalInput;
		hkReal m_steeringWheelInput;
		bool m_handbrakeOn;
		bool m_tryingToReverse;

	protected:

		    /// reset all values except pointer values
		inline void reinitializeInternal( const hkDriverInputComponentBP &bp );


};

#include <hkvehicle2/driverinput/hkDriverInputComponent.inl>



#endif //HK_DRIVERINPUTCOMPONENT_H

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
