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

//	_uport.serial().println("$PSRF103,00,00,00,01*24\r\n");	// Turn off GGA msg
//	_uport.serial().flush();
//	_uport.serial().println("$PSRF103,01,00,00,01*25\r\n");	// Turn off GLL msg
//	_uport.serial().flush();
//	_uport.serial().println("$PSRF103,02,00,00,01*26\r\n");	// Turn off GSA msg
//	_uport.serial().flush();
//	_uport.serial().println("$PSRF103,03,00,00,01*27\r\n");	// Turn off GSV msg
//	_uport.serial().flush();
//	_uport.serial().println("$PSRF103,04,00,FF,01*20\r\n");	// Send RMC msg @ max rate
//	_uport.serial().flush();
//	_uport.serial().println("$PSRF103,05,00,00,01*21\r\n");	// Turn off VTG msg
	_uport.serial().flush();
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
