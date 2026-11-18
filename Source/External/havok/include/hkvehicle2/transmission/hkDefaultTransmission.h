/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DEFAULTTRANSMISSION_H
#define HK_DEFAULTTRANSMISSION_H




/// Default implementation of TransmissionComponent. See hkDefaultTransmissionBP for more parameter details.
class DefaultTransmission: public TransmissionComponent
{
	public:

			/// Cconstructor.
		DefaultTransmission(const hkDefaultTransmissionBP &);

			/// Reset all values except pointer values.
		void reinitialize( const hkDefaultTransmissionBP &bp );

			/// Destructor
		~DefaultTransmission();
		/*
		** OVERRIDEN FUNCTIONS FROM VEHICLECOMPONENT
		*/

			/// Overriden from vehicle component.
		void updateComponent(const hkStepInfo& stepInfo);
			// calculates torque and rpm, and updates the current gear

	public:

		/*
		** SET METHODS
		*/

		inline void setDownshiftRPM (const hkReal &rpm);
		inline void setUpshiftRPM (const hkReal &rpm);
		inline void setPrimaryTransmissionRatio (const hkReal &ratio);
		inline void setClutchDelayTime (const hkReal &time);
		inline void setReverseGearRatio (const hkReal &ratio);
		inline void setGearRatio (int gear, const hkReal &ratio);
		inline void setWheelTorqueRatio (int wheel, const hkReal &torque_ratio);

		/*
		** GET METHODS
		*/

		inline const hkReal &getDownshiftRPM () const;
		inline const hkReal &getUpshiftRPM () const;
		inline const hkReal &getPrimaryTransmissionRatio () const;
		inline const hkReal &getClutchDelayTime () const;
		inline const hkReal &getReverseGearRatio () const;
		inline const hkReal &getGearRatio (int gear) const;
		inline const hkReal &getWheelTorqueRatio (int wheel) const;

	protected:

		hkReal calcMainTransmittedTorque() const;
		hkReal calcTransmissionRPM() const;
		bool calcIsReversing() const;

		void updateCurrentGear();
		inline hkReal getCurrentRatio() const;

		/*
		** Per-Wheel Info
		*/
		hkReal *m_wheelsTorqueRatio;

		/*
		** Per-Gear Info
		*/
		hkReal *m_gearsRatio;


		hkReal m_primaryTransmissionRatio;
		hkReal m_reverseGearRatio;
		hkReal m_downshiftRPM;
		hkReal m_upshiftRPM;
		hkReal m_clutchDelayTime;

		// to implement clutch delay
		bool m_delayed;
		
		hkReal	m_lastTimeGearChanged;

	protected:

		const int m_numGears;

	private:

		    /// Reset all values except pointer values.
		void reinitializeInternal( const hkDefaultTransmissionBP &bp );
};

#include <hkvehicle2/transmission/hkDefaultTransmission.inl>



#endif //HK_DEFAULTTRANSMISSION_H

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
