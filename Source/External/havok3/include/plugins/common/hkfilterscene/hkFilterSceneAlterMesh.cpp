/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkfilterscene/hkFilterScene.h>
#include <hkfilterscene/hkFilterSceneAlterMesh.h>

#include <hkbase/stream/impl/hkMemoryStreamReader.h>
#include <hkbase/stream/impl/hkArrayStreamWriter.h>

#include <hkscenedata/graph/hkxNode.h>
#include <hkscenedata/mesh/hkxMesh.h>
#include <hkscenedata/mesh/hkxMeshSection.h>
#include <hkscenedata/mesh/hkxVertexBuffer.h>
#include <hkscenedata/mesh/hkxVertexFormat.h>
#include <hkscenedata/mesh/hkxIndexBuffer.h>

#include <hkfiltermanager/hkFilterUtils.h>

hkFilterSceneAlterMeshDesc g_SceneAlterMeshDesc;

hkFilterSceneAlterMesh::hkFilterSceneAlterMesh()
: m_optionsDialog(NULL)
{
	m_options.m_removeIndices = true;
}
	
hkFilterSceneAlterMesh::~hkFilterSceneAlterMesh()
{

}

void hkFilterSceneAlterMesh::setOptions(const void* optionData, int optionDataSize, unsigned int version)
{	
	// Check if the options have been saved as raw data.
	if ( version < HK_FILTER_MAKE_VERSION(1,0,1) )
	{
		hkString::memCpy( &m_options, optionData, optionDataSize );
		return;
	}
	else if ( version != g_SceneAlterMeshDesc.getFilterVersion() )
	{
		HK_WARN_ALWAYS( 0xabba7ade, "The " << g_SceneAlterMeshDesc.getShortName() << " option data was of an incompatible version and could not be loaded." );
		return;
	}

	// Get the options from the XML data.
	if ( hkFilterUtils::hkFilterReadOptionsXml( optionData, optionDataSize, m_optionsBuf, hkFilterSceneAlterMeshOptionsClass ) == HK_SUCCESS )
	{
		hkFilterSceneAlterMeshOptions* options = reinterpret_cast<hkFilterSceneAlterMeshOptions*>( m_optionsBuf.begin() );

		m_options.m_removeIndices = options->m_removeIndices;
	}
	else
	{
		HK_WARN_ALWAYS( 0xabba482b, "The XML for the " << g_SceneAlterMeshDesc.getShortName() << " option data could not be loaded." );
		return;
	}
}

int hkFilterSceneAlterMesh::getOptionsSize() const
{
	hkFilterUtils::hkFilterWriteOptionsXml( hkFilterSceneAlterMeshOptionsClass, &m_options, m_optionsBuf, g_SceneAlterMeshDesc.getShortName() );
	return m_optionsBuf.getSize();
}

void hkFilterSceneAlterMesh::getOptions(void* optionData) const
{
	// can memcpy as the options have no strings, char* etc.
	hkString::memCpy( optionData, m_optionsBuf.begin(), m_optionsBuf.getSize() );
}

inline hkInt16 _getIndex16( hkxIndexBuffer* ib, int i )
{
	return (hkInt16)(ib->m_vertexBaseOffset + ( ib->m_indices16? ib->m_indices16[i] : i )); 
}

inline hkInt32 _getIndex32( hkxIndexBuffer* ib, int i )
{
	return (hkInt32)(ib->m_vertexBaseOffset + ( ib->m_indices32? ib->m_indices32[i] : i )); 
}

void hkFilterSceneAlterMesh::process( hkRootLevelContainer& data, hkFilterClassRegistry& classReg, bool batchMode, const char* infoString, const char* configName, HWND ownerWindow )
{
	// EXP-370
	hkMemory& sceneMemory = hkMemory::getInstance();

	// Find the scene in the root level container
	hkxScene* scenePtr = reinterpret_cast<hkxScene*>( data.findObjectByType( hkxSceneClass.getName() ) );
	if (scenePtr == HK_NULL)
	{
		HK_WARN_ALWAYS(0xabbaa5f0, "No scene data found");
		return;
	}
	hkxScene& scene = *scenePtr;

	// Remove Index Buffers 
	// It removes the indices, but leaves the index buffer as they contain the triangle storage (list, strip, etc)
	// so become an offset into the vb instead of a buffer itself.
	if (m_options.m_removeIndices)
	{
		for (int i=0; i < scene.m_numMeshes; ++i)
		{
		
			hkxMesh* curMesh = scene.m_meshes[i];
			for (int j=0; j < curMesh->m_numSections; ++j)
			{
				hkxMeshSection* section = curMesh->m_sections[j];
				hkxVertexBuffer* origVB = section->m_vertexBuffer;
				if (!origVB) continue; // dodgy

				int unwoundVerts = 0;
				bool trivialBuffers = true;
				int ib;
				for (ib=0; ib < section->m_numIndexBuffers; ++ib)
				{
					unwoundVerts += section->m_indexBuffers[ib]->m_length;
					trivialBuffers &= (section->m_indexBuffers[ib]->m_indices16 == HK_NULL) && (section->m_indexBuffers[ib]->m_indices32 == HK_NULL);
				}

				if (trivialBuffers || (unwoundVerts < 1)) // all tivial (no actual indices) or no buffers at all
					continue; // next section

				// allocate enough space for all the verts
				int vertexSize = origVB->m_format->m_stride;
				char* origVbuf = (char*) origVB->m_vertexData;
				char* vbuf = (char*)( sceneMemory.alignedAllocate(16, vertexSize * unwoundVerts, HK_MEMORY_CLASS_EXPORT) );
				char* curVertex = vbuf;
				int curVertexIndex = 0;
				for (ib=0; ib < section->m_numIndexBuffers; ++ib)
				{
					hkxIndexBuffer* indexBuf = section->m_indexBuffers[ib];
					bool sixteenBit = (indexBuf->m_indices16 != HK_NULL);
					unsigned int numI = section->m_indexBuffers[ib]->m_length;
					for (hkUint32 ci=0; ci < numI; ++ci)
					{
						char* origVert = origVbuf + (( sixteenBit? _getIndex16(indexBuf, ci) : _getIndex32(indexBuf, ci) ) * vertexSize );
						hkString::memCpy(curVertex, origVert, vertexSize); // copy the vert
						curVertex += vertexSize;
					}

					indexBuf->m_vertexBaseOffset = curVertexIndex;
					curVertexIndex += numI;
					indexBuf->m_indices16 = HK_NULL; // previous owner will deallocate old buf
					indexBuf->m_numIndices16 = 0; 
					indexBuf->m_indices32 = HK_NULL; // previous owner will deallocate old buf
					indexBuf->m_numIndices32 = 0; 
				}

				// set the vb ptrs
				section->m_vertexBuffer->m_vertexData = vbuf; // previous owner will deallocate old buf
				section->m_vertexBuffer->m_numVertexData = unwoundVerts;
			}
		}
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
