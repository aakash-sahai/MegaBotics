/*
 * SmartRover.cpp
 *
 *  Created on: May 6, 2015
 *      Author: Srinivas Raj
 */

#include "SmartRover.h"
#include <Arduino.h>

SmartRover SmartRover::_instance;

SmartRover::SmartRover() {
	_estore = EepromStore::getInstance();
	_encoder = WheelEncoder::getInstance();
	_rover = Rover::getInstance();
	_logger = Logger::getInstance();
	_ahrs = new AHRS(); //TODO: implement getInstance() in AHRS
	_steeringPid = NULL;

	_waypointQty = 0;
	_currentWaypoint = -1;
	_distanceTraveled = 0.0f;
	_steer = 0.0f;
	_throttle = 0.0f;
	_hdg = 0.0f;
	_distance = 0.0f;
}

SmartRover::~SmartRover() {
	delete _ahrs;
	delete _steeringPid;
}

void SmartRover::setup() {
	setup(_config);
}

void SmartRover::setup(SmartRover::Config& config) {
	_config = config;

	_estore->setup();
	_encoder->setup();
	_ahrs->setup();
	_ahrs->resetIMU();

	_rover->setup();
	_rover->setControlMode(Rover::MANUAL);
	_ahrs->resetIMU();

	_steeringPid = new PID(_config.steerKp, _config.steerKi, _config.steerKd, _config.steerClamp);
}

void SmartRover::addWaypoint(float distance, float hdg) {
	addWaypoint(distance, hdg, _config.throttleMax, _config.wpProximRadius);
}

void SmartRover::addWaypoint(float distance, float hdg, int8_t maxThrottle) {
	addWaypoint(distance, hdg, maxThrottle, _config.wpProximRadius);
}

void SmartRover::addWaypoint(float distance, float hdg, int8_t maxThrottle, float proximRadius) {
	_waypoints[_waypointQty].distance = distance;
	_waypoints[_waypointQty].hdg = hdg;
	_waypoints[_waypointQty].maxThrottle = maxThrottle;
	_waypoints[_waypointQty].proximRadius = proximRadius;
	_waypointQty++;
}

void SmartRover::autoRun() {
	_rover->setControlMode(Rover::AUTO);
	_encoder->reset();
	_ahrs->resetYPR();

	while(nextWaypoint()) {
		float pr = _waypoints[_currentWaypoint].proximRadius;
		while (fabs(_distance) > pr) {
			doNavigate();
			_rover->steer((int8_t)_steer);
			_rover->throttle((int8_t)_throttle);
			delay(_config.navLoopDelay);
		}
		_steeringPid->resetIntegrator();
	}
	_rover->stop();
	_rover->straight();
	_rover->setControlMode(Rover::MANUAL);
}

void SmartRover::doNavigate() {
	float d = _encoder->getDistance();
	_encoder->reset();
	_distanceTraveled += d;

	float currentHdg = DEG2RAD(_ahrs->getOrientation());
	float theta = currentHdg - _hdg;

	float dsin = (d * sin (theta));
	float dcos = (d * cos (theta));
	float phi = atan2(dsin, (_distance - dcos));
	float sinphi = sin(phi);

	if (sinphi != 0.0) {
		_distance = dsin / sinphi;
	} else {
		_distance -= d;
	}

	float error = normalizeAngle(-(theta + phi));
	_hdg = currentHdg + error;

	_steer = clampSteering(_steeringPid->getPid(error, _config.steerScale));
	_throttle = clampThrottle();

	_logger->begin(Logger::LEVEL_DEBUG, F("RUN")).
					nv(F("  TOTALDIST"), _distanceTraveled).
					nv(F("  DIST"), _distance).
					nv(F("  DCOS"), dcos).
					nv(F("  DSIN"), dsin).
					nv(F("  CURHDG "), RAD2DEG(currentHdg)).
					nv(F("  HDG "), RAD2DEG(_hdg)).
					nv(F("  ERROR "), RAD2DEG(error)).
					nv(F("  STEER "), (int)_steer).endln();
}

float SmartRover::clampSteering(float val) {
	if (val > 100.0)
		return 100.0;
	if (val < -100.0)
		return -100.0;
	return val;
}
float SmartRover::clampThrottle() {

	int8_t maxThrottle = _waypoints[_currentWaypoint].maxThrottle;
	float throttle = _distance * maxThrottle / _config.curiseDistThres;

	if (throttle > maxThrottle) {
		throttle = maxThrottle;
	}

	if (throttle < _config.throttleMin) {
		throttle = _config.throttleMin;
	}

	return throttle;
}

bool SmartRover::nextWaypoint() {
	_currentWaypoint++;
	if (_currentWaypoint >= _waypointQty) {
		_distance = 0;
		_hdg = 0;
		return false;
	} else {
		_distance = _waypoints[_currentWaypoint].distance;
		_hdg = DEG2RAD(_waypoints[_currentWaypoint].hdg);
		return true;
	}
}

float SmartRover::normalizeAngle(float angle)
{
	if (angle < (-1 * PI)) {
		angle += (2 * PI);
	} else if (angle > PI) {
		angle -= (2 * PI);
	}
	return angle;
}
