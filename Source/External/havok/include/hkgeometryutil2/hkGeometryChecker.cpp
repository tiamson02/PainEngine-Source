/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgeometryutil2/hkGeometryUtil2.h>
#include <hkgeometryutil2/hkGeometryChecker.h>
#include <hkbase/sort/hkUnionFind.h>




/// The four requirements we have for our polytope are:\n
/// 1. We have at least 4 vertices.\n
/// 2. We use all the vertices supplied, in at least 3 triangles.\n
/// 3. All edges occur exactly twice, in opposite directions.\n
/// 4. We are connected.
bool HK_CALL hkGeometryChecker::isValidPolytope(const hkGeometry *geom, hkGeometryCheckerDetails &details, bool allowEarlyExit)
{
	details.init();

	// First, check at least four vertices used:
	bool atLeastFourVerts = atLeastFourVertices(geom, details, allowEarlyExit);
	if ((!atLeastFourVerts) && (allowEarlyExit))
	{
		details.m_reasonForTestExit = NUM_VERTS_LESS_THAN_4;
		details.m_testsExitedEarly = true;
		return false;
	}

	// Next, check ALL vertices used correctly
	bool allUsed = allVerticesUsedCorrectly(geom, details, allowEarlyExit);
	if ((!allUsed) && (allowEarlyExit))
	{
		details.m_reasonForTestExit = SOME_VERTICES_USED_LESS_THAN_THREE_TIMES;
		details.m_testsExitedEarly = true;
		return false;
	}

	// Next, check edges used exactly twice (closed)
	bool usedTwice = edgesUsedExactlyTwice(geom, details, allowEarlyExit);
	if ((!usedTwice) && (allowEarlyExit))
	{
		details.m_reasonForTestExit = SOME_EDGES_ARE_INVALID;
		details.m_testsExitedEarly = true;
		return false;
	}

	// Next, check faces well defined and unique
	bool facesValid = facesWellDefinedAndUnique(geom, details, allowEarlyExit);
	if ((!facesValid) && (allowEarlyExit))
	{
		details.m_reasonForTestExit = SOME_FACES_ARE_INVALID;
		details.m_testsExitedEarly = true;
		return false;
	}


	// Next, check connected.
	bool connected = isConnected(geom, details, allowEarlyExit);
	if ((!connected) && (allowEarlyExit))
	{
		details.m_reasonForTestExit = NOT_CONNECTED;
		details.m_testsExitedEarly = true;
		return false;
	}

	if(		(details.m_numVertsTest == PASSED)
		&&	(details.m_vertsUsedThreeTimesTest == PASSED)
		&& 	(details.m_edgesValidTest == PASSED)
		&&	(details.m_facesValidTest == PASSED)
		&& 	(details.m_connectedTest == PASSED))
	{
		return true;
	}

	return false;

}


/// The four requirements we have for our polytope are:\n
/// 1. We have at least 4 vertices.\n
/// 2. We use all the vertices supplied, in at least 3 triangles.\n
/// 3. All edges occur exactly twice, in opposite directions.\n
/// 4. We are connected.\n
/// In addition, the three requirements we have for non-degeneracy are: \n
/// 5. We are not coplanar.\n
/// 6. The triangles are not too small.\n
/// 7. The edges are not too big.\n
/// Each of these three tests requires a tolerance, specified in the details.m_tolerances member.
bool HK_CALL hkGeometryChecker::isNondegeneratePolytope(const hkGeometry *geom, hkGeometryCheckerDetails &details, bool allowEarlyExit)
{
	details.init();
		// First test if it's a valid polytope. This performs the first 4 tests.
	if(!isValidPolytope(geom, details, allowEarlyExit) && (details.m_testsExitedEarly == true) && (allowEarlyExit))
	{
		return false;
	}


	// Next, check for coplanarity:
	bool notCoplanarV = notCoplanar(geom, details, allowEarlyExit);
	if ((!notCoplanarV) && (allowEarlyExit))
	{
		details.m_reasonForTestExit = VERTICES_ARE_COPLANAR;
		details.m_testsExitedEarly = true;
		return false;
	}

	// Next, check for zero-area vertices:
	bool noZeroArea = noZeroAreaTriangles(geom, details, allowEarlyExit);
	if ((!noZeroArea) && (allowEarlyExit))
	{
		details.m_reasonForTestExit = SOME_ZERO_AREA_TRIANGLES_EXIST;
		details.m_testsExitedEarly = true;
		return false;
	}

		// Next, check for very large edges:
	bool noLarge = noLargeEdges(geom, details, allowEarlyExit);
	if ((!noLarge) && (allowEarlyExit))
	{
		details.m_reasonForTestExit = SOME_EDGES_ARE_VERY_LARGE;
		details.m_testsExitedEarly = true;
		return false;
	}


	if(		(details.m_numVertsTest == PASSED)
		&&	(details.m_vertsUsedThreeTimesTest == PASSED)
		&& 	(details.m_edgesValidTest == PASSED)
		&&	(details.m_facesValidTest == PASSED)
		&& 	(details.m_connectedTest == PASSED)
		&&  (details.m_coplanarTest == PASSED)
		&&  (details.m_zeroAreaTriTest == PASSED)
		&&  (details.m_largeEdgesTest == PASSED))
	{
		return true;
	}

	return false;

}



