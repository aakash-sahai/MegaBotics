/*
 * GPS.cpp
 *
 *  Created on: May 25, 2015
 *      Author: sraj
 */

#include <GPS.h>
#include <HardwareSerial.h>

GPS GPS::_instance;

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
}

void GPS::collect() {
	serialEventRun();
	while (_uport.serial().available()) {
		int ch = _uport.serial().read();
		_gps.encode(ch);
		//if (_gps.encode(ch))
		//	break;
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
