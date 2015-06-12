/*
 * Canine.h
 *
 *  Created on: May 23, 2015
 *      Author: sraj
 */

#ifndef CANINE_H_
#define CANINE_H_

#include <MegaBotics.h>
#include "Waypoints.h"

class Canine {
public:
	struct Config {
		float steerKp;
		float steerKi;
		float steerKd;
		float steerClamp;
		float steerScale;

		float steerGain;
		float distGain;
		uint8_t minThrottle;
		uint8_t maxThrottle;

		float curiseDistThres;
		float wpProximRadius;
		uint8_t navLoopDelay;
		uint16_t gpsMinAge;

		bool enableLogger;
		bool displayLogger;

		Config() {
			steerKp = 4.0f;
			steerKi = 0.0f;
			steerKd = 0.00f;
			steerClamp = 10;		// Clamp Steering integral correction to 10 degrees
			steerScale = 10.0/36.0;

			steerGain = 1.0f;			// increase this value to lower the speed w.r.t turn angle
			distGain = 1.0f;			// increase this value to lower the speed w.r.t distance from way point
			minThrottle = 5;
			maxThrottle = 15;

			curiseDistThres = 15.0f;
			wpProximRadius = 3.0f;
			navLoopDelay = 0;
			gpsMinAge = 1000;

			enableLogger = false;
			displayLogger = false;
		}
	};

	struct Waypoint {
		float latitude;
		float longitude;
		float proximRadius;
		uint8_t maxThrottle;
	};

	Canine();
	virtual ~Canine();

	void setup();
	void setup(Config& config);
	void autoRun();
	void addWaypoint(float latitude, float longitude);
	void addWaypoint(float latitude, float longitude, uint8_t maxSpeed);
	void addWaypoint(float latitude, float longitude, uint8_t maxSpeed, float proximRadius);
	void clearWaypoints(void) { _waypointQty = 0; _currentWaypoint = -1; }
	void waitForGpsFix();
	void waitForClick();
	void runSetup();
	void configWaypoints();
	void loadWaypoints();

	static Canine * getInstance() { return &_instance; }
	static Canine & getReference() { return _instance; }

private:
	static Canine _instance;

	AHRS* _ahrs;
	GPS* _gps;
	Display* _display;
	Rover* _rover;
	JoyStick* _joyStick;

	EepromStore* _estore;
	PID* _steeringPid;
	Logger* _logger;
	Waypoints* _wpHelper;
	Config _config;
	Waypoint _waypoints[MAX_WAYPOINTS];

	uint8_t _waypointQty;

	uint8_t _currentWaypoint;
	float _distance;
	float _hdg;
	float _steer;
	float _throttle;
	float _refHdg;

	Geo2D _curPos;

	bool nextWaypoint();
	void doNavigate();
	void update(uint8_t waypoint);
	float calcSteering();
	float calcThrottle(float steer);
	float normalizeAngle(float angle);
	float denormalizeAngle(float angle);
};

#endif /* CANINE_H_ */
