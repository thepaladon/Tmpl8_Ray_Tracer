#include "precomp.h"
#include "jobsys.h"    // include our interface

#include <algorithm>    // std::max
#include <atomic>    
#include <thread>    
#include <condition_variable>

namespace JobSystem
{

    //Credit to : https://wickedengine.net/2018/11/24/simple-job-system-using-standard-c/ for helping me implement and understand this job system.

    uint32_t numThreads = 0;                                     // number of worker threads, it will be initialized in the Initialize() function
    ThreadSafeRingBuffer<std::function<void()>, 256> jobPool;    // a thread safe queue to put pending jobs onto the end (with a capacity of 256 jobs).
																 // A worker thread can grab a job from the beginning

    std::condition_variable wakeCondition;                       // used in conjunction with the wakeMutex below. Worker threads just sleep when
																 //...there is no job, and the main thread can wake them up

	std::mutex wakeMutex;                                        // used in conjunction with the wakeCondition above
    uint64_t currentLabel = 0;                                   // tracks the state of execution of the main thread
    std::atomic<uint64_t> finishedLabel;                         // track the state of execution across background worker threads

	void Initialize()
    {
        // Initialize the worker execution state to 0:
        finishedLabel.store(0);

        // Retrieve the number of hardware threads in this system:
        const unsigned numCores = processor_count;

        // Calculate the actual number of worker threads we want: 
        numThreads = std::max(1u, numCores);

        // Create all our worker threads while immediately starting them:
        for (uint32_t threadID = 0; threadID < numThreads; ++threadID)
        {
            std::thread worker([] {

                std::function<void()> job; // the current job for the thread, it's empty at start.

                // This is the infinite loop that a worker thread will do 
                while (true)
                {
                    if (jobPool.pop_front(job)) // try to grab a job from the jobPool queue
                    {
                        // It found a job, execute it:
                        job(); // execute job
                        finishedLabel.fetch_add(1); // update worker label state
                    }
                    else
                    {
                        // no job, put thread to sleep
                        std::unique_lock<std::mutex> lock(wakeMutex);
                        wakeCondition.wait(lock);
                    }
                }
                });

            worker.detach(); // forget about this thread, let it do it's job in the infinite loop that we created above
        }
    }

    // This little helper function will not let the system to be deadlocked while the main thread is waiting for something
    inline void poll()
    {
        wakeCondition.notify_one(); // wake one worker thread
        std::this_thread::yield(); // allow this thread to be rescheduled
    }

    void Execute(const std::function<void()>& job)
    {
        // The main thread label state is updated:
        currentLabel += 1;

        // Try to push a new job until it is pushed successfully:
        while (!jobPool.push_back(job)) { poll(); }

        wakeCondition.notify_one(); // wake one thread
    }

    bool IsBusy()
    {
        // Whenever the main thread label is not reached by the workers, it indicates that some worker is still alive
        return finishedLabel.load() < currentLabel;
    }

    void Wait()
    {
        while (IsBusy()) { poll(); }
    }

}