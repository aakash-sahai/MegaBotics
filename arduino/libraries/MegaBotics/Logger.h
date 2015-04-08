/*
 * Logger.h
 *
 *  Created on: Apr 7, 2015
 *      Author: aakash
 */

#ifndef MEGABOTICS_LOGGER_H_
#define MEGABOTICS_LOGGER_H_

#include <Arduino.h>
#include <CircularBuffer.h>
#include <MegaBotics.h>

/*
 * Logger for logging data and status. This logger maintains a circular buffer to
 * store the logged data which should be flushed periodically to one or more destinations
 * periodically by calling the private _flush routine.
 */

#define LOGGER_DEFAULT_BUFSIZE	64
#define LOGGER_MAX_DESTINATIONS	3

class Logger {
public:

	enum Level {
		LEVEL_NONE = 0,
		LEVEL_CRITICAL = 1,
		LEVEL_ERROR = 2,
		LEVEL_WARN = 3,
		LEVEL_INFO = 4,
		LEVEL_DEBUG = 5
	};

	enum Destination {
		LOG_SERIAL = 0,
		LOG_SD = 1,
		LOG_TCP = 2
	};

	struct Config {
		int	bufsize;	// should preferably be a power of 2
		WiFi *wifi;		// Reference to WiFi object
	};

	static Logger & instance() { static Logger instance; return instance; }

	virtual ~Logger();
	void setup(Config &config);

	void enable(Destination dest) { _destMask |= ( 1 << dest); }		// Enable logging to a destination
	void disable(Destination dest) { _destMask &= ~( 1 << dest); }		// Disable logging to a destination
	void setLevel(Destination dest, Level level)  {
		_level[dest] = level;
		_maxLevel = LEVEL_NONE;
		for (int i = 0; i < LOGGER_MAX_DESTINATIONS; i++) {
			if (_level[dest] > _maxLevel) _maxLevel = _level[dest];
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
	Logger & nv(const __FlashStringHelper *name, char *value);
	Logger & nv(const __FlashStringHelper *name, int value);
	Logger & nv(const __FlashStringHelper *name, double value);
	Logger & nv(const __FlashStringHelper *name, float value);
	Logger & end(void);

	void   flush(void);		// Blocking call to force flush the data to all destinations
							// Should preferably be called periodically at low priority to flush the buffer

private:
	char _destMask;	// Bit-field of destinations to send the log to
	Level _level[LOGGER_MAX_DESTINATIONS];
	Level _maxLevel;
	Level _nvLevel;
	bool _firstValue;
	Config _config;
	CircularBuffer<char> _buffer;		// The circular buffer used to store the logs
	Logger() { _destMask = 0; _nvLevel = _maxLevel = LEVEL_NONE; _firstValue = false; _config.bufsize = 0; _config.wifi = 0; }
	void writeLevelType(Level level, const __FlashStringHelper *type);
	void writeNameValue(const __FlashStringHelper *name, char *value);
	bool doLog(Level level) { return (_destMask != 0 && level <= _maxLevel); }
	Logger(Logger const&);              // Don't Implement to disallow copy by assignment
    void operator=(Logger const&);		// Don't implement to disallow copy by assignment
};

#endif /* MEGABOTICS_LOGGER_H_ */
