/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_ENGINECOMPONENT_H
#define HK_ENGINECOMPONENT_H



/// An abstract component for the vehicle engine which controls the the engine torque and RPM.
class EngineComponent : public VehicleComponent
{
	public:

			/// Constructor.
		inline EngineComponent(const hkEngineComponentBP &bp);

			/// Gets the current RPM of the engine.
		inline const hkReal &getEngineRPM() const;

			/// Gets the torque supplied by the engine.
		inline const hkReal &getEngineTorque() const;


	protected:

		// These members must be updated by the method updateComponent()
		hkReal m_engineTorque;
		hkReal m_engineRPM;

	protected:

		    /// reset all values except pointer values
		inline void reinitializeInternal( const hkEngineComponentBP &bp );

};

#include <hkvehicle2/engine/hkEngineComponent.inl>



#endif //HK_ENGINECOMPONENT_H

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
