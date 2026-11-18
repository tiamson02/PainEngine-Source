/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgeometryutil2/hkGeometryUtil2.h>
#include <hkgeometryutil2/hkGeometryUtil.h>

#include <hkgeometry2/hkGeometry.h>
#include <hkgeometry2/hkTriangleIndices.h>
#include <hkgeometry2/hkTriangleUtil.h>
#include <hkgeometry2/hkSphere.h>
#include <hkgeometryutil2/hkGeometryFilterer.h>
#include <hkgeometryutil2/hkGeometryChecker.h>

// These functions are defined in qhullinterface.cpp
bool HK_CALL hkQhullComputeConvexHullTriangles(int numpoints, float *points,int *numtris, int **tridata );
bool HK_CALL hkQhullComputePlanesFromVertices(const float *vertices,int numverts, int vertexStriding,hkArray<hkVector4>& planeEquations, hkArray<hkVector4>& usedVertices);


/// This method creates a convex hull about the vertex array specified and returns a pointer to
/// the generated hkGeometry. If the convex hull code fails, it returns HK_NULL. See convexHull3D().\n
///
/// This threshold has no effect unless the optimize parameter is set to true.
///
/// \param verts       A block of memory containing hkVector4s.
/// \param vertexCount The number of hkVector4s in the block of memory pointed to by the vertexCount
///                    parameter.
/// \param optimize    If true, strip duplicate vertices.
/// \param threshold   The minimum distance between vertices, else they are considered duplicates.
///
hkGeometry* HK_CALL hkGeometryUtil::createConvexGeometry(const hkVector4* verts, const int vertexCount, bool optimize, hkReal threshold)
{

	hkArray<hkVector4> vertsArray;
	for(int i = 0; i < vertexCount; i++)
	{
		vertsArray.pushBack(verts[i]);
	}

		// First remove verts too close to each other if optimization required
	if(optimize)
	{
		hkGeometryFilterer::removeCloseVertices(vertsArray, threshold);
	}

	return createConvexHull(vertsArray);
}


/// \param verts       A block of memory containing hkVector4s.
/// \param vertexCount The number of hkVector4s in the block of memory pointed to by the vertexCount
///                    parameter.
/// \param tolerances  Tolerances passed to the hkGeometryFilterer to determine which vertices to ignore.
/// \param coplanarThreshold Threshold determining when it is required to consider the hull impossible to build due to vertices
///                           being coplanar. This allows you to ensure that very "thin" data never returns a valid hkGeometry.
///
hkGeometry* HK_CALL hkGeometryUtil::createFilteredConvexGeometry(const hkVector4* verts, int vertexCount, const hkGeometryFiltererTolerances& tolerances, hkReal coplanarThreshold)
{
		// Fill in array
	hkArray<hkVector4> vertsArray;
	for(int i = 0; i < vertexCount; i++)
	{
		vertsArray.pushBack(verts[i]);
	}

		// Check for complanarity
	if(hkGeometryChecker::isCoplanarSet(vertsArray, coplanarThreshold))
	{
		return HK_NULL;
	}


	// First remove verts too close to each other
	hkGeometryFilterer::removeCloseVertices(vertsArray, tolerances.m_minAllowableVertexDistance);

	hkGeometry* tempGeom = createConvexHull(vertsArray);
	if(tempGeom == HK_NULL)
	{
		// Not enough verts after removeclose
		return HK_NULL;
	}


		// Then remove colinear verts (on edges)
	hkArray<bool> validVertsArray;


		// Then remove coplanar verts (on edges). Must do this FIRST (before removeColinearVertices)
		// or some edges may be rejected, which would cause otherwise "interior" coplanar vertices
		// (definitely NOT on the hull) to become exterior (on the HULL), but we would still reject
		// them with this test, and the hull may be decimated excessively.
	vertsArray.clear();
	bool cpvResult = hkGeometryFilterer::removeCoplanarVertices(tempGeom, validVertsArray, vertsArray, tolerances.m_minAllowableFaceNormalDot);
	if(!cpvResult)
	{
		// Not enough verts after removecoplanar
		delete tempGeom;
		return HK_NULL;
	}


	vertsArray.clear();
	bool clvResult = hkGeometryFilterer::removeColinearVertices(tempGeom, validVertsArray, vertsArray, tolerances.m_minRejectionEdgeLength, tolerances.m_colinearVertexThreshold);
	if(!clvResult)
	{
		// Not enough verts after removecolinear
		delete tempGeom;
		return HK_NULL;
	}


	// Otherwise, we're fine, so delete the current goemetry and rebuild from the (filtered) set of vertices.
	delete tempGeom;


	return createConvexHull(vertsArray);

}

