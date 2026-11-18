/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_HKVEHICLEFRAMEWORKBP
#define INC_HKVEHICLEFRAMEWORKBP



// Helper structure BluePrint for a default vehicle implementation framework with 1 vehicle, 1 camera, and some "skidmarks".
struct hkVehicleFrameworkBP
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, hkVehicleFrameworkBP);

		  /// The vehicle in question (1st person only)
		HavokVehicle *m_vehicle;

		  /// The number of skidmark points that should be stored (to be displayed later)
		int m_numSkidPoints;

		  /// The minimum amount of energy lost on skidding to start displaying skidmarks and playing skidsound
		hkReal m_minSkidmarkEnergy;

		  /// The amount of energy lost on skidding where skidmarks and sound reacht their "strongest" values
		hkReal m_maxSkidmarkEnergy;

		/// The name of the vehicle
		const char* m_vehicleName;

		hkVehicleFrameworkBP(): m_vehicleName(HK_NULL) 
		{
		}
};



#endif //INC_HKVEHICLEFRAMEWORKBP

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
