/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKMONITOR_SIMPLE_STATISTICS_COLLECTOR_H
#define HKBASE_HKMONITOR_SIMPLE_STATISTICS_COLLECTOR_H

#include <hkbase/debugutil/hkStatisticsCollector.h>
#include <hkbase/monitor/hkMonitorStreamAnalyzer.h>
#include <hkbase/htl/hkPointerMap.h>

class hkSimpleStatisticsCollector: public hkReferencedObject, public hkStatisticsCollector
{
	public:
		hkSimpleStatisticsCollector();

		virtual ~hkSimpleStatisticsCollector();
		
			/// begin one snapshot, statisticClasses is an ordered field
		void beginSnapshot( int statisticClasses );

		void commitSnapshot(   );

			/// create a user readable statistics of the raw data
		void writeStatistics( hkOstream& outstream, int reportLevel = hkMonitorStreamAnalyzer::REPORT_SUMMARIES );

		void reset();
		
		typedef hkMonitorStreamAnalyzer::Node Node;

		virtual void beginObject( const char* name, StatisticClass statisticClass, const hkReferencedObject* object );

		virtual void addChunk( const char* name, StatisticClass statisticClass, const void* chunkAddress, int usedSize, int allocatedSize);

		virtual void addChildObject( const char* name, StatisticClass statisticClass, const hkReferencedObject* object );

		virtual void endObject( );

		virtual void pushDir( const char* dirName );

		virtual void popDir();

	protected:

		Node* newNode( Node* parent, const char* name, int size, int allocatedSize);

	public:
		hkArray<Node*> m_snapshots;

	protected:
		hkArray<Node*> m_currentStack;
		hkPointerMap<hkReferencedObject*,int> m_knownObjects;
		int	m_enabledStatisticClasses;
};


#endif // HKBASE_HKMONITOR_SIMPLE_STATISTICS_COLLECTOR_H

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
