/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_SIMPLE_CONSTRAINT_OWNER_H
#define HK_DYNAMICS2_SIMPLE_CONSTRAINT_OWNER_H

#include <hkdynamics/constraint/hkConstraintOwner.h>
#include <hkdynamics/constraint/hkConstraintData.h>


class hkSimpleConstraintOwner: public hkConstraintOwner
{
	public:
			// max total size of jacobians added by one constraints
		int m_maxSizeOfJacobians;
			// sum of sizes all jacobians held by the owner
		int m_sumSizeOfJacobians;
		int m_sumSizeOfSchemas;
		int m_sumSizeOfMotions;
		int m_sumNumSolverResults;

	public:

		inline virtual void addConstraintInfo( hkConstraintInstance* constraint, hkConstraintInfo& delta )
		{
			m_maxSizeOfJacobians = hkMath::max2( m_maxSizeOfJacobians, delta.m_maxSizeOfJacobians);
			m_maxSizeOfJacobians = hkMath::max2( m_maxSizeOfJacobians, delta.m_sizeOfJacobians);
			m_sumSizeOfJacobians  += delta.m_sizeOfJacobians;
			m_sumSizeOfSchemas    += delta.m_sizeOfSchemas;
			m_sumNumSolverResults += delta.m_numSolverResults;

#ifdef HK_STORE_CONSTRAINT_INFO_IN_INTERNAL
			hkConstraintInternal* intern = constraint->m_internal;
			if (intern)
			{
				intern->m_sumNumSolverResults = hkUint8 (intern->m_sumNumSolverResults + delta.m_numSolverResults);
				intern->m_sumSizeOfJacobians  = hkUint16(intern->m_sumSizeOfJacobians  + delta.m_sizeOfJacobians);
				intern->m_sumSizeOfSchemas    = hkUint16(intern->m_sumSizeOfSchemas    + delta.m_sizeOfSchemas);
			}
#endif
		}

		inline virtual void subConstraintInfo( hkConstraintInstance* constraint, hkConstraintInfo& delta )
		{
			m_sumSizeOfJacobians  -= delta.m_sizeOfJacobians;
			m_sumSizeOfSchemas    -= delta.m_sizeOfSchemas;
			m_sumNumSolverResults -= delta.m_numSolverResults;

#ifdef HK_STORE_CONSTRAINT_INFO_IN_INTERNAL
			hkConstraintInternal* intern = constraint->getInternal();
			intern->m_sumNumSolverResults = hkUint8 (intern->m_sumNumSolverResults - delta.m_numSolverResults);
			intern->m_sumSizeOfJacobians  = hkUint16(intern->m_sumSizeOfJacobians  - delta.m_sizeOfJacobians);
			intern->m_sumSizeOfSchemas    = hkUint16(intern->m_sumSizeOfSchemas    - delta.m_sizeOfSchemas);
#endif
		}

		inline virtual void addMotionInfo( int sizeofMotion )
		{
			m_sumSizeOfMotions += sizeofMotion;
		}

		inline virtual void subMotionInfo( int sizeofMotion )
		{
			m_sumSizeOfMotions -= sizeofMotion;
		}

		inline void clearConstraintInfo()
		{
			m_maxSizeOfJacobians = 0;
			m_sumSizeOfJacobians = 0;
			m_sumSizeOfSchemas = 0;
			m_sumSizeOfMotions = 0;
			m_sumNumSolverResults = 0; 
		}

		inline virtual void removeConstraint( hkConstraintInstance* constraint)
		{
			HK_ASSERT2(0x4aebccbb,  0, "Abstract virtual function called" );
		}

		inline void mergeConstraintInfo( hkSimpleConstraintOwner& other )
		{
			m_maxSizeOfJacobians = hkMath::max2( m_maxSizeOfJacobians, other.m_maxSizeOfJacobians);
			m_sumSizeOfJacobians  += other.m_sumSizeOfJacobians;
			m_sumSizeOfSchemas    += other.m_sumSizeOfSchemas;
			m_sumNumSolverResults += other.m_sumNumSolverResults;
			m_sumSizeOfMotions += other.m_sumSizeOfMotions;
		}

		inline void splitConstraintInfo( hkSimpleConstraintOwner& other )
		{
			m_sumSizeOfJacobians  -= other.m_sumSizeOfJacobians;
			m_sumSizeOfSchemas    -= other.m_sumSizeOfSchemas;
			m_sumNumSolverResults -= other.m_sumNumSolverResults;
			m_sumSizeOfMotions    -= other.m_sumSizeOfMotions;
		}

		inline hkSimpleConstraintOwner()
		{
			clearConstraintInfo();
		}

};

#endif // HK_DYNAMICS2_SIMPLE_CONSTRAINT_OWNER_H

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
