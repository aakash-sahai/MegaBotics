/*
 * Waypoints.cpp
 *
 *  Created on: May 29, 2015
 *      Author: sraj
 */

#include "Waypoints.h"
#include "SD.h"
#include "Utils.h"
#include "JoyStick.h"
#include <Arduino.h>

#define GPS_PRECISION_DIGITS 16

Waypoints Waypoints::_instance;

Waypoints::Waypoints() {
	_gps = GPS::getInstance();
	_display = Display::getInstance();
	_logger = Logger::getInstance();
	_joyStick = JoyStick::getInstance();
}

Waypoints::~Waypoints() {
}

int Waypoints::configWaypoints() {
	// close the logger because we can have only one file open at a time
	_logger->close();

	SDCard::getReference().setup();
	SDCard::getReference().remove(DEFAULT_WAYPOINT_FILE);
	File file = SDCard::getReference().open(DEFAULT_WAYPOINT_FILE, FILE_WRITE);
	if (!file) {
		_display->println("Failed to open waypoints file");
		Utils::waitForEver();
	}

	int count = 0;
	Geo2D prevWp;

	_display->clearScr();

	while (true) {
		_display->reset();
		_display->print("LAT: ");_display->println(_gps->getLatitude(), 8);
		_display->print("LONG: ");_display->println(_gps->getLongitude(), 8);
		_display->print("SPEED: ");_display->println(_gps->getSpeed());
		_display->print("HDG: ");_display->println(_gps->getHeading());
		_display->print("SATS: ");_display->println(_gps->getRawGps().satellites.value());
		_display->print("AGE: ");_display->println(_gps->getRawGps().location.age());

		float distance = TinyGPSPlus::distanceBetween(_gps->getLatitude(), _gps->getLongitude(), prevWp.getLatitude(), prevWp.getLongitude());
		float course = TinyGPSPlus::courseTo(_gps->getLatitude(), _gps->getLongitude(), prevWp.getLatitude(), prevWp.getLongitude());

		if (prevWp.getLatitude() > 0.0) {
			_display->print("PREV WP DIST: ");_display->println(distance);
			_display->print("PREV WP HDG: ");_display->println(course);
		} else {
			_display->println("PREV WP DIST: NA");
			_display->println("PREV WP HDG: NA");
		}

		_display->println();
		_display->println("Add waypoints");
		_display->println("CENTER: add");
		_display->println("LEFT: exit");

		JoyStick::Input input = _joyStick->waitForInput(1000);

		if (input == JoyStick::LEFT) {
			break;
		} else if (input == JoyStick::CENTER) {
			file.print(_gps->getLatitude(), GPS_PRECISION_DIGITS);
			file.print(" ");
			file.println(_gps->getLongitude(), GPS_PRECISION_DIGITS);
			file.flush();

			count++;
			_display->print("Added way point# ");_display->println(count);

			prevWp.setLatitude(_gps->getLatitude());
			prevWp.setLongitude(_gps->getLongitude());
			delay(2000);
		}
	}

	// close the waypoints file and open the logger
	file.close();
	_logger->open();

	return count;
}

void Waypoints::open() {
	// close the logger because we can have only one file open at a time
	_logger->close();

	_file = SDCard::getReference().open(DEFAULT_WAYPOINT_FILE, FILE_READ);

	if (!_file) {
		_display->println("Failed to open waypoints file");
		Utils::waitForEver();
	}
}

void Waypoints::close() {
	if (_file) {
		_file.close();
	}

	_logger->open();
}

bool Waypoints::nextWaypoint(Geo2D& waypoint) {
	if (_file.position() < _file.size()) {
		char *fcurr;
		const char *curr;

		String line = _file.readStringUntil('\n');
		curr = line.c_str();

		if (line.length() > 0) {
			waypoint.setLatitude(strtod(curr, &fcurr));
			curr = fcurr;
			waypoint.setLongitude(strtod(curr, &fcurr));
			return true;
		} else {
			return false;
		}
	}

	return false;
}
