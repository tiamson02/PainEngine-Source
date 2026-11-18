/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//----------------------------------------------------------------
//
// The following changes have been made to the QHull Code:
//
// 1. All filenames have been changed from xxx to qhull_xxx
//
// 2. All uses of NULL have been replaced with HK_NULL
//
// 3. All i/o statments have been replaced as follows:
//		fprintf() has been replaced with an empty function FPRINTF_REPLACEMENT
//		fputs() has been replaced with an empty function FPUTS_REPLACEMENT
//		fgets() has been replaced with an empty function FGETS_REPLACEMENT
//		freopen() has been replaced with an empty function FREOPEN_REPLACEMENT
//
// 4. The memory management has been changed to use havok's memory management
//		malloc() has been replaced with the function hkAllocate()
//		free() has been replaced with the function hkDeallocate()
//		The implementations of hkAllocate and hkDeallocate may be found in hkBase/memory
//
// By: Nick Gray
// Date: 19/10/2001
//
// 5. 'char*' has been changed to 'const char*' where appropriate
//      Added declarations for hkQhullComputeConvexHullTriangles and
//      hkQhullComputePlanesFromVertices
// Stephen Kennedy 05/06/2002
//
//----------------------------------------------------------------

#include <hkgeometryutil2/hkGeometryUtil2.h>
#include <hkbase/types/hkBaseTypes.h>


	void HK_CALL FPRINTF_REPLACEMENT( void *stream, const char *bob,...);
		// Create dummy fprintf to ensure QHull doesn't output anything
	void HK_CALL FPRINTF_REPLACEMENT( void *stream, const char *bob,...)
	{
	}
	int HK_CALL FPUTS_REPLACEMENT( const char *string, void *stream );
		// Create dummy fputs to ensure QHull doesn't output anything
	int HK_CALL FPUTS_REPLACEMENT( const char *string, void *stream )
	{
		return 1;
	}
	char* HK_CALL FGETS_REPLACEMENT( const char *string, int n, void *stream );
	char* HK_CALL FGETS_REPLACEMENT( const char *string, int n, void *stream )
	{
		return HK_NULL;
	}

	void* HK_CALL FREOPEN_REPLACEMENT( const char *filename, const char *mode, void *stream);
	void* HK_CALL FREOPEN_REPLACEMENT( const char *filename, const char *mode, void *stream)
	{
		return HK_NULL;
	}

	// Need this to get hull
#include <hkgeometryutil2/qhull/qhull_qhull.h>

	// Need this to iterate through result data
#include <hkgeometryutil2/qhull/qhull_qset.h>
#include <hkgeometryutil2/qhull/qhull_poly.h>

char qh_version[] = "computeConvexHullTriangles 3.1 2001/10/19";  // used for error messages

bool HK_CALL hkQhullComputeConvexHullTriangles(int numpoints, float *points,int *numtris, int **tridata );
bool HK_CALL hkQhullComputePlanesFromVertices(const float *vertices,int numverts, int vertexStriding,hkArray<hkVector4>& planeEquations);

// Compute convex hull from array (block) of vertices, and write to array (block) of triangleindices.
bool HK_CALL hkQhullComputeConvexHullTriangles(int numpoints, float *points,int *numtris, int **tridata )
{
	*numtris = 0;
	*tridata = HK_NULL;

	boolT ismalloc = false;   // True if qhull should free points in qh_freeqhull() or reallocation
	char flags[]= "qhull Qt Pp W1e-7 E1e-7"; // option flags for qhull

	int exitcode;             // 0 if no error from qhull

	facetT *facet;	          // set by FORALLfacets
	int curlong, totlong;	  // memory remaining after qh_memfreeshort


	exitcode= qh_new_qhull (3, numpoints, points, ismalloc, flags, HK_NULL, HK_NULL);

	if (!exitcode)
	{
		// 'qh facet_list' contains the convex hull, but we want triangles so triangulate
		qh_triangulate();

		*numtris = qh num_facets;
		*tridata = hkAllocate<int>( *numtris *3, HK_MEMORY_CLASS_GEOMETRY );

		int j=0;

		FORALLfacets
		{
			setT *vertices = qh_facet3vertex (facet);
			vertexT *vertex, **vertexp;

			FOREACHvertex_(vertices)
			{
					// extract vertex index (there should be three, since we've triangulated)
				(*tridata)[j] = qh_pointid(vertex->point);
				j++;
			}

			qh_settempfree(&vertices);
		}
	}


	qh_freeqhull(!qh_ALL);
	qh_memfreeshort (&curlong, &totlong);

	return (exitcode == 0);
}

bool HK_CALL hkQhullComputePlanesFromVertices(const float *orgpoints,int numpoints, int vertexStriding, hkArray<hkVector4>& planeEquations, hkArray<hkVector4>& usedVertices);
// Compute all faces (in plane equation normal+offset format) from the convex hull of the points
// PlaneEquations are hkVector4's with xyz as normal and w component as distance / plane offset.
bool HK_CALL hkQhullComputePlanesFromVertices(const float *orgpoints,int numpoints, int vertexStriding, hkArray<hkVector4>& planeEquations, hkArray<hkVector4>& usedVertices)
{
	boolT ismalloc = false;   // True if qhull should free points in qh_freeqhull() or reallocation
	char flags[] = "qhull n Pp W1e-7 E1e-7"; // option flags for qhull

	int exitcode;             // 0 if no error from qhull

	facetT *facet;	          // set by FORALLfacets
	int curlong, totlong;	  // memory remaining after qh_memfreeshort

	//qhull wants 3 floats per vertex (and not 4)
	float* points = hkAllocate<float>( numpoints*3, HK_MEMORY_CLASS_GEOMETRY );
	int i;
	int floatstriding = vertexStriding/hkSizeOf(float);
	for (i = 0; i < numpoints; i++)
	{
		points[i*3] = orgpoints[i*floatstriding];
		points[i*3+1] = orgpoints[i*floatstriding+1];
		points[i*3+2] = orgpoints[i*floatstriding+2];
	}

	exitcode= qh_new_qhull (3, numpoints, points, ismalloc, flags, HK_NULL, HK_NULL);

	if (!exitcode)
	{
		int numfaces = qh num_facets;
		planeEquations.setSize(numfaces);
		int faceindex = 0;
		FORALLfacets
		{
			planeEquations[faceindex](0) = facet->normal[0];
			planeEquations[faceindex](1) = facet->normal[1];
			planeEquations[faceindex](2) = facet->normal[2];
			planeEquations[faceindex](3) = facet->offset;
			faceindex++;
		}
		HK_ASSERT(faceindex == numfaces);
		vertexT *vertex;
		FORALLvertices {
			if (vertex->deleted)
			{
				continue;
			}
			hkVector4* newVertex = usedVertices.expandBy(1);
			newVertex->set( vertex->point[0], vertex->point[1], vertex->point[2] );
		}
	}

	hkDeallocate(points);

	qh_freeqhull(!qh_ALL);
	qh_memfreeshort (&curlong, &totlong);
	return (exitcode == 0);
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
