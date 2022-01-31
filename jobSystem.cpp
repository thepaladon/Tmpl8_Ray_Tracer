#include "jobSystem.h"

	/*// main function of each worker thread
	while ()
	{
		Job* job = GetJob();
		if (job)
		{
			Execute(job);
		}
	}

	Job* GetJob(void)
	{
		WorkStealingQueue* queue = GetWorkerThreadQueue();

		Job* job = queue->Pop();
		if (IsEmptyJob(job))
		{
			// this is not a valid job because our own queue is empty, so try stealing from some other queue
			unsigned int randomIndex = GenerateRandomNumber(0, g_workerThreadCount + 1);
			WorkStealingQueue* stealQueue = g_jobQueues[randomIndex];
			if (stealQueue == queue)
			{
				// don't try to steal from ourselves
				Yield();
				return nullptr;
			}

			Job* stolenJob = stealQueue->Steal();
			if (IsEmptyJob(stolenJob))
			{
				// we couldn't steal a job from the other queue either, so we just yield our time slice for now
				Yield();
				return nullptr;
			}

			return stolenJob;
		}

		return job;
	}

	void Execute(Job * job)
	{
		(job->function)(job, job->data);
		Finish(job);
	}*/