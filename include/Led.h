#pragma once
#include "Arduino.h"

typedef enum {mode_off,  mode_on, mode_blink} led_mode;
typedef enum {state_off = HIGH, state_on = LOW} led_state;

class Led
{
public:
	Led(uint8_t pin);
	void loop();			
	void turnOn();
	void turnOff();			
	void blink(uint8_t sec = 0);	
	void setFreq(uint8_t hz);
private:	
	uint8_t pin;
	led_mode prev, curr;
	led_state state;
	
	bool changed;

	// bink times per sec
	uint8_t freq;	
	uint16_t intervalOn;
	uint16_t intervalOff;

	// blink unitl time (ms)
	unsigned long blinkTime;	
	// last state update (ms)
	unsigned long updated;

	void refresh();
};