/// Check hkGeometry has at least 4 vertices.\n
/// Algorithm: Query number of vertices using hkGeometry::getNumVertices().
/// N.B. If they details passed in has already been used by a previous call to this test, this method returns false (fails).
/// Use hkGeometryCheckerDetails::init() to clear a hkGeometryCheckerDetails structure.
bool HK_CALL hkGeometryChecker::atLeastFourVertices(const hkGeometry *geom, hkGeometryCheckerDetails &details, bool allowEarlyExit)
{
	// Check that this test has not already been performed using this details.
	if(details.m_numVertsTest != NOT_TRIED)
	{
		HK_ASSERT(0);
		return false;
	}

	details.m_numVerts = geom->getNumVertices();

	if(details.m_numVerts < 4)
	{
		details.m_numVertsTest = FAILED;
		return false;
	}
	details.m_numVertsTest = PASSED;
	return true;

}



/// Check hkGeometry uses all vertices at least three times (in at least three faces).\n
/// Algorithm: Run through all faces and total up number of times each vertex is used.
/// Then run through this array, and make sure every vertex is used at least 3 times.
/// N.B. If they details passed in has already been used by a previous call to this test, this method returns false (fails).
/// Use hkGeometryCheckerDetails::init() to clear a hkGeometryCheckerDetails structure.
bool HK_CALL hkGeometryChecker::allVerticesUsedCorrectly(const hkGeometry *geom, hkGeometryCheckerDetails &details, bool allowEarlyExit)
{
	// Check that this test has not already been performed using this details.
	if(details.m_vertsUsedThreeTimesTest != NOT_TRIED)
	{
		HK_ASSERT(0);
		return false;
	}

	details.m_numTimesVertexUsed.setSize(geom->getNumVertices(),0);

	int numTriangles = geom->getNumTriangles();

	int i;

	for (i = 0; i < numTriangles; i++)
	{
		details.m_numTimesVertexUsed[geom->getTriangle(i).getVertex(0)]++;
		details.m_numTimesVertexUsed[geom->getTriangle(i).getVertex(1)]++;
		details.m_numTimesVertexUsed[geom->getTriangle(i).getVertex(2)]++;
	}

	// Now check array

	int numVertices = geom->getNumVertices();

	for (i = 0; i < numVertices; i++)
	{
		if(details.m_numTimesVertexUsed[i] < 3)
		{
			details.m_vertsUsedThreeTimesTest = FAILED;
			return false;
		}
	}

	details.m_vertsUsedThreeTimesTest = PASSED;
	return true;
}


template <typename T>
T* HK_CALL hkFind(T* a, T* b, const T& t)
{
	while(a!=b)
	{
		if( *a == t)
		{
			return a;
		}
		++a;
	}
	return b;
}

