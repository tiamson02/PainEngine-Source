
/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_CONSTRAINTSOLVER2_VELOCITY_ACCUMULATOR_H
#define HK_CONSTRAINTSOLVER2_VELOCITY_ACCUMULATOR_H

	/// Represents the velocities of a motion
class hkVelocityAccumulator
{
	public:
		/// A list of possible types
		enum hkAccumulatorType
		{
				/// A rigid body
			HK_RIGID_BODY,
				/// A fixed rigid body, which ignores gravity
			HK_KEYFRAMED_RIGID_BODY,
				
			HK_END
		};
	public:

		hkEnum<hkAccumulatorType,hkUint8> m_type;
};


#endif // HK_CONSTRAINTSOLVER2_VELOCITY_ACCUMULATOR_H

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
