/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_PRIMITIVEDRAWER_H
#define HK_PRIMITIVEDRAWER_H

#include <hkdynamics2/constraint/hkConstraint.h>
#include <hkdynamics2/constraint/hkRigidConstraint.h>
#include <hkutilities2/visualdebugger/hkColor.h>
#include <hkutilities2/visualdebugger/hkDisplayHandler.h>

/// Base class for constraint viewers

class hkPrimitiveDrawer 
{
	private:

		hkDisplayHandler* m_displayHandler;

	public:

        hkPrimitiveDrawer();
		
			/// Sets the display handler	
		void setDisplayHandler(hkDisplayHandler* displayHandler);

			/// Displays an oriented point.
		void displayOrientedPoint(const hkVector4& position,const hkRotation& rot,hkReal size, int color, int tag);

			/// Displays an arrow.
		void displayArrow (const hkVector4& startPos, const hkVector4& arrowDirection, const hkVector4& perpDirection, int color, hkReal scale, int tag);
        
			/// Displays a cone.
		void displayCone (hkReal cone_angle, const hkVector4& startPos, const hkVector4& coneAaxis, const hkVector4& perpVector, int numSegments, int color, hkReal coneSize, int tag);

			/// Displays a plane.
		void displayPlane(const hkVector4& startPos, const hkVector4& planeNormal, const hkVector4& vectorOnPlane, int color, hkReal scale, int tag);

			/// Draws a semi circle.
		void drawSemiCircle(const hkVector4& center, hkVector4& normal, 
						    hkVector4& startPerp, hkReal thetaMin, hkReal thetaMax,
							hkReal radius,int numSegments, int color, int tag);
};

#endif // HK_PRIMITIVEDRAWER_H


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
