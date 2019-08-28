#include "Cursor.h"

namespace Cursor {

bool LineReader::available() {return false; };
bool LineReader::onLineReader(String& line) {
    line = readLine();
    next(); 
    return available();
}

String LineReader::readLine() { return String(); }

size_t LineReader::peek() { return -1; }

size_t LineReader::first() { return -1; }

bool LineReader::next() { return false; }

size_t LineReader::last() { return -1; }

bool LineWriter::write(String& str) { return false; }

}  // namespace Cursor