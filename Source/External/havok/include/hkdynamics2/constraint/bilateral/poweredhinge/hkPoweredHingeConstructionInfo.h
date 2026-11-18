/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_DYNAMICS2_POWEREDHINGE_CONSTRUCTION_INFO_H
#define HK_DYNAMICS2_POWEREDHINGE_CONSTRUCTION_INFO_H

#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkdynamics2/constraint/bilateral/limitedhinge/hkLimitedHingeConstructionInfo.h>

class hkMotorController;

/// The information needed to create a limited hinge constraint.
/// All vector members are specified in body space.

class hkPoweredHingeConstructionInfo : public hkLimitedHingeConstructionInfo 
{

	public:
  
		hkPoweredHingeConstructionInfo();

			/// the motor that powers this constraint
		inline void setMotor( hkMotorController* motor );

			/// Set whether to ignore the angular limits, so it can act like a hinge
		inline void setIgnoreLimits(bool state);

			// the motor that powers this constraint
		hkMotorController* m_motor;

			// Ignore angular limits, so it acts like a hinge
		bool m_ignoreLimits;

};

#include <hkdynamics2/constraint/bilateral/poweredhinge/hkPoweredHingeConstructionInfo.inl>

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
