/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY2_GEOMETRY_H
#define HK_GEOMETRY2_GEOMETRY_H

#include <hkmath2/hkMath.h>
#include <hkgeometry2/hkTriangleIndices.h>

//class hkTriangleIndices;


/// This class is used to store and retrieve the 
/// geometrical representation of all mesh based objects simulated in the physics engine. While 
/// it is possible to share geometries with the graphics engine (provided the format 
/// is compatible), it is also possible to use a simplified (e.g. convex) version of the 
/// geometries and add them to the physical system using this class.
class hkGeometry
{

	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_GEOMETRY, hkGeometry);

			/// Standard constructor
		hkGeometry();

			/// Standard destructor.
		~hkGeometry();

		//
		// BUILDER METHODS
		//

			/// Inserts copies of the vertices into the hkGeometry.
		int addVertices ( const hkVector4* vertices, int count );


			/// Inserts copies of the triangles into the hkGeometry.
		int addTriangles ( const hkTriangleIndices* triangles, int count );


			/// Replaces the internal vertex list with a pointer to an external one.
		bool setVertices ( hkVector4* vertices, int count );


			/// Replaces the internal triangle list with a pointer to an external one.
		bool setTriangles ( hkTriangleIndices* triangles, int count );

			/// Clean memory overhead needed during adding of vertices/triangles.
		int optimizeGeometry();


		//
		// ACCESSOR METHODS 
		//

			/// Returns the number of vertices in the hkGeometry.
		inline int getNumVertices() const;


			/// Returns the number of triangles in the hkGeometry.
		inline int getNumTriangles() const;


			/// Returns the ith vertex of this hkGeometry.
		inline const hkVector4&	getVertex(int i) const;


			/// Returns the ith triangle of this hkGeometry.
		inline const hkTriangleIndices& getTriangle(int i) const;


			/// Returns the array of vertices representing this hkGeometry.
		inline const hkVector4* getVertices() const;


			/// Returns the array of triangles representing this hkGeometry.
		inline const hkTriangleIndices* getTriangles() const;


	private:

		int				m_precached_vertex_count;
		int				m_precached_triangle_count;

		int				m_vertex_count;

		hkVector4*		m_vertices;

		int				m_triangle_count;

		hkTriangleIndices*		m_triangles;

		bool			m_internal_vertex_array;

		bool			m_internal_triangle_array;

};

#include <hkgeometry2/hkGeometry.inl>

#endif // HK_GEOMETRY2_GEOMETRY_H

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
