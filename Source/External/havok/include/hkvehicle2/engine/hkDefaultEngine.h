/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DEFAULTENGINE_H
#define HK_DEFAULTENGINE_H




/// Default implementation of EngineComponent. See hkDefaultEngineBP for more parameter details.
class DefaultEngine: public EngineComponent
{
	public:

			/// Constructor
		DefaultEngine (const hkDefaultEngineBP &bp);

			/// reset all values except pointer values
		void reinitialize( const hkDefaultEngineBP &bp );

		/*
		** Function overriden from VehicleComponent
		*/
			/// Overriden from VehicleComponent, called every simulation step.
			/// It updates the values of the current rpm and torque of the engine
		void updateComponent(const hkStepInfo& stepInfo);

	public:

		/*
		** SET Methods
		*/

		inline void setTorque (const hkReal &torque);
		inline void setTorqueFactorAtMinRPM (const hkReal &factor);
		inline void setTorqueFactorAtMaxRPM (const hkReal &factor);
		inline void setResistanceFactorAtMinRPM (const hkReal &factor);
		inline void setResistanceFactorAtOptRPM (const hkReal &factor);
		inline void setResistanceFactorAtMaxRPM (const hkReal &factor);
		inline void setClutchSlipRPM (const hkReal &rpm);

		/*
		** GET METHODS
		*/
		inline const hkReal &getTorque() const;
		inline const hkReal &getTorqueFactorAtMinRPM() const;
		inline const hkReal &getTorqueFactorAtMaxRPM() const;
		inline const hkReal &getResistanceFactorAtMinRPM() const;
		inline const hkReal &getResistanceFactorAtOptRPM() const;
		inline const hkReal &getResistanceFactorAtMaxRPM() const;
		inline const hkReal &getClutchSlipRPM() const;

	protected:

		hkReal calcRPM() const; 
			// calculates the current RPM
		
		hkReal calcTorque() const;
			// calculates the current Torque

	protected:

		hkReal m_clutchSlipRPM;

		hkReal m_minRPM;
		hkReal m_optRPM;
		hkReal m_maxRPM;

		hkReal m_torque;

		hkReal m_torqueFactorAtMinRPM;
		hkReal m_torqueFactorAtMaxRPM;
		
		hkReal m_resistanceFactorAtMinRPM;
		hkReal m_resistanceFactorAtOptRPM;
		hkReal m_resistanceFactorAtMaxRPM;

	private:

		    /// reset all values except pointer values
		void reinitializeInternal( const hkDefaultEngineBP &bp );
};

#include <hkvehicle2/engine/hkDefaultEngine.inl>



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
