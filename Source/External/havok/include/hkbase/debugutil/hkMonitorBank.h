/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKMONITORBANK_H
#define HKBASE_HKMONITORBANK_H

#include <hkbase/config/hkConfigMonitors.h>
#include <hkbase/debugutil/hkMonitor.h>
#include <hkbase/debugutil/hkMonitorBank.h>

///An hkMonitorBank manages all your current hkMonitor instances, using a tree structure. 
/// The hkMonitorBank class provides you with a number of useful macros for creating and working with monitors. 
/// hkMonitorBank also has functions for navigating around the monitor tree structure and for printing out monitor values.
/// See the hkBase user guide for more information on how to use these macros and functions.
class hkMonitorBank: public hkSingleton<hkMonitorBank>
{
	public:

			/// A node in the hkMonitor tree.
		struct DmbNode
		{

			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MONITOR, DmbNode)

			DmbNode();

				/// The monitor.
			hkMonitor* m_monitor;

				/// A pointer to a static var pointing to the monitor
			hkInplaceArray<hkMonitor**,1> m_monitorVar;

				/// The first child of this node.
			DmbNode* m_firstChild;

				/// The next node in the same directory.
			DmbNode* m_next;

				/// The parent node.
			DmbNode* m_parent;

				/// User flags, e.g. can be used to control the visibility.
			int m_userFlags;	

				/// The monitor's type.
			hkMonitor::Type m_type;
		};

	public:

		hkMonitorBank();

			/// Creates a new monitor with the specified name, monitor reference, and monitor type.
			/// You should not need to use this directly - use the HK_MONITOR_ADD_VALUE macro instead.	
		hkMonitor* createMonitor( const char* name, hkMonitor** varPntr, hkMonitor::Type type);

			/// Changes the current directory (relative or absolute).
			/// You should not need to use this directly - use the HK_MONITOR_PUSH_DIR macro instead.
		void pushDir(const char*);
			/// Changes back to the previous directory.
			/// You should not need to use this directly - use the HK_MONITOR_POP_DIR macro instead.
		void popDir();

			/// Calls to beginNewFrameAllMonitors will be ignored.
		void setNewFrameDisabled(bool b);

			/// Rebuilds all known monitors.
		void rebuildAllMonitors(DmbNode* rootNode = HK_NULL);

			/// Calls reset on all monitors.
		void resetAllMonitors(DmbNode* rootNode = HK_NULL);

			/// Calls beginNewFrame on all monitors and resyncs all timers.
		void beginNewFrameAllMonitors(DmbNode* rootNode = HK_NULL);

			/// A basic print function. Printing starts from the specified rootNode in the monitor tree, indents using the recurscion depth and steps in recursion 'depthToPrint'
		void printAllMonitors( class hkOstream& ostream, DmbNode *rootNode = HK_NULL, int recursionDepth = 0, int depthToPrint = -1 );

			/// Get the data from hkMonitorStream, timerId selects the timer and can be 0 or 1 (timer 1 is currently only usefull on PS2)
		void examineMonitorCapture( int timerId, hkReal ticksDeltaTime );

			/// Changes the monitor factory. To also rebuild all monitors, call rebuildAllMonitors.
		void setMonitorFactory( hkMonitorFactory* newFactory );
			
			/// Gets the current monitor factory.
		hkMonitorFactory* getMonitorFactory( );

			/// Gets the monitor tree root node.
		DmbNode* getRootNode();

			/// Finds a node in the monitor tree using the specified path.
		DmbNode* findNode( const char* path );


	protected:

		friend class hkSingleton<hkMonitorBank>;
		~hkMonitorBank();

		DmbNode* getCurrentDirectory();
		DmbNode* createNode(DmbNode* parent, const char* name, hkMonitor** varPntr, hkMonitor::Type monType);

		DmbNode				m_rootNode;
		hkArray<DmbNode*>	m_directoryStack;
		bool				m_newFrameDisabled;

	private:
		hkMonitorFactory*	m_monitorFactory;
};


#endif // HKBASE_HKMONITORBANK_H

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
