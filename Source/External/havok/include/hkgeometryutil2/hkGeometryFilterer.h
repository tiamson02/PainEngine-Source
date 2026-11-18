/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//-------------------------------------------------------------------------
//                    Qhull, Copyright (c) 1993-1999
//
//       The National Science and Technology Research Center for
//        Computation and Visualization of Geometric Structures
//                    (The hkGeometry Center)
//                     University of Minnesota
//                          400 Lind Hall
//                      207 Church Street S.E.
//                    Minneapolis, MN 55455  USA
//
//                     email: qhull@geom.umn.edu
//
//This software includes Qhull from The hkGeometry Center.  Qhull is 
//copyrighted as noted above.  Qhull is free software and may be obtained 
//via anonymous ftp from geom.umn.edu.  
//-------------------------------------------------------------------------
//
// Note: This software uses a modified form of QHull
//


#ifndef HK_GEOMETRYUTIL2GEOMETRYFILTERER_H
#define HK_GEOMETRYUTIL2GEOMETRYFILTERER_H

#include <hkgeometry2/hkGeometry.h>


 


/// A useful structure holding tolerances which may be used by the hkGeometryFilterer. 
struct hkGeometryFiltererTolerances
{
		/// This tolerance is used in the removeCloseVertices() filter to remove vertices which are too close to each other
		/// and hence which contribute little to the shape of a geometry.
	hkReal m_minAllowableVertexDistance;

		/// This tolerance is used in the removeColinearVertices() filter to remove vertices which are too close to an edge
		/// on the convex hull and hence which contribute little to the shape of a geometry. 
	hkReal m_minRejectionEdgeLength;
		/// This tolerance is used in the removeColinearVertices() filter to remove vertices which are too close to an edge
		/// on the convex hull and hence which contribute little to the shape of a geometry. 
	hkReal m_colinearVertexThreshold;

		/// This tolerance is used in the removeCoplanarVertices() filter to remove vertices which are too close to a face
		/// on the convex hull and hence which contribute little to the shape of a geometry. 
	hkReal m_minAllowableFaceNormalDot;

	hkGeometryFiltererTolerances() :
		m_minAllowableVertexDistance(0.001f),
		m_minRejectionEdgeLength(0.01f),
		m_colinearVertexThreshold(0.02f),
		m_minAllowableFaceNormalDot(0.992f)
	{
	}
};

/// This class filters out vertices from vertex lists based on proximity, or other surface properties.
/// The tolerances used assume that the hkGeometry is not very large (ie. they are scale-dependent).
class hkGeometryFilterer
{
	public:
			/// Pick out all vertices which are outside "closeTolerance" distance of all
			/// other vertices of the vertices currently picked (recursively).\n
			/// Basically this replaces "groups" of close vertices with a single vertex.
			/// If there are not at least 4 such vertices it returns false.
		static bool HK_CALL removeCloseVertices(hkArray<hkVector4> &vertsArray, hkReal closeTolerance);

			/// Remove all vertices from the list which are within "distTol" distance of
			/// (the infinite line formed by) an edge (which is itself at least "minEdge" in length).\n
			/// This effectively removes vertices which are "on" the edge of a hkGeometry, and as such
			/// contribute nothing to the convex hull.\n
			/// Note: If the validVertsArray is empty, it is resized to hold an element for each vertex,
			/// otherwise it is asssumed to have the right size to represent validity for each vertex in g.\n
			/// Note: Any vertices which are already marked as invalid in the validVertsArray, will be ignored 
			/// (left as invalid). \n
			/// All vertices which are still valid (both from this test and from the validVertsArray)
			/// are then pushed back onto the newVertsArray. If there are not at least 4 vertices it returns false.
		static bool HK_CALL removeColinearVertices(hkGeometry *g, hkArray<bool> &validVertsArray, hkArray<hkVector4> &newVertsArray, hkReal minEdge, hkReal distTol);

			/// Remove all vertices from the list which have all adjacent faces "coplanar".\n
			/// To dermine "coplanarity" we check that all normals aren't pointing the same way
			/// by specifying that their dot products with the vertex normal are greater than "minDot".
			/// This effectively removes vertices which are "on" a face of a hkGeometry, and as such
			/// contribute nothing to the convex hull.\n
			/// Note: If the validVertsArray is empty, it is resized to hold an element for each vertex,
			/// otherwise it is asssumed to have the right size to represent validity for each vertex in g.\n
			/// Note: Any vertices which are already marked as invalid in the validVertsArray, will be ignored 
			/// (left as invalid).\n
			/// The newVertsArray is always cleared initially.\n
			/// All vertices which are still valid (both from this test and from the validVertsArray)
			/// are then pushed back onto the newVertsArray. If there are not at least 4 vertices it returns false.
		static bool HK_CALL removeCoplanarVertices(hkGeometry *g, hkArray<bool> &validVertsArray, hkArray<hkVector4> &newVertsArray, hkReal minDot);


			/// Helper method to find all vertices colinear with the edge (v0_index, v1_index).
			/// The array "vertsToErase" is filled in with all vertices which are less than "distTol"
			/// from the infinite ray through the v0_index'th and v1_index'th vertices of the hkGeometry "g",
			/// except for the furthest in both directions.
		static void HK_CALL findAllColinearVerts(hkGeometry *g, int v0_index, int v1_index, hkArray<bool> &vertsValid, hkArray<int > &vertsToErase, hkReal distTol);


};


#endif // HK_GEOMETRYUTIL2GEOMETRYFILTERER_H

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
