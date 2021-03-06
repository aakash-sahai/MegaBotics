/*
 * Route.cpp
 *
 *  Created on: May 29, 2015
 *      Author: sraj
 */

#include "Route.h"
#include "SD.h"
#include "Utils.h"
#include "JoyStick.h"
#include "ConfigManager.h"
#include <Arduino.h>

#define GPS_PRECISION_DIGITS 16

Route Route::_instance;

/*
 * Assumes that the Display, Logger and Joystick have already been setup
 * since they are shared by other modules. However, setting up the GPS
 * module is Route's responsibility.
 */

Route::Route() {
	_gps = GPS::getInstance();
	_display = Display::getInstance();
	_logger = Logger::getInstance();
	_joyStick = JoyStick::getInstance();
	_waypointQty = 0;
	_currentWaypoint = -1;
}

Route::~Route() {
}

void Route::setup(void) {
	ConfigManager & cm = ConfigManager::getReference();
	_gps->setup(cm.gpsConfig);
	waitForGpsFix();
	// configWaypoints();	// Uncomment to manually configure waypoints
	loadWaypoints();
	logConfig();
}

void Route::setup(Route::Config & config) {
	_config = config;
	setup();
}

void Route::logConfig() {
	_logger->begin(Logger::LEVEL_DEBUG, F("ROUTE-CONFIG")) //
			.nv(F(" ProximRadius"), _config.proximRadius) //
			.nv(F(" RefHeading"), _config.refHeading) //
			.endln().flush();

	for (int i = 0; i < _waypointQty; i++) {
		_logger->begin(Logger::LEVEL_DEBUG, F("WAYPOINT")) //
				.nv(F(" lat"), _waypoints[i].getLatitude()) //
				.nv(F(" lon"), _waypoints[i].getLongitude()) //
				.nv(F(" proxim radius"), _waypoints[i].getProximRadius()) //
				.nv(F(" max throttle"), _waypoints[i].getMaxThrottle()) //
				.endln().flush();
	}
}

void Route::addWaypoint(Waypoint &wp) {
	_waypoints[_waypointQty] = wp;
	_waypointQty++;
}

int Route::nextWaypoint() {
	_currentWaypoint++;
	if (_currentWaypoint >= _waypointQty) {
		_currentLocation.distance = 0;
		_currentLocation.hdg = 0;
		return 0;					// Return value of 0 implies no more waypoints
	} else {
		updateLocation();
		if (_currentWaypoint == 0) {
			if (_config.refHeading == DEFAULT_INVALID_REF_HED) {
				_currentLocation.refHdg = _currentLocation.hdg;
			} else {
				_currentLocation.refHdg = _config.refHeading;
			}
		}
		return _currentWaypoint + 1;	// Returns a 1-based number
	}
}

void Route::waitForGpsFix() {
	_display->println("Acquiring GPS Signal");

	while (!_gps->getRawGps().location.isValid()) {
		_gps->collect();
	}
}

Route::Location & Route::updateLocation() {
	return updateLocation(_currentWaypoint);
}

Route::Location & Route::updateLocation(byte waypoint) {
	_gps->collect();
	_currentLocation.lat = _gps->getLatitude();
	_currentLocation.lon = _gps->getLongitude();
	_currentLocation.age = (unsigned int)_gps->getRawGps().location.age();
	_currentLocation.speed = _gps->getSpeed();
	_currentLocation.curHdg = Utils::normalizeAngle(_gps->getHeading());

	if (_currentWaypoint > -1) {
		Waypoint currentWp = _waypoints[waypoint];
		_currentLocation.distance = TinyGPSPlus::distanceBetween(_currentLocation.lat, _currentLocation.lon, currentWp.getLatitude(), currentWp.getLongitude());
		_currentLocation.hdg = TinyGPSPlus::courseTo(_currentLocation.lat, _currentLocation.lon, currentWp.getLatitude(), currentWp.getLongitude());
		_currentLocation.hdg = Utils::normalizeAngle(_currentLocation.hdg);
	} else {
		_currentLocation.distance = 0.0;
		_currentLocation.hdg = 0.0;
	}
	return _currentLocation;
}

