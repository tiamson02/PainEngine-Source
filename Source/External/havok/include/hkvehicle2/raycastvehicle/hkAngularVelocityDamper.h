/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_ANGULARVELOCITYDAMPER
#define INC_ANGULARVELOCITYDAMPER



/// An angular velocity damper which reduces a vehicle's ability to spin in the air, making it more stable.
class AngularVelocityDamper : public ExternalVehicleController
{
	public:

			/// Constructor.
		AngularVelocityDamper (const hkAngularVelocityDamperBP &bp);

			/// Reset all values except pointer values.
		void reinitialize( const hkAngularVelocityDamperBP &bp );

			/// Destructor.
		~AngularVelocityDamper();

			/// Overriden from ExternalVehicleController, applies the controller.
			/// In this case it damps the vehicle angular velocity.
		void applyExternalController (const hkStepInfo& stepInfo, RaycastVehicle &raycast_vehicle);

	private:

		hkReal	m_normalSpinDamping;
		hkReal    m_collisionSpinDamping;
		hkReal	m_collisionThreshold;

		hkReal	m_cachedCsd;
		hkReal	m_cachedNsd;

	private:

		    /// Reset all values except pointer values.
		void reinitializeInternal( const hkAngularVelocityDamperBP &bp );

};



#endif //INC_ANGULARVELOCITYDAMPER
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
