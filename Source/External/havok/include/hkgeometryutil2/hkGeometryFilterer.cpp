/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgeometryutil2/hkGeometryUtil2.h>
#include <hkgeometryutil2/hkGeometryFilterer.h>
#include <hkgeometryutil2/hkGeometryChecker.h>
#include <hkgeometry2/hkTriangle.h>
#include <hkgeometry2/hkTriangleUtil.h>



/// Algorithm: For each vertex, examine distance to all other vertices currently in list of valid vertices
/// (which is empty initially).
/// If distance to at least one is less than closeTolerance, vertex is invalid.
bool HK_CALL hkGeometryFilterer::removeCloseVertices(hkArray<hkVector4> &vertsArray, hkReal closeTolerance)
{
	hkReal closeToleranceSqrd = closeTolerance * closeTolerance;
	hkArray<hkVector4> newVertsArray;

	int numVerts = vertsArray.getSize(); 

	int i;

	for(i = 0; i < vertsArray.getSize(); i++)
	{
		hkVector4 currentVert = vertsArray[i];
		int j;
		bool found = false;

		for(j = 0; j < newVertsArray.getSize(); j++)
		{
				// If it is sufficiently close to any other vertex, it's not valid
			hkVector4 vij; vij.setSub4(vertsArray[i], newVertsArray[j]);
			hkReal dist = vij.lengthSquared3();
			if ( dist < closeToleranceSqrd )
			{
				found = true;
				break;
			}	
		}

		if (!found)
		{
			newVertsArray.pushBack(vertsArray[i]);
		}
	}

		// Sanity: we MUST have at least 4, so if we're about to screw this up, cancel everything!
	if(newVertsArray.getSize() >3)
	{
		vertsArray.clear();
		for(i = 0; i < newVertsArray.getSize(); i++)
		{
			vertsArray.pushBack(newVertsArray[i]);
		}
		return true;
	}
	else
	{
		return false;
	}
}


/// Algorithm: Project all (valid) vertices onto the infinite ray. Record max/min projections, and push back
/// all vertices which are not the corresponding "max"/"min" vertices.
void HK_CALL hkGeometryFilterer::findAllColinearVerts(hkGeometry *g, int v0_index, int v1_index, hkArray<bool> &vertsValid, hkArray<int > &vertsToErase, hkReal distTol)
{
		// This is the minimum distance we allow a vertex to be in order to be classified as being
		// colinear to the line from vertex(v0_index) to vertex(v1_index)
	hkReal tol = distTol;
	
	hkVector4 a = g->getVertex(v0_index);
	hkVector4 b = g->getVertex(v1_index);

	// First get this normalised vector

	hkVector4 d1;
	d1.setSub4( b, a );
	d1.normalize3();

	hkArray<int> possibles;

	int numv = g->getNumVertices();

	int min_index = 0;
	int max_index = 0;
	hkReal min_proj = hkMath::HK_REAL_MAX;
	hkReal max_proj = -hkMath::HK_REAL_MAX;

	int i;
	for(i = 0; i < numv; i++)
	{
			// Only remove if we've not already done so. Bear in mind that we're passed in an array
			// of vertsValid, so this may already have disallowed some vertices even before we start
		if(vertsValid[i])
		{
			// Project this vertex onto v
			hkVector4 v = g->getVertex(i);
			hkVector4 d2;
			d2.setSub4( v, a );
			hkReal proj = d2.dot3(d1);

			hkVector4 p = a;
			p.addMul4( proj, d1 );
			
			hkVector4 vpDiff;
			vpDiff.setSub4( v, p ); 

			hkReal dist = vpDiff.length3();

			if(dist < tol)
			{
				// Then close enough to (infinite) line, so record
				possibles.pushBack(i);

				// compare distance
				if(proj < min_proj)
				{
					min_index = i;
					min_proj = proj;
				}
				if(proj > max_proj)
				{
					max_index = i;
					max_proj = proj;
				}
			}	// end if close enough

		}// end if valid
	}

		// Now we've got a (possibly empty) list of possibles. Push back all but the min and max
	for(i = 0; i < possibles.getSize(); i++)
	{
		if((possibles[i] != min_index) && (possibles[i] != max_index))
		{
			vertsToErase.pushBack(possibles[i]);
		}
	}
}


