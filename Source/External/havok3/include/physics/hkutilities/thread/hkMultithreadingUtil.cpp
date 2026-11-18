/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics/hkDynamics.h>
#include <hkbase/thread/hkCriticalSection.h>

#include <hkutilities/thread/hkMultithreadingUtil.h>

#include <hkdynamics/world/hkWorld.h>
#include <hkdynamics/world/simulation/hkSimulation.h>

#include <hkbase/basesystem/hkBaseSystem.h>
#include <hkbase/monitor/hkMonitorStream.h>
#include <hkbase/monitor/hkMonitorStreamAnalyzer.h>
#include <hkutilities/visualdebugger/hkPhysicsContext.h>
#include <hkbase/stopwatch/hkStopwatch.h>

static void* HK_CALL WorkerThreadFunc(void* v);

hkMultithreadingUtil::State::State() : m_workerThreadFinished(0, hkMultithreadingUtil::MAX_NUM_PHYSICS_THREADS), m_workerThreads()
{
	m_numThreads = 0;
	m_physicsRunning = false;

	m_world = HK_NULL;
	m_deltaTime = 1.0f / 60.0f;
}

hkMultithreadingUtil::WorkerThreadData::WorkerThreadData() : m_semaphore(0,1) 
{
	m_monitorStreamBegin = HK_NULL;
	m_monitorStreamEnd = HK_NULL;
	m_streamAnalyzer = HK_NULL;
	m_localMonitorStreamBegin = HK_NULL;
	m_localMonitorStreamEnd = HK_NULL;
	m_killThread = false;
	m_threadId = -1;
	m_state = HK_NULL;
}

	/// Initialize multi-threading state and create threads.
hkMultithreadingUtil::hkMultithreadingUtil(const hkMultithreadingUtilCinfo& ci)
{
	m_state.m_localStackSize = ci.m_localStackSize;
	ci.m_world->addReference();
	initMultithreading( ci.m_world, ci.m_numThreads);

}

// Destroy threads and delete state.
hkMultithreadingUtil::~hkMultithreadingUtil()
{
	m_state.m_world->removeReference();
	quitMultithreading();
}


void hkMultithreadingUtil::initMultithreading(hkWorld* world, int numThreads)
{
	HK_ASSERT2(0xad67dd83, world && (numThreads <= 1 || world->m_simulationType >= hkWorldCinfo::SIMULATION_TYPE_MULTITHREADED), "To create more than one physics thread you must use the hkWorldCinfo::SIMULATION_TYPE_MULTITHREADED simulation type.");
	
	m_state.m_world = world;

	if (numThreads > MAX_NUM_PHYSICS_THREADS)
	{
		numThreads = MAX_NUM_PHYSICS_THREADS;
	}

	for (int i = 0; i < numThreads; i++ )
	{
		WorkerThreadData& data = m_state.m_workerThreads[m_state.m_numThreads++];
		data.m_state = &m_state;
		data.m_threadId = i;
		data.m_thread.startThread( &WorkerThreadFunc, &data );
	}
}

void hkMultithreadingUtil::startStepWorld(hkReal frameDeltaTime, hkReal physicsDeltaTime)
{
	HK_ASSERT2(0xad56dd77, m_state.m_physicsRunning == false, "Calling hkMultithreadingUtil::startStepWorld() for the second time, without having called hkMultithreadingUtil::waitForStepWorldFinished().");
	m_state.m_physicsRunning = true;

	m_state.m_deltaTime = physicsDeltaTime;
	m_state.m_frameDeltaTime = frameDeltaTime;

	for (int i = m_state.m_numThreads - 1; i >=0; i--)
	{
		WorkerThreadData& data = m_state.m_workerThreads[i];
		data.m_semaphore.release();
	}
}

