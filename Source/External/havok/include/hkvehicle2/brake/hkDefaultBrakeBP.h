/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DEFAULT_VEHICLE_BRAKES_BP
#define HK_DEFAULT_VEHICLE_BRAKES_BP



/// Default implementation of hkBrakeComponentBP
class hkDefaultBrakeBP: public hkBrakeComponentBP
{
public:
	
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, hkDefaultBrakeBP);

	/// Constructor
	hkDefaultBrakeBP(int num_wheels);

	/// Destructor
	virtual ~hkDefaultBrakeBP();

public:

	/*
	** Per-wheel values
	*/

		/// The maximum torque the wheel can apply when braking
	hkReal *m_wheelsMaxBrakingTorque;

		/// The min amount of braking from the driver that could cause the wheel to block (range [0..1])
	hkReal *m_wheelsMinPedalInputToBlock;

		/// The time (in secs) after which, if the user has applied enough brake input, the wheel will block
	hkReal m_wheelsMinTimeToBlock;

		/// True if the particular wheel is connected to handrake (locks when handbrake is on)
	bool *m_wheelsIsConnectedToHandbrake;
};



#endif /* HK_STANDARD_VEHICLE_BRAKES_BP */

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
