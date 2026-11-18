/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>
#include <hkbase/stopwatch/hkStopwatch.h>
#include <hkbase/debugutil/hkMonitorBank.h>
#include <hkbase/debugutil/hkMonitorStream.h>
#include <hkbase/debugutil/hkDefaultMonitorFactory.h>


hkMonitorBank::DmbNode::DmbNode()
	:	m_monitor(HK_NULL),
		m_monitorVar(HK_NULL),
		m_firstChild(HK_NULL),
		m_next(HK_NULL),
		m_parent(HK_NULL),
		m_userFlags(0)
{
}

hkMonitorBank::~hkMonitorBank()
{
	HK_ASSERT(m_rootNode.m_next == HK_NULL);
	
	// avoid a recursive delete of nodes: use a lifo to store
	// nodes to recurse on and iterate
	hkInplaceArray<DmbNode*, 128> nodes;
	nodes.pushBack(m_rootNode.m_firstChild);
	while(nodes.getSize() != 0)
	{
		DmbNode* cur = nodes[ nodes.getSize() - 1 ];
		nodes.popBack();
		while(cur)
		{
			if(cur->m_monitor)
			{
				delete cur->m_monitor;
			}
			for(int i = 0; i < cur->m_monitorVar.getSize(); ++i)
			{
				*(cur->m_monitorVar[i]) = HK_NULL;
			}
			if(cur->m_firstChild != HK_NULL)
			{
				nodes.pushBack(cur->m_firstChild);
			}
			DmbNode* oldcur = cur;
			cur = cur->m_next;
			delete oldcur;
		}
	}
	delete m_monitorFactory;
}
/// Note that if a monitor already exists at the location in the monitor tree specified by the name, no new 
/// monitor instance is created.
hkMonitor* hkMonitorBank::createMonitor( const char* name, hkMonitor** varPntr, hkMonitor::Type monType)
{
	// check for paths in the name of the monitor
	// we may need to chdir before creating
	const char* parentdir = hkString::strRchr(name, '/');
	if ( parentdir )
	{
		hkInplaceArray<char, 128> par;
		if ( parentdir == name )
		{
			par.pushBack('/');
			++name;
		}
		else
		{
			par = hkArray<char>(const_cast<char*>(name), parentdir - name, parentdir - name);
			name += parentdir - name + 1;
		}
		par.pushBack(HK_NULL);

		pushDir( &par[0] );
	}
	
	DmbNode* node = HK_NULL;

	// try to find the monitor first
	{
		if ( getCurrentDirectory()->m_firstChild )
		{
			DmbNode* s = getCurrentDirectory()->m_firstChild;
			while ( s )
			{
				if ( s->m_monitor && hkString::strCmp( s->m_monitor->getName(), name) == 0 )
				{
					// ok, we found it
					if(varPntr)
					{
						s->m_monitorVar.pushBack(varPntr);
					}
					node = s;
					break;
				}
				s = s->m_next;
			}
		}
	}

	// no? create one
	if(node == HK_NULL)
	{
		node = createNode(getCurrentDirectory(), name, varPntr, monType);
	}

	if ( parentdir )
	{
		popDir();
	}
	return node->m_monitor;
}

void hkMonitorBank::pushDir(const char* path)
{
	HK_ASSERT2(path!=HK_NULL, "pushDir called with null path");

	const char* rest = path;
	DmbNode* searchDir = HK_NULL;

	// check for leading /
	if ( rest[0] == '/' )
	{
		searchDir = &m_rootNode;
		rest++;
	}
	else
	{
		searchDir = getCurrentDirectory();
	}

	while ( rest != HK_NULL && rest[0] != HK_NULL )
	{
		// check for ../ and ./
		if ( rest[0] == '.' )
		{
			if ( rest[1] == '.' )
			{
				HK_ASSERT2( searchDir->m_parent, "Already at root level, cannot go up: '" << path << "'" );
				searchDir = searchDir->m_parent;
				if ( rest[2] == 0 )
				{
					break;
				}
				HK_ASSERT2( rest[2] == '/', "Chdir: '..' is not followed by a '/' character: '" << path << "'" );
				rest += 3;
				continue;
			}

			HK_ASSERT2( rest[1] == '/', "Chdir: '.' is not followed by a '/' character: '" << path << "'" );
			rest += 2;
			continue;
		}

		char buffer[1024];
		const char* ns = hkString::strChr( rest, '/');
		if ( !ns )
		{
			hkString::strCpy( buffer, rest );
			rest = HK_NULL;
		}
		else
		{
			hkString::strNcpy( buffer, rest, ns - rest );
			buffer[ ns - rest ] = 0;
			rest = ns + 1;
		}

		HK_ASSERT2( buffer[0] != 0, "Chdir: Empty directory found (maybe '//' in the path: '" << path << "'" );

		// find the entry
		if ( searchDir->m_firstChild )
		{
			DmbNode* s = searchDir->m_firstChild;
			while ( s )
			{
				if ( s->m_monitor && hkString::strCmp( s->m_monitor->getName(), buffer) == 0 )
				{
					searchDir = s;
					break;
				}
				s = s->m_next;
			}
			if ( s )
			{
				continue;
			}
		}
		
		// not found, create
		searchDir = createNode( searchDir, buffer, HK_NULL, hkMonitor::TYPE_DIRECTORY);
	}

	m_directoryStack.pushBack(searchDir);
}

