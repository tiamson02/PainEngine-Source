/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_STORAGEMESHSHAPE_H
#define HK_STORAGEMESHSHAPE_H

#include <hkcollide2/shape/hkMeshShape.h>

/// This shape is a very simple container for triangle soups and can't handle triangle strips.
/// There are no editing facilities (remove triangles etc.) and all data gets copied.
/// Hence, it does not allow sharing of triangle data with the renderer through referencing.
/// Use hkMeshShape or your own implementation of a hkShapeCollection to share triangle data with the renderer.
class hkStorageMeshShape : public hkShapeCollection
{
	public:
		struct Triangle
		{
			int m_a;
			int m_b;
			int m_c;
		};

		
			/// Empty Constructor
		hkStorageMeshShape();

			/// Constructor using an index array
		hkStorageMeshShape(const hkArray<hkVector4>& vertices , const hkArray<int>& indices);

			/// Add a vertex and return its index
		int	addVertex(const hkVector4& vertex);

			/// Add a triangle 
		void	addTriangle(int index0, int index1, int index2);

			/// reserve enough memory so that addVertex and addTriangle do not allocate more memory
		void	reserveMemory(int numVerts,int numTriangles);

			/// Get all triangles
		const hkArray<Triangle>& getTriangles() const;

			/// Get all vertices
		const hkArray<hkVector4>& getVertices() const;

		
			///	Return the number of triangles
		int		getNumChildShapes( ) const;


			//	hkShapeCollection interface implementation.
		hkShapeKey	getKeyAt( int i ) const;

			//	hkShapeCollection interface implementation.
		const hkShape* getChildShape(const hkShapeKey& key, AllocBuffer& buffer ) const;

			// hkShape interface implementation.
 		virtual void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out ) const;

			/// Gets the extra radius for every triangle.
		hkReal getRadius() const;

			/// Sets the extra radius for every triangle.
		void setRadius(hkReal r );

	protected:
		hkReal						m_radius;

		hkArray<Triangle>	m_trianglesIndices;

		hkArray<hkVector4>	m_trianglesVertices;

};

#endif //HK_STORAGEMESHSHAPE_H

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
