/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_SERIALIZE_XML_OBJECT_WRITER_H
#define HK_SERIALIZE_XML_OBJECT_WRITER_H

#include <hkbase/hkBase.h>
#include <hkserialize/serialize/hkObjectWriter.h>
#include <hkbase/htl/hkPointerMap.h>

class hkStreamWriter;
class hkClass;

/// Writes a single object to an XML stream.
class hkXmlObjectWriter : public hkObjectWriter
{
	public:

			/// Create an xml object writer.
		hkXmlObjectWriter();

			/// Begin an object. Usually called before writeObject.
		void beginObject(hkStreamWriter* writer, const char* name, const char* klassName);

			/// End an object. Called after writeObject.
		void endObject(hkStreamWriter* writer);

			/// Opens a new element with optional attributes.
			/// "attributes" is a optional null-terminated array of attribute name value pairs.
			/// The indentation is increased by one.
			/// Optionally write a newline before the tag if prefixNewline is true.
		void beginElement(hkStreamWriter* writer, const char* name, const char*const* attributes=HK_NULL, hkBool prefixNewline=true);

			/// End an element. Optionally write a newline before the tag if prefixNewline is true.
		void endElement(hkStreamWriter* writer, const char* name, hkBool prefixNewline=true);

			/// Save object data, using class information klass.
			/// Note that this writer does not supply relocations, since
			/// these must be computed at load time on the host.
		hkResult writeObject(hkStreamWriter* writer, const void* data, const hkClass& klass, hkRelocationInfo& reloc);

			/// Write a raw binary chunk.
		hkResult writeRaw(hkStreamWriter* writer, const void* buf, int len );

			/// Change the indentation level by delta tabstops.
		void adjustIndent( int delta );

			/// Get the index of this object.
		int addressToIndex(const void* object);

			/// Write null instead of references to this object.
		void ignoreObject(const void* object);

			/// Write replacement instead of original.
		void replaceObject(const void* original, const void* replacement);

			/// Get the string identifier of object
			/// The default implementation uses the index as a string. e.g. "#0042".
		virtual hkString addressToName(const void* object);

	public:

		static hkResult HK_CALL base64write( hkStreamWriter* w, const void* buf, int len );

	private:

		hkInplaceArray<char,16> m_indent;
		hkPointerMap<const void*, int> m_indexMap;
		hkPointerMap<const void*, const void*> m_replacements;
};

#endif //HK_SERIALIZE_XML_OBJECT_WRITER_H

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
