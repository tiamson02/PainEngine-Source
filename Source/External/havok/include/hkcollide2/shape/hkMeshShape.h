/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_MESH_SHAPE_H
#define HK_COLLIDE2_MESH_SHAPE_H

#include <hkcollide2/shape/hkShapeCollection.h>

/// A class for wrapping geometric collision detection information.  It can directly reference
/// sets of triangle strips with vertex striding, and either 16 or 32 bit indices to vertices.
/// It can also directly reference triangle soups, using three indices per triangle rather than one.
/// If you wrap this shape inside a hkMoppShape, degenerate triangles are filtered out.
/// Otherwise you must ensure that the shape contains no degenerate triangles.
class hkMeshShape: public hkShapeCollection
{
	public:

		enum IndexStridingType
		{
			HK_INT16_INDICES,
			HK_INT32_INDICES
		};

			/// A subpart defines a triangle or a triangle strip.
		struct Subpart
		{
				/// A pointer to the first vertex, defined by three floats.
			float*  m_vertexBase;

				/// The byteoffset between two consecutive vertices (usually 12, 16 or more).
			int		m_vertexStriding;

				/// The number of vertices.
			int		m_numVertices;

				/// A pointer to triples of vertex indices.
			union 
			{
				hkUint32*	m_index32Base;
				hkUint16*	m_index16Base;
			};

				/// A type defining whether 16 or 32 bits are used to index vertices.
			IndexStridingType	m_stridingType:8;

				/// The byteoffset between two indices triples. (Usually sizeof(hkUint16) or...)
			int					m_indexStriding;

				/// The number of index triples, which is equal to the number of triangles.
			int		m_numTriangles;
		};

	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO, hkMeshShape);

			/// Constructs a new hkMeshShape.
		hkMeshShape( );

		void setScaling( const hkVector4& scaling );

			/// Adds a subpart. To modify this subpart later on, call getSubpartAt(int ).xxxx = xxxx.
		void addSubpart( const Subpart& part );

			/// Returns the number of subparts.
		inline int getNumSubparts() const;

			/// Gets read/write access to a subpart.
		inline Subpart& getSubpartAt( int i );

			/// Gets const access to a subpart.
		inline const Subpart& getSubpartAt( int i ) const;


			/// Gets the extra radius for every triangle.
		inline hkReal getRadius() const;

			/// Sets the extra radius for every triangle.
		inline void setRadius(hkReal r );

			//	hkShapeCollection interface implementation.
		int		getNumChildShapes( ) const;

			//	hkShapeCollection interface implementation.
		hkShapeKey	getKeyAt( int i ) const;


			//	hkShapeCollection interface implementation.
		const hkShape* getChildShape(const hkShapeKey& key, AllocBuffer& buffer ) const;



			//	hkShape interface implementation.
 		void getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out  ) const;

			///Gets this hkShape's primary type. For hkMeshShapes, this is HK_SHAPE_COLLECTION.
		virtual hkShapeType getPrimaryType() const { return HK_SHAPE_COLLECTION; }

		inline const hkVector4&	getScaling() const;

	protected:
		hkVector4					m_scaling;
		hkReal						m_radius;
		hkInplaceArray<Subpart,1>	m_subparts;
};


#include <hkcollide2/shape/hkMeshShape.inl>


#endif // HK_COLLIDE2_MESH_SHAPE_H

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