/// \param (strided) vertices			Array of vertices of size numPoints (size of each element is vertexStriding) to bound with a 3-dimensional convex hull. Must contain AT LEAST 4 non-coplanar vertices.
///
hkGeometry* HK_CALL hkGeometryUtil::createConvexHull(const float* vertices, int numPoints, int vertexStriding)
{

		// If insufficient data to build 3D hull, return immediately.
	if(numPoints < 4)
	{
		return HK_NULL;
	}
	// Fill in float array:
	float *points = (float*) hkAllocate<unsigned char>( numPoints * 3 * hkSizeOf(float), HK_MEMORY_CLASS_GEOMETRY) ;
	int i;
	int floatstriding = vertexStriding/hkSizeOf(float);
	for(i = 0; i < numPoints; i++)
	{
		points[3*i] = vertices[i*floatstriding+0];
		points[3*i+1] = vertices[i*floatstriding+1];
		points[3*i+2] = vertices[i*floatstriding+2];
	}
		// Call QHull
	int numTris;
	int *triData;
	bool result = hkQhullComputeConvexHullTriangles(numPoints, points, &numTris, &triData);

	if(!result)
	{
		hkDeallocate<unsigned char>( (unsigned char*) points);
		return HK_NULL;
	}

		// Otherwise, data is cool, so build a geom from it.

		// First get all triangles
	hkArray<hkTriangleIndices> triArray;
	for (i = 0; i < numTris; i++)
	{
			// Notice: Winding is the other way for QHull!
		hkTriangleIndices tri(triData[3*i], triData[3*i+2], triData[3*i+1]);
		triArray.pushBack(tri);
	}

	hkDeallocate<unsigned char>( (unsigned char*) points );	// Allocated by us
	hkDeallocate<unsigned char>( (unsigned char*) triData );	// Allocated by hkQhullComputeConvexHullTriangles();

		// Now remove unsed vertices
	hkArray<hkVector4> newVerts;
	hkArray<hkTriangleIndices> newTris;
	removeUnusedVertices(vertices, numPoints, vertexStriding, triArray, newVerts, newTris);

		// Finally add all vertices and triangles
	hkGeometry* body = new hkGeometry();
	body->addVertices(&(newVerts[0]), newVerts.getSize());
	body->addTriangles(&(newTris[0]), newTris.getSize());

	return body;


}


/// \param vertsArray       Array of vertices to bound with a 3-dimensional convex hull. Must contain AT LEAST 4 non-coplanar vertices.
///
hkGeometry* HK_CALL hkGeometryUtil::createConvexHull(hkArray<hkVector4>& vertsArray)
{
	return hkGeometryUtil::createConvexHull(&(vertsArray[0](0)), vertsArray.getSize(), sizeof(hkVector4));
}

void HK_CALL hkGeometryUtil::removeUnusedVertices(const float* inverts, int numverts, int vertexStriding, const hkArray<hkTriangleIndices>& inTrianglesArray, hkArray<hkVector4>& outVertsArray, hkArray<hkTriangleIndices>& outTrianglesArray)
{

	hkArray<bool> vertsUsed;
	vertsUsed.setSize(numverts,false);

	outVertsArray.clear();
	outTrianglesArray.clear();

	int numTris = inTrianglesArray.getSize();

	for (int i = 0; i < numTris; i++)
	{
		const hkTriangleIndices& tri = inTrianglesArray[i];
		outTrianglesArray.pushBack(tri);

		vertsUsed[tri.getVertex(0)] = true;
		vertsUsed[tri.getVertex(1)] = true;
		vertsUsed[tri.getVertex(2)] = true;
	}


	// Now we have a true/false array of whether each vertex is used or not.
	// We can thus add all the ones we do use, and rewrite the indices in the
	// face array as we go.

	int new_index=0;
	int floatstriding = vertexStriding/hkSizeOf(float);
	for (int k = 0; k < numverts; k++)
	{
		if(vertsUsed[k])
		{
			hkUint32 old_index = (hkUint32)k;


			// Add vertex
			outVertsArray.pushBack(hkVector4(inverts[k*floatstriding ],inverts[k*floatstriding +1],inverts[k*floatstriding +2]));

			// Rewrite all faces which use this vertex
			for (int j = 0; j < numTris; j++)
			{
				// Rewrite indices
				// We'd like to do this in three lines, but can't due to accessors for hkTriangleIndices
				hkUint32 vertexIndices[3];
				vertexIndices[0] = outTrianglesArray[j].getVertex(0);
				vertexIndices[1] = outTrianglesArray[j].getVertex(1);
				vertexIndices[2] = outTrianglesArray[j].getVertex(2);

				if(vertexIndices[0] == old_index)	{ vertexIndices[0] = new_index; }
				if(vertexIndices[1] == old_index)	{ vertexIndices[1] = new_index; }
				if(vertexIndices[2] == old_index)	{ vertexIndices[2] = new_index; }

				outTrianglesArray[j].setTriangleIndices(vertexIndices);


			}
			new_index++;
		}
	}
}
/// \param inVertsArray       Input array of vertices.
/// \param inTrianglesArray       Input array of triangles.
/// \param outVertsArray       Output array of vertices.
/// \param outTrianglesArray       Output array of vertices.
///
void HK_CALL hkGeometryUtil::removeUnusedVertices(const hkArray<hkVector4>& inVertsArray,const hkArray<hkTriangleIndices>& inTrianglesArray, hkArray<hkVector4>& outVertsArray, hkArray<hkTriangleIndices>& outTrianglesArray)
{
	removeUnusedVertices(&inVertsArray[0](0),inVertsArray.getSize(),sizeof(hkVector4),inTrianglesArray,outVertsArray, outTrianglesArray);
}

