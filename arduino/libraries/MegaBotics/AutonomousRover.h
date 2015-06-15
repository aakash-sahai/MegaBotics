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
 * AutonomousRover.h
 *
 * This is the abstract base class for an Autonomous rover. Other classes can be derived from
 * it that may  different navigation strategies. This class derives from the Rover class and
 * assumes that the following advanced hardware functionality will be present on the Rover:
 *
 * 	GPS, AHRS (aka IMU), LCD Display, Joystick and SD Card
 *
 *  Created on: Jun 14, 2015
 *      Author: Aakash Sahai
 */

#ifndef MEGABOTICS_AUTONOMOUSROVER_H_
#define MEGABOTICS_AUTONOMOUSROVER_H_

#include "MegaBotics.h"

class AutonomousRover {
public:
	struct Config {
		int		logging;				// Enable/disable logging
		int		cruiseDistance;			// Distance (in meter) from next WP above which the Rover cruises at max speed
		int		navLoopDelay;			// Artificial delay (in msec) to introduce in navigation loop
		int		gpsStaleThres;			// Time (in msec) when GPS reading is considered too stale
		int		waypointTooFarThres;	// Distance (in meter) from next WP that is considered too far
		float	steerScale;				// Scale factor for Steering PID

	    Config() {
	    	logging = 1;
	    	cruiseDistance = 10;
			navLoopDelay = 0;
			gpsStaleThres = 2000;
			waypointTooFarThres = 100;
			steerScale = 10.0/36.0;
		}
	};

	AutonomousRover();
	virtual ~AutonomousRover();

	void setup(void);
	void setup(Config& config);
	void calibrate(void);
	void waitToStart(void);

	virtual void autoRun(void) = 0;

protected:
	Config _config;
	Rover::ThrottleConfig _throttleConfig;

	AHRS* _ahrs;
	Display* _display;
	Rover* _rover;
	JoyStick* _joystick;
	Logger* _logger;
	EepromStore* _estore;
	Route* _route;
	PID* _steeringPid;

	void setupLogging();
	void logConfig();
};

#endif /* MEGABOTICS_AUTONOMOUSROVER_H_ */
