/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/ragdoll/hkSkeleton.h>
#include <hkutilities2/ragdoll/hkLoToHiBoneMapper.h>

hkLoToHiBoneMapper::hkLoToHiBoneMapper( const hkArray<int>& mappings )
{
	// Store a mapping for each lo-rez bone in a skeleton.
	m_mappings = mappings;
}

int	hkLoToHiBoneMapper::getImage( int iLo )
{
	HK_ASSERT( m_mappings.getSize() > iLo );
		
	return m_mappings[iLo];
}

int	hkLoToHiBoneMapper::getPreImage( int iHi )
{
	// Loop through mappings for twin index.
	// Worst case no mapping is found and returns -1.
	int result = -1;
	for( int i = 0; i < m_mappings.getSize(); i++ )
	{
		if( m_mappings[i] == iHi )
		{
			result = i;
			break;
		}
	}

	return result;
}

void testDepthFirst( const hkArray<hkInt16>& hierarchy )
{
	hkArray<int> leaves;
	leaves.reserve( hierarchy.getSize() );

	HK_ASSERT2( hierarchy[0] == -1, "Invalid hierarchy - first node must be root (-1)" );

	{
		for ( int i = 1; i < hierarchy.getSize(); ++i )
		{
			// if the node's parent is not the previous node in the array, the previous node has to be a leaf
			if ( hierarchy[i] != i - 1 )
			{
				leaves.pushBack( i - 1 );
			}
		}
	}

	{
		for ( int i = 0; i < hierarchy.getSize(); ++i )
		{
			for ( int j = 0; j < leaves.getSize(); ++j )
			{
				HK_ASSERT2(hierarchy[i] != leaves[j], "Hierarchy is not depth first");
			}
		}
	}
}

void testLinearMap( const hkArray<int>& mappings )
{
	for ( int i = 1; i < mappings.getSize(); ++i )
	{
		HK_ASSERT2( mappings[i - 1] < mappings[i], "Non-linear map needs to be re-ordered" );
	}

}

void hkLoToHiBoneMapper::applyMapping( const hkSkeleton& skeletonHi, hkSkeleton& skeletonLo )
{
	HK_ASSERT( m_mappings.getSize() <= skeletonLo.getPose().getSize() );
	HK_ASSERT( m_mappings.getSize() <= skeletonHi.getPose().getSize() );

/*
	{
	#ifdef HK_DEBUG
		// to be done at setup only...
//		testDepthFirst( skeletonLo.getHierarchy()->getParents() );
//		testDepthFirst( skeletonHi.getHierarchy()->getParents() );
//		testLinearMap( m_mappings );
	#endif

		
		//TODO -need to skip sub branches of the animation hierarchy that are not used

		int hiCounter = 0;
		for ( int i = 0; i < m_mappings.getSize(); ++ i)
		{
			skeletonLo.m_pose[i] = skeletonHi.m_pose[hiCounter];
			while ( m_mappings[i] > hiCounter )
			{
				hiCounter++;
				// can't pass self into hkQuaternion.setMul or setSlerp
				hkQuaternion q; q.setMul( skeletonLo.m_pose[i].m_rotation, skeletonHi.m_pose[hiCounter].m_rotation );
				hkVector4 p; p.setRotatedDir( skeletonLo.m_pose[i].m_rotation, skeletonHi.m_pose[hiCounter].m_position );
				skeletonLo.m_pose[i].m_rotation = q;
				skeletonLo.m_pose[i].m_position.add4( p );
			}
			hiCounter++;
		}
	}
*/
	{
		// Get the world transforms for each of the bones in the hi-rez skeleton
		hkArray<hkBone> skeletonHiPoseInWorld;
		skeletonHi.getPoseInWorld( skeletonHiPoseInWorld );

		// Setup an array to store the world transforms for the lo-rez skeleton
		hkArray<hkBone> skeletonLoPoseInWorld;
		skeletonLoPoseInWorld.setSize( skeletonLo.getPose().getSize() );
											 
		// Loop through the lo-rez bones
		for( int iLo = 0; iLo < skeletonLoPoseInWorld.getSize(); iLo++ )
		{
			int iHi = getImage( iLo );

			// there must be a mapping for each bone in the low-rez skeleton.
			// no point in storing extra bone is not required.
			HK_ASSERT( -1 != iHi );
			
			// Copy the world transform across
			skeletonLoPoseInWorld[iLo] = skeletonHiPoseInWorld[iHi];

			// assumes no extra offset transform between bones is needed
		}

		// Calculate and store the local space bone transform for the low-rez skeleton
		skeletonLo.setPoseInWorld( skeletonLoPoseInWorld );
	}
}

