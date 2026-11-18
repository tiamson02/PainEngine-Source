/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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
class hkUnionFind
{
	public:

		// typedefs make it easier to templatize this class if needed
		struct Edge
		{
			int m_first;
			int m_second;
		};

		typedef hkArray<Edge>	EdgeVector;
		typedef hkArray<int>	IntVector;

	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkUnionFind);

			/// Constructor.
		hkUnionFind();

			/// Destructor.
		~hkUnionFind();

		struct Group
		{
			int* m_nodes;
			int m_numNodes;
			int m_currentNode;
		};


		struct NodeGroups
		{
			~NodeGroups();

			hkArray<Group> m_groups;
		};



			/// Sets the edges of the hkUnionFind.
		void setEdges( const EdgeVector& e, int numnode );

			/// If the next group exists, clear i, put into i and return true.
			/// Otherwise i is untouched and this function returns false.
		bool getNextGroup(IntVector& i);

		void findGroupNodes( NodeGroups& nodeGroups );

		inline bool isOneGroup() { return m_parents[0] == -m_parents.getSize(); }

	private:

		int addEdge( int i1, int i2 );
		inline int findRootOfNode(int i);
		inline void unionRoots(int r1, int r2);
		void collapseTree();

	private:
			// i'th element contains parent of node i
		IntVector	m_parents;

			// the i'th edge connects eventually to node m_edgeRoot[i]
		IntVector	m_edgeRoot;

			// group counter
		int m_startat;
};


#endif //HAVOK_UNIONFIND_H


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
