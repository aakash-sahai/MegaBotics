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

#include <MegaBotics.h>

#define ROVER_CONFIG_NAME "ROVR"

class Rover {
public:
	enum Direction {
		FORWARD = 0,
		REVERSE = 1,
		STOP = 2,
		IDLE = 3,
	};

	enum ControlMode {
		MANUAL = 0,		// Manual control through RC remote
		AUTO = 1,		// Autopilot mode under full program control
		ASSIST = 2		// Manual control through RC remote but assisted through AHRS and WheelEncoder
	};

	struct Config {
		byte		throttleChannel;
		byte		steerChannel;
		byte		controlChannel;

		uint16_t	idlePwm;
		uint16_t	fwdPwmMin;
		uint16_t	fwdPwmMax;
		uint16_t	revPwmMin;
		uint16_t	revPwmMax;
		uint16_t	steerPwmMin;
		uint16_t	steerPwmMax;
		uint16_t	steerPwmMid;

		int			steerMin;
		int			steerMid;
		int			steerMax;

		bool		allowIdleToStop;

		Config() {
			throttleChannel = DEF_THROTTLE_CHANNEL;
			steerChannel = DEF_STEER_CHANNEL;
			controlChannel = DEF_CONTROL_CHANNEL;

			idlePwm = DEF_IDLE_PWM;
			fwdPwmMin = DEF_FWD_PWM_MIN;
			fwdPwmMax = DEF_FWD_PWM_MAX;
			revPwmMin = DEF_REV_PWM_MIN;
			revPwmMax = DEF_REV_PWM_MAX;
			steerPwmMin = DEF_STEER_PWM_MIN;
			steerPwmMax = DEF_STEER_PWM_MAX;
			steerPwmMid = DEF_STEER_PWM_MID;

			steerMin = DEF_STEER_MIN;
			steerMid = DEF_STEER_MID;
			steerMax = DEF_STEER_MAX;
			allowIdleToStop = false;
		}
	};

	static const byte  DEF_THROTTLE_CHANNEL = 2;		// default PwmIn and PwmOut channels controlling the ESC throttle
	static const byte  DEF_STEER_CHANNEL = 1;		// default PwmIn and PwmOut channel controlling the steering servo
	static const byte  DEF_CONTROL_CHANNEL = 5;		// default PwmIn channel connected to control manual/autopilot mode
	static const uint16_t DEF_IDLE_PWM =	1340;		// default PWM value to idle the throttle
	static const uint16_t DEF_FWD_PWM_MAX = 1800;		// default PWM value to move forward at full speed
	static const uint16_t DEF_FWD_PWM_MIN = 1410;		// default PWM value to start moving forward
	static const uint16_t DEF_REV_PWM_MIN = 1220;		// default PWM value to start moving reverse
	static const uint16_t DEF_REV_PWM_MAX = 770;		// default PWM value to move reverse at full speed
	static const uint16_t DEF_STEER_PWM_MIN = 1000;		// default PWM value to steer 100% right
	static const uint16_t DEF_STEER_PWM_MAX = 2000;		// default PWM value to steer 100% left
	static const uint16_t DEF_STEER_PWM_MID = 1500;		// default PWM value to steer straight

	static const int DEF_STEER_MIN = 53;				// default trim to steer right
	static const int DEF_STEER_MID = 90;				// default trim to steer straight
	static const int DEF_STEER_MAX = 153;				// default trim to steer left
	static const int RAMP_STEP = 50;					// increment step when ramping the throttle to the desired usec
	static const int RAMP_STEP_DELAY = 1;				// delay in milliseconds when ramping the throttle to the desired usec
	static const int NO_OF_FWD_STOP_PULSES = 1000;		// number of stop pulses to send to stop the rover while moving forward
	static const int NO_OF_REV_STOP_PULSES = 10;		// number of stop pulses to send to stop the rover while moving reverse

	virtual ~Rover();

	static Rover * getInstance() { return &_instance; }
	static Rover & getReference() { return _instance; }

	void setup(Config &aConfig);
	void setup();

	void steerLeft(uint8_t pct) { steer(-1 * pct); }
	void steerRight(uint8_t pct) { steer(pct); }
	void straight(void) { steer(0); }
	void steer(int8_t pct);
	void steerRaw(int val) { _steeringOut.writeServo(val); }	// Takes values from 0 - 180
	void forward(uint8_t pct) {throttle(pct);}
	void reverse(uint8_t pct ) {throttle(-1 * pct);}
	void idle(void) { throttle(0); };
	void throttle(int8_t pct);
	void throttleRaw(int val) { rampTo(val); } 					// Takes values from 0 - 180
	void stop(void);
	void _break(uint8_t pct);
	Direction getDirection() {  return _direction; }
	ControlMode getControlMode() {  return _controlMode; }
	void setControlMode(ControlMode mode);

private:
	static Rover _instance;
	Config	_config;
	PwmOut	_throttleOut;
	PwmOut	_steeringOut;
	PwmIn *	_throttleIn;
	PwmIn *	_steeringIn;
	PwmIn *	_controlIn;
	PwmMux * _autoManualMux;

	Direction	_direction;
	ControlMode _controlMode;
	int			_currentUsec;

	int8_t percentToDegrees(int8_t percent);
	Direction getDirection(int8_t pct);
	void rampTo(int usec);
	static void setManual(unsigned int val);
	static void setAuto(unsigned int val);
	void setDefaultConfig();
	void loadConfig();
	void setupConfig();
	void stop(int stopUsec, uint16_t noOfStopPulses);

	Rover();
	Rover(Rover const&);          	// Don't Implement to disallow copy by assignment
    void operator=(Rover const&);	// Don't implement to disallow copy by assignment
};

#endif /* ROVER_H_ */
