/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_AERODYNAMICS_COMPONENT
#define HK_AERODYNAMICS_COMPONENT




/// Aerodynamics accounts for lift, drag, (or other environmental forces).
class AerodynamicsComponent : public VehicleComponent
{
	public:

			/// Constructor
		inline AerodynamicsComponent(const hkAerodynamicsComponentBP &bp);

			/// Returns the total force applied by all aerodynamics forces
		inline const hkVector4 &getTotalForceOnChassisWS() const;

			/// Returns the total torque applied by all aerodynamics forces
		inline const hkVector4 &getTotalTorqueOnChassisWS() const;


	protected:

		// These members must be updated by the method updateComponent()
		hkVector4 m_totalForceWS;
		hkVector4 m_totalTorqueWS;

	protected:

		    /// Reset all values except pointer values
		void reinitializeInternal( const hkAerodynamicsComponentBP &bp );


};

#include <hkvehicle2/aerodynamics/hkAeroDynamicsComponent.inl>



#endif //HK_AERODYNAMICS_COMPONENT

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
