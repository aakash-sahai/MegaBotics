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
 * Logger.h
 *
 *  Created on: Apr 7, 2015
 *      Author: Aakash Sahai
 */

#ifndef MEGABOTICS_LOGGER_H_
#define MEGABOTICS_LOGGER_H_

#include <string.h>
#include <stdlib.h>
#include "CircularBuffer.h"
#ifdef ARDUINO
#include <avr/pgmspace.h>
#include "MegaBotics.h"
#else
#include <iostream>
#include <stdio.h>
using namespace std;
#define __FlashStringHelper	char
#define	PGM_P	const char *
#define pgm_read_byte(c)	(*c)
#define	WiFi	char
#define F(s)	s
#define strlen_P	strlen
char * itoa(int value, char *buf, int radix);
char * dtostrf(double value, int x, int y, char * buf);
#endif

/*
 * Logger for logging data and status. This logger maintains a circular buffer to
 * store the logged data which should be flushed periodically to one or more destinations
 * periodically by calling the private _flush routine.
 */

#define LOGGER_DEFAULT_BUFSIZE	256
#define LOGGER_MAX_DESTINATIONS	3

#define MAX_IPADDR_LEN	15

class Logger {
public:

	enum Level {
		LEVEL_NONE = 0,
		LEVEL_CRITICAL = 1,
		LEVEL_ERROR = 2,
		LEVEL_WARN = 3,
		LEVEL_INFO = 4,
		LEVEL_DEBUG = 5,
		LEVEL_MAX = 5
	};

	enum Destination {
		LOG_SERIAL = 0,
		LOG_SD = 1,
		LOG_UDP = 2
	};

	struct Config {
		int	bufsize;				// Buffer size: should preferably be a power of 2
		char ip[MAX_IPADDR_LEN+1];	// IP Address to send the log to
		unsigned int port;			// UDP port to send the log to
		const char * fileName;		// Name of the file to write the logs on the SD card
		bool logSerial;
		bool logSD;
		bool logUDP;
		bool autoFlush;

		Config() {
			bufsize = LOGGER_DEFAULT_BUFSIZE;
			port = -1;
			fileName = "LOG.TXT";
			logSerial = false;
			logSD = false;
			logUDP = false;
			autoFlush = false;
		}
	};

	static Logger * getInstance() { return & _instance; }
	static Logger & getReference() { return _instance; }

	virtual ~Logger();
	void setup(void);
	void setup(Config &config);
	void close();
	void open();

	void enable(Destination dest) { _destMask |= ( 1 << dest); }		// Enable logging to a destination
	void disable(Destination dest) { _destMask &= ~( 1 << dest); }		// Disable logging to a destination
	void autoFlush(bool val) { _config.autoFlush = val; }
	void setLevel(Destination dest, Level level)  {
		_level[dest] = level;
		_logLevel = LEVEL_NONE;
		for (int i = 0; i < LOGGER_MAX_DESTINATIONS; i++) {
			if (_level[dest] > _logLevel) _logLevel = _level[dest];
		}
	}

	Level getLevel(Destination dest) { return _level[dest]; }

	bool start(const char *openMessge);
	bool finish(const char *closeMessge);

	bool log(Level level, const __FlashStringHelper *type, const char *message);
	void debug(const __FlashStringHelper *type, const char *message) { log(LEVEL_DEBUG, type, message); }
	void info(const __FlashStringHelper *type, const char *message) { log(LEVEL_INFO, type, message); }
	void warn(const __FlashStringHelper *type, const char *message) { log(LEVEL_WARN, type, message); }
	void error(const __FlashStringHelper *type, const char *message) { log(LEVEL_ERROR, type, message); }
	void critical(const __FlashStringHelper *type, const char *message) { log(LEVEL_CRITICAL, type, message); }

	Logger & begin(Level level, const __FlashStringHelper *type);
	Logger & nv(const __FlashStringHelper *name, const char *value);
	Logger & nv(const __FlashStringHelper *name, char *value);
	Logger & nv(const __FlashStringHelper *name, unsigned int value);
	Logger & nv(const __FlashStringHelper *name, int value);
	Logger & nv(const __FlashStringHelper *name, double value);
	Logger & nv(const __FlashStringHelper *name, float value);
	Logger & nv(const __FlashStringHelper *name, char value);
	Logger & endln(void);
	Logger & space(void);

	void   flush(void);		// Blocking call to force flush the data to all destinations
							// Should preferably be called periodically at low priority to flush the buffer

private:
	static Logger _instance;
#ifdef LOG_WIFI
	WiFi *_wifi;
	byte _wifiConnectionId;
#endif
	File _file;
	char _destMask;		// Bit-field of destinations to send the log to
	Level _level[LOGGER_MAX_DESTINATIONS];
	Level _logLevel;
	Level _nvLevel;
	bool _firstValue;
	Config _config;
	CircularBuffer<char> _buffer;		// The circular buffer used to store the logs
	void writeLevelType(Level level, const __FlashStringHelper *type);
	void writeNameValue(const __FlashStringHelper *name, char *value);
	void writeTime(Level level);
	bool doLog(Level level) { return (_destMask != 0 && level <= _logLevel); }

	Logger();
	Logger(Logger const&);              // Don't Implement to disallow copy by assignment
    void operator=(Logger const&);		// Don't implement to disallow copy by assignment
};

#endif /* MEGABOTICS_LOGGER_H_ */
