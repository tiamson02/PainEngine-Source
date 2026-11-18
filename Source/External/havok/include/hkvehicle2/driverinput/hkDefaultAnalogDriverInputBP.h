/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_STANDARD_VEHICLE_ANALOG_DRIVER_INPUT_BP
#define HK_STANDARD_VEHICLE_ANALOG_DRIVER_INPUT_BP

#include <hkvehicle2/driverinput/hkDriverInputComponentBP.h>

/// Default (analog) implementation of hkDriverInputComponentBP
class hkDefaultAnalogDriverInputBP: public hkDriverInputComponentBP
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, hkDefaultAnalogDriverInputBP);

		/// Constructor. Sets defaults (continuous slope).
		hkDefaultAnalogDriverInputBP();

	public:

		/// The input value, up to which the m_initialSlope is valid.
		hkReal m_slopeChangePointX;
		//  For higher input values the factor is increased so that for
		//  an input value of 1.0f the output is also 1.0f.

		/// The initial slope. Used for small steering angles.
		hkReal m_initialSlope;
		//  Dead zone to slopeChangePointX.

		/// The deadZone of the joystick.
		hkReal m_deadZone;

		/// If true, the car will start reversing when the brake is applied and the car is stopped.
		bool m_autoReverse;
};



#endif /* HK_STANDARD_VEHICLE_ANALOG_DRIVER_INPUT_BP */

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
