/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_MOTION_H
#define HK_DYNAMICS2_MOTION_H

#include <hkmath2/hkMath.h>
#include <hkdynamics2/motion/hkMotionTypes.h>

class hkVelocityAccumulator;
class hkStepInfo;

/// This class is used internally.
/// An object's hkMotion stores all the information relating to
/// how it moves, including its mass, position, and velocity.
/// A hkMotion is owned by exactly one object, usually a hkEntity
/// or another hkMotion.
class hkMotion: public hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DYNAMICS);
		
		inline hkMotion();

		virtual void step(const hkStepInfo& info) = 0;

		virtual void applyForces(const hkStepInfo& info, const hkVector4& forceField) = 0;


		HK_FORCE_INLINE hkVelocityAccumulator* applyForcesAndBuildAccumulator(const hkStepInfo& info, const hkVector4& forceField, hkVelocityAccumulator* accumulators, int solverData);

		virtual const hkVelocityAccumulator* applyAccumulator(const hkStepInfo& info, float accumulatorFraction, const hkVelocityAccumulator* m ) = 0;

		virtual int getAccumulatorSize() = 0;

		virtual hkMotionType getType() = 0;

	protected:

		virtual hkVelocityAccumulator* applyForcesAndBuildAccumulator(const hkStepInfo& info, const hkVector4& forceField, hkVelocityAccumulator* accumulators) = 0;

	private:
		friend class hkConstraintSolverSetup;
		int m_solverData;
};


#include <hkdynamics2/motion/hkMotion.inl>


#endif // HK_DYNAMICS2_MOTION_H

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
