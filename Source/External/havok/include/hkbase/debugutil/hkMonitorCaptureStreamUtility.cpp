/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>
#include <hkbase/stopwatch/hkStopwatch.h>
#include <hkbase/debugutil/hkMonitorCaptureStreamUtility.h>

extern "C"
{
#include <stdio.h>
}

hkMonitorCaptureStreamUtility::hkMonitorCaptureStreamUtility(int memorySize)
{
	m_data.reserve( memorySize );

#ifdef HK_PS2
	m_timerFactor0 = 1000000.0f  / 300e6f;
	m_timerFactor1 = 1000000.0f  / 300e6f;
	m_heading = "Timer values (icache, cpu cycles) are shown in usec (10e-6s)";
#else
	m_timerFactor0 = 1e6f / float(hkStopwatch::getTicksPerSecond());
	m_timerFactor1 = 0.0f;
	m_heading = "Timer values (cpu cycles) are shown in usec (10e-6s)";
#endif

	m_nodeIdForFrameOverview = "Simulate";
}


hkMonitorCaptureStreamUtility::~hkMonitorCaptureStreamUtility()
{

}

void hkMonitorCaptureStreamUtility::reset()
{
	m_mappings.clear();
	m_data.clear();
}

void hkMonitorCaptureStreamUtility::setDataByCopying(const char* buffer, int length)
{

	// We don't want to reset the string mappings as we don't want to resend
	// all the strings
	//reset();

	m_data.setSize(length);
	hkString::memCpy(&m_data[0], buffer, length);
}


hkMonitorCaptureStreamUtility::Node::~Node()
{
	for (int i=0; i< m_children.getSize();i++)
	{
		delete m_children[i];
	}
}


void hkMonitorCaptureStreamUtility::createMonitor( const char* name, int len )
{
	// search for existing mappings
	for (int i = m_mappings.getSize()-1; i>=0;i--)
	{
		if ( name == m_mappings[i].m_string )
		{
			return;
		}
	};

	// mark as known entry
	{
		StringMap map; map.m_id = name; map.m_string = name;
		m_mappings.pushBack( map );
	}

	// write command into buffer
	if ( len == 0 )
	{
		len = hkString::strLen(name)+1;
	}
	len = ( (len-1)& ~3) + 4;	// 4 byte aligned

	int totalSize = len + hkSizeOf(hkMonitorIdStringMap);

	char *data = m_data.expandBy(totalSize);
	hkMonitorIdStringMap* com = reinterpret_cast<hkMonitorIdStringMap*>(data);
	com->m_commandAndMonitor = HK_NULL;
	com->m_id = name;
	com->m_sizeOfString = len;
	hkString::memCpy( data + hkSizeOf(hkMonitorIdStringMap), name, len );
}

