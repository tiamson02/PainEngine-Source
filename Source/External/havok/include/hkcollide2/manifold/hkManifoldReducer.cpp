/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkcollide2/hkCollide2.h>
#include <hkmath2/linear/hkVector4Util.h>

#include <hkcollide2/contact/hkCollisionResults.h>
#include <hkcollide2/manifold/hkManifoldReducer.h>
#include <hkcollide2/dispatch/hkCollisionEnvironment.h>
#include <hkcollide2/contact/hkContactMgr.h>


namespace hkManifoldReducerMathUtil
{

//*
	inline hkReal HK_CALL leftOf( const hkVector4& vZero, const hkVector4& vA, const hkVector4& vB, const hkVector4& n )
	{
		hkVector4 za, zb, zbXza;

		za.setSub4( vA, vZero );
		zb.setSub4( vB, vZero );

		zbXza.setCross( zb, za );

		hkReal retVal = zbXza.dot3(n);
//		hkReal retVal = zbXza.lengthSquared3();
		return retVal;

	}
//*/

/*
	inline hkReal HK_CALL leftOf( const hkVector4& vZero, const hkVector4& vA, const hkVector4& vB, const hkVector4& n )
	{

		hkVector4 za, zb, nXzb;

		za.setSub4( vA, vZero );
		zb.setSub4( vB, vZero );

		nXzb.setCross( n, zb );

		nXzb.normalize3();

		hkReal dist = za.dot3( nXzb );

		return dist;

	}
//*/

}


// keeps only the points that form a convex hull.  removes those that don't.  The contact point list
// will be sorted as a connected convex hull as a usefull side effect
int hkManifoldReducer::makeConvex( hkCollisionResult& manifold, int numPoints )
{
//!me there is room for optimization here.  especially if you assume only 1 point has been added
//!me to an already sorted convex hull


	hkArray<hkCollisionContactPoint>& contactPoints = manifold.m_contactPoints;

	int resultOffset = contactPoints.getSize() - numPoints;
	const hkVector4& n = contactPoints[resultOffset].m_normal;
/*	hkVector4 ab,ac;
	ab.setSub4( contactPoints[1].m_position, contactPoints[0].m_position );
	ac.setSub4( contactPoints[2].m_position, contactPoints[0].m_position );
	hkVector4 n;
	n.setCross( ab, ac );
	n.normalize3();
*/
	// find point that max along a major an minor axis, so our point 0 ( and 1 ) are in the convex hull
	int manX, manY;
	if( n(0) < n(1) )
	{
		manX = 0;
		manY = 1;
	}
	else
	{
		manX = 1;
		manY = 0;
	}

	if( n(2) < manY )
	{
		manY = 2;
	}

	// pre-sort first
	int iPoint0 = resultOffset;
	int iFindPoint;
	for( iFindPoint = 1+resultOffset; iFindPoint < numPoints+resultOffset; iFindPoint++ )
	{
		if( contactPoints[ iFindPoint ].m_position(manY) <
			contactPoints[ iPoint0 ].m_position(manY) )
		{
				iPoint0 = iFindPoint;
		}
		else if( contactPoints[ iFindPoint ].m_position(manY) ==
				contactPoints[ iPoint0 ].m_position(manY) &&
				contactPoints[ iFindPoint ].m_position(manX) >
				contactPoints[ iPoint0 ].m_position(manX) )
		{
			iPoint0 = iFindPoint;
		}

	}

	// sort angularly using point 0 as reference
	// insertion sort

	const hkCollisionContactPoint swapTemp( contactPoints[resultOffset] );
	contactPoints[resultOffset] = contactPoints[iPoint0];
	contactPoints[iPoint0] = swapTemp;

	const hkVector4& vZero = contactPoints[resultOffset].m_position;

	int iCandidate = 2+resultOffset;

	while( iCandidate < numPoints+resultOffset )
	{
		hkVector4 vCandidate = contactPoints[ iCandidate ].m_position;

		hkCollisionContactPoint cpCandidate = contactPoints[ iCandidate ];

		int iSearch = iCandidate;

		// find insertion slot and shift elements to right to make room for insertion
//*
		//while( iSearch >= 2+resultOffset )
		while( hkManifoldReducerMathUtil::leftOf( vZero, contactPoints[ iSearch-1 ].m_position, vCandidate, n ) > 0 )
		{
			contactPoints[ iSearch ] = contactPoints[ iSearch-1 ];
			iSearch--;
		}

		contactPoints[ iSearch ] = cpCandidate;

		iCandidate++;
//*/
/*
		while( 1 )
		{

			hkReal cmp = hkManifoldReducerMathUtil::leftOf( vZero, contactPoints[ iSearch-1 ].m_position, vCandidate, n );
			if( cmp > 0.05f )
			{
				iSearch--;
			}
			else if( cmp > -0.05f )
			{
				// choose the furthest point from zero in a tie and delete the other
			//	hkReal dCandidate, dSearch;
				hkVector4 candidateZero, searchZero;
				candidateZero.setSub4( vCandidate, vZero );
				searchZero.setSub4( contactPoints[ iSearch-1 ].m_position, vZero );
			//	hkCollisionContactPoint deletedPoint = cpCandidate;

				if( candidateZero.lengthSquared3() < searchZero.lengthSquared3() )
				{
					cpCandidate = contactPoints[ iSearch-1 ];
					iSearch--;
				}
			//	else
			//	{
			//		deletedPoint = contactPoints[ iSearch-1 ];
			//	}

				// move elements into the hole created by deleted candidate
				int iShift;
				numPoints--;
				for( iShift = iCandidate; iShift < numPoints; iShift++ )
				{
					contactPoints[ iShift ] = contactPoints[ iShift+1 ];
				}

			//	contactPoints[contactPoints.getSize()-1] = deletedPoint;
				contactPoints.popBack();

				break;

			}
			else
			{
				// shift to make room
				int iShift;
				for( iShift = iCandidate; iShift > iSearch; iShift-- )
				{
					contactPoints[ iShift ] = contactPoints[ iShift-1 ];
				}

				break;
			}

		}

		contactPoints[ iSearch ] = cpCandidate;

		iCandidate++;
//*/
	}

	numPoints = contactPoints.getSize();

	// Graham Scan convex hull algorithm
	hkArray< hkCollisionContactPoint* > convexStack;

	convexStack.reserveExactly( numPoints );

	int iAddPoint = 2;

	convexStack.pushBackUnchecked( &(contactPoints[ 0+resultOffset ]) );
	convexStack.pushBackUnchecked( &(contactPoints[ 1+resultOffset ]) );

	while( iAddPoint < numPoints )
	{

//		HK_ASSERT( convexStack.getSize() >= 2 );

		if( convexStack.getSize() < 2 || hkManifoldReducerMathUtil::leftOf(	convexStack[convexStack.getSize()-2]->m_position,
					contactPoints[ iAddPoint+resultOffset ].m_position,
					convexStack[convexStack.getSize()-1]->m_position, n ) > 0.00f )
		{
			convexStack.pushBackUnchecked( &(contactPoints[ iAddPoint+resultOffset ]) );
			iAddPoint++;
		}
		else
		{
			// remove the point
			convexStack.popBack();
		}

	}

	// copy our convex hull in sorted order
	hkArray<hkCollisionContactPoint> copyArea;
	copyArea.reserveExactly(convexStack.getSize());
	int iCopy;
	for( iCopy = 0; iCopy < convexStack.getSize(); iCopy++ )
	{
		copyArea.pushBackUnchecked( *(convexStack[iCopy]) );
	}

	iCopy = 0;
	for( ; iCopy < copyArea.getSize(); iCopy++ )
	{
		contactPoints[iCopy+resultOffset] = copyArea[iCopy];
	}

	contactPoints.setSize( copyArea.getSize()+resultOffset );

	return copyArea.getSize();
}


