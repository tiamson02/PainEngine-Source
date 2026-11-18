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

#ifndef HK_GEOMETRYUTIL2GEOMETRYCHECKER_H
#define HK_GEOMETRYUTIL2GEOMETRYCHECKER_H

#include <hkgeometry2/hkGeometry.h>



/// This class is used by the hkGeometry Checker to store (ordered) edges for various tests.\n
/// The vertices are usually stored with v1 < v2, and the "flipped" member tells whether 
/// the edge is actually (v1,v2) or (v2,v1). 
/// NOTE: The overloaded == operator equates (x,y) with (y,x), but it is possible to check
/// the edges exactly by using the isFlipped() method. This allows us to test whether an ordered edge
/// (x,y) is already in a list of ordered edges as (y,x) using hkfind().
class hkGeomCheckerEdgeIndices
{
public:
		/// Default constructor, set as invalid edge.
	hkGeomCheckerEdgeIndices() :
		m_v1(0),
		m_v2(0),
		m_flipped(false)
	{}
	

		/// Construct given two vertices, and whether edge was flipped or not.
	hkGeomCheckerEdgeIndices(int vtx1, int vtx2, bool f ) :
		m_v1(vtx1),
		m_v2(vtx2),
		m_flipped(false)
	{}


		/// Get ith vertex.
	int getVertex(int i) const
	{
		return (i) ? m_v2 : m_v1;
	}

		/// Get if flipped or not.
	bool isFlipped() const
	{
		return m_flipped;
	}

		/// Set given two vertices, and whether edge was flipped or not. The vertices will not be reordered.
	void setGeomCheckerEdgeIndices(int a, int b, bool f)
	{
		m_v1 = a;
		m_v2 = b;
		m_flipped = f;
	}

		/// Set given two vertices, which will be reordered so that v1 < v2, and the "flipped" member will be
		/// set accordingly.
	void setOrderedGeomCheckerEdgeIndices(int a, int b)
	{
		if(a < b)
		{
			m_v1 = a;
			m_v2 = b;
			m_flipped = false;
		}
		else
		{
			m_v1 = b;
			m_v2 = a;
			m_flipped = true;
		}
	}

		/// Set flipped
	bool setFlipped(bool f)
	{
		m_flipped = f;
        return m_flipped;
	}

private:
	int m_v1;
	int m_v2;
	bool m_flipped;
};


// Need operator== so edge lists could be searched in a hkGeometryChecker.
inline bool HK_CALL operator== (const hkGeomCheckerEdgeIndices& a, const hkGeomCheckerEdgeIndices& b)
{
	return (	((a.getVertex(0) == b.getVertex(0)) && (a.getVertex(1) == b.getVertex(1)))
			||	((a.getVertex(0) == b.getVertex(1)) && (a.getVertex(1) == b.getVertex(0))) );
}

inline bool HK_CALL operator!= (const hkGeomCheckerEdgeIndices& a, const hkGeomCheckerEdgeIndices& b)
{
	return ! (a == b);
}





struct hkGeometryCheckerDetails;

/// This class is a helper class which is not used by the SDK core, but may be useful in determining
/// when Geometries are "bad" in that they will (or are likely to) break the SDK.\n
/// The hkGeometryChecker class contains 8 "Geometry Tests", grouped into 5 "Valid" subtests and 3 "Robustness" subtests.
/// It also has two group tests:
/// isValidPolytope() checks all "Valid" subtests.\n
/// isNondegeneratePolytope() checks all "Valid" and all "Robustness" subtests.\n
/// Results are true/false on success/failure, and additional details are stored in a hkGeometryCheckerDetails
/// structure which is cleared by each of the group tests before use.
/// In addition, an "allowEarlyExit" flag is can be set used to allow exit as soon as a single test has failed,
/// in which case the hkGeometryCheckerDetails structure will only contain partial details.
class hkGeometryChecker
{

	public:

	/// The ReasonForTestExit corresponds to results from the hkGeometryChecker's subtests.
	/// The 8 subtests used are:\n
	/// 1. Do we have at least 4 vertices?\n
	/// 2. Do we use all the vertices supplied in at least 3 triangles?\n
	/// 3. Do all edges occur exactly twice, in opposite directions?\n
	/// 4. Are all triangles well-defined (have 3 different vertices, and unique)?\n
	/// 5. Are we connected?\n
	/// These rules define a valid polytope (it may have very small edges, faces, or thin triangles, but
	/// topologically it is valid). If a hkGeometry fails these tests, it is certainly not right. In addition
	/// the next tests test the "robustness" of the geometry. They require tolerances to be specified in
	/// order to determine success/failure.\n
	/// 6. Are the vertices numerically coplanar?\n
	/// 7. Are some triangles too small?\n
	/// 8. Are some edges very large?
	enum ReasonForTestExit
	{
		NOT_YET_EXITED,
		NUM_VERTS_LESS_THAN_4,
		SOME_VERTICES_USED_LESS_THAN_THREE_TIMES,
		SOME_EDGES_ARE_INVALID,
		SOME_FACES_ARE_INVALID,
		NOT_CONNECTED,

