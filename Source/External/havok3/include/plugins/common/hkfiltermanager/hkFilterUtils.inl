/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkfiltermanager/hkFilterUtils.h>

#include <hkserialize/util/xml/hkXmlParser.h>
#include <hkbase/stream/hkIstream.h>
#include <hkserialize/serialize/xml/hkXmlObjectReader.h>

#include <stdlib.h>
#include <shlwapi.h>

inline void* hkFilterUtils::hkFilterInterpretOptions( void* data )
{
	// Relocations:
	int* curInt = reinterpret_cast<int*>( data );

	// num local fixups
	int numLocal = *( curInt++ );

	// start of the cinfo data
	char* dataBegin = ((char*)data) + sizeof(int) + sizeof(int)*2*numLocal;
	for (int nl =0; nl < numLocal; ++nl)
	{
		int srcOff = *(curInt++);
		if (srcOff >=0 )
		{
			int dstOff = *(curInt++);
			*(hkUlong*)(dataBegin+srcOff) = hkUlong(dataBegin+dstOff);
		}
	}
	return dataBegin;
}

inline void* hkFilterUtils::hkFilterWriteOptionsXml( const hkClass& klass, const void* data, hkArray<char>& buffer, const char* filterName )
{
	buffer.clear();
	hkRelocationInfo ri;
	hkArrayStreamWriter wr( &buffer, hkArrayStreamWriter::ARRAY_BORROW );
	
	hkXmlObjectWriter xmlWriter;
	xmlWriter.adjustIndent( 1 );

	xmlWriter.beginObject( &wr, filterName, klass.getName() );
	xmlWriter.writeObject( &wr, data, klass, ri );
	xmlWriter.endObject( &wr );

	// Return the location of the data.
	return buffer.begin(); 
}

inline hkResult hkFilterUtils::hkFilterReadOptionsXml( const void* optionData, const int optionDataSize, hkArray<char>& buffer, const hkClass klass )
{
	hkXmlParser parser;
	hkXmlObjectReader xmlReader( &parser );

	hkIstream optionStream( optionData, optionDataSize );
	HK_ASSERT( 0x5469fae1, optionStream.isOk() );

	hkStreamReader* reader = optionStream.getStreamReader();

	hkRelocationInfo ri;

	if ( xmlReader.readObject( reader, buffer, klass, ri ) == HK_SUCCESS )
	{
		ri.applyLocalAndGlobal( buffer.begin() );
		return HK_SUCCESS;
	}

	return HK_FAILURE;
}

inline void* hkFilterUtils::hkFilterDeepCopyObject(hkFilterClassRegistry* classReg,
											const void* obj, const hkClass* klass,
											hkArray<char>& storage)
{
	void* retObj = HK_NULL;
	hkBinaryPackfileWriter pw;
	pw.setContentsWithRegistry(obj, *klass, &classReg->getVtableRegistry() );

	hkBinaryPackfileWriter::Options o;
	o.m_userTag = 0x0;
	o.m_writeMetaInfo = true; // no need to copy class info?

	hkArray<char> localStorage;
	hkArrayStreamWriter asw(&localStorage, hkArrayStreamWriter::ARRAY_BORROW);
	if ( pw.save( &asw, o ) == HK_SUCCESS )
	{
		hkBinaryPackfileReader pr;
		hkMemoryStreamReader msr(localStorage.begin(), localStorage.getSize(), hkMemoryStreamReader::MEMORY_INPLACE );

		// Load the file header
		hkUint32 totalBufferSize = 0;
		if ( (pr.loadFileHeader(&msr) == HK_SUCCESS ) && (pr.loadSectionHeadersNoSeek(&msr) == HK_SUCCESS))
		{
			// Load the sections manually
			int s;
			for (s=0; s < pr.getNumSections(); s++)
			{
				// Grab the section header
				const hkPackfileSectionHeader& sHeader = pr.getSectionHeader( s );
				totalBufferSize += sHeader.m_endOffset;
			}			
			storage.setSize(totalBufferSize);
			totalBufferSize = 0;
			for (s=0; s < pr.getNumSections(); s++)
			{
				const hkPackfileSectionHeader& sHeader = pr.getSectionHeader( s );
				// Load the section (fixes up local fixups)
				if (sHeader.m_endOffset > 0) // can't deref 'storage[totalBufferSize]'' if last section is size 0 say 
				{
					if (pr.loadSection( &msr, s, &storage[totalBufferSize] ) == HK_FAILURE )
					{
						totalBufferSize = 0;
						break;
					}
					totalBufferSize += sHeader.m_endOffset;
				}
			}
		}

		// Do global fixups
		if (totalBufferSize > 0)
		{
			pr.fixupGlobalReferences();
			retObj = pr.getContentsWithRegistry( hkRootLevelContainerClass.getName(), &classReg->getFinishObjectRegistry());
		}
		else
		{
			storage.setSize(0);
		}
	}
	return retObj; 
}

