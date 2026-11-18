/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_CONSTRAINTDRAWER_H
#define HK_CONSTRAINTDRAWER_H

#include <hkutilities2/visualdebugger/viewer/constraint/hkPrimitiveDrawer.h>

/// Base class for constraint drawers

class hkConstraintDrawer 
{
	public:

		//HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_UTILITY);

		hkConstraintDrawer();

			/// Sets the size of lines drawn.
		void setLineLength(hkReal lineLength);

			/// Sets the radius of arcs drawn.
		void setArcRadius(hkReal arcRadius);

			/// Allows the user to up the number of segments used when displaying curved geometry.
		void setNumSegments(int numSegments);

			/// Set the size of the pivots drawn.
		void setPivotSize(hkReal size);


	protected:

		void drawPivots(int tag);
		
		void drawBodyFrames(int tag);

		void updateCommonParameters(hkConstraint* constraint,hkTransform& refLocalToWorld,hkTransform& attLocalToWorld);

		hkReal m_lineLength;
		hkReal m_arcRadius;
		hkReal m_pivotSize;
		hkReal m_arrowSize;
		hkReal m_coneSize;
		hkReal m_planeSize;
		int	   m_numSegments;

		// world positions
		hkVector4 m_bodyAWPos;
		hkVector4 m_bodyBWPos;

		// world positions of pivots
		hkVector4 m_bodyAWPivot;
		hkVector4 m_bodyBWPivot;

		hkRotation m_RA;
		hkRotation m_RB;

		hkPrimitiveDrawer m_primitiveDrawer;
};

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
