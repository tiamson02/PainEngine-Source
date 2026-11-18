/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_PARAMETRIC_CURVE_H
#define HK_PARAMETRIC_CURVE_H

#include <hkmath2/hkMath.h>


class hkParametricCurve : public hkReferencedObject
{

	public:

			/// Given a parametric value map to a point on the curve. 
		virtual void getPoint( hkReal t, hkVector4& pointOnPathOut ) const = 0;

			/// Map a parametric value and a point to the nearest point on the curve.  Returns the parametric value.
		virtual hkReal getNearestPoint( hkReal t, const hkVector4& nearPoint, hkVector4& pointOnPathOut ) const = 0;

			/// Return the normalized tangent at the point on the curve specified by the parametric value.
		virtual void getTangent( hkReal t, hkVector4& tangentOut ) const = 0;

			/// Return the smallest parametric value that is on the curve.
		virtual hkReal getStart() const = 0;

			/// Return the largest parametric value that is on the curve.
		virtual hkReal getEnd() const = 0;

			/// Return physical length along the curve from the start of the curve to this parametric point.
		virtual hkReal getLengthFromStart( hkReal t ) const = 0;

			/// Return the normalized vector that defines "up" for a path.
			/// This is only used by a path constraint if ALL of the angular degrees of freedom are being constrained.
		virtual void getBinormal( hkReal t, hkVector4& up ) const = 0;

			/// Returns true if the path is closed.  i.e. the beginning is the end and the end is the beginning.
		virtual bool isClosedLoop() const = 0;

			/// Return an array of points to draw.
		virtual void getPointsToDraw(hkArray<hkVector4>& pathPoints) const = 0;
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