/// Helper function: Finds all edges of a hkGeometry by extracting them three at a time from the
/// face/triangle list. The comparisons are not order-dependent, so each edge is only stored once.\n
/// In addition, the edges are always stored as (x,y) with x < y.
void HK_CALL hkGeometryChecker::findAllEdges(const hkGeometry *geom, hkArray<hkGeomCheckerEdgeIndices> &edges)
{
	int numTriangles = geom->getNumTriangles();

		// Get all edges
	for (int i = 0; i < numTriangles; i++)
	{
		// Get triangle vertex indices
		int v0, v1, v2;

		v0 = geom->getTriangle(i).getVertex(0);
		v1 = geom->getTriangle(i).getVertex(1);
		v2 = geom->getTriangle(i).getVertex(2);

		hkGeomCheckerEdgeIndices edge;

		edge.setOrderedGeomCheckerEdgeIndices(v0, v1);


		hkArray<hkGeomCheckerEdgeIndices>::iterator begin;
		hkArray<hkGeomCheckerEdgeIndices>::iterator end;
		begin = edges.begin();
		end = edges.end();
		if( hkFind(begin, end, edge) == end)
		{
			// New edge!
			edges.pushBack(edge);
		}

		edge.setOrderedGeomCheckerEdgeIndices(v1, v2);


		begin = edges.begin();
		end = edges.end();
		if( hkFind(begin, end, edge) == end)
		{
			// New edge!
			edges.pushBack(edge);
		}

		edge.setOrderedGeomCheckerEdgeIndices(v2, v0);


		begin = edges.begin();
		end = edges.end();
		if( hkFind(begin, end, edge) == end)
		{
			// New edge!
			edges.pushBack(edge);
		}

	}

}

/// Check hkGeometry uses each edge exactly twice, once in each direction.\n
/// Algorithm :\n
/// Maintain a list of ordered edges, with associated list of faces (on which the edge list), and number of times used.
///	For each triangle, extract the edges.\n
///		For each edge:\n
///			If the unordered edge is not in the list, add it, record the face it's on, and set #times used to 1\n
///			If it's already on the list and either #times > 1 OR order is the same, we've an error, so
///				exit early if allowEarlyExit flag set.
///			Otherwise, just increment the #times and add face it's on to appropriate list.\n
///
/// After all triangles have been examined, run through lists and look for either overuse (used > twice) or
/// incorrect winding (used twice, but both have same direction).
/// N.B. If they details passed in has already been used by a previous call to this test, this method returns false (fails).
/// Use hkGeometryCheckerDetails::init() to clear a hkGeometryCheckerDetails structure.
bool HK_CALL hkGeometryChecker::edgesUsedExactlyTwice(const hkGeometry *geom, hkGeometryCheckerDetails &details, bool allowEarlyExit)
{
	// Check that this test has not already been performed using this details.
	if(details.m_edgesValidTest != NOT_TRIED)
	{
		HK_ASSERT(0);
		return false;
	}

	int numTriangles = geom->getNumTriangles();

	int i;

	hkArray<int> numTimesEdgesUsed;


		// Get all edges
	for (i = 0; i < numTriangles; i++)
	{
		// Get triangle vertex indices
		int v0, v1, v2;

		v0 = geom->getTriangle(i).getVertex(0);
		v1 = geom->getTriangle(i).getVertex(1);
		v2 = geom->getTriangle(i).getVertex(2);


		hkGeomCheckerEdgeIndices edgeList[3];
		hkGeomCheckerEdgeIndices edge;


			// Construct all three possible edges (ordered as [low vert index,high vert index])
		edgeList[0].setOrderedGeomCheckerEdgeIndices(v0, v1);
		edgeList[1].setOrderedGeomCheckerEdgeIndices(v1, v2);
		edgeList[2].setOrderedGeomCheckerEdgeIndices(v2, v0);

		hkArray<hkGeomCheckerEdgeIndices>::iterator begin;
		hkArray<hkGeomCheckerEdgeIndices>::iterator end;
		hkArray<hkGeomCheckerEdgeIndices>::iterator found;

		for(int j = 0; j < 3; j++)
		{
			edge = edgeList[j];


			begin = details.m_edges.begin();
			end = details.m_edges.end();
			found = hkFind(begin, end, edge);
			if(found == end)
			{
				// New edge!
				details.m_edges.pushBack(edge);
				numTimesEdgesUsed.pushBack(1);

				details.m_edgesWoundCorrectly.pushBack(true);

				// Push back face it's used in
				hkArray<int> newFaceList;
				newFaceList.pushBack(i);
				details.m_facesUsingEdge.pushBack(newFaceList);

			}
			else
			{
				// Old edge, so increment number of times used
				numTimesEdgesUsed[found - begin] ++;
				details.m_facesUsingEdge[found - begin].pushBack(i);
				if(edge.isFlipped() == found->isFlipped())
				{
					details.m_edgesWoundCorrectly[found - begin] = false;
				}



				if( (numTimesEdgesUsed[found - begin] > 2) || (details.m_edgesWoundCorrectly[found - begin] == false) )
				{

					// We definitely HAVE a bad edge here, so we can exit early
					// (However, we won't catch the other case (edge ONLY used once) here, so we
					// catch this later on.)
					if(allowEarlyExit)
					{
						details.m_subTestExitedEarly =  true;
						details.m_badEdges = true;
						details.m_firstBadEdge = found - begin;
						details.m_edgesValidTest = FAILED;
						return false;
					}
				}
			}	// end examinination of new edge
		}	// loop through 3 pairs of verts

	}


	// Now go through list of edges to test for overuse or incorrect winding
	for (i = 0; i < numTimesEdgesUsed.getSize(); i++)
	{
		if(numTimesEdgesUsed[i] != 2)
		{
			if(allowEarlyExit)
			{
				details.m_subTestExitedEarly =  true;
				details.m_firstBadEdge = i;
			}
			details.m_badEdges = true;
			details.m_edgesValidTest = FAILED;
			return false;
		}
		else
		{
			// Check for incorrect winding (this will have been caught before if early exit)
			if(details.m_edgesWoundCorrectly[i] == false)
			{
				details.m_badEdges = true;
				details.m_edgesValidTest = FAILED;
				return false;
			}
		}
	}

	details.m_edgesValidTest = PASSED;
	return true;
}


