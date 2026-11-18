/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKMONITOR_STREAM_STATISTICS_COLLECTOR_H
#define HKBASE_HKMONITOR_STREAM_STATISTICS_COLLECTOR_H

#include <hkbase/debugutil/hkStatisticsCollector.h>
#include <hkbase/monitor/hkMonitorStreamAnalyzer.h>
#include <hkbase/htl/hkPointerMap.h>

#define HK_MEM_STREAM_COMMAND_PUSH  'P' // new dir or new node
#define HK_MEM_STREAM_COMMAND_POP   'p' // out of node/dir.
#define HK_MEM_STREAM_COMMAND_CHUNK 'C' // new chunk alloc

/// Similar to simple stat collector except that it
/// does not create a nodes, rather it writes to a command
/// stream enough data to create the nodes on demand latter 
/// possibly on a different platform (for use with the VDB)
class hkStreamStatisticsCollector: public hkReferencedObject, public hkStatisticsCollector
{
	public:

		hkStreamStatisticsCollector(int memSize);

		virtual ~hkStreamStatisticsCollector();
		
		typedef hkMonitorStreamAnalyzer::Node Node;

			/// begin one snapshot, statisticClasses is an ordered field
		void beginSnapshot( int statisticClasses );

		void endSnapshot( );

		void reset();
		
			// set the array from a different source
		void setDataByCopying(const char* buffer, int length, hkBool endianSwap = false);

			// make a node tree from the stream
		void makeTree(hkArray<Node*>& m_nodesOut, hkObjectArray<hkString>* stringTable = HK_NULL);

			// write stats calls makeTree first and deletes the nodes once done
		void writeStatistics( hkOstream& outstream, int reportLevel );
		
		virtual void beginObject( const char* name, StatisticClass statisticClass, const hkReferencedObject* object );

		virtual void addChunk( const char* name, StatisticClass statisticClass, const void* chunkAddress, int usedSize, int allocatedSize);

		virtual void addChildObject( const char* name, StatisticClass statisticClass, const hkReferencedObject* object );

		virtual void endObject( );

		virtual void pushDir( const char* dirName );

		virtual void popDir();

		inline const char* getDataStream() { return m_stream.getSize() > 0? m_stream.begin() : HK_NULL; }
		inline int getDataStreamSize() { return m_stream.getSize(); }

	public:

		struct Command
		{
			char cmd; /// one of the three above
			hkUint8 slen; // chars following for name string. can be 0.
			hkUint16 memSize; // 0 for just a dir 
			hkUint16 allocSize; // 0 for just a dir
		};

		hkArray<char> m_stream; // stream of commands (and names)

		inline hkBool hasSpace() { return m_stream.getCapacity() > (m_stream.getSize() + (int)sizeof(Command) + 64); }

	protected:

		hkPointerMap<hkReferencedObject*,int> m_knownObjects;
		int	m_enabledStatisticClasses;
};


#endif // HKBASE_HKMONITOR_STREAM_STATISTICS_COLLECTOR_H

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
