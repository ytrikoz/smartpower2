#pragma once
#include <Arduino.h>
#include <mcp4652.h>

#define MEASUREMENT_INTERVAL 50
#define SECONDS_IN_HOUR 3600

class Multimeter
{
private:
	unsigned long lastUpdated;
	bool calcWattHours;  
	bool enabled;	

    float voltage; 
    float current; 
    float power; 	
	double wattSeconds;	
public:
	Multimeter();	

	void begin();	
	void end();	
	void loop();

	String toString();

	float getVoltage();
	float getCurrent();
	float getPower();

	double getWattHours();		
	void setWattHours(double value);
	void enableWattHours(bool enabled);			
	bool isWattHoursEnabled();
};