void hkMonitorBank::popDir()
{
	m_directoryStack.popBack();
}



void hkMonitorBank::rebuildAllMonitors(DmbNode *rootNode)
{
	if ( !rootNode)
	{
		rootNode = &m_rootNode;
	}

	if ( rootNode->m_monitorVar.getSize() )
	{
		hkMonitor* newmon = this->m_monitorFactory->createMonitor( rootNode->m_monitor->getName(), rootNode->m_type );
		delete rootNode->m_monitor;
		rootNode->m_monitor = newmon;
		for (int i = 0; i < rootNode->m_monitorVar.getSize(); ++i)
		{
			rootNode->m_monitorVar[i][0] = newmon;
		}
	}
	// do the recursion
	DmbNode *childNode = rootNode->m_firstChild;
	while ( childNode )
	{
		rebuildAllMonitors( childNode );
		childNode = childNode->m_next;
	}
}

/// call reset on all monitors
void hkMonitorBank::resetAllMonitors(DmbNode *rootNode)
{
	if ( !rootNode)
	{
		rootNode = &m_rootNode;
	}

	if ( rootNode->m_monitor )
	{
		rootNode->m_monitor->resetValue();
	}

	// do the recursion
	DmbNode *childNode = rootNode->m_firstChild;
	while ( childNode )
	{
		resetAllMonitors( childNode );
		childNode = childNode->m_next;
	}
}

/// call newFrame on all monitors
void hkMonitorBank::beginNewFrameAllMonitors(DmbNode *rootNode)
{
	if ( !rootNode)
	{
		if (m_newFrameDisabled)
		{
			return;
		}
		rootNode = &m_rootNode;
	}

	if ( rootNode->m_monitor )
	{
		rootNode->m_monitor->beginNewFrame();
	}

	// do the recursion
	DmbNode *childNode = rootNode->m_firstChild;
	while ( childNode )
	{
		beginNewFrameAllMonitors( childNode );
		childNode = childNode->m_next;
	}
}

void hkMonitorBank::printAllMonitors( class hkOstream& ostream, DmbNode *rootNode, int recursionDepth, int depthToPrint  )
{
	if ( !rootNode)
	{
		rootNode = &m_rootNode;
	}

	if ( rootNode->m_monitor )
	{
		for (int i=0; i < recursionDepth; i++)
		{
			ostream << "\t";
		}
		rootNode->m_monitor->print(ostream);
		ostream << "\n";
	}

	if ( !depthToPrint )
	{
		return;
	}
	// do the recursion
	DmbNode *childNode = rootNode->m_firstChild;
	while ( childNode )
	{
		printAllMonitors( ostream, childNode, recursionDepth+1, depthToPrint-1 );
		childNode = childNode->m_next;
	}
}

hkMonitorBank::DmbNode* hkMonitorBank::findNode( const char* path )
{
	pushDir(path);
	DmbNode* n = getCurrentDirectory();
	popDir();
	return n;
}

void hkMonitorBank::setNewFrameDisabled(bool b)
{
	m_newFrameDisabled = b;
}

void hkMonitorBank::setMonitorFactory( hkMonitorFactory* newFactory)
{
	delete m_monitorFactory;

	if (newFactory == HK_NULL)
	{
		m_monitorFactory = new hkDefaultMonitorFactory();
	}
	else
	{
		m_monitorFactory = newFactory;
	}
}

