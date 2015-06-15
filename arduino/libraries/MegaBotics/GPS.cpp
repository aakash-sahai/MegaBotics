/*
 * GPS.cpp
 *
 *  Created on: May 25, 2015
 *      Author: sraj
 */

#include <GPS.h>
#include <HardwareSerial.h>

GPS GPS::_instance;

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
	GPS::getInstance()->doCollect();
}

GPS::GPS() {
}

GPS::GPS(byte port) {
	_config.port = port;
}

GPS::GPS(byte port, long baud) {
	_config.port = port;
	_config.baud = baud;
}

GPS::~GPS() {
}

void GPS::setup() {
	setup(_config);
}

void GPS::setup(Config &config) {
	Logger::getInstance()->begin(Logger::LEVEL_DEBUG, F("GPS-CONFIG")) //
			.nv(F(" Baud"), (int)config.baud) //
			.nv(F(" Port"), config.port) //
			.nv(F(" InterruptRead"), config.interruptRead) //
			.endln().flush();

	_config = config;
	_uport = UPort(_config.port);
	_uport.serial().begin(_config.baud);

	// uBlox commands to disable all the sentences except the RMC
	_uport.serial().println("$PUBX,40,GLL,0,0,0,0*5C");
	_uport.serial().println("$PUBX,40,GGA,0,0,0,0*5A");
	_uport.serial().println("$PUBX,40,GSA,0,0,0,0*4E");
	_uport.serial().println("$PUBX,40,GSV,0,0,0,0*59");
	_uport.serial().println("$PUBX,40,VTG,0,0,0,0*5E");
	_uport.serial().flush();

	if (_config.interruptRead != 0) {
		// Timer0 is already used for millis() - we'll just interrupt somewhere
		// in the middle and call the "Compare A" function above
		OCR0A = 0xAF;
		TIMSK0 |= _BV(OCIE0A);
	}
}

void GPS::collect() {
	if (_config.interruptRead)
		return;
	doCollect();
}

void GPS::doCollect(void) {
	serialEventRun();
	while (_uport.serial().available()) {
		int ch = _uport.serial().read();
		_gps.encode(ch);
		serialEventRun();
	}
}

double GPS::getLatitude() {
	return _gps.location.lat();
}

double GPS::getLongitude() {
	return _gps.location.lng();
}

float GPS::getSpeed() {
	return _gps.speed.mps();
}

float GPS::getHeading() {
	return _gps.course.deg();
}

void GPS::display() {
	Display & display = Display::getReference();

	display.print("LAT: ");display.println(getLatitude(), 8);
	display.print("LONG: ");display.println(getLongitude(), 8);
	display.print("SPEED: ");display.println(getSpeed());
	display.print("HDG: ");display.println(getHeading());
	display.print("SATS: ");display.println(getRawGps().satellites.value());
	display.print("AGE: ");display.println(getRawGps().location.age());
}
