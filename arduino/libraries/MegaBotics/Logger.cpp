/*
 * This file is part of the support libraries for the Arduino based MegaBotics
 * platform.
 *
 * Copyright (c) 2015, Aakash Sahai and other contributors, a list of which is
 * maintained in the associated repository. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    + Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    + Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    + Neither the name of the copyright holder nor the names of its contributors
 *	may be used to endorse or promote products derived from this software
 *	without specific prior written permission.
 */

/*
 * Logger.cpp
 *
 *  Created on: Apr 7, 2015
 *      Author: Aakash Sahai
 */

#include "MegaBotics.h"

Logger Logger::_instance;

Logger::Logger() {
	_destMask = 0;
	_nvLevel = _logLevel = LEVEL_NONE;
	_firstValue = false;
	_config.bufsize = LOGGER_DEFAULT_BUFSIZE;
	strcpy(_config.ip, "");
#ifdef LOG_WIFI
	_config.port = 0;
	_wifiConnectionId = -1;
	_wifi = WiFi::getInstance();
#endif
}


Logger::~Logger() {
}

void Logger::setup(void) {
	_buffer.setup(_config.bufsize);
	if (_config.logSerial)
		enable(Logger::LOG_SERIAL);
	if (_config.logSD)
		enable(Logger::LOG_SD);
	if (_config.logUDP)
		enable(Logger::LOG_UDP);
}

void Logger::setup(Config &aConfig) {
	_config.bufsize = aConfig.bufsize;
#ifdef LOG_WIFI
	strncpy(_config.ip, aConfig.ip, MAX_IPADDR_LEN);
	_config.ip[MAX_IPADDR_LEN] = 0;
	_config.port = aConfig.port;
	if (aConfig.port != 0) {
		_wifi->connect(UDP, aConfig.ip, aConfig.port, _wifiConnectionId);
	}
#endif
	_buffer.setup(_config.bufsize);
	_config.fileName = aConfig.fileName;

	open();
}

void Logger::close() {
	if (_file) {
		_file.close();
		_file == NULL;
	};
}

void Logger::open() {
	if (_config.fileName != NULL && _file == NULL) {
		SDCard::getReference().setup();
		_file = SDCard::getReference().open(_config.fileName, FILE_WRITE);
	}
}

bool Logger::start(const char *message) {
	return log(LEVEL_CRITICAL, F("OPEN"), message);
}

bool Logger::finish(const char *message) {
	return log(LEVEL_CRITICAL, F("CLOSE"), message);
}

void Logger::writeTime(Level level) {
	if (!doLog(level)) return;

	char buf[34];
	char ch;
	char *value = ltoa(millis(), buf, 10);
	_buffer.enqueue('[');
	while ((ch = *value++)) {
		_buffer.enqueue(ch);
	}
	_buffer.enqueue(']');
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
	if (_buffer.capacity() < (38 + typeLen + msgLen)) { // 4 + 34 (for millis) + typeLen + msgLen, Make sure the whole log will fit
		if (_config.autoFlush) {
			flush();
		} else {
			return false;
		}
	}
	char ch;
	writeTime(level);
	writeLevelType(level, type);
	while ((ch = *message++)) {
		_buffer.enqueue(ch);
	}
	_buffer.enqueue('\r');
	_buffer.enqueue('\n');
	return true;
}

Logger & Logger::begin(Level level, const __FlashStringHelper *type) {
	_nvLevel = level;
	if (!doLog(level)) return *this;
	int typeLen = strlen_P((const char *)type);
	if (_buffer.capacity() < (37 + typeLen)) {	// 3 + 34 (for millis) + typeLen, Make sure the whole begin clause will fit
		if (_config.autoFlush) {
			flush();
		} else {
			return *this;
		}
	}
	writeTime(level);
	writeLevelType(level, type);
	_firstValue = true;
	return *this;
}

Logger & Logger::nv(const __FlashStringHelper *name, char *value) {
	if (!doLog(_nvLevel)) return *this;
	int nameLen = strlen_P((const char *)name);
	int valueLen = strlen(value);
	if (_buffer.capacity() < (3 + nameLen + valueLen)) {	// Make sure the whole NameValue will fit
		if (_config.autoFlush) {
			flush();
		} else {
			return *this;
		}
	}
	writeNameValue(name, value);
	return *this;
}

Logger & Logger::nv(const __FlashStringHelper *name, const char *value) {
	return nv(name, (char *)value);
}

Logger & Logger::nv(const __FlashStringHelper *name, int value) {
	char buf[10];
	return nv(name, itoa(value, buf, 10));
}

Logger & Logger::nv(const __FlashStringHelper *name, unsigned int value) {
	char buf[10];
	return nv(name, utoa(value, buf, 10));
}

Logger & Logger::nv(const __FlashStringHelper *name, double number) {
	  char buf[20];
	  return nv(name, dtostrf(number, 4, 7, buf));
}

Logger & Logger::nv(const __FlashStringHelper *name, float value) {
	return nv(name, (double)value);
}

Logger & Logger::endln(void) {
	if (doLog(_nvLevel)) {
		_buffer.enqueue('\r');
		_buffer.enqueue('\n');
	}
	_nvLevel = LEVEL_NONE;
	return *this;
}

Logger & Logger::nv(const __FlashStringHelper *name, char value) {
	if (doLog(_nvLevel))
		_buffer.enqueue(value);
	return *this;
}

Logger & Logger::space(void) {
	if (doLog(_nvLevel))
		_buffer.enqueue(' ');
	_nvLevel = LEVEL_NONE;
	return *this;
}

void   Logger::flush(void) {
	char ch, level = -1;

	while (_buffer.dequeue(&ch)) {
		if (ch <= LEVEL_MAX) {		// This must be the level
			level = ch;
			continue;
		}
		if ((_destMask & (1 << LOG_SERIAL)) && level <= _level[LOG_SERIAL])	{
#ifdef ARDUINO
			Serial.write(ch);
#else
			cout << ch;
#endif
		}
#ifdef LOG_WIFI
		if (_wifiConnectionId != -1 && (_destMask & (1 << LOG_UDP)) && level <= _level[LOG_UDP])	{
			_wifi->write(_wifiConnectionId, ch);
		}
#endif
		if (_file && (_destMask & (1 << LOG_SD)) && level <= _level[LOG_SD])	{
			_file.write(ch);
		}
	}

	if (_file) {
		_file.flush();
	}
}
