/*
 * GPS.cpp
 *
 *  Created on: May 25, 2015
 *      Author: sraj
 */

#include <GPS.h>

GPS GPS::_instance;

GPS::GPS() {
	_uport = UPort(GPS_DEFAULT_UPORT);
}

GPS::GPS(int port) {
	_uport = UPort(port);
}

GPS::~GPS() {
}

void GPS::setup() {
	setup(_config);
}

void GPS::setup(Config &config) {
	_config = config;

	_uport.serial().begin(_config.baud);

	// Disable all the sentences except the RMC
	_uport.serial().println("$PUBX,40,GLL,0,0,0,0*5C");
	_uport.serial().println("$PUBX,40,GGA,0,0,0,0*5A");
	_uport.serial().println("$PUBX,40,GSA,0,0,0,0*4E");
	_uport.serial().println("$PUBX,40,GSV,0,0,0,0*59");
	_uport.serial().println("$PUBX,40,VTG,0,0,0,0*5E");
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
