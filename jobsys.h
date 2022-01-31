#pragma once
#include "precomp.h"

#include <functional>
#include <future>
#include <functional>

namespace JobSystem {

	// Fixed size very  simple thread safe ring buffer -
	// Credit to: https://wickedengine.net/2018/11/24/simple-job-system-using-standard-c/
	template <typename T, size_t capacity>
	class ThreadSafeRingBuffer
	{
	public:
		//  Push an item to the end if there is free space
		//  Returns true if successful
		//  Returns false if there is not enough space
		inline bool push_back(const T& item)
		{
			bool result = false;
			lock.lock();
			const size_t next = (head + 1) % capacity;
			if (next != tail)
			{
				data[head] = item;
				head = next;
				result = true;
			}
			lock.unlock();
			return result;
		}

		// Get an item if there are any
		//  Returns true if successful
		//  Returns false if there are no items
		inline bool pop_front(T& item)
		{
			bool result = false;
			lock.lock();
			if (tail != head)
			{
				item = data[tail];
				tail = (tail + 1) % capacity;
				result = true;
			}
			lock.unlock();
			return result;
		}

	private:
		T data[capacity];
		size_t head = 0;
		size_t tail = 0;
		std::mutex lock; // this just works better than a spinlock here (on windows)
	};

	// A Dispatched job will receive this as function argument:
	struct JobDispatchArgs
	{
		uint32_t jobIndex;
		uint32_t groupIndex;
	};

	// Create the internal resources such as worker threads, etc. Call it once when initializing the application.
	void Initialize();

	// Add a job to execute asynchronously. Any idle thread will execute this job.
	void Execute(const std::function<void()>& job);

	// Check if any threads are working currently or not
	bool IsBusy();

	// Wait until all threads become idle
	void Wait();
}