void hkMonitorCaptureStreamUtility::exportFrame()
{
	hkMonitorStream::reset();

	char* current = &m_data[0];
	char* end = current + m_data.getSize();

	while(current < end) // for all frames
	{
		hkMonitorCaptureCommand* com = reinterpret_cast<hkMonitorCaptureCommand*>(current);
		const char* string = HK_NULL;

		if(com->m_commandAndMonitor == HK_NULL)
		{
			hkMonitorIdStringMap* com = reinterpret_cast<hkMonitorIdStringMap*>(current);
			addString( com->m_id, current + hkSizeOf(hkMonitorIdStringMap) );
			current = com->m_sizeOfString + (char *)(com + 1);
			continue;
		}
		else
		{
			string = remapString(com->m_commandAndMonitor);
		}
	
		switch(string[0])
		{
		  case 'T': // timer begin
		  case 'E': // timer end
		  case 'L': // timer list begin
		  case 'S': // split list
		  case 'l': // list end
			{
				hkMonitorTimer* serializedCommand = reinterpret_cast<hkMonitorTimer*>( current );
				current = (char*)(serializedCommand + 1);

				// output the raw monitor stream version
				if( hkMonitorStream::memoryAvailable() )
				{
					hkMonitorTimer* rawCommand = reinterpret_cast<hkMonitorTimer*>(hkMonitorStream::current());
					(*rawCommand) = (*serializedCommand);
					rawCommand->m_commandAndMonitor = remapString(serializedCommand->m_commandAndMonitor);
					hkMonitorStream::current() = (char*)(rawCommand + 1);
				}

				break;
			}
		  case 'M':
			{
				hkMonitorAddValue* serializedCommand = reinterpret_cast<hkMonitorAddValue*>( current );
				current = (char*)(serializedCommand + 1);

				// output the raw monitor stream version
				if( hkMonitorStream::memoryAvailable() )
				{
					hkMonitorAddValue* rawCommand = reinterpret_cast<hkMonitorAddValue*>(hkMonitorStream::current());
					(*rawCommand) = (*serializedCommand);
					rawCommand->m_commandAndMonitor = remapString(serializedCommand->m_commandAndMonitor);
					hkMonitorStream::current() = (char*)(rawCommand + 1);
				}

				break;
			}
		  case 'P':
		  case 'p':
			{
				hkMonitorCaptureCommand* serializedCommand = reinterpret_cast<hkMonitorCaptureCommand*>( current );
				current = (char*)(serializedCommand + 1);

				// output the raw monitor stream version
				if( hkMonitorStream::memoryAvailable() )
				{
					hkMonitorCaptureCommand* rawCommand = reinterpret_cast<hkMonitorCaptureCommand*>(hkMonitorStream::current());
					(*rawCommand) = (*serializedCommand);
					rawCommand->m_commandAndMonitor = remapString(serializedCommand->m_commandAndMonitor);
					hkMonitorStream::current() = (char*)(rawCommand + 1);
				}

				break;
			}
		  case 'I':
			{
				hkMonitorStreamParams* serializedCommand = reinterpret_cast<hkMonitorStreamParams*>( current );
				current = (char*)(serializedCommand + 1);
				
				// record a local copy of the information
				m_heading = remapString(serializedCommand->m_heading);
				m_nodeIdForFrameOverview = remapString(serializedCommand->m_nodeIdForFrameOverview);
				m_timerFactor0 = serializedCommand->m_timerFactor0;
				m_timerFactor1 = serializedCommand->m_timerFactor1;

				// output the raw monitor stream version
				if( hkMonitorStream::memoryAvailable() )
				{
					hkMonitorStreamParams* rawCommand = reinterpret_cast<hkMonitorStreamParams*>(hkMonitorStream::current());
					(*rawCommand) = (*serializedCommand);
					rawCommand->m_commandAndMonitor = remapString(serializedCommand->m_commandAndMonitor);
					hkMonitorStream::current() = (char*)(rawCommand + 1);
				}

				break;
			}

		  case 'F':	// new frame
			{
				hkMonitorCaptureCommand* com = reinterpret_cast<hkMonitorCaptureCommand*>(current);
				current = (char*)(com + 1);
				if(current < end)
				{
//					hkMonitorStream::reset(); 
				}
				break;
			}
		  default:
			HK_ASSERT2(0, "Inconsistent Monitor capture data" ); return;
		}

	}
}