void hkMultithreadingUtil::waitForStepWorldFinished()
{
	// This function does nothing if waitForStepWorldFinished() is called before startStepWorld()
	if (m_state.m_physicsRunning)
	{
		for (int i = 0; i < m_state.m_numThreads; ++i)
		{
			m_state.m_workerThreadFinished.acquire();
		}
		m_state.m_physicsRunning = false;
	}

	// copy all monitor streams to the internal buffer
	{
		for (int i = 0; i < m_state.m_numThreads; ++i)
		{
			WorkerThreadData& data = m_state.m_workerThreads[i];
			if ( data.m_localMonitorStreamBegin )
			{
				int size = int(data.m_localMonitorStreamEnd - data.m_localMonitorStreamBegin);
				hkString::memCpy( data.m_monitorStreamBegin, data.m_localMonitorStreamBegin, size );
				data.m_monitorStreamEnd = data.m_monitorStreamBegin + size;
			}
			else
			{
				data.m_monitorStreamEnd = data.m_monitorStreamBegin;
			}
		}
	}
}

void hkMultithreadingUtil::stepWorld(hkReal frameDeltaTime, hkReal physicsDeltaTime)
{
	startStepWorld(frameDeltaTime, physicsDeltaTime);
	waitForStepWorldFinished();
}

void hkMultithreadingUtil::quitMultithreading()
{
	waitForStepWorldFinished();
	
	{
		for (int i = 0; i < m_state.m_numThreads; i++)
		{
			WorkerThreadData& data = m_state.m_workerThreads[i];
			data.m_killThread = true;
			data.m_semaphore.release(); // sets the thread off to enable it to finish
		}
	}
	{
		for (int i = 0; i < m_state.m_numThreads; i++)
		{
			m_state.m_workerThreadFinished.acquire(); // wait for the N threads to actually end
		}
	}
}

// Get number of threads used for physics
int hkMultithreadingUtil::getNumThreads()
{
	return m_state.m_numThreads;
}

// Set the number of threads to be used for physics. Returns the number of threads actually set.
int hkMultithreadingUtil::setNumThreads(int numThreads)
{
	while( m_state.m_numThreads < numThreads && HK_SUCCESS == addThread()    ) { }
	while( m_state.m_numThreads > numThreads && HK_SUCCESS == removeThread() ) { }
	return m_state.m_numThreads;
}


hkResult hkMultithreadingUtil::addThread()
{
	HK_ASSERT2(0xad67bd88, ! m_state.m_physicsRunning, "You can only add or remove working threads via calls from the master thread and not during stepWorld(). ");

	if (m_state.m_world->m_simulationType < hkWorldCinfo::SIMULATION_TYPE_MULTITHREADED)
	{
		HK_WARN_ONCE(0xad67dd83, "To create more than one physics thread you must use the hkWorldCinfo::SIMULATION_TYPE_MULTITHREADED simulation type.");
	}

	int maxNumThreads = m_state.m_world->m_simulationType >= hkWorldCinfo::SIMULATION_TYPE_MULTITHREADED ? MAX_NUM_PHYSICS_THREADS : 1;
	if (m_state.m_numThreads < maxNumThreads)
	{
		WorkerThreadData& data = m_state.m_workerThreads[m_state.m_numThreads];
		data.m_state = &m_state;
		data.m_threadId = m_state.m_numThreads;
		data.m_monitorStreamBegin = HK_NULL;
		data.m_monitorStreamEnd = HK_NULL;
		data.m_killThread = false;
		data.m_thread.startThread( &WorkerThreadFunc, &data );

		m_state.m_numThreads++;

		return HK_SUCCESS;
	}

	return HK_FAILURE;
}


hkResult hkMultithreadingUtil::removeThread()
{
	HK_ASSERT2(0xad67bd88, ! m_state.m_physicsRunning, "You can only add or remove working threads via calls from the master thread and not between initializeWorldStep() and finishWorldStep() calls. ");

	if (m_state.m_numThreads > 0)
	{
		--m_state.m_numThreads;
		
		WorkerThreadData& data = m_state.m_workerThreads[m_state.m_numThreads];

		// Signal the thread to be killed, and release the thread
		data.m_killThread = true;
		data.m_semaphore.release();

		// Wait until the thread actually finishes
		m_state.m_workerThreadFinished.acquire();

		return HK_SUCCESS;
	}
	return HK_FAILURE;
}