/// Checks that each triangle is facing away from all other vertices in the geometry. Returns true if hkGeometry is convex, false otherwise.
/// \param geom The geometry to be tested for convexity
/// \param tolerance The tolerance for convexity (to allow for numerical error when many faces are coplanar)
bool HK_CALL hkGeometryUtil::isApproximatelyConvex(const hkGeometry *geom, hkReal tolerance)
{
	int i = 0;	// Iterate through triangles...
	for (i = 0; i < geom->getNumTriangles(); i++)
	{
		const hkTriangleIndices& tri = geom->getTriangle(i);

		hkVector4 normal;
		hkTriangleUtil::calcNormal( normal, geom->getVertex( tri.getVertex(0) ), geom->getVertex( tri.getVertex(1) ), geom->getVertex( tri.getVertex(2) ));

			// Get the tri normal. Normalize is not necessary, but helps accuracy.
		normal.normalize3();

			// Get the distance between triangle and origin (perpendicular to tri face).
		hkReal distance = normal.dot3( geom->getVertex(tri.getVertex(0)) );

		// Check that each vertex is closer to the origin (along normal)
		for (hkUint32 j = 0; j < (hkUint32)geom->getNumVertices(); j++)	// Cast valid since num vertices MUST be >=0
		{
				// If j is not actually one of this triangles vertices...
			if( (j != tri.getVertex(0)) && (j != tri.getVertex(1)) && (j != tri.getVertex(2)) )
			{
				const hkReal vertexDist = geom->getVertex(j).dot3(normal);
				if ((hkMath::fabs(vertexDist - distance) > tolerance) && ( vertexDist > distance ))
				{
					return false;
				}
			}
		}
	}

	return true;
}

