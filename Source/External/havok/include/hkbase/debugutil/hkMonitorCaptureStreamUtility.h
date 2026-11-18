/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKMONITOR_CAPTURE_STREAM_UTILITY_H
#define HKBASE_HKMONITOR_CAPTURE_STREAM_UTILITY_H

#include <hkbase/debugutil/hkMonitor.h>

/// Allows to capture several frames and convert the data stream into a stream that can be moved accross the network
class hkMonitorCaptureStreamUtility
{
	public:

		struct Node
		{
			const char*		m_name;
			float			m_value;
			float			m_value2;
			hkMonitor::Type m_type;
			Node*			m_parent;
			hkArray<Node*>	m_children;

			Node(Node* parent, const char* name, hkMonitor::Type type)
			{
				m_parent = parent;
				if (parent){ parent->m_children.pushBack(this);}
				m_name = name;
				m_value = 0.0f;
				m_value2 = 0.0f;
				m_type = type;
			}

			~Node();
		};
		
		enum
		{
			REPORT_ALL = 0xFF,
			REPORT_TOTAL = 0x01,
			REPORT_PERFRAME_TIME = 0x02,
			REPORT_PERFRAME_SUMMARY = 0x04,
			REPORT_PERFRAME_DETAIL = 0x08
		};
		
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MONITOR, hkMonitorCaptureStreamUtility)

		hkMonitorCaptureStreamUtility( int maxMemorySize );
		~hkMonitorCaptureStreamUtility();

			/// Should be called at the end of each frame, all the data from one frame gets appended at the end of the internal memory
		void captureFrame();

			/// Should be called to recreate the monitor stream from the serialized monitor data.
			/// The exported data is setup in the memory buffer of hkMonitorStream which can be
			/// parsed by hkMonitorBank::examineMonitorCapture(...)
			/// The exported data contains pointers to string data allocated in this
			/// hkMinitorCaptureStreamUtility so it must not be deleted until the exported data
			/// has been parsed and hkMonitorStream::reset/resize has been called.
			/// Note1: Local hkMonitorStream data will be overwritten by this call.
			/// Note2: Currently if there is more than one serialized frame to export the exported
			/// data is not valid.  (This is not a problem in practice.)
		void exportFrame();

			/// complete reset
		void reset();

			/// write the raw captured data to a file
		void writeRawDataToFile( const char* filename );

			/// read the raw captured data from a file
		hkResult readRawDataFromFile( const char* filename );
		
			/// create a user readable statistics of the raw data
		void writeStatistics( hkOstream& outstream, int reportLevel = REPORT_ALL );

			/// Create a statistics tree, one root node for each frame
		void makeStatisticsTree(hkArray<Node*>& m_nodesOut);

			/// extract the low level data
		char* getData() { return &m_data[0]; }

			/// get the size of the data
		int getDataSize() { return m_data.getSize(); }

		void setDataByCopying(const char* buffer, int length);
		//void updateStringMap();

	protected:

		struct StringMap
		{
			const void *m_id;
			const char *m_string;
		};

	protected:

		void createMonitor( const char* name, int len = 0 );
		const char* remapString( const char* id );
		void addString( const char* id, const char* string );

		Node* newNode( Node* parent, const char* name, hkMonitor::Type type );

		hkArray<StringMap> m_mappings;
		//hkArray<const char*> m_strings; this will be used for buffering streamed 
		hkArray<char> m_data;

	public:
		hkReal m_timerFactor0;
		hkReal m_timerFactor1;
		hkString m_heading;
		hkString m_nodeIdForFrameOverview;
};

#endif // HKBASE_HKMONITOR_CAPTURE_STREAM_UTILITY_H

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
