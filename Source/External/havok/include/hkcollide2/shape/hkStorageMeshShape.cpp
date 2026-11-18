/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkcollide2/hkCollide2.h>
#include <hkcollide2/shape/hkStorageMeshShape.h>
#include <hkcollide2/shape/hkTriangleShape.h>

hkStorageMeshShape::hkStorageMeshShape()
: m_radius (0.0f )
{
	
}


hkStorageMeshShape::hkStorageMeshShape(const hkArray<hkVector4>& vertices , const hkArray<int>& indices)
: m_radius (0.0f )
{
	m_trianglesVertices.setSize(vertices.getSize());
	{
		for (int i=0;i<vertices.getSize();i++)
		{
			m_trianglesVertices[i] = vertices[i];
		}
	}

	m_trianglesIndices.setSize(indices.getSize()/3);
 
	{
		int triIndex = 0;
		for (int i=0;i<indices.getSize();i+=3)
		{
			m_trianglesIndices[triIndex].m_a = indices[i+0];
			m_trianglesIndices[triIndex].m_b = indices[i+1];
			m_trianglesIndices[triIndex].m_c = indices[i+2];
			triIndex++;
		}
	}
}

//avoid having to re-allocate every time a vertex is added
void hkStorageMeshShape::reserveMemory(int numverts,int numTriangles)
{
	if(numverts > m_trianglesVertices.getCapacity())
	{
		m_trianglesVertices.reserve(numverts);
	}

	if(numTriangles > m_trianglesIndices.getCapacity())
	{
		m_trianglesIndices.reserve(numTriangles);
	}
}




int hkStorageMeshShape::addVertex(const hkVector4& vertex)
{
	int index = m_trianglesVertices.getSize();
	m_trianglesVertices.pushBack(vertex);
	return index;
}

void hkStorageMeshShape::addTriangle(int index0, int index1, int index2)
{
	Triangle* t= m_trianglesIndices.expandBy(1);
	HK_ASSERT( index0 < m_trianglesVertices.getSize() );
	HK_ASSERT( index1 < m_trianglesVertices.getSize() );
	HK_ASSERT( index2 < m_trianglesVertices.getSize() );
	t->m_a = index0;
	t->m_b = index1;
	t->m_c = index2;
}

//accessor methods
const hkArray<hkStorageMeshShape::Triangle>& hkStorageMeshShape::getTriangles() const
{
	return m_trianglesIndices;
}

//accessor methods
const hkArray<hkVector4>& hkStorageMeshShape::getVertices() const
{
	return m_trianglesVertices;
}



int	hkStorageMeshShape::getNumChildShapes() const 
{
	return m_trianglesIndices.getSize();
}

void hkStorageMeshShape::getAabb(const hkTransform& localToWorld, hkReal tolerance, hkAabb& out) const
{
	// warning: not all vertices might be used, so it is not enough to go through vertexarray !
	// potential optimization (same for hkMeshShape): speedup by lazy evaluation and storing the cached version, having a modified flag

	out.m_min = hkVector4(  hkMath::HK_REAL_MAX,  hkMath::HK_REAL_MAX,  hkMath::HK_REAL_MAX );
	out.m_max = hkVector4( -hkMath::HK_REAL_MAX, -hkMath::HK_REAL_MAX, -hkMath::HK_REAL_MAX );

	for (int i=0;i<m_trianglesVertices.getSize();i++)
	{
		const hkVector4& vLocal = m_trianglesVertices[i];
		hkVector4 vWorld; vWorld.setTransformedPos( localToWorld, vLocal );

		out.m_min.setMin4( out.m_min, vWorld );
		out.m_max.setMax4( out.m_max, vWorld );
	}

	hkVector4 tol4; tol4.setAll( tolerance + m_radius);
	out.m_min.sub4( tol4 );
	out.m_max.add4( tol4 );
}

const hkShape* hkStorageMeshShape::getChildShape(const hkShapeKey& key,AllocBuffer& buffer) const
{
	
	HK_ASSERT2(key.m_primary >= 0 && key.m_primary < m_trianglesIndices.getSize(), "hkShapeKey invalid");
	
	hkTriangleShape *ts = new( buffer ) hkTriangleShape;

	ts->setVertex( 0, m_trianglesVertices[m_trianglesIndices[key.m_primary].m_a] );
	ts->setVertex( 1, m_trianglesVertices[m_trianglesIndices[key.m_primary].m_b] );
	ts->setVertex( 2, m_trianglesVertices[m_trianglesIndices[key.m_primary].m_c] );

	ts->setUserIdB( key.m_secondary );
	ts->setUserIdA( key.m_primary );

	ts->setRadius( m_radius );

	return ts;
}


hkShapeCollection::hkShapeKey hkStorageMeshShape::getKeyAt(int i) const
{
	hkShapeKey key;
	key.m_primary = i;
	key.m_secondary = 0;
	return key;
}


hkReal hkStorageMeshShape::getRadius() const
{
	return m_radius;
}

void hkStorageMeshShape::setRadius(hkReal r )
{
	m_radius = r;
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
