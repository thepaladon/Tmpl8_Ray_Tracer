#pragma once
#include <atomic>
#include "utilites.h"
namespace JobSystem
{
	struct Job;
	typedef void (*JobFunction)(Job*, const void*);

	enum ThreadStatus
	{
		IDLE,
		WORKING,
		EXIT
	};

	ThreadStatus status; 

	struct Job
	{
		Job(int x, int y) :yPos(y), xPos(x) {}
		Job() = default;
		~Job() = default;

	public:
		void ExecuteJob() {};

	private:
		int xPos;
		int yPos;
		
		const int ratiox = 4;
		const int ratioy = 3;
		const int screenSeparationX = ScreenWidth / ratiox; 
		const int screenSeparationY = ScreenHeight / ratioy; 

		JobFunction function;
		Job* parent;
		std::atomic_int32_t unfinishedJobs; // atomic
		//char padding[];
		int numCores = std::thread::hardware_concurrency();
		

	};

}

