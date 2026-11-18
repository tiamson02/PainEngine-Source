/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DRAWUTIL_H
#define HK_DRAWUTIL_H

#include <hkmath2/hkMath.h>
#include <hkutilities2/visualdebugger/hkColor.h>

/// This class facilitates drawing cones, arrows, planes representing ragdoll limits.
class hkDrawUtil: public hkReferencedObject
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

		hkDrawUtil(hkReal new_scale);

			/// Displays an oriented point.
		void displayOrientedPoint(const hkVector4& position,const hkRotation& rot,hkReal size,unsigned int color);

			/// Displays a point.
		void displayPoint(const hkVector4& position,hkReal size,hkUint32 color);

			/// Displays a line segement.
		void displaySegment(const hkVector4& p1, const hkVector4& p2, hkUint32 color);

			/// Displays a cone.
		void displayCone (hkReal cone_angle, const hkVector4& startPos, const hkVector4& coneAaxis, const hkVector4& perpVector, int numSegments, unsigned color);

			/// Displays a plane.
		void displayPlane (const hkVector4& startPos, const hkVector4& planeNormal, const hkVector4& vectorOnPlane, unsigned color, bool showNormal = true);

			/// Displays an arrow.
		void displayArrow (const hkVector4& startPos, const hkVector4& arrowDirection, const hkVector4& perpDirection, unsigned color);
        
			/// Draws a semi circle.
		void drawSemiCircle(const hkVector4& center, hkVector4& normal, 
						    hkVector4& startPerp, hkReal thetaMin, hkReal thetaMax,
							hkReal radius,int numSegments, 
							unsigned int color);

			/// Gets the scale by which the visual representation of cones, arrows and planes will be scaled.
		hkReal getScale();

			/// Sets the scale by which the visual representation of cones, arrows and planes will be scaled.
		void setScale(hkReal new_scale);
	
	private:
		hkReal m_scale;
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