inline hkString hkFilterUtils::hkFilterReplaceVariables(const char* inputString, const char* config, const char* fullAssetName )
{
	hkString outputString = hkString(inputString);

	// Find the asset name, i.e. remove the path and file extension.
	hkString assetName = hkString( fullAssetName );
	{
		int offset = assetName.lastIndexOf( '\\' ) + 1;
		int len = ( assetName.lastIndexOf( '.' ) - offset > 0 ) ? assetName.lastIndexOf( '.' ) - offset : assetName.getLength();

		assetName = assetName.substr( offset, len );

		if( assetName.getLength() == 0 )
		{
			assetName = "untitled";
		}
	}

	// The string variables to replace.
	hkString assetVariable = "$(asset)";
	hkString configurationVariable = "$(configuration)";

	// Check any $ symbols in the string.
	int dollarIndex = outputString.indexOf( '$' );
	while( dollarIndex != -1 )
	{
		hkString curStringVariable = "";
		hkString replacementString = "";

		// Determine what the variable is and what to replace it with.
		if( outputString.asLowerCase().substr( dollarIndex ).beginsWith( assetVariable.cString() ) )
		{
			curStringVariable = assetVariable;
			replacementString = assetName;
		}
		else if( outputString.asLowerCase().substr( dollarIndex ).beginsWith( configurationVariable.cString() ) )
		{
			curStringVariable = configurationVariable;
			replacementString = hkString(config);
		}
		else
		{
			int bracketIndex1 = outputString.indexOf( '(', dollarIndex );
			int bracketIndex2 = outputString.indexOf( ')', bracketIndex1 );

			if( ( bracketIndex1 == dollarIndex+1 ) && ( bracketIndex2 > bracketIndex1 ) )
			{
				curStringVariable = outputString.substr( dollarIndex, bracketIndex2+1 - dollarIndex );
				replacementString = hkString( getenv( curStringVariable.substr( 2, curStringVariable.getLength()-3 ).cString() ) );
			}
		}

		// Replace the variable.
		if( replacementString.getLength() > 0 )
		{
			outputString = outputString.substr( 0, dollarIndex ) 
					+ replacementString 
					+ outputString.substr( dollarIndex + curStringVariable.getLength() );
				
			dollarIndex = outputString.indexOf( '$', dollarIndex + replacementString.getLength() );
		}
		else
		{
			dollarIndex = outputString.indexOf( '$', dollarIndex+1 );
		}
	}

	return outputString;
}

inline void hkFilterUtils::createArrayFromDelimitedString( hkObjectArray<hkString>& buf, const char* delimitedString, const char delimiter )
{
	buf.setSize( 0 );

	int curCh = 0;
	int prevCh = 0;

	while ( delimitedString && delimitedString[curCh] )
	{
		prevCh = curCh;
		while ( delimitedString[curCh] && ( delimitedString[curCh] != delimiter ) )
		{
			curCh++;
		}
		if ( curCh > prevCh )
		{
			hkString* str = buf.expandBy(1);
			*str = hkString( &( delimitedString[prevCh] ), curCh - prevCh );
		}
		if ( delimitedString[curCh] )
		{
			curCh++;
		}
	}
}

	/// Create a single string from the given array of strings, using "delimiter" to split the strings.
inline void hkFilterUtils::createDelimitedStringFromArray( const hkObjectArray<hkString>& array, const char* delimiter, hkString& delimitedStringOut)
{
	delimitedStringOut = "";

	for (int i=0; i<array.getSize(); i++)
	{
		delimitedStringOut += array[i];
		delimitedStringOut += delimiter;
	}
}

inline void hkFilterUtils::getAssetPath( const char* fullAssetFilename, hkString& assetPath )
{
	int assetLength = hkString::strLen( fullAssetFilename );
	if( assetLength > 1 )
	{
		int curIndex = assetLength-1;

		while( (curIndex >= 0) && (fullAssetFilename[curIndex] != '\\') && (fullAssetFilename[curIndex] != '/') )
		{
			curIndex--;
		}

		assetPath = curIndex > 0? hkString( fullAssetFilename, curIndex + 1 ) : ""; // include the '\' etc
	}
	else
	{
		assetPath = ""; 
	}
}


inline void hkFilterUtils::getReducedFilename( const char* fname, const hkString& assetPath, hkString& reducedName )
{
	// Take m_assetPath out of fname
	char rel[MAX_PATH];

	hkString assetPathLessLast( assetPath.cString(), assetPath.getLength() > 0? assetPath.getLength() - 1 : 0 );

	if ( PathFileExists( assetPathLessLast.cString() ) && 
		 PathRelativePathTo( rel, assetPathLessLast.cString(), FILE_ATTRIBUTE_DIRECTORY, fname, FILE_ATTRIBUTE_NORMAL) )
	{
		reducedName = rel;
	}
	else
	{
		reducedName = fname; // ah well...
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