// remove the point that supports the smallest moment arm relative to the rest of the manifold.
// do not remove any points that were added this step, we need to respond to them
// expects the list of contact points to be an ordered convex hull ( i.e. you can trace a convex
// hull by drawing edges sequentially between points in the array )
//!me figure out if hull will remain convex after this method
int hkManifoldReducer::removeLeastUsefullPoint( hkContactPointId freshPointId, hkCollisionResult& manifold, int numPoints )
{
	hkArray<hkCollisionContactPoint>& contactPoints = manifold.m_contactPoints;

	// this array is to handle wrap around nicely
	hkArray<int> indexes;
	int resultOffset = contactPoints.getSize() - numPoints;

	indexes.reserveExactly( numPoints + 2 );
	int i;
	for( i = 0; i < numPoints; i++ )
	{
		indexes.pushBackUnchecked( i + resultOffset );
	}

	indexes.pushBackUnchecked( 0 + resultOffset );
	indexes.pushBackUnchecked( 1 + resultOffset );

	int iSmallestArm = -1;
	float smallestArmLength = FLT_MAX;

	i = 0;
	for( ; i < numPoints; i++ )
	{
		if( contactPoints[indexes[i+1]].m_contactPointId != freshPointId )
		{
			hkReal armLength = hkVector4Util::distToLineSquared( contactPoints[ indexes[i] ].m_position, contactPoints[indexes[i+2]].m_position, contactPoints[indexes[i+1]].m_position );

			if( armLength < smallestArmLength )
			{
				iSmallestArm = indexes[i+1];
				smallestArmLength = armLength;
			}
		}
	}

	if( iSmallestArm == -1 )
	{
		// we have a messed up manifold.  A's veiw of it is as 1 point where B sees it as more
		// just remove an arbitrary existing point
		iSmallestArm = ( freshPointId == 0 ) ? 1 + resultOffset : 0 + resultOffset;
	}

	for( i = iSmallestArm; i < contactPoints.getSize()-1; i++ )
	{
		contactPoints[ i ] = contactPoints[ i + 1 ];
	}
	contactPoints.popBack();

	return numPoints - 1;

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
