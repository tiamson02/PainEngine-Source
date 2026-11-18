/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>
#include <hkbase/stopwatch/hkStopwatch.h>
#include <hkbase/config/hkConfigVersion.h>

#if HK_CONFIG_MEMORY_CLASS == HK_CONFIG_MEMORY_CLASS_ENABLED
#	include <hkbase/memory/hkDefaultMemory.h>
#endif
#include <hkbase/console/hkDefaultConsole.h>
#include <hkbase/error/hkDefaultError.h>
#include <hkbase/stream/hkDefaultStreambufFactory.h>

bool hkBaseSystemIsInitialized;

static void HK_CALL showHavokBuild()
{
	hkprintf("---------------------------\n");
	hkprintf(" Havok - Build (%d)\n",HAVOK_BUILD_NUMBER);
	hkprintf(" Base system initialized.   \n");
	hkprintf("----------------------------\n");
}

/// All global non plain old data (POD) types are encapsulated in
/// singletons. At program starup, static constructors put all these
/// singletons and functions to create them into a linked list
/// (see hkSingleton.h). At init time, we can then traverse the list,
/// setting up singletons which have not been explicitly set up already.
void HK_CALL hkBaseSystem::initSingletons()
{
	// ptrToCur is a handle to the current node.
	hkSingletonInitNode** ptrToCur = &hkSingletonInitList;
	hkSingletonInitNode* cur = *ptrToCur;

	hkArray<hkSingletonInitNode*> again;


	#ifdef HK_DEBUG
		if (cur == HK_NULL )
		{
			// *****************************************************************
			// The singletons list is empty - the static variable constructors have not been called.
			// If you are using Metrowerks, check that you have called mwInit()
			// prior to calling hkBaseSystem::init()
			// *****************************************************************

			HK_BREAKPOINT();
		}
	#endif

	while(cur)
	{
		if( *cur->m_value == HK_NULL )
		{
			void* p = (*cur->m_func)();
			if(p)
			{
				// constructed ok
				*cur->m_value = p;
				ptrToCur = &cur->m_next;
				cur = cur->m_next;
			}
			else
			{
				// If a singleton creation function returns HK_NULL it means
				// that the object was not ready to be constructed (perhaps
				// it depends on another singleton?)
				// Remove it from the list and save it for later
				again.pushBack(cur);
				cur = cur->m_next;
				(*ptrToCur)->m_next = HK_NULL;
				*ptrToCur = cur;
			}
		}
		else // skip already done nodes
		{
			ptrToCur = &cur->m_next;
			cur = cur->m_next;
		}
	}

	// Go through the deferred list.
	while( again.getSize() )
	{
		int origSize = again.getSize();
		for(int i = origSize-1; i>=0; --i)
		{
			cur = again[i];
			HK_ASSERT(cur->m_next == HK_NULL);

			void* p = (*cur->m_func)();
			if(p)
			{
				// succeeded, put it back on the global list.
				*cur->m_value = p;
				*ptrToCur = cur;
				ptrToCur = &cur->m_next;
				again.removeAt(i);
			}
		}
		HK_ASSERT2( again.getSize() < origSize, "cycle detected in singleton construction");
	}
	// array of singleton nodes is now sorted by dependency.
}

/// See also initSingletons().
void HK_CALL hkBaseSystem::quitSingletons()
{
	// destroy singletons in reverse order to creation.
	hkArray<hkSingletonInitNode*> nodes;
	nodes.reserve(16);
	hkSingletonInitNode* cur = hkSingletonInitList;
	while(cur)
	{
		nodes.pushBack(cur);
		cur = cur->m_next;
	}

	for(int i = nodes.getSize()-1; i >= 0; --i)
	{
		hkReferencedObject* obj = static_cast<hkReferencedObject*>(*nodes[i]->m_value);
		obj->removeReference();
		*nodes[i]->m_value = HK_NULL;
	}
}


hkResult HK_CALL hkBaseSystem::defaultInit()
{
	extern bool hkBaseSystemIsInitialized;
	if(hkBaseSystemIsInitialized==false)
	{
#if HK_CONFIG_MEMORY_CLASS == HK_CONFIG_MEMORY_CLASS_ENABLED
		hkMemory::replaceInstance( new hkDefaultMemory() );
#endif
		hkStreambufFactory::replaceInstance( new hkDefaultStreambufFactory() );
		hkConsole::replaceInstance( new hkDefaultConsole() );
		hkError::replaceInstance( new hkDefaultError() );

		initSingletons();

		hkBaseSystemIsInitialized = true;
		showHavokBuild();
	}
	return HK_SUCCESS;
}

hkResult HK_CALL hkBaseSystem::defaultQuit()
{
	extern bool hkBaseSystemIsInitialized;
	if(hkBaseSystemIsInitialized==true)
	{
		quitSingletons();

		// reverse order to init.
		hkError::replaceInstance( HK_NULL );
		hkConsole::replaceInstance( HK_NULL );
		hkStreambufFactory::replaceInstance( HK_NULL );
#if HK_CONFIG_MEMORY_CLASS == HK_CONFIG_MEMORY_CLASS_ENABLED
		hkMemory::replaceInstance( HK_NULL );
#endif

		hkBaseSystemIsInitialized = false;
	}
	return HK_SUCCESS;
}

	//  Initialize the default subsystems. It is safe to call multiple times.
	/// This function should initialize the singletons which everything else
	/// depends on: hkMemory, hkStreambufFactory, hkConsole and hkError in that order.
	/// Next it can optionally replace other singletons with custom implementations.
	/// Then it may call initSingletons() to initialize the remaining singletons with
	/// their default implementations. Finally, set the hkBaseSystemIsInitialized flag.
hkResult (*hkBaseSystem::init)() = hkBaseSystem::defaultInit;

	//  Quit the subsystems. It is safe to call multiple times.
	/// It cleans up in reverse order to init: default singletons, hkError,
	/// hkConsole, hkStreambufFactory and hkMemory
hkResult (*hkBaseSystem::quit)() = hkBaseSystem::defaultQuit;

bool HK_CALL hkBaseSystem::isInitialized()
{
	return hkBaseSystemIsInitialized;
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
