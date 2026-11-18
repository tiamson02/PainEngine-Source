/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKSERIALIZE_SERIALIZE_BINARY_HKPACKFILESECTIONHEADER_XML_H
#define HKSERIALIZE_SERIALIZE_BINARY_HKPACKFILESECTIONHEADER_XML_H


/// hkPackfileSectionHeader meta information
extern const class hkClass hkPackfileSectionHeaderClass;

/// 
class hkPackfileSectionHeader
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SERIALIZE, hkPackfileSectionHeader);
		HK_DECLARE_REFLECTION();
	
			/// Default constructor
		hkPackfileSectionHeader()
		{
			hkString::memSet(this,0,sizeof(this));
		}
		
	public:
		
			/// 
		char m_sectionTag[19];
		
			/// 
		char m_nullByte;
		
			/// Absolute file offset of where this sections data begins.
		hkInt32 m_absoluteDataStart;
		
			/// Offset of local fixups from absoluteDataStart.
		hkInt32 m_localFixupsOffset;
		
			/// Offset of global fixups from absoluteDataStart.
		hkInt32 m_globalFixupsOffset;
		
			/// Offset of virtual fixups from absoluteDataStart.
		hkInt32 m_virtualFixupsOffset;
		
			/// Offset of external fixups from absoluteDataStart.
		hkInt32 m_externalFixupsOffset;
		
			/// Offset of the end of section. Also the section size.
		hkInt32 m_endOffset;
		
			/// 
		hkInt32 m_spare;
};

#endif // HKSERIALIZE_SERIALIZE_BINARY_HKPACKFILESECTIONHEADER_XML_H

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
