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
 * Rover.cpp
 *
 *  Created on: 14-Mar-2015
 *      Author: Aakash Sahai
 */

#include <MegaBotics.h>

Rover Rover::_instance;

Rover::Rover() {
	_config.throttleChannel = DEF_THROTTLE_CHANNEL;
	_config.steerChannel = DEF_STEER_CHANNEL;
	_config.controlChannel = DEF_CONTROL_CHANNEL;
	_config.idlePwm = DEF_IDLE_PWM;
	_config.fwdPwmMin = DEF_FWD_PWM_MIN;
	_config.fwdPwmMax = DEF_FWD_PWM_MAX;
	_config.revPwmMin = DEF_REV_PWM_MIN;
	_config.revPwmMax = DEF_REV_PWM_MAX;
	_config.steerMin = DEF_STEER_MIN;
	_config.steerMid = DEF_STEER_MID;
	_config.steerMax = DEF_STEER_MAX;
	_currentDirection = FORWARD;
	_runMode = IDLE;
	_controlMode = MANUAL;
	_currentUsec = 0;
	_throttleIn = PwmIn::getInstance(DEF_THROTTLE_CHANNEL);
	_steeringIn = PwmIn::getInstance(DEF_STEER_CHANNEL);
	_controlIn = PwmIn::getInstance(DEF_CONTROL_CHANNEL);
	_autoManualMux = PwmMux::getInstance();
}

Rover::~Rover() {
	straight();
	idle();
}

void Rover::setup(Config &aConfig)
{
	_config = aConfig;
	setup();
}

void Rover::setManual(unsigned int val) {
	_instance._controlMode = MANUAL;
	_instance._autoManualMux->setMode(PwmMux::PWMIN);
}

void Rover::setAuto(unsigned int val) {
	_instance._controlMode = AUTO;
	_instance._autoManualMux->setMode(PwmMux::PROGRAM);
}

void Rover::setControlMode(ControlMode mode) {
	if (mode == AUTO) {
		setAuto(0);
	} else {
		setManual(0);
	}
}

void Rover::setup(void) {
	_steeringOut.setup(_config.steerChannel);
	_throttleOut.setup(_config.throttleChannel);
	_throttleIn = PwmIn::getInstance(_config.steerChannel);
	_steeringIn = PwmIn::getInstance(_config.throttleChannel);
	_controlIn = PwmIn::getInstance(_config.controlChannel);
	_steeringOut.attachServo();
	_throttleOut.attachServo();
	_controlIn->onBelowThreshold(1200, Rover::setManual);
	_controlIn->onAboveThreshold(1600, Rover::setAuto);
	delay(2000);
	straight();
	idle();
}

void Rover::steer(int8_t percent) {
	int degrees;
	percent = clipPercent(percent);
	if (percent == 0) {
		// to straight
		degrees = _config.steerMid;
	} else if (percent < 0) {
		// steer to left
		degrees = map(percent, 0, -100, _config.steerMid, _config.steerMax);
	} else {
		// steer to right
		degrees = map(percent, 0, 100, _config.steerMid, _config.steerMin);
	}
	_steeringOut.writeServo(degrees);
}

void Rover::throttle(int8_t percent) {
	int usec;

	percent = clipPercent(percent);
	Direction direc = throtleToDirection(percent);

	if (_currentDirection == FORWARD && direc == REVERSE) {
		stop();
	}

	if (percent == 0) {
		usec = _config.idlePwm;
	} else if (direc == FORWARD) {
		// forward
		usec = map(percent, 0, 100, _config.fwdPwmMin, _config.fwdPwmMax);
	} else {
		// reverse
		usec = map(percent, 0, -100, _config.revPwmMin, _config.revPwmMax);
	}

	rampTo(usec);
	_currentDirection = direc;
}

Rover::Direction Rover::throtleToDirection(int8_t percent) {
	if (percent < 0)
		return REVERSE;
	return FORWARD;
}

void Rover::rampTo(int usec) {
	if (usec == _currentUsec || _currentUsec == 0) {
		_throttleOut.writeMicrosecondsServo(usec);
	} else if (usec < _currentUsec) {
		for (int i = _currentUsec; i > usec; i -= RAMP_STEP) {
			_throttleOut.writeMicrosecondsServo(i);
			delay(RAMP_STEP_DELAY);
		}
		_throttleOut.writeMicrosecondsServo(usec);
	} else if (usec > _currentUsec) {
		for (int i = _currentUsec; i < usec; i += RAMP_STEP) {
			_throttleOut.writeMicrosecondsServo(i);
			delay(RAMP_STEP_DELAY);
		}
		_throttleOut.writeMicrosecondsServo(usec);
	}

	_currentUsec = usec;
}

void Rover::stop() {
	rampTo(_config.revPwmMax);
	delay(100);
	rampTo(_config.idlePwm);
	delay(100);
}

int8_t Rover::clipPercent(int8_t percent) {
	if (percent < -100)
		return -100;
	if (percent > 100)
		return 100;
	return percent;
}
