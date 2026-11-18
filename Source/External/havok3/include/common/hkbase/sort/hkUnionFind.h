/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_UNIONFIND_H
#define HK_UNIONFIND_H

#include <hkbase/hkBase.h>

/// A class for partitioning sets into connected subsets.
/// This implementation uses the integers [0,n] as nodes
/// and generates subsets all at once via the setEdges method.
/// We can then query for (a) connected nodes or (b) the sets
/// of edges which connect the nodes.
///
/// Example use:\n
/// hkUnionFind unionFind;\n
///	hkUnionFind::EdgeVector edges;\n
/// // pushBack objects created using hk_make_pair(edgeStart,edgeEnd)\n
/// unionFind.setEdges(edges, numberOfVertices);\n
/// // Now we can extract all groups one at a time using getNextGroup()\n
class hkUnionFind
{
	public:

		typedef hkArray<int>	IntVector;

	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkUnionFind);

			/// Constructor which takes a pointer to a working buffer
		hkUnionFind( hkArray<int>& parents, int numElements );

			/// Destructor.
		~hkUnionFind(){}

			/// add an edge
		void addEdge( int i1, int i2 );

			/// replace each parent by the group number, also returns the number of elements in each group
		void assignGroups( hkArray<int>& elementsPerGroup );

		HK_FORCE_INLINE hkBool isOneGroup() { return m_parents[0] == -m_parents.getSize(); }

	private:

		HK_FORCE_INLINE int findRootOfNode(int i);
		HK_FORCE_INLINE void unionRoots(int r1, int r2);
		void collapseTree();

	protected:
			// i'th element contains parent of node i
		hkArray<int>&	m_parents;
};


#endif //HAVOK_UNIONFIND_H


/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