hkMonitorFactory* hkMonitorBank::getMonitorFactory( )
{
	return m_monitorFactory;
}

hkMonitorBank::DmbNode*	hkMonitorBank::getRootNode()
{
	return &m_rootNode;
}

hkMonitorBank::DmbNode* hkMonitorBank::getCurrentDirectory()
{
	HK_ASSERT2(m_directoryStack.getSize()>=1, "no current directory");
	return m_directoryStack[ m_directoryStack.getSize() - 1 ];
}

hkMonitorBank::hkMonitorBank()
{
	m_directoryStack.pushBack(&m_rootNode);
	m_monitorFactory = new hkDefaultMonitorFactory();
	m_newFrameDisabled = false;
}

hkMonitorBank::DmbNode* hkMonitorBank::createNode(DmbNode* parent, const char* name, hkMonitor** varPntr, hkMonitor::Type monType)
{
	HK_ASSERT( hkString::strChr(name, '/') == HK_NULL );

	DmbNode* node = new DmbNode();

	node->m_monitor = m_monitorFactory->createMonitor( name, monType);
	node->m_type = monType;
	node->m_parent = parent;
	if(varPntr)
	{
		node->m_monitorVar.pushBack(varPntr);
	}

	/// if there is a currentDir, use it
	if ( parent->m_firstChild == HK_NULL )
	{
		parent->m_firstChild = node;
	}
	else
	{
		DmbNode* lastNode = parent->m_firstChild;

		while ( lastNode->m_next != HK_NULL)
		{
			lastNode = lastNode->m_next;
		}
		lastNode->m_next = node;
	}

	return node;
}