void hkLoToHiBoneMapper::applyMappingInverse( const hkSkeleton& skeletonLo, hkSkeleton& skeletonHi )
{
	HK_ASSERT( m_mappings.getSize() <= skeletonLo.getPose().getSize() );
	HK_ASSERT( m_mappings.getSize() <= skeletonHi.getPose().getSize() );
/*
	{
		int hiCounter = 0;
		for ( int i = 0; i < m_mappings.getSize(); ++ i)
		{
			if ( m_mappings[i] == hiCounter )
			{
				skeletonHi.m_pose[i] = skeletonLo.m_pose[hiCounter];
			}
			else
			{
				hkBone temp = skeletonLo.m_pose[hiCounter];
				while ( m_mappings[i] > hiCounter )
				{
					temp.m_position.sub4( skeletonHi.m_pose[hiCounter].m_position );
					hkQuaternion inv; inv.setInverse( skeletonHi.m_pose[hiCounter].m_rotation );
					temp.m_position.setRotatedDir( inv, temp.m_position );
					inv.mul( temp.m_rotation );
					temp.m_rotation = inv;
					hiCounter++;
				}
				skeletonHi.m_pose[i] = temp;
			}
			hiCounter++;
		}
	}
*/

	{
		// Get the world transforms for each of the bones in the low-rez skeleton
		hkArray<hkBone> skeletonLoPoseInWorld;
		skeletonLo.getPoseInWorld( skeletonLoPoseInWorld );
		
		// Setup an array to store the world transforms for the hi-rez skeleton
		hkArray<hkBone> skeletonHiPoseInWorld;
		skeletonHiPoseInWorld.setSize( skeletonHi.getPose().getSize() );
											 
		// Loop through the hi-rez bones
		for( int iHi = 0; iHi < skeletonHiPoseInWorld.getSize(); iHi++ )
		{
			int iLo = getPreImage( iHi );
			if( -1 != iLo )
			{
				const hkBone&	boneLoInWorld = skeletonLoPoseInWorld[iLo];
				const hkBone&	boneHiInLocal = skeletonHi.getBone( iHi );
				hkBone&			boneHiInWorld = skeletonHiPoseInWorld[iHi];

				boneHiInWorld = boneLoInWorld;

				// assumes no extra offset transform between bones is needed
			}
			else
			{
				const hkBone&	boneHiInLocal = skeletonHi.getBone( iHi );
				hkBone&			boneHiInWorld = skeletonHiPoseInWorld[iHi];

				hkInt16 parent = skeletonHi.getHierarchy()->getParents()[ iHi ];
				if( -1 != parent )
				{
					HK_ASSERT( parent < iHi );

					const hkBone&	parentboneHiInWorld = skeletonHiPoseInWorld[parent];

					boneHiInWorld.m_rotation.setMul( parentboneHiInWorld.m_rotation, boneHiInLocal.m_rotation );
					boneHiInWorld.m_rotation.normalize();
					boneHiInWorld.m_position.setRotatedDir( parentboneHiInWorld.m_rotation, boneHiInLocal.m_position );
					boneHiInWorld.m_position.add4( parentboneHiInWorld.m_position );
				}
				else
				{
					boneHiInWorld = boneHiInLocal;
				}
			}
		}
		skeletonHi.setPoseInWorld( skeletonHiPoseInWorld );
	}
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
