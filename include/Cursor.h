#pragma once

#include <Arduino.h>

namespace Cursor {

class LineReader {
   public:
	virtual String readLine();	
	virtual bool available();
	virtual bool onLineReader(String& line);
   protected:
	virtual size_t peek();
	virtual size_t first();
	virtual bool next();
	virtual size_t last();
};

class LineWriter {
   public:
    virtual bool write(String& line);

   protected:
    virtual bool writeln(String& line);
};

class StringBuffer : LineReader, LineWriter {
   public:
    StringBuffer(size_t size);
    bool read(String& str);
    bool write(String& str);
    bool available();

   protected:
    String readString();
    String readString(size_t pos);
    virtual size_t first();
    virtual size_t last();
    virtual String getString(size_t pos);
    size_t writePos;
    size_t capacity;
};
// size_t ConfigHelper::first() { return 0; }
// size_t ConfigHelper::last() { return PARAM_COUNT - 1; }
// size_t ConfigHelper::next() { return this->readPos++; }

}  // namespace Cursor