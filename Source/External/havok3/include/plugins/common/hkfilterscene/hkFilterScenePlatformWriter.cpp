/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkfilterscene/hkFilterScene.h>
#include <hkfilterscene/hkFilterScenePlatformWriter.h>
#include <hkfilterscene/hkFilterScenePlatformWriterOptions.h>
#include <hkscenedata/scene/hkxSceneUtils.h>
#include <hkfiltermanager/hkFilterUtils.h>

hkFilterScenePlatformWriterDesc g_PlatformWriterDesc;

hkFilterScenePlatformWriter::hkFilterScenePlatformWriter()
:	m_optionsDialog(NULL)
{
	m_options.m_filename = HK_NULL;
	m_options.m_preset = hkFilterScenePlatformWriterOptions::MSVC_WIN32;
	m_options.m_bytesInPointer = 4;
	m_options.m_littleEndian = 1;
	m_options.m_reusePaddingOptimized = 0;
	m_options.m_emptyBaseClassOptimized = 1;
	m_options.m_removeMetadata = false;
	m_options.m_userTag = 0;
}
	
hkFilterScenePlatformWriter::~hkFilterScenePlatformWriter()
{

}


// process() is in the Dialog cpp so as to make use of the static lookup tables etc.

void hkFilterScenePlatformWriter::setOptions(const void* optionData, int optionDataSize, unsigned int version)
{
	hkFilterScenePlatformWriterOptions* options;

	// Check if the options have been saved as raw data.
	if ( version < HK_FILTER_MAKE_VERSION(1,0,1) )
	{
		m_optionsBuf.setSize(optionDataSize);
		hkString::memCpy( m_optionsBuf.begin(), optionData, optionDataSize );
		options = (hkFilterScenePlatformWriterOptions*)hkFilterUtils::hkFilterInterpretOptions(m_optionsBuf.begin());
	}
	else if ( version != g_PlatformWriterDesc.getFilterVersion() )
	{
		HK_WARN_ALWAYS( 0xabba4722, "The " << g_PlatformWriterDesc.getShortName() << " option data was of an incompatible version and could not be loaded." );
		return;
	}
	else
	{
		// Get the options from the XML data.
		if ( hkFilterUtils::hkFilterReadOptionsXml( optionData, optionDataSize, m_optionsBuf, hkFilterScenePlatformWriterOptionsClass ) == HK_SUCCESS )
		{
			options = reinterpret_cast<hkFilterScenePlatformWriterOptions*>( m_optionsBuf.begin() );
		}
		else
		{
			HK_WARN_ALWAYS( 0xabba7523, "The XML for the " << g_PlatformWriterDesc.getShortName() << " option data could not be loaded." );
			return;
		}
	}

	// the filename ptr will only be valid as long as the buf still is (until next write)
	// so we store the filename separately
	m_filename = options->m_filename;
	m_options = *options; // copy the main part.
}
		  
int hkFilterScenePlatformWriter::getOptionsSize() const
{
	m_options.m_filename = (char*) m_filename.cString(); // so that the ptr is valid

	hkFilterUtils::hkFilterWriteOptionsXml(hkFilterScenePlatformWriterOptionsClass, &m_options, m_optionsBuf, g_PlatformWriterDesc.getShortName() );
	return m_optionsBuf.getSize();
}

void hkFilterScenePlatformWriter::getOptions(void* optionData) const
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