/// Check hkGeometry has face which are the faces well-defined and unique.\n
/// Algorithm: Run through all faces and check vertices are all different, and face has not already been used.
/// N.B. If they details passed in has already been used by a previous call to this test, this method returns false (fails).
/// Use hkGeometryCheckerDetails::init() to clear a hkGeometryCheckerDetails structure.
bool HK_CALL hkGeometryChecker::facesWellDefinedAndUnique(const hkGeometry *geom, hkGeometryCheckerDetails &details, bool allowEarlyExit)
{
	// Check that this test has not already been performed using this details.
	if(details.m_facesValidTest != NOT_TRIED)
	{
		HK_ASSERT(0);
		return false;
	}

	hkArray<hkInt64> triangleHashes;

	int numTriangles = geom->getNumTriangles();
	int numVertices = geom->getNumVertices();

	int i;

		// Loop through all triangles
	for (i = 0; i < numTriangles; i++)
	{
		// Get triangle vertex indices
		int v0, v1, v2;

		v0 = geom->getTriangle(i).getVertex(0);
		v1 = geom->getTriangle(i).getVertex(1);
		v2 = geom->getTriangle(i).getVertex(2);

		// Reorder so that: min < mid < max
		int min, max, mid;
		min = (v0 < v1)? ((v0 < v2)?v0:v2) : ((v1 < v2)?v1:v2);
		max = (v0 > v1)? ((v0 > v2)?v0:v2) : ((v1 > v2)?v1:v2);
		mid = static_cast<unsigned short> ((int)v0 + (int)v1 + (int)v2 - (int)min - (int)max);

			// Check if:
			// 1. They're out of range range
			// 2. They're not distinct
		if( (min >= numVertices) || (mid >= numVertices) || (max >= numVertices) ||
			(min == mid) || (mid == max))
		{
			details.m_illdefinedTriangles.pushBack(i);
			if(allowEarlyExit)
			{
				details.m_facesValidTest = FAILED;
				return false;
			}
		}

			// See if we've already go this face.
		hkArray<hkInt64>::iterator begin;
		hkArray<hkInt64>::iterator end;
		begin = triangleHashes.begin();
		end = triangleHashes.end();

			// Hash the triangle triple
		hkInt64 newTriHash = (hkInt64)min + (hkInt64)mid * 65536 + (hkInt64)max * 65536 * 65536;

		if( hkFind(begin, end, newTriHash) == end)
		{
			triangleHashes.pushBack(newTriHash);
		}
		else
		{
			details.m_duplicatedTriangles.pushBack(i);
			if(allowEarlyExit)
			{
				details.m_facesValidTest = FAILED;
				return false;
			}
		}
	}

	if((details.m_illdefinedTriangles.getSize() > 0) || (details.m_duplicatedTriangles.getSize() > 0))
	{
		details.m_facesValidTest = FAILED;
		return false;
	}

	details.m_facesValidTest = PASSED;
	return true;
}