		VERTICES_ARE_COPLANAR,
		SOME_ZERO_AREA_TRIANGLES_EXIST,
		SOME_EDGES_ARE_VERY_LARGE
	};

	/// The "result" of each subtest must be one of the following:\n
	/// NOT_TRIED, PASSED, FAILED.\n
	/// Although the subtest itself may return
	/// true/false, the possibility that a test has not yet been tried must also be catered for.
	enum SubTestResult
	{
		NOT_TRIED,
		PASSED,
		FAILED
	};

		// These are the two main tests:

		/// Is this a valid polytope (topologically correct)?
	static bool HK_CALL isValidPolytope(const hkGeometry *geom, hkGeometryCheckerDetails &details, bool allowEarlyExit = false);
		/// Is this a "robust" polytope?
	static bool HK_CALL isNondegeneratePolytope(const hkGeometry *geom, hkGeometryCheckerDetails &details, bool allowEarlyExit = false);


		// These are the five tests used for "polytopeness":

		/// Does this have at least 4 vertices?
	static bool HK_CALL atLeastFourVertices(const hkGeometry *geom, hkGeometryCheckerDetails &details, bool allowEarlyExit);
		/// Are all vertices used correctly?
	static bool HK_CALL allVerticesUsedCorrectly(const hkGeometry *geom, hkGeometryCheckerDetails &details, bool allowEarlyExit);
		/// Are all edges used correctly?
	static bool HK_CALL edgesUsedExactlyTwice(const hkGeometry *geom, hkGeometryCheckerDetails &details, bool allowEarlyExit);
		/// Are the faces well-defined and unique? 
	static bool HK_CALL facesWellDefinedAndUnique(const hkGeometry *geom, hkGeometryCheckerDetails &details, bool allowEarlyExit);
		/// Is this connected? 
	static bool HK_CALL isConnected(const hkGeometry *geom, hkGeometryCheckerDetails &details, bool allowEarlyExit);


		// These are the three tests for "robustness":

		/// Does this have zero-area triangles?
	static bool HK_CALL noZeroAreaTriangles(const hkGeometry* geom, hkGeometryCheckerDetails &details, bool allowEarlyExit);
		/// Does this have large edges? 
	static bool HK_CALL noLargeEdges(const hkGeometry* geom, hkGeometryCheckerDetails &details, bool allowEarlyExit);
		/// Is this coplanar?
	static bool notCoplanar(const hkGeometry* geom, hkGeometryCheckerDetails &details, bool allowEarlyExit);


		// Helper methods:

		/// Get all edges from triangles.
	static void HK_CALL findAllEdges(const hkGeometry *geom, hkArray<hkGeomCheckerEdgeIndices> &edges);		
		/// Is the set of vertices (numerically) coplanar?
	static bool HK_CALL isCoplanarSet(const hkArray<hkVector4> &vertices, hkReal tolerance);
		
};


















/// The tolerances used in the "robustness" subtests. They should be altered before calling the tests
/// directly, or via hkGeometryChecker::isNondegeneratePolytope().
struct hkGeometryCheckerTolerances
{
		// This tolerance is used in the noZeroAreaTriangles() test.
	hkReal m_minAllowableFaceArea;

		// This tolerance is used in the noLargeEdges() test.
	hkReal m_maxAllowableEdgeLength;

		// This tolerance is used in the notCoplanar() test.
	hkReal m_minAllowableWidth;

	hkGeometryCheckerTolerances() :
		m_minAllowableFaceArea(1e-5f),
		m_maxAllowableEdgeLength(1e4f),
		m_minAllowableWidth(1e-2f)
	{
	}
};

