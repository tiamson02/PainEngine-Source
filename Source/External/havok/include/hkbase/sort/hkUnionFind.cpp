/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkbase/sort/hkUnionFind.h>

/// \class hkUnionFind
/// Example use:\n
/// hkUnionFind unionFind;\n
///	hkUnionFind::EdgeVector edges;\n
/// // pushBack objects created using hk_make_pair(edgeStart,edgeEnd)\n
/// unionFind.setEdges(edges, numberOfVertices);\n
/// // Now we can extract all groups one at a time using getNextGroup()\n


/* Heres the deal, m_nodes contains all possible nodes in the graph.
 * we make sure all nodes are known before adding edges so that we
 * can do clever preallocation of m_parents.
 *
 * m_parents is a list of indices.
 * Let p = m_parents[i] for any i.
 * p>=0 means that the m_nodes[i] is connected to m_nodes[p]
 * p<0 means that m_nodes[i] is a root. Also m_nodes[i] has -p child nodes
 */


/*************************************************************** HELPER */

#ifdef HK_USE_DEBUGCONSOLE

hkOstream& operator << (hkOstream& os, const hkUnionFind::EdgeVector& edges )
{
	os << "edges are : \n";
	hkUnionFind::EdgeVector::const_iterator c = edges.begin();
	hkUnionFind::EdgeVector::const_iterator e = edges.end();
	for( ; c!=e; ++c)
	{
		os << "["<<c->first<<","<<c->second<<"] ";
	}
	os << "\n\n";

	return os;
}

hkOstream& operator << (hkOstream& os, const hkUnionFind::IntVector& parents )
{
	hkUnionFind::IntVector::const_iterator c = parents.begin();
	hkUnionFind::IntVector::const_iterator e = parents.end();
	os << "[ ";
	for( ; c!=e; ++c)
	{
		os << *c << " ";
	}
	os << "]\n\n";

	return os;
}

#endif //HK_USE_DEBUGCONSOLE

/*************************************************************** CONSTRUCT DESTRUCT */

hkUnionFind::hkUnionFind()
{
}

hkUnionFind::~hkUnionFind()
{
}

/*************************************************************** PRIVATE FUNC */

// do path compression on the way up the tree
inline int hkUnionFind::findRootOfNode(int i)
{
	// find root
	int root = i;
	while ( 1 )
	{
		if ( m_parents[root] < 0 )
		{
			break;
		}
		root = m_parents[root];
	}

	// set all 
	while(m_parents[i]>=0) 
	{
		int j = m_parents[i];
		m_parents[i] = root;
		i = j;
	}
	return i;
}

/*************************************************************** PRIVATE FUNC */

/*
// join two roots - make the one with more children the new root
inline void hkUnionFind::unionRoots(int r1, int r2) 
{
	int n1 = m_parents[r1];
	int n2 = m_parents[r2];

	if(n1<=n2) // n1 more children
	{
		m_parents[r1] += n2;
		m_parents[r2] = r1;
	}
	else
	{
		m_parents[r2] += n1;
		m_parents[r1] = r2;
	}
}
*/

// join two roots - make the one earlier in the list the new root
inline void hkUnionFind::unionRoots(int r1, int r2) 
{
	int n1 = m_parents[r1];
	int n2 = m_parents[r2];

	if(r1 < r2) 
	{
		m_parents[r1] += n2;
		m_parents[r2] = r1;
	}
	else
	{
		m_parents[r2] += n1;
		m_parents[r1] = r2;
	}
}


hkUnionFind::NodeGroups::~NodeGroups()
{
	for ( int i = 0; i < m_groups.getSize(); ++i )
	{
		hkDeallocate<int>( m_groups[i].m_nodes );
	}
}