/// Algorithm: For each edge, find all vertices which are colinear. Remove those which are in between the
/// "ends" of the (extended) edge.
bool HK_CALL hkGeometryFilterer::removeColinearVertices(hkGeometry *g, hkArray<bool> &validVertsArray, hkArray<hkVector4> &newVertsArray, hkReal minEdge, hkReal distTol)
{
	if(validVertsArray.getSize() == 0)
	{
		validVertsArray.setSize(g->getNumVertices(), true);
	}

	hkArray<hkGeomCheckerEdgeIndices> edges;
	hkGeometryChecker::findAllEdges(g,edges);

	
		// This is the Minimum length we allow for an edge to be used as a "rejecting" edge
	hkReal edgetolSqrd = minEdge * minEdge;

	int v0_index, v1_index;

	int j;
	for(int i = 0; i < edges.getSize(); i++)
	{
		hkGeomCheckerEdgeIndices &edge = edges[i];
		v0_index = edge.getVertex(0);
		v1_index = edge.getVertex(1);
	
		hkVector4 v0 = g->getVertex(v0_index);
		hkVector4 v1 = g->getVertex(v1_index);

		hkVector4 v10; v10.setSub4(v1, v0);
		if( v10.lengthSquared3() > edgetolSqrd)
		{
			hkArray<int> vertsToErase;
			findAllColinearVerts(g, v0_index, v1_index, validVertsArray, vertsToErase, distTol);
			for(j=0; j<vertsToErase.getSize(); j++)
			{
				validVertsArray[vertsToErase[j]]= false;
			}
		}

	}

	int numValid = 0;

	
		// Count num valid
	newVertsArray.clear();
	for(j = 0; j < g->getNumVertices(); j++)
	{
		if(validVertsArray[j])
		{
			numValid++;
			newVertsArray.pushBack(g->getVertex(j));
		}
	}

		// Sanity: we MUST have at least 4, so if we're about to screw this up, cancel everything!
	if(newVertsArray.getSize() <4)
	{
		for(j = 0; j < g->getNumVertices(); j++)
		{
			newVertsArray.pushBack(g->getVertex(j));
		}
		return false;
	}
	else
	{
		return true;
	}


}

/// Algorithm: For each vertex, examine all adjacebt faces. If the normals are all within an angle determined by 
/// minDot of the vertex normal (determined by the average of all face normals), remove the vertex.
bool HK_CALL hkGeometryFilterer::removeCoplanarVertices(hkGeometry *g, hkArray<bool> &validVertsArray, hkArray<hkVector4> &newVertsArray, hkReal minDot)
{
	if(validVertsArray.getSize() == 0)
	{
		validVertsArray.setSize(g->getNumVertices(), true);
	}

	// Loop through all vertices. If ANY of the normals of all faces adjacent to this vertex is sufficiently "bent"
	// away from the vertex normal (average), then remove this vertex.

	int numv = g->getNumVertices();
	int numt = g->getNumTriangles();

	int i,j;
	for(i = 0; i < numv; i++)
	{
			// Only remove if we've not already done so. Bear in mind that we're passed in an array
			// of vertsValid, so this may already have disallowed some vertices even before we start
		if(validVertsArray[i])
		{
			hkArray<hkVector4> normals;
			for(j = 0; j < numt; j++)
			{
				int v0_index, v1_index, v2_index;

				v0_index = g->getTriangle(j).getVertex(0);
				v1_index = g->getTriangle(j).getVertex(1);
				v2_index = g->getTriangle(j).getVertex(2);

				if(	(v0_index == i) || (v1_index == i) || (v2_index == i))
				{
					// Also, verts must be valid. Unsure about this check. Do I remove it?
				//	if(	(validVertsArray[v0_index]) && (validVertsArray[v1_index]) && (validVertsArray[v2_index]))
					{
						hkVector4 n;
						const hkTriangleIndices& t = g->getTriangle(j);
						hkTriangleUtil::calcNormal(n, g->getVertex( t.getVertex(0) ), g->getVertex( t.getVertex(1) ), g->getVertex( t.getVertex(2) ) );
						n.normalize3();
						normals.pushBack(n);	
					}
				}
			}

			if(normals.getSize() > 0)
			{
				// Get normal
				int k;
				hkVector4 normal(0,0,0);
				for(k = 0; k < normals.getSize(); k++)
				{
					normal.add4( normals[k] );
				}
				normal.normalize3();
				bool valid = false;

				for(k = 0; k < normals.getSize(); k++)
				{
					hkReal dot = normals[k].dot3(normal);
					if(dot < minDot)
					{
						valid = true;
						break;
					}
				}

				// If invalid, remove
				if(!valid)
				{
					validVertsArray[i] = false;
				}
			}

		}
	}

	int numValid = 0;
		// Count num valid
	newVertsArray.clear();
	for(j = 0; j < g->getNumVertices(); j++)
	{
		if(validVertsArray[j])
		{
			numValid++;
			newVertsArray.pushBack(g->getVertex(j));
		}
	}

		// Sanity: we MUST have at least 4, so if we're about to screw this up, cancel everything!
	if(newVertsArray.getSize() <4)
	{
		for(j = 0; j < g->getNumVertices(); j++)
		{
			newVertsArray.pushBack(g->getVertex(j));
		}
		return false;
	}
	else
	{
		return true;
	}
	
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
