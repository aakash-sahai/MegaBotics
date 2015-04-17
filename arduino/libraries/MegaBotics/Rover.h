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
 *      Author: Aakash Sahai
 */

#ifndef ROVER_H_
#define ROVER_H_

#include <PwmOut.h>

class Rover {
public:

	enum Direction {
		FORWARD = 0,
		REVERSE = 1
	};

	struct Config {
		uint8_t		throttleChannel;
		uint8_t		steerChannel;

		int			idlePwm;
		int		 	fwdPwmMin;
		int			fwdPwmMax;
		int		 	revPwmMin;
		int			revPwmMax;

		int			steerMin;
		int			steerMid;
		int			steerMax;
	};

	static const uint8_t  DEF_THROTTLE_CHANNEL = 1;	// default pin connected to ESC throttle
	static const uint8_t  DEF_STEER_CHANNEL = 2;	// default pin connected to steering servo
	static const uint16_t DEF_IDLE_PWM =	1340;	// default PWM value to idle the throttle
	static const uint16_t DEF_FWD_PWM_MAX = 1800;	// default PWM value to move forward at full speed
	static const uint16_t DEF_FWD_PWM_MIN = 1410;	// default PWM value to start moving forward
	static const uint16_t DEF_REV_PWM_MIN = 1220;	// default PWM value to start moving reverse
	static const uint16_t DEF_REV_PWM_MAX = 770;	// default PWM value to move reverse at full speed
	static const int DEF_STEER_MIN = 53;			// default trim to steer right
	static const int DEF_STEER_MID = 90;			// default trim to steer straight
	static const int DEF_STEER_MAX = 153;			// default trim to steer left
	static const int RAMP_STEP = 50;				// increment step when ramping the throttle to the desired usec
	static const int RAMP_STEP_DELAY = 50;			// delay in milliseconds when ramping the throttle to the desired usec

	Rover();
	virtual ~Rover();

	void setup(Config &aConfig);
	void setup();

	void steerLeft(uint8_t pct) { steer(-1 * pct); }
	void steerRight(uint8_t pct) { steer(pct); }
	void straight(void) { steer(0); }
	void steer(int8_t pct);
	void steerRaw(int val) { _steering.writeServo(val); }	// Takes values from 0 - 180
	void forward(uint8_t pct) {throttle(pct);}
	void reverse(uint8_t pct ) {throttle(-1 * pct);}
	void idle(void) { throttle(0); };
	void throttle(int8_t pct);
	void throttleRaw(int val) { rampTo(val); } // Takes values from 0 - 180
	void stop(void);
	Direction currentDirection() {return _currentDirection;}

private:
	Config _config;
	PwmOut	_throttle;
	PwmOut	_steering;
	Direction _currentDirection;
	int _currentUsec;

	int8_t clipPercent(int8_t pct);
	int8_t percentToDegrees(int8_t percent);
	Direction direction(int8_t pct);
	void rampTo(int usec);
};

#endif /* ROVER_H_ */
