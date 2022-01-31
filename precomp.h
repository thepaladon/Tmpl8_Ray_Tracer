#pragma once
/* This the precompiled header for this project. Each cpp file should include this on the very first line (otherwise a C1010 compilation error will be thrown)
 *
 * Meant for including _STABLE_ headers that are commonly used in your project to help speed up compilation.
 * In this case it is also used to ensure that the classes defined in the template.h / surface.h files are always to reduce complexity.
 *
 * Compilation is sped up by the included headers only being parsed once, and the parsed state is save to be re-used for every compilation.
 * The parsed state will be re-used until a change is detected in an included file.
 * Stable meaning the headers don't change often, or don't change at all over the course of your project.
 * Examples include standard headers that ship with the compiler or third party library headers
 *
 * This is _NOT_ a dumping ground for all headers in your project.
 * Any time anything is changed in a header that is included in this file it will trigger a full rebuild of your project.
 *
 * This is also not some magic file, the settings can be found in Project Properties -> C++ -> Precompiled Headers
 */

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>

#include "template.h"
#include "surface.h"

#include <random>
#include <thread>

 //Options
//#define FULLSCREEN
//#define ADVANCEDGL 

#if defined (_MSC_VER)  // Visual studio
#define thread_local __declspec( thread )
#elif defined (__GCC__) // GCC
#define thread_local __thread
#endif


#define SINGLETHREADED false	//for best performance: false
#define JOBSYSTEM true			//for best performance: true	

#define DEBUG_LINES false
#define ON_SCREEN_INFO true
#define MOUSE_CONTROLS true
#define MULTITHREADED_WRITE_COLOR true //doesn't seem to make any difference in performance lol 

//Screen
constexpr float aspect_ratio = 1.f / 1.f;
constexpr int ScreenWidth = 512;
constexpr int ScreenHeight = static_cast<int>(ScreenWidth / aspect_ratio);

// Values for Multithreading 
constexpr int squaresX = 16;
constexpr int squaresY = 16;
constexpr int screenSeparationX = ScreenWidth / squaresX; 
constexpr int screenSeparationY = ScreenHeight / squaresY; 

//CPU info
const unsigned int processor_count = std::thread::hardware_concurrency();

// Math Constants
constexpr float friction = 1.5f; //used for movement
const float infinity = std::numeric_limits<float>::infinity();
constexpr float PI = 3.14159265358979323846264338327950288419716939937510582097494459072381640628620899862803482534211706798f;


// Utility Functions
inline float degrees_to_radians(float degrees) {
	return degrees * PI / 180.0f;
}

inline float random_float()
{
	return (rand()) / (RAND_MAX + 1.0f);
}

inline float random_float(float min, float max)
{
	return min + (max - min) * random_float();
}

inline float clamp(float x, float min, float max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}


//THIS LEAKS MEMORY AND IS A HORRIBLE WAY TO DO IT, it'll be fixed once I have a job system so it's an OK placeholder
inline float random_float_threadSafe(const float& min, const float& max) {
	static thread_local std::mt19937 * generator = nullptr;
	if (!generator) generator = new std::mt19937(clock());
	const std::uniform_real_distribution<float> distribution(min, max);
	return distribution(*generator);
}
//Note from the future: A xorshift rand() would be better for a ray tracer. Too bad I don't have time to implement it right now

// ------------- TO-DO LIST ----------------------
// Mandatory ILOs:
// All done.
//
// RECOMMENDED by Peter Shirley:
// All done. 