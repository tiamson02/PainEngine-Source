/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HAVOK_FILTER_UTILS__H
#define HAVOK_FILTER_UTILS__H

#include <hkbase/stream/impl/hkArrayStreamWriter.h>
#include <hkbase/stream/impl/hkMemoryStreamReader.h>
#include <hkserialize/packfile/binary/hkBinaryPackfileWriter.h>
#include <hkserialize/packfile/binary/hkBinaryPackfileReader.h>
#include <hkserialize/serialize/platform/hkPlatformObjectWriter.h>
#include <hkserialize/serialize/hkRelocationInfo.h>
#include <hkserialize/util/hkRootLevelContainer.h>
#include <hkserialize/util/hkFinishLoadedObjectRegistry.h>
#include <hkserialize/util/hkFilterClassRegistry.h>
#include <hkserialize/packfile/binary/hkPackfileSectionHeader.h>
#include <hkscenedata/scene/hkxScene.h>

#include <hkserialize/serialize/xml/hkXmlObjectWriter.h>

	/// A collection of useful utility methods for the filters.
class hkFilterUtils
{
public:

		/// Get the position of the cinfo data (i.e. not the local fixups) in the options data block.
	inline static void* hkFilterInterpretOptions( void* data );

		/// Write the options data to memory. A pointer to the data is returned.
	inline static void* hkFilterWriteOptionsXml( const hkClass& klass, const void* data, hkArray<char>& buffer, const char* filterName );

		/// Read the options data from memory.
	inline static hkResult hkFilterReadOptionsXml( const void* optionData, const int optionDataSize, hkArray<char>& buffer, const hkClass klass );

		/// Create a complete copy of an entire filter set,
		/// so that processing does not affect the original data.
	inline static void* hkFilterDeepCopyObject(hkFilterClassRegistry* classReg, const void* obj, const hkClass* klass, hkArray<char>& storage);

		/// Replace the string variables in the given string with the proper values.
		/// $(asset) = the name of the Max or Maya asset the scene has been taken from.
		/// $(configuration) = the name of the configuration the filter belongs to.
		/// $(<ENV>) = the value of the specified environment variable.
	inline static hkString hkFilterReplaceVariables(const char* inputString, const char* config, const char* fullAssetName );

		/// Create an array using the items listed in the delimited string.
	inline static void createArrayFromDelimitedString( hkObjectArray<hkString>& buf, const char* delimitedString, const char delimiter );

		/// Create a single string from the given array of strings, using "delimiter" to split the strings.
	inline static void createDelimitedStringFromArray( const hkObjectArray<hkString>& array, const char* delimiter, hkString& delimitedStringOut);

		/// Returns the path to the specified asset file.
	inline static void hkFilterUtils::getAssetPath( const char* fullAssetFilename, hkString& assetPath );

		/// Get the path to the specified file relative to the specified asset path.
	inline static void hkFilterUtils::getReducedFilename( const char* fname, const hkString& assetPath, hkString& reducedName );
};

#include <hkfiltermanager/hkFilterUtils.inl>

#endif // HAVOK_FILTER_UTILS__H

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
