#include "Timer.h"
#include <Windows.h>

Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::Init()
{

	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);

	PCFreq = double(li.QuadPart)/1000.0f;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;

	myCpuSpeedDouble = 1/((double)myCpuSpeed);
	//clockTickStart = GetCpuClocks();
	//clockTickNow = GetCpuClocks();
	deltaTime = 0.0;
	elapsedTime = 0.0;
	maxDeltaTime = 0.0;
	minDeltaTime = 999.999;
	averageDeltaTime = 0.0;
}

//Taken from http://stackoverflow.com/questions/275004/c-timer-function-to-provide-time-in-nano-seconds
__int64 Timer::GetCpuClocks()
{
// 
//     // Counter
//     struct { int low, high; } counter;
// 
//     // Use RDTSC instruction to get clocks count
//     __asm push EAX
//     __asm push EDX
//     __asm __emit 0fh __asm __emit 031h // RDTSC
//     __asm mov counter.low, EAX
//     __asm mov counter.high, EDX
//     __asm pop EDX
//     __asm pop EAX
// 
//     // Return result
//     return *(__int64 *)(&counter);

	return 0;

}

void Timer::TimeStep()
{
// 	clockTickNow = GetCpuClocks();
// 	clockTickStart = clockTickNow;

	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	deltaClockTicks = (li.QuadPart-CounterStart);
	deltaTime = double(deltaClockTicks)/PCFreq;
	CounterStart = li.QuadPart;

	elapsedTime += deltaTime;

	if(deltaTime>maxDeltaTime)
		(maxDeltaTime) ? maxDeltaTime = deltaTime:maxDeltaTime=0.1;

	if(deltaTime<minDeltaTime)
		minDeltaTime = deltaTime;

	(averageDeltaTime)?averageDeltaTime=(averageDeltaTime+deltaTime)*0.5:averageDeltaTime+=deltaTime;
}

__int64 Timer::GetDeltaClockTicks()
{
	return deltaClockTicks;
}

double Timer::GetElapsedTime()
{
	return elapsedTime;
}

double Timer::GetDeltaTime()// Milliseconds
{
	return deltaTime;
}

float Timer::GetDeltaTimeFloat()
{
	return (float)deltaTime;
}

double Timer::GetDeltaTimeInSeconds()
{
	return deltaTime/1000.0f;
}