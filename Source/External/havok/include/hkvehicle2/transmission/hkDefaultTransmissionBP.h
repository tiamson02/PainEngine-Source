/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_STANDARD_VEHICLE_TRANSMISSION_BP
#define HK_STANDARD_VEHICLE_TRANSMISSION_BP



/// Default implementation of hkTransmissionComponentBP
class hkDefaultTransmissionBP: public hkTransmissionComponentBP
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, hkDefaultTransmissionBP);

		/// Constructor. Sets defaults (straight through transmission, all wheels drive).
		hkDefaultTransmissionBP (int num_wheels, int num_gears = 0);

		/// Destructor
		~hkDefaultTransmissionBP ();

	public:
		/// The number of forward gears.
		const int m_numGears;

		/// The RPM of the engine the transmission shifts down.
		hkReal m_downshiftRPM;

		/// The RPM of the engine the transmission shifts up.
		hkReal m_upshiftRPM;
		// Note: m_upshiftRpm > m_downshiftRpm

		/// An extra factor to the gear ratio.
		hkReal m_primaryTransmissionRatio;

		/// The time needed [seconds] to shift a gear.
		hkReal m_clutchDelayTime;

		/// The back gear ratio.
		hkReal m_reverseGearRatio;

		/*
		** Per-gear information
		*/

		/// The ratio of the forward gears.
		hkReal *m_gearsRatio;

		/*
		** Per-wheel information
		*/

		/// The transmission ratio for every wheel
		hkReal *m_wheelsTorqueRatio;

};



#endif /* HK_STANDARD_VEHICLE_TRANSMISSION_BP */

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
