/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/***
 *** INLINE FUNCTION DEFINITIONS
 ***/



template<typename AGENT>
inline hkSymmetricAgent<AGENT>::hkSymmetricAgent( const hkCollidable& A, const hkCollidable& B ) 
:	AGENT( B, A )
{
}

template<typename AGENT>
inline hkSymmetricAgent<AGENT>::hkSymmetricAgent( const hkCollidable& A, const hkCollidable& B, const hkCollisionEnvironment& env ) 
:	AGENT( B, A, env )
{
}



template<typename AGENT>
void hkSymmetricAgent<AGENT>::processCollision(	const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result)
{

	int i = result.m_contactPoints.getSize();
//	int oldFeatureSize = result.m_features->getSize();

	AGENT::processCollision(collB, collA, env, result);

	//
	//	Flip normals
	//
	for ( ; i < result.m_contactPoints.getSize(); ++i)
	{
		result.m_contactPoints[i].m_normal.mul4(-1.f);
	}

	/*
	//
	//	flip features 
	//
	hkCollisionFeature* feature = (*result.m_features).begin() + oldFeatureSize;
	for ( int j = result.m_features->getSize()-1-oldFeatureSize; j>=0; j--)
	{
		int dim = feature->m_dimA + feature->m_dimB;
		hkAlgorithm::swap16( feature->m_vertices[0], feature->m_vertices[dim-1] );

		if ( (feature->m_dimA == 2) && (feature->m_dimB == 2) )
		{
			hkAlgorithm::swap16( feature->m_vertices[1], feature->m_vertices[2] );
		}

		hkAlgorithm::swap( feature->m_dimA, feature->m_dimB );
		feature->m_normal.mul4( -1.0f );
		feature++;
	}
	*/

}


template<typename AGENT>
void hkSymmetricAgent<AGENT>::getClosestPoints(const  hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, hkClosestPointsDetails& pointDetails )
{
	AGENT::getClosestPoints(collidableB , collidableA , env, pointDetails);
}


template<typename AGENT>
bool hkSymmetricAgent<AGENT>::arePenetrating(	const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env)
{
	return AGENT::arePenetrating(collB, collA, env);
}

template<typename AGENT>
void hkSymmetricAgent<AGENT>::cleanup(	const hkCollisionEnvironment& env)
{
	AGENT::cleanup(env);
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
