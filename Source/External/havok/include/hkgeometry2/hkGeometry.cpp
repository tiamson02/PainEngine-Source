/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgeometry2/hkGeometry.h>


/// Allocates a new hkGeometry object. This class is used to store and retrieve the
/// geometrical representation of all mesh based objects simulated in the physics engine. While
/// it is possible to share geometries with the graphics engine (provided the format
/// is compatible), it is also possible to use a simplified (e.g. convex) version of the
/// geometries and add them to the physical system using this class.
hkGeometry::hkGeometry ()
{
	m_vertex_count	= 0;		// no vertices
	m_vertices		= HK_NULL;
	m_precached_vertex_count=0;
	m_precached_triangle_count=0;


	m_triangle_count = 0;
	m_triangles		 = HK_NULL;

	m_internal_vertex_array		= true;		// be careful with this.
	m_internal_triangle_array   = true;		// be careful with this.
}

/// Deletes all allocated arrays of hkTriangleIndices and Vector4s (vertices).
hkGeometry::~hkGeometry ()
{

	// deallocate the arrays if we allocated them.
	if (m_internal_vertex_array)
	{
		hkDeallocate<hkVector4>(m_vertices);
	}

	if (m_internal_triangle_array)
	{
		delete [] m_triangles;
	}
}


/// Clean memory overhead needed during adding of vertices/triangles
int hkGeometry::optimizeGeometry()
{
	// we cannot add a vertex to an external list.
	if (!m_internal_vertex_array)
	{
		return 0;
	}

	if (m_vertex_count<m_precached_vertex_count)
	{
		// allocate enough memory for the new vertices...
		hkVector4* old_list = m_vertices;
		m_vertices = hkAllocate<hkVector4>(m_vertex_count, HK_MEMORY_CLASS_GEOMETRY);
		HK_ASSERT (m_vertices != HK_NULL);
		// copy the existing vertices to the new array.
		hkString::memCpy(m_vertices, old_list, hkSizeOf(hkVector4)*m_vertex_count);
		// deallocate the old_list
		if(old_list)
		{
			hkDeallocate<hkVector4>(old_list);
		}
	}

	if (m_triangle_count<m_precached_triangle_count)
	{
		// allocate enough memory for the new triangles...
		hkTriangleIndices * old_list = m_triangles;
		m_triangles = new hkTriangleIndices [m_triangle_count];
		HK_ASSERT (m_triangles != HK_NULL);
		// copy the existing triangles to the new array.
		hkString::memCpy(m_triangles, old_list, hkSizeOf(hkTriangleIndices)*m_triangle_count);
		// deallocate the old_list
		delete [] old_list;
	}
	
	return (m_precached_vertex_count - m_vertex_count);
}





// Adds the list of vertices to the hkGeometry.
/// \param vertices A pointer to a block of memory containing a number of Vector4s
/// \param count The number of Vector4s in the block of memory pointed to by the vertices parameter
int hkGeometry::addVertices ( const hkVector4* vertices, int count )
{
	HK_ASSERT (m_internal_vertex_array==true);

	// we cannot add a vertex to an external list.
	if (!m_internal_vertex_array)
	{
		return -1;
	}

	if ((m_vertex_count+count)>m_precached_vertex_count)
	{
		// allocate enough memory for the new vertices...
		hkVector4* old_list = m_vertices;
		m_precached_vertex_count=(m_vertex_count + count)*2;

		m_vertices = hkAllocate<hkVector4>(m_precached_vertex_count, HK_MEMORY_CLASS_GEOMETRY);

		HK_ASSERT (m_vertices != HK_NULL);

		// copy the existing vertices to the new array.
		hkString::memCpy(m_vertices, old_list, hkSizeOf(hkVector4)*m_vertex_count);

		// deallocate the old_list
		if(old_list)
		{
			hkDeallocate<hkVector4>(old_list);
		}
	}

	// add in the new vertices
	for ( int i = 0; i < count; i++ )
		m_vertices[m_vertex_count++] = vertices[i];

	return (m_vertex_count - count);
}

// Adds the triangles specified by the array of triangle indices to the hkGeometry.
/// \param triangles A pointer to a block of memory containing a number of hkTriangleIndices
/// \param count The number of hkTriangleIndices in the block of memory pointed to by the triangles parameter
int hkGeometry::addTriangles( const hkTriangleIndices* triangles, int count )
{
	
	HK_ASSERT (m_internal_triangle_array==true);

	// we cannot add a vertex to an external list.
	if (!m_internal_triangle_array)
		return -1;

	if ((m_triangle_count+count)>m_precached_triangle_count)
	{
		// allocate enough memory for the new triangles...
		hkTriangleIndices * old_list = m_triangles;
		m_precached_triangle_count = (m_triangle_count + count)*2;

		m_triangles = new hkTriangleIndices [m_precached_triangle_count];

		HK_ASSERT (m_triangles != HK_NULL);

		// copy the existing triangles to the new array.
		hkString::memCpy(m_triangles, old_list, hkSizeOf(hkTriangleIndices)*m_triangle_count);

		// deallocate the old_list
		delete [] old_list;
	}

	// add in the new triangles
	for ( int i = 0; i < count; i++ )
		m_triangles[m_triangle_count++] = triangles[i];

	return (m_triangle_count - count);
}

/// To save memory and to share geometries with the graphics, this function call
/// replaces the internal vertex  list with a pointer to an external one.
/// Subsequent calls to addVertices will fail.
///
/// \param vertices A pointer to a block of memory containing Vector4s.
/// \param count    The number of Vector4s in the block of memory pointed
///                 to by the vertices parameter.
///
bool hkGeometry::setVertices ( hkVector4* vertices, int count )
{
	HK_ASSERT (vertices != HK_NULL);

	// destroy what was previously allocated.
	if ((m_vertex_count > 0) && (m_internal_vertex_array))
	{
		hkDeallocate<hkVector4>(m_vertices);
	}

	// point to the external vertex array
	m_internal_vertex_array		= false;
	m_vertices					= vertices;
	m_vertex_count				= count;

	return true;	// it worked
}

/// To save memory and to share the hkGeometry from the graphics engine, the function
/// replaces the internal triangle list with a pointer to an external one.
/// Subsequent calls to addTriangle will fail.
///
/// \param triangles A pointer to a block of memory containing hkTriangleIndices.
/// \param count     The number of hkTriangleIndices in the block of memory pointed
///                  to by the triangles parameter.
///
bool hkGeometry::setTriangles ( hkTriangleIndices* triangles, int count )
{
	HK_ASSERT (triangles != HK_NULL);

	// destroy what was previously allocated.
	if ((m_triangle_count > 0) && (m_internal_triangle_array))
		delete [] m_triangles;

	// point to the external triangle array
	m_internal_triangle_array	= false;
	m_triangles					= triangles;
	m_triangle_count			= count;

	return true;	// it worked
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
