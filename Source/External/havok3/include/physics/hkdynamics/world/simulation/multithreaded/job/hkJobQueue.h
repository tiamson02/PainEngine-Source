/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS_JOB_QUEUE_H
#define HK_DYNAMICS_JOB_QUEUE_H

#include <hkbase/thread/hkCriticalSection.h>
#include <hkbase/thread/hkSemaphoreBusyWait.h>
#include <hkbase/thread/hkSpuUtils.h>

#if defined HK_PLATFORM_PS3SPU
#include <hkbase/htl/hkFixedSizeQueue.h>
# define HK_QUEUE hkFixedSizeQueue
# else
#include <hkbase/htl/hkQueue.h>
# define HK_QUEUE hkQueue
# endif

	/// This class implements a job queue with all necessary locking and waiting
class hkJobQueue: public hkSynchronized
{
	public:

			/// The basic struct stored in the job queue
		struct JobQueueEntry
		{
			hkUchar m_data[128];
		};

		//
		// Enums to specify job types
		//

			/// Types of job that can be placed on and retrieved from the job queue
		enum JobType
		{
			JOB_TYPE_CPU_PRIMARY,
			JOB_TYPE_CPU,
			JOB_TYPE_SPU
		};

			/// Types of threads that can place jobs on and retrieve jobs from the job queue
		enum ThreadType
		{
			THREAD_TYPE_CPU_PRIMARY,
			THREAD_TYPE_CPU,
			THREAD_TYPE_SPU
		};

			/// Whether a job is placed at the front or the end of its appropriate queue
		enum JobPriority
		{
			JOB_HIGH_PRIORITY,
			JOB_LOW_PRIORITY,
		};


		//
		// Callback functions used from within critical sections
		//


				/// The result from JobPopFunc
			enum JobPopFuncResult
			{
					/// Set to this if the function is taking the element off the queue
				POP_QUEUE_ENTRY,
					/// Set to this if the function has modified the element, and left it on the queue
				DO_NOT_POP_QUEUE_ENTRY 
			};

		typedef JobPopFuncResult (HK_CALL *JobPopFunc)( JobQueueEntry& jobIn, JobQueueEntry& jobOut );

			enum JobCreationStatus
			{
				JOB_CREATED,
				NO_JOB_CREATED
			};

			struct JobQueueEntryInput
			{
				JobType m_jobThreadType;
				JobPriority m_jobPriority;
				JobQueueEntry m_job;
			};

		typedef JobCreationStatus (HK_CALL *FinishJobFunc)( const JobQueueEntry& jobIn, JobQueueEntryInput& newJobCreatedOut, void* params );


	public:

			/// Note: this class should only ever be constructed on a CPU. If running on an SPU it must be dma'd into an
			/// appropriately sized 128 byte aligned local store buffer.
		hkJobQueue();

		~hkJobQueue();

		//
		// Runtime functions
		//


			/// Adds a new job to the queue for a given priority and job thread type.
		void addJob( JobQueueEntry& job, JobPriority priority, JobType type );

		
				/// A flag to tell getNextJob what to do if no job is immediately available
			enum WaitStatus
			{
				WAIT_FOR_NEXT_JOB,
				DO_NOT_WAIT_FOR_NEXT_JOB
			};

				/// Whether a getNextJob call got another job or not
			enum JobStatus
			{
				GOT_NEXT_JOB,
				NO_JOBS_AVAILABLE,
				ALL_JOBS_FINISHED
			};

			/// Gets a new job. Note: this function should only be called at the start of the main loop.
			/// If you can you should call addAndGetNextJob
		JobStatus getNextJob( ThreadType threadType, JobQueueEntry& job, WaitStatus waitStatus = WAIT_FOR_NEXT_JOB );


		//
		// Compound functions
		//

			/// Call this when one job is finished and you are not calling addJobAndGetNextJob
		JobStatus finishJobAndGetNextJob( ThreadType threadType, const JobQueueEntry* oldJob, JobQueueEntry& jobOut, void* params = HK_NULL, WaitStatus waitStatus = WAIT_FOR_NEXT_JOB );

			/// Finishes a job and gets a new one
			/// This is faster than calling addJob and getNextJob separately, because it can do the
			/// entire operation in one critical section, rather than two critical sections.
		JobStatus addJobAndGetNextJob( JobType jobThreadType, JobPriority priority, ThreadType threadType, JobQueueEntry& jobInOut, WaitStatus waitStatus = WAIT_FOR_NEXT_JOB );

	public:
		struct DynamicData;

	protected:
			// Call this to release all waiting threads
		void releaseWaitingThreads(DynamicData* data);

	public:
		HK_CPU_PTR(DynamicData*) m_data;

		bool m_isLocked;

		//
		// Data to transfer once
		//
		
			// A semaphore used to signal waiting jobs
		hkSemaphoreBusyWait m_cpuWaitSemaphore;
		hkSemaphoreBusyWait m_spuWaitSemaphore;


		//
		// Static, locally set data
		//

		struct JobQueryRule
		{
			hkUint8 m_numJobTypesAvailable;
			hkEnum<JobType, hkUint8> m_jobTypeOrder[3];
		};

		JobQueryRule m_jobQueryRules[3];

		JobPopFunc m_jobPopFunc; 
		FinishJobFunc m_finishJobFunc;

	public:
		void setInitialNumSpuJobs( int num );

	private:


			// Check if a thread is waiting for a job on a semaphore.
			// If one thread is, release the semaphore
		HK_FORCE_INLINE void releaseOneWaitingThread( JobType jobThreadType, DynamicData* data  );

		HK_FORCE_INLINE JobStatus findNextJob(ThreadType threadType, JobQueueEntry& jobOut, DynamicData* data );
		
		HK_FORCE_INLINE hkBool allQueuesEmpty( DynamicData* data );

		HK_FORCE_INLINE DynamicData* lockQueue( );
		HK_FORCE_INLINE void unlockQueue( );

};


#endif // HK_DYNAMICS_JOB_QUEUE_H

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