// Returns the mean (average) value of (a random point on) the triangles given, as well as the
// covariance (spread of values).
///
/// \param verts    A block of memory containing the vertices.
/// \param tri      A block of memory containing the triangle indices.
/// \param triCount The number of Triangles.
/// \param cov      The covariance (output).
/// \param mu       The average (output).
/// \param perturb  Use perturb = true to slightly perturb the area calculation.  This
///                 may help create an asymmetric distribution from symmetric data, which
///                 is useful when extracting the eigenvectors which are used to pick OBB
///                 extents.
///
void HK_CALL hkGeometryUtil::getMeanAndCovariance(const hkVector4* verts, const hkTriangleIndices* tri, const int triCount, hkMatrix3 &cov, hkVector4 &mu,bool perturb)
{

	hkReal Ah = 0.0f;		// Area of convex hull
	hkReal Ak;					// Area of triangle i

	int numTris=triCount;

	HK_ASSERT(numTris!=0);

	hkVector4 mh;
	mh.setZero4();								// Centroid of convex hull
	hkVector4 * m = new hkVector4[numTris];		// Centroid of triangle;

	cov.setZero();								// The covariance matrix.


	// Use Knuth Lewis quick and dirty rand ( Num. Recip. p284) Compiler messes up unless you use the ? operator, SO LEAVE IT IN
	hkInt32 idum=1238221;

//#define MEANCOV_KNUTH_RAND (idum=1664525L*idum+1013904223L,((idum?idum:idum) & RAND_MAX)*(1.0f/(RAND_MAX)))
#define MEANCOV_KNUTH_RAND hkMath::rand01()

	for ( int k = 0; k < numTris; k++ )
	{
	//	hkcout << "cov: " << cov << hkendl;
		const hkTriangleIndices & t = tri[k];
		const hkVector4 & p = verts[t.getVertex (0)];
		const hkVector4 & q = verts[t.getVertex (1)];
		const hkVector4 & r = verts[t.getVertex (2)];

		if(perturb)
		{
			hkVector4 randvec(MEANCOV_KNUTH_RAND,MEANCOV_KNUTH_RAND,MEANCOV_KNUTH_RAND);
			//	Ak = 0.5f * (p - q + randvec * 0.01f).cross (p - r).length ();		// area of this triangle.

			hkVector4 vecA;
			vecA.setMul4(.01f, randvec);
			vecA.add4(p);
			vecA.sub4(q);

			hkVector4 vecB;
			vecB.setSub4(p, r);

			hkVector4 cross;
			cross.setCross(vecA, vecB);
			Ak = hkReal(cross.length3()) * .5f;
		}
		else
		{
			// Ak = 0.5f * (p - q).cross (p - r).length ();		// area of this triangle.

			hkVector4 vecA;
			vecA.setSub4(p, q);

			hkVector4 vecB;
			vecB.setSub4(p, r);

			hkVector4 cross;
			cross.setCross(vecA, vecB);
			Ak = hkReal(cross.length3()) * .5f;

		}
		if ( Ak > 0 )
		{
			Ah += Ak;										// total area of hull

			hkVector4 tmpV;
			tmpV.setAdd4(p,q);
			tmpV.add4(r);
			tmpV.mul4(1.f / 3.f);

			m[k] = tmpV;		// centroid.

			// mh += Ak * m[k];						// weighted centroid of convex hull.
			hkVector4 addVec;
			addVec.setMul4(Ak, m[k]);
			mh.add4(addVec);

			// Calculate covariance matrix C (C is symmetric, so this can be sped up)
			for ( int i = 0; i < 3; i++ )
				for ( int j = 0; j < 3; j++ )
//					cov.elt (i,j) += Ak * ( 9*m[k](i)*m[k](j) + p(i)*p(j) + q(i)*q(j) + r(i)*r(j) );
					cov(i,j) += Ak * ( 9*m[k](i)*m[k](j) + p(i)*p(j) + q(i)*q(j) + r(i)*r(j) );
		}
		else	// watch out for zero area triangles.
		{
//			m[k] = (p + q + r) /3;
			m[k].setAdd4(p, q);
			m[k].add4(r);
			m[k].mul4(1.f / 3.f);

			// Calculate covariance matrix C (C is symmetric, so this can be sped up)
			for ( int i = 0; i < 3; i++ )
				for ( int j = 0; j < 3; j++ )
					cov(i,j) += ( p(i)*p(j) + q(i)*q(j) + r(i)*r(j) );
		}
	}
#undef 	MEANCOV_KNUTH_RAND

#ifdef SUPERDEBUG
		HK_ASSERT(Ah>0.0f);
#endif

		// Possibly we had (several) zero area triangles in which case just assume
		// mean is 0 and covariance is identity. Should we do this?
		// What is the variance/mean of zero? TODO.
	if(Ah<=0.0f)
	{
		mu.setZero4();
		cov.setIdentity();
	}
	else
	{
		mh.mul4(1.f / Ah); // Centroid of convex hull.

	// Finish the covariance matrix.
		for ( int i = 0; i < 3; i++ )
			for ( int j = 0; j < 3; j++ )
				cov(i,j) = cov(i,j) / 12 - mh(i)*mh(j)*Ah;
		mu=mh;
	}

//	hkcout << "Final cov: " << cov << hkendl;

	delete[] m;

}


