// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM		: ALL
// PRODUCT		: HAVOK_2
// VISIBILITY	: PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

#ifndef HK_CONVEX_PLANEEQUATIONS_UTIL_H
#define HK_CONVEX_PLANEEQUATIONS_UTIL_H

#include <hkmath2/hkMath.h>

class hkConvexVerticesShape;

/// hkConvexPlaneEquationsUtil generates the convex hull planeEquations.
/// Brute force approach has non-realtime performance for bigger sets of vertices (>50),
/// so don't use it in the middle of a realtime simulation.
/// the approximation takes 26 different directions for an approximation. 
/// This approximation can be usefull for very large sets of vertices, where accuracy is not important.
class hkConvexPlaneEquationsUtil
{
	public:

		struct	Tolerances
		{
			Tolerances()
			{
				// this tolerance is using in comparing dotproduct of two normals
				// so it is effectively squared
				m_planeNormalEqualityTolerance = 1e-7f;
				//this distance tolerance checks component-wise
				m_hullDistanceTolerance = 1e-3f;
			};
			hkReal	m_hullDistanceTolerance;
			hkReal	m_planeNormalEqualityTolerance;
		};

			/// calculate the convex hull planeequations from the 'vertices' point cloud
			/// might be slow, so preferable call this offline
		static bool computePlanesFromVertices(const float *vertices, int numverts, int vertexStriding, hkArray<hkVector4>& planeEquations, hkArray<hkVector4>& usedVertices,const Tolerances& tolerance = Tolerances() );
		
			/// approximate planeequations for k evenly distributed directions
		static bool approximatePlanesFromVerticesWithKDop(const float *vertices, int numverts, int vertexStriding, hkArray<hkVector4>& planeEquations, hkArray<hkVector4>& usedVertices);

	protected:

		static bool generateTrianglePlanes(const float *verticesin, int numverts, int vertexStriding, hkArray<hkVector4>& planeEquations, hkArray<hkVector4>& usedVertices, const Tolerances& tolerance );

			/// try to build a planeequation, and if it is valid just add it
		static HK_FORCE_INLINE bool tryAddPlane(
			 hkVector4& triangleSupportingPlaneDirection,
			 hkReal triangleSupportingPlaneConstant,
			 hkConvexVerticesShape* convexShape, 
			 const Tolerances& tolerances, 
			 hkArray<hkVector4>& planeEquations );

};

#endif //HK_CONVEX_PLANEEQUATIONS_UTIL_H