/// The hkGeometryCheckerDetails structure contains information about a hkGeometry after a call to
/// hkGeometryChecker::isValidPolytope() or hkGeometryChecker::isNondegeneratePolytope().\n
/// See the hkGeometryChecker class for the tests which may be performed.\n
/// The amount of information filled in depends on the test called, and the state of the "allowEarlyExit" flag.\n
/// Both tests attempt to perform several subtests, and will pass only only if all are passed.
/// However, if the "allowEarlyExit" flag is set to true for either test then the test will exit as soon as 
/// one of the subtests has failed.\n
/// \n
/// SubTests: \n
/// Each subtest returns true/false, and fills info into the hkGeometryCheckerDetails. The subtests
/// are designed to be called either individually, or in a "batch" of subtests, like the two higher
/// level tests defined above: isValidPolytope() and isNondegeneratePolytope().\n
/// Each subtest has the property that it can be told to "exit early" as soon as it has failed, or
/// it can continue, and provide more information. For example, if the "allowEarlyExit" flag is set to
/// true, the 'edgesValidTest' will return as soon as it finds one edge which is shared incorrectly
/// (incorrect winding or shared more than twice). If the flag is set to false, it will find ALL
/// such bad edges.\n
/// \n
/// On construction, or a call to init() ALL tests are initialised to NOT_TRIED.\n
/// A test result will be set to either PASSED or FAILED once the corresponding test has been called.
/// In addition, the test MAY exit early, in which case it will set the "exitedEarly" flag to true
/// and supply the reason for the exit in the "reasonForExit". This allows you to use the higher
/// level test which call several subtests, and automatically know which subtest caused the higher level test
/// to fail. It alows allows you to determine whether any extra "analysed" results are fully accurate or 
/// whether they are incomplete (as they may be if the test exited early).\n
//  \n
/// This structure also contains input in the m_tolerances member, which may be used by one of
/// the "robustness" subtests.\n
/// \n
/// N.B. the init() method MUST be called to clear all internal methods so that the structure can be reused 
/// in successive calls.
struct hkGeometryCheckerDetails
{
	int m_numVerts;
	hkArray<int>				m_numTimesVertexUsed;
	hkArray<int>		m_zeroAreaTriangles; 

	hkArray<bool>				m_edgesWoundCorrectly;
	hkArray<hkGeomCheckerEdgeIndices>		m_edges;
	hkObjectArray< hkArray<int> >	m_facesUsingEdge;

	hkArray<int>		m_illdefinedTriangles; 
	hkArray<int>		m_duplicatedTriangles; 


	hkArray<hkGeomCheckerEdgeIndices>		m_largeEdges;

	int m_firstBadEdge;
	bool m_badEdges;

	int m_numConnectedComponenents;

	hkGeometryChecker::ReasonForTestExit m_reasonForTestExit:8;
	bool m_testsExitedEarly;
	bool m_subTestExitedEarly;

	hkGeometryChecker::SubTestResult m_numVertsTest:8;
	hkGeometryChecker::SubTestResult m_vertsUsedThreeTimesTest:8;
	hkGeometryChecker::SubTestResult m_edgesValidTest:8;
	hkGeometryChecker::SubTestResult m_facesValidTest:8;
	hkGeometryChecker::SubTestResult m_connectedTest:8;
	hkGeometryChecker::SubTestResult m_zeroAreaTriTest:8;
	hkGeometryChecker::SubTestResult m_largeEdgesTest:8;
	hkGeometryChecker::SubTestResult m_coplanarTest:8;

	hkGeometryCheckerTolerances m_tolerances;



	/// Initialise the structure (by clearing all stored data).\n
	/// N.B. the init() method MUST be called to clear all internal methods so that the structure can be reused 
	/// in successive calls.
	void init()
	{
		m_numTimesVertexUsed.clear();
		m_zeroAreaTriangles.clear();
		m_edgesWoundCorrectly.clear();
		m_edges.clear();
		m_facesUsingEdge.clear();
		m_illdefinedTriangles.clear();
		m_duplicatedTriangles.clear();
		m_largeEdges.clear();

		m_reasonForTestExit = hkGeometryChecker::NOT_YET_EXITED;
		m_testsExitedEarly = false;
		m_subTestExitedEarly = false;

		m_numVerts = 0;

		m_badEdges = false;
		m_firstBadEdge = -1;

		m_numConnectedComponenents = -1;

		m_numVertsTest = hkGeometryChecker::NOT_TRIED;
		m_vertsUsedThreeTimesTest = hkGeometryChecker::NOT_TRIED;
		m_edgesValidTest = hkGeometryChecker::NOT_TRIED;
		m_facesValidTest = hkGeometryChecker::NOT_TRIED;
		m_connectedTest = hkGeometryChecker::NOT_TRIED;
		m_zeroAreaTriTest = hkGeometryChecker::NOT_TRIED;
		m_largeEdgesTest = hkGeometryChecker::NOT_TRIED;
		m_coplanarTest = hkGeometryChecker::NOT_TRIED;
	}

	/// Constructor. Calls init() to initialise the structure (by clearing all stored data).
	hkGeometryCheckerDetails()
	{
		init();
	}
};




#endif // HK_GEOMETRYUTIL2GEOMETRYCHECKER_H

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
