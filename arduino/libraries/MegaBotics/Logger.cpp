/*
 * Logger.cpp
 *
 *  Created on: Apr 7, 2015
 *      Author: Aakash Sahai
 */

#include <Logger.h>
#include <string.h>
#include <avr/pgmspace.h>

Logger::~Logger() {
}

void Logger::setup(Config &aConfig) {
	_config.bufsize = aConfig.bufsize;
	_config.wifi = aConfig.wifi;
	_buffer.setup(_config.bufsize);
}

bool Logger::start(const char *message) {
	return log(LEVEL_INFO, F("[OPEN]"), message);
}

bool Logger::finish(const char *message) {
	return log(LEVEL_INFO, F("[CLOSE]"), message);
}

void Logger::writeLevelType(Level level, const __FlashStringHelper *type) {
	if (!doLog(level)) return;
	char ch;
	_buffer.enqueue((char)level);
	_buffer.enqueue('[');
	PGM_P p = reinterpret_cast<PGM_P>(type);
	while(1) {
		ch = pgm_read_byte(p++);
		if (ch == 0) break;
		_buffer.enqueue(ch);
	}
	_buffer.enqueue(']');
}

void Logger::writeNameValue(const __FlashStringHelper *name, char *value) {
	char ch;
	if (!_firstValue) {
		_buffer.enqueue(',');
	}
	_firstValue = false;

	PGM_P p = reinterpret_cast<PGM_P>(name);
	while(1) {
		ch = pgm_read_byte(p++);
		if (ch == 0) break;
		_buffer.enqueue(ch);
	}
	_buffer.enqueue(':');
	while ((ch = *value++)) {
		_buffer.enqueue(ch);
	}
}

bool Logger::log(Level level, const __FlashStringHelper *type, const char *message) {
	if (!doLog(level)) return false;
	int typeLen = strlen_P((const char *)type);
	int msgLen = strlen(message);
	if (_buffer.capacity() < (4 + typeLen + msgLen)) {	// Make sure the whole log will fit
		return false;
	} else {
		char ch;
		writeLevelType(level, type);
		while ((ch = *message++)) {
			_buffer.enqueue(ch);
		}
		_buffer.enqueue('\n');
	}
	return true;
}

Logger & Logger::begin(Level level, const __FlashStringHelper *type) {
	if (!doLog(level)) return *this;
	_nvLevel = level;
	int typeLen = strlen_P((const char *)type);
	if (_buffer.capacity() >= (3 + typeLen)) {	// Make sure the whole begin clause will fit
		writeLevelType(level, type);
	}
	_firstValue = true;
	return *this;
}

Logger & Logger::nv(const __FlashStringHelper *name, char *value) {
	if (!doLog(_nvLevel)) return *this;
	int nameLen = strlen_P((const char *)name);
	int valueLen = strlen(value);
	if (_buffer.capacity() >= (3 + nameLen + valueLen)) {	// Make sure the whole NameValue will fit
		char ch;
		writeNameValue(name, value);
		while ((ch = *value++)) {
			_buffer.enqueue(ch);
		}
	}
	return *this;
}

Logger & Logger::nv(const __FlashStringHelper *name, int value) {
	char buf[10];
	return nv(name, itoa(value, buf, 10));
}

Logger & Logger::nv(const __FlashStringHelper *name, double number) {
	  char buf[20];
	  return nv(name, dtostrf(number, 4, 7, buf));
}

Logger & Logger::nv(const __FlashStringHelper *name, float value) {
	return nv(name, (double)value);
}

Logger & Logger::end(void) {
	if (doLog(_nvLevel))
		_buffer.enqueue('\n');
	_nvLevel = LEVEL_NONE;
	return *this;
}

void   Logger::flush(void) {

}
