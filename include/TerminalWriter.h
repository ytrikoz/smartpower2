#pragma once
#include <Print.h>

#include "SystemClock.h"

#include "consts.h"
#include "sysinfo.h"

typedef enum {CRLF, LFCR, LF, CR} EOLMarker;
typedef enum {MD_LEFT, MD_RIGHT, MD_UP, MD_DOWN} MoveDirection;

class TerminalWriter : public Print
{
public:
	TerminalWriter(Print *output);
	~TerminalWriter();
	void enableCtrlCodes(bool enabled = true);
	void prompt();
	void del();
	void clear_eol();

    size_t write(uint8_t c);
    size_t write(const uint8_t *buffer, size_t size); 

	size_t println(const char* str);
	size_t println(void);

private:
	Print *output;
	bool ctrlCodesEnabled;
	EOLMarker eol;
};