void hkMonitorCaptureStreamUtility::captureFrame()
{
	char* current = hkMonitorStream::begin();
	char* end = hkMonitorStream::current();

	// Serialize all (new) strings/ids
	while ( current < end )
	{
		char* next;
		hkMonitorCaptureCommand* com = reinterpret_cast<hkMonitorCaptureCommand*>( current );

			// check for memory overflow
		if ( m_data.getCapacity() - m_data.getSize() < 1024 )
		{
			return;
		}

		switch( com->m_commandAndMonitor[0] )
		{
		case 'S':		// split list
		case 'T':		// timer begin
		case 'E':		// timer end
		case 'l':		// list end
			{
				hkMonitorTimer* com = reinterpret_cast<hkMonitorTimer*>( current );
				createMonitor( com->m_commandAndMonitor );
				next = (char *)(com+1);
				break;
			}
		case 'L':			// timer list begin
			{
				hkMonitorTimer* com = reinterpret_cast<hkMonitorTimer*>( current );
				int strl = hkString::strLen(com->m_commandAndMonitor);
				strl += hkString::strLen(com->m_commandAndMonitor + strl + 1) + 2;
				createMonitor( com->m_commandAndMonitor, strl );
				next = (char *)(com+1);
				break;
			}
		case 'M':
			{
				hkMonitorAddValue* com = reinterpret_cast<hkMonitorAddValue*>( current );
				createMonitor( com->m_commandAndMonitor );
				next = (char *)(com+1);
				break;
			}
		case 'P':
			{
				hkMonitorCaptureCommand* com = reinterpret_cast<hkMonitorCaptureCommand*>( current );
				createMonitor( com->m_commandAndMonitor );
				next = (char *)(com+1);
				break;
			}
		case 'p':
			{
				hkMonitorCaptureCommand* com = reinterpret_cast<hkMonitorCaptureCommand*>( current );
				createMonitor( com->m_commandAndMonitor );
				next = (char *)(com+1);
				break;
			}
		default:
			HK_ASSERT2(0, "Inconsistent Monitor capture data" ); return;
		}
		//int size = next - current;
		//char *data = m_data.expandBy(size);
		//hkString::memCpy( data, current, size );
		current = next;

	}

	// copy data

	// Place heading and overview id information in the stream
	static const char* captureFrame = "Fd";
	createMonitor( captureFrame );	// new frame

	static const char* streamParams = "Ii";
	createMonitor( streamParams );	// send out parameters

	createMonitor( m_heading.cString() );
	createMonitor( m_nodeIdForFrameOverview.cString() );

	// check for overflow
	if ( m_data.getCapacity() - m_data.getSize() < 1024 )
	{
		return;
	}

	// make sure there is enough capacity for the actual capture data (+params +end of frame)
	int size = hkMonitorStream::current() - hkMonitorStream::begin();
	if( m_data.getCapacity() - m_data.getSize() < size + hkSizeOf(hkMonitorCaptureCommand) + hkSizeOf(hkMonitorStreamParams) )
	{
		return;
	}

	// allocate the space
	char *data = m_data.expandBy( size + hkSizeOf(hkMonitorCaptureCommand) + hkSizeOf(hkMonitorStreamParams) );

	// copy the stream parameters
	{
		hkMonitorStreamParams* com = reinterpret_cast<hkMonitorStreamParams*>(data);
		com->m_commandAndMonitor = streamParams;
		com->m_heading = m_heading.cString();
		com->m_nodeIdForFrameOverview = m_nodeIdForFrameOverview.cString();
		com->m_timerFactor0 = m_timerFactor0;
		com->m_timerFactor1 = m_timerFactor1;
	}

	// copy the capture data
	hkString::memCpy( data + hkSizeOf(hkMonitorStreamParams), hkMonitorStream::begin(), size );

	// copy the end of frame
	{
		hkMonitorCaptureCommand* com = reinterpret_cast<hkMonitorCaptureCommand*>(data + size + hkSizeOf(hkMonitorStreamParams));
		com->m_commandAndMonitor = captureFrame;
	}
}

void hkMonitorCaptureStreamUtility::writeRawDataToFile( const char* fileName )
{
	FILE* file = fopen(fileName, "wb" );
	if (!file)
	{
		return;
	}
	fwrite( &m_data[0], 1, (unsigned)m_data.getSize(), file );

	fclose( file );
}

hkResult hkMonitorCaptureStreamUtility::readRawDataFromFile( const char* fileName )
{
	FILE* file = fopen(fileName, "rb" );
	if (!file)
	{
		return HK_FAILURE;
	}
	fseek( file, 0, SEEK_END);
	int size = (int)ftell(file);
	rewind(file);
	m_data.setSize( size );
	fread( &m_data[0], 1, (unsigned)size, file );

	fclose( file );

	return HK_SUCCESS;
}


hkMonitorCaptureStreamUtility::Node* hkMonitorCaptureStreamUtility::newNode( Node* parent, const char* name, hkMonitor::Type type )
{
	Node* node = new Node( parent, name, type );
	return node;
}


static const char* HK_CAPTURE_STREAM_UTILITY_ERROR = "<string not found>";

const char* hkMonitorCaptureStreamUtility::remapString( const char* id )
{
	for(int i = m_mappings.getSize() - 1; i >= 0; i--)
	{
		if(m_mappings[i].m_id == id)
		{
			return m_mappings[i].m_string;
		}
	}

	return HK_CAPTURE_STREAM_UTILITY_ERROR;
}

void hkMonitorCaptureStreamUtility::addString( const char* id, const char* string )
{
	bool found = false;

	// See if the id already exists?
	for(int i = 0; i < m_mappings.getSize(); i++)
	{
		if(m_mappings[i].m_id == id)
		{
			//m_mappings[i].m_string = string;
			found = true;
			break;
		}
	}

	// if it was not found then append it to the list of mappings
	if(!found)
	{
		StringMap map;
		map.m_id = id;
		map.m_string = string;
		m_mappings.pushBack( map );
	}
}

