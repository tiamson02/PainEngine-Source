/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef VEHICLE__FRAMEWORK
#define VEHICLE__FRAMEWORK



/*********************************************************
 *   defines our example class which holds some vehicle
 *   global parameters
 *********************************************************/

/// Helper structure for a default vehicle implementation framework with 1 vehicle, and some "skidmarks".
class VehicleFramework : public hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE);

		/// Construction
		VehicleFramework (const hkVehicleFrameworkBP &bp);

		/// Destruction
		~VehicleFramework ();

		/// Retrieves the current Havok vehicle associated with the framework
		inline const HavokVehicle* getCurrentVehicle() const;
		
		/// Changes the current vehicle associated with the framework
		inline void setCurrentVehicle(const HavokVehicle *vehicle);

	public:

		/* 
		** Camera management
		*/
		
			/// Updates skidmark information
		void updateSkidmarksInfo(const hkVector4& offset);

			/// Retrieves the skidmark information in the form of a strip
		void getWheelSkidmarksStrips(int wheel, hkVector4 *strips_out) const;

			/// Retrieves the current RPM of the vehicle
		inline hkReal getRPM() const;

			/// Retrieves the current speed of the vehicle in KM/H
		inline hkReal getKMPH() const;

			/// Retrieves the current speed of the vehicle in MP/H
		inline hkReal getMPH() const;


	protected:
		
		const HavokVehicle*		m_vehicle;
		char					m_vehicleName[32];


	public:

		// Skidmarks
		SkidmarksInfo			m_skidmarksInfo;
		hkReal					m_minSkidmarkEnergy;
		hkReal					m_maxSkidmarkEnergy;

};

#include <hkvehicle2/framework/hkVehicleFramework.inl>


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
