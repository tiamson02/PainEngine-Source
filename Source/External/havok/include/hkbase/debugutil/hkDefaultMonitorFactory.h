/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKDEFAULTMONITORFACTORY_H
#define HKBASE_HKDEFAULTMONITORFACTORY_H

#include <hkbase/debugutil/hkMonitor.h>

/// The default monitor factory, used by the hkMonitorBank.
class hkDefaultMonitorFactory : public hkMonitorFactory
{
	public:
		
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MONITOR)

		hkDefaultMonitorFactory();

		//
		//	hkSingleMonitor
		//
		/// Reports the sum of its input elements.
		class hkSingleMonitor : public hkMonitor
		{
			public:

				hkSingleMonitor( const char* name, hkDefaultMonitorFactory *factory );
				const Type getType() const { return hkMonitor::TYPE_SINGLE; }

				void addValue( float value, const void* object );
				void addValue( float value, const hkArray<const void*>& involvedObjects);
				float getValue();

				void resetValue();
				void beginNewFrame();

				void print( class hkOstream& ostream );

					/// The sum of all values.
				float	m_sumValue;

					/// The number of calls to this value.
				int		m_addValueCount;

					/// Backlink to the monitor's factory.
				hkDefaultMonitorFactory *m_myFactory;
		};

		//
		//	hkIntMonitor
		//

		///Reports the sum of its input as an integer, printed as an integer
		class hkIntMonitor : public hkSingleMonitor
		{
			public:

				hkIntMonitor( const char* name, hkDefaultMonitorFactory *factory );
				const Type getType() const { return hkMonitor::TYPE_INT; }
				void print( class hkOstream& ostream );
		};


		//
		//	hkNoneMonitor
		//

		///This monitor does nothing, and can be used to store directories in the tree structure.
		class hkNoneMonitor : public hkSingleMonitor
		{
			public:

				hkNoneMonitor( const char* name, hkDefaultMonitorFactory *factory );
				const Type getType() const { return hkMonitor::TYPE_DIRECTORY; }
				void print( class hkOstream& ostream );
		};


		//
		//	hkRangeMonitor
		//

		///This monitor finds the last, the avg, min, and max per frame of the value to be monitored.
		class hkRangeMonitor : public hkMonitor
		{
			public:

				hkRangeMonitor( const char* name, hkDefaultMonitorFactory *factory );
				const Type getType() const { return hkMonitor::TYPE_RANGE; }

				void addValue( float value, const void* object );
				void addValue( float value, const hkArray<const void*>& involvedObjects);
				float getValue();

				void print( class hkOstream& ostream );
				void resetValue();
				void beginNewFrame();

			public:

					/// The last value.
				float   m_lastValue;

					/// The sum of all values.
				float	m_sumValue;

					/// The max of all values.
				float   m_maxValue;

					/// The min of all values.
				float   m_minValue;

				hkArray<const void*>	 m_minValueObjects;
				hkArray<const void*>	 m_maxValueObjects;

					/// The number of calls to this value.
				int		m_addValueCount;

					/// Backlink to the monitor's factory.
				hkDefaultMonitorFactory *m_myFactory;
		};




		//
		//	hkPersistentRangeMonitor
		//

		/// Finds the last, the avg, min, and max till next reset
		class hkPersistentRangeMonitor : public hkRangeMonitor
		{
			public:

				hkPersistentRangeMonitor( const char* name, hkDefaultMonitorFactory *factory );
				const Type getType() const { return hkMonitor::TYPE_PERSISTENT_RANGE; }
				void beginNewFrame();
				void resetValue();
		};



		//
		//	hkSmoothMonitor
		//

		/// This monitor smoothes the last 60 frames and displays min, avg, and max per frame
		class hkSmoothMonitor : public hkSingleMonitor
		{
			public:

				enum { HK_FRAMES_TO_SMOOTH = 60 };

				hkSmoothMonitor( const char* name, hkDefaultMonitorFactory *factory );
				const Type getType() const { return hkMonitor::TYPE_SMOOTH; }

				void print( class hkOstream& ostream );
				void resetValue();
				void beginNewFrame();

			public:
				//
				// the history
				//
				int m_historyCurrentFrame;
				int m_historyFirstValidFrame;
				float m_historySums[ HK_FRAMES_TO_SMOOTH ];
		};


		//
		//	hkTimerMonitor
		//

		/// Like the hkSmoothMonitor, but also displays the % CPU per 60Hz
		class hkTimerMonitor : public hkSmoothMonitor
		{
			public:

				hkTimerMonitor( const char* name, hkDefaultMonitorFactory *factory );
				const Type getType() const { return hkMonitor::TYPE_TIMER; }

				void print( class hkOstream& ostream );
		};


	public:

		virtual hkMonitor* createMonitor( const char* name, hkMonitor::Type hint );

			// if interested in only a few objects you may call the next function
		void setObjectsToCheckFor( hkArray<const void*> objects );

	public:

		enum CheckObjectResult { HK_OBJECT_NOT_FOUND = false, HK_OBJECT_FOUND = true};
		CheckObjectResult findObject( const void* object );
		CheckObjectResult findAnyObject( const hkArray<const void*>& involvedObjects );

	protected:

			// a list of all objects to check for
		hkArray<const void*> m_objects;
	
};

#endif // HKBASE_HKDEFAULTMONITORFACTORY_H

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
