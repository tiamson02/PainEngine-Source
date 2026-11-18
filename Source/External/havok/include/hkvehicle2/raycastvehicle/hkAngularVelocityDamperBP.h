/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_HKANGULARVELOCITYDAMPERBP
#define INC_HKANGULARVELOCITYDAMPERBP



/// AngularVelocityDamper BluePrint.
class hkAngularVelocityDamperBP
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, hkAngularVelocityDamperBP);

		/// Constructor
		hkAngularVelocityDamperBP();

	public:

		/// A damping of the chassis angular velocity in normal mode
		hkReal	m_normalSpinDamping;

		/// A damping of the chassis angular velocity in collision mode
		hkReal    m_collisionSpinDamping;

		/// If the chassis angular velocity is higher than this value, the engine assumes collision 
		hkReal	m_collisionThreshold;
};



#endif //INC_HKANGULARVELOCITYDAMPERBP

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
