/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DEFAULTANALOGDRIVERINPUT_H
#define HK_DEFAULTANALOGDRIVERINPUT_H

#include <hkvehicle2/driverinput/hkDriverInputComponent.h>

/// Helper structure for an analog input device for driver input.
struct AnalogDeviceStatus
{
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, AnalogDeviceStatus);

	/// The current position, defined from -1 to 1, in the X axis of the device
	hkReal m_positionX;

	/// The current position, defined from -1 to 1, in the Y axis of the device
	hkReal m_positionY;

	/// True if the handbrake button is pressed
	bool m_handbrakeButtonPressed;

	/// True if the reverse button is pressed
	bool m_reverseButtonPressed;

};

/// Default (analog) implementation of DriverInputComponent. See hkDefaultAnalogDriverInputBP for more parameter details.
class DefaultAnalogDriverInput : public DriverInputComponent
{
	public:

			/// Constructor
		DefaultAnalogDriverInput (const hkDefaultAnalogDriverInputBP &bp);

			/// reset all values except pointer values
		void reinitialize( const hkDefaultAnalogDriverInputBP &bp );
		/*
		** Function overriden from VehicleComponent
		*/
	
			/// Overriden from VehicleComponent, this method sets the input values
		void updateComponent(const hkStepInfo& stepInfo);
			// (accelerator, brake, steering wheel and handbrake) from the driver

	public:

			/// Device status : the user should update it often enough
		AnalogDeviceStatus m_deviceStatus;

	public:

		/*
		** GET Methods
		*/

		inline const hkReal &getSlopeChangePointX() const;
		inline const hkReal &getInitialSlope() const;
		inline const hkReal &getDeadZone() const;

		/*
		** SET Methods
		*/

		inline void setSlopeChangePointX(const hkReal &slope_change_point_x);
		inline void setInitialSlope(const hkReal &initial_slope);
		inline void setDeadZone(const hkReal &dead_zone);

	protected:

		hkReal calcAcceleratorInput() const;
		hkReal calcBrakeInput() const;
		hkReal calcSteeringInput() const;
		bool calcTryingToReverse() const;

		hkReal m_slopeChangePointX;
		hkReal m_initialSlope;
		hkReal m_secondSlope;			// derived (from slopeChangePointX and initialSlope)
		hkReal m_slopeChangePointY;	// derived (from slopeChangePointX and initialSlope)
		hkReal m_deadZone; 
		bool m_autoReverse;

	private:

		    /// reset all values except pointer values
		void reinitializeInternal( const hkDefaultAnalogDriverInputBP &bp );
};

#include <hkvehicle2/driverinput/hkDefaultAnalogDriverInput.inl>



#endif //HK_DEFAULTANALOGDRIVERINPUT_H

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
