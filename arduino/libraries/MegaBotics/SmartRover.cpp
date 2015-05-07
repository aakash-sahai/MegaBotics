/*
 * SmartRover.cpp
 *
 *  Created on: May 6, 2015
 *      Author: sraj
 */

#include <SmartRover.h>

SmartRover::SmartRover() {
	_estore = EepromStore::getInstance();
	_encoder = WheelEncoder::getInstance();
	_rover = Rover::getInstance();
	_ahrs = new AHRS(); //TODO: implement getInstance() in AHRS
	_steeringPid = NULL;

	_waypointQty = 0;
	_currentWaypoint = 0;
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

	_rover->setup();
	_rover->setControlMode(Rover::MANUAL);

	_config = config;
	_steeringPid = new PID(_config.steerKp, _config.steerKi, _config.steerKd, _config.steerClamp);
}

void SmartRover::addWaypoint(float distance, float hdg) {
	_waypoints[_waypointQty].distance = distance;
	_waypoints[_waypointQty].hdg = hdg;
	_waypointQty++;
}

void SmartRover::autoRun() {
	while(_currentWaypoint < _waypointQty) {
		serialEventRun();

		if (_distance < _config.wpProximRadius) {
			_currentWaypoint++;

			if (_currentWaypoint > _waypointQty) {
				_rover->stop();
				break;
			}

			nextWaypoint();
		}

		doNavigate();

		_rover->steer(_steer);
		_rover->throttle(_throttle);

		delay(_config.navLoopDelay);
	}
}

void SmartRover::doNavigate() {
	float d = _encoder->getDistance();
	_distanceTraveled += d;
	_encoder->reset();

	float currentHdg = DEG2RAD(_ahrs->getOrientation());
	float theta = currentHdg - _hdg;

	float dsin = fabs(d * sin (theta));
	float dcos = fabs(d * cos (theta));
	float phi = atan2(dsin, (_distance - dcos));
	float sinphi = sin(phi);

	if (sinphi != 0.0) {
		_distance = dsin / sinphi;
	} else {
		_distance -= d;
	}

	float error = -(theta + phi);
	_hdg = currentHdg + error;

	_steer = (int8_t)_steeringPid->getPid(error, _config.steerScale);
	_throttle = clampThrottle(_distance * _config.throttleMax / _config.curiseDistThres);
}

float SmartRover::clampThrottle(float throttle) {
	if (throttle > _config.throttleMax) {
		throttle = _config.throttleMax;
	}

	if (throttle < _config.throttleMin) {
		throttle = _config.throttleMin;
	}

	return throttle;
}

void SmartRover::nextWaypoint() {
	_encoder->reset();
	_ahrs->resetYPR();
	_steeringPid->resetIntegrator();

	_distance = _waypoints[_currentWaypoint-1].distance;
	_hdg = DEG2RAD(_waypoints[_currentWaypoint-1].hdg);
}