void hkMonitorCaptureStreamUtility::makeStatisticsTree(hkArray<Node*>& m_nodesOut)
{
	char* current = &m_data[0];
	char* end = current + m_data.getSize();

	Node* cwd = new Node(HK_NULL, "/", hkMonitor::TYPE_DIRECTORY);
	m_nodesOut.pushBack(cwd);

	hkInplaceArray<hkMonitorTimer,16> timerStack;

	while(current < end) // for all frames
	{
		hkMonitorCaptureCommand* com = reinterpret_cast<hkMonitorCaptureCommand*>(current);
		const char* string = HK_NULL;

		if(com->m_commandAndMonitor == HK_NULL)
		{
			hkMonitorIdStringMap* com = reinterpret_cast<hkMonitorIdStringMap*>(current);
			addString( com->m_id, current + hkSizeOf(hkMonitorIdStringMap) );
			current = com->m_sizeOfString + (char *)(com + 1);
			continue;
		}
		else
		{
			string = remapString(com->m_commandAndMonitor);
		}

		switch(string[0])
		{
		case 'T': // timer begin
			{
				cwd = newNode( cwd, string + 2, hkMonitor::TYPE_TIMER );
				hkMonitorTimer* com = reinterpret_cast<hkMonitorTimer*>( current );
				timerStack.pushBack( *com );
				current = (char *)(com+1);
				break;
			}
		case 'E': // timer end
			{
				const hkMonitorTimer& start = timerStack[timerStack.getSize() - 1];
				hkMonitorTimer* com = reinterpret_cast<hkMonitorTimer*>( current );
				cwd->m_value = float(com->m_time0 - start.m_time0 );
				cwd->m_value2 = float(com->m_time1 - start.m_time1 );
				cwd = cwd->m_parent;
				timerStack.popBack();
				current = (char *)(com + 1);
				break;
			}
		case 'L': // timer list begin
			{
				hkMonitorTimer* com = reinterpret_cast<hkMonitorTimer*>( current );
				{
					timerStack.pushBack( *com );
					cwd = newNode( cwd, string + 2, hkMonitor::TYPE_TIMER );
				}
				{
					hkMonitorTimer com2 = *com;
					com2.m_commandAndMonitor = string + hkString::strLen( string ) - 1;
					timerStack.pushBack( com2 );
					cwd = newNode( cwd, com2.m_commandAndMonitor+2, hkMonitor::TYPE_TIMER );
				}
				current = (char*)(com + 1);
				break;
			}
		case 'S': // split list
			{
				hkMonitorTimer& start = timerStack[ timerStack.getSize()- 1 ];
				hkMonitorTimer* com = reinterpret_cast<hkMonitorTimer*>( current );
				cwd->m_value = float(com->m_time0 - start.m_time0 );
				cwd->m_value2 = float(com->m_time1 - start.m_time1 );
				cwd = newNode( cwd->m_parent, string + 2, hkMonitor::TYPE_TIMER );
				start = *com;
				current = (char*)(com + 1);
				break;
			}
		case 'l': // list end
			{
				hkMonitorTimer* com = reinterpret_cast<hkMonitorTimer*>( current );
				{
					const hkMonitorTimer& start = timerStack[ timerStack.getSize()- 1 ];
					cwd->m_value = float(com->m_time0 - start.m_time0 );
					cwd->m_value2 = float(com->m_time1 - start.m_time1 );
					cwd = cwd->m_parent;
					timerStack.popBack();
				}
				{
					const hkMonitorTimer& start = timerStack[ timerStack.getSize()- 1 ];
					cwd->m_value = float(com->m_time0 - start.m_time0 );
					cwd->m_value2 = float(com->m_time1 - start.m_time1 );
					cwd = cwd->m_parent;
					timerStack.popBack();
				}
				current = (char*)(com + 1);
				break;
			}
		case 'M':
			{
				hkMonitorAddValue* com = reinterpret_cast<hkMonitorAddValue*>( current );
				Node *node = newNode( cwd, string + 2, hkMonitor::TYPE_SINGLE );
				node->m_value = com->m_value;
				current = (char*)(com + 1);
				break;
			}
		case 'P':
			{
				hkMonitorCaptureCommand* com = reinterpret_cast<hkMonitorCaptureCommand*>( current );
				cwd = newNode( cwd, string + 2, hkMonitor::TYPE_DIRECTORY );
				current = (char*)(com + 1);
				break;
			}
		case 'p':
			{
				hkMonitorCaptureCommand* com = reinterpret_cast<hkMonitorCaptureCommand*>( current );
				cwd = cwd->m_parent;
				current = (char*)(com + 1);
				break;
			}
		case 'I':
			{
				hkMonitorStreamParams* com = reinterpret_cast<hkMonitorStreamParams*>(current);
				current = (char*)(com + 1);
				m_heading = remapString(com->m_heading);
				m_nodeIdForFrameOverview = remapString(com->m_nodeIdForFrameOverview);
				m_timerFactor0 = com->m_timerFactor0;
				m_timerFactor1 = com->m_timerFactor1;
				break;
			}

		case 'F':	// new frame
			{
				hkMonitorCaptureCommand* com = reinterpret_cast<hkMonitorCaptureCommand*>(current);
				current = (char*)(com + 1);
				if(current < end)
				{
					cwd = new Node(HK_NULL, "/", hkMonitor::TYPE_DIRECTORY);
					m_nodesOut.pushBack(cwd);
					timerStack.clear();
				}
				break;
			}
		default:
			HK_ASSERT2(0, "Inconsistent Monitor capture data" ); return;
		}

	}
}



