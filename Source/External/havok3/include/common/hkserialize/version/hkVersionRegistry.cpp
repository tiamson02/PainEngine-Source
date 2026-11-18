/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkserialize/hkSerialize.h>
#include <hkserialize/version/hkVersionRegistry.h>
#include <hkbase/config/hkConfigVersion.h>

static int getNumElements(const hkVersionRegistry::Updater** p)
{
	int i = 0;
	while( *p != HK_NULL )
	{
		++i;
		++p;
	}
	return i;
}

hkVersionRegistry::hkVersionRegistry()
	: m_updaters( StaticLinkedUpdaters, getNumElements(StaticLinkedUpdaters), getNumElements(StaticLinkedUpdaters) )
{
}

hkVersionRegistry::~hkVersionRegistry()
{
}

void hkVersionRegistry::registerUpdater( const Updater* updater )
{
	m_updaters.pushBack(updater);
}

static inline int strEqual(const char* s0, const char* s1)
{
	return hkString::strCmp( s0, s1 ) == 0;
}

hkResult hkVersionRegistry::getVersionPath( const char* fromVersion, const char* toVersion, hkArray<const Updater*>& pathOut ) const
{
	if( hkString::strCmp(fromVersion, toVersion) == 0 )
	{
		return HK_SUCCESS; // succeed trivially
	}

	hkArray<int> nextEdge;
	nextEdge.setSize( m_updaters.getSize(), -1 );

	hkArray<int> sourceIndices;
	hkArray<int> targetIndices;
	{
		for( int updaterIndex = 0; updaterIndex < m_updaters.getSize(); ++updaterIndex )
		{
			if( strEqual( toVersion, m_updaters[updaterIndex]->toVersion) )
			{
				// early out if we're there
				if( strEqual( fromVersion, m_updaters[updaterIndex]->fromVersion) )
				{
					pathOut.pushBack( m_updaters[updaterIndex] );
					return HK_SUCCESS;
				}

				targetIndices.pushBack( updaterIndex );
			}
			else
			{
				sourceIndices.pushBack( updaterIndex );
			}
		}
	}

	while( targetIndices.getSize() )
	{
		hkArray<int> nextTargetIndices;
		for( int sourceIndexIndex = sourceIndices.getSize()-1; sourceIndexIndex >= 0; --sourceIndexIndex )
		{
			int sourceIndex = sourceIndices[sourceIndexIndex];

			for( int targetIndexIndex = 0; targetIndexIndex < targetIndices.getSize(); ++targetIndexIndex )
			{
				int targetIndex = targetIndices[targetIndexIndex];

				if( strEqual( m_updaters[sourceIndex]->toVersion, m_updaters[targetIndex]->fromVersion) )
				{
					nextEdge[sourceIndex] = targetIndex;
					if( strEqual( m_updaters[sourceIndex]->fromVersion, fromVersion ) )
					{
						int i = sourceIndex;
						while( i != -1 )
						{
							pathOut.pushBack(m_updaters[i]);
							i = nextEdge[i];
						}
						return HK_SUCCESS;
					}
					
					nextTargetIndices.pushBack( sourceIndex );
					sourceIndices.removeAt( sourceIndexIndex );
				}
			}
		}
		targetIndices.swap( nextTargetIndices );
	}
	
	return HK_FAILURE;
}

HK_SINGLETON_IMPLEMENTATION(hkVersionRegistry);

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
