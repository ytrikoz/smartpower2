#include "DiffLog.h"

DiffLog::DiffLog(const char* label, size_t size, float base): PsuLog(label, size) {
	this->base = base;
}