void hkMonitorBank::examineMonitorCapture( int timerId, hkReal ticksDeltaTime)
{
	char *current = hkMonitorStream::begin();
	char *end = hkMonitorStream::current();
	hkInplaceArray<hkMonitorTimer,16> timerStack;

	int stackDepth = m_directoryStack.getSize();
	
	while ( current < end )
	{
		hkMonitorCaptureCommand* com = reinterpret_cast<hkMonitorCaptureCommand*>( current );

		hkMonitor::Type type;
		switch ( com->m_commandAndMonitor[1] )
		{
			case 'i': type = hkMonitor::TYPE_INT; break;
			case 'f': type = hkMonitor::TYPE_SINGLE; break;
			case 't': type = hkMonitor::TYPE_TIMER; break;
			case 'd': type = hkMonitor::TYPE_DIRECTORY; break;
			default: HK_ASSERT2(0, "Inconsistent Monitor capture data" ); return;
		}


		switch( com->m_commandAndMonitor[0] )
		{
		case 'T':			// timer begin
			{
				hkMonitor* mon = this->createMonitor( com->m_commandAndMonitor + 2, HK_NULL, hkMonitor::TYPE_TIMER );
				hkMonitorTimer* com = reinterpret_cast<hkMonitorTimer*>( current );
				timerStack.pushBack( *com );
				current = (char *)(com+1);
				this->pushDir( com->m_commandAndMonitor + 2 );
				break;
			}
		case 'E':		// timer end
			{
				this->popDir();
				const hkMonitorTimer& start = timerStack[ timerStack.getSize()- 1 ];
				hkMonitor* mon = this->createMonitor( start.m_commandAndMonitor + 2, HK_NULL, hkMonitor::TYPE_TIMER );
				hkMonitorTimer* com = reinterpret_cast<hkMonitorTimer*>( current );
				float deltaTime =  ( timerId == 0 ) ? ( (com->m_time0 - start.m_time0) * ticksDeltaTime ) : ( (com->m_time1 - start.m_time1) * ticksDeltaTime );
				mon->addValue( deltaTime );
				timerStack.popBack(  );
				current = (char *)(com+1);
				break;
			}
		case 'L':			// timer list begin
			{
				hkMonitorTimer* com = reinterpret_cast<hkMonitorTimer*>( current );
				{
					this->createMonitor( com->m_commandAndMonitor + 2, HK_NULL, hkMonitor::TYPE_TIMER );
					timerStack.pushBack( *com );
					this->pushDir( com->m_commandAndMonitor + 2 );
				}
				{
					hkMonitorTimer com2 = *com;
					com2.m_commandAndMonitor = com->m_commandAndMonitor + hkString::strLen( com->m_commandAndMonitor ) - 1;
					timerStack.pushBack( com2 );
					this->createMonitor( com2.m_commandAndMonitor+2, HK_NULL, hkMonitor::TYPE_TIMER );
					this->pushDir( com2.m_commandAndMonitor+2 );
				}
				current = (char *)(com+1);
				break;
			}
		case 'S':		// split list
			{
				this->popDir();
				hkMonitorTimer& start = timerStack[ timerStack.getSize()- 1 ];
				hkMonitor* mon = this->createMonitor( start.m_commandAndMonitor + 2, HK_NULL, hkMonitor::TYPE_TIMER );
				hkMonitorTimer* com = reinterpret_cast<hkMonitorTimer*>( current );
				float deltaTime =  ( timerId == 0 ) ? ( (com->m_time0 - start.m_time0) * ticksDeltaTime ) : ( (com->m_time1 - start.m_time1) * ticksDeltaTime );
				mon->addValue( deltaTime );

				mon = this->createMonitor( com->m_commandAndMonitor + 2, HK_NULL, hkMonitor::TYPE_TIMER );
				start = *com;
				current = (char *)(com+1);
				this->pushDir( com->m_commandAndMonitor + 2 );
				break;
			}
		case 'l':		// list end
			{
				hkMonitorTimer* com = reinterpret_cast<hkMonitorTimer*>( current );
				{
					this->popDir();
					const hkMonitorTimer& start = timerStack[ timerStack.getSize()- 1 ];
					hkMonitor* mon = this->createMonitor( start.m_commandAndMonitor + 2, HK_NULL, hkMonitor::TYPE_TIMER );
					float deltaTime =  ( timerId == 0 ) ? ( (com->m_time0 - start.m_time0) * ticksDeltaTime ) : ( (com->m_time1 - start.m_time1) * ticksDeltaTime );
					mon->addValue( deltaTime );
					timerStack.popBack(  );
				}
				{
					this->popDir();
					const hkMonitorTimer& start = timerStack[ timerStack.getSize()- 1 ];
					hkMonitor* mon = this->createMonitor( start.m_commandAndMonitor + 2, HK_NULL, hkMonitor::TYPE_TIMER );
					float deltaTime =  ( timerId == 0 ) ? ( (com->m_time0 - start.m_time0) * ticksDeltaTime ) : ( (com->m_time1 - start.m_time1) * ticksDeltaTime );
					mon->addValue( deltaTime  );
					timerStack.popBack(  );
				}
				current = (char *)(com+1);
				break;
			}
		case 'M':
			{
				hkMonitor* mon = this->createMonitor( com->m_commandAndMonitor + 2, HK_NULL, type );
				hkMonitorAddValue* com = reinterpret_cast<hkMonitorAddValue*>( current );
				mon->addValue( com->m_value );
				current = (char *)(com+1);
				break;
			}
		case 'P':
			{
				hkMonitorCaptureCommand* com = reinterpret_cast<hkMonitorCaptureCommand*>( current );
				this->pushDir( com->m_commandAndMonitor + 2 );
				current = (char *)(com+1);
				break;
			}
		case 'p':
			{
				hkMonitorCaptureCommand* com = reinterpret_cast<hkMonitorCaptureCommand*>( current );
				this->popDir(  );
				current = (char *)(com+1);
				break;
			}
		case 'I':
			{
				hkMonitorStreamParams* com = reinterpret_cast<hkMonitorStreamParams*>( current );
				current = (char*)(com + 1);
				
				// record a local copy of the information
				{
					// currently not cannot remap strings with hkmonitorbank
					// m_heading = remapString(serializedCommand->m_heading);
					// m_nodeIdForFrameOverview = remapString(serializedCommand->m_nodeIdForFrameOverview);

					// get tick scaling information
					switch(timerId)
					{
					  case 0:
						ticksDeltaTime = com->m_timerFactor0;
						break;
					  case 1:
						ticksDeltaTime = com->m_timerFactor1;
						break;
					  default:
						HK_ASSERT2(0, "Invalid timerId must be 0 or 1!");
					}
				}
				break;
			}

		default:
			HK_ASSERT2(0, "Inconsistent Monitor capture data" ); return;
		}

	}

	m_directoryStack.setSize(stackDepth);

}

#if defined(HK_COMPILER_MWERKS)
#	pragma force_active on
#endif

HK_SINGLETON_IMPLEMENTATION(hkMonitorBank);

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