/// Algorithm is O(N), where N=num of triangles in the geometry
/// It requires an "arbitrary" direction (for a raycast), the value of which is not important if the geometry is closed.
/// If allow_nonclosed=true, it deals with nonclosed geometries, considering a point to be
/// inside if the closest triangle in the given direction is not facing the point
/// \param geom            hkGeometry to test against.
/// \param pt              Point to test.
/// \param use_dir         Ray direction to use for inside/outside test.
/// \param allow_nonclosed Use allow_nonclosed = true to allow testing against a non-closed hkGeometry.
bool HK_CALL hkGeometryUtil::isPointInside(const hkGeometry *geom, const hkVector4& pt, const hkVector4& use_dir, bool allow_nonclosed)
{
	// FOR CONCAVE AND CONVEX
	hkReal closest_left_t=-FLT_MAX, closest_right_t=FLT_MAX;

	bool any_face_right=false, any_face_left=false;
	bool right_in=false,left_in=false;

	for (int f=0; f<geom->getNumTriangles(); f++)
	{
		const hkTriangleIndices &triangle=geom->getTriangle(f);
		hkReal u,v,t;
//		if (triangle.rayIntersect(t,u,v,pt,use_dir,geom->getVertices(),false))
		if (hkTriangleUtil::rayIntersect(t,u,v,pt,use_dir,geom->getVertex(triangle.getVertex(0)),geom->getVertex(triangle.getVertex(1)),geom->getVertex(triangle.getVertex(2)),false))
		{
			if (t>0)
			{
				if (t<closest_right_t)
				{
					any_face_right=true;
					closest_right_t=t;
					right_in=!(hkTriangleUtil::inFront(pt, geom->getVertex(triangle.getVertex(0)), geom->getVertex(triangle.getVertex(1)), geom->getVertex(triangle.getVertex(2))));
				}
			}
			else
			{
				if (t>closest_left_t)
				{
					any_face_left=true;
					closest_left_t=t;
					left_in=!(hkTriangleUtil::inFront(pt, geom->getVertex(triangle.getVertex(0)), geom->getVertex(triangle.getVertex(1)), geom->getVertex(triangle.getVertex(2))));
				}
			}
		}
	}

	if (!allow_nonclosed)
	{
		if (!any_face_right) return false;
		if (!any_face_left) return false;
	}

	// Case      x          ->OUT
	if ((!any_face_right)&&(!any_face_left)) return false;

	// Case     [x          ->IN  (allow_nonclosed=true)
	// Case     ]x          ->OUT (allow_nonclosed=true)
	if (!any_face_right) return left_in;

	// Case      x]			->IN  (allow_nonclosed=true)
	// Case      x[			->OUT (allow_nonclosed=true)
	if (!any_face_left) return right_in;

	// Case     [x]			->IN
	if ((right_in)&&(left_in)) return true;
	// Case     ]x[			->OUT
	if ((!right_in)&&(!left_in)) return false;

	// Strange cases
	// Case     ]x]			->IN iff allow_nonclosed;
	// Case     [x[			->IN iff allow_nonclosed;
	if ((right_in)&&(!left_in)) return allow_nonclosed;
	if ((!right_in)&&(left_in)) return allow_nonclosed;

	HK_ASSERT(!"Uncovered case");

	return false;
}

///	The vertex normals are written into the normals array, which may be preallocated, or allocated by the method.
///  If you supply a pre-allocated faceNormals array (of length3 numTriangles)
///  if it is filled with the normals of all the triangles
///
///	\param triangles Must point to an allocated block of hkTriangleIndices of size numTriangles
///	\param vertices  Must point to an allocated block of hkVector4 of size numVertices
/// \param normals   Must either point to an allocated block of hkVector4 of size numVertices, or
///                  may be HK_NULL. If null, memory will be allocated by the method in order to
///                  store the normals, otherwise it must be of sufficient size to hold numTriangles
///                  normals.
///
void HK_CALL hkGeometryUtil::getVertexNormals(const hkTriangleIndices* triangles, int numTriangles, const hkVector4* vertices, int numVertices, hkVector4* normals, hkVector4* triangleNormals)
{
	//
	// allocate face normals space if it is not there
	//

	bool allocatedNormals = false;
	if (triangleNormals == HK_NULL)
	{
		triangleNormals = new hkVector4[numTriangles];
		allocatedNormals = true;
	}

	//
	// compute face normals
	//

	for (int i = 0; i < numTriangles; ++i)
	{
		const hkTriangleIndices* tri = &triangles[i];
		hkVector4 v0Sv1, v1Sv2;
		v0Sv1.setSub4( vertices[tri->getVertex(0)], vertices[tri->getVertex(1)] );
		v1Sv2.setSub4( vertices[tri->getVertex(1)], vertices[tri->getVertex(2)] );
		triangleNormals[i].setCross( v0Sv1, v1Sv2 );

		triangleNormals[i].fastNormalize3();
	}

	//
	// add face normals to vertex normals
	//


	hkString::memSet(normals, 0, numVertices * hkSizeOf(hkVector4));

	for (int j = 0; j < numTriangles; ++j)
	{
		const hkTriangleIndices* tri = &triangles[j];
		const hkVector4* vec = &triangleNormals[j];

		HK_ASSERT(triangles[j].getVertex(0) < (hkUint32)numVertices);	// Cast valid since num vertices MUST be >=0
		HK_ASSERT(triangles[j].getVertex(1) < (hkUint32)numVertices);	// Cast valid since num vertices MUST be >=0
		HK_ASSERT(triangles[j].getVertex(2) < (hkUint32)numVertices);	// Cast valid since num vertices MUST be >=0

		normals[triangles[j].getVertex(0)].add4( *vec );
		normals[triangles[j].getVertex(1)].add4( *vec );
		normals[triangles[j].getVertex(2)].add4( *vec );
	}

	//
	// normalize vertex normals
	//

	for (int k = 0; k < numVertices; ++k)
	{
		normals[k].fastNormalize3();
	}

	//
	// dealocate face normals if necessary
	//

	if (allocatedNormals)
	{
		delete [] triangleNormals;
	}
}


