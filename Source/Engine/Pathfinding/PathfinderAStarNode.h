// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _PATHFINDERASTARNODE_
#define _PATHFINDERASTARNODE_


// Those nodes are used in A-Star lists for global search in portals.

class PathfinderPortalNode;

class PathfinderAStarNode
{
public:
	// Variables
	PathfinderPortalNode* mPortal;
	int mFromIndex;
	int mLastZoneIndex;
	float mCost;
	float mHeuristic;

	PathfinderAStarNode( PathfinderPortalNode* portal = 0 ) :  mPortal(portal), mFromIndex(0),
		mCost(0), mHeuristic(0)
	{
	};

	PathfinderAStarNode( const PathfinderAStarNode& node )
	{
		mPortal = node.mPortal;
		mFromIndex = node.mFromIndex;
		mCost = node.mCost;
		mHeuristic = node.mHeuristic;
	};

	~PathfinderAStarNode(void)
	{
	};

	BOOL operator<( PathfinderAStarNode& node ) const
	{
		return ( mCost + mHeuristic < node.mCost + node.mHeuristic );
	};

	BOOL operator>( PathfinderAStarNode& node ) const
	{
		return ( mCost + mHeuristic > node.mCost + node.mHeuristic );
	};
};


#endif //_PATHFINDERASTARNODE_

