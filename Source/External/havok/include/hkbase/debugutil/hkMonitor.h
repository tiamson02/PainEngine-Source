/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKMONITOR_H
#define HKBASE_HKMONITOR_H


/// Base class for all monitor types.
class hkMonitor : public hkBaseObject
{
	public:
		///A monitor type.
		enum Type
		{
			/// Just a structure placeholder
			TYPE_DIRECTORY,
			/// The sum of its input elements
			TYPE_SINGLE,
			///The sum of its input as an integer
			TYPE_INT,
			/// The last, the avg, min and max per frame
			TYPE_RANGE,	
			///The last, the avg, min and max till next reset
			TYPE_PERSISTENT_RANGE,
			/// Smooths the last 60 frames and displays min, avg and max per frame
			TYPE_SMOOTH,
			/// Same as smooth, but also displays the % CPU per 60Hz
			TYPE_TIMER,		

			TYPE_LASTTYPE
		};

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MONITOR)

			/// Specifies a value to be monitored (which might be associated with a given object)
		virtual void addValue( hkReal value, const void* object = HK_NULL ) = 0;

			/// Specifies an (int) value to be monitored (which might be associated with a given object)
		virtual void addValue( int value, const void* object = HK_NULL );

			/// Specifies a value to monitor that is associated with a number of objects.
		virtual void addValue( float value, const hkArray<const void*>& involvedObjects) = 0;

			/// Gets the current value.
		virtual float getValue() = 0;

			/// Resets the current value.
		virtual void resetValue() = 0;


			/// Tells the monitor a new frame has begun.
		virtual void beginNewFrame() = 0;

			/// Gets the monitor's string name.
		inline const char* getName() const;

			/// Gets the monitor's type.
		virtual const Type getType() const = 0;

			/// Prints the current value to the specified output stream.
		virtual void print( class hkOstream& ostream ) = 0;

	protected:

		hkMonitor( const char* name);

		hkInplaceArray<char,16> m_name;
};

inline const char* hkMonitor::getName() const
{
	return &m_name[0];
}





/// Base class for factories that create hkMonitors.
class hkMonitorFactory : public hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MONITOR)

			/// The function that really creates a monitor.
		virtual hkMonitor* createMonitor( const char* name, hkMonitor::Type hint ) = 0;
};

#endif // HKBASE_HKMONITOR_H

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
