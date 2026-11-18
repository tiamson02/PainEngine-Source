/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkfilterscene/hkFilterScene.h>
#include <hkfilterscene/hkFilterSceneRemoveScale.h>

#include <hkbase/stream/impl/hkMemoryStreamReader.h>
#include <hkbase/stream/impl/hkArrayStreamWriter.h>

#include <hkscenedata/scene/hkxScene.h>
#include <hkscenedata/graph/hkxNode.h>
#include <hkscenedata/mesh/hkxMesh.h>
#include <hkscenedata/mesh/hkxMeshSection.h>
#include <hkscenedata/mesh/hkxVertexBuffer.h>
#include <hkscenedata/mesh/hkxVertexFormat.h>
#include <hkscenedata/skin/hkxSkinBinding.h>

// included for matrix polar decomposition util
#include <hkmath/linear/hkMathUtil.h>

hkFilterSceneRemoveScaleDesc g_SceneRemoveScale;

hkFilterSceneRemoveScale::hkFilterSceneRemoveScale()
{

}
	
hkFilterSceneRemoveScale::~hkFilterSceneRemoveScale()
{

}

HWND hkFilterSceneRemoveScale::showOptions(HWND owner, const hkRootLevelContainer& data, class hkFilterClassRegistry& classReg)
{
	return NULL;
}

void hkFilterSceneRemoveScale::hideOptions()
{

}

void hkFilterSceneRemoveScale::setOptions(const void* optionData, int optionDataSize, unsigned int version)
{

}

int hkFilterSceneRemoveScale::getOptionsSize() const
{
	return 0;
}

void hkFilterSceneRemoveScale::getOptions(void* optionData) const
{

}


static void _SafelyDecompose( hkMatrix4& full, hkTransform& transform )
{
	// polar decompose the transform
	hkMathUtil::Decomposition decomposition;
	hkMathUtil::decomposeMatrix( full, decomposition );
	if( !decomposition.m_hasSkew )
	{
		transform.set( decomposition.m_rotation, decomposition.m_translation );
	}
	else
	{
		HK_WARN_ALWAYS( 0xabbaf0d1, "Matrix Decomposition Failed! There is skew present" );

		// set to origin with identity
		transform.setIdentity();
	}
}


