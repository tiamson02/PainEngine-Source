/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKMONITOR_STATISTICS_COLLECTOR_H
#define HKBASE_HKMONITOR_STATISTICS_COLLECTOR_H

class hkStatisticsCollector
{
	public:

		virtual ~hkStatisticsCollector() { }

		enum StatisticClass
		{
			MEMORY_ALL = 0xf,
				// All the static objects allocated by the user outside of the engine which can be simultaneously 
				// shared between hkWorlds, e.g. hkShapes, mopp, hkConstraintData
			MEMORY_SHARED = 1,
				// All the static objects allocated by the user outside of the engine, except MEMORY_SHARED objects
				// e.g. hkConstraintsInstance, hkEntities, hkActions
			MEMORY_INSTANCE = 2,
				// Extra (fixed size) overhead added by the engine for every static object,e.g hkConstraintRuntime.
			MEMORY_ENGINE = 4,
				// Everything allocated by the engine, and what the user has no control over: 
				// e.g. agents, arrays, pointerLists
			MEMORY_RUNTIME = 8,
				// memory used in demos
			MEMORY_APPLICATION = 16
		};

			/// Should be put at the start of each object. If a name is given, it overrides the name the parent gave this object
		virtual void beginObject( const char* name, StatisticClass statisticClass, const hkReferencedObject* object ) = 0;

		virtual void addChunk( const char* name, StatisticClass statisticClass, const void* chunkAddress, int usedSize, int allocatedSize = 0) = 0;

		virtual void addChildObject( const char* name, StatisticClass statisticClass, const hkReferencedObject* object ) = 0;

		virtual void pushDir( const char* dirName ) = 0;

		virtual void popDir() = 0;

		// for convinience
		template<typename T>
		HK_FORCE_INLINE void addArray( const char* name, StatisticClass statisticClass, const hkArray<T>& array )
		{
			if ( !(array.m_capacityAndFlags & array.DONT_DEALLOCATE_FLAG) )
			{
				addChunk( name, statisticClass, array.begin(), array.getSize() * hkSizeOf(T), array.getCapacity() * hkSizeOf(T) );
			}
		}

		virtual void endObject( ) = 0;
};


#endif // HKBASE_HKMONITOR_STATISTICS_COLLECTOR_H

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
