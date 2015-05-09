/*
 * This file is part of the support libraries for the Arduino based MegaBotics
 * platform.
 *
 * Copyright (c) 2015, Aakash Sahai and other contributors, a list of which is
 * maintained in the associated repository. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    + Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    + Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    + Neither the name of the copyright holder nor the names of its contributors
 *		may be used to endorse or promote products derived from this software
 *		without specific prior written permission.
 */

/*
 * Module:		SmartRover.h
 *
 * Created on:	May 6, 2015
 * Author:		Srinivas Raj
 */

#ifndef MEGABOTICS_SMARTROVER_H_
#define MEGABOTICS_SMARTROVER_H_

#include <MegaBotics.h>
#include <WheelEncoder.h>

#define DEF_STEER_KP				2.0
#define DEF_STEER_KI				0.1
#define DEF_STEER_KD				0.01
#define DEF_STEER_SCALE				(RAD_TO_DEG * 20 / 36)
#define DEF_STEER_ICLAMP			(PI / 8)		// Clamp Steering integral correction to PI / 8
#define DEF_THROTTLE_MIN			5.0
#define DEF_THROTTLE_MAX			30.0
#define DEF_CRUISE_DISTANCE_THRES	10.0	// 10.0 ft.
#define DEF_WP_PROXIM_RADUS			1.0		// 1.0 ft.
#define DEF_NAV_LOOP_DELAY			5		// 5 ms.

#define MAX_WAYPOINTS				10

class SmartRover {
public:
	struct Config {
		float steerKp;
		float steerKi;
		float steerKd;
		float steerClamp;
		float steerScale;
		float throttleMin;
		float throttleMax;
		float curiseDistThres;
		float wpProximRadius;
		uint8_t navLoopDelay;

		Config() {
			steerKp = DEF_STEER_KP;
			steerKi = DEF_STEER_KI;
			steerKd = DEF_STEER_KD;
			steerClamp = DEF_STEER_ICLAMP;
			steerScale = DEF_STEER_SCALE;
			throttleMin = DEF_THROTTLE_MIN;
			throttleMax = DEF_THROTTLE_MAX;
			curiseDistThres = DEF_CRUISE_DISTANCE_THRES;
			wpProximRadius = DEF_WP_PROXIM_RADUS;
			navLoopDelay = DEF_NAV_LOOP_DELAY;
		}
	};

	struct Waypoint {
		float distance;
		float hdg;
	};

	SmartRover();
	virtual ~SmartRover();

	void setup();
	void setup(Config& config);
	void autoRun();
	void addWaypoint(float distance, float hdg);
	void clearWaypoints(void) { _waypointQty = 0; _currentWaypoint = 0; }

	static SmartRover * getInstance() { return &_instance; }
	static SmartRover & getReference() { return _instance; }

private:
	static SmartRover _instance;

	AHRS* _ahrs;
	WheelEncoder* _encoder;
	Rover* _rover;
	EepromStore* _estore;
	PID* _steeringPid;
	Config _config;
	Waypoint _waypoints[MAX_WAYPOINTS];

	uint8_t _waypointQty;

	uint8_t _currentWaypoint;
	float _distance;
	float _hdg;
	float _distanceTraveled;
	float _steer;
	float _throttle;

	void nextWaypoint();
	void doNavigate();
	float clampThrottle(float throttle);
};

#endif /* MEGABOTICS_SMARTROVER_H_ */