static void _burnNodeScaleRecursive( hkxNode* node )
{
	if( !node ) return;

	if( node->m_numKeyFrames > 0 )
	{
		// Decompose the first keyframe
		hkMatrix4 worldFromNode;
		hkMathUtil::Decomposition decomposition;
		hkMathUtil::decomposeMatrix( node->m_keyFrames[0], decomposition );
		if( decomposition.m_hasScale || decomposition.m_hasSkew )
		{
			// Get the scale/skew variations
			hkMatrix4 scaleSkew;
			hkMatrix4 scaleSkewInverse;
			hkMatrix3 scaleSkewInverseTranspose;
			{
				scaleSkew.setCols(	decomposition.m_scaleAndSkew.getColumn(0),
									decomposition.m_scaleAndSkew.getColumn(1),
									decomposition.m_scaleAndSkew.getColumn(2),
									hkVector4(0,0,0,1) );
				
				scaleSkewInverse = scaleSkew;
				scaleSkewInverse.invert( hkMath::HK_REAL_EPSILON );
				
				scaleSkewInverseTranspose = decomposition.m_scaleAndSkew;
				scaleSkewInverseTranspose.invert( hkMath::HK_REAL_EPSILON );
				scaleSkewInverseTranspose.transpose();
			}
			
			
			// Remove the scale/skew from this node's keyframes
			int i, j;
			for( i=0; i<node->m_numKeyFrames; ++i )
			{
				node->m_keyFrames[i].mul( scaleSkewInverse );
			}
			
			// Add the scale/skew into the child node keyframes
			for( i=0; i<node->m_numChildren; ++i )
			{
				hkxNode* childNode = node->m_children[i];
				for( j=0; j<childNode->m_numKeyFrames; ++j )
				{
					hkMatrix4 kf( childNode->m_keyFrames[j] );
					childNode->m_keyFrames[j].setMul( scaleSkew, kf );
				}
			}
			
			
			// Burn the scale/skew into the mesh if there is one
			const hkClass* klass = node->m_object.m_class;
			void* object = node->m_object.m_object;
			if( klass && object && hkString::strCmp( klass->getName(), hkxMeshClass.getName() ) == 0 )
			{
				hkxMesh* mesh = static_cast<hkxMesh*>(object);
				
				hkReal* data;
				hkVector4 vec;
				for( int i=0; i<mesh->m_numSections; ++i )
				{
					hkxVertexBuffer* vb = mesh->m_sections[i]->m_vertexBuffer;
					hkUlong vertexData = (hkUlong)vb->m_vertexData;
					
					int numVertices = vb->m_numVertexData;
					for( int j=0; j<numVertices; ++j, vertexData += vb->m_format->m_stride )
					{
						// Vertex
						data = (hkReal*)(vertexData + vb->m_format->m_positionOffset);
						vec.set( data[0], data[1], data[2] );
						vec._setMul4( decomposition.m_scaleAndSkew, vec );
						data[0] = vec(0);
						data[1] = vec(1);
						data[2] = vec(2);
						
						// Normal
						if( vb->m_format->m_normalOffset != HK_VFMT_NOT_PRESENT )
						{
							data = (hkReal*)(vertexData + vb->m_format->m_normalOffset);
							vec.set( data[0], data[1], data[2] );
							vec._setMul4( scaleSkewInverseTranspose, vec );
							vec.normalize3();
							data[0] = vec(0);
							data[1] = vec(1);
							data[2] = vec(2);
						}
						
						// Tangent
						if( vb->m_format->m_tangentOffset != HK_VFMT_NOT_PRESENT )
						{
							data = (hkReal*)(vertexData + vb->m_format->m_tangentOffset);
							vec.set( data[0], data[1], data[2] );
							vec._setMul4( scaleSkewInverseTranspose, vec );
							vec.normalize3();
							data[0] = vec(0);
							data[1] = vec(1);
							data[2] = vec(2);
						}
						
						// Binormal
						if( vb->m_format->m_binormalOffset != HK_VFMT_NOT_PRESENT )
						{
							data = (hkReal*)(vertexData + vb->m_format->m_binormalOffset);
							vec.set( data[0], data[1], data[2] );
							vec._setMul4( scaleSkewInverseTranspose, vec );
							vec.normalize3();
							data[0] = vec(0);
							data[1] = vec(1);
							data[2] = vec(2);
						}
					}
				}
			}
		}
	}
	
	// Recurse over the children of the node
	for( int i=0; i<node->m_numChildren; ++i )
	{
		_burnNodeScaleRecursive( node->m_children[i] );
	}
}


void hkFilterSceneRemoveScale::process( hkRootLevelContainer& data, hkFilterClassRegistry& classReg,bool batchMode, const char* infoString, const char* configName, HWND ownerWindow )
{
	// Find the scene in the root level container
	hkxScene* scenePtr = reinterpret_cast<hkxScene*>( data.findObjectByType( hkxSceneClass.getName() ) );
	if( scenePtr == HK_NULL )
	{
		HK_WARN_ALWAYS (0xabbaa5f0, "No scene data found");
		return;
	}
	hkxScene& scene = *scenePtr;
	
	// we don't need to register any classes as we are not adding any extra types
	// that are not 'known' already.
	
	// Push the scale out of the scene graph, starting from the root node
	_burnNodeScaleRecursive( scene.m_rootNode );
	
	// Remove the scale from the world bind pose in the blended mesh infos
	for( int i=0; i<scene.m_numSkinBindings; ++i )
	{
		hkxSkinBinding* skin = scene.m_skinBindings[i];
		for (int mi=0; mi < skin->m_numBindPose; ++mi)
		{
			hkMatrix4& worldT = skin->m_bindPose[mi];
			hkTransform noScaleWorld;
			
			// polar decompose the transform
			_SafelyDecompose( worldT, noScaleWorld );
			
			worldT.set( noScaleWorld ); 
		}
		
		hkMatrix4& worldT_skin = skin->m_initSkinTransform;
		hkTransform noScaleWorld_skin;
		
		// polar decompose the transform
		_SafelyDecompose( worldT_skin, noScaleWorld_skin );
		
		worldT_skin.set( noScaleWorld_skin );
	}
}

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
