/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// see hkstreamsources for the definition of hkUsingCdrom

// The hkPackedStreambufSource will work with any underlying steambuf

class hkPackedStreambufSource : public hkStreambufFactory
{
	public:

		hkPackedStreambufSource(hkStreambuf* streambuf, bool seekBug=false);

		~hkPackedStreambufSource();

		virtual hkStreambuf* open(hkStreambuf::StdStream s)
		{
			return HK_NULL;
		}

		virtual hkStreambuf* open(const char* name, hkStreambuf::OpenMode mode);

	protected:

		hkStreambuf* m_streambuf;

		struct TocEntry
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, TocEntry)
			hkString m_filename;
			int m_length;
			int m_offset;
		};

		hkArray<TocEntry*> m_toc;

			// Does a fuzzy match on the filename to try and find an appropriate
			// entry. This method tries to be as forgiving as possible
			// it ignores case and will match substrings automagically
		TocEntry* findEntry(const hkString& filename);
};


template <typename T>
static void HK_CALL readRaw(hkStreambuf* sb, T& t)
{
	sb->read( reinterpret_cast<char*>(&t), sizeof(T));
}


hkPackedStreambufSource::hkPackedStreambufSource(hkStreambuf* streambuf, bool seekBug)
{
	///WARNING - all this assumes the same endian-ness as the packing tool
	// Switch this to use an hkIArchive if you need it for gamecube

	m_streambuf = streambuf;
	m_streambuf->addReference();

	if (m_streambuf->isOk())
	{

		// Read in the Toc
		// As with Zip files the Toc is stored at the end of the file
		hkUint32 tocStart;

		// Need to switch sign depending on whether or not we're reading from
		// PS2 cdrom
		if (seekBug)
		{
			m_streambuf->seek( sizeof(hkUint32), hkStreambuf::STREAM_END);
		}
		else
		{
			m_streambuf->seek( -hkSizeOf(hkUint32), hkStreambuf::STREAM_END);
		}
		readRaw(m_streambuf, tocStart);
		m_streambuf->seek(tocStart , hkStreambuf::STREAM_SET);

		hkUint32 numEntries;
		readRaw(m_streambuf, numEntries);

		for (unsigned i=0; i< numEntries; i++)
		{
			int filenameLength;
			readRaw(m_streambuf, filenameLength);

			hkArray<char> buf;
			buf.setSize(filenameLength+1);
			m_streambuf->read( &buf[0], filenameLength);
			buf[filenameLength] = '\0';

			int fileOffset;
			int fileLength;

			// read filesize and offset
			readRaw(m_streambuf, fileLength);
			readRaw(m_streambuf, fileOffset);

			hkprintf("File(%06d) Offset:%09d %s\n", fileLength, fileOffset, &buf[0]);

			// Store in Toc
			TocEntry* entry = new TocEntry;
			entry->m_filename = &buf[0];
			entry->m_length = fileLength;
			entry->m_offset = fileOffset;
			m_toc.pushBack(entry);
		}
	}
}

hkPackedStreambufSource::~hkPackedStreambufSource()
{
	for(int i=0; i<m_toc.getSize(); ++i)
	{
		delete m_toc[i];
	}
	m_streambuf->removeReference();
}

hkStreambuf* hkPackedStreambufSource::open(const char* name, hkStreambuf::OpenMode mode)
{
	if( mode == hkStreambuf::INPUT )
	{
		TocEntry* entry = findEntry(name);
		if( entry != HK_NULL )
		{
			m_streambuf->seek( entry->m_offset, hkStreambuf::STREAM_SET);

			hkArray<char> buf;
			buf.setSize(entry->m_length);
			int nread = m_streambuf->read( &buf[0], entry->m_length);

			if(nread == entry->m_length)
			{
				return new hkStringStreambuf(&buf[0], entry->m_length);
			}
		}
	}
	return HK_NULL;
}

// This does a pretty fuzzy match to try and find a file.
hkPackedStreambufSource::TocEntry* hkPackedStreambufSource::findEntry( const hkString& filename)
{
	// for PS2 we automatically strip host0: and cdrom0:
	hkString fname = filename;

	const hkString hostPrefix = filename.substr(0,6);
	if (hostPrefix.compareToIgnoreCase("host0:")==0)
	{
		fname = filename.substr(6, filename.getLength()-6);
	}

	const hkString cdromPrefix = filename.substr(0,7);
	if (cdromPrefix.compareToIgnoreCase("cdrom0:")==0)
	{
		fname = filename.substr(7, filename.getLength()-7);
	}

	for (int i=0; i< m_toc.getSize(); i++)
	{
		const TocEntry& current = *(m_toc[i]);

		if (fname.compareToIgnoreCase(current.m_filename) == 0)
		{
			return m_toc[i];
		}

		const int fLen = fname.getLength();
		const int eLen = m_toc[i]->m_filename.getLength();

		// If filname is shorter then compare it against the end of the entry filename
		if ((fLen < eLen) && (fname.compareToIgnoreCase(current.m_filename.substr(eLen - fLen, fLen))==0))
		{
			return m_toc[i];
		}

		// If filename is longer then compare end of it against the the entry filename
		if ((fLen > eLen) && (current.m_filename.compareToIgnoreCase(fname.substr(fLen - eLen, eLen))==0))
		{
			return m_toc[i];
		}
	}
	return false;
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
