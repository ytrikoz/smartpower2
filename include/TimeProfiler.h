#pragma once

#include <Arduino.h>

class TimeProfiler
{
	public:
	TimeProfiler();
	~TimeProfiler();
	private:
	unsigned long start = 0;
};

inline TimeProfiler::TimeProfiler()
{
	start = micros();
}

inline TimeProfiler::~TimeProfiler()
{
	Serial.print("Elapsed: ");
	Serial.print( micros() - start);
	Serial.println();
}