static void HK_CALL hkWriteRec( hkOstream& outstream, hkMonitorCaptureStreamUtility::Node* node, int RecDepth, float m_timerFactor0, float m_timerFactor1 )
{
	if( RecDepth )
	{
		for( int j = 1; j < RecDepth; j++ )
		{
			outstream.printf("\t" );
		}
		double val  = node->m_value;
		double val2 = node->m_value2;

		if( node->m_type == hkMonitor::TYPE_TIMER )
		{
			val *= m_timerFactor0;
			val2 *= m_timerFactor1;
		}

		if( val2 )
		{
			outstream.printf("%-12s %4.3f (%4.3f)\n" , node->m_name, val, val2 );
		}
		else
		{
			outstream.printf("%-12s %4.3f\n" , node->m_name, val );
		}
	}

	for ( int i = 0; i < node->m_children.getSize(); i++ )
	{
		hkWriteRec( outstream, node->m_children[i], RecDepth + 1, m_timerFactor0, m_timerFactor1 );
	}
}

static hkMonitorCaptureStreamUtility::Node* HK_CALL hkFindChildByName( hkMonitorCaptureStreamUtility::Node* parent, const char* childName, bool searchAnyChild )
{

	// search child
	for( int j = 0; j < parent->m_children.getSize(); j++ )
	{
		if( hkString::strCmp( childName, parent->m_children[j]->m_name ) == 0 )
		{
			return parent->m_children[j];
		}
	}
	if ( searchAnyChild && parent->m_children.getSize()>0 )
	{
		return parent->m_children[0];
	}
	return HK_NULL;
}


static void HK_CALL hkMakeSum( hkMonitorCaptureStreamUtility::Node* sum, hkMonitorCaptureStreamUtility::Node* tree )
{
	for( int i = 0; i < tree->m_children.getSize(); i++ )
	{
		hkMonitorCaptureStreamUtility::Node* childIn = tree->m_children[i];
		hkMonitorCaptureStreamUtility::Node* childOut = hkFindChildByName( sum, childIn->m_name, false);

		if( !childOut )
		{
			childOut = new hkMonitorCaptureStreamUtility::Node( sum,childIn->m_name, childIn->m_type );
		}
		childOut->m_value  += childIn->m_value;
		childOut->m_value2 += childIn->m_value2;

		hkMakeSum( childOut, childIn );
	}
}

	/// produces several output lists:
	///		- a total hierarchical avarage
	///		- a per frame single node
	///		- a summary per frame
	///		- the detailed view
