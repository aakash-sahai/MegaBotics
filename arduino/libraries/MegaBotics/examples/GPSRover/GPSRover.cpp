/*
 * FUI.cpp
 *
 *  Created on: May 31, 2015
 *      Author: aakash
 */

#include <examples/GPSRover/GPSRover.h>
#include "ConfigManager.h"

GPSRover GPSRover::_instance;

GPSRover::GPSRover() : AutonomousRover() {
	_configMgr = ConfigManager::getInstance();
	_steer = 0.0f;
	_throttle = 0.0f;
}

void GPSRover::autoRun() {
	_logger->start("======================== Starting Auto Run ========================");

	_rover->setControlMode(Rover::AUTO);
	_ahrs->resetIMU();
	_ahrs->resetYPR();

	long startTime = millis();
	long noOfRunSamples = 0;
	int wpNumber = -1;

	while ((wpNumber = _route->nextWaypoint()) > 0) {
		while (! _route->reachedNextWaypoint()) {
			noOfRunSamples++;
			doNavigate();
			delay(_config.navLoopDelay);
		}

		_steeringPid->resetIntegrator();

		if (_config.logging) {
			_logger->begin(Logger::LEVEL_DEBUG, F("RUN")) //
					.nv(F("  Reached Waypoint #"), wpNumber) //
					.endln().flush();
		}

		_display->print(F("Reached waypoint #")); _display->println(wpNumber);
	}

	_rover->straight();
	_rover->stop();
	_rover->setControlMode(Rover::MANUAL);

	_display->clearScr();
	_display->println(F("Reached last WP"));
	_display->print(F("# of samples: ")); _display->println(noOfRunSamples);
	_display->print(F("Run duration: ")); _display->println((long)((millis() - startTime)/1000));

	_logger->finish("======================== End of Auto Run ========================");
}


void GPSRover::doNavigate() {
	Route::Location & loc = _route->updateLocation();
	_steer = calcSteering(loc);
	_throttle = calcThrottle(loc, _steer);
	_rover->steer((int8_t) _steer);
	_rover->throttle((int8_t) _throttle);
}

float GPSRover::calcSteering(Route::Location & loc) {
	float orientation = _ahrs->getOrientation();

	float currentHdg = Utils::normalizeAngle(orientation + loc.refHdg);

	float error = Utils::normalizeAngle(loc.hdg - currentHdg);
	float steer = Utils::clamp(_steeringPid->getPid(error, _config.steerScale), -100.0, 100.0);

	if (_config.logging) {
		_logger->begin(Logger::LEVEL_DEBUG, F("RUN-STEER")) //
				.nv(F("  ORIENT"), orientation) //
				.nv(F("  DESIRED HDG"), loc.hdg) //
				.nv(F("  CUR HDG"), currentHdg) //
				.nv(F("  HDG ERROR"), error) //
				.nv(F("  STEER"), steer) //
				.endln().flush();
	}

	return steer;
}

float GPSRover::calcThrottle(Route::Location & loc, float steer) {
	int throttle = (loc.distance * _throttleConfig.maximum ) / _config.cruiseDistance;

	if (fabs(steer) > 10.0) {
		throttle = _throttleConfig.turn;
	} else if (throttle > _throttleConfig.maximum) {
		throttle = _throttleConfig.maximum;
	}

	if (throttle < _throttleConfig.minimum) {
		throttle = _throttleConfig.minimum;
	}

	if (_config.logging) {
		_logger->begin(Logger::LEVEL_DEBUG, F("RUN-THROTTLE")) //
				.nv(F("  LAT"), loc.lat) //
				.nv(F("  LONG"), loc.lon) //
				.nv(F("  AGE"), loc.age) //
				.nv(F("  DISTANCE"), loc.distance) //
				.nv(F("  SPEED"), loc.speed) //
				.nv(F("  THROTTLE"), (int) throttle) //
				.endln().flush();
	}

	return throttle;
}

GPSRover::~GPSRover() {
	delete _ahrs;
	delete _steeringPid;
}

