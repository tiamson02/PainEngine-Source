/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_MANIFOLD_REDUCER_H
#define HK_COLLIDE2_MANIFOLD_REDUCER_H

class hkManifoldReducer
{
public:

	int makeConvex( hkCollisionResult& contactPoints, int numPoints );

	int removeLeastUsefullPoint( hkContactPointId freshPointId, hkCollisionResult& contactPoints, int numPoints );

};


namespace hkManifoldReducerUtil
{ 
//	template<class Tsource>
//	void syncSourceFromResults( Tsource& sourceManifold, const hkCollisionResult& resultManifold, hkCollidable& ca, hkCollidable& cb, hkCollisionEnvironment& env );

	template<typename T>
	inline void swapAlign16(T& x, T& y)
	{
		const T HK_ALIGNED_VARIABLE( t, 16) = x;
		x = y;
		y = t;
	}


		/// takes a modified collision result manifold and alters the source manifold to match it
		/// source manifold must be a super set of collision result
	template<class Tsource>
	bool syncSourceFromResults( Tsource& sourceManifold, int newNumSourcePoints, const hkCollisionResult& resultManifold, hkCollidable& ca, hkCollidable& cb, hkCollisionEnvironment& env )
	{
		const hkArray<hkCollisionContactPoint>& resultContactPoints = resultManifold.m_contactPoints;

		int resultOffset = resultContactPoints.getSize() - newNumSourcePoints;

//		HK_ASSERT( sourceManifold.getNumPoints() >= resultContactPoints.getSize() );

		int iResult;
		for( iResult = 0; iResult < newNumSourcePoints; iResult++ )
		{

			if( sourceManifold.m_contactPoints[iResult].m_contactPointId != 
				resultContactPoints[ iResult + resultOffset ].m_contactPointId )
			{
				// if we don't have a match, then search for match and swap
				int iSource = iResult+1;
				while(	sourceManifold.m_contactPoints[iSource].m_contactPointId != 
						resultContactPoints[ iResult + resultOffset ].m_contactPointId )
				{
					iSource++;
				}

				HK_ASSERT(iSource < sourceManifold.getNumPoints());
				HK_ASSERT( iSource != iResult );

		//		Tsource swapTemp = sourceManifold.m_contactPoints[iResult];
		//		sourceManifold.m_contactPoints[iResult] = sourceManifold.m_contactPoints[iSource];
		//		sourceManifold.m_contactPoints[iSource] = swapTemp;
				swapAlign16( sourceManifold.m_contactPoints[iResult], sourceManifold.m_contactPoints[iSource] );

			}

			// ok, while same points are in sync, just iterate
		}

		// any points left are deleted points
		int deletedPoints = sourceManifold.getNumPoints() - newNumSourcePoints;
		int iSource = newNumSourcePoints;
		while( iSource < sourceManifold.getNumPoints() )
		{
			// remove from the solvers manifold
			env.m_contactMgr->removeContactPoint( sourceManifold.m_contactPoints[iSource].m_contactPointId, env );
			iSource++;
		}

		sourceManifold.setNumPoints( newNumSourcePoints );

		return true;

	}

}
/*
template<class Tsource>
void hkManifoldReducerUtil::syncSourceFromResults( Tsource& sourceManifold, const hkCollisionResult& resultManifold, hkCollidable& ca, hkCollidable& cb, hkCollisionEnvironment& env )
{
	hkInplaceArray<hkCollisionContactPoint, HK_COLLISION_RESULT_ARRAY_SIZE>& resultContactPoints = resultManifold.m_contactPoints;

	HK_ASSERT( sourceManifold.getNumPoints() >= resultContactPoints.getSize() );

	int iResult;
	for( iResult = 0; iResult < resultContactPoints.getSize(); iResult++ )
	{

		if( sourceManifold.m_contactPoints[iResult].m_contactPointId != resultContactPoints[iResult].m_contactPointId )
		{
			// if we don't have a match, then search for match and swap
			int iSource = iResult+1;
			while( sourceManifold.m_contactPoints[iSource].m_contactPointId != resultContactPoints[iResult].m_contactPointId )
			{
				iSource++;
			}

			HK_ASSERT( iSource != iResult );
			HK_ASSERT( iSource < resultContactPoints.getSize() );

			Tsource swapTemp = sourceManifold.m_contactPoints[iResult];
			sourceManifold.m_contactPoints[iResult] = sourceManifold.m_contactPoints[iSource];
			sourceManifold.m_contactPoints[iSource] = swapTemp;

		}

		// ok, while same points are in sync, just iterate
	}

	// any points left are deleted points
	int deletedPoints = sourceManifold.getNumPoints() - resultContactPoints.getSize();
	iSource = resultContactPoints.getSize();
	while( iSource < sourceManifold.getNumPoints() )
	{
		// remove from the solvers manifold
		env.m_contactMgr->removeContactPoint( sourceManifold.m_contactPoints[iSource].m_contactPointId, ca, cb, env );
		iSource++;
	}

	sourceManifold.setNumPoints( resultContactPoints.getSize() );

}
*/

#endif  // HK_COLLIDE2_MANIFOLD_REDUCER_H
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
