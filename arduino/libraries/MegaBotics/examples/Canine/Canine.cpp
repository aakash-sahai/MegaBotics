/*
 * Canine.cpp
 *
 *  Created on: May 23, 2015
 *      Author: sraj
 */

#if 0
#include "Canine.h"

Canine Canine::_instance;

Canine::Canine() {
	_estore = EepromStore::getInstance();
	_rover = Rover::getInstance();
	_logger = Logger::getInstance();
	_ahrs = new AHRS(); //TODO: implement getInstance() in AHRS
	_gps = GPS::getInstance();
	_display = Display::getInstance();
	_joyStick = JoyStick::getInstance();
	_wpHelper = Waypoints::getInstance();

	_steeringPid = NULL;

	_waypointQty = 0;
	_currentWaypoint = -1;
	_steer = 0.0f;
	_throttle = 0.0f;
	_hdg = 0.0f;
	_distance = 0.0f;
	_refHdg = 0.0f;
}

Canine::~Canine() {
	delete _ahrs;
	delete _steeringPid;
}

void Canine::setup() {
	setup(_config);
}

void Canine::setup(Canine::Config& config) {
	_config = config;

	_estore->setup();
	_ahrs->setup();
	_ahrs->resetIMU();
	_gps->setup();

	Display::Config dispConfig;
	dispConfig.rotation = 2;
	dispConfig.textSize = 2;
	_display->setup(dispConfig);

	_rover->setup();
	_rover->setControlMode(Rover::MANUAL);

	_steeringPid = new PID(_config.steerKp, _config.steerKi, _config.steerKd, _config.steerClamp);
}

void Canine::autoRun() {
	_logger->start("======================== Starting Auto Run ========================");

	_rover->setControlMode(Rover::AUTO);
	_ahrs->resetYPR();

	long startTime = millis();
	long noOfRunSamples = 0;

	while (nextWaypoint()) {
		float pr = _waypoints[_currentWaypoint].proximRadius;
		update(_currentWaypoint);

		while (_distance > pr) {
			noOfRunSamples++;
			doNavigate();
			delay(_config.navLoopDelay);
		}

		_steeringPid->resetIntegrator();

		if (_config.enableLogger) {
			_logger->begin(Logger::LEVEL_DEBUG, F("RUN")) //
					.nv(F("  Reached Waypoint #"), _currentWaypoint+1) //
					.endln().flush();
		}

		_display->print(F("Reached waypoint #")); _display->println(_currentWaypoint+1); // _currentWaypoint is 0 based
	}

	_rover->straight();
	_rover->stop();
	_rover->setControlMode(Rover::MANUAL);

	_display->clearScr();
	_display->println(F("Reached last WP"));
	_display->print("# samples: "); _display->println(noOfRunSamples);
	_display->print("duration: "); _display->println((long)((millis() - startTime)/1000));

	_logger->finish("======================== End of Auto Run ========================");
}

void Canine::doNavigate() {
	_gps->collect();
	update(_currentWaypoint);

	_steer = calcSteering();
	_throttle = calcThrottle(_steer);

	bool staleGps = false;
	if (_gps->getRawGps().location.age() > _config.gpsMinAge) {
		_rover->throttle(_config.minThrottle);
		staleGps = true;
	} else {
		_rover->steer((int8_t) _steer);
		_rover->throttle((int8_t) _throttle);
	}

	if (staleGps) {
		while (_gps->getRawGps().location.age() > _config.gpsMinAge) {
			_gps->collect();
		}
	}

	if (_config.enableLogger) {
		_logger->begin(Logger::LEVEL_DEBUG, F("RUN-NAV")) //
				.nv(F("  CONTROL"), _rover->getControlMode()) //
				.nv(F("  STALE GPS"), staleGps) //
				.endln().flush();
	}
}

void Canine::update(uint8_t waypoint) {
	Waypoint wp = _waypoints[waypoint];
	_curPos.setLatitude(_gps->getLatitude());
	_curPos.setLongitude(_gps->getLongitude());

	_distance = TinyGPSPlus::distanceBetween(_curPos.getLatitude(), _curPos.getLongitude(), wp.latitude, wp.longitude);
	_hdg = TinyGPSPlus::courseTo(_curPos.getLatitude(), _curPos.getLongitude(), wp.latitude, wp.longitude);
	_hdg = normalizeAngle(_hdg);
}

float Canine::calcSteering() {
	float orientation = normalizeAngle(_ahrs->getOrientation());
	float currentHdg = normalizeAngle(orientation + _refHdg);

	float error = normalizeAngle(_hdg - currentHdg);
	float steer = Utils::clamp(_steeringPid->getPid(error, _config.steerScale), -100.0, 100.0);

	if (_config.enableLogger) {
		_logger->begin(Logger::LEVEL_DEBUG, F("RUN-STEER")) //
				.nv(F("  ORIENT"), orientation) //
				.nv(F("  DESIRED HDG"), _hdg) //
				.nv(F("  CUR HDG"), currentHdg) //
				.nv(F("  GPS HDG"), _gps->getHeading()) //
				.nv(F("  HDG ERROR"), error) //
				.nv(F("  STEER"), steer) //
				.endln().flush();
	}

	return steer;
}