// Example of how to extract results from the hkGeometryCheckerDetails structure.
// N.B. details.m_testsExitedEarly is true if the higher level test
// hkGeometryChecker::isValidPolytope() or hkGeometryChecker::isNondegeneratePolytope() exited
// before performing all of its subtests. In this case details.m_reasonForTestExit
// will hold which test failed.
// details.m_subTestExitedEarly is true if a subtest exited early, before identifying ALL
// problems with the hkGeometry (for that test).
void HK_CALL hkGeometryUtil::parseDetails(const hkGeometryCheckerDetails& details, hkOstream& outputStream)
{
	outputStream << "\n\n";

	if(details.m_testsExitedEarly)
	{
		outputStream << "N.B. Tests exited early because first failure (so information below \n is accurate, but may not not be complete)" << "\n";
	}

	outputStream << "\nREASON(S) for exit: " << "\n";


		// Check polytopness criteria

	if ( details.m_numVertsTest == hkGeometryChecker::FAILED)
	{
		outputStream << "Number of vertices is LESS than 4!" << "\n";
	}

	if ( details.m_vertsUsedThreeTimesTest == hkGeometryChecker::FAILED)
	{
		outputStream << "Some vertices are used less than three times!" << "\n";
	}

	if ( details.m_edgesValidTest == hkGeometryChecker::FAILED)
	{
		outputStream << "Some edges are are not shared exactly twice with correct winding!\n";
	}


	if ( details.m_facesValidTest == hkGeometryChecker::FAILED)
	{
		outputStream << "Some faces are invalid!\n";
	}

	if ( details.m_connectedTest == hkGeometryChecker::FAILED)
	{
		outputStream << "Not connected!\n";
	}


		// Check degeneracy criteria

	if ( details.m_coplanarTest == hkGeometryChecker::FAILED)
	{
		outputStream << "Vertices are coplanar!\n";
	}

	if ( details.m_zeroAreaTriTest == hkGeometryChecker::FAILED)
	{
		outputStream << "Some triangles have zero (or very small) area!\n";
	}

	if (details.m_largeEdgesTest == hkGeometryChecker::FAILED)
	{
		outputStream << "Some edges are too large!\n";
	}


	// Details
	outputStream << "\nDETAILS: \n";

	if(details.m_numVertsTest == hkGeometryChecker::FAILED)
	{
		outputStream << "Number of vertices is actually " << details.m_numVerts << " !\n";
	}

	if(details.m_vertsUsedThreeTimesTest == hkGeometryChecker::FAILED)
	{
		outputStream << "The following vertices are used less than three times:\n";
		for(int i = 0; i < details.m_numTimesVertexUsed.getSize(); i++)
		{
			if(details.m_numTimesVertexUsed[i] < 3)
			{
				outputStream << "\t" << i << " is used " << details.m_numTimesVertexUsed[i] << " time(s)\n";
			}
		}

	}

		if(details.m_edgesValidTest == hkGeometryChecker::FAILED)
	{
		// If didn't exit early, print ALL
		if(!details.m_subTestExitedEarly)
		{
			int i;
			for (i = 0; i < details.m_edges.getSize(); i++)
			{
				if((details.m_facesUsingEdge[i].getSize() != 2) || (details.m_edgesWoundCorrectly[i] == false) )
				{
					outputStream << "Edge not used exactly twice with correct winding\n";
					outputStream << "\tEdge is: " << details.m_edges[i].getVertex(0) << " " << details.m_edges[i].getVertex(1) << "\n";
					outputStream << "\tEdge used " << details.m_facesUsingEdge[i].getSize() << " times by face(s): \n";
					int j;
					for (j = 0; j < details.m_facesUsingEdge[i].getSize(); j++)
					{
						outputStream << "\t" << details.m_facesUsingEdge[i][j] << "\n";
					}
				}
			}
		}
		else
		{
			// Just print the (first) bad one (because it may not yet have enumerated all triangles
			// so it may be impossible to say for the other edges whether they were used twice or not.
			// To be precise: If we exited early because of an edge used >2 times).
			int i = details.m_firstBadEdge;
			outputStream << "Edge not used twice with correct winding\n";
			outputStream << "\tEdge is: " << details.m_edges[i].getVertex(0) << " " << details.m_edges[i].getVertex(1) << "\n";
			outputStream << "\tEdge used " << details.m_facesUsingEdge[i].getSize() << " times by face(s): \n";
			int j;
			for (j = 0; j < details.m_facesUsingEdge[i].getSize(); j++)
			{
				outputStream << "\t" << details.m_facesUsingEdge[i][j] << "\n";
			}
		}
	}

	if(details.m_facesValidTest == hkGeometryChecker::FAILED)
	{
		if(details.m_illdefinedTriangles.getSize() != 0)
		{
			outputStream << "The following faces are ill-defined:\n";
			for(int i = 0; i < details.m_illdefinedTriangles.getSize(); i++)
			{
				outputStream << "\t" << details.m_illdefinedTriangles[i] << "\n";
			}
		}
		if(details.m_duplicatedTriangles.getSize() != 0)
		{
			outputStream << "The following faces are duplicated triangles:\n";
			for(int i = 0; i < details.m_duplicatedTriangles.getSize(); i++)
			{
				outputStream << "\t" << details.m_duplicatedTriangles[i] << "\n";
			}
		}

	}


	if(details.m_connectedTest == hkGeometryChecker::FAILED)
	{
		outputStream << "Number of connected components: " << details.m_numConnectedComponenents << "\n";
	}

	if (details.m_zeroAreaTriTest == hkGeometryChecker::FAILED)
	{
		outputStream << "The following triangles have small area:\n";
		for(int i = 0; i < details.m_zeroAreaTriangles.getSize(); i++)
		{
			outputStream << "\t" << details.m_zeroAreaTriangles[i] << "\n";
		}
	}

	if (details.m_largeEdgesTest == hkGeometryChecker::FAILED)
	{
		outputStream << "The following edges have large length3:\n";
		for(int i = 0; i < details.m_largeEdges.getSize(); i++)
		{
			outputStream << "\tEdge is: " << details.m_largeEdges[i].getVertex(0) << " " << details.m_largeEdges[i].getVertex(1) << "\n";
		}
	}


	outputStream << "\n\n";
}



