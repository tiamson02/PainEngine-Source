/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_SPATIAL_RIGID_BODY_DEACTIVATOR_H
#define HK_DYNAMICS2_SPATIAL_RIGID_BODY_DEACTIVATOR_H

#include <hkdynamics2/entity/hkRigidBodyDeactivator.h>

class hkRigidBody;

	/// An hkEntityDeactivator for rigid bodies.
class hkSpatialRigidBodyDeactivator : public hkRigidBodyDeactivator
{
	public:

		hkSpatialRigidBodyDeactivator( hkRigidBody* rigidBody );

		virtual hkRigidBodyDeactivatorType getRigidBodyDeactivatorType() const;

		virtual bool shouldDeactivateHighFrequency( const hkEntity* entity );

		virtual bool shouldDeactivateLowFrequency( const hkEntity* entity );

			/// Set m_radiusSqrd.
		hkReal setRadiusSqrd(const hkShape* shape);

	private:

			/// An indication of the size of the rigid body,
		hkReal m_radiusSqrd; 

		struct Sample
		{
			hkVector4    m_refPosition;
			hkQuaternion m_refRotation;
		};

		Sample m_highFrequencySample;
		Sample m_lowFrequencySample;

		hkReal m_minHighFrequencyTranslation;
		hkReal m_minHighFrequencyRotation;
		hkReal m_minLowFrequencyTranslation;
		hkReal m_minLowFrequencyRotation;
};

#endif // HK_DYNAMICS2_SPATIAL_RIGID_BODY_DEACTIVATOR_H

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