/// Check hkGeometry is connected.\n
/// Algorithm: Use UnionFind to split vertices up into connected sets. Count number of sets. This should be 1.
/// N.B. If they details passed in has already been used by a previous call to this test, this method returns false (fails).
/// Use hkGeometryCheckerDetails::init() to clear a hkGeometryCheckerDetails structure.
bool HK_CALL hkGeometryChecker::isConnected(const hkGeometry *geom, hkGeometryCheckerDetails &details, bool allowEarlyExit)
{
	// Check that this test has not already been performed using this details.
	if(details.m_connectedTest != NOT_TRIED)
	{
		HK_ASSERT(0);
		return false;
	}

	hkArray<hkGeomCheckerEdgeIndices> geomEdges;
	findAllEdges(geom, geomEdges);


	hkArray<hkUnionFind::Edge> edges;

	int numEdges = geomEdges.getSize();
			// Get all edges
	for (int i = 0; i < numEdges; i++)
	{
		hkGeomCheckerEdgeIndices &edge = geomEdges[i];

		hkUnionFind::Edge newEdge;
		newEdge.m_first = static_cast<int>(edge.getVertex(0));
		newEdge.m_second = static_cast<int>(edge.getVertex(1));
		edges.pushBack(newEdge);

//		edges.pushBack( hk_make_pair(static_cast<int>(edge.getVertex(0)), static_cast<int>(edge.getVertex(1)) ) );
	}

	hkUnionFind unionFind;
	unionFind.setEdges(edges, geom->getNumVertices());

	int numgroups = 0;
	hkArray<int> ivec;
	while(unionFind.getNextGroup(ivec))
	{
		numgroups++;
	}

	details.m_numConnectedComponenents = numgroups;


	if(numgroups != 1)
	{
		details.m_connectedTest = FAILED;
		return false;
	}

	details.m_connectedTest = PASSED;
	return true;

}

/// Check hkGeometry has no zero area triangles.\n
/// Algorithm:\n
/// For each triangle, calculate area.\n
///	If < tol, record the bad triangle.\n
///	If allowEarlyExit, we're done, otherwise, continue.\n
///
/// If we've recorded any "zero" area triangles, the test returns false.
/// N.B. If they details passed in has already been used by a previous call to this test, this method returns false (fails).
/// Use hkGeometryCheckerDetails::init() to clear a hkGeometryCheckerDetails structure.
bool HK_CALL hkGeometryChecker::noZeroAreaTriangles(const hkGeometry* geom, hkGeometryCheckerDetails &details, bool allowEarlyExit)
{
	// Check that this test has not already been performed using this details.
	if(details.m_zeroAreaTriTest != NOT_TRIED)
	{
		HK_ASSERT(0);
		return false;
	}

	details.m_zeroAreaTriangles.clear();

	int numTriangles = geom->getNumTriangles();

		// Instead of checking area = (0.5 * e1.cross(e2).length()) < tol,
		// we'll check e1.cross(e2).lengthSquared3() < 4 * tol*tol
	hkReal minVal = 4 * details.m_tolerances.m_minAllowableFaceArea * details.m_tolerances.m_minAllowableFaceArea;

	int i;
	for (i = 0; i < numTriangles; i++)
	{
		const hkVector4& v1 = geom->getVertex(geom->getTriangle(i).getVertex(0));
		const hkVector4& v2 = geom->getVertex(geom->getTriangle(i).getVertex(1));
		const hkVector4& v3 = geom->getVertex(geom->getTriangle(i).getVertex(2));

		hkVector4 e1; e1.setSub4(v2,v1);
		hkVector4 e2; e2.setSub4(v3,v1);

		// Area of parallelogram
		hkVector4 crossP;
		crossP.setCross( e1, e2 );

		// If triangle has negligable area
		if (crossP.lengthSquared3() < minVal)
		{
			details.m_zeroAreaTriangles.pushBack(i);

			if(allowEarlyExit)
			{
				details.m_subTestExitedEarly =  true;
				details.m_zeroAreaTriTest = FAILED;
				return false;
			}
		}
	}

	if (details.m_zeroAreaTriangles.getSize() > 0)
	{
		details.m_zeroAreaTriTest = FAILED;
		return false;
	}

	details.m_zeroAreaTriTest = PASSED;
	return true;

}