hkSphere HK_CALL hkGeometryUtil::makeSphere(const hkVector4 &pA, const hkVector4 &pB, const hkReal radiusEpsilon)
{
	hkVector4 diamV, radiusV;
	diamV.setSub4( pB, pA );
	radiusV.setMul4( 0.5f, diamV );

	hkVector4 pr;
	pr.setAdd4( pA, radiusV );

	hkReal radius = radiusV.length3();
	return hkSphere(pr, radius + radiusEpsilon);

}

hkSphere HK_CALL hkGeometryUtil::makeSphere(const hkVector4 &pA, const hkVector4 &pB, const hkVector4 &pC, const hkReal radiusEpsilon)
{
	hkVector4 b,c,bxc;
	b.setSub4( pB, pA );
	c.setSub4( pC, pA );

		// Now solve linear system to get radius vector
	bxc.setCross( b, c );
	hkReal det = bxc.dot3( bxc );

	hkVector4 xrs2, radiusV(0,0,0);
	xrs2.setCross( bxc, b );
	radiusV.addMul4( (c.lengthSquared3()), xrs2 );
	xrs2.setCross( c, bxc );
	radiusV.addMul4( (b.lengthSquared3()), xrs2 );

	hkVector4 pA2 = pA;
	radiusV.setMul4( 0.5f/det, radiusV );
	pA2.add4( radiusV );

	const hkReal rad = radiusV.length3();
	return hkSphere(pA2, rad + radiusEpsilon);

}


