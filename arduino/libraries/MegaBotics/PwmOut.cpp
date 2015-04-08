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
 * PwmOut.cpp
 *
 *  Created on: Mar 29, 2015
 *      Author: Aakash Sahai
 */

#include <PwmOut.h>

const byte PwmOut::_pinMappings[MAX_PWMOUT_CHANNELS] = { 4, 5, 6, 7, 8, 9 };

PwmOut::PwmOut() {
	_channel = 1;
}

PwmOut::~PwmOut() {
	if (_servo.attached()) {
		_servo.detach();
	}
}

void PwmOut::setup(byte channel) {
	_channel = channel;
}

void PwmOut::writeAnalog(int value) {
	 analogWrite(_pinMappings[_channel-1], value);

}

int PwmOut::attachServo() {
	return _servo.attach(_pinMappings[_channel-1]);
}

int PwmOut::attachServo(int minPwm, int maxPwm) {
	return _servo.attach((int)_pinMappings[_channel-1], minPwm, maxPwm);
}

void PwmOut::detachServo() {
	_servo.detach();
}

bool PwmOut::isServoAttached() {
	return _servo.attached();
}

int PwmOut::readServo() {
	return _servo.read();
}

void PwmOut::writeServo(int value) {
#ifdef DEBUG
	Serial.print("CH: ");
	Serial.print(_channel);
	Serial.print(", PIN: ");
	Serial.print(_pinMappings[_channel-1]);
	Serial.print(", VALUE: ");
	Serial.println(value);
#endif
	_servo.write(value);
}

void PwmOut::writeMicrosecondsServo(int value) {
	_servo.writeMicroseconds(value);
}
