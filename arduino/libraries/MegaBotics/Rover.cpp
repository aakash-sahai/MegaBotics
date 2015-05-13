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
	_direction = FORWARD;
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

void Rover::setDefaultConfig() {
	_config.throttleChannel = DEF_THROTTLE_CHANNEL;
	_config.steerChannel = DEF_STEER_CHANNEL;
	_config.controlChannel = DEF_CONTROL_CHANNEL;
	_config.idlePwm = DEF_IDLE_PWM;
	_config.fwdPwmMin = DEF_FWD_PWM_MIN;
	_config.fwdPwmMax = DEF_FWD_PWM_MAX;
	_config.revPwmMin = DEF_REV_PWM_MIN;
	_config.revPwmMax = DEF_REV_PWM_MAX;
	_config.steerPwmMax = DEF_STEER_PWM_MAX;
	_config.steerPwmMin = DEF_STEER_PWM_MIN;
	_config.steerMin = DEF_STEER_MIN;
	_config.steerMid = DEF_STEER_MID;
	_config.steerMax = DEF_STEER_MAX;
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

void Rover::setup(Config &aConfig) {
	_config = aConfig;
	setupConfig();
}

void Rover::setup(void) {
	loadConfig();
	setupConfig();
}

void Rover::loadConfig() {
	int len = sizeof(_config);
	EepromStore::Status status = EepromStore::getInstance()->loadSection(ROVER_CONFIG_NAME, &_config, &len);

	if (status != EepromStore::SUCCESS || len != sizeof(_config)) {
		setDefaultConfig();
	}
}

void Rover::setupConfig() {
	_steeringOut.setup(_config.steerChannel);
	_throttleOut.setup(_config.throttleChannel);
	_throttleIn = PwmIn::getInstance(_config.steerChannel);
	_steeringIn = PwmIn::getInstance(_config.throttleChannel);
	_controlIn = PwmIn::getInstance(_config.controlChannel);
	_steeringOut.attachServo(_config.steerPwmMax, _config.steerPwmMin);
	_throttleOut.attachServo(_config.revPwmMax, _config.fwdPwmMax);

	_controlIn->onBelowThreshold(_config.steerPwmMid - 200, Rover::setManual);
	_controlIn->onAboveThreshold(_config.steerPwmMid + 200, Rover::setManual);

	setControlMode(AUTO);
	delay(2000);
	straight();
	idle();
	setControlMode(MANUAL);
}

void Rover::steer(int8_t percent) {
	int degrees;
	percent = Utils::clamp(percent, (int8_t)-100, (int8_t)100);
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
	int usec = _config.idlePwm;
	percent = Utils::clamp(percent, (int8_t)-100, (int8_t)100);
	Direction direction = getDirection(percent);

	if ((_direction == FORWARD && direction == REVERSE)) {
		stop();
	}

	switch(direction) {
	case FORWARD:
		usec = percent == 0 ? _config.idlePwm : map(percent, 0, 100, _config.fwdPwmMin, _config.fwdPwmMax);
		break;
	case REVERSE:
		usec = map(percent, 0, -100, _config.revPwmMin, _config.revPwmMax);
		break;
	default:
		break;
	};

	rampTo(usec);
	_direction = direction;
}

Rover::Direction Rover::getDirection(int8_t percent) {
	if (percent >= 0) return FORWARD;
	return REVERSE;
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
	if (_direction == FORWARD) {
		stop(_config.revPwmMax, NO_OF_FWD_STOP_PULSES);
	} else if (_direction == REVERSE) {
		stop(_config.fwdPwmMax, NO_OF_REV_STOP_PULSES);
	}

	_direction = STOP;
}

void Rover::stop(int stopUsec, uint16_t noOfStopPulses) {
	for (uint16_t i = 0; i < noOfStopPulses; i++) {
		rampTo(stopUsec);
		delay(RAMP_STEP_DELAY);
	}

	rampTo(_config.idlePwm);
	delay(100);
}
