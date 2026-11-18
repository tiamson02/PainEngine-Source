/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_SIMPLE_CONSTRAINT_OWNER_H
#define HK_DYNAMICS2_SIMPLE_CONSTRAINT_OWNER_H

#include <hkdynamics2/constraint/hkConstraintOwner.h>

class hkSimpleConstraintOwner: public hkConstraintOwner
{
	public:
		int m_maxSizeOfJacobians;
		int m_sumSizeOfJacobians;
		int m_sumSizeOfSchemas;
		int m_sumSizeOfMotions;
		int m_sumNumSolverResults;

	public:

		inline virtual void addConstraintInfo( hkConstraint* constraint, hkConstraintInfo& delta )
		{
			m_maxSizeOfJacobians = hkMath::max2( m_maxSizeOfJacobians, delta.m_maxSizeOfJacobians);
			m_maxSizeOfJacobians = hkMath::max2( m_maxSizeOfJacobians, delta.m_sizeOfJacobians);
			m_sumSizeOfJacobians  += delta.m_sizeOfJacobians;
			m_sumSizeOfSchemas    += delta.m_sizeOfSchemas;
			m_sumNumSolverResults += delta.m_numSolverResults;
		}

		inline virtual void subConstraintInfo( hkConstraint* constraint, hkConstraintInfo& delta )
		{
			m_sumSizeOfJacobians  -= delta.m_sizeOfJacobians;
			m_sumSizeOfSchemas    -= delta.m_sizeOfSchemas;
			m_sumNumSolverResults -= delta.m_numSolverResults;
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

		inline virtual void removeConstraint( hkConstraint* constraint)
		{
			HK_ASSERT2( 0, "Abstract virtual function called" );
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

HK_COMPILE_TIME_ASSERT( sizeof( hkSimpleConstraintOwner ) == 28 );




#endif // HK_DYNAMICS2_SIMPLE_CONSTRAINT_OWNER_H
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
