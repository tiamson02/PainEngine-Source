/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkserialize/version/hkObjectUpdateTracker.h>
#include <hkserialize/util/hkPointerMultiMap.h>

/// A tracker which understands the layout of packfiles.
class hkPackfileObjectUpdateTracker : public hkObjectUpdateTracker
{
	public:

		hkPackfileObjectUpdateTracker( hkPackfileReader::AllocatedData& data )
			: m_allocatedData(data)
		{
			m_topLevelObject = HK_NULL;
			m_topLevelClassName = HK_NULL;
		}

			//
			// Memory
			//

		virtual void addAllocation(void* p)
		{
			m_allocatedData.addAllocation(p);
		}
		virtual void addChunk(void* p, int n, HK_MEMORY_CLASS c)
		{
			m_allocatedData.addChunk(p,n,c);
		}

			//
			// Pointers
			//

		void objectPointedBy( void* object, void* fromWhere )
		{
			m_pointers.insert(object,fromWhere);
		}

		virtual void replaceObject( void* oldObject, void* newObject, const hkClass* newClass )
		{
			if( oldObject == m_topLevelObject )
			{
				m_topLevelObject = newObject;
				m_topLevelClassName = newClass->getName();
			}

			int index = m_pointers.getFirstIndex(oldObject);
			m_pointers.m_indexMap.insert( newObject, index );
			while( index != -1 )
			{
				void* ptrOldObject = m_pointers.getValue(index);
				HK_ASSERT(0x7fe24edd, *static_cast<void**>(ptrOldObject) == oldObject );
				*static_cast<void**>(ptrOldObject) = newObject;
				index = m_pointers.getNextIndex(index);
			}			
			
			if( newClass && newClass->hasVtable() )
			{
				addFinish(newObject, newClass->getName());
			}
		}

			//
			// Finish
			//

		void addFinish( void* newObject, const char* className )
		{
			HK_ASSERT( 0x567037f2, m_finish.hasKey(newObject) == false );
			m_finish.insert( newObject, className );
		}

		void removeFinish( void* oldObject )
		{
			m_finish.remove(oldObject);
		}

		hkPackfileReader::AllocatedData& m_allocatedData;
		hkPointerMultiMap<void*> m_pointers;
		hkPointerMap<void*, const char*> m_finish;
		void* m_topLevelObject;
		const char* m_topLevelClassName;
};

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
