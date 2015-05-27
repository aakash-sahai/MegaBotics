/*
 * GPS.cpp
 *
 *  Created on: May 25, 2015
 *      Author: sraj
 */

#include <MegaBotics.h>

GPS GPS::_instance;

GPS::GPS() {
	_uport = UPort(GPS_DEFAULT_UPORT);
}

GPS::GPS(int port) {
	_uport = UPort(port);
}

GPS::~GPS() {
}

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
	GPS::getInstance()->collect();
}

void GPS::setup() {
	setup(_config);
}

void GPS::setup(Config &config) {
	_config = config;

	_uport.serial().begin(_config.baud);

	// Timer0 is already used for millis() - we'll just interrupt somewhere
	// in the middle and call the "Compare A" function above
	OCR0A = 0xAF;
	TIMSK0 |= _BV(OCIE0A);
}

void GPS::collect() {
	while (_uport.serial().available()) {
	  _gps.encode(_uport.serial().read());
	}
}

double GPS::getLatitude() {
	return _gps.location.lat();
}

double GPS::getLongitude() {
	return _gps.location.lng();
}

float GPS::getSpeed() {
	return _gps.speed.mph();
}

float GPS::getHeading() {
	return _gps.course.deg();
}
