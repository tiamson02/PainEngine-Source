/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_TRANSMISSIONCOMPONENT_H
#define INC_TRANSMISSIONCOMPONENT_H



/// An abstract component for the vehicle transmission.\n
/// A typical implementation will query the EngineComponent.
class TransmissionComponent : public VehicleComponent
{
	public:

			/// Constructor.
		TransmissionComponent(const hkTransmissionComponentBP &bp);

			/// Destructor.
		~TransmissionComponent();

			/// Gets the torque transmitted by the transmission.
		inline const hkReal getMainTransmittedTorque() const;

			/// Gets the current rpm on the transmission.
		inline const hkReal getTransmissionRPM() const;

			/// Gets the current gear of the transmission.
		inline const int getCurrentGear() const;

			/// Gets if the transmission is reversing.
		inline const bool getIsReversing() const;

			/// Gets the torque transmitted to the particular wheel.
		inline const hkReal getWheelTransmittedTorque(int wheel) const;

	protected:

		// This member must be updated by the method updateComponent().
		int m_currentGear;
		bool m_isReversing;
		hkReal m_transmissionRPM;
		hkReal m_mainTransmittedTorque;
		hkReal *m_wheelsTransmittedTorque;

	protected:

		const int m_numWheels;

	protected:

		    /// Reset all values except pointer values.
		void reinitializeInternal( const hkTransmissionComponentBP &bp );

};

#include <hkvehicle2/transmission/hkTransmissionComponent.inl>



#endif //INC_TRANSMISSIONCOMPONENT_H

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