float Canine::calcThrottle(float steer) {
	int8_t maxThrottle = _config.maxThrottle;
	float throttle = _distance * maxThrottle / _config.curiseDistThres;

	if (throttle > maxThrottle) {
		throttle = maxThrottle;
	}

	if (throttle < _config.minThrottle) {
		throttle = _config.minThrottle;
	}

	if (_config.enableLogger) {
		_logger->begin(Logger::LEVEL_DEBUG, F("RUN-THROTTLE")) //
				.nv(F("  LAT"), _curPos.getLatitude()) //
				.nv(F("  LONG"), _curPos.getLongitude()) //
				.nv(F("  AGE"), (unsigned int)_gps->getRawGps().location.age()) //
				.nv(F("  DISTANCE"), _distance) //
				.nv(F("  SPEED"), _gps->getSpeed()) //
				.nv(F("  THROTTLE"), (int) throttle) //
				.endln().flush();
	}

	return throttle;
}

bool Canine::nextWaypoint() {
	_currentWaypoint++;
	if (_currentWaypoint >= _waypointQty) {
		_distance = 0;
		_hdg = 0;
		return false;
	} else {
		if (_currentWaypoint == 0) {
			update(_currentWaypoint);
			_refHdg = _hdg;
		}
		return true;
	}
}

float Canine::normalizeAngle(float angle) {
	if (angle < -180) {
		angle += 360;
	} else if (angle > 180) {
		angle -= 360;
	}
	return angle;
}

float Canine::denormalizeAngle(float angle) {
	if (angle < 0) {
		angle += 360;
	}

	return angle;
}

void Canine::addWaypoint(float latitude, float longitude) {
	addWaypoint(latitude, longitude, _config.maxThrottle, _config.wpProximRadius);
}

void Canine::addWaypoint(float latitude, float longitude, uint8_t maxThrottle) {
	addWaypoint(latitude, longitude, maxThrottle, _config.wpProximRadius);
}

void Canine::addWaypoint(float latitude, float longitude, uint8_t maxThrottle, float proximRadius) {
	_waypoints[_waypointQty].latitude = latitude;
	_waypoints[_waypointQty].longitude = longitude;
	_waypoints[_waypointQty].maxThrottle = maxThrottle;
	_waypoints[_waypointQty].proximRadius = proximRadius;
	_waypointQty++;
}

void Canine::waitForGpsFix() {
	_display->println("Acquiring GPS Signal");

	while (!_gps->getRawGps().location.isValid()) {
		_gps->collect();
	}
}

void Canine::waitForClick() {
	_joyStick->clear();
	_display->clearScr();
	_ahrs->resetYPR();

	while (!_joyStick->waitForPush(10)) {
		_display->reset();
		_gps->collect();

		//Serial.print(_ahrs->getYPR().yaw);Serial.print(", ");Serial.println(_ahrs->getOrientation());
		_display->print("YAW: ");_display->println(normalizeAngle(_ahrs->getOrientation()));
		_gps->collect();

		_display->print("LAT: ");_display->println(_gps->getLatitude(), 8);
		_gps->collect();

		_display->print("LONG: ");_display->println(_gps->getLongitude(), 8);
		_gps->collect();

		_display->print("SPEED: ");_display->println(_gps->getSpeed());
		_gps->collect();

		_display->print("SATS: ");_display->println(_gps->getRawGps().satellites.value());
		_gps->collect();

		_display->print("AGE: ");_display->println(_gps->getRawGps().location.age());
		_gps->collect();

		_display->print("NO OF WPs: ");_display->println(_waypointQty);
		_gps->collect();

		if (_waypointQty > 0) {
			update(0);
			_display->print("DIST TO WP: ");_display->println(_distance);
			_gps->collect();

			_display->print("HDG TO WP: ");_display->println(_hdg);
			_gps->collect();

		} else {
			_display->println("DIST TO WP: NO WP");
			_gps->collect();

			_display->println("HDG TO WP: NO WP");
			_gps->collect();
		}

		_display->println(F("Press the button to start"));
	}

	_display->clearScr();
}

void Canine::runSetup() {
	_display->clearScr();
	_display->println(F("Press the button to add waypoints"));

	if (_joyStick->waitForPush(5000)) {
		waitForGpsFix();
		configWaypoints();
		Utils::waitForEver();
	}
}

void Canine::configWaypoints() {
	int count = _wpHelper->configWaypoints();
	_display->clearScr();
	_display->print("Added ");_display->print(count);_display->println(" waypoints");
	_display->print("Reset to restart");
}

void Canine::loadWaypoints() {
	Geo2D waypoint;
	_wpHelper->open();
	while (_wpHelper->nextWaypoint(waypoint)) {
		addWaypoint(waypoint.getLatitude(), waypoint.getLongitude());
	}
	_wpHelper->close();
}

#endif