void hkMonitorCaptureStreamUtility::writeStatistics( hkOstream& outstream, int reportLevel )
{

	hkArray<Node*> nodes;
	makeStatisticsTree(nodes);

		// the average value
	hkReal avgValue = 1000.0f;

	//
	//	summarize everything
	//
	if( reportLevel & REPORT_TOTAL )
	{
		Node sum(0 , "Sum", hkMonitor::TYPE_DIRECTORY); // sum over all frames
		for ( int i = 0; i < nodes.getSize();i++ )
		{
			hkMakeSum( &sum, nodes[i] );
		}
		outstream.printf("\n\n");
		outstream.printf("*********************************\n" );
		outstream.printf("********** Total Times    *******\n" );
		outstream.printf("*********************************\n" );
		outstream.printf("%s\n", m_heading.cString() );
		float f = 1.0f / nodes.getSize();
		hkWriteRec( outstream, &sum, 0, m_timerFactor0 * f, m_timerFactor1 * f);

		Node* node = hkFindChildByName( &sum, m_nodeIdForFrameOverview.cString(), true );
		if (node)
		{
			avgValue = (node->m_value * m_timerFactor0 + node->m_value2 * m_timerFactor1) * f;
		}
	}

	//
	// print each frame in a single line
	//
	if( reportLevel & REPORT_PERFRAME_TIME )
	{
		outstream.printf("\n\n");
		outstream.printf("*********************************\n" );
		outstream.printf("********** Per Frame Time *******\n" );
		outstream.printf("*********************************\n" );
		outstream.printf("%s\n", m_heading.cString() );

		const int GRAPH_SIZE = 40;
		char buffer[GRAPH_SIZE+10];
		hkString::memSet( buffer, ' ', GRAPH_SIZE );
		buffer[GRAPH_SIZE] = 0;

		for ( int i = 0; i < nodes.getSize();i++ )
		{
			Node* frameRoot = nodes[i];
			if ( frameRoot->m_children.getSize() == 0 )
			{
				continue;
			}
			Node* node = hkFindChildByName( frameRoot, m_nodeIdForFrameOverview.cString(), true );

			//
			// draw graph
			//
				{
				hkReal relVal = 0.5f * GRAPH_SIZE * (node->m_value * m_timerFactor0 + node->m_value2 * m_timerFactor1) / avgValue;
				int index = int(relVal);
				if ( index < 0) index = 0;
				if (index >=GRAPH_SIZE ) index = GRAPH_SIZE-1;
				char *p = buffer;
				int i = 0;
				for (; i +4 < index; i+=4){ *(p++) = '\t'; }
				int i2 = i;
				for (; i2    < index; i2+=1){ *(p++) = ' '; }
				*(p++) = '#';
				i2++;
				i += 4;
				for (; i2    < i; i2+=1){ *(p++) = ' '; }
				for (; i < GRAPH_SIZE; i+=4){ *(p++) = '\t'; }
				*(p) = 0;
				outstream.printf(buffer);
				}

			float val =  node->m_value  * m_timerFactor0;
			float val2 = node->m_value2 * m_timerFactor1;
			outstream.printf("%i %-12s %f (%f)\n" , i, node->m_name, val, val2 );
		}
	}

	//
	//	summary per frame
	//
	if( reportLevel & REPORT_PERFRAME_SUMMARY )
	{
		for ( int i = 0; i < nodes.getSize();i++ )
		{
			Node sum(0 , "Sum", hkMonitor::TYPE_DIRECTORY); // sum over all frames
			hkMakeSum( &sum, nodes[i] );
			outstream.printf("\n");
			outstream.printf("*******************************\n" );
			outstream.printf("****** Summary Frame-%i   *****\n",i );
			outstream.printf("*******************************\n" );
			outstream.printf("%s\n", m_heading.cString() );
			float f = 1.0f;
			hkWriteRec( outstream, &sum, 0, m_timerFactor0 * f, m_timerFactor1 * f);
		}
	}


	//
	//	detailed view
	//
	if( reportLevel & REPORT_PERFRAME_DETAIL )
	{
		for ( int i = 0; i < nodes.getSize();i++ )
		{
			outstream.printf("\n\n");
			outstream.printf("********************************\n" );
			outstream.printf("***** Details Frame:%i    ******\n", i );
			outstream.printf("********************************\n" );
			outstream.printf("%s\n", m_heading.cString() );
			hkWriteRec( outstream, nodes[i], 0, m_timerFactor0, m_timerFactor1);
		}
	}

	//
	//	Free memory
	//
	{
		for ( int i = 0; i < nodes.getSize();i++ )
		{
			delete nodes[i];
		}
	}
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