/// Check hkGeometry has no very large edges.\n
/// Algorithm:\n
/// For each triangle, examine all three edges.\n
///	If > tol, record the bad edge.\n
///	If allowEarlyExit, we're done, otherwise, continue.\n
///
/// If we've recorded any "big" edges, the test returns false.
/// N.B. If they details passed in has already been used by a previous call to this test, this method returns false (fails).
/// Use hkGeometryCheckerDetails::init() to clear a hkGeometryCheckerDetails structure.
bool HK_CALL hkGeometryChecker::noLargeEdges(const hkGeometry* geom, hkGeometryCheckerDetails &details, bool allowEarlyExit)
{
	// Check that this test has not already been performed using this details.
	if(details.m_largeEdgesTest != NOT_TRIED)
	{
		HK_ASSERT(0);
		return false;
	}

	details.m_largeEdges.clear();

	hkReal maxEdgeSqrd = details.m_tolerances.m_maxAllowableEdgeLength * details.m_tolerances.m_maxAllowableEdgeLength;


	hkArray<hkGeomCheckerEdgeIndices>::iterator begin;
	hkArray<hkGeomCheckerEdgeIndices>::iterator end;
	hkArray<hkGeomCheckerEdgeIndices>::iterator found;

	int numTriangles = geom->getNumTriangles();

	int i;
	for (i = 0; i < numTriangles; i++)
	{
		// Get triangle vertex indices
		int v0, v1, v2;

		v0 = geom->getTriangle(i).getVertex(0);
		v1 = geom->getTriangle(i).getVertex(1);
		v2 = geom->getTriangle(i).getVertex(2);

		hkGeomCheckerEdgeIndices edgeList[3];

			// Construct all three possible edges (ordered as [low vert index,high vert index])
		edgeList[0].setOrderedGeomCheckerEdgeIndices(v0, v1);
		edgeList[1].setOrderedGeomCheckerEdgeIndices(v1, v2);
		edgeList[2].setOrderedGeomCheckerEdgeIndices(v2, v0);

		for(int j = 0; j < 3; j++)
		{
			hkVector4 edge;
			edge.setSub4(	geom->getVertex(edgeList[j].getVertex(0)),
							geom->getVertex(edgeList[j].getVertex(1)) );

			hkReal length = edge.lengthSquared3();
			if (length > maxEdgeSqrd)
			{
				// If not already in the list
				begin = details.m_largeEdges.begin();
				end = details.m_largeEdges.end();
				found = hkFind(begin, end, edgeList[j]);
				if(found == end)
				{
					details.m_largeEdges.pushBack(edgeList[j]);

					if(allowEarlyExit)
					{
						details.m_subTestExitedEarly =  true;
						details.m_largeEdgesTest = FAILED;
						return false;
					}
				}
			}
		}
	}

	if (details.m_largeEdges.getSize() > 0)
	{
		details.m_largeEdgesTest = FAILED;
		return false;
	}

	details.m_largeEdgesTest = PASSED;
	return true;

}


