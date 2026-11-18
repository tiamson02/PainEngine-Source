/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_SOFT_CONTACT_CONSTRAINT_H
#define HK_DYNAMICS2_SOFT_CONTACT_CONSTRAINT_H

#include <hkdynamics/constraint/contact/hkSimpleContactConstraintData.h>
#include <hkconstraintsolver/simpleconstraints/hkSimpleConstraintUtil.h>

// This is an experimental constraint modifier used to soften contact constraints using the hkSoftContactUtil. It has many restrictions (documented there) 
// This feature is currently unsupported - use it with care.
class hkSoftContactConstraintData : public hkSimpleContactConstraintData
{
	public:

		hkSoftContactConstraintData() {}

		void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut& out);

		void getConstraintInfo( hkConstraintInfo& info )const;

		virtual void toiCollisionResponseBeginCallback( const hkContactPoint& cp, struct hkSimpleConstraintInfoInitInput& inA, struct hkBodyVelocity& velA, hkSimpleConstraintInfoInitInput& inB, hkBodyVelocity& velB);
		
		virtual void toiCollisionResponseEndCallback( const hkContactPoint& cp, hkReal impulseApplied, hkSimpleConstraintInfoInitInput& inA, hkBodyVelocity& velA, hkSimpleConstraintInfoInitInput& inB, hkBodyVelocity& velB);

	public:
		union voidFloat
		{
			void* pointer;
			float value;
		};

		//static HK_THREAD_LOCAL( hkBodyVelocity ) s_bodyVelocities[2];
		//static HK_THREAD_LOCAL( hkBool ) s_bodyVelocitiesInitialized;
		static hkBodyVelocity s_bodyVelocities[2];
		static hkBool s_bodyVelocitiesInitialized;

};


#endif //HK_DYNAMICS2_SOFT_CONTACT_CONSTRAINT_H

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