static void* HK_CALL WorkerThreadFunc(void *v)
{
	hkMultithreadingUtil::WorkerThreadData& data = *static_cast<hkMultithreadingUtil::WorkerThreadData*>(v);
	hkMultithreadingUtil::State& state = *data.m_state;

#ifdef HK_PLATFORM_XBOX360
	// If the platform is xbox 360 then take one thread per cpu for the 1st 3 threads, then start hyperthreading.
	int tid = (data.m_threadId % 3) * 2;
	if (data.m_threadId > 2)
		tid++; // take other slot on this cpu
	XSetThreadProcessor(GetCurrentThread(), tid);
#endif

	// Initialize the thread memory for this thread, referencing the global memory instance
	hkThreadMemory threadMemory(&hkMemory::getInstance(), 16);
	{ 
		hkBaseSystem::initThread( &threadMemory );
	}

	// Allocate stack memory for this thread 
	char* stackBuffer;
	{
		int stackSize = state.m_localStackSize; 
		stackBuffer = hkAllocate<char>( stackSize, HK_MEMORY_CLASS_BASE);
		threadMemory.setStackArea( stackBuffer, stackSize);
	}

	// Allocate a monitor stream for this thread - this  enables timers.
	{
		int monitorStreamSize = 2000000;
		hkMonitorStream::resize(monitorStreamSize);
		data.m_monitorStreamBegin = hkAllocate<char>(monitorStreamSize, HK_MEMORY_CLASS_BASE);
		data.m_monitorStreamEnd = data.m_monitorStreamBegin;
	}
	// Allocate a large buffer for this thread - this is only needed if we wish to gather
	// detailed stats for the simulation.
	{
		data.m_streamAnalyzer = new hkMonitorStreamAnalyzer( 10000000 ); // 10megs
	}

	// Wait for the main thread to release the physics thread 
	data.m_semaphore.acquire();

	// The thread "main loop"
	while (data.m_killThread == false)
	{
		hkMonitorStream::reset();


		// Enable timers for critical sections just during the step call
		hkCriticalSection::setTimersEnabled();

		// Note: This overall timer needs to be here for our timer picture output
		HK_TIMER_BEGIN("MT Step", HK_NULL);
	
		// Step the world forward. Calling this with multiple threads simultaneously
		// causes the world step to be multithreaded.

		if (state.m_world->prepareMultithreadStep() == ARE_MAIN_THREAD_NOW_CALL_STEP_DELTA_TIME)
		{
			state.m_world->stepAsynchronously(state.m_frameDeltaTime, state.m_deltaTime);
		}
		else
		{
			state.m_world->childThreadStep();
		}
		HK_TIMER_END();

		// Disable timers for critical sections just during the step call
		hkCriticalSection::setTimersDisabled();

		// Note collected timer data
		data.m_localMonitorStreamBegin = hkMonitorStream::getStart();
		data.m_localMonitorStreamEnd   = hkMonitorStream::getEnd();

		// Release any thread (usually the main thread) which may be waiting for all worker threads to finish.
		state.m_workerThreadFinished.release();

		// Immediately wait until the main thread releases the physics thread again 
		// - we do nothing but physics in this thread
		data.m_semaphore.acquire();
	}


	// Perform cleanup operations

	hkDeallocate(data.m_monitorStreamBegin);
	data.m_monitorStreamBegin = HK_NULL;
	data.m_monitorStreamEnd  = HK_NULL;
	data.m_localMonitorStreamBegin = HK_NULL;
	data.m_localMonitorStreamEnd = HK_NULL;
	delete data.m_streamAnalyzer;
	data.m_streamAnalyzer = HK_NULL;

	threadMemory.setStackArea(0, 0);
	hkDeallocate(stackBuffer);

	hkBaseSystem::clearThreadResources();

	state.m_workerThreadFinished.release();

	return 0;
}


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
