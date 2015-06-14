/*
 * FUI.cpp
 *
 *  Created on: May 31, 2015
 *      Author: aakash
 */

#include <examples/GPSRover/GPSRover.h>
#include "ConfigManager.h"

GPSRover GPSRover::_instance;

GPSRover::GPSRover() {
	_rover = Rover::getInstance();
	_logger = Logger::getInstance();
	_ahrs = new AHRS();
	_display = Display::getInstance();
	_estore = EepromStore::getInstance();
	_joystick = JoyStick::getInstance();
	_route = Route::getInstance();
	_configMgr = ConfigManager::getInstance();
	_steeringPid = NULL;
	_throttleMin = 5;
	_throttleMax = 100;
	_throttleTurn = 10;
	_cruiseDistance = 10;
	_proximRadius = 3;

	_steer = 0.0f;
	_throttle = 0.0f;
}

void GPSRover::setup() {
	setup(_config);
}

void GPSRover::setup(GPSRover::Config& config) {
	_config = config;
	_estore->setup();
	_ahrs->setup();
	_ahrs->resetIMU();
	_display->setup();
	_configMgr->setup();
	_rover->setup();
	_rover->setControlMode(Rover::MANUAL);
	_steeringPid = new PID(_configMgr->steeringPidConfig);
	_route->setup();
	_throttleMin = _configMgr->throttleConfig.minimum;
	_throttleMax = _configMgr->throttleConfig.maximum;
	_throttleTurn = _configMgr->throttleConfig.turn;
	_cruiseDistance = _configMgr->throttleConfig.cruiseDistance;
	_proximRadius = _configMgr->throttleConfig.proximRadius;
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

		if (_config.enableLogger) {
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

	if (_config.enableLogger) {
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
	int throttle = (loc.distance * _throttleMax ) / _cruiseDistance;

	if (fabs(steer) > 45.0) {
		throttle = _throttleTurn;
	} else if (throttle > _throttleMax) {
		throttle = _throttleMax;
	}

	if (throttle < _throttleMin) {
		throttle = _throttleMin;
	}

	if (_config.enableLogger) {
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


void GPSRover::waitForClick() {
	_joystick->clear();
	_display->clearScr();
	_ahrs->resetIMU();
	_ahrs->resetYPR();
	while (!_joystick->waitForPush(10)) {
		_route->updateLocation(0);
		_display->reset();
		_display->print("YAW: ");_display->println(_ahrs->getOrientation());
		_route->updateLocation(0);
		_route->display();
		_display->println(F("Press the button to start"));
		_route->updateLocation(0);
	}
	_display->clearScr();
}

GPSRover::~GPSRover() {
	delete _ahrs;
	delete _steeringPid;
}

