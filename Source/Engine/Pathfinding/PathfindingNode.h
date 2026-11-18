// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _PATHFINDERNODE_
#define _PATHFINDERNODE_


// Those nodes are used in A-Star lists.

class PathfindingNode
{
public:
	// Variables
	int mFromIndex;
	float mCost;
	float mHeuristic;

	PathfindingNode(void)
	{
	};

	PathfindingNode( const PathfindingNode& node )
	{
		mFromIndex = node.mFromIndex;
		mCost = node.mCost;
		mHeuristic = node.mHeuristic;
	};

	~PathfindingNode(void)
	{
	};

	BOOL operator<( PathfindingNode& node ) const
	{
		return ( mCost + mHeuristic < node.mCost + node.mHeuristic );
	};

	BOOL operator>( PathfindingNode& node ) const
	{
		return ( mCost + mHeuristic > node.mCost + node.mHeuristic );
	};
};


#endif //_PATHFINDERNODE_

