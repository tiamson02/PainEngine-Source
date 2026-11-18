/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/hkDynamics2.h>
#include <hkdynamics2/constraint/bilateral/hkConstraintUtils.h>

void HK_CALL calcRelativeAngle( 
	const hkVector4& twistAxisAinWorld, const hkVector4& twistAxisBinWorld, 
	const hkVector4& planeAxisAinWorld, const hkVector4& planeAxisBinWorld,
	hkVector4& axisOut, hkReal& angleOut ) 
{

		// This is the "twist angle calcualtion" code nicked from Havok 1.6.3. 
		// We really have to optimize this, but for the moment it produces behaviour identical to 
		// that in 1.6.3 ( at least in terms of twist limits). Note that the angle is flipped
		// because 1.6.3 calculates the angle the wrong way around, and that atan is used to 
		// calculate the angle, but the jacobian code then takes the sin of this to calculate
		// whether it's inside the limits, so that's two expensive functions!

	hkVector4 twist_axis_ws;
	twist_axis_ws.setAdd4( twistAxisBinWorld , twistAxisAinWorld );
	twist_axis_ws.normalize3();

	hkRotation m_ws_us;
	{
		hkVector4 c0, c1, c2;
		c0 = twist_axis_ws;
		c1.setCross(twist_axis_ws, planeAxisBinWorld);
		c1.normalize3();
		c2.setCross(c1, c0);
		m_ws_us.setCols(c0, c1, c2);
	}
		
	hkVector4 a_us;
	a_us._setRotatedInverseDir(m_ws_us, planeAxisAinWorld);

	hkReal currentTwistAngle  = hkMath::atan2fApproximation( a_us(1), a_us(2));
	
		// Explicitly specify angle calculated above. Need to flip sign
		// because Havok 163 measures the angle the wrong way (of course it does).
	angleOut = currentTwistAngle;
	axisOut = twist_axis_ws;
}
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