void hkUnionFind::findGroupNodes( NodeGroups& nodeGroups )
{
	hkArray<Group>& groups = nodeGroups.m_groups;

	// populate the groups array
	for (int i = 0; i < m_parents.getSize(); ++i)
	{
		if (m_parents[i] < 0)
		{
			// a new root node - add a new group
			groups.setSize(groups.getSize() + 1);
			Group& newGroup = groups[groups.getSize() - 1];

			newGroup.m_nodes = hkAllocate<int>( -m_parents[i], HK_MEMORY_CLASS_BASE);
			newGroup.m_numNodes = -m_parents[i];
			newGroup.m_nodes[0] = i;
			newGroup.m_currentNode = 1;

			m_parents[i] = groups.getSize() - 1;
		}
		else
		{
			HK_ASSERT2(m_parents[m_parents[i]] < groups.getSize(), "Internal error");

			Group& currentGroup = groups[m_parents[m_parents[i]]];

			HK_ASSERT2(currentGroup.m_currentNode < currentGroup.m_numNodes, "Internal error");

			currentGroup.m_nodes[currentGroup.m_currentNode++] = i;

		}
	}

	m_parents.setSize(0);
	m_edgeRoot.setSize(0);
}



/*************************************************************** ADDEDGE */

int hkUnionFind::addEdge( int i1, int i2 )
{
	const int r1 = findRootOfNode(i1);
	const int r2 = findRootOfNode(i2);

	HK_ASSERT(m_parents[r1]<0 && m_parents[r2]<0);

	if(r1!=r2)
	{
		unionRoots(r1, r2);
	}
	return r1<r2 ? r1 : r2;
}

/*************************************************************** COLLAPSE */

void hkUnionFind::collapseTree()
{
	// collapse nodes

	IntVector::iterator c = m_parents.begin();
	IntVector::iterator e = m_parents.end();

	for( ; c!=e; ++c)
	{
		if(*c>=0)
		{
			while( m_parents[*c]>=0 )
			{
				*c = m_parents[*c];
			}
		}
	}

	// collapse edges

	c = m_edgeRoot.begin();
	e = m_edgeRoot.end();

	for( ; c!=e; ++c)
	{
		while( m_parents[*c]>=0 )
		{
			*c = m_parents[*c];
		}
	}
}

/*************************************************************** SETEDGES */

void hkUnionFind::setEdges( const EdgeVector& edges, int numnode )
{
//	COUT(edges);

	// each node is its own parent at begin
	m_parents.setSize(numnode);
	for(int i = 0; i < numnode; ++i)
	{
		m_parents[i] = -1;
	}
	// and also each edge
	m_edgeRoot.clear();
	m_edgeRoot.reserve(edges.getSize());


	// add edges
	{
		m_edgeRoot.setSize(edges.getSize());

		for (int i = 0; i < edges.getSize(); ++i)
		{
			if (edges[i].m_first < edges[i].m_second)
			{
				m_edgeRoot[i] = addEdge(edges[i].m_first, edges[i].m_second);
			}
			else
			{
				m_edgeRoot[i] = addEdge(edges[i].m_second, edges[i].m_first);
			}
		}
	}

	// connect all directly to root
//	COUT("Parents before collapse "<<m_parents);
//	COUT("Edges before collapse "<<m_edgeRoot);
	collapseTree();
//	COUT("Parents after collapse "<<m_parents);
//	COUT("Edges after collapse "<<m_edgeRoot);

	m_startat = 0;
}

/******************************************************************* GET GROUPS */
// yuck - destroys stuff as it goes along
bool hkUnionFind::getNextGroup(IntVector& group)
{
	const int N = m_edgeRoot.getSize();

	for(int i=m_startat; i<N; ++i)
	{
		const int mygroup = m_edgeRoot[i];

		if(mygroup>=0)
		{			
			group.clear();
			group.pushBack(i);

			m_startat = i+1;
			m_edgeRoot[i] = -1;

			for(int j=i+1; j<N; j++)
			{
				if(m_edgeRoot[j]==mygroup)
				{
					group.pushBack(j);
					m_edgeRoot[j] = -1;
				}
			}
			return true;
		}				
	}
	return false;
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
