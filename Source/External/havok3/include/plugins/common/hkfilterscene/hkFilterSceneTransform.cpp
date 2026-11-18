/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkfilterscene/hkFilterScene.h>
#include <hkfilterscene/hkFilterSceneTransform.h>
#include <hkscenedata/scene/hkxSceneUtils.h>

#include <hkfiltermanager/hkFilterUtils.h>

hkFilterSceneTransformDesc g_SceneTransformDesc;

hkFilterSceneTransform::hkFilterSceneTransform()
: m_optionsDialog(NULL), m_fillingControls(false), m_doNotRefreshEdit(false)
{
	m_options.m_matrix.setIdentity();
	m_options.m_applyToBuffers = true;
	m_options.m_applyToCameras = true;
	m_options.m_applyToLights = true;
	m_options.m_applyToNodes = true;
	m_options.m_flipWinding = false;
	m_options.m_preset = hkFilterSceneTransformOptions::IDENTITY;
}
	
hkFilterSceneTransform::~hkFilterSceneTransform()
{

}


void hkFilterSceneTransform::process( class hkRootLevelContainer& data, hkFilterClassRegistry& classReg, bool batchMode, const char* infoString, const char* configName, HWND ownerWindow )
{
	// Find the scene in the root level container
	hkxScene* scenePtr = reinterpret_cast<hkxScene*>( data.findObjectByType( hkxSceneClass.getName() ) );
	if (scenePtr == HK_NULL)
	{
		HK_WARN_ALWAYS (0xabbaa5f0, "No scene data found");
		return;
	}
	hkxScene& scene = *scenePtr;

	// we don't need to regsiter any classes as we are not adding any extra types that are not 'known' already.
	
	// which options to process with?
	hkxSceneUtils::SceneTransformOptions options;

	// 
	options.m_applyToBuffers = m_options.m_applyToBuffers;
	options.m_applyToCameras = m_options.m_applyToCameras;
	options.m_applyToLights = m_options.m_applyToLights;
	options.m_applyToNodes = m_options.m_applyToNodes;
	options.m_transformMatrix.setCols(m_options.m_matrix.getColumn(0), m_options.m_matrix.getColumn(1), m_options.m_matrix.getColumn(2));
	options.m_flipWinding = m_options.m_flipWinding;

	// use hkxSceneUtils to process the scene
	hkxSceneUtils::transformScene( scene, options );
}

void hkFilterSceneTransform::setOptions(const void* optionData, int optionDataSize, unsigned int version)
{
	// Check if the options have been saved as raw data.
	if ( version < HK_FILTER_MAKE_VERSION(1,2,1) )
	{
		hkString::memCpy( &m_options, optionData, optionDataSize );
		return;
	}
	else if ( version != g_SceneTransformDesc.getFilterVersion() )
	{
		HK_WARN_ALWAYS( 0xabba854d, "The " << g_SceneTransformDesc.getShortName() << " option data was of an incompatible version and could not be loaded." );
		return;
	}

	// Get the options from the XML data.
	if ( hkFilterUtils::hkFilterReadOptionsXml( optionData, optionDataSize, m_optionsBuf, hkFilterSceneTransformOptionsClass ) == HK_SUCCESS )
	{
		hkFilterSceneTransformOptions* options = reinterpret_cast<hkFilterSceneTransformOptions*>( m_optionsBuf.begin() );

		m_options = *options;
	}
	else
	{
		HK_WARN_ALWAYS( 0xabba72f3, "The XML for the " << g_SceneTransformDesc.getShortName() << " option data could not be loaded." );
		return;
	}
}

int hkFilterSceneTransform::getOptionsSize() const
{
	hkFilterUtils::hkFilterWriteOptionsXml( hkFilterSceneTransformOptionsClass, &m_options, m_optionsBuf, g_SceneTransformDesc.getShortName() );
	return m_optionsBuf.getSize();
}

void hkFilterSceneTransform::getOptions(void* optionData) const
{
	hkString::memCpy( optionData, m_optionsBuf.begin(), m_optionsBuf.getSize() );
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
