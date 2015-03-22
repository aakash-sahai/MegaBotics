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
 *	may be used to endorse or promote products derived from this software
 *	without specific prior written permission.
 */

/*
 * Rover.h
 *
 *  Created on: 14-Mar-2015
 *      Author: eashan
 */

#ifndef ROVER_H_
#define ROVER_H_

struct RoverConfig {
	uint8_t		throttlePin;
	uint8_t		steerPin;

	int			idlePwm;
	int		 	fwdPwmMin;
	int			fwdPwmMax;
	int		 	revPwmMin;
	int			revPwmMax;

	int			steerMin;
	int			steerMid;
	int			steerMax;
};

class Rover {
public:
	static const uint8_t  DEF_THROTTLE_PIN = 9;		// default pin connected to ESC throttle
	static const uint8_t  DEF_STEER_PIN = 10;		// default pin connected to steering servo
	static const uint16_t DEF_IDLE_PWM =	1340;	// default PWM value to idle the throttle
	static const uint16_t DEF_FWD_PWM_MAX = 1800;	// default PWM value to move forward at full speed
	static const uint16_t DEF_FWD_PWM_MIN = 1410;	// default PWM value to start moving forward
	static const uint16_t DEF_REV_PWM_MIN = 1220;	// default PWM value to start moving reverse
	static const uint16_t DEF_REV_PWM_MAX = 770;	// default PWM value to move reverse at full speed
	static const int DEF_STEER_MIN = 53;			// default trim to steer right
	static const int DEF_STEER_MID = 90;			// default trim to steer straight
	static const int DEF_STEER_MAX = 153;			// default trim to steer left

	Rover();
	virtual ~Rover();

	void setup(RoverConfig &aConfig);
	void setup();

	void steerLeft(uint8_t pct) { steer(-1 * pct); }
	void steerRight(uint8_t pct) { steer(pct); }
	void straight(void) { steer(0); }
	void steer(int8_t pct);
	void steerRaw(int val) { _steering.write(val); }	// Takes values from 0 - 180
	void forward(uint8_t pct) { throttle(pct); };
	void reverse(uint8_t pct ) { throttle(-1 * pct); };
	void idle(void) { throttle(0); };
	void throttle(int8_t pct);
	void throttleRaw(int val) { _throttle.writeMicroseconds(val); } // Takes values from 0 - 180
	void brake(void);

private:
	RoverConfig _config;
	Servo	_throttle;
	Servo	_steering;

	int8_t clipPercent(int8_t pct);
	int8_t percentToDegrees(int8_t percent);
};

#endif /* ROVER_H_ */
