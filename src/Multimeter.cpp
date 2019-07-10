#include "Multimeter.h"

Multimeter::Multimeter()
{
	mcp4652_init();

	this->voltage = 0;
	this->power = 0;
	this->current = 0;
	this->wattSeconds = 0;
	this->calcWattHours = false;
	this->lastUpdated = 0;		
	this->enabled = false;
}

void Multimeter::begin()
{
	this->enabled = true;
}

void Multimeter::end()
{
	this->enabled = false;
}

void Multimeter::loop()
{
	if (!this->enabled)
	{
		return;
	}
	
	unsigned long timePassed;

	if (this->lastUpdated == 0)
	{
		timePassed = MEASUREMENT_INTERVAL;
	} 
	else
	{
		timePassed	= millis() - this->lastUpdated;
	}

	if (timePassed >= MEASUREMENT_INTERVAL)
	{
		this->voltage = ina231_read_voltage();
		this->power = ina231_read_power();
		this->current = ina231_read_current();

		if (this->calcWattHours)
		{
			this->wattSeconds += this->power * (timePassed / 1000);
		}

		this->lastUpdated = millis();
	}
}

void Multimeter::setWattHours(double value)
{
	this->wattSeconds = value * 3600;
}

double Multimeter::getWattHours()
{
	return this->wattSeconds / SECONDS_IN_HOUR;
}

void Multimeter::enableWattHours(bool enabled)
{
	this->calcWattHours = enabled;
}

bool Multimeter::isWattHoursEnabled()
{
	return this->calcWattHours;
}

float Multimeter::getVoltage()
{
	return this->voltage;
}

float Multimeter::getCurrent()
{
	return this->current;
}

float Multimeter::getPower()
{
	return this->power;
}

String Multimeter::toString()
{
	String str = String(this->getVoltage(), 3);
	str += ",";
	str += String(this->getCurrent(), 3);
	str += ",";
	str += String(this->getPower(), 3);
	str += ",";
	str += String(this->getWattHours(), 3);
	
	return str;
}