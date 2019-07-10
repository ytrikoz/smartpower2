#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>

#include <LiquidCrystal_I2C.h>

#include "consts.h"
#include "str_utils.h"

#define LCD_ROWS 2
#define LCD_COLS 16
#define REFRESH_INTERVAL 250

#define MAX_LABEL_LENGTH 4
#define FPS_ITER 1
#define DELAY_TIME 3500

typedef struct 
{	
	bool needsToRefresh;
	char param[LCD_COLS + 1];
	char value[LCD_COLS * 4 + 1];
	uint8_t index;
	uint8_t page;
	
} DisplayLine;

class Display
{
private:		
	uint8_t addr;	
	LiquidCrystal_I2C *lcd;    	
	DisplayLine line[LCD_ROWS];

	unsigned long lastUpdateTime;
	bool connected;
	
	bool getAddr();	

	void showByteXfer(unsigned long etime);
	void showFPS(unsigned long etime, const char *type);
	unsigned long timeFPS(uint8_t iter, uint8_t cols, uint8_t rows);	
	
	void show(const String &s);

	HardwareSerial *_serial;	
	void _print(String arg);
	void _println(void);
	void _println(String arg);
		
public:
	Display(HardwareSerial *serial);
	bool init();
	bool isConnected();
	
	void turnOn();
	void turnOff();		
	
	void onProgress(uint8_t per, const char* message = NULL);

	void setLine(uint8_t n, const char* textStr);
    const char* getParam(uint8_t n);
	void setParamLine(uint8_t row, const char *paramStr, const char *valueStr);
	void update(boolean forced = false);
};
