/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_PACKFILE_READER_H
#define HK_PACKFILE_READER_H

#include <hkbase/hkBase.h>

class hkStreamReader;
class hkObjectUpdateTracker;
class hkClassNameRegistry;
class hkFinishLoadedObjectRegistry;
template <typename T> class hkStringMap;
template <typename T> class hkPointerMultiMap;

/// Base interface to read in a packed file.
/// Reading is done two steps - first the file is read
/// and reconstructed in memory (loadEntireFile). Secondly
/// versioning, pointer fixups and finishing functions
/// are run (getContents).
class hkPackfileReader : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SERIALIZE);

			/// Simple interface. Load the entire file, fixes up all pointers.
			/// hkBinaryPackfileReader has finer grained loading operations.
			/// hkXmlPackfile reader needs class descriptions to load the objects.
			/// We look for class descriptions in two places : first the file itself
			/// and secondly in the hkBuiltinTypeRegistry. See also
			/// hkXmlPackfileReader::loadEntireFileWithRegistry.
		virtual hkResult loadEntireFile( hkStreamReader* reader ) = 0;

			/// Get the top level object in the file.
			/// This function assumes that the packfile version matches the in-memory
			/// version. You may use hkVersionUtil if this is not the case. See the
			/// serialization section of the manual for more details.
			/// Usually you will use getContents instead of this function.
			/// If the packfile contents type do not match the expected class type
			/// this method will return HK_NULL. You may pass HK_NULL as the expected
			/// class to disable this check.
			/// Some objects will require a finishing step. i.e. to initialize
			/// members which have not been serialized or to initialize vtables.
			/// If "finish" is not null, apply these finishing steps.
		virtual void* getContentsWithRegistry( const char* expectedClassName, const hkFinishLoadedObjectRegistry* finish ) = 0;

			/// Get the top level object in the file.
			/// Calls getContentsWithRegistry with the finish registry from hkBuiltinTypeRegistry.
		virtual void* getContents( const char* expectedClassName );

			/// Get the class name of contents, if available.
		virtual const char* getContentsClassName() = 0;

			//
			// Versioning access functions.
			//

			/// Get all objects in the packfile.
			/// Used by the versioning code. Note that if a packfile has been
			/// stripped of metadata, then this array may be empty.
		virtual hkArray<hkVariant>& getLoadedObjects() = 0;

			/// Get a tracker which knows about the internal structure of the packfile.
		virtual hkObjectUpdateTracker& getUpdateTracker() = 0;

			/// Get a string representing the version of objects within the file.
			/// E.g. "Havok-3.2.0". 
		virtual const char* getOriginalContentsVersion() = 0;


			//
			// Utility
			//

			/// The format of a packfile.
		enum FormatType
		{
			FORMAT_ERROR, // read or open error.
			FORMAT_UNKNOWN, // readable but not recognised.
			FORMAT_BINARY, // binary packfile
			FORMAT_XML, // xml packfile

		};

			/// Detect the type of a packfile stream.
		static hkEnum<FormatType,hkInt32> HK_CALL detectFormat( hkStreamReader* reader );

	public:

			/// A handle to the memory allocations made by any packfile reader
		class AllocatedData : public hkReferencedObject
		{
			public:

				~AllocatedData();

				struct Chunk
				{
					Chunk(void* p, int n, HK_MEMORY_CLASS c) : pointer(p), numBytes(n), memClass(c) { }
					void* pointer;
					int numBytes;
					HK_MEMORY_CLASS memClass;
				};

				void addAllocation(void* p) { m_memory.pushBack(p); }
				void addChunk(void*p, int n, HK_MEMORY_CLASS c) { m_chunks.pushBack( Chunk(p,n,c) ); }

			protected:

				hkArray<void*> m_memory;
				hkArray<Chunk> m_chunks;
		};


			/// Get a handle to memory allocations made during loading.
			/// You can add a reference to this, and delete the pack file reader after loading.
			/// Removing your reference will deallocate any loaded data.
		virtual AllocatedData* getAllocatedData() = 0;

	public:

			///
		typedef const char* SectionTag;

			///
		static void convertClassVersion1Inplace( hkClass* classInOut );
};

#endif // HK_PACKFILE_READER_H

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
