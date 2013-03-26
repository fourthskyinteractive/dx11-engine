#ifndef TIMER_H
#define TIMER_H

#include "../../Game/Definitions.h"

class Timer
{
public:
	Timer();
	Timer(const Timer& _timer);
	~Timer();

	void Init();
	__int64 GetCpuClocks();//asm
	void TimeStep();

	__int64 GetDeltaClockTicks();
	double GetElapsedTime();// Milliseconds
	double GetDeltaTime(); // Milliseconds
	float GetDeltaTimeFloat();
	double GetDeltaTimeInSeconds();

private:
	__int64 clockTickStart;
	__int64 clockTickNow;
	__int64 myCpuSpeed;
	__int64 deltaClockTicks;

	double deltaTime;
	double elapsedTime;
	double maxDeltaTime;
	double minDeltaTime;
	double averageDeltaTime;
	double myCpuSpeedDouble;
	double PCFreq;
	__int64 CounterStart;

};

#endif