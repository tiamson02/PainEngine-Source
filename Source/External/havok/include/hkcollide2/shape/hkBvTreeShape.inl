/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


/// \class hkBvTreeShape
///
/// <b>What does the bounding volume tree do?</b><br>
///
/// A bounding volume tree is useful in situations where you need to check for collisions between a moving object
/// and a large static geometry, such as a landscape. <br> \n
/// The shapes that make up the landscape are hierarchically grouped in
/// a binary bounding volume tree.
/// At every node in the tree there exists a bounding polytope, which encapsulates all 
/// of its children. The top-level bounding volume contains the entire landscape, while
/// the nodes on the leaf levels encapsulate one geometric primitive, normally a
/// triangle. The fit of this bounding volume can be perfect (as in some AABB trees), or can 
/// have an extra margin/tolerance built in (e.g. MOPP):\n\n\n
/// <center><img src="pix/twoTriangles.gif"></center>\n
///
/// Instead of checking whether the moving object is colliding with each of the triangles in the landscape in turn, which would be extremely time-consuming, the bounding box of the moving object
/// is checked against the bounding volume tree - first, whether it is intersecting with the top-level bounding volume, then with
/// any of its child bounding volumes, and so on until the check reaches the leaf nodes. A list of any potentially colliding triangles
/// is then passed to the narrow phase collision detection. You can think of
/// the bounding volume tree as a filter to the narrow phase collision
/// detection system.<br>


hkBvTreeShape::hkBvTreeShape( const hkShapeCollection* collection )
{
	m_shapeCollection = collection;
	m_shapeCollection->addReference();
}

inline hkBvTreeShape::~hkBvTreeShape()
{
	m_shapeCollection->removeReference();
}


const hkShapeCollection* hkBvTreeShape::getShapeCollection() const
{
	return &(*m_shapeCollection);
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