hkSphere HK_CALL hkGeometryUtil::makeSphere(const hkVector4 &pA, const hkVector4 &pB, const hkVector4 &pC, const hkVector4 &pD, const hkReal radiusEpsilon)
{
	hkVector4 b,c,d;
	b.setSub4( pB, pA );
	c.setSub4( pC, pA );
	d.setSub4( pD, pA );


	hkReal det =	  b(0) * (c(1) * d(2) - c(2) * d(1))
				- b(1) * (c(0) * d(2) - c(2) * d(0))
				+ b(2) * (c(0) * d(1) - c(1) * d(0));


	hkVector4 xrs, radiusV(0,0,0);
	xrs.setCross( b, c );
	radiusV.addMul4( (d.lengthSquared3()), xrs );
	xrs.setCross( d, b );
	radiusV.addMul4( (c.lengthSquared3()), xrs );
	xrs.setCross( c, d );
	radiusV.addMul4( (b.lengthSquared3()), xrs );

	radiusV.setMul4( 0.5f/det, radiusV );

	hkVector4 pA2;
	pA2.setAdd4( pA, radiusV );

	const hkReal rad = radiusV.length3();
	return hkSphere(pA2, rad + radiusEpsilon);

}


hkSphere HK_CALL hkGeometryUtil::miniSphereRecurseCall(hkVector4* verts, int numVerts, int supportSize, const hkReal radiusEpsilon)
{
	hkSphere currentSphere;

	switch(supportSize)
	{
	case 0:
		{
			// "Empty" sphere, with negative radius ensuring that any vertex automatically gets added to it.
			hkVector4 vZero(0,0,0);
			currentSphere = hkSphere(vZero, -1.0f);
		}
		break;
	case 1:
		// hkSphere with single vertex, and radius equal to epsilon to avoid numerical error.
		currentSphere = hkSphere(verts[-1], radiusEpsilon);
		break;
	case 2:
		currentSphere = makeSphere(verts[-1], verts[-2], radiusEpsilon);
		break;
	case 3:
		currentSphere = makeSphere(verts[-1], verts[-2], verts[-3], radiusEpsilon);
		break;
	case 4:
		currentSphere = makeSphere(verts[-1], verts[-2], verts[-3], verts[-4], radiusEpsilon);
			// Exit since max support is 4
		return currentSphere;
	}

	// Run through every other vertex and consider adding it to the current sphere
	for(int i = 0; i < numVerts; i++)
	{
		hkVector4 vip; vip.setSub4( verts[i], currentSphere.getPosition() );
		hkReal dist = vip.length3();
		if(dist > currentSphere.getRadius())
		{
				// Move this vertex to front
			for(int j = i; j > 0; j--)
			{
				hkVector4 temp = verts[j];
				verts[j] = verts[j - 1];
				verts[j - 1] = temp;
			}

				// Call with 1 more vertex pushed to support
			currentSphere = miniSphereRecurseCall(verts + 1, i, supportSize + 1, radiusEpsilon);
		}
	}

	return currentSphere;
}

/// \param vertexArray A block of memory containing a number of hkVector4s, this is the cloud of points to bound
/// \param vertexCount The number of hkVector4s in the block of memory pointed to by the parameter vertices
/// \param radiusEpsilon To avoid numerical rounding error, the sphere calculated is increased in size by this value.
///                      which should not need to be changed.
///
hkSphere HK_CALL hkGeometryUtil::createMinBoundingSphere(const hkVector4* vertexArray, int vertexCount, const hkReal radiusEpsilon)
{
		// Create array of vertices which we can reorder inside miniSphereRecurseCall()
	hkVector4* array = hkAllocate<hkVector4>(vertexCount, HK_MEMORY_CLASS_GEOMETRY);

	for(int i=0; i < vertexCount; i++)
	{
		array[i] = vertexArray[i];
	}

	hkSphere result = miniSphereRecurseCall(array, vertexCount, 0, radiusEpsilon);

    hkDeallocate<hkVector4>(array);

	return result;
}

// Compute all faces (in plane equation normal+offset format) from the convex hull of the points
// PlaneEquations are hkVector4's with xyz as normal and w component as distance / plane offset.
bool HK_CALL hkGeometryUtil::computePlanesFromVertices(const float *vertices, int numverts, int vertexStriding, hkArray<hkVector4>& planeEquations, hkArray<hkVector4>& usedVertices)
{
	return hkQhullComputePlanesFromVertices(vertices, numverts, vertexStriding, planeEquations, usedVertices);
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