void Route::display() {
	_display->print("DIST from WP: ");_display->println(_currentLocation.distance);
	updateLocation(0);
	_display->print("HDG from WP: ");_display->println(_currentLocation.hdg);
	updateLocation(0);
	_display->print("No of WP: ");_display->println(_waypointQty);
	updateLocation(0);

	_gps->display();
}

int Route::configWaypoints() {
	_display->clearScr();
	_display->println(F("Press the joystick to configure waypoints"));
	if (_joyStick->waitForInput(5000) != JoyStick::CENTER) {
		return 0;
	}
	// close the logger because we can have only one file open at a time
	_logger->close();

	SDCard::getReference().setup();
	SDCard::getReference().remove(DEFAULT_WAYPOINT_FILE);
	File file = SDCard::getReference().open(DEFAULT_WAYPOINT_FILE, FILE_WRITE);
	if (!file) {
		_display->println(F("Failed to open Waypoint file"));
		Utils::waitForEver();
	}

	_waypointQty = 0;
	_currentWaypoint = -1;

	_display->clearScr();
	_display->println("Use joystick to add waypoints");
	_display->println("  CLICK => ADD");
	_display->println("  LEFT => EXIT");
	delay(2000);
	_display->clearScr();

	while (true) {
		_display->reset();
		updateLocation();
		display();
		JoyStick::Input input = _joyStick->waitForInput(10);

		if (input == JoyStick::LEFT) {
			break;
		} else if (input == JoyStick::CENTER) {
			file.print(_gps->getLatitude(), GPS_PRECISION_DIGITS);
			file.print(" ");
			file.println(_gps->getLongitude(), GPS_PRECISION_DIGITS);
			file.flush();
			updateLocation();
			Waypoint wp(Waypoint::LAT_LON, _gps->getLatitude(), _gps->getLongitude());
			addWaypoint(wp);
			_currentWaypoint++;
			_display->print("Added way point# ");_display->println(_waypointQty);
			delay(2000);
		}
	}

	// close the waypoints file and open the logger
	file.close();
	_logger->open();

	_display->clearScr();
	_display->print("Added ");_display->print(_waypointQty);_display->println(" waypoints");
	_display->print("Reset to restart");
	Utils::waitForEver();
	return _waypointQty;
}

void Route::open() {
	// close the logger because we can have only one file open at a time
	_logger->close();

	_file = SDCard::getReference().open(DEFAULT_WAYPOINT_FILE, FILE_READ);

	if (!_file) {
		_display->println(F("Failed to open Waypoint file"));
		Utils::waitForEver();
	}
}

void Route::close() {
	if (_file) {
		_file.close();
	}
	_logger->open();
}

void Route::loadWaypoints() {
	ConfigManager & cm = ConfigManager::getReference();

	open();
	while (_file.position() < _file.size()) {
		char *fcurr;
		const char *curr;

		String line = _file.readStringUntil('\n');
		curr = line.c_str();

		if (line.length() > 0) {
			_waypoints[_waypointQty].setLatitude(strtod(curr, &fcurr));
			curr = fcurr;
			_waypoints[_waypointQty].setLongitude(strtod(curr, &fcurr));
			curr = fcurr;

			double proximRadius = strtod(curr, &fcurr);
			curr = fcurr;
			if (proximRadius != 0) {
				_waypoints[_waypointQty].setProximRadius(proximRadius);
			} else {
				_waypoints[_waypointQty].setProximRadius(_config.proximRadius);
			}

			int8_t maxThrottle = (int8_t)strtol(curr, &fcurr, 10);
			curr = fcurr;
			if (maxThrottle != 0) {
				_waypoints[_waypointQty].setMaxThrottle(maxThrottle);
			} else {
				_waypoints[_waypointQty].setMaxThrottle(cm.throttleConfig.maximum);
			}

			_waypointQty++;
		} else {
			break;
		}
	}
	close();
}