/// Helper function. Determines if a point set is coplanar (to within tolerance) or not.\n
/// Find a triangle large enough to ensure computation of a valid normal  (ie. plane). If we can't, we're done.
/// Otherwise, project all vertices onto this normal to find distance to plane, and if any one is off the plane
/// by more than tolerance, the set is not coplanar. If none are, it is.
bool HK_CALL hkGeometryChecker::isCoplanarSet(const hkArray<hkVector4> &vertices, hkReal tolerance)
{
	// We require the following:
	// At least one subset of 4 four vertices which are non-coplanar, within the tolerance specified

	if(vertices.getSize() < 4)
	{
		return true;
	}

	// Only fail if ALL vertices are "coplanar".
	// 1. Find normal of triangle formed by "first valid triple of vertices".
	// This may not be the first three, since they may be equal (within tolerance).
	// So let's find a triple such that each vertex is at least "tolerance" away from the others,
	// and form a valid triangle (form which we'll get a nice robust normal).

	hkReal tolSqrd = tolerance * tolerance;

	int numV = vertices.getSize();

	hkVector4 v0,v1,v2;
	v0 = vertices[0];

	int i = 1 , j = -1 ;
	bool found = false;

		// First get v1 as one which is max distance away from v0
	hkReal min = -1.0f;
	while(i < numV)
	{
		hkVector4 vDiff;
		vDiff.setSub4( vertices[i], v0 );
		hkReal dist = vDiff.lengthSquared3();
		if( (dist > tolSqrd) && (dist > min))
		{
			j = i;
			min = dist;
			v1 = vertices[i];
			found = true;
		}
		i++;

	}

	if(!found)
		return true;

		// Now get v2, which must form a nice triangle.
	i = 1;
	found = false;
	while(i < numV)
	{
		if (i==j)
		{
			i++;
			continue;
		}

		hkVector4 v0Diff, v1Diff;
		v0Diff.setSub4( vertices[i], v0 );
		hkReal d0 = v0Diff.lengthSquared3();
		v1Diff.setSub4( vertices[i], v1 );
		hkReal d1 = v1Diff.lengthSquared3();

		if ( (d0 > tolSqrd) &&	(d1  > tolSqrd))
		{
			v0Diff.normalize3();
			v1Diff.normalize3();
			hkReal a0 = v0Diff.dot3(v1Diff);

			if ( hkMath::fabs(a0) < 0.99f )
			{
				v2 = vertices[i];
				found = true;
				break;
			}
		}
		i++;

	}

	if(!found)
		return true;

	// Now calc normal
	hkVector4 normal;
	hkVector4 vDiff20, vDiff10;
	vDiff20.setSub4( v2, v0 );
	vDiff10.setSub4( v1, v0 );
	normal.setCross( vDiff20, vDiff10 );
	normal.normalize3();

	// Find planar constant by projecting (center of) triangle.
	hkVector4 center = v0;
	center.add4( v1 );
	center.add4( v2 );
	center.setMul4( 1.0f/3.0f, center );
	hkReal centerProj = center.dot3(normal);

	// Now project each vertex. If fabs(dist) > tol, then done (not coplanar)
	i=0;
	bool planar = true;
	while(i < numV)
	{
		hkReal dist = (hkReal)(vertices[i].dot3(normal)) - centerProj;
		if( dist*dist > tolSqrd )
		{
			planar = false;
			break;
		}
		i++;

	}

	return planar;


}

/// Check hkGeometry does not have entirely coplanar vertices.\n
/// Algorithm:\n
/// See isCoplanarSet().
/// N.B. If they details passed in has already been used by a previous call to this test, this method returns false (fails).
/// Use hkGeometryCheckerDetails::init() to clear a hkGeometryCheckerDetails structure.
bool HK_CALL hkGeometryChecker::notCoplanar(const hkGeometry* geom, hkGeometryCheckerDetails &details, bool allowEarlyExit)
{
	// Check that this test has not already been performed using this details.
	if(details.m_coplanarTest != NOT_TRIED)
	{
		HK_ASSERT(0);
		return false;
	}


	hkArray<hkVector4> vertices;

		// Extract vertices
	int numVertices = geom->getNumVertices();

	int i;
	for (i = 0; i < numVertices; i++)
	{
		vertices.pushBack(geom->getVertex(i));
	}

	if(isCoplanarSet(vertices,details.m_tolerances.m_minAllowableWidth))
	{
		details.m_coplanarTest = FAILED;
		return false;
	}

	details.m_coplanarTest = PASSED;
	return true;
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
