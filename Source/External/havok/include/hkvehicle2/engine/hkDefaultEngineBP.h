/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_STANDARD_VEHICLE_ENGINE_BP
#define HK_STANDARD_VEHICLE_ENGINE_BP



/// Default implementation of hkEngineComponentBP
class hkDefaultEngineBP: public hkEngineComponentBP
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, hkDefaultEngineBP);

		/// Constructor. Sets defaults (no engine).
		hkDefaultEngineBP();

	public:

		/// The min RPM of the engine.
		hkReal m_minRPM;

		/// The optimum RPM, where the gross torque of the engine is maximal.
		hkReal m_optRPM;

		/// The max RPM the engine can reach.
		hkReal m_maxRPM;

		/// The maximum gross torque the engine can supply at the optimum RPM.
		hkReal m_torque;
		// This parameter also serves as a base for the following Factor values.

		/// Defines the gross torque at the min rpm as a factor to the torque at optimal RPM.
		hkReal m_torqueFactorAtMinRPM;

		/// Defines the gross torque at the max rpm as a factor to the torque at optimal RPM.
		hkReal m_torqueFactorAtMaxRPM;
		
		/// Defines the engine resistance torque at the min rpm as a factor to the torque at optimal RPM.
		hkReal m_resistanceFactorAtMinRPM;

		/// Defines the engine resistance torque at the opt rpm as a factor to the torque at optimal RPM.
		hkReal m_resistanceFactorAtOptRPM;

		/// Defines the engine resistance torque at the max rpm as a factor to the torque at optimal RPM.
		hkReal m_resistanceFactorAtMaxRPM;

		/// An extra RPM for the motor in case the clutch is slipping at very low speeds.
		hkReal m_clutchSlipRPM;
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
