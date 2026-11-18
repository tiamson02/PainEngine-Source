/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HAVOK_LINEAR_PARAMETRIC_CURVE_H
#define HAVOK_LINEAR_PARAMETRIC_CURVE_H

#include <hkdynamics2/constraint/bilateral/pointtopath/hkParametricCurve.h>

/// Piecewise linear curve class

class hkLinearParametricCurve : public hkParametricCurve
{
	public:

			/// Default constructor.
		hkLinearParametricCurve();

			// ParametricCurve interface

			/// Given a parametric value, map to a point on the curve. 
		void getPoint( hkReal t, hkVector4& pointOnCurve ) const;

			/// Map a parametric value and a point to the nearest point on the curve.  Returns the parametric value.
		hkReal getNearestPoint( hkReal t, const hkVector4& nearPoint, hkVector4& pointOnPath ) const;

			/// Return the normalized tangent at the point on the curve specified by the parametric value.
		void getTangent( hkReal t, hkVector4& tangent ) const;

			/// Return the smallest parametric value that is on the curve.
		hkReal getStart() const;

			/// Return the largest parametric value that is on the curve.
		hkReal getEnd() const;

			/// Return physical length along the curve from the start of the curve to this parametric point.
		hkReal getLengthFromStart( hkReal t ) const;

			/// Return the normalized vector that defines "up" for a path.
		void getBinormal( hkReal t, hkVector4& up ) const;

			/// Returns true if the path is closed.  i.e. the beginning is the end and the end is the beginning.
		bool isClosedLoop() const;

			// hkLinearParametricCurve interface

			/// Add a point to the path.
		void addPoint( hkVector4& p );

			/// Gets the smoothing factor.
			/// Smoothing factor determines how much to smooth out seams between line segments.
		hkReal getSmoothingFactor() const;

			/// Sets the smoothing factor.
			/// Smoothing factor determines how much to smooth out seams between line segments.
		void setSmoothingFactor( hkReal smooth );

            /// Get an array of points to draw.
		void getPointsToDraw(hkArray<hkVector4>& pathPoints) const;

			/// Closes or uncloses the loop.
			/// A closed loop needs to have the last segment overlap the first segment exactly.
		void setClosedLoop( bool closeLoop );

	public:

		hkReal m_smoothingFactor;
		bool m_closedLoop;

		hkArray<hkVector4> m_points;
		hkArray<hkReal> m_distance;

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
