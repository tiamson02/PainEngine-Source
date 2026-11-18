/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_STANDARD_VEHICLE_STEERING_BP
#define HK_STANDARD_VEHICLE_STEERING_BP



/// Default implementation of hkSteeringComponentBP
class hkDefaultSteeringBP: public hkSteeringComponentBP
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, hkDefaultSteeringBP);

		/// Constructor. Sets defaults (no steering).
	   hkDefaultSteeringBP(int num_wheels);

	   /// Destructor
	   ~hkDefaultSteeringBP();

	public:

		/// The maximum steering angle (valid for velocities less than m_maxSpeedFullSteeringAngle).
	   hkReal m_maxSteeringAngle;

		/// The maximum speed the car still can use the m_maxSteeringAngle.
	   hkReal m_maxSpeedFullSteeringAngle;
		// For higher speeds the maximum steering angle is decreased.

		/*
		** Per-Wheel Information
		*/

		/// For every wheel, should be true if the wheel is connected to steering.
	   bool *m_wheelsDoesSteer;

};



#endif /* HK_STANDARD_VEHICLE_STEERING_BP */